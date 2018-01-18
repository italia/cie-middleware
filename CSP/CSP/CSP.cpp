#include "../stdafx.h"
#include <windows.h>
#include "Cardmod.h"
#include "../PCSC/Token.h"
#include "../util/moduleinfo.h"
#include "CSP.h"
#include "IAS.h"
#include <bcrypt.h>
#include <functional>
#include <fstream>

#define CIE_KEY_BITLEN 2048

#define CIE_CONTAINER_ID 0
#define CIE_CONTAINER_NAME L"CIE"
#define CIE_PIN_ID ROLE_USER
#define CIE_PUK_ID ROLE_ADMIN
#define CIE_SUPPORTED_CYPHER_ALGORITHM L"\0"
#define CIE_SUPPORTED_ASYMMETRIC_ALGORITHM L"RSA\0"

#ifdef _WIN64
	#pragma comment(linker, "/export:CardAcquireContext")
#else
#pragma comment(linker, "/export:CardAcquireContext=_CardAcquireContext@8")
#endif

typedef struct {
	PUBLICKEYSTRUC  publickeystruc;
	RSAPUBKEY rsapubkey;
} PUBKEYSTRUCT_BASE;

int TokenTransmitCallback(PCARD_DATA data, BYTE *apdu, DWORD apduSize, BYTE *resp, DWORD *respSize) {
	if (apduSize == 2) {
		WORD code = *(WORD*)apdu;
		if (code == 0xfffd) {
			*respSize = sizeof(data->hScard)+2;
			memcpy(resp, &data->hScard, sizeof(data->hScard));
			resp[sizeof(data->hScard)] = 0;
			resp[sizeof(data->hScard)+1] = 0;
			return SCARD_S_SUCCESS;
		}
		if (code == 0xfffe) {
			DWORD protocol=0;
			ODS("UNPOWER CARD");
			auto sw = SCardReconnect(data->hScard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_UNPOWER_CARD, &protocol);
			if (sw == SCARD_S_SUCCESS)
				SCardBeginTransaction(data->hScard);
			return 0x9000;
		}
		else if (code == 0xffff) {
			DWORD protocol = 0;
			auto sw = SCardReconnect(data->hScard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_RESET_CARD, &protocol);
			if (sw == SCARD_S_SUCCESS)
				SCardBeginTransaction(data->hScard);
			ODS("RESET CARD");
			return 0x9000;
		}
	}
	ODS(stdPrintf("APDU: %s\n",dumpHexData(ByteArray(apdu, apduSize), std::string()).c_str()).c_str());
	auto sw=SCardTransmit(data->hScard, SCARD_PCI_T1, apdu, apduSize, NULL, resp, respSize);
	if (sw == SCARD_S_SUCCESS) {
		ODS(stdPrintf("RESP: %s\n", dumpHexData(ByteArray(resp, *respSize), std::string()).c_str()).c_str());
	}
	return sw;
}

DWORD WINAPI CardDeleteContext(__inout PCARD_DATA  pCardData) {
	init_CSP_func
	if (pCardData->pvVendorSpecific != nullptr)
		delete (IAS*)pCardData->pvVendorSpecific;
	return SCARD_S_SUCCESS;
	exit_CSP_func
	return E_UNEXPECTED;
}

DWORD WINAPI CardAuthenticatePin(
	__in                   PCARD_DATA   pCardData,
	__in                   LPWSTR       pwszUserId,
	__in_bcount(cbPin)     PBYTE        pbPin,
	__in                   DWORD        cbPin,
	__out_opt              PDWORD       pcAttemptsRemaining) {
	init_CSP_func
	if (pcAttemptsRemaining!=nullptr)
		*pcAttemptsRemaining = 0;
	throw CSP_error(SCARD_E_UNSUPPORTED_FEATURE);
	exit_CSP_func
}

