#include "..\StdAfx.h"
#include "CIEP11Template.h"
#include "../CSP/IAS.h"
#include "../PCSC/CardLocker.h"
#include "../crypto/ASNParser.h"
#include <stdio.h>
#include "../crypto/AES.h"
#include "../PCSC/PCSC.h"

int TokenTransmitCallback(CSlot *data, BYTE *apdu, DWORD apduSize, BYTE *resp, DWORD *respSize) {
	if (apduSize == 2) {
		WORD code = *(WORD*)apdu;
		if (code == 0xfffd) {
			*respSize = sizeof(data->hCard)+2;
			memcpy(resp, &data->hCard, sizeof(data->hCard));
			resp[sizeof(data->hCard)] = 0;
			resp[sizeof(data->hCard) + 1] = 0;

			return SCARD_S_SUCCESS;
		}
		else if (code == 0xfffe) {
			DWORD protocol = 0;
			ODS("UNPOWER CARD");
			auto sw = SCardReconnect(data->hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_UNPOWER_CARD, &protocol);
			if (sw == SCARD_S_SUCCESS)
				SCardBeginTransaction(data->hCard);
			return sw;
		}
		else if (code == 0xffff) {
			DWORD protocol = 0;
			auto sw = SCardReconnect(data->hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_RESET_CARD, &protocol);
			if (sw == SCARD_S_SUCCESS)
				SCardBeginTransaction(data->hCard);
			ODS("RESET CARD");
			return sw;
		}
	}
	//ODS(String().printf("APDU: %s\n", dumpHexData(ByteArray(apdu, apduSize), String()).lock()).lock());
	auto sw = SCardTransmit(data->hCard, SCARD_PCI_T1, apdu, apduSize, NULL, resp, respSize);
	if (sw!=SCARD_S_SUCCESS)
		ODS("Errore trasmissione APDU");
	//else 
		//ODS(String().printf("RESP: %s\n", dumpHexData(ByteArray(resp, *respSize), String()).lock()).lock());
	
	return sw;
}

class CIEData {
public:
	CK_USER_TYPE userType;
	CAES aesKey;
	CToken token;
	bool init;
	CIEData(CSlot *slot,ByteArray atr) : ias((CToken::TokenTransmitCallback)TokenTransmitCallback,atr), slot(*slot) {
		ByteDynArray key;
		aesKey.Init(key.random(32));
		token.setTransmitCallbackData(slot);
		userType = -1;
		init = false;
	}
	CSlot &slot;
	IAS ias;
	std::shared_ptr<CP11PublicKey> pubKey;
	std::shared_ptr<CP11PrivateKey> privKey;
	std::shared_ptr<CP11Certificate> cert;
	ByteDynArray SessionPIN;
};

RESULT CIEtemplateInitLibrary(class CCardTemplate &Template, void *templateData){ return FAIL; }
RESULT CIEtemplateInitCard(void *&pTemplateData, CSlot &pSlot){ 
	init_func
	ByteArray ATR;
	pSlot.GetATR(ATR);

	pTemplateData = new CIEData(&pSlot, ATR);
	_return(OK)
	exit_func
	_return(FAIL)
}
void CIEtemplateFinalCard(void *pTemplateData){ 
	if (pTemplateData)
		delete (CIEData*)pTemplateData;
}

ByteArray SkipZero(ByteArray &ba) {
	for (DWORD i = 0; i < ba.size(); i++) {
		if (ba[i] != 0)
			return ba.mid(i);
	}
	return ByteArray();
}

