// P11Emissione.cpp : Defines the entry point for the DLL application.
//

#include "..\StdAfx.h"
#include "PKCS11Functions.h"
#include "InitP11.h"
#include <winscard.h>
#include "session.h"
#include "cardtemplate.h"
#include <malloc.h>
#include "../util/ModuleInfo.h"
#include "../util/util.h"
#include "../util/syncroevent.h"

static char *szCompiledFile=__FILE__;

// flag: P11 inizializzato
bool bP11Initialized=false;
bool bP11Terminate=false;

using namespace p11;

// Function list P11
CK_FUNCTION_LIST m_FunctionList;
CSyncroMutex p11Mutex;
CSyncroEvent p11slotEvent/*("CardOS_P11_Event")*/;

// meccanismi supportati

CK_MECHANISM_TYPE P11mechanisms[]= {
	//CKM_RSA_PKCS_KEY_PAIR_GEN,
	CKM_RSA_PKCS,
	CKM_RSA_X_509,
	CKM_MD5,
	CKM_SHA_1,
	CKM_SHA1_RSA_PKCS,
	CKM_MD5_RSA_PKCS
};

char *getAttributeName(DWORD dwId);
extern CModuleInfo moduleInfo; // informazioni sulla dll (o so)
bool bModuleInit=false;

// funzione DllMain
// inizilizzo moduleInfo
BOOL APIENTRY DllMainP11( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	init_func_internal
	if (ul_reason_for_call==DLL_PROCESS_ATTACH && !bModuleInit) {
		bModuleInit=true;
		moduleInfo.init(hModule);
		std::string mainMutexName;
		mainMutexName="CIE_P11_Mutex_"+moduleInfo.szModuleName;
		p11Mutex.Create(mainMutexName.c_str());
		//xmlInit();
		std::string configPath;
		configPath = moduleInfo.szModulePath + moduleInfo.szModuleName + ".ini";
		initLog(configPath.c_str(), __DATE__ " " __TIME__);
		Log.initModule("PKCS11", __DATE__ " " __TIME__);
		p11::InitP11(configPath.c_str());

	}

	if (ul_reason_for_call==DLL_PROCESS_DETACH && bModuleInit) {

		if (bP11Initialized) {
			Log.write("%s","Forzatura C_Finalize");
			C_Finalize(NULL);
		}
		//xmlCleanup();
		bModuleInit=false;

		bP11Terminate=true;

		if (CSlot::DeleteSlotList()==FAIL) 
			return FALSE;

		if (CCardTemplate::DeleteTemplateList() == FAIL)
			return FALSE;

		p11slotEvent.Signal();

	}

	return TRUE;
	exit_func_internal
    return FALSE;
}

