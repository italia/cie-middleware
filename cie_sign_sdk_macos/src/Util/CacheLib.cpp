
#include "CacheLib.h"
//#include <Shlwapi.h>
//#include <Shlobj.h>
#include <stdio.h>
#include <vector>
#include <fstream>
//#include "sddl.h"
//#include "Aclapi.h"
//#include <VersionHelpers.h>
#include "cryptopp/misc.h"
#include "util.h"

//#ifndef WIN32

#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <regex>

#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include "keys.h"
#include "cryptopp/sha.h"

#include <pwd.h>

using namespace CryptoPP;

int decrypt(std::string& ciphertext, std::string& message);
//#endif

/// Questa implementazione della cache del PIN e del certificato è fornita solo a scopo dimostrativo. Questa versione
/// NON protegge a sufficienza il PIN dell'utente, che potrebbe essere ricavato da un'applicazione malevola. Si raccomanda di
/// utilizzare, in contesti di produzione, un'implementazione che fornisca un elevato livello di sicurezza

#ifdef WIN32

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
	auto Path=GetCardDir();

	if (Path[Path.length()] != '\\')
		Path += '\\';

	Path += std::string(PAN);
	Path += ".cache";
    
    strlcpy(szPath, Path.c_str(), Path.size());
}

bool CacheExists(const char *PAN) {
	char szPath[MAX_PATH];
	GetCardPath(PAN, szPath);
	return (PathFileExists(szPath)!=FALSE);
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

	auto szDir=GetCardDir();
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

#else

bool file_exists (const char* name) {
    struct stat buffer;
    return (stat (name, &buffer) == 0);
}

std::string GetCardDir()
{
    char* home = getenv("HOME");
    if(home == NULL)
	{
		struct passwd *pw = getpwuid(getuid());

		home = pw->pw_dir;
		printf("home: %s", home);
	}

    std::string path(home);
    
    std::smatch match;
    std::regex_search(path, match, std::regex("^/home/"));
    std::string suffix = match.suffix();
    if(suffix.find("/") != std::string::npos)
        throw 1;
    
    path.append("/.CIEPKI/");

    printf("Card Dir: %s\n", path.c_str());
    
    return path.c_str();
}

void GetCardPath(const char *PAN, std::string& sPath) {
    auto Path=GetCardDir();
    
    Path += std::string(PAN);
    Path += ".cache";
    sPath = Path;
}

bool CacheExists(const char *PAN) {
    std::string sPath;
    GetCardPath(PAN, sPath);
    return file_exists(sPath.c_str());
}

bool CacheRemove(const char *PAN) {
    std::string sPath;
    GetCardPath(PAN, sPath);
    return remove(sPath.c_str());
}

void CacheGetCertificate(const char *PAN, std::vector<uint8_t>&certificate)
{
    if (PAN == nullptr)
        throw logged_error("Il PAN è necessario");
    
    std::string sPath;
    GetCardPath(PAN, sPath);
    
    if (file_exists(sPath.c_str())) {
        
        ByteDynArray data, Cert;
        data.load(sPath.c_str());
        
        std::string ciphertext((char*)data.data(), data.size());
        std::string plaintext;
        
        decrypt(ciphertext, plaintext);
        
        uint8_t *ptr = (uint8_t *)plaintext.c_str();
        
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
    
    std::string sPath;
    GetCardPath(PAN, sPath);
    
    if (file_exists(sPath.c_str())) {
        ByteDynArray data, ClearPIN;
        data.load(sPath.c_str());
        
        std::string ciphertext((char*)data.data(), data.size());
        std::string plaintext;
        
        decrypt(ciphertext, plaintext);
        
        uint8_t *ptr = (uint8_t *)plaintext.c_str();
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
    
    struct stat st = {0};
        
    if (stat(szDir.c_str(), &st) == -1) {
        int r = mkdir(szDir.c_str(), 0700);
        printf("mkdir: %d, %x\n", r, errno);
    }
    
    std::string sPath;
    GetCardPath(PAN, sPath);
    
    ByteArray baCertificate(certificate, certificateSize);
    ByteArray baFirstPIN(FirstPIN, FirstPINSize);
    
    uint32_t pinlen = (uint32_t)baFirstPIN.size();
    uint32_t certlen = (uint32_t)baCertificate.size();
    
    byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];
    memset( key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );
    
    std::string ciphertext;
    std::string enckey = ENCRYPTION_KEY;
    
    byte digest[SHA1::DIGESTSIZE];
    SHA1().CalculateDigest(digest, (byte*)enckey.c_str(), enckey.length());
    memcpy(key, digest, CryptoPP::AES::DEFAULT_KEYLENGTH );
    //
    // Create Cipher Text
    //
    CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );
    
    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>(&pinlen), sizeof(pinlen));
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>( baFirstPIN.data() ), pinlen );
    
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>(&certlen), sizeof(certlen));
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>( baCertificate.data() ), certlen );
    
    stfEncryptor.MessageEnd();
    
            
    std::ofstream file(sPath.c_str(), std::ofstream::out | std::ofstream::binary);
    file.write(ciphertext.c_str(), ciphertext.length());
    file.close();
}

#endif
