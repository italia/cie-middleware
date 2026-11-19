#include "../stdafx.h"
#ifdef WIN32
#include <windows.h>
#include <imagehlp.h>
#endif
#include "utilexception.h"
#include <stdio.h>
#include "../LOGGER/Logger.h"

extern thread_local std::unique_ptr<CFuncCallInfoList> callQueue;

logged_error::logged_error(const char *message) : std::runtime_error(message) {
	OutputDebugString(what());
	LOG_ERROR("%s", message);

	CFuncCallInfoList *ptr = callQueue.get();
	while (ptr != nullptr) {
		OutputDebugString(ptr->info->FunctionName());
		ptr = ptr->next.get();
	}
}

scard_error::scard_error(StatusWord sw) : logged_error(stdPrintf("Errore smart card:%04x", sw)) { }

#ifdef WIN32
windows_error::windows_error(long ris) : logged_error(stdPrintf("Errore windows:%s (%08x) ", WinErr(ris), ris)) {}
#else
windows_error::windows_error(long ris) : logged_error(stdPrintf("Errore sistema:%ld (%08lx) ", ris, ris)) {}
#endif
