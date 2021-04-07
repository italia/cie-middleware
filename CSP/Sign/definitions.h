/***************************************************************************
 *            utiltypes.h
 *
 *  Fri Nov 17 01:15:58 2006
 *  Copyright  2006  Ugo Chirico
 *  Email
 ****************************************************************************/

#ifndef __UTILITYTYPES_H
#define __UTILITYTYPES_H


#ifdef WIN32
#include <../StdAfx.h>
#define STRICMP stricmp
#else

#define STRICMP strcasecmp

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

#define NNULL 0
#define UINT unsigned int
#ifndef LONG
#define LONG long
#endif
#ifndef LPCTSTR 
#define LPCTSTR const char*
#endif

#ifndef BYTE 
#define BYTE unsigned char
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif


#ifndef BOOL
typedef bool BOOL;
//#define BOOL unsigned char
#endif


#define HANDLE void*
#define PCHAR char*
#define CHAR char
#define VOID void


#ifndef LOWORD
#define LOWORD(l) l & 0xFFFF
#define HIWORD(l) (l >> 16) & 0xFFFF 
#endif

#ifndef LOBYTE
#define LOBYTE(l) l & 0xFF
#define HIBYTE(l) (l >> 8) & 0xFF
#endif

#define MAX_PATH 256

#define MAKEWORD(lo, hi) lo + (hi * 256)

void SetLastError(unsigned long nErr);
unsigned long GetLastError();

#endif // WIN32

#define szIdAASigningCertificateV2OID	"1.2.840.113549.1.9.16.2.47"
#define szSHA256OID						"2.16.840.1.101.3.4.2.1"
#define szSHA512OID						"2.16.840.1.101.3.4.2.3"
#define szSHA1OID						"1.3.14.3.2.26"//"2.16.840.1.101.3.4.1.1"
#define szContentTypeOID				"1.2.840.113549.1.9.3"
#define szDataOID						"1.2.840.113549.1.7.1"
#define szEncDataOID					"1.2.840.113549.1.7.6"
#define szMessageDigestOID				"1.2.840.113549.1.9.4"
#define szSigningTimeOID				"1.2.840.113549.1.9.5"
#define szSha256WithRsaEncryptionOID    "1.2.840.113549.1.1.11"
#define szSignedDataOID					"1.2.840.113549.1.7.2"
#define szCounterSignatureOID			"1.2.840.113549.1.9.6"
#define szCrlDistributionPointsOID		"2.5.29.31"
#define szKeyUsageOID					"2.5.29.15"
#define szTimestampTokenOID				"1.2.840.113549.1.9.16.2.14"
#define szAuthorityInfoAccess		    "1.3.6.1.5.5.7.1.1"
#define szMethodOCSP					"1.3.6.1.5.5.7.48.1"
#define szAuthorityKeyIdentifier		"2.5.29.35"
#define szSubjectKeyIdentifier           "2.5.29.14"

#define szCertificatePolicies 			"2.5.29.32"
#define szTimeStampDataOID				"1.2.840.113549.1.9.16.1.31"

#define IdEtsiQcsQcCompliance		"0.4.0.1862.1.1"
#define IdEtsiQcsLimitValue			"0.4.0.1862.1.2"
#define IdEtsiQcsRetentionPeriod	"0.4.0.1862.1.3"
#define IdEtsiQcsQcSscd				"0.4.0.1862.1.4"

// PKCS#12 OID
#define szKeyBagOID					"1.2.840.113549.1.12.10.1.1"
#define szPkcs8ShroudedKeyBagOID	"1.2.840.113549.1.12.10.1.2"
#define szCertBagOID				"1.2.840.113549.1.12.10.1.3"
#define szCrlBagOID					"1.2.840.113549.1.12.10.1.4"
#define szSecretBagOID				"1.2.840.113549.1.12.10.1.5"
#define szSafeContentsBagOID		"1.2.840.113549.1.12.10.1.6"


int atox(const char* szVal);

typedef void (*logFunc)(const char*);
extern logFunc pfnCrashliticsLog;
//extern void exceptionToCrashlitics(const char* error);


#define BUFFERSIZE 1000

#define SAFEDELETE(pointer) try { if(pointer) { delete pointer; pointer = NULL;}} catch(...) {}

#define __TRY try {

#define __CATCH } \
catch(long err) { LOG_ERR((0, "__CATCH", "Unexpected Long Exception: %d", err)); ; return DISIGON_ERROR_UNEXPECTED;} \
	catch(CASN1Exception* pExc) { LOG_ERR((0, "__CATCH", "Unexpected ASN1 Exception: %s", pExc->m_lpszMsg)); return DISIGON_ERROR_UNEXPECTED;} \
	catch(...) { LOG_ERR((0, "__CATCH", "Unexpected Exception"));  return DISIGON_ERROR_UNEXPECTED;}

#endif //__UTILITYTYPES_H