DWORD WINAPI CardReadFile(
	__in                            PCARD_DATA  pCardData,
	__in_opt                        LPSTR       pszDirectoryName,
	__in                            LPSTR       pszFileName,
	__in                            DWORD       dwFlags,
	__deref_out_bcount_opt(*pcbData)    PBYTE       *ppbData,
	__out                           PDWORD      pcbData) {
	init_CSP_func
	*pcbData = 0;
	*ppbData = nullptr;
	if (dwFlags != 0)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);
	ByteDynArray response;
	if (pszDirectoryName == nullptr) {
		if (lstrcmp(pszFileName, szCACHE_FILE) == 0) {
			CARD_CACHE_FILE_FORMAT value;
			memset(&value, 0, sizeof(value));
			response = VarToByteArray(value);
		}
		else
			throw CSP_error(SCARD_E_FILE_NOT_FOUND);
	}
	else if (lstrcmp(pszDirectoryName, DirCIE) == 0) {
		auto ias = ((IAS*)pCardData->pvVendorSpecific);
		if (ias == nullptr)
			throw logged_error("IAS non inizializzato");
		ias->SetCardContext(pCardData);
		if (lstrcmp(pszFileName, EfCertCIE) == 0)
			ias->ReadCertCIE(response);
		else if (lstrcmp(pszFileName, EfSOD) == 0)
			ias->ReadSOD(response);
		else if (lstrcmp(pszFileName, EfIdServizi) == 0)
			ias->ReadIdServizi(response);
		else if (lstrcmp(pszFileName, EfIntAuth) == 0)
			ias->ReadDappPubKey(response);
		else if (lstrcmp(pszFileName, EfIntAuthServizi) == 0)
			ias->ReadServiziPubKey(response);
		else if (lstrcmp(pszFileName, EfSerial) == 0)
			ias->ReadSerialeCIE(response);
		else if (lstrcmp(pszFileName, EfDH) == 0)
			ias->ReadDH(response);
		else
			throw CSP_error(SCARD_E_FILE_NOT_FOUND);
	}
	else if (lstrcmp(pszDirectoryName, szBASE_CSP_DIR) == 0) {
		if (lstrcmp(pszFileName, szCONTAINER_MAP_FILE) == 0) {
			auto ias = ((IAS*)pCardData->pvVendorSpecific);
			if (ias == nullptr)
				throw logged_error("IAS non inizializzato");
			ias->SetCardContext(pCardData);
			ByteDynArray cert;
			ias->GetCertificate(cert, false);
			DWORD keylen = 2048;
			if (!cert.isEmpty()) {
				PCCERT_CONTEXT cer = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, cert.data(), (DWORD)cert.size());
				if (cer == nullptr)
					throw logged_error(stdPrintf("Errore nella lettura del certificato:%08x", GetLastError()));
				auto _1 = scopeExit([&]() noexcept {CertFreeCertificateContext(cer); });

				keylen = CertGetPublicKeyLength(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, &cer->pCertInfo->SubjectPublicKeyInfo);
				if (keylen == 0)
					throw logged_error(stdPrintf("Errore nella lettura della lunghezza della chiave del certificato:%08x", GetLastError()));
			}

			CONTAINER_MAP_RECORD value;
			
			swprintf_s(value.wszGuid, L"%s-%S", CIE_CONTAINER_NAME, dumpHexData(ias->PAN.mid(5, 6), std::string(), false).c_str());
			value.wSigKeySizeBits = (WORD)keylen;
			value.wKeyExchangeKeySizeBits = 0;
			value.bReserved = 0;
			value.bFlags = CONTAINER_MAP_VALID_CONTAINER | CONTAINER_MAP_DEFAULT_CONTAINER;
			response = VarToByteArray(value);
		}
		else if (lstrcmp(pszFileName, "ksc00") == 0) {
			auto ias = ((IAS*)pCardData->pvVendorSpecific);
			if (ias == nullptr)
				throw logged_error("IAS non inizializzato");
			ias->SetCardContext(pCardData);
			ias->GetCertificate(response);
			if (response.isEmpty())
				throw CSP_error(SCARD_E_FILE_NOT_FOUND);
		}
		else
			throw CSP_error(SCARD_E_FILE_NOT_FOUND);
	}
	else
		throw CSP_error(SCARD_E_FILE_NOT_FOUND);
	size_t dataLen = response.size();
	if (pcbData != nullptr && *pcbData != 0)
		dataLen = min(dataLen, *pcbData);
	*ppbData = (PBYTE)pCardData->pfnCspAlloc(dataLen);
	memcpy_s(*ppbData, dataLen, response.data(), dataLen);
	if (pcbData != nullptr)
		*pcbData = (DWORD)dataLen;
	return SCARD_S_SUCCESS;
	exit_CSP_func
	return E_UNEXPECTED;
}

BYTE sha1OID[] = { 0x2B, 0x0E, 0x03, 0x02, 0x1A };
BYTE sha256OID[] = { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01 };
BYTE sha384OID[] = { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02 };
BYTE sha512OID[] = { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03 };