// funzione CheckMechanismParam
RESULT CheckMechanismParam(CK_MECHANISM *pParam,bool &bValid) {
	init_func
	// tutti i mechanism non hanno parametri
	if (pParam->pParameter==NULL && pParam->ulParameterLen==0) {
		bValid=true;
		_return(OK)
	}
	bValid=FALSE;
	_return(OK)
	exit_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_UpdateSlotList()
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	CSlot::InitSlotList();

	_return(OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_GetSlotList(CK_BBOOL tokenPresent, CK_SLOT_ID_PTR pSlotList, CK_ULONG_PTR pulCount)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutArray(pSlotList,pulCount)

	logParam(tokenPresent)
	logParam(pSlotList)
	logParam(pulCount)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	bool bOver=false;
	std::vector<CK_SLOT_ID> slotsRet;
	unsigned int iCnt;
	if (tokenPresent) {
		// solo i lettori con la carta inserita
		iCnt=0;

		if (CSlot::g_mSlots.size()==0) {
			*pulCount=0;
			_return(CKR_OK)

		}
		SlotMap::const_iterator it=CSlot::g_mSlots.end();
		do {
			it--;
			std::shared_ptr<CSlot> pSlot=it->second;
			bool bPresent=false;

			pSlot->IsTokenPresent(&bPresent);
			if (bPresent) {
				if (pSlotList) {
					if (iCnt<*pulCount)
						pSlotList[iCnt]=pSlot->hSlot;
					else
						bOver=true;
				}
				iCnt++;
			}
		}
		while (it!=CSlot::g_mSlots.begin());
	}
	else {
		// restituisco tutti i lettori
		if (!pSlotList) {
			iCnt=(CK_ULONG)CSlot::g_mSlots.size();
		}
		else {
			iCnt=0;
			SlotMap::const_iterator it=CSlot::g_mSlots.end();
			while (it!=CSlot::g_mSlots.begin())
			{
				it--;
				if (iCnt<*pulCount)
					pSlotList[iCnt]=it->first;
				else
					bOver=true;
				iCnt++;
			}
		}
	}
	*pulCount=iCnt;
	if (bOver)
		_return(CKR_BUFFER_TOO_SMALL)

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_Initialize(CK_VOID_PTR pReserved)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);
	
	logParam(pReserved)
	
	CK_C_INITIALIZE_ARGS_PTR ptr=(CK_C_INITIALIZE_ARGS_PTR)pReserved;

	if (bP11Initialized)
		_return(OK)
	//	_return(CKR_CRYPTOKI_ALREADY_INITIALIZED)

	// verifico che i flag siano supportati
	CK_C_INITIALIZE_ARGS_PTR iargs = NULL_PTR;
	if (pReserved) {
		iargs =	(CK_C_INITIALIZE_ARGS_PTR)pReserved;
		if (iargs->pReserved !=	NULL)
			_return(CKR_ARGUMENTS_BAD)

		if(iargs->flags	& CKF_OS_LOCKING_OK)
		{
			if ((iargs->CreateMutex) ||	(iargs->DestroyMutex) || (iargs->LockMutex)	|| (iargs->UnlockMutex))
				_return(CKR_CANT_LOCK)
		}
		else if	(iargs->flags == 0)
		{
			if((iargs->CreateMutex)	|| (iargs->DestroyMutex) ||	(iargs->LockMutex) || (iargs->UnlockMutex))
				_return(CKR_CANT_LOCK)
		}
		else if (iargs->flags & CKF_LIBRARY_CANT_CREATE_OS_THREADS)
			_return( CKR_NEED_TO_CREATE_THREADS)
		else
			_return( CKR_ARGUMENTS_BAD)
	}


	if (CCardTemplate::g_mCardTemplates.size()==0) {
		EDF_CALL(CCardTemplate::InitTemplateList(),
			ERR_CANT_INIT_TEMPLATES)
	}
	bP11Initialized = true;

	EDF_CALL(CSlot::InitSlotList()
		,ERR_CANT_INIT_SLOTLIST)

	_return(CKR_OK)

	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_Finalize(CK_VOID_PTR pReserved)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	logParam(pReserved)

	if (pReserved!=NULL)
		_return(CKR_ARGUMENTS_BAD)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	bP11Initialized = false;

	if (CSlot::Thread.dwThreadID != 0) {
		CCardContext *tc = CSlot::ThreadContext;
		if (tc != nullptr) {
			SCARDCONTEXT hC = tc->hContext;
			if (hC != NULL)
				SCardCancel(hC);
		}
		p11Mutex.Unlock();
		CSlot::Thread.joinThread(INFINITE);
		p11Mutex.Lock();
	}

	CSlot::Thread.close();

	for(SlotMap::const_iterator it=CSlot::g_mSlots.begin();it!=CSlot::g_mSlots.end();it++) {
		it->second->CloseAllSessions();
	}

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_OpenSession(CK_SLOT_ID slotID, CK_FLAGS flags, CK_VOID_PTR pApplication, CK_NOTIFY notify, CK_SESSION_HANDLE_PTR phSession)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutPtr(phSession)
		
	logParam(slotID)
	logParam(flags)
	logParam(pApplication)
	logParam(notify)
	logParam(phSession)




	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	if(!(flags & CKF_SERIAL_SESSION)) 
		_return(CKR_SESSION_PARALLEL_NOT_SUPPORTED)

	std::shared_ptr<CSlot> pSlot;
	EDF_CALL(CSlot::GetSlotFromID(slotID,pSlot),
		ERR_CANT_GET_SLOT);
	if (!pSlot)
		_return(CKR_SLOT_ID_INVALID)
	
	auto pSession = std::unique_ptr<CSession>(new CSession());
	pSession->pSlot=pSlot;
	pSession->flags=flags;
	pSession->notify=notify;
	pSession->pApplication=pApplication;

	if ((flags & CKF_RW_SESSION)==0) {
		// se una sessione RO devo
		// verificare che non ci siano sessioni SO RW
		bool bExists=false;
		EDF_CALL(pSession->ExistsSO_RW(bExists),
			ERR_SO_RW_SESSION)
		if (bExists)
			_return(CKR_SESSION_READ_WRITE_SO_EXISTS)
	}

	EDF_CALL(pSlot->Init(),ERR_INIT_SESSION)
	
	pSession->slotID=slotID;

// aggiungo la sessione all'elenco globale
	EDF_CALL(CSession::AddSession(std::move(pSession),*phSession),
		ERR_CANT_ADD_SESSION)

	if (Log.Enabled) {
		Log.writePure("Sessione: %i",*phSession);
		Log.writePure("Lettore: %s",pSlot->szName.c_str());
		Log.writePure("CardManager: %s",pSlot->pTemplate->szName.c_str());
		std::string szModel;
		pSlot->pTemplate->FunctionList.templateGetModel(*pSlot,szModel);
		Log.writePure("Tipo Carta: %s",szModel.c_str());
	}

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_GetTokenInfo(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutPtr(pInfo)
		
	logParam(slotID)
	logParam(pInfo)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSlot> pSlot;
	EDF_CALL(CSlot::GetSlotFromID(slotID,pSlot),
		ERR_CANT_GET_SLOT);

	if (!pSlot)
		_return(CKR_SLOT_ID_INVALID)

	if (pInfo == NULL) 
		_return(CKR_ARGUMENTS_BAD)

	bool bPresent=false;

	RESULT ris;
	ris = pSlot->IsTokenPresent(&bPresent);

	if (!bPresent)
		_return(CKR_TOKEN_NOT_PRESENT)

	pSlot->GetTokenInfo(pInfo);

	info.logParameter(pInfo->firmwareVersion.major);
	info.logParameter(pInfo->firmwareVersion.minor);
	info.logParameter(pInfo->flags);
	info.logParameter(pInfo->hardwareVersion.major);
	info.logParameter(pInfo->hardwareVersion.minor);
	info.logParameter(pInfo->label,sizeof(pInfo->label));
	info.logParameter(pInfo->manufacturerID,sizeof(pInfo->manufacturerID));
	info.logParameter(pInfo->model,sizeof(pInfo->model));
	info.logParameter(pInfo->serialNumber,sizeof(pInfo->serialNumber));
	info.logParameter(pInfo->ulFreePrivateMemory);
	info.logParameter(pInfo->ulFreePublicMemory);
	info.logParameter(pInfo->ulMaxPinLen);
	info.logParameter(pInfo->ulMaxRwSessionCount);
	info.logParameter(pInfo->ulMaxSessionCount);
	info.logParameter(pInfo->ulMinPinLen);
	info.logParameter(pInfo->ulRwSessionCount);
	info.logParameter(pInfo->ulSessionCount);
	info.logParameter(pInfo->ulTotalPrivateMemory);
	info.logParameter(pInfo->ulTotalPublicMemory);
	info.logParameter(pInfo->utcTime,sizeof(pInfo->utcTime));
	
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_CloseSession(CK_SESSION_HANDLE hSession)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	logParam(hSession)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);

	if (!pSession)
		_return(CKR_SESSION_HANDLE_INVALID);

	CSession::DeleteSession(hSession);

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_CloseAllSessions(CK_SLOT_ID slotID)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	logParam(slotID)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSlot> pSlot;
	EDF_CALL(CSlot::GetSlotFromID(slotID,pSlot),
		ERR_CANT_GET_SLOT);

	if (!pSlot)
		_return(CKR_SLOT_ID_INVALID);

	pSlot->CloseAllSessions();

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}


