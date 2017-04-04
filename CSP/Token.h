#pragma once

#include <winscard.h>
#include "apdu.h"
#include "syncromutex.h"

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
	RESULT SelectMinPath(ByteArray &path); 
	RESULT TestAuth(BYTE TestID,bool &bVerif);
	RESULT Select(BYTE *id,DWORD *size=NULL);
	RESULT SelectMF();
	RESULT SelectParent();
	RESULT BinaryRead(ByteDynArray &data,WORD start,BYTE size,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT ReadFile(ByteDynArray &data,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT ReadRecord(BYTE bRec,ByteDynArray &data,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT WriteFile(ByteArray &data,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT Verify(bool bBacktrack,BYTE pinID,ByteArray &pin,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT ExternalAuthenticate(bool bBacktrack,BYTE keyID,ByteArray &response,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT Unblock(bool bBacktrack,BYTE pinID,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT UnblockChange(bool bBacktrack,BYTE pinID,ByteArray &Puk,ByteArray &Pin,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT Change(bool bBacktrack,BYTE pinID,ByteArray &Pin,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT SelectPath(ByteArray &path,DWORD *iSize=NULL);
	RESULT MSERestore(BYTE bSecurityEnvironment);
	RESULT MSESet(CardPSO operation,BYTE bKeyID);
	RESULT PSO_DEC(ByteArray& baInput,ByteDynArray& baOutput,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT PSO_CDS(ByteArray& baInput,ByteDynArray& baOutput,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT GetChallenge(ByteArray& baRandomData);
	RESULT GiveRandom(ByteArray& baRandomData);
	RESULT ChangeKeyData(BYTE keyType,bool bBackTrack,BYTE keyID,ByteArray &Data,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT AddTLVRecord(BYTE tag,ByteArray &Data,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT ChangeBSOAC(BYTE keyClass,BYTE keyID,ByteArray &AC,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT ChangeEFAC(ByteArray &AC,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT Delete(BYTE *id,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT GetChipSerial(ByteDynArray &baSerial);
	RESULT GetChipSerialST(ByteDynArray &baSerial);
	RESULT GenerateKey(BYTE* key,BYTE *pubkey,ByteDynArray *Module,ByteDynArray *Exponent,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT GenerateKeyST(BYTE* key,BYTE *pubkey,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT GenerateKeyAthena(BYTE* key, BYTE *pubkey, ByteArray *sigIn = NULL, ByteArray *encIn = NULL, ByteArray *sigOut = NULL, ByteArray *encOut = NULL);	
	RESULT GenerateKeyOberthur(BYTE* key,BYTE *pubkey,int modSize=16,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);	
	RESULT Deactivate(ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT Deactivate(BYTE *id,ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT Activate(ByteArray *sigIn=NULL,ByteArray *encIn=NULL,ByteArray *sigOut=NULL,ByteArray *encOut=NULL);
	RESULT Activate(BYTE *id, ByteArray *sigIn = NULL, ByteArray *encIn = NULL, ByteArray *sigOut = NULL, ByteArray *encOut = NULL);
	RESULT Reset(bool unpower = false);

	RESULT setTransmitCallback(TokenTransmitCallback func,void *data);
	RESULT setTransmitCallbackData(void *data);
	void* getTransmitCallbackData();
	RESULT Transmit(APDU &apdu, ByteDynArray *resp);
	RESULT Transmit(ByteArray &apdu, ByteDynArray *resp);
};
