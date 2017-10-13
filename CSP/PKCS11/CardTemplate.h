#pragma once

#include "../PCSC/token.h"
#include "session.h"
#include <memory>


namespace p11 {

typedef RESULT (*templateInitLibraryFunc)(class CCardTemplate &Template,void *templateData);
typedef RESULT (*templateInitCardFunc)(void *&pTemplateData,CSlot &pSlot);
typedef void (*templateFinalCardFunc)(void *pTemplateData);
typedef RESULT (*templateInitSessionFunc)(void *pTemplateData);
typedef RESULT (*templateFinalSessionFunc)(void *pTemplateData);
typedef RESULT (*templateMatchCardFunc)(bool &bMatched,CSlot &pSlot);
typedef RESULT (*templateGetSerialFunc)(CSlot &pSlot,ByteDynArray &baSerial);
typedef RESULT (*templateGetModelFunc)(CSlot &pSlot,String &szModel);
typedef RESULT (*templateGetTokenFlagsFunc)(CSlot &pSlot,DWORD &dwFlags);
typedef RESULT (*templateLoginFunc)(void *pTemplateData,CK_USER_TYPE userType, ByteArray &Pin);
typedef RESULT (*templateLogoutFunc)(void *pTemplateData,CK_USER_TYPE userType);
typedef RESULT (*templateReadObjectAttributesFunc)(void *pCardTemplateData,CP11Object *pObject);
typedef RESULT (*templateSignFunc)(void *pCardTemplateData,CP11PrivateKey *pPrivKey,ByteArray &baSignBuffer,ByteDynArray &baSignature,CK_MECHANISM_TYPE mechanism,bool bSilent);
typedef RESULT (*templateSignRecoverFunc)(void *pCardTemplateData,CP11PrivateKey *pPrivKey,ByteArray &baSignBuffer,ByteDynArray &baSignature,CK_MECHANISM_TYPE mechanism,bool bSilent);
typedef RESULT (*templateDecryptFunc)(void *pCardTemplateData,CP11PrivateKey *pPrivKey,ByteArray &baEncryptedData,ByteDynArray &baData,CK_MECHANISM_TYPE mechanism,bool bSilent);
typedef RESULT (*templateGenerateRandomFunc)(void *pCardTemplateData,ByteArray &baRandomData);
typedef RESULT (*templateInitPINFunc)(void *pCardTemplateData,ByteArray &baPin);
typedef RESULT (*templateSetPINFunc)(void *pCardTemplateData,ByteArray &baOldPin,ByteArray &baNewPin,CK_USER_TYPE User);
typedef RESULT (*templateGetObjectSizeFunc)(void *pCardTemplateData,CP11Object *pObject,CK_ULONG_PTR pulSize);
typedef RESULT (*templateSetKeyPINFunc)(void *pTemplateData,CP11Object *pObject,ByteArray &Pin);
typedef RESULT (*templateSetAttributeFunc)(void *pTemplateData,CP11Object *pObject,CK_ATTRIBUTE_PTR pTemplate,CK_ULONG ulCount);
typedef RESULT (*templateCreateObjectFunc)(void *pTemplateData,CK_ATTRIBUTE_PTR pTemplate,CK_ULONG ulCount,std::shared_ptr<CP11Object>&pObject);
typedef RESULT (*templateDestroyObjectFunc)(void *pTemplateData,CP11Object &Object);
typedef RESULT (*templateGenerateKeyFunc)(void *pCardTemplateData,CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, std::shared_ptr<CP11Object>&pObject);
typedef RESULT (*templateGenerateKeyPairFunc)(void *pCardTemplateData,CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount, CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount, std::shared_ptr<CP11Object>&pPublicKey, std::shared_ptr<CP11Object>&pPrivateKey);

class TemplateFuncList {
public:
	templateInitLibraryFunc				templateInitLibrary;
	templateInitCardFunc				templateInitCard;
	templateFinalCardFunc				templateFinalCard;
	templateInitSessionFunc				templateInitSession;
	templateFinalSessionFunc			templateFinalSession;
	templateMatchCardFunc				templateMatchCard;
	templateGetSerialFunc				templateGetSerial;
	templateGetModelFunc				templateGetModel;
	templateLoginFunc					templateLogin;
	templateLogoutFunc					templateLogout;
	templateReadObjectAttributesFunc	templateReadObjectAttributes;
	templateSignFunc					templateSign;
	templateSignRecoverFunc				templateSignRecover;
	templateDecryptFunc					templateDecrypt;
	templateGenerateRandomFunc			templateGenerateRandom;
	templateInitPINFunc					templateInitPIN;
	templateSetPINFunc					templateSetPIN;
	templateGetObjectSizeFunc			templateGetObjectSize;
	templateSetKeyPINFunc				templateSetKeyPIN;
	templateSetAttributeFunc			templateSetAttribute;
	templateCreateObjectFunc			templateCreateObject;
	templateDestroyObjectFunc			templateDestroyObject;
	templateGetTokenFlagsFunc			templateGetTokenFlags;
	templateGenerateKeyFunc				templateGenerateKey;
	templateGenerateKeyPairFunc			templateGenerateKeyPair;
};

typedef std::vector<std::shared_ptr<CCardTemplate>> TemplateVector;
typedef RESULT (*templateFuncListFunc)(TemplateFuncList *);

class CCardTemplate
{
public:
	CCardTemplate(void);
	~CCardTemplate(void);

	static TemplateVector g_mCardTemplates;

	static RESULT AddTemplate(std::shared_ptr<CCardTemplate> pTemplate);

	static RESULT InitTemplateList();
	static RESULT DeleteTemplateList();

	static RESULT GetTemplate(CSlot &pSlot,std::shared_ptr<CCardTemplate>&pTemplate);

	RESULT InitLibrary(const char *szPath,void *templateData);

	HMODULE hLibrary;
	TemplateFuncList FunctionList;

	String szName;
	String szManifacturer;
};

};