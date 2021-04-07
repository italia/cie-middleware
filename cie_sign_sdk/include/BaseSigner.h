#pragma once

#include "Certificate.h"

class CBaseSigner
{
public:
	virtual long GetCertificate(const char* alias, CCertificate** ppCertificate, UUCByteArray& id) = 0;

	virtual long Sign(UUCByteArray& data, UUCByteArray& id, int algo, UUCByteArray& signature) = 0;

	virtual long Close() = 0;
};