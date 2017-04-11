#include <windows.h>
#include "Cardmod.h"

DWORD WINAPI CardDeleteContainer(
	__in    PCARD_DATA  pCardData,
	__in    BYTE        bContainerIndex,
	__in    DWORD       dwReserved) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}


DWORD
WINAPI
CardCreateContainer(
__in    PCARD_DATA  pCardData,
__in    BYTE        bContainerIndex,
__in    DWORD       dwFlags,
__in    DWORD       dwKeySpec,
__in    DWORD       dwKeySize,
__in    PBYTE       pbKeyData) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD
WINAPI
#pragma warning(suppress: 6101)
CardGetChallengeEx(
__in                                    PCARD_DATA  pCardData,
__in                                    PIN_ID      PinId,
__deref_out_bcount(*pcbChallengeData)   PBYTE       *ppbChallengeData,
__out                                   PDWORD      pcbChallengeData,
__in                                    DWORD       dwFlags) {
	if (pcbChallengeData != nullptr)
		*pcbChallengeData = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD
WINAPI
CardChangeAuthenticatorEx(
__in                                    PCARD_DATA  pCardData,
__in                                    DWORD       dwFlags,
__in                                    PIN_ID      dwAuthenticatingPinId,
__in_bcount(cbAuthenticatingPinData)    PBYTE       pbAuthenticatingPinData,
__in                                    DWORD       cbAuthenticatingPinData,
__in                                    PIN_ID      dwTargetPinId,
__in_bcount(cbTargetData)               PBYTE       pbTargetData,
__in                                    DWORD       cbTargetData,
__in                                    DWORD       cRetryCount,
__out_opt                               PDWORD      pcAttemptsRemaining) {
	if (pcAttemptsRemaining != nullptr)
		*pcAttemptsRemaining = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}

//
// Function: CardCreateContainerEx
//

DWORD
WINAPI
CardCreateContainerEx(
__in    PCARD_DATA  pCardData,
__in    BYTE        bContainerIndex,
__in    DWORD       dwFlags,
__in    DWORD       dwKeySpec,
__in    DWORD       dwKeySize,
__in    PBYTE       pbKeyData,
__in    PIN_ID      PinId) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}

//
// Function: CardGetChallenge
//

DWORD
WINAPI
#pragma warning(suppress: 6101)
CardGetChallenge(
__in                                    PCARD_DATA  pCardData,
__deref_out_bcount(*pcbChallengeData)   PBYTE       *ppbChallengeData,
__out                                   PDWORD      pcbChallengeData) {
	if (pcbChallengeData != nullptr)
		*pcbChallengeData = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}
//
// Function: CardAuthenticateChallenge
//

DWORD
WINAPI
CardAuthenticateChallenge(
__in                             PCARD_DATA pCardData,
__in_bcount(cbResponseData)      PBYTE      pbResponseData,
__in                             DWORD      cbResponseData,
__out_opt                        PDWORD     pcAttemptsRemaining) {
	if (pcAttemptsRemaining != nullptr)
		*pcAttemptsRemaining = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}
//
// Function: CardChangeAuthenticator
//

DWORD
WINAPI
CardChangeAuthenticator(
__in                                 PCARD_DATA  pCardData,
__in                                 LPWSTR      pwszUserId,
__in_bcount(cbCurrentAuthenticator)  PBYTE       pbCurrentAuthenticator,
__in                                 DWORD       cbCurrentAuthenticator,
__in_bcount(cbNewAuthenticator)      PBYTE       pbNewAuthenticator,
__in                                 DWORD       cbNewAuthenticator,
__in                                 DWORD       cRetryCount,
__in                                 DWORD       dwFlags,
__out_opt                            PDWORD      pcAttemptsRemaining) {
	if (pcAttemptsRemaining != nullptr)
		*pcAttemptsRemaining = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}
//
// Function: CardDeauthenticate
//
// Purpose: De-authenticate the specified logical user name on the card.
//
// This is an optional API.  If implemented, this API is used instead
// of SCARD_RESET_CARD by the Base CSP.  An example scenario is leaving
// a transaction in which the card has been authenticated (a Pin has been
// successfully presented).
//
// The pwszUserId parameter will point to a valid well-known User Name (see
// above).
//
// The dwFlags parameter is currently unused and will always be zero.
//
// Card modules that choose to not implement this API must set the CARD_DATA
// pfnCardDeauthenticate pointer to NULL.
//

