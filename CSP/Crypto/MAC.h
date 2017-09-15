#pragma once
#include <openssl\des.h>
#include "../util/util.h"
#include "../util/utilexception.h"

class CMAC
{
	des_key_schedule k1,k2,k3;
	des_cblock initVec;
	DWORD _Mac(const ByteArray &data,ByteDynArray &resp);
public:
	CMAC();
	CMAC(ByteArray &key);
	~CMAC(void);

	void Init(ByteArray &key);
	const ByteDynArray &Mac(const ByteArray &data,ByteDynArray &result);

};
