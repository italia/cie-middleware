#include "StdAfx.h"
#include "apdu.h"
#include "tlv.h"
#include "util.h"
#include "des3.h"
#include "mac.h"
#include "token.h"

static char *szCompiledFile=__FILE__;

APDU::APDU() : baRandom(pbtRandom,8) {
	baSigCommandKey=NULL;
	baEncCommandKey=NULL;
	baSigResponseKey=NULL;
	baEncResponseKey=NULL;
}
APDU::APDU(BYTE CLA,BYTE INS,BYTE P1,BYTE P2,BYTE LC,BYTE *pData,BYTE LE)  : baRandom(pbtRandom,8){
	init_func
	if (LC>250) throw;
	baSigCommandKey=NULL;
	baEncCommandKey=NULL;
	baSigResponseKey=NULL;
	baEncResponseKey=NULL;		
	btINS=INS;btCLA=CLA;btP1=P1;btP2=P2;btLC=LC;pbtData=pData;btLE=LE;
	bLC=true;bLE=true;
	exit_func
}
APDU::APDU(BYTE CLA,BYTE INS,BYTE P1,BYTE P2,BYTE LC,BYTE *pData)  : baRandom(pbtRandom,8) {
	if (LC>251) throw;
	baSigCommandKey=NULL;
	baEncCommandKey=NULL;
	baSigResponseKey=NULL;
	baEncResponseKey=NULL;
	btINS=INS;btCLA=CLA;btP1=P1;btP2=P2;btLC=LC;pbtData=pData;btLE=0;
	bLC=true;bLE=false;
}
APDU::APDU(BYTE CLA,BYTE INS,BYTE P1,BYTE P2,BYTE LE)  : baRandom(pbtRandom,8) {
	baSigCommandKey=NULL;
	baEncCommandKey=NULL;
	baSigResponseKey=NULL;
	baEncResponseKey=NULL;
	btINS=INS;btCLA=CLA;btP1=P1;btP2=P2;btLE=LE;btLC=0;
	bLC=false;bLE=true;
}
APDU::APDU(BYTE CLA,BYTE INS,BYTE P1,BYTE P2)  : baRandom(pbtRandom,8) {
	baSigCommandKey=NULL;
	baEncCommandKey=NULL;
	baSigResponseKey=NULL;
	baEncResponseKey=NULL;
	btINS=INS;btCLA=CLA;btP1=P1;btP2=P2;btLE=0;btLC=0;
	bLC=false;bLE=false;
}

APDU::~APDU()
{
}

