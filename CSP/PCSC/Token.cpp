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

RESULT CToken::Select(BYTE *id,DWORD *size) {
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0xA4,0x00,0x00,0x02,id,0xFF);
	ByteDynArray resp;
	CARD_R_CALL(Transmit(apdu,&resp))

	if (size) {
		*size=-1;
		CTLV tlv(resp.mid(2));
		ByteArray *baSize=NULL;
		ER_CALL(tlv.getTAG(0x80,baSize),"Errore nel TLV")
		if (baSize==NULL) {
			ER_CALL(tlv.getTAG(0x81,baSize),"Errore nel TLV")
		}
		ER_ASSERT(baSize,"Impossibile trovare l'EF Size")
		*size=(*baSize)[2] << 8 | (*baSize)[3];
	}
	
	_return(OK)
	exit_func
	_return(FAIL)
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

RESULT CToken::SelectParent() {
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0xA4,0x03,0x00);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::BinaryRead(ByteDynArray &data,WORD start,BYTE size,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut) {
	init_func
	DWORD dwBase=0;
	std::vector<BYTE> dt;

	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0xB0,BYTE(start>>8),BYTE(start & 0xff),size);
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	ByteDynArray resp;
	CARD_R_CALL(Transmit(apdu,&resp))

	data.alloc_copy(resp);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::ReadFile(ByteDynArray &data,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut) {
	init_func
	DWORD dwBase=0;
	std::vector<BYTE> dt;

	ER_ASSERT(transmitCallback,"Carta non Connessa")

	ByteDynArray resp;
	do {
		APDU apdu(0x00,0xB0,BYTE(dwBase>>8),BYTE(dwBase & 0xff),0xF1);
		apdu.setSM(sigIn,encIn,sigOut,encOut);
		CARD_R_CALL(Transmit(apdu,&resp))
		if (resp.size()>0) dt.insert(dt.end(),resp.lock(),resp.lock()+resp.size());
		dwBase+=resp.size();
	} while(resp.size()==0xF1);

	data.alloc_copy(&dt[0],(DWORD)dt.size());
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::ReadRecord(BYTE bRec,ByteDynArray &data,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func

	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0xB2,bRec,0x04,0x00);
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,&data))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::WriteFile(ByteArray &data,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	DWORD dwDataSize=(DWORD)data.size();

	DWORD dwBase=0;
	while (dwBase<dwDataSize) {
		DWORD dwSpan=(dwDataSize-dwBase);
		if (dwSpan>0xF0) dwSpan=0xF0;
		APDU apdu(0x00,0xD6,BYTE(dwBase>>8),BYTE(dwBase & 0xff),(BYTE)dwSpan,data.lock(dwSpan,dwBase));
		apdu.setSM(sigIn,encIn,sigOut,encOut);
		CARD_R_CALL(Transmit(apdu,NULL))
		dwBase+=dwSpan;
	};
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::Delete(BYTE *id,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0xE4,0x00,0x00,0x02,id);
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::Unblock(bool bBacktrack,BYTE pinID,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")


	APDU apdu(0x00,0x2C,0x03,pinID & 0x7F | (bBacktrack ? 0x80 : 0));
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::UnblockChange(bool bBacktrack,BYTE pinID,ByteArray &Puk,ByteArray &Pin,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	ByteDynArray PukPin(Puk.size()+Pin.size());
	PukPin.copy(Puk);
	PukPin.rightcopy(Pin);
	APDU apdu(0x00, 0x2C, 0x00, pinID & 0x7F | (bBacktrack ? 0x80 : 0), (BYTE)PukPin.size(), PukPin.lock());
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::Change(bool bBacktrack,BYTE pinID,ByteArray &Pin,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0x24,0x01,pinID & 0x7F | (bBacktrack ? 0x80 : 0),(BYTE)Pin.size(),Pin.lock());
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::ChangeKeyData(BYTE keyType,bool bBacktrack,BYTE keyID,ByteArray &Data,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func

	ER_ASSERT(transmitCallback,"Carta non Connessa")
	ByteDynArray TLVData(Data.size()+2);
	TLVData[0] = (BYTE)Data.size() + 1;
	TLVData[1]=0;
	TLVData.copy(Data,2);

	APDU apdu(0x90, 0x24, keyType, keyID & 0x7F | (bBacktrack ? 0x80 : 0), (BYTE)TLVData.size(), TLVData.lock());
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::AddTLVRecord(BYTE tag,ByteArray &Data,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func

	ER_ASSERT(transmitCallback,"Carta non Connessa")
	ByteDynArray TLVData(Data.size()+4);
	TLVData[0] = tag;
	TLVData[1] = (BYTE)Data.size() + 2;
	TLVData[2] = (BYTE)Data.size() + 1;
	TLVData[3] = 0;
	TLVData.copy(Data,4);

	APDU apdu(0x00, 0xe2, 0x00, 0x00, (BYTE)TLVData.size(), TLVData.lock());
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::Deactivate(ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0x04,0x00,0x00);
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::Deactivate(BYTE *id,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0x04,0x00,0x00,0x02,id);
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::Activate(ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0x44,0x00,0x00);
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::Activate(BYTE *id, ByteArray *sigIn, ByteArray *encIn, ByteArray *sigOut, ByteArray *encOut)
{
	init_func
		ER_ASSERT(transmitCallback, "Carta non Connessa")

		APDU apdu(0x00, 0x44, 0x00, 0x00, 0x02, id);
	apdu.setSM(sigIn, encIn, sigOut, encOut);
	CARD_R_CALL(Transmit(apdu, NULL))

		_return(OK)
		exit_func
		_return(FAIL)
}

RESULT CToken::Verify(bool bBacktrack,BYTE pinID,ByteArray &pin,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")


	APDU apdu(0x00,0x20,0x00,pinID & 0x7F | (bBacktrack ? 0x80 : 0),(BYTE)pin.size(),pin.lock());
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::ExternalAuthenticate(bool bBacktrack,BYTE keyID,ByteArray &response,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")


	APDU apdu(0x00,0x82,0x00,keyID & 0x7F | (bBacktrack ? 0x80 : 0),(BYTE)response.size(),response.lock());
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::GenerateKeyAthena(BYTE* key, BYTE *pubkey, ByteArray *sigIn, ByteArray *encIn, ByteArray *sigOut, ByteArray *encOut)
{
	init_func
		ER_ASSERT(transmitCallback, "Carta non Connessa")

	ByteDynArray response;
	APDU getData(0x00, 0xCA, 0x01, 0x12, 0x00);
	CARD_R_CALL(Transmit(getData, &response))

	BYTE data[] = { key[0], key[1], pubkey[0], pubkey[1], 0x00, 0x00, 0x00, 0x18 };
	APDU apdu(0x00, 0x46, 0x00, 0x00, sizeof(data), data);
	apdu.setSM(sigIn, encIn, sigOut, encOut);
	CARD_R_CALL(Transmit(apdu, &response))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::GenerateKey(BYTE* key,BYTE *pubkey,ByteDynArray *Module,ByteDynArray *Exponent,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")


	BYTE data[]={key[0],key[1],pubkey[0],pubkey[1],0x00,0x00,0x00,0x18};
	APDU apdu(0x00,0x46,0x00,0x00,sizeof(data),data,0x00);
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	ByteDynArray response;
	CARD_R_CALL(Transmit(apdu,&response))

	if (Module)
		*Module=response.mid(7,0x80);
	if (Exponent)
		*Exponent=response.right(3);

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::GenerateKeyOberthur(BYTE* key,BYTE *pubkey,int modSize,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")


	BYTE data[]={key[0],key[1],pubkey[0],pubkey[1],0x00,0x000,0x00,modSize};
	APDU apdu(0x00,0x46,0x00,0x00,sizeof(data),data,0x00);
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	ByteDynArray response;
	CARD_R_CALL(Transmit(apdu,&response))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::GenerateKeyST(BYTE* key,BYTE *pubkey,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")


	BYTE data[]={key[0],key[1],pubkey[0],pubkey[1],0x00,0x000,0x00,0x00};
	APDU apdu(0x00,0x46,0x00,0x00,sizeof(data),data,0x00);
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	ByteDynArray response;
	CARD_R_CALL(Transmit(apdu,&response))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::SelectPath(ByteArray &path,DWORD *size)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	BYTE *pbtPath=path.lock();
	BYTE btPathSize = (BYTE)path.size();
	if (path.size()>=2) {
		if (path[0]==0x3f && path[1]==0x00) {
			pbtPath+=2;
			btPathSize-=2;
		}
	}
	if (btPathSize==0) {
		ER_CALL(SelectMF(),
			"Errore nella selezine del MF")
		_return(OK)
	}

	APDU apdu(0x00,0xA4,0x08,0x00,btPathSize,pbtPath,0xFF);
	ByteDynArray resp;
	CARD_R_CALL(Transmit(apdu,&resp))

	if (size) {
		*size=-1;
		for (DWORD i=0;i<resp.size()-4;i++) {
			if (resp[i]==0x80 && resp[i+1]==0x02) {
				*size=resp[i+2] << 8 | resp[i+3];
				break;
			}
		}
	}

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::MSERestore(BYTE bSecurityEnvironment)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00,0x22,0xF3,bSecurityEnvironment);

	CARD_R_CALL(Transmit(apdu,NULL))
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::MSESet(CardPSO operation,BYTE bKeyID)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")
	
	switch(operation) {
		case Op_PSO_DEC : {
			BYTE btData[]={0x83,0x01,bKeyID};
			APDU apdu(0x00,0x22,0xF1,0xB8,0x03,btData);
			CARD_R_CALL(Transmit(apdu,NULL))
			break;
		}
		case Op_PSO_CDS : {
			BYTE btData[]={0x83,0x01,bKeyID};
			APDU apdu(0x00,0x22,0xF1,0xB6,0x03,btData);
			CARD_R_CALL(Transmit(apdu,NULL))
			break;
		}
	}

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::ChangeBSOAC(BYTE keyClass,BYTE keyID,ByteArray &AC,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	ByteDynArray Data(6+AC.size());
	Data[0]=0x83;
	Data[1]=0x02;
	Data[2]=keyClass;
	Data[3]=keyID;
	Data[4]=0x86;
	Data[5] = (BYTE)AC.size();
	Data.copy(AC,6);
	APDU apdu(0x00, 0xDA, 0x01, 0x6E, (BYTE)Data.size(), Data.lock());
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))
	

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::ChangeEFAC(ByteArray &AC,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	ByteDynArray Data(2+AC.size());
	Data[0]=0x86;
	Data[1] = (BYTE)AC.size();
	Data.copy(AC,2);
	APDU apdu(0x00, 0xDA, 0x01, 0x6F, (BYTE)Data.size(), Data.lock());
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,NULL))
	

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::PSO_DEC(ByteArray& baInput,ByteDynArray& baOutput,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	ByteDynArray baAPDUInput(baInput.size()+1);
	baAPDUInput[0]=0x00;
	baAPDUInput.rightcopy(baInput);
	APDU apdu(0x00, 0x2A, 0x80, 0x86, (BYTE)baAPDUInput.size(), baAPDUInput.lock(), 0);
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,&baOutput))
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::PSO_CDS(ByteArray& baInput,ByteDynArray& baOutput,ByteArray *sigIn,ByteArray *encIn,ByteArray *sigOut,ByteArray *encOut)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00, 0x2A, 0x9E, 0x9A, (BYTE)baInput.size(), baInput.lock(), 0);
	apdu.setSM(sigIn,encIn,sigOut,encOut);
	CARD_R_CALL(Transmit(apdu,&baOutput))

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::GetChallenge(ByteArray& baRandomData)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")

	APDU apdu(0x00, 0x84, 0x00, 0x00, (BYTE)baRandomData.size());

	ByteDynArray resp(baRandomData.size());
	CARD_R_CALL(Transmit(apdu,&resp))

	baRandomData.copy(resp);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::GiveRandom(ByteArray& baRandomData)
{
	init_func
	ER_ASSERT(transmitCallback,"Carta non Connessa")


	APDU apdu(0x80, 0x86, 0x00, 0x00, (BYTE)baRandomData.size(), baRandomData.lock());

	CARD_R_CALL(Transmit(apdu,NULL))
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::GetChipSerialST(ByteDynArray &baSerial)
{
	init_func
	APDU getChipData(0xB0,0xFD,0x00,0x00,0x51);
	ByteDynArray baChipData;
	CARD_R_CALL(Transmit(getChipData,&baChipData))
	baSerial.alloc_copy(baChipData.left(16));
	_return(OK)
	exit_func
	_return(FAIL)

}

RESULT CToken::GetChipSerial(ByteDynArray &baSerial)
{
	init_func
	APDU getChipData(0x00,0xCA,0x01,0x81,0xFF);
	ByteDynArray baChipData;
	CARD_R_CALL(Transmit(getChipData,&baChipData))
	baSerial.alloc_copy(baChipData.left(16));
	_return(OK)
	exit_func
	_return(FAIL)

}

RESULT CToken::SelectMinPath(ByteArray &path) {
	init_func
	APDU getAbsPath(0x00,0xCA,0x01,0x87,0xFF);
	ByteDynArray baAbsPath;
	CARD_R_CALL(Transmit(getAbsPath,&baAbsPath))
	ByteArray fullPath;
	if (path.size()>=2) {
		BYTE MFID[]={0x3f,0x00};
		if (path.left(2)==ByteArray(MFID,2))
			fullPath=path.mid(2);
	}
	else 
		fullPath=path;

	ByteArray selPath;
	DWORD dwComPoint=-1;
	DWORD dwMinSize=min(fullPath.size(),baAbsPath.size());
	for (DWORD i=0;i<dwMinSize;i+=2) {
		if (fullPath.mid(i,2)!=baAbsPath.mid(i,2)) {
			dwComPoint=i;
			break;
		}
	}
	if (dwComPoint==-1)
		dwComPoint=dwMinSize;
	if (dwComPoint==0) {
		_return(SelectPath(path));
	}
	else {
		// torno indietro nei df del curpath
		DWORD dwUp=(baAbsPath.size()-dwComPoint)/2;
		for (DWORD i=0;i<dwUp;i++)
			CARD_R_CALL(SelectParent())

		// e proseguo nel nuovo path
		for (DWORD i=dwComPoint;i<fullPath.size();i++)
			CARD_R_CALL(Select(fullPath.mid(i,2).lock(2)))
	}

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::TestAuth(BYTE TestID,bool &bVerif)
{
	init_func
	bVerif=false;
	APDU getSecurityStatus(0x00,0xCA,0x01,0x84,0xFF);
	ByteDynArray baSecurity;
	CARD_R_CALL(Transmit(getSecurityStatus,&baSecurity))
	for (DWORD i=0;i<baSecurity.size();i++) {
		if (baSecurity[i]==TestID) {
			bVerif=true;
			break;
		}
	}

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CToken::Transmit(ByteArray &apdu, ByteDynArray *resp)
{
	init_func
	BYTE pbtResp[3000];
	DWORD dwResp = 3000;
	HRESULT res = transmitCallback(transmitCallbackData, apdu.lock(), apdu.size(), pbtResp, &dwResp);

	if (res != SCARD_S_SUCCESS) // la smart card è stata estratta durante l'operazione
		throw CWinException(res);

	if (resp != nullptr)
		resp->alloc_copy(pbtResp, dwResp - 2);

	_return(pbtResp[dwResp - 2] << 8 | pbtResp[dwResp - 1])
	exit_func
	_return(FAIL)
}

RESULT CToken::Transmit(APDU &apdu, ByteDynArray *resp)
{
	init_func

	BYTE pbtAPDU[3000];
	BYTE pbtResp[3000];

	ByteDynArray baSMData;
	APDU *SMApdu = &apdu;
	ER_CALL(apdu.EncodeSM(*this, baSMData, SMApdu),
		"Errore nella codifica dell'APDU")

		Allocator<APDU> sendApdu(SMApdu);

	int iAPDUSize = 0;
	pbtAPDU[0] = sendApdu->btCLA;
	pbtAPDU[1] = sendApdu->btINS;
	pbtAPDU[2] = sendApdu->btP1;
	pbtAPDU[3] = sendApdu->btP2;
	if (sendApdu->bLC && sendApdu->bLE) {
		iAPDUSize = sendApdu->btLC + 6;
		pbtAPDU[4] = sendApdu->btLC;
		memcpy_s(pbtAPDU + 5, 2995, sendApdu->pbtData, sendApdu->btLC);
		pbtAPDU[5 + sendApdu->btLC] = sendApdu->btLE;
	}
	else if (sendApdu->bLC && !sendApdu->bLE) {
		iAPDUSize = sendApdu->btLC + 5;
		pbtAPDU[4] = sendApdu->btLC;
		memcpy_s(pbtAPDU + 5, 2995, sendApdu->pbtData, sendApdu->btLC);
	}
	else if (!sendApdu->bLC && sendApdu->bLE) {
		iAPDUSize = 5;
		pbtAPDU[4] = sendApdu->btLE;
	}
	else  { // (!bLC && !bLE)
		iAPDUSize = 4;
	}

	DWORD dwResp = 3000;

	if (sendApdu == &apdu)
		sendApdu.detach();

	HRESULT res = transmitCallback(transmitCallbackData, pbtAPDU, iAPDUSize, pbtResp, &dwResp);
	//HRESULT res=SCardTransmit(hCard,SCARD_PCI_T1,pbtAPDU,iAPDUSize,NULL,pbtResp,&dwResp);
	if (res != SCARD_S_SUCCESS) // la smart card è stata estratta durante l'operazione
		_return(FAIL)

		if (resp) {
			ER_CALL(apdu.DecodeSM(ByteArray(pbtResp, dwResp), *resp),
				"Errore nella decodifica dell'APDU")
		}

	_return(pbtResp[dwResp - 2] << 8 | pbtResp[dwResp - 1])
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
