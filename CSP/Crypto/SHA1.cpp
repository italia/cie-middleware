#include "../stdafx.h"
#include "SHA1.h"
#include "../util/util.h"

static char *szCompiledFile=__FILE__;


ByteDynArray CSHA1::Digest(ByteArray data)
{
	ByteDynArray digest(SHA_DIGEST_LENGTH);
	SHA1(data.data(), data.size(), digest.data());

	return digest;
}
