#include "../stdafx.h"
#include "moduleinfo.h"
#include "utilexception.h"

static char *szCompiledFile=__FILE__;

CModuleInfo::CModuleInfo()
{
}

HANDLE CModuleInfo::getApplicationModule()
{
	init_func_internal
	return (HANDLE)GetModuleHandle(NULL);
	exit_func_internal
}

HANDLE CModuleInfo::getModule() {
	return module;
}

void CModuleInfo::init(HANDLE module)
{
	init_func_internal
	this->module = module;
	char path[MAX_PATH];
	if (GetModuleFileName((HMODULE)module,path,MAX_PATH)==0)
		throw CWinException();
	
	szModuleFullPath=path;

	char drive[_MAX_DRIVE], dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath_s(szModuleFullPath.c_str(), drive, dir, fname, ext);
	szModuleName = fname;
	char moddir[MAX_PATH];
	_makepath_s( moddir,drive,dir,NULL,NULL);
	szModulePath = moddir;
	exit_func_internal
}

CModuleInfo::~CModuleInfo(void)
{
}
