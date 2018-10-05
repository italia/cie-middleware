#include "..\StdAfx.h"
#include <Winhttp.h>
#include "..\util/ModuleInfo.h"
#include "..\util/IniSettings.h"
#include "..\UI\SystemTray.h"
#include <vector>
#include <fstream>
#include "..\res\resource.h"
#include "CSP.h"
#include <VersionHelpers.h>

extern CModuleInfo moduleInfo;
std::string latestVersionURL = "";
WNDCLASS wndClass;

#ifdef _WIN64
#pragma comment(linker, "/export:Update")
#else
#pragma comment(linker, "/export:Update=_Update@16")
#endif

void UpdateClick(CSystemTray* tray, WPARAM uID, LPARAM lEvent) {
	if (lEvent == WM_LBUTTONUP || lEvent == NIN_BALLOONUSERCLICK) {
		ShellExecute(NULL, "open", latestVersionURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
		tray->HideIcon();
		PostQuitMessage(0);
	}
}

void QuitClick(CSystemTray* tray, WPARAM uID, LPARAM lEvent) {
	if (lEvent == WM_LBUTTONUP || lEvent == NIN_BALLOONUSERCLICK) {
		tray->HideIcon();
		PostQuitMessage(0);
	}
}
void QuitUpdate(CSystemTray* tray) {
	tray->HideIcon();
	PostQuitMessage(0);
}

void DisplayBaloon(std::string &tip, void(*TrayNotification)(CSystemTray* tray, WPARAM uID, LPARAM lEvent), void(*TrayBaloonTimeout)(CSystemTray* tray))
{
	CSystemTray tray(wndClass.hInstance, nullptr, WM_APP, "Aggiornamento middleware CIE",
		LoadIcon(wndClass.hInstance, MAKEINTRESOURCE(IDI_CIE)), 1);

	tray.ShowIcon();
	tray.ShowBalloon(tip.c_str(), "CIE", NIIF_INFO);
	tray.TrayNotification = TrayNotification;
	tray.TrayBaloonTimeout = TrayBaloonTimeout;
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

void checkVersion(std::string iniPath,bool alwaysDisplay) {
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

	bool deprecated = false;
	bool openURL = false;
	for (int i = 1;;i++) {
		std::string versionVar("version");
		versionVar.append(std::to_string(i));
		IniSettingsString IniVersion("Versions", versionVar.c_str(),"","Versions");
		std::string versionNumber;
		IniVersion.GetValue(iniPath.c_str(), versionNumber);
		if (versionNumber.empty())
			break;

		IniSettingsString IniVersionNumber(versionNumber.c_str(), "number", "", "Version number");
		IniSettingsString IniVersionStatus(versionNumber.c_str(), "status", "", "Version status");
		IniSettingsString IniVersionURL(versionNumber.c_str(), "httpURL", "", "Version URL");
		IniSettingsString IniVersionInstallerURL(versionNumber.c_str(), "installerURL", "", "Installer URL");
		IniSettingsString IniVersionAutoUpdate(versionNumber.c_str(), "autoupdate", "", "Version autoupdate");

		std::string num;
		IniVersionNumber.GetValue(iniPath.c_str(), num);
		std::string status;
		IniVersionStatus.GetValue(iniPath.c_str(), status);
		std::string httpURL;
		IniVersionURL.GetValue(iniPath.c_str(), httpURL);
		std::string installerURL;
		IniVersionInstallerURL.GetValue(iniPath.c_str(), installerURL);
		std::string autoupdate;
		IniVersionAutoUpdate.GetValue(iniPath.c_str(), autoupdate);

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

		if (isMoreRecent && (status == "stable")) {
			// versione più recente
			if (PathIsURL(httpURL.c_str())) {
				if (latestVersionURL == "")
					latestVersionURL = httpURL;
				openURL = true;
			}
		}
		if (major == thisMajor && minor == thisMinor && revision == thisRevision && build == thisBuild) {
			// questa release
			if (status == "deprecated")
				deprecated = true;
			break;
		}
	}
	std::vector<std::string> rows;
	if (deprecated) {
		rows.push_back("Attenzione! Questa versione del");
		rows.push_back("Middleware CIE non è affidabile!");
		if (!openURL)
			rows.push_back("Si consiglia di disinstallarla");
	}
	else {
		if (openURL) {
			rows.push_back("E' stata pubblicata una versione");
			rows.push_back("aggiornata del Middleware CIE");
		}
	}
	if (openURL) {
		rows.push_back("Si consiglia di installare l'ultima versione.");
		rows.push_back("Premere OK per aprire la pagina di download");
	}

	std::string tip;
	if (rows.size() == 0)
		if (alwaysDisplay)
			tip = "La versione installata è già aggiornata";
		else
			return;
	else {
		tip = rows[0];
		for (size_t i = 1; i < rows.size(); i++) {
			tip += "\n";
			tip += rows[i];
		}
	}

	if (openURL)
		DisplayBaloon(tip, UpdateClick, QuitUpdate);
	else
		DisplayBaloon(tip, QuitClick, QuitUpdate);
	return;
}

string getRandomPrefix() {
	// genera un prefisso di 3 caratteri alfanumerici random
	string prefix;
	ByteDynArray rand(3);
	rand.random();
	for (int i = 0; i < 3; i++) {
		int seed = rand[i] % 36;
		if (seed < 10)
			prefix += ('0' + (char)seed);
		else
			prefix += ('A' + (char)(seed-10));
	}
	return prefix;
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

	std::string displayCommand = "Display";
	bool alwaysDisplay = false;
	alwaysDisplay = displayCommand == lpCmdLine;

	GetClassInfo(NULL, WC_DIALOG, &wndClass);
	wndClass.hInstance = (HINSTANCE)moduleInfo.getModule();
	wndClass.style |= CS_DROPSHADOW;
	wndClass.lpszClassName = "CIEDialog";
	RegisterClass(&wndClass);

	BOOL bResults = FALSE;
	HINTERNET hSession = nullptr,
		hConnect = nullptr,
		hRequest = nullptr;

	DWORD accessType = 0;
	if (IsWindows8Point1OrGreater())
		accessType = WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY;
	else
		accessType = WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;

	// Use WinHttpOpen to obtain a session handle.
	hSession = WinHttpOpen(L"CIE Middleware",
		accessType,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	bool downloadIni = false;
	// Specify an HTTP server.
	if (hSession) {
		hConnect = WinHttpConnect(hSession, L"www.cartaidentita.interno.gov.it",
			INTERNET_DEFAULT_HTTPS_PORT, 0);

		// Create an HTTP Request handle.
		if (hConnect != nullptr) {
			hRequest = WinHttpOpenRequest(hConnect, L"GET",
				L"version-list.txt",
				NULL, WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				WINHTTP_FLAG_SECURE);

			// Send a Request.
			if (hRequest != nullptr) {
				bResults = WinHttpSendRequest(hRequest,
					WINHTTP_NO_ADDITIONAL_HEADERS,
					0, WINHTTP_NO_REQUEST_DATA, 0,
					0, 0);

				if (bResults) {
					bResults = WinHttpReceiveResponse(hRequest, nullptr);

					if (bResults) {
						DWORD StatusCode = 0;
						DWORD Size = sizeof(StatusCode);

						if (WinHttpQueryHeaders(hRequest,
							WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
							WINHTTP_HEADER_NAME_BY_INDEX,
							&StatusCode, &Size, WINHTTP_NO_HEADER_INDEX)) {

							if (StatusCode == 200) {
								ByteDynArray response;
								BYTE chunk[4096];
								DWORD readBytes = 0;
								while (WinHttpReadData(hRequest, chunk, 4096, &readBytes)) {
									if (readBytes == 0)
										break;
									response.append(ByteArray(chunk, readBytes));
								}

								downloadIni = true;

								WinHttpCloseHandle(hRequest);
								WinHttpCloseHandle(hConnect);
								WinHttpCloseHandle(hSession);
								hRequest = nullptr;
								hConnect = nullptr;
								hSession = nullptr;

								char TempPathBuffer[MAX_PATH-14];
								char TempFileName[MAX_PATH];
								if (GetTempPath(MAX_PATH - 14, TempPathBuffer) != 0) {
									string prefix = getRandomPrefix();
									if (GetTempFileName(TempPathBuffer, prefix.c_str(), 0, TempFileName) != 0) {
										std::ofstream str(TempFileName, std::ofstream::binary);
										str.write((const char*)response.data(),response.size());
										str.close();
										checkVersion(std::string(TempFileName), alwaysDisplay);
										DeleteFile(TempFileName);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	if (!downloadIni && alwaysDisplay)
		DisplayBaloon(std::string("Errore nella comunicazione con il server"), QuitClick, QuitUpdate);
	exit_CSP_func
		return 0;
}

