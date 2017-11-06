#pragma once
#include <openssl\sha.h>
#include <openssl\md5.h>
#pragma pack(1)
#include "pkcs11.h"
#pragma pack()

#include <memory>

namespace p11 {

class CSession;

class CMechanism
{
public:
	CK_MECHANISM_TYPE mtType;
	CMechanism();
	CMechanism(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CMechanism(void);
	std::shared_ptr<CSession> pSession;
};

class CDigest : public CMechanism
{
public:
	CDigest();
	CDigest(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CDigest();

	virtual RESULT DigestInit()=0;
	virtual RESULT DigestUpdate(ByteArray &Part)=0;
	virtual RESULT DigestFinal(ByteArray &Digest)=0;
	virtual RESULT DigestLength(CK_ULONG_PTR pulDigestLen)=0;
	virtual RESULT DigestInfo(ByteArray *&pbaDigestInfo)=0;
	virtual RESULT DigestGetOperationState(ByteDynArray &OperationState)=0;
	virtual RESULT DigestSetOperationState(ByteArray &OperationState)=0;
};

class CVerify : public CMechanism
{
public:
	CK_OBJECT_HANDLE hVerifyKey;

	CVerify(void);
	CVerify(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CVerify();

	virtual RESULT VerifySupportMultipart(bool &Support)=0;
	virtual RESULT VerifyInit(CK_OBJECT_HANDLE PublicKey)=0;
	virtual RESULT VerifyUpdate(ByteArray &Part)=0;
	virtual RESULT VerifyFinal(ByteArray &Signature)=0;
	virtual RESULT VerifyLength(CK_ULONG_PTR pulVerifyLen)=0;
	virtual RESULT VerifyDecryptSignature(ByteArray &Signature,ByteDynArray &baDecryptedSignature)=0;
	virtual RESULT VerifyGetOperationState(ByteDynArray &OperationState)=0;
	virtual RESULT VerifySetOperationState(ByteArray &OperationState)=0;
};

class CVerifyRSA : public CVerify
{
public:
	CVerifyRSA(void);
	CVerifyRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CVerifyRSA();

	RESULT VerifySupportMultipart(bool &Support);
	RESULT VerifyDecryptSignature(ByteArray &Signature,ByteDynArray &baDecryptedSignature);
	RESULT VerifyLength(CK_ULONG_PTR pulVerifyLen);
	RESULT VerifyGetOperationState(ByteDynArray &OperationState);
	RESULT VerifySetOperationState(ByteArray &OperationState);
};

class CVerifyRecover : public CMechanism
{
public:
	CK_OBJECT_HANDLE hVerifyRecoverKey;

	CVerifyRecover(void);
	CVerifyRecover(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CVerifyRecover();

	virtual RESULT VerifyRecoverInit(CK_OBJECT_HANDLE PublicKey)=0;
	virtual RESULT VerifyRecover(ByteArray &Signature,ByteDynArray &baData)=0;
	virtual RESULT VerifyRecoverLength(CK_ULONG_PTR pulVerifyLen)=0;
	virtual RESULT VerifyRecoverDecryptSignature(ByteArray &Signature,ByteDynArray &baDecryptedSignature)=0;
	virtual RESULT VerifyRecoverGetOperationState(ByteDynArray &OperationState)=0;
	virtual RESULT VerifyRecoverSetOperationState(ByteArray &OperationState)=0;
};

class CVerifyRecoverRSA : public CVerifyRecover
{
public:
	CVerifyRecoverRSA(void);
	CVerifyRecoverRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CVerifyRecoverRSA();

	RESULT VerifyRecoverDecryptSignature(ByteArray &Signature,ByteDynArray &baDecryptedSignature);
	RESULT VerifyRecoverLength(CK_ULONG_PTR pulVerifyLen);
	RESULT VerifyRecoverGetOperationState(ByteDynArray &OperationState);
	RESULT VerifyRecoverSetOperationState(ByteArray &OperationState);
};

class CSign : public CMechanism
{
public:
	CK_OBJECT_HANDLE hSignKey;