CK_RV CK_ENTRY C_GetSlotInfo(CK_SLOT_ID slotID, CK_SLOT_INFO_PTR pInfo)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutPtr(pInfo)

	logParam(slotID)
	logParam(pInfo)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSlot> pSlot;
	EDF_CALL(CSlot::GetSlotFromID(slotID,pSlot),
		ERR_CANT_GET_SLOT);
	if (!pSlot)
		_return(CKR_SLOT_ID_INVALID);

	if (pInfo == NULL)
		_return(CKR_ARGUMENTS_BAD);

	pSlot->GetInfo(pInfo);
	
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_GetInfo(CK_INFO_PTR pInfo /* location that receives information */)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutPtr(pInfo)

	logParam(pInfo)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	pInfo->cryptokiVersion.major = 2; /* Cryptoki interface ver */
	pInfo->cryptokiVersion.minor = 10;   //12345678901234567890123456789012
	memcpy_s((char*)pInfo->manufacturerID,32,"IPZS                            ", 32);

	pInfo->flags = 0; /* must be zero */

	/* libraryDescription and libraryVersion are new for v2.0 */
	memcpy_s((char*)pInfo->libraryDescription,32,"CIE PKCS11                      ", 32);

	pInfo->libraryVersion.major = 1; /* version of library */
	pInfo->libraryVersion.minor = 0; /* version of library */

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutPtr(ppFunctionList)

	logParam(ppFunctionList)

	if (ppFunctionList ==NULL) 
		_return(CKR_ARGUMENTS_BAD)
	
	*ppFunctionList = &m_FunctionList;

	memset(&m_FunctionList,0,sizeof(m_FunctionList));
	
	m_FunctionList.version.major	= LIBRARY_VERSION_MAJOR;
	m_FunctionList.version.minor	= LIBRARY_VERSION_MINOR;
	
	// Riempie la lista della funzioni
	m_FunctionList.C_CloseAllSessions=&C_CloseAllSessions;
	m_FunctionList.C_CloseSession=&C_CloseSession;
	m_FunctionList.C_CreateObject=&C_CreateObject;
	m_FunctionList.C_Digest=&C_Digest;
	m_FunctionList.C_DigestFinal=&C_DigestFinal;
	m_FunctionList.C_DigestInit=&C_DigestInit;
	m_FunctionList.C_DigestUpdate=&C_DigestUpdate;
	m_FunctionList.C_Finalize=&C_Finalize;
	m_FunctionList.C_FindObjects=&C_FindObjects;
	m_FunctionList.C_FindObjectsFinal=&C_FindObjectsFinal;
	m_FunctionList.C_FindObjectsInit=&C_FindObjectsInit;
	m_FunctionList.C_GetAttributeValue=&C_GetAttributeValue;
	m_FunctionList.C_GetFunctionList=&C_GetFunctionList;
	m_FunctionList.C_GetInfo=&C_GetInfo;
	m_FunctionList.C_GetMechanismInfo=&C_GetMechanismInfo;
	m_FunctionList.C_GetMechanismList=&C_GetMechanismList;
	m_FunctionList.C_GetSessionInfo=&C_GetSessionInfo;
	m_FunctionList.C_GetSlotInfo=&C_GetSlotInfo;
	m_FunctionList.C_GetSlotList=&C_GetSlotList;
	m_FunctionList.C_GetTokenInfo=&C_GetTokenInfo;
	m_FunctionList.C_Initialize=&C_Initialize;
	m_FunctionList.C_Login=&C_Login;
	m_FunctionList.C_Logout=&C_Logout;
	m_FunctionList.C_OpenSession=&C_OpenSession;
	m_FunctionList.C_SetAttributeValue=&C_SetAttributeValue;
	m_FunctionList.C_Sign=&C_Sign;
	m_FunctionList.C_SignFinal=&C_SignFinal;
	m_FunctionList.C_SignInit=&C_SignInit;
	m_FunctionList.C_SignUpdate=&C_SignUpdate;
	m_FunctionList.C_Decrypt=&C_Decrypt;
	m_FunctionList.C_DecryptFinal=&C_DecryptFinal;
	m_FunctionList.C_DecryptInit=&C_DecryptInit;
	m_FunctionList.C_DecryptUpdate=&C_DecryptUpdate;
	m_FunctionList.C_Encrypt=&C_Encrypt;
	m_FunctionList.C_EncryptFinal=&C_EncryptFinal;
	m_FunctionList.C_EncryptInit=&C_EncryptInit;
	m_FunctionList.C_EncryptUpdate=&C_EncryptUpdate;
	m_FunctionList.C_Verify=&C_Verify;
	m_FunctionList.C_VerifyFinal=&C_VerifyFinal;
	m_FunctionList.C_VerifyInit=&C_VerifyInit;
	m_FunctionList.C_VerifyUpdate=&C_VerifyUpdate;
	m_FunctionList.C_WaitForSlotEvent=&C_WaitForSlotEvent;
	m_FunctionList.C_InitToken=&C_InitToken;
	m_FunctionList.C_InitPIN=&C_InitPIN;
	m_FunctionList.C_SetPIN=&C_SetPIN;
	m_FunctionList.C_GetOperationState=&C_GetOperationState;
	m_FunctionList.C_SetOperationState=&C_SetOperationState;
	m_FunctionList.C_CopyObject=&C_CopyObject;
	m_FunctionList.C_DestroyObject=&C_DestroyObject;
	m_FunctionList.C_GetObjectSize=&C_GetObjectSize;
	m_FunctionList.C_DigestKey=&C_DigestKey;
	m_FunctionList.C_SignRecoverInit=&C_SignRecoverInit;
	m_FunctionList.C_SignRecover=&C_SignRecover;
	m_FunctionList.C_VerifyRecoverInit=&C_VerifyRecoverInit;
	m_FunctionList.C_VerifyRecover=&C_VerifyRecover;
	m_FunctionList.C_DigestEncryptUpdate=&C_DigestEncryptUpdate;
	m_FunctionList.C_DecryptDigestUpdate=&C_DecryptDigestUpdate;
	m_FunctionList.C_SignEncryptUpdate=&C_SignEncryptUpdate;
	m_FunctionList.C_DecryptVerifyUpdate=&C_DecryptVerifyUpdate;
	m_FunctionList.C_GenerateKey=&C_GenerateKey;
	m_FunctionList.C_GenerateKeyPair=&C_GenerateKeyPair;
	m_FunctionList.C_WrapKey=&C_WrapKey;
	m_FunctionList.C_UnwrapKey=&C_UnwrapKey;
	m_FunctionList.C_DeriveKey=&C_DeriveKey;
	m_FunctionList.C_SeedRandom=&C_SeedRandom;
	m_FunctionList.C_GenerateRandom=&C_GenerateRandom;
	m_FunctionList.C_GetFunctionStatus=&C_GetFunctionStatus;
	m_FunctionList.C_CancelFunction=&C_CancelFunction;

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_CreateObject(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR phObject)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);
	

	checkOutPtr(phObject)

	logParam(hSession)
	logParam(pTemplate)
	logParam(ulCount)
	logParam(phObject)


	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED);

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);

	if (!pSession) 
		_return(CKR_SESSION_HANDLE_INVALID);

	pSession->CreateObject(pTemplate, ulCount, phObject);

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_GenerateKey(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR phKey)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);
	
	checkInPtr(pMechanism)
	checkOutPtr(phKey)
	
	logParam(hSession)
	logParam(pMechanism)
	logParam(pTemplate)
	logParam(ulCount)
	logParam(phKey)


	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);

	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	DF_CALL(pSession->GenerateKey(pMechanism, pTemplate, ulCount, phKey))
	_return(CKR_OK)

	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_GenerateKeyPair(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount, CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount, CK_OBJECT_HANDLE_PTR phPublicKey, CK_OBJECT_HANDLE_PTR phPrivateKey)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);
	
	checkInPtr(pMechanism)
		checkOutPtr(phPublicKey)
		checkOutPtr(phPrivateKey)

		logParam(hSession)
		logParam(pMechanism)
		logParam(pPublicKeyTemplate)
		logParam(pPrivateKeyTemplate)
		logParam(ulPublicKeyAttributeCount)
		logParam(ulPrivateKeyAttributeCount)
		logParam(phPublicKey)
		logParam(phPrivateKey)


		if (!bP11Initialized)
			_return(CKR_CRYPTOKI_NOT_INITIALIZED);

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);

	if (!pSession)
		_return(CKR_SESSION_HANDLE_INVALID);

	pSession->GenerateKeyPair(pMechanism, pPublicKeyTemplate, ulPublicKeyAttributeCount, pPrivateKeyTemplate, ulPrivateKeyAttributeCount, phPublicKey, phPrivateKey);
	_return(CKR_OK)

	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_DestroyObject(CK_SESSION_HANDLE hSession,CK_OBJECT_HANDLE hObject)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);
	
	logParam(hSession)
	logParam(hObject)


	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);

	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	DF_CALL(pSession->DestroyObject(hObject))
	_return(CKR_OK)

	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_DigestInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism) 
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInPtr(pMechanism)

	logParam(hSession)
	logParam(pMechanism)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession)
		_return(CKR_SESSION_HANDLE_INVALID);

	bool bValid=false;
	EDF_CALL(CheckMechanismParam(pMechanism, bValid),
		ERR_CHECK_MECHANISM_PARAM);
	if (!bValid)
		_return(CKR_MECHANISM_PARAM_INVALID);

	pSession->DigestInit(pMechanism);
	_return(CKR_OK)	
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_Digest(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pDigest, CK_ULONG_PTR pulDigestLen) 
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pData,ulDataLen)
	checkOutArray(pDigest,pulDigestLen)

	logParam(hSession)
	logParamBuf(pData,ulDataLen)
	logParamBuf(pDigest,pulDigestLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	ByteArray Digest(pDigest, *pulDigestLen);
	DF_CALL(pSession->Digest(ByteArray(pData, ulDataLen), Digest))
	*pulDigestLen = (CK_ULONG)Digest.size();

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_DigestFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pDigest, CK_ULONG_PTR pulDigestLen) 
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutArray(pDigest,pulDigestLen)

	logParam(hSession)
	logParamBuf(pDigest,pulDigestLen)
	
	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);

	if (!pSession)
		_return(CKR_SESSION_HANDLE_INVALID);

	ByteArray Digest(pDigest, *pulDigestLen);
	pSession->DigestFinal(Digest);
	*pulDigestLen = (CK_ULONG)Digest.size();

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_DigestUpdate (CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pPart, ulPartLen)

		logParam(hSession)
		logParamBuf(pPart, ulPartLen)

		if (!bP11Initialized)
			_return(CKR_CRYPTOKI_NOT_INITIALIZED);

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession)
		_return(CKR_SESSION_HANDLE_INVALID);

	pSession->DigestUpdate(ByteArray(pPart, ulPartLen));
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_FindObjects(CK_SESSION_HANDLE hSession,CK_OBJECT_HANDLE_PTR phObject,CK_ULONG ulMaxObjectCount,CK_ULONG_PTR pulObjectCount)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutBuffer(phObject,sizeof(CK_OBJECT_HANDLE)*ulMaxObjectCount)

	logParam(hSession)
	logParam(phObject)
	logParam(ulMaxObjectCount)
	logParam(pulObjectCount)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	if (phObject==NULL && ulMaxObjectCount>0) 
		_return(CKR_ARGUMENTS_BAD)
	
	DF_CALL(pSession->FindObjects(phObject,ulMaxObjectCount,pulObjectCount))
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_FindObjectsFinal(CK_SESSION_HANDLE hSession)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	logParam(hSession)

		if (!bP11Initialized)
			_return(CKR_CRYPTOKI_NOT_INITIALIZED);

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession)
		_return(CKR_SESSION_HANDLE_INVALID);

	pSession->FindObjectsFinal();
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}
CK_RV CK_ENTRY C_FindObjectsInit(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInArray(pTemplate,ulCount)

	logParam(hSession)
	logParam(pTemplate)
	logParam(ulCount)

	if (Log.LogParam) {
		for (DWORD i=0;i<ulCount;i++) {
			Log.writePure("Template %i:",i+1);
			Log.writePure("Type: %s (%x)",getAttributeName(pTemplate[i].type),pTemplate[i].type);
			info.logParameter(pTemplate[i].pValue,pTemplate[i].ulValueLen);
		}
	}

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	if (pTemplate==NULL && ulCount>0) 
		_return(CKR_ARGUMENTS_BAD)

	DF_CALL(pSession->FindObjectsInit(pTemplate,ulCount))
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_GetAttributeValue(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount )
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInArray(pTemplate, ulCount)

		logParam(hSession)
		logParam(hObject)
		logParam(pTemplate)
		logParam(ulCount)

		if (!bP11Initialized)
			_return(CKR_CRYPTOKI_NOT_INITIALIZED);

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession)
		_return(CKR_SESSION_HANDLE_INVALID);

	pSession->GetAttributeValue(hObject, pTemplate, ulCount);
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_GetMechanismList(CK_SLOT_ID slotID, CK_MECHANISM_TYPE_PTR pMechanismList, CK_ULONG_PTR pulCount)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutArray(pMechanismList,pulCount)

	logParam(slotID)
	logParam(pMechanismList)
	logParam(pulCount)

	DWORD dwNumMechansms=sizeof(P11mechanisms)/sizeof(CK_MECHANISM_TYPE);
	
	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSlot> pSlot;
	EDF_CALL(CSlot::GetSlotFromID(slotID,pSlot),
		ERR_CANT_GET_SLOT);
	if (!pSlot)
		_return(CKR_SLOT_ID_INVALID)

	if (pMechanismList == NULL)
	{
		*pulCount = dwNumMechansms;
		_return(CKR_OK)
	}

	if (*pulCount>=dwNumMechansms) {
		memcpy_s(pMechanismList, dwNumMechansms*sizeof(CK_MECHANISM_TYPE),P11mechanisms, dwNumMechansms*sizeof(CK_MECHANISM_TYPE));
		_return(CKR_OK)
	}
	else 
		_return(CKR_BUFFER_TOO_SMALL)

	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_GetMechanismInfo(CK_SLOT_ID slotID, CK_MECHANISM_TYPE type, CK_MECHANISM_INFO_PTR pInfo)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	logParam(slotID)
	logParam(type)
	logParam(pInfo)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSlot> pSlot;
	EDF_CALL(CSlot::GetSlotFromID(slotID,pSlot),
		ERR_CANT_GET_SLOT);

	if (!pSlot)
		_return(CKR_SLOT_ID_INVALID)

	switch (type) {
		case CKM_RSA_PKCS:
			pInfo->flags = CKF_HW | CKF_ENCRYPT | CKF_DECRYPT | CKF_SIGN | CKF_VERIFY | CKF_SIGN_RECOVER | CKF_VERIFY_RECOVER;
			pInfo->ulMinKeySize = 1024;
			pInfo->ulMaxKeySize = 1024;
			break;
		case CKM_RSA_X_509:
			pInfo->flags = CKF_HW | CKF_ENCRYPT | CKF_DECRYPT | CKF_SIGN | CKF_VERIFY | CKF_SIGN_RECOVER | CKF_VERIFY_RECOVER;
			pInfo->ulMinKeySize = 1024;
			pInfo->ulMaxKeySize = 1024;
			break;
		case CKM_SHA1_RSA_PKCS:
			pInfo->flags = CKF_HW | CKF_SIGN | CKF_VERIFY;
			pInfo->ulMinKeySize = 1024;
			pInfo->ulMaxKeySize = 1024;
			break;
		case CKM_MD5_RSA_PKCS:
			pInfo->flags = CKF_HW | CKF_SIGN | CKF_VERIFY;
			pInfo->ulMinKeySize = 1024;
			pInfo->ulMaxKeySize = 1024;
			break;
		case CKM_MD5:
			pInfo->flags = CKF_DIGEST;
			pInfo->ulMinKeySize = 0;
			pInfo->ulMaxKeySize = 0;
			break;
		case CKM_SHA_1:
			pInfo->flags = CKF_DIGEST;
			pInfo->ulMinKeySize = 0;
			pInfo->ulMaxKeySize = 0;
			break;
		default:
			_return(CKR_MECHANISM_INVALID)
	}
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_GetSessionInfo(CK_SESSION_HANDLE hSession, CK_SESSION_INFO_PTR pInfo)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutPtr(pInfo)

	logParam(hSession)
	logParam(pInfo)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession)
		_return(CKR_SESSION_HANDLE_INVALID)

	pInfo->slotID = pSession->pSlot->hSlot;

	if (pSession->pSlot->User==CKU_NOBODY)
		{
		if (pSession->flags & CKF_RW_SESSION)
			pInfo->state=CKS_RW_PUBLIC_SESSION;
		else
			pInfo->state=CKS_RO_PUBLIC_SESSION;
		}
	else if (pSession->pSlot->User==CKU_USER)
		{
		if (pSession->flags & CKF_RW_SESSION)
			pInfo->state=CKS_RW_USER_FUNCTIONS;
		else
			pInfo->state=CKS_RO_USER_FUNCTIONS;
		}
	else
		pInfo->state=CKS_RW_SO_FUNCTIONS;

	if (pSession->flags & CKF_RW_SESSION)
		pInfo->flags = CKF_RW_SESSION;
	else
		pInfo->flags = 0;

	pInfo->flags = 	pInfo->flags | CKF_SERIAL_SESSION;

	pInfo->ulDeviceError = 0;

	_return(CKR_OK)		
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_Login(CK_SESSION_HANDLE hSession, CK_USER_TYPE userType, CK_CHAR_PTR pPin, CK_ULONG ulPinLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pPin,ulPinLen)

	logParam(hSession)
	logParam(userType)
	logParamBufHide(pPin,ulPinLen)
	
	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession)
		_return(CKR_SESSION_HANDLE_INVALID)

	if (userType!=CKU_SO && userType!=CKU_USER)
		_return(CKR_USER_TYPE_INVALID)

	DF_CALL(pSession->Login(userType,pPin,ulPinLen))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)
}

