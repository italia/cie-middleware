#pragma once
#include "stdafx.h"
#include <string>

class  CBase64
{
public:
	CBase64();
	~CBase64();

	std::string &Encode(ByteArray &data, std::string &encodedData);
	ByteDynArray &Decode(const char *encodedData, ByteDynArray &data);
};
