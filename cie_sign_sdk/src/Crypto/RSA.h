#pragma once

#ifdef WIN32
//#include <bcrypt.h>
#include "..\cryptopp\cryptlib.h"
#include "..\cryptopp\integer.h"
#include "..\cryptopp\nbtheory.h"
#include "..\cryptopp\osrng.h"
#include "..\cryptopp\rsa.h"

#include "..\cryptopp\sha.h"
#include "..\cryptopp\pssr.h"
#else
#include <openssl\rsa.h>
#endif

#include "../util/array.h"

class CRSA
{
#ifdef WIN32
	BCRYPT_KEY_HANDLE key;
	CryptoPP::RSA::PublicKey pubKey;
#else
	RSA* keyPriv;
#endif

public:
	CRSA(ByteArray &mod, ByteArray &exp);
	~CRSA(void);

	ByteDynArray RSA_PURE(ByteArray &data);
	bool RSA_PSS(ByteArray &signatureData, ByteArray &toSign);
	void GenerateKey(DWORD size, ByteDynArray &module, ByteDynArray &pubexp, ByteDynArray &privexp);
	size_t KeySize;
};