	CSign(void);
	CSign(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CSign();

	virtual RESULT SignSupportMultipart(bool &Support)=0;
	virtual RESULT SignInit(CK_OBJECT_HANDLE PrivateKey)=0;
	virtual RESULT SignReset()=0;
	virtual RESULT SignUpdate(ByteArray &Part)=0;
	virtual RESULT SignFinal(ByteDynArray &baSignBuffer)=0;
	virtual RESULT SignLength(CK_ULONG_PTR pulSignatureLen)=0;
	virtual RESULT SignGetOperationState(ByteDynArray &OperationState)=0;
	virtual RESULT SignSetOperationState(ByteArray &OperationState)=0;
};

class CSignRSA : public CSign
{
public:
	CSignRSA(void);
	CSignRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CSignRSA();

	RESULT SignSupportMultipart(bool &Support);
	RESULT SignLength(CK_ULONG_PTR pulSignatureLen);
	virtual RESULT SignGetOperationState(ByteDynArray &OperationState);
	virtual RESULT SignSetOperationState(ByteArray &OperationState);
};

class CSignRecover : public CMechanism
{
public:
	CK_OBJECT_HANDLE hSignRecoverKey;

	CSignRecover(void);
	CSignRecover(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CSignRecover();

	virtual RESULT SignRecoverInit(CK_OBJECT_HANDLE PrivateKey)=0;
	virtual RESULT SignRecover(ByteArray &baData,ByteDynArray &baSignBuffer)=0;
	virtual RESULT SignRecoverLength(CK_ULONG_PTR pulSignatureLen)=0;
	virtual RESULT SignRecoverGetOperationState(ByteDynArray &OperationState)=0;
	virtual RESULT SignRecoverSetOperationState(ByteArray &OperationState)=0;
};

class CSignRecoverRSA : public CSignRecover
{
public:
	CSignRecoverRSA(void);
	CSignRecoverRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CSignRecoverRSA();

	RESULT SignRecoverLength(CK_ULONG_PTR pulSignatureLen);
	virtual RESULT SignRecoverGetOperationState(ByteDynArray &OperationState);
	virtual RESULT SignRecoverSetOperationState(ByteArray &OperationState);
};

class CEncrypt : public CMechanism
{
public:
	CK_OBJECT_HANDLE hEncryptKey;

	CEncrypt(void);
	CEncrypt(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CEncrypt();

	virtual RESULT EncryptSupportMultipart(bool &Support)=0;
	virtual RESULT EncryptInit(CK_OBJECT_HANDLE PublicKey)=0;
	virtual RESULT EncryptUpdate(ByteArray &Data,ByteDynArray &EncryptedData)=0;
	virtual RESULT EncryptFinal(ByteDynArray &baEncryptBuffer)=0;
	virtual RESULT EncryptLength(CK_ULONG_PTR pulEncryptLen)=0;
	virtual RESULT EncryptGetOperationState(ByteDynArray &OperationState)=0;
	virtual RESULT EncryptSetOperationState(ByteArray &OperationState)=0;
};

class CEncryptRSA : public CEncrypt
{
public:
	CEncryptRSA(void);
	CEncryptRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CEncryptRSA();

	RESULT EncryptSupportMultipart(bool &Support);
	RESULT EncryptLength(CK_ULONG_PTR pulEncryptLen);
	RESULT EncryptCompute(ByteArray &baPlainData,ByteDynArray &baEncryptedData);
	RESULT EncryptGetOperationState(ByteDynArray &OperationState);
	RESULT EncryptSetOperationState(ByteArray &OperationState);
};

class CDecrypt : public CMechanism
{
	static BYTE uninitializedCacheData;
public:
	CK_OBJECT_HANDLE hDecryptKey;

