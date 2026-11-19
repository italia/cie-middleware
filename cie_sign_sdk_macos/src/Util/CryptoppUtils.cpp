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


#include "CryptoppUtils.h"
#include <sstream>

#include "cryptopp/base64.h"
#include "cryptopp/hex.h"
#include "cryptopp/osrng.h"

using namespace CryptoPP;

namespace lcp
{
    word32 CryptoppUtils::Cert::Cert::ReadVersion(BERSequenceDecoder & toBeSignedCertificate, word32 defaultVersion)
    {
        word32 version = defaultVersion;
        byte versionTag = toBeSignedCertificate.PeekByte();
        if (versionTag == ContextSpecificTagZero)
        {
            BERGeneralDecoder context(toBeSignedCertificate, ContextSpecificTagZero);
            BERDecodeUnsigned<word32>(toBeSignedCertificate, version);
        }
        else if (versionTag == INTEGER)
        {
            BERDecodeUnsigned<word32>(toBeSignedCertificate, version);
        }
        return version;
    }

    void CryptoppUtils::Cert::SkipNextSequence(BERSequenceDecoder & parentSequence)
    {
        BERSequenceDecoder sequence(parentSequence);
        sequence.SkipAll();
    }

    std::string CryptoppUtils::Cert::IntegerToString(const Integer & integer)
    {
        std::stringstream strm;
        strm << integer;
        std::string result(strm.str());
        return result.substr(0, result.find_first_of('.'));
    }

    std::string CryptoppUtils::Cert::ReadIntegerAsString(BERSequenceDecoder & sequence)
    {
        Integer value;
        value.BERDecode(sequence);
        return CryptoppUtils::Cert::Cert::IntegerToString(value);
    }

    std::vector<unsigned char> CryptoppUtils::Base64ToVector(const std::string & base64)
    {
        if (base64.empty())
        {
            throw std::runtime_error("base64 data is empty");
        }

        Base64Decoder decoder;
        decoder.Put(reinterpret_cast<const byte *>(base64.data()), base64.size());
        decoder.MessageEnd();

        std::vector<unsigned char> result;
        lword size = decoder.MaxRetrievable();
        if (size > 0 && size <= SIZE_MAX)
        {
            result.resize(static_cast<size_t>(size));
            decoder.Get(reinterpret_cast<byte *>(result.data()), result.size());
        }
        else
        {
            throw std::runtime_error("result data is empty");
        }
        return result;
    }

    std::string CryptoppUtils::GenerateUuid()
    {
        const static int UuidRawSize = 16;

        AutoSeededRandomPool rnd;
        std::vector<unsigned char> guid(UuidRawSize);
        rnd.GenerateBlock(guid.data(), guid.size());

        std::stringstream guidHex;
        guidHex << RawToHex(guid);
        return guidHex.str().insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
    }

