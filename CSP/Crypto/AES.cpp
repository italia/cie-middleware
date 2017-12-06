#include "..\stdafx.h"
#include ".\aes.h"

static char *szCompiledFile=__FILE__;

#ifdef WIN32

class init_aes {
public:
	BCRYPT_ALG_HANDLE algo;
	init_aes() {
		if (BCryptOpenAlgorithmProvider(&algo, BCRYPT_AES_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0) != 0)
			throw std::runtime_error("Errore nell'inizializzazione dell'algoritmo AES");
	}
	~init_aes() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_aes;

void CAES::Init(const ByteArray &key)
{
	init_func
	iv.resize(16, false);

	if (BCryptGenerateSymmetricKey(algo_aes.algo, &this->key, nullptr, 0, key.data(), (ULONG)key.size(), 0) != 0)
		throw std::runtime_error("Errore nella creazione della chiave AES");

	exit_func
}

CAES::CAES() : key(nullptr) 
{
}

CAES::~CAES(void)
{
	if (key!=nullptr)
		BCryptDestroyKey(key);
}

DWORD CAES::AES(const ByteArray &data,ByteDynArray &resp,int encOp)
{
	init_func

	iv.fill(0);
	size_t AppSize = data.size() - 1;
	resp.resize(AppSize - (AppSize % 16) + 16);

	NTSTATUS rs;
	ULONG result = (ULONG)resp.size();
	if (encOp == AES_ENCRYPT)
		rs = BCryptEncrypt(key, data.data(), (ULONG)data.size(), nullptr, iv.data(), (ULONG)iv.size(), resp.data(), (ULONG)resp.size(), &result, 0);
	
	if (encOp == AES_DECRYPT)
		rs = BCryptDecrypt(key, data.data(), (ULONG)data.size(), nullptr, iv.data(), (ULONG)iv.size(), resp.data(), (ULONG)resp.size(), &result, 0);
	
	if (rs != 0)
		throw std::runtime_error("Errore nella cifratura AES");

	_return(OK)
	exit_func
	_return(FAIL)
}
#else

DWORD CAES::AES(const ByteArray &data,ByteDynArray &resp,int encOp)
{
	init_func

		iv.fill(0);

	AES_KEY aesKey;
	if (encOp == AES_ENCRYPT)
		AES_set_encrypt_key(key.data(), (int)key.size() * 8, &aesKey);
	else
		AES_set_decrypt_key(key.data(), (int)key.size() * 8, &aesKey);
	size_t AppSize = data.size() - 1;
	resp.resize(AppSize - (AppSize % 16) + 16);
	AES_cbc_encrypt(data.data(), resp.data(), data.size(), &aesKey, iv.data(), encOp);

	_return(OK)
		exit_func
		_return(FAIL)
}
void CAES::Init(const ByteArray &key)
{
	init_func
	iv.resize(16, false);
	this->key = key;

	exit_func
}

CAES::CAES() {
}

CAES::~CAES(void)
{
}
#endif

CAES::CAES(const ByteArray &key) {
	Init(key);
}


ByteDynArray CAES::Encode(const ByteArray &data)
{
	init_func
	ByteDynArray result;
	ER_CALL(AES(ISOPad16(data), result, AES_ENCRYPT), "Errore della cifratura AES");
	_return (result)
	exit_func
}

ByteDynArray CAES::RawEncode(const ByteArray &data)
{
	init_func
	ByteDynArray result;
	ER_ASSERT((data.size() % AES_BLOCK_SIZE) == 0, "La dimensione dei dati da cifrare deve essere multipla di 16")
	ER_CALL(AES(data,result,AES_ENCRYPT),"Errore della cifratura AES");
	_return (result)
	exit_func
}

ByteDynArray CAES::Decode(const ByteArray &data)
{
	init_func
	ByteDynArray result;
	ER_CALL(AES(data, result, AES_DECRYPT), "Errore della decifratura AES");
	result.resize(RemoveISOPad(result),true);
	_return (result)
	exit_func
}

ByteDynArray CAES::RawDecode(const ByteArray &data)
{
	init_func
	ByteDynArray result;
	ER_ASSERT((data.size() % AES_BLOCK_SIZE) == 0, "La dimensione dei dati da cifrare deve essere multipla di 16")
	ER_CALL(AES(data, result, AES_DECRYPT), "Errore della decifratura DES");
	_return (result)
	exit_func
}
