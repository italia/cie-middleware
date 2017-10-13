#pragma once
#include "CardTemplate.h"
#include "Slot.h"

using namespace p11;

RESULT CIEtemplateInitLibrary(class CCardTemplate &Template, void *templateData);
RESULT CIEtemplateInitCard(void *&pTemplateData, CSlot &pSlot);
void CIEtemplateFinalCard(void *pTemplateData);
RESULT CIEtemplateInitSession(void *pTemplateData);
RESULT CIEtemplateFinalSession(void *pTemplateData);
RESULT CIEtemplateMatchCard(bool &bMatched, CSlot &pSlot);
RESULT CIEtemplateGetSerial(CSlot &pSlot, ByteDynArray &baSerial);
RESULT CIEtemplateGetModel(CSlot &pSlot, String &szModel);
RESULT CIEtemplateGetTokenFlags(CSlot &pSlot, DWORD &dwFlags);
RESULT CIEtemplateLogin(void *pTemplateData, CK_USER_TYPE userType, ByteArray &Pin);
RESULT CIEtemplateLogout(void *pTemplateData, CK_USER_TYPE userType);
RESULT CIEtemplateReadObjectAttributes(void *pCardTemplateData, CP11Object *pObject);
RESULT CIEtemplateSign(void *pCardTemplateData, CP11PrivateKey *pPrivKey, ByteArray &baSignBuffer, ByteDynArray &baSignature, CK_MECHANISM_TYPE mechanism, bool bSilent);
RESULT CIEtemplateSignRecover(void *pCardTemplateData, CP11PrivateKey *pPrivKey, ByteArray &baSignBuffer, ByteDynArray &baSignature, CK_MECHANISM_TYPE mechanism, bool bSilent);
RESULT CIEtemplateDecrypt(void *pCardTemplateData, CP11PrivateKey *pPrivKey, ByteArray &baEncryptedData, ByteDynArray &baData, CK_MECHANISM_TYPE mechanism, bool bSilent);
RESULT CIEtemplateGenerateRandom(void *pCardTemplateData, ByteArray &baRandomData);
RESULT CIEtemplateInitPIN(void *pCardTemplateData, ByteArray &baPin);
RESULT CIEtemplateSetPIN(void *pCardTemplateData, ByteArray &baOldPin, ByteArray &baNewPin, CK_USER_TYPE User);
RESULT CIEtemplateGetObjectSize(void *pCardTemplateData, CP11Object *pObject, CK_ULONG_PTR pulSize);
RESULT CIEtemplateUnblockSecAuthPIN(void *pTemplateData, CP11Object *pObject, ByteArray &Puk, ByteArray &newPin);
RESULT CIEtemplateSetKeyPIN(void *pTemplateData, CP11Object *pObject, ByteArray &Pin);
RESULT CIEtemplateSetAttribute(void *pTemplateData, CP11Object *pObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
RESULT CIEtemplateCreateObject(void *pTemplateData, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, std::shared_ptr<CP11Object>&pObject);
RESULT CIEtemplateDestroyObject(void *pTemplateData, CP11Object &Object);
RESULT CIEtemplateGenerateKey(void *pCardTemplateData, CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, std::shared_ptr<CP11Object>&pObject);
RESULT CIEtemplateGenerateKeyPair(void *pCardTemplateData, CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount, CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount, std::shared_ptr<CP11Object>&pPublicKey, std::shared_ptr<CP11Object>&pPrivateKey);
