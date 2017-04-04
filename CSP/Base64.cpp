#include "stdafx.h"
#include ".\Base64.h"
#include "CBase64.h"
#include <Wincrypt.h>

static char *szCompiledFile=__FILE__;

CBase64::CBase64()
{
}

CBase64::~CBase64()
{
}

String &CBase64::Encode(ByteArray &data,String &encodedData) {
	init_func
	DWORD dwStrSize=0;
	CBase64Ex base64;

	dwStrSize=base64.CalculateRecquiredEncodeOutputBufferSize(data.size());
	encodedData.resize(dwStrSize);
	base64.EncodeBuffer((const char *)data.lock(),data.size(),encodedData.lock());
	
	return encodedData;
	exit_func
}

ByteDynArray &CBase64::Decode(const char *encodedData,ByteDynArray &data) {
	init_func
	DWORD dwDataSize=0;	
	DWORD dwSkip=0;
	DWORD dwFlags=0;
	CBase64Ex base64;
	dwDataSize=base64.CalculateRecquiredDecodeOutputBufferSize(encodedData);
	data.resize(dwDataSize);
	base64.DecodeBuffer(encodedData,(char *)data.lock());
	
	_return(data)
	exit_func
}
