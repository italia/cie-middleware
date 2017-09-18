#include "../stdafx.h"
#include <Wincrypt.h>
#include <stdio.h>
#include "../util/moduleinfo.h"

//#define TEST_CERT "C:\\Progetti\\CIE\\Middleware\\test.crt"

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

	HCRYPTPROV prov=0;
	CryptAcquireContext(&prov, nullptr, MS_SCARD_PROV, PROV_RSA_FULL, CRYPT_SILENT);
	if (prov == 0) {
		CryptAcquireContext(&prov, nullptr, MS_SCARD_PROV, PROV_RSA_FULL, CRYPT_SILENT);
		if (prov == 0) {
			return TRUE;
		}
	}

	BYTE containerName[100];
	DWORD containerNameSize = 100;
	CryptGetProvParam(prov, PP_CONTAINER, containerName, &containerNameSize, 0);
	DWORD keySpecs[] = { AT_SIGNATURE, AT_KEYEXCHANGE };
	for (int i = 0; i < 2; i++) {
		HCRYPTKEY key=0;
		CryptGetUserKey(prov, keySpecs[i], &key);
		if (key == 0)
			continue;
		BYTE cert[5000];
		DWORD certSize = 5000;
		auto ris = CryptGetKeyParam(key, KP_CERTIFICATE, cert, &certSize, 0);
		CryptDestroyKey(key);
		if (ris != 0) {
			WCHAR containerW[100];
			swprintf_s(containerW, L"%S", containerName);
			PCCERT_CONTEXT cer = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, cert, certSize);
			CRYPT_KEY_PROV_INFO KeyProvInfo;
			ZeroMem(KeyProvInfo);
			KeyProvInfo.pwszProvName = MS_SCARD_PROV_W;
			KeyProvInfo.pwszContainerName = containerW;
			KeyProvInfo.dwKeySpec = keySpecs[i];
			KeyProvInfo.dwProvType = PROV_RSA_FULL;

			CertSetCertificateContextProperty(cer, CERT_KEY_PROV_INFO_PROP_ID, 0, &KeyProvInfo);
			PCCERT_CONTEXT storeCert;
			CertAddCertificateContextToStore(hCertStore, cer, CERT_STORE_ADD_REPLACE_EXISTING, &storeCert);
		}
	}
	CryptReleaseContext(prov, 0);

	return TRUE;
}

extern "C" HRESULT __stdcall DllUnregisterServer(void) {
	SCARDCONTEXT hSC;
	SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hSC);
	SCardForgetCardType(hSC, "CIE_1");
	SCardForgetCardType(hSC, "CIE_2");
	SCardForgetCardType(hSC, "CIE_3");
	SCardReleaseContext(hSC);

	CertUnregisterPhysicalStore(L"MY", CERT_SYSTEM_STORE_CURRENT_USER, L"CIEStore");
	CryptUnregisterOIDFunction(0, CRYPT_OID_OPEN_STORE_PROV_FUNC, "CIECertProvider");
	return S_OK;
}

LONG RegisterCard(SCARDCONTEXT hSC, char *name, BYTE* ATR, int ATRLen) {
	MessageBox(NULL, "a", "a", MB_OK);
	ByteDynArray ATRMask;
	LONG ris;
	SCardForgetCardType(hSC, name);
	ATRMask.resize(ATRLen);
	ATRMask.fill(0xff);
	if ((ris = SCardIntroduceCardType(hSC, name, nullptr, nullptr, 0, ATR, ATRMask.lock(), ATRMask.size()) != SCARD_S_SUCCESS))
	{
		return E_UNEXPECTED;
	}
	if ((ris = SCardSetCardTypeProviderName(hSC, name, SCARD_PROVIDER_CSP, MS_SCARD_PROV) != SCARD_S_SUCCESS))
	{
		return E_UNEXPECTED;
	}
	if ((ris = SCardSetCardTypeProviderName(hSC, name, 0x80000001, moduleInfo.szModuleFullPath.lock()) != SCARD_S_SUCCESS))
	{
		return E_UNEXPECTED;
	}
	if ((ris = SCardSetCardTypeProviderNameA(hSC, name, SCARD_PROVIDER_KSP, "Microsoft Smart Card Key Storage Provider") != SCARD_S_SUCCESS))
	{
		return E_UNEXPECTED;
	}
	String kName;
	kName.printf("%s%s", "SOFTWARE\\Microsoft\\Cryptography\\Calais\\SmartCards\\", name);
	HKEY scardKey=NULL;
	RegOpenKeyA(HKEY_LOCAL_MACHINE, kName.lock(), &scardKey);
	char pinLabel[] = "it-IT,Immettere le ultime 4 cifre del PIN";
	RegSetKeyValueA(scardKey, NULL, "80000100", REG_SZ, pinLabel, sizeof(pinLabel));
	return S_OK;
}

extern "C" HRESULT __stdcall DllRegisterServer(void) {
	
	SCARDCONTEXT hSC;
	SCardEstablishContext(SCARD_SCOPE_SYSTEM,NULL,NULL,&hSC);
	BYTE ATR[] = { 0x3B, 0x8F, 0x80, 0x01, 0x80, 0x31, 0x80, 0x65, 0xB0, 0x85, 0x03, 0x00, 0xEF, 0x12, 0x0F, 0xFF, 0x82, 0x90, 0x00, 0x73 };
	BYTE ATR2[] = { 0x3B, 0x8F, 0x80, 0x01, 0x80, 0x31, 0x80, 0x65, 0xB0, 0x85, 0x04, 0x00, 0x11, 0x12, 0x0F, 0xFF, 0x82, 0x90, 0x00, 0x73 };
	BYTE ATR3[] = { 0x3B, 0x88, 0x80, 0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01 };
	LONG ris;
	if (ris = RegisterCard(hSC, "CIE_1", ATR, sizeof(ATR)) != SCARD_S_SUCCESS)
		return E_UNEXPECTED;
	if (ris = RegisterCard(hSC, "CIE_2", ATR2, sizeof(ATR2)) != SCARD_S_SUCCESS)
		return E_UNEXPECTED;
	if (ris = RegisterCard(hSC, "CIE_3", ATR3, sizeof(ATR3)) != SCARD_S_SUCCESS)
		return E_UNEXPECTED;

	SCardReleaseContext(hSC);

	WCHAR *modName = new WCHAR[moduleInfo.szModuleName.size() + 10];
	swprintf_s(modName, moduleInfo.szModuleName.size() + 10, L"%S.dll", moduleInfo.szModuleName.lock());
	if (!CryptRegisterOIDFunction(0, CRYPT_OID_OPEN_STORE_PROV_FUNC, "CIECertProvider", modName, CRYPT_OID_OPEN_STORE_PROV_FUNC))
		return E_UNEXPECTED;
	delete[] modName;

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
}
