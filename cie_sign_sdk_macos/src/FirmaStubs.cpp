#include "cryptopp/integer.h"
#include "cryptopp/filters.h"
#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/sha.h"
#include "keys.h"
#include <stdio.h>
#include <string>
#include <string.h>

void notifyCardNotRegistered(const char* szPAN)
{
	printf("notifyCardNotRegistered: %s\n", szPAN ? szPAN : "(null)");
}

void GetPublicKeyFromCert(CryptoPP::BufferedTransformation & certin,
                          CryptoPP::BufferedTransformation & btKey,
                          CryptoPP::BufferedTransformation & btIssuer,
                          CryptoPP::Integer & serial)
{
	(void)certin;
	(void)btKey;
	(void)btIssuer;
	(void)serial;
}

int decrypt(std::string& ciphertext, std::string& message)
{
	using namespace CryptoPP;
	byte key[AES::DEFAULT_KEYLENGTH], iv[AES::BLOCKSIZE];
	memset(key, 0x00, AES::DEFAULT_KEYLENGTH);
	memset(iv, 0x00, AES::BLOCKSIZE);

	std::string enckey = ENCRYPTION_KEY;
	byte digest[SHA1::DIGESTSIZE];
	SHA1().CalculateDigest(digest, (byte*)enckey.c_str(), enckey.length());
	memcpy(key, digest, AES::DEFAULT_KEYLENGTH);

	AES::Decryption aesDecryption(key, AES::DEFAULT_KEYLENGTH);
	CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);
	StreamTransformationFilter stfDecryptor(cbcDecryption, new StringSink(message));
	stfDecryptor.Put(reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size());
	stfDecryptor.MessageEnd();
	return 0;
}