BYTE label[] = { 'C','I','E','0' };
RESULT CIEtemplateInitSession(void *pTemplateData){ 
	CIEData* cie=(CIEData*)pTemplateData;

	if (!cie->init) {
		ByteDynArray certRaw;
		cie->slot.Connect();
		{
			safeConnection faseConn(cie->slot.hCard);
			CCardLocker lockCard(cie->slot.hCard);
			cie->ias.SetCardContext(&cie->slot);
			cie->ias.ReadPAN();
			ByteDynArray resp;
			cie->ias.ReadDappPubKey(resp);
			cie->ias.InitEncKey();
			cie->ias.GetCertificate(certRaw, true);
		}

		CK_BBOOL vtrue = TRUE;
		CK_BBOOL vfalse = FALSE;

		PCCERT_CONTEXT certDS = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certRaw.lock(), certRaw.size());
		if (certDS != nullptr) {
			cie->pubKey = std::make_shared<CP11PublicKey>(cie);
			cie->privKey = std::make_shared<CP11PrivateKey>(cie);
			cie->cert = std::make_shared<CP11Certificate>(cie);

			CASNParser keyParser;
			keyParser.Parse(ByteArray(certDS->pCertInfo->SubjectPublicKeyInfo.PublicKey.pbData, certDS->pCertInfo->SubjectPublicKeyInfo.PublicKey.cbData));
			auto Module = SkipZero(keyParser.tags[0]->tags[0]->content);
			auto Exponent = SkipZero(keyParser.tags[0]->tags[1]->content);
			CK_LONG keySizeBits = Module.size() * 8;
			cie->pubKey->addAttribute(CKA_LABEL, VarToByteArray(label));
			cie->pubKey->addAttribute(CKA_ID, VarToByteArray(label));
			cie->pubKey->addAttribute(CKA_PRIVATE, VarToByteArray(vfalse));
			cie->pubKey->addAttribute(CKA_TOKEN, VarToByteArray(vtrue));
			cie->pubKey->addAttribute(CKA_MODULUS, Module);
			cie->pubKey->addAttribute(CKA_PUBLIC_EXPONENT, Exponent);
			cie->pubKey->addAttribute(CKA_MODULUS_BITS, VarToByteArray(keySizeBits));
			cie->pubKey->addAttribute(CKA_VERIFY, VarToByteArray(vtrue));
			CK_KEY_TYPE keyrsa = CKK_RSA;
			cie->pubKey->addAttribute(CKA_KEY_TYPE, VarToByteArray(keyrsa));
			cie->slot.AddP11Object(cie->pubKey);

			cie->privKey->addAttribute(CKA_LABEL, VarToByteArray(label));
			cie->privKey->addAttribute(CKA_ID, VarToByteArray(label));
			cie->privKey->addAttribute(CKA_PRIVATE, VarToByteArray(vtrue));
			cie->privKey->addAttribute(CKA_TOKEN, VarToByteArray(vtrue));
			cie->privKey->addAttribute(CKA_KEY_TYPE, VarToByteArray(keyrsa));
			cie->privKey->addAttribute(CKA_MODULUS, Module);
			cie->privKey->addAttribute(CKA_PUBLIC_EXPONENT, Exponent);
			cie->privKey->addAttribute(CKA_SIGN, VarToByteArray(vtrue));
			cie->slot.AddP11Object(cie->privKey);

			cie->cert->addAttribute(CKA_LABEL, VarToByteArray(label));
			cie->cert->addAttribute(CKA_ID, VarToByteArray(label));
			cie->cert->addAttribute(CKA_PRIVATE, VarToByteArray(vfalse));
			cie->cert->addAttribute(CKA_TOKEN, VarToByteArray(vtrue));
			cie->cert->addAttribute(CKA_VALUE, ByteArray(certDS->pbCertEncoded, certDS->cbCertEncoded));
			cie->cert->addAttribute(CKA_ISSUER, ByteArray(certDS->pCertInfo->Issuer.pbData, certDS->pCertInfo->Issuer.cbData));
			cie->cert->addAttribute(CKA_SERIAL_NUMBER, ByteArray(certDS->pCertInfo->SerialNumber.pbData, certDS->pCertInfo->SerialNumber.cbData));
			cie->cert->addAttribute(CKA_SUBJECT, ByteArray(certDS->pCertInfo->Subject.pbData, certDS->pCertInfo->Subject.cbData));
			CK_CERTIFICATE_TYPE certx509 = CKC_X_509;
			cie->cert->addAttribute(CKA_CERTIFICATE_TYPE, VarToByteArray(certx509));
			CK_DATE start, end;
			SYSTEMTIME sFrom, sTo;
			char temp[10];
			if (!FileTimeToSystemTime(&certDS->pCertInfo->NotBefore, &sFrom))
				return FAIL;
			if (!FileTimeToSystemTime(&certDS->pCertInfo->NotAfter, &sTo))
				return FAIL;
			sprintf_s(temp, "%04i", sFrom.wYear); VarToByteArray(start.year).copy(ByteArray((BYTE*)temp, 4));
			sprintf_s(temp, "%02i", sFrom.wMonth); VarToByteArray(start.month).copy(ByteArray((BYTE*)temp, 2));
			sprintf_s(temp, "%02i", sFrom.wDay); VarToByteArray(start.day).copy(ByteArray((BYTE*)temp, 2));
			sprintf_s(temp, "%04i", sTo.wYear); VarToByteArray(end.year).copy(ByteArray((BYTE*)temp, 2));
			sprintf_s(temp, "%02i", sTo.wMonth); VarToByteArray(end.month).copy(ByteArray((BYTE*)temp, 2));
			sprintf_s(temp, "%02i", sTo.wDay); VarToByteArray(end.day).copy(ByteArray((BYTE*)temp, 2));
			cie->cert->addAttribute(CKA_START_DATE, VarToByteArray(start));
			cie->cert->addAttribute(CKA_END_DATE, VarToByteArray(end));

			cie->slot.AddP11Object(cie->cert);
		}
		cie->init = true;
	}
	return OK;
}
RESULT CIEtemplateFinalSession(void *pTemplateData){ 
	//delete (CIEData*)pTemplateData;
	return OK; 
}

