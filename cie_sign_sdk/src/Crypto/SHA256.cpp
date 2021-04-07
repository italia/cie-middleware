#include "stdafx.h"
#include ".\sha256.h"

static char *szCompiledFile=__FILE__;

#ifdef WIN32

class init_sha256 {
public:
	BCRYPT_ALG_HANDLE algo;
	init_sha256() {
		if (BCryptOpenAlgorithmProvider(&algo, BCRYPT_SHA256_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0) != 0)
			throw logged_error("Errore nell'inizializzazione dell'algoritmo SHA256");
	}
	~init_sha256() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_sha256;

ByteDynArray CSHA256::Digest(ByteArray &data)
{
	BCRYPT_HASH_HANDLE hash;
	if (BCryptCreateHash(algo_sha256.algo, &hash, nullptr, 0, nullptr, 0, 0) != 0)
		throw logged_error("Errore nella creazione dell'hash SHA256");
	ByteDynArray resp(SHA256_DIGEST_LENGTH);
	if (BCryptHashData(hash, data.data(), (ULONG)data.size(), 0) != 0)
		throw logged_error("Errore nell'hash dei dati SHA256");
	if (BCryptFinishHash(hash, resp.data(), (ULONG)resp.size(), 0) != 0)
		throw logged_error("Errore nel calcolo dell'hash SHA256");
	BCryptDestroyHash(hash);

	return resp;
}

void CSHA256::Digest(ByteArray &dataIn, ByteArray &dataOut)
{
	BCRYPT_HASH_HANDLE hash;
	if (BCryptCreateHash(algo_sha256.algo, &hash, nullptr, 0, nullptr, 0, 0) != 0)
		throw logged_error("Errore nella creazione dell'hash SHA256");
	ByteDynArray resp(SHA256_DIGEST_LENGTH);
	if (BCryptHashData(hash, dataIn.data(), (ULONG)dataIn.size(), 0) != 0)
		throw logged_error("Errore nell'hash dei dati SHA256");
	if (BCryptFinishHash(hash, dataOut.data(), (ULONG)dataOut.size(), 0) != 0)
		throw logged_error("Errore nel calcolo dell'hash SHA256");
	BCryptDestroyHash(hash);
}

#else
	
ByteDynArray CSHA256::Digest(ByteArray &data)
{
	ByteDynArray resp(SHA256_DIGEST_LENGTH);
	ER_ASSERT(SHA256(data.data(), data.size(), resp.data()) != NULL, "Errore nel calcolo dello SHA256")

	return resp;
}
#endif