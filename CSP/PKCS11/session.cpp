#include "../StdAfx.h"
#include "session.h"
#include "cardtemplate.h"
#include "../util/util.h"
#include "../crypto\rsa.h"
#include "../util/tlv.h"

static char *szCompiledFile = __FILE__;


namespace {
	template<class T>
	class resetter;

	template<class T>
	resetter<T> make_resetter(T&) noexcept;

	template<class T>
	class resetter<std::unique_ptr<T>>
	{
	private:
		std::unique_ptr<T> * m_p;

		friend resetter<std::unique_ptr<T>> make_resetter<std::unique_ptr<T>>(std::unique_ptr<T>& p) noexcept;
		resetter(std::unique_ptr<T>& p) noexcept : m_p(&p) {}

		void reset() noexcept(noexcept(m_p->reset()))
		{
			if (m_p)
				m_p->reset();

			m_p = nullptr;
		}

	public:
		resetter(const resetter&) = delete;
		resetter(resetter&& other) noexcept : m_p(std::exchange(other.m_p, nullptr)) {}

		resetter& operator=(const resetter&) = delete;

		resetter& operator=(resetter&& other) noexcept(noexcept(reset()))
		{
			reset();
			m_p = std::exchange(other.m_p, nullptr);
			return *this;
		}

		~resetter() noexcept(noexcept(reset()))
		{
			reset();
		}

		void release() noexcept
		{
			m_p = nullptr;
		}
	};

	template<class T>
	resetter<T> make_resetter(T& p) noexcept
	{
		return resetter<T>(p);
	}

}

namespace p11 {

	DWORD CSession::dwSessionCnt = 0;
	SessionMap CSession::g_mSessions;

	CSession::CSession()
	{
		pSlot = NULL;
		bFindInit = false;
	}


	CK_SLOT_ID CSession::GetNewSessionID() {
		init_func
		return InterlockedIncrement(&dwSessionCnt);
	}

	CK_SESSION_HANDLE CSession::AddSession(std::unique_ptr<CSession> pSession)
	{
		init_func
		pSession->hSessionHandle = GetNewSessionID();
		auto id = pSession->hSessionHandle;

		pSession->pSlot->pTemplate->FunctionList.templateInitSession(pSession->pSlot->pTemplateData);

		pSession->pSlot->dwSessionCount++;
		g_mSessions.insert(std::make_pair(pSession->hSessionHandle, std::move(pSession)));

		return id;
	}

	void CSession::DeleteSession(CK_SESSION_HANDLE hSessionHandle)
	{
		init_func
		std::shared_ptr<CSession> pSession=GetSessionFromID(hSessionHandle);

		ER_ASSERT(pSession != nullptr, ERR_SESSION_NOT_OPENED);

		pSession->pSlot->dwSessionCount--;
		if (pSession->pSlot->dwSessionCount == 0) {
			if (pSession->pSlot->User != CKU_NOBODY) {
				pSession->Logout();
			}
		}

		pSession->pSlot->pTemplate->FunctionList.templateFinalSession(pSession->pSlot->pTemplateData);
		g_mSessions.erase(hSessionHandle);
	}

	std::shared_ptr<CSession> CSession::GetSessionFromID(CK_SESSION_HANDLE hSessionHandle)
	{
		init_func
		SessionMap::const_iterator pPair;
		pPair = g_mSessions.find(hSessionHandle);
		if (pPair == g_mSessions.end())
			return nullptr;
		
		return pPair->second;
	}

	/* ******************* */
	/*    LOGIN e LOGOUT   */
	/* ******************* */

	void CSession::Login(CK_USER_TYPE userType, CK_CHAR_PTR pPin, CK_ULONG ulPinLen)
	{
		init_func

			if (pSlot->User == CKU_USER && userType == CKU_SO)
				throw p11_error(CKR_USER_ANOTHER_ALREADY_LOGGED_IN);
		if (pSlot->User == CKU_SO && userType == CKU_USER)
			throw p11_error(CKR_USER_ANOTHER_ALREADY_LOGGED_IN);
					bool bExistsRO = false;
		if (userType == CKU_SO) {
			if (ExistsRO())
				throw p11_error(CKR_SESSION_READ_ONLY_EXISTS);
		}
		if (pSlot->User != CKU_NOBODY)
			throw p11_error(CKR_USER_ALREADY_LOGGED_IN);

		ByteArray baPin(pPin, ulPinLen);
		pSlot->pTemplate->FunctionList.templateLogin(pSlot->pTemplateData, userType, baPin);

		pSlot->User = userType;
	}

	void CSession::Logout() {
		init_func

			pSlot->pTemplate->FunctionList.templateLogout(pSlot->pTemplateData, pSlot->User);

		for (auto obj = pSlot->P11Objects.begin(); obj != pSlot->P11Objects.end(); obj++) {
				if ((*obj)->IsPrivate())
					pSlot->DelObjectHandle(*obj);
			}
		pSlot->User = CKU_NOBODY;
	}

	/* ******************* */
	/*    Find Objects     */
	/* ******************* */

