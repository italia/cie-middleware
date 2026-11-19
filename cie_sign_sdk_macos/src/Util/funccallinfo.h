#pragma once

#include "log.h"
#include <memory>

class  CFuncCallInfo {
	const char *fName;
	unsigned int LogNum;
	CLog &log;
public:
	CFuncCallInfo(const char *name,CLog &logInfo);
	~CFuncCallInfo();

	const char *FunctionName();
};

class CFuncCallInfoList {
public:
	CFuncCallInfoList(CFuncCallInfo *info) : info(info) {}
	CFuncCallInfo *info;
	std::unique_ptr<CFuncCallInfoList> next = nullptr;
};