DWORD WINAPI CardSignData(
	__in    PCARD_DATA          pCardData,
	__inout PCARD_SIGNING_INFO  pInfo) {
	init_CSP_func
	if (pInfo->bContainerIndex != CIE_CONTAINER_ID)
		throw CSP_error(SCARD_E_NO_KEY_CONTAINER);
	if (pInfo->dwKeySpec != AT_SIGNATURE)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);

	ByteDynArray resp;
	ByteDynArray toSign;
	ALG_ID alg = 0;
	if (pInfo->dwVersion == CARD_SIGNING_INFO_BASIC_VERSION)
		alg = pInfo->aiHashAlg;
	else if (pInfo->dwVersion == CARD_SIGNING_INFO_CURRENT_VERSION && 
		((pInfo->dwSigningFlags & CARD_PADDING_INFO_PRESENT) == CARD_PADDING_INFO_PRESENT)) {
		if (pInfo->dwPaddingType == CARD_PADDING_PKCS1) {
			auto bPad = (BCRYPT_PKCS1_PADDING_INFO *)pInfo->pPaddingInfo;
			if (bPad->pszAlgId==nullptr)
				alg = 0;
			else if (lstrcmpW(bPad->pszAlgId, L"SHA256") == 0)
				alg = CALG_SHA_256;
			else if (lstrcmpW(bPad->pszAlgId, L"SHA512") == 0)
				alg = CALG_SHA_512;
			else if (lstrcmpW(bPad->pszAlgId, L"SHA384") == 0)
				alg = CALG_SHA_384;
			else if (lstrcmpW(bPad->pszAlgId, L"SHA1") == 0)
				alg = CALG_SHA1;
		}
	}
	else 
		throw CSP_error(SCARD_E_INVALID_PARAMETER);

	toSign= ByteArray(pInfo->pbData, pInfo->cbData);
	switch (alg) {
		case 0:
			// nessun OID da aggiungere
			break;
		case CALG_SHA1:
			toSign = ASN1Tag(0x30, ASN1Tag(0x30, ASN1Tag(6, VarToByteArray(sha1OID)).append(ASN1Tag(5, ByteArray()))).append(ASN1Tag(4,toSign)));
			break;
		case CALG_SHA_256:
			toSign = ASN1Tag(0x30, ASN1Tag(0x30, ASN1Tag(6, VarToByteArray(sha256OID)).append(ASN1Tag(5, ByteArray()))).append(ASN1Tag(4, toSign)));
			break;
		case CALG_SHA_384:
			toSign = ASN1Tag(0x30, ASN1Tag(0x30, ASN1Tag(6, VarToByteArray(sha384OID)).append(ASN1Tag(5, ByteArray()))).append(ASN1Tag(4, toSign)));
			break;
		case CALG_SHA_512:
			toSign = ASN1Tag(0x30, ASN1Tag(0x30, ASN1Tag(6, VarToByteArray(sha512OID)).append(ASN1Tag(5, ByteArray()))).append(ASN1Tag(4, toSign)));
			break;
		default:
			throw CSP_error(SCARD_E_UNSUPPORTED_FEATURE);
	}

	auto ias = ((IAS*)pCardData->pvVendorSpecific);
	if (ias == nullptr)
		throw logged_error("IAS non inizializzato");
	ias->SetCardContext(pCardData);
	ias->Sign(toSign, resp);

	pInfo->pbSignedData = (BYTE*)pCardData->pfnCspAlloc(resp.size());
	pInfo->cbSignedData = (DWORD)resp.size();
	resp.reverse();
	ByteArray(pInfo->pbSignedData, pInfo->cbSignedData).copy(resp);
	return SCARD_S_SUCCESS;
	exit_CSP_func
	return E_UNEXPECTED;
}

bool abilitaSbloccoPIN = true;

#pragma warning(suppress: 6101)
DWORD WINAPI CardAuthenticateEx(
	__in                                    PCARD_DATA  pCardData,
	__in                                    PIN_ID      PinId,
	__in                                    DWORD       dwFlags,
	__in_bcount(cbPinData)                  PBYTE       pbPinData,
	__in                                    DWORD       cbPinData,
	__deref_opt_out_bcount(*pcbSessionPin)  PBYTE       *ppbSessionPin,
	__out_opt                               PDWORD      pcbSessionPin,
	__out_opt                               PDWORD      pcAttemptsRemaining) {
	init_CSP_func
	if (pcbSessionPin != nullptr)
		pcbSessionPin = nullptr;
	if (pcAttemptsRemaining != nullptr)
		*pcAttemptsRemaining = 0;
	if (pbPinData == nullptr)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);
	int expectedLen = 8;
	if (((dwFlags & FULL_PIN) != FULL_PIN) && PinId == ROLE_USER)
		expectedLen = 4;
	
	if (cbPinData != expectedLen) {
		if (pcAttemptsRemaining != nullptr)
			*pcAttemptsRemaining = -1;
		throw CSP_error(SCARD_W_WRONG_CHV);
	}
	if ((dwFlags & CARD_AUTHENTICATE_GENERATE_SESSION_PIN) == CARD_AUTHENTICATE_GENERATE_SESSION_PIN)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);

	auto ias = (IAS*)pCardData->pvVendorSpecific;
	if (ias == nullptr)
		throw logged_error("IAS non inizializzato");
	ias->SetCardContext(pCardData);

	// leggo i parametri di dominio DH e della chiave di extauth
	if (ias->Callback != nullptr)
		ias->Callback(0, "Init", ias->CallbackData);
	ias->InitDHParam();
	ias->InitExtAuthKeyParam();
	// faccio lo scambio di chiavi DH	
	if (ias->Callback != nullptr)
		ias->Callback(1, "DiffieHellman", ias->CallbackData);
	ias->DHKeyExchange();
	// DAPP
	if (ias->Callback != nullptr)
		ias->Callback(2, "DAPP", ias->CallbackData);
	ias->DAPP();
	// verifica PIN
	StatusWord sw;
	if (ias->Callback != nullptr)
		ias->Callback(3, "Verify PIN", ias->CallbackData);
	if (PinId == ROLE_USER) {

		ByteDynArray PIN;
		if ((dwFlags & FULL_PIN) != FULL_PIN)
			ias->GetFirstPIN(PIN);
		PIN.append(ByteArray(pbPinData, cbPinData));
		sw = ias->VerifyPIN(PIN);
	}
	else if (PinId == ROLE_ADMIN) {
		sw = ias->VerifyPUK(ByteArray(pbPinData, cbPinData));
	}
	else
		throw CSP_error(SCARD_E_INVALID_PARAMETER);

	if (sw == 0x6983) {
		if (PinId == ROLE_USER)
			ias->IconaSbloccoPIN();
		throw CSP_error(SCARD_W_CHV_BLOCKED);
	}
	if (sw >= 0x63C0 && sw <= 0x63CF) {
		if (pcAttemptsRemaining!=nullptr)
			*pcAttemptsRemaining = sw - 0x63C0;
		throw CSP_error(SCARD_W_WRONG_CHV);
	}
	if (sw == 0x6700) {
		throw CSP_error(SCARD_W_WRONG_CHV);
	}
	if (sw == 0x6300)
		throw CSP_error(SCARD_W_WRONG_CHV);
	if (sw != 0x9000) {
		throw scard_error(sw);
	}

	return SCARD_S_SUCCESS;
	exit_CSP_func
	return E_UNEXPECTED;
}

