#include "../stdafx.h"
#include "mac.h"
#include "../util/util.h"

static char *szCompiledFile=__FILE__;

class init_mac {
public:
	BCRYPT_ALG_HANDLE algo;
	init_mac() {
		BCryptOpenAlgorithmProvider(&algo, BCRYPT_3DES_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0);
	}
	~init_mac() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_mac;

CMAC::CMAC() {
}

CMAC::CMAC(ByteArray &key) {
	Init(key);
}

void CMAC::Init(ByteArray &key)
{
	init_func
	size_t dwKeySize=key.size();
	ER_ASSERT((dwKeySize % 8) == 0,"Errore nella lunghezza della chiave MAC (non divisibile per 8)")
		
	ER_ASSERT(dwKeySize>=8 && dwKeySize<=32,"Errore nella lunghezza della chiave MAC (<8 o >32)")
	des_cblock *keyVal1 = nullptr, *keyVal2 = nullptr, *keyVal3 = nullptr;

	switch (dwKeySize) {
		case 8:
			memset(initVec,0,8);
			keyVal1 = keyVal2 = keyVal3 = (des_cblock *)key.data();
			break;
		case 16:
			memset(initVec,0,8);
			keyVal1 = keyVal3 = (des_cblock *)key.data();
			keyVal2 = (des_cblock *)key.mid(8).data();
			break;		
		case 24:
			memset(initVec,0,8);
			keyVal1 = (des_cblock *)key.data();
			keyVal2 = (des_cblock *)key.mid(8).data();
			keyVal3 = (des_cblock *)key.mid(16).data();
			break;
		case 32:
			memcpy_s(initVec, sizeof(des_cblock), key.mid(24).data(), 8);
			keyVal1 = (des_cblock *)key.data();
			keyVal2 = (des_cblock *)key.mid(8).data();
			keyVal3 = (des_cblock *)key.mid(16).data();
	}

	des_set_key(keyVal1,k1);
	des_set_key(keyVal2,k2);
	des_set_key(keyVal3,k3);

	ByteDynArray BCrytpKey;
	switch (dwKeySize) {
	case 8:
		BCrytpKey.set(&key, &key, &key);
		break;
	case 16:
		BCrytpKey.set(&key, &key.left(8));
		break;
	case 24:
		BCrytpKey = key;
		break;
	case 32:
		BCrytpKey = key.left(24);
		break;
	}

	ByteDynArray k1;
	k1.set(&key.left(8), &key.left(8), &key.left(8));

	BCryptGenerateSymmetricKey(algo_mac.algo, &this->key1, nullptr, 0, k1.data(), (ULONG)k1.size(), 0);
	BCryptGenerateSymmetricKey(algo_mac.algo, &this->key2, nullptr, 0, BCrytpKey.data(), (ULONG)BCrytpKey.size(), 0);

	exit_func
}

CMAC::~CMAC(void)
{
	BCryptDestroyKey(key1);
	BCryptDestroyKey(key2);
}

ByteDynArray CMAC::Mac(const ByteArray &data)
{
	init_func
	ByteDynArray result;
	ER_CALL(_Mac(data,result),"Errore nel calcolo del MAC");
	_return (result)
	exit_func
}

DWORD CMAC::_Mac(const ByteArray &data,ByteDynArray &resp)
{
	init_func

	des_cblock iv;
	memcpy_s(iv, sizeof(des_cblock), initVec, sizeof(initVec));

	size_t dwANSILen=ANSIPadLen(data.size());
	ByteDynArray iv2(8);
	if (data.size()>8) {
		ByteDynArray baOutTmp(dwANSILen-8);
		des_ncbc_encrypt(data.data(), baOutTmp.data(), (long)dwANSILen - 8, k1, &iv, DES_ENCRYPT);

		iv2.fill(0);
		ByteDynArray baOutTmp2(dwANSILen - 8);
		ULONG result = (ULONG)baOutTmp2.size();
		BCryptEncrypt(key1, data.data(), (long)dwANSILen - 8, nullptr, iv2.data(), (ULONG)iv2.size(), baOutTmp2.data(), (ULONG)baOutTmp2.size(), &result, 0);
		if (baOutTmp != baOutTmp2) {
			ODS("Crypt Err");
			return OK;
		}
	}
	uint8_t dest[8];
	des_ede3_cbc_encrypt(data.mid(dwANSILen - 8).data(), dest, (long)(data.size() - dwANSILen) + 8, k1, k2, k3, &iv, DES_ENCRYPT);
	resp = ByteArray(dest, 8);

	ByteDynArray resp2(8);
	ULONG result = (ULONG)resp2.size();
	BCryptEncrypt(key2, data.mid(dwANSILen - 8).data(), (long)(data.size() - dwANSILen) + 8, nullptr, iv2.data(), (ULONG)iv2.size(), resp2.data(), (ULONG)resp2.size(), &result, 0);

	if (resp != resp2) {
		ODS("Crypt Err");
		return OK;
	}

	_return(OK)
	exit_func
	_return(FAIL)
}