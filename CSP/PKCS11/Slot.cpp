#include "../StdAfx.h"
#include "slot.h"
#include "PKCS11Functions.h"
#include "../PCSC/token.h"
#include "../PCSC/PCSC.h"

#include "CardTemplate.h"
#include "../util/util.h"
#include "../util/syncroevent.h"
#include <mutex>

static char *szCompiledFile = __FILE__;
//extern CSyncroMutex p11EventMutex;
extern std::mutex p11Mutex;
extern auto_reset_event p11slotEvent;
extern bool bP11Terminate;
extern bool bP11Initialized;

namespace p11 {

	DWORD CSlot::dwSlotCnt = 0;
	SlotMap CSlot::g_mSlots;
	std::thread CSlot::Thread;
	CCardContext *CSlot::ThreadContext = NULL;
	bool CSlot::bMonitorUpdate = false;

	CSlot::CSlot(const char *reader, const char *vendor) {
		szName = reader;
		szVendor = vendor;
		lastEvent = SE_NoEvent;
		bUpdated = 0;
		User = CKU_NOBODY;
		dwP11ObjCnt = 0;
		dwSessionCount = 0;
		pTemplate = NULL;
		//slotMutex.Create(mutexName(szReader));
		pSerialTemplate = NULL;
		hCard = NULL;
	}

	CSlot::~CSlot() {
		Final();
	}

	CK_SLOT_ID CSlot::GetNewSlotID() {
		init_func
			return InterlockedIncrement(&dwSlotCnt);
	}

	static DWORD slotMonitor(SlotMap *pSlotMap)
	{
		while (true) {
			CCardContext Context;
			CSlot::ThreadContext = &Context;
			size_t dwSlotNum = pSlotMap->size();
			std::vector<SCARD_READERSTATE> state(dwSlotNum);
			std::vector<std::shared_ptr<CSlot>> slot(dwSlotNum);
			size_t i = 0;
			DWORD ris;
			{
				std::unique_lock<std::mutex> lock(p11Mutex);
				for (SlotMap::const_iterator it = pSlotMap->begin(); it != pSlotMap->end(); it++, i++) {
					if (!bP11Initialized) {
						CSlot::ThreadContext = NULL;
						return 0;
					}

					state[i].szReader = it->second->szName.c_str();
					slot[i] = it->second;
					if (ris = SCardGetStatusChange(Context, 0, &state[i], 1) != S_OK) {
						if (ris != SCARD_E_TIMEOUT) {
							Log.write("Errore nella SCardGetStatusChange - %08X", ris);
							// non uso la ExitThread!!!
							// altrimenti non chiamo i distruttori, e mi rimane tutto appeso
							CSlot::ThreadContext = NULL;
							return 1;
						}
					}
					state[i].dwCurrentState = state[i].dwEventState & (~SCARD_STATE_CHANGED);
				}
			}
			CSlot::bMonitorUpdate = false;
			while (true) {
				Context.validate();
				ris = SCardGetStatusChange(Context, 1000, state.data(), (DWORD)dwSlotNum);
				if (ris != S_OK) {
					if (CSlot::bMonitorUpdate || ris == SCARD_E_SYSTEM_CANCELLED || ris == SCARD_E_SERVICE_STOPPED || ris == SCARD_E_INVALID_HANDLE || ris == ERROR_INVALID_HANDLE) {
						Log.write("Monitor Update");
						break;
					}
					if (ris == SCARD_E_CANCELLED || bP11Terminate || !bP11Initialized) {
						Log.write("Terminate");
						p11slotEvent.set();
						CSlot::ThreadContext = NULL;
						// no exitThread, vedi sopra;
						return 0;
					}
					if (ris != SCARD_E_TIMEOUT && ris != SCARD_E_NO_READERS_AVAILABLE) {
						Log.write("Errore nella SCardGetStatusChange - %08X", ris);
						p11slotEvent.set();
						CSlot::ThreadContext = NULL;
						// no exitThread, vedi sopra;
						return 1;
					}
					if (ris == SCARD_E_NO_READERS_AVAILABLE) {
						Log.write("Nessun lettore connesso - %08X", ris);
						CSlot::ThreadContext = NULL;
						// no exitThread, vedi sopra;
						return 1;
					}
				}
				if (bP11Terminate || !bP11Initialized) {
					Log.write("Terminate");
					p11slotEvent.set();
					CSlot::ThreadContext = NULL;
					// no exitThread, vedi sopra;
					return 0;
				}

				for (size_t i = 0; i < dwSlotNum; i++) {
					if ((state[i].dwCurrentState & SCARD_STATE_PRESENT) &&
						((state[i].dwEventState & SCARD_STATE_EMPTY) ||
						(state[i].dwEventState & SCARD_STATE_UNAVAILABLE))) {

						std::unique_lock<std::mutex> lock(p11Mutex);

						slot[i]->lastEvent = SE_Removed;
						slot[i]->Final();
						slot[i]->baATR.clear();
						p11slotEvent.set();
					}
					if (((state[i].dwCurrentState & SCARD_STATE_UNAVAILABLE) ||
						(state[i].dwCurrentState & SCARD_STATE_EMPTY)) &&
						(state[i].dwEventState & SCARD_STATE_PRESENT)) {
						// una carta è stata inserita!!
						std::unique_lock<std::mutex> lock(p11Mutex);

						slot[i]->lastEvent = SE_Inserted;
						ByteArray ba;
						slot[i]->GetATR(ba);
						p11slotEvent.set();
					}
					state[i].dwCurrentState = state[i].dwEventState & (~SCARD_STATE_CHANGED);
				}
			}
			CSlot::ThreadContext = NULL;
		}
		// no exitThread, vedi sopra;
		return 0;
	}

