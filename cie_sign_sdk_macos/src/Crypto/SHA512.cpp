#include "sha512.h"



#ifdef WIN32

static char *szCompiledFile=__FILE__;

class init_sha512 {
public:
	BCRYPT_ALG_HANDLE algo;
	init_sha512() {
		if (BCryptOpenAlgorithmProvider(&algo, BCRYPT_SHA512_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0) != 0)
			throw logged_error("Errore nell'inizializzazione dell'algoritmo SHA512");
	}
	~init_sha512() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_sha512;

ByteDynArray CSHA512::Digest(ByteArray &data)
{
	BCRYPT_HASH_HANDLE hash;
	if (BCryptCreateHash(algo_sha512.algo, &hash, nullptr, 0, nullptr, 0, 0) != 0)
		throw logged_error("Errore nella creazione dell'hash SHA512");
	ByteDynArray resp(SHA512_DIGEST_LENGTH);
	if (BCryptHashData(hash, data.data(), (ULONG)data.size(), 0) != 0)
		throw logged_error("Errore nell'hash dei dati SHA512");
	if (BCryptFinishHash(hash, resp.data(), (ULONG)resp.size(), 0) != 0)
		throw logged_error("Errore nel calcolo dell'hash SHA512");
	BCryptDestroyHash(hash);

	return resp;
}

#else

#include "cryptopp/sha.h"
#include "cryptopp/filters.h"
#include "cryptopp/base64.h"

void CSHA512::Init() {
//    if (isInit)
//    throw logged_error("Un'operazione di hash Ë gi‡ in corso");
    SHA512_Init(&ctx);
    isInit = true;
}
void CSHA512::Update(ByteArray data) {
    if (!isInit)
    throw logged_error("Hash non inizializzato");
    SHA512_Update(&ctx, data.data(), data.size());
}
ByteDynArray CSHA512::Final() {
    if (!isInit)
    throw logged_error("Hash non inizializzato");
    ByteDynArray resp(SHA_DIGEST_LENGTH);
    SHA512_Final(resp.data(), &ctx);
    isInit = false;
    
    return resp;
}

ByteDynArray CSHA512::Digest(ByteArray &data)
{
	const BYTE* pbData = (BYTE*) data.data();
	unsigned int nDataLen = data.size();
	BYTE abDigest[CryptoPP::SHA512::DIGESTSIZE];

	CryptoPP::SHA512().CalculateDigest(abDigest, pbData, nDataLen);

	ByteArray resp(abDigest, CryptoPP::SHA512::DIGESTSIZE);

//	ByteDynArray resp(SHA512_DIGEST_LENGTH);
//	SHA512_Init(&ctx);
//	SHA512_Update(&ctx, data.data(), data.size());
//	SHA512_Final(resp.data(), &ctx);

	//ER_ASSERT(SHA512(data.data(), data.size(), resp.data()) != NULL, "Errore nel calcolo dello SHA512")

	return resp;
}
#endif