CK_RV CK_ENTRY C_Logout(CK_SESSION_HANDLE hSession)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	logParam(hSession)
		 
	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession)
		_return(CKR_SESSION_HANDLE_INVALID)

	if (pSession->pSlot->User==CKU_NOBODY)
		_return(CKR_USER_NOT_LOGGED_IN)

	DF_CALL(pSession->Logout())
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_SetAttributeValue(CK_SESSION_HANDLE hSession,CK_OBJECT_HANDLE hObject,CK_ATTRIBUTE_PTR pTemplate,CK_ULONG ulCount)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	logParam(hSession)
	logParam(hObject)
	logParam(pTemplate)
	logParam(ulCount)
	
	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession)
		_return(CKR_SESSION_HANDLE_INVALID)

	DF_CALL(pSession->SetAttributeValue(hObject,pTemplate,ulCount))
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	

}

CK_RV CK_ENTRY C_Sign(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pSignature, CK_ULONG_PTR pulSignatureLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pData,ulDataLen)
	checkOutArray(pSignature,pulSignatureLen)
	checkInBuffer(pData,ulDataLen)
	checkOutArray(pSignature,pulSignatureLen)

	logParam(hSession)
	logParamBuf(pData,ulDataLen)
	logParamBuf(pSignature,pulSignatureLen)
	
	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	ByteArray Signature(pSignature,*pulSignatureLen);
	DF_CALL(pSession->Sign(ByteArray(pData,ulDataLen),Signature))
		*pulSignatureLen = (CK_ULONG)Signature.size();
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
	}