	CK_SLOT_ID CSlot::AddSlot(std::shared_ptr<CSlot> pSlot)
	{
		init_func
			pSlot->hSlot = GetNewSlotID();
		auto id = pSlot->hSlot;
		g_mSlots.insert(std::make_pair(pSlot->hSlot, std::move(pSlot)));
		return id;
	}

	void CSlot::DeleteSlot(CK_SLOT_ID hSlotId)
	{
		init_func
			std::shared_ptr<CSlot> pSlot = GetSlotFromID(hSlotId);

		if (!pSlot)
			throw p11_error(CKR_SLOT_ID_INVALID);

		pSlot->CloseAllSessions();

		g_mSlots.erase(hSlotId);
		pSlot->Final();
	}

	std::shared_ptr<CSlot> CSlot::GetSlotFromReaderName(const char *name)
	{
		init_func
			for (SlotMap::iterator it = g_mSlots.begin(); it != g_mSlots.end(); it++) {
				if (strcmp(it->second->szName.c_str(), name) == 0) {
					return it->second;
				}
			}
		return nullptr;
	}

	std::shared_ptr<CSlot> CSlot::GetSlotFromID(CK_SLOT_ID hSlotId)
	{
		init_func
			SlotMap::const_iterator pPair;
		pPair = g_mSlots.find(hSlotId);
		if (pPair == g_mSlots.end()) {
			return nullptr;
		}
		return pPair->second;
	}

	void CSlot::DeleteSlotList()
	{
		init_func

			int i = 0;

		if (Thread.joinable())
			Thread.join();

		// TODO: verificare se è il caso di usare un thread con join a tempo

		/*while (i<5) {
			if (Thread.join(1000)==OK)
				break;
			i=i+1;
			if (CSlot::ThreadContext!=NULL)
				SCardCancel(*CSlot::ThreadContext);
		}
		if (i==5) {
			Thread.terminateThread();
		}*/

		SlotMap::iterator it = CSlot::g_mSlots.begin();
		while (it != CSlot::g_mSlots.end()) {
			DeleteSlot(it->second->hSlot);
			it = CSlot::g_mSlots.begin();
		}
	}

