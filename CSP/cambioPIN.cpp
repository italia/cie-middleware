#include "stdafx.h"
#include "IAS.h"
#include "ModuleInfo.h"
#include "Message.h"
#include "Pin.h"
#include "Verifica.h"
#include "SHA256.h"
#include <functional>
#include "ASNParser.h"
#include "safeDesktop.h"
#include "PCSC.h"
#include <atlbase.h>

extern CModuleInfo moduleInfo;
extern "C" DWORD WINAPI CardAcquireContext(IN PCARD_DATA pCardData, __in DWORD dwFlags);

#ifdef _WIN64
#pragma comment(linker, "/export:CambioPIN")
#else
#pragma comment(linker, "/export:CambioPIN=_CambioPIN@16")
#endif

DWORD WINAPI _cambioPIN(
	LPVOID lpThreadParameter) {
	init_main_func

	SCARDCONTEXT hSC;

	SCardEstablishContext(SCARD_SCOPE_SYSTEM, nullptr, nullptr, &hSC);
	char *readers = nullptr;
	DWORD len = SCARD_AUTOALLOCATE;
	if (SCardListReaders(hSC, nullptr, (char*)&readers, &len) != SCARD_S_SUCCESS || readers==nullptr) {
		CMessage msg(MB_OK,
			"Cambio PIN",
			"Nessun lettore di smartcard installato");
		msg.DoModal();
		return 0;
	}
	
	char *curreader = readers;
	bool foundCIE = false;

	auto changePIN = [](CARD_DATA &cData,DWORD &attempts) -> DWORD {
		CPin oldPin("Inserire il PIN della CIE", "Cambio PIN");
		if (oldPin.DoModal() == IDOK) {
			CPin newPin("Inserire il nuovo PIN", "Cambio PIN", true);
			if (newPin.DoModal() == IDOK) {
				auto ias = ((IAS*)cData.pvVendorSpecific);
				auto ris= CardChangeAuthenticatorEx(&cData, PIN_CHANGE_FLAG_CHANGEPIN,
					ROLE_USER, (BYTE*)oldPin.PIN, (DWORD)strnlen(oldPin.PIN, sizeof(oldPin.PIN)),
					ROLE_USER, (BYTE*)newPin.PIN, (DWORD)strnlen(newPin.PIN, sizeof(newPin.PIN)), 0, &attempts);
				if (ris == SCARD_W_CHV_BLOCKED)
					ias->IconaSbloccoPIN();

				return ris;
			}
			else
				return ERROR_CANCELLED;
		}
		else
			return ERROR_CANCELLED;
	};

	auto showMessage = [](DWORD ris, int attempts) -> void {


		switch (ris) {
		case SCARD_S_SUCCESS:
		{
			CMessage msg(MB_OK, "Cambio PIN", "Il PIN è stato cambiato correttamente");
			msg.DoModal();
			return;
		}
		case ERROR_CANCELLED:
		{
			return;
		}
		case SCARD_W_WRONG_CHV:
		{
			String msg1, msg2;
			if (attempts >= 0) {
				msg1.printf("PIN errato. Sono rimasti %i tentativi", attempts);
				msg2 = "prima di bloccare il PIN";
			}
			else
				msg1 = "PIN errato";
			CMessage msg(MB_OK, "Cambio PIN", "", msg1.lock(), msg2.lock());
			msg.DoModal();
			return;
		}
		case SCARD_W_CHV_BLOCKED:
		{
			CMessage msg(MB_OK, "Cambio PIN", "Il PIN è bloccato");
			msg.DoModal();
			return;
		}
		default:
		{
			CMessage msg(MB_OK, "Cambio PIN", "Errore nel cambio PIN");
			msg.DoModal();
			return;
		}
		}};

	auto checkCIE = [](SCARDHANDLE hCard, SCARDCONTEXT hSC, CARD_DATA &cData) -> bool {
		ZeroMem(cData);
		cData.dwVersion = 7;
		cData.hSCardCtx = hSC;

		DWORD len = SCARD_AUTOALLOCATE;
		cData.hScard = hCard;
		SCardGetAttrib(cData.hScard, SCARD_ATTR_ATR_STRING, (BYTE*)&cData.pbAtr, &len);
		cData.pfnCspAlloc = (PFN_CSP_ALLOC)CryptMemAlloc;
		cData.pfnCspReAlloc = (PFN_CSP_REALLOC)CryptMemRealloc;
		cData.pfnCspFree = (PFN_CSP_FREE)CryptMemFree;
		cData.cbAtr = len;
		cData.pwszCardName = L"CIE";
		auto isCIE = CardAcquireContext(&cData, 0)==SCARD_S_SUCCESS;
		SCardFreeMemory(cData.hScard, cData.pbAtr);
		return isCIE;		
	};


	for (; curreader[0] != 0; curreader += strnlen(curreader, len) + 1) {
		safeConnection conn(hSC, curreader, SCARD_SHARE_SHARED);
		if (conn.hCard == NULL)
			continue;

		{
			safeTransaction checkTran(conn, SCARD_LEAVE_CARD);
			if (!checkTran.isLocked())
				continue;

			CARD_DATA cData;
			DWORD attempts = 0;
			if (checkCIE(conn.hCard, hSC, cData)) {
				showMessage(changePIN(cData, attempts), attempts);
				return 0;
			}
		}
	}
	SCardFreeMemory(hSC, readers);

	if (!foundCIE) {
		struct _data {
			CMessage *win;
			SCARDCONTEXT hSC;
			bool(*checkCIE)(SCARDHANDLE hCard, SCARDCONTEXT hSC, CARD_DATA &cData);
			DWORD(*changePIN)(CARD_DATA &cData,DWORD &attempts);
			void(*showMessage)(DWORD ris, int attempts);
		} data;
		CMessage aa(MB_CANCEL,
			"Cambio PIN",
			"Appoggiare la CIE sul lettore",
			"per effettuare il cambio PIN");
		data.win = &aa;
		data.hSC = hSC;
		data.checkCIE = checkCIE;
		data.changePIN = changePIN;
		data.showMessage = showMessage;
		readerMonitor monitor([](String &reader, bool insert, void* appData) -> void {
			struct _data *data = (struct _data *)appData;
			if (insert) {
				DWORD ris = 0;
				safeConnection conn(data->hSC, reader.lock(), SCARD_SHARE_SHARED);
				if (conn.hCard == NULL)
					return;

				{
					safeTransaction checkTran(conn, SCARD_LEAVE_CARD);
					if (!checkTran.isLocked())
						return;
					CARD_DATA cData;
					if (data->checkCIE(conn.hCard, data->hSC, cData)) {
						CMessage *aa = (CMessage *)appData;
						data->win->EndDialog(0);
						DWORD attempts = 0;
						data->showMessage(data->changePIN(cData, attempts), attempts);
					}
				}
			}
		},&data);
		auto ris = aa.DoModal();
	}

	return 0;
	exit_main_func
	return E_UNEXPECTED;
}

extern "C" int CALLBACK CambioPIN(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
	)
{
	if (_AtlWinModule.cbSize != sizeof(_ATL_WIN_MODULE)) {
		_AtlWinModule.cbSize = sizeof(_ATL_WIN_MODULE);
		AtlWinModuleInit(&_AtlWinModule);
	}
	WNDCLASS wndClass;
	GetClassInfo(NULL, WC_DIALOG, &wndClass);
	wndClass.hInstance = (HINSTANCE)moduleInfo.getModule();
	wndClass.style |= CS_DROPSHADOW;
	wndClass.lpszClassName = "CIEDialog";
	RegisterClass(&wndClass);

	ODS("Start CambioPIN");

	DWORD id;
	HANDLE thread = CreateThread(nullptr, 0, _cambioPIN, lpCmdLine, 0, &id);
	if (thread == NULL) {
		ODS("Errore in creazione thread su CambioPIN");
		return 0;
	}

	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);
	ODS("End CambioPIN");
	return 0;
}
