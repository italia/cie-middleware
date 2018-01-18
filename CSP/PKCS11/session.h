#pragma once

#pragma pack(1)
#include "pkcs11.h"
#pragma pack()

#include "mechanism.h"
#include "slot.h"
#include "../PCSC/token.h"
#include <winscard.h>
#include "p11object.h"
#include <memory>

namespace p11 {

class p11_error : public logged_error {
	CK_RV p11ErrorCode;
public:
	p11_error(CK_RV p11ErrorCode, const char *message) : p11ErrorCode(p11ErrorCode), logged_error(message) {}
	p11_error(CK_RV p11ErrorCode) : p11_error(p11ErrorCode,stdPrintf("%s:%08x","Errore PKCS11", p11ErrorCode).c_str()) {}
	CK_RV getP11ErrorCode() { return p11ErrorCode; }
};


enum OperationStateTag {
	OS_Flags,
	OS_User,
	OS_Digest,
	OS_Sign,
	OS_Verify,
	OS_Encrypt,
	OS_Decrypt,
	OS_Algo,
	OS_Data,
	OS_Key
};

typedef std::map<CK_SESSION_HANDLE,std::shared_ptr<CSession>> SessionMap;

class CCardTemplate;
class CP11PublicKey;
class CP11PrivateKey;

class CSession : public std::enable_shared_from_this<CSession>
{
private:
	static DWORD dwSessionCnt;
public:
	static SessionMap g_mSessions;

	CK_SESSION_HANDLE hSessionHandle;
	CK_SLOT_ID slotID;
	CK_FLAGS flags;
	CK_VOID_PTR pApplication;
	CK_NOTIFY notify;

	std::shared_ptr<CSlot> pSlot;
	CSession();
	static std::shared_ptr<CSession> GetSessionFromID(CK_SESSION_HANDLE hSessionHandle);
	static CK_SESSION_HANDLE AddSession(std::unique_ptr<CSession> pSession);
	static void DeleteSession(CK_SESSION_HANDLE hSessionHandle);
	static CK_SLOT_ID GetNewSessionID();

	void GenerateRandom(ByteArray &RandomData);

	void FindObjectsInit(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
	void FindObjects(CK_OBJECT_HANDLE_PTR phObject,CK_ULONG ulMaxObjectCount,CK_ULONG_PTR pulObjectCount);
	void FindObjectsFinal();
	std::vector<CK_OBJECT_HANDLE> findResult;
	bool bFindInit;

	void Login(CK_USER_TYPE userType, CK_CHAR_PTR pPin, CK_ULONG ulPinLen);
	void Logout();

	void InitPIN(ByteArray &Pin);
	void SetPIN(ByteArray &OldPin,ByteArray &NewPin);

	void SetAttributeValue(CK_OBJECT_HANDLE hObject,CK_ATTRIBUTE_PTR pTemplate,CK_ULONG ulCount);
	void GetAttributeValue(CK_OBJECT_HANDLE hObject,CK_ATTRIBUTE_PTR pTemplate,CK_ULONG ulCount);
	CK_ULONG GetObjectSize(CK_OBJECT_HANDLE hObject);
	CK_OBJECT_HANDLE CreateObject(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
	void DestroyObject(CK_OBJECT_HANDLE hObject);

	CK_OBJECT_HANDLE GenerateKey(CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
	void GenerateKeyPair(CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount, CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount, CK_OBJECT_HANDLE_PTR phPublicKey, CK_OBJECT_HANDLE_PTR phPrivateKey);

	void DigestInit(CK_MECHANISM_PTR pMechanism);
	void Digest(ByteArray &Data, ByteArray &Digest);
	void DigestUpdate(ByteArray &Data);
	void DigestFinal(ByteArray &Digest);
	std::unique_ptr<CDigest> pDigestMechanism;

	void VerifyInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	void Verify(ByteArray &Data, ByteArray &Signature);
	void VerifyUpdate(ByteArray &Data);
	void VerifyFinal(ByteArray &Signature);
	std::unique_ptr<CVerify> pVerifyMechanism;

	void VerifyRecoverInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	void VerifyRecover(ByteArray &Signature, ByteArray &Data);
	std::unique_ptr<CVerifyRecover> pVerifyRecoverMechanism;

	void SignInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	void Sign(ByteArray &Data, ByteArray &Signature);
	void SignUpdate(ByteArray &Data);
	void SignFinal(ByteArray &Signature);
	std::unique_ptr<CSign> pSignMechanism;

	void SignRecoverInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	void SignRecover(ByteArray &Data, ByteArray &Signature);
	std::unique_ptr<CSignRecover> pSignRecoverMechanism;

	void EncryptInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	void Encrypt(ByteArray &Data, ByteArray &EncryptedData);
	void EncryptUpdate(ByteArray &Data,ByteArray &EncryptedData);
	void EncryptFinal(ByteArray &EncryptedData);
	std::unique_ptr<CEncrypt> pEncryptMechanism;

	void DecryptInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	void Decrypt(ByteArray &EncryptedData, ByteArray &DataData);
	void DecryptUpdate(ByteArray &EncryptedData,ByteArray &Data);
	void DecryptFinal(ByteArray &Data);
	std::unique_ptr<CDecrypt> pDecryptMechanism;

	void SetOperationState(ByteArray &OperationState);
	void GetOperationState(ByteArray &OperationState);

	bool ExistsRO();
	bool ExistsSO_RW();

};

}

