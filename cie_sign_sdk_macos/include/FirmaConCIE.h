#ifndef FirmaConCIE_h
#define FirmaConCIE_h

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long CK_RV;
#ifndef CK_ENTRY
#ifdef __APPLE__
#define CK_ENTRY __attribute__((visibility("default")))
#else
#define CK_ENTRY
#endif
#endif

typedef CK_RV (*PROGRESS_CALLBACK)(const int progress, const char* szMessage);
typedef CK_RV (*SIGN_COMPLETED_CALLBACK)(const int ret);

CK_RV CK_ENTRY firmaConCIE(const char* inFilePath,
                           const char* type,
                           const char* pin,
                           const char* pan,
                           int page,
                           float x,
                           float y,
                           float w,
                           float h,
                           const char* imagePathFile,
                           const char* outFilePath,
                           PROGRESS_CALLBACK progressCallBack,
                           SIGN_COMPLETED_CALLBACK completedCallBack);

CK_RV CK_ENTRY leggiPANCIE(char* outPan, unsigned int outLen);

#ifdef __cplusplus
}
#endif

#endif /* FirmaConCIE_h */
