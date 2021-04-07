#pragma once
#include "util.h"


class CModuleInfo
{
	HANDLE module;
public:
	std::string szModuleFullPath;
	std::string szModulePath;
	std::string szModuleName;

	CModuleInfo(void);
	virtual ~CModuleInfo(void);
	static HANDLE getApplicationModule();
	HANDLE getModule();
	void init(HANDLE module);
};
