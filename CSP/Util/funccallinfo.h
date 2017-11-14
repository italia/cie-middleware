#pragma once

#include "log.h"

class  CFuncCallInfo {
	char *fName;
	unsigned int LogNum;
	CLog &log;
public:
	CFuncCallInfo(char *name,CLog &logInfo);
	~CFuncCallInfo();

	static void startCall();
	
	void logRet(DWORD val, unsigned int line);
	void logRet(char *val, unsigned int line);
	void logRet(BOOL val, unsigned int line);
	void logRet(void *val, unsigned int line);
	void logRet(unsigned int line);
	void logRet(ByteArray &val, unsigned int line);
	void logRet();

	void logParameter(DWORD val);
	void logParameter(DWORD *val);
	void logParameter(void *val);
	void logParameter(char *val);
	void logParameter(unsigned char *val, size_t len);
	void logParameter(char *val, size_t len);
	void logParameter(void *val, size_t len);
	void logParameter(unsigned char *val, size_t *len);
	void logParameter(char *val, size_t *len);
	void logParameter(void *val, size_t *len);
	void logParameter(unsigned char *val, unsigned long len);
	void logParameter(char *val, unsigned long len);
	void logParameter(void *val, unsigned long len);
	void logParameter(unsigned char *val, unsigned long *len);
	void logParameter(char *val, unsigned long *len);
	void logParameter(void *val, unsigned long *len);
	void logParameterHide(char *val, size_t len);
	void logParameterHide(void *val, size_t len);
	void logParameterHide(char *val, size_t *len);
	void logParameterHide(void *val, size_t *len);
};

#define logParam(a) info.logParameter(a);
#define logParamBuf(a,b) info.logParameter(a,b);
#define logParamBufHide(a,b) info.logParameterHide(a,b);
