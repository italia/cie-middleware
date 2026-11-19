
#include "DES3.h"
#include "cryptopp/misc.h"

extern CLog Log;



#ifdef WIN32
static char *szCompiledFile = __FILE__;
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

void CDES3::Init(const ByteArray &key, const ByteArray &iv)
{
	init_func
	size_t KeySize = key.size();
	ER_ASSERT((KeySize % 8) == 0, "Errore nella lunghezza della chiave DES (non divisibile per 8)")
	ER_ASSERT(iv.size() == 8, "Errore nella lunghezza dell'Initial Vector")
	ER_ASSERT(KeySize >= 8 && KeySize <= 24, "Errore nella lunghezza della chiave DES (<8 o >24)")

	ByteDynArray BCrytpKey;
	this->iv = iv;
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
 
void CDES3::Init(const ByteArray &key, const ByteArray &iv)
{
	init_func
	long KeySize = key.size();
//	ER_ASSERT((KeySize % 8) == 0, "Errore nella lunghezza della chiave DES (non divisibile per 8)")
//	ER_ASSERT(iv.size() == 8, "Errore nella lunghezza dell'Initial Vector")

//	ER_ASSERT(KeySize >= 8 && KeySize <= 24, "Errore nella lunghezza della chiave DES (<8 o >24)")
	DES_cblock *keyVal1 = nullptr, *keyVal2 = nullptr, *keyVal3 = nullptr;
    CryptoPP::memcpy_s(initVec, sizeof(DES_cblock), iv.data(), 8);

	switch (KeySize) {
	case 8:
		throw logged_error("Chiave 3DES 8 byte non supportata");
		//keyVal1 = keyVal2 = keyVal3 = (DES_cblock *)key.data();
		break;
	case 16:
        keyVal1 = keyVal3 = (DES_cblock *)key.left(8).data();//data();
		keyVal2 = (DES_cblock *)key.mid(8, 8).data();
		break;
	case 24:
		keyVal1 = (DES_cblock *)key.left(8).data();
		keyVal2 = (DES_cblock *)key.mid(8, 8).data();
		keyVal3 = (DES_cblock *)key.mid(16, 8).data();
		break;
	}

    DES_set_key(keyVal1, &k1);
    DES_set_key(keyVal2, &k2);
    DES_set_key(keyVal3, &k3);
    
//    DES_set_key(keyVal1, k1);
//    DES_set_key(keyVal2, k2);
//    DES_set_key(keyVal3, k3);

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

	DES_cblock iv;
	CryptoPP::memcpy_s(iv, sizeof(DES_cblock), initVec, sizeof(initVec));
	size_t AppSize = data.size() - 1;
	ByteDynArray resp(AppSize - (AppSize % 8) + 8);
	DES_ede3_cbc_encrypt(data.data(), resp.data(), (long)data.size(), &k1, &k2, &k3, &iv, encOp);

    return resp;
}
#endif

CDES3::CDES3(const ByteArray &key, const ByteArray &iv) {
	Init(key,iv);
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
