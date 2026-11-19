#pragma once

#ifdef WIN32
#include "../stdafx.h"
#include <bcrypt.h>

#define DES_ENCRYPT 0
#define DES_DECRYPT 1
#else
#include <openssl/des.h>
#endif
#include "../Util/util.h"
#include "../Util/UtilException.h"

#define DESKEY_LENGHT 8

class CDES3
{
	ByteDynArray Des3(const ByteArray &data, int encOp);
#ifdef WIN32
	BCRYPT_KEY_HANDLE key;
	ByteDynArray iv;
#else
	DES_key_schedule k1, k2, k3;
	DES_cblock initVec;
#endif

public:
	CDES3();
	CDES3(const ByteArray &key, const ByteArray &iv);
	~CDES3(void);

	void Init(const ByteArray &key, const ByteArray &iv);
	ByteDynArray Encode(const ByteArray &data);
	ByteDynArray Decode(const ByteArray &data);
	ByteDynArray RawEncode(const ByteArray &data);
	ByteDynArray RawDecode(const ByteArray &data);
};
