//
//  PINManager.hpp
//  cie-pkcs11
//
//  Created by ugo chirico on 06/10/18.
//  Copyright © 2018 IPZS. All rights reserved.
//

#ifndef PINManager_h
#define PINManager_h

#include <stdio.h>
#include "cryptoki.h"
#include "../PKCS11/PKCS11Functions.h"

#define DEF_CALLBACK_FUNCTION(returnType, name) \
  returnType (__stdcall CK_PTR name)


/* CK_NOTIFY is an application callback that processes events */
typedef DEF_CALLBACK_FUNCTION(CK_RV, PROGRESS_CALLBACK)(
	const int progress,
	const char* szMessage);

typedef DEF_CALLBACK_FUNCTION(CK_RV, COMPLETED_CALLBACK)(
	const char* pan,
	const char* name,
	const char* seriale);

typedef DEF_CALLBACK_FUNCTION(CK_RV, SIGN_COMPLETED_CALLBACK)(
	const int resValue);


typedef CK_RV (*CambioPINfn)(const char*  szCurrentPIN,
                            const char*  szNewPIN,
                            int* attempts,
                            PROGRESS_CALLBACK progressCallBack);

typedef CK_RV (*SbloccoPINfn)(const char*  szPUK,
                            const char*  szNewPIN,
                            int* attempts,
                            PROGRESS_CALLBACK progressCallBack);

#endif /* PINManager_h */