RESULT CIEtemplateMatchCard(bool &bMatched, CSlot &pSlot){ 
	init_func
	CToken token;

	pSlot.Connect();
	{
		safeConnection faseConn(pSlot.hCard);
		ByteArray ATR;
		pSlot.GetATR(ATR);
		token.setTransmitCallback((CToken::TokenTransmitCallback)TokenTransmitCallback, &pSlot);
		IAS ias((CToken::TokenTransmitCallback)TokenTransmitCallback, ATR);
		ias.SetCardContext(&pSlot);
		{
			safeTransaction trans(faseConn,SCARD_LEAVE_CARD);
			ias.ReadPAN();
		}
		bMatched = true;
		_return(OK);
	}
	exit_func
	_return(FAIL)
}

RESULT CIEtemplateGetSerial(CSlot &pSlot, ByteDynArray &baSerial){
	init_func
		CToken token;

	pSlot.Connect();
	{
		safeConnection faseConn(pSlot.hCard);
		CCardLocker lockCard(pSlot.hCard);
		ByteArray ATR;
		pSlot.GetATR(ATR);
		IAS ias((CToken::TokenTransmitCallback)TokenTransmitCallback, ATR);
		ias.SetCardContext(&pSlot);
		ias.ReadPAN();
		std::string numSerial;
		dumpHexData(ias.PAN.mid(5, 6), numSerial, false);
		baSerial = ByteArray((BYTE*)numSerial.c_str(),numSerial.length());
		_return(OK);
	}
	exit_func
		_return(FAIL)
}
RESULT CIEtemplateGetModel(CSlot &pSlot, std::string &szModel){ 
	szModel = ""; 
	return OK;
}
RESULT CIEtemplateGetTokenFlags(CSlot &pSlot, DWORD &dwFlags){ 
	dwFlags = CKF_LOGIN_REQUIRED | CKF_USER_PIN_INITIALIZED | CKF_TOKEN_INITIALIZED | CKF_REMOVABLE_DEVICE;
	return OK;
}

