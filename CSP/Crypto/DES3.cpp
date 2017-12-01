#include "..\stdafx.h"
#include ".\des3.h"

static char *szCompiledFile = __FILE__;

class init_3des {
public:
	BCRYPT_ALG_HANDLE algo;
	init_3des() {
		BCryptOpenAlgorithmProvider(&algo, BCRYPT_3DES_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0);
	}
	~init_3des() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_3des;

CDES3::CDES3() {
}

CDES3::CDES3(const ByteArray &key) {
	Init(key);
}

void CDES3::Init(const ByteArray &key)
{
	init_func
		size_t dwKeySize = key.size();
	ER_ASSERT((dwKeySize % 8) == 0, "Errore nella lunghezza della chiave DES (non divisibile per 8)")

		ER_ASSERT(dwKeySize >= 8 && dwKeySize <= 32, "Errore nella lunghezza della chiave DES (<8 o >32)")
		des_cblock *keyVal1 = nullptr, *keyVal2 = nullptr, *keyVal3 = nullptr;

	switch (dwKeySize) {
	case 8:
		memset(initVec, 0, 8);
		keyVal1 = keyVal2 = keyVal3 = (des_cblock *)key.data();
		break;
	case 16:
		memset(initVec, 0, 8);
		keyVal1 = keyVal3 = (des_cblock *)key.data();
		keyVal2 = (des_cblock *)key.mid(8).data();
		break;
	case 24:
		memset(initVec, 0, 8);
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

	des_set_key(keyVal1, k1);
	des_set_key(keyVal2, k2);
	des_set_key(keyVal3, k3);



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

	BCryptGenerateSymmetricKey(algo_3des.algo, &this->key, nullptr, 0, BCrytpKey.data(), (ULONG)BCrytpKey.size(), 0);

	exit_func
}

CDES3::~CDES3(void)
{
	BCryptDestroyKey(key);
}

ByteDynArray CDES3::Encode(const ByteArray &data)
{
	init_func
		ByteDynArray result;
	ER_CALL(Des3(ISOPad(data), result, DES_ENCRYPT), "Errore della cifratura DES");
	_return(result)
		exit_func
}

ByteDynArray CDES3::RawEncode(const ByteArray &data)
{
	init_func
		ByteDynArray result;
	ER_ASSERT((data.size() % 8) == 0, "La dimensione dei dati da cifrare deve essere multipla di 8")
		ER_CALL(Des3(data, result, DES_ENCRYPT), "Errore della cifratura DES");
	_return(result)
		exit_func
}

ByteDynArray CDES3::Decode(const ByteArray &data)
{
	init_func
		ByteDynArray result;
	ER_CALL(Des3(data, result, DES_DECRYPT), "Errore della decifratura DES");
	result.resize(RemoveISOPad(result), true);
	_return(result)
		exit_func
}

ByteDynArray CDES3::RawDecode(const ByteArray &data)
{
	init_func
		ByteDynArray result;
	ER_ASSERT((data.size() % 8) == 0, "La dimensione dei dati da cifrare deve essere multipla di 8")
		ER_CALL(Des3(data, result, DES_DECRYPT), "Errore della decifratura DES");
	_return(result)
		exit_func
}

DWORD CDES3::Des3(const ByteArray &data, ByteDynArray &resp, int encOp)
{
	init_func

		des_cblock iv;
	memcpy_s(iv, sizeof(des_cblock), initVec, sizeof(initVec));
	size_t dwAppSize = data.size() - 1;
	resp.resize(dwAppSize - (dwAppSize % 8) + 8);
	des_ede3_cbc_encrypt(data.data(), resp.data(), (long)data.size(), k1, k2, k3, &iv, encOp);

	ByteDynArray iv2(8), resp2(resp.size());
	iv2.fill(0);
	ULONG result = (ULONG)resp2.size();
	if (encOp == DES_ENCRYPT) {
		BCryptEncrypt(key, data.data(), (ULONG)data.size(), nullptr, iv2.data(), (ULONG)iv2.size(), resp2.data(), (ULONG)resp2.size(), &result, 0);
		if (resp != resp2) {
			ODS("Crypt Err");
			return OK;
		}
	}

	if (encOp == DES_DECRYPT) {
		BCryptDecrypt(key, data.data(), (ULONG)data.size(), nullptr, iv2.data(), (ULONG)iv2.size(), resp2.data(), (ULONG)resp2.size(), &result, 0);
		if (resp != resp2) {
			ODS("Crypt Err");
			return OK;
		}
	}

	_return(OK)
		exit_func
		_return(FAIL)
}