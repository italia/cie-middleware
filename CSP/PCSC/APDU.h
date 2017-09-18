#pragma once
#include <winscard.h>

class CToken;
class APDU
{
public:
	APDU();
	APDU(BYTE CLA,BYTE INS,BYTE P1,BYTE P2,BYTE LC,BYTE *pData,BYTE LE);
	APDU(BYTE CLA,BYTE INS,BYTE P1,BYTE P2,BYTE LC,BYTE *pData);
	APDU(BYTE CLA,BYTE INS,BYTE P1,BYTE P2,BYTE LE);
	APDU(BYTE CLA,BYTE INS,BYTE P1,BYTE P2);
	~APDU();

	RESULT setSM(ByteArray *SigCommandKey,ByteArray *EncCommandKey,ByteArray *SigResponseKey,ByteArray *EncResponseKey);
	RESULT GenerateRandom(ByteArray &baRandom);

	RESULT DecodeSM(ByteArray &SMresp,ByteDynArray &resp);
	RESULT EncodeSM(CToken &Token,ByteDynArray &SMDataBuffer,APDU *&apdu);
	RESULT CypherTextObject(ByteDynArray &CypherTextObject);
	RESULT HeaderBlock(ByteArray &Challenge,ByteDynArray &HeaderBlock);
	RESULT NetLEMACObject(ByteDynArray &NetLEMACObject);
	RESULT MacObject(ByteArray &HeaderObject,ByteArray &CypherTextBlock,ByteArray &ThirdObject,ByteArray &SigKey,ByteDynArray &MacObject);

	BYTE btINS;	//INS dell'APDU
	BYTE btCLA;	//CLA dell'APDU
	BYTE btP1;	//P1 dell'APDU
	BYTE btP2;	//P2 dell'APDU
	BYTE btLC;	//LC dell'APDU
	bool bLC;	//flag: il campo dati è da includere? (caso 1 e 3)
	BYTE *pbtData;	//campo dati dell'APDU
	BYTE btLE;	//flag: LE è da includere? (caso 2 e 4)
	bool bLE;	//LE dell'APDU
	ByteArray *baSigCommandKey;		//chiave da usare per il SM_SIG_COMMAND
	ByteArray *baEncCommandKey;		//chiave da usare per il SM_ENC_COMMAND
	ByteArray *baSigResponseKey;	//chiave da usare per il SM_SIG_RESPONSE
	ByteArray *baEncResponseKey;	//chiave da usare per il SM_ENC_RESPONSE
	BYTE pbtRandom[8];
	ByteArray baRandom;				//random da verificare nella decodifica dell'apdu
};
