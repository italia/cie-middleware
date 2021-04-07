#include "stdafx.h"
#include "IAS.h"
#include "../crypto/ASNParser.h"
#include "../crypto/RSA.h"
#include "../crypto/AES.h"
#include "../crypto/SHA256.h"
#include "../crypto/SHA512.h"
#include "../crypto/SHA1.h"
#include "../crypto/DES3.h"
#include "../crypto/MAC.h"
#include <shlwapi.h>
#include <shlobj.h>
#include "ModuleInfo.h"
//#include "resource.h"
#include "cacheLib.h"
#include <intsafe.h>
#include <wincrypt.h>

#define CIE_KEY_DH_ID 0x81
#define CIE_KEY_ExtAuth_ID 0x84
#define CIE_PIN_ID 0x81
#define CIE_PUK_ID 0x82
#define CIE_KEY_Sign_ID 0x81

CModuleInfo moduleInfo;

extern ByteArray SkipZero(ByteArray &ba);
extern DWORD WINAPI _abilitaCIE(LPVOID lpThreadParameter);


IAS::IAS(CToken::TokenTransmitCallback transmit,ByteArray ATR)
{
	init_func

	Callback = nullptr;
	this->ATR = ATR;
	uint8_t gemaltoAID[] = { 0xA0, 0x00, 0x00, 0x00, 0x30, 0x80, 0x00, 0x00, 0x00, 0x09, 0x81, 0x60, 0x01 };
	IAS_AID = VarToByteArray(gemaltoAID);

	uint8_t AID[] = { 0xA0, 0x00, 0x00, 0x00, 0x00, 0x39 };
	CIE_AID = VarToByteArray(AID);
	
	ActiveSM = false;
	ActiveDF = DF_Root;

	token.setTransmitCallback(transmit, nullptr);
}


IAS::~IAS()
{
}

uint8_t defModule[] = { 0xba, 0x28, 0x37, 0xab, 0x4c, 0x6b, 0xb8, 0x27, 0x57, 0x7b, 0xff, 0x4e, 0xb7, 0xb1, 0xe4, 0x9c, 0xdd, 0xe0, 0xf1, 0x66, 0x14, 0xd1, 0xef, 0x24, 0xc1, 0xb7, 0x5c, 0xf7, 0x0f, 0xb1, 0x2c, 0xd1, 0x8f, 0x4d, 0x14, 0xe2, 0x81, 0x4b, 0xa4, 0x87, 0x7e, 0xa8, 0x00, 0xe1, 0x75, 0x90, 0x60, 0x76, 0xb5, 0x62, 0xba, 0x53, 0x59, 0x73, 0xc5, 0xd8, 0xb3, 0x78, 0x05, 0x1d, 0x8a, 0xfc, 0x74, 0x07, 0xa1, 0xd9, 0x19, 0x52, 0x9e, 0x03, 0xc1, 0x06, 0xcd, 0xa1, 0x8d, 0x69, 0x9a, 0xfb, 0x0d, 0x8a, 0xb4, 0xfd, 0xdd, 0x9d, 0xc7, 0x19, 0x15, 0x9a, 0x50, 0xde, 0x94, 0x68, 0xf0, 0x2a, 0xb1, 0x03, 0xe2, 0x82, 0xa5, 0x0e, 0x71, 0x6e, 0xc2, 0x3c, 0xda, 0x5b, 0xfc, 0x4a, 0x23, 0x2b, 0x09, 0xa4, 0xb2, 0xc7, 0x07, 0x45, 0x93, 0x95, 0x49, 0x09, 0x9b, 0x44, 0x83, 0xcb, 0xae, 0x62, 0xd0, 0x09, 0x96, 0x74, 0xdb, 0xf6, 0xf3, 0x9b, 0x72, 0x23, 0xa9, 0x9d, 0x88, 0xe3, 0x3f, 0x1a, 0x0c, 0xde, 0xde, 0xeb, 0xbd, 0xc3, 0x55, 0x17, 0xab, 0xe9, 0x88, 0x0a, 0xab, 0x24, 0x0e, 0x1e, 0xa1, 0x66, 0x28, 0x3a, 0x27, 0x4a, 0x9a, 0xd9, 0x3b, 0x4b, 0x1d, 0x19, 0xf3, 0x67, 0x9f, 0x3e, 0x8b, 0x5f, 0xf6, 0xa1, 0xe0, 0xed, 0x73, 0x6e, 0x84, 0xd5, 0xab, 0xe0, 0x3c, 0x59, 0xe7, 0x34, 0x6b, 0x42, 0x18, 0x75, 0x5d, 0x75, 0x36, 0x6c, 0xbf, 0x41, 0x36, 0xf0, 0xa2, 0x6c, 0x3d, 0xc7, 0x0a, 0x69, 0xab, 0xaa, 0xf6, 0x6e, 0x13, 0xa1, 0xb2, 0xfa, 0xad, 0x05, 0x2c, 0xa6, 0xec, 0x9c, 0x51, 0xe2, 0xae, 0xd1, 0x4d, 0x16, 0xe0, 0x90, 0x25, 0x4d, 0xc3, 0xf6, 0x4e, 0xa2, 0xbd, 0x8a, 0x83, 0x6b, 0xba, 0x99, 0xde, 0xfa, 0xcb, 0xa3, 0xa6, 0x13, 0xae, 0xed, 0xd9, 0x3a, 0x96, 0x15, 0x27, 0x3d };
uint8_t defPrivExp[] = { 0x47, 0x16, 0xc2, 0xa3, 0x8c, 0xcc, 0x7a, 0x07, 0xb4, 0x15, 0xeb, 0x1a, 0x61, 0x75, 0xf2, 0xaa, 0xa0, 0xe4, 0x9c, 0xea, 0xf1, 0xba, 0x75, 0xcb, 0xa0, 0x9a, 0x68, 0x4b, 0x04, 0xd8, 0x11, 0x18, 0x79, 0xd3, 0xe2, 0xcc, 0xd8, 0xb9, 0x4d, 0x3c, 0x5c, 0xf6, 0xc5, 0x57, 0x53, 0xf0, 0xed, 0x95, 0x87, 0x91, 0x0b, 0x3c, 0x77, 0x25, 0x8a, 0x01, 0x46, 0x0f, 0xe8, 0x4c, 0x2e, 0xde, 0x57, 0x64, 0xee, 0xbe, 0x9c, 0x37, 0xfb, 0x95, 0xcd, 0x69, 0xce, 0xaf, 0x09, 0xf4, 0xb1, 0x35, 0x7c, 0x27, 0x63, 0x14, 0xab, 0x43, 0xec, 0x5b, 0x3c, 0xef, 0xb0, 0x40, 0x3f, 0x86, 0x8f, 0x68, 0x8e, 0x2e, 0xc0, 0x9a, 0x49, 0x73, 0xe9, 0x87, 0x75, 0x6f, 0x8d, 0xa7, 0xa1, 0x01, 0xa2, 0xca, 0x75, 0xa5, 0x4a, 0x8c, 0x4c, 0xcf, 0x9a, 0x1b, 0x61, 0x47, 0xe4, 0xde, 0x56, 0x42, 0x3a, 0xf7, 0x0b, 0x20, 0x67, 0x17, 0x9c, 0x5e, 0xeb, 0x64, 0x68, 0x67, 0x86, 0x34, 0x78, 0xd7, 0x52, 0xc7, 0xf4, 0x12, 0xdb, 0x27, 0x75, 0x41, 0x57, 0x5a, 0xa0, 0x61, 0x9d, 0x30, 0xbc, 0xcc, 0x8d, 0x87, 0xe6, 0x17, 0x0b, 0x33, 0x43, 0x9a, 0x2c, 0x93, 0xf2, 0xd9, 0x7e, 0x18, 0xc0, 0xa8, 0x23, 0x43, 0xa6, 0x01, 0x2a, 0x5b, 0xb1, 0x82, 0x28, 0x08, 0xf0, 0x1b, 0x5c, 0xfd, 0x85, 0x67, 0x3a, 0xc0, 0x96, 0x4c, 0x5f, 0x3c, 0xfd, 0x2d, 0xaf, 0x81, 0x42, 0x35, 0x97, 0x64, 0xa9, 0xad, 0xb9, 0xe3, 0xf7, 0x6d, 0xb6, 0x13, 0x46, 0x1c, 0x1b, 0xc9, 0x13, 0xdc, 0x9a, 0xc0, 0xab, 0x50, 0xd3, 0x65, 0xf7, 0x7c, 0xb9, 0x31, 0x94, 0xc9, 0x8a, 0xa9, 0x66, 0xd8, 0x9c, 0xdd, 0x55, 0x51, 0x25, 0xa5, 0xe5, 0x9e, 0xcf, 0x4f, 0xa3, 0xf0, 0xc3, 0xfd, 0x61, 0x0c, 0xd3, 0xd0, 0x56, 0x43, 0x93, 0x38, 0xfd, 0x81 };
uint8_t defPubExp[] = { 0x00, 0x01, 0x00, 0x01 };

