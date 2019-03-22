#pragma once
#include "../Crypto/SHA1.h"
#include "../Crypto/sha256.h"
#include "../Crypto/MD5.h"
#include "cryptoki.h"
//#pragma pack(1)
//#include "pkcs11.h"
//#pragma pack()

#include <memory>

namespace p11 {

	class CSession;

	class CMechanism
	{
	public:
		CK_MECHANISM_TYPE mtType;
		CMechanism(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CMechanism(void);
		std::shared_ptr<CSession> pSession;
	};

	class CDigest : public CMechanism
	{
	public:
		CDigest(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CDigest();

		virtual void DigestInit() = 0;
		virtual void DigestUpdate(ByteArray &Part) = 0;
		virtual void DigestFinal(ByteArray &Digest) = 0;
		virtual CK_ULONG DigestLength() = 0;
		virtual ByteArray DigestInfo() = 0;
		virtual ByteDynArray  DigestGetOperationState() = 0;
		virtual void DigestSetOperationState(ByteArray &OperationState) = 0;
	};

	class CVerify : public CMechanism
	{
	public:
		CK_OBJECT_HANDLE hVerifyKey;

		CVerify(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CVerify();

		virtual bool VerifySupportMultipart() = 0;
		virtual void VerifyInit(CK_OBJECT_HANDLE PublicKey) = 0;
		virtual void VerifyUpdate(ByteArray &Part) = 0;
		virtual void VerifyFinal(ByteArray &Signature) = 0;
		virtual CK_ULONG VerifyLength() = 0;
		virtual ByteDynArray  VerifyDecryptSignature(ByteArray &Signature) = 0;
		virtual ByteDynArray VerifyGetOperationState() = 0;
		virtual void VerifySetOperationState(ByteArray &OperationState) = 0;
	};

	class CVerifyRSA : public CVerify
	{
	public:
		CVerifyRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CVerifyRSA();

		bool VerifySupportMultipart();
		ByteDynArray  VerifyDecryptSignature(ByteArray &Signature);
		CK_ULONG VerifyLength();
		ByteDynArray VerifyGetOperationState();
		void VerifySetOperationState(ByteArray &OperationState);
	};

	class CVerifyRecover : public CMechanism
	{
	public:
		CK_OBJECT_HANDLE hVerifyRecoverKey;

		CVerifyRecover(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CVerifyRecover();

		virtual void VerifyRecoverInit(CK_OBJECT_HANDLE PublicKey) = 0;
		virtual ByteDynArray VerifyRecover(ByteArray &Signature) = 0;
		virtual CK_ULONG VerifyRecoverLength() = 0;
		virtual ByteDynArray VerifyRecoverDecryptSignature(ByteArray &Signature) = 0;
		virtual ByteDynArray VerifyRecoverGetOperationState() = 0;
		virtual void VerifyRecoverSetOperationState(ByteArray &OperationState) = 0;
	};

	class CVerifyRecoverRSA : public CVerifyRecover
	{
	public:
		CVerifyRecoverRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CVerifyRecoverRSA();

		ByteDynArray VerifyRecoverDecryptSignature(ByteArray &Signature);
		CK_ULONG VerifyRecoverLength();
		virtual ByteDynArray VerifyRecoverGetOperationState();
		virtual void VerifyRecoverSetOperationState(ByteArray &OperationState);
	};

	class CSign : public CMechanism
	{
	public:
		CK_OBJECT_HANDLE hSignKey;

		CSign(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CSign();

		virtual bool SignSupportMultipart() = 0;
		virtual void SignInit(CK_OBJECT_HANDLE PrivateKey) = 0;
		virtual void SignReset() = 0;
		virtual void SignUpdate(ByteArray &Part) = 0;
		virtual ByteDynArray SignFinal() = 0;
		virtual CK_ULONG SignLength() = 0;
		virtual ByteDynArray  SignGetOperationState() = 0;
		virtual void SignSetOperationState(ByteArray &OperationState) = 0;
	};

	class CSignRSA : public CSign
	{
	public:
		CSignRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CSignRSA();

		CK_ULONG SignLength();
		bool SignSupportMultipart();
		ByteDynArray SignFinal() = 0;
		virtual ByteDynArray  SignGetOperationState();
		virtual void SignSetOperationState(ByteArray &OperationState);
	};

	class CSignRecover : public CMechanism
	{
	public:
		CK_OBJECT_HANDLE hSignRecoverKey;

		CSignRecover(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CSignRecover();

		virtual void SignRecoverInit(CK_OBJECT_HANDLE PrivateKey) = 0;
		virtual ByteDynArray SignRecover(ByteArray &baData) = 0;
		virtual CK_ULONG SignRecoverLength() = 0;
		virtual ByteDynArray SignRecoverGetOperationState() = 0;
		virtual void SignRecoverSetOperationState(ByteArray &OperationState) = 0;
	};

