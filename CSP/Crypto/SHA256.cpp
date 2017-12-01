#include "..\stdafx.h"
#include ".\sha256.h"

static char *szCompiledFile=__FILE__;

class init_sha256 {
public:
	BCRYPT_ALG_HANDLE algo;
	init_sha256() {
		BCryptOpenAlgorithmProvider(&algo, BCRYPT_SHA256_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0);
	}
	~init_sha256() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_sha256;

ByteDynArray CSHA256::Digest(ByteArray &data)
{
	ByteDynArray resp(SHA256_DIGEST_LENGTH);
	ER_ASSERT(SHA256(data.data(), data.size(), resp.data()) != NULL, "Errore nel calcolo dello SHA256")

	BCRYPT_HASH_HANDLE hash;
	BCryptCreateHash(algo_sha256.algo, &hash, nullptr, 0, nullptr, 0, 0);
	ByteDynArray resp2(SHA256_DIGEST_LENGTH);
	BCryptHashData(hash, data.data(), (ULONG)data.size(), 0);
	BCryptFinishHash(hash, resp2.data(), (ULONG)resp2.size(), 0);
	BCryptDestroyHash(hash);

	if (resp != resp2) {
		ODS("Crypt Err");
	}

	return resp;
}