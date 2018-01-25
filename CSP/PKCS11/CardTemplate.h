#pragma once

#include "../PCSC/token.h"
#include "session.h"
#include <memory>


namespace p11 {

typedef void (*templateInitLibraryFunc)(class CCardTemplate &Template,void *templateData);
typedef void(*templateInitCardFunc)(void *&pTemplateData,CSlot &pSlot);
typedef void (*templateFinalCardFunc)(void *pTemplateData);
typedef void (*templateInitSessionFunc)(void *pTemplateData);
typedef void (*templateFinalSessionFunc)(void *pTemplateData);
typedef bool (*templateMatchCardFunc)(CSlot &pSlot);
typedef ByteDynArray(*templateGetSerialFunc)(CSlot &pSlot);
typedef void(*templateGetModelFunc)(CSlot &pSlot, std::string &szModel);
typedef void (*templateGetTokenFlagsFunc)(CSlot &pSlot, CK_FLAGS &dwFlags);
typedef void (*templateLoginFunc)(void *pTemplateData,CK_USER_TYPE userType, ByteArray &Pin);
typedef void (*templateLogoutFunc)(void *pTemplateData,CK_USER_TYPE userType);
typedef void (*templateReadObjectAttributesFunc)(void *pCardTemplateData,CP11Object *pObject);
typedef void (*templateSignFunc)(void *pCardTemplateData,CP11PrivateKey *pPrivKey,ByteArray &baSignBuffer,ByteDynArray &baSignature,CK_MECHANISM_TYPE mechanism,bool bSilent);
typedef void (*templateSignRecoverFunc)(void *pCardTemplateData,CP11PrivateKey *pPrivKey,ByteArray &baSignBuffer,ByteDynArray &baSignature,CK_MECHANISM_TYPE mechanism,bool bSilent);
typedef void (*templateDecryptFunc)(void *pCardTemplateData,CP11PrivateKey *pPrivKey,ByteArray &baEncryptedData,ByteDynArray &baData,CK_MECHANISM_TYPE mechanism,bool bSilent);
typedef void (*templateGenerateRandomFunc)(void *pCardTemplateData,ByteArray &baRandomData);
typedef void (*templateInitPINFunc)(void *pCardTemplateData,ByteArray &baPin);
typedef void (*templateSetPINFunc)(void *pCardTemplateData,ByteArray &baOldPin,ByteArray &baNewPin,CK_USER_TYPE User);
typedef CK_ULONG (*templateGetObjectSizeFunc)(void *pCardTemplateData,CP11Object *pObject);
typedef void (*templateSetKeyPINFunc)(void *pTemplateData,CP11Object *pObject,ByteArray &Pin);
typedef void (*templateSetAttributeFunc)(void *pTemplateData,CP11Object *pObject,CK_ATTRIBUTE_PTR pTemplate,CK_ULONG ulCount);
typedef std::shared_ptr<CP11Object>(*templateCreateObjectFunc)(void *pTemplateData,CK_ATTRIBUTE_PTR pTemplate,CK_ULONG ulCount);
typedef void (*templateDestroyObjectFunc)(void *pTemplateData,CP11Object &Object);
typedef std::shared_ptr<CP11Object>(*templateGenerateKeyFunc)(void *pCardTemplateData,CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
typedef void (*templateGenerateKeyPairFunc)(void *pCardTemplateData,CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount, CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount, std::shared_ptr<CP11Object>&pPublicKey, std::shared_ptr<CP11Object>&pPrivateKey);

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
typedef void (*templateFuncListFunc)(TemplateFuncList *);

class CCardTemplate
{
public:
	CCardTemplate(void);
	~CCardTemplate(void);

	static TemplateVector g_mCardTemplates;

	static void AddTemplate(std::shared_ptr<CCardTemplate> pTemplate);

	static void InitTemplateList();
	static void DeleteTemplateList();

	static std::shared_ptr<CCardTemplate> GetTemplate(CSlot &pSlot);

	void InitLibrary(const char *szPath,void *templateData);

	HMODULE hLibrary;
	TemplateFuncList FunctionList;

	std::string szName;
	std::string szManifacturer;
};

};