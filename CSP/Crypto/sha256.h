#pragma once
#include "../util/array.h"

#define SHA256_DIGEST_LENGTH 32

class CSHA256
{
public:
	ByteDynArray Digest(const ByteArray &data);

	void Digest(const ByteArray &dataIn, ByteArray &dataOut);
};
