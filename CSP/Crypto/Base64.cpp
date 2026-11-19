#include "../stdafx.h"
#include "./Base64.h"

#ifdef WIN32
#include <Wincrypt.h>
#else
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#endif

static char *szCompiledFile=__FILE__;

CBase64::CBase64()
{
}

CBase64::~CBase64()
{
}

#ifdef WIN32

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

#else

std::string &CBase64::Encode(ByteArray &data, std::string &encodedData) {
	init_func
	BIO *bio = BIO_new(BIO_s_mem());
	BIO *b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);
	BIO_write(bio, data.data(), (int)data.size());
	BIO_flush(bio);
	BUF_MEM *bptr;
	BIO_get_mem_ptr(bio, &bptr);
	encodedData.assign(bptr->data, bptr->length);
	BIO_free_all(bio);
	return encodedData;
	exit_func
}

ByteDynArray &CBase64::Decode(const char *encodedData, ByteDynArray &data) {
	init_func
	size_t len = strlen(encodedData);
	data.resize(len);
	BIO *bio = BIO_new_mem_buf(encodedData, (int)len);
	BIO *b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);
	int decoded = BIO_read(bio, data.data(), (int)len);
	BIO_free_all(bio);
	if (decoded > 0)
		data.resize(decoded, true);
	else
		data.resize(0);
	return data;
	exit_func
}

#endif
