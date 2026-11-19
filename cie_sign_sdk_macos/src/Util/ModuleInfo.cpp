
#include "ModuleInfo.h"
#include "UtilException.h"

CModuleInfo::CModuleInfo()
{
}

HANDLE CModuleInfo::getApplicationModule()
{
    return 0;
	//return (HANDLE)GetModuleHandle(NULL);
}

HANDLE CModuleInfo::getModule() {
	return module;
}

void CModuleInfo::init(HANDLE module)
{
	this->module = module;
//    char path[MAX_PATH];
//    if (GetModuleFileName((HMODULE)module,path,MAX_PATH)==0)
//        throw windows_error(GetLastError());
//
//    szModuleFullPath=path;
//
//    char drive[_MAX_DRIVE], dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
//    _splitpath_s(szModuleFullPath.c_str(), drive, dir, fname, ext);
//    szModuleName = fname;
//    char moddir[MAX_PATH];
//    _makepath_s( moddir,drive,dir,NULL,NULL);
//    szModulePath = moddir;
}

CModuleInfo::~CModuleInfo(void)
{
}
