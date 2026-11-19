#pragma once

#ifdef WIN32
#include <bcrypt.h>
#define SHA_DIGEST_LENGTH 20
#else
#include <openssl/sha.h>
#endif

#include "../Util/Array.h"

#define SHA512_DIGEST_LENGTH 64

class CSHA512
{
#ifndef WIN32
    bool isInit;
    SHA512_CTX ctx;
    void Init();
    void Update(ByteArray data);
    ByteDynArray Final();

#endif

public:
	ByteDynArray Digest(ByteArray &data);
};