	void CSession::FindObjectsInit(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
	{
		init_func
			if (bFindInit)
				throw p11_error(CKR_OPERATION_ACTIVE);
		findResult.clear();
		if (ulCount == 0) {
			for (auto obj = pSlot->P11Objects.begin(); obj != pSlot->P11Objects.end(); obj++) {
				try {
					findResult.push_back(pSlot->GetIDFromObject(*obj));
				}
				catch (p11_error &err) {
					if (err.getP11ErrorCode() != CKR_USER_NOT_LOGGED_IN)
						throw;
				}
			}
		}
		else {
			for (auto obj = pSlot->P11Objects.begin(); obj != pSlot->P11Objects.end(); obj++) {
				bool bSkip = false;

				for (unsigned int j = 0; j < ulCount && !bSkip; j++) {
					try {
						ByteArray* attr = (*obj)->getAttribute(pTemplate[j].type);
						if (attr==nullptr)
							bSkip = true;
						else {
							if (attr->size() != pTemplate[j].ulValueLen)
								bSkip = true;
							else
								if ((*attr) != ByteArray((BYTE*)pTemplate[j].pValue, pTemplate[j].ulValueLen))
									bSkip = true;
						}
					}
					catch (p11_error &err) {
						if (err.getP11ErrorCode() != CKR_USER_NOT_LOGGED_IN)
							throw;
						bSkip = true;
					}
				}
				if (!bSkip) {
					try {
						findResult.push_back(pSlot->GetIDFromObject(*obj));
					}
					catch (p11_error &err) {
						if (err.getP11ErrorCode() != CKR_USER_NOT_LOGGED_IN)
							throw;
					}
					/*if (Log.LogParam) {
						ByteArray *Label = NULL;
						pSlot->P11Objects[i]->getAttribute(CKA_LABEL, Label);
						Log.writePure("Object found %i:", i);
						Log.writePure("Class: %x", pSlot->P11Objects[i]->ObjClass);
						if (Label) {
							Log.writePure("Label:");
							info.logParameter(Label->data(), Label->size());
						}
					}*/
				}
			}
		}
		bFindInit = true;
	}

	void CSession::FindObjects(CK_OBJECT_HANDLE_PTR phObject, CK_ULONG ulMaxObjectCount, CK_ULONG_PTR pulObjectCount)
	{
		init_func
		if (!bFindInit)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);
		*pulObjectCount = 0;
		int iCnt = 0;
		while (!findResult.empty() && ulMaxObjectCount>0) {
			phObject[iCnt] = findResult.back();
			findResult.pop_back();
			iCnt++;
			ulMaxObjectCount--;
		}
		*pulObjectCount = iCnt;
	}

	void CSession::FindObjectsFinal()
	{
		init_func
			if (!bFindInit)
				throw p11_error(CKR_OPERATION_NOT_INITIALIZED);
		findResult.clear();
		bFindInit = false;
	}

	void CSession::GetAttributeValue(CK_OBJECT_HANDLE hObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
	{
		init_func

		std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(hObject);
		if (pObject == nullptr)
			throw p11_error(CKR_OBJECT_HANDLE_INVALID);

		pObject->GetAttributeValue(pTemplate, ulCount);
	}

	ByteDynArray GetTemplateValue(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_ATTRIBUTE_TYPE type)
	{
		init_func
		for (unsigned int i = 0; i<ulCount; i++) {
			if (pTemplate[i].type == type) {
				return ByteArray((uint8_t*)pTemplate[i].pValue, pTemplate[i].ulValueLen);
			}
		}
		throw p11_error(CKR_ATTRIBUTE_TYPE_INVALID);
	}

	CK_OBJECT_HANDLE CSession::CreateObject(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount) {
		init_func

		if ((flags & CKF_RW_SESSION) == 0)
			throw p11_error(CKR_SESSION_READ_ONLY);

		if (pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		std::shared_ptr<CP11Object> pObject = pSlot->pTemplate->FunctionList.templateCreateObject(pSlot->pTemplateData, pTemplate, ulCount);

		if (pObject != nullptr)
			return pSlot->GetIDFromObject(pObject);
		else
			throw p11_error(CKR_GENERAL_ERROR);

	}

	CK_OBJECT_HANDLE CSession::GenerateKey(CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount) {
		init_func

			// NON SUPPORTATO DALLA CIE
			throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);

		/*if ((flags & CKF_RW_SESSION) == 0)
			throw p11_error(CKR_SESSION_READ_ONLY);

		if (pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		std::shared_ptr<CP11Object> pKey = pSlot->pTemplate->FunctionList.templateGenerateKey(pSlot->pTemplateData, pMechanism, pTemplate, ulCount);

		if (pKey != nullptr) {
			return pSlot->GetIDFromObject(pKey);
		}
		else
			throw p11_error(CKR_GENERAL_ERROR);*/
	}

	void CSession::GenerateKeyPair(CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount, CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount, CK_OBJECT_HANDLE_PTR phPublicKey, CK_OBJECT_HANDLE_PTR phPrivateKey) {
		init_func

			// NON SUPPORTATO DALLA CIE
			throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);

		/*	if ((flags & CKF_RW_SESSION) == 0)
				throw p11_error(CKR_SESSION_READ_ONLY);

		if (pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		std::shared_ptr<CP11Object> pPublicKey = nullptr;
		std::shared_ptr<CP11Object> pPrivateKey = nullptr;
		pSlot->pTemplate->FunctionList.templateGenerateKeyPair(pSlot->pTemplateData, pMechanism, pPublicKeyTemplate, ulPublicKeyAttributeCount, pPrivateKeyTemplate, ulPrivateKeyAttributeCount, pPublicKey, pPrivateKey);

		if (pPublicKey != nullptr) {
			*phPublicKey = pSlot->GetIDFromObject(pPublicKey);
		}
		else
			throw p11_error(CKR_GENERAL_ERROR);

		if (pPrivateKey != nullptr) {
			*phPrivateKey = pSlot->GetIDFromObject(pPrivateKey);
		}
		else
			throw p11_error(CKR_GENERAL_ERROR);
		*/
	}

