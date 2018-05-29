#include "StdAfx.h"
#include <Winhttp.h>
#include "Util\tinyxml2.h"
#include <Winver.h>
#include "util/ModuleInfo.h"
#include "UI/SystemTray.h"
#include <vector>
#include "res\resource.h"
#include "CSP\CSP.h"

extern CModuleInfo moduleInfo;
std::string latestVersionURL = "";
WNDCLASS wndClass;

#ifdef _WIN64
#pragma comment(linker, "/export:Update")
#else
#pragma comment(linker, "/export:Update=_Update@16")
#endif

void UpdateClick(CSystemTray* tray, WPARAM uID, LPARAM lEvent) {
	if (lEvent == WM_LBUTTONUP || lEvent == 0x405) {
		ShellExecute(NULL, "open", latestVersionURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
		tray->HideIcon();
		PostQuitMessage(0);
	}
}

void QuitClick(CSystemTray* tray, WPARAM uID, LPARAM lEvent) {
	if (lEvent == WM_LBUTTONUP || lEvent == 0x405) {
		tray->HideIcon();
		PostQuitMessage(0);
	}
}

void checkVersion(tinyxml2::XMLDocument &doc) {
	VS_FIXEDFILEINFO    *pFileInfo = NULL;
	UINT                puLenFileInfo = 0;
	
	auto InfoSize = GetFileVersionInfoSize(moduleInfo.szModuleFullPath.c_str(), NULL);
	if (InfoSize == 0)
		return;

	ByteDynArray VersionInfo(InfoSize);
	if (!GetFileVersionInfo(moduleInfo.szModuleFullPath.c_str(), 0, InfoSize, VersionInfo.data()))
		return;

	if (!VerQueryValue(VersionInfo.data(), TEXT("\\"), (LPVOID*)&pFileInfo, &puLenFileInfo))
		return;

	auto thisMajor = (pFileInfo->dwProductVersionMS >> 16) & 0xffff;
	auto thisMinor = (pFileInfo->dwProductVersionMS >> 0) & 0xffff;
	auto thisRevision = (pFileInfo->dwProductVersionLS >> 16) & 0xffff;
	auto thisBuild = (pFileInfo->dwProductVersionLS >> 0) & 0xffff;

	auto versions = doc.FirstChildElement("versions");
	if (versions == NULL)
		return;
	std::string verName("version");
	bool deprecated = false;
	bool later = false;
	for (auto version = versions->FirstChildElement(); version != NULL; version = version->NextSiblingElement()) {
		if (verName != version->Name())
			continue;
		auto number = version->FirstChildElement("number");
		auto status = version->FirstChildElement("status");
		auto httpURL = version->FirstChildElement("httpURL");
		auto installerURL = version->FirstChildElement("installerURL");
		auto autoupdate = version->FirstChildElement("autoupdate");
		std::string num = number->GetText();
		size_t off = 0, off2;
		DWORD major, minor, revision, build;
		off2 = num.find('.', off); major = std::stoi(num.substr(off, off2 - off)); off = off2 + 1;
		off2 = num.find('.', off); minor = std::stoi(num.substr(off, off2 - off)); off = off2 + 1;
		off2 = num.find('.', off); revision = std::stoi(num.substr(off, off2 - off)); off = off2 + 1;
		off2 = num.find('.', off); build = std::stoi(num.substr(off, off2 - off)); off = off2 + 1;

		bool isMoreRecent = false;
		if (thisMajor < major)
			isMoreRecent = true;
		else if (thisMajor == major) {
			if (thisMinor < minor)
				isMoreRecent = true;
			else if (thisMinor == minor) {
				if (thisRevision < revision)
					isMoreRecent = true;
				else if (thisRevision == revision) {
					if (thisBuild < build)
						isMoreRecent = true;
				}
			}
		}

		if (isMoreRecent) {
			// versione più recente
			if (latestVersionURL == "")
				latestVersionURL = httpURL->GetText();
			later = true;
		}
		if (major == thisMajor && minor == thisMinor && revision == thisRevision && build == thisBuild) {
			// questa release
			if (std::string(status->GetText()) == "deprecated")
				deprecated = true;
			break;
		}
	}
	std::vector<std::string> rows;
	if (deprecated) {
		rows.push_back("Attenzione! Questa versione del");
		rows.push_back("Middleware CIE non è affidabile!");
		if (!later)
			rows.push_back("Si consiglia di disinstallarla");
	}
	else {
		if (later) {
			rows.push_back("E' stata pubblicata una versione");
			rows.push_back("aggiornata del Middleware CIE");
		}
	}
	if (later) {
		rows.push_back("Si consiglia di installare l'ultima versione.");
		rows.push_back("Premere OK per aprire la pagina di download");
	}
	if (rows.size() == 0)
		return;

	std::string tip = rows[0];
	for (int i = 1; i < rows.size(); i++) {
		tip += "\n";
		tip += rows[i];
	}


	CSystemTray tray(wndClass.hInstance, nullptr, WM_APP, "Aggiornamento middleware CIE",
		LoadIcon(wndClass.hInstance, MAKEINTRESOURCE(IDI_CIE)), 1);
	
	tray.ShowBalloon(tip.c_str(), "CIE", NIIF_INFO);
	tray.ShowIcon();
	if (later)
		tray.TrayNotification = UpdateClick;
	else
		tray.TrayNotification = QuitClick;
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return;
}

extern "C" int CALLBACK Update(
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

	GetClassInfo(NULL, WC_DIALOG, &wndClass);
	wndClass.hInstance = (HINSTANCE)moduleInfo.getModule();
	wndClass.style |= CS_DROPSHADOW;
	wndClass.lpszClassName = "CIEDialog";
	RegisterClass(&wndClass);

	BOOL bResults = FALSE;
	HINTERNET hSession = nullptr,
		hConnect = nullptr,
		hRequest = nullptr;

	// Use WinHttpOpen to obtain a session handle.
	hSession = WinHttpOpen(L"CIE Middleware",
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	// Specify an HTTP server.
	if (hSession)
		hConnect = WinHttpConnect(hSession, L"localhost",
			INTERNET_DEFAULT_HTTP_PORT, 0);
	
	// Create an HTTP Request handle.
	if (hConnect != nullptr)
		hRequest = WinHttpOpenRequest(hConnect, L"GET",
			L"/temp/version-list.xml",
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			0);

	// Send a Request.
	if (hRequest != nullptr)
		bResults = WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0, WINHTTP_NO_REQUEST_DATA, 0,
			0, 0);

	if (bResults) {
		bResults = WinHttpReceiveResponse(hRequest, nullptr);

		ByteDynArray response;
		BYTE chunk[4096];
		DWORD readBytes = 0;
		while (WinHttpReadData(hRequest, chunk, 4096, &readBytes)) {
			if (readBytes == 0)
				break;
			response.append(ByteArray(chunk, readBytes));
		}
		tinyxml2::XMLDocument doc;
		doc.Parse((char*)response.data(), response.size());
		checkVersion(doc);
	}

	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);
	exit_CSP_func
		return 0;
}