    std::string CryptoppUtils::RawToHex(const std::vector<unsigned char> & key)
    {
        std::string hex;
        CryptoPP::ArraySource hexSource(
            &key.at(0),
            key.size(),
            true,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(hex),
                false
                )
            );
        return hex;
    }

    std::vector<unsigned char> CryptoppUtils::HexToRaw(const std::string & hex)
    {
        std::vector<unsigned char> value(hex.size() / 2);
        CryptoPP::StringSource(
            hex, true,
            new CryptoPP::HexDecoder(
                new CryptoPP::ArraySink(
                    &value.at(0), value.size()
                    )
                )
            );
        return value;
    }

    void CryptoppUtils::Base64ToSecBlock(const std::string & base64, SecByteBlock & result)
    {
        if (base64.empty())
        {
            throw std::runtime_error("base64 data is empty");
        }

        Base64Decoder decoder;
        decoder.Put(reinterpret_cast<const byte *>(base64.data()), base64.size());
        decoder.MessageEnd();

        lword size = decoder.MaxRetrievable();
        if (size > 0 && size <= SIZE_MAX)
        {
            result.resize(static_cast<size_t>(size));
            decoder.Get(reinterpret_cast<byte *>(result.data()), result.size());
        }
        else
        {
            throw std::runtime_error("result data is empty");
        }
    }

    void CryptoppUtils::Cert::BERDecodeTime(CryptoPP::BufferedTransformation& bt, std::string& time)
    {
        byte b;
        if (!bt.Get(b) || (b != GENERALIZED_TIME && b != UTC_TIME))
            BERDecodeError();

        size_t bc;
        if (!BERLengthDecode(bt, bc))
            BERDecodeError();

        SecByteBlock secBlockTime(bc);
        if (bc != bt.Get(secBlockTime, bc))
            BERDecodeError();

        time.assign(secBlockTime.begin(), secBlockTime.end());
        if (b == UTC_TIME)
        {
            int years = std::atoi(time.substr(0, 2).c_str());

            if (years < 50)
                time = "20" + time;
            else
                time = "19" + time;
        }
        time = time.substr(0, 8) + "T" + time.substr(8);
    }

    void CryptoppUtils::Cert::ReadDateTimeSequence(
        BERSequenceDecoder & toBeSignedCertificate,
        std::string & notBefore,
        std::string & notAfter
        )
    {
        BERSequenceDecoder validity(toBeSignedCertificate);
        {
            CryptoppUtils::Cert::BERDecodeTime(validity, notBefore);
            CryptoppUtils::Cert::BERDecodeTime(validity, notAfter);
        }
        validity.MessageEnd();
    }

    void CryptoppUtils::Cert::ReadSubjectPublicKeyECDSA(BERSequenceDecoder & toBeSignedCertificate, CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey & result)
    {
        ByteQueue subjectPublicKey;

        BERSequenceDecoder subjPublicInfoFrom(toBeSignedCertificate);
        DERSequenceEncoder subjPublicInfoOut(subjectPublicKey);
        subjPublicInfoFrom.TransferTo(subjPublicInfoOut, subjPublicInfoFrom.RemainingLength());
        subjPublicInfoOut.MessageEnd();
        subjPublicInfoFrom.MessageEnd();

        result.BERDecode(subjectPublicKey);
    }

    void CryptoppUtils::Cert::ReadSubjectPublicKeyRSA(BERSequenceDecoder & toBeSignedCertificate, RSA::PublicKey & result)
    {
        ByteQueue subjectPublicKey;

        BERSequenceDecoder subjPublicInfoFrom(toBeSignedCertificate);
        DERSequenceEncoder subjPublicInfoOut(subjectPublicKey);
        subjPublicInfoFrom.TransferTo(subjPublicInfoOut, subjPublicInfoFrom.RemainingLength());
        subjPublicInfoOut.MessageEnd();
        subjPublicInfoFrom.MessageEnd();

        result.BERDecode(subjectPublicKey);
    }

    void CryptoppUtils::Cert::ReadOID(BERSequenceDecoder & certificate, OID & algorithmId)
    {
        BERSequenceDecoder algorithm(certificate);
        algorithmId.BERDecode(algorithm);
        algorithm.SkipAll();
    }

    void CryptoppUtils::Cert::PullToBeSignedData(const SecByteBlock & rawCertificate, SecByteBlock & result)
    {
        if (rawCertificate.empty())
        {
            throw std::runtime_error("raw certificate data is empty");
        }

        ByteQueue certificateQueue;
        ByteQueue resultQueue;
        certificateQueue.Put(rawCertificate.data(), rawCertificate.size());
        certificateQueue.MessageEnd();

        BERSequenceDecoder certificate(certificateQueue);
        BERSequenceDecoder tbsCertificate(certificate);
        DERSequenceEncoder tbsCertifciateEnc(resultQueue);
        tbsCertificate.CopyTo(tbsCertifciateEnc);
        tbsCertifciateEnc.MessageEnd();

        lword size = resultQueue.MaxRetrievable();
        if (size > 0 && size <= SIZE_MAX)
        {
            result.resize(static_cast<size_t>(size));
            resultQueue.Get(result.data(), result.size());
        }
        else
        {
            throw std::runtime_error("signed data is empty");
        }
    }
}