RESULT CIEtemplateLogin(void *pTemplateData, CK_USER_TYPE userType, ByteArray &Pin) {
	init_func
	CToken token;
	CIEData* cie = (CIEData*)pTemplateData;

	cie->SessionPIN.clear();
	cie->userType = -1;

	cie->slot.Connect();
	cie->ias.SetCardContext(&cie->slot);
	cie->ias.token.Reset();
	{
		safeConnection safeConn(cie->slot.hCard);
		CCardLocker lockCard(cie->slot.hCard);

		cie->ias.SelectAID_IAS();
		cie->ias.InitDHParam();

		if (cie->ias.DappPubKey.isEmpty()) {
			ByteDynArray DappKey;
			cie->ias.ReadDappPubKey(DappKey);
		}

		cie->ias.InitExtAuthKeyParam();
		// faccio lo scambio di chiavi DH	
		if (cie->ias.Callback != nullptr)
			cie->ias.Callback(1, "DiffieHellman", cie->ias.CallbackData);
		cie->ias.DHKeyExchange();
		// DAPP
		if (cie->ias.Callback != nullptr)
			cie->ias.Callback(2, "DAPP", cie->ias.CallbackData);
		cie->ias.DAPP();
		// verifica PIN
		DWORD sw;
		if (cie->ias.Callback != nullptr)
			cie->ias.Callback(3, "Verify PIN", cie->ias.CallbackData);
		if (userType == CKU_USER) {
			ByteDynArray FullPIN;
			cie->ias.GetFirstPIN(FullPIN);
			FullPIN.append(Pin);
			sw = cie->ias.VerifyPIN(FullPIN);
		}
		else if (userType == CKU_SO) {
			sw = cie->ias.VerifyPUK(Pin);
		}
		else
			return CKR_ARGUMENTS_BAD;

		if (sw == 0x6983) {
			if (userType == CKU_USER)
				cie->ias.IconaSbloccoPIN();
			return CKR_PIN_LOCKED;
		}
		if (sw >= 0x63C0 && sw <= 0x63CF) {
			//*pcAttemptsRemaining = sw - 0x63C0;
			return CKR_PIN_INCORRECT;
		}
		if (sw == 0x6700) {
			return CKR_PIN_INCORRECT;
		}
		if (sw == 0x6300)
			return CKR_PIN_INCORRECT;
		if (sw != 0x9000) {
			throw CSCardException((WORD)sw);
		}

		cie->aesKey.Encode(Pin, cie->SessionPIN);
		cie->userType = userType;
		_return(OK);
	}
	exit_func
		_return(FAIL)
}
RESULT CIEtemplateLogout(void *pTemplateData, CK_USER_TYPE userType){ 
	CIEData* cie = (CIEData*)pTemplateData;
	cie->userType = -1;
	cie->SessionPIN.clear();
	return OK; 
}
RESULT CIEtemplateReadObjectAttributes(void *pCardTemplateData, CP11Object *pObject){ 
	return OK; 
}
RESULT CIEtemplateSign(void *pCardTemplateData, CP11PrivateKey *pPrivKey, ByteArray &baSignBuffer, ByteDynArray &baSignature, CK_MECHANISM_TYPE mechanism, bool bSilent){ 
	init_func
	CToken token;
	CIEData* cie = (CIEData*)pCardTemplateData;
	if (cie->userType == CKU_USER) {
		ByteDynArray Pin;
		cie->slot.Connect();
		cie->ias.SetCardContext(&cie->slot);
		cie->ias.token.Reset();
		{
			safeConnection safeConn(cie->slot.hCard);
			CCardLocker lockCard(cie->slot.hCard);
			cie->aesKey.Decode(cie->SessionPIN, Pin);
			cie->ias.SelectAID_IAS();
			cie->ias.SelectAID_CIE();
			cie->ias.DHKeyExchange();
			cie->ias.DAPP();

			ByteDynArray FullPIN;
			cie->ias.GetFirstPIN(FullPIN);
			FullPIN.append(Pin);
			CARD_R_CALL(cie->ias.VerifyPIN(FullPIN));
			cie->ias.Sign(baSignBuffer, baSignature);
		}
	}
	_return(OK);
	exit_func
	_return(FAIL)
}

