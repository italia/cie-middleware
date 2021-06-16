
#ifndef __DISIGON_H
#define __DISIGON_H

#ifndef WIN32
#define DISIGON_API API //__declspec(dllexport)
#else
#ifdef DISIGON_STATIC
#define DISIGON_API
#else
#ifdef DISIGON_EXPORTS
#define DISIGON_API //__declspec(dllexport)
#else
#define DISIGON_API //__declspec(dllimport)
#endif
#endif
#endif

#define MAX_LEN							256

#include "definitions.h"

#define VERIFIED_CERT_VALIDITY			0x000001
#define VERIFIED_CERT_REVOKED			0x000002
#define VERIFIED_CERT_SUSPENDED			0x000004
#define VERIFIED_CERT_GOOD				0x000008
#define VERIFIED_CERT_SHA256			0x000010
#define VERIFIED_CERT_QUALIFIED			0x000020
#define VERIFIED_CERT_CHAIN			0x000040

#define VERIFIED_CRL_LOADED				0x000080
#define VERIFIED_CRL_EXPIRED			0x000100

#define VERIFIED_SIGNED_ATTRIBUTE_CT	0x000200
#define VERIFIED_SIGNED_ATTRIBUTE_MD    0x000400
#define VERIFIED_SIGNED_ATTRIBUTE_SC    0x000800

#define VERIFIED_SIGNATURE				0x001000
#define VERIFIED_SHA256					0x002000

#define VERIFIED_CACERT_VALIDITY		0x004000
#define VERIFIED_CACERT_REVOKED			0x008000
#define VERIFIED_CACERT_SUSPENDED		0x010000
#define VERIFIED_CACERT_GOOD			0x020000
#define VERIFIED_CACERT_SIGNATURE		0x040000
#define VERIFIED_CACRL_LOADED			0x080000
#define VERIFIED_CACERT_FOUND			0x100000

#define VERIFIED_KEY_USAGE				0x200000

// TODO
// certificate store con cache degli esiti delle verifiche dei certificati
// stato revoca e validit� dei certificati nella chain


#define REVOCATION_STATUS_GOOD  0
#define REVOCATION_STATUS_REVOKED  1
#define REVOCATION_STATUS_SUSPENDED 2
#define REVOCATION_STATUS_UNKNOWN  3
#define REVOCATION_STATUS_NOTLOADED  4

#define DISIGON_OPT_PKCS11				1
#define DISIGON_OPT_SLOT					2
#define DISIGON_OPT_PIN					3
#define DISIGON_OPT_ALIAS				4
#define DISIGON_OPT_CADES				5
#define DISIGON_OPT_XADES				5
#define DISIGON_OPT_DETACHED				6
#define DISIGON_OPT_INPUTFILE			7
#define DISIGON_OPT_OUTPUTFILE			8
#define DISIGON_OPT_INPUTFILE_TYPE		9
#define DISIGON_OPT_TSA_URL				10
#define DISIGON_OPT_TSA_USERNAME			11
#define DISIGON_OPT_TSA_PASSWORD			12
#define DISIGON_OPT_VERIFY_REVOCATION	13
#define DISIGON_OPT_LOG_LEVEL			14
#define DISIGON_OPT_LOG_FILE				15
#define DISIGON_OPT_INPUTFILE_PLAINTEXT	16
#define DISIGON_OPT_CACERT_DIR			17
#define DISIGON_OPT_PDF_SUBFILTER		18
#define DISIGON_OPT_CONFIG_FILE			19
#define DISIGON_OPT_PROXY				20
#define DISIGON_OPT_PROXY_PORT			21
#define DISIGON_OPT_PROXY_USRPASS		22
#define DISIGON_OPT_OID_MAP_FILE			23
#define DISIGON_OPT_TCP_TIMEOUT			24
#define DISIGON_OPT_PDF_REASON			25
#define DISIGON_OPT_PDF_NAME				26
#define DISIGON_OPT_PDF_LOCATION			27
#define DISIGON_OPT_PDF_PAGE				28
#define DISIGON_OPT_PDF_LEFT				29
#define DISIGON_OPT_PDF_BOTTOM			30
#define DISIGON_OPT_PDF_WIDTH			31
#define DISIGON_OPT_PDF_HEIGHT			32
#define DISIGON_OPT_PDF_IMAGEPATH			33
#define DISIGON_OPT_PDF_GRAPHOMETRIC_DATA	34
#define DISIGON_OPT_PDF_GRAPHOMETRIC_DATA_VER 35

#define DISIGON_OPT_ATR_LIST_FILE		36
#define DISIGON_OPT_HASH_ALGO			37

#define DISIGON_OPT_LICENSEE				38
#define DISIGON_OPT_PRODUCTKEY			39

#define DISIGON_OPT_RS_OTP_PIN			40
#define DISIGON_OPT_RS_HSMTYPE			41
#define DISIGON_OPT_RS_TYPE_OTP_AUTH		42
#define DISIGON_OPT_RS_USERNAME			43
#define DISIGON_OPT_RS_PASSWORD			44
#define DISIGON_OPT_RS_CERTID			45
#define DISIGON_OPT_RS_SERVICE_URL		46
#define DISIGON_OPT_RS_USER_CODE			47
#define DISIGON_OPT_RS_SERVICE_TYPE		48