	void CSlot::InitSlotList()
	{
		// la InitSlotList deve aggiornare la liste degli slot;
		// cioè, deve aggiungere gli slot che non c'erano prima e
		// cancellare quelli che non ci sono più
		init_func
			bool bMapChanged = false;
		DWORD readersLen = 0;

		CCardContext Context;

		if (!bP11Initialized)
			return;

		auto ris = SCardListReaders(Context, NULL, NULL, &readersLen);
		if (ris != S_OK) {
			if (ris == SCARD_E_NO_READERS_AVAILABLE)
				return;
			throw windows_error(ris);
		}
		std::string readers;
		readers.resize(readersLen + 1);
		if ((ris = SCardListReaders(Context, NULL, &readers[0], &readersLen)) != SCARD_S_SUCCESS)
			throw windows_error(ris);

		const char *szReaderName = readers.c_str();

		while (*szReaderName != 0) {
			if (!bP11Initialized)
				return;

			// vediamo questo slot c'era già prima
			Log.write("reader:%s", szReaderName);
			std::shared_ptr<CSlot> pSlot = GetSlotFromReaderName(szReaderName);
			if (pSlot == nullptr) {
				std::string VendorName;
				{
					SCARDHANDLE tempCard = 0;
					DWORD proto = 0;
					SCardConnect(Context, szReaderName, SCARD_SHARE_DIRECT, SCARD_PROTOCOL_UNDEFINED, &tempCard, &proto);
					safeConnection safeConn(tempCard);
					DWORD len = 0;
					if (SCardGetAttrib(tempCard, SCARD_ATTR_VENDOR_NAME, NULL, &len) == 0) {
						len++;
						ByteDynArray vendor(len);
						vendor.fill(0);
						SCardGetAttrib(tempCard, SCARD_ATTR_VENDOR_NAME, vendor.data(), &len);
						SCardDisconnect(tempCard, SCARD_LEAVE_CARD);
						VendorName=(char*)vendor.data();
					}
				}
				auto pSlot = std::make_shared<CSlot>(szReaderName, VendorName.c_str());
				CK_SLOT_ID hSlotID = AddSlot(pSlot);
				bMapChanged = true;
			}
			szReaderName = szReaderName + strnlen(szReaderName, readersLen) + 1;
		}
		// adesso vedo se tutti gli slot nella mappa ci sono ancora
		for (SlotMap::iterator it = g_mSlots.begin(); it != g_mSlots.end(); it++) {
			if (!bP11Initialized)
				return;

			Log.write("%s", it->second->szName.c_str());
			const char *name = it->second->szName.c_str();

			const char *szReaderName = readers.c_str();
			//char *szReaderName=szReaderAlloc;
			bool bFound = false;
			while (*szReaderName != 0) {
				if (strcmp(name, szReaderName) == 0) {
					bFound = true;
					break;
				}
				szReaderName = szReaderName + strnlen(szReaderName, readersLen) + 1;
			}
			if (!bFound) {
				CK_SLOT_ID ID = it->second->hSlot;
				it--;
				DeleteSlot(ID);
				bMapChanged = true;
			}
		}
		bMonitorUpdate = bMapChanged;

		if (!bP11Initialized)
			return;

		if (!Thread.joinable())
			Thread = std::thread(slotMonitor, &g_mSlots);

	}

	bool CSlot::IsTokenPresent()
	{
		init_func
			SCARD_READERSTATE state;
		memset(&state, 0, sizeof(SCARD_READERSTATE));
		state.szReader = szName.c_str();

		Context.validate();
		bool retry = false;
		while (true) {
			DWORD ris = SCardGetStatusChange(Context, 0, &state, 1);
			if (ris == S_OK) {
				if ((state.dwEventState & SCARD_STATE_UNAVAILABLE) == SCARD_STATE_UNAVAILABLE)
					throw p11_error(CKR_DEVICE_REMOVED);
				if ((state.dwEventState & SCARD_STATE_PRESENT) == SCARD_STATE_PRESENT)
					return true;
				else
					return false;
			}
			else {
				if (ris == SCARD_E_SERVICE_STOPPED || ris == SCARD_E_INVALID_HANDLE || ris == ERROR_INVALID_HANDLE) {
					// devo prendere un nuovo context e riprovare
					if (!retry)
						retry = true;
					else
						throw windows_error(ris);
					Context.renew();
					continue;
				}
				if (ris == SCARD_E_NO_READERS_AVAILABLE)
					throw p11_error(CKR_DEVICE_REMOVED);
				throw windows_error(ris);
			}
		}
	}

