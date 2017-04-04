#include "..\StdAfx.h"
#include ".\slot.h"
#include "PKCS11Functions.h"
#include "..\token.h"

#include "CardTemplate.h"
#include "..\util.h"
#include "..\syncroevent.h"

static char *szCompiledFile=__FILE__;
//extern CSyncroMutex p11EventMutex;
extern CSyncroMutex p11Mutex;
extern CSyncroEvent p11slotEvent;
extern bool bP11Terminate;

namespace p11 {

DWORD CSlot::dwSlotCnt=1;
SlotMap CSlot::g_mSlots;
CThread CSlot::Thread;
CCardContext *CSlot::ThreadContext=NULL;
bool CSlot::bMonitorUpdate=false;
static char szMutexName[200]="p11mutex_";

char *CSlot::mutexName(const char *szName) {
	int dwNameSize=strlen(szName);
	if (dwNameSize>180) dwNameSize=180;
	strncpy_s(szMutexName+9,190,szName,dwNameSize);
	szMutexName[dwNameSize+9]=0;
	return(szMutexName);
}

CSlot::CSlot(const char *szReader) {
	szName.alloc_copy(szReader);
	lastEvent=SE_NoEvent;
	bUpdated=0;
	User=CKU_NOBODY;
	dwP11ObjCnt=1;
	dwSessionCount=0;
	pTemplate=NULL;
	slotMutex.Create(mutexName(szReader));
	pSerialTemplate=NULL;
	hCard = NULL;
}

CSlot::~CSlot() {
	Final();
}

RESULT CSlot::GetNewSlotID(CK_SLOT_ID *pSlotID) {
	init_func
	*pSlotID=dwSlotCnt;
	dwSlotCnt++;
	_return(OK)
	exit_func
	_return(FAIL)
}

static DWORD slotMonitor(SlotMap *pSlotMap)
{
	while (true) {
		CCardContext Context;
		CSlot::ThreadContext=&Context;
		DWORD dwSlotNum=pSlotMap->size();
		DynArray<SCARD_READERSTATE> state(dwSlotNum);
		DynArray<CSlot*> slot(dwSlotNum);
		ZeroMemory(state.lock(),sizeof(SCARD_READERSTATE)*dwSlotNum);
		DWORD i=0;
		DWORD ris;
		{
			CSyncroLocker lock(p11Mutex);
			for (SlotMap::const_iterator it=pSlotMap->begin();it!=pSlotMap->end();it++,i++) {
				state[i].szReader=it->second->szName.stringlock();
				slot[i]=it->second;
				if (ris=SCardGetStatusChange(Context,0,&state[i],1)!=S_OK) {
					if (ris!=SCARD_E_TIMEOUT) {
						Log.write("Errore nella SCardGetStatusChange - %08X",ris);
						// non uso la ExitThread!!!
						// altrimenti non chiamo i distruttori, e mi rimane tutto appeso
						// SOPRATTUTTO il p11Mutex
						return 1;
					}
				}
				state[i].dwCurrentState=state[i].dwEventState & (~SCARD_STATE_CHANGED);
			}
		}
		CSlot::bMonitorUpdate=false;
		while (true) {
			Context.validate();
			ris=SCardGetStatusChange(Context,1000,state.lock(),dwSlotNum);
			if (ris!=S_OK) {
				if (CSlot::bMonitorUpdate || ris==SCARD_E_SYSTEM_CANCELLED || ris==SCARD_E_SERVICE_STOPPED || ris==SCARD_E_INVALID_HANDLE || ris==ERROR_INVALID_HANDLE) {
					Log.write("Monitor Update");
					break;
				}
				if (ris==SCARD_E_CANCELLED || bP11Terminate) {
					Log.write("Terminate");
					// no exitThread, vedi sopra;
					return 0;
				}
				if (ris!=SCARD_E_TIMEOUT && ris!=SCARD_E_NO_READERS_AVAILABLE) {
					Log.write("Errore nella SCardGetStatusChange - %08X",ris);
					// no exitThread, vedi sopra;
					return 1;
				}
			}
			for (DWORD i=0;i<dwSlotNum;i++) {
				if ((state[i].dwCurrentState & SCARD_STATE_PRESENT) &&
					((state[i].dwEventState & SCARD_STATE_EMPTY) || 
					(state[i].dwEventState & SCARD_STATE_UNAVAILABLE))) {
					// una carta è stata estratta!!
					// sincronizzo sul mutex principale p11
					// le funzioni attualmente in esecuzione che vanno sulla
					// carta falliranno miseramente, ma se levi la carta
					// mentre sto firmado mica è colpa mia!

					CSyncroLocker lock(p11Mutex);
					//CSyncroLocker lock2(p11EventMutex);

					slot[i]->lastEvent=SE_Removed;
					slot[i]->Final();
					slot[i]->baATR.clear();
					p11slotEvent.Signal();
				}
				if (((state[i].dwCurrentState & SCARD_STATE_UNAVAILABLE) ||
					(state[i].dwCurrentState & SCARD_STATE_EMPTY)) &&
					(state[i].dwEventState & SCARD_STATE_PRESENT)) {
					// una carta è stata inserita!!
					CSyncroLocker lock(p11Mutex);
					//CSyncroLocker lock(p11EventMutex);
					slot[i]->lastEvent=SE_Inserted;
					ByteArray ba;
					slot[i]->GetATR(ba);
					p11slotEvent.Signal();
				}
				state[i].dwCurrentState=state[i].dwEventState & (~SCARD_STATE_CHANGED);
			}
			if (ris==SCARD_E_NO_READERS_AVAILABLE) {
				Log.write("Nessun lettore connesso - %08X",ris);
				// no exitThread, vedi sopra;
				return 1;
			}
		}
		CSlot::ThreadContext=NULL;
	}
	// no exitThread, vedi sopra;
	return 0;
}

RESULT CSlot::AddSlot(CSlot* pSlot,CK_SLOT_ID *pSlotID)
{
	init_func
	ER_CALL(GetNewSlotID(&pSlot->hSlot),
		ERR_GET_NEW_SLOT)

	*pSlotID=pSlot->hSlot;

	g_mSlots.insert(std::pair<CK_SLOT_ID,CSlot*>(pSlot->hSlot,pSlot));
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::DeleteSlot(CK_SLOT_ID hSlotId)
{
	init_func
	CSlot *pSlot=NULL;
	ER_CALL(GetSlotFromID(hSlotId, &pSlot),
		ERR_CANT_GET_SLOT);

	if (pSlot == NULL) _return(CKR_SLOT_ID_INVALID);

	pSlot->CloseAllSessions();

	g_mSlots.erase(hSlotId);
	delete(pSlot);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::GetSlotFromReaderName(const char *name,CSlot **ppSlot)
{
	init_func
	for (SlotMap::iterator it=g_mSlots.begin();it!=g_mSlots.end();it++) {
		if (strcmp(it->second->szName.stringlock(),name)==0) {
			*ppSlot=it->second;
			_return(OK)
		}
	}
	*ppSlot=NULL;

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::GetSlotFromID(CK_SLOT_ID hSlotId,CSlot**ppSlot) 
{
	init_func
	CSlot* pSlot=NULL;
	SlotMap::const_iterator pPair;
	pPair=g_mSlots.find(hSlotId);
	if (pPair==g_mSlots.end()) {
		*ppSlot=NULL;
		_return(OK)
	}
	*ppSlot=pPair->second;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::DeleteSlotList()
{
	init_func

	int i=0;

	while (i<5) {
		if (Thread.joinThread(1000)==OK)
			break;
		i=i+1;
		if (CSlot::ThreadContext!=NULL)
			SCardCancel(*CSlot::ThreadContext);
	}
	if (i==5) {
		Thread.terminateThread();
	}

	SlotMap::iterator it=CSlot::g_mSlots.begin();
	while (it!=CSlot::g_mSlots.end()) { 
		DeleteSlot(it->second->hSlot);
		it=CSlot::g_mSlots.begin();
	}
	
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::InitSlotList()
{
	// la InitSlotList deve aggiornare la liste degli slot;
	// cioè, deve aggiungere gli slot che non c'erano prima e
	// cancellare quelli che non ci sono più
	init_func
	bool bMapChanged=false;
	DWORD readersLen=0;

	CCardContext Context;

	DWORD ris=SCardListReaders(Context,NULL,NULL,&readersLen);
	if (ris!=S_OK) {
		if (ris==SCARD_E_NO_READERS_AVAILABLE)
			_return(OK)
		_return(FAIL)
	}
	String readers(readersLen+1);
	WIN_R_CALL(SCardListReaders(Context,NULL,readers.lock(readersLen+1),&readersLen),SCARD_S_SUCCESS)
	
	char *szReaderName=readers.stringlock();

	while (*szReaderName!=0) {
		// vediamo questo slot c'era già prima
		Log.write("reader:%s",szReaderName);
		CSlot *pSlot=NULL;
		GetSlotFromReaderName(szReaderName, &pSlot);
		if (!pSlot) {
			Allocator<CSlot,const char *> pSlot(szReaderName);
			CK_SLOT_ID hSlotID;
			AddSlot(pSlot, &hSlotID);
			bMapChanged=true;
			pSlot.detach();
		}
		szReaderName=szReaderName+strlen(szReaderName)+1;
	}
	// adesso vedo se tutti gli slot nella mappa ci sono ancora
	for (SlotMap::iterator it=g_mSlots.begin();it!=g_mSlots.end();it++) {
		Log.write("%s",it->second->szName.pbtData);
		const char *name=it->second->szName.stringlock();

		char *szReaderName=readers.stringlock();
		//char *szReaderName=szReaderAlloc;
		bool bFound=false;
		while (*szReaderName!=0) {
			if (strcmp(name,szReaderName)==0) {
				bFound=true;
				break;
			}
			szReaderName=szReaderName+strlen(szReaderName)+1;
		}
		if (!bFound) {
			CK_SLOT_ID ID=it->second->hSlot;
			it--;
			DeleteSlot(ID);
			bMapChanged=true;
		}
	}
	bMonitorUpdate=bMapChanged;

	if (Thread.dwThreadID==0) {
		Thread.createThread(slotMonitor, &g_mSlots);
	}

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::IsTokenPresent(bool *bPresent)
{
	init_func
	SCARD_READERSTATE state;
	memset(&state,0,sizeof(SCARD_READERSTATE));
	state.szReader=szName.stringlock();

	Context.validate();
	bool retry=false;
	while(true) {
		DWORD ris=SCardGetStatusChange(Context,0,&state,1);
		if (ris==S_OK) {
			if ((state.dwEventState & SCARD_STATE_UNAVAILABLE)==SCARD_STATE_UNAVAILABLE)
				_return(CKR_DEVICE_REMOVED)
			if ((state.dwEventState & SCARD_STATE_PRESENT)==SCARD_STATE_PRESENT)
				*bPresent=true;
			else
				*bPresent=false;
			_return(OK)
		}
		else {
			if (ris==SCARD_E_SERVICE_STOPPED || ris==SCARD_E_INVALID_HANDLE || ris==ERROR_INVALID_HANDLE) {
				// devo prendere un nuovo context e riprovare
				if (!retry)
					retry=true;
				else
					_return(FAIL)
				ER_CALL(Context.renew(), ERR_CANT_ESTABLISH_CONTEXT)
				continue;
			}
			if (ris==SCARD_E_NO_READERS_AVAILABLE)
				_return(CKR_DEVICE_REMOVED)
			_return(FAIL)
		}
	}

	exit_func
	_return(FAIL)
}

CK_RV CSlot::GetInfo(CK_SLOT_INFO_PTR pInfo)
{
	init_func
	pInfo->flags=CKF_REMOVABLE_DEVICE|CKF_HW_SLOT;
	// verifico che ci sia una carta inserita
	bool bPresent=false;
	ER_CALL(IsTokenPresent(&bPresent),
		ERR_TOKEN_PRESENT)

	if (bPresent)
		pInfo->flags |= CKF_TOKEN_PRESENT;

	memset(pInfo->slotDescription,' ',64);
	int iSDLen=min(64,szName.size()-1);
	memcpy(pInfo->slotDescription,szName.lock(iSDLen),iSDLen);

	memset(pInfo->manufacturerID,' ',32);
	// non so esattamente perchè, ma nella R1 il manufacturerID sono i primi 32 dello slotDescription
	int iMIDLen=min(32,szName.size());
	memcpy(pInfo->manufacturerID,szName.lock(iMIDLen),iMIDLen);

	pInfo->hardwareVersion.major = 0;
	pInfo->hardwareVersion.minor = 0;

	pInfo->firmwareVersion.major = 0; 
	pInfo->firmwareVersion.minor = 0; 	

	_return(OK)
	exit_func
	_return(FAIL)
}

CK_RV CSlot::GetTokenInfo(CK_TOKEN_INFO_PTR pInfo)
{
	init_func

	//CToken token;
	//if (token.Connect(szName.stringlock())) _return(CKR_TOKEN_NOT_PRESENT)
	if (!pTemplate) {
		ER_CALL(CCardTemplate::GetTemplate(*this,pTemplate),
			ERR_GET_TEMPLATE)
	}

	if (!pTemplate)
		_return(CKR_TOKEN_NOT_RECOGNIZED)
	
	memset(pInfo->label,' ', sizeof(pInfo->label));
	memcpy((char*)pInfo->label, pTemplate->szName.lock(),min(pTemplate->szName.strlen(),sizeof(pInfo->label)));
	memset(pInfo->manufacturerID, ' ', sizeof(pInfo->manufacturerID));
	memcpy((char*)pInfo->manufacturerID, pTemplate->szManifacturer.lock(), min(pTemplate->szManifacturer.strlen(), sizeof(pInfo->manufacturerID)));
	
	if (baSerial.isEmpty() || pSerialTemplate!=pTemplate) {
		pSerialTemplate=pTemplate;
		ER_CALL(pTemplate->FunctionList.templateGetSerial(*this,baSerial),
			ERR_READ_SERIAL)
	}

	String model;
	ER_CALL(pTemplate->FunctionList.templateGetModel(*this,model),
		ERR_READ_MODEL)

	memset(pInfo->serialNumber,' ',sizeof(pInfo->serialNumber));
	int UIDsize=min(sizeof(pInfo->serialNumber),baSerial.size());
	memcpy(pInfo->serialNumber,baSerial.lock(UIDsize),UIDsize);

	memset(pInfo->model,' ',sizeof(pInfo->model));
	memcpy(pInfo->model,model.lock(),min(model.strlen(),sizeof(pInfo->model)));	

	DWORD dwFlags;
	ER_CALL(pTemplate->FunctionList.templateGetTokenFlags(*this,dwFlags),
		ERR_READ_SERIAL)
	pInfo->flags=dwFlags;

	pInfo->ulTotalPublicMemory=CK_UNAVAILABLE_INFORMATION;
	pInfo->ulTotalPrivateMemory=CK_UNAVAILABLE_INFORMATION;
	pInfo->ulFreePublicMemory=CK_UNAVAILABLE_INFORMATION;
	pInfo->ulFreePrivateMemory=CK_UNAVAILABLE_INFORMATION;
	pInfo->ulMaxSessionCount = MAXSESSIONS;
	DWORD dwSessCount=0;
	SessionCount(dwSessCount);

	pInfo->ulSessionCount = dwSessCount;
	DWORD dwRWSessCount=0;
	RWSessionCount(dwRWSessCount);

	pInfo->ulRwSessionCount = dwRWSessCount;
	pInfo->ulMaxRwSessionCount = MAXSESSIONS; 

	pInfo->ulMinPinLen=5;
	pInfo->ulMaxPinLen=8;

	pInfo->hardwareVersion.major=0;
	pInfo->hardwareVersion.minor=0;

	pInfo->firmwareVersion.major=0;
	pInfo->firmwareVersion.minor=0;

	memcpy((char*)pInfo->utcTime,"1234567890123456",16);  // OK

	_return(OK)
	exit_func
	_return(FAIL)

}

CK_RV CSlot::CloseAllSessions()
{
	init_func

	SessionMap::iterator it=CSession::g_mSessions.begin();
	while (it!=CSession::g_mSessions.end()) {
		if (it->second->pSlot==this) 
		{
			CSession *pSession=it->second;
			it++;
			CSession::DeleteSession(pSession->hSessionHandle);
		}
		else it++;
	}

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::Init()
{
	init_func
	if (!bUpdated) {

		if (!pTemplate) {
			ER_CALL(CCardTemplate::GetTemplate(*this,pTemplate),
				ERR_GET_TEMPLATE)
		}

		if (!pTemplate)
			_return(CKR_TOKEN_NOT_RECOGNIZED)

		ER_CALL(pTemplate->FunctionList.templateInitCard(pTemplateData,*this),
			ERR_CANT_INITIALIZE_CARD_STRUCTURES)
		bUpdated=true;
	}
	_return(OK)
	exit_func
	_return(FAIL)
}

void CSlot::Final()
{
	if (bUpdated) {
		// cancello i dati del template
		pTemplate->FunctionList.templateFinalCard(pTemplateData);
		pTemplate=NULL;

		baATR.clear();
		baSerial.clear();

		for (DWORD i=0;i<P11Objects.size();i++)
			delete(P11Objects[i]);

		P11Objects.clear();
		
		// cancello tutte le sessioni
		SessionMap::iterator it=CSession::g_mSessions.begin();
		while (it!=CSession::g_mSessions.end()) {
			if (it->second->pSlot==this) 
			{
				CSession *pSession=it->second;
				it=CSession::g_mSessions.erase(it);
				dwSessionCount--;
				delete(pSession);
			}
			else it++;
		}
		// dwSessionCount dovrebbe essere già a 0...
		// ma per sicurezza lo setto a manina

		User=CKU_NOBODY;
		dwSessionCount=0;
		bUpdated=false;
	}
}

RESULT CSlot::FindP11Object(CK_OBJECT_CLASS objClass,CK_ATTRIBUTE_TYPE attr,BYTE *val,int valLen,CP11Object *&pObject)
{
	init_func
	pObject=NULL;
	for(DWORD i=0;i<P11Objects.size();i++)
	{
		CP11Object *obj=P11Objects[i];
		if (obj->ObjClass==objClass) {
			ByteArray *attrVal=NULL;
			obj->getAttribute(attr, attrVal);
			if (attrVal && attrVal->size()==valLen) {
				if (memcmp(attrVal->lock(valLen),val,valLen)==0) {
					pObject=obj;
					_return(OK)
				}
			}
		}
	}
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::AddP11Object(CP11Object* p11obj)
{
	init_func
	P11Objects.push_back(p11obj);
	p11obj->pSlot=this;
	_return(OK)
	exit_func
	_return(NULL)
}

RESULT CSlot::ClearP11Objects()
{
	init_func
	for (P11ObjectVector::iterator it = P11Objects.begin(); it != P11Objects.end(); it++) {
		delete (*it);
	}

	P11Objects.clear();
	ObjP11Map.clear();
	HandleP11Map.clear();
	_return(OK)
	exit_func
	_return(NULL)
}

RESULT CSlot::DelP11Object(CP11Object*object)
{
	init_func
	bool bFound=false;
	for(P11ObjectVector::iterator it=P11Objects.begin();it!=P11Objects.end();it++) {
		if (*it==object) {
			bFound=true;
			P11Objects.erase(it);
			break;
		}
	}
	ER_ASSERT(bFound,ERR_FIND_OBJECT)

	ObjHandleMap::iterator itObj=ObjP11Map.find(object);
	if (itObj!=ObjP11Map.end()) {
		HandleObjMap::iterator itHandle=HandleP11Map.find(itObj->second);
		ObjP11Map.erase(itObj);
		if (itHandle!=HandleP11Map.end()) 
			HandleP11Map.erase(itHandle);
	}

	delete object;

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::SessionCount(DWORD &dwSessCount)
{
	init_func
	dwSessCount=dwSessionCount;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::RWSessionCount(DWORD &dwRWSessCount)
{
	init_func
	dwRWSessCount=0;
	for(SessionMap::iterator it=CSession::g_mSessions.begin();it!=CSession::g_mSessions.end();it++) {
		if (it->second->pSlot==this && (it->second->flags & CKF_RW_SESSION)!=0)
			dwRWSessCount++;
	}
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::GetIDFromObject(CP11Object *pObject,CK_OBJECT_HANDLE &hObject)
{
	init_func

	// controllo se l'oggetto è privato
	bool bPrivate=false;
	ER_CALL(pObject->IsPrivate(bPrivate),
		ERR_GET_PRIVATE)

	if (bPrivate && User!=CKU_USER)
		_return(CKR_USER_NOT_LOGGED_IN)

	ObjHandleMap::const_iterator pPair;
	pPair=ObjP11Map.find(pObject);
	if (pPair==ObjP11Map.end()) {
		// non ho trovato l'oggetto nella mappa degli oggetti;
		// devo aggiungerlo

		ER_CALL(GetNewObjectID(hObject),
			ERR_GET_NEW_OBJECT)

		ObjP11Map[pObject]=hObject;
		HandleP11Map[hObject]=pObject;
		_return(OK)
	}
	hObject=pPair->second;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::GetNewObjectID(CK_OBJECT_HANDLE &hObject) {
	init_func	

	hObject=dwP11ObjCnt;
	dwP11ObjCnt++;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::DelObjectHandle(CP11Object *pObject)
{
	init_func
	ObjHandleMap::iterator pPair;
	pPair=ObjP11Map.find(pObject);
	if (pPair!=ObjP11Map.end()) {
		HandleObjMap::iterator pPair2;
		pPair2=HandleP11Map.find(pPair->second);
		if (pPair2!=HandleP11Map.end())
			HandleP11Map.erase(pPair2);
		ObjP11Map.erase(pPair);
	}
	
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::GetObjectFromID(CK_OBJECT_HANDLE hObjectHandle,CP11Object *&pObject)
{
	init_func
	pObject=NULL;
	HandleObjMap::const_iterator pPair;
	pPair=HandleP11Map.find(hObjectHandle);
	if (pPair==HandleP11Map.end()) {
		pObject=NULL;
		_return(OK)
	}
	pObject=pPair->second;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSlot::Connect() {
	init_func
		DWORD dwProtocol;

	Context.validate();
	bool retry = false;
	while (true) {
		DWORD ris = SCardConnect(Context, szName.stringlock(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T1, &hCard, &dwProtocol);
		if (ris == OK) {
			_return(OK)
		}
		else {
			if (ris == SCARD_E_SERVICE_STOPPED || ris == SCARD_E_INVALID_HANDLE || ris == ERROR_INVALID_HANDLE) {
				if (!retry)
					retry = true;
				else {
					throw CStringException(ERR_CANT_ESTABLISH_CONTEXT);
				}
				ER_CALL(Context.renew(),
					ERR_CANT_ESTABLISH_CONTEXT)
			}
			else {
				WIN_R_CALL(ris,SCARD_S_SUCCESS);
			}
		}
	}

	_return(OK)
		exit_func
		_return(FAIL)
}

RESULT CSlot::GetATR(ByteDynArray &ATR)
{
	init_func
		ER_ASSERT(hCard, ERR_CARD_NOT_CONNECTED)


		DWORD dwATRLen = 0;
	WIN_R_CALL(SCardGetAttrib(hCard, SCARD_ATTR_ATR_STRING, NULL, &dwATRLen),SCARD_S_SUCCESS)

		ATR.resize(dwATRLen);
	WIN_R_CALL(SCardGetAttrib(hCard, SCARD_ATTR_ATR_STRING, ATR.lock(), &dwATRLen), SCARD_S_SUCCESS)

		Log.write("ATR Letto:");
	Log.writeBinData(ATR.lock(), ATR.size());
	_return(OK)
		exit_func
		_return(FAIL)
}

RESULT CSlot::GetATR(ByteArray &ATR) {
	init_func
	if (baATR.size()!=0) {
		ATR=baATR;
		_return(OK)
	}
	ER_CALL(Connect(),"Errore nella connessione al token");
	GetATR(baATR);
	ATR=baATR;

	if (hCard)
		SCardDisconnect(hCard, SCARD_LEAVE_CARD);
	hCard = NULL;


	_return(OK)
	exit_func
	_return(FAIL)
}

}
