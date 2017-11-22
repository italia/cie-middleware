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
	BN_bn2bin(keyPriv->n, module.data());
	privexp.resize(BN_num_bytes(keyPriv->d));
	BN_bn2bin(keyPriv->d, privexp.data());
	pubexp.resize(BN_num_bytes(keyPriv->e));
	BN_bn2bin(keyPriv->e, pubexp.data());

	_return(OK)
	exit_func
	_return(FAIL)
}
CRSA::CRSA(ByteArray &mod,ByteArray &exp)
{
	keyPriv = RSA_new();
	keyPriv->n = BN_bin2bn(mod.data(), (int)mod.size(), keyPriv->n);
	keyPriv->d = BN_new(); 
	keyPriv->e = BN_bin2bn(exp.data(), (int)exp.size(), keyPriv->e);

	dwKeySize=mod.size();
}

CRSA::~CRSA(void)
{
	if (keyPriv)
		RSA_free(keyPriv);
}

ByteDynArray CRSA::RSA_PURE(ByteArray &data )
{
	ByteDynArray resp(RSA_size(keyPriv));
	int iSignSize = RSA_public_encrypt((int)data.size(), data.data(), resp.data(), keyPriv, RSA_NO_PADDING);

	ER_ASSERT(iSignSize == dwKeySize, "Errore nella lunghezza dei dati per operazione RSA")

	return resp;
}