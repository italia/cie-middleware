cmake_minimum_required(VERSION 3.16)
project(cie_sign_sdk CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "" FORCE)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(PREFIX_DIR ${CMAKE_SOURCE_DIR}/../third_party/prefix)

find_package(OpenSSL REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

set(FREETYPE_ROOT /opt/homebrew/opt/freetype)
set(LIBXML_ROOT /opt/homebrew/opt/libxml2)
set(FONTCONFIG_ROOT /opt/homebrew/opt/fontconfig)
set(CURL_ROOT /opt/homebrew/opt/curl)
set(BZIP2_ROOT /opt/homebrew/opt/bzip2)
set(ICONV_ROOT /opt/homebrew/opt/libiconv)
set(JPEG_ROOT /opt/homebrew/opt/jpeg)
set(TIFF_ROOT /opt/homebrew/opt/libtiff)

# Full self-contained sign SDK (macOS port), including its CSP/PCSC copies.
set(SRC_LIST
    ${SOURCE_DIR}/Base64.cpp
    ${SOURCE_DIR}/BigInteger.cpp
    ${SOURCE_DIR}/BigIntegerAlgorithms.cpp
    ${SOURCE_DIR}/BigIntegerUtils.cpp
    ${SOURCE_DIR}/BigUnsigned.cpp
    ${SOURCE_DIR}/BigUnsignedInABase.cpp
    ${SOURCE_DIR}/CIESigner.cpp
    ${SOURCE_DIR}/CIEEngine.c
    ${SOURCE_DIR}/CIEEngineHelper.c
    ${SOURCE_DIR}/CertStore.cpp
    ${SOURCE_DIR}/CounterSignatureGenerator.cpp
    ${SOURCE_DIR}/SignatureGenerator.cpp
    ${SOURCE_DIR}/LdapCrl.cpp
    ${SOURCE_DIR}/M7MParser.cpp
    ${SOURCE_DIR}/PdfSignatureGenerator.cpp
    ${SOURCE_DIR}/PdfVerifier.cpp
    ${SOURCE_DIR}/SignedDataGeneratorEx.cpp
    ${SOURCE_DIR}/SignedDocument.cpp
    ${SOURCE_DIR}/SignerInfoGenerator.cpp
    ${SOURCE_DIR}/TSAClient.cpp
    ${SOURCE_DIR}/UUCLogger.cpp
    ${SOURCE_DIR}/UUCStringTable.cpp
    ${SOURCE_DIR}/UUCTextFileReader.cpp
    ${SOURCE_DIR}/UUCTextFileWriter.cpp
    ${SOURCE_DIR}/XAdESGenerator.cpp
    ${SOURCE_DIR}/XAdESVerifier.cpp
    ${SOURCE_DIR}/disigonsdk.cpp
    ${SOURCE_DIR}/ASN1/ASN1BitString.cpp
    ${SOURCE_DIR}/ASN1/ASN1Boolean.cpp
    ${SOURCE_DIR}/ASN1/ASN1GenericSequence.cpp
    ${SOURCE_DIR}/ASN1/ASN1Integer.cpp
    ${SOURCE_DIR}/ASN1/ASN1Null.cpp
    ${SOURCE_DIR}/ASN1/ASN1Object.cpp
    ${SOURCE_DIR}/ASN1/ASN1ObjectIdentifier.cpp
    ${SOURCE_DIR}/ASN1/ASN1Octetstring.cpp
    ${SOURCE_DIR}/ASN1/ASN1OptionalField.cpp
    ${SOURCE_DIR}/ASN1/ASN1Sequence.cpp
    ${SOURCE_DIR}/ASN1/ASN1Setof.cpp
    ${SOURCE_DIR}/ASN1/ASN1UTCTime.cpp
    ${SOURCE_DIR}/ASN1/AlgorithmIdentifier.cpp
    ${SOURCE_DIR}/ASN1/Certificate.cpp
    ${SOURCE_DIR}/ASN1/CertificateInfo.cpp
    ${SOURCE_DIR}/ASN1/ContentInfo.cpp
    ${SOURCE_DIR}/ASN1/ContentType.cpp
    ${SOURCE_DIR}/ASN1/Crl.cpp
    ${SOURCE_DIR}/ASN1/DigestInfo.cpp
    ${SOURCE_DIR}/ASN1/IssuerAndSerialNumber.cpp
    ${SOURCE_DIR}/ASN1/Name.cpp
    ${SOURCE_DIR}/ASN1/OCSPRequest.cpp
    ${SOURCE_DIR}/ASN1/PKIStatusInfo.cpp
    ${SOURCE_DIR}/ASN1/RSAPrivateKey.cpp
    ${SOURCE_DIR}/ASN1/RSAPublicKey.cpp
    ${SOURCE_DIR}/ASN1/RelativeDistinguishedName.cpp
    ${SOURCE_DIR}/ASN1/SignedData.cpp
    ${SOURCE_DIR}/ASN1/SignerInfo.cpp
    ${SOURCE_DIR}/ASN1/SubjectPublicKeyInfo.cpp
    ${SOURCE_DIR}/ASN1/TSTInfo.cpp
    ${SOURCE_DIR}/ASN1/TimeStampData.cpp
    ${SOURCE_DIR}/ASN1/TimeStampRequest.cpp
    ${SOURCE_DIR}/ASN1/TimeStampResponse.cpp
    ${SOURCE_DIR}/ASN1/TimeStampToken.cpp
    ${SOURCE_DIR}/ASN1/UUCBufferedReader.cpp
    ${SOURCE_DIR}/RSA/desc.c
    ${SOURCE_DIR}/RSA/nn.c
    ${SOURCE_DIR}/RSA/r_encode.c
    ${SOURCE_DIR}/RSA/r_stdlib.c
    ${SOURCE_DIR}/RSA/rc2.c
    ${SOURCE_DIR}/RSA/rsa.c
    ${SOURCE_DIR}/RSA/sha1.c
    ${SOURCE_DIR}/RSA/sha2.c
    ${SOURCE_DIR}/CSP/IAS.cpp
    ${SOURCE_DIR}/CSP/ATR.cpp
    ${SOURCE_DIR}/CSP/ExtAuthKey.cpp
    ${SOURCE_DIR}/PCSC/APDU.cpp
    ${SOURCE_DIR}/PCSC/CardLocker.cpp
    ${SOURCE_DIR}/PCSC/PCSC.cpp
    ${SOURCE_DIR}/PCSC/Token.cpp
    ${SOURCE_DIR}/Util/Array.cpp
    ${SOURCE_DIR}/Util/CacheLib.cpp
    ${SOURCE_DIR}/Util/CryptoppUtils.cpp
    ${SOURCE_DIR}/Util/funccallinfo.cpp
    ${SOURCE_DIR}/Util/IniSettings.cpp
    ${SOURCE_DIR}/Util/log.cpp
    ${SOURCE_DIR}/Util/ModuleInfo.cpp
    ${SOURCE_DIR}/Util/TLV.cpp
    ${SOURCE_DIR}/Util/util.cpp
    ${SOURCE_DIR}/Util/UtilException.cpp
    ${SOURCE_DIR}/Util/UUCByteArray.cpp
    ${SOURCE_DIR}/Util/UUCProperties.cpp
    ${SOURCE_DIR}/Util/SyncroMutex.cpp
    ${SOURCE_DIR}/Crypto/AES.cpp
    ${SOURCE_DIR}/Crypto/ASNParser.cpp
    ${SOURCE_DIR}/Crypto/Base64.cpp
    ${SOURCE_DIR}/Crypto/DES3.cpp
    ${SOURCE_DIR}/Crypto/MAC.cpp
    ${SOURCE_DIR}/Crypto/MD5.cpp
    ${SOURCE_DIR}/Crypto/RSA.cpp
    ${SOURCE_DIR}/Crypto/SHA1.cpp
    ${SOURCE_DIR}/Crypto/SHA256.cpp
    ${SOURCE_DIR}/Crypto/SHA512.cpp
)

add_library(cie_sign_sdk STATIC ${SRC_LIST})

target_compile_definitions(cie_sign_sdk PRIVATE
    CURL_STATICLIB=1
    LIBXML_STATIC
    PODOFO_STATIC
)

target_compile_options(cie_sign_sdk PRIVATE
    -Wno-deprecated-declarations
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-sign-compare
    -Wno-reorder-ctor
    -Wno-format-security
)

target_include_directories(cie_sign_sdk PRIVATE
    ${INCLUDE_DIR}
    ${SOURCE_DIR}
    ${SOURCE_DIR}/ASN1
    ${SOURCE_DIR}/RSA
    ${SOURCE_DIR}/PCSC
    ${SOURCE_DIR}/CSP
    ${SOURCE_DIR}/Util
    ${SOURCE_DIR}/Crypto
    ${SOURCE_DIR}/cryptopp
    ${PREFIX_DIR}/include
    ${OPENSSL_INCLUDE_DIR}
    ${FREETYPE_ROOT}/include/freetype2
    ${LIBXML_ROOT}/include/libxml2
    ${FONTCONFIG_ROOT}/include
    ${CURL_ROOT}/include
    ${PNG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${BZIP2_ROOT}/include
    ${ICONV_ROOT}/include
    ${JPEG_ROOT}/include
    ${TIFF_ROOT}/include
)

install(TARGETS cie_sign_sdk DESTINATION ${CMAKE_SOURCE_DIR}/../CSP/Sign)
