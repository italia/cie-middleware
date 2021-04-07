#pragma once
#include <functional>

class CToken;
class APDU
{
public:
	APDU();
	APDU(uint8_t CLA,uint8_t INS,uint8_t P1,uint8_t P2,uint8_t LC,uint8_t *pData,uint8_t LE);
	APDU(uint8_t CLA,uint8_t INS,uint8_t P1,uint8_t P2,uint8_t LC,uint8_t *pData);
	APDU(uint8_t CLA,uint8_t INS,uint8_t P1,uint8_t P2,uint8_t LE);
	APDU(uint8_t CLA,uint8_t INS,uint8_t P1,uint8_t P2);
	~APDU();

	uint8_t btINS;	//INS dell'APDU
	uint8_t btCLA;	//CLA dell'APDU
	uint8_t btP1;	//P1 dell'APDU
	uint8_t btP2;	//P2 dell'APDU
	uint8_t btLC;	//LC dell'APDU
	bool bLC;	//flag: il campo dati è da includere? (caso 1 e 3)
	uint8_t *pbtData;	//campo dati dell'APDU
	uint8_t btLE;	//flag: LE è da includere? (caso 2 e 4)
	bool bLE;	//LE dell'APDU
};
