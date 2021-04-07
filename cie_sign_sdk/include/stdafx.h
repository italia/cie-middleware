// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <windows.h>
#include <atlbase.h>
#include <atlhost.h>
#include <atlstr.h>
#include <stdint.h> 
#include "defines.h"
#include "Array.h"
#include "log.h"
#include "funccallinfo.h"
#include "util.h"
#include "utilexception.h"
#include "ModuleInfo.h"

#ifdef _DEBUG
#define ODS(s) OutputDebugString(s);Log.writePure(s)
#else
#define ODS(s) Log.writePure(s) /*ODS(s)*/
#endif


#if 0
BOOL APIENTRY DllMainP11(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
	) {
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		moduleInfo.init(hinstDLL);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	if (!DllMainP11(hinstDLL, fdwReason, lpvReserved))
		return FALSE;
	return TRUE;
}
#endif

// TODO: reference additional headers your program requires here