void IAS::ReadSOD(ByteDynArray &data) {
	init_func
	readfile(0x1006,data);
	exit_func
}
void IAS::ReadDH(ByteDynArray &data) {
	init_func
	readfile(0xd004, data);
	exit_func
}
void IAS::ReadCertCIE(ByteDynArray &data) {
	init_func
	readfile(0x1003, data);
	exit_func
}
void IAS::ReadServiziPubKey(ByteDynArray &data) {
	init_func
	readfile(0x1005, data);
	exit_func
}
void IAS::ReadSerialeCIE(ByteDynArray &data) {
	init_func
	readfile(0x1002, data);
	exit_func
}
void IAS::ReadIdServizi(ByteDynArray &data) {
	init_func
	readfile(0x1001, data);
	exit_func
}

void IAS::Sign(ByteArray &data, ByteDynArray &signedData) {
	init_func
	ByteDynArray resp;
	uint8_t SetKey[] = { 0x00, 0x22, 0x41, 0xA4 };
	uint8_t val02 = 2;
	uint8_t keyId = CIE_KEY_Sign_ID;
	StatusWord sw;
	if ((sw = SendAPDU_SM(VarToByteArray(SetKey), ASN1Tag(0x80, VarToByteArray(val02)).append(ASN1Tag(0x84, VarToByteArray(keyId))), resp)) != 0x9000)
		throw scard_error(sw);
	
	uint8_t Sign[] = { 0x00, 0x88, 0x00, 0x00 };
	if ((sw = SendAPDU_SM(VarToByteArray(Sign), data, signedData)) != 0x9000)
		throw scard_error(sw);
}

StatusWord IAS::VerifyPUK(ByteArray &PIN) {
	init_func
	ByteDynArray resp;
	uint8_t verifyPIN[] = { 0x00, 0x20, 0x00, CIE_PUK_ID };
	return SendAPDU_SM(VarToByteArray(verifyPIN), PIN, resp);
}

StatusWord IAS::VerifyPIN(ByteArray &PIN) {
	init_func
	ByteDynArray resp;
	uint8_t verifyPIN[] = { 0x00, 0x20, 0x00, CIE_PIN_ID };
	return SendAPDU_SM(VarToByteArray(verifyPIN), PIN, resp);
	exit_func
}

StatusWord IAS::UnblockPIN() {
	init_func
	ByteDynArray resp;
	uint8_t UnblockPIN[] = { 0x00, 0x2C, 0x03, CIE_PIN_ID };
	return SendAPDU_SM(VarToByteArray(UnblockPIN), ByteArray(), resp);
	exit_func
}

StatusWord IAS::ChangePIN(ByteArray &oldPIN,ByteArray &newPIN) {
	init_func
	ByteDynArray resp;
	ByteDynArray data=oldPIN;
	data.append(newPIN);
	uint8_t ChangePIN[] = { 0x00, 0x24, 0x00, CIE_PIN_ID };
	return SendAPDU_SM(VarToByteArray(ChangePIN), data, resp);
	exit_func
}

StatusWord IAS::ChangePIN(ByteArray &newPIN) {
	init_func
	ByteDynArray resp;
	uint8_t ChangePIN[] = { 0x00, 0x2C, 0x02, CIE_PIN_ID };
	return SendAPDU_SM(VarToByteArray(ChangePIN), newPIN, resp);
	exit_func
}

void IAS::readfile(WORD id, ByteDynArray &content){
	init_func

	if (ActiveSM)
		return readfile_SM(id, content);

	ByteDynArray resp;
	uint8_t selectFile[] = { 0x00, 0xa4, 0x02, 0x04 };
	uint8_t fileId[] = { HIBYTE(id), LOBYTE(id) };
	StatusWord sw;
	if ((sw = SendAPDU(VarToByteArray(selectFile), VarToByteArray(fileId), resp)) != 0x9000)
	throw scard_error(sw);


	WORD cnt = 0;
	uint8_t chunk = 128;
	while (true) {
		ByteDynArray chn;
		uint8_t readFile[] = { 0x00, 0xb0, HIBYTE(cnt), LOBYTE(cnt) };
		sw = SendAPDU(VarToByteArray(readFile), ByteArray(), chn, &chunk);
		if ((sw >> 8) == 0x6c)  {
			uint8_t le = sw & 0xff;
			sw = SendAPDU(VarToByteArray(readFile), ByteArray(), chn, &le);
		}
		if (sw == 0x9000) {
			content.append(chn);
			WORD chnSize;
			if (FAILED(SizeTToWord(chn.size(), &chnSize)) || FAILED(WordAdd(cnt, chnSize, &cnt)))
				throw logged_error("File troppo grande");
			chunk = 128;
		}
		else {
			if (sw == 0x6282)
				content.append(chn);
			else if (sw != 0x6b00)
				throw scard_error(sw);
			break;
		}
	}
	exit_func
}

void IAS::readfile_SM(WORD id, ByteDynArray &content) {
	init_func

	ByteDynArray resp;
	uint8_t selectFile[] = { 0x00, 0xa4, 0x02, 0x04 };
	uint8_t fileId[] = { HIBYTE(id), LOBYTE(id) };
	StatusWord sw;
	if ((sw = SendAPDU_SM(VarToByteArray(selectFile), VarToByteArray(fileId), resp)) != 0x9000)
	throw scard_error(sw);


	WORD cnt = 0;
	uint8_t chunk = 128;
	while (true) {
		ByteDynArray chn;
		uint8_t readFile[] = { 0x00, 0xb0, HIBYTE(cnt), LOBYTE(cnt) };
		sw = SendAPDU_SM(VarToByteArray(readFile), ByteArray(), chn, &chunk);
		if ((sw >> 8) == 0x6c)  {
			uint8_t le = sw & 0xff;
			sw = SendAPDU_SM(VarToByteArray(readFile), ByteArray(), chn, &le);
		}
		if (sw == 0x9000) {
			content.append(chn);
			WORD chnSize;
			if (FAILED(SizeTToWord(chn.size(), &chnSize)) || FAILED(WordAdd(cnt, chnSize, &cnt)))
				throw logged_error("File troppo grande");
			chunk = 128;
		}
		else {
			if (sw == 0x6282) 
				content.append(chn);
			else if (sw != 0x6b00)
				throw scard_error(sw);
			break;
		}
	}
	exit_func
}

void IAS::SelectAID_CIE(bool SM) {
	init_func
	ByteDynArray resp;
	uint8_t selectCIE[] = { 0x00, 0xa4, 0x04, 0x0c };
	ByteDynArray selectCIEapdu;
	StatusWord sw;
	if (SM) {
		if ((sw = SendAPDU_SM(VarToByteArray(selectCIE), CIE_AID, resp)) != 0x9000)
		throw scard_error(sw);
	}
	else
	{
		if ((sw = SendAPDU(VarToByteArray(selectCIE), CIE_AID, resp)) != 0x9000)
		throw scard_error(sw);
	}
	ActiveDF = DF_CIE;
	ActiveSM = false;
	exit_func
}

uint8_t NXP_ATR[] = { 0x80, 0x31, 0x80, 0x65, 0x49, 0x54, 0x4E, 0x58, 0x50 };
uint8_t Gemalto_ATR[] = { 0x80, 0x31, 0x80, 0x65, 0xB0, 0x85, 0x04, 0x00, 0x11 };
uint8_t Gemalto2_ATR[] = { 0x80, 0x31, 0x80, 0x65, 0xB0, 0x85, 0x03, 0x00, 0xEF };
uint8_t STM_ATR[] = {0x80, 0x66, 0x47, 0x50, 0x00, 0xB8, 0x00, 0x7F};
uint8_t STM2_ATR[] = { 0x80, 0x80, 0x01, 0x01 };

ByteArray baNXP_ATR(NXP_ATR, sizeof(NXP_ATR));
ByteArray baGemalto_ATR(Gemalto_ATR, sizeof(Gemalto_ATR));
ByteArray baGemalto2_ATR(Gemalto2_ATR, sizeof(Gemalto2_ATR));
ByteArray baSTM_ATR(STM_ATR, sizeof(STM_ATR));
ByteArray baSTM2_ATR(STM2_ATR, sizeof(STM2_ATR));

void IAS::ReadCIEType() {
	init_func
		size_t position;
	if (ATR.indexOf(baNXP_ATR, position))
		type = CIE_Type::CIE_NXP;
	else if (ATR.indexOf(baGemalto_ATR, position))
		type = CIE_Type::CIE_Gemalto;
	else if (ATR.indexOf(baGemalto2_ATR, position))
		type = CIE_Type::CIE_Gemalto;
	else if (ATR.indexOf(baSTM_ATR, position))
		type = CIE_Type::CIE_STM;
	else if (ATR.indexOf(baSTM2_ATR, position))
		type = CIE_Type::CIE_STM2;
	else
		throw logged_error("CIE non riconosciuta");
}


