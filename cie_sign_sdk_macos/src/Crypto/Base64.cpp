
#include "definitions.h"
#include "Base64.h"
#include "cryptopp/cryptlib.h"
#include "cryptopp/base64.h"

extern CLog Log;

//#include <Wincrypt.h>

CBase64::CBase64()
{
}

CBase64::~CBase64()
{
}

std::string &CBase64::Encode(ByteArray &data, std::string &encodedData) {

	init_func
	    
    CryptoPP::ArraySink sink;
    CryptoPP::Base64Encoder encoder(&sink, false);
    CryptoPP::StringSource(data.data(), data.size(), true, &encoder);

    CryptoPP::byte* encoded = new CryptoPP::byte[sink.AvailableSize()];
    
    sink.Get(encoded, sink.AvailableSize());
    encodedData.append((char*)encoded, sink.AvailableSize());
    
	return encodedData;
	exit_func
}

ByteDynArray &CBase64::Decode(const char *encodedData,ByteDynArray &data) {
	init_func

    
    CryptoPP::ArraySink sink;
    CryptoPP::Base64Decoder decoder(&sink);
    CryptoPP::StringSource((BYTE*)encodedData, strlen(encodedData), true, &decoder);
    
    CryptoPP::byte* decoded = new CryptoPP::byte[sink.AvailableSize()];
    
    sink.Get(decoded, sink.AvailableSize());
    ByteArray decodedBa((BYTE*)decoded, sink.AvailableSize());
    
    data.append(decodedBa);
    
    return data;
	exit_func
}
