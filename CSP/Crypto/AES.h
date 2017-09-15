#pragma once
#include "../stdafx.h"
#include <openssl\aes.h>
#include "../util/util.h"
#include "../util/utilexception.h"

#define AESKEY_LENGHT 32

class CAES
{
	DWORD AES(const ByteArray &data,ByteDynArray &resp,int encOp);
	ByteDynArray key;
	ByteDynArray iv;

public:
	CAES();
	CAES(const ByteArray &key);
	~CAES(void);

	void Init(const ByteArray &key);
	const ByteDynArray &Encode(const ByteArray &data,ByteDynArray &result);
	const ByteDynArray &Decode(const ByteArray &data,ByteDynArray &result);
	const ByteDynArray &RawEncode(const ByteArray &data,ByteDynArray &result);
	const ByteDynArray &RawDecode(const ByteArray &data,ByteDynArray &result);
};
