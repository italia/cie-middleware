#pragma once
#include "util.h"


class CModuleInfo
{
	HANDLE module;
public:
	String szModuleFullPath;
	String szModulePath;
	String szModuleName;

	CModuleInfo(void);
	virtual ~CModuleInfo(void);
	static HANDLE getApplicationModule();
	HANDLE getModule();
	void init(HANDLE module);
};
