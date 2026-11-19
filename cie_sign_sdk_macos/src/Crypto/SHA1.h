#pragma once

#ifdef WIN32
#include <bcrypt.h>
#define SHA_DIGEST_LENGTH 20
#else
#include <openssl/sha.h>
#endif

#include "../Util/util.h"
#include "../Util/UtilException.h"

class CSHA1
{
#ifdef WIN32
	BCRYPT_HASH_HANDLE hash;
#else
	bool isInit;
	SHA_CTX ctx;
#endif
public:
	CSHA1();
	~CSHA1(void);

	ByteDynArray Digest(ByteArray data);

	void Init();
	void Update(ByteArray data);
	ByteDynArray Final();
};
