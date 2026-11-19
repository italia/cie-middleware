// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef __CERTIFICATE_UTILS_H__
#define __CERTIFICATE_UTILS_H__

#include <string>

#include "cryptopp/eccrypto.h"
#include "cryptopp/rsa.h"
#include "cryptopp/secblock.h"
#include "definitions.h"

#define Buffer std::vector<unsigned char>

using namespace CryptoPP;

namespace lcp
{
    class CryptoppUtils
    {
    public:
        static void Base64ToSecBlock(const std::string & base64, SecByteBlock & result);
        static Buffer Base64ToVector(const std::string & base64);
        static std::string RawToHex(const Buffer & key);
        static Buffer HexToRaw(const std::string & hex);
        static std::string GenerateUuid();

        class Cert
        {
        public:
            static std::string IntegerToString(const Integer & integer);
            static void SkipNextSequence(BERSequenceDecoder & parentSequence);

            static std::string ReadIntegerAsString(BERSequenceDecoder & sequence);
            static word32 ReadVersion(BERSequenceDecoder & toBeSignedCertificate, word32 defaultVersion);
            static void ReadOID(BERSequenceDecoder & certificate, OID & algorithmId);

            static void ReadSubjectPublicKeyRSA(BERSequenceDecoder & toBeSignedCertificate, CryptoPP::RSA::PublicKey & result);
            static void ReadSubjectPublicKeyECDSA(BERSequenceDecoder & toBeSignedCertificate, CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey & result);

            static void ReadDateTimeSequence(
                BERSequenceDecoder & toBeSignedCertificate,
                std::string & notBefore,
                std::string & notAfter
                );
            static void BERDecodeTime(CryptoPP::BufferedTransformation& bt, std::string& time);

            static void PullToBeSignedData(const SecByteBlock & rawCertificate, SecByteBlock & result);

            static const BYTE ContextSpecificTagZero  = 0xa0;
            static const BYTE ContextSpecificTagThree = 0xa3;
            static const BYTE ContextSpecificTagSixIA5String = 0x86;
        };
    };
}

#endif //__CERTIFICATE_UTILS_H__
