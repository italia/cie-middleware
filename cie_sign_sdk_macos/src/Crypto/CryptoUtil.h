//
//  CryptoUtil.h
//  cie-pkcs11
//
//  Created by ugo chirico on 07/01/2019.
//  Copyright © 2019 IPZS. All rights reserved.
//

#ifndef CryptoUtil_h
#define CryptoUtil_h

#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include "../keys.h"
#include "cryptopp/sha.h"
#include <string>

using namespace CryptoPP;

int encrypt(std::string& message, std::string& ciphertext)
{
    byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];
    memset( key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );
    
    std::string enckey = ENCRYPTION_KEY;
    
    byte digest[SHA1::DIGESTSIZE];
    CryptoPP::SHA1().CalculateDigest(digest, (byte*)enckey.c_str(), enckey.length());
    memcpy(key, digest, CryptoPP::AES::DEFAULT_KEYLENGTH );
    //
    // Create Cipher Text
    //
    CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );
    
    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>( message.c_str() ), message.length() + 1 );
    stfEncryptor.MessageEnd();
    
    return 0;
};

int decrypt(std::string& ciphertext, std::string& message)
{
    byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];
    memset( key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );
    
    std::string enckey = ENCRYPTION_KEY;
    
    byte digest[SHA1::DIGESTSIZE];
    CryptoPP::SHA1().CalculateDigest(digest, (byte*)enckey.c_str(), enckey.length());
    memcpy(key, digest, CryptoPP::AES::DEFAULT_KEYLENGTH );
    
    //
    // Decrypt
    //
    CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );
    
    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( message ) );
    stfDecryptor.Put( reinterpret_cast<const unsigned char*>( ciphertext.c_str() ), ciphertext.size() );
    stfDecryptor.MessageEnd();
    
    return 0;
};

#endif /* CryptoUtil_h */
