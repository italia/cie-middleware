#include "../stdafx.h"
#include "IAS.h"
#include "CSP.h"
#include "../util/ModuleInfo.h"
#include "../UI/Message.h"
#include "../UI/Pin.h"
#include "../UI/Verifica.h"
#include "../crypto/SHA256.h"
#include "../crypto/sha512.h"
#include <functional>
#include "../crypto/ASNParser.h"
#include "../UI/safeDesktop.h"
#include "../PCSC/PCSC.h"
#include <atlbase.h>
#include <string>
#include "PINManager.h"
#include <shlobj_core.h>
#include "../UI/SystemTray.h"
#include "CIESign.h"
#include "CIEVerify.h"

#include "../LOGGER/Logger.h"

using namespace CieIDLogger;

#define CARD_ALREADY_ENABLED		0x000000F0;
#define CARD_PAN_MISMATCH			(int)(0x000000F1);

typedef CK_RV(*AbilitaCIEfn)(const char*  szPAN,
	const char*  szPIN,
	int* attempts,
	PROGRESS_CALLBACK progressCallBack,
	COMPLETED_CALLBACK completedCallBack);

typedef CK_RV(*VerificaCIEAbilitatafn)(const char*  szPAN);
typedef CK_RV(*DisabilitaCIEfn)(const char*  szPAN);

int TokenTransmitCallback(safeConnection *data, uint8_t *apdu, DWORD apduSize, uint8_t *resp, DWORD *respSize);

DWORD CardAuthenticateEx(IAS*       ias,
	DWORD       PinId,
	DWORD       dwFlags,
	BYTE*       pbPinData,
	DWORD       cbPinData,
	BYTE*       *ppbSessionPin,
	DWORD*      pcbSessionPin,
	int*        pcAttemptsRemaining);