CK_RV CK_ENTRY C_SignFinal(CK_SESSION_HANDLE hSession,CK_BYTE_PTR pSignature,CK_ULONG_PTR pulSignatureLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutArray(pSignature,pulSignatureLen)

	logParam(hSession)
	logParamBuf(pSignature,pulSignatureLen)
	
	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	bool bSupport=false;
	R_CALL(pSession->pSignMechanism->SignSupportMultipart(bSupport));
	if (!bSupport) 
		_return(CKR_KEY_FUNCTION_NOT_PERMITTED);

	ByteArray Signature(pSignature,*pulSignatureLen);
	DF_CALL(pSession->SignFinal(Signature))
		*pulSignatureLen = (CK_ULONG)Signature.size();

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_SignInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInPtr(pMechanism)

	logParam(hSession)
	logParam(pMechanism)
	logParam(hKey)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)
	
	bool bValid=false;
	EDF_CALL(CheckMechanismParam(pMechanism,bValid),
		ERR_CHECK_MECHANISM_PARAM)

	if (!bValid)
		_return(CKR_MECHANISM_PARAM_INVALID)

	DF_CALL(pSession->SignInit(pMechanism,hKey))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}
CK_RV CK_ENTRY C_SignUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pPart,ulPartLen)

	logParam(hSession)
	logParamBuf(pPart,ulPartLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	bool bSupport=false;
	R_CALL(pSession->pSignMechanism->SignSupportMultipart(bSupport))
	if (!bSupport) _return(CKR_KEY_FUNCTION_NOT_PERMITTED)

	DF_CALL(pSession->SignUpdate(ByteArray(pPart,ulPartLen)))
	
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_SignRecoverInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInPtr(pMechanism)

	logParam(hSession)
	logParam(pMechanism)
	logParam(hKey)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)
	
	bool bValid=false;
	EDF_CALL(CheckMechanismParam(pMechanism,bValid),
		ERR_CHECK_MECHANISM_PARAM)

	if (!bValid)
		_return(CKR_MECHANISM_PARAM_INVALID)

	DF_CALL(pSession->SignRecoverInit(pMechanism,hKey))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_SignRecover(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pSignature, CK_ULONG_PTR pulSignatureLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pData,ulDataLen)
	checkOutArray(pSignature,pulSignatureLen)

	logParam(hSession)
	logParamBuf(pData,ulDataLen)
	logParamBuf(pSignature,pulSignatureLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	ByteArray Signature(pSignature,*pulSignatureLen);
	DF_CALL(pSession->SignRecover(ByteArray(pData,ulDataLen),Signature))
		*pulSignatureLen = (CK_ULONG)Signature.size();
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_VerifyRecoverInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInPtr(pMechanism)

	logParam(hSession)
	logParam(pMechanism)
	logParam(hKey)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)
	
	bool bValid=false;
	EDF_CALL(CheckMechanismParam(pMechanism,bValid),
		ERR_CHECK_MECHANISM_PARAM)

	if (!bValid)
		_return(CKR_MECHANISM_PARAM_INVALID)

	DF_CALL(pSession->VerifyRecoverInit(pMechanism,hKey))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_VerifyRecover(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pSignature, CK_ULONG ulSignatureLen, CK_BYTE_PTR pData, CK_ULONG_PTR pulDataLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutArray(pData,pulDataLen)
	checkInBuffer(pSignature,ulSignatureLen)

	logParam(hSession)
	logParamBuf(pSignature,ulSignatureLen)
	logParamBuf(pData,pulDataLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	ByteArray Data(pData,*pulDataLen);
	DF_CALL(pSession->VerifyRecover(ByteArray(pSignature,ulSignatureLen),Data))
		*pulDataLen = (CK_ULONG)Data.size();
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_VerifyInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInPtr(pMechanism)

	logParam(hSession)
	logParam(pMechanism)
	logParam(hKey)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)
	
	bool bValid=false;
	if (CheckMechanismParam(pMechanism,bValid)) {
		_return(CKR_GENERAL_ERROR)
	}

	if (!bValid)
		_return(CKR_MECHANISM_PARAM_INVALID)

	DF_CALL(pSession->VerifyInit(pMechanism,hKey))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_Verify(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pSignature, CK_ULONG ulSignatureLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pData,ulDataLen)
	checkInBuffer(pSignature,ulSignatureLen)

	logParam(hSession)
	logParamBuf(pData,ulDataLen)
	logParamBuf(pSignature,ulSignatureLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	DF_CALL(pSession->Verify(ByteArray(pData,ulDataLen),ByteArray(pSignature,ulSignatureLen)))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_VerifyUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pData,ulDataLen)

	logParam(hSession)
	logParamBuf(pData,ulDataLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	bool bSupport=false;
	R_CALL(pSession->pVerifyMechanism->VerifySupportMultipart(bSupport))
	if (!bSupport) _return(CKR_KEY_FUNCTION_NOT_PERMITTED)

	DF_CALL(pSession->VerifyUpdate(ByteArray(pData,ulDataLen)))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_VerifyFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pSignature, CK_ULONG ulSignatureLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pSignature,ulSignatureLen)

	logParam(hSession)
	logParamBuf(pSignature,ulSignatureLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	bool bSupport=false;
	R_CALL(pSession->pVerifyMechanism->VerifySupportMultipart(bSupport))
	if (!bSupport) _return(CKR_KEY_FUNCTION_NOT_PERMITTED)

	DF_CALL(pSession->VerifyFinal(ByteArray(pSignature,ulSignatureLen)))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_Encrypt(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pEncryptedData, CK_ULONG_PTR pulEncryptedDataLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pData,ulDataLen)
	checkOutArray(pEncryptedData,pulEncryptedDataLen)

	logParam(hSession)
	logParamBuf(pData,ulDataLen)
	logParamBuf(pEncryptedData,pulEncryptedDataLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	ByteArray EncryptedData(pEncryptedData, *pulEncryptedDataLen);
	DF_CALL(pSession->Encrypt(ByteArray(pData, ulDataLen), EncryptedData))
		*pulEncryptedDataLen = (CK_ULONG)EncryptedData.size();

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_EncryptFinal(CK_SESSION_HANDLE hSession,CK_BYTE_PTR pEncryptedData,CK_ULONG_PTR pulEncryptedDataLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutArray(pEncryptedData,pulEncryptedDataLen)

	logParam(hSession)
	logParamBuf(pEncryptedData,pulEncryptedDataLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	bool bSupport=false;
	R_CALL(pSession->pEncryptMechanism->EncryptSupportMultipart(bSupport))
	if (!bSupport) _return(CKR_KEY_FUNCTION_NOT_PERMITTED)

	ByteArray EncryptedData(pEncryptedData, *pulEncryptedDataLen);
	DF_CALL(pSession->EncryptFinal(EncryptedData))
		*pulEncryptedDataLen = (CK_ULONG)EncryptedData.size();

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_EncryptInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInPtr(pMechanism)

	logParam(hSession)
	logParam(pMechanism)
	logParam(hKey)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)
	
	bool bValid=false;
	if (CheckMechanismParam(pMechanism,bValid)) {
		_return(CKR_GENERAL_ERROR)
	}

	if (!bValid)
		_return(CKR_MECHANISM_PARAM_INVALID)

	DF_CALL(pSession->EncryptInit(pMechanism,hKey))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_EncryptUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen, CK_BYTE_PTR pEncryptedPart, CK_ULONG_PTR pulEncryptedPartLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pPart,ulPartLen)
	checkOutArray(pEncryptedPart,pulEncryptedPartLen)

	logParam(hSession)
	logParamBuf(pPart,ulPartLen)
	logParamBuf(pEncryptedPart,pulEncryptedPartLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	bool bSupport=false;
	R_CALL(pSession->pEncryptMechanism->EncryptSupportMultipart(bSupport))
	if (!bSupport) _return(CKR_KEY_FUNCTION_NOT_PERMITTED)

	ByteArray EncryptedPart(pEncryptedPart, *pulEncryptedPartLen);
	DF_CALL(pSession->EncryptUpdate(ByteArray(pPart,ulPartLen),EncryptedPart))
		*pulEncryptedPartLen = (CK_ULONG)EncryptedPart.size();

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_Decrypt(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedData, CK_ULONG ulEncryptedDataLen, CK_BYTE_PTR pData, CK_ULONG_PTR pulDataLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pEncryptedData,ulEncryptedDataLen)
	checkOutArray(pData,pulDataLen)

	logParam(hSession)
	logParamBuf(pEncryptedData,ulEncryptedDataLen)
	logParamBuf(pData,pulDataLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	ByteArray Data(pData, *pulDataLen);
	DF_CALL(pSession->Decrypt(ByteArray(pEncryptedData, ulEncryptedDataLen), Data))
		*pulDataLen = (CK_ULONG)Data.size();
	
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_DecryptFinal(CK_SESSION_HANDLE hSession,CK_BYTE_PTR pData,CK_ULONG_PTR pulDataLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutArray(pData,pulDataLen)

	logParam(hSession)
	logParamBuf(pData,pulDataLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	bool bSupport=false;
	R_CALL(pSession->pDecryptMechanism->DecryptSupportMultipart(bSupport))
	if (!bSupport) _return(CKR_KEY_FUNCTION_NOT_PERMITTED)

	ByteArray Data(pData, *pulDataLen);
	DF_CALL(pSession->DecryptFinal(Data))
		*pulDataLen = (CK_ULONG)Data.size();

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_DecryptInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInPtr(pMechanism)

	logParam(hSession)
	logParam(pMechanism)
	logParam(hKey)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)
	
	bool bValid=false;
	if (CheckMechanismParam(pMechanism,bValid)) {
		_return(CKR_GENERAL_ERROR)
	}

	if (!bValid)
		_return(CKR_MECHANISM_PARAM_INVALID)

	DF_CALL(pSession->DecryptInit(pMechanism,hKey))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_DecryptUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedPart, CK_ULONG ulEncryptedPartLen, CK_BYTE_PTR pPart, CK_ULONG_PTR pulPartLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pEncryptedPart,ulEncryptedPartLen)
	checkOutArray(pPart,pulPartLen)

	logParam(hSession)
	logParamBuf(pEncryptedPart,ulEncryptedPartLen)
	logParamBuf(pPart,pulPartLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	bool bSupport=false;
	R_CALL(pSession->pDecryptMechanism->DecryptSupportMultipart(bSupport))
	if (!bSupport) _return(CKR_KEY_FUNCTION_NOT_PERMITTED)

	ByteArray Part(pPart, *pulPartLen);
	DF_CALL(pSession->DecryptUpdate(ByteArray(pEncryptedPart, ulEncryptedPartLen), Part))
		*pulPartLen = (CK_ULONG)Part.size();

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_WaitForSlotEvent(CK_FLAGS flags, CK_SLOT_ID_PTR pSlot, CK_VOID_PTR pReserved)
{
	init_main_func

	checkOutPtr(pSlot)

	logParam(flags)
	logParam(pSlot)
	logParam(pReserved)

	if (pReserved != NULL)
		_return(CKR_ARGUMENTS_BAD)

	if (flags != 0 && flags != CKF_DONT_BLOCK)
		_return(CKR_ARGUMENTS_BAD)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	if (flags & CKF_DONT_BLOCK) {
		CSyncroLocker lock(p11Mutex);
		//CSyncroLocker lock(p11EventMutex);
		SlotMap::iterator it=CSlot::g_mSlots.begin();
		while (it!=CSlot::g_mSlots.end()) {
			if (it->second->lastEvent!=SE_NoEvent) {
				*pSlot=it->second->hSlot;
				it->second->lastEvent=SE_NoEvent;
				_return(CKR_OK)
			}
			it++;
		}
		_return(CKR_NO_EVENT)
	}

	while (1) {
		p11slotEvent.Wait();
		//CSyncroLocker lock(p11EventMutex);
		if (!bP11Initialized) {
			*pSlot=0;
			_return(CKR_CRYPTOKI_NOT_INITIALIZED)
		}
		SlotMap::iterator it=CSlot::g_mSlots.begin();
		while (it!=CSlot::g_mSlots.end()) {
			if (it->second->lastEvent!=SE_NoEvent) {
				*pSlot=it->second->hSlot;
				it->second->lastEvent=SE_NoEvent;
				_return(CKR_OK)
			}
			it++;
		}
	}

	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_SeedRandom(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pSeed, CK_ULONG ulSeedLen)
{
	init_main_func

	logParam(hSession)
	logParamBuf(pSeed,ulSeedLen)

	_return(CKR_RANDOM_SEED_NOT_SUPPORTED)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_GenerateRandom(CK_SESSION_HANDLE hSession, CK_BYTE_PTR RandomData, CK_ULONG ulRandomLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutBuffer(RandomData,ulRandomLen)

	logParam(hSession)
	logParamBuf(RandomData,ulRandomLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)
	
	DF_CALL(pSession->GenerateRandom(ByteArray(RandomData,ulRandomLen)))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_InitPIN(CK_SESSION_HANDLE hSession,CK_CHAR_PTR pPin,CK_ULONG ulPinLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pPin,ulPinLen);

	logParam(hSession)
	logParamBufHide(pPin,ulPinLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)
	
	DF_CALL(pSession->InitPIN(ByteArray(pPin,ulPinLen)))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_SetPIN(CK_SESSION_HANDLE hSession,CK_CHAR_PTR pOldPin,CK_ULONG ulOldLen,CK_CHAR_PTR pNewPin,CK_ULONG ulNewLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pOldPin,ulOldLen);
	checkInBuffer(pNewPin,ulNewLen);

	logParam(hSession)
	logParamBufHide(pOldPin,ulOldLen)
	logParamBufHide(pNewPin,ulNewLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)
	
	DF_CALL(pSession->SetPIN(ByteArray(pOldPin,ulOldLen),ByteArray(pNewPin,ulNewLen)))

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_GetObjectSize(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject,CK_ULONG_PTR pulSize) 
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutPtr(pulSize);

	logParam(hSession)
	logParam(hObject)
	logParam(pulSize)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	DF_CALL(pSession->GetObjectSize(hObject,pulSize));
	
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_GetOperationState(CK_SESSION_HANDLE hSession,CK_BYTE_PTR pOperationState,CK_ULONG_PTR pulOperationStateLen)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkOutArray(pOperationState,pulOperationStateLen)

	logParam(hSession)
	logParamBuf(pOperationState,pulOperationStateLen)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	ByteArray OperationState(pOperationState,*pulOperationStateLen);
	DF_CALL(pSession->GetOperationState(OperationState))
		*pulOperationStateLen = (CK_ULONG)OperationState.size();

	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

CK_RV CK_ENTRY C_SetOperationState(CK_SESSION_HANDLE hSession,CK_BYTE_PTR pOperationState,CK_ULONG ulOperationStateLen,CK_OBJECT_HANDLE hEncryptionKey,CK_OBJECT_HANDLE hAuthenticationKey)
{
	init_main_func
	CSyncroLocker lock(p11Mutex);

	checkInBuffer(pOperationState,ulOperationStateLen)

	logParam(hSession)
	logParamBuf(pOperationState,ulOperationStateLen)
	logParam(hEncryptionKey)
	logParam(hAuthenticationKey)

	if (!bP11Initialized)
		_return(CKR_CRYPTOKI_NOT_INITIALIZED)

	std::shared_ptr<CSession> pSession;
	EDF_CALL(CSession::GetSessionFromID(hSession,pSession),
		ERR_CANT_GET_SESSION);
	if (!pSession) _return(CKR_SESSION_HANDLE_INVALID)

	if(hEncryptionKey!=CK_INVALID_HANDLE)
		_return(CKR_KEY_NOT_NEEDED)
	if(hAuthenticationKey!=CK_INVALID_HANDLE)
		_return(CKR_KEY_NOT_NEEDED)

	DF_CALL(pSession->SetOperationState(ByteArray(pOperationState,ulOperationStateLen)))
	
	_return(CKR_OK)
	exit_main_func
	_return(CKR_GENERAL_ERROR)	
}

#define unsupported \
{ \
	init_main_func \
	Log.write("%s","Funzione non supportata!!"); \
	_return(CKR_FUNCTION_NOT_SUPPORTED) \
	exit_main_func \
	_return(CKR_FUNCTION_NOT_SUPPORTED) \
}

CK_RV CK_ENTRY C_InitToken(CK_SLOT_ID slotID,CK_CHAR_PTR pPin,CK_ULONG ulPinLen,CK_UTF8CHAR_PTR pLabel) unsupported
CK_RV CK_ENTRY C_CopyObject(CK_SESSION_HANDLE hSession,CK_OBJECT_HANDLE hObject,CK_ATTRIBUTE_PTR pTemplate,CK_ULONG ulCount,CK_OBJECT_HANDLE_PTR phNewObject) unsupported
CK_RV CK_ENTRY C_DigestKey(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hKey) unsupported
CK_RV CK_ENTRY C_DigestEncryptUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen, CK_BYTE_PTR pEncryptedPart, CK_ULONG_PTR pulEncryptedPartLen) unsupported
CK_RV CK_ENTRY C_DecryptDigestUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedPart, CK_ULONG ulEncryptedPartLen, CK_BYTE_PTR pPart, CK_ULONG_PTR pulPartLen) unsupported
CK_RV CK_ENTRY C_SignEncryptUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen, CK_BYTE_PTR pEncryptedPart, CK_ULONG_PTR pulEncryptedPartLen) unsupported
CK_RV CK_ENTRY C_DecryptVerifyUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedPart, CK_ULONG ulEncryptedPartLen, CK_BYTE_PTR pPart, CK_ULONG_PTR pulPartLen) unsupported
CK_RV CK_ENTRY C_WrapKey(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hWrappingKey, CK_OBJECT_HANDLE hKey, CK_BYTE_PTR pWrappedKey, CK_ULONG_PTR pulWrappedKeyLen) unsupported
CK_RV CK_ENTRY C_UnwrapKey(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hUnwrappingKey, CK_BYTE_PTR pWrappedKey, CK_ULONG ulWrappedKeyLen, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulAttributeCount, CK_OBJECT_HANDLE_PTR phKey) unsupported
CK_RV CK_ENTRY C_DeriveKey(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hBaseKey, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulAttributeCount, CK_OBJECT_HANDLE_PTR phKey) unsupported
CK_RV CK_ENTRY C_GetFunctionStatus(CK_SESSION_HANDLE hSession) unsupported
CK_RV CK_ENTRY C_CancelFunction(CK_SESSION_HANDLE hSession) unsupported


char *getAttributeName(DWORD dwId) {
	switch (dwId) {
		case 0x00000000: return("CKA_CLASS");
		case 0x00000001: return("CKA_TOKEN");
		case 0x00000002: return("CKA_PRIVATE");
		case 0x00000003: return("CKA_LABEL");
		case 0x00000010: return("CKA_APPLICATION");
		case 0x00000011: return("CKA_VALUE");
		case 0x00000012: return("CKA_OBJECT_ID");
		case 0x00000080: return("CKA_CERTIFICATE_TYPE");
		case 0x00000081: return("CKA_ISSUER");
		case 0x00000082: return("CKA_SERIAL_NUMBER");
		case 0x00000083: return("CKA_AC_ISSUER");
		case 0x00000084: return("CKA_OWNER");
		case 0x00000085: return("CKA_ATTR_TYPES");
		case 0x00000086: return("CKA_TRUSTED");
		case 0x00000100: return("CKA_KEY_TYPE");
		case 0x00000101: return("CKA_SUBJECT");
		case 0x00000102: return("CKA_ID");
		case 0x00000103: return("CKA_SENSITIVE");
		case 0x00000104: return("CKA_ENCRYPT");
		case 0x00000105: return("CKA_DECRYPT");
		case 0x00000106: return("CKA_WRAP");
		case 0x00000107: return("CKA_UNWRAP");
		case 0x00000108: return("CKA_SIGN");
		case 0x00000109: return("CKA_SIGN_RECOVER");
		case 0x0000010A: return("CKA_VERIFY");
		case 0x0000010B: return("CKA_VERIFY_RECOVER");
		case 0x0000010C: return("CKA_DERIVE");
		case 0x00000110: return("CKA_START_DATE");
		case 0x00000111: return("CKA_END_DATE");
		case 0x00000120: return("CKA_MODULUS");
		case 0x00000121: return("CKA_MODULUS_BITS");
		case 0x00000122: return("CKA_PUBLIC_EXPONENT");
		case 0x00000123: return("CKA_PRIVATE_EXPONENT");
		case 0x00000124: return("CKA_PRIME_1");
		case 0x00000125: return("CKA_PRIME_2");
		case 0x00000126: return("CKA_EXPONENT_1");
		case 0x00000127: return("CKA_EXPONENT_2");
		case 0x00000128: return("CKA_COEFFICIENT");
		case 0x00000130: return("CKA_PRIME");
		case 0x00000131: return("CKA_SUBPRIME");
		case 0x00000132: return("CKA_BASE");
		case 0x00000133: return("CKA_PRIME_BITS");
		case 0x00000134: return("CKA_SUB_PRIME_BITS");
		case 0x00000160: return("CKA_VALUE_BITS");
		case 0x00000161: return("CKA_VALUE_LEN");
		case 0x00000162: return("CKA_EXTRACTABLE");
		case 0x00000163: return("CKA_LOCAL");
		case 0x00000164: return("CKA_NEVER_EXTRACTABLE");
		case 0x00000165: return("CKA_ALWAYS_SENSITIVE");
		case 0x00000166: return("CKA_KEY_GEN_MECHANISM");
		case 0x00000170: return("CKA_MODIFIABLE");
		case 0x00000180: return("CKA_EC_PARAMS");
		case 0x00000181: return("CKA_EC_POINT");
		case 0x00000200: return("CKA_SECONDARY_AUTH");
		case 0x00000201: return("CKA_AUTH_PIN_FLAGS");
		case 0x00000300: return("CKA_HW_FEATURE_TYPE");
		case 0x00000301: return("CKA_RESET_ON_INIT");
		case 0x00000302: return("CKA_HAS_RESET");
		case 0x80000000: return("CKA_VENDOR_DEFINED");
		case 0x80000001: return("CKA_SM");
		case 0x80000002: return("CKA_SM_PUB");
	}
	return("UNKNOWN");
}