void GetContainerInfo(CONTAINER_INFO &value, PCARD_DATA  pCardData) {
	value.dwVersion = CONTAINER_INFO_CURRENT_VERSION;
	value.dwReserved = 0;

	auto ias = (IAS*)pCardData->pvVendorSpecific;
	if (ias == nullptr)
		throw logged_error("IAS non inizializzato");
	ias->SetCardContext(pCardData);
	ByteDynArray cert;
	ias->GetCertificate(cert);

	PCCERT_CONTEXT cer = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, cert.data(), (DWORD)cert.size());
	if (cer == nullptr)
		throw logged_error(stdPrintf("Errore nella lettura del certificato:%08x", GetLastError()));
	auto _1 = scopeExit([&]() noexcept {CertFreeCertificateContext(cer); });

	PCERT_PUBLIC_KEY_INFO pinf = &(cer->pCertInfo->SubjectPublicKeyInfo);
	DWORD PubKeyLen = 0;
	if (!CryptDecodeObject(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, RSA_CSP_PUBLICKEYBLOB, pinf->PublicKey.pbData, pinf->PublicKey.cbData, 0, NULL, &PubKeyLen))
		throw logged_error(stdPrintf("Errore nella decodifica della chiave pubblica:%08x", GetLastError()));
	PUBKEYSTRUCT_BASE* PubKey = (PUBKEYSTRUCT_BASE*)pCardData->pfnCspAlloc(PubKeyLen);
	if (!CryptDecodeObject(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, RSA_CSP_PUBLICKEYBLOB, pinf->PublicKey.pbData, pinf->PublicKey.cbData, 0, PubKey, &PubKeyLen))
		throw logged_error(stdPrintf("Errore nella decodifica della chiave pubblica:%08x", GetLastError()));
	PubKey->publickeystruc.aiKeyAlg = CALG_RSA_SIGN;

	value.cbKeyExPublicKey = 0;
	value.pbKeyExPublicKey = NULL;
	value.cbSigPublicKey = PubKeyLen;
	value.pbSigPublicKey = (PBYTE)PubKey;
	value.dwVersion = CONTAINER_INFO_CURRENT_VERSION;
}

DWORD WINAPI CardGetContainerProperty(
	__in                                        PCARD_DATA  pCardData,
	__in                                        BYTE        bContainerIndex,
	__in                                        LPCWSTR     wszProperty,
	__out_bcount_part_opt(cbData, *pdwDataLen)  PBYTE       pbData,
	__in                                        DWORD       cbData,
	__out                                       PDWORD      pdwDataLen,
	__in                                        DWORD       dwFlags){
	init_CSP_func
	ByteDynArray response;
	*pdwDataLen = 0;
	if (bContainerIndex != 0)
		throw CSP_error(SCARD_E_NO_KEY_CONTAINER);
	if (dwFlags != 0)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);

	if (lstrcmpW(wszProperty, CCP_CONTAINER_INFO) == 0) {
		CONTAINER_INFO value;
		GetContainerInfo(value, pCardData);
		response = VarToByteArray(value);
	}
	else if (lstrcmpW(wszProperty, CCP_PIN_IDENTIFIER) == 0) {
		PIN_ID pin = CIE_PIN_ID;
		response = VarToByteArray(pin);
	}
	else
		throw CSP_error(SCARD_E_INVALID_PARAMETER);
	*pdwDataLen = (DWORD)response.size();
	if (cbData < response.size())
		return ERROR_INSUFFICIENT_BUFFER;
	memcpy_s(pbData, cbData, response.data(), response.size());
	return SCARD_S_SUCCESS;
	exit_CSP_func
	return E_UNEXPECTED;
}