	void CSession::DestroyObject(CK_OBJECT_HANDLE hObject) {
		init_func

			if ((flags & CKF_RW_SESSION) == 0)
				throw p11_error(CKR_SESSION_READ_ONLY);

		if (pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(hObject);
		if (pObject == nullptr)
			throw p11_error(CKR_OBJECT_HANDLE_INVALID);

		pSlot->pTemplate->FunctionList.templateDestroyObject(pSlot->pTemplateData, *pObject);
		pSlot->DelP11Object(pObject);
	}

	bool CSession::ExistsRO()
	{
		init_func
		for (SessionMap::const_iterator it = g_mSessions.begin(); it != g_mSessions.end(); it++)
		{
			if (it->second->pSlot == pSlot && (it->second->flags & CKF_RW_SESSION) == 0) {
				return true;
			}
		}

		return false;
	}

	bool CSession::ExistsSO_RW()
	{
		init_func
		if (pSlot->User != CKU_SO)
			return false;
		for (SessionMap::const_iterator it = g_mSessions.begin(); it != g_mSessions.end(); it++)
		{
			if (it->second->pSlot == pSlot && (it->second->flags & CKF_RW_SESSION) != 0)
				return true;
		}

		return false;
	}

	/* ******************* */
	/*       Digest        */
	/* ******************* */

	void CSession::DigestInit(CK_MECHANISM_PTR pMechanism)
	{
		init_func
			if (pDigestMechanism != nullptr)
				throw p11_error(CKR_OPERATION_ACTIVE);

				switch (pMechanism->mechanism) {
				case CKM_SHA_1:
				{
					auto mech = std::unique_ptr<CDigestSHA>(new CDigestSHA(shared_from_this()));
					mech->DigestInit();

					pDigestMechanism = std::move(mech);
					break;
				}
				case CKM_MD5:
				{
					auto mech = std::unique_ptr<CDigestMD5>(new CDigestMD5(shared_from_this()));
					mech->DigestInit();

					pDigestMechanism = std::move(mech);
					break;
				}
				default:
					throw p11_error(CKR_MECHANISM_INVALID);
			}
	}

	void CSession::Digest(ByteArray &Data, ByteArray &Digest)
	{
		init_func

		CK_ULONG ulReqLen = pDigestMechanism->DigestLength();

		if (!Digest.isNull() && Digest.size() < ulReqLen)
			throw p11_error(CKR_BUFFER_TOO_SMALL);

		Digest = Digest.left(ulReqLen);
		if (Digest.isNull())
			return;

		DigestUpdate(Data);
		DigestFinal(Digest);
	
	}

	void CSession::DigestUpdate(ByteArray &Data)
	{
		init_func
			if (pDigestMechanism==nullptr)
				throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		pDigestMechanism->DigestUpdate(Data);
	}

	void CSession::DigestFinal(ByteArray &Digest)
	{
		init_func
			if (pDigestMechanism == nullptr)
				throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		auto mech = std::move(pDigestMechanism);
		CK_ULONG ulReqLen = pDigestMechanism->DigestLength();

		if (!Digest.isNull() && Digest.size()<ulReqLen) {
				pDigestMechanism = std::move(mech);
				throw p11_error(CKR_BUFFER_TOO_SMALL);
			}

		Digest = Digest.left(ulReqLen);
		if (Digest.isNull()) {
			pDigestMechanism = std::move(mech);
			return;
		}
		pDigestMechanism->DigestFinal(Digest);

	}

	/* ******************* */
	/*       Verify        */
	/* ******************* */

	void CSession::VerifyInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	{
		init_func
			if (pVerifyMechanism != nullptr)
				throw p11_error(CKR_OPERATION_ACTIVE);

		std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(hKey);
		if (pObject == nullptr)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		if (pObject->ObjClass != CKO_PUBLIC_KEY)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		auto pVerifyKey = std::static_pointer_cast<CP11PublicKey>(pObject);


		if (pVerifyKey->IsPrivate() && pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		ByteArray *baAttrVal = pVerifyKey->getAttribute(CKA_VERIFY);
		if (baAttrVal==nullptr)
				throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
		else {
			if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
				throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
		}

		switch (pMechanism->mechanism) {
		case CKM_SHA1_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSAwithSHA1>(new CRSAwithSHA1(shared_from_this()));
			mech->VerifyInit(hKey);
			pVerifyMechanism = std::move(mech);
			break;
		}
		case CKM_MD5_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSAwithMD5>(new CRSAwithMD5(shared_from_this()));
			mech->VerifyInit(hKey);
			pVerifyMechanism = std::move(mech);
			break;
		}
		case CKM_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
			mech->VerifyInit(hKey);
			pVerifyMechanism = std::move(mech);
			break;
		}
		/*	NON SUPPORTATO DALLA CIE
		case CKM_RSA_X_509:
		{
			auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
			mech->VerifyInit(hKey);
			pVerifyMechanism = std::move(mech);
			break;
		}*/
		default:
			throw p11_error(CKR_MECHANISM_INVALID);
		}
	}

	void CSession::Verify(ByteArray &Data, ByteArray &Signature)
	{
		init_func

		if (pVerifyMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		VerifyUpdate(Data);
		VerifyFinal(Signature);
	}

	void CSession::VerifyUpdate(ByteArray &Data)
	{
		init_func
		if (pVerifyMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		pVerifyMechanism->VerifyUpdate(Data);
	}

	void CSession::VerifyFinal(ByteArray &Signature)
	{
		init_func
			if (pVerifyMechanism == nullptr)
				throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		auto mech = make_resetter(pVerifyMechanism);
		pVerifyMechanism->VerifyFinal(Signature);
	}

	/* ******************** */
	/*		VerifyRecover	*/
	/* ******************** */

	void CSession::VerifyRecoverInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	{
		init_func
			if (pVerifyRecoverMechanism!= nullptr)
				throw p11_error(CKR_OPERATION_ACTIVE);

		std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(hKey);
		if (pObject == NULL)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		if (pObject->ObjClass != CKO_PUBLIC_KEY)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		auto pVerifyRecoverKey = std::static_pointer_cast<CP11PublicKey>(pObject);


		if (pVerifyRecoverKey->IsPrivate() && pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		ByteArray *baAttrVal = pVerifyRecoverKey->getAttribute(CKA_VERIFY_RECOVER);
		if (baAttrVal==nullptr)
			throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
		else {
			if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
				throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
		}

		switch (pMechanism->mechanism) {
		case CKM_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
			mech->VerifyRecoverInit(hKey);
			pVerifyRecoverMechanism = std::move(mech);
			break;
		}
		/* NON SUPPORTATO DALLA CIE
		case CKM_RSA_X_509:
		{
			auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
			mech->VerifyRecoverInit(hKey);
			pVerifyRecoverMechanism = std::move(mech);
			break;
		}*/
		default:
			throw p11_error(CKR_MECHANISM_INVALID);
		}
	}

	void CSession::VerifyRecover(ByteArray &Signature, ByteArray &Data)
	{
		init_func
		if (pVerifyRecoverMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		auto mech = std::move(pVerifyRecoverMechanism);

		CK_ULONG ulKeyLen = pVerifyRecoverMechanism->VerifyRecoverLength();
		ByteDynArray baData = pVerifyRecoverMechanism->VerifyRecover(Signature);

		if (!Data.isNull() && Data.size()<baData.size()) {
			pVerifyRecoverMechanism = std::move(mech);
			throw p11_error(CKR_BUFFER_TOO_SMALL);
		}

		Data = Data.left(baData.size());
		if (Data.isNull()) {
			pVerifyRecoverMechanism = std::move(mech);
			return;
		}

		Data.copy(baData);
	}

	/* ******************* */
	/*       Sign          */
	/* ******************* */

	void CSession::SignInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	{
		init_func
		if (pSignMechanism != nullptr)
			throw p11_error(CKR_OPERATION_ACTIVE);

		std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(hKey);
		if (pObject == nullptr)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		if (pObject->ObjClass != CKO_PRIVATE_KEY)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		auto pSignKey = std::static_pointer_cast<CP11PrivateKey>(pObject);


		if (pSignKey->IsPrivate() && pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

			ByteArray *baAttrVal = pSignKey->getAttribute(CKA_SIGN);
			if (baAttrVal==nullptr)
				throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
			else {
				if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
					throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
			}

		switch (pMechanism->mechanism) {
		case CKM_SHA1_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSAwithSHA1>(new CRSAwithSHA1(shared_from_this()));
			mech->SignInit(hKey);
			pSignMechanism = std::move(mech);
			break;
		}
		case CKM_MD5_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSAwithMD5>(new CRSAwithMD5(shared_from_this()));
			mech->SignInit(hKey);
			pSignMechanism = std::move(mech);
			break;
		}
		case CKM_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
			mech->SignInit(hKey);
			pSignMechanism = std::move(mech);
			break;
		}
		/* NON SUPPORTATO DALLA CIE
		case CKM_RSA_X_509:
		{
			auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
			mech->SignInit(hKey);
			pSignMechanism = std::move(mech);
			break;
		}*/
		default:
			throw p11_error(CKR_MECHANISM_INVALID);
		}
	}

	void CSession::Sign(ByteArray &Data, ByteArray &Signature)
	{
		init_func
		if (pSignMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		pSignMechanism->SignReset();
		SignUpdate(Data);
		SignFinal(Signature);
	}

	void CSession::SignUpdate(ByteArray &Data)
	{
		init_func
		if (pSignMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		pSignMechanism->SignUpdate(Data);
	}

	void CSession::SignFinal(ByteArray &Signature)
	{
		init_func
		if (pSignMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		auto mech = make_resetter(pSignMechanism);

		std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(pSignMechanism->hSignKey);
		if (pObject == NULL)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		if (pObject->ObjClass != CKO_PRIVATE_KEY)
			throw p11_error(CKR_KEY_HANDLE_INVALID);

		auto pSignKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		bool bPrivate = false;

		if (pSignKey->IsPrivate() && pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		if (Signature.isNull()) {
			CK_ULONG ulSignLength = pSignMechanism->SignLength();
			Signature = ByteArray(nullptr, ulSignLength);
			mech.release();
			return;
		}

		ByteDynArray baSignBuffer = pSignMechanism->SignFinal();

		bool bSilent = false;
		ByteDynArray baSignature;
		pSlot->pTemplate->FunctionList.templateSign(pSlot->pTemplateData, pSignKey.get(), baSignBuffer, baSignature, pSignMechanism->mtType, bSilent);

		if (Signature.size()<baSignature.size()) {
			Signature = Signature.left(baSignature.size());
			mech.release();
			throw p11_error(CKR_BUFFER_TOO_SMALL);
		}
		else {
			Signature.copy(baSignature);
			Signature = Signature.left(baSignature.size());
		}
	}

	/* ******************* */
	/*       SignRecover   */
	/* ******************* */

	void CSession::SignRecoverInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	{
		init_func
		if (pSignRecoverMechanism != nullptr)
			throw p11_error(CKR_OPERATION_ACTIVE);

		std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(hKey);
		if (pObject == NULL)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		if (pObject->ObjClass != CKO_PRIVATE_KEY)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		auto pSignRecoverKey = std::static_pointer_cast<CP11PrivateKey>(pObject);


		if (pSignRecoverKey->IsPrivate() && pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		ByteArray *baAttrVal = pSignRecoverKey->getAttribute(CKA_SIGN_RECOVER);
		if (baAttrVal == nullptr)
			throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
		else {
			if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
				throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
		}

		switch (pMechanism->mechanism) {
		case CKM_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
			mech->SignRecoverInit(hKey);
			pSignRecoverMechanism = std::move(mech);
			break;
		}
		/*
		NON SUPPORTATO DALLA CIE
		case CKM_RSA_X_509:
		{
			auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
			mech->SignRecoverInit(hKey);
			pSignRecoverMechanism = std::move(mech);
			break;
		}*/
		default:
			throw p11_error(CKR_MECHANISM_INVALID);
		}
	}

	void CSession::SignRecover(ByteArray &Data, ByteArray &Signature)
	{
		init_func
		if (pSignRecoverMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		auto mech = make_resetter(pSignRecoverMechanism);

		std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(pSignRecoverMechanism->hSignRecoverKey);
		if (pObject == nullptr)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		if (pObject->ObjClass != CKO_PRIVATE_KEY)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		auto pSignRecoverKey = std::static_pointer_cast<CP11PrivateKey>(pObject);


		if (pSignRecoverKey->IsPrivate() && pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		if (Signature.isNull()) {
			CK_ULONG ulSignRecoverLength = pSignRecoverMechanism->SignRecoverLength();
			Signature = ByteArray(nullptr, ulSignRecoverLength);
			mech.release();
			return;
		}

		ByteDynArray baSignRecoverBuffer;
		pSignRecoverMechanism->SignRecover(Data);

		bool bSilent = false;

		ByteDynArray baSignature;
		pSlot->pTemplate->FunctionList.templateSignRecover(pSlot->pTemplateData, pSignRecoverKey.get(), baSignRecoverBuffer, baSignature, pSignRecoverMechanism->mtType, bSilent);

		if (Signature.size()<baSignature.size()) {
			Signature = Signature.left(baSignature.size());
			mech.release();
			throw p11_error(CKR_BUFFER_TOO_SMALL);
		}
	}

	/* ******************* */
	/*       Encrypt        */
	/* ******************* */

	//void CSession::EncryptInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	//{
	//	init_func

	//		// NON SUPPORTATO DALLA CIE
	//		throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);

		//if (pEncryptMechanism != nullptr)
		//	throw p11_error(CKR_OPERATION_ACTIVE);

		//std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(hKey);
		//if (pObject == NULL)
		//	throw p11_error(CKR_KEY_HANDLE_INVALID);
		//if (pObject->ObjClass != CKO_PUBLIC_KEY)
		//	throw p11_error(CKR_KEY_HANDLE_INVALID);
		//auto pEncryptKey = std::static_pointer_cast<CP11PublicKey>(pObject);

		//if (pEncryptKey->IsPrivate() && pSlot->User != CKU_USER)
		//	throw p11_error(CKR_USER_NOT_LOGGED_IN);

		//ByteArray *baAttrVal = pEncryptKey->getAttribute(CKA_ENCRYPT);
		//if (baAttrVal == nullptr)
		//	throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
		//else {
		//	if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
		//		throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
		//}

		//switch (pMechanism->mechanism) {
		//case CKM_RSA_PKCS:
		//{
		//	auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
		//	mech->EncryptInit(hKey);
		//	pEncryptMechanism = std::move(mech);
		//	break;
		//}
		///* NON SUPPORTATO DALLA CIE
		//case CKM_RSA_X_509:
		//{
		//	auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
		//	mech->EncryptInit(hKey);
		//	pEncryptMechanism = std::move(mech);
		//	break;
		//}*/
		//default:
		//	throw p11_error(CKR_MECHANISM_INVALID);
		//}
	//}

	//void CSession::Encrypt(ByteArray &Data, ByteArray &EncryptedData)
	//{
	//	init_func

	//		// NON SUPPORTATO DALLA CIE
	//		throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);

		/*if (pEncryptMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		CK_ULONG ulReqLen = pEncryptMechanism->EncryptLength();

		if (!EncryptedData.isNull() && EncryptedData.size() < ulReqLen)
			throw p11_error(CKR_BUFFER_TOO_SMALL);

		EncryptedData = EncryptedData.left(ulReqLen);
		if (EncryptedData.isNull())
			return;

		EncryptUpdate(Data, EncryptedData);

		EncryptFinal(EncryptedData);*/
	/*}*/

	//void CSession::EncryptUpdate(ByteArray &Data, ByteArray &EncryptedData)
	//{
	//	init_func

	//	// NON SUPPORTATO DALLA CIE
	//	throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);

		/*if (pEncryptMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		ByteDynArray baEncryptedData = pEncryptMechanism->EncryptUpdate(Data);
		if (!EncryptedData.isNull() && EncryptedData.size() < baEncryptedData.size())
			throw p11_error(CKR_BUFFER_TOO_SMALL);
		EncryptedData = EncryptedData.left(baEncryptedData.size());
		if (EncryptedData.isNull())
			return;
		EncryptedData.copy(baEncryptedData);*/
	//}

	//void CSession::EncryptFinal(ByteArray &EncryptedData)
	//{
	//	init_func

	//		// NON SUPPORTATO DALLA CIE
	//		throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);

		/*if (pEncryptMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		auto mech = make_resetter(pEncryptMechanism);
		CK_ULONG ulReqLen = pEncryptMechanism->EncryptLength();

		if (!EncryptedData.isNull() && EncryptedData.size()<ulReqLen) {
			mech.release();
			throw p11_error(CKR_BUFFER_TOO_SMALL);
		}

		EncryptedData = EncryptedData.left(ulReqLen);
		if (EncryptedData.isNull()) {
			mech.release();
			return;
		}

		ByteDynArray baEncryptedBuffer = pEncryptMechanism->EncryptFinal();

		EncryptedData.copy(baEncryptedBuffer);
*/
	//}

	/* ******************** */
	/*		Decrypt			*/
	/* ******************** */

	//void CSession::DecryptInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	//{
	//	init_func

	//		// NON SUPPORTATO DALLA CIE
	//		throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);			

		
		//if (pDecryptMechanism != nullptr)
		//	throw p11_error(CKR_OPERATION_ACTIVE);

		//std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(hKey);
		//if (pObject == nullptr)
		//	throw p11_error(CKR_KEY_HANDLE_INVALID);
		//if (pObject->ObjClass != CKO_PRIVATE_KEY)
		//	throw p11_error(CKR_KEY_HANDLE_INVALID);
		//auto pDecryptKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		//
		//if (pDecryptKey->IsPrivate() && pSlot->User != CKU_USER)
		//	throw p11_error(CKR_USER_NOT_LOGGED_IN);

		//ByteArray *baAttrVal = pDecryptKey->getAttribute(CKA_DECRYPT);
		//if (baAttrVal == nullptr)
		//	throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
		//	else {
		//		if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
		//			throw p11_error(CKR_KEY_FUNCTION_NOT_PERMITTED);
		//	}

		//switch (pMechanism->mechanism) {
		//case CKM_RSA_PKCS:
		//{
		//	auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
		//	mech->DecryptInit(hKey);
		//	pDecryptMechanism = std::move(mech);
		//	break;
		//}
		///* NON SUPPORTATO DALLA CIE
		//case CKM_RSA_X_509:
		//{
		//	auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
		//	mech->DecryptInit(hKey);
		//	pDecryptMechanism = std::move(mech);
		//	break;
		//}*/
		//default:
		//	throw p11_error(CKR_MECHANISM_INVALID);
		//}

	//}

	//void CSession::Decrypt(ByteArray &EncryptedData, ByteArray &Data)
	//{
	//	init_func

	//		// NON SUPPORTATO DALLA CIE
	//		throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);

		/*if (pDecryptMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		bool bFound = pDecryptMechanism->checkCache(EncryptedData, Data);

		if (bFound) {
			pDecryptMechanism.reset();
			return;
		}

		DecryptUpdate(EncryptedData, Data);
		DecryptFinal(Data);

		if (Data.isNull())
			pDecryptMechanism->cacheData = EncryptedData;*/
	//}

	//void CSession::DecryptUpdate(ByteArray &EncryptedData, ByteArray &Data)
	//{
	//	init_func

	//		// NON SUPPORTATO DALLA CIE
	//		throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);

		/*if (pDecryptMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		ByteDynArray baData = pDecryptMechanism->DecryptUpdate(EncryptedData);

		if (!Data.isNull() && Data.size() < baData.size())
			throw p11_error(CKR_BUFFER_TOO_SMALL);
		Data = Data.left(baData.size());
		if (Data.isNull())
			return;
		Data.copy(baData);*/
	//}

	//void CSession::DecryptFinal(ByteArray &Data)
	//{
	//	init_func

	//		// NON SUPPORTATO DALLA CIE
	//		throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);

		/*if (pDecryptMechanism == nullptr)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		auto mech = make_resetter(pDecryptMechanism);
		bool bFound = pDecryptMechanism->checkCache(ByteArray(), Data);

		if (bFound)
			return;

		std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(pDecryptMechanism->hDecryptKey);
		if (pObject == nullptr)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		if (pObject->ObjClass != CKO_PRIVATE_KEY)
			throw p11_error(CKR_KEY_HANDLE_INVALID);
		auto pDecryptKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		if (pDecryptKey->IsPrivate() && pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		ByteArray *baKeyModule = pDecryptKey->getAttribute(CKA_MODULUS);
		ER_ASSERT(baKeyModule != nullptr, ERR_CANT_GET_PUBKEY_MODULUS)

		size_t dwKeyLenBytes = baKeyModule->size();

		ByteDynArray baDecryptBuffer = pDecryptMechanism->DecryptFinal();

		ByteDynArray baData;
		pSlot->pTemplate->FunctionList.templateDecrypt(pSlot->pTemplateData, pDecryptKey.get(), baDecryptBuffer, baData, pDecryptMechanism->mtType, false);

		ByteDynArray baUnpaddedData = pDecryptMechanism->DecryptRemovePadding(baData);

		if (Data.isNull()) {
			pDecryptMechanism->setCache(ByteArray(), baUnpaddedData);
			Data = ByteArray(nullptr, baUnpaddedData.size());
			mech.release();
		}
		else if (Data.size()<baUnpaddedData.size()) {
			pDecryptMechanism->setCache(ByteArray(), baUnpaddedData);
			Data = Data.left(baUnpaddedData.size());
			mech.release();
			throw p11_error(CKR_BUFFER_TOO_SMALL);
		}
		else {
			pDecryptMechanism->setCache(ByteArray(), baUnpaddedData);
			Data.copy(baUnpaddedData);
			Data = Data.left(baUnpaddedData.size());
		}*/
	//}

	void CSession::GenerateRandom(ByteArray &RandomData)
	{
		init_func
		ByteDynArray baRandom(RandomData.size());
		pSlot->pTemplate->FunctionList.templateGenerateRandom(pSlot->pTemplateData, baRandom);
		RandomData.copy(baRandom);
	}

	void CSession::InitPIN(ByteArray &Pin)
	{
		init_func

		if (pSlot->User != CKU_SO)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		pSlot->pTemplate->FunctionList.templateInitPIN(pSlot->pTemplateData, Pin);
	}

	void CSession::SetPIN(ByteArray &OldPin, ByteArray &NewPin)
	{
		init_func

		pSlot->pTemplate->FunctionList.templateSetPIN(pSlot->pTemplateData, OldPin, NewPin, pSlot->User);
	}

	CK_ULONG CSession::GetObjectSize(CK_OBJECT_HANDLE hObject)
	{
		init_func

			std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(hObject);
		if (pObject == nullptr)
			throw p11_error(CKR_OBJECT_HANDLE_INVALID);


		if (pObject->IsPrivate() && pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		return pObject->GetObjectSize();
	}

	void CSession::SetAttributeValue(CK_OBJECT_HANDLE hObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
	{
		init_func
			std::shared_ptr<CP11Object> pObject = pSlot->GetObjectFromID(hObject);
		if (pObject == nullptr)
			throw p11_error(CKR_OBJECT_HANDLE_INVALID);

		if ((flags & CKF_RW_SESSION) == 0)
			throw p11_error(CKR_SESSION_READ_ONLY);

		if (pSlot->User != CKU_USER)
			throw p11_error(CKR_USER_NOT_LOGGED_IN);

		pSlot->pTemplate->FunctionList.templateSetAttribute(pSlot->pTemplateData, pObject.get(), pTemplate, ulCount);
	}

	void CSession::SetOperationState(ByteArray &OperationState)
	{
		init_func
			CTLV Tlv(OperationState);

		ByteArray Flags, User;
		Flags = Tlv.getValue(OS_Flags);
		if (Flags.isNull()) throw p11_error(CKR_SAVED_STATE_INVALID);
		User = Tlv.getValue(OS_User);
		if (User.isNull())
			throw p11_error(CKR_SAVED_STATE_INVALID);

		if (Flags != VarToByteArray(flags))
			throw p11_error(CKR_SAVED_STATE_INVALID);
		if (User != VarToByteArray(pSlot->User))
			throw p11_error(CKR_SAVED_STATE_INVALID);

		ByteArray SignOperationState = Tlv.getValue(OS_Sign);
		if (!SignOperationState.isNull()) {
			pSignMechanism.reset();
			CTLV SignTlv(SignOperationState);
			ByteArray Algo = SignTlv.getValue(OS_Algo);
			if (Algo.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);
			CK_MECHANISM mech = { ByteArrayToVar(Algo, CK_MECHANISM_TYPE), NULL, 0 };

			ByteArray Key = SignTlv.getValue(OS_Key);
			if (Key.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);

			std::shared_ptr<CP11Object> pKey = pSlot->FindP11Object(CKO_PRIVATE_KEY, CKA_ID, Key.data(), (int)Key.size());
			ER_ASSERT(pKey != nullptr, ERR_CANT_GET_OBJECT);
			CK_OBJECT_HANDLE hKey = pSlot->GetIDFromObject(pKey);

			ByteArray Data = SignTlv.getValue(OS_Data);
			if (Data.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);

			SignInit(&mech, hKey);
			pSignMechanism->SignSetOperationState(Data);
		}

		/*ByteArray DecryptOperationState = Tlv.getValue(OS_Decrypt);
		if (!DecryptOperationState.isNull()) {
			pDecryptMechanism.reset();
			CTLV DecryptTlv(DecryptOperationState);
			ByteArray Algo = DecryptTlv.getValue(OS_Algo);
			throw p11_error(CKR_SAVED_STATE_INVALID);
			CK_MECHANISM mech = { ByteArrayToVar(Algo, CK_MECHANISM_TYPE), NULL, 0 };

			ByteArray Key = DecryptTlv.getValue(OS_Key);
			if (Key.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);

			std::shared_ptr<CP11Object> pKey = pSlot->FindP11Object(CKO_PRIVATE_KEY, CKA_ID, Key.data(), (int)Key.size());
			ER_ASSERT(pKey != nullptr, ERR_CANT_GET_OBJECT)
				CK_OBJECT_HANDLE hKey = pSlot->GetIDFromObject(pKey);

			ByteArray Data = DecryptTlv.getValue(OS_Data);
			if (Data.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);

			DecryptInit(&mech, hKey);
			pDecryptMechanism->DecryptSetOperationState(Data);
		}*/

		ByteArray VerifyOperationState = Tlv.getValue(OS_Verify);
		if (!VerifyOperationState.isNull()) {
			pVerifyMechanism.reset();
			CTLV VerifyTlv(VerifyOperationState);
			ByteArray Algo = VerifyTlv.getValue(OS_Algo);
			if (Algo.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);
			CK_MECHANISM mech = { ByteArrayToVar(Algo, CK_MECHANISM_TYPE), NULL, 0 };

			ByteArray Key = VerifyTlv.getValue(OS_Key);
			if (Key.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);

			std::shared_ptr<CP11Object> pKey = pSlot->FindP11Object(CKO_PUBLIC_KEY, CKA_ID, Key.data(), (int)Key.size());
			ER_ASSERT(pKey != nullptr, ERR_CANT_GET_OBJECT)
				CK_OBJECT_HANDLE hKey = pSlot->GetIDFromObject(pKey);

			ByteArray Data = VerifyTlv.getValue(OS_Data);
			if (Data.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);

			VerifyInit(&mech, hKey);

			pVerifyMechanism->VerifySetOperationState(Data);
		}

		ByteArray EncryptOperationState = Tlv.getValue(OS_Encrypt);
		/*if (!EncryptOperationState.isNull()) {
			pEncryptMechanism.reset();
			CTLV EncryptTlv(EncryptOperationState);
			ByteArray Algo = EncryptTlv.getValue(OS_Algo);
			if (Algo.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);
			CK_MECHANISM mech = { ByteArrayToVar(Algo, CK_MECHANISM_TYPE), NULL, 0 };

			ByteArray Key = EncryptTlv.getValue(OS_Key);
			if (Key.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);

			std::shared_ptr<CP11Object> pKey = pSlot->FindP11Object(CKO_PUBLIC_KEY, CKA_ID, Key.data(), (int)Key.size());
			ER_ASSERT(pKey != nullptr, ERR_CANT_GET_OBJECT);
			CK_OBJECT_HANDLE hKey = pSlot->GetIDFromObject(pKey);

			ByteArray Data = EncryptTlv.getValue(OS_Data);
			if (Data.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);

			EncryptInit(&mech, hKey);

			pEncryptMechanism->EncryptSetOperationState(Data);
		}*/

		ByteArray DigestOperationState = Tlv.getValue(OS_Digest);
		if (!DigestOperationState.isNull()) {
			pDigestMechanism.reset();
			CTLV DigestTlv(DigestOperationState);
			ByteArray Algo = DigestTlv.getValue(OS_Algo);
			if (Algo.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);
			CK_MECHANISM mech = { ByteArrayToVar(Algo, CK_MECHANISM_TYPE), NULL, 0 };

			ByteArray Data = DigestTlv.getValue(OS_Data);
			if (Data.isNull())
				throw p11_error(CKR_SAVED_STATE_INVALID);

			DigestInit(&mech);

			pDigestMechanism->DigestSetOperationState(Data);
		}
	}

	void CSession::GetOperationState(ByteArray &OperationState)
	{
		init_func

			CTLVCreate Tlv;
		Tlv.setValue(OS_Flags, ByteArray((BYTE*)&flags, sizeof(flags)));
		Tlv.setValue(OS_User, ByteArray((BYTE*)&pSlot->User, sizeof(pSlot->User)));
		if (pSignMechanism) {
			CTLVCreate SignTlv;
			SignTlv.setValue(OS_Algo, ByteArray((BYTE*)&pSignMechanism->mtType, sizeof(pSignMechanism->mtType)));
			ByteDynArray SignData = pSignMechanism->SignGetOperationState();
			if (!SignData.isEmpty())
				SignTlv.setValue(OS_Data, SignData);

			std::shared_ptr<CP11Object> pKey = pSlot->GetObjectFromID(pSignMechanism->hSignKey);
			ER_ASSERT(pKey != nullptr, ERR_CANT_GET_OBJECT);
			ByteArray *SignKeyID = pKey->getAttribute(CKA_ID);
			ER_ASSERT(SignKeyID != nullptr, ERR_CANT_FIND_ID);
			SignTlv.setValue(OS_Key, *SignKeyID);

			ByteDynArray *SignOperationState = Tlv.addValue(OS_Sign);
			*SignOperationState = SignTlv.getBuffer();
		}
		if (pVerifyMechanism) {
			CTLVCreate VerifyTlv;
			VerifyTlv.setValue(OS_Algo, ByteArray((BYTE*)&pVerifyMechanism->mtType, sizeof(pVerifyMechanism->mtType)));
			ByteDynArray verifyData = pVerifyMechanism->VerifyGetOperationState();
			if (!verifyData.isEmpty())
				VerifyTlv.setValue(OS_Data, verifyData);


			std::shared_ptr<CP11Object> pKey = pSlot->GetObjectFromID(pVerifyMechanism->hVerifyKey);
			ER_ASSERT(pKey != nullptr, ERR_CANT_GET_OBJECT);
			ByteArray *VerifyKeyID = pKey->getAttribute(CKA_ID);
			ER_ASSERT(VerifyKeyID != nullptr, ERR_CANT_FIND_ID);
			VerifyTlv.setValue(OS_Key, *VerifyKeyID);

			ByteDynArray *VerifyOperationState = Tlv.addValue(OS_Verify);
			*VerifyOperationState = VerifyTlv.getBuffer();
		}
		if (pDigestMechanism) {
			CTLVCreate DigestTlv;
			DigestTlv.setValue(OS_Algo, ByteArray((BYTE*)&pDigestMechanism->mtType, sizeof(pDigestMechanism->mtType)));
			ByteDynArray DigestData = pDigestMechanism->DigestGetOperationState();
			if (!DigestData.isEmpty())
				DigestTlv.setValue(OS_Data, DigestData);

			ByteDynArray *DigestOperationState = Tlv.addValue(OS_Digest);
			*DigestOperationState = DigestTlv.getBuffer();
		}
		/*if (pEncryptMechanism) {
			CTLVCreate EncryptTlv;
			EncryptTlv.setValue(OS_Algo, ByteArray((BYTE*)&pEncryptMechanism->mtType, sizeof(pEncryptMechanism->mtType)));
			ByteDynArray EncryptData = pEncryptMechanism->EncryptGetOperationState();
			if (!EncryptData.isEmpty())
				EncryptTlv.setValue(OS_Data, EncryptData);


			std::shared_ptr<CP11Object> pKey = pSlot->GetObjectFromID(pEncryptMechanism->hEncryptKey);
				ER_ASSERT(pKey != nullptr, ERR_CANT_GET_OBJECT);
			ByteArray *EncryptKeyID = pKey->getAttribute(CKA_ID);
			ER_ASSERT(EncryptKeyID != nullptr, ERR_CANT_FIND_ID);
			EncryptTlv.setValue(OS_Key, *EncryptKeyID);

			ByteDynArray *EncryptOperationState = Tlv.addValue(OS_Encrypt);
			*EncryptOperationState = EncryptTlv.getBuffer();
		}*/
		/*if (pDecryptMechanism) {
			CTLVCreate DecryptTlv;
			DecryptTlv.setValue(OS_Algo, ByteArray((BYTE*)&pDecryptMechanism->mtType, sizeof(pDecryptMechanism->mtType)));
			ByteDynArray DecryptData = pDecryptMechanism->DecryptGetOperationState();
			if (!DecryptData.isEmpty())
				DecryptTlv.setValue(OS_Data, DecryptData);


			std::shared_ptr<CP11Object> pKey = pSlot->GetObjectFromID(pDecryptMechanism->hDecryptKey);
				ER_ASSERT(pKey != nullptr, ERR_CANT_GET_OBJECT);
			ByteArray *DecryptKeyID = pKey->getAttribute(CKA_ID);
			ER_ASSERT(DecryptKeyID != nullptr, ERR_CANT_FIND_ID);
			DecryptTlv.setValue(OS_Key, *DecryptKeyID);

			ByteDynArray *DecryptOperationState = Tlv.addValue(OS_Decrypt);
			*DecryptOperationState = DecryptTlv.getBuffer();
		}*/
		ByteDynArray newOperationState = Tlv.getBuffer();

		if (newOperationState.size() == 0)
			throw p11_error(CKR_OPERATION_NOT_INITIALIZED);

		if (OperationState.isNull()) {
			OperationState = ByteArray(nullptr, newOperationState.size());
			return;
		}
		if (OperationState.size() < newOperationState.size())
			throw p11_error(CKR_BUFFER_TOO_SMALL);

		OperationState.copy(newOperationState);
	}
};
