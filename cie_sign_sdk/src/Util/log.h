#pragma once

#include <stdlib.h>
#include "defines.h"
#include <string>
#include "windows.h"

class CLog {
public:
	bool Enabled;
	std::string logDir;
	std::string logPath;
	std::string logName;
	std::string logFileName;
	bool LogParam;
	std::string::iterator threadPos;
	bool _stack_logged;

	CLog(void);
	~CLog(void);
	DWORD write(const char *format,...);
	void writePure(const char *format,...);
	void writeBinData(uint8_t *data, size_t datalen);
	void initParam(bool enabled);
	void writeModuleInfo();
	void dumpErr();

};

void initLog();
extern CLog Log;
