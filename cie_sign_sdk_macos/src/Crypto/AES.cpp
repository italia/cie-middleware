
#include "AES.h"

extern CLog Log;

#ifdef WIN32

class init_aes {
public:
	BCRYPT_ALG_HANDLE algo;
	init_aes() {
		if (BCryptOpenAlgorithmProvider(&algo, BCRYPT_AES_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0) != 0)
			throw logged_error("Errore nell'inizializzazione dell'algoritmo AES");
	}
	~init_aes() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_aes;

void CAES::Init(const ByteArray &key, const ByteArray &iv)
{
	init_func
	ER_ASSERT(iv.size() == 16, "Errore nella lunghezza dell'Initial Vector")
	this->iv = iv;
	NTSTATUS ris;
	if ((ris=BCryptGenerateSymmetricKey(algo_aes.algo, &this->key, nullptr, 0, key.data(), (ULONG)key.size(), 0)) != 0)
		throw logged_error(stdPrintf("Errore nella creazione della chiave AES: %i %08x %08x", key.size(), algo_aes.algo, ris).c_str());

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

ByteDynArray CAES::AES(const ByteArray &data,int encOp)
{
	init_func

	ByteDynArray iv2 = iv;
	size_t AppSize = data.size() - 1;
	ByteDynArray resp(AppSize - (AppSize % 16) + 16);

	NTSTATUS rs;
	ULONG result = (ULONG)resp.size();
	if (encOp == AES_ENCRYPT)
		rs = BCryptEncrypt(key, data.data(), (ULONG)data.size(), nullptr, iv2.data(), (ULONG)iv2.size(), resp.data(), (ULONG)resp.size(), &result, 0);
	
	if (encOp == AES_DECRYPT)
		rs = BCryptDecrypt(key, data.data(), (ULONG)data.size(), nullptr, iv2.data(), (ULONG)iv2.size(), resp.data(), (ULONG)resp.size(), &result, 0);
	
	if (rs != 0)
		throw logged_error("Errore nella cifratura AES");

	return resp;
}
#else

ByteDynArray CAES::AES(const ByteArray &data,int encOp)
{
	init_func

	ByteDynArray iv2 = iv;

    
	AES_KEY aesKey;
	if (encOp == AES_ENCRYPT)
		AES_set_encrypt_key(key.data(), (int)key.size() * 8, &aesKey);
	else
		AES_set_decrypt_key(key.data(), (int)key.size() * 8, &aesKey);
	size_t AppSize = data.size() - 1;
	ByteDynArray resp(AppSize - (AppSize % 16) + 16);
	AES_cbc_encrypt(data.data(), resp.data(), data.size(), &aesKey, iv2.data(), encOp);

	return resp;
}
void CAES::Init(const ByteArray &key, const ByteArray &iv)
{
	init_func
	this->iv = iv;
	this->key = key;

	exit_func
}

CAES::CAES() {
}

CAES::~CAES(void)
{
}
#endif

CAES::CAES(const ByteArray &key, const ByteArray &iv) {
	Init(key, iv);
}


ByteDynArray CAES::Encode(const ByteArray &data)
{
	init_func
	return AES(ISOPad16(data), AES_ENCRYPT);
}

ByteDynArray CAES::RawEncode(const ByteArray &data)
{
	init_func
	ER_ASSERT((data.size() % AES_BLOCK_SIZE) == 0, "La dimensione dei dati da cifrare deve essere multipla di 16");
	return AES(data, AES_ENCRYPT);
}

ByteDynArray CAES::Decode(const ByteArray &data)
{
	init_func
	ByteDynArray result=AES(data, AES_DECRYPT);
	result.resize(RemoveISOPad(result),true);
	return result;
}

ByteDynArray CAES::RawDecode(const ByteArray &data)
{
	init_func
	ER_ASSERT((data.size() % AES_BLOCK_SIZE) == 0, "La dimensione dei dati da cifrare deve essere multipla di 16");
	return AES(data, AES_DECRYPT);
}
