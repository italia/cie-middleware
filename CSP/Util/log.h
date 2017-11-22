#pragma once

#include <stdlib.h>
#include "defines.h"
#include <string>


struct _ATL_SYMBOL_INFO
{
	ULONG_PTR dwAddress;
	ULONG_PTR dwOffset;
	CHAR	szModule[260];
	CHAR	szSymbol[1024];
};

class CLog {
public:
	unsigned int LogCount;
	bool Initialized;
	bool Enabled;
	bool FunctionLog;
	bool LogParam;
	unsigned int ModuleNum;
	std::string logDir;
	std::string logPath;
	std::string logName;
	std::string logFileName;
	std::string::iterator threadPos;
	std::string logVersion;
	bool FirstLog;

	bool _stack_logged;

	CLog(void);
	~CLog(void);
	DWORD write(const char *format,...);
	void writePure(const char *format,...);
	void writeBinData(uint8_t *data, size_t datalen);
	void initModule(const char *name,const char *version);
	void initParam(CLog &log);
	void writeModuleInfo();
	void dumpErr();


	void ___dumpStack(LPVOID param);
	void __dumpStack();
	static LPVOID __stdcall FunctionTableAccess(HANDLE hProcess, ULONG_PTR dwPCAddress);
	static BOOL ResolveSymbol(HANDLE hProcess, UINT_PTR dwAddress,_ATL_SYMBOL_INFO &siSymbol);
	static ULONG_PTR __stdcall GetModuleBase(HANDLE hProcess, ULONG_PTR dwRetAddress);

};

void initLog(const char *iniFile,const char *version);
extern CLog Log;

#define _return(a) {info.logRet(a,__LINE__); return a;}
#define _returnVoid {info.logRet(__LINE__);return;}
