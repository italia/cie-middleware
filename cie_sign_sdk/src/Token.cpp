#include "stdafx.h"

#include "token.h"
#include "apdu.h"
#include "../util/tlv.h"
#include <vector>

static char *szCompiledFile=__FILE__;

CToken::CToken()
{
	transmitCallback=NULL;
	transmitCallbackData=NULL;
}

CToken::~CToken()
{
}


void CToken::Reset(bool unpower) {
	init_func
	ER_ASSERT(transmitCallback!=nullptr, "Carta non Connessa")

	WORD reset = unpower ? 0xfffe : 0xffff;
	StatusWord sw;
	if ((sw = Transmit(VarToByteArray(reset), NULL)) != 0x9000)
		throw scard_error(sw);
}

void CToken::SelectMF() {
	init_func
	ER_ASSERT(transmitCallback!=nullptr,"Carta non Connessa")

	APDU apdu(0x00,0xA4,0x00,0x00);
	StatusWord sw;
	if ((sw = Transmit(apdu, NULL)) != 0x9000)
		throw scard_error(sw);
}


ByteDynArray CToken::BinaryRead(WORD start,BYTE size) {
	init_func
	DWORD dwBase=0;
	std::vector<BYTE> dt;

	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0xB0,BYTE(start>>8),BYTE(start & 0xff),size);
	ByteDynArray resp;
	StatusWord sw;
	if ((sw = Transmit(apdu, &resp)) != 0x9000)
		throw scard_error(sw);

	return resp;
}

StatusWord CToken::Transmit(ByteArray &apdu, ByteDynArray *resp)
{
	init_func
	BYTE pbtResp[3000];
	DWORD dwResp = 3000;
	HRESULT res = transmitCallback(transmitCallbackData, apdu.data(), apdu.size(), pbtResp, &dwResp);
	ByteArray scResp(pbtResp, dwResp);

	if (res != SCARD_S_SUCCESS) // la smart card � stata estratta durante l'operazione
		throw windows_error(res);

	if (scResp.size() < 2)
		throw logged_error("Risposta della smart card non valida");

	if (resp != nullptr)
		*resp = scResp.left(scResp.size() - 2);
	
	auto sw = ByteArrayToVar(scResp.right(2).reverse(), uint16_t);
	return sw;
}

StatusWord CToken::Transmit(APDU &apdu, ByteDynArray *resp)
{
	init_func

	BYTE pbtAPDU[3000];
	BYTE pbtResp[3000];

	ByteDynArray baSMData;
	
	int iAPDUSize = 0;
	pbtAPDU[0] = apdu.btCLA;
	pbtAPDU[1] = apdu.btINS;
	pbtAPDU[2] = apdu.btP1;
	pbtAPDU[3] = apdu.btP2;
	if (apdu.bLC && apdu.bLE) {
		iAPDUSize = apdu.btLC + 6;
		pbtAPDU[4] = apdu.btLC;
		memcpy_s(pbtAPDU + 5, 2995, apdu.pbtData, apdu.btLC);
		pbtAPDU[5 + apdu.btLC] = apdu.btLE;
		}
	else if (apdu.bLC && !apdu.bLE) {
		iAPDUSize = apdu.btLC + 5;
		pbtAPDU[4] = apdu.btLC;
		memcpy_s(pbtAPDU + 5, 2995, apdu.pbtData, apdu.btLC);
		}
	else if (!apdu.bLC && apdu.bLE) {
			iAPDUSize = 5;
		pbtAPDU[4] = apdu.btLE;
		}
		else  { // (!bLC && !bLE)
			iAPDUSize = 4;
		}

	DWORD dwResp = 3000;
	HRESULT res = transmitCallback(transmitCallbackData, pbtAPDU, iAPDUSize, pbtResp, &dwResp);
	ByteArray scResp(pbtResp, dwResp);

	if (res != SCARD_S_SUCCESS) // la smart card � stata estratta durante l'operazione
		throw windows_error(res);

	if (scResp.size() < 2)
		throw logged_error("Risposta della smart card non valida");

	if (resp != nullptr)
		*resp = scResp.left(scResp.size() - 2);

	auto sw = ByteArrayToVar(scResp.right(2).reverse(), uint16_t);
	return sw;
}

void CToken::setTransmitCallback(TokenTransmitCallback func,void *data)
{
	init_func
	transmitCallback=func;
	transmitCallbackData=data;
}

void CToken::setTransmitCallbackData(void *data)
{
	init_func
	transmitCallbackData = data;
}

void *CToken::getTransmitCallbackData()
{
	return transmitCallbackData;
}
