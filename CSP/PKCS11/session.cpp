#include "../StdAfx.h"
#include "session.h"
#include "cardtemplate.h"
#include "../util/util.h"
#include "../crypto\rsa.h"
#include "../util/tlv.h"

static char *szCompiledFile = __FILE__;

#define noexcept throw()

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

		void reset() //noexcept(noexcept(m_p->reset()))
		{
			if (m_p)
				m_p->reset();

			m_p = nullptr;
		}

	public:
		resetter(const resetter&) = delete;
		resetter(resetter&& other) noexcept : m_p(std::exchange(other.m_p, nullptr)) {}

		resetter& operator=(const resetter&) = delete;

		resetter& operator=(resetter&& other) //noexcept(noexcept(reset()))
		{
			reset();
			m_p = std::exchange(other.m_p, nullptr);
			return *this;
		}

		~resetter() //noexcept(noexcept(reset())) 
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

	DWORD CSession::dwSessionCnt = 1;
	SessionMap CSession::g_mSessions;

	CSession::CSession()
	{
		init_func_internal
			pSlot = NULL;
		bFindInit = false;
		exit_func_internal
	}

	RESULT CSession::GetNewSessionID(CK_SLOT_ID &hSlotID) {
		init_func

			hSlotID = dwSessionCnt;
		dwSessionCnt++;
		_return(OK)
			exit_func
			_return(FAIL)
	}

	RESULT CSession::AddSession(std::unique_ptr<CSession> pSession, CK_SESSION_HANDLE &hSession)
	{
		init_func
			P11ER_CALL(GetNewSessionID(pSession->hSessionHandle),
			ERR_GET_NEW_SESSION)

			hSession = pSession->hSessionHandle;
		P11ER_CALL(pSession->pSlot->pTemplate->FunctionList.templateInitSession(pSession->pSlot->pTemplateData),
			ERR_INIT_SESSION)

			pSession->pSlot->dwSessionCount++;
		g_mSessions.insert(std::make_pair(pSession->hSessionHandle, std::move(pSession)));

		_return(OK)
			exit_func
			_return(FAIL)
	}

	RESULT CSession::DeleteSession(CK_SESSION_HANDLE hSessionHandle)
	{
		init_func
			std::shared_ptr<CSession> pSession;
		P11ER_CALL(GetSessionFromID(hSessionHandle, pSession),
			ERR_CANT_GET_SESSION);

		ER_ASSERT(pSession, ERR_SESSION_NOT_OPENED);

		pSession->pSlot->dwSessionCount--;
		if (pSession->pSlot->dwSessionCount == 0) {
			if (pSession->pSlot->User != CKU_NOBODY) {
				P11ER_CALL(pSession->Logout(),
					ERR_CANT_LOGOUT)
			}
		}

		P11ER_CALL(pSession->pSlot->pTemplate->FunctionList.templateFinalSession(pSession->pSlot->pTemplateData),
			ERR_FINAL_SESSION)

			g_mSessions.erase(hSessionHandle);

		_return(OK)
			exit_func
			_return(FAIL)
	}

	RESULT CSession::GetSessionFromID(CK_SESSION_HANDLE hSessionHandle, std::shared_ptr<CSession>&pSession)
	{
		init_func
			pSession.reset();
		SessionMap::const_iterator pPair;
		pPair = g_mSessions.find(hSessionHandle);
		if (pPair == g_mSessions.end()) {
			pSession.reset();
			_return(OK)
		}
		pSession = pPair->second;
		_return(OK)
			exit_func
			_return(FAIL)
	}

	/* ******************* */
	/*    LOGIN e LOGOUT   */
	/* ******************* */

	CK_RV CSession::Login(CK_USER_TYPE userType, CK_CHAR_PTR pPin, CK_ULONG ulPinLen)
	{
		init_func

			if (pSlot->User == CKU_USER && userType == CKU_SO)
				_return(CKR_USER_ANOTHER_ALREADY_LOGGED_IN)
				if (pSlot->User == CKU_SO && userType == CKU_USER)
					_return(CKR_USER_ANOTHER_ALREADY_LOGGED_IN)
					bool bExistsRO = false;
		if (userType == CKU_SO) {
			P11ER_CALL(ExistsRO(bExistsRO),
				ERR_RO_SESSION)
				if (bExistsRO)
					_return(CKR_SESSION_READ_ONLY_EXISTS)
		}
		if (pSlot->User != CKU_NOBODY)
			_return(CKR_USER_ALREADY_LOGGED_IN)

			ByteArray baPin(pPin, ulPinLen);
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateLogin(pSlot->pTemplateData, userType, baPin),
			ERR_CANT_LOGIN)

			pSlot->User = userType;

		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::Logout() {
		init_func

			P11ER_CALL(pSlot->pTemplate->FunctionList.templateLogout(pSlot->pTemplateData, pSlot->User),
			ERR_CANT_LOGOUT)

			for (DWORD i = 0; i<pSlot->P11Objects.size(); i++) {
				auto p11Obj = pSlot->P11Objects[i];
				bool bPrivate;
				P11ER_CALL(p11Obj->IsPrivate(bPrivate),
					ERR_GET_PRIVATE)

					if (bPrivate)
						pSlot->DelObjectHandle(p11Obj);
			}
		pSlot->User = CKU_NOBODY;
		_return(OK)
			exit_func
			_return(FAIL)
	}

	/* ******************* */
	/*    Find Objects     */
	/* ******************* */

	CK_RV CSession::FindObjectsInit(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
	{
		init_func
			if (bFindInit)
				_return(CKR_OPERATION_ACTIVE)
				findResult.clear();
		if (ulCount == 0) {
			for (DWORD i = 0; i<pSlot->P11Objects.size(); i++) {
				CK_OBJECT_HANDLE hObject;
				RESULT ris;
				if (ris = pSlot->GetIDFromObject(pSlot->P11Objects[i], hObject)) {
					continue;
				}
				else {
					findResult.push_back(hObject);
					if (Log.LogParam) {
						ByteArray *Label = NULL;
						pSlot->P11Objects[i]->getAttribute(CKA_LABEL, Label);
						Log.writePure("Object found %i:", i);
						Log.writePure("Class: %x", pSlot->P11Objects[i]->ObjClass);
						if (Label) {
							Log.writePure("Label:");
							info.logParameter(Label->data(), Label->size());
						}
					}
				}
			}
		}
		else {
			for (DWORD i = 0; i<pSlot->P11Objects.size(); i++) {
				auto obj = pSlot->P11Objects[i];
				CK_OBJECT_HANDLE hObject;
				bool bSkip = false;

				for (unsigned int j = 0; j<ulCount && !bSkip; j++) {
					ByteArray *attr = NULL;
					if (obj->getAttribute(pTemplate[j].type, attr) == 0) {
						if (attr) {
							if (attr->size() != pTemplate[j].ulValueLen) bSkip = true;
							else if (memcmp(attr->data(), pTemplate[j].pValue, pTemplate[j].ulValueLen) != 0) bSkip = true;
						}
						else bSkip = true;
					}
					else bSkip = true;
				}
				if (!bSkip) {
					RESULT ris;
					if (ris = pSlot->GetIDFromObject(pSlot->P11Objects[i], hObject)) {
						continue;
					}
					findResult.push_back(hObject);
					if (Log.LogParam) {
						ByteArray *Label = NULL;
						pSlot->P11Objects[i]->getAttribute(CKA_LABEL, Label);
						Log.writePure("Object found %i:", i);
						Log.writePure("Class: %x", pSlot->P11Objects[i]->ObjClass);
						if (Label) {
							Log.writePure("Label:");
							info.logParameter(Label->data(), Label->size());
						}
					}
				}
			}
		}
		bFindInit = true;
		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::FindObjects(CK_OBJECT_HANDLE_PTR phObject, CK_ULONG ulMaxObjectCount, CK_ULONG_PTR pulObjectCount)
	{
		init_func
			if (!bFindInit)
				_return(CKR_OPERATION_NOT_INITIALIZED)
				*pulObjectCount = 0;
		int iCnt = 0;
		while (!findResult.empty() && ulMaxObjectCount>0) {
			phObject[iCnt] = findResult.back();
			findResult.pop_back();
			iCnt++;
			ulMaxObjectCount--;
		}
		*pulObjectCount = iCnt;
		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::FindObjectsFinal()
	{
		init_func
			if (!bFindInit)
				_return(CKR_OPERATION_NOT_INITIALIZED)
				findResult.clear();
		bFindInit = false;
		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::GetAttributeValue(CK_OBJECT_HANDLE hObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
	{
		init_func

			std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(hObject, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_OBJECT_HANDLE_INVALID)

				DWORD ret = pObject->GetAttributeValue(pTemplate, ulCount);
		_return(ret)

			exit_func
			_return(FAIL)
	}

	RESULT GetTemplateValue(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_ATTRIBUTE_TYPE type, ByteDynArray &val)
	{
		init_func
			for (unsigned int i = 0; i<ulCount; i++) {
				if (pTemplate[i].type == type) {
					val = ByteArray((uint8_t*)pTemplate[i].pValue, pTemplate[i].ulValueLen);
					_return(OK)
				}
			}
		_return(FAIL)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::CreateObject(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR phObject) {
		init_func

			if ((flags & CKF_RW_SESSION) == 0)
				_return(CKR_SESSION_READ_ONLY)

				if (pSlot->User != CKU_USER)
					_return(CKR_USER_NOT_LOGGED_IN)

					std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateCreateObject(pSlot->pTemplateData, pTemplate, ulCount, pObject),
			ERR_CREATE_OBJECT)

			if (pObject != NULL) {
				P11ER_CALL(pSlot->GetIDFromObject(pObject, *phObject),
					ERR_GET_OBJECT_HANDLE)
			}
			else
				_return(CKR_GENERAL_ERROR)

				_return(OK)
				exit_func
				_return(FAIL)
	}

	CK_RV CSession::GenerateKey(CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR phKey) {
		init_func

			if ((flags & CKF_RW_SESSION) == 0)
				_return(CKR_SESSION_READ_ONLY)

				if (pSlot->User != CKU_USER)
					_return(CKR_USER_NOT_LOGGED_IN)

					std::shared_ptr<CP11Object> pKey;
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateGenerateKey(pSlot->pTemplateData, pMechanism, pTemplate, ulCount, pKey),
			ERR_CREATE_OBJECT)

			if (pKey != NULL) {
				P11ER_CALL(pSlot->GetIDFromObject(pKey, *phKey),
					ERR_GET_OBJECT_HANDLE)
			}
			else
				_return(CKR_GENERAL_ERROR)

				_return(OK)
				exit_func
				_return(FAIL)
	}

	CK_RV CSession::GenerateKeyPair(CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount, CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount, CK_OBJECT_HANDLE_PTR phPublicKey, CK_OBJECT_HANDLE_PTR phPrivateKey) {
		init_func

			if ((flags & CKF_RW_SESSION) == 0)
				_return(CKR_SESSION_READ_ONLY)

				if (pSlot->User != CKU_USER)
					_return(CKR_USER_NOT_LOGGED_IN)

					std::shared_ptr<CP11Object> pPublicKey = NULL;
		std::shared_ptr<CP11Object> pPrivateKey = NULL;
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateGenerateKeyPair(pSlot->pTemplateData, pMechanism, pPublicKeyTemplate, ulPublicKeyAttributeCount, pPrivateKeyTemplate, ulPrivateKeyAttributeCount, pPublicKey, pPrivateKey),
			ERR_CREATE_OBJECT)

			if (pPublicKey != NULL) {
				P11ER_CALL(pSlot->GetIDFromObject(pPublicKey, *phPublicKey),
					ERR_GET_OBJECT_HANDLE)
			}
			else
				_return(CKR_GENERAL_ERROR)

				if (pPrivateKey != NULL) {
					P11ER_CALL(pSlot->GetIDFromObject(pPrivateKey, *phPrivateKey),
						ERR_GET_OBJECT_HANDLE)
				}
				else
					_return(CKR_GENERAL_ERROR)

					_return(OK)
					exit_func
					_return(FAIL)
	}

	RESULT CSession::DestroyObject(CK_OBJECT_HANDLE hObject) {
		init_func

			if ((flags & CKF_RW_SESSION) == 0)
				_return(CKR_SESSION_READ_ONLY)

				if (pSlot->User != CKU_USER)
					_return(CKR_USER_NOT_LOGGED_IN)

					std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(hObject, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_OBJECT_HANDLE_INVALID)

				P11ER_CALL(pSlot->pTemplate->FunctionList.templateDestroyObject(pSlot->pTemplateData, *pObject),
				ERR_CREATE_OBJECT)

				P11ER_CALL(pSlot->DelP11Object(pObject),
				ERR_CANT_DELETE_OBJECT)

				_return(OK)
				exit_func
				_return(FAIL)
	}

	RESULT CSession::ExistsRO(bool &bExistsRO)
	{
		init_func
			bExistsRO = false;
		for (SessionMap::const_iterator it = g_mSessions.begin(); it != g_mSessions.end(); it++)
		{
			if (it->second->pSlot == pSlot && (it->second->flags & CKF_RW_SESSION) == 0) {
				bExistsRO = true;
				_return(OK)
			}
		}

		_return(OK)
			exit_func
			_return(FAIL)
	}

	RESULT CSession::ExistsSO_RW(bool &bExists)
	{
		init_func
			bExists = false;
		if (pSlot->User != CKU_SO)
			_return(OK)
			for (SessionMap::const_iterator it = g_mSessions.begin(); it != g_mSessions.end(); it++)
			{
				if (it->second->pSlot == pSlot && (it->second->flags & CKF_RW_SESSION) != 0) {
					bExists = true;
					_return(OK)
				}
			}

		_return(OK)
			exit_func
			_return(FAIL)
	}

	/* ******************* */
	/*       Digest        */
	/* ******************* */

	CK_RV CSession::DigestInit(CK_MECHANISM_PTR pMechanism)
	{
		init_func
			if (pDigestMechanism)
				_return(CKR_OPERATION_ACTIVE)

				switch (pMechanism->mechanism) {
				case CKM_SHA_1:
				{
					auto mech = std::unique_ptr<CSHA>(new CSHA(shared_from_this()));
					P11ER_CALL(mech->DigestInit(),
						ERR_CANT_INITIALIZE_MECHANISM)

						pDigestMechanism = std::move(mech);
					break;
				}
				case CKM_MD5:
				{
					auto mech = std::unique_ptr<CMD5>(new CMD5(shared_from_this()));
					P11ER_CALL(mech->DigestInit(),
						ERR_CANT_INITIALIZE_MECHANISM)

						pDigestMechanism = std::move(mech);
					break;
				}
				default:
					_return(CKR_MECHANISM_INVALID)
			}

		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::Digest(ByteArray &Data, ByteArray &Digest)
	{
		init_func

			CK_ULONG ulReqLen = 0;
		P11ER_CALL(pDigestMechanism->DigestLength(&ulReqLen),
			ERR_CANT_GET_MECHANISM_LENGTH)

			if (!Digest.isNull() && Digest.size() < ulReqLen)
				_return(CKR_BUFFER_TOO_SMALL)

				Digest = Digest.left(ulReqLen);
		if (Digest.isNull())
			_return(OK)

			CK_RV result = DigestUpdate(Data);
		if (result != OK)
			_return(result)

			DWORD ret = DigestFinal(Digest);
		_return(ret)

			exit_func
			_return(FAIL)

	}

	CK_RV CSession::DigestUpdate(ByteArray &Data)
	{
		init_func
			if (!pDigestMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				P11ER_CALL(pDigestMechanism->DigestUpdate(Data),
				ERR_CANT_UPDATE_MECHANISM)

				_return(OK)
				exit_func
				_return(FAIL)

	}

	CK_RV CSession::DigestFinal(ByteArray &Digest)
	{
		init_func
			if (!pDigestMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				auto mech = std::move(pDigestMechanism);
		CK_ULONG ulReqLen = 0;
		P11ER_CALL(pDigestMechanism->DigestLength(&ulReqLen),
			ERR_CANT_GET_MECHANISM_LENGTH)

			if (!Digest.isNull() && Digest.size()<ulReqLen) {
				pDigestMechanism = std::move(mech);
				_return(CKR_BUFFER_TOO_SMALL)
			}

		Digest = Digest.left(ulReqLen);
		if (Digest.isNull()) {
			pDigestMechanism = std::move(mech);
			_return(OK)
		}

		RESULT ris;
		if (ris = pDigestMechanism->DigestFinal(Digest))
			_return(ris)

			_return(OK)

			exit_func
			_return(FAIL)

	}

	/* ******************* */
	/*       Verify        */
	/* ******************* */

	CK_RV CSession::VerifyInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	{
		init_func
			if (pVerifyMechanism)
				_return(CKR_OPERATION_ACTIVE)

				std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(hKey, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_KEY_HANDLE_INVALID)
				if (pObject->ObjClass != CKO_PUBLIC_KEY)
					_return(CKR_KEY_HANDLE_INVALID)
					auto pVerifyKey = std::static_pointer_cast<CP11PublicKey>(pObject);

		bool bPrivate = false;
		P11ER_CALL(pVerifyKey->IsPrivate(bPrivate),
			ERR_GET_PRIVATE)

			if (bPrivate && pSlot->User != CKU_USER)
				_return(CKR_USER_NOT_LOGGED_IN)

				ByteArray *baAttrVal = NULL;
		P11ER_CALL(pVerifyKey->getAttribute(CKA_VERIFY, baAttrVal),
			ERR_GET_ATTRIBUTE)
			if (baAttrVal == NULL)
				_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			else {
				if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
					_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			}

		switch (pMechanism->mechanism) {
		case CKM_SHA1_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSAwithSHA1>(new CRSAwithSHA1(shared_from_this()));
			P11ER_CALL(mech->VerifyInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pVerifyMechanism = std::move(mech);
			break;
		}
		case CKM_MD5_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSAwithMD5>(new CRSAwithMD5(shared_from_this()));
			P11ER_CALL(mech->VerifyInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pVerifyMechanism = std::move(mech);
			break;
		}
		case CKM_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
			P11ER_CALL(mech->VerifyInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pVerifyMechanism = std::move(mech);
			break;
		}
		case CKM_RSA_X_509:
		{
			auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
			P11ER_CALL(mech->VerifyInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pVerifyMechanism = std::move(mech);
			break;
		}
		default:
			_return(CKR_MECHANISM_INVALID)
		}

		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::Verify(ByteArray &Data, ByteArray &Signature)
	{
		init_func

			if (!pVerifyMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				CK_RV result = VerifyUpdate(Data);
		if (result != OK)
			_return(result)

			DWORD ret = VerifyFinal(Signature);
		_return(ret)

			exit_func
			_return(FAIL)
	}

	CK_RV CSession::VerifyUpdate(ByteArray &Data)
	{
		init_func
			if (!pVerifyMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				P11ER_CALL(pVerifyMechanism->VerifyUpdate(Data),
				ERR_CANT_UPDATE_MECHANISM)
				_return(OK)

				exit_func
				_return(FAIL)
	}

	CK_RV CSession::VerifyFinal(ByteArray &Signature)
	{
		init_func
			if (!pVerifyMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				auto mech = make_resetter(pVerifyMechanism);
		P11ER_CALL(pVerifyMechanism->VerifyFinal(Signature),
			ERR_CANT_FINAL_MECHANISM)

			_return(OK)
			exit_func
			_return(FAIL)
	}

	/* ******************** */
	/*		VerifyRecover	*/
	/* ******************** */

	CK_RV CSession::VerifyRecoverInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	{
		init_func
			if (pVerifyRecoverMechanism)
				_return(CKR_OPERATION_ACTIVE)

				std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(hKey, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_KEY_HANDLE_INVALID)
				if (pObject->ObjClass != CKO_PUBLIC_KEY)
					_return(CKR_KEY_HANDLE_INVALID)
					auto pVerifyRecoverKey = std::static_pointer_cast<CP11PublicKey>(pObject);

		bool bPrivate = false;
		P11ER_CALL(pVerifyRecoverKey->IsPrivate(bPrivate),
			ERR_GET_PRIVATE)

			if (bPrivate && pSlot->User != CKU_USER)
				_return(CKR_USER_NOT_LOGGED_IN)

				ByteArray *baAttrVal = NULL;
		P11ER_CALL(pVerifyRecoverKey->getAttribute(CKA_VERIFY_RECOVER, baAttrVal),
			ERR_GET_ATTRIBUTE)
			if (baAttrVal == NULL)
				_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			else {
				if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
					_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			}

		switch (pMechanism->mechanism) {
		case CKM_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
			P11ER_CALL(mech->VerifyRecoverInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pVerifyRecoverMechanism = std::move(mech);
			break;
		}
		case CKM_RSA_X_509:
		{
			auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
			P11ER_CALL(mech->VerifyRecoverInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pVerifyRecoverMechanism = std::move(mech);
			break;
		}
		default:
			_return(CKR_MECHANISM_INVALID)
		}

		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::VerifyRecover(ByteArray &Signature, ByteArray &Data)
	{
		init_func
			if (!pVerifyRecoverMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				auto mech = std::move(pVerifyRecoverMechanism);

		CK_ULONG ulKeyLen = 0;
		P11ER_CALL(pVerifyRecoverMechanism->VerifyRecoverLength(&ulKeyLen),
			ERR_CANT_GET_MECHANISM_LENGTH)

			ByteDynArray baData(ulKeyLen);
		P11ER_CALL(pVerifyRecoverMechanism->VerifyRecover(Signature, baData),
			ERR_CANT_FINAL_MECHANISM)

			if (!Data.isNull() && Data.size()<baData.size()) {
				pVerifyRecoverMechanism = std::move(mech);
				_return(CKR_BUFFER_TOO_SMALL)
			}

		Data = Data.left(baData.size());
		if (Data.isNull()) {
			pVerifyRecoverMechanism = std::move(mech);
			_return(OK)
		}


		Data.copy(baData);

		_return(OK)
			exit_func
			_return(FAIL)
	}

	/* ******************* */
	/*       Sign          */
	/* ******************* */

	CK_RV CSession::SignInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	{
		init_func
			if (pSignMechanism)
				_return(CKR_OPERATION_ACTIVE)

				std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(hKey, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_KEY_HANDLE_INVALID)
				if (pObject->ObjClass != CKO_PRIVATE_KEY)
					_return(CKR_KEY_HANDLE_INVALID)
					auto pSignKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		bool bPrivate = false;
		P11ER_CALL(pSignKey->IsPrivate(bPrivate),
			ERR_GET_PRIVATE)

			if (bPrivate && pSlot->User != CKU_USER)
				_return(CKR_USER_NOT_LOGGED_IN)

				ByteArray *baAttrVal = NULL;
		P11ER_CALL(pSignKey->getAttribute(CKA_SIGN, baAttrVal),
			ERR_GET_ATTRIBUTE)
			if (baAttrVal == NULL)
				_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			else {
				if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
					_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			}

		switch (pMechanism->mechanism) {
		case CKM_SHA1_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSAwithSHA1>(new CRSAwithSHA1(shared_from_this()));
			P11ER_CALL(mech->SignInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pSignMechanism = std::move(mech);
			break;
		}
		case CKM_MD5_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSAwithMD5>(new CRSAwithMD5(shared_from_this()));
			P11ER_CALL(mech->SignInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pSignMechanism = std::move(mech);
			break;
		}
		case CKM_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
			P11ER_CALL(mech->SignInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pSignMechanism = std::move(mech);
			break;
		}
		case CKM_RSA_X_509:
		{
			auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
			P11ER_CALL(mech->SignInit(hKey), ERR_CANT_INITIALIZE_MECHANISM)
				pSignMechanism = std::move(mech);
			break;
		}
		default:
			_return(CKR_MECHANISM_INVALID)
		}

		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::Sign(ByteArray &Data, ByteArray &Signature)
	{
		init_func
			if (!pSignMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				bool bFound = false;
		CK_RV result = pSignMechanism->SignReset();
		if (result != OK)
			_return(result)

			result = SignUpdate(Data);
		if (result != OK)
			_return(result)

			DWORD ret = SignFinal(Signature);

		_return(ret)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::SignUpdate(ByteArray &Data)
	{
		init_func
			if (!pSignMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				DWORD ret = pSignMechanism->SignUpdate(Data);
		_return(ret)

			_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::SignFinal(ByteArray &Signature)
	{
		init_func
			if (!pSignMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				auto mech = make_resetter(pSignMechanism);

		std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(pSignMechanism->hSignKey, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_KEY_HANDLE_INVALID)
				if (pObject->ObjClass != CKO_PRIVATE_KEY)
					_return(CKR_KEY_HANDLE_INVALID)
					auto pSignKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		bool bPrivate = false;
		P11ER_CALL(pSignKey->IsPrivate(bPrivate),
			ERR_GET_PRIVATE)

			if (bPrivate && pSlot->User != CKU_USER)
				_return(CKR_USER_NOT_LOGGED_IN)

				if (Signature.isNull()) {
					CK_ULONG ulSignLength;
					P11ER_CALL(pSignMechanism->SignLength(&ulSignLength),
						ERR_CANT_GET_PRIVKEY_LENGTH)
						Signature = Signature.left(ulSignLength);
					mech.release();
					_return(OK)
				}

		ByteDynArray baSignBuffer;
		RESULT ris;
		if (ris = pSignMechanism->SignFinal(baSignBuffer))
			_return(ris)

			bool bSilent = false;
		ByteDynArray baSignature;
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateSign(pSlot->pTemplateData, pSignKey.get(), baSignBuffer, baSignature, pSignMechanism->mtType, bSilent),
			ERR_CANT_SIGN)

			if (Signature.size()<baSignature.size()) {
				Signature = Signature.left(baSignature.size());
				mech.release();
				_return(CKR_BUFFER_TOO_SMALL)
			}
			else {
				Signature.copy(baSignature);
				Signature = Signature.left(baSignature.size());
			}

			_return(OK)
				exit_func
				_return(FAIL)
	}

	/* ******************* */
	/*       SignRecover   */
	/* ******************* */

	CK_RV CSession::SignRecoverInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	{
		init_func
			if (pSignRecoverMechanism)
				_return(CKR_OPERATION_ACTIVE)

				std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(hKey, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_KEY_HANDLE_INVALID)
				if (pObject->ObjClass != CKO_PRIVATE_KEY)
					_return(CKR_KEY_HANDLE_INVALID)
					auto pSignRecoverKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		bool bPrivate = false;
		P11ER_CALL(pSignRecoverKey->IsPrivate(bPrivate),
			ERR_GET_PRIVATE)

			if (bPrivate && pSlot->User != CKU_USER)
				_return(CKR_USER_NOT_LOGGED_IN)

				ByteArray *baAttrVal = NULL;
		P11ER_CALL(pSignRecoverKey->getAttribute(CKA_SIGN_RECOVER, baAttrVal),
			ERR_GET_ATTRIBUTE)
			if (baAttrVal == NULL)
				_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			else {
				if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
					_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			}

		switch (pMechanism->mechanism) {
		case CKM_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
			P11ER_CALL(mech->SignRecoverInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pSignRecoverMechanism = std::move(mech);
			break;
		}
		case CKM_RSA_X_509:
		{
			auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
			P11ER_CALL(mech->SignRecoverInit(hKey), ERR_CANT_INITIALIZE_MECHANISM)
				pSignRecoverMechanism = std::move(mech);
			break;
		}
		default:
			_return(CKR_MECHANISM_INVALID)
		}

		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::SignRecover(ByteArray &Data, ByteArray &Signature)
	{
		init_func
			if (!pSignRecoverMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				auto mech = make_resetter(pSignRecoverMechanism);

		std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(pSignRecoverMechanism->hSignRecoverKey, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_KEY_HANDLE_INVALID)
				if (pObject->ObjClass != CKO_PRIVATE_KEY)
					_return(CKR_KEY_HANDLE_INVALID)
					auto pSignRecoverKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		bool bPrivate = false;
		P11ER_CALL(pSignRecoverKey->IsPrivate(bPrivate),
			ERR_GET_PRIVATE)

			if (bPrivate && pSlot->User != CKU_USER)
				_return(CKR_USER_NOT_LOGGED_IN)

				if (Signature.isNull()) {
					CK_ULONG ulSignRecoverLength;
					P11ER_CALL(pSignRecoverMechanism->SignRecoverLength(&ulSignRecoverLength),
						ERR_CANT_GET_PRIVKEY_LENGTH)
						Signature = Signature.left(ulSignRecoverLength);
					mech.release();
					_return(OK)
				}

		ByteDynArray baSignRecoverBuffer;
		RESULT ris;
		if (ris = pSignRecoverMechanism->SignRecover(Data, baSignRecoverBuffer))
			_return(ris)

			bool bSilent = false;

		ByteDynArray baSignature;
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateSignRecover(pSlot->pTemplateData, pSignRecoverKey.get(), baSignRecoverBuffer, baSignature, pSignRecoverMechanism->mtType, bSilent),
			ERR_CANT_SIGN)

			if (Signature.size()<baSignature.size()) {
				Signature = Signature.left(baSignature.size());
				mech.release();
				_return(CKR_BUFFER_TOO_SMALL)
			}

		_return(OK)
			exit_func
			_return(FAIL)
	}

	/* ******************* */
	/*       Encrypt        */
	/* ******************* */

	CK_RV CSession::EncryptInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	{
		init_func
			if (pEncryptMechanism)
				_return(CKR_OPERATION_ACTIVE)

				std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(hKey, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_KEY_HANDLE_INVALID)
				if (pObject->ObjClass != CKO_PUBLIC_KEY)
					_return(CKR_KEY_HANDLE_INVALID)
					auto pEncryptKey = std::static_pointer_cast<CP11PublicKey>(pObject);

		bool bPrivate = false;
		P11ER_CALL(pEncryptKey->IsPrivate(bPrivate),
			ERR_GET_PRIVATE)

			if (bPrivate && pSlot->User != CKU_USER)
				_return(CKR_USER_NOT_LOGGED_IN)

				ByteArray *baAttrVal = NULL;
		P11ER_CALL(pEncryptKey->getAttribute(CKA_ENCRYPT, baAttrVal),
			ERR_GET_ATTRIBUTE)
			if (baAttrVal == NULL)
				_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			else {
				if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
					_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			}

		switch (pMechanism->mechanism) {
		case CKM_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
			P11ER_CALL(mech->EncryptInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pEncryptMechanism = std::move(mech);
			break;
		}
		case CKM_RSA_X_509:
		{
			auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
			P11ER_CALL(mech->EncryptInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pEncryptMechanism = std::move(mech);
			break;
		}
		default:
			_return(CKR_MECHANISM_INVALID)
		}

		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::Encrypt(ByteArray &Data, ByteArray &EncryptedData)
	{
		init_func

			if (!pEncryptMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				CK_ULONG ulReqLen = 0;
		P11ER_CALL(pEncryptMechanism->EncryptLength(&ulReqLen),
			ERR_CANT_GET_MECHANISM_LENGTH)

			if (!EncryptedData.isNull() && EncryptedData.size()<ulReqLen)
				_return(CKR_BUFFER_TOO_SMALL)

				EncryptedData = EncryptedData.left(ulReqLen);
		if (EncryptedData.isNull())
			_return(OK)

			CK_RV result = EncryptUpdate(Data, EncryptedData);
		if (result != OK)
			_return(result)

			DWORD ret = EncryptFinal(EncryptedData);
		_return(ret)

			exit_func
			_return(FAIL)
	}

	CK_RV CSession::EncryptUpdate(ByteArray &Data, ByteArray &EncryptedData)
	{
		init_func
			if (!pEncryptMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				ByteDynArray baEncryptedData;
		P11ER_CALL(pEncryptMechanism->EncryptUpdate(Data, baEncryptedData),
			ERR_CANT_UPDATE_MECHANISM)

			_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::EncryptFinal(ByteArray &EncryptedData)
	{
		init_func
			if (!pEncryptMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				auto mech = make_resetter(pEncryptMechanism);
		CK_ULONG ulReqLen = 0;
		P11ER_CALL(pEncryptMechanism->EncryptLength(&ulReqLen),
			ERR_CANT_GET_MECHANISM_LENGTH)

			if (!EncryptedData.isNull() && EncryptedData.size()<ulReqLen) {
				mech.release();
				_return(CKR_BUFFER_TOO_SMALL)
			}

		EncryptedData = EncryptedData.left(ulReqLen);
		if (EncryptedData.isNull()) {
			mech.release();
			_return(OK)
		}

		ByteDynArray baEncryptedBuffer;
		RESULT ris;
		if (ris = pEncryptMechanism->EncryptFinal(baEncryptedBuffer))
			_return(ris)

			EncryptedData.copy(baEncryptedBuffer);

		_return(OK)
			exit_func
			_return(FAIL)
	}

	/* ******************** */
	/*		Decrypt			*/
	/* ******************** */

	CK_RV CSession::DecryptInit(CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
	{
		init_func
			if (pDecryptMechanism)
				_return(CKR_OPERATION_ACTIVE)

				std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(hKey, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_KEY_HANDLE_INVALID)
				if (pObject->ObjClass != CKO_PRIVATE_KEY)
					_return(CKR_KEY_HANDLE_INVALID)
					auto pDecryptKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		bool bPrivate = false;
		P11ER_CALL(pDecryptKey->IsPrivate(bPrivate),
			ERR_GET_PRIVATE)

			if (bPrivate && pSlot->User != CKU_USER)
				_return(CKR_USER_NOT_LOGGED_IN)

				ByteArray *baAttrVal = NULL;
		P11ER_CALL(pDecryptKey->getAttribute(CKA_DECRYPT, baAttrVal),
			ERR_GET_ATTRIBUTE)
			if (baAttrVal == NULL)
				_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			else {
				if (ByteArrayToVar(*baAttrVal, CK_BBOOL) == FALSE)
					_return(CKR_KEY_FUNCTION_NOT_PERMITTED)
			}

		switch (pMechanism->mechanism) {
		case CKM_RSA_PKCS:
		{
			auto mech = std::unique_ptr<CRSA_PKCS1>(new CRSA_PKCS1(shared_from_this()));
			P11ER_CALL(mech->DecryptInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pDecryptMechanism = std::move(mech);
			break;
		}
		case CKM_RSA_X_509:
		{
			auto mech = std::unique_ptr<CRSA_X509>(new CRSA_X509(shared_from_this()));
			P11ER_CALL(mech->DecryptInit(hKey),
				ERR_CANT_INITIALIZE_MECHANISM)
				pDecryptMechanism = std::move(mech);
			break;
		}
		default:
			_return(CKR_MECHANISM_INVALID)
		}

		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::Decrypt(ByteArray &EncryptedData, ByteArray &Data)
	{
		init_func

			if (!pDecryptMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				bool bFound = false;
		P11ER_CALL(pDecryptMechanism->checkCache(EncryptedData, Data, bFound),
			ERR_MECHANISM_CACHE)
			if (bFound) {
				pDecryptMechanism.reset();
				_return(OK)
			}

		CK_RV result = DecryptUpdate(EncryptedData, Data);
		if (result != OK)
			_return(result)

			DWORD ret = DecryptFinal(Data);

		if (ret == OK && Data.isNull())
			pDecryptMechanism->cacheData = EncryptedData;

		_return(ret)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::DecryptUpdate(ByteArray &EncryptedData, ByteArray &Data)
	{
		init_func
			if (!pDecryptMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				ByteDynArray baData;
		P11ER_CALL(pDecryptMechanism->DecryptUpdate(EncryptedData, baData),
			ERR_CANT_UPDATE_MECHANISM)

			_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::DecryptFinal(ByteArray &Data)
	{
		init_func
			if (!pDecryptMechanism)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				auto mech = make_resetter(pDecryptMechanism);
		bool bFound = false;
		P11ER_CALL(pDecryptMechanism->checkCache(ByteArray(), Data, bFound),
			ERR_MECHANISM_CACHE)
			if (bFound)
				_return(OK)

				std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(pDecryptMechanism->hDecryptKey, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_KEY_HANDLE_INVALID)
				if (pObject->ObjClass != CKO_PRIVATE_KEY)
					_return(CKR_KEY_HANDLE_INVALID)
					auto pDecryptKey = std::static_pointer_cast<CP11PrivateKey>(pObject);

		bool bPrivate = false;
		P11ER_CALL(pDecryptKey->IsPrivate(bPrivate),
			ERR_GET_PRIVATE)

			if (bPrivate && pSlot->User != CKU_USER)
				_return(CKR_USER_NOT_LOGGED_IN)

				ByteArray *baKeyModule = NULL;

		P11ER_CALL(pDecryptKey->getAttribute(CKA_MODULUS, baKeyModule),
			ERR_CANT_GET_PUBKEY_MODULUS)
			ER_ASSERT(baKeyModule != NULL, ERR_CANT_GET_PUBKEY_MODULUS)

			size_t dwKeyLenBytes = baKeyModule->size();

		ByteDynArray baDecryptBuffer;
		RESULT ris;
		if (ris = pDecryptMechanism->DecryptFinal(baDecryptBuffer))
			_return(ris)

			ByteDynArray baData;
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateDecrypt(pSlot->pTemplateData, pDecryptKey.get(), baDecryptBuffer, baData, pDecryptMechanism->mtType, false),
			ERR_CANT_SIGN)

			ByteDynArray baUnpaddedData;
		P11ER_CALL(pDecryptMechanism->DecryptRemovePadding(baData, baUnpaddedData),
			ERR_PADDING)

			if (Data.isNull()) {
				P11ER_CALL(pDecryptMechanism->setCache(ByteArray(), baUnpaddedData),
					ERR_MECHANISM_CACHE)
					Data = Data.left(baUnpaddedData.size());
				mech.release();
			}
			else if (Data.size()<baUnpaddedData.size()) {
				P11ER_CALL(pDecryptMechanism->setCache(ByteArray(), baUnpaddedData),
					ERR_MECHANISM_CACHE)
					Data = Data.left(baUnpaddedData.size());
				mech.release();
				_return(CKR_BUFFER_TOO_SMALL)
			}
			else {
				P11ER_CALL(pDecryptMechanism->setCache(ByteArray(), baUnpaddedData),
					ERR_MECHANISM_CACHE)
					Data.copy(baUnpaddedData);
				Data = Data.left(baUnpaddedData.size());
			}

			_return(OK)
				exit_func
				_return(FAIL)
	}

	CK_RV CSession::GenerateRandom(ByteArray &RandomData)
	{
		init_func
			ByteDynArray baRandom(RandomData.size());
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateGenerateRandom(pSlot->pTemplateData, baRandom),
			ERR_GENERATE_RANDOM)
			RandomData.copy(baRandom);
		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::InitPIN(ByteArray &Pin)
	{
		init_func

			if (pSlot->User != CKU_SO)
				_return(CKR_USER_NOT_LOGGED_IN)

				P11ER_CALL(pSlot->pTemplate->FunctionList.templateInitPIN(pSlot->pTemplateData, Pin),
				ERR_INIT_PIN)
				_return(OK)
				exit_func
				_return(FAIL)
	}

	CK_RV CSession::SetPIN(ByteArray &OldPin, ByteArray &NewPin)
	{
		init_func

			P11ER_CALL(pSlot->pTemplate->FunctionList.templateSetPIN(pSlot->pTemplateData, OldPin, NewPin, pSlot->User),
			ERR_SET_PIN)
			_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::GetObjectSize(CK_OBJECT_HANDLE hObject, CK_ULONG_PTR pulSize)
	{
		init_func

			std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(hObject, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_OBJECT_HANDLE_INVALID)

				bool bPrivate = false;
		P11ER_CALL(pObject->IsPrivate(bPrivate),
			ERR_GET_PRIVATE)

			if (bPrivate && pSlot->User != CKU_USER)
				_return(CKR_USER_NOT_LOGGED_IN)

				DWORD ret = pObject->GetObjectSize(pulSize);
		_return(ret)

			exit_func
			_return(FAIL)
	}

	CK_RV CSession::SetAttributeValue(CK_OBJECT_HANDLE hObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
	{
		init_func
			std::shared_ptr<CP11Object> pObject;
		P11ER_CALL(pSlot->GetObjectFromID(hObject, pObject),
			ERR_CANT_GET_OBJECT)
			if (pObject == NULL)
				_return(CKR_OBJECT_HANDLE_INVALID)

				if ((flags & CKF_RW_SESSION) == 0)
					_return(CKR_SESSION_READ_ONLY)

					if (pSlot->User != CKU_USER)
						_return(CKR_USER_NOT_LOGGED_IN)

						P11ER_CALL(pSlot->pTemplate->FunctionList.templateSetAttribute(pSlot->pTemplateData, pObject.get(), pTemplate, ulCount),
						ERR_SET_ATTRIBUTE)


						_return(OK)
						exit_func
						_return(FAIL)
	}

	CK_RV CSession::SetOperationState(ByteArray &OperationState)
	{
		init_func
			CTLV Tlv(OperationState);

		ByteArray Flags, User;
		P11ER_CALL(Tlv.getValue(OS_Flags, Flags),
			ERR_TLV)
			if (Flags.isNull()) _return(CKR_SAVED_STATE_INVALID)
				P11ER_CALL(Tlv.getValue(OS_User, User),
				ERR_TLV)
				if (User.isNull()) _return(CKR_SAVED_STATE_INVALID)

					if (Flags != VarToByteArray(flags))
						_return(CKR_SAVED_STATE_INVALID)
						if (User != VarToByteArray(pSlot->User))
							_return(CKR_SAVED_STATE_INVALID)

							ByteArray SignOperationState;
		P11ER_CALL(Tlv.getValue(OS_Sign, SignOperationState),
			ERR_TLV)
			if (!SignOperationState.isNull()) {
				pSignMechanism.reset();
				CTLV SignTlv(SignOperationState);
				ByteArray Algo;
				P11ER_CALL(SignTlv.getValue(OS_Algo, Algo), ERR_TLV)
					if (Algo.isNull())
						_return(CKR_SAVED_STATE_INVALID)
						CK_MECHANISM mech = { ByteArrayToVar(Algo, CK_MECHANISM_TYPE), NULL, 0 };

				ByteArray Key;
				P11ER_CALL(SignTlv.getValue(OS_Key, Key), ERR_TLV)
					if (Key.isNull())
						_return(CKR_SAVED_STATE_INVALID)

						std::shared_ptr<CP11Object> pKey;
				P11ER_CALL(pSlot->FindP11Object(CKO_PRIVATE_KEY, CKA_ID, Key.data(), (int)Key.size(), pKey),
					ERR_FIND_OBJECT)
					ER_ASSERT(pKey, ERR_CANT_GET_OBJECT)
					CK_OBJECT_HANDLE hKey;
				P11ER_CALL(pSlot->GetIDFromObject(pKey, hKey),
					ERR_GET_OBJECT_HANDLE)

					ByteArray Data;
				P11ER_CALL(SignTlv.getValue(OS_Data, Data), ERR_TLV)
					if (Data.isNull())
						_return(CKR_SAVED_STATE_INVALID)

						P11ER_CALL(SignInit(&mech, hKey),
						ERR_CANT_INITIALIZE_MECHANISM);

				P11ER_CALL(pSignMechanism->SignSetOperationState(Data),
					ERR_SET_OPERATION_STATE)
			}

		ByteArray DecryptOperationState;
		P11ER_CALL(Tlv.getValue(OS_Decrypt, DecryptOperationState),
			ERR_TLV)
			if (!DecryptOperationState.isNull()) {
				pDecryptMechanism.reset();
				CTLV DecryptTlv(DecryptOperationState);
				ByteArray Algo;
				P11ER_CALL(DecryptTlv.getValue(OS_Algo, Algo), ERR_TLV)
					if (Algo.isNull())
						_return(CKR_SAVED_STATE_INVALID)
						CK_MECHANISM mech = { ByteArrayToVar(Algo, CK_MECHANISM_TYPE), NULL, 0 };

				ByteArray Key;
				P11ER_CALL(DecryptTlv.getValue(OS_Key, Key), ERR_TLV)
					if (Key.isNull())
						_return(CKR_SAVED_STATE_INVALID)

						std::shared_ptr<CP11Object> pKey;
				P11ER_CALL(pSlot->FindP11Object(CKO_PRIVATE_KEY, CKA_ID, Key.data(), (int)Key.size(), pKey),
					ERR_FIND_OBJECT)
					ER_ASSERT(pKey, ERR_CANT_GET_OBJECT)
					CK_OBJECT_HANDLE hKey;
				P11ER_CALL(pSlot->GetIDFromObject(pKey, hKey),
					ERR_GET_OBJECT_HANDLE)

					ByteArray Data;
				P11ER_CALL(DecryptTlv.getValue(OS_Data, Data), ERR_TLV)
					if (Data.isNull())
						_return(CKR_SAVED_STATE_INVALID)

						P11ER_CALL(DecryptInit(&mech, hKey),
						ERR_CANT_INITIALIZE_MECHANISM);

				P11ER_CALL(pDecryptMechanism->DecryptSetOperationState(Data),
					ERR_SET_OPERATION_STATE)
			}

		ByteArray VerifyOperationState;
		P11ER_CALL(Tlv.getValue(OS_Verify, VerifyOperationState),
			ERR_TLV)
			if (!VerifyOperationState.isNull()) {
				pVerifyMechanism.reset();
				CTLV VerifyTlv(VerifyOperationState);
				ByteArray Algo;
				P11ER_CALL(VerifyTlv.getValue(OS_Algo, Algo), ERR_TLV)
					if (Algo.isNull())
						_return(CKR_SAVED_STATE_INVALID)
						CK_MECHANISM mech = { ByteArrayToVar(Algo, CK_MECHANISM_TYPE), NULL, 0 };

				ByteArray Key;
				P11ER_CALL(VerifyTlv.getValue(OS_Key, Key), ERR_TLV)
					if (Key.isNull())
						_return(CKR_SAVED_STATE_INVALID)

						std::shared_ptr<CP11Object> pKey;
				P11ER_CALL(pSlot->FindP11Object(CKO_PUBLIC_KEY, CKA_ID, Key.data(), (int)Key.size(), pKey),
					ERR_FIND_OBJECT)
					ER_ASSERT(pKey, ERR_CANT_GET_OBJECT)
					CK_OBJECT_HANDLE hKey;
				P11ER_CALL(pSlot->GetIDFromObject(pKey, hKey),
					ERR_GET_OBJECT_HANDLE)

					ByteArray Data;
				P11ER_CALL(VerifyTlv.getValue(OS_Data, Data), ERR_TLV)
					if (Data.isNull())
						_return(CKR_SAVED_STATE_INVALID)

						P11ER_CALL(VerifyInit(&mech, hKey),
						ERR_CANT_INITIALIZE_MECHANISM);

				P11ER_CALL(pVerifyMechanism->VerifySetOperationState(Data),
					ERR_SET_OPERATION_STATE)
			}

		ByteArray EncryptOperationState;
		P11ER_CALL(Tlv.getValue(OS_Encrypt, EncryptOperationState),
			ERR_TLV)
			if (!EncryptOperationState.isNull()) {
				pEncryptMechanism.reset();
				CTLV EncryptTlv(EncryptOperationState);
				ByteArray Algo;
				P11ER_CALL(EncryptTlv.getValue(OS_Algo, Algo), ERR_TLV)
					if (Algo.isNull())
						_return(CKR_SAVED_STATE_INVALID)
						CK_MECHANISM mech = { ByteArrayToVar(Algo, CK_MECHANISM_TYPE), NULL, 0 };

				ByteArray Key;
				P11ER_CALL(EncryptTlv.getValue(OS_Key, Key), ERR_TLV)
					if (Key.isNull())
						_return(CKR_SAVED_STATE_INVALID)

						std::shared_ptr<CP11Object> pKey;
				P11ER_CALL(pSlot->FindP11Object(CKO_PUBLIC_KEY, CKA_ID, Key.data(), (int)Key.size(), pKey),
					ERR_FIND_OBJECT)
					ER_ASSERT(pKey, ERR_CANT_GET_OBJECT)
					CK_OBJECT_HANDLE hKey;
				P11ER_CALL(pSlot->GetIDFromObject(pKey, hKey),
					ERR_GET_OBJECT_HANDLE)

					ByteArray Data;
				P11ER_CALL(EncryptTlv.getValue(OS_Data, Data), ERR_TLV)
					if (Data.isNull())
						_return(CKR_SAVED_STATE_INVALID)

						P11ER_CALL(EncryptInit(&mech, hKey),
						ERR_CANT_INITIALIZE_MECHANISM);

				P11ER_CALL(pEncryptMechanism->EncryptSetOperationState(Data),
					ERR_SET_OPERATION_STATE)
			}

		ByteArray DigestOperationState;
		P11ER_CALL(Tlv.getValue(OS_Digest, DigestOperationState),
			ERR_TLV)
			if (!DigestOperationState.isNull()) {
				pDigestMechanism.reset();
				CTLV DigestTlv(DigestOperationState);
				ByteArray Algo;
				P11ER_CALL(DigestTlv.getValue(OS_Algo, Algo), ERR_TLV)
					if (Algo.isNull())
						_return(CKR_SAVED_STATE_INVALID)
						CK_MECHANISM mech = { ByteArrayToVar(Algo, CK_MECHANISM_TYPE), NULL, 0 };

				ByteArray Data;
				P11ER_CALL(DigestTlv.getValue(OS_Data, Data), ERR_TLV)
					if (Data.isNull())
						_return(CKR_SAVED_STATE_INVALID)

						P11ER_CALL(DigestInit(&mech),
						ERR_CANT_INITIALIZE_MECHANISM);

				P11ER_CALL(pDigestMechanism->DigestSetOperationState(Data),
					ERR_SET_OPERATION_STATE)
			}

		_return(OK)
			exit_func
			_return(FAIL)
	}

	CK_RV CSession::GetOperationState(ByteArray &OperationState)
	{
		init_func

			CTLVCreate Tlv;
		Tlv.setValue(OS_Flags, ByteArray((BYTE*)&flags, sizeof(flags)));
		Tlv.setValue(OS_User, ByteArray((BYTE*)&pSlot->User, sizeof(pSlot->User)));
		if (pSignMechanism) {
			CTLVCreate SignTlv;
			P11ER_CALL(SignTlv.setValue(OS_Algo, ByteArray((BYTE*)&pSignMechanism->mtType, sizeof(pSignMechanism->mtType))),
				ERR_TLV)
				ByteDynArray SignData;
			P11ER_CALL(pSignMechanism->SignGetOperationState(SignData),
				ERR_GET_OPERATION_STATE)
				if (!SignData.isEmpty()) {
					P11ER_CALL(SignTlv.setValue(OS_Data, SignData),
						ERR_TLV)
				}

			std::shared_ptr<CP11Object> pKey;
			P11ER_CALL(pSlot->GetObjectFromID(pSignMechanism->hSignKey, pKey),
				ERR_CANT_GET_OBJECT)
				ER_ASSERT(pKey, ERR_CANT_GET_OBJECT);
			ByteArray *SignKeyID = NULL;
			P11ER_CALL(pKey->getAttribute(CKA_ID, SignKeyID), ERR_GET_ATTRIBUTE);
			ER_ASSERT(SignKeyID, ERR_CANT_FIND_ID);
			SignTlv.setValue(OS_Key, *SignKeyID);

			ByteDynArray *SignOperationState;
			P11ER_CALL(Tlv.addValue(OS_Sign, SignOperationState),
				ERR_TLV)
				P11ER_CALL(SignTlv.getBuffer(*SignOperationState),
				ERR_TLV)
		}
		if (pVerifyMechanism) {
			CTLVCreate VerifyTlv;
			P11ER_CALL(VerifyTlv.setValue(OS_Algo, ByteArray((BYTE*)&pVerifyMechanism->mtType, sizeof(pVerifyMechanism->mtType))),
				ERR_TLV)
				ByteDynArray verifyData;
			P11ER_CALL(pVerifyMechanism->VerifyGetOperationState(verifyData),
				ERR_GET_OPERATION_STATE)
				if (!verifyData.isEmpty()) {
					P11ER_CALL(VerifyTlv.setValue(OS_Data, verifyData),
						ERR_TLV)
				}

			std::shared_ptr<CP11Object> pKey;
			P11ER_CALL(pSlot->GetObjectFromID(pVerifyMechanism->hVerifyKey, pKey),
				ERR_CANT_GET_OBJECT)
				ER_ASSERT(pKey, ERR_CANT_GET_OBJECT);
			ByteArray *VerifyKeyID = NULL;
			P11ER_CALL(pKey->getAttribute(CKA_ID, VerifyKeyID), ERR_GET_ATTRIBUTE);
			ER_ASSERT(VerifyKeyID, ERR_CANT_FIND_ID);
			VerifyTlv.setValue(OS_Key, *VerifyKeyID);

			ByteDynArray *VerifyOperationState;
			P11ER_CALL(Tlv.addValue(OS_Verify, VerifyOperationState),
				ERR_TLV)
				P11ER_CALL(VerifyTlv.getBuffer(*VerifyOperationState),
				ERR_TLV)
		}
		if (pDigestMechanism) {
			CTLVCreate DigestTlv;
			P11ER_CALL(DigestTlv.setValue(OS_Algo, ByteArray((BYTE*)&pDigestMechanism->mtType, sizeof(pDigestMechanism->mtType))),
				ERR_TLV)
				ByteDynArray DigestData;
			P11ER_CALL(pDigestMechanism->DigestGetOperationState(DigestData),
				ERR_GET_OPERATION_STATE)
				if (!DigestData.isEmpty())
					P11ER_CALL(DigestTlv.setValue(OS_Data, DigestData),
					ERR_TLV)

					ByteDynArray *DigestOperationState;
			P11ER_CALL(Tlv.addValue(OS_Digest, DigestOperationState),
				ERR_TLV)
				P11ER_CALL(DigestTlv.getBuffer(*DigestOperationState),
				ERR_TLV)
		}
		if (pEncryptMechanism) {
			CTLVCreate EncryptTlv;
			P11ER_CALL(EncryptTlv.setValue(OS_Algo, ByteArray((BYTE*)&pEncryptMechanism->mtType, sizeof(pEncryptMechanism->mtType))),
				ERR_TLV)
				ByteDynArray EncryptData;
			P11ER_CALL(pEncryptMechanism->EncryptGetOperationState(EncryptData),
				ERR_GET_OPERATION_STATE)
				if (!EncryptData.isEmpty()) {
					P11ER_CALL(EncryptTlv.setValue(OS_Data, EncryptData),
						ERR_TLV)
				}

			std::shared_ptr<CP11Object> pKey;
			P11ER_CALL(pSlot->GetObjectFromID(pEncryptMechanism->hEncryptKey, pKey),
				ERR_CANT_GET_OBJECT)
				ER_ASSERT(pKey, ERR_CANT_GET_OBJECT);
			ByteArray *EncryptKeyID = NULL;
			P11ER_CALL(pKey->getAttribute(CKA_ID, EncryptKeyID), ERR_GET_ATTRIBUTE);
			ER_ASSERT(EncryptKeyID, ERR_CANT_FIND_ID);
			EncryptTlv.setValue(OS_Key, *EncryptKeyID);

			ByteDynArray *EncryptOperationState;
			P11ER_CALL(Tlv.addValue(OS_Encrypt, EncryptOperationState),
				ERR_TLV)
				P11ER_CALL(EncryptTlv.getBuffer(*EncryptOperationState),
				ERR_TLV)
		}
		if (pDecryptMechanism) {
			CTLVCreate DecryptTlv;
			P11ER_CALL(DecryptTlv.setValue(OS_Algo, ByteArray((BYTE*)&pDecryptMechanism->mtType, sizeof(pDecryptMechanism->mtType))),
				ERR_TLV)
				ByteDynArray DecryptData;
			P11ER_CALL(pDecryptMechanism->DecryptGetOperationState(DecryptData),
				ERR_GET_OPERATION_STATE)
				if (!DecryptData.isEmpty()) {
					P11ER_CALL(DecryptTlv.setValue(OS_Data, DecryptData),
						ERR_TLV)
				}

			std::shared_ptr<CP11Object> pKey;
			P11ER_CALL(pSlot->GetObjectFromID(pDecryptMechanism->hDecryptKey, pKey),
				ERR_CANT_GET_OBJECT)
				ER_ASSERT(pKey, ERR_CANT_GET_OBJECT);
			ByteArray *DecryptKeyID = NULL;
			P11ER_CALL(pKey->getAttribute(CKA_ID, DecryptKeyID), ERR_GET_ATTRIBUTE);
			ER_ASSERT(DecryptKeyID, ERR_CANT_FIND_ID);
			DecryptTlv.setValue(OS_Key, *DecryptKeyID);

			ByteDynArray *DecryptOperationState;
			P11ER_CALL(Tlv.addValue(OS_Decrypt, DecryptOperationState),
				ERR_TLV)
				P11ER_CALL(DecryptTlv.getBuffer(*DecryptOperationState),
				ERR_TLV)
		}
		ByteDynArray newOperationState;
		P11ER_CALL(Tlv.getBuffer(newOperationState),
			ERR_TLV)

			if (newOperationState.size() == 0)
				_return(CKR_OPERATION_NOT_INITIALIZED)

				if (OperationState.isNull()) {
					OperationState = OperationState.left(newOperationState.size());
					_return(OK)
				}
		if (OperationState.size()<newOperationState.size())
			_return(CKR_BUFFER_TOO_SMALL)

			OperationState.copy(newOperationState);
		_return(OK)
			exit_func
			_return(FAIL)
	}

};

