#include "../stdafx.h"
#include "IAS.h"
#include "CSP.h"
#include "../util/ModuleInfo.h"
#include "../UI/Message.h"
#include "../UI/Pin.h"
#include <functional>
#include "../crypto/ASNParser.h"
#include "Cardmod.h"
#include "../UI/SystemTray.h"
#include "../UI/safeDesktop.h"
#include "../PCSC/PCSC.h"
#include <atlbase.h>

extern CModuleInfo moduleInfo;
extern "C" DWORD WINAPI CardAcquireContext(IN PCARD_DATA pCardData, __in DWORD dwFlags);

#ifdef _WIN64
	#pragma comment(linker, "/export:CertificatoScaduto")
#else
	#pragma comment(linker, "/export:CertificatoScaduto=_CertificatoScaduto@16")
#endif

void CloseTrayNotification(CSystemTray* tray, WPARAM uID, LPARAM lEvent) {
	if (lEvent == WM_LBUTTONUP || lEvent == 0x405) {
		tray->HideIcon();
		PostQuitMessage(0);
	}
}

extern "C" int CALLBACK CertificatoScaduto(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
	)
{
	init_CSP_func
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

	ODS("Start CIEScaduta");
	if (!CheckOneInstance("CIEScadutaOnce")) {
		ODS("Already running CIEScaduta");
		return 0;
	}

	CSystemTray tray(wndClass.hInstance, nullptr, WM_APP, "Documento scaduto",
		LoadIcon(wndClass.hInstance, MAKEINTRESOURCE(IDI_CIE)), 1);

	std::string msg = "Attenzione! La CIE ";
	msg = msg.append(lpCmdLine).append(" è scaduta!");
	tray.ShowBalloon(msg.c_str(), "CIE", NIIF_INFO);
	tray.ShowIcon();
	tray.TrayNotification = CloseTrayNotification;
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		if (Msg.message == WM_COMMAND) {
			if (Msg.wParam == 0)
				return 0;
			else {
				tray.ShowIcon();
				tray.ShowBalloon(msg.c_str(), "CIE", NIIF_INFO);
			}
		}
		if (Msg.message == WM_QUIT) {
			return 0;
		}
		DispatchMessage(&Msg);
	}
	exit_CSP_func
	return 0;
}
