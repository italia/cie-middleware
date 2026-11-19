#pragma once

#include <stdlib.h>
#include "defines.h"
#include <string>
#include <wintypes.h>

#ifndef OutputDebugString
#define OutputDebugString printf
#endif

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
//    void initModule(const char *name,const char *version);
	void init();
	void writeModuleInfo();
	void dumpErr();

};

void initLog(const char *moduleName, const char *iniFile,const char *version);
