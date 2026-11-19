/***************************************************************************
 *            utiltypes.h
 *
 *  Fri Nov 17 01:15:58 2006
 *  Copyright  2006  User
 *  Email
 ****************************************************************************/

#ifndef __UTILITYTYPES_H
#define __UTILITYTYPES_H

#include <stdint.h>
#include <memory.h>

#define IN
#define OUT

#define BT0_PADDING   0
#define BT1_PADDING   1
#define BT2_PADDING   2
#define CALG_MD2   1
#define CALG_MD5   2
#define CALG_SHA1  3

#define ERROR_FILE_NOT_FOUND  0x02
#define ERROR_MORE_DATA  0xE0
#define ERROR_INVALID_DATA 0xE1
#define ERROR_UNABLE_TO_ALLOCATE  0xE2

#define NNULL 0
#define UINT unsigned int
#ifndef LONG
#define LONG long
#endif
#ifndef LPCTSTR 
#define LPCTSTR const char*
#endif

#ifndef LPCSTR
#define LPCSTR const char*
#endif

#ifndef BYTE 
#define BYTE unsigned char
#endif

#ifndef DWORD
#define DWORD uint32_t
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef BOOL
#define BOOL unsigned char
#endif

#define HANDLE void*
#define PCHAR char*
#define CHAR char
#define VOID void

#ifndef HRESULT
#define HRESULT unsigned long
#endif

#ifndef LOWORD
#define LOWORD(l) l & 0xFFFF
#define HIWORD(l) (l >> 16) & 0xFFFF 
#endif

#ifndef LOBYTE
#define LOBYTE(l) (uint8_t)(l & 0xFF)
#define HIBYTE(l) (uint8_t)((l >> 8) & 0xFF)
#endif

#define MAX_PATH 1024

#define MAKEWORD(lo, hi) lo + (hi * 256)

#define S_OK  0

void SetLastError(unsigned long nErr);
unsigned long GetLastError();
int atox(const char* szVal);

#ifndef ERROR_INVALID_HANDLE
#define ERROR_INVALID_HANDLE 6
#endif


#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS 0
#endif
#define ODS printf
#endif //__UTILITYTYPES_H