	class CSignRecoverRSA : public CSignRecover
	{
	public:
		CSignRecoverRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CSignRecoverRSA();

		CK_ULONG SignRecoverLength();
		virtual ByteDynArray SignRecoverGetOperationState();
		virtual void SignRecoverSetOperationState(ByteArray &OperationState);
	};

	/*class CEncrypt : public CMechanism
	{
	public:
		CK_OBJECT_HANDLE hEncryptKey;

		CEncrypt(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CEncrypt();

		virtual bool EncryptSupportMultipart() = 0;
		virtual void EncryptInit(CK_OBJECT_HANDLE PublicKey) = 0;
		virtual ByteDynArray  EncryptUpdate(ByteArray &Data) = 0;
		virtual ByteDynArray  EncryptFinal() = 0;
		virtual CK_ULONG EncryptLength() = 0;
		virtual ByteDynArray EncryptGetOperationState() = 0;
		virtual void EncryptSetOperationState(ByteArray &OperationState) = 0;
	};

	class CEncryptRSA : public CEncrypt
	{
	public:
		CEncryptRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CEncryptRSA();

		bool EncryptSupportMultipart();
		CK_ULONG EncryptLength();
		ByteDynArray EncryptCompute(ByteArray &baPlainData);
		ByteDynArray EncryptGetOperationState();
		void EncryptSetOperationState(ByteArray &OperationState);
	};*/


	/*class CDecrypt : public CMechanism
	{
		static uint8_t uninitializedCacheData;
	public:
		CK_OBJECT_HANDLE hDecryptKey;

		CDecrypt(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CDecrypt();

		virtual bool DecryptSupportMultipart() = 0;
		virtual void DecryptInit(CK_OBJECT_HANDLE PrivateKey) = 0;
		virtual ByteDynArray  DecryptUpdate(ByteArray &EncryptedData) = 0;
		virtual ByteDynArray DecryptFinal() = 0;
		virtual CK_ULONG DecryptLength() = 0;
		virtual ByteDynArray DecryptRemovePadding(ByteArray &paddedData) = 0;
		virtual ByteDynArray  DecryptGetOperationState() = 0;
		virtual void DecryptSetOperationState(ByteArray &OperationState) = 0;

		ByteDynArray resultCache;
		ByteArray cacheData;
		bool checkCache(ByteArray &Data, ByteArray &Result);
		void setCache(ByteArray &Data, ByteArray &Result);
	};

	class CDecryptRSA : public CDecrypt
	{
	public:
		CDecryptRSA(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session);
		virtual ~CDecryptRSA();

		bool DecryptSupportMultipart();
		CK_ULONG DecryptLength();
		ByteDynArray  DecryptGetOperationState();
		void DecryptSetOperationState(ByteArray &OperationState);
	};*/

	class CDigestSHA : public CDigest
	{
	public:
		CDigestSHA(std::shared_ptr<CSession> Session);
		virtual ~CDigestSHA();

		CSHA1 sha1;

		void DigestInit();
		void DigestUpdate(ByteArray &Part);
		void DigestFinal(ByteArray &Digest);
		CK_ULONG DigestLength();
		ByteArray DigestInfo();
		ByteDynArray  DigestGetOperationState();
		void DigestSetOperationState(ByteArray &OperationState);
	};

	class CDigestSHA256 : public CDigest
	{
	public:
		CDigestSHA256(std::shared_ptr<CSession> Session);
		virtual ~CDigestSHA256();

		ByteDynArray data;
		CSHA256 sha256;

		void DigestInit();
		void DigestUpdate(ByteArray &Part);
		void DigestFinal(ByteArray &Digest);
		CK_ULONG DigestLength();
		ByteArray DigestInfo();
		ByteDynArray  DigestGetOperationState();
		void DigestSetOperationState(ByteArray &OperationState);
	};

	class CDigestMD5 : public CDigest
	{
	public:
		CDigestMD5(std::shared_ptr<CSession> Session);
		virtual ~CDigestMD5();

		CMD5 md5;

		void DigestInit();
		void DigestUpdate(ByteArray &Part);
		void DigestFinal(ByteArray &Digest);
		CK_ULONG DigestLength();
		ByteArray DigestInfo();
		ByteDynArray  DigestGetOperationState();
		void DigestSetOperationState(ByteArray &OperationState);
	};