DWORD WINAPI CardGetProperty(
__in                                        PCARD_DATA  pCardData,
__in                                        LPCWSTR     wszProperty,
__out_bcount_part_opt(cbData, *pdwDataLen)  PBYTE       pbData,
__in                                        DWORD       cbData,
__out                                       PDWORD      pdwDataLen,
__in                                        DWORD       dwFlags)
{
	init_CSP_func
	*pdwDataLen = 0;
	ByteDynArray response;
	if (lstrcmpW(wszProperty, CP_CARD_GUID) == 0) {
		if (dwFlags != 0)
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		response = ((IAS*)pCardData->pvVendorSpecific)->PAN;
	}
	else if (lstrcmpW(wszProperty, CP_CARD_READ_ONLY) == 0) {
		if (dwFlags != 0)
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		BOOL trueVal = TRUE;
		response = VarToByteArray(trueVal);
	}
	else if (lstrcmpW(wszProperty, CP_CARD_FREE_SPACE) == 0) {
		if (dwFlags != 0)
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		CARD_FREE_SPACE_INFO cfsVal;
		cfsVal.dwVersion = CARD_FREE_SPACE_INFO_CURRENT_VERSION;
		cfsVal.dwBytesAvailable = 0;
		cfsVal.dwKeyContainersAvailable = 0;
		cfsVal.dwMaxKeyContainers = 1;
		response = VarToByteArray(cfsVal);
	}
	else if (lstrcmpW(wszProperty, CP_CARD_CAPABILITIES) == 0) {
		if (dwFlags != 0)
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		CARD_CAPABILITIES val;
		val.dwVersion = CARD_CAPABILITIES_CURRENT_VERSION;
		val.fCertificateCompression = TRUE;
		val.fKeyGen = FALSE;
		response = VarToByteArray(val);
	}
	else if (lstrcmpW(wszProperty, CP_CARD_KEYSIZES) == 0) {
		if (dwFlags != AT_SIGNATURE)
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		CARD_KEY_SIZES val;
		val.dwVersion = CARD_KEY_SIZES_CURRENT_VERSION;
		val.dwDefaultBitlen = CIE_KEY_BITLEN;
		val.dwIncrementalBitlen = 64;
		val.dwMaximumBitlen = CIE_KEY_BITLEN;
		val.dwMinimumBitlen = 1024;
		response = VarToByteArray(val);
	}
	else if (lstrcmpW(wszProperty, CP_CARD_CACHE_MODE) == 0) {
		if (dwFlags != 0)
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		DWORD val = CP_CACHE_MODE_NO_CACHE;
		response = VarToByteArray(val);
	}
	else if (lstrcmpW(wszProperty, CP_SUPPORTS_WIN_X509_ENROLLMENT) == 0) {
		if (dwFlags != 0)
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		BOOL val = FALSE;
		response = VarToByteArray(val);
	}
	else if (lstrcmpW(wszProperty, CP_CARD_PIN_INFO) == 0) {
		if (dwFlags != CIE_PIN_ID && 
			dwFlags != CIE_PUK_ID) // PIN o PUK
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		PIN_INFO val;
		val.dwVersion = PIN_INFO_CURRENT_VERSION;
		val.PinType = AlphaNumericPinType;
		if (dwFlags == CIE_PIN_ID) {
			val.PinPurpose = AuthenticationPin;
			val.dwChangePermission = 1 << (CIE_PIN_ID);
			val.dwUnblockPermission = 1 << (CIE_PUK_ID);
			val.PinCachePolicy.dwVersion = PIN_CACHE_POLICY_CURRENT_VERSION;
			val.PinCachePolicy.PinCachePolicyType = PinCacheNormal;
			val.PinCachePolicy.dwPinCachePolicyInfo = 0;
		}
		else if (dwFlags == CIE_PUK_ID) {
			val.PinPurpose = UnblockOnlyPin;
			val.dwChangePermission = 0;
			val.dwUnblockPermission = 0;
			val.PinCachePolicy.dwVersion = PIN_CACHE_POLICY_CURRENT_VERSION;
			val.PinCachePolicy.PinCachePolicyType = PinCacheNone;
			val.PinCachePolicy.dwPinCachePolicyInfo = 0;
		}
		else
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		response = VarToByteArray(val);
	}
	else if (lstrcmpW(wszProperty, CP_CARD_LIST_PINS) == 0) {
		if (dwFlags != 0)
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		PIN_SET val = (1 << CIE_PIN_ID) | (1 << CIE_PUK_ID);
		response = VarToByteArray(val);
	}
	else if (lstrcmpW(wszProperty, CP_CARD_PIN_STRENGTH_VERIFY) == 0) {
		if (dwFlags != CIE_PIN_ID &&
			dwFlags != CIE_PUK_ID) // PIN o PUK
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		DWORD val = CARD_PIN_STRENGTH_PLAINTEXT;
		response = VarToByteArray(val);
	}
	else if (lstrcmpW(wszProperty, CP_ENUM_ALGORITHMS) == 0) {
		if (dwFlags == CARD_ASYMMETRIC_OPERATION) {
			response = VarToByteArray(CIE_SUPPORTED_ASYMMETRIC_ALGORITHM);
		}
		else if (dwFlags == CARD_CIPHER_OPERATION) {
			response = VarToByteArray(CIE_SUPPORTED_CYPHER_ALGORITHM);
		}
		else
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
	}
	else if (lstrcmpW(wszProperty, CP_PADDING_SCHEMES) == 0) {
		DWORD val;
		if (dwFlags == CARD_ASYMMETRIC_OPERATION) {
			val = CARD_PADDING_PKCS1;
		}
		else if (dwFlags == CARD_CIPHER_OPERATION) {
			val = 0;
		}
		else
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
	}
	else if (lstrcmpW(wszProperty, CP_CHAINING_MODES) == 0) {
		if (dwFlags != 0)
			throw CSP_error(SCARD_E_INVALID_PARAMETER);
		response = VarToByteArray(L"\0");
	}
	else
		throw CSP_error(SCARD_E_INVALID_PARAMETER);
	*pdwDataLen = (DWORD)response.size();
	if (cbData < response.size())
		return ERROR_INSUFFICIENT_BUFFER;
	memcpy_s(pbData, cbData, response.data(), response.size());
	return SCARD_S_SUCCESS;
	exit_CSP_func
	return E_UNEXPECTED;
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
	init_CSP_func
	if (pcAttemptsRemaining != nullptr)
		*pcAttemptsRemaining = 0;

	if (dwTargetPinId != ROLE_USER)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);

	if (cbTargetData != 4)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);

	if (dwFlags == PIN_CHANGE_FLAG_UNBLOCK) {
		if (dwAuthenticatingPinId != ROLE_ADMIN)
			throw CSP_error(SCARD_E_INVALID_PARAMETER);

		if (cbAuthenticatingPinData != 8)
			throw CSP_error(SCARD_W_WRONG_CHV);

		return CardUnblockPin(pCardData, wszCARD_USER_USER, pbAuthenticatingPinData, cbAuthenticatingPinData, pbTargetData, cbTargetData, 0, CARD_AUTHENTICATE_PIN_PIN);
	}
	if (dwFlags == PIN_CHANGE_FLAG_CHANGEPIN) {
		if (dwAuthenticatingPinId != ROLE_USER)
			throw CSP_error(SCARD_E_INVALID_PARAMETER);

		if (cbAuthenticatingPinData != 4)
			throw CSP_error(SCARD_W_WRONG_CHV);

		auto ias = (IAS*)pCardData->pvVendorSpecific;
		if (ias == nullptr)
			throw logged_error("IAS non inizializzato");
		ias->SetCardContext(pCardData);
		ias->attemptsRemaining = -1;

		// leggo i parametri di dominio DH e della chiave di extauth
		if (ias->Callback != nullptr)
			ias->Callback(0, "Init", ias->CallbackData);
		ias->InitDHParam();
		ias->InitExtAuthKeyParam();
		// faccio lo scambio di chiavi DH	
		if (ias->Callback != nullptr)
			ias->Callback(1, "DiffieHellman", ias->CallbackData);
		ias->DHKeyExchange();
		// DAPP
		if (ias->Callback != nullptr)
			ias->Callback(2, "DAPP", ias->CallbackData);
		ias->DAPP();
		// verifica PIN
		StatusWord sw;
		if (ias->Callback != nullptr)
			ias->Callback(3, "Verify PIN", ias->CallbackData);

		ByteDynArray PIN, veriPIN;
		if (dwAuthenticatingPinId == ROLE_USER) {

			ias->GetFirstPIN(PIN);
			veriPIN = PIN;
			veriPIN.append(ByteArray(pbAuthenticatingPinData, cbAuthenticatingPinData));
			sw = ias->VerifyPIN(veriPIN);
		}

		if (sw >= 0x63C0 && sw <= 0x63CF) {
			ias->attemptsRemaining = sw - 0x63C0;

			if (pcAttemptsRemaining != nullptr)
				*pcAttemptsRemaining = ias->attemptsRemaining;

			throw CSP_error(SCARD_W_WRONG_CHV);
		}
		if (sw == 0x6700) {
			throw CSP_error(SCARD_W_WRONG_CHV);
		}
		if (sw == 0x6983) {
			throw CSP_error(SCARD_W_CHV_BLOCKED);
		}
		if (sw == 0x6300)
			throw CSP_error(SCARD_W_WRONG_CHV);
		if (sw != 0x9000)
			throw scard_error(sw);

		ByteDynArray newPIN = PIN;
		newPIN.append(ByteArray(pbTargetData, cbTargetData));
		sw = ias->ChangePIN(veriPIN, newPIN);
		if (sw != 0x9000)
			throw scard_error(sw);
		return SCARD_S_SUCCESS;
	}
	else
		throw CSP_error(SCARD_E_INVALID_PARAMETER);
	exit_CSP_func
	return E_UNEXPECTED;
}

