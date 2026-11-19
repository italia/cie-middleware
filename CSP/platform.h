#pragma once

#if defined(WIN32) || defined(_WIN32)
#error "platform.h should not be included on Windows builds"
#endif

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <limits.h>
#include <sys/stat.h>
#include <pthread.h>
#include <atomic>
#include <string>

#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>

#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif

#ifndef _MAX_DRIVE
#define _MAX_DRIVE 3
#define _MAX_DIR 256
#define _MAX_FNAME 256
#define _MAX_EXT 256
#endif

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// PCSC/wintypes.h already defines: BOOL, LONG, WORD, DWORD, ULONG,
// BYTE, LPVOID, LPCSTR, LPSTR, LPCWSTR, PDWORD, LPDWORD, LPBYTE
typedef long HRESULT;
typedef long NTSTATUS;
typedef void* HANDLE;
typedef void* HMODULE;
typedef wchar_t* LPWSTR;
typedef void* LPCONTEXT;
typedef SCARD_READERSTATE* LPSCARD_READERSTATE;

#ifndef S_OK
#define S_OK 0L
#endif

#ifndef E_UNEXPECTED
#define E_UNEXPECTED ((HRESULT)0x8000FFFFL)
#endif

#ifndef ERROR_INVALID_HANDLE
#define ERROR_INVALID_HANDLE 6
#endif

#ifndef ERROR_FILE_NOT_FOUND
#define ERROR_FILE_NOT_FOUND 2
#endif

#ifndef ERROR_INSUFFICIENT_BUFFER
#define ERROR_INSUFFICIENT_BUFFER 122
#endif

#ifndef WAIT_ABANDONED
#define WAIT_ABANDONED 0x00000080L
#endif

#ifndef SCARD_PROTOCOL_Tx
#define SCARD_PROTOCOL_Tx (SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1)
#endif

#ifndef SCARD_ATTR_ATR_STRING
#define SCARD_ATTR_ATR_STRING 0x00090303
#endif

#ifndef SCARD_PROTOCOL_UNDEFINED
#define SCARD_PROTOCOL_UNDEFINED 0
#endif

#ifndef HIBYTE
#define HIBYTE(w) ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#endif
#ifndef LOBYTE
#define LOBYTE(w) ((BYTE)((WORD)(w) & 0xFF))
#endif
#ifndef MAKEWORD
#define MAKEWORD(a, b) ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#endif
#ifndef MAKELONG
#define MAKELONG(a, b) ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#endif
#ifndef MAKELANGID
#define MAKELANGID(p, s) ((((WORD)(s)) << 10) | (WORD)(p))
#endif

#ifndef FAILED
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif
#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

#define memcpy_s(dest, destSize, src, count) memcpy(dest, src, count)
#define sprintf_s(buf, size, fmt, ...) snprintf(buf, size, fmt, ##__VA_ARGS__)
#define swprintf_s(buf, size, fmt, ...) swprintf(buf, size, fmt, ##__VA_ARGS__)
#define vsprintf_s(buf, size, fmt, args) vsnprintf(buf, size, fmt, args)
#define strnlen_s strnlen

#define lstrcmp strcmp
#define lstrcmpW wcscmp
#define lstrcpyW wcscpy

#define OutputDebugString(s) fprintf(stderr, "%s\n", (s))

#define Sleep(ms) usleep((useconds_t)(ms) * 1000)

#define InterlockedIncrement(p) __sync_add_and_fetch(p, 1)

#define GetCurrentProcessId() ((DWORD)getpid())
#define GetCurrentThreadId() ((DWORD)(uintptr_t)pthread_self())

#define WINAPI
#define APIENTRY
#define __cdecl
#define __declspec(x)
#define __forceinline inline

#define DLL_PROCESS_ATTACH 1
#define DLL_PROCESS_DETACH 0

#define CK_ENTRY __attribute__((visibility("default")))

#define GetLastError() ((HRESULT)errno)

static inline void _splitpath_s(const char *path, char *drive, char *dir, char *fname, char *ext) {
    if (drive) drive[0] = 0;
    std::string spath(path);
    size_t lastSlash = spath.rfind('/');
    size_t lastDot = spath.rfind('.');
    if (dir) {
        if (lastSlash != std::string::npos)
            snprintf(dir, _MAX_DIR, "%s", spath.substr(0, lastSlash + 1).c_str());
        else
            dir[0] = 0;
    }
    if (fname) {
        size_t start = (lastSlash != std::string::npos) ? lastSlash + 1 : 0;
        size_t end = (lastDot != std::string::npos && lastDot > start) ? lastDot : spath.size();
        snprintf(fname, _MAX_FNAME, "%s", spath.substr(start, end - start).c_str());
    }
    if (ext) {
        if (lastDot != std::string::npos && (lastSlash == std::string::npos || lastDot > lastSlash))
            snprintf(ext, _MAX_EXT, "%s", spath.substr(lastDot).c_str());
        else
            ext[0] = 0;
    }
}

static inline void _makepath_s(char *path, const char *drive, const char *dir, const char *fname, const char *ext) {
    path[0] = 0;
    if (drive && drive[0]) strcat(path, drive);
    if (dir && dir[0]) strcat(path, dir);
    if (fname && fname[0]) strcat(path, fname);
    if (ext && ext[0]) strcat(path, ext);
}

static inline int _vscprintf(const char *format, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    return len;
}
