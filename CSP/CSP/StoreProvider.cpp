#include "../stdafx.h"
#include <Wincrypt.h>
#include <stdio.h>
#include "../util/moduleinfo.h"
#include "csp.h"
#include <vector>

#ifdef _WIN64
#pragma comment(linker, "/export:CertDllOpenStoreProv")
#pragma comment(linker, "/export:DllRegisterServer,PRIVATE")
#pragma comment(linker, "/export:DllUnregisterServer,PRIVATE")
#else
#pragma comment(linker, "/export:CertDllOpenStoreProv=_CertDllOpenStoreProv@28")
#pragma comment(linker, "/export:DllRegisterServer=_DllRegisterServer@0,PRIVATE")
#pragma comment(linker, "/export:DllUnregisterServer=_DllUnregisterServer@0,PRIVATE")
#endif

extern CModuleInfo moduleInfo;
extern "C" BOOL WINAPI CertDllOpenStoreProv(
	_In_          LPCSTR                lpszStoreProvider,
	_In_          DWORD                 dwEncodingType,
	_In_          HCRYPTPROV            hCryptProv,
	_In_          DWORD                 dwFlags,
	_In_    const void                  *pvPara,
	_In_          HCERTSTORE            hCertStore,
	_Inout_       PCERT_STORE_PROV_INFO pStoreProvInfo
) {
	init_CSP_func
	HCRYPTPROV prov = 0;
	CryptAcquireContext(&prov, nullptr, MS_SCARD_PROV, PROV_RSA_FULL, CRYPT_SILENT);
	if (prov == 0) {
		return TRUE;
	}

	BYTE containerName[200];
	DWORD containerNameSize = 200;
	CryptGetProvParam(prov, PP_CONTAINER, containerName, &containerNameSize, 0);
	std::string ciePrefix(CIE_CONTAINER_NAME);
	if (containerNameSize<= ciePrefix.length())
		return TRUE;
	if (ByteArray(containerName, ciePrefix.length())!=ByteArray((BYTE*)ciePrefix.c_str(), ciePrefix.length()))
		return TRUE;

	HCRYPTKEY key = 0;
	CryptGetUserKey(prov, AT_KEYEXCHANGE, &key);
	if (key != 0) {

		BYTE cert[5000];
		DWORD certSize = 5000;
		auto ris = CryptGetKeyParam(key, KP_CERTIFICATE, cert, &certSize, 0);
		CryptDestroyKey(key);
		if (ris != 0) {
			WCHAR containerW[100];
			swprintf_s(containerW, L"%S", containerName);
			PCCERT_CONTEXT cer = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, cert, certSize);
			if (cer != nullptr) {

				auto _1 = scopeExit([&]() noexcept {
					CertFreeCertificateContext(cer);
				});

				PCCERT_CONTEXT storeCert = nullptr;
				CertAddCertificateContextToStore(hCertStore, cer, CERT_STORE_ADD_REPLACE_EXISTING, &storeCert);
				if (storeCert != nullptr) {

					auto _2 = scopeExit([&]() noexcept {
						CertFreeCertificateContext(storeCert);
					});


					CRYPT_KEY_PROV_INFO KeyProvInfo;
					ZeroMem(KeyProvInfo);
					KeyProvInfo.pwszProvName = MS_SCARD_PROV_W;
					KeyProvInfo.pwszContainerName = containerW;
					KeyProvInfo.dwKeySpec = AT_KEYEXCHANGE;
					KeyProvInfo.dwProvType = PROV_RSA_FULL;

					CertSetCertificateContextProperty(storeCert, CERT_KEY_PROV_INFO_PROP_ID, 0, &KeyProvInfo);
				}
			}
		}
	}
	CryptReleaseContext(prov, 0);

	return TRUE;
	exit_CSP_func
	return FALSE;
}

extern "C" HRESULT __stdcall DllUnregisterServer(void) {
	init_CSP_func
	SCARDCONTEXT hSC;
	SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hSC);
	SCardForgetCardType(hSC, "CIE_1");
	SCardForgetCardType(hSC, "CIE_2");
	SCardForgetCardType(hSC, "CIE_3");
	SCardForgetCardType(hSC, "CIE_4");
	SCardForgetCardType(hSC, "CIE_5");
	SCardForgetCardType(hSC, "CIE_6");
	SCardReleaseContext(hSC);

	CertUnregisterPhysicalStore(L"MY", CERT_SYSTEM_STORE_CURRENT_USER, L"CIEStore");
	CryptUnregisterOIDFunction(0, CRYPT_OID_OPEN_STORE_PROV_FUNC, "CIECertProvider");
	return S_OK;
	exit_CSP_func
	return E_UNEXPECTED;
}

LONG RegisterCard(SCARDCONTEXT hSC, const char *name, BYTE* ATR, int ATRLen, BYTE *Mask) {
	init_func
	ByteDynArray ATRMask;
	LONG ris;
	SCardForgetCardType(hSC, name);	
	if (Mask == nullptr) {
		ATRMask.resize(ATRLen);
		ATRMask.fill(0xff);
	}
	else
		ATRMask = ByteArray(Mask, ATRLen);
	if ((ris = SCardIntroduceCardType(hSC, name, nullptr, nullptr, 0, ATR, ATRMask.data(), (DWORD)ATRMask.size()) != SCARD_S_SUCCESS))
	{
		return E_UNEXPECTED;
	}
	if ((ris = SCardSetCardTypeProviderName(hSC, name, SCARD_PROVIDER_CSP, MS_SCARD_PROV) != SCARD_S_SUCCESS))
	{
		return E_UNEXPECTED;
	}
	if ((ris = SCardSetCardTypeProviderName(hSC, name, 0x80000001, moduleInfo.szModuleFullPath.c_str()) != SCARD_S_SUCCESS))
	{
		return E_UNEXPECTED;
	}
	if ((ris = SCardSetCardTypeProviderNameA(hSC, name, SCARD_PROVIDER_KSP, "Microsoft Smart Card Key Storage Provider") != SCARD_S_SUCCESS))
	{
		return E_UNEXPECTED;
	}
	std::string kName;
	kName = "SOFTWARE\\Microsoft\\Cryptography\\Calais\\SmartCards\\";
	kName += name;
	HKEY scardKey=NULL;
	RegOpenKeyA(HKEY_LOCAL_MACHINE, kName.c_str(), &scardKey);
	char pinLabel[] = "it-IT,Immettere le ultime 4 cifre del PIN";
	RegSetKeyValueA(scardKey, NULL, "80000100", REG_SZ, pinLabel, sizeof(pinLabel));
	return S_OK;
	exit_func
}