extern "C" {


	CK_RV CK_ENTRY __stdcall VerificaCIEAbilitata(const char*  szPAN)
	{

		if (IAS::IsEnrolled(szPAN))
			return 1;
		else
			return 0;

	}


	CK_RV CK_ENTRY __stdcall DisabilitaCIE(const char*  szPAN)
	{

		if (IAS::IsEnrolled(szPAN))
		{
			IAS::Unenroll(szPAN);
			LOG_INFO("DisabilitaCIE - CIE number %s removed", szPAN);
			return CKR_OK;
		}

		LOG_ERROR("DisabilitaCIE - Unable to remove CIE number %s, CIE is not enrolled", szPAN);
		return CKR_FUNCTION_FAILED;
	}


	VERIFY_RESULT verifyResult;

	CK_RV CK_ENTRY __stdcall getNumberOfSign()
	{
		return (CK_RV)verifyResult.verifyInfo.pSignerInfos->nCount;
	}

	CK_RV CK_ENTRY __stdcall getVerifyInfo(int index, struct verifyInfo_t* vInfos)
	{

		if (index >= 0 && index < getNumberOfSign())
		{
			SIGNER_INFO tmpSignerInfo = (verifyResult.verifyInfo.pSignerInfos->pSignerInfo)[index];// +(index * sizeof(SIGNER_INFO)));
			strcpy_s(vInfos->name, 512, tmpSignerInfo.szGIVENNAME);
			strcpy_s(vInfos->surname, 512, tmpSignerInfo.szSURNAME);
			strcpy_s(vInfos->cn, 512, tmpSignerInfo.szCN);
			strcpy_s(vInfos->cadn, 512, tmpSignerInfo.szCADN);
			strcpy_s(vInfos->signingTime, 512, tmpSignerInfo.szSigningTime);
			vInfos->CertRevocStatus = tmpSignerInfo.pRevocationInfo->nRevocationStatus;
			vInfos->isCertValid = (tmpSignerInfo.bitmask & VERIFIED_CERT_GOOD) == VERIFIED_CERT_GOOD;
			vInfos->isSignValid = (tmpSignerInfo.bitmask & VERIFIED_SIGNATURE) == VERIFIED_SIGNATURE;
		}

		return 0;
	}

	CK_RV CK_ENTRY __stdcall verificaConCIE(const char* inFilePath, const char* proxyAddress, int proxyPort, const char* usrPass)
	{
		CIEVerify* verifier = new CIEVerify();

		verifier->verify(inFilePath, (VERIFY_RESULT*)&verifyResult, proxyAddress, proxyPort, usrPass);

		if (verifyResult.nErrorCode == 0)
		{
			printf("verificaConCIE OK");
			return (CK_RV)verifyResult.verifyInfo.pSignerInfos->nCount;
		}
		else
		{
			printf("Errore nella verifica: %lu\n", verifyResult.nErrorCode);
			return verifyResult.nErrorCode;
		}

	}

	CK_RV CK_ENTRY __stdcall estraiP7m(const char* inFilePath, const char* outFilePath) {

		CIEVerify* verifier = new CIEVerify();

		long res = verifier->get_file_from_p7m(inFilePath, outFilePath);

		return res;

	}

	CK_RV CK_ENTRY __stdcall firmaConCIE(const char* inFilePath, const char* type, const char* pin, const char* pan, int page, float x, float y, float w, float h, const char* imagePathFile, const char* outFilePath, PROGRESS_CALLBACK progressCallBack, SIGN_COMPLETED_CALLBACK completedCallBack)
	{

		printf("page: %d, x: %f, y: %f, w: %f, h: %f", page, x, y, w, h);

		char* readers = NULL;
		char* ATR = NULL;
		try
		{
			std::map<uint8_t, ByteDynArray> hashSet;

			DWORD len = 0;
			ByteDynArray CertCIE;
			ByteDynArray SOD;

			SCARDCONTEXT hSC;

			progressCallBack(1, "");

			long nRet = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &hSC);
			if (nRet != SCARD_S_SUCCESS)
				return CKR_DEVICE_ERROR;

			LOG_DEBUG("firmaConCIE - Establish Context ok\n");

			if (SCardListReaders(hSC, nullptr, NULL, &len) != SCARD_S_SUCCESS) {
				LOG_ERROR("firmaConCIE - SCardListReaders list readers failed\n");
				return CKR_TOKEN_NOT_PRESENT;
			}

			if (len == 1)
				return CKR_TOKEN_NOT_PRESENT;

			readers = (char*)malloc(len);

			if (SCardListReaders(hSC, nullptr, (char*)readers, &len) != SCARD_S_SUCCESS) {
				free(readers);
				return CKR_TOKEN_NOT_PRESENT;
			}

			char *curreader = readers;
			bool foundCIE = false;
			for (; curreader[0] != 0; curreader += strnlen(curreader, len) + 1)
			{

				safeConnection conn(hSC, curreader, SCARD_SHARE_SHARED);
				if (!conn.hCard)
					continue;

				LONG res = 0;

				res = SCardBeginTransaction(conn.hCard);

				while (res != SCARD_S_SUCCESS)
				{
					DWORD protocol = 0;
					SCardReconnect(conn.hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_UNPOWER_CARD, &protocol);
					
					res = SCardBeginTransaction(conn.hCard);
				}

				DWORD atrLen = 40;
				res = SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR, &atrLen);
				if (res != SCARD_S_SUCCESS) {
					free(readers);

					LOG_ERROR("firmaConCIE - GetAttrib error: %d", res);
					return CKR_DEVICE_ERROR;
				}

				ATR = (char*)malloc(atrLen);

				if (SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR, &atrLen) != SCARD_S_SUCCESS) {
					free(readers);
					free(ATR);
					return CKR_DEVICE_ERROR;
				}

				ByteArray atrBa((BYTE*)ATR, atrLen);

				progressCallBack(20, "");

				IAS* ias = new IAS((CToken::TokenTransmitCallback)TokenTransmitCallback, atrBa);
				ias->SetCardContext(&conn);
				foundCIE = false;

				ias->token.Reset();
				ias->SelectAID_IAS();
				ias->ReadPAN();

				ByteDynArray IntAuth;
				ias->SelectAID_CIE();
				ias->ReadDappPubKey(IntAuth);
				ias->SelectAID_CIE();
				ias->InitEncKey();

				ByteDynArray IdServizi;
				ias->ReadIdServizi(IdServizi);
				ByteArray baPan = ByteArray((uint8_t*)pan, strlen(pan));

				if (memcmp(baPan.data(), IdServizi.data(), IdServizi.size()) != 0)
				{
					return CARD_PAN_MISMATCH;
				}

				foundCIE = true;
				ByteDynArray FullPIN;
				ByteArray LastPIN = ByteArray((uint8_t*)pin, strlen(pin));
				ias->GetFirstPIN(FullPIN);
				FullPIN.append(LastPIN);
				ias->token.Reset();
				
				progressCallBack(40, "");

				char fullPinCStr[9];
				memcpy(fullPinCStr, FullPIN.data(), 8);
				fullPinCStr[8] = 0;

				CIESign* cieSign = new CIESign(ias);

				uint16_t ret = cieSign->sign(inFilePath, type, fullPinCStr, page, x, y, w, h, imagePathFile, outFilePath);
				
				if ((ret & (0x63C0)) == 0x63C0)
				{
					return CKR_PIN_INCORRECT;
				}
				else if (ret == 0x6983)
				{
					return CKR_PIN_LOCKED;
				}
				else if (ret != 0)
				{
					return CKR_GENERAL_ERROR;
				}

				progressCallBack(100, "");
				
				LOG_DEBUG("firmaConCIE - cie_sign_sdk sign return: %d", ret);
				

				free(ias);
				free(cieSign);

				completedCallBack(ret);
			}

			if (!foundCIE) {
				free(ATR);
				free(readers);
				return CKR_TOKEN_NOT_RECOGNIZED;

			}
		}
		catch (std::exception &ex) {
			OutputDebugString(ex.what());
			if (ATR)
				free(ATR);
			LOG_ERROR("firmaConCIE - Eccezione: %s", ex.what());
			if (readers)
				free(readers);

			LOG_ERROR("firmaConCIE - General error\n");
			return CKR_GENERAL_ERROR;
		}

		if (ATR)
			free(ATR);

		free(readers);

		return SCARD_S_SUCCESS;
	}

	CK_RV CK_ENTRY __stdcall AbbinaCIE(const char*  szPAN, const char*  szPIN, int* attempts, PROGRESS_CALLBACK progressCallBack, COMPLETED_CALLBACK completedCallBack)
	{
		LOG_INFO("***** Starting AbbinaCIE *****");
		LOG_DEBUG("szPAN:%s, pin len : %d, attempts : %d", szPAN, strlen(szPIN), *attempts);

		char* readers = NULL;
		char* ATR = NULL;
		try
		{

			std::map<uint8_t, ByteDynArray> hashSet;

			DWORD len = 0;
			ByteDynArray CertCIE;
			ByteDynArray SOD;

			SCARDCONTEXT hSC;
			LONG res = 0;
			
			LOG_INFO("AbbinaCIE - Connecting to CIE...");
			progressCallBack(1, "Connessione alla CIE");

			long nRet = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &hSC);
			if (nRet != SCARD_S_SUCCESS)
				return CKR_DEVICE_ERROR;

			LOG_DEBUG("AbbinaCIE - Establish Context ok");

			res = SCardListReaders(hSC, nullptr, NULL, &len);
			if (res != SCARD_S_SUCCESS) {
				LOG_ERROR("AbbinaCIE - SCardListReaders error: %d", res);
				return CKR_TOKEN_NOT_PRESENT;
			}

			if (len == 1) {
				LOG_ERROR("AbbinaCIE - SCardListReaders error. Len: %d", len);
				return CKR_TOKEN_NOT_PRESENT;
			}

			readers = (char*)malloc(len);

			res = SCardListReaders(hSC, nullptr, (char*)readers, &len);
			if (res != SCARD_S_SUCCESS) {
				LOG_ERROR("AbbinaCIE - SCardListReaders error: %d", res);
				free(readers);
				return CKR_TOKEN_NOT_PRESENT;
			}
			
			char *curreader = readers;
			bool foundCIE = false;
			for (; curreader[0] != 0; curreader += strnlen(curreader, len) + 1)
			{
				
				safeConnection conn(hSC, curreader, SCARD_SHARE_SHARED);
				if (!conn.hCard)
					continue;

				res = SCardBeginTransaction(conn.hCard);

				while (res != SCARD_S_SUCCESS)
				{
					DWORD protocol = 0;
					SCardReconnect(conn.hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_UNPOWER_CARD, &protocol);
					res = SCardBeginTransaction(conn.hCard);
				}

				progressCallBack(5, "CIE Connessa");
				LOG_INFO("AbbinaCIE - CIE Connected");

				DWORD atrLen = 40;
				res = SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR, &atrLen);
				if (res != SCARD_S_SUCCESS) {
					free(readers);
					LOG_ERROR("AbbinaCIE - SCardGetAttrib error, %d\n", res);
					return CKR_DEVICE_ERROR;
				}

				ATR = (char*)malloc(atrLen);

				res = SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR, &atrLen);
				if (res != SCARD_S_SUCCESS) {
					LOG_ERROR("AbbinaCIE - SCardGetAttrib error, %d\n", res);
					free(readers);
					free(ATR);
					return CKR_DEVICE_ERROR;
				}

				ByteArray atrBa((BYTE*)ATR, atrLen);
				
				progressCallBack(10, "Verifica carta esistente");

				LOG_DEBUG("AbbinaCIE - Checking if card has been activated yet...");
				IAS ias((CToken::TokenTransmitCallback)TokenTransmitCallback, atrBa);

				LOG_DEBUG("AbbinaCIE - CIE ATR: %s", dumpHexData(atrBa, std::string()).c_str());

				ias.SetCardContext(&conn);

				foundCIE = false;

				ias.token.Reset();
				ias.SelectAID_IAS();
				ias.ReadPAN();
				
				ByteDynArray IntAuth;
				ias.SelectAID_CIE();
				ias.ReadDappPubKey(IntAuth);
				//ias.SelectAID_CIE();
				ias.InitEncKey();

				ByteDynArray IdServizi;
				ias.ReadIdServizi(IdServizi);

				if (ias.IsEnrolled())
				{
					LOG_ERROR("AbbinaCIE - CIE already enabled. Serial number: %s\n", IdServizi.data());
					return CARD_ALREADY_ENABLED;
				}

				progressCallBack(15, "Lettura dati dalla CIE");
				
				LOG_INFO("AbbinaCIE - Reading data from CIE...");
				ByteArray serviziData(IdServizi.left(12));
				
				ByteDynArray SOD;
				ias.ReadSOD(SOD);


				//LOG_DEBUG("AbbinaCIE - SOD:");
				//LOG_BUFFER(SOD.data(), SOD.size());
				
				uint8_t digest = ias.GetSODDigestAlg(SOD);

				ByteArray intAuthData(IntAuth.left(GetASN1DataLenght(IntAuth)));

				ByteDynArray IntAuthServizi;
				ias.ReadServiziPubKey(IntAuthServizi);
				ByteArray intAuthServiziData(IntAuthServizi.left(GetASN1DataLenght(IntAuthServizi)));

				ias.SelectAID_IAS();
				ByteDynArray DH;
				ias.ReadDH(DH);
				ByteArray dhData(DH.left(GetASN1DataLenght(DH)));

				if (szPAN && IdServizi != ByteArray((uint8_t*)szPAN, strnlen(szPAN, 20)))
					continue;

				foundCIE = true;

				progressCallBack(20, "Autenticazione...");

				//Scambio di chiavi, lettura certificato (DAPP da verificare meglio) e verify pin
				LOG_INFO("AbbinaCIE - Verifying PIN and performing authentication...");
				DWORD rs = CardAuthenticateEx(&ias, ROLE_USER, FULL_PIN, (BYTE*)szPIN, (DWORD)strnlen(szPIN, 8), nullptr, 0, attempts);
				if (rs == SCARD_W_WRONG_CHV)
				{
					LOG_ERROR("AbbinaCIE - CardAuthenticateEx Wrong Pin");
					free(ATR);
					free(readers);
					return CKR_PIN_INCORRECT;
				}
				else if (rs == SCARD_W_CHV_BLOCKED)
				{
					LOG_ERROR("AbbinaCIE - CardAuthenticateEx Pin locked");
					free(ATR);
					free(readers);
					return CKR_PIN_LOCKED;
				}
				else if (rs != SCARD_S_SUCCESS)
				{
					LOG_ERROR("AbbinaCIE - CardAuthenticateEx Generic error, res:%d", rs);
					free(ATR);
					free(readers);
					return CKR_GENERAL_ERROR;
				}
				
				progressCallBack(45, "Lettura seriale");

				ByteDynArray Serial;
				ias.ReadSerialeCIE(Serial);
				ByteArray serialData = Serial.left(9);

				std::string seriale((char*)Serial.data(), Serial.size());


				progressCallBack(55, "Lettura certificato");
				LOG_INFO("AbbinaCIE - Reading certificate...");

				ByteDynArray CertCIE;
				ias.ReadCertCIE(CertCIE);

				//LOG_DEBUG("AbbinaCIE - Certificate: ");
				//LOG_BUFFER(CertCIE.data(), CertCIE.size());

				ByteArray certCIEData = CertCIE.left(GetASN1DataLenght(CertCIE));


				LOG_INFO("AbbinaCIE - Verifying SOD, digest algorithm: %s", (digest == 1) ? "RSA/SHA256" : "RSA-PSS/SHA512");
				if (digest == 1)
				{
					CSHA256 sha256;
					hashSet[0xa1] = sha256.Digest(serviziData);
					hashSet[0xa4] = sha256.Digest(intAuthData);
					hashSet[0xa5] = sha256.Digest(intAuthServiziData);
					hashSet[0x1b] = sha256.Digest(dhData);
					hashSet[0xa2] = sha256.Digest(serialData);
					hashSet[0xa3] = sha256.Digest(certCIEData);

					ias.VerificaSOD(SOD, hashSet);
				}
				else
				{
					CSHA512 sha512;
					hashSet[0xa1] = sha512.Digest(serviziData);
					hashSet[0xa4] = sha512.Digest(intAuthData);
					hashSet[0xa5] = sha512.Digest(intAuthServiziData);
					hashSet[0x1b] = sha512.Digest(dhData);
					hashSet[0xa2] = sha512.Digest(serialData);
					hashSet[0xa3] = sha512.Digest(certCIEData);
					ias.VerificaSODPSS(SOD, hashSet);
				}
				
				ByteArray pinBa((uint8_t*)szPIN, 4);

				progressCallBack(85, "Memorizzazione in cache");
				LOG_INFO("AbbinaCIE - Saving certificate in cache...");

				std::string span((char*)IdServizi.data(), IdServizi.size());

				ias.SetCache(span.c_str(), CertCIE, pinBa);

				std::string name;
				std::string surname;

				PCCERT_CONTEXT cert = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certCIEData.data(), (DWORD)certCIEData.size());

				char szGivenName[MAX_PATH];
				char szSurname[MAX_PATH];

				DWORD dwRes = CertGetNameString(cert, CERT_NAME_ATTR_TYPE, NULL, szOID_GIVEN_NAME, szGivenName, MAX_PATH);
				
				dwRes = CertGetNameString(cert, CERT_NAME_ATTR_TYPE, NULL, szOID_SUR_NAME, szSurname, MAX_PATH);
				
				name = szGivenName;
				surname = szSurname;

				std::string fullname = name + " " + surname;
				completedCallBack(span.c_str(), fullname.c_str(), seriale.c_str());

				SCardEndTransaction(conn.hCard, SCARD_RESET_CARD);

			}

			if (!foundCIE) {
				LOG_ERROR("AbbinaCIE - No CIE available");
				free(ATR);
				free(readers);
				return CKR_TOKEN_NOT_RECOGNIZED;

			}

		}
		catch (std::exception &ex) {

			if (ATR)
				free(ATR);
			LOG_ERROR("AbbinaCIE - Exception %s ", ex.what());

			if (readers)
				free(readers);

			return CKR_GENERAL_ERROR;
		}

		if (ATR)
			free(ATR);

		free(readers);

		LOG_INFO("AbbinaCIE - CIE paired successfully");
		progressCallBack(100, "");

		LOG_INFO("***** AbbinaCIE Ended *****");
		return SCARD_S_SUCCESS;
	}

}

