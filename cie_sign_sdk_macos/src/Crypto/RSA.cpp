
#include "RSA.h"
#include <openssl/bn.h>
#include "../Util/util.h"


extern CLog Log;

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

CRSA::CRSA(ByteArray &mod, ByteArray &exp)
{
	KeySize = mod.size();
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
	if (BCryptImportKeyPair(algo_rsa.algo, nullptr, BCRYPT_RSAPUBLIC_BLOB, &this->key, KeyData.data(), (ULONG)KeyData.size(), BCRYPT_NO_KEY_VALIDATION) != 0)
		throw logged_error("Errore nella creazione della chiave RSA");
}

void CRSA::GenerateKey(DWORD size, ByteDynArray &module, ByteDynArray &pubexp, ByteDynArray &privexp)
{
	init_func
	throw logged_error("Non supportato");
}

CRSA::~CRSA(void)
{
	if (key != nullptr)
		BCryptDestroyKey(key);
}

ByteDynArray CRSA::RSA_PURE(ByteArray &data)
{
	ULONG size = 0;
	if (BCryptEncrypt(key, data.data(), (ULONG)data.size(), nullptr, nullptr, 0, nullptr, 0, &size, 0) != 0)
		throw logged_error("Errore nella cifratura RSA");
	ByteDynArray resp(size);
	if (BCryptEncrypt(key, data.data(), (ULONG)data.size(), nullptr, nullptr, 0, resp.data(), (ULONG)resp.size(), &size, 0) != 0)
		throw logged_error("Errore nella cifratura RSA");

	ER_ASSERT(size == KeySize, "Errore nella lunghezza dei dati per operazione RSA")
	return resp;
}

#else

#include "cryptopp/rsa.h"
#include "cryptopp/secblock.h"
#include "cryptopp/pssr.h"

using CryptoPP::InvertibleRSAFunction;
using CryptoPP::RSASS;
using CryptoPP::RSA;
using CryptoPP::SHA512;
using CryptoPP::SecByteBlock;
using CryptoPP::PSS;
using CryptoPP::DecodingResult;
using CryptoPP::byte;

DWORD CRSA::GenerateKey(DWORD size, ByteDynArray &module, ByteDynArray &pubexp, ByteDynArray &privexp)
{

#if 0
	keyPriv = RSA_new();
	auto BNpubexp = BN_new();
	BN_set_word(BNpubexp, 65537);
	RSA_generate_key_ex(keyPriv, size, BNpubexp, nullptr);
	module.resize(BN_num_bytes(keyPriv->n));..
	BN_bn2bin(keyPriv->n, module.data());
	privexp.resize(BN_num_bytes(keyPriv->d));
	BN_bn2bin(keyPriv->d, privexp.data());
	pubexp.resize(BN_num_bytes(keyPriv->e));
	BN_bn2bin(keyPriv->e, pubexp.data());

    BN_clear_free(BNpubexp);

    return(S_OK);
    exit_func
    return(-1);
#endif

    init_func
    throw logged_error("Non supportato");

}

ByteArray modulusBa;
ByteArray exponentBa;

CRSA::CRSA(ByteArray &mod,ByteArray &exp)
{
    modulusBa = mod;
    exponentBa = exp;

    CryptoPP::Integer n(mod.data(), mod.size()), e(exp.data(), exp.size());
    pubKey.Initialize(n, e);

#if 0
    ByteDynArray modBa(mod.size() + 1);
    modBa.fill(0);
    modBa.rightcopy(mod);

    ByteDynArray expBa(exp.size() + 1);
    expBa.fill(0);
    expBa.rightcopy(exp);

	KeySize = mod.size();
	keyPriv = RSA_new();
	keyPriv->n = BN_bin2bn(mod.data(), (int)mod.size(), keyPriv->n);
	keyPriv->d = BN_new();
	keyPriv->e = BN_bin2bn(exp.data(), (int)exp.size(), keyPriv->e);
#endif

}

CRSA::~CRSA(void)
{
	//if (keyPriv!=nullptr)
	//	RSA_free(keyPriv);

}

ByteDynArray CRSA::RSA_PURE(ByteArray &data)
{

#if 0
    ByteDynArray resp(RSA_size(keyPriv));
    int SignSize = RSA_public_encrypt((int)data.size(), data.data(), resp.data(), keyPriv, RSA_NO_PADDING);
    ER_ASSERT(SignSize == KeySize, "Errore nella lunghezza dei dati per operazione RSA")

      printf("\nRSA resp1: %s\n", dumpHexData(resp).c_str());  // DEBUG

    return resp;
#endif

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

bool CRSA::RSA_PSS(ByteArray &signatureData, ByteArray &toSign)
{
    RSASS<PSS, SHA512>::Verifier verifier(pubKey);
    SecByteBlock signatureBlock((const byte*)signatureData.data(), signatureData.size());

    return verifier.VerifyMessage((const byte*)toSign.data(), toSign.size(), signatureBlock, signatureBlock.size());
}

#endif
