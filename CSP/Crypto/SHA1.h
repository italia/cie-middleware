#pragma once
#include <openssl/sha.h>
#include "../util/util.h"
#include "../util/utilexception.h"

class CSHA1
{
public:
	ByteDynArray &Digest(ByteArray data,ByteDynArray &digest);
};
