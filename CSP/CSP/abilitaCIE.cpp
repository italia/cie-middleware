#include "../stdafx.h"
#include "IAS.h"
#include "CSP.h"
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
#include <string>

extern CModuleInfo moduleInfo;
extern "C" DWORD WINAPI CardAcquireContext(IN PCARD_DATA pCardData, __in DWORD dwFlags);

#ifdef _WIN64
	#pragma comment(linker, "/export:AbilitaCIE")
#else
	#pragma comment(linker, "/export:AbilitaCIE=_AbilitaCIE@16")
#endif

struct threadData {
	HWND *progWin;
	HDESK hDesk;
};

DWORD WINAPI _abilitaCIE(
	LPVOID lpThreadParameter) {
	init_CSP_func
	const char *PAN = (char *)lpThreadParameter;

	std::string container ("CIE-");
	container += PAN;

	try {

		CSHA256 sha256;
		std::map<uint8_t, ByteDynArray> hashSet;
		uint8_t* data;
		DWORD len = 0;
		ByteDynArray CertCIE;
		ByteDynArray SOD;
		ByteDynArray IdServizi;
		std::unique_ptr<safeDesktop> desk;

		SCARDCONTEXT hSC;

		SCardEstablishContext(SCARD_SCOPE_SYSTEM, nullptr, nullptr, &hSC);
		char *readers = nullptr;
		len = SCARD_AUTOALLOCATE;
		if (SCardListReaders(hSC, nullptr, (char*)&readers, &len) != SCARD_S_SUCCESS || readers==nullptr) {
			CMessage msg(MB_OK,
				"Abilitazione CIE",
				"Nessun lettore di smartcard installato");
			msg.DoModal();
			return 0;
		}

		char *curreader = readers;
		bool foundCIE = false;
		for (; curreader[0] != 0; curreader += strnlen(curreader, len) + 1) {
			CARD_DATA cData;
			ZeroMem(cData);
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
					auto isCIE = CardAcquireContext(&cData, 0);
					SCardFreeMemory(cData.hScard, cData.pbAtr);
					if (isCIE != 0)
						continue;					
				}

				foundCIE = true;
				if (!desk)
					desk.reset(new safeDesktop("AbilitaCIE"));

				CMessage msg(MB_OKCANCEL, "Abilitazione CIE",
					"Premere OK per effettuare la verifica di autenticità",
					"e abilitare l'uso della CIE su questo PC");

				if (msg.DoModal() == IDOK) {
					
					CPin pin(8, "Inserire le 8 cifre del PIN della CIE", "Non verranno mai richieste in seguito tutte le 8 cifre del PIN,", "ma solo le ultime 4", "Abilitazione CIE");
					if (pin.DoModal() == IDOK) {
						try {

							safeTransaction Tran(conn, SCARD_LEAVE_CARD);
							if (!Tran.isLocked())
								continue;

							len = 0;
							CardReadFile(&cData, DirCIE, EfIdServizi, 0, &data, &len);
							IdServizi = ByteArray(data, len);
							cData.pfnCspFree(data);
							hashSet[0xa1] = sha256.Digest(IdServizi.left(12));

							len = 0;
							CardReadFile(&cData, DirCIE, EfSOD, 0, &data, &len);
							SOD = ByteArray(data, len);
							cData.pfnCspFree(data);

							ByteDynArray IntAuth; len = 0;
							CardReadFile(&cData, DirCIE, EfIntAuth, 0, &data, &len);
							IntAuth = ByteArray(data, len);
							cData.pfnCspFree(data);
							hashSet[0xa4] = sha256.Digest(IntAuth.left(GetASN1DataLenght(IntAuth)));

							ByteDynArray IntAuthServizi; len = 0;
							CardReadFile(&cData, DirCIE, EfIntAuthServizi, 0, &data, &len);
							IntAuthServizi = ByteArray(data, len);
							cData.pfnCspFree(data);
							hashSet[0xa5] = sha256.Digest(IntAuthServizi.left(GetASN1DataLenght(IntAuthServizi)));

							ByteDynArray DH; len = 0;
							CardReadFile(&cData, DirCIE, EfDH, 0, &data, &len);
							DH = ByteArray(data, len);
							cData.pfnCspFree(data);

							hashSet[0x1b] = sha256.Digest(DH.left(GetASN1DataLenght(DH)));
							auto ias = ((IAS*)cData.pvVendorSpecific);
							if (IdServizi != ByteArray((uint8_t*)PAN, strnlen(PAN, 20)))
								continue;

							//DWORD id;
							HWND progWin = nullptr;
							struct threadData th;
							th.progWin = &progWin;
							th.hDesk = (HDESK)(*desk);
							std::thread([&th]() -> DWORD {
								SetThreadDesktop(th.hDesk);
								CVerifica ver(th.progWin);
								ver.DoModal();
								return 0;
							}).detach();

							ias->Callback = [](int prog, char *desc, void *data) {
								HWND progWin = *(HWND*)data;
								if (progWin != nullptr)
									SendMessage(progWin, WM_COMMAND, 100 + prog, (LPARAM)desc);
							};
							ias->CallbackData = &progWin;

							DWORD attempts = -1;

							DWORD rs = CardAuthenticateEx(&cData, ROLE_USER, FULL_PIN, (BYTE*)pin.PIN, (DWORD)strnlen(pin.PIN, sizeof(pin.PIN)), nullptr, 0, &attempts);
							if (rs == SCARD_W_WRONG_CHV) {
								if (progWin != nullptr)
									SendMessage(progWin, WM_COMMAND, 100 + 7, (LPARAM)"");
								std::string num;
								if (attempts > 0)
									num = "Sono rimasti " + std::to_string(attempts ) + " tentativi prima del blocco";
								else
									num = "";
								CMessage msg(MB_OK,
									"Abilitazione CIE",
									"PIN Errato",
									num.c_str());
								msg.DoModal();
								break;
							}
							else if (rs == SCARD_W_CHV_BLOCKED) {
								if (progWin != nullptr)
									SendMessage(progWin, WM_COMMAND, 100 + 7, (LPARAM)"");
								CMessage msg(MB_OK,
									"Abilitazione CIE",
									"Il PIN è bloccato. Può esere sbloccato verificando il PUK");
								msg.DoModal();
								break;
							}
							else if (rs != SCARD_S_SUCCESS)
								throw logged_error("Autenticazione fallita");

							if (progWin != nullptr)
								SendMessage(progWin, WM_COMMAND, 100 + 4, (LPARAM)"Lettura certificato");

							ByteDynArray Serial; len = 0;
							CardReadFile(&cData, DirCIE, EfSerial, 0, &data, &len);
							Serial = ByteArray(data, len);
							cData.pfnCspFree(data);
							hashSet[0xa2] = sha256.Digest(Serial.left(9));
							len = 0;
							CardReadFile(&cData, DirCIE, EfCertCIE, 0, &data, &len);
							CertCIE = ByteArray(data, len);
							cData.pfnCspFree(data);
							hashSet[0xa3] = sha256.Digest(CertCIE.left(GetASN1DataLenght(CertCIE)));

							if (progWin != nullptr)
								SendMessage(progWin, WM_COMMAND, 100 + 5, (LPARAM)"Verifica SOD");
							ias->VerificaSOD(SOD, hashSet);

							if (progWin != nullptr)
								SendMessage(progWin, WM_COMMAND, 100 + 6, (LPARAM)"Cifratura dati");
							ias->SetCache(PAN, CertCIE, ByteArray((uint8_t*)pin.PIN, 4));
							if (progWin != nullptr)
								SendMessage(progWin, WM_COMMAND, 100 + 7, (LPARAM)"");

							Tran.unlock();

							CMessage msg(MB_OK,
								"Abilitazione CIE",
								"La CIE è abilitata all'uso");
							msg.DoModal();
						}
						catch (std::exception &ex) {
							std::string dump;
							OutputDebugString(ex.what());
							CMessage msg(MB_OK,
								"Abilitazione CIE",
								"Si è verificato un errore nella verifica di",
								"autenticità del documento");

							msg.DoModal();
							break;
						}
					}
					break;
				}
			}
		}
		if (!foundCIE) {
			if (!desk)
				desk.reset(new safeDesktop("AbilitaCIE"));
			std::string num(PAN);
			num+=" nei lettori di smart card";
			CMessage msg(MB_OK,
				"Abilitazione CIE",
				"Impossibile trovare la CIE con Numero Identificativo",
				num.c_str());
			msg.DoModal();
		}
		SCardFreeMemory(hSC, readers);
	}
	catch (std::exception &ex) {
		OutputDebugString(ex.what());
		MessageBox(nullptr, "Si è verificato un errore nella verifica di autenticità del documento", "CIE", MB_OK);
	}

	return SCARD_S_SUCCESS;
	exit_CSP_func
	return E_UNEXPECTED;
}


extern "C" int CALLBACK AbilitaCIE(
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

	ODS("Start AbilitaCIE");
	if (!CheckOneInstance("CIEAbilitaOnce")) {
		ODS("Already running AbilitaCIE");
		return 0;
	}
	//std::thread thread(_abilitaCIE, lpCmdLine);
	//thread.join();
	_abilitaCIE(lpCmdLine);
	ODS("End AbilitaCIE");
	return 0;
}
