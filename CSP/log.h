#pragma once

#include <stdlib.h>
#include "defines.h"


struct _ATL_SYMBOL_INFO
{
	ULONG_PTR dwAddress;
	ULONG_PTR dwOffset;
	CHAR	szModule[260];
	CHAR	szSymbol[1024];
};

class CLog {
public:
	DWORD dwLogCount;
	bool bInitialized;
	bool bEnabled;
	bool bFunctionLog;
	bool bLogParam;
	DWORD dwModuleNum;
	String logDir;
	String logPath;
	String logName;
	String logFileName;
	char *threadPos;
	String logVersion;
	bool bFirstLog;

	bool _stack_logged;

	CLog(void);
	~CLog(void);
	DWORD write(const char *format,...);
	void writePure(const char *format,...);
	void writeBinData(BYTE *data,int datalen);
	void initModule(const char *name,char *version);
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