RESULT APDU::setSM(ByteArray *SigCommandKey,ByteArray *EncCommandKey,ByteArray *SigResponseKey,ByteArray *EncResponseKey)
{
	init_func
	baSigCommandKey=SigCommandKey;
	baEncCommandKey=EncCommandKey;
	baSigResponseKey=SigResponseKey;
	baEncResponseKey=EncResponseKey;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT APDU::CypherTextObject(ByteDynArray &CypherTextObject) {
	init_func
	ByteDynArray Data(ISOPadLen(btLC));
	Data.copy(pbtData,btLC);
	ISOPad(Data, btLC);
	
    CDES3 des(*baEncCommandKey);
	ByteDynArray EncData;
	des.Encode(Data, EncData);

	CypherTextObject.resize(EncData.size()+3);
	CypherTextObject[0]=0x87;
	CypherTextObject[1]=CypherTextObject.size()-2;
	CypherTextObject[2]=1;
	CypherTextObject.copy(EncData,3);

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT APDU::HeaderBlock(ByteArray &Challenge,ByteDynArray &HeaderBlock) {
	init_func
	ER_ASSERT((Challenge.size() % 8)==0,
		"Errore nella lunghezza del challenge")

	DWORD dwPtr=Challenge.size();
	HeaderBlock.resize(ANSIPadLen(dwPtr+4));
	HeaderBlock.fill(0);
	HeaderBlock.copy(Challenge);
	HeaderBlock[dwPtr++]=btCLA | 0x0c;
	HeaderBlock[dwPtr++]=btINS;
	HeaderBlock[dwPtr++]=btP1;
	HeaderBlock[dwPtr++]=btP2;
	ANSIPad(HeaderBlock, dwPtr);

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT APDU::NetLEMACObject(ByteDynArray &NetLEMACObject) {
	init_func
	NetLEMACObject.resize(3);
	NetLEMACObject[0]=0x97;
	NetLEMACObject[1]=0x01;
	NetLEMACObject[2]=btLE;
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT APDU::MacObject(ByteArray &HeaderBlock,ByteArray &CypherTextObject,ByteArray &ThirdObject,ByteArray &SigKey,ByteDynArray &MacObject) {
	init_func
	// ThirdObject può essere il NetLEMACNetLEMACObject o il ProcessingStatusObject, a
	// seconda che faccia codifica e decodifica
	DWORD dwCypherTextBlockLen=ANSIPadLen(CypherTextObject.size());
	DWORD dwThirdBlockLen=ANSIPadLen(ThirdObject.size());
	ByteDynArray baMACInput(HeaderBlock.size()+dwCypherTextBlockLen+dwThirdBlockLen);
	baMACInput.copy(HeaderBlock);
	baMACInput.copy(CypherTextObject,HeaderBlock.size());
	ANSIPad(baMACInput,HeaderBlock.size()+CypherTextObject.size());
	if (ThirdObject.size()>0)
		baMACInput.copy(ThirdObject,HeaderBlock.size()+dwCypherTextBlockLen);

	CMAC mac(SigKey);
	ByteDynArray MAC;
	mac.Mac(baMACInput, MAC);

	MacObject.resize(MAC.size()+2);
	MacObject[0]=0x8E;
	MacObject[1]=0x08;
	MacObject.rightcopy(MAC);

	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT APDU::DecodeSM(ByteArray &SMresp,ByteDynArray &resp)
{
	init_func
	if (baSigCommandKey==NULL && baEncCommandKey==NULL && baSigResponseKey==NULL && baEncResponseKey==NULL) {
		// caso base; no SM. Mi limito a levare la SW
		ER_ASSERT(SMresp.size()>=2,"Errore nella decodifica dell'APDU")

		resp.alloc_copy(SMresp.revmid(2));
		_return(OK)
	}
	if (baSigCommandKey!=NULL && baEncCommandKey!=NULL && baSigResponseKey==NULL && baEncResponseKey==NULL) {
		// SM in ingresso ma non in uscita; 
		ER_ASSERT(SMresp.size()>=4,"Errore nella decodifica dell'APDU")

		resp.alloc_copy(SMresp.mid(2).revmid(2));
		_return(OK)
	}
	if (baSigResponseKey!=NULL && baEncResponseKey!=NULL) {
		// SM in ingresso e in uscita
		ER_ASSERT(SMresp.size()>=2,"Errore nella decodifica dell'APDU")

		ByteArray SW=SMresp.right(2);
		CTLV tlv(SMresp.revmid(2));
		ByteArray baNullTag;
		ByteArray *baCypherTextObject=NULL;
		ByteArray *baMACObject=NULL;
		ByteArray *baProcessingStatusObject=NULL;
		ER_CALL(tlv.getTAG(0x87,baCypherTextObject),
			"Errore nel TLV")
		ER_ASSERT(baCypherTextObject!=NULL,"Errore nella decodifica dell'APDU")

		ER_CALL(tlv.getTAG(0x8E,baMACObject),
			"Errore nel TLV")

		// se non ho il MAC c'è qualcosa che non va!
		// ovviamente in alcuni casi va solo sulla 4.2b
		if (baMACObject==NULL)
			_return(FAIL)

		ER_CALL(tlv.getTAG(0x99,baProcessingStatusObject),
			"Errore nel TLV")
		if (baProcessingStatusObject==NULL)
			baProcessingStatusObject=&baNullTag;
		else {
			ER_ASSERT(SW==baProcessingStatusObject->mid(2),"Errore nella decodifica dell'APDU")
		}

		ByteDynArray baHeaderBlock;
		ER_CALL(HeaderBlock(baRandom,baHeaderBlock),
			"Errore nell'Header Block")

		ByteDynArray baCalcMacObject;
		ER_CALL(MacObject(baHeaderBlock,*baCypherTextObject,*baProcessingStatusObject,*baSigResponseKey,baCalcMacObject),
			"Errore nel MAC Block")

		// se il MAC della carta non corrisponde a
		// quello che ho calcolato, allora la carta ha qualcosa
		// che non va!
		if (*baMACObject!=baCalcMacObject)
			_return(FAIL)

		CDES3 des(*baEncResponseKey);
		ByteDynArray responsePadded;
		des.Decode(baCypherTextObject->mid(3),responsePadded);
		DWORD dwPadLen=0;
		dwPadLen = RemoveISOPad(responsePadded);

		resp.alloc_copy(responsePadded.revmid(dwPadLen));
	}
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT APDU::EncodeSM(CToken &Token,ByteDynArray &SMDataBuffer,APDU *&apdu)
{
	init_func
	if (baSigCommandKey==NULL && baEncCommandKey==NULL) {
		// caso base; no SM
		apdu=this;
		_return(OK)
	}
	if (baSigCommandKey!=NULL && baEncCommandKey!=NULL) {
		// SM in ingresso e in uscita
		Allocator<APDU> SMapdu;
		SMapdu->btCLA=btCLA | 0x0c;
		SMapdu->btINS=btINS;
		SMapdu->btP1=btP1;
		SMapdu->btP2=btP2;
		SMapdu->btLE=0x00;
		SMapdu->bLE=true;
		SMapdu->bLC=true;
		
		ByteDynArray baChallenge(8);
		ER_CALL(Token.GetChallenge(baChallenge),
			"Errore nella richiesta del Challenge")

		// vedo se serve anche una give random (solo se sign su response)
		if (baSigResponseKey) {
			ER_CALL(GenerateRandom(baRandom),
				"Errore nella generazione del Challenge")

			ER_CALL(Token.GiveRandom(baRandom),
				"Errore nella GiveRandom")
		}
		
		ByteDynArray baCypherTextObject;
		ER_CALL(CypherTextObject(baCypherTextObject),
			"Errore nel CypherText Block")

		ByteDynArray baHeaderBlock;
		ER_CALL(HeaderBlock(baChallenge,baHeaderBlock),
			"Errore nell'Header Block")

		ByteDynArray baNetLEMACObject;

		ByteDynArray baMacObject;
		ER_CALL(MacObject(baHeaderBlock,baCypherTextObject,baNetLEMACObject,*baSigCommandKey,baMacObject),
			"Errore nel MAC Block")

		SMDataBuffer.resize(baCypherTextObject.size()+baNetLEMACObject.size()+baMacObject.size());
		SMDataBuffer.copy(baCypherTextObject);
		SMDataBuffer.copy(baNetLEMACObject,baCypherTextObject.size());
		SMDataBuffer.copy(baMacObject,baCypherTextObject.size()+baNetLEMACObject.size());

		SMapdu->btLC=SMDataBuffer.size();
		SMapdu->pbtData=SMDataBuffer.lock();
		
		apdu=SMapdu.detach();
		_return(OK)
	}
	exit_func
	_return(FAIL)
}

RESULT APDU::GenerateRandom(ByteArray &baRandom)
{
	init_func
	for (DWORD i=0;i<baRandom.size();i++)
		baRandom[i]=rand()%0x100;
	_return(OK)
	exit_func
	_return(FAIL)
}
