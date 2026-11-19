#include "../stdafx.h"
#include "moduleinfo.h"
#include "utilexception.h"
#ifndef WIN32
#include <dlfcn.h>
#include <libgen.h>
#endif

static char *szCompiledFile=__FILE__;

CModuleInfo::CModuleInfo()
{
}

#ifdef WIN32
HANDLE CModuleInfo::getApplicationModule()
{
	return (HANDLE)GetModuleHandle(NULL);
}

HANDLE CModuleInfo::getModule() {
	return module;
}

void CModuleInfo::init(HANDLE module)
{
	this->module = module;
	char path[MAX_PATH];
	if (GetModuleFileName((HMODULE)module,path,MAX_PATH)==0)
		throw windows_error(GetLastError());

	szModuleFullPath=path;

	char drive[_MAX_DRIVE], dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath_s(szModuleFullPath.c_str(), drive, dir, fname, ext);
	szModuleName = fname;
	char moddir[MAX_PATH];
	_makepath_s( moddir,drive,dir,NULL,NULL);
	szModulePath = moddir;
}
#else
HANDLE CModuleInfo::getApplicationModule()
{
	return NULL;
}

HANDLE CModuleInfo::getModule() {
	return module;
}

void CModuleInfo::init(HANDLE module)
{
	this->module = module;
	Dl_info info;
	if (dladdr((void*)CModuleInfo::getApplicationModule, &info) && info.dli_fname) {
		szModuleFullPath = info.dli_fname;
	} else {
		szModuleFullPath = "ciepki.dylib";
	}

	char pathbuf[MAX_PATH];
	strncpy(pathbuf, szModuleFullPath.c_str(), MAX_PATH - 1);
	pathbuf[MAX_PATH - 1] = 0;

	char *dirp = dirname(pathbuf);
	szModulePath = std::string(dirp) + "/";

	strncpy(pathbuf, szModuleFullPath.c_str(), MAX_PATH - 1);
	pathbuf[MAX_PATH - 1] = 0;
	char *basep = basename(pathbuf);
	std::string base(basep);
	size_t dot = base.rfind('.');
	if (dot != std::string::npos)
		szModuleName = base.substr(0, dot);
	else
		szModuleName = base;
}
#endif

CModuleInfo::~CModuleInfo(void)
{
}