void IAS::SelectAID_IAS(bool SM) {
	init_func
	if (type == CIE_Type::CIE_Unknown) {
		ReadCIEType();
	}
	ByteDynArray resp;
	StatusWord sw;
	if (type == CIE_Type::CIE_NXP) {
		uint8_t selectMF[] = { 0x00, 0xa4, 0x00, 0x00 };
		if (SM)
		{
			if ((sw = SendAPDU_SM(VarToByteArray(selectMF), ByteArray(), resp)) != 0x9000)
				throw scard_error(sw);
		}
		else
		{
			if ((sw = SendAPDU(VarToByteArray(selectMF), ByteArray(), resp)) != 0x9000)
				throw scard_error(sw);
		}
	}
	else 	
		if (type == CIE_Type::CIE_Gemalto || type == CIE_Type::CIE_STM || type == CIE_Type::CIE_STM2) {
			uint8_t selectIAS[] = { 0x00, 0xa4, 0x04, 0x0c };
			if (SM)
			{
				if ((sw = SendAPDU_SM(VarToByteArray(selectIAS), IAS_AID, resp)) != 0x9000)
					throw scard_error(sw);
			}
			else
			{
				if ((sw = SendAPDU(VarToByteArray(selectIAS), IAS_AID, resp)) != 0x9000)
					throw scard_error(sw);
			}
		}
		else
		{
			throw logged_error("Tipo CIE sconosciuto");
		}
	
	SM = false;
	ActiveDF = DF_IAS;
	ActiveSM = false;
	exit_func
}

void IAS::ReadDappPubKey(ByteDynArray &DappKey) {
	init_func
	ByteDynArray resp;
	readfile(0x1004, DappKey);

	CASNParser parser;
	parser.Parse(DappKey);
	Log.write("parsing ok");

	ByteArray module = parser.tags[0]->tags[0]->content;
	while (module[0] == 0)
		module = module.mid(1);
	DappModule = module;
	ByteArray pubKey = parser.tags[0]->tags[1]->content;
	while (pubKey[0] == 0)
		pubKey = pubKey.mid(1);
	DappPubKey = pubKey;

	Log.write("exit readdapppubkey");

	exit_func
}

void IAS::ReadPAN() {
	init_func
	readfile(0xd003, PAN);
	//Log.writeBinData(PAN.data(), PAN.size());
	exit_func
}

