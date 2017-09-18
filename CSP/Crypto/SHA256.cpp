#include "..\stdafx.h"
#include ".\sha256.h"

static char *szCompiledFile=__FILE__;

CSHA256::CSHA256()
{
}

CSHA256::~CSHA256(void)
{
}

RESULT CSHA256::Digest(ByteArray &data,ByteDynArray &resp)
{
	init_func
	resp.resize(SHA256_DIGEST_LENGTH);
	
	ER_ASSERT(SHA256(data.lock(),data.size(),resp.lock())!=NULL,"Errore nel calcolo dello SHA256")

	_return(OK)
	exit_func
	_return(FAIL)
}