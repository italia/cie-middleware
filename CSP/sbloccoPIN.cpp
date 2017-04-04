#include "stdafx.h"
#include "IAS.h"
#include "ModuleInfo.h"
#include "Message.h"
#include "Pin.h"
#include <functional>
#include "ASNParser.h"
#include "Cardmod.h"
#include "SystemTraySDK.h"
#include "safeDesktop.h"
#include <atlbase.h>

extern CModuleInfo moduleInfo;
extern "C" DWORD WINAPI CardAcquireContext(IN PCARD_DATA pCardData, __in DWORD dwFlags);

#ifdef _WIN64
#pragma comment(linker, "/export:SbloccoPIN")
#else
#pragma comment(linker, "/export:SbloccoPIN=_SbloccoPIN@16")
#endif

DWORD WINAPI _sbloccoPIN(
	LPVOID lpThreadParameter) {
	init_main_func

		try {
		DWORD len = 0;
		ByteDynArray IdServizi;
		safeDesktop *desk = nullptr;
		Allocator<safeDesktop> alloDesktop(desk);

		SCARDCONTEXT hSC;

		SCardEstablishContext(SCARD_SCOPE_SYSTEM, nullptr, nullptr, &hSC);
		char *readers = nullptr;
		len = SCARD_AUTOALLOCATE;
		SCardListReaders(hSC, nullptr, (char*)&readers, &len);
		char *curreader = readers;
		bool foundCIE = false;
		for (; curreader[0] != 0; curreader += strlen(curreader) + 1) {
			CARD_DATA cData;
			ZeroMemory(&cData, sizeof(cData));
			cData.dwVersion = 7;
			cData.hSCardCtx = hSC;
			{
				safeConnection conn(hSC, curreader, SCARD_SHARE_SHARED);
				if (conn.hCard == NULL)
					continue;


				{
					safeTransaction checkTran(conn, SCARD_LEAVE_CARD);
					if (!checkTran.isLocked())
						continue;

					len = SCARD_AUTOALLOCATE;
					cData.hScard = conn;
					SCardGetAttrib(cData.hScard, SCARD_ATTR_ATR_STRING, (BYTE*)&cData.pbAtr, &len);
					cData.pfnCspAlloc = (PFN_CSP_ALLOC)CryptMemAlloc;
					cData.pfnCspReAlloc = (PFN_CSP_REALLOC)CryptMemRealloc;
					cData.pfnCspFree = (PFN_CSP_FREE)CryptMemFree;
					cData.cbAtr = len;
					cData.pwszCardName = L"CIE";
					if (CardAcquireContext(&cData, 0) != 0)
						continue;
				}
				foundCIE = true;
				if (desk == nullptr)
					desk = new safeDesktop("AbilitaCIE");

				CPin pin(IDB_BACKGROUND, "Inserire il PUK della CIE");
				if (pin.DoModal() == IDOK) {
					try {

						safeTransaction Tran(conn, SCARD_LEAVE_CARD);
						if (!Tran.isLocked())
							continue;

						len = 0;
						auto ias = ((IAS*)cData.pvVendorSpecific);

						auto ris = CardUnblockPin(&cData, wszCARD_USER_USER, (BYTE*)pin.PIN, strlen(pin.PIN), nullptr, 0, -1, CARD_AUTHENTICATE_PIN_PIN);
						if (ris == SCARD_W_WRONG_CHV) {
							String num;
							if (ias->attemptsRemaining >= 0)
								num.printf("Sono rimasti %i tentativi prima del blocco del PUK", ias->attemptsRemaining);
							else
								num = "";
							CMessage aa(IDB_BACKGROUND, MB_OK,
								"PUK Errato",
								num.lock());
							aa.DoModal();
							if (lpThreadParameter != nullptr)
								PostThreadMessage((DWORD)lpThreadParameter, WM_COMMAND, 1, 0);

							break;
						}
						else if (ris == SCARD_W_CHV_BLOCKED) {
							CMessage aa(IDB_BACKGROUND, MB_OK,
								"",
								"Il PUK è bloccato. La CIE non può più essere sbloccata");
							aa.DoModal();
							if (lpThreadParameter != nullptr)
								PostThreadMessage((DWORD)lpThreadParameter, WM_COMMAND, 0, 0);
							break;
						}
						else if (ris != 0)
							throw CStringException("Autenticazione fallita");

						CMessage aa(IDB_BACKGROUND, MB_OK, "",
							"Il PIN è sbloccato");
						aa.DoModal();
						if (lpThreadParameter != nullptr)
							PostThreadMessage((DWORD)lpThreadParameter, WM_COMMAND, 0, 0);

					}
					catch (CBaseException &ex) {
						String dump;
						ex.DumpTree(dump);
						CMessage aa(IDB_BACKGROUND, MB_OK,
							"Si è verificato un errore nella verifica del PUK");
						aa.DoModal();
						break;
					}
				}
				else
					if (lpThreadParameter != nullptr)
						PostThreadMessage((DWORD)lpThreadParameter, WM_COMMAND, 1, 0);
				break;
			}
		}
		if (!foundCIE) {
			if (desk == nullptr)
				desk = new safeDesktop("AbilitaCIE");
			CMessage aa(IDB_BACKGROUND, MB_OK,
				"Impossibile trovare una CIE",
				"nei lettori di smart card");
			aa.DoModal();
			if (lpThreadParameter != nullptr)
				PostThreadMessage((DWORD)lpThreadParameter, WM_COMMAND, 0, 0);
		}
	}
	catch (CBaseException &ex) {
		String dump;
		ex.DumpTree(dump);
		MessageBox(nullptr, String().printf("Si è verificato un errore nella verifica di autenticità del documento :%s", dump.lock()).lock(), "CIE", MB_OK);
	}

	return 0;
	exit_main_func
	return E_UNEXPECTED;
}

void TrayNotification(CSystemTray* tray, WPARAM uID, LPARAM lEvent) {
	if (lEvent == WM_LBUTTONUP || lEvent== 0x405) {
		DWORD id;
		HANDLE thread = CreateThread(nullptr, 0, _sbloccoPIN, (LPVOID)GetCurrentThreadId(), 0, &id);
		tray->HideIcon();
	}
}

extern "C" int CALLBACK SbloccoPIN(
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

	ODS("Start SbloccoPIN");
	if (!CheckOneInstance("CIESbloccoOnce")) {
		ODS("Already running SbloccoPIN");
		return 0;
	}	

	if (strcmp(lpCmdLine, "ICON") == 0) {
		CSystemTray tray(wndClass.hInstance, nullptr, WM_APP, "Premere per sbloccare il PIN della CIE",
			LoadIcon(wndClass.hInstance, MAKEINTRESOURCE(IDI_CIE)), 1);
		tray.ShowBalloon("Premere per sbloccare il PIN dalla CIE", "CIE", NIIF_INFO);
		tray.ShowIcon();
		tray.TrayNotification = TrayNotification;
		MSG Msg;
		while (GetMessage(&Msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&Msg);
			if (Msg.message == WM_COMMAND) {
				if (Msg.wParam == 0)
					return 0;
				else {
					tray.ShowIcon();
					tray.ShowBalloon("Premere per sbloccare il PIN dalla CIE", "CIE", NIIF_INFO);
				}
			}
			DispatchMessage(&Msg);
		}
	}
	else {
		DWORD id;
		HANDLE thread = CreateThread(nullptr, 0, _sbloccoPIN, lpCmdLine, 0, &id);
		WaitForSingleObject(thread, INFINITE);
		ODS("End SbloccoPIN");
		return 0;
	}
}
