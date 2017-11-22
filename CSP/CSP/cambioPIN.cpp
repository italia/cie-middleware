#include "../stdafx.h"
#include "IAS.h"
#include "../util/ModuleInfo.h"
#include "../UI/Message.h"
#include "../UI/Pin.h"
#include "../UI/Verifica.h"
#include "../crypto/SHA256.h"
#include <functional>
#include "../crypto/ASNParser.h"
#include "../UI/safeDesktop.h"
#include "../PCSC/PCSC.h"
#include <atlbase.h>

extern CModuleInfo moduleInfo;
extern "C" DWORD WINAPI CardAcquireContext(IN PCARD_DATA pCardData, __in DWORD dwFlags);

#ifdef _WIN64
#pragma comment(linker, "/export:CambioPIN")
#else
#pragma comment(linker, "/export:CambioPIN=_CambioPIN@16")
#endif

DWORD WINAPI _cambioPIN() {
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
		CPin oldPin(4, "Inserire le ultime 4 cifre del PIN della CIE", "", "", "Cambio PIN");
		if (oldPin.DoModal() == IDOK) {
			CPin newPin(4, "Inserire le ultime 4 cifre del nuovo PIN", "", "", "Cambio PIN", true);
			if (newPin.DoModal() == IDOK) {

				safeConnection sc(cData.hScard);
				safeTransaction changeTran(sc, SCARD_LEAVE_CARD);
				if (!changeTran.isLocked())
					return SCARD_E_NO_SMARTCARD;

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
			std::string msg1, msg2;
			if (attempts >= 0) {
				msg1 = "PIN errato. Sono rimasti " + std::to_string(attempts) + " tentativi";
				msg2 = "prima di bloccare il PIN";
			}
			else
				msg1 = "PIN errato";
			CMessage msg(MB_OK, "Cambio PIN", "", msg1.c_str(), msg2.c_str());
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

		CARD_DATA cData;
		DWORD attempts = 0;
		bool isCIE = false;
		bool isEnrolled = false;
		{
			safeTransaction checkTran(conn, SCARD_LEAVE_CARD);
			if (!checkTran.isLocked())
				continue;

			isCIE = checkCIE(conn.hCard, hSC, cData);
			if (isCIE)
				isEnrolled = ((IAS*)cData.pvVendorSpecific)->IsEnrolled();

		}
		if (isCIE) {
			if (!isEnrolled) {
				CMessage msg(MB_OK, "Cambio PIN", "La CIE deve essere abilitata per cambiare PIN");
				msg.DoModal();
				return 0;
			}
			showMessage(changePIN(cData, attempts), attempts);
			return 0;
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
		CMessage msg(MB_CANCEL,
			"Cambio PIN",
			"Appoggiare la CIE sul lettore",
			"per effettuare il cambio PIN");
		data.win = &msg;
		data.hSC = hSC;
		data.checkCIE = checkCIE;
		data.changePIN = changePIN;
		data.showMessage = showMessage;
		readerMonitor monitor([](std::string &reader, bool insert, void* appData) -> void {
			struct _data *data = (struct _data *)appData;
			if (insert) {
				DWORD ris = 0;
				safeConnection conn(data->hSC, reader.c_str(), SCARD_SHARE_SHARED);
				if (conn.hCard == NULL)
					return;

				CARD_DATA cData;
				DWORD attempts = 0;
				bool isCIE = false;
				bool isEnrolled = false;

				{
					safeTransaction checkTran(conn, SCARD_LEAVE_CARD);
					if (!checkTran.isLocked())
						return;
					isCIE = data->checkCIE(conn.hCard, data->hSC, cData);
					if (isCIE)
						isEnrolled = ((IAS*)cData.pvVendorSpecific)->IsEnrolled();
				}
				if (isCIE) {
					if (!isEnrolled) {
						data->win->EndDialog(0);
						CMessage msg(MB_OK, "Cambio PIN", "La CIE deve essere abilitata per cambiare PIN");
						msg.DoModal();
						return;
					}
					data->win->EndDialog(0);
					DWORD attempts = 0;
					data->showMessage(data->changePIN(cData, attempts), attempts);
				}
			}
		},&data);
		auto ris = msg.DoModal();
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

	_cambioPIN();

	ODS("End CambioPIN");
	return 0;
}
