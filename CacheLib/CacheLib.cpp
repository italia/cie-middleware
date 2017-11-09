#include "stdafx.h"
#include "CacheLib.h"
#include <Shlwapi.h>
#include <Shlobj.h>
#include <stdio.h>
#include <vector>

#include "..\CSP\Util\util.h"


/// Questa implementazione della cache del PIN e del certificato è fornita solo a scopo dimostrativo. Questa versione
/// NON protegge a sufficienza il PIN dell'utente, che potrebbe essere ricavato da un'applicazione malevola. Si raccomanda di
/// utilizzare, in contesti di produzione, un'implementazione che fornisca un elevato livello di sicurezza


char commonData[MAX_PATH] = "";

void GetCardPath(const char *PAN, char szPath[MAX_PATH]) {
	if (commonData[0]==0)
		if (SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath) != S_OK)
			throw CStringException("Errore in GetFolderPath per COMMONDATA");

	PathAppend(szPath, "CIEPKI");
	std::string s(PAN);
	s+= ".cache";
	PathAppend(szPath, s.c_str());
}

bool CacheExists(const char *PAN) {
	char szPath[MAX_PATH];
	GetCardPath(PAN, szPath);
	return (PathFileExists(szPath)!=FALSE);
}

void CacheGetCertificate(const char *PAN, std::vector<BYTE>&certificate)
{
	if (PAN == nullptr)
		throw CStringException("Il PAN è necessario");

	char szPath[MAX_PATH];
	GetCardPath(PAN, szPath);

	if (PathFileExists(szPath)) {

		ByteDynArray data,Cert;
		data.load(szPath);
		BYTE *ptr = data.lock();
		int len = *(int*)ptr; ptr += sizeof(int);
		// salto il PIN
		ptr += len;
		len = *(int*)ptr; ptr += sizeof(int);
		Cert.resize(len); Cert.copy(ptr, len);

		certificate.resize(Cert.size());
		ByteArray(certificate.data(), certificate.size()).copy(Cert);
	}
	else
		throw CStringException("CIE non abilitata");
}

void CacheGetPIN(const char *PAN, std::vector<BYTE>&PIN) {
	if (PAN == nullptr)
		throw CStringException("Il PAN è necessario");

	char szPath[MAX_PATH];
	GetCardPath(PAN, szPath);

	if (PathFileExists(szPath)) {
		ByteDynArray data, ClearPIN;
		data.load(szPath);
		BYTE *ptr = data.lock();
		int len = *(int*)ptr; ptr += sizeof(int);
		ClearPIN.resize(len); ClearPIN.copy(ptr, len);

		PIN.resize(ClearPIN.size());
		ByteArray(PIN.data(), PIN.size()).copy(ClearPIN);

	}
	else
		throw CStringException("CIE non abilitata");
		
}



void CacheSetData(const char *PAN, BYTE *certificate, int certificateSize, BYTE *FirstPIN, int FirstPINSize) {
	if (PAN == nullptr)
		throw CStringException("Il PAN è necessario");

	char szPath[MAX_PATH];
	if (SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath) != S_OK)
		throw CStringException("Errore in GetFolderPath per COMMONDATA");
	PathAppend(szPath, "\\CIEPKI");
	if (!PathFileExists(szPath))
		CreateDirectory(szPath, nullptr);
	PathAppend(szPath, std::string("\\").append(PAN).append(".cache").c_str());

	ByteArray baCertificate(certificate, certificateSize);
	ByteArray baFirstPIN(FirstPIN, FirstPINSize);

	FILE *f = nullptr;
	fopen_s(&f, szPath, "wb");
	if (f == nullptr)
		throw CStringException("Errore in scrittura file cache del certificato");

	int len = baFirstPIN.size();
	fwrite(&len, sizeof(len), 1, f);
	fwrite(baFirstPIN.lock(), len, 1, f);

	len = baCertificate.size();
	fwrite(&len, sizeof(len), 1, f);
	fwrite(baCertificate.lock(), len, 1, f);

	fclose(f);
}
