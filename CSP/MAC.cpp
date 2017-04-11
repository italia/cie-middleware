#include "stdafx.h"
#include "mac.h"
#include "util.h"

static char *szCompiledFile=__FILE__;

CMAC::CMAC() {
}

CMAC::CMAC(ByteArray &key) {
	Init(key);
}

void CMAC::Init(ByteArray &key)
{
	init_func
	DWORD dwKeySize=key.size();
	ER_ASSERT((dwKeySize % 8) == 0,"Errore nella lunghezza della chiave MAC (non divisibile per 8)")
		
	ER_ASSERT(dwKeySize>=8 && dwKeySize<=32,"Errore nella lunghezza della chiave MAC (<8 o >32)")

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
			memcpy_s(initVec, sizeof(des_cblock), key.lock(8, 24), 8);
			keyVal1=(des_cblock *)key.lock(8);
			keyVal2=(des_cblock *)key.lock(8,8);
			keyVal3=(des_cblock *)key.lock(8,16);
	}

	des_set_key(keyVal1,k1);
	des_set_key(keyVal2,k2);
	des_set_key(keyVal3,k3);
	exit_func
}

CMAC::~CMAC(void)
{
}

const ByteDynArray &CMAC::Mac(const ByteArray &data,ByteDynArray &result)
{
	init_func
	ER_CALL(_Mac(data,result),"Errore nel calcolo del MAC");
	_return (result)
	exit_func
}

DWORD CMAC::_Mac(const ByteArray &data,ByteDynArray &resp)
{
	init_func

	des_cblock iv;
	memcpy_s(iv, sizeof(des_cblock), initVec, sizeof(initVec));

	DWORD dwANSILen=ANSIPadLen(data.size());
	if (data.size()>8) {
		ByteDynArray baOutTmp(dwANSILen-8);
		des_ncbc_encrypt(data.lock(dwANSILen-8),baOutTmp.lock(),dwANSILen-8,k1,&iv,DES_ENCRYPT);
	}
	BYTE dest[8];
	des_ede3_cbc_encrypt(data.lock(-1,dwANSILen-8),dest,data.size()-dwANSILen+8,k1,k2,k3,&iv,DES_ENCRYPT);
	resp.alloc_copy(dest,8);

	_return(OK)
	exit_func
	_return(FAIL)
}