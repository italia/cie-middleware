#pragma once

#pragma pack(1)
#include "pkcs11.h"
#pragma pack()

#include "mechanism.h"
#include "slot.h"
#include "../PCSC/token.h"
#include <winscard.h>
#include "p11object.h"

namespace p11 {

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

typedef std::map<CK_SESSION_HANDLE,class CSession*> SessionMap;

class CCardTemplate;
class CP11PublicKey;
class CP11PrivateKey;

class CSession 
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

	CSlot *pSlot;
	CSession();
	~CSession();
	static RESULT GetSessionFromID(CK_SESSION_HANDLE hSessionHandle,CSession *&pSession);
	static RESULT AddSession(CSession* pSession,CK_SESSION_HANDLE &phSession);
	static RESULT DeleteSession(CK_SESSION_HANDLE hSessionHandle);
	static RESULT GetNewSessionID(CK_SLOT_ID &hSlotID);

	CK_RV GenerateRandom(ByteArray &RandomData);

	CK_RV FindObjectsInit(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
	CK_RV FindObjects(CK_OBJECT_HANDLE_PTR phObject,CK_ULONG ulMaxObjectCount,CK_ULONG_PTR pulObjectCount);
	CK_RV FindObjectsFinal();
	std::vector<CK_OBJECT_HANDLE> findResult;
	bool bFindInit;

	CK_RV Login(CK_USER_TYPE userType, CK_CHAR_PTR pPin, CK_ULONG ulPinLen);
	CK_RV Logout();

	CK_RV InitPIN(ByteArray &Pin);
	CK_RV SetPIN(ByteArray &OldPin,ByteArray &NewPin);

	CK_RV SetAttributeValue(CK_OBJECT_HANDLE hObject,CK_ATTRIBUTE_PTR pTemplate,CK_ULONG ulCount);
	CK_RV GetAttributeValue(CK_OBJECT_HANDLE hObject,CK_ATTRIBUTE_PTR pTemplate,CK_ULONG ulCount);
	CK_RV GetObjectSize(CK_OBJECT_HANDLE hObject,CK_ULONG_PTR pulSize);
	CK_RV CreateObject(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR phObject);
	CK_RV DestroyObject(CK_OBJECT_HANDLE hObject);

	CK_RV GenerateKey(CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR phKey);
	CK_RV GenerateKeyPair(CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount, CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount, CK_OBJECT_HANDLE_PTR phPublicKey, CK_OBJECT_HANDLE_PTR phPrivateKey);

	CK_RV DigestInit(CK_MECHANISM_PTR pMechanism);
	CK_RV Digest(ByteArray &Data, ByteArray &Digest);
	CK_RV DigestUpdate(ByteArray &Data);
	CK_RV DigestFinal(ByteArray &Digest);
	CDigest *pDigestMechanism;

	CK_RV VerifyInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	CK_RV Verify(ByteArray &Data, ByteArray &Signature);
	CK_RV VerifyUpdate(ByteArray &Data);
	CK_RV VerifyFinal(ByteArray &Signature);
	CVerify *pVerifyMechanism;

	CK_RV VerifyRecoverInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	CK_RV VerifyRecover(ByteArray &Signature, ByteArray &Data);
	CVerifyRecover *pVerifyRecoverMechanism;

	CK_RV SignInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	CK_RV Sign(ByteArray &Data, ByteArray &Signature);
	CK_RV SignUpdate(ByteArray &Data);
	CK_RV SignFinal(ByteArray &Signature);
	CSign *pSignMechanism;

	CK_RV SignRecoverInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	CK_RV SignRecover(ByteArray &Data, ByteArray &Signature);
	CSignRecover *pSignRecoverMechanism;

	CK_RV EncryptInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	CK_RV Encrypt(ByteArray &Data, ByteArray &EncryptedData);
	CK_RV EncryptUpdate(ByteArray &Data,ByteArray &EncryptedData);
	CK_RV EncryptFinal(ByteArray &EncryptedData);
	CEncrypt *pEncryptMechanism;

	CK_RV DecryptInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
	CK_RV Decrypt(ByteArray &EncryptedData, ByteArray &DataData);
	CK_RV DecryptUpdate(ByteArray &EncryptedData,ByteArray &Data);
	CK_RV DecryptFinal(ByteArray &Data);
	CDecrypt *pDecryptMechanism;

	CK_RV SetOperationState(ByteArray &OperationState);
	CK_RV GetOperationState(ByteArray &OperationState);

	CK_RV UnblockSecAuthPIN(ByteArray &KeyLabel, ByteArray &Puk,ByteArray &NewPin);

	RESULT ExistsRO(bool &bExistsRO);
	RESULT ExistsSO_RW(bool &bExists);

};

}

