#pragma once
#include "CardTemplate.h"
#include "Slot.h"

using namespace p11;

void CIEtemplateInitLibrary(class CCardTemplate &Template, void *templateData);
void CIEtemplateInitCard(void *&pTemplateData, CSlot &pSlot);
void CIEtemplateFinalCard(void *pTemplateData);
void CIEtemplateInitSession(void *pTemplateData);
void CIEtemplateFinalSession(void *pTemplateData);
bool CIEtemplateMatchCard(CSlot &pSlot);
ByteDynArray  CIEtemplateGetSerial(CSlot &pSlot);
void CIEtemplateGetModel(CSlot &pSlot, std::string &szModel);
void CIEtemplateGetTokenFlags(CSlot &pSlot, DWORD &dwFlags);
void CIEtemplateLogin(void *pTemplateData, CK_USER_TYPE userType, ByteArray &Pin);
void CIEtemplateLogout(void *pTemplateData, CK_USER_TYPE userType);
void CIEtemplateReadObjectAttributes(void *pCardTemplateData, CP11Object *pObject);
void CIEtemplateSign(void *pCardTemplateData, CP11PrivateKey *pPrivKey, ByteArray &baSignBuffer, ByteDynArray &baSignature, CK_MECHANISM_TYPE mechanism, bool bSilent);
void CIEtemplateSignRecover(void *pCardTemplateData, CP11PrivateKey *pPrivKey, ByteArray &baSignBuffer, ByteDynArray &baSignature, CK_MECHANISM_TYPE mechanism, bool bSilent);
void CIEtemplateDecrypt(void *pCardTemplateData, CP11PrivateKey *pPrivKey, ByteArray &baEncryptedData, ByteDynArray &baData, CK_MECHANISM_TYPE mechanism, bool bSilent);
void CIEtemplateGenerateRandom(void *pCardTemplateData, ByteArray &baRandomData);
void CIEtemplateInitPIN(void *pCardTemplateData, ByteArray &baPin);
void CIEtemplateSetPIN(void *pCardTemplateData, ByteArray &baOldPin, ByteArray &baNewPin, CK_USER_TYPE User);
CK_ULONG CIEtemplateGetObjectSize(void *pCardTemplateData, CP11Object *pObject);
void CIEtemplateSetKeyPIN(void *pTemplateData, CP11Object *pObject, ByteArray &Pin);
void CIEtemplateSetAttribute(void *pTemplateData, CP11Object *pObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
std::shared_ptr<CP11Object> CIEtemplateCreateObject(void *pTemplateData, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
void CIEtemplateDestroyObject(void *pTemplateData, CP11Object &Object);
std::shared_ptr<CP11Object> CIEtemplateGenerateKey(void *pCardTemplateData, CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
void CIEtemplateGenerateKeyPair(void *pCardTemplateData, CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount, CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount, std::shared_ptr<CP11Object>&pPublicKey, std::shared_ptr<CP11Object>&pPrivateKey);