	void CSlot::GetInfo(CK_SLOT_INFO_PTR pInfo)
	{
		init_func
			pInfo->flags = CKF_REMOVABLE_DEVICE | CKF_HW_SLOT;
		// verifico che ci sia una carta inserita

		if (IsTokenPresent())
			pInfo->flags |= CKF_TOKEN_PRESENT;

		memset(pInfo->slotDescription, 0, 64);
		size_t SDLen = min(64, szName.size());
		memcpy_s(pInfo->slotDescription, 64, szName.c_str(), SDLen);

		memset(pInfo->manufacturerID, 0, 32);

		size_t MIDLen = min(32, szVendor.size());
		memcpy_s(pInfo->manufacturerID, 32, szVendor.c_str(), MIDLen);

		pInfo->hardwareVersion.major = 0;
		pInfo->hardwareVersion.minor = 0;

		pInfo->firmwareVersion.major = 0;
		pInfo->firmwareVersion.minor = 0;
	}

	void CSlot::GetTokenInfo(CK_TOKEN_INFO_PTR pInfo)
	{
		init_func

			if (pTemplate == nullptr)
				pTemplate = CCardTemplate::GetTemplate(*this);

		if (pTemplate == nullptr)
			throw p11_error(CKR_TOKEN_NOT_RECOGNIZED);

		memset(pInfo->label, 0, sizeof(pInfo->label));
		memcpy_s((char*)pInfo->label, 32, pTemplate->szName.c_str(), min(pTemplate->szName.length(), sizeof(pInfo->label)));
		memset(pInfo->manufacturerID, ' ', sizeof(pInfo->manufacturerID));
		
		Log.write("CIE ATR:");
		Log.writeBinData(baATR.data(), baATR.size());

		std::string manifacturer;
		size_t position;
		if (baATR.indexOf(baNXP_ATR, position))
			manifacturer = "NXP";
		else if ((baATR.indexOf(baGemalto_ATR, position)) ||
			(baATR.indexOf(baGemalto2_ATR, position)))
			manifacturer = "Gemalto";
		else if ((baATR.indexOf(baSTM_ATR, position)))
			manifacturer = "STM";
		else if ((baATR.indexOf(baSTM2_ATR, position)))
			manifacturer = "STM2";
		else if ((baATR.indexOf(baSTM3_ATR, position)))
			manifacturer = "STM3";
		else
			throw p11_error(CKR_TOKEN_NOT_RECOGNIZED);

		memcpy_s((char*)pInfo->manufacturerID, 32, manifacturer.c_str(), manifacturer.size());

		if (baSerial.isEmpty() || pSerialTemplate != pTemplate) {
			pSerialTemplate = pTemplate;
			baSerial = pTemplate->FunctionList.templateGetSerial(*this);
		}

		std::string model;
		pTemplate->FunctionList.templateGetModel(*this, model);

		memset(pInfo->serialNumber, 0, sizeof(pInfo->serialNumber));
		size_t UIDsize = min(sizeof(pInfo->serialNumber), baSerial.size());
		memcpy_s(pInfo->serialNumber, 16, baSerial.data(), UIDsize);

		memcpy_s((char*)pInfo->label + pTemplate->szName.length() + 1, sizeof(pInfo->label) - pTemplate->szName.length() - 1, baSerial.data(), baSerial.size());

		memset(pInfo->model, 0, sizeof(pInfo->model));
		memcpy_s(pInfo->model, 16, model.c_str(), min(model.length(), sizeof(pInfo->model)));

		CK_FLAGS dwFlags;
		pTemplate->FunctionList.templateGetTokenFlags(*this, dwFlags);
		pInfo->flags = dwFlags;

		pInfo->ulTotalPublicMemory = CK_UNAVAILABLE_INFORMATION;
		pInfo->ulTotalPrivateMemory = CK_UNAVAILABLE_INFORMATION;
		pInfo->ulFreePublicMemory = CK_UNAVAILABLE_INFORMATION;
		pInfo->ulFreePrivateMemory = CK_UNAVAILABLE_INFORMATION;
		pInfo->ulMaxSessionCount = MAXSESSIONS;
		size_t dwSessCount = SessionCount();

		pInfo->ulSessionCount = (CK_ULONG)dwSessCount;
		size_t dwRWSessCount = RWSessionCount();

		pInfo->ulRwSessionCount = (CK_ULONG)dwRWSessCount;
		pInfo->ulMaxRwSessionCount = MAXSESSIONS;

		pInfo->ulMinPinLen = 4;
		pInfo->ulMaxPinLen = 8;

		pInfo->hardwareVersion.major = 0;
		pInfo->hardwareVersion.minor = 0;

		pInfo->firmwareVersion.major = 0;
		pInfo->firmwareVersion.minor = 0;

		memcpy_s((char*)pInfo->utcTime, 16, "1234567890123456", 16);  // OK
	}