	CDecrypt(void);
	CDecrypt(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CDecrypt();

	virtual RESULT DecryptSupportMultipart(bool &Support)=0;
	virtual RESULT DecryptInit(CK_OBJECT_HANDLE PrivateKey)=0;
	virtual RESULT DecryptUpdate(ByteArray &EncryptedData,ByteDynArray &Data)=0;
	virtual RESULT DecryptFinal(ByteDynArray &baDecryptBuffer)=0;
	virtual RESULT DecryptLength(CK_ULONG_PTR pulDecryptLen)=0;
	virtual RESULT DecryptRemovePadding(ByteArray &paddedData,ByteDynArray &unpaddedData)=0;
	virtual RESULT DecryptGetOperationState(ByteDynArray &OperationState)=0;
	virtual RESULT DecryptSetOperationState(ByteArray &OperationState)=0;

	ByteDynArray resultCache;
	ByteArray cacheData;
	RESULT checkCache(ByteArray &Data,ByteArray &Result,bool &bFound);
	RESULT setCache(ByteArray &Data,ByteArray &Result);
};

class CDecryptRSA : public CDecrypt
{
public:
	CDecryptRSA(void);
	CDecryptRSA(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session);
	virtual ~CDecryptRSA();

	RESULT DecryptSupportMultipart(bool &Support);
	RESULT DecryptLength(CK_ULONG_PTR pulDecryptLen);
	RESULT DecryptGetOperationState(ByteDynArray &OperationState);
	RESULT DecryptSetOperationState(ByteArray &OperationState);
};

class CSHA : public CDigest
{
public:
	CSHA(void);
	CSHA(std::shared_ptr<CSession> Session);
	virtual ~CSHA();

	SHA_CTX Sha1Context;

	RESULT DigestInit();
	RESULT DigestUpdate(ByteArray &Part);
	RESULT DigestFinal(ByteArray &Digest);
	RESULT DigestLength(CK_ULONG_PTR pulDigestLen);
	RESULT DigestInfo(ByteArray *&pbaDigestInfo);
	RESULT DigestGetOperationState(ByteDynArray &OperationState);
	RESULT DigestSetOperationState(ByteArray &OperationState);
};

class CMD5 : public CDigest
{
public:
	CMD5();
	CMD5(std::shared_ptr<CSession> Session);
	virtual ~CMD5();

	MD5_CTX	MD5Context;

	RESULT DigestInit();
	RESULT DigestUpdate(ByteArray &Part);
	RESULT DigestFinal(ByteArray &Digest);
	RESULT DigestLength(CK_ULONG_PTR pulDigestLen);
	RESULT DigestInfo(ByteArray *&pbaDigestInfo);
	RESULT DigestGetOperationState(ByteDynArray &OperationState);
	RESULT DigestSetOperationState(ByteArray &OperationState);
};

class CRSA_X509 : public CSignRSA,public CSignRecoverRSA,public CVerifyRSA,public CVerifyRecoverRSA,public CEncryptRSA,public CDecryptRSA
{
public:
	CRSA_X509();
	CRSA_X509(std::shared_ptr<CSession> Session);
	virtual ~CRSA_X509();

	ByteDynArray baVerifyBuffer;
	ByteDynArray baSignBuffer;
	ByteDynArray baEncryptBuffer;
	ByteDynArray baDecryptBuffer;

	RESULT VerifyInit(CK_OBJECT_HANDLE PublicKey);
	RESULT VerifyUpdate(ByteArray &Part);
	RESULT VerifyFinal(ByteArray &Signature);
	
	RESULT VerifyRecoverInit(CK_OBJECT_HANDLE PublicKey);
	RESULT VerifyRecover(ByteArray &Signature,ByteDynArray &baData);

	RESULT SignInit(CK_OBJECT_HANDLE PrivateKey);
	RESULT SignReset();
	RESULT SignUpdate(ByteArray &Part);
	RESULT SignFinal(ByteDynArray &baSignBuffer);
	
	RESULT SignRecoverInit(CK_OBJECT_HANDLE PrivateKey);
	RESULT SignRecover(ByteArray &baData,ByteDynArray &baSignBuffer);

	RESULT EncryptInit(CK_OBJECT_HANDLE PublicKey);
	RESULT EncryptUpdate(ByteArray &Part,ByteDynArray &EncryptedData);
	RESULT EncryptFinal(ByteDynArray &baEncryptBuffer);