DWORD CardAuthenticateEx(IAS*       ias,
	DWORD       PinId,
	DWORD       dwFlags,
	BYTE*       pbPinData,
	DWORD       cbPinData,
	BYTE*       *ppbSessionPin,
	DWORD*      pcbSessionPin,
	int*      pcAttemptsRemaining) {

	LOG_INFO("***** Starting CardAuthenticateEx *****");
	LOG_DEBUG("Pin id: %d, dwFlags: %d, cbPinData: %d, pbSessionPin: %s, pcAttemptsRemaining: %d", PinId, dwFlags, cbPinData, pcbSessionPin, *pcAttemptsRemaining);


	LOG_INFO("CardAuthenticateEx - Selecting IAS and CIE AID");

	ias->SelectAID_IAS();
	ias->SelectAID_CIE();


	// leggo i parametri di dominio DH e della chiave di extauth
	LOG_INFO("CardAuthenticateEx - Reading DH parameters");
	ias->InitDHParam();

	ByteDynArray dappData;
	ias->ReadDappPubKey(dappData);

	ias->InitExtAuthKeyParam();

	LOG_INFO("CardAuthenticateEx - Performing DH Exchange");
	ias->DHKeyExchange();

	// DAPP
	ias->DAPP();

	// verifica PIN
	StatusWord sw;
	if (PinId == ROLE_USER) {
		LOG_INFO("CardAuthenticateEx - Verifying PIN");
		ByteDynArray PIN;
		if ((dwFlags & FULL_PIN) != FULL_PIN)
			ias->GetFirstPIN(PIN);
		PIN.append(ByteArray(pbPinData, cbPinData));
		sw = ias->VerifyPIN(PIN);
	}
	else if (PinId == ROLE_ADMIN) {
		LOG_INFO("CardAuthenticateEx - Verifying PUK");
		ByteArray pinBa(pbPinData, cbPinData);
		sw = ias->VerifyPUK(pinBa);
	}
	else {
		LOG_ERROR("CardAuthenticateEx - Invalid parameter: wrong PinId value");
		return SCARD_E_INVALID_PARAMETER;
	}

	if (sw == 0x6983) {
		//if (PinId == ROLE_USER)
		//	ias->IconaSbloccoPIN();
		LOG_ERROR("CardAuthenticateEx - Pin locked");
		return SCARD_W_CHV_BLOCKED;
	}
	if (sw >= 0x63C0 && sw <= 0x63CF) {
		if (pcAttemptsRemaining != nullptr)
			*pcAttemptsRemaining = sw - 0x63C0;
		LOG_ERROR("CardAuthenticateEx - Wrong Pin");
		return SCARD_W_WRONG_CHV;
	}
	if (sw == 0x6700) {
		LOG_ERROR("CardAuthenticateEx - Wrong Pin");
		return SCARD_W_WRONG_CHV;
	}
	if (sw == 0x6300) {
		LOG_ERROR("CardAuthenticateEx - Wrong Pin");
		return SCARD_W_WRONG_CHV;
	}
	if (sw != 0x9000) {
		LOG_ERROR("CarduAuthenticateEx - Smart Card error: 0x%04X", sw);
	}

	LOG_INFO("***** CardAuthenticateEx Ended *****");
	return SCARD_S_SUCCESS;
}

