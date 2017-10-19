#pragma once
#include <winscard.h>
#include <functional>

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

	BYTE btINS;	//INS dell'APDU
	BYTE btCLA;	//CLA dell'APDU
	BYTE btP1;	//P1 dell'APDU
	BYTE btP2;	//P2 dell'APDU
	BYTE btLC;	//LC dell'APDU
	bool bLC;	//flag: il campo dati è da includere? (caso 1 e 3)
	BYTE *pbtData;	//campo dati dell'APDU
	BYTE btLE;	//flag: LE è da includere? (caso 2 e 4)
	bool bLE;	//LE dell'APDU
};
