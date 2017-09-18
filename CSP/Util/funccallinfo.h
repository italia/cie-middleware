#pragma once

#include "log.h"

class  CFuncCallInfo {
	char *fName;
	DWORD dwLogNum;
	CLog &log;
public:
	CFuncCallInfo(char *name,CLog &logInfo);
	~CFuncCallInfo();

	static void startCall();
	
	void logRet(DWORD val,DWORD line);
	void logRet(char *val,DWORD line);
	void logRet(BOOL val,DWORD line);
	void logRet(void *val,DWORD line);
	void logRet(DWORD line);
	void logRet(ByteArray &val,DWORD line);
	void logRet();

	void logParameter(DWORD val);
	void logParameter(DWORD *val);
	void logParameter(void *val);
	void logParameter(char *val);
	void logParameter(char *val,DWORD len);
	void logParameter(void *val,DWORD len);
	void logParameter(char *val,DWORD *len);
	void logParameter(void *val,DWORD *len);
	void logParameterHide(char *val,DWORD len);
	void logParameterHide(void *val,DWORD len);
	void logParameterHide(char *val,DWORD *len);
	void logParameterHide(void *val,DWORD *len);
};

#define logParam(a) info.logParameter(a);
#define logParamBuf(a,b) info.logParameter(a,b);
#define logParamBufHide(a,b) info.logParameterHide(a,b);
