#pragma once
#include "stdafx.h"

class  CBase64
{
public:
	CBase64();
	~CBase64();

	String &Encode(ByteArray &data,String &encodedData);
	ByteDynArray &Decode(const char *encodedData,ByteDynArray &data);
};
