#include "../stdAfx.h"

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


RESULT CToken::Reset(bool unpower) {
	init_func
	ER_ASSERT(transmitCallback, "Carta non Connessa")

	WORD reset = unpower ? 0xfffe : 0xffff;
	RESULT resp=Transmit(VarToByteArray(reset), NULL);

	ER_ASSERT(resp != FAIL, "Errore in Reset Card");

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::SelectMF() {
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0xA4,0x00,0x00);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}


RESULT CToken::BinaryRead(ByteDynArray &data,WORD start,BYTE size) {
	init_func
	DWORD dwBase=0;
	std::vector<BYTE> dt;

	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0xB0,BYTE(start>>8),BYTE(start & 0xff),size);
	ByteDynArray resp;
	CARD_R_CALL(Transmit(apdu,&resp))

	data = resp;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::Transmit(ByteArray &apdu, ByteDynArray *resp)
{
	init_func
	BYTE pbtResp[3000];
	DWORD dwResp = 3000;
	HRESULT res = transmitCallback(transmitCallbackData, apdu.data(), apdu.size(), pbtResp, &dwResp);
	ByteArray scResp(pbtResp, dwResp);

	if (res != SCARD_S_SUCCESS) // la smart card è stata estratta durante l'operazione
		throw CWinException(res);

	if (scResp.size() < 2)
		throw CStringException("Risposta della smart card non valida");

	if (resp != nullptr)
		*resp = scResp.left(scResp.size() - 2);
	
	auto sw = ByteArrayToVar(scResp.right(2).reverse(), uint16_t);
	_return(sw)
	exit_func
	_return(FAIL)
}

RESULT CToken::Transmit(APDU &apdu, ByteDynArray *resp)
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

	if (res != SCARD_S_SUCCESS) // la smart card è stata estratta durante l'operazione
		_return(FAIL)

	if (scResp.size() < 2)
		_return(FAIL)

	if (resp != nullptr)
		*resp = scResp.left(scResp.size() - 2);

	auto sw = ByteArrayToVar(scResp.right(2).reverse(), uint16_t);
	_return(sw)
	exit_func
	_return(FAIL)
}

RESULT CToken::setTransmitCallback(TokenTransmitCallback func,void *data)
{
	init_func
	transmitCallback=func;
	transmitCallbackData=data;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::setTransmitCallbackData(void *data)
{
	init_func
	transmitCallbackData = data;
	_return(OK)
		exit_func
		_return(FAIL)
}

void *CToken::getTransmitCallbackData()
{
	return transmitCallbackData;
}
