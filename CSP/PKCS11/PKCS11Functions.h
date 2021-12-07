#pragma once

#include "cryptoki.h"

//#pragma pack(1)
//#include "pkcs11.h"
//#pragma pack()

#include <winscard.h>

#define MAXVAL 0xffffff
#define MAXSESSIONS MAXVAL

#define CK_ENTRY __declspec(dllexport)
#define LIBRARY_VERSION_MAJOR 2
#define LIBRARY_VERSION_MINOR 0

#define PIN_LEN 8
#define USER_PIN_ID 0x10

#define init_p11_func \
	LOG_INFO("[PKCS11]	%s", __FUNCTION__);	\
	try {

#define exit_p11_func } \
	catch (p11_error &p11Err) { \
		LOG_ERROR("EXCLOG %d", p11Err.getP11ErrorCode()); \
		OutputDebugString("EXCLOG->"); \
		OutputDebugString(p11Err.what()); \
		OutputDebugString("<-EXCLOG");\
		return p11Err.getP11ErrorCode(); \
	} \
	catch (std::exception &err) { \
			LOG_ERROR("EXCLOG: CKR_GENERAL_ERROR"); \
			OutputDebugString("EXCLOG->"); \
		OutputDebugString(err.what()); \
		OutputDebugString("<-EXCLOG");\
		return CKR_GENERAL_ERROR; \
	} \
catch (...) { return CKR_GENERAL_ERROR; }

extern "C" {
	CK_RV CK_ENTRY C_UpdateSlotList();
}