void IAS::DAPP() {
	init_func

	ByteDynArray resp;
	uint8_t psoVerifyAlgo = 0x41;
	uint8_t PKdScheme = 0x9B;
	uint8_t shaOID = 0x04;
	DWORD shaSize = 32;
	CSHA256 sha256;
	uint8_t Val01 = 1;

	if (DappPubKey.isEmpty()) {
		throw logged_error("La chiave DAPP non e' diponibile");
	}

	ByteDynArray module = VarToByteArray(defModule);
	ByteDynArray pubexp = VarToByteArray(defPubExp);
	ByteDynArray privexp = VarToByteArray(defPrivExp);

	ByteDynArray cert;

	ByteDynArray CHR, CHA, OID;


	uint8_t snIFD[] = { 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
	uint8_t CPI=0x8A;
	uint8_t baseCHR[] = { 0x00, 0x00, 0x00, 0x00 };
	CHR.set(&VarToByteArray(baseCHR), &VarToByteArray(snIFD));
	CHA.set(&CA_AID,01);
	uint8_t baseOID[] = { 0x2A, 0x81, 0x22, 0xF4, 0x2A, 0x02, 0x04, 0x01 };
	OID.set(&VarToByteArray(baseOID), shaOID);

	ByteDynArray endEntityCert;
	endEntityCert.set(CPI, &CA_CAR, &CHR, &CHA, &OID, &module, &pubexp);

	ByteDynArray certSign, toSign;
	uint8_t ValBC = 0xBC;
	toSign.set(0x6A, &(endEntityCert.left(CA_module.size() - shaSize - 2)), &(sha256.Digest(endEntityCert)), 0xbc);
	CRSA caKey(CA_module, CA_privexp);
	certSign = caKey.RSA_PURE(toSign);
	ByteDynArray certVerif;
	CRSA caPubKey(CA_module, CA_pubexp);
	certVerif = caPubKey.RSA_PURE(certSign);
	ER_ASSERT(certVerif == toSign, "Errore in verifica firma!")

	ByteDynArray PkRem;
	PkRem = endEntityCert.mid(CA_module.size() - shaSize - 2);

	cert.setASN1Tag(0x7F21, ASN1Tag(0x5F37, certSign).append(ASN1Tag(0x5F38, PkRem)).append(ASN1Tag(0x42, CA_CAR)));

	uint8_t SelectKey[] = { 0x00, 0x22, 0x81, 0xb6 };
	uint8_t id = CIE_KEY_ExtAuth_ID;
	StatusWord sw;

	uint8_t le = 0;
	if ((sw = SendAPDU_SM(VarToByteArray(SelectKey), ASN1Tag(0x80, VarToByteArray(psoVerifyAlgo)).append(ASN1Tag(0x83, VarToByteArray(id))), resp, &le)) != 0x9000)
		throw scard_error(sw);

	uint8_t VerifyCert[] = { 0x00, 0x2A, 0x00, 0xAE };
	if ((sw = SendAPDU_SM(VarToByteArray(VerifyCert), cert, resp)) != 0x9000)
		throw scard_error(sw);

	uint8_t SetCHR[] = { 0x00, 0x22, 0x81, 0xA4 };
	if ((sw = SendAPDU_SM(VarToByteArray(SetCHR), ASN1Tag(0x83, CHR), resp)) != 0x9000)
		throw scard_error(sw);

	ByteDynArray challenge;
	uint8_t GetChallenge[] = { 0x00, 0x84, 0x00, 0x00 };
	uint8_t chLen = 8;

	if ((sw = SendAPDU_SM(VarToByteArray(GetChallenge), ByteArray(), challenge, &chLen)) != 0x9000)
	throw scard_error(sw);

	ByteDynArray toHash;
	size_t padSize = module.size() - shaSize - 2;
	ByteDynArray PRND(padSize);
	PRND.random();
	toHash.set(&PRND, &dh_pubKey, &VarToByteArray(snIFD), &challenge, &dh_ICCpubKey, &dh_g, &dh_p, &dh_q);
	
	toSign.set(0x6a, &PRND, &(sha256.Digest(toHash)), 0xBC);
	
	CRSA certKey(module, privexp);
	ByteDynArray signResp = certKey.RSA_PURE(toSign);
	ByteDynArray chResponse;
	chResponse.set(&VarToByteArray(snIFD), &signResp);

	uint8_t ExtAuth[] = { 0x00, 0x82, 0x00, 0x00 };
	if ((sw = SendAPDU_SM(VarToByteArray(ExtAuth), chResponse, resp)) != 0x9000)
	throw scard_error(sw);


	uint8_t IntAuth[] = { 0x00, 0x22, 0x41, 0xa4 };
	uint8_t Val82 = 0x82;
	if ((sw = SendAPDU_SM(VarToByteArray(IntAuth), ASN1Tag(0x84, VarToByteArray(Val82)).append(ASN1Tag(0x80, VarToByteArray(PKdScheme))), resp)) != 0x9000)
	throw scard_error(sw);

	ByteDynArray rndIFD(8);
	rndIFD.random();
	uint8_t GiveRandom[] = { 0x00, 0x88, 0x00, 0x00 };
	if ((sw = SendAPDU_SM(VarToByteArray(GiveRandom), rndIFD, resp, 0)) != 0x9000)
	throw scard_error(sw);

	ByteDynArray SN_ICC = resp.mid(0, 8);
	
	CRSA intAuthKey(DappModule, DappPubKey);
	ByteDynArray intAuthResp = intAuthKey.RSA_PURE(resp.mid(8));
	ER_ASSERT(intAuthResp[0] == 0x6a, "Errore nell'autenticazione del chip");
	ByteArray PRND2 = intAuthResp.mid(1, intAuthResp.size() - 32 - 2);
	ByteArray hashICC = intAuthResp.mid(PRND2.size() + 1, 32);

	ByteDynArray toHashIFD;
	toHashIFD.set(&PRND2, &dh_ICCpubKey, &SN_ICC, &rndIFD, &dh_pubKey, &dh_g, &dh_p, &dh_q);
	ByteDynArray calcHashIFD = sha256.Digest(toHashIFD);
	ER_ASSERT(calcHashIFD == hashICC, "Errore nell'autenticazione del chip")
	ER_ASSERT(intAuthResp.right(1)[0] == 0xbc, "Errore nell'autenticazione del chip");

	sessSSC.set(&(challenge.right(4)), &(rndIFD.right(4)));
	ActiveSM = true;
	exit_func
}

void IAS::DHKeyExchange() {
	init_func
	CASNParser asn1;

	ByteDynArray dh_prKey, secret, resp,d1;
	do {
		dh_prKey.resize(dh_q.size());
		dh_prKey.random();
	} while (dh_q[0] < dh_prKey[0]);

	// dh_prKey deve essere dispari
	dh_prKey.right(1)[0] |= 1;

	ByteDynArray dhg(dh_g.size());
	dhg.fill(0);
	dhg.rightcopy(dh_g);
	CRSA rsa(dh_p, dh_prKey);
	dh_pubKey = rsa.RSA_PURE(dhg);

	uint8_t algo = 0x9b;
	uint8_t keyId = CIE_KEY_DH_ID;
	d1.setASN1Tag(0x80, VarToByteArray(algo)).append(ASN1Tag(0x83, VarToByteArray(keyId))).append(ASN1Tag(0x91, dh_pubKey));

	uint8_t MSE_SET[] = { 0x00, 0x22, 0x41, 0xa6 };
	StatusWord sw;
	if ((sw = SendAPDU(VarToByteArray(MSE_SET), d1, resp)) != 0x9000)
		throw scard_error(sw);
	uint8_t GET_DATA[] = { 0x00, 0xcb, 0x3f, 0xff };
	uint8_t GET_DATA_Data[] = { 0x4d, 0x04, 0xa6, 0x02, 0x91, 0x00 };
	if ((sw = SendAPDU(VarToByteArray(GET_DATA), VarToByteArray(GET_DATA_Data), resp)) != 0x9000)
	throw scard_error(sw);

	asn1.Parse(resp);
	dh_ICCpubKey = asn1.tags[0]->tags[0]->content;

	secret = rsa.RSA_PURE(dh_ICCpubKey);

	CSHA256 sha256;
	
	uint8_t diffENC[] = { 0x00, 0x00, 0x00, 0x01 };
	uint8_t diffMAC[] = { 0x00, 0x00, 0x00, 0x02 };
	sessENC = sha256.Digest(ByteDynArray(secret).append(VarToByteArray(diffENC))).left(16);
	sessMAC = sha256.Digest(ByteDynArray(secret).append(VarToByteArray(diffMAC))).left(16);
	sessSSC.resize(8); 
	sessSSC.fill(0);
	sessSSC[7] = 1;

	ActiveSM = true;
	exit_func
}

void IAS::increment(ByteArray &seq) {
	for (size_t i = seq.size() - 1; i >= 0; i--) {
		if (seq[i] < 255) {
			seq[i]++;
			for (size_t j = i + 1; j < seq.size(); j++)
				seq[j] = 0;
			return;
		}
	}
}


ByteDynArray IAS::SM(ByteArray &keyEnc, ByteArray &keySig, ByteArray &apdu, ByteArray &seq) {
	init_func

	std::string dmp;
	ODS(dumpHexData(seq, dmp).c_str());

	increment(seq);
	ByteDynArray smHead;
	smHead = apdu.left(4);
	smHead[0] |= 0x0C;
	auto calcMac = ISOPad(ByteDynArray(seq).append(smHead));
	ByteDynArray iv(8);
	iv.fill(0); // IV for APDU encryption and signature should be 0. Please refer to IAS specification §7.1.9 Secure messaging – Command APDU protection
	CDES3 encDes(keyEnc, iv);
	CMAC sigMac(keySig, iv);
	uint8_t Val01 = 1;
	uint8_t Val00 = 0;

	ByteDynArray datafield, doob;
	if (apdu[4] != 0 && apdu.size() > 5) {
		
		ByteDynArray enc = encDes.RawEncode(ISOPad(apdu.mid(5, apdu[4])));
		if ((apdu[1] & 1) == 0)
			doob.setASN1Tag(0x87, VarToByteDynArray(Val01).append(enc));
		else
			doob.setASN1Tag(0x85, enc);

		calcMac.append(doob);
		datafield.append(doob);
	}
	if (apdu[4] == 0 && apdu.size() > 7) {

		ByteDynArray enc = encDes.RawEncode(ISOPad(apdu.mid(7, (apdu[5] << 8)| apdu[6] )));
		if ((apdu[1] & 1) == 0)
			doob.setASN1Tag(0x87, VarToByteDynArray(Val01).append(enc));
		else
			doob.setASN1Tag(0x85, enc);

		calcMac.append(doob);
		datafield.append(doob);
	}
	if (apdu.size() == 5 || apdu.size() == (apdu[4] + 6)) {
		uint8_t le = apdu[apdu.size() - 1];
		doob.setASN1Tag(0x97, VarToByteArray(le));
		calcMac.append(doob);
		datafield.append(doob);
	}
	datafield.append(ASN1Tag(0x8e, sigMac.Mac(ISOPad(calcMac))));
	
	ByteDynArray elabResp;
	if (datafield.size()<0x100)
		elabResp.set(&smHead, (uint8_t)datafield.size(), &datafield, (uint8_t)0x00);
	else {
		auto len = datafield.size();
		auto lenBA = VarToByteArray(len);
		elabResp.set(&smHead, &(lenBA.reverse().right(3)), &datafield, (uint8_t)0x00, (uint8_t)0x00);
	}
	return elabResp;
}

StatusWord IAS::respSM(ByteArray &keyEnc, ByteArray &keySig, ByteArray &resp, ByteArray &seq, ByteDynArray &elabResp) {
	init_func

	increment(seq);
	DWORD index, llen, lgn; 
	StatusWord sw = 0xffff;
	ByteDynArray encData;
	ByteDynArray respMac, calcMac;
	ByteDynArray iv(8);
	iv.fill(0); // IV for APDU encryption should be 0. Please refer to IAS specification §7.1.9 Secure messaging – Command APDU protection
	CDES3 encDes(keyEnc, iv);
	CMAC sigMac(keySig, iv);

	calcMac = seq;
	index = 0;
	do {

		if (resp[index] == 0x99) {
			calcMac.append(resp.mid(index, resp[index + 1] + 2));
			sw = resp[index + 2] << 8 | resp[index + 3];
			index += 4;
		}
		else if (resp[index] == 0x8e) {
			if (resp[index + 1] != 0x08)
				throw logged_error("Lunghezza del MAC non valida");
			respMac = resp.mid(index + 2, 8);
			index += 10;
		}
		else if (resp[index] == 0x85) {
			if (resp[index + 1] > 0x80) {
				llen = resp[index + 1] - 0x80;
				if (llen == 1) 
					lgn = resp[index + 2];
				else 
					if (llen == 2) 
						lgn = (resp[index + 2] << 8) | resp[index + 3];
					else 
						throw logged_error(stdPrintf("Lunghezza ASN1 non valida: %i", llen));
				encData = resp.mid(index + llen + 2, lgn);
				calcMac.append(resp.mid(index, lgn + llen + 2));
				index += llen + lgn + 2;
			}
			else {
				encData = resp.mid(index + 2, resp[index + 1]);
				calcMac.append(resp.mid(index, resp[index + 1] + 2));
				index += resp[index + 1] + 2;
			}
		}
		else if (resp[index] == 0x87) {
			if (resp[index + 1] > 0x80) {
				llen = resp[index + 1] - 0x80;
				if (llen == 1) 
					lgn = resp[index + 2];
				else 
					if (llen == 2) 
						lgn = (resp[index + 2] << 8) | resp[index + 3];
					else
						throw logged_error(stdPrintf("Lunghezza ASN1 non valida: %i", llen));
				encData = resp.mid(index + llen + 3, lgn - 1);
				calcMac.append(resp.mid(index, lgn + llen + 2));
				index += llen + lgn + 2;
			}
			else {
				encData = resp.mid(index + 3, resp[index + 1] - 1);
				calcMac.append(resp.mid(index, resp[index + 1] + 2));
				index += resp[index + 1] + 2;
			}
		}
		else
			throw logged_error("Tag non riconosciuto nella risposta in Secure Messaging");
	} while (index < resp.size());

	auto smMac = sigMac.Mac(ISOPad(calcMac));
	ER_ASSERT(smMac == respMac,"Errore nel checksum della risposta del chip")

	if (!encData.isEmpty()) {
		elabResp=encDes.RawDecode(encData);
		elabResp.resize(RemoveISOPad(elabResp),true);
	}
	else
		elabResp.clear();

	return sw;
}

StatusWord IAS::getResp(ByteDynArray &resp, StatusWord sw,ByteDynArray &elabresp) {
	init_func
	elabresp.clear();
	if (resp.size() != 0)
		elabresp.append(resp);
		
	ByteDynArray curresp;
	while (true) {
		if ((sw >> 8) == 0x61) {
			uint8_t ln = sw & 0xff;
			if (ln != 0) {
				uint8_t apdu[] = { 0x00, 0xc0, 0x00, 0x00, ln };
				sw = token.Transmit(VarToByteArray(apdu), &curresp);
				elabresp.append(curresp);
				return sw;
			}
			else {
				uint8_t apdu[] = { 0x00, 0xc0, 0x00, 0x00, 0x00 };
				sw = token.Transmit(VarToByteArray(apdu), &curresp);
				elabresp.append(curresp);
			}
		}
		else {
			return sw;
		}
	}
	exit_func
}

StatusWord IAS::getResp_SM(ByteArray &resp, StatusWord sw, ByteDynArray &elabresp) {
	init_func

	
	ByteDynArray s, ap;
	CASNParser p;
	elabresp.clear();
	if (resp.size() != 0)
		elabresp.append(resp);

	ByteDynArray curresp;
	while (true) {
		if ((sw >> 8) == 0x61) {
			uint8_t ln = sw & 0xff;
			if (ln != 0) {
				uint8_t apdu[] = { 0x00, 0xc0, 0x00, 0x00, ln };
				sw = token.Transmit(VarToByteArray(apdu), &curresp);
				elabresp.append(curresp);
				if (sw == 0x9000)
					break;
				if ((sw >> 8) != 0x61)
					throw scard_error(sw);
			}
			else {
				uint8_t apdu[] = { 0x0c, 0xc0, 0x00, 0x00, 0x00 };
				sw = token.Transmit(VarToByteArray(apdu), &curresp);
				elabresp.append(curresp);
			}
		}
		else  if (sw == 0x9000 || sw == 0x6b00 || sw==0x6282)
			break;
		else
			return sw;
	}
	return respSM(sessENC, sessMAC, elabresp, sessSSC, elabresp);
	exit_func
}


StatusWord IAS::SendAPDU_SM(ByteArray &head, ByteArray &data, ByteDynArray &resp, uint8_t *le) {
	init_func
	ByteDynArray smApdu;
	ByteDynArray s, curresp;
	StatusWord sw;
	if (data.size() < 0xE7) {
		smApdu.set(&head, (uint8_t)data.size(), &data, le == nullptr ? &(ByteArray()) : &(VarToByteArray(*le)));

		ODS(std::string().append("Clear APDU:").append(dumpHexData(smApdu, std::string())).append("\n").c_str());
		smApdu = SM(sessENC, sessMAC, smApdu, sessSSC);
		sw = token.Transmit(smApdu, &curresp);				
		sw = getResp_SM(curresp, sw, resp);


		ODS(std::string().append("Clear RESP:").append(dumpHexData(resp, std::string())).append(HexByte(sw >> 8)).append(HexByte(sw & 0xff)).append("\n").c_str());
		return sw;
	}
	else {
		// attenzione:
		// in alcuni casi la carta ritorna 61xx fra un comando e l'altro in chaining. Questo è un grosso problema, perchè
		// la get response sembra che faccia saltare il chaining. Forse è una questione di driver del lettore?
		// Per daesso l'ho osservato solo su una virtual machine Win7 con il lettore in sharing con l'host
		

		size_t ds = data.size();
		size_t i = 0;
		uint8_t cla = head[0];
		while (true) {
			s = data.mid(i, min(0xE7, data.size() - i));
			i += s.size();
			if (i != data.size())
				head[0] = cla | 0x10;
			else
				head[0] = cla;
			if (s.size() != 0)
				smApdu.set(&head, (BYTE)s.size(), &s, (le == nullptr || i < data.size()) ? &(ByteArray()) : &(VarToByteArray(*le)));
			else
				smApdu.set(&head, (le == nullptr || i < data.size()) ? &(ByteArray()) : &(VarToByteArray(*le)));

			ODS(std::string("Clear APDU:").append(dumpHexData(smApdu, std::string())).append("\n").c_str());
			smApdu = SM(sessENC, sessMAC, smApdu, sessSSC);
			sw = token.Transmit(smApdu, &curresp);
			sw = getResp_SM(curresp, sw, resp);

			ODS(std::string("Clear RESP:").append(dumpHexData(resp, std::string())).append(HexByte(sw >> 8)).append(HexByte(sw & 0xff)).append("\n").c_str());
			if (i == data.size())
				return sw;
		}

	}
	exit_func
}


StatusWord IAS::SendAPDU(ByteArray &head, ByteArray &data, ByteDynArray &resp, uint8_t *le) {
	init_func

	ByteDynArray apdu, curresp;
	auto ds = data.size();

	if (ds > 255) {
		size_t i = 0;
		uint8_t cla = head[0];
		while (true) {
			auto s = data.mid(i, min(data.size()-i,255));
			i += s.size();
			if (i != data.size())
				head[0] = (cla | 0x10);
			else
				head[0] = cla;

			apdu.set(&head, (BYTE)s.size(), &s, le == nullptr ? &(ByteArray()) : &(VarToByteArray(*le)));
			
			StatusWord sw=token.Transmit(apdu, &curresp);
			if (i == data.size()) {
				sw = getResp(curresp, sw, resp);

				return sw;
			}
		}
	}
	else {
		if (data.size()!=0)
			apdu.set(&head, (BYTE)data.size(), &data, le == nullptr ? &(ByteArray()) : &(VarToByteArray(*le)));
		else
			apdu.set(&head, le == nullptr ? &(ByteArray()) : &(VarToByteArray(*le)));

		StatusWord sw = token.Transmit(apdu, &curresp);
		sw=getResp(curresp, sw, resp);

		return sw;
	}
	exit_func
}


void IAS::InitDHParam() {
	init_func
	ByteDynArray resp;

	CASNParser parser;
	StatusWord sw;

	if (type == CIE_Type::CIE_Gemalto) {
		uint8_t getDHDoup[] = { 00, 0xcb, 0x3f, 0xff };
		uint8_t getDHDuopData[] = { 0x4d, 0x08, 0x70, 0x06, 0xBF, 0xA1, 0x01, 0x02, 0xA3, 0x80 };

		if ((sw = SendAPDU(VarToByteArray(getDHDoup), VarToByteArray(getDHDuopData), resp)) != 0x9000)
		throw scard_error(sw);

		parser.Parse(resp);

		dh_g = parser.tags[0]->tags[0]->tags[0]->tags[0]->content;
		dh_p = parser.tags[0]->tags[0]->tags[0]->tags[1]->content;
		dh_q = parser.tags[0]->tags[0]->tags[0]->tags[2]->content;
	}
	else if (type == CIE_Type::CIE_NXP || type == CIE_Type::CIE_STM || type == CIE_Type::CIE_STM2) {
		uint8_t getDHDoup[] = { 00, 0xcb, 0x3f, 0xff };
		uint8_t getDHDuopData_g[] = { 0x4D, 0x0A, 0x70, 0x08, 0xBF, 0xA1, 0x01, 0x04, 0xA3, 0x02, 0x97, 0x00 };

		if ((sw = SendAPDU(VarToByteArray(getDHDoup), VarToByteArray(getDHDuopData_g), resp)) != 0x9000)
		throw scard_error(sw);
		parser.Parse(resp);
		dh_g = parser.tags[0]->tags[0]->tags[0]->tags[0]->content;

		uint8_t getDHDuopData_p[] = { 0x4D, 0x0A, 0x70, 0x08, 0xBF, 0xA1, 0x01, 0x04, 0xA3, 0x02, 0x98, 0x00 };
		if ((sw = SendAPDU(VarToByteArray(getDHDoup), VarToByteArray(getDHDuopData_p), resp)) != 0x9000)
		throw scard_error(sw);
		parser.Parse(resp);
		dh_p = parser.tags[0]->tags[0]->tags[0]->tags[0]->content;

		uint8_t getDHDuopData_q[] = { 0x4D, 0x0A, 0x70, 0x08, 0xBF, 0xA1, 0x01, 0x04, 0xA3, 0x02, 0x99, 0x00 };
		if ((sw = SendAPDU(VarToByteArray(getDHDoup), VarToByteArray(getDHDuopData_q), resp)) != 0x9000)
		throw scard_error(sw);
		parser.Parse(resp);
		dh_q = parser.tags[0]->tags[0]->tags[0]->tags[0]->content;
	}
	else 
		throw logged_error("CIE non riconosciuta");


	exit_func
}

CASNTag *GetTag(CASNTagArray &tags, DWORD id) {
	for (std::size_t i = 0; i < tags.size(); i++) {
		if (tags[i]->tagInt() == id)
			return tags[i].get();
	}
	return nullptr;
}

void IAS::InitExtAuthKeyParam() {
	init_func
	ByteDynArray resp;

	uint8_t getKeyDoup[] = { 00, 0xcb, 0x3f, 0xff };
	uint8_t getKeyDuopData[] = { 0x4d, 0x09, 0x70, 0x07, 0xBF, 0xA0, CIE_KEY_ExtAuth_ID & 0x7f, 0x03, 0x7F, 0x49, 0x80 };
	StatusWord sw;
	if ((sw = SendAPDU(VarToByteArray(getKeyDoup), VarToByteArray(getKeyDuopData), resp)) != 0x9000)
	throw scard_error(sw);

	CASNParser parser;
	parser.Parse(resp);

	CA_module = GetTag(parser.tags[0]->tags[0]->tags[0]->tags,0x81)->content;
	CA_pubexp = GetTag(parser.tags[0]->tags[0]->tags[0]->tags, 0x82)->content;	
	CA_privexp = baExtAuth_PrivExp;
	CA_CHR = GetTag(parser.tags[0]->tags[0]->tags[0]->tags, 0x5F20)->content;
	CA_CHA = GetTag(parser.tags[0]->tags[0]->tags[0]->tags, 0x5F4C)->content;
	CA_CAR = CA_CHR.mid(4);
	CA_AID = CA_CHA.left(6);
}

void IAS::SetCardContext(void* pCardData) {
	token.setTransmitCallbackData(pCardData);
}

void IAS::Deauthenticate() {
	init_func
		token.Reset(true);
}

extern uint8_t encMod[];
extern uint8_t encPriv[];
extern uint8_t encPub[];

void IAS::InitEncKey() {
	// uso la chiave di intAuth per i servizi per decifrare il certificato
	init_func
	std::string strPAN;
	dumpHexData(PAN.mid(5, 6), strPAN, false);

	uint8_t mseSet[] = { 0x00, 0x22, 0x41, 0xa4 };
	uint8_t mseSetData[] = { 0x80, 0x01, 0x02, 0x84, 0x01, 0x83 };
	ByteDynArray resp;
	StatusWord sw;
	if (sessSSC.isEmpty()) {
		if ((sw = SendAPDU(VarToByteArray(mseSet), VarToByteArray(mseSetData), resp)) != 0x9000)
		throw scard_error(sw);
		uint8_t intAuth[] = { 0x00, 0x88, 0x00, 0x00 };
		if ((sw = SendAPDU(VarToByteArray(intAuth), ByteArray((BYTE*)strPAN.c_str(), strPAN.length()), resp)) != 0x9000)
		throw scard_error(sw);
	}
	else {
		if ((sw = SendAPDU_SM(VarToByteArray(mseSet), VarToByteArray(mseSetData), resp)) != 0x9000)
		throw scard_error(sw);
		uint8_t intAuth[] = { 0x00, 0x88, 0x00, 0x00 };
		if ((sw = SendAPDU_SM(VarToByteArray(intAuth), ByteArray((BYTE*)strPAN.c_str(), strPAN.length()), resp)) != 0x9000)
		throw scard_error(sw);
	}

	CSHA512 sha512;
	auto cardSeed = sha512.Digest(resp);
	CardEncKey = cardSeed.left(32);
	CardEncIv= cardSeed.mid(32).left(16);
}

void IAS::SetCache(const char *PAN, ByteArray &certificate, ByteArray &FirstPIN) {
	init_func
	ByteDynArray encCert, encPIN;
	CAES enc(CardEncKey, CardEncIv);
	encCert=enc.Encode(certificate);
	encPIN=enc.Encode(FirstPIN);
	CacheSetData(PAN, encCert.data(), (int)encCert.size(), encPIN.data(), (int)encPIN.size());
}

int integrity = 0;
bool IsLowIntegrity()
{
	if (integrity == 0) {

		HANDLE hToken;
		HANDLE hProcess;

		DWORD dwLengthNeeded;
		DWORD dwError = ERROR_SUCCESS;

		PTOKEN_MANDATORY_LABEL pTIL = NULL;
		DWORD dwIntegrityLevel;

		hProcess = GetCurrentProcess();
		if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
		{
			if (!GetTokenInformation(hToken, TokenIntegrityLevel,
				nullptr, 0, &dwLengthNeeded))
			{
				dwError = GetLastError();
				if (dwError == ERROR_INSUFFICIENT_BUFFER)
				{
					pTIL = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0,
						dwLengthNeeded);
					if (pTIL != NULL)
					{
						if (GetTokenInformation(hToken, TokenIntegrityLevel,
							pTIL, dwLengthNeeded, &dwLengthNeeded))
						{
							dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid,
								(DWORD)(UCHAR)(*GetSidSubAuthorityCount(pTIL->Label.Sid) - 1));

							if (dwIntegrityLevel == SECURITY_MANDATORY_LOW_RID)
								integrity = 1;
							else if (dwIntegrityLevel >= SECURITY_MANDATORY_MEDIUM_RID &&
								dwIntegrityLevel < SECURITY_MANDATORY_HIGH_RID)
								integrity = 2;
							else if (dwIntegrityLevel >= SECURITY_MANDATORY_HIGH_RID)
								integrity = 3;
							else if (dwIntegrityLevel >= SECURITY_MANDATORY_SYSTEM_RID)
								integrity = 4;
						}
						LocalFree(pTIL);
					}
				}
			}
			CloseHandle(hToken);
		}
	}
	return 
		integrity == 1;
}

