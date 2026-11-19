#pragma once

#ifdef WIN32
#include <bcrypt.h>
#define AES_ENCRYPT 0
#define AES_DECRYPT 1
#define AES_BLOCK_SIZE 16

#else
#include <openssl/aes.h>
//#include <OpenSSL-Static/aes.h>
//#include "cryptopp/aes.h"
#endif
#include "../Util/util.h"
#include "../Util/UtilException.h"

#define AESKEY_LENGHT 32

class CAES
{
#ifdef WIN32
	BCRYPT_KEY_HANDLE key;
#else
	ByteDynArray key;
#endif

	ByteDynArray AES(const ByteArray &data, int encOp);
	ByteDynArray iv;

public:
	CAES();
	CAES(const ByteArray &key, const ByteArray &iv);
	~CAES(void);

	void Init(const ByteArray &key, const ByteArray &iv);
	ByteDynArray Encode(const ByteArray &data);
	ByteDynArray Decode(const ByteArray &data);
	ByteDynArray RawEncode(const ByteArray &data);
	ByteDynArray RawDecode(const ByteArray &data);
};
