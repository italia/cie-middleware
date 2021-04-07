#pragma once

#include <winscard.h>
#include "apdu.h"
//#include "syncromutex.h"
#include "Array.h"

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
	typedef HRESULT(*TokenTransmitCallback)(void *data, uint8_t *apdu, DWORD apduSize, uint8_t *resp, DWORD *respSize);

private:
	TokenTransmitCallback transmitCallback;
	void *transmitCallbackData;
public:
	CToken();
	~CToken();

	void SelectMF();
	ByteDynArray BinaryRead(uint16_t start, uint8_t size);
	void Reset(bool unpower = false);

	void setTransmitCallback(TokenTransmitCallback func,void *data);
	void setTransmitCallbackData(void *data);
	void* getTransmitCallbackData();
	StatusWord Transmit(APDU &apdu, ByteDynArray *resp);
	StatusWord Transmit(ByteArray &apdu, ByteDynArray *resp);
};
