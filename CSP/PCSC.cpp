#include "PCSC.h"
#include "UtilException.h"

struct transData {
	SCARDCONTEXT context;
	bool started;
	HANDLE thread;
};
bool safeTransaction::isLocked() {
	return locked;
}
safeTransaction::safeTransaction(safeConnection &conn, DWORD dwDisposition) {
	this->hCard = conn.hCard;
	this->dwDisposition = dwDisposition;
	locked = false;

	struct transData *td = new struct transData;
	td->context = conn.hContext;
	td->started = false;
	DWORD id = 0;
	td->thread = CreateThread(nullptr, 0, [](LPVOID lpThreadParameter) -> DWORD {
		struct transData *td = (struct transData*)lpThreadParameter;
		for (int i = 0; i < 10; i++) {
			Sleep(500);
			if (td->started) {
				CloseHandle(td->thread);
				delete td;
				return 0;
			}
		}
		SCardCancel(td->context);
		CloseHandle(td->thread);
		delete td;
		return 0;
	}, td, 0, &id);


	if (SCardBeginTransaction(hCard) != SCARD_S_SUCCESS) {
		this->hCard = NULL;
		this->dwDisposition = 0;
		return;
	}
	else {
		td->started = true;
		locked = true;
	}
}

void safeTransaction::unlock() {
	if (hCard != NULL && locked) {
		SCardEndTransaction(hCard, dwDisposition);
		locked = false;
	}
}

safeTransaction::~safeTransaction() {
	if (hCard != NULL && locked) {
		SCardEndTransaction(hCard, dwDisposition);
	}
}

safeConnection::safeConnection(SCARDHANDLE hCard) {
	this->hCard = hCard;
}

safeConnection::safeConnection(SCARDCONTEXT hContext, LPCSTR szReader, DWORD dwShareMode) {
	DWORD dwProtocol;
	this->hContext = hContext;
	if (SCardConnect(hContext, szReader, dwShareMode, SCARD_PROTOCOL_T1, &hCard, &dwProtocol) != SCARD_S_SUCCESS)
		hCard = NULL;
}

safeConnection::~safeConnection() {
	if (hCard) {
		SCardDisconnect(hCard, SCARD_RESET_CARD);
	}
}
safeConnection::operator SCARDHANDLE() {
	return hCard;
}

readerMonitor::~readerMonitor() {
	stopMonitor = true;
	SCardCancel(hContext);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	SCardReleaseContext(hContext);
}

readerMonitor::readerMonitor(void(*eventHandler)(String &reader, bool insert, void *appData), void *appData) : appData(appData) {
	LONG _call_ris;
	if ((_call_ris = (SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext))) != S_OK) {
		throw CWinException();
	}
	stopMonitor = false;
	readerEvent = eventHandler;
	DWORD tid = 0;
	hThread = CreateThread(nullptr, 0, [](LPVOID lpThreadParameter) -> DWORD {
		readerMonitor *rm = (readerMonitor *)lpThreadParameter;
		std::vector<String> readerList;
		DynArray<SCARD_READERSTATE> states;

		auto loadReaderList = [&]() -> void {
			char *readers = nullptr;
			DWORD len = SCARD_AUTOALLOCATE;
			if (SCardListReaders(rm->hContext, nullptr, (char*)&readers, &len) != SCARD_S_SUCCESS || readers==nullptr) {
				throw CStringException("Nessun lettore installato");
			}
			char *curReader = readers;
			readerList.clear();
			for (; curReader[0] != 0; curReader += strnlen(curReader, len) + 1)
				readerList.push_back(String(curReader));

			SCardFreeMemory(rm->hContext, readers);
			states.resize((DWORD)readerList.size() + 1);
			for (DWORD i = 0; i < readerList.size(); i++) {
				ZeroMem(states[i]);
				states[i].szReader = readerList[i].lock();
			}
			auto &PnP = states[(DWORD)readerList.size()];
			ZeroMem(PnP);
			PnP.szReader = "\\\\?PnP?\\Notification";
			PnP.pvUserData = (void*)PnP.szReader;		

			SCardGetStatusChange(rm->hContext, 0, states.lock(), states.size());
			for (DWORD i = 0; i < states.size(); i++)
				states[i].dwCurrentState = states[i].dwEventState;
		};
		loadReaderList();

		while (!rm->stopMonitor) {
			if (SCardGetStatusChange(rm->hContext, INFINITE, states.lock(), states.size()) == SCARD_E_CANCELLED)
				break;
			for (DWORD i = 0; i < states.size(); i++) {
				auto &state = states[i];
				if (state.pvUserData != nullptr && (state.dwEventState & SCARD_STATE_CHANGED) == SCARD_STATE_CHANGED)
				{
					loadReaderList();
					break;
				}
				if (((state.dwCurrentState & SCARD_STATE_PRESENT) == SCARD_STATE_PRESENT) &&
					((state.dwEventState & SCARD_STATE_PRESENT) == 0))
					rm->readerEvent(readerList[i], false, rm->appData);

				else if (((state.dwCurrentState & SCARD_STATE_PRESENT) == 0) &&
					((state.dwEventState & SCARD_STATE_PRESENT) == SCARD_STATE_PRESENT))
					rm->readerEvent(readerList[i], true, rm->appData);

				state.dwCurrentState = state.dwEventState;
			}
		}
		return 0;
	}, this, 0, &tid);
}