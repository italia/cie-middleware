#pragma once
#include <openssl\sha.h>
#include "array.h"

#define SHA256_DIGEST_LENGTH 32

class CSHA256
{
public:
	CSHA256(void);
	~CSHA256(void);

	RESULT Digest(ByteArray &data,ByteDynArray &resp);
};
