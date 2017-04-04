#include "stdafx.h"
#include ".\des3.h"

static char *szCompiledFile=__FILE__;

CDES3::CDES3() {
}

CDES3::CDES3(const ByteArray &key) {
	Init(key);
}

void CDES3::Init(const ByteArray &key)
{
	init_func
	DWORD dwKeySize=key.size();
	ER_ASSERT((dwKeySize % 8) == 0,"Errore nella lunghezza della chiave DES (non divisibile per 8)")
		
	ER_ASSERT(dwKeySize>=8 && dwKeySize<=32,"Errore nella lunghezza della chiave DES (<8 o >32)")
	des_cblock *keyVal1 = nullptr, *keyVal2 = nullptr, *keyVal3 = nullptr;

	switch (dwKeySize) {
		case 8:
			memset(initVec,0,8);
			keyVal1=keyVal2=keyVal3=(des_cblock *)key.lock(8);
			break;
		case 16:
			memset(initVec,0,8);
			keyVal1=keyVal3=(des_cblock *)key.lock(8);
			keyVal2=(des_cblock *)key.lock(8,8);
			break;		
		case 24:
			memset(initVec,0,8);
			keyVal1=(des_cblock *)key.lock(8);
			keyVal2=(des_cblock *)key.lock(8,8);
			keyVal3=(des_cblock *)key.lock(8,16);
			break;
		case 32:
			memcpy(initVec,key.lock(8,24),8);
			keyVal1=(des_cblock *)key.lock(8);
			keyVal2=(des_cblock *)key.lock(8,8);
			keyVal3=(des_cblock *)key.lock(8,16);
	}

	des_set_key(keyVal1,k1);
	des_set_key(keyVal2,k2);
	des_set_key(keyVal3,k3);

	exit_func
}

CDES3::~CDES3(void)
{
}

const ByteDynArray &CDES3::Encode(const ByteArray &data,ByteDynArray &result)
{
	init_func
	ByteDynArray inData;
	ER_CALL(Des3(ISOPad(data,inData),result,DES_ENCRYPT),"Errore della cifratura DES");
	_return (result)
	exit_func
}

const ByteDynArray &CDES3::RawEncode(const ByteArray &data,ByteDynArray &result)
{
	init_func
	ER_ASSERT((data.size()%8)==0,"La dimensione dei dati da cifrare deve essere multipla di 8")
	ER_CALL(Des3(data,result,DES_ENCRYPT),"Errore della cifratura DES");
	_return (result)
	exit_func
}

const ByteDynArray &CDES3::Decode(const ByteArray &data,ByteDynArray &result)
{
	init_func
	ER_CALL(Des3(data,result,DES_DECRYPT),"Errore della decifratura DES");
	result.resize(RemoveISOPad(result),true);
	_return (result)
	exit_func
}

const ByteDynArray &CDES3::RawDecode(const ByteArray &data,ByteDynArray &result)
{
	init_func
	ER_ASSERT((data.size()%8)==0,"La dimensione dei dati da cifrare deve essere multipla di 8")
	ER_CALL(Des3(data,result,DES_DECRYPT),"Errore della decifratura DES");
	_return (result)
	exit_func
}

DWORD CDES3::Des3(const ByteArray &data,ByteDynArray &resp,int encOp)
{
	init_func

	des_cblock iv;
	memcpy(iv,initVec,sizeof(initVec));
	DWORD dwAppSize=data.size()-1;
	resp.resize(dwAppSize-(dwAppSize % 8)+8);
	des_ede3_cbc_encrypt(data.lock(),resp.lock(),data.size(),k1,k2,k3,&iv,encOp);
	_return(OK)
	exit_func
	_return(FAIL)
}