//
// Function: CardUnblockPin
//

DWORD
WINAPI
CardUnblockPin(
__in                               PCARD_DATA  pCardData,
__in                               LPWSTR      pwszUserId,
__in_bcount(cbAuthenticationData)  PBYTE       pbAuthenticationData,
__in                               DWORD       cbAuthenticationData,
__in_bcount(cbNewPinData)          PBYTE       pbNewPinData,
__in                               DWORD       cbNewPinData,
__in                               DWORD       cRetryCount,
__in                               DWORD       dwFlags) {
	init_CSP_func
	if (dwFlags != CARD_AUTHENTICATE_PIN_PIN)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);
	if (cRetryCount != 0)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);
	if (cbAuthenticationData != 8)
		throw CSP_error(SCARD_W_WRONG_CHV);

	auto ias = (IAS*)pCardData->pvVendorSpecific;
	if (ias == nullptr)
		throw logged_error("IAS non inizializzato");
	bool enrolled = ias->IsEnrolled();

	if (enrolled && cbNewPinData != 4)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);
	if (!enrolled && cbNewPinData != 8)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);

	ias->SetCardContext(pCardData);
	ias->attemptsRemaining = -1;

	// leggo i parametri di dominio DH e della chiave di extauth
	if (ias->Callback != nullptr)
		ias->Callback(0, "Init", ias->CallbackData);
	ias->InitDHParam();
	ias->InitExtAuthKeyParam();
	// faccio lo scambio di chiavi DH	
	if (ias->Callback != nullptr)
		ias->Callback(1, "DiffieHellman", ias->CallbackData);
	ias->DHKeyExchange();
	// DAPP
	if (ias->Callback != nullptr)
		ias->Callback(2, "DAPP", ias->CallbackData);
	ias->DAPP();
	// verifica PIN
	StatusWord sw;
	if (ias->Callback != nullptr)
		ias->Callback(3, "Verify PIN", ias->CallbackData);

	if (StrCmpCW(pwszUserId, wszCARD_USER_USER) == 0) {
		sw = ias->VerifyPUK(ByteArray(pbAuthenticationData, cbAuthenticationData));
	}
	else
		throw CSP_error(SCARD_E_INVALID_PARAMETER);

	if (sw >= 0x63C0 && sw <= 0x63CF) {
		ias->attemptsRemaining = sw - 0x63C0;
		throw CSP_error(SCARD_W_WRONG_CHV);
	}
	if (sw == 0x6700) {
		throw CSP_error(SCARD_W_WRONG_CHV);
	}
	if (sw == 0x6983) {
		throw CSP_error(SCARD_W_CHV_BLOCKED);
	}
	if (sw == 0x6300)
		throw CSP_error(SCARD_W_WRONG_CHV);
	if (sw != 0x9000)
		throw scard_error(sw);

	if ((sw = ias->UnblockPIN()) != 0x9000)
		throw scard_error(sw);
	ByteDynArray changePIN;
	if (ias->IsEnrolled())
		ias->GetFirstPIN(changePIN);
	
	changePIN.append(ByteArray(pbNewPinData, cbNewPinData));
	if ((sw = ias->ChangePIN(changePIN)) != 0x9000)
		throw scard_error(sw);
	return SCARD_S_SUCCESS;
	exit_CSP_func
	return E_UNEXPECTED;
}