	/*class CRSA_X509 : public CSignRSA, public CSignRecoverRSA, public CVerifyRSA, public CVerifyRecoverRSA, public CEncryptRSA, public CDecryptRSA
	{
	public:
		CRSA_X509(std::shared_ptr<CSession> Session);
		virtual ~CRSA_X509();

		ByteDynArray baVerifyBuffer;
		ByteDynArray baSignBuffer;
		ByteDynArray baEncryptBuffer;
		ByteDynArray baDecryptBuffer;

		void VerifyInit(CK_OBJECT_HANDLE PublicKey);
		void VerifyUpdate(ByteArray &Part);
		void VerifyFinal(ByteArray &Signature);

		void VerifyRecoverInit(CK_OBJECT_HANDLE PublicKey);
		ByteDynArray VerifyRecover(ByteArray &Signature);

		void SignInit(CK_OBJECT_HANDLE PrivateKey);
		void SignReset();
		void SignUpdate(ByteArray &Part);
		ByteDynArray SignFinal();

		void SignRecoverInit(CK_OBJECT_HANDLE PrivateKey);
		ByteDynArray SignRecover(ByteArray &baData);

		void EncryptInit(CK_OBJECT_HANDLE PublicKey);
		ByteDynArray  EncryptUpdate(ByteArray &Data);
		ByteDynArray  EncryptFinal();

		void DecryptInit(CK_OBJECT_HANDLE PrivateKey);
		ByteDynArray  DecryptUpdate(ByteArray &EncryptedData);
		ByteDynArray DecryptFinal();
		ByteDynArray DecryptRemovePadding(ByteArray &paddedData);

	};*/

	class CRSA_PKCS1 : public CSignRSA, public CSignRecoverRSA, public CVerifyRSA, public CVerifyRecoverRSA/*, public CEncryptRSA, public CDecryptRSA*/
	{
	public:
		CRSA_PKCS1(std::shared_ptr<CSession> Session);
		virtual ~CRSA_PKCS1();

		ByteDynArray baVerifyBuffer;
		ByteDynArray baSignBuffer;
		/*ByteDynArray baEncryptBuffer;
		ByteDynArray baDecryptBuffer;*/

		void VerifyInit(CK_OBJECT_HANDLE PublicKey);
		void VerifyUpdate(ByteArray &Part);
		void VerifyFinal(ByteArray &Signature);

		void VerifyRecoverInit(CK_OBJECT_HANDLE PublicKey);
		ByteDynArray VerifyRecover(ByteArray &Signature);

		void SignInit(CK_OBJECT_HANDLE PrivateKey);
		void SignReset();
		void SignUpdate(ByteArray &Part);
		ByteDynArray SignFinal();

		void SignRecoverInit(CK_OBJECT_HANDLE PrivateKey);
		ByteDynArray SignRecover(ByteArray &baData);

		/*void EncryptInit(CK_OBJECT_HANDLE PublicKey);
		ByteDynArray  EncryptUpdate(ByteArray &Data);
		ByteDynArray  EncryptFinal();

		void DecryptInit(CK_OBJECT_HANDLE PrivateKey);
		ByteDynArray  DecryptUpdate(ByteArray &EncryptedData);
		ByteDynArray DecryptFinal();
		ByteDynArray DecryptRemovePadding(ByteArray &paddedData);*/
	};

	class CSignRSAwithDigest : public CSignRSA
	{
	public:
		CSignRSAwithDigest(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session, CDigest *Digest);
		virtual ~CSignRSAwithDigest();

		CDigest *pDigest;
		bool SignSupportMultipart();
		void SignInit(CK_OBJECT_HANDLE PrivateKey);
		void SignReset();
		void SignUpdate(ByteArray &Part);
		ByteDynArray SignFinal();
		ByteDynArray  SignGetOperationState();
		void SignSetOperationState(ByteArray &OperationState);
	};

	class CVerifyRSAwithDigest : public CVerifyRSA
	{
	public:
		CVerifyRSAwithDigest(CK_MECHANISM_TYPE type, std::shared_ptr<CSession> Session, CDigest *Digest);
		virtual ~CVerifyRSAwithDigest();

		CDigest *pDigest;
		bool VerifySupportMultipart();
		void VerifyInit(CK_OBJECT_HANDLE PublicKey);
		void VerifyUpdate(ByteArray &Part);
		void VerifyFinal(ByteArray &Signature);
		ByteDynArray VerifyGetOperationState();
		void VerifySetOperationState(ByteArray &OperationState);
	};

	class CRSAwithMD5 : public CSignRSAwithDigest, public CVerifyRSAwithDigest
	{
	public:
		CRSAwithMD5(std::shared_ptr<CSession> Session);
		virtual ~CRSAwithMD5();

		CDigestMD5 md5;
	};

	class CRSAwithSHA1 : public CSignRSAwithDigest, public CVerifyRSAwithDigest
	{
	public:
		CRSAwithSHA1(std::shared_ptr<CSession> Session);
		virtual ~CRSAwithSHA1();

		CDigestSHA sha1;
	};

	class CRSAwithSHA256 : public CSignRSAwithDigest, public CVerifyRSAwithDigest
	{
	public:
		CRSAwithSHA256(std::shared_ptr<CSession> Session);
		virtual ~CRSAwithSHA256();

		CDigestSHA256 sha256;
	};

}