int TokenTransmitCallback(safeConnection *conn, BYTE *apdu, DWORD apduSize, BYTE *resp, DWORD *respSize) {

	LOG_DEBUG("TokenTransmitCallback - Apdu:");
	LOG_BUFFER(apdu, apduSize);
	
	if (apduSize == 2) {
		WORD code = *(WORD*)apdu;
		if (code == 0xfffd) {
			long bufLen = *respSize;
			*respSize = sizeof(conn->hCard) + 2;
			memcpy_s(resp, bufLen, &conn->hCard, sizeof(conn->hCard));
			resp[sizeof(&conn->hCard)] = 0;
			resp[sizeof(&conn->hCard) + 1] = 0;

			return SCARD_S_SUCCESS;
		}
		else if (code == 0xfffe) {
			DWORD protocol = 0;
			LOG_INFO("TokenTransmitCallback - Unpowering Card");
			auto ris = SCardReconnect(conn->hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_UNPOWER_CARD, &protocol);

			if (ris == SCARD_S_SUCCESS) {
				SCardBeginTransaction(conn->hCard);
				*respSize = 2;
				resp[0] = 0x90;
				resp[1] = 0x00;
			}
			return ris;
		}
		else if (code == 0xffff) {
			DWORD protocol = 0;
			auto ris = SCardReconnect(conn->hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_RESET_CARD, &protocol);
			if (ris == SCARD_S_SUCCESS) {
				SCardBeginTransaction(conn->hCard);
				*respSize = 2;
				resp[0] = 0x90;
				resp[1] = 0x00;
			}
			LOG_INFO("TokenTransmitCallback - Resetting Card");
			return ris;
		}
	}
	//ODS(String().printf("APDU: %s\n", dumpHexData(ByteArray(apdu, apduSize), String()).lock()).lock());
	auto ris = SCardTransmit(conn->hCard, SCARD_PCI_T1, apdu, apduSize, NULL, resp, respSize);

	LOG_DEBUG("TokenTransmitCallback - Smart card response:");
	LOG_BUFFER(resp, *respSize);
	
	if (ris == SCARD_W_RESET_CARD || ris == SCARD_W_UNPOWERED_CARD)
	{
		LOG_INFO("TokenTransmitCallback - Card Reset done");
		DWORD protocol = 0;
		ris = SCardReconnect(conn->hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_LEAVE_CARD, &protocol);
		if (ris != SCARD_S_SUCCESS)
		{
			LOG_ERROR("TokenTransmitCallback - ScardReconnect error: %d");
		}
		else
		{
			ris = SCardTransmit(conn->hCard, SCARD_PCI_T1, apdu, apduSize, NULL, resp, respSize);

			LOG_DEBUG("TokenTransmitCallback - Smart card response:");
			LOG_BUFFER(resp, *respSize);
		}
	}

	if (ris != SCARD_S_SUCCESS) {
		LOG_ERROR("TokenTransmitCallback - SCardTransmit error: %d", ris);
	}


	return ris;
}