DWORD WINAPI CardGetContainerInfo(
__in    PCARD_DATA      pCardData,
__in    BYTE            bContainerIndex,
__in    DWORD           dwFlags,
__inout PCONTAINER_INFO pContainerInfo){
	init_CSP_func
	if (dwFlags != 0)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);
	if (bContainerIndex != CIE_CONTAINER_ID)
		return SCARD_E_NO_KEY_CONTAINER;
	GetContainerInfo(*pContainerInfo, pCardData);
	return SCARD_S_SUCCESS;
	exit_CSP_func
	return E_UNEXPECTED;
}

#include "../crypto/Base64.h"

extern "C" DWORD WINAPI CardAcquireContext(
	IN      PCARD_DATA pCardData,
	__in    DWORD      dwFlags
	)

{
	init_CSP_func		

	if ((dwFlags & CARD_SECURE_KEY_INJECTION_NO_CARD_MODE) == CARD_SECURE_KEY_INJECTION_NO_CARD_MODE)
		return SCARD_E_UNSUPPORTED_FEATURE;
	if (pCardData->pbAtr == nullptr ||
		pCardData->pwszCardName == nullptr ||
		pCardData->pfnCspAlloc == nullptr ||
		pCardData->pfnCspReAlloc == nullptr ||
		pCardData->pfnCspFree == nullptr ||
		pCardData->hSCardCtx == NULL || 
		pCardData->hScard == NULL)
		throw CSP_error(SCARD_E_INVALID_PARAMETER);

	auto ias = std::unique_ptr<IAS>(new IAS((CToken::TokenTransmitCallback)TokenTransmitCallback, ByteArray(pCardData->pbAtr, pCardData->cbAtr)));
	ias->SetCardContext(pCardData);


	CModuleInfo info;
	info.init(info.getApplicationModule());
	auto pid = GetCurrentProcessId();
	OutputDebugString(stdPrintf("Process: %i %08x %s", pid, pid, info.szModuleName.c_str()).c_str());

	ByteDynArray data;
	ias->ReadPAN();

	ByteDynArray resp;
	ias->InitEncKey();

	//if ((dwFlags & USE_NON_ENROLLED_CIE) != USE_NON_ENROLLED_CIE) {
	//	ByteDynArray cert;
	//	ias->GetCertificate(cert, true);
	//	if (cert.isEmpty())
	//		throw logged_error("CIE non abilitata");
	//}

	ias->ReadDappPubKey(resp);

	pCardData->dwVersion =
		min(pCardData->dwVersion, 7);
	pCardData->pfnCardReadFile = CardReadFile;
	pCardData->pfnCardDeleteContext = CardDeleteContext;
	pCardData->pfnCardAuthenticatePin = CardAuthenticatePin;
	pCardData->pfnCardGetFileInfo = CardGetFileInfo;
	pCardData->pfnCardEnumFiles = CardEnumFiles;
	pCardData->pfnCardQueryFreeSpace = CardQueryFreeSpace;
	pCardData->pfnCardQueryCapabilities = CardQueryCapabilities;
	pCardData->pfnCardSignData = CardSignData;
	pCardData->pfnCardQueryKeySizes = CardQueryKeySizes;
	pCardData->pfnCardAuthenticateEx = CardAuthenticateEx;
	pCardData->pfnCardDeauthenticateEx = CardDeauthenticateEx;
	pCardData->pfnCardGetContainerProperty = CardGetContainerProperty;
	pCardData->pfnCardGetContainerInfo = CardGetContainerInfo;
	pCardData->pfnCardGetProperty = CardGetProperty;
	pCardData->pfnCardSetProperty = CardSetProperty;

	pCardData->pfnCardDeleteContainer = CardDeleteContainer;
	pCardData->pfnCardCreateContainer = CardCreateContainer;
	pCardData->pfnCardAuthenticatePin = CardAuthenticatePin;
	pCardData->pfnCardGetChallenge = CardGetChallenge;
	pCardData->pfnCardAuthenticateChallenge = CardAuthenticateChallenge;
	pCardData->pfnCardUnblockPin = CardUnblockPin;
	pCardData->pfnCardChangeAuthenticator = CardChangeAuthenticator;
	pCardData->pfnCardDeauthenticate = CardDeauthenticate;
	pCardData->pfnCardCreateDirectory = CardCreateDirectory;
	pCardData->pfnCardDeleteDirectory = CardDeleteDirectory;
	pCardData->pfnCardCreateFile = CardCreateFile;
	pCardData->pfnCardWriteFile = CardWriteFile;
	pCardData->pfnCardDeleteFile = CardDeleteFile;
	pCardData->pfnCardRSADecrypt = CardRSADecrypt;
	pCardData->pfnCardConstructDHAgreement = CardConstructDHAgreement;

	pCardData->pfnCardDeriveKey = CardDeriveKey;
	pCardData->pfnCardDestroyDHAgreement = CardDestroyDHAgreement;
	pCardData->pfnCspGetDHAgreement = CspGetDHAgreement;

	pCardData->pfnCardGetChallengeEx = CardGetChallengeEx;
	pCardData->pfnCardChangeAuthenticatorEx = CardChangeAuthenticatorEx;
	pCardData->pfnCardSetContainerProperty = CardSetContainerProperty;

	pCardData->pfnMDImportSessionKey = MDImportSessionKey;
	pCardData->pfnMDEncryptData = MDEncryptData;
	pCardData->pfnCardImportSessionKey = CardImportSessionKey;
	pCardData->pfnCardGetSharedKeyHandle = CardGetSharedKeyHandle;
	pCardData->pfnCardGetAlgorithmProperty = CardGetAlgorithmProperty;
	pCardData->pfnCardGetKeyProperty = CardGetKeyProperty;
	pCardData->pfnCardSetKeyProperty = CardSetKeyProperty;
	pCardData->pfnCardDestroyKey = CardDestroyKey;
	pCardData->pfnCardProcessEncryptedData = CardProcessEncryptedData;
	pCardData->pfnCardCreateContainerEx = CardCreateContainerEx;
	pCardData->pvVendorSpecific = ias.release();


	return SCARD_S_SUCCESS;
	exit_CSP_func
	return E_UNEXPECTED;
}

