#include "../stdafx.h"
#include "./rsa.h"

#ifdef WIN32
#include "../cryptopp/sha.h"
#include "../cryptopp/rsa.h"
#include "../cryptopp/secblock.h"
#include "../cryptopp/pssr.h"
#include "../cryptopp/cryptlib.h"

using CryptoPP::InvertibleRSAFunction;
using CryptoPP::RSASS;
using CryptoPP::RSA;
using CryptoPP::SHA512;
using CryptoPP::SecByteBlock;
using CryptoPP::PSS;
using CryptoPP::DecodingResult;
#endif

static char *szCompiledFile=__FILE__;

#ifdef WIN32

class init_rsa {
public:
	BCRYPT_ALG_HANDLE algo;
	init_rsa() {
		if (BCryptOpenAlgorithmProvider(&algo, BCRYPT_RSA_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0) != 0)
			throw logged_error("Errore nell'inizializzazione dell'algoritmo RSA");
	}
	~init_rsa() {
		BCryptCloseAlgorithmProvider(algo, 0);
	}
} algo_rsa;

CRSA::CRSA(const ByteArray &mod, const ByteArray &exp)
{
	CryptoPP::Integer n(mod.data(), mod.size()), e(exp.data(), exp.size());	
	pubKey.Initialize(n, e);
	
	/*KeySize = mod.size();
	ByteDynArray KeyData(sizeof(BCRYPT_RSAKEY_BLOB) + mod.size() + exp.size());
	BCRYPT_RSAKEY_BLOB *rsaImpKey = (BCRYPT_RSAKEY_BLOB *)KeyData.data();
	rsaImpKey->Magic = BCRYPT_RSAPUBLIC_MAGIC;
	rsaImpKey->BitLength = (ULONG)(KeySize << 3);
	rsaImpKey->cbModulus = (ULONG)KeySize;
	rsaImpKey->cbPublicExp = (ULONG)exp.size();
	rsaImpKey->cbPrime1 = 0;
	rsaImpKey->cbPrime2 = 0;
	KeyData.copy(exp, sizeof(BCRYPT_RSAKEY_BLOB));
	KeyData.rightcopy(mod);

	this->key = nullptr;
	DWORD dwRes = BCryptImportKeyPair(algo_rsa.algo, nullptr, BCRYPT_RSAPUBLIC_BLOB, &this->key, KeyData.data(), (ULONG)KeyData.size(), BCRYPT_NO_KEY_VALIDATION);
	if (dwRes != 0)
	{
		DWORD dwRes1 = GetLastError();
	
		Log.write("Errore %x, %x nella creazione della chiave RSA", dwRes, dwRes1);

		throw logged_error("Errore nella creazione della chiave RSA");
	}*/
}

void CRSA::GenerateKey(DWORD size, ByteDynArray &module, ByteDynArray &pubexp, ByteDynArray &privexp)
{
	init_func
	throw logged_error("Non supportato");
}


CRSA::~CRSA(void)
{
	/*if (key != nullptr)
		BCryptDestroyKey(key);*/
}

ByteDynArray CRSA::RSA_PURE(const ByteArray &data)
{
	CryptoPP::Integer m((const byte *)data.data(), data.size());

	CryptoPP::Integer c = pubKey.ApplyFunction(m);

	size_t len = c.MinEncodedSize();
	if (len == 0xff)
		len = 0x100;

	ByteDynArray resp(len);

	c.Encode((byte *)resp.data(), resp.size(), CryptoPP::Integer::UNSIGNED);

	
	/*ULONG size = 0;
	if (BCryptEncrypt(key, data.data(), (ULONG)data.size(), nullptr, nullptr, 0, nullptr, 0, &size, 0) != 0)
		throw logged_error("Errore nella cifratura RSA");
	ByteDynArray resp1(size);
	if (BCryptEncrypt(key, data.data(), (ULONG)data.size(), nullptr, nullptr, 0, resp1.data(), (ULONG)resp1.size(), &size, 0) != 0)
		throw logged_error("Errore nella cifratura RSA");

	ER_ASSERT(size == KeySize, "Errore nella lunghezza dei dati per operazione RSA")*/




	return resp;
}

bool CRSA::RSA_PSS(const ByteArray &signatureData, const ByteArray &toSign)
{
	RSASS<PSS, SHA512>::Verifier verifier(pubKey);
	SecByteBlock signatureBlock((const byte*)signatureData.data(), signatureData.size());

	return verifier.VerifyMessage((const byte*)toSign.data(), toSign.size(), signatureBlock, signatureBlock.size());
}

#else

#include <openssl/bn.h>
#include <openssl/core_names.h>
#include <openssl/param_build.h>

CRSA::CRSA(const ByteArray &mod, const ByteArray &exp)
{
	KeySize = mod.size();
	pkey = nullptr;
	BIGNUM *n = BN_bin2bn(mod.data(), (int)mod.size(), NULL);
	BIGNUM *e = BN_bin2bn(exp.data(), (int)exp.size(), NULL);

	OSSL_PARAM_BLD *bld = OSSL_PARAM_BLD_new();
	OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_N, n);
	OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_E, e);
	OSSL_PARAM *params = OSSL_PARAM_BLD_to_param(bld);

	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_from_name(NULL, "RSA", NULL);
	EVP_PKEY_fromdata_init(ctx);
	EVP_PKEY_fromdata(ctx, &pkey, EVP_PKEY_PUBLIC_KEY, params);

	EVP_PKEY_CTX_free(ctx);
	OSSL_PARAM_BLD_free(bld);
	OSSL_PARAM_free(params);
	BN_free(n);
	BN_free(e);
}

void CRSA::GenerateKey(DWORD size, ByteDynArray &module, ByteDynArray &pubexp, ByteDynArray &privexp)
{
	throw logged_error("Non supportato");
}

CRSA::~CRSA(void)
{
	if (pkey != nullptr)
		EVP_PKEY_free(pkey);
}

ByteDynArray CRSA::RSA_PURE(const ByteArray &data)
{
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
	EVP_PKEY_encrypt_init(ctx);
	EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_NO_PADDING);

	size_t outlen = 0;
	EVP_PKEY_encrypt(ctx, NULL, &outlen, data.data(), data.size());
	ByteDynArray resp(outlen);
	EVP_PKEY_encrypt(ctx, resp.data(), &outlen, data.data(), data.size());
	EVP_PKEY_CTX_free(ctx);

	ER_ASSERT(outlen == KeySize, "Errore nella lunghezza dei dati per operazione RSA")
	return resp;
}

bool CRSA::RSA_PSS(const ByteArray &signatureData, const ByteArray &toSign)
{
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	EVP_PKEY_CTX *pctx = NULL;

	if (EVP_DigestVerifyInit(mdctx, &pctx, EVP_sha512(), NULL, pkey) <= 0) {
		EVP_MD_CTX_free(mdctx);
		return false;
	}
	EVP_PKEY_CTX_set_rsa_padding(pctx, RSA_PKCS1_PSS_PADDING);
	EVP_PKEY_CTX_set_rsa_pss_saltlen(pctx, RSA_PSS_SALTLEN_AUTO);

	int rc = EVP_DigestVerify(mdctx, signatureData.data(), signatureData.size(),
	                          toSign.data(), toSign.size());
	EVP_MD_CTX_free(mdctx);
	return rc == 1;
}

#endif