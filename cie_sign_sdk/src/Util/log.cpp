#include "stdafx.h"
#include "util.h"
#include "moduleinfo.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include "log.h"
#include "DbgHelp.h"
#include "UtilException.h"
//#include "thread.h"
#include "IniSettings.h"
#include <thread>
#include "Aclapi.h"
#include <VersionHelpers.h>
#include <Shlwapi.h>


static char *szCompiledFile=__FILE__;

std::string logDirGlobal;
unsigned int GlobalModuleNum=1;
unsigned int GlobalCount;
unsigned int GlobalDepth = 0;
bool FunctionLog = true;
char szLogDir[MAX_PATH];

CLog Log;

void initLog() 
{
	OutputDebugString("InitLog\n");
	ExpandEnvironmentStrings("%PROGRAMDATA%\\CIEPKI\\", szLogDir, MAX_PATH);

	if (!PathFileExists(szLogDir)) {

		//creo la directory dando l'accesso a Edge (utente Packege).
		//Edge gira in low integrity quindi non potr� scrivere (enrollare) ma solo leggere il certificato
		bool done = false;
		CreateDirectory(szLogDir, nullptr);

		if (IsWindows8OrGreater()) {
			PACL pOldDACL = nullptr, pNewDACL = nullptr;
			PSECURITY_DESCRIPTOR pSD = nullptr;
			EXPLICIT_ACCESS ea;
			SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;

			DWORD dwRes = GetNamedSecurityInfo(szLogDir, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, &pSD);
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
			ea.grfAccessPermissions = GENERIC_READ | GENERIC_WRITE;
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

			if (SetNamedSecurityInfo(szLogDir, SE_FILE_OBJECT, si, NULL, NULL, pNewDACL, NULL) != ERROR_SUCCESS)
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
//	GetTempPath(MAX_PATH, szLogDir);

	Log.initParam(true);
}

CLog::CLog() {
	
	Enabled = true;
	LogParam = true;
}

CLog::~CLog() {
	Enabled=false;
}

void CLog::initParam(bool enabled) {
	
	Enabled=enabled;

	logFileName = "CIEPKI";

	SYSTEMTIME  stTime;
	GetLocalTime(&stTime);

	std::stringstream th;
	th << std::setw(8) << std::setfill('0');

	th << szLogDir << logFileName << "_" << std::setw(4) << stTime.wYear << "-" << std::setw(2) << stTime.wMonth << "-" << std::setw(2) << stTime.wDay << ".log";
	logPath = th.str();

	std::stringstream line;

	line << std::setw(4) << stTime.wYear << "-" << std::setw(2) << stTime.wMonth << "-" << std::setw(2) << stTime.wDay;

	writePure("--------------------");
	writePure(line.str().c_str());
}

DWORD CLog::write(const char *format,...) {
	
	if (!Enabled)
		return 0;

 	va_list params;
	va_start (params, format);
	char pbtDate[0x1000];

	SYSTEMTIME  stTime;
	GetLocalTime(&stTime);
	sprintf_s(pbtDate,sizeof(pbtDate),"[%02d:%02d:%02d.%03d]", stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);	
	 
	std::hash<std::thread::id> hasher;
	auto dwThreadID = hasher(std::this_thread::get_id());
	FILE *lf=nullptr;
	errno_t err = fopen_s(&lf,logPath.c_str(), "a+t");
	if (err == 0)
	{
		//printf("file: %p\n", lf);

		fprintf(lf, "%s|%04i|%04i|", pbtDate, GetCurrentProcessId(), dwThreadID);

		//printf("file2: %p\n", lf);
		vfprintf(lf, format, params);
		fprintf(lf, "\n");
		fclose(lf);
	}
	else
	{
		//printf("error %x\n", err);
	}

#ifdef _DEBUG
#ifdef WIN32
	vsprintf_s(pbtDate, format, params);
	int dtLen = (int)strnlen(pbtDate, sizeof(pbtDate));
	sprintf_s(pbtDate + dtLen, 2048 - dtLen, "|thread:%08x|%s|", GetCurrentThreadId(), logName.c_str());
	dtLen = (int)strnlen(pbtDate, sizeof(pbtDate));
	sprintf_s(pbtDate+ dtLen, 2048 - dtLen , "\n");
	OutputDebugString(pbtDate);
#else
	puts(pbtDate);
#endif
#endif
 	va_end(params);
	
	return(0);
}

void CLog::writePure(const char *format,...) {
	
	if (!Enabled)
		return;

	va_list params;
	va_start (params, format);
	char pbtDate[0x800]={NULL};
	
	std::hash<std::thread::id> hasher;
	auto dwThreadID = hasher(std::this_thread::get_id());
	
	FILE *lf = nullptr;
	fopen_s(&lf,logPath.c_str(), "a+t");
	if (lf) {
		vfprintf(lf, format, params);
		fprintf(lf, "\n");
		fclose(lf);
	}
	
#ifdef _DEBUG
#ifdef WIN32
	int dtLen = (int)strnlen(pbtDate, sizeof(pbtDate));
	vsprintf_s(pbtDate+dtLen,2048-dtLen, format, params);
	dtLen = (int)strnlen(pbtDate, sizeof(pbtDate));
	sprintf_s(pbtDate + dtLen, 2048 - dtLen , "\n");
	OutputDebugString(pbtDate);
#else
	puts(pbtDate);
#endif
#endif
 	va_end(params);
}

void CLog::writeBinData(BYTE *data, size_t datalen) {
	
	if (!Enabled)
		return;

	char pbtDate[0x800]={NULL};

	// se siamo in LM_thread devo scrivere il thread nel nome del file
	std::hash<std::thread::id> hasher;
	auto dwThreadID = hasher(std::this_thread::get_id());
	
	FILE *lf = nullptr;
	fopen_s(&lf,logPath.c_str(), "a+t");
	if (lf) {
		if (datalen>100) datalen=100;
		for (size_t i=0;i<datalen;i++)
			fprintf(lf, "%02x ", data[i]);
		fprintf(lf, "\n");
		fclose(lf);
	}
}

void CLog::writeModuleInfo() {
	
	if (!Enabled) return;

	CModuleInfo module;
	HANDLE mainModule = module.getApplicationModule();
	module.init(mainModule);
	write("Applicazione chiamante: %s",module.szModuleName.c_str());
}