bool IsUserInteractive()
{
	BOOL bIsUserInteractive = TRUE;

	HWINSTA hWinStation = GetProcessWindowStation();
	if (hWinStation != NULL)
	{
		USEROBJECTFLAGS uof = { 0 };
		if (GetUserObjectInformation(hWinStation, UOI_FLAGS, &uof, sizeof(USEROBJECTFLAGS), NULL) && ((uof.dwFlags & WSF_VISIBLE) == 0))
		{
			bIsUserInteractive = FALSE;
		}
	}
	return bIsUserInteractive == TRUE;
}

bool IAS::IsImpersonationRunning() {
	HANDLE token = NULL;
	OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, FALSE, &token);
	if (token != NULL) {
		CloseHandle(token);
		return true;
	}
	return false;
}

void IAS::IconaCertificatoScaduto(const char *seriale) {
	init_func
		if (IsUserInteractive()) {
			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			ZeroMem(si);
			si.cb = sizeof(STARTUPINFO);

			char runDll32Path[MAX_PATH];
			GetSystemDirectory(runDll32Path, MAX_PATH);
			strcat_s(runDll32Path, "\\");
			strcat_s(runDll32Path, "rundll32.exe");

			if (!IsImpersonationRunning()) {
				if (CreateProcess(runDll32Path, (char*)std::string("rundll32.exe \"").append(moduleInfo.szModuleFullPath).append("\",CertificatoScaduto ").append(seriale).c_str(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
				}
			}
		}
}
	

void IAS::IconaSbloccoPIN() {
	init_func
		if (IsUserInteractive()) {
			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			ZeroMem(si);
			si.cb = sizeof(STARTUPINFO);

			WORD getHandle = 0xfffd;
			ByteDynArray resp;
			token.Transmit(VarToByteArray(getHandle), &resp);
			SCARDHANDLE hCard = *(SCARDHANDLE*)resp.data();

			char runDll32Path[MAX_PATH];
			GetSystemDirectory(runDll32Path, MAX_PATH);
			strcat_s(runDll32Path, "\\");
			strcat_s(runDll32Path, "rundll32.exe");

			// check impersonation
			HANDLE token = NULL;
			OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, FALSE, &token);

			if (token == NULL) {
				if (CreateProcess(runDll32Path, (char*)std::string("rundll32.exe \"").append(moduleInfo.szModuleFullPath).append("\",SbloccoPIN ICON").c_str(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
				}
			}
			else {
				CloseHandle(token);
			}
		}
}

void IAS::GetFirstPIN(ByteDynArray &PIN) {
	init_func
		std::string PANStr;
	dumpHexData(PAN.mid(5, 6), PANStr, false);
	std::vector<BYTE> EncPINBuf;
	CacheGetPIN(PANStr.c_str(), EncPINBuf);

	CAES enc(CardEncKey, CardEncIv);
	PIN = enc.Decode(ByteArray(EncPINBuf.data(), EncPINBuf.size()));
}

bool IAS::IsEnrolled() {
	init_func
		std::string PANStr;
	dumpHexData(PAN.mid(5, 6), PANStr, false);
	return CacheExists(PANStr.c_str());
}

bool IAS::Unenroll() {
	init_func
		std::string PANStr;
	dumpHexData(PAN.mid(5, 6), PANStr, false);
	return CacheRemove(PANStr.c_str());
}

bool IAS::IsEnrolled(const char* szPAN) {
	init_func

		return CacheExists(szPAN);
}

bool IAS::Unenroll(const char* szPAN) {
	init_func
		return CacheRemove(szPAN);
}


void IAS::GetCertificate(ByteDynArray &certificate,bool askEnable) {
	init_func
		if (!Certificate.isEmpty()) {
		certificate = Certificate;
		return;
	}

	std::string PANStr;
	dumpHexData(PAN.mid(5, 6), PANStr, false);
	if (!CacheExists(PANStr.c_str())) {
		Log.writePure("Cache non esiste");
		Log.writePure("askEnabled %d", askEnable);
		Log.writePure("IsUserInteractive() %d", IsUserInteractive());
		Log.writePure("IsLowIntegrity() %d", IsLowIntegrity());

		if (askEnable /*&& IsUserInteractive()*/ && !IsLowIntegrity()) {
			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			ZeroMem(si);
			si.cb = sizeof(STARTUPINFO);

			WORD getHandle = 0xfffd;
			ByteDynArray resp;
			token.Transmit(VarToByteArray(getHandle), &resp);
			SCARDHANDLE hCard = *(SCARDHANDLE*)resp.data();

			SCardEndTransaction(hCard, SCARD_UNPOWER_CARD);

			// check impersonation
			HANDLE token = NULL;
			OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, FALSE, &token);

			Log.writePure("token %d", token);

			if (token == NULL) {
				char runDll32Path[MAX_PATH];
				GetSystemDirectory(runDll32Path, MAX_PATH);
				strcat_s(runDll32Path, "\\");
				strcat_s(runDll32Path, "rundll32.exe");

				Log.writePure("run AbilitaCIE");

				if (!CreateProcess(runDll32Path, (char*)std::string("rundll32.exe \"").append(moduleInfo.szModuleFullPath).append("\",AbilitaCIE ").append(dumpHexData(PAN.mid(5, 6), std::string(), false)).c_str(), NULL, NULL, FALSE, 0, nullptr, nullptr, &si, &pi))
				{
					Log.writePure("Errore in creazione processo AbilitaCIE");
					throw logged_error("Errore in creazione processo AbilitaCIE");
				}
				else
				{
					Log.writePure("AbilitaCIE creato");
					CloseHandle(pi.hThread);
				}
				//WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
			}
			else {
				CloseHandle(token);
			}
			SCardBeginTransaction(hCard);
		}
		else {
			certificate.clear();
			return;
		}
	}
	if (!CacheExists(PANStr.c_str()))
		throw logged_error("Errore in abilitazione CIE");
	std::vector<BYTE> certEncBuf;
	CacheGetCertificate(PANStr.c_str(), certEncBuf);

	CAES enc(CardEncKey, CardEncIv);
	certificate = enc.Decode(ByteArray(certEncBuf.data(), certEncBuf.size()));
	Certificate = certificate;
}


uint8_t IAS::GetSODDigestAlg(ByteArray &SOD)
{
	CASNParser parser;
	uint8_t OID_SHA512[] = { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03 };
	uint8_t OID_SHA256[] = { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01 };

	parser.Parse(SOD);

	std::string dump;
	dumpHexData(SOD, dump);

	CASNTag &SODTag = *parser.tags[0];

	CASNTag &temp = SODTag.Child(0, 0x30);

	CASNTag &temp2 = temp.Child(1, 0xA0).Child(0, 0x30);

	auto &digestAlgo = temp2.Child(1, 0x31).Child(0, 0x30).Child(0, 6).content;

	if (digestAlgo == VarToByteArray(OID_SHA256))
	{
		return 1;
	}
	else if (digestAlgo == VarToByteArray(OID_SHA512))
	{
		return 2;
	}
	else throw logged_error("Algoritmo di digest del SOD non supportato");;
}

void IAS::VerificaSODPSS(ByteArray &SOD, std::map<uint8_t, ByteDynArray> &hashSet)
{
	init_func
	CASNParser parser;
	parser.Parse(SOD);

	std::string dump;
	dumpHexData(SOD, dump);

	CASNTag &SODTag = *parser.tags[0];

	CASNTag &temp = SODTag.Child(0, 0x30);
	
	/* Verifica OID contentInfo */
	uint8_t OID[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x02 };
	temp.Child(0, 06).Verify(VarToByteArray(OID));

	uint8_t val3 = 3;
	CASNTag &temp2 = temp.Child(1, 0xA0).Child(0, 0x30);
	temp2.Child(0, 2).Verify(VarToByteArray(val3));

	uint8_t OID_SHA512[] = { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03 };
	temp2.Child(1, 0x31).Child(0, 0x30).Child(0, 6).Verify(VarToByteArray(OID_SHA512));

	uint8_t OID3[] = { 0x67, 0x81, 0x08, 0x01, 0x01, 0x01 };
	temp2.Child(2, 0x30).Child(0, 06).Verify(VarToByteArray(OID3));

	/* Prendo gli sha512 dei DG letti */
	ByteArray ttData = temp2.Child(2, 0x30).Child(1, 0xA0).Child(0, 04).content;

	CASNParser ttParser;
	ttParser.Parse(ttData);
	CASNTag &signedData = *ttParser.tags[0];
	signedData.CheckTag(0x30);

	/* Prendo il certificato del signer */
	CASNTag &signerCert = temp2.Child(3, 0xA0).Child(0, 0x30);

	CASNTag &temp3 = temp2.Child(4, 0x31).Child(0, 0x30);
	uint8_t val1 = 1;
	temp3.Child(0, 02).Verify(VarToByteArray(val1));

	CASNTag &issuerName = temp3.Child(1, 0x30).Child(0, 0x30);
	CASNTag &signerCertSerialNumber = temp3.Child(1, 0x30).Child(1, 02);

	temp3.Child(2, 0x30).Child(0, 06).Verify(VarToByteArray(OID_SHA512));

	CASNTag &signerInfo = temp3.Child(3, 0xA0);

	uint8_t OID4[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x03 };
	signerInfo.Child(0, 0x30).Child(0, 06).Verify(VarToByteArray(OID4));

	uint8_t OID5[] = { 0x67, 0x81, 0x08, 0x01, 0x01, 0x01 };
	signerInfo.Child(0, 0x30).Child(1, 0x31).Child(0, 06).Verify(VarToByteArray(OID5));

	uint8_t OID6[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x04 };
	signerInfo.Child(1, 0x30).Child(0, 06).Verify(VarToByteArray(OID6));

	CASNTag &digest = temp3.Child(3, 0xA0).Child(1, 0x30).Child(1, 0x31).Child(0, 04);
	ByteArray digest_ = SOD.mid((int)digest.startPos, (int)(digest.endPos - digest.startPos));

	uint8_t OID_RSAPSS[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0A };
	auto &signAlgo = temp3.Child(4, 0x30).Child(0, 06).content;
	auto &digestAlgo = temp3.Child(4, 0x30).Child(1, 0x30).Child(0, 0xA0).Child(0, 0x30).Child(0, 06).content;

	if (digestAlgo != VarToByteArray(OID_SHA512))
		throw logged_error("Algoritmo del digest della firma non valido");
	if(signAlgo != VarToByteArray(OID_RSAPSS))
		throw logged_error("Algoritmo di firma non valido");

	CSHA512 sha512;
	ByteDynArray calcDigest_ = sha512.Digest(ttData.mid((int)signedData.startPos, (int)(signedData.endPos - signedData.startPos)));

	if (calcDigest_ != digest.content)
		throw logged_error("Digest del SOD non corrispondente ai dati");

	CASNTag &signature = temp3.Child(5, 04);

	ByteArray certRaw = SOD.mid((int)signerCert.startPos, (int)(signerCert.endPos - signerCert.startPos));
	PCCERT_CONTEXT certDS = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certRaw.data(), (DWORD)certRaw.size());
	if (certDS == nullptr)
		throw logged_error("Certificato DS non valido");

	auto _1 = scopeExit([&]() noexcept {CertFreeCertificateContext(certDS); });

	ByteArray pubKeyData(certDS->pCertInfo->SubjectPublicKeyInfo.PublicKey.pbData, certDS->pCertInfo->SubjectPublicKeyInfo.PublicKey.cbData);
	CASNParser pubKeyParser;
	pubKeyParser.Parse(pubKeyData);
	CASNTag &pubKey = *pubKeyParser.tags[0];
	CASNTag &modTag = pubKey.Child(0, 02);
	ByteArray mod = modTag.content;
	while (mod[0] == 0)
		mod = mod.mid(1);
	CASNTag &expTag = pubKey.Child(1, 02);
	ByteArray exp = expTag.content;
	while (exp[0] == 0)
		exp = exp.mid(1);

	ByteArray signatureData = signature.content;

	CRSA rsa(mod, exp);

	ByteArray toSign = SOD.mid((int)signerInfo.tags[0]->startPos, (int)(signerInfo.tags[signerInfo.tags.size() - 1]->endPos - signerInfo.tags[0]->startPos));
	
	bool result = rsa.RSA_PSS(signatureData, toSign.getASN1Tag(0x31));

	if (!result)
	{
		throw logged_error("Firma del SOD non valida");
	}

	issuerName.Reparse();
	CASNParser issuerParser;
	issuerParser.Parse(ByteArray(certDS->pCertInfo->Issuer.pbData, certDS->pCertInfo->Issuer.cbData));
	CASNTag &CertIssuer = *issuerParser.tags[0];
	if (issuerName.tags.size() != CertIssuer.tags.size())
		throw logged_error("Issuer name non corrispondente");
	for (std::size_t i = 0; i < issuerName.tags.size(); i++) {
		CASNTag &certElem = *CertIssuer.tags[i]->tags[0];
		CASNTag &SODElem = *issuerName.tags[i]->tags[0];
		certElem.tags[0]->Verify(SODElem.tags[0]->content);
		certElem.tags[1]->Verify(SODElem.tags[1]->content);
	}

	ByteDynArray certSerial = ByteArray(certDS->pCertInfo->SerialNumber.pbData, certDS->pCertInfo->SerialNumber.cbData);
	if (certSerial.reverse() != signerCertSerialNumber.content)
		throw logged_error("Serial Number del certificato non corrispondente");

	uint8_t val = 1;
	signedData.Child(0, 02).Verify(VarToByteArray(val));
	signedData.Child(1, 0x30).Child(0, 06).Verify(VarToByteArray(OID_SHA512));

	CASNTag &hashTag = signedData.Child(2, 0x30);
	for (std::size_t i = 0; i < hashTag.tags.size(); i++) {
		CASNTag &hashDG = *(hashTag.tags[i]);
		CASNTag &dgNum = hashDG.CheckTag(0x30).Child(0, 02);
		CASNTag &dgHash = hashDG.Child(1, 04);
		uint8_t num = ByteArrayToVar(dgNum.content, BYTE);

		if (hashSet.find(num) == hashSet.end() || hashSet[num].size() == 0)
			throw logged_error(stdPrintf("Digest non trovato per il DG %02X", num));

		if (hashSet[num] != dgHash.content)
			throw logged_error(stdPrintf("Digest non corrispondente per il DG %02X", num));
	}

	exit_func
}

void IAS::VerificaSOD(ByteArray &SOD, std::map<BYTE, ByteDynArray> &hashSet) {
	init_func
		CASNParser parser;
	parser.Parse(SOD);

	std::string dump;
	dumpHexData(SOD, dump);

	CASNTag &SODTag = *parser.tags[0];

	CASNTag &temp = SODTag.Child(0, 0x30);
	uint8_t OID[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x02 };
	temp.Child(0, 06).Verify(VarToByteArray(OID));

	uint8_t val3 = 3;
	CASNTag &temp2 = temp.Child(1, 0xA0).Child(0, 0x30);
	temp2.Child(0, 2).Verify(VarToByteArray(val3));

	uint8_t OID_SH256[] = { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01 };
	temp2.Child(1, 0x31).Child(0, 0x30).Child(0, 6).Verify(VarToByteArray(OID_SH256));

	uint8_t OID3[] = { 0x67, 0x81, 0x08, 0x01, 0x01, 0x01 };
	temp2.Child(2, 0x30).Child(0, 06).Verify(VarToByteArray(OID3));

	ByteArray ttData = temp2.Child(2, 0x30).Child(1, 0xA0).Child(0, 04).content;

	CASNParser ttParser;
	ttParser.Parse(ttData);
	CASNTag &signedData = *ttParser.tags[0];
	signedData.CheckTag(0x30);

	CASNTag &signerCert = temp2.Child(3, 0xA0).Child(0, 0x30);

	CASNTag &temp3 = temp2.Child(4, 0x31).Child(0, 0x30);
	uint8_t val1 = 1;
	temp3.Child(0, 02).Verify(VarToByteArray(val1));

	CASNTag &issuerName = temp3.Child(1, 0x30).Child(0, 0x30);
	CASNTag &signerCertSerialNumber = temp3.Child(1, 0x30).Child(1, 02);
	temp3.Child(2, 0x30).Child(0, 06).Verify(VarToByteArray(OID_SH256));

	CASNTag &signerInfo = temp3.Child(3, 0xA0);

	uint8_t OID4[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x03 };
	signerInfo.Child(0, 0x30).Child(0, 06).Verify(VarToByteArray(OID4));

	uint8_t OID5[] = { 0x67, 0x81, 0x08, 0x01, 0x01, 0x01 };
	signerInfo.Child(0, 0x30).Child(1, 0x31).Child(0, 06).Verify(VarToByteArray(OID5));

	uint8_t OID6[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x04 };
	signerInfo.Child(1, 0x30).Child(0, 06).Verify(VarToByteArray(OID6));

	CASNTag &digest = temp3.Child(3, 0xA0).Child(1, 0x30).Child(1, 0x31).Child(0, 04);

	uint8_t OID_RSAwithSHA256[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0b };
	uint8_t OID_RSAwithSHA1[] = { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05 };
	auto &digestAlgo = temp3.Child(4, 0x30).Child(0, 06).content;
	bool isSHA1 = false;
	bool isSHA256 = false;
	if (digestAlgo == VarToByteArray(OID_RSAwithSHA1))
		isSHA1 = true;
	else
		if (digestAlgo == VarToByteArray(OID_RSAwithSHA256))
			isSHA256 = true;
		else
			throw logged_error("Algoritmo del digest della firma non valido");

	CASNTag &signature = temp3.Child(5, 04);

	//Calcolo del digest del SOD (signed data) con SHA256
	CSHA256 sha256;
	ByteDynArray calcDigest = sha256.Digest(ttData.mid((int)signedData.startPos, (int)(signedData.endPos - signedData.startPos)));
	if (calcDigest != digest.content)
		throw logged_error("Digest del SOD non corrispondente ai dati");

	ByteArray certRaw = SOD.mid((int)signerCert.startPos, (int)(signerCert.endPos - signerCert.startPos));
	PCCERT_CONTEXT certDS = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certRaw.data(), (DWORD)certRaw.size());
	if (certDS == nullptr)
		throw logged_error("Certificato DS non valido");

	auto _1 = scopeExit([&]() noexcept {CertFreeCertificateContext(certDS); });

	ByteArray pubKeyData(certDS->pCertInfo->SubjectPublicKeyInfo.PublicKey.pbData, certDS->pCertInfo->SubjectPublicKeyInfo.PublicKey.cbData);
	CASNParser pubKeyParser;
	pubKeyParser.Parse(pubKeyData);
	CASNTag &pubKey = *pubKeyParser.tags[0];
	CASNTag &modTag = pubKey.Child(0, 02);
	ByteArray mod = modTag.content;
	while (mod[0] == 0)
		mod = mod.mid(1);
	CASNTag &expTag = pubKey.Child(1, 02);
	ByteArray exp = expTag.content;
	while (exp[0] == 0)
		exp = exp.mid(1);

	ByteArray signatureData = signature.content;

	CRSA rsa(mod, exp);

	ByteDynArray decryptedSignature = rsa.RSA_PURE(signatureData);
	decryptedSignature = decryptedSignature.mid(RemovePaddingBT1(decryptedSignature));
	ByteArray toSign = SOD.mid((int)signerInfo.tags[0]->startPos, (int)(signerInfo.tags[signerInfo.tags.size() - 1]->endPos - signerInfo.tags[0]->startPos));
	ByteDynArray digestSignature;
	if (isSHA1) {
		CSHA1 sha1;
		decryptedSignature = decryptedSignature.mid(RemoveSha1(decryptedSignature));
		digestSignature = sha1.Digest(toSign.getASN1Tag(0x31));
	}
	if (isSHA256) {
		CSHA256 sha256;
		decryptedSignature = decryptedSignature.mid(RemoveSha256(decryptedSignature));
		digestSignature = sha256.Digest(toSign.getASN1Tag(0x31));
	}
	if (digestSignature != decryptedSignature)
		throw logged_error("Firma del SOD non valida");

	//log.Info("Verifica issuer");

	issuerName.Reparse();
	CASNParser issuerParser;
	issuerParser.Parse(ByteArray(certDS->pCertInfo->Issuer.pbData, certDS->pCertInfo->Issuer.cbData));
	CASNTag &CertIssuer = *issuerParser.tags[0];
	if (issuerName.tags.size() != CertIssuer.tags.size())
		throw logged_error("Issuer name non corrispondente");
	for (std::size_t i = 0; i < issuerName.tags.size(); i++) {
		CASNTag &certElem = *CertIssuer.tags[i]->tags[0];
		CASNTag &SODElem = *issuerName.tags[i]->tags[0];
		certElem.tags[0]->Verify(SODElem.tags[0]->content);
		certElem.tags[1]->Verify(SODElem.tags[1]->content);
	}

	ByteDynArray certSerial = ByteArray(certDS->pCertInfo->SerialNumber.pbData, certDS->pCertInfo->SerialNumber.cbData);
	if (certSerial.reverse() != signerCertSerialNumber.content)
		throw logged_error("Serial Number del certificato non corrispondente");

	// ora verifico gli hash dei DG
	//log.Info("Verifica hash DG");
	uint8_t val0 = 0;
	signedData.Child(0, 02).Verify(VarToByteArray(val0));
	signedData.Child(1, 0x30).Child(0, 06).Verify(VarToByteArray(OID_SH256));

	CASNTag &hashTag = signedData.Child(2, 0x30);
	for (std::size_t i = 0; i < hashTag.tags.size(); i++) {
		CASNTag &hashDG = *(hashTag.tags[i]);
		CASNTag &dgNum = hashDG.CheckTag(0x30).Child(0, 02);
		CASNTag &dgHash = hashDG.Child(1, 04);
		uint8_t num = ByteArrayToVar(dgNum.content, BYTE);

		if (hashSet.find(num) == hashSet.end() || hashSet[num].size() == 0)
			throw logged_error(stdPrintf("Digest non trovato per il DG %02X", num));

		if (hashSet[num] != dgHash.content)
			throw logged_error(stdPrintf("Digest non corrispondente per il DG %02X", num));
	}

	/*if (CSCA != null && CSCA.Count > 0)
	{
		log.Info("Verifica catena CSCA");
		X509CertChain chain = new X509CertChain(CSCA);
		var certChain = chain.getPath(certDS);
		if (certChain == null)
			throw Exception("Il certificato di Document Signer non č valido");
		var rootCert = certChain[0];
		if (!new ByteArray(rootCert.SubjectName.RawData).IsEqual(rootCert.IssuerName.RawData))
			throw Exception("Impossibile validare il certificato di Document Signer");
	}
	*/
	exit_func
}

#define DWL_MSGRESULT 0

BOOL CheckOneInstance(char *nome)
{
	auto m_hStartEvent = CreateEvent(NULL, TRUE, FALSE, nome);
	if (GetLastError() == ERROR_ALREADY_EXISTS && m_hStartEvent != nullptr) {
		CloseHandle(m_hStartEvent);
		m_hStartEvent = nullptr;
		return FALSE;
	}
	return TRUE;
}
