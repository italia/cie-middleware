#include "../StdAfx.h"
#include ".\mechanism.h"
#include "..\crypto\rsa.h"
#include "p11object.h"
#include "../util/util.h"

static char *szCompiledFile=__FILE__;

static BYTE SHA1_RSAcode[]={0x30,0x21,0x30,0x09,0x06,0x05,0x2b,0x0e,0x03,0x02,0x1a,0x05,0x00,0x04,0x14};
static BYTE MD5_RSAcode[]={0x30,0x20,0x30,0x0C,0x06,0x08,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x02,0x05,0x05,0x00,0x04,0x10};
static ByteArray baSHA1DigestInfo(SHA1_RSAcode,sizeof(SHA1_RSAcode));
static ByteArray baMD5DigestInfo(MD5_RSAcode,sizeof(MD5_RSAcode));

namespace p11 {

CMechanism::CMechanism() {}
CMechanism::CMechanism(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session): mtType(type), pSession(std::move(Session)) {}
CMechanism::~CMechanism() {}
RESULT CDecrypt::checkCache(ByteArray &Data,ByteArray &Result,bool &bFound)
{
	init_func
	bFound=false;
	if (Data.lock()==cacheData.lock() && Data.size()==cacheData.size()) {
		if (!Result.isNull()) {
			if (Result.size()<resultCache.size())
				_return(CKR_BUFFER_TOO_SMALL)
		
			Result.copy(resultCache);
			Result.dwSize=resultCache.size();
			bFound=true;
		}
	}
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CDecrypt::setCache(ByteArray &Data,ByteArray &Result)
{
	init_func
	cacheData=Data;
	resultCache.alloc_copy(Result);
	_return(OK)
	exit_func
	_return(FAIL)
}

CVerify::CVerify() {}
CVerify::CVerify(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CMechanism(type,std::move(Session)) {}
CVerify::~CVerify() {}

CVerifyRecover::CVerifyRecover() {}
CVerifyRecover::CVerifyRecover(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CMechanism(type,std::move(Session)) {}
CVerifyRecover::~CVerifyRecover() {}

CDigest::CDigest() {}
CDigest::CDigest(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CMechanism(type,std::move(Session)) {}
CDigest::~CDigest() {}

CSign::CSign() {}
CSign::CSign(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CMechanism(type,std::move(Session)) {}
CSign::~CSign() {}

CSignRecover::CSignRecover() {}
CSignRecover::CSignRecover(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CMechanism(type,std::move(Session)) {}
CSignRecover::~CSignRecover() {}

CEncrypt::CEncrypt() {}
CEncrypt::CEncrypt(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CMechanism(type,std::move(Session)) {}
CEncrypt::~CEncrypt() {}

CDecrypt::CDecrypt() {}
CDecrypt::CDecrypt(CK_MECHANISM_TYPE type,CSession *Session) : CMechanism(type,Session) {
	cacheData.pbtData=(BYTE*)~(ULONG_PTR)0;
}
CDecrypt::~CDecrypt() {}

/* ******************** */
/*		   SHA1	        */
/* ******************** */
CSHA::CSHA(std::shared_ptr<CSession> Session) : CDigest (CKM_SHA_1,std::move(Session)) {}
CSHA::~CSHA() {}

RESULT CSHA::DigestInit() {
	init_func
	SHA1_Init(&Sha1Context);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSHA::DigestUpdate(ByteArray &Part) {
	init_func
	SHA1_Update(&Sha1Context,Part.lock(),Part.size());
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSHA::DigestFinal(ByteArray &Digest) {
	init_func
	SHA1_Final(Digest.lock(),&Sha1Context);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSHA::DigestLength(CK_ULONG_PTR pulDigestLen) {
	init_func
	*pulDigestLen=SHA_DIGEST_LENGTH;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSHA::DigestInfo(ByteArray *&pbaDigestInfo) {
	init_func
	pbaDigestInfo=&baSHA1DigestInfo;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSHA::DigestGetOperationState(ByteDynArray &OperationState)
{
	init_func
	OperationState.resize(sizeof(Sha1Context));
	OperationState.copy((BYTE*)&Sha1Context,sizeof(Sha1Context));
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSHA::DigestSetOperationState(ByteArray &OperationState)
{
	init_func
	if (OperationState.size()!=sizeof(Sha1Context))
		_return(CKR_SAVED_STATE_INVALID)
		memcpy_s(&Sha1Context, sizeof(SHA_CTX), OperationState.lock(), sizeof(Sha1Context));
	_return(OK)
	exit_func
	_return(FAIL)
}

/* ******************** */
/*		   MD5	        */
/* ******************** */
CMD5::CMD5(std::shared_ptr<CSession> Session) : CDigest (CKM_MD5,std::move(Session)) {}
CMD5::~CMD5() {}

RESULT CMD5::DigestInit() {
	init_func
	MD5_Init(&MD5Context);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CMD5::DigestUpdate(ByteArray &Part) {
	init_func
	MD5_Update(&MD5Context,Part.lock(),Part.size());
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CMD5::DigestFinal(ByteArray &Digest) {
	init_func
	MD5_Final(Digest.lock(),&MD5Context);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CMD5::DigestLength(CK_ULONG_PTR pulDigestLen) {
	init_func
	*pulDigestLen=MD5_DIGEST_LENGTH;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CMD5::DigestInfo(ByteArray *&pbaDigestInfo) {
	init_func
	pbaDigestInfo=&baMD5DigestInfo;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CMD5::DigestGetOperationState(ByteDynArray &OperationState)
{
	init_func
	OperationState.resize(sizeof(MD5Context));
	OperationState.copy((BYTE*)&MD5Context,sizeof(MD5Context));
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CMD5::DigestSetOperationState(ByteArray &OperationState)
{
	init_func
	if (OperationState.size()!=sizeof(MD5Context))
		_return(CKR_SAVED_STATE_INVALID)
	memcpy_s(&MD5Context,sizeof(MD5_CTX),OperationState.lock(),sizeof(MD5Context));
	_return(OK)
	exit_func
	_return(FAIL)
}

/* ******************** */
/*		Verify RSA		*/
/* ******************** */
CVerifyRSA::CVerifyRSA() {}
CVerifyRSA::CVerifyRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CVerify(type,std::move(Session)) {}
CVerifyRSA::~CVerifyRSA() {}

RESULT CVerifyRSA::VerifySupportMultipart(bool &Support) {
	init_func
	Support=false;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRSA::VerifyLength(CK_ULONG_PTR pulVerifyLength)
{
	init_func

	std::shared_ptr<CP11Object> pObject;
	P11ER_CALL(pSession->pSlot->GetObjectFromID(hVerifyKey,pObject),
		ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject!=NULL,ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject->ObjClass==CKO_PUBLIC_KEY,ERR_WRONG_OBJECT_TYPE)
	auto pPublicKey=std::static_pointer_cast<CP11PublicKey>(pObject);

	ByteArray *baKeyModule;
	P11ER_CALL(pPublicKey->getAttribute(CKA_MODULUS,baKeyModule),
		ERR_CANT_GET_PUBKEY_MODULUS)
	ER_ASSERT(baKeyModule!=NULL,ERR_CANT_GET_PUBKEY_MODULUS)
	*pulVerifyLength=baKeyModule->size();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRSA::VerifyDecryptSignature(ByteArray &Signature,ByteDynArray &baPlainSignature)
{
	init_func
	ByteArray *baKeyExponent=NULL,*baKeyModule=NULL;

	std::shared_ptr<CP11Object> pObject;
	P11ER_CALL(pSession->pSlot->GetObjectFromID(hVerifyKey,pObject),
		ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject!=NULL,ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject->ObjClass==CKO_PUBLIC_KEY,ERR_WRONG_OBJECT_TYPE)
	auto pPublicKey=std::static_pointer_cast<CP11PublicKey>(pObject);

	P11ER_CALL(pPublicKey->getAttribute(CKA_PUBLIC_EXPONENT,baKeyExponent),
		ERR_CANT_GET_PUBKEY_EXPONENT)
	ER_ASSERT(baKeyExponent!=NULL,ERR_CANT_GET_PUBKEY_EXPONENT)

	P11ER_CALL(pPublicKey->getAttribute(CKA_MODULUS,baKeyModule),
		ERR_CANT_GET_PUBKEY_MODULUS)
	ER_ASSERT(baKeyModule!=NULL,ERR_CANT_GET_PUBKEY_MODULUS)

	DWORD dwKeyLenBytes=baKeyModule->size();

	if (Signature.size()!=dwKeyLenBytes)
		_return(CKR_SIGNATURE_LEN_RANGE)

	CRSA rsa(*baKeyModule,*baKeyExponent);
	baPlainSignature.resize(Signature.size());
	P11ER_CALL(rsa.RSA_PURE(Signature,baPlainSignature),
		ERR_CRYPTO_ERROR)
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRSA::VerifyGetOperationState(ByteDynArray &OperationState)
{
	init_func
	OperationState.clear();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRSA::VerifySetOperationState(ByteArray &OperationState)
{
	init_func
	if (OperationState.size()!=0)
		_return(CKR_SAVED_STATE_INVALID)
	_return(OK)
	exit_func
	_return(FAIL)
}

/* ******************** */
/*	VerifyRecover RSA	*/
/* ******************** */
CVerifyRecoverRSA::CVerifyRecoverRSA() {}
CVerifyRecoverRSA::CVerifyRecoverRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CVerifyRecover(type,std::move(Session)) {}
CVerifyRecoverRSA::~CVerifyRecoverRSA() {}

RESULT CVerifyRecoverRSA::VerifyRecoverLength(CK_ULONG_PTR pulVerifyRecoverLength)
{
	init_func

	std::shared_ptr<CP11Object> pObject;
	P11ER_CALL(pSession->pSlot->GetObjectFromID(hVerifyRecoverKey,pObject),
		ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject!=NULL,ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject->ObjClass==CKO_PUBLIC_KEY,ERR_WRONG_OBJECT_TYPE)
	auto pPublicKey=std::static_pointer_cast<CP11PublicKey>(pObject);

	ByteArray *baKeyModule;
	P11ER_CALL(pPublicKey->getAttribute(CKA_MODULUS,baKeyModule),
		ERR_CANT_GET_PUBKEY_MODULUS)
	ER_ASSERT(baKeyModule!=NULL,ERR_CANT_GET_PUBKEY_MODULUS)
	*pulVerifyRecoverLength=baKeyModule->size();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRecoverRSA::VerifyRecoverDecryptSignature(ByteArray &Signature,ByteDynArray &baPlainSignature)
{
	init_func
	ByteArray *baKeyExponent=NULL,*baKeyModule=NULL;

	std::shared_ptr<CP11Object> pObject;
	P11ER_CALL(pSession->pSlot->GetObjectFromID(hVerifyRecoverKey,pObject),
		ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject!=NULL,ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject->ObjClass==CKO_PUBLIC_KEY,ERR_WRONG_OBJECT_TYPE)
	auto pPublicKey=std::static_pointer_cast<CP11PublicKey>(pObject);

	P11ER_CALL(pPublicKey->getAttribute(CKA_PUBLIC_EXPONENT,baKeyExponent),
		ERR_CANT_GET_PUBKEY_EXPONENT)
	ER_ASSERT(baKeyExponent!=NULL,ERR_CANT_GET_PUBKEY_EXPONENT)

	P11ER_CALL(pPublicKey->getAttribute(CKA_MODULUS,baKeyModule),
		ERR_CANT_GET_PUBKEY_MODULUS)
	ER_ASSERT(baKeyModule!=NULL,ERR_CANT_GET_PUBKEY_MODULUS)

	DWORD dwKeyLenBytes=baKeyModule->size();

	if (Signature.size()!=dwKeyLenBytes)
		_return(CKR_SIGNATURE_LEN_RANGE)

	CRSA rsa(*baKeyModule,*baKeyExponent);
	baPlainSignature.resize(Signature.size());
	P11ER_CALL(rsa.RSA_PURE(Signature,baPlainSignature),
		ERR_CRYPTO_ERROR)
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRecoverRSA::VerifyRecoverGetOperationState(ByteDynArray &OperationState)
{
	init_func
	OperationState.clear();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRecoverRSA::VerifyRecoverSetOperationState(ByteArray &OperationState)
{
	init_func
	if (OperationState.size()!=0)
		_return(CKR_SAVED_STATE_INVALID)
	_return(OK)
	exit_func
	_return(FAIL)
}

/* ******************** */
/*		SignRSA			*/
/* ******************** */
CSignRSA::CSignRSA() {}
CSignRSA::CSignRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CSign(type,std::move(Session)) {}
CSignRSA::~CSignRSA() {}

RESULT CSignRSA::SignSupportMultipart(bool &Support) {
	init_func
	Support=false;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSignRSA::SignLength(CK_ULONG_PTR pulSignatureLen) {
	init_func
	std::shared_ptr<CP11Object> pObject;
	P11ER_CALL(pSession->pSlot->GetObjectFromID(hSignKey,pObject),
		ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject!=NULL,ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject->ObjClass==CKO_PRIVATE_KEY,ERR_WRONG_OBJECT_TYPE)
	auto pPrivateKey=std::static_pointer_cast<CP11PrivateKey>(pObject);

	ByteArray *baKeyModule;
	P11ER_CALL(pPrivateKey->getAttribute(CKA_MODULUS,baKeyModule),
		ERR_CANT_GET_PUBKEY_MODULUS)
	ER_ASSERT(baKeyModule!=NULL,ERR_CANT_GET_PUBKEY_MODULUS)
	*pulSignatureLen=baKeyModule->size();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSignRSA::SignGetOperationState(ByteDynArray &OperationState)
{
	init_func
	OperationState.clear();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSignRSA::SignSetOperationState(ByteArray &OperationState)
{
	init_func
	if (OperationState.size()!=0)
		_return(CKR_SAVED_STATE_INVALID)
	_return(OK)
	exit_func
	_return(FAIL)
}

/* ******************** */
/*	SignRecoverRSA		*/
/* ******************** */
CSignRecoverRSA::CSignRecoverRSA() {}
CSignRecoverRSA::CSignRecoverRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CSignRecover(type,std::move(Session)) {}
CSignRecoverRSA::~CSignRecoverRSA() {}

RESULT CSignRecoverRSA::SignRecoverLength(CK_ULONG_PTR pulSignRecoveratureLen) {
	init_func
	std::shared_ptr<CP11Object> pObject;
	P11ER_CALL(pSession->pSlot->GetObjectFromID(hSignRecoverKey,pObject),
		ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject!=NULL,ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject->ObjClass==CKO_PRIVATE_KEY,ERR_WRONG_OBJECT_TYPE)
	auto pPrivateKey=std::static_pointer_cast<CP11PrivateKey>(pObject);

	ByteArray *baKeyModule;
	P11ER_CALL(pPrivateKey->getAttribute(CKA_MODULUS,baKeyModule),
		ERR_CANT_GET_PUBKEY_MODULUS)
	ER_ASSERT(baKeyModule!=NULL,ERR_CANT_GET_PUBKEY_MODULUS)
	*pulSignRecoveratureLen=baKeyModule->size();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSignRecoverRSA::SignRecoverGetOperationState(ByteDynArray &OperationState)
{
	init_func
	OperationState.clear();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSignRecoverRSA::SignRecoverSetOperationState(ByteArray &OperationState)
{
	init_func
	if (OperationState.size()!=0)
		_return(CKR_SAVED_STATE_INVALID)
	_return(OK)
	exit_func
	_return(FAIL)
}

/* ******************** */
/*		RSA_X509		*/
/* ******************** */
CRSA_X509::CRSA_X509(std::shared_ptr<CSession> Session) :	CSignRSA (CKM_RSA_X_509,Session), 
															CSignRecoverRSA (CKM_RSA_X_509,Session), 
															CVerifyRSA (CKM_RSA_X_509,Session), 
															CVerifyRecoverRSA (CKM_RSA_X_509,Session), 
															CEncryptRSA (CKM_RSA_X_509,Session), 
															CDecryptRSA (CKM_RSA_X_509,Session) {}
CRSA_X509::~CRSA_X509() {}

RESULT CRSA_X509::VerifyInit(CK_OBJECT_HANDLE PublicKey) {
	init_func
	hVerifyKey=PublicKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::VerifyUpdate(ByteArray &Part) {
	init_func
	DWORD dwSize=baVerifyBuffer.size();
	baVerifyBuffer.resize(dwSize+Part.size(),true);
	baVerifyBuffer.mid(dwSize,Part.size()).copy(Part);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::VerifyFinal(ByteArray &Signature)
{
	init_func
	ByteDynArray baPlainSignature;
	CK_ULONG ulVerifyLength=0;
	P11ER_CALL(VerifyLength(&ulVerifyLength),
		ERR_CANT_GET_KEY_LENGTH)

	if (Signature.size()!=ulVerifyLength)
		_return(CKR_SIGNATURE_LEN_RANGE)

	// il buffer da verificare può anche essere
	// più corto della chiave, viene paddato automaticamente
	// specifiche P11
	if (baVerifyBuffer.size()>ulVerifyLength) 
		_return(CKR_DATA_LEN_RANGE)

	P11ER_CALL(VerifyDecryptSignature(Signature,baPlainSignature),
		ERR_CANT_DECRYPT_SIGNATURE)

	ByteDynArray baExpectedResult(ulVerifyLength);
	baExpectedResult.rightcopy(baVerifyBuffer);
	PutPaddingBT0(baExpectedResult, baVerifyBuffer.size());

	if (baPlainSignature==baExpectedResult)
		_return(OK)
	else
		_return(CKR_SIGNATURE_INVALID)

	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::VerifyRecoverInit(CK_OBJECT_HANDLE PublicKey) {
	init_func
	hVerifyRecoverKey=PublicKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::VerifyRecover(ByteArray &Signature,ByteDynArray &Data)
{
	init_func
	CK_ULONG ulVerifyRecoverLength=0;
	P11ER_CALL(VerifyRecoverLength(&ulVerifyRecoverLength),
		ERR_CANT_GET_KEY_LENGTH)

	if (Signature.size()!=ulVerifyRecoverLength)
		_return(CKR_SIGNATURE_LEN_RANGE)

	P11ER_CALL(VerifyRecoverDecryptSignature(Signature,Data),
		ERR_CANT_DECRYPT_SIGNATURE)

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::SignInit(CK_OBJECT_HANDLE PrivateKey)
{
	init_func
	hSignKey=PrivateKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::SignReset()
{
	init_func
	baSignBuffer.clear();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::SignUpdate(ByteArray &Part) {
	init_func
	DWORD dwSize=baSignBuffer.size();
	baSignBuffer.resize(dwSize+Part.size(),true);
	baSignBuffer.mid(dwSize,Part.size()).copy(Part);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::SignFinal(ByteDynArray &SignBuffer)
{
	init_func
	CK_ULONG ulSignatureLength=0;
	P11ER_CALL(SignLength(&ulSignatureLength),
		ERR_CANT_GET_KEY_LENGTH)

	if (baSignBuffer.size()>ulSignatureLength)
		_return(CKR_DATA_LEN_RANGE)

	SignBuffer.alloc_copy(baSignBuffer);

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::SignRecoverInit(CK_OBJECT_HANDLE PrivateKey) {
	init_func
	hSignRecoverKey=PrivateKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::SignRecover(ByteArray &Data,ByteDynArray &SignBuffer) {
	init_func

	CK_ULONG ulSignatureLength=0;
	P11ER_CALL(SignRecoverLength(&ulSignatureLength),
		ERR_CANT_GET_KEY_LENGTH)

	if (Data.size()>ulSignatureLength)
		_return(CKR_DATA_LEN_RANGE)

	SignBuffer.alloc_copy(Data);

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::EncryptInit(CK_OBJECT_HANDLE PublicKey) {
	init_func
	hEncryptKey=PublicKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::EncryptUpdate(ByteArray &Part,ByteDynArray &EncryptedData) {
	init_func
	DWORD dwSize=baEncryptBuffer.size();
	baEncryptBuffer.resize(dwSize+Part.size(),true);
	baEncryptBuffer.mid(dwSize,Part.size()).copy(Part);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::EncryptFinal(ByteDynArray &baEncryptedData)
{
	init_func
	CK_ULONG ulEncryptLength=0;
	P11ER_CALL(EncryptLength(&ulEncryptLength),
		ERR_CANT_GET_KEY_LENGTH)

	if (baEncryptBuffer.size()>ulEncryptLength) 
		_return(CKR_DATA_LEN_RANGE)

	ByteDynArray baPlainData(ulEncryptLength);
	baPlainData.rightcopy(baEncryptBuffer);
	PutPaddingBT0(baPlainData, baEncryptBuffer.size());
	
	P11ER_CALL(EncryptCompute(baPlainData,baEncryptedData),
		ERR_CANT_ENCRYPT_DATA)

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::DecryptInit(CK_OBJECT_HANDLE PrivateKey)
{
	init_func
	hDecryptKey=PrivateKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::DecryptUpdate(ByteArray &Part,ByteDynArray &Data) {
	init_func
	DWORD dwSize=baDecryptBuffer.size();
	baDecryptBuffer.resize(dwSize+Part.size(),true);
	baDecryptBuffer.mid(dwSize,Part.size()).copy(Part);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::DecryptFinal(ByteDynArray &DecryptBuffer)
{
	init_func
	CK_ULONG ulDecryptLength=0;
	P11ER_CALL(DecryptLength(&ulDecryptLength),
		ERR_CANT_GET_KEY_LENGTH)

	// nella decrypt il buffer deve essere lungo esasttamente k
	// (specifiche P11)
	if (baDecryptBuffer.size()!=ulDecryptLength)
		_return(CKR_ENCRYPTED_DATA_LEN_RANGE)

	DecryptBuffer.alloc_copy(baDecryptBuffer);

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_X509::DecryptRemovePadding(ByteArray &paddedData,ByteDynArray &unpaddedData)
{
	init_func
		// non faccio nulla perchè l'RSA_X509 non leva il padding
	unpaddedData.alloc_copy(paddedData);
	_return(OK)
	exit_func
	_return(FAIL)
}
/* ******************** */
/*		RSA_PKCS1		*/
/* ******************** */
CRSA_PKCS1::CRSA_PKCS1(std::shared_ptr<CSession> Session) :	CSignRSA (CKM_RSA_PKCS,Session),
															CSignRecoverRSA (CKM_RSA_PKCS,Session),
															CVerifyRSA (CKM_RSA_PKCS,Session),
															CVerifyRecoverRSA (CKM_RSA_PKCS,Session),
															CEncryptRSA (CKM_RSA_PKCS,Session),
															CDecryptRSA (CKM_RSA_PKCS,Session) {}
CRSA_PKCS1::~CRSA_PKCS1() {}

RESULT CRSA_PKCS1::VerifyInit(CK_OBJECT_HANDLE PublicKey) {
	init_func
	hVerifyKey=PublicKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::VerifyUpdate(ByteArray &Part) {
	init_func
	DWORD dwSize=baVerifyBuffer.size();
	baVerifyBuffer.resize(dwSize+Part.size(),true);
	baVerifyBuffer.mid(dwSize,Part.size()).copy(Part);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::VerifyFinal(ByteArray &Signature)
{
	init_func
	ByteDynArray baPlainSignature;
	CK_ULONG ulVerifyLength=0;
	P11ER_CALL(VerifyLength(&ulVerifyLength),
		ERR_CANT_GET_KEY_LENGTH)

	if (Signature.size()!=ulVerifyLength)
		_return(CKR_SIGNATURE_LEN_RANGE)

	// max k-11 (specifiche p11)
	if (baVerifyBuffer.size()>ulVerifyLength-11) 
		_return(CKR_DATA_LEN_RANGE)

	P11ER_CALL(VerifyDecryptSignature(Signature,baPlainSignature),
		ERR_CANT_DECRYPT_SIGNATURE)

	ByteDynArray baExpectedResult(ulVerifyLength);
	baExpectedResult.rightcopy(baVerifyBuffer);
	PutPaddingBT1(baExpectedResult, baVerifyBuffer.size());

	if (baPlainSignature==baExpectedResult)
		_return(OK)
	else
		_return(CKR_SIGNATURE_INVALID)

	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::VerifyRecoverInit(CK_OBJECT_HANDLE PublicKey) {
	init_func
	hVerifyRecoverKey=PublicKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::VerifyRecover(ByteArray &Signature,ByteDynArray &Data)
{
	init_func
	CK_ULONG ulVerifyRecoverLength=0;
	P11ER_CALL(VerifyRecoverLength(&ulVerifyRecoverLength),
		ERR_CANT_GET_KEY_LENGTH)

	if (Signature.size()!=ulVerifyRecoverLength)
		_return(CKR_SIGNATURE_LEN_RANGE)

	ByteDynArray baPlainSignature;
	P11ER_CALL(VerifyRecoverDecryptSignature(Signature, baPlainSignature),
		ERR_CANT_DECRYPT_SIGNATURE);

	// se non posso levare il padding, la firma ha
	// qualcosa di sbagliato
	DWORD dwPadLen;
	try {
		dwPadLen=RemovePaddingBT1(baPlainSignature);
	}
	catch (...) {
		_return(CKR_SIGNATURE_INVALID)
	}

	// i dati restutuiti non possono essere più
	// lunghi di k-11!! (specifiche p11)
	Data.alloc_copy(baPlainSignature.mid(dwPadLen));
	if (Data.size()>ulVerifyRecoverLength-11)
		_return(CKR_DATA_LEN_RANGE)

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::SignInit(CK_OBJECT_HANDLE PrivateKey)
{
	init_func    
	hSignKey=PrivateKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::SignReset()
{
	init_func
	baSignBuffer.clear();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::SignUpdate(ByteArray &Part) {
	init_func
	DWORD dwSize=baSignBuffer.size();
	baSignBuffer.resize(dwSize+Part.size(),true);
	baSignBuffer.mid(dwSize,Part.size()).copy(Part);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::SignFinal(ByteDynArray &SignBuffer)
{
	init_func
	CK_ULONG ulSignatureLength=0;
	P11ER_CALL(SignLength(&ulSignatureLength),
		ERR_CANT_GET_KEY_LENGTH)

	// al massimo k-11 bytes (specifiche p11)
	if (baSignBuffer.size()>ulSignatureLength-11)
		_return(CKR_DATA_LEN_RANGE)

	SignBuffer.alloc_copy(baSignBuffer);

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::SignRecoverInit(CK_OBJECT_HANDLE PrivateKey)
{
	init_func    
	hSignRecoverKey=PrivateKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::SignRecover(ByteArray &Data,ByteDynArray &SignBuffer) {
	init_func

	CK_ULONG ulSignatureLength=0;
	P11ER_CALL(SignRecoverLength(&ulSignatureLength),
		ERR_CANT_GET_KEY_LENGTH)

	// al massimo k-11 bytes (specifiche p11)
	if (Data.size()>ulSignatureLength-11)
		_return(CKR_DATA_LEN_RANGE)

	SignBuffer.alloc_copy(Data);

	_return(OK)
	exit_func
	_return(FAIL)
}
RESULT CRSA_PKCS1::EncryptInit(CK_OBJECT_HANDLE PublicKey) {
	init_func
	hEncryptKey=PublicKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::EncryptUpdate(ByteArray &Part,ByteDynArray &EncryptedData) {
	init_func
	DWORD dwSize=baEncryptBuffer.size();
	baEncryptBuffer.resize(dwSize+Part.size(),true);
	baEncryptBuffer.mid(dwSize,Part.size()).copy(Part);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::EncryptFinal(ByteDynArray &baEncryptedData)
{
	init_func
	CK_ULONG ulEncryptLength=0;
	P11ER_CALL(EncryptLength(&ulEncryptLength),
		ERR_CANT_GET_KEY_LENGTH)

	// al massimo k-11 bytes (specifiche p11)
	if (baEncryptBuffer.size()>(ulEncryptLength-11)) 
		_return(CKR_DATA_LEN_RANGE)

	ByteDynArray baPlainData(ulEncryptLength);
	baPlainData.rightcopy(baEncryptBuffer);
	PutPaddingBT2(baPlainData, baEncryptBuffer.size());
	
	P11ER_CALL(EncryptCompute(baPlainData,baEncryptedData),
		ERR_CANT_ENCRYPT_DATA)

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::DecryptInit(CK_OBJECT_HANDLE PrivateKey)
{
	init_func
	hDecryptKey=PrivateKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::DecryptUpdate(ByteArray &Part,ByteDynArray &Data) {
	init_func
	DWORD dwSize=baDecryptBuffer.size();
	baDecryptBuffer.resize(dwSize+Part.size(),true);
	baDecryptBuffer.mid(dwSize,Part.size()).copy(Part);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::DecryptFinal(ByteDynArray &DecryptBuffer)
{
	init_func
	CK_ULONG ulDecryptLength=0;
	P11ER_CALL(DecryptLength(&ulDecryptLength),
		ERR_CANT_GET_KEY_LENGTH)

	// esattamente k bytes (specifiche p11, ma mi sembra ovvio!)
	if (baDecryptBuffer.size()!=ulDecryptLength)
		_return(CKR_ENCRYPTED_DATA_LEN_RANGE)

	DecryptBuffer.alloc_copy(baDecryptBuffer);

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CRSA_PKCS1::DecryptRemovePadding(ByteArray &paddedData,ByteDynArray &unpaddedData)
{
	init_func
	
	// devo levare il padding BT2 da paddedData e copiare tutto in unpaddedData;
	// se non posso levare il padding i dati criptati non sono validi!
	DWORD dwPaddingLen;
	try {
		dwPaddingLen = RemovePaddingBT2(paddedData);
	}
	catch (...) {
		_return(CKR_ENCRYPTED_DATA_INVALID)
	}

	unpaddedData.alloc_copy(paddedData.mid(dwPaddingLen));

	// i dati possono essere lunghi al massiko k-11
	// (specifiche p11)
	if (unpaddedData.size()>paddedData.size()-11)
		_return(CKR_ENCRYPTED_DATA_INVALID)

	_return(OK)
	exit_func
	_return(FAIL)
}

/* ************************ */
/*		SignRSA_withDigest	*/
/* ************************ */

CSignRSAwithDigest::CSignRSAwithDigest(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session,CDigest *Digest) : pDigest(Digest), CSignRSA (type,std::move(Session)) {}
CSignRSAwithDigest::~CSignRSAwithDigest() {}

RESULT CSignRSAwithDigest::SignSupportMultipart(bool &Support) {
	init_func
	Support=true;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CSignRSAwithDigest::SignInit(CK_OBJECT_HANDLE hPrivateKey) {
	init_func
	hSignKey=hPrivateKey;
	DWORD ret=pDigest->DigestInit();
	_return(ret)
	exit_func
	_return(FAIL)
}

RESULT CSignRSAwithDigest::SignReset() {
	init_func
	DWORD ret=pDigest->DigestInit();
	_return(ret)
	exit_func
	_return(FAIL)
}


RESULT CSignRSAwithDigest::SignUpdate(ByteArray &Part) {
	init_func
	DWORD ret=pDigest->DigestUpdate(Part);
	_return(ret)
	exit_func
	_return(FAIL)
}

RESULT CSignRSAwithDigest::SignFinal(ByteDynArray &SignBuffer) {
	init_func
	CK_ULONG ulDigestLength=0;
	P11ER_CALL(pDigest->DigestLength(&ulDigestLength),
		ERR_CANT_GET_DIGEST_LENGTH)

	SignBuffer.resize(ulDigestLength);
	DWORD ret=pDigest->DigestFinal(SignBuffer);
	_return(ret)

	exit_func
	_return(FAIL)
}

RESULT CSignRSAwithDigest::SignGetOperationState(ByteDynArray &OperationState)
{
	init_func
	DWORD ret=pDigest->DigestGetOperationState(OperationState);
	_return(ret)
	exit_func
	_return(FAIL)
}

RESULT CSignRSAwithDigest::SignSetOperationState(ByteArray &OperationState)
{
	init_func
	DWORD ret=pDigest->DigestSetOperationState(OperationState);
	_return(ret)
	exit_func
	_return(FAIL)
}

/* ************************ */
/*	VerifyRSA_withDigest	*/
/* ************************ */

CVerifyRSAwithDigest::CVerifyRSAwithDigest(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session,CDigest *Digest) : pDigest(Digest), CVerifyRSA (type,std::move(Session)) {}
CVerifyRSAwithDigest::~CVerifyRSAwithDigest() {}

RESULT CVerifyRSAwithDigest::VerifySupportMultipart(bool &Support) {
	init_func
	Support=true;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRSAwithDigest::VerifyInit(CK_OBJECT_HANDLE PublicKey) {
	init_func
	hVerifyKey=PublicKey;
	DWORD ret=pDigest->DigestInit();
	_return(ret)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRSAwithDigest::VerifyUpdate(ByteArray &Part) {
	init_func
	DWORD ret=pDigest->DigestUpdate(Part);
	_return(ret)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRSAwithDigest::VerifyFinal(ByteArray &Signature)
{
	init_func
	ByteDynArray baPlainSignature;
	CK_ULONG ulVerifyLength=0;
	P11ER_CALL(VerifyLength(&ulVerifyLength),
		ERR_CANT_GET_KEY_LENGTH)

	if (Signature.size()!=ulVerifyLength)
		_return(CKR_SIGNATURE_LEN_RANGE)

	P11ER_CALL(VerifyDecryptSignature(Signature,baPlainSignature),
		ERR_CANT_DECRYPT_SIGNATURE)

	ByteDynArray baExpectedResult(ulVerifyLength);
	CK_ULONG ulDigestLen=0;
	P11ER_CALL(pDigest->DigestLength(&ulDigestLen),
		ERR_CANT_GET_DIGEST_LENGTH)

	ByteArray *baDigestInfo=NULL;
	P11ER_CALL(pDigest->DigestInfo(baDigestInfo),
		ERR_CANT_GET_DIGEST_INFO)

	ER_ASSERT(baDigestInfo!=NULL,ERR_CANT_GET_DIGEST_INFO)

	pDigest->DigestFinal(baExpectedResult.revmid(0,ulDigestLen));
	baExpectedResult.rightcopy(*baDigestInfo,ulDigestLen);
	PutPaddingBT1(baExpectedResult, ulDigestLen + baDigestInfo->size());

	if (baPlainSignature==baExpectedResult)
		_return(OK)
	else
		_return(CKR_SIGNATURE_INVALID)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRSAwithDigest::VerifyGetOperationState(ByteDynArray &OperationState)
{
	init_func
	DWORD ret=pDigest->DigestGetOperationState(OperationState);
	_return(ret)
	exit_func
	_return(FAIL)
}

RESULT CVerifyRSAwithDigest::VerifySetOperationState(ByteArray &OperationState)
{
	init_func
	DWORD ret=pDigest->DigestSetOperationState(OperationState);
	_return(ret)
	exit_func
	_return(FAIL)
}
/* ******************** */
/*		RSA_withMD5		*/
/* ******************** */
CRSAwithMD5::CRSAwithMD5(std::shared_ptr<CSession> Session) : CSignRSAwithDigest (CKM_MD5_RSA_PKCS,Session,&md5), CVerifyRSAwithDigest (CKM_MD5_RSA_PKCS,Session,&md5),md5(Session) {}
CRSAwithMD5::~CRSAwithMD5() {}

/* ******************** */
/*		RSA_withSHA1	*/
/* ******************** */
CRSAwithSHA1::CRSAwithSHA1(std::shared_ptr<CSession> Session) : CSignRSAwithDigest (CKM_SHA1_RSA_PKCS,Session,&sha1), CVerifyRSAwithDigest (CKM_SHA1_RSA_PKCS,Session,&sha1),sha1(Session) {}
CRSAwithSHA1::~CRSAwithSHA1() {}

/* ******************** */
/*		EncryptRSA		*/
/* ******************** */
CEncryptRSA::CEncryptRSA() {}
CEncryptRSA::CEncryptRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CEncrypt(type,std::move(Session)) {}
CEncryptRSA::~CEncryptRSA() {}

RESULT CEncryptRSA::EncryptSupportMultipart(bool &Support) {
	init_func
	Support=false;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CEncryptRSA::EncryptLength(CK_ULONG_PTR pulEncryptLen) {
	init_func
	std::shared_ptr<CP11Object> pObject;
	P11ER_CALL(pSession->pSlot->GetObjectFromID(hEncryptKey,pObject),
		ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject!=NULL,ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject->ObjClass==CKO_PUBLIC_KEY,ERR_WRONG_OBJECT_TYPE)
	auto pPrivateKey=std::static_pointer_cast<CP11PrivateKey>(pObject);

	ByteArray *baKeyModule;
	P11ER_CALL(pPrivateKey->getAttribute(CKA_MODULUS,baKeyModule),
		ERR_CANT_GET_PUBKEY_MODULUS)
	ER_ASSERT(baKeyModule!=NULL,ERR_CANT_GET_PUBKEY_MODULUS)
	*pulEncryptLen=baKeyModule->size();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CEncryptRSA::EncryptCompute(ByteArray &baPlainData,ByteDynArray &baEncryptedData)
{
	init_func
	ByteArray *baKeyExponent=NULL,*baKeyModule=NULL;

	std::shared_ptr<CP11Object> pObject;
	P11ER_CALL(pSession->pSlot->GetObjectFromID(hEncryptKey,pObject),
		ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject!=NULL,ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject->ObjClass==CKO_PUBLIC_KEY,ERR_WRONG_OBJECT_TYPE)
	auto pPublicKey=std::static_pointer_cast<CP11PublicKey>(pObject);

	P11ER_CALL(pPublicKey->getAttribute(CKA_PUBLIC_EXPONENT,baKeyExponent),
		ERR_CANT_GET_PUBKEY_EXPONENT)
	ER_ASSERT(baKeyExponent!=NULL,ERR_CANT_GET_PUBKEY_EXPONENT)

	P11ER_CALL(pPublicKey->getAttribute(CKA_MODULUS,baKeyModule),
		ERR_CANT_GET_PUBKEY_MODULUS)
	ER_ASSERT(baKeyModule!=NULL,ERR_CANT_GET_PUBKEY_MODULUS)

	DWORD dwKeyLenBytes=baKeyModule->size();

	// è molto strano che baPlainData non sia lungo quanto la chiave...
	// il controllo su CKR_DATA_LEN_RANGE è stato fatto prima, ma
	// lo uso anche in questo caso
	if (baPlainData.size()!=dwKeyLenBytes)
		_return(CKR_DATA_LEN_RANGE)

	CRSA rsa(*baKeyModule,*baKeyExponent);
	baEncryptedData.resize(dwKeyLenBytes);
	P11ER_CALL(rsa.RSA_PURE(baPlainData,baEncryptedData),
		ERR_CRYPTO_ERROR)

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CEncryptRSA::EncryptGetOperationState(ByteDynArray &OperationState)
{
	init_func
	OperationState.clear();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CEncryptRSA::EncryptSetOperationState(ByteArray &OperationState)
{
	init_func
	if (OperationState.size()!=0)
		_return(CKR_SAVED_STATE_INVALID)
	_return(OK)
	exit_func
	_return(FAIL)
}

/* ******************** */
/*		DecryptRSA		*/
/* ******************** */
CDecryptRSA::CDecryptRSA() {}
CDecryptRSA::CDecryptRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session) : CDecrypt(type,std::move(Session)) {}
CDecryptRSA::~CDecryptRSA() {}

RESULT CDecryptRSA::DecryptSupportMultipart(bool &Support) {
	init_func
	Support=false;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CDecryptRSA::DecryptLength(CK_ULONG_PTR pulDecryptLen) {
	init_func
	std::shared_ptr<CP11Object> pObject;
	P11ER_CALL(pSession->pSlot->GetObjectFromID(hDecryptKey,pObject),
		ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject!=NULL,ERR_CANT_GET_OBJECT)
	ER_ASSERT(pObject->ObjClass==CKO_PRIVATE_KEY,ERR_WRONG_OBJECT_TYPE)
	auto pPrivateKey=std::static_pointer_cast<CP11PrivateKey>(pObject);

	ByteArray *baKeyModule;
	P11ER_CALL(pPrivateKey->getAttribute(CKA_MODULUS,baKeyModule),
		ERR_CANT_GET_PUBKEY_MODULUS)
	ER_ASSERT(baKeyModule!=NULL,ERR_CANT_GET_PUBKEY_MODULUS)
	*pulDecryptLen=baKeyModule->size();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CDecryptRSA::DecryptGetOperationState(ByteDynArray &OperationState)
{
	init_func
	OperationState.clear();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CDecryptRSA::DecryptSetOperationState(ByteArray &OperationState)
{
	init_func
	if (OperationState.size()!=0)
		_return(CKR_SAVED_STATE_INVALID)
	_return(OK)
	exit_func
	_return(FAIL)
}

}