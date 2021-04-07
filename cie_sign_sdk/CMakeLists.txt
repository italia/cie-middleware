cmake_minimum_required(VERSION 3.15.3)

project(cie_sign_sdk)

set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/../CSP/Sign)

message("DEPENDENCIES_DIR: " ${DEPENDENCIES_DIR})
message("SOURCE_DIR: " ${SOURCE_DIR})
message("INCLUDE_DIR: " ${INCLUDE_DIR})
message("INSTALL_DIR: " ${INSTALL_DIR})

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
    ${SOURCE_DIR}/UUCProperties.cpp
    ${SOURCE_DIR}/UUCStringTable.cpp
    ${SOURCE_DIR}/UUCTextFileReader.cpp
    ${SOURCE_DIR}/UUCTextFileWriter.cpp
    ${SOURCE_DIR}/XAdESGenerator.cpp
    ${SOURCE_DIR}/XAdESVerifier.cpp
    ${SOURCE_DIR}/definitions.cpp
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
    ${SOURCE_DIR}/ASN1/UUCByteArray.cpp
    ${SOURCE_DIR}/RSA/desc.c
    ${SOURCE_DIR}/RSA/nn.c
    ${SOURCE_DIR}/RSA/r_encode.c
    ${SOURCE_DIR}/RSA/r_stdlib.c
    ${SOURCE_DIR}/RSA/rc2.c
    ${SOURCE_DIR}/RSA/rc2.h
    ${SOURCE_DIR}/RSA/rsa.c
    ${SOURCE_DIR}/RSA/sha1.c
    ${SOURCE_DIR}/RSA/sha2.c
    ${SOURCE_DIR}/CSP/IAS.cpp
    ${SOURCE_DIR}/CSP/ExtAuthKey.cpp
    ${SOURCE_DIR}/PCSC/APDU.cpp  
    ${SOURCE_DIR}/PCSC/CardLocker.cpp  
    ${SOURCE_DIR}/PCSC/PCSC.cpp  
    ${SOURCE_DIR}/PCSC/Token.cpp
    ${SOURCE_DIR}/Util/Array.cpp
    ${SOURCE_DIR}/Util/IniSettings.cpp
    ${SOURCE_DIR}/Util/ModuleInfo.cpp
    ${SOURCE_DIR}/Util/TLV.cpp
    ${SOURCE_DIR}/Util/UtilException.cpp
    ${SOURCE_DIR}/Util/funccallinfo.cpp
    ${SOURCE_DIR}/Util/log.cpp
    ${SOURCE_DIR}/Util/util.cpp
    ${SOURCE_DIR}/Crypto/AES.cpp
    ${SOURCE_DIR}/Crypto/ASNParser.cpp
    ${SOURCE_DIR}/Crypto/Base64.cpp
    ${SOURCE_DIR}/Crypto/DES3.cpp
    ${SOURCE_DIR}/Crypto/MAC.cpp
    ${SOURCE_DIR}/Crypto/MD5.cpp
    ${SOURCE_DIR}/Crypto/RSA.cpp
    ${SOURCE_DIR}/Crypto/SHA1.cpp
    ${SOURCE_DIR}/Crypto/SHA256.cpp
    ${SOURCE_DIR}/CacheLib/CacheLib.cpp
    ${SOURCE_DIR}/Crypto/SHA512.cpp
    )

add_library(${PROJECT_NAME} STATIC ${SRC_LIST})
target_compile_definitions(${PROJECT_NAME} PUBLIC CURL_STATICLIB=1 LIBXML_STATIC)

set(INCLUDE_LIST 
    ${INCLUDE_DIR}
    ${SOURCE_DIR}/ASN1
    ${SOURCE_DIR}/RSA
    ${SOURCE_DIR}/PCSC
    ${SOURCE_DIR}/CSP
    ${SOURCE_DIR}/Util
    ${SOURCE_DIR}/Crypto
    ${SOURCE_DIR}/cryptopp
    ${SOURCE_DIR}/CacheLib
    ${DEPENDENCIES_DIR}/freetype/include/freetype2
    ${DEPENDENCIES_DIR}/libcurl/include
    ${DEPENDENCIES_DIR}/libpng/include
    ${DEPENDENCIES_DIR}/openssl/include
    ${DEPENDENCIES_DIR}/podofo/include
    ${DEPENDENCIES_DIR}/zlib/include
    ${DEPENDENCIES_DIR}/libxml2/include/libxml2
    ${DEPENDENCIES_DIR}/libiconv/include
 )

 set(LIBRARY_PATH
    ${DEPENDENCIES_DIR}/freetype/freetype.lib
    ${DEPENDENCIES_DIR}/libcurl/libcurl.lib
    ${DEPENDENCIES_DIR}/libpng/libpng16.lib
    ${DEPENDENCIES_DIR}/openssl/libcrypto.lib
    ${DEPENDENCIES_DIR}/openssl/libssl.lib
    ${DEPENDENCIES_DIR}/podofo/podofo.lib
    ${DEPENDENCIES_DIR}/zlib/zlib.lib
    ${DEPENDENCIES_DIR}/libxml2/libxml2_a.lib
    ${DEPENDENCIES_DIR}/libiconv/iconv.lib
    ${SOURCE_DIR}/cryptopp/x64/cryptlib.lib
     )

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_LIST})

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})
install(FILES ${INCLUDE_DIR}/disigonsdk.h ${INCLUDE_DIR}/CIEEngine.h DESTINATION ${INSTALL_DIR})
