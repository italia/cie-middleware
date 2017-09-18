#include "..\stdafx.h"
#include ".\rsa.h"

static char *szCompiledFile=__FILE__;

CRSA::CRSA()
{
}

DWORD CRSA::GenerateKey(DWORD size, ByteDynArray &module, ByteDynArray &pubexp, ByteDynArray &privexp) 
{
	init_func
	keyPriv = RSA_new();
	auto BNpubexp = BN_new();
	BN_set_word(BNpubexp, 65537);
	RSA_generate_key_ex(keyPriv, size, BNpubexp, nullptr);
	module.resize(BN_num_bytes(keyPriv->n));
	BN_bn2bin(keyPriv->n, module.lock());
	privexp.resize(BN_num_bytes(keyPriv->d));
	BN_bn2bin(keyPriv->d, privexp.lock());
	pubexp.resize(BN_num_bytes(keyPriv->e));
	BN_bn2bin(keyPriv->e, pubexp.lock());

	_return(OK)
	exit_func
	_return(FAIL)
}
CRSA::CRSA(ByteArray &mod,ByteArray &exp)
{
	init_func_internal
	keyPriv = RSA_new();
	keyPriv->n = BN_bin2bn(mod.lock(),mod.size(), keyPriv->n); 
	keyPriv->d = BN_new(); 
	keyPriv->e = BN_bin2bn(exp.lock(),exp.size(), keyPriv->e); 

	dwKeySize=mod.size();
	exit_func_internal
}

CRSA::~CRSA(void)
{
	if (keyPriv)
		RSA_free(keyPriv);
}

RESULT CRSA::RSA_PURE(ByteArray &data,ByteDynArray &resp)
{
	init_func
	resp.resize(RSA_size(keyPriv));
	int iSignSize=RSA_public_encrypt(data.size(), data.lock(), resp.lock(), keyPriv, RSA_NO_PADDING);

	ER_ASSERT(iSignSize==dwKeySize,"Errore nella lunghezza dei dati per operazione RSA")

	_return(OK)
	exit_func
	_return(FAIL)
}