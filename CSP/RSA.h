#pragma once
#include <openssl\rsa.h>
#include "array.h"

class CRSA
{
	RSA* keyPriv;
public:
	CRSA();
	CRSA(ByteArray &mod, ByteArray &exp);
	~CRSA(void);

	RESULT RSA_PURE(ByteArray &data,ByteDynArray &resp);
	RESULT GenerateKey(DWORD size, ByteDynArray &module, ByteDynArray &pubexp, ByteDynArray &privexp);
	DWORD dwKeySize;
};