extern CModuleInfo moduleInfo;
extern "C" DWORD WINAPI CardAcquireContext(IN PCARD_DATA pCardData, __in DWORD dwFlags);

#ifdef _WIN64
	#pragma comment(linker, "/export:AbilitaCIE")
#else
	#pragma comment(linker, "/export:AbilitaCIE=_AbilitaCIE@16")
#endif

struct threadData {
	HWND *progWin;
	HDESK hDesk;
};

DWORD WINAPI _abilitaCIE(
	LPVOID lpThreadParameter) {
	init_CSP_func
	const char *PAN = (char *)lpThreadParameter;

	std::string container ("CIE-");
	container += PAN;

	try {

		CSHA256 sha256;
		std::map<uint8_t, ByteDynArray> hashSet;
		uint8_t* data;
		DWORD len = 0;
		ByteDynArray CertCIE;
		ByteDynArray SOD;
		ByteDynArray IdServizi;
		std::unique_ptr<safeDesktop> desk;

		SCARDCONTEXT hSC;

		SCardEstablishContext(SCARD_SCOPE_SYSTEM, nullptr, nullptr, &hSC);
		char *readers = nullptr;
		len = SCARD_AUTOALLOCATE;
		if (SCardListReaders(hSC, nullptr, (char*)&readers, &len) != SCARD_S_SUCCESS || readers==nullptr) {
			CMessage msg(MB_OK,
				"Abilitazione CIE",
				"Nessun lettore di smartcard installato");
			msg.DoModal();
			return 0;
		}

		char *curreader = readers;
		bool foundCIE = false;
		for (; curreader[0] != 0; curreader += strnlen(curreader, len) + 1) {
			CARD_DATA cData;
			ZeroMem(cData);
			cData.dwVersion = 7;
			cData.hSCardCtx = hSC;
			{
				safeConnection conn(hSC, curreader, SCARD_SHARE_SHARED);
				if (conn.hCard == NULL)
					continue;

				{
					safeTransaction checkTran(conn, SCARD_LEAVE_CARD);
					if (!checkTran.isLocked())
						continue;

					len = SCARD_AUTOALLOCATE;
					cData.hScard = conn;
					SCardGetAttrib(cData.hScard, SCARD_ATTR_ATR_STRING, (BYTE*)&cData.pbAtr, &len);
					cData.pfnCspAlloc = (PFN_CSP_ALLOC)CryptMemAlloc;
					cData.pfnCspReAlloc = (PFN_CSP_REALLOC)CryptMemRealloc;
					cData.pfnCspFree = (PFN_CSP_FREE)CryptMemFree;
					cData.cbAtr = len;
					cData.pwszCardName = L"CIE";
					auto isCIE = CardAcquireContext(&cData, 0);
					SCardFreeMemory(cData.hScard, cData.pbAtr);
					if (isCIE != 0)
						continue;					
				}

				foundCIE = true;
				if (!desk)
					desk.reset(new safeDesktop("AbilitaCIE"));

				CMessage msg(MB_OKCANCEL, "Abilitazione CIE",
					"Premere OK per effettuare la verifica di autenticit�",
					"e abilitare l'uso della CIE su questo PC");

				if (msg.DoModal() == IDOK) {
					
					CPin pin(8, "Inserire le 8 cifre del PIN della CIE", "Non verranno mai richieste in seguito tutte le 8 cifre del PIN,", "ma solo le ultime 4", "Abilitazione CIE");
					if (pin.DoModal() == IDOK) {
						try {

							safeTransaction Tran(conn, SCARD_LEAVE_CARD);
							if (!Tran.isLocked())
								continue;

							len = 0;
							DWORD ris;
							if ((ris = CardReadFile(&cData, DirCIE, EfIdServizi, 0, &data, &len)) != SCARD_S_SUCCESS)
								throw windows_error(ris);
							IdServizi = ByteArray(data, len);
							cData.pfnCspFree(data);
							hashSet[0xa1] = sha256.Digest(IdServizi.left(12));

							len = 0;
							if ((ris = CardReadFile(&cData, DirCIE, EfSOD, 0, &data, &len)) != SCARD_S_SUCCESS)
								throw windows_error(ris);
							SOD = ByteArray(data, len);
							cData.pfnCspFree(data);

							ByteDynArray IntAuth; len = 0;
							if ((ris = CardReadFile(&cData, DirCIE, EfIntAuth, 0, &data, &len)) != SCARD_S_SUCCESS)
								throw windows_error(ris);
							IntAuth = ByteArray(data, len);
							cData.pfnCspFree(data);
							hashSet[0xa4] = sha256.Digest(IntAuth.left(GetASN1DataLenght(IntAuth)));

							ByteDynArray IntAuthServizi; len = 0;
							if ((ris = CardReadFile(&cData, DirCIE, EfIntAuthServizi, 0, &data, &len)) != SCARD_S_SUCCESS)
								throw windows_error(ris);
							IntAuthServizi = ByteArray(data, len);
							cData.pfnCspFree(data);
							hashSet[0xa5] = sha256.Digest(IntAuthServizi.left(GetASN1DataLenght(IntAuthServizi)));

							ByteDynArray DH; len = 0;
							if ((ris = CardReadFile(&cData, DirCIE, EfDH, 0, &data, &len)) != SCARD_S_SUCCESS)
								throw windows_error(ris);
							DH = ByteArray(data, len);
							cData.pfnCspFree(data);

							hashSet[0x1b] = sha256.Digest(DH.left(GetASN1DataLenght(DH)));
							auto ias = ((IAS*)cData.pvVendorSpecific);
							if (IdServizi != ByteArray((uint8_t*)PAN, strnlen(PAN, 20)))
								continue;

							//DWORD id;
							HWND progWin = nullptr;
							struct threadData th;
							th.progWin = &progWin;
							th.hDesk = (HDESK)(*desk);
							std::thread([&th]() -> DWORD {
								SetThreadDesktop(th.hDesk);
								CVerifica ver(th.progWin);
								ver.DoModal();
								return 0;
							}).detach();

							ias->Callback = [](int prog, char *desc, void *data) {
								HWND progWin = *(HWND*)data;
								if (progWin != nullptr)
									SendMessage(progWin, WM_COMMAND, 100 + prog, (LPARAM)desc);
							};
							ias->CallbackData = &progWin;

							DWORD attempts = -1;

							DWORD rs = CardAuthenticateEx(&cData, ROLE_USER, FULL_PIN, (BYTE*)pin.PIN, (DWORD)strnlen(pin.PIN, sizeof(pin.PIN)), nullptr, 0, &attempts);
							if (rs == SCARD_W_WRONG_CHV) {
								if (progWin != nullptr)
									SendMessage(progWin, WM_COMMAND, 100 + 7, (LPARAM)"");
								std::string num;
								if (attempts > 0)
									num = "Sono rimasti " + std::to_string(attempts ) + " tentativi prima del blocco";
								else
									num = "";
								CMessage msg(MB_OK,
									"Abilitazione CIE",
									"PIN Errato",
									num.c_str());
								msg.DoModal();
								break;
							}
							else if (rs == SCARD_W_CHV_BLOCKED) {
								if (progWin != nullptr)
									SendMessage(progWin, WM_COMMAND, 100 + 7, (LPARAM)"");
								CMessage msg(MB_OK,
									"Abilitazione CIE",
									"Il PIN � bloccato. Pu� esere sbloccato verificando il PUK");
								msg.DoModal();
								break;
							}
							else if (rs != SCARD_S_SUCCESS)
								throw logged_error("Autenticazione fallita");

							if (progWin != nullptr)
								SendMessage(progWin, WM_COMMAND, 100 + 4, (LPARAM)"Lettura certificato");

							ByteDynArray Serial; len = 0;

							if ((ris = CardReadFile(&cData, DirCIE, EfSerial, 0, &data, &len)) != SCARD_S_SUCCESS)
								throw windows_error(ris);
							Serial = ByteArray(data, len);
							cData.pfnCspFree(data);
							hashSet[0xa2] = sha256.Digest(Serial.left(9));
							len = 0;
							if ((ris = CardReadFile(&cData, DirCIE, EfCertCIE, 0, &data, &len)) != SCARD_S_SUCCESS)
								throw windows_error(ris);

//							len = GetASN1DataLenght(CertCIE);
							//unsigned int actualLength = data[2] * 256 + data[3] + 4;

//							Log.write("cert actual length: %d", len);

							CertCIE = ByteArray(data, len);

							cData.pfnCspFree(data);
							hashSet[0xa3] = sha256.Digest(CertCIE.left(GetASN1DataLenght(CertCIE)));

							if (progWin != nullptr)
								SendMessage(progWin, WM_COMMAND, 100 + 5, (LPARAM)"Verifica SOD");
							ias->VerificaSOD(SOD, hashSet);

							if (progWin != nullptr)
								SendMessage(progWin, WM_COMMAND, 100 + 6, (LPARAM)"Cifratura dati");
							ias->SetCache(PAN, CertCIE, ByteArray((uint8_t*)pin.PIN, 4));
							if (progWin != nullptr)
								SendMessage(progWin, WM_COMMAND, 100 + 7, (LPARAM)"");

							Tran.unlock();

							CMessage msg(MB_OK,
								"Abilitazione CIE",
								"La CIE � abilitata all'uso");
							msg.DoModal();
						}
						catch (std::exception &ex) {
							//std::string dump;
							LOG_ERROR("_abilitaCIE - Error: %s", ex.what());
							OutputDebugString(ex.what());
							CMessage msg(MB_OK,
								"Abilitazione CIE",
								"Si � verificato un errore nella verifica di",
								"autenticit� del documento");

							msg.DoModal();
							break;
						}
					}
					break;
				}
			}
		}
		if (!foundCIE) {
			if (!desk)
				desk.reset(new safeDesktop("AbilitaCIE"));
			std::string num(PAN);
			num+=" nei lettori di smart card";
			CMessage msg(MB_OK,
				"Abilitazione CIE",
				"Impossibile trovare la CIE con Numero Identificativo",
				num.c_str());
			msg.DoModal();
		}
		SCardFreeMemory(hSC, readers);
	}
	catch (std::exception &ex) {
		LOG_ERROR("_abilitaCIE - error EXCLOG %s", ex.what());

		OutputDebugString(ex.what());
		MessageBox(nullptr, "Si � verificato un errore nella verifica di autenticit� del documento", "CIE", MB_OK);
	}

	return SCARD_S_SUCCESS;
	exit_CSP_func
	return E_UNEXPECTED;
}

