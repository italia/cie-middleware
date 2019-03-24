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

	CMechanism::CMechanism(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : mtType(type), pSession(std::move(Session)) {}
	CMechanism::~CMechanism() {}
	/*bool CDecrypt::checkCache(ByteArray &Data, ByteArray &Result)
	{
		init_func
			if (Data.data() == cacheData.data() && Data.size() == cacheData.size()) {
				if (!Result.isNull()) {
					if (Result.size() < resultCache.size())
						throw new p11_error(CKR_BUFFER_TOO_SMALL);

					Result.copy(resultCache);
					Result = Result.left(resultCache.size());
					return true;
				}
			}
		return false;
	}

	void CDecrypt::setCache(ByteArray &Data, ByteArray &Result)
	{
		init_func
			cacheData = Data;
		resultCache = Result;
	}*/

	CVerify::CVerify(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CMechanism(type, std::move(Session)) {}
	CVerify::~CVerify() {}

	CVerifyRecover::CVerifyRecover(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CMechanism(type, std::move(Session)) {}
	CVerifyRecover::~CVerifyRecover() {}

	CDigest::CDigest(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CMechanism(type, std::move(Session)) {}
	CDigest::~CDigest() {}

	CSign::CSign(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CMechanism(type, std::move(Session)) {}
	CSign::~CSign() {}

	CSignRecover::CSignRecover(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CMechanism(type, std::move(Session)) {}
	CSignRecover::~CSignRecover() {}

	/*CEncrypt::CEncrypt(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CMechanism(type, std::move(Session)) {}
	CEncrypt::~CEncrypt() {}

	BYTE CDecrypt::uninitializedCacheData = 0;
	CDecrypt::CDecrypt(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session)
		: CMechanism(type, Session), cacheData(&uninitializedCacheData, 1) {}
	CDecrypt::~CDecrypt() {}*/

	/* ******************** */
	/*		   SHA1	        */
	/* ******************** */
	CDigestSHA::CDigestSHA(std::shared_ptr<CSession> Session) : CDigest(CKM_SHA_1, std::move(Session)) {}
	CDigestSHA::~CDigestSHA() {}

	void CDigestSHA::DigestInit() {
		init_func
			sha1.Init();
	}

	void CDigestSHA::DigestUpdate(ByteArray &Part) {
		init_func
			sha1.Update(Part);
	}

	void CDigestSHA::DigestFinal(ByteArray &Digest) {
		init_func
			sha1.Final(Digest);
	}

	CK_ULONG CDigestSHA::DigestLength() {
		init_func
			return SHA_DIGEST_LENGTH;
	}

	ByteArray CDigestSHA::DigestInfo() {
		init_func
			return baSHA1DigestInfo;
	}

	ByteDynArray  CDigestSHA::DigestGetOperationState()
	{
		init_func
			throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
	}

	void  CDigestSHA::DigestSetOperationState(ByteArray &OperationState)
	{
		init_func
			throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
	}

	/* ******************** */
	/*		   SHA256	        */
	/* ******************** */
	CDigestSHA256::CDigestSHA256(std::shared_ptr<CSession> Session) : CDigest(CKM_SHA_1, std::move(Session)) {}
	CDigestSHA256::~CDigestSHA256() {}

	void CDigestSHA256::DigestInit() {
		init_func
			data.clear();
	}

	void CDigestSHA256::DigestUpdate(ByteArray &Part) {
		init_func
			data.append(Part);
	}

	void CDigestSHA256::DigestFinal(ByteArray &Digest) {
		init_func

			//			data.append(Digest);
			ByteDynArray dataOut(SHA256_DIGEST_LENGTH);
			sha256.Digest(data, dataOut);
			Digest.copy(dataOut);
	}

	CK_ULONG CDigestSHA256::DigestLength() {
		init_func
			return SHA256_DIGEST_LENGTH;
	}

	ByteArray CDigestSHA256::DigestInfo() {
		init_func
			return baSHA1DigestInfo;
	}

	ByteDynArray  CDigestSHA256::DigestGetOperationState()
	{
		init_func
			throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
	}

	void  CDigestSHA256::DigestSetOperationState(ByteArray &OperationState)
	{
		init_func
			throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
	}

	/* ******************** */
	/*		   MD5	        */
	/* ******************** */
	CDigestMD5::CDigestMD5(std::shared_ptr<CSession> Session) : CDigest(CKM_MD5, std::move(Session)) {}
	CDigestMD5::~CDigestMD5() {}

	void CDigestMD5::DigestInit() {
		init_func
			md5.Init();
	}

	void CDigestMD5::DigestUpdate(ByteArray &Part) {
		init_func
			md5.Update(Part);
	}

	void CDigestMD5::DigestFinal(ByteArray &Digest) {
		init_func
			md5.Final(Digest);
	}

	CK_ULONG CDigestMD5::DigestLength() {
		init_func
			return MD5_DIGEST_LENGTH;
	}

	ByteArray CDigestMD5::DigestInfo() {
		init_func
			return baMD5DigestInfo;
	}

	ByteDynArray CDigestMD5::DigestGetOperationState()
	{
		init_func
			throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
	}

	void CDigestMD5::DigestSetOperationState(ByteArray &OperationState)
	{
		init_func
			throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
	}

	/* ******************** */
	/*		Verify RSA		*/
	/* ******************** */
	CVerifyRSA::CVerifyRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CVerify(type, std::move(Session)) {}
	CVerifyRSA::~CVerifyRSA() {}

	bool  CVerifyRSA::VerifySupportMultipart() {
		init_func
			return false;
	}

	CK_ULONG CVerifyRSA::VerifyLength()
	{
		init_func

			std::shared_ptr<CP11Object> pObject = pSession->pSlot->GetObjectFromID(hVerifyKey);
		ER_ASSERT(pObject != nullptr, ERR_CANT_GET_OBJECT)
			ER_ASSERT(pObject->ObjClass == CKO_PUBLIC_KEY, ERR_WRONG_OBJECT_TYPE)
			auto pPublicKey = std::static_pointer_cast<CP11PublicKey>(pObject);

		ByteArray *baKeyModule = pPublicKey->getAttribute(CKA_MODULUS);
		ER_ASSERT(!baKeyModule->isNull(), ERR_CANT_GET_PUBKEY_MODULUS)
			return (CK_ULONG)baKeyModule->size();
	}

	ByteDynArray CVerifyRSA::VerifyDecryptSignature(ByteArray &Signature)
	{
		init_func
			ByteArray *baKeyExponent = nullptr, *baKeyModule = nullptr;

		std::shared_ptr<CP11Object> pObject = pSession->pSlot->GetObjectFromID(hVerifyKey);
		ER_ASSERT(pObject != nullptr, ERR_CANT_GET_OBJECT)
			ER_ASSERT(pObject->ObjClass == CKO_PUBLIC_KEY, ERR_WRONG_OBJECT_TYPE)
			auto pPublicKey = std::static_pointer_cast<CP11PublicKey>(pObject);

		baKeyExponent = pPublicKey->getAttribute(CKA_PUBLIC_EXPONENT);
		ER_ASSERT(baKeyExponent != nullptr, ERR_CANT_GET_PUBKEY_EXPONENT)

			baKeyModule = pPublicKey->getAttribute(CKA_MODULUS);
		ER_ASSERT(baKeyModule != nullptr, ERR_CANT_GET_PUBKEY_MODULUS)

		if (Signature.size() != baKeyModule->size())
			throw p11_error(CKR_SIGNATURE_LEN_RANGE);

		CRSA rsa(*baKeyModule, *baKeyExponent);
		return rsa.RSA_PURE(Signature);
	}

	ByteDynArray CVerifyRSA::VerifyGetOperationState()
	{
		init_func
			return ByteDynArray();
	}

	void CVerifyRSA::VerifySetOperationState(ByteArray &OperationState)
	{
		init_func
			if (OperationState.size() != 0)
				throw p11_error(CKR_SAVED_STATE_INVALID);
	}

	/* ******************** */
	/*	VerifyRecover RSA	*/
	/* ******************** */
	CVerifyRecoverRSA::CVerifyRecoverRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CVerifyRecover(type, std::move(Session)) {}
	CVerifyRecoverRSA::~CVerifyRecoverRSA() {}

	CK_ULONG CVerifyRecoverRSA::VerifyRecoverLength()
	{
		init_func

			std::shared_ptr<CP11Object> pObject = pSession->pSlot->GetObjectFromID(hVerifyRecoverKey);
		ER_ASSERT(pObject != nullptr, ERR_CANT_GET_OBJECT)
			ER_ASSERT(pObject->ObjClass == CKO_PUBLIC_KEY, ERR_WRONG_OBJECT_TYPE)
			auto pPublicKey = std::static_pointer_cast<CP11PublicKey>(pObject);

		ByteArray *baKeyModule = pPublicKey->getAttribute(CKA_MODULUS);
		ER_ASSERT(baKeyModule != nullptr, ERR_CANT_GET_PUBKEY_MODULUS)
			return (CK_ULONG )baKeyModule->size();
	}

	ByteDynArray CVerifyRecoverRSA::VerifyRecoverDecryptSignature(ByteArray &Signature)
	{
		init_func
			ByteArray *baKeyExponent = nullptr, *baKeyModule = nullptr;

		std::shared_ptr<CP11Object> pObject = pSession->pSlot->GetObjectFromID(hVerifyRecoverKey);
		ER_ASSERT(pObject != nullptr, ERR_CANT_GET_OBJECT)
			ER_ASSERT(pObject->ObjClass == CKO_PUBLIC_KEY, ERR_WRONG_OBJECT_TYPE)
			auto pPublicKey = std::static_pointer_cast<CP11PublicKey>(pObject);

		pPublicKey->getAttribute(CKA_PUBLIC_EXPONENT);
		ER_ASSERT(baKeyExponent != nullptr, ERR_CANT_GET_PUBKEY_EXPONENT);

		baKeyModule = pPublicKey->getAttribute(CKA_MODULUS);
		ER_ASSERT(baKeyModule != nullptr, ERR_CANT_GET_PUBKEY_MODULUS)

		if (Signature.size() != baKeyModule->size())
			throw p11_error(CKR_SIGNATURE_LEN_RANGE);

		CRSA rsa(*baKeyModule, *baKeyExponent);
		return rsa.RSA_PURE(Signature);
	}

	ByteDynArray  CVerifyRecoverRSA::VerifyRecoverGetOperationState()
	{
		init_func
			return ByteDynArray();
	}

	void CVerifyRecoverRSA::VerifyRecoverSetOperationState(ByteArray &OperationState)
	{
		init_func
			if (OperationState.size() != 0)
				throw p11_error(CKR_SAVED_STATE_INVALID);
	}

	/* ******************** */
	/*		SignRSA			*/
	/* ******************** */
	CSignRSA::CSignRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CSign(type, std::move(Session)) {}
	CSignRSA::~CSignRSA() {}

	bool CSignRSA::SignSupportMultipart() {
		init_func
			return false;
	}

	CK_ULONG CSignRSA::SignLength() {
		init_func
			std::shared_ptr<CP11Object> pObject = pSession->pSlot->GetObjectFromID(hSignKey);
		ER_ASSERT(pObject != nullptr, ERR_CANT_GET_OBJECT)
			ER_ASSERT(pObject->ObjClass == CKO_PRIVATE_KEY, ERR_WRONG_OBJECT_TYPE)
			auto pPrivateKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		ByteArray *baKeyModule = pPrivateKey->getAttribute(CKA_MODULUS);
		ER_ASSERT(baKeyModule != nullptr, ERR_CANT_GET_PUBKEY_MODULUS)
			return (CK_ULONG )baKeyModule->size();
	}

	ByteDynArray CSignRSA::SignGetOperationState()
	{
		init_func
			return ByteDynArray();
	}

	void CSignRSA::SignSetOperationState(ByteArray &OperationState)
	{
		init_func
			if (OperationState.size() != 0)
				throw p11_error(CKR_SAVED_STATE_INVALID);
	}

	/* ******************** */
	/*	SignRecoverRSA		*/
	/* ******************** */
	CSignRecoverRSA::CSignRecoverRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CSignRecover(type, std::move(Session)) {}
	CSignRecoverRSA::~CSignRecoverRSA() {}

	CK_ULONG CSignRecoverRSA::SignRecoverLength() {
		init_func
			std::shared_ptr<CP11Object> pObject = pSession->pSlot->GetObjectFromID(hSignRecoverKey);
		ER_ASSERT(pObject != nullptr, ERR_CANT_GET_OBJECT)
			ER_ASSERT(pObject->ObjClass == CKO_PRIVATE_KEY, ERR_WRONG_OBJECT_TYPE)
			auto pPrivateKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		ByteArray *baKeyModule = pPrivateKey->getAttribute(CKA_MODULUS);
		ER_ASSERT(baKeyModule != nullptr, ERR_CANT_GET_PUBKEY_MODULUS)
			return (CK_ULONG )baKeyModule->size();
	}

	ByteDynArray CSignRecoverRSA::SignRecoverGetOperationState()
	{
		init_func
			return ByteDynArray();
	}

	void CSignRecoverRSA::SignRecoverSetOperationState(ByteArray &OperationState)
	{
		init_func
			if (OperationState.size() != 0)
				throw p11_error(CKR_SAVED_STATE_INVALID);
	}

	///* ******************** */
	///*		RSA_X509		*/
	///* ******************** */
	//CRSA_X509::CRSA_X509(std::shared_ptr<CSession> Session) : CSignRSA(CKM_RSA_X_509, Session),
	//	CSignRecoverRSA(CKM_RSA_X_509, Session),
	//	CVerifyRSA(CKM_RSA_X_509, Session),
	//	CVerifyRecoverRSA(CKM_RSA_X_509, Session),
	//	CEncryptRSA(CKM_RSA_X_509, Session),
	//	CDecryptRSA(CKM_RSA_X_509, Session) {}
	//CRSA_X509::~CRSA_X509() {}

	//void CRSA_X509::VerifyInit(CK_OBJECT_HANDLE PublicKey) {
	//	init_func
	//		hVerifyKey = PublicKey;
	//}

	//void CRSA_X509::VerifyUpdate(ByteArray &Part) {
	//	init_func
	//	auto dwSize = baVerifyBuffer.size();
	//	baVerifyBuffer.resize(dwSize + Part.size(), true);
	//	baVerifyBuffer.mid(dwSize, Part.size()).copy(Part);
	//}

	//void CRSA_X509::VerifyFinal(ByteArray &Signature)
	//{
	//	init_func
	//		ByteDynArray baPlainSignature;
	//	CK_ULONG ulVerifyLength = VerifyLength();

	//	if (Signature.size() != ulVerifyLength)
	//		throw p11_error(CKR_SIGNATURE_LEN_RANGE);

	//	// il buffer da verificare può anche essere
	//	// più corto della chiave, viene paddato automaticamente
	//	// specifiche P11
	//	if (baVerifyBuffer.size() > ulVerifyLength)
	//		throw p11_error(CKR_DATA_LEN_RANGE);

	//	baPlainSignature = VerifyDecryptSignature(Signature);

	//	ByteDynArray baExpectedResult(ulVerifyLength);
	//	baExpectedResult.rightcopy(baVerifyBuffer);
	//	PutPaddingBT0(baExpectedResult, baVerifyBuffer.size());

	//	if (baPlainSignature == baExpectedResult)
	//		return;
	//	else
	//		throw p11_error(CKR_SIGNATURE_INVALID);
	//}

	//void CRSA_X509::VerifyRecoverInit(CK_OBJECT_HANDLE PublicKey) {
	//	init_func
	//		hVerifyRecoverKey = PublicKey;
	//}

	//ByteDynArray CRSA_X509::VerifyRecover(ByteArray &Signature)
	//{
	//	init_func
	//		CK_ULONG ulVerifyRecoverLength = VerifyRecoverLength();

	//	if (Signature.size() != ulVerifyRecoverLength)
	//		throw p11_error(CKR_SIGNATURE_LEN_RANGE);

	//	return VerifyRecoverDecryptSignature(Signature);
	//}

	//void CRSA_X509::SignInit(CK_OBJECT_HANDLE PrivateKey)
	//{
	//	init_func
	//		hSignKey = PrivateKey;
	//}

	//void CRSA_X509::SignReset()
	//{
	//	init_func
	//		baSignBuffer.clear();
	//}

	//void CRSA_X509::SignUpdate(ByteArray &Part) {
	//	init_func
	//	auto dwSize = baSignBuffer.size();
	//	baSignBuffer.resize(dwSize + Part.size(), true);
	//	baSignBuffer.mid(dwSize, Part.size()).copy(Part);
	//}

	//ByteDynArray CRSA_X509::SignFinal()
	//{
	//	init_func
	//		CK_ULONG ulSignatureLength = SignLength();

	//	if (baSignBuffer.size() > ulSignatureLength)
	//		throw p11_error(CKR_DATA_LEN_RANGE);

	//	return baSignBuffer;
	//}

	//void CRSA_X509::SignRecoverInit(CK_OBJECT_HANDLE PrivateKey) {
	//	init_func
	//		hSignRecoverKey = PrivateKey;
	//}

	//ByteDynArray CRSA_X509::SignRecover(ByteArray &Data) {
	//	init_func

	//		CK_ULONG ulSignatureLength = SignRecoverLength();

	//	if (Data.size() > ulSignatureLength)
	//		throw p11_error(CKR_DATA_LEN_RANGE);

	//	return Data;

	//}

	//void CRSA_X509::EncryptInit(CK_OBJECT_HANDLE PublicKey) {
	//	init_func
	//		hEncryptKey = PublicKey;
	//}

	//ByteDynArray CRSA_X509::EncryptUpdate(ByteArray &Part) {
	//	init_func
	//	auto dwSize = baEncryptBuffer.size();
	//	baEncryptBuffer.resize(dwSize + Part.size(), true);
	//	baEncryptBuffer.mid(dwSize, Part.size()).copy(Part);
	//	return ByteDynArray();
	//}

	//ByteDynArray CRSA_X509::EncryptFinal()
	//{
	//	init_func
	//		CK_ULONG ulEncryptLength = EncryptLength();

	//	if (baEncryptBuffer.size() > ulEncryptLength)
	//		throw p11_error(CKR_DATA_LEN_RANGE);

	//	ByteDynArray baPlainData(ulEncryptLength);
	//	baPlainData.rightcopy(baEncryptBuffer);
	//	PutPaddingBT0(baPlainData, baEncryptBuffer.size());

	//	return EncryptCompute(baPlainData);
	//}

	//void CRSA_X509::DecryptInit(CK_OBJECT_HANDLE PrivateKey)
	//{
	//	init_func
	//		hDecryptKey = PrivateKey;
	//}

	//ByteDynArray CRSA_X509::DecryptUpdate(ByteArray &Part) {
	//	init_func
	//	auto dwSize = baDecryptBuffer.size();
	//	baDecryptBuffer.resize(dwSize + Part.size(), true);
	//	baDecryptBuffer.mid(dwSize, Part.size()).copy(Part);
	//	return ByteDynArray();
	//}

	//ByteDynArray CRSA_X509::DecryptFinal()
	//{
	//	init_func
	//		CK_ULONG ulDecryptLength = DecryptLength();

	//	// nella decrypt il buffer deve essere lungo esasttamente k
	//	// (specifiche P11)
	//	if (baDecryptBuffer.size() != ulDecryptLength)
	//		throw p11_error(CKR_ENCRYPTED_DATA_LEN_RANGE);

	//	return baDecryptBuffer;

	//}

	//ByteDynArray CRSA_X509::DecryptRemovePadding(ByteArray &paddedData)
	//{
	//	init_func
	//		// non faccio nullptra perchè l'RSA_X509 non leva il padding
	//		return paddedData;
	//}

	/* ******************** */
	/*		RSA_PKCS1		*/
	/* ******************** */
	CRSA_PKCS1::CRSA_PKCS1(std::shared_ptr<CSession> Session) : CSignRSA(CKM_RSA_PKCS, Session),
		CSignRecoverRSA(CKM_RSA_PKCS, Session),
		CVerifyRSA(CKM_RSA_PKCS, Session),
		CVerifyRecoverRSA(CKM_RSA_PKCS, Session) /*,
		CEncryptRSA(CKM_RSA_PKCS, Session),
		CDecryptRSA(CKM_RSA_PKCS, Session) */
	{}
	CRSA_PKCS1::~CRSA_PKCS1() {}

	void CRSA_PKCS1::VerifyInit(CK_OBJECT_HANDLE PublicKey) {
		init_func
			hVerifyKey = PublicKey;
	}

	void CRSA_PKCS1::VerifyUpdate(ByteArray &Part) {
		init_func
		auto dwSize = baVerifyBuffer.size();
		baVerifyBuffer.resize(dwSize + Part.size(), true);
		baVerifyBuffer.mid(dwSize, Part.size()).copy(Part);
	}

	void  CRSA_PKCS1::VerifyFinal(ByteArray &Signature)
	{
		init_func
			ByteDynArray baPlainSignature;
		CK_ULONG ulVerifyLength = VerifyLength();

		if (Signature.size() != ulVerifyLength)
			throw p11_error(CKR_SIGNATURE_LEN_RANGE);

		// max k-11 (specifiche p11)
		if (baVerifyBuffer.size() > ulVerifyLength - 11)
			throw p11_error(CKR_DATA_LEN_RANGE);

		baPlainSignature = VerifyDecryptSignature(Signature);

		ByteDynArray baExpectedResult(ulVerifyLength);
		baExpectedResult.rightcopy(baVerifyBuffer);
		PutPaddingBT1(baExpectedResult, baVerifyBuffer.size());

		if (baPlainSignature == baExpectedResult)
			return;
		else
			throw p11_error(CKR_SIGNATURE_INVALID);
	}

	void CRSA_PKCS1::VerifyRecoverInit(CK_OBJECT_HANDLE PublicKey) {
		init_func
			hVerifyRecoverKey = PublicKey;
	}

	ByteDynArray CRSA_PKCS1::VerifyRecover(ByteArray &Signature)
	{
		init_func
			CK_ULONG ulVerifyRecoverLength = VerifyRecoverLength();

		if (Signature.size() != ulVerifyRecoverLength)
			throw p11_error(CKR_SIGNATURE_LEN_RANGE);

		ByteDynArray baPlainSignature = VerifyRecoverDecryptSignature(Signature);

		// se non posso levare il padding, la firma ha
		// qualcosa di sbagliato
		size_t dwPadLen;
		try {
			dwPadLen = RemovePaddingBT1(baPlainSignature);
		}
		catch (...) {
			throw p11_error(CKR_SIGNATURE_INVALID);
		}

		// i dati restutuiti non possono essere più
		// lunghi di k-11!! (specifiche p11)
		auto Data = baPlainSignature.mid(dwPadLen);
		if (Data.size() > ulVerifyRecoverLength - 11)
			throw p11_error(CKR_DATA_LEN_RANGE);
		return Data;
	}

	void CRSA_PKCS1::SignInit(CK_OBJECT_HANDLE PrivateKey)
	{
		init_func
			hSignKey = PrivateKey;
	}

	void CRSA_PKCS1::SignReset()
	{
		init_func
			baSignBuffer.clear();
	}

	void CRSA_PKCS1::SignUpdate(ByteArray &Part) {
		init_func
		auto dwSize = baSignBuffer.size();
		baSignBuffer.resize(dwSize + Part.size(), true);
		baSignBuffer.mid(dwSize, Part.size()).copy(Part);
	}

	ByteDynArray CRSA_PKCS1::SignFinal()
	{
		init_func
			CK_ULONG ulSignatureLength = SignLength();

		// al massimo k-11 bytes (specifiche p11)
		if (baSignBuffer.size() > ulSignatureLength - 11)
			throw p11_error(CKR_DATA_LEN_RANGE);

		return baSignBuffer;
	}

	void CRSA_PKCS1::SignRecoverInit(CK_OBJECT_HANDLE PrivateKey)
	{
		init_func
			hSignRecoverKey = PrivateKey;
	}

	ByteDynArray CRSA_PKCS1::SignRecover(ByteArray &Data) {
		init_func

			CK_ULONG ulSignatureLength = SignRecoverLength();

		// al massimo k-11 bytes (specifiche p11)
		if (Data.size() > ulSignatureLength - 11)
			throw p11_error(CKR_DATA_LEN_RANGE);

		return Data;

	}
	//void  CRSA_PKCS1::EncryptInit(CK_OBJECT_HANDLE PublicKey) {
	//	init_func
	//		hEncryptKey = PublicKey;
	//}

	//ByteDynArray CRSA_PKCS1::EncryptUpdate(ByteArray &Part) {
	//	init_func
	//	auto dwSize = baEncryptBuffer.size();
	//	baEncryptBuffer.resize(dwSize + Part.size(), true);
	//	baEncryptBuffer.mid(dwSize, Part.size()).copy(Part);
	//	return ByteDynArray();
	//}

	//ByteDynArray CRSA_PKCS1::EncryptFinal()
	//{
	//	init_func
	//		CK_ULONG ulEncryptLength = EncryptLength();

	//	// al massimo k-11 bytes (specifiche p11)
	//	if (baEncryptBuffer.size() > (ulEncryptLength - 11))
	//		throw p11_error(CKR_DATA_LEN_RANGE);

	//	ByteDynArray baPlainData(ulEncryptLength);
	//	baPlainData.rightcopy(baEncryptBuffer);
	//	PutPaddingBT2(baPlainData, baEncryptBuffer.size());

	//	return EncryptCompute(baPlainData);

	//}

	//void CRSA_PKCS1::DecryptInit(CK_OBJECT_HANDLE PrivateKey)
	//{
	//	init_func
	//		hDecryptKey = PrivateKey;
	//}

	//ByteDynArray CRSA_PKCS1::DecryptUpdate(ByteArray &Part) {
	//	init_func
	//	auto dwSize = baDecryptBuffer.size();
	//	baDecryptBuffer.resize(dwSize + Part.size(), true);
	//	baDecryptBuffer.mid(dwSize, Part.size()).copy(Part);
	//	return ByteDynArray();
	//}

	//ByteDynArray CRSA_PKCS1::DecryptFinal()
	//{
	//	init_func
	//		CK_ULONG ulDecryptLength = DecryptLength();

	//	// esattamente k bytes (specifiche p11, ma mi sembra ovvio!)
	//	if (baDecryptBuffer.size() != ulDecryptLength)
	//		throw p11_error(CKR_ENCRYPTED_DATA_LEN_RANGE);

	//	return baDecryptBuffer;
	//}

	//ByteDynArray CRSA_PKCS1::DecryptRemovePadding(ByteArray &paddedData)
	//{
	//	init_func

	//		// devo levare il padding BT2 da paddedData e copiare tutto in unpaddedData;
	//		// se non posso levare il padding i dati criptati non sono validi!
	//	size_t dwPaddingLen;
	//	try {
	//		dwPaddingLen = RemovePaddingBT2(paddedData);
	//	}
	//	catch (...) {
	//		throw p11_error(CKR_ENCRYPTED_DATA_INVALID);
	//	}

	//	auto unpaddedData = paddedData.mid(dwPaddingLen);

	//	// i dati possono essere lunghi al massiko k-11
	//	// (specifiche p11)
	//	if (unpaddedData.size() > paddedData.size() - 11)
	//		throw p11_error(CKR_ENCRYPTED_DATA_INVALID);

	//	return unpaddedData;
	//}

	/* ************************ */
	/*		SignRSA_withDigest	*/
	/* ************************ */

	CSignRSAwithDigest::CSignRSAwithDigest(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session, CDigest *Digest) : pDigest(Digest), CSignRSA(type, std::move(Session)) {}
	CSignRSAwithDigest::~CSignRSAwithDigest() {}

	bool CSignRSAwithDigest::SignSupportMultipart() {
		init_func
			return true;
	}

	void CSignRSAwithDigest::SignInit(CK_OBJECT_HANDLE hPrivateKey) {
		init_func
			hSignKey = hPrivateKey;
		pDigest->DigestInit();
	}

	void CSignRSAwithDigest::SignReset() {
		init_func
			pDigest->DigestInit();
	}


	void  CSignRSAwithDigest::SignUpdate(ByteArray &Part) {
		init_func
			pDigest->DigestUpdate(Part);
	}

	ByteDynArray CSignRSAwithDigest::SignFinal() {
		init_func
			CK_ULONG ulDigestLength = pDigest->DigestLength();

		ByteDynArray SignBuffer(ulDigestLength);
		pDigest->DigestFinal(SignBuffer);

		ByteDynArray baDigestInfo = pDigest->DigestInfo();

		return baDigestInfo.append(SignBuffer);
	}

	ByteDynArray CSignRSAwithDigest::SignGetOperationState()
	{
		init_func
			return pDigest->DigestGetOperationState();
	}

	void CSignRSAwithDigest::SignSetOperationState(ByteArray &OperationState)
	{
		init_func
			pDigest->DigestSetOperationState(OperationState);
	}

	/* ************************ */
	/*	VerifyRSA_withDigest	*/
	/* ************************ */

	CVerifyRSAwithDigest::CVerifyRSAwithDigest(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session, CDigest *Digest) : pDigest(Digest), CVerifyRSA(type, std::move(Session)) {}
	CVerifyRSAwithDigest::~CVerifyRSAwithDigest() {}

	bool CVerifyRSAwithDigest::VerifySupportMultipart() {
		init_func
			return true;
	}

	void CVerifyRSAwithDigest::VerifyInit(CK_OBJECT_HANDLE PublicKey) {
		init_func
			hVerifyKey = PublicKey;
		pDigest->DigestInit();
	}

	void CVerifyRSAwithDigest::VerifyUpdate(ByteArray &Part) {
		init_func
			pDigest->DigestUpdate(Part);
	}

	void CVerifyRSAwithDigest::VerifyFinal(ByteArray &Signature)
	{
		init_func
			ByteDynArray baPlainSignature;
		CK_ULONG ulVerifyLength = VerifyLength();

		if (Signature.size() != ulVerifyLength)
			throw p11_error(CKR_SIGNATURE_LEN_RANGE);

		baPlainSignature = VerifyDecryptSignature(Signature);

		ByteDynArray baExpectedResult(ulVerifyLength);
		CK_ULONG ulDigestLen = pDigest->DigestLength();

		ByteArray baDigestInfo = pDigest->DigestInfo();

		pDigest->DigestFinal(baExpectedResult.right(ulDigestLen));
		baExpectedResult.rightcopy(baDigestInfo, ulDigestLen);
		PutPaddingBT1(baExpectedResult, ulDigestLen + baDigestInfo.size());

		if (baPlainSignature == baExpectedResult)
			return;
		else
			throw p11_error(CKR_SIGNATURE_INVALID);
	}

	ByteDynArray CVerifyRSAwithDigest::VerifyGetOperationState()
	{
		init_func
			return pDigest->DigestGetOperationState();
	}

	void CVerifyRSAwithDigest::VerifySetOperationState(ByteArray &OperationState)
	{
		init_func
			pDigest->DigestSetOperationState(OperationState);
	}
	/* ******************** */
	/*		RSA_withMD5		*/
	/* ******************** */
	CRSAwithMD5::CRSAwithMD5(std::shared_ptr<CSession> Session) : CSignRSAwithDigest(CKM_MD5_RSA_PKCS, Session, &md5), CVerifyRSAwithDigest(CKM_MD5_RSA_PKCS, Session, &md5), md5(Session) {}
	CRSAwithMD5::~CRSAwithMD5() {}

	/* ******************** */
	/*		RSA_withSHA1	*/
	/* ******************** */
	CRSAwithSHA1::CRSAwithSHA1(std::shared_ptr<CSession> Session) : CSignRSAwithDigest(CKM_SHA1_RSA_PKCS, Session, &sha1), CVerifyRSAwithDigest(CKM_SHA1_RSA_PKCS, Session, &sha1), sha1(Session) {}
	CRSAwithSHA1::~CRSAwithSHA1() {}

	/* ******************** */
	/*		RSA_withSHA1	*/
	/* ******************** */
	CRSAwithSHA256::CRSAwithSHA256(std::shared_ptr<CSession> Session) : CSignRSAwithDigest(CKM_SHA256_RSA_PKCS, Session, &sha256), CVerifyRSAwithDigest(CKM_SHA256_RSA_PKCS, Session, &sha256), sha256(Session) {}
	CRSAwithSHA256::~CRSAwithSHA256() {}

	///* ******************** */
	///*		EncryptRSA		*/
	///* ******************** */
	//CEncryptRSA::CEncryptRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CEncrypt(type, std::move(Session)) {}
	//CEncryptRSA::~CEncryptRSA() {}

	//bool CEncryptRSA::EncryptSupportMultipart() {
	//	init_func
	//		return false;
	//}

	//CK_ULONG CEncryptRSA::EncryptLength() {
	//	init_func
	//		std::shared_ptr<CP11Object> pObject = pSession->pSlot->GetObjectFromID(hEncryptKey);
	//	ER_ASSERT(pObject != nullptr, ERR_CANT_GET_OBJECT)
	//		ER_ASSERT(pObject->ObjClass == CKO_PUBLIC_KEY, ERR_WRONG_OBJECT_TYPE)
	//		auto pPrivateKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

	//	ByteArray *baKeyModule = pPrivateKey->getAttribute(CKA_MODULUS);
	//	ER_ASSERT(baKeyModule != nullptr, ERR_CANT_GET_PUBKEY_MODULUS)
	//		return (CK_ULONG)baKeyModule->size();
	//}

	//ByteDynArray CEncryptRSA::EncryptCompute(ByteArray &baPlainData)
	//{
	//	init_func
	//		ByteArray *baKeyExponent = nullptr, *baKeyModule = nullptr;

	//	std::shared_ptr<CP11Object> pObject = pSession->pSlot->GetObjectFromID(hEncryptKey);
	//	ER_ASSERT(pObject != nullptr, ERR_CANT_GET_OBJECT)
	//		ER_ASSERT(pObject->ObjClass == CKO_PUBLIC_KEY, ERR_WRONG_OBJECT_TYPE)
	//		auto pPublicKey = std::static_pointer_cast<CP11PublicKey>(pObject);

	//	baKeyExponent = pPublicKey->getAttribute(CKA_PUBLIC_EXPONENT);
	//	ER_ASSERT(baKeyExponent != nullptr, ERR_CANT_GET_PUBKEY_EXPONENT)

	//		baKeyModule = pPublicKey->getAttribute(CKA_MODULUS);
	//	ER_ASSERT(baKeyModule != nullptr, ERR_CANT_GET_PUBKEY_MODULUS)

	//	auto dwKeyLenBytes = baKeyModule->size();

	//	// è molto strano che baPlainData non sia lungo quanto la chiave...
	//	// il controllo su CKR_DATA_LEN_RANGE è stato fatto prima, ma
	//	// lo uso anche in questo caso
	//	if (baPlainData.size() != dwKeyLenBytes)
	//		throw p11_error(CKR_DATA_LEN_RANGE);

	//	CRSA rsa(*baKeyModule, *baKeyExponent);
	//	return rsa.RSA_PURE(baPlainData);
	//}

	//ByteDynArray CEncryptRSA::EncryptGetOperationState()
	//{
	//	init_func
	//		return ByteDynArray();
	//}

	//void CEncryptRSA::EncryptSetOperationState(ByteArray &OperationState)
	//{
	//	init_func
	//		if (OperationState.size() != 0)
	//			throw p11_error(CKR_SAVED_STATE_INVALID);
	//}

	/* ******************** */
	/*		DecryptRSA		*/
	/* ******************** */
	//CDecryptRSA::CDecryptRSA() {}
	/*CDecryptRSA::CDecryptRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session) : CDecrypt(type, std::move(Session)) {}
	CDecryptRSA::~CDecryptRSA() {}

	bool CDecryptRSA::DecryptSupportMultipart() {
		init_func
			return false;
	}

	CK_ULONG CDecryptRSA::DecryptLength() {
		init_func
			std::shared_ptr<CP11Object> pObject = pSession->pSlot->GetObjectFromID(hDecryptKey);
		ER_ASSERT(pObject != nullptr, ERR_CANT_GET_OBJECT)
			ER_ASSERT(pObject->ObjClass == CKO_PRIVATE_KEY, ERR_WRONG_OBJECT_TYPE)
			auto pPrivateKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		ByteArray *baKeyModule = pPrivateKey->getAttribute(CKA_MODULUS);
		ER_ASSERT(baKeyModule != nullptr, ERR_CANT_GET_PUBKEY_MODULUS)
			return (CK_ULONG)baKeyModule->size();
	}

	ByteDynArray CDecryptRSA::DecryptGetOperationState()
	{
		init_func
			return ByteDynArray();
	}

	void CDecryptRSA::DecryptSetOperationState(ByteArray &OperationState)
	{
		init_func
			if (OperationState.size() != 0)
				throw p11_error(CKR_SAVED_STATE_INVALID);
	}*/

}