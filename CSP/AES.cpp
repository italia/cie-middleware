#include "stdafx.h"
#include ".\aes.h"

static char *szCompiledFile=__FILE__;

CAES::CAES() {
}

CAES::CAES(const ByteArray &key) {
	Init(key);
}

void CAES::Init(const ByteArray &key)
{
	init_func
	iv.resize(16, false);
	this->key = key;
	exit_func
}

CAES::~CAES(void)
{
}

const ByteDynArray &CAES::Encode(const ByteArray &data,ByteDynArray &result)
{
	init_func
	ByteDynArray inData;
	ER_CALL(AES(ISOPad16(data, inData), result, AES_ENCRYPT), "Errore della cifratura AES");
	_return (result)
	exit_func
}

const ByteDynArray &CAES::RawEncode(const ByteArray &data,ByteDynArray &result)
{
	init_func
	ER_ASSERT((data.size() % AES_BLOCK_SIZE) == 0, "La dimensione dei dati da cifrare deve essere multipla di 16")
	ER_CALL(AES(data,result,AES_ENCRYPT),"Errore della cifratura AES");
	_return (result)
	exit_func
}

const ByteDynArray &CAES::Decode(const ByteArray &data,ByteDynArray &result)
{
	init_func
	ER_CALL(AES(data, result, AES_DECRYPT), "Errore della decifratura AES");
	result.resize(RemoveISOPad(result),true);
	_return (result)
	exit_func
}

const ByteDynArray &CAES::RawDecode(const ByteArray &data,ByteDynArray &result)
{
	init_func
	ER_ASSERT((data.size() % AES_BLOCK_SIZE) == 0, "La dimensione dei dati da cifrare deve essere multipla di 16")
	ER_CALL(AES(data, result, AES_DECRYPT), "Errore della decifratura DES");
	_return (result)
	exit_func
}

DWORD CAES::AES(const ByteArray &data,ByteDynArray &resp,int encOp)
{
	init_func

	iv.fill(0);

	AES_KEY aesKey;
	if (encOp == AES_ENCRYPT)
		AES_set_encrypt_key(key.lock(), key.size() * 8, &aesKey);
	else
		AES_set_decrypt_key(key.lock(), key.size() * 8, &aesKey);
	DWORD dwAppSize = data.size() - 1;
	resp.resize(dwAppSize - (dwAppSize % 16) + 16);
	AES_cbc_encrypt(data.lock(), resp.lock(), data.size(), &aesKey, iv.lock(), encOp);
	
	_return(OK)
	exit_func
	_return(FAIL)
}