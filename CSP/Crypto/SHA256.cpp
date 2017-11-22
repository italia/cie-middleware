#include "..\stdafx.h"
#include ".\sha256.h"

static char *szCompiledFile=__FILE__;

ByteDynArray CSHA256::Digest(ByteArray &data)
{
	ByteDynArray resp(SHA256_DIGEST_LENGTH);
	
	ER_ASSERT(SHA256(data.data(), data.size(), resp.data()) != NULL, "Errore nel calcolo dello SHA256")
	return resp;
}