#pragma once
#ifdef WIN32
#include <bcrypt.h>
#else
#include <openssl\des.h>
#endif
#include "../util/util.h"
#include "../util/utilexception.h"

class CMAC
{
#ifdef WIN32
	BCRYPT_KEY_HANDLE key1;
	BCRYPT_KEY_HANDLE key2;
	ByteDynArray iv;
#else
	des_key_schedule k1,k2,k3;
	des_cblock initVec;
#endif
public:
	CMAC();
	CMAC(const ByteArray &key, const ByteArray &iv);
	~CMAC(void);

	void Init(const ByteArray &key, const ByteArray &iv);
	ByteDynArray Mac(const ByteArray &data);
};
