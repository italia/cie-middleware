
#include <stdio.h>
#include "UtilException.h"
#include "util.h"

//#ifdef WIN32
//
//#include <windows.h>
//#include <imagehlp.h>
//
//extern thread_local std::unique_ptr<CFuncCallInfoList> callQueue;
//
//
//logged_error::logged_error(std::string message) {
//    const char* msg = message.c_str();
//    logged_error(msg);
//}
//
//logged_error::logged_error(const char *message) : std::runtime_error(message) {
//    OutputDebugString(what());
//    CFuncCallInfoList *ptr = callQueue.get();
//    while (ptr != nullptr) {
//        OutputDebugString(ptr->info->FunctionName());
//        ptr = ptr->next.get();
//    }
//}
//
//scard_error::scard_error(StatusWord sw) : logged_error(stdPrintf("Errore smart card:%04x", sw)) { }
//
//windows_error::windows_error(long ris) : logged_error(stdPrintf("Errore windows:%s (%08x) ", WinErr(ris), ris)) {}
//
//#else

logged_error::logged_error(std::string message)
: std::runtime_error(message.c_str())
{
    logged_error(message.c_str());
}

logged_error::logged_error(const char *message) : std::runtime_error(message) {
    OutputDebugString(what());
//    CFuncCallInfoList *ptr = callQueue.get();
//    while (ptr != nullptr) {
//        OutputDebugString(ptr->info->FunctionName());
//        ptr = ptr->next.get();
//    }
}

scard_error::scard_error(StatusWord sw) : logged_error(stdPrintf("Errore smart card:%x", sw)) { }

windows_error::windows_error(long ris) : logged_error(stdPrintf("Errore windows:(%08x) ", ris)) {}

//#endif