extern "C" HRESULT __stdcall DllRegisterServer(void) {
	init_CSP_func
	SCARDCONTEXT hSC;
	SCardEstablishContext(SCARD_SCOPE_SYSTEM,NULL,NULL,&hSC);
	BYTE ATR[] = { 0x3B, 0x8F, 0x80, 0x01, 0x80, 0x31, 0x80, 0x65, 0xB0, 0x85, 0x03, 0x00, 0xEF, 0x12, 0x0F, 0xFF, 0x82, 0x90, 0x00, 0x73 };
	BYTE ATR2[] = { 0x3B, 0x8F, 0x80, 0x01, 0x80, 0x31, 0x80, 0x65, 0xB0, 0x85, 0x04, 0x00, 0x11, 0x12, 0x0F, 0xFF, 0x82, 0x90, 0x00, 0x8A };
	BYTE ATR3[] =      { 0x3B, 0x8E, 0x80, 0x01, 0x80, 0x31, 0x80, 0x65, 0x49, 0x54, 0x4E, 0x58, 0x50, 0x12, 0x0F, 0x00, 0x00, 0x00, 0x00 };
	BYTE ATR3_Mask[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00 };
	BYTE ATR4[] = {0x3B, 0x8B, 0x80, 0x01, 0x80, 0x66, 0x47, 0x50, 0x00, 0xB8, 0x00, 0x7F, 0x82, 0x90, 0x00, 0x2E};
	BYTE ATR5[] = { 0x3B, 0x80, 0x80, 0x01, 0x01 };
	BYTE ATR6[] = { 0x3B, 0x8B, 0x80, 0x01, 0x80, 0x66, 0x47, 0x50, 0x00, 0xB8, 0x00, 0x94, 0x82, 0x90, 0x00, 0xC5 };
	LONG ris;
	if (ris = RegisterCard(hSC, "CIE_1", ATR, sizeof(ATR), nullptr) != SCARD_S_SUCCESS)
		return E_UNEXPECTED;

	if (ris = RegisterCard(hSC, "CIE_2", ATR2, sizeof(ATR2), nullptr) != SCARD_S_SUCCESS)
		return E_UNEXPECTED;
	if (ris = RegisterCard(hSC, "CIE_3", ATR3, sizeof(ATR3), ATR3_Mask) != SCARD_S_SUCCESS)
		return E_UNEXPECTED;
	if (ris = RegisterCard(hSC, "CIE_4", ATR4, sizeof(ATR4), nullptr) != SCARD_S_SUCCESS)
		return E_UNEXPECTED;
	if (ris = RegisterCard(hSC, "CIE_5", ATR5, sizeof(ATR5), nullptr) != SCARD_S_SUCCESS)
		return E_UNEXPECTED;
	if (ris = RegisterCard(hSC, "CIE_6", ATR6, sizeof(ATR6), nullptr) != SCARD_S_SUCCESS)
		return E_UNEXPECTED;

	SCardReleaseContext(hSC);

	{
		CryptUnregisterOIDFunction(0, CRYPT_OID_OPEN_STORE_PROV_FUNC, "CIECertProvider");

		auto modName = std::vector<WCHAR>(moduleInfo.szModuleFullPath.size() + 10);
		swprintf_s(modName.data(), modName.size(), L"%S", moduleInfo.szModuleFullPath.c_str());
		if (!CryptRegisterOIDFunction(0, CRYPT_OID_OPEN_STORE_PROV_FUNC, "CIECertProvider", modName.data(), CRYPT_OID_OPEN_STORE_PROV_FUNC))
			return E_UNEXPECTED;
	}

	CertUnregisterPhysicalStore(L"MY", CERT_STORE_DELETE_FLAG, L"CIEStore");

	CERT_PHYSICAL_STORE_INFO PhysicalStoreInfo;
	PhysicalStoreInfo.cbSize = sizeof(CERT_PHYSICAL_STORE_INFO);
	PhysicalStoreInfo.pszOpenStoreProvider = "CIECertProvider";
	PhysicalStoreInfo.dwFlags = 0;
	PhysicalStoreInfo.dwOpenFlags = 0;
	PhysicalStoreInfo.OpenParameters.pbData = NULL;
	PhysicalStoreInfo.OpenParameters.cbData = 0;
	PhysicalStoreInfo.dwPriority = 1;
	PhysicalStoreInfo.dwOpenEncodingType = 0;

	// Register the physical store. 
	if (!CertRegisterPhysicalStore(
		L"MY",
		CERT_SYSTEM_STORE_CURRENT_USER,
		L"CIEStore",
		&PhysicalStoreInfo,
		NULL
		))
	{
		return E_UNEXPECTED;
	}
	return S_OK;
	exit_CSP_func
	return E_UNEXPECTED;
}
