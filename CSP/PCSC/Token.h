#pragma once

#include <winscard.h>
#include "apdu.h"
#include "../util/syncromutex.h"

extern SCARDCONTEXT hContext;
	
enum CardPSO {
	Op_PSO_DEC,
	Op_PSO_ENC,
	Op_PSO_CDS
};

class CCardLocker;
class CToken
{
public:
	typedef int(*TokenTransmitCallback)(void *data, BYTE *apdu, DWORD apduSize, BYTE *resp, DWORD *respSize);

private:
	TokenTransmitCallback transmitCallback;
	void *transmitCallbackData;
public:
	CToken();
	~CToken();

	RESULT Connect(char * reader);
	RESULT Select(BYTE *id,DWORD *size=NULL);
	RESULT SelectMF();
	RESULT BinaryRead(ByteDynArray &data,WORD start,BYTE size);
	RESULT Reset(bool unpower = false);

	RESULT setTransmitCallback(TokenTransmitCallback func,void *data);
	RESULT setTransmitCallbackData(void *data);
	void* getTransmitCallbackData();
	RESULT Transmit(APDU &apdu, ByteDynArray *resp);
	RESULT Transmit(ByteArray &apdu, ByteDynArray *resp);
};