	RESULT DecryptInit(CK_OBJECT_HANDLE PrivateKey);
	RESULT DecryptUpdate(ByteArray &Part,ByteDynArray &Data);
	RESULT DecryptFinal(ByteDynArray &baDecryptBuffer);
	RESULT DecryptRemovePadding(ByteArray &paddedData,ByteDynArray &unpaddedData);
};

class CRSA_PKCS1 : public CSignRSA,public CSignRecoverRSA,public CVerifyRSA,public CVerifyRecoverRSA,public CEncryptRSA,public CDecryptRSA
{
public:
	CRSA_PKCS1();
	CRSA_PKCS1(std::shared_ptr<CSession> Session);
	virtual ~CRSA_PKCS1();

	ByteDynArray baVerifyBuffer;
	ByteDynArray baSignBuffer;
	ByteDynArray baEncryptBuffer;
	ByteDynArray baDecryptBuffer;

	RESULT VerifyInit(CK_OBJECT_HANDLE PublicKey);
	RESULT VerifyUpdate(ByteArray &Part);
	RESULT VerifyFinal(ByteArray &Signature);

	RESULT VerifyRecoverInit(CK_OBJECT_HANDLE PublicKey);
	RESULT VerifyRecover(ByteArray &Signature,ByteDynArray &baData);

	RESULT SignInit(CK_OBJECT_HANDLE PrivateKey);
	RESULT SignReset();
	RESULT SignUpdate(ByteArray &Part);
	RESULT SignFinal(ByteDynArray &baSignBuffer);

	RESULT SignRecoverInit(CK_OBJECT_HANDLE PrivateKey);
	RESULT SignRecover(ByteArray &baData,ByteDynArray &baSignBuffer);

	RESULT EncryptInit(CK_OBJECT_HANDLE PublicKey);
	RESULT EncryptUpdate(ByteArray &Part,ByteDynArray &EncryptedData);
	RESULT EncryptFinal(ByteDynArray &baEncryptBuffer);

	RESULT DecryptInit(CK_OBJECT_HANDLE PrivateKey);
	RESULT DecryptUpdate(ByteArray &Part,ByteDynArray &Data);
	RESULT DecryptFinal(ByteDynArray &baDecryptBuffer);
	RESULT DecryptRemovePadding(ByteArray &paddedData,ByteDynArray &unpaddedData);
};	

class CSignRSAwithDigest : public CSignRSA
{
public:
	CSignRSAwithDigest(void);
	CSignRSAwithDigest(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session,CDigest *Digest);
	virtual ~CSignRSAwithDigest();
	
	CDigest *pDigest;
	RESULT SignSupportMultipart(bool &Support);
	RESULT SignInit(CK_OBJECT_HANDLE PrivateKey);
	RESULT SignReset();
	RESULT SignUpdate(ByteArray &Part);
	RESULT SignFinal(ByteDynArray &baSignBuffer);
	RESULT SignGetOperationState(ByteDynArray &OperationState);
	RESULT SignSetOperationState(ByteArray &OperationState);
};

class CVerifyRSAwithDigest : public CVerifyRSA
{
public:
	CVerifyRSAwithDigest(void);
	CVerifyRSAwithDigest(CK_MECHANISM_TYPE type,std::shared_ptr<CSession> Session,CDigest *Digest);
	virtual ~CVerifyRSAwithDigest();
	
	CDigest *pDigest;
	RESULT VerifySupportMultipart(bool &Support);
	RESULT VerifyInit(CK_OBJECT_HANDLE PublicKey);
	RESULT VerifyUpdate(ByteArray &Part);
	RESULT VerifyFinal(ByteArray &Signature);
	RESULT VerifyGetOperationState(ByteDynArray &OperationState);
	RESULT VerifySetOperationState(ByteArray &OperationState);
};

class CRSAwithMD5 : public CSignRSAwithDigest,public CVerifyRSAwithDigest
{
public:
	CRSAwithMD5();
	CRSAwithMD5(std::shared_ptr<CSession> Session);
	virtual ~CRSAwithMD5();

	CMD5 md5;
};

class CRSAwithSHA1 : public CSignRSAwithDigest,public CVerifyRSAwithDigest
{
public:
	CRSAwithSHA1();
	CRSAwithSHA1(std::shared_ptr<CSession> Session);
	virtual ~CRSAwithSHA1();

	CSHA sha1;
};

}