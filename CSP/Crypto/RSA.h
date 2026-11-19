#pragma once

#ifdef WIN32
//#include <bcrypt.h>
#include "../cryptopp/cryptlib.h"
#include "../cryptopp/integer.h"
#include "../cryptopp/nbtheory.h"
#include "../cryptopp/osrng.h"
#include "../cryptopp/rsa.h"

#include "../cryptopp/sha.h"
#include "../cryptopp/pssr.h"
#else
#include <openssl/rsa.h>
#include <openssl/evp.h>
#endif

#include "../util/array.h"

class CRSA
{
#ifdef WIN32
	BCRYPT_KEY_HANDLE key;
	CryptoPP::RSA::PublicKey pubKey;
#else
	EVP_PKEY *pkey;
#endif

public:
	CRSA(const ByteArray &mod, const ByteArray &exp);
	~CRSA(void);

	ByteDynArray RSA_PURE(const ByteArray &data);
	bool RSA_PSS(const ByteArray &signatureData, const ByteArray &toSign);
	void GenerateKey(DWORD size, ByteDynArray &module, ByteDynArray &pubexp, ByteDynArray &privexp);
	size_t KeySize;
};