DWORD
WINAPI
CardDeauthenticate(
__in    PCARD_DATA  pCardData,
__in    LPWSTR      pwszUserId,
__in    DWORD       dwFlags) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD WINAPI CardDeauthenticateEx(
	__in    PCARD_DATA   pCardData,
	__in    PIN_SET      PinId,
	__in    DWORD        dwFlags) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}

// Directory Control Group
//
// Function: CardCreateDirectory
//
// Purpose: Register the specified application name on the card, and apply the
//          provided access condition.
//
// Return Value:
//          ERROR_FILE_EXISTS - directory already exists
//

DWORD
WINAPI
CardCreateDirectory(
__in    PCARD_DATA                      pCardData,
__in    LPSTR                           pszDirectoryName,
__in    CARD_DIRECTORY_ACCESS_CONDITION AccessCondition) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}
//
// Function: CardDeleteDirectory
//
// Purpose: Unregister the specified application from the card.
//
// Return Value:
//          SCARD_E_DIR_NOT_FOUND - directory does not exist
//          ERROR_DIR_NOT_EMPTY - the directory is not empty
//

DWORD
WINAPI
CardDeleteDirectory(
__in    PCARD_DATA  pCardData,
__in    LPSTR       pszDirectoryName) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}
// File Control Group
//
// Function: CardCreateFile
//

DWORD
WINAPI
CardCreateFile(
__in        PCARD_DATA                  pCardData,
__in_opt    LPSTR                       pszDirectoryName,
__in        LPSTR                       pszFileName,
__in        DWORD                       cbInitialCreationSize,
__in        CARD_FILE_ACCESS_CONDITION  AccessCondition) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}
//
// Function: CardWriteFile
//

DWORD
WINAPI
CardWriteFile(
__in                     PCARD_DATA  pCardData,
__in_opt                 LPSTR       pszDirectoryName,
__in                     LPSTR       pszFileName,
__in                     DWORD       dwFlags,
__in_bcount(cbData)      PBYTE       pbData,
__in                     DWORD       cbData) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}
//
// Function: CardDeleteFile
//

DWORD
WINAPI
CardDeleteFile(
__in        PCARD_DATA  pCardData,
__in_opt    LPSTR       pszDirectoryName,
__in        LPSTR       pszFileName,
__in        DWORD       dwFlags) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}

// CARD_RSA_DECRYPT_INFO_VERSION_ONE is provided for pre-v7 certified
// mini-drivers that do not have logic for on-card padding removal.
//
// Function: CardRSADecrypt
//
// Purpose: Perform a private key decryption on the supplied data.  The
//          card module should assume that pbData is the length of the
//          key modulus.
//

DWORD
WINAPI
CardRSADecrypt(
__in    PCARD_DATA              pCardData,
__inout PCARD_RSA_DECRYPT_INFO  pInfo) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}

//
//
// Type: CARD_DH_AGREEMENT_INFO
//
// CARD_DH_AGREEMENT_INFO version 1 is no longer supported and should
// not be implemented
//

DWORD
WINAPI
CardConstructDHAgreement(
__in    PCARD_DATA pCardData,
__inout PCARD_DH_AGREEMENT_INFO pAgreementInfo) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}
//
// Type: CARD_DERIVE_KEY_INFO
//


DWORD
WINAPI
CardDeriveKey(
__in    PCARD_DATA pCardData,
__inout PCARD_DERIVE_KEY pAgreementInfo) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}
//
// Function:  CardDestroyAgreement
//
// Purpose: Force a deletion of the DH agreed secret.
//

DWORD
WINAPI
CardDestroyDHAgreement(
__in PCARD_DATA pCardData,
__in BYTE       bSecretAgreementIndex,
__in DWORD      dwFlags) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}
//
// Function:  CspGetDHAgreement
//
// Purpose: The CARD_DERIVE_KEY structure contains a list of parameters
// (pParameterList) which might contain a reference to one or more addition
// agreed secrets (KDF_NCRYPT_SECRET_HANDLE).  This callback is provided by
// the caller of CardDeriveKey and will translate the parameter into the
// on card agreed secret handle.
//

