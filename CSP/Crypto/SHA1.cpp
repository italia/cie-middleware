#include "../stdafx.h"
#include "SHA1.h"
#include "../util/util.h"

static char *szCompiledFile=__FILE__;


ByteDynArray &CSHA1::Digest(ByteArray data,ByteDynArray &digest)
{
	init_func

	digest.resize(SHA_DIGEST_LENGTH);
	SHA1(data.lock(),data.size(),digest.lock());

	_return(digest)
	exit_func
}