#define DISIGON_OPT_PDF_DESCRIPTION		50
#define DISIGON_OPT_PDF_NAME_LABEL		51
#define DISIGON_OPT_PDF_REASON_LABEL		52
#define DISIGON_OPT_PDF_LOCATION_LABEL	53

#define DISIGON_OPT_TSL_URL				60
#define DISIGON_OPT_VERIFY_USER_CERTIFICATE	61

#define DISIGON_OPT_P12_FILEPATH			70
#define DISIGON_OPT_P12_PASSWORD			71

#define DISIGON_OPT_IAS_INSTANCE           80

#define DISIGON_PDF_SUBFILTER_PKCS_DETACHED "adbe.pkcs7.detached"
#define DISIGON_PDF_SUBFILTER_ETSI_CADES "ETSI.CAdES.detached"


#define DISIGON_ERROR_BASE			0x84000000UL


#define DISIGON_ERROR_UNEXPECTED			DISIGON_ERROR_BASE + 1
#define DISIGON_ERROR_FILE_NOT_FOUND		DISIGON_ERROR_BASE + 2
#define DISIGON_ERROR_DETACHED_PKCS7		DISIGON_ERROR_BASE + 3
#define DISIGON_ERROR_CERT_REVOKED		DISIGON_ERROR_BASE + 4
#define DISIGON_ERROR_INVALID_FILE		DISIGON_ERROR_BASE + 5
#define DISIGON_ERROR_INVALID_P11		DISIGON_ERROR_BASE + 6
#define DISIGON_ERROR_INVALID_ALIAS		DISIGON_ERROR_BASE + 7
#define DISIGON_ERROR_INVALID_SIGOPT		DISIGON_ERROR_BASE + 8
#define DISIGON_ERROR_ARRS_BASE			DISIGON_ERROR_BASE + 0x00100000
#define DISIGON_ERROR_CERT_INVALID		DISIGON_ERROR_BASE + 9
#define DISIGON_ERROR_CERT_EXPIRED		DISIGON_ERROR_BASE + 10
#define DISIGON_ERROR_CACERT_NOTFOUND	DISIGON_ERROR_BASE + 11
#define DISIGON_ERROR_CERT_NOTFOUND		DISIGON_ERROR_BASE + 12
#define DISIGON_ERROR_CERT_NOT_FOR_SIGNATURE		DISIGON_ERROR_BASE + 13

#define DISIGON_ERROR_TSL_LOAD			DISIGON_ERROR_BASE + 20
#define DISIGON_ERROR_TSL_PARSE			DISIGON_ERROR_BASE + 21
#define DISIGON_ERROR_TSL_INVALID		DISIGON_ERROR_BASE + 22
#define DISIGON_ERROR_TSL_CACERTDIR_NOT_SET		DISIGON_ERROR_BASE + 23
#define DISIGON_ERROR_TSA		DISIGON_ERROR_BASE + 30

#define  DISIGON_ERROR_WRONG_PIN     DISIGON_ERROR_BASE + 40
#define  DISIGON_ERROR_PIN_LOCKED  DISIGON_ERROR_BASE + 41

    
#define DISIGON_FILETYPE_PLAINTEXT	0
#define DISIGON_FILETYPE_P7M			1
#define DISIGON_FILETYPE_PDF			2
#define DISIGON_FILETYPE_M7M			3
#define DISIGON_FILETYPE_TSR			4
#define DISIGON_FILETYPE_TST			5
#define DISIGON_FILETYPE_TSD			6
#define DISIGON_FILETYPE_XML			7
#define DISIGON_FILETYPE_AUTO		8

/*
#define DISIGON_VERIFYTYPE_P7M		1
#define DISIGON_VERIFYTYPE_TSD		2
#define DISIGON_VERIFYTYPE_TST		3
#define DISIGON_VERIFYTYPE_TSR		4
#define DISIGON_VERIFYTYPE_PDF		5
#define DISIGON_VERIFYTYPE_XML		6
#define DISIGON_VERIFYTYPE_M7M        7
*/
#define DISIGON_ALGO_SHA1			1
#define DISIGON_ALGO_SHA256			2
#define DISIGON_ALGO_SHA512			3
#define DISIGON_ALGO_MD5				4

#define LOG_TYPE_ERROR			1
#define LOG_TYPE_WARNING		2
#define LOG_TYPE_MESSAGE		3
#define LOG_TYPE_DEBUG			4  

#define TYPE_OCSP 1
#define TYPE_CRL 2

#define DISIGON_RS_SERVICE_TYPE_NONE			0
#define DISIGON_RS_SERVICE_TYPE_ARUBA		1
#define DISIGON_RS_SERVICE_TYPE_ITTELECOM	2



typedef struct _REVOCATION_INFO
{
	int nType;  // OCSP, CRL
	char szExpiration[60];
	char szThisUpdate[60];
	int nRevocationStatus;
	char szRevocationDate[60];
} REVOCATION_INFO;


