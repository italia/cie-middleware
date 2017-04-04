#pragma once
#include <openssl/sha.h>
#include "util.h"
#include "utilexception.h"

class CSHA1
{
public:
	ByteDynArray &Digest(ByteArray data,ByteDynArray &digest);
};
