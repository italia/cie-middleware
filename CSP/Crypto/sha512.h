#pragma once
#include "../util/array.h"

#define SHA512_DIGEST_LENGTH 64

class CSHA512
{
	public:
		ByteDynArray Digest(ByteArray &data);
};