	void CSlot::CloseAllSessions()
	{
		init_func

			SessionMap::iterator it = CSession::g_mSessions.begin();
		while (it != CSession::g_mSessions.end()) {
			if (it->second->pSlot.get() == this)
			{
				CSession *pSession = it->second.get();
				it++;
				CSession::DeleteSession(pSession->hSessionHandle);
			}
			else it++;
		}
	}

	void CSlot::Init()
	{
		init_func
			if (!bUpdated) {

				if (pTemplate == nullptr)
					pTemplate = CCardTemplate::GetTemplate(*this);


				if (pTemplate == nullptr)
					throw p11_error(CKR_TOKEN_NOT_RECOGNIZED);

				pTemplate->FunctionList.templateInitCard(pTemplateData, *this);
				bUpdated = true;
			}
	}

	void CSlot::Final()
	{
		if (bUpdated) {
			// cancello i dati del template
			pTemplate->FunctionList.templateFinalCard(pTemplateData);
			pTemplate = NULL;

			baATR.clear();
			baSerial.clear();

			P11Objects.clear();

			// cancello tutte le sessioni
			SessionMap::iterator it = CSession::g_mSessions.begin();
			while (it != CSession::g_mSessions.end()) {
				if (it->second->pSlot.get() == this)
				{
					it = CSession::g_mSessions.erase(it);
					dwSessionCount--;
				}
				else it++;
			}
			// dwSessionCount dovrebbe essere già a 0...
			// ma per sicurezza lo setto a manina

			User = CKU_NOBODY;
			dwSessionCount = 0;
			bUpdated = false;
		}
	}

	std::shared_ptr<CP11Object> CSlot::FindP11Object(CK_OBJECT_CLASS objClass, CK_ATTRIBUTE_TYPE attr, CK_BYTE *val, int valLen)
	{
		init_func
			for (DWORD i = 0; i < P11Objects.size(); i++)
			{
				auto obj = P11Objects[i];
				if (obj->ObjClass == objClass) {
					ByteArray *attrVal = obj->getAttribute(attr);
					if (attrVal && attrVal->size() == valLen) {
						if (memcmp(attrVal->data(), val, valLen) == 0) {
							return obj;
						}
					}
				}
			}
		return nullptr;
	}

	void CSlot::AddP11Object(std::shared_ptr<CP11Object> p11obj)
	{
		init_func
			p11obj->pSlot = this;
		P11Objects.emplace_back(std::move(p11obj));
	}

	void CSlot::ClearP11Objects()
	{
		init_func
			P11Objects.clear();
		ObjP11Map.clear();
		HandleP11Map.clear();
	}

	void CSlot::DelP11Object(const std::shared_ptr<CP11Object>& object)
	{
		init_func
			bool bFound = false;
		for (P11ObjectVector::iterator it = P11Objects.begin(); it != P11Objects.end(); it++) {
			if (*it == object) {
				bFound = true;
				P11Objects.erase(it);
				break;
			}
		}
		ER_ASSERT(bFound, ERR_FIND_OBJECT)

			ObjHandleMap::iterator itObj = ObjP11Map.find(object);
		if (itObj != ObjP11Map.end()) {
			HandleObjMap::iterator itHandle = HandleP11Map.find(itObj->second);
			ObjP11Map.erase(itObj);
			if (itHandle != HandleP11Map.end())
				HandleP11Map.erase(itHandle);
		}
	}