RESULT CIEtemplateInitPIN(void *pCardTemplateData, ByteArray &baPin){ 
	init_func
	CToken token;
	CIEData* cie = (CIEData*)pCardTemplateData;
	if (cie->userType == CKU_SO) {
		// posso usarla solo se sono loggato come so
		ByteDynArray Pin;
		cie->slot.Connect();
		cie->ias.SetCardContext(&cie->slot);
		cie->ias.token.Reset();
		{
			safeConnection safeConn(cie->slot.hCard);
			CCardLocker lockCard(cie->slot.hCard);
			cie->aesKey.Decode(cie->SessionPIN, Pin);
			cie->ias.SelectAID_IAS();
			cie->ias.SelectAID_CIE();

			cie->ias.DHKeyExchange();
			cie->ias.DAPP();
			CARD_R_CALL(cie->ias.VerifyPUK(Pin))
			CARD_R_CALL(cie->ias.UnblockPIN())

			ByteDynArray changePIN;
			cie->ias.GetFirstPIN(changePIN);
			changePIN.append(baPin);

			CARD_R_CALL(cie->ias.ChangePIN(changePIN))
		}
	}
	else
		return CKR_FUNCTION_NOT_SUPPORTED;
	_return(OK);
	exit_func
		_return(FAIL)
}

RESULT CIEtemplateSetPIN(void *pCardTemplateData, ByteArray &baOldPin, ByteArray &baNewPin, CK_USER_TYPE User)
{
	init_func
	CToken token;
	CIEData* cie = (CIEData*)pCardTemplateData;
	if (cie->userType != CKU_SO) {
		// posso usarla sia se sono loggato come user sia se non sono loggato
		ByteDynArray Pin;
		cie->slot.Connect();
		cie->ias.SetCardContext(&cie->slot);
		cie->ias.token.Reset();
		{
			safeConnection safeConn(cie->slot.hCard);
			CCardLocker lockCard(cie->slot.hCard);
			cie->ias.SelectAID_IAS();
			if (cie->userType != CKU_USER)
				cie->ias.InitDHParam();
			cie->ias.SelectAID_CIE();

			if (cie->userType != CKU_USER) {
				cie->ias.ReadPAN();
				ByteDynArray resp;
				cie->ias.ReadDappPubKey(resp);
			}

			cie->ias.DHKeyExchange();
			cie->ias.DAPP();
			ByteDynArray oldPIN,newPIN;
			cie->ias.GetFirstPIN(oldPIN);
			newPIN = oldPIN;
			oldPIN.append(baOldPin);
			newPIN.append(baNewPin);

			CARD_R_CALL(cie->ias.VerifyPIN(oldPIN))
			CARD_R_CALL(cie->ias.ChangePIN(oldPIN,newPIN))
		}
	}
	else
		return CKR_FUNCTION_NOT_SUPPORTED;
	_return(OK);
	exit_func
	_return(FAIL)
}

RESULT CIEtemplateSignRecover(void *pCardTemplateData, CP11PrivateKey *pPrivKey, ByteArray &baSignBuffer, ByteDynArray &baSignature, CK_MECHANISM_TYPE mechanism, bool bSilent){ return FAIL; }
RESULT CIEtemplateDecrypt(void *pCardTemplateData, CP11PrivateKey *pPrivKey, ByteArray &baEncryptedData, ByteDynArray &baData, CK_MECHANISM_TYPE mechanism, bool bSilent){ return FAIL; }
RESULT CIEtemplateGenerateRandom(void *pCardTemplateData, ByteArray &baRandomData){ return FAIL; }
RESULT CIEtemplateGetObjectSize(void *pCardTemplateData, CP11Object *pObject, CK_ULONG_PTR pulSize){ return FAIL; }
RESULT CIEtemplateSetKeyPIN(void *pTemplateData, CP11Object *pObject, ByteArray &Pin){ return FAIL; }
RESULT CIEtemplateSetAttribute(void *pTemplateData, CP11Object *pObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount){ return FAIL; }
RESULT CIEtemplateCreateObject(void *pTemplateData, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, std::shared_ptr<CP11Object>&pObject){ return FAIL; }
RESULT CIEtemplateDestroyObject(void *pTemplateData, CP11Object &Object){ return FAIL; }
RESULT CIEtemplateGenerateKey(void *pCardTemplateData, CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, std::shared_ptr<CP11Object>&pObject){ return FAIL; }
RESULT CIEtemplateGenerateKeyPair(void *pCardTemplateData, CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount, CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount, std::shared_ptr<CP11Object>&pPublicKey, std::shared_ptr<CP11Object>&pPrivateKey){ return FAIL; }
