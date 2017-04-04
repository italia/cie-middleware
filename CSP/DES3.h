#pragma once
#include "stdafx.h"
#include <openssl\des.h>
#include "util.h"
#include "utilexception.h"

#define DESKEY_LENGHT 8

class CDES3
{
	DWORD Des3(const ByteArray &data,ByteDynArray &resp,int encOp);
	des_key_schedule k1,k2,k3;
	des_cblock initVec;

public:
	CDES3();
	CDES3(const ByteArray &key);
	~CDES3(void);

	void Init(const ByteArray &key);
	const ByteDynArray &Encode(const ByteArray &data,ByteDynArray &result);
	const ByteDynArray &Decode(const ByteArray &data,ByteDynArray &result);
	const ByteDynArray &RawEncode(const ByteArray &data,ByteDynArray &result);
	const ByteDynArray &RawDecode(const ByteArray &data,ByteDynArray &result);
};
