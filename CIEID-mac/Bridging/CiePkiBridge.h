#ifndef CIE_PKI_BRIDGE_H
#define CIE_PKI_BRIDGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long CK_RV;
typedef unsigned long CK_ULONG;
typedef unsigned long CK_FLAGS;
typedef unsigned long CK_SLOT_ID;
typedef unsigned char CK_BYTE;
typedef CK_BYTE CK_UTF8CHAR;
typedef CK_BYTE CK_BBOOL;
typedef CK_BYTE CK_CHAR;

typedef struct CK_VERSION {
    CK_BYTE major;
    CK_BYTE minor;
} CK_VERSION;

typedef struct CK_INFO {
    CK_VERSION cryptokiVersion;
    CK_UTF8CHAR manufacturerID[32];
    CK_FLAGS flags;
    CK_UTF8CHAR libraryDescription[32];
    CK_VERSION libraryVersion;
} CK_INFO;

typedef struct CK_SLOT_INFO {
    CK_UTF8CHAR slotDescription[64];
    CK_UTF8CHAR manufacturerID[32];
    CK_FLAGS flags;
    CK_VERSION hardwareVersion;
    CK_VERSION firmwareVersion;
} CK_SLOT_INFO;

typedef struct CK_TOKEN_INFO {
    CK_UTF8CHAR label[32];
    CK_UTF8CHAR manufacturerID[32];
    CK_UTF8CHAR model[16];
    CK_CHAR serialNumber[16];
    CK_FLAGS flags;
    CK_ULONG ulMaxSessionCount;
    CK_ULONG ulSessionCount;
    CK_ULONG ulMaxRwSessionCount;
    CK_ULONG ulRwSessionCount;
    CK_ULONG ulMaxPinLen;
    CK_ULONG ulMinPinLen;
    CK_ULONG ulTotalPublicMemory;
    CK_ULONG ulFreePublicMemory;
    CK_ULONG ulTotalPrivateMemory;
    CK_ULONG ulFreePrivateMemory;
    CK_VERSION hardwareVersion;
    CK_VERSION firmwareVersion;
    CK_CHAR utcTime[16];
} CK_TOKEN_INFO;

#define CKR_OK                 0x00000000UL
#define CKR_CANCEL             0x00000001UL
#define CKR_GENERAL_ERROR      0x00000005UL
#define CKR_DEVICE_ERROR       0x00000030UL
#define CKR_PIN_INCORRECT      0x000000A0UL
#define CKR_PIN_INVALID        0x000000A1UL
#define CKR_PIN_LEN_RANGE      0x000000A2UL
#define CKR_PIN_EXPIRED        0x000000A3UL
#define CKR_PIN_LOCKED         0x000000A4UL
#define CKR_TOKEN_NOT_PRESENT  0x000000E0UL
#define CKR_TOKEN_NOT_RECOGNIZED 0x000000E1UL

#define CKF_TOKEN_PRESENT      0x00000001UL

#define CK_TRUE  1
#define CK_FALSE 0

typedef CK_RV (*PROGRESS_CALLBACK)(const int progress, const char *szMessage);

CK_RV C_Initialize(void *pInitArgs);
CK_RV C_Finalize(void *pReserved);
CK_RV C_GetInfo(CK_INFO *pInfo);
CK_RV C_GetSlotList(CK_BBOOL tokenPresent, CK_SLOT_ID *pSlotList, CK_ULONG *pulCount);
CK_RV C_GetSlotInfo(CK_SLOT_ID slotID, CK_SLOT_INFO *pInfo);
CK_RV C_GetTokenInfo(CK_SLOT_ID slotID, CK_TOKEN_INFO *pInfo);
CK_RV C_UpdateSlotList(void);

CK_RV ChangePIN(const char *szCurrentPIN,
                const char *szNewPIN,
                int *pAttempts,
                PROGRESS_CALLBACK progressCallBack);

CK_RV UnlockPIN(const char *szPUK,
                const char *szNewPIN,
                int *pAttempts,
                PROGRESS_CALLBACK progressCallBack);

typedef CK_RV (*SIGN_COMPLETED_CALLBACK)(const int ret);

CK_RV firmaConCIE(const char *inFilePath,
                  const char *type,
                  const char *pin,
                  const char *pan,
                  int page,
                  float x,
                  float y,
                  float w,
                  float h,
                  const char *imagePathFile,
                  const char *outFilePath,
                  PROGRESS_CALLBACK progressCallBack,
                  SIGN_COMPLETED_CALLBACK completedCallBack);

CK_RV leggiPANCIE(char *outPan, unsigned int outLen);

#ifdef __cplusplus
}
#endif

#endif /* CIE_PKI_BRIDGE_H */