	size_t CSlot::SessionCount()
	{
		init_func
		return dwSessionCount;
	}

	size_t CSlot::RWSessionCount()
	{
		init_func
			size_t  dwRWSessCount = 0;
		for (SessionMap::iterator it = CSession::g_mSessions.begin(); it != CSession::g_mSessions.end(); it++) {
			if (it->second->pSlot.get() == this && (it->second->flags & CKF_RW_SESSION) != 0)
				dwRWSessCount++;
		}
		return dwRWSessCount;
	}

	CK_OBJECT_HANDLE CSlot::GetIDFromObject(const std::shared_ptr<CP11Object>&pObject)
	{
		init_func

			if (pObject->IsPrivate() && User != CKU_USER)
				throw p11_error(CKR_USER_NOT_LOGGED_IN);

		ObjHandleMap::const_iterator pPair;
		pPair = ObjP11Map.find(pObject);
		if (pPair == ObjP11Map.end()) {
			// non ho trovato l'oggetto nella mappa degli oggetti;
			// devo aggiungerlo

			auto hObject = GetNewObjectID();
			ObjP11Map[pObject] = hObject;
			HandleP11Map[hObject] = pObject;
			return hObject;
		}
		return pPair->second;
	}

	CK_OBJECT_HANDLE CSlot::GetNewObjectID() {
		init_func
			return InterlockedIncrement(&dwP11ObjCnt);
	}

	void CSlot::DelObjectHandle(const std::shared_ptr<CP11Object>& pObject)
	{
		init_func
			ObjHandleMap::iterator pPair;
		pPair = ObjP11Map.find(pObject);
		if (pPair != ObjP11Map.end()) {
			HandleObjMap::iterator pPair2;
			pPair2 = HandleP11Map.find(pPair->second);
			if (pPair2 != HandleP11Map.end())
				HandleP11Map.erase(pPair2);
			ObjP11Map.erase(pPair);
		}
	}

	std::shared_ptr<CP11Object> CSlot::GetObjectFromID(CK_OBJECT_HANDLE hObjectHandle)
	{
		init_func
			HandleObjMap::const_iterator pPair;
		pPair = HandleP11Map.find(hObjectHandle);
		if (pPair == HandleP11Map.end())
			return nullptr;

		return pPair->second;
	}

	void CSlot::Connect() {
		init_func
			DWORD dwProtocol;

		Context.validate();
		bool retry = false;
		while (true) {
			Log.write("Slot scelto: ");
			Log.write(szName.c_str());
			DWORD ris = SCardConnect(Context, szName.c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T1, &hCard, &dwProtocol);
			if (ris == SCARD_S_SUCCESS) {
				return;
			}
			else {
				if (ris == SCARD_E_SERVICE_STOPPED || ris == SCARD_E_INVALID_HANDLE || ris == ERROR_INVALID_HANDLE) {
					if (!retry)
						retry = true;
					else {
						throw windows_error(ris);
					}
					Context.renew();
				}
				else {
					if (ris != SCARD_S_SUCCESS)
						throw windows_error(ris);
				}
			}
		}
	}

	ByteDynArray CSlot::GetATR()
	{
		init_func

			SCARD_READERSTATE state;
		state.szReader = this->szName.data();
		SCardGetStatusChange(CSlot::Context, 0, &state, 1);
		if (state.cbAtr > 0) {
			Log.write("ATR Letto:");
			Log.writeBinData(state.rgbAtr, state.cbAtr);
			return ByteArray(state.rgbAtr, state.cbAtr);
		}
		else {			
			Log.write("ATR Letto: -nessuna carta inserita-");
			return ByteArray();
		}
	}

	void CSlot::GetATR(ByteArray &ATR) {
		init_func
			if (baATR.size() != 0) {
				ATR = baATR;
				return;
			}
		baATR = GetATR();
		ATR = baATR;
	}

}
