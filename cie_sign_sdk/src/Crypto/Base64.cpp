#include "stdafx.h"
#include ".\Base64.h"
#include <Wincrypt.h>

static char *szCompiledFile=__FILE__;

CBase64::CBase64()
{
}

CBase64::~CBase64()
{
}

std::string &CBase64::Encode(ByteArray &data, std::string &encodedData) {

	init_func
	DWORD dwStrSize = 0;
	CryptBinaryToString(data.data(), (DWORD)data.size(), CRYPT_STRING_BASE64, NULL, &dwStrSize);
	encodedData.resize(dwStrSize);
	CryptBinaryToString(data.data(), (DWORD)data.size(), CRYPT_STRING_BASE64, &encodedData.front(), &dwStrSize);

	return encodedData;
	exit_func
}

ByteDynArray &CBase64::Decode(const char *encodedData,ByteDynArray &data) {
	init_func

	DWORD dwDataSize = 0;
	CryptStringToBinary(encodedData, 0, CRYPT_STRING_BASE64, NULL, &dwDataSize, NULL, NULL);
	data.resize(dwDataSize);
	CryptStringToBinary(encodedData, 0, CRYPT_STRING_BASE64, data.data(), &dwDataSize, NULL, NULL);
	
	return data;
	exit_func
}
