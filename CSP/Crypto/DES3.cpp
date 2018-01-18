#include "..\stdafx.h"
#include ".\des3.h"

static char *szCompiledFile = __FILE__;

#ifdef WIN32

class init_3des {
public:
	BCRYPT_ALG_HANDLE algo;
	init_3des() {
		if (BCryptOpenAlgorithmProvider(&algo, BCRYPT_3DES_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0) != 0)
			throw logged_error("Errore nell'inizializzazione dell'algoritmo DES");
	}
	~init_3des() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_3des;

void CDES3::Init(const ByteArray &key)
{
	init_func
	size_t KeySize = key.size();
	ER_ASSERT((KeySize % 8) == 0, "Errore nella lunghezza della chiave DES (non divisibile per 8)")

	ER_ASSERT(KeySize >= 8 && KeySize <= 32, "Errore nella lunghezza della chiave DES (<8 o >32)")

	ByteDynArray BCrytpKey;
	iv.resize(8);
	iv.fill(0);
	switch (KeySize) {
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
		iv = key.right(8);
		break;
	}

	if (BCryptGenerateSymmetricKey(algo_3des.algo, &this->key, nullptr, 0, BCrytpKey.data(), (ULONG)BCrytpKey.size(), 0) != 0)
		throw logged_error("Errore nella creazione della chiave DES");

	exit_func
}

CDES3::~CDES3(void)
{
	if (key!=nullptr)
		BCryptDestroyKey(key);
}

CDES3::CDES3(void) : key(nullptr)
{
}

ByteDynArray CDES3::Des3(const ByteArray &data, int encOp)
{
	init_func

	size_t AppSize = data.size() - 1;
	ByteDynArray resp(AppSize - (AppSize % 8) + 8);

	NTSTATUS rs;
	ByteDynArray iv2 = iv;
	ULONG result = (ULONG)resp.size();
	if (encOp == DES_ENCRYPT) 
		rs = BCryptEncrypt(key, data.data(), (ULONG)data.size(), nullptr, iv2.data(), (ULONG)iv2.size(), resp.data(), (ULONG)resp.size(), &result, 0);	

	if (encOp == DES_DECRYPT) 
		rs = BCryptDecrypt(key, data.data(), (ULONG)data.size(), nullptr, iv2.data(), (ULONG)iv2.size(), resp.data(), (ULONG)resp.size(), &result, 0);	

	if (rs != 0)
		throw logged_error("Errore nella cifratura DES");

	return resp;
}

#else

void CDES3::Init(const ByteArray &key)
{
	init_func
	size_t KeySize = key.size();
	ER_ASSERT((KeySize % 8) == 0, "Errore nella lunghezza della chiave DES (non divisibile per 8)")

		ER_ASSERT(KeySize >= 8 && KeySize <= 32, "Errore nella lunghezza della chiave DES (<8 o >32)")
		des_cblock *keyVal1 = nullptr, *keyVal2 = nullptr, *keyVal3 = nullptr;

	switch (KeySize) {
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

	exit_func
}

CDES3::~CDES3(void)
{
}

CDES3::CDES3() 
{
}

ByteDynArray CDES3::Des3(const ByteArray &data, int encOp)
{
	init_func

	des_cblock iv;
	memcpy_s(iv, sizeof(des_cblock), initVec, sizeof(initVec));
	size_t AppSize = data.size() - 1;
	ByteDynArray resp(AppSize - (AppSize % 8) + 8);
	des_ede3_cbc_encrypt(data.data(), resp.data(), (long)data.size(), k1, k2, k3, &iv, encOp);

	return resp;
}
#endif

CDES3::CDES3(const ByteArray &key) {
	Init(key);
}

ByteDynArray CDES3::Encode(const ByteArray &data)
{
	init_func
	return Des3(ISOPad(data), DES_ENCRYPT);
}

ByteDynArray CDES3::RawEncode(const ByteArray &data)
{
	init_func
	ByteDynArray result;
	ER_ASSERT((data.size() % 8) == 0, "La dimensione dei dati da cifrare deve essere multipla di 8");
	return Des3(data, DES_ENCRYPT);
}

ByteDynArray CDES3::Decode(const ByteArray &data)
{
	init_func
	auto result=Des3(data, DES_DECRYPT);
	result.resize(RemoveISOPad(result), true);
	return result;
}

ByteDynArray CDES3::RawDecode(const ByteArray &data)
{
	init_func
	ByteDynArray result;
	ER_ASSERT((data.size() % 8) == 0, "La dimensione dei dati da cifrare deve essere multipla di 8");
	return Des3(data, DES_DECRYPT);
}
