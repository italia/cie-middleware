#pragma once
#include "../util/array.h"

#define SHA256_DIGEST_LENGTH 32

class CSHA256
{
public:
	ByteDynArray Digest(ByteArray &data);

	void Digest(ByteArray &dataIn, ByteArray &dataOut);
};
