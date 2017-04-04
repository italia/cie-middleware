#pragma once

#pragma pack(1)
#include "pkcs11.h"
#pragma pack()

#include <winscard.h>

#define MAXVAL 0xffffff
#define MAXSESSIONS MAXVAL

#define CK_ENTRY __declspec(dllexport)
#define LIBRARY_VERSION_MAJOR 2
#define LIBRARY_VERSION_MINOR 0

#define PIN_LEN 8
#define USER_PIN_ID 0x10

#define R_CALL(call) \
if ((_call_ris=(call))!=OK) { \
	_return(_call_ris) \
}
#define DF_CALL(call) call;
#define EDF_CALL(call,err) \
if ((_call_ris=(call))!=OK) { \
	if (_call_ris==FAIL) { \
		_return(CKR_GENERAL_ERROR) \
		} \
	_return(_call_ris) \
}

extern "C" {
	CK_RV CK_ENTRY C_UpdateSlotList();
}