typedef struct _SIGNER_INFO
{
	char szCN[MAX_LEN * 2];
    char szDN[MAX_LEN * 2];
    char szGIVENNAME[MAX_LEN * 2];
    char szSURNAME[MAX_LEN * 2];
	char szSN[MAX_LEN * 2];
	char szCADN[MAX_LEN * 2];
	char** pszExtensions;
	int nExtensionsCount;
	char szExpiration[MAX_LEN];
	char szValidFrom[MAX_LEN];
	long bitmask;
	char szDigestAlgorithm[MAX_LEN];
	char szSigningTime[MAX_LEN];
	char szCertificateV2[MAX_LEN];
	BOOL b2011Error;
	BYTE* pCertificate;
	int nCertLen;
	void* pTimeStamp;
	REVOCATION_INFO* pRevocationInfo;
	void* pCounterSignatures;
	int nCounterSignatureCount;
} SIGNER_INFO;

typedef struct _TS_INFO
{
	SIGNER_INFO signerInfo;
	char szTimestamp[MAX_LEN];  
	char szTimeStampImprintAlgorithm[MAX_LEN];
	char szTimeStampMessageImprint[MAX_LEN];
	char szTimeStampSerial[MAX_LEN];
} TS_INFO;
 

typedef struct _SIGNER_INFOS 
{
	SIGNER_INFO* pSignerInfo;
	int nCount;
} SIGNER_INFOS;

typedef struct _VERIFY_INFO
{
	SIGNER_INFOS* pSignerInfos;
	TS_INFO* pTSInfo;

} VERIFY_INFO;

typedef struct _VERIFY_RESULT
{
	int nResultType;
	BOOL bVerifyCRL;
	VERIFY_INFO verifyInfo;
	long nErrorCode;
	char szInputFile[MAX_PATH];
	char szPlainTextFile[MAX_PATH];
} VERIFY_RESULT;


typedef struct _CERTIFICATE
{
	char szLabel[MAX_LEN * 2];
	char szCN[MAX_LEN * 2];
	char szSN[MAX_LEN * 2];
	char szCACN[MAX_LEN * 2];
	char szExpiration[MAX_LEN];
	char szValidFrom[MAX_LEN];
	BYTE* pCertificate;
	int nCertLen;
} CERTIFICATE;


typedef struct _CERTIFICATES 
{
	CERTIFICATE* pCertificate;
	int nCount;
} CERTIFICATES;


typedef void* DISIGON_CTX;

/*
#ifdef  __cplusplus
extern "C" {
#endif
*/

// Imposta le opzioni generali della libreria
DISIGON_API long disigon_set(int option, void* value);

DISIGON_API long disigon_set_int(int option, int value);

DISIGON_API long disigon_set_string(int option, char* value);

// libera la memoria allocata dalla libreria
DISIGON_API void disigon_cleanup();

///////////////////////////////////
// Funzioni di firma digitale

// inizializza la funzione di firma
DISIGON_API DISIGON_CTX disigon_sign_init(void);

// imposta le opzioni di firma
DISIGON_API long disigon_sign_set_int(DISIGON_CTX ctx, int option, int value);

DISIGON_API long disigon_sign_set_string(DISIGON_CTX ctx, int option, char* value);

DISIGON_API long disigon_sign_set(DISIGON_CTX ctx, int option, void* value);

// esegue la firma secondo le opzioni passate
DISIGON_API long disigon_sign_sign(DISIGON_CTX ctx);

// libera la memoria allocata nell'operazione di firma
DISIGON_API long disigon_sign_cleanup(DISIGON_CTX ctx);

DISIGON_API long disigon_sign_getcertificates(DISIGON_CTX ctx, CERTIFICATES* certs);

DISIGON_API void disigon_sign_freecertificates(CERTIFICATES* certs);

//
///////////////////////////////////////////

//////////////////////////////////////////
// Funzioni di verifica della firma

// inizializza l'operazione di verifica
DISIGON_API DISIGON_CTX disigon_verify_init(void);

// imposta le opzioni di verifica
DISIGON_API long disigon_verify_set(DISIGON_CTX ctx, int option, void* value);
DISIGON_API long disigon_verify_set_int(DISIGON_CTX ctx, int option, int value);
DISIGON_API long disigon_verify_set_string(DISIGON_CTX ctx, int option, char* value);

// esegue l'operazione di verifica secondo le opzioni passate
DISIGON_API long disigon_verify_verify(DISIGON_CTX ctx, VERIFY_RESULT* pVerifyResult);

// libera la memoria allocata per la struttura dei risultati delle verifica
DISIGON_API long disigon_verify_cleanup_result(VERIFY_RESULT* pVerifyResult);

// libera la memoria allocata nell'operazione di verifica
DISIGON_API long disigon_verify_cleanup(DISIGON_CTX ctx);

//estra il documento originale da un file p7m
DISIGON_API long disigon_get_file_from_p7m(DISIGON_CTX ctx);
// operazione di sbustamento veloce

/*
#ifdef  __cplusplus
}
#endif
*/
#endif // __DISIGON_H

