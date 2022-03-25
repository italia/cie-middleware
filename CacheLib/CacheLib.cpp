#include "stdafx.h"
#include "CacheLib.h"
#include <Shlwapi.h>
#include <Shlobj.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include "sddl.h"
#include "Aclapi.h"
#include <VersionHelpers.h>

#include "..\CSP\Util\util.h"


/// Questa implementazione della cache del PIN e del certificato è fornita solo a scopo dimostrativo. Questa versione
/// NON protegge a sufficienza il PIN dell'utente, che potrebbe essere ricavato da un'applicazione malevola. Si raccomanda di
/// utilizzare, in contesti di produzione, un'implementazione che fornisca un elevato livello di sicurezza

std::string commonData;

std::string GetCardDir() {

	if (commonData[0] == 0) {
		char szPath[MAX_PATH];
		ExpandEnvironmentStrings("%PROGRAMDATA%\\CIEPKI", szPath, MAX_PATH);
		commonData = szPath;
	}
	return commonData;
}

void GetCardPath(const char *PAN, char szPath[MAX_PATH]) {
	auto Path = GetCardDir();

	if (Path[Path.length()] != '\\')
		Path += '\\';

	Path += std::string(PAN);
	Path += ".cache";
	strcpy_s(szPath, MAX_PATH, Path.c_str());
}

bool CacheExists(const char *PAN) {
	char szPath[MAX_PATH];
	GetCardPath(PAN, szPath);
	return PathFileExists(szPath);
}

bool CacheRemove(const char *PAN)
{
	char szPath[MAX_PATH];
	GetCardPath(PAN, szPath);
	if (PathFileExists(szPath))
	{
		return DeleteFile(szPath);
	}

	return false;
}


void CacheGetCertificate(const char *PAN, std::vector<uint8_t>&certificate)
{
	if (PAN == nullptr)
		throw logged_error("Il PAN è necessario");

	char szPath[MAX_PATH];
	GetCardPath(PAN, szPath);

	if (PathFileExists(szPath)) {

		ByteDynArray data, Cert;
		data.load(szPath);
		uint8_t *ptr = data.data();
		uint32_t len = *(uint32_t*)ptr; ptr += sizeof(uint32_t);
		// salto il PIN
		ptr += len;
		len = *(uint32_t*)ptr; ptr += sizeof(uint32_t);
		Cert.resize(len); Cert.copy(ByteArray(ptr, len));

		certificate.resize(Cert.size());
		ByteArray(certificate.data(), certificate.size()).copy(Cert);
	}
	else
	{
		throw logged_error("CIE non abilitata");
	}
}

void CacheGetPIN(const char *PAN, std::vector<uint8_t>&PIN) {
	if (PAN == nullptr)
		throw logged_error("Il PAN è necessario");

	char szPath[MAX_PATH];
	GetCardPath(PAN, szPath);

	if (PathFileExists(szPath)) {
		ByteDynArray data, ClearPIN;
		data.load(szPath);
		uint8_t *ptr = data.data();
		uint32_t len = *(uint32_t*)ptr; ptr += sizeof(uint32_t);
		ClearPIN.resize(len); ClearPIN.copy(ByteArray(ptr, len));

		PIN.resize(ClearPIN.size());
		ByteArray(PIN.data(), PIN.size()).copy(ClearPIN);

	}
	else
		throw logged_error("CIE non abilitata");

}



void CacheSetData(const char *PAN, uint8_t *certificate, int certificateSize, uint8_t *FirstPIN, int FirstPINSize) {
	if (PAN == nullptr)
		throw logged_error("Il PAN è necessario");

	auto szDir = GetCardDir();
	char chDir[MAX_PATH];
	strcpy_s(chDir, szDir.c_str());

	if (!PathFileExists(chDir)) {

		//creo la directory dando l'accesso a Edge (utente Packege).
		//Edge gira in low integrity quindi non potrà scrivere (enrollare) ma solo leggere il certificato
		bool done = false;
		CreateDirectory(chDir, nullptr);

		if (IsWindows8OrGreater()) {
			PACL pOldDACL = nullptr, pNewDACL = nullptr;
			PSECURITY_DESCRIPTOR pSD = nullptr;
			EXPLICIT_ACCESS ea;
			SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;

			DWORD dwRes = GetNamedSecurityInfo(chDir, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, &pSD);
			if (dwRes != ERROR_SUCCESS)
				throw logged_error("Impossibile attivare la CIE nel processo corrente");

			PSID TheSID = nullptr;
			DWORD SidSize = SECURITY_MAX_SID_SIZE;
			if (!(TheSID = LocalAlloc(LMEM_FIXED, SidSize))) {
				if (pSD != NULL)
					LocalFree((HLOCAL)pSD);
				throw logged_error("Impossibile attivare la CIE nel processo corrente");
			}

			if (!CreateWellKnownSid(WinBuiltinAnyPackageSid, NULL, TheSID, &SidSize)) {
				if (TheSID != NULL)
					LocalFree((HLOCAL)TheSID);
				if (pSD != NULL)
					LocalFree((HLOCAL)pSD);
				throw logged_error("Impossibile attivare la CIE nel processo corrente");
			}

			ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
			ea.grfAccessPermissions = GENERIC_READ;
			ea.grfAccessMode = SET_ACCESS;
			ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
			ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
			ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
			ea.Trustee.ptstrName = (LPSTR)TheSID;

			if (SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL) != ERROR_SUCCESS)
			{
				if (TheSID != NULL)
					LocalFree((HLOCAL)TheSID);
				if (pSD != NULL)
					LocalFree((HLOCAL)pSD);
				if (pNewDACL != NULL)
					LocalFree((HLOCAL)pNewDACL);
				throw logged_error("Impossibile attivare la CIE nel processo corrente");
			}

			if (SetNamedSecurityInfo(chDir, SE_FILE_OBJECT, si, NULL, NULL, pNewDACL, NULL) != ERROR_SUCCESS)
			{
				if (pNewDACL != NULL)
					LocalFree((HLOCAL)pNewDACL);
				if (TheSID != NULL)
					LocalFree((HLOCAL)TheSID);
				if (pSD != NULL)
					LocalFree((HLOCAL)pSD);
				throw logged_error("Impossibile attivare la CIE nel processo corrente");
			}

		}
	}
	char szPath[MAX_PATH];
	GetCardPath(PAN, szPath);

	ByteArray baCertificate(certificate, certificateSize);
	ByteArray baFirstPIN(FirstPIN, FirstPINSize);

	std::ofstream file(szPath, std::ofstream::out | std::ofstream::binary);

	uint32_t len = (uint32_t)baFirstPIN.size();
	file.write((char*)&len, sizeof(len));
	file.write((char*)baFirstPIN.data(), len);

	len = (uint32_t)baCertificate.size();
	file.write((char*)&len, sizeof(len));
	file.write((char*)baCertificate.data(), len);
}
