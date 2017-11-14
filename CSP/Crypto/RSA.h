#pragma once
#include <openssl\rsa.h>
#include "../util/array.h"

class CRSA
{
	RSA* keyPriv;
public:
	CRSA();
	CRSA(ByteArray &mod, ByteArray &exp);
	~CRSA(void);

	ByteDynArray RSA_PURE(ByteArray &data);
	RESULT GenerateKey(DWORD size, ByteDynArray &module, ByteDynArray &pubexp, ByteDynArray &privexp);
	size_t dwKeySize;
};