#pragma data_seg("Shared")
int g_working = 0;
#pragma data_seg()

#pragma comment(linker, "/section:Shared,RWS")


void TrayNotificationAbilitaCIE(CSystemTray* tray, WPARAM uID, LPARAM lEvent) 
{
	if (lEvent == WM_LBUTTONUP || lEvent == 0x405) {

		tray->HideIcon();

		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		ZeroMem(si);
		si.cb = sizeof(STARTUPINFO);

		char szProgramFilesDir[MAX_PATH];
		if (!SHGetSpecialFolderPath(NULL, szProgramFilesDir, CSIDL_PROGRAM_FILESX86, 0))
			SHGetSpecialFolderPath(NULL, szProgramFilesDir, CSIDL_PROGRAM_FILES, 0);

		LOG_INFO("TrayNotificationAbilitaCIE - szProgramFilesDir %s", szProgramFilesDir);

		if (!CreateProcess(NULL, (char*)std::string(szProgramFilesDir).append("\\CIEPKI\\CIEID").c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			DWORD dwerr = GetLastError();
			LOG_ERROR("TrayNotificationAbilitaCIE - error run CIEID %x", dwerr);

			throw logged_error("Errore in creazione processo CIEID");
		}
		else
		{
			throw logged_error("Errore in creazione processo CIEID");
		}

		//std::thread thread(_sbloccoPIN, GetCurrentThreadId());
		//thread.detach();
		//tray->HideIcon();
	}
}

extern "C" int CALLBACK AbilitaCIE(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	if (g_working)
		return 0;

	g_working = 1;

	LOG_INFO("AbilitaCIE - Running AbilitaCIE Process");

	if (_AtlWinModule.cbSize != sizeof(_ATL_WIN_MODULE)) {
		_AtlWinModule.cbSize = sizeof(_ATL_WIN_MODULE);
		AtlWinModuleInit(&_AtlWinModule);
	}

	WNDCLASS wndClass;
	GetClassInfo(NULL, WC_DIALOG, &wndClass);
	wndClass.hInstance = (HINSTANCE)moduleInfo.getModule();
	wndClass.style |= CS_DROPSHADOW;
	wndClass.lpszClassName = "CIEDialog";
	RegisterClass(&wndClass);

	
	CSystemTray tray(wndClass.hInstance, nullptr, WM_APP, "La CIE non � abilitata. Premere per abilitarla ora",
		LoadIcon(wndClass.hInstance, MAKEINTRESOURCE(IDI_CIE)), 1);
	tray.ShowBalloon("Premere per abilitare la CIE", "CIE", NIIF_INFO);
	tray.ShowIcon();
	tray.TrayNotification = TrayNotificationAbilitaCIE;
	
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		if (Msg.message == WM_COMMAND) 
		{
			ODS("WMCOMMAND");
			if (Msg.wParam == 0)
				return 0;
			else 
			{
				tray.ShowIcon();
				ODS("Show Baloon");
				tray.ShowBalloon("Premere per abilitare la CIE", "CIE", NIIF_INFO, 30);
			}
		}
		DispatchMessage(&Msg);
	}

	return 0;
}


	//if (_AtlWinModule.cbSize != sizeof(_ATL_WIN_MODULE)) {
	//	_AtlWinModule.cbSize = sizeof(_ATL_WIN_MODULE);
	//	AtlWinModuleInit(&_AtlWinModule);
	//}
	//WNDCLASS wndClass;
	//GetClassInfo(NULL, WC_DIALOG, &wndClass);
	//wndClass.hInstance = (HINSTANCE)moduleInfo.getModule();
	//wndClass.style |= CS_DROPSHADOW;
	//wndClass.lpszClassName = "CIEDialog";
	//RegisterClass(&wndClass);

	//ODS("Start AbilitaCIE");
	//if (!CheckOneInstance("CIEAbilitaOnce")) {
	//	ODS("Already running AbilitaCIE");
	//	return 0;
	//}
	////std::thread thread(_abilitaCIE, lpCmdLine);
	////thread.join();
	//_abilitaCIE(lpCmdLine);
	//ODS("End AbilitaCIE");
//	return 0;
//}
