#include "../stdafx.h"
#include "SHA1.h"
#include "../util/util.h"

static char *szCompiledFile=__FILE__;

class init_sha1 {
public:
	BCRYPT_ALG_HANDLE algo;
	init_sha1() {
		BCryptOpenAlgorithmProvider(&algo, BCRYPT_SHA1_ALGORITHM, MS_PRIMITIVE_PROVIDER, BCRYPT_HASH_REUSABLE_FLAG);
	}
	~init_sha1() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_sha1;

ByteDynArray CSHA1::Digest(ByteArray data)
{
	ByteDynArray digest(SHA_DIGEST_LENGTH);
	SHA1(data.data(), data.size(), digest.data());

	BCRYPT_HASH_HANDLE hash;
	BCryptCreateHash(algo_sha1.algo, &hash, nullptr, 0, nullptr, 0, BCRYPT_HASH_REUSABLE_FLAG);
	ByteDynArray resp2(SHA_DIGEST_LENGTH);
	BCryptHashData(hash, data.data(), (ULONG)data.size(), 0);
	BCryptFinishHash(hash, resp2.data(), (ULONG)resp2.size(), 0);
	BCryptDestroyHash(hash);

	if (digest != resp2) {
		ODS("Crypt Err");
	}

	return digest;
}