DWORD
WINAPI
CspGetDHAgreement(
__in    PCARD_DATA  pCardData,
__in    PVOID       hSecretAgreement,
__out   BYTE*       pbSecretAgreementIndex,
__in    DWORD       dwFlags) {
	*pbSecretAgreementIndex = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}
//
// Memory Management Routines
//
// These routines are supplied to the card module
// by the calling CSP.
//
//
// Function: PFN_CSP_ALLOC
//

//
// Function: PFN_CSP_REALLOC
//

//
// Function: PFN_CSP_FREE
//
// Note: Data allocated for the CSP by the card module must
//       be freed by the CSP.
//

//
// Function: PFN_CSP_CACHE_ADD_FILE
//
// A copy of the pbData parameter is added to the cache.
//

//
// Function: PFN_CSP_CACHE_LOOKUP_FILE
//
// If the cache lookup is successful,
// the caller must free the *ppbData pointer with pfnCspFree.
//

//
// Function: PFN_CSP_CACHE_DELETE_FILE
//
// Deletes the specified item from the cache.
//

//
// Function: PFN_CSP_PAD_DATA
//
// Callback to pad buffer for crypto operation.  Used when
// the card does not provide this.
//

//
// Function: PFN_CSP_UNPAD_DATA
//
// Callback to unpad buffer for crypto operation. Used when
// the card does not provide this.
//

DWORD
WINAPI
CardSetContainerProperty(
__in                    PCARD_DATA  pCardData,
__in                    BYTE        bContainerIndex,
__in                    LPCWSTR     wszProperty,
__in_bcount(cbDataLen)  PBYTE       pbData,
__in                    DWORD       cbDataLen,
__in                    DWORD       dwFlags) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}

// **************************
// Secure key injection flags
// **************************


DWORD
WINAPI
MDImportSessionKey(
__in                    PCARD_DATA          pCardData,
__in                    LPCWSTR             pwszBlobType,
__in                    LPCWSTR             pwszAlgId,
__out                   PCARD_KEY_HANDLE    phKey,
__in_bcount(cbInput)    PBYTE               pbInput,
__in                    DWORD               cbInput) {
	*phKey = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD
WINAPI
#pragma warning(suppress: 6101)
MDEncryptData(
__in                                    PCARD_DATA              pCardData,
__in                                    CARD_KEY_HANDLE         hKey,
__in                                    LPCWSTR                 pwszSecureFunction,
__in_bcount(cbInput)                    PBYTE                   pbInput,
__in                                    DWORD                   cbInput,
__in                                    DWORD                   dwFlags,
__deref_out_ecount(*pcEncryptedData)    PCARD_ENCRYPTED_DATA    *ppEncryptedData,
__out                                   PDWORD                  pcEncryptedData) {
	if (pcEncryptedData != nullptr)
		*pcEncryptedData = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD
WINAPI
#pragma warning(suppress: 6101)
CardGetSharedKeyHandle(
__in                                PCARD_DATA          pCardData,
__in_bcount(cbInput)                PBYTE               pbInput,
__in                                DWORD               cbInput,
__deref_opt_out_bcount(*pcbOutput)  PBYTE               *ppbOutput,
__out_opt                           PDWORD              pcbOutput,
__out                               PCARD_KEY_HANDLE    phKey) {
	if (phKey != nullptr)
		*phKey = 0;
	if (pcbOutput != nullptr)
		*pcbOutput = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD
WINAPI
CardDestroyKey(
__in    PCARD_DATA      pCardData,
__in    CARD_KEY_HANDLE hKey) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD
WINAPI
CardGetAlgorithmProperty(
__in                                        PCARD_DATA  pCardData,
__in                                        LPCWSTR     pwszAlgId,
__in                                        LPCWSTR     pwszProperty,
__out_bcount_part_opt(cbData, *pdwDataLen)  PBYTE       pbData,
__in                                        DWORD       cbData,
__out                                       PDWORD      pdwDataLen,
__in                                        DWORD       dwFlags) {
	*pdwDataLen = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD
WINAPI
CardGetKeyProperty(
__in                                        PCARD_DATA      pCardData,
__in                                        CARD_KEY_HANDLE hKey,
__in                                        LPCWSTR         pwszProperty,
__out_bcount_part_opt(cbData, *pdwDataLen)  PBYTE           pbData,
__in                                        DWORD           cbData,
__out                                       PDWORD          pdwDataLen,
__in                                        DWORD           dwFlags) {
	*pdwDataLen = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD
WINAPI
CardSetKeyProperty(
__in                    PCARD_DATA      pCardData,
__in                    CARD_KEY_HANDLE hKey,
__in                    LPCWSTR         pwszProperty,
__in_bcount(cbInput)    PBYTE           pbInput,
__in                    DWORD           cbInput,
__in                    DWORD           dwFlags) {
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD
WINAPI
CardImportSessionKey(
__in                    PCARD_DATA          pCardData,
__in                    BYTE                bContainerIndex,
__in                    LPVOID              pPaddingInfo,
__in                    LPCWSTR             pwszBlobType,
__in                    LPCWSTR             pwszAlgId,
__out                   PCARD_KEY_HANDLE    phKey,
__in_bcount(cbInput)    PBYTE               pbInput,
__in                    DWORD               cbInput,
__in                    DWORD               dwFlags) {
	*phKey = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD
WINAPI
CardProcessEncryptedData(
__in                                            PCARD_DATA              pCardData,
__in                                            CARD_KEY_HANDLE         hKey,
__in                                            LPCWSTR                 pwszSecureFunction,
__in_ecount(cEncryptedData)                     PCARD_ENCRYPTED_DATA    pEncryptedData,
__in                                            DWORD                   cEncryptedData,
__out_bcount_part_opt(cbOutput, *pdwOutputLen)  PBYTE                   pbOutput,
__in                                            DWORD                   cbOutput,
__out_opt                                       PDWORD                  pdwOutputLen,
__in                                            DWORD                   dwFlags) {
	if (pdwOutputLen != nullptr)
		*pdwOutputLen = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}


DWORD WINAPI CardGetFileInfo(
	__in        PCARD_DATA      pCardData,
	__in_opt    LPSTR           pszDirectoryName,
	__in        LPSTR           pszFileName,
	__inout     PCARD_FILE_INFO pCardFileInfo){
	return SCARD_E_UNSUPPORTED_FEATURE;
}

#pragma warning(suppress: 6101)
#pragma warning(suppress: 6054)
DWORD WINAPI CardEnumFiles(
	__in                                PCARD_DATA  pCardData,
	__in_opt                            LPSTR       pszDirectoryName,
	__deref_out_ecount(*pdwcbFileName)  LPSTR       *pmszFileNames,
	__out                               LPDWORD     pdwcbFileName,
	__in                                DWORD       dwFlags){
	if (pdwcbFileName != nullptr)
		*pdwcbFileName = 0;
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD WINAPI CardQueryFreeSpace(
	__in    PCARD_DATA              pCardData,
	__in    DWORD                   dwFlags,
	__inout PCARD_FREE_SPACE_INFO   pCardFreeSpaceInfo){
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD WINAPI CardQueryCapabilities(
	__in      PCARD_DATA          pCardData,
	__inout   PCARD_CAPABILITIES  pCardCapabilities){
	return SCARD_E_UNSUPPORTED_FEATURE;
}

DWORD WINAPI CardQueryKeySizes(
	__in    PCARD_DATA      pCardData,
	__in    DWORD           dwKeySpec,
	__in    DWORD           dwFlags,
	__inout PCARD_KEY_SIZES pKeySizes){
	return 0;
}

DWORD WINAPI CardSetProperty(
	__in                    PCARD_DATA  pCardData,
	__in                    LPCWSTR     wszProperty,
	__in_bcount(cbDataLen)  PBYTE       pbData,
	__in                    DWORD       cbDataLen,
	__in                    DWORD       dwFlags){
	return 0;
}
