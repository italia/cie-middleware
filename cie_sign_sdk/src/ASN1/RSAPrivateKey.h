#pragma once
#include "ASN1Object.h"
#include "ASN1Integer.h"
#include "ASN1Sequence.h"

//RSAPrivateKey :: = SEQUENCE{
//	version           Version,
//	modulus           INTEGER, --n
//	publicExponent    INTEGER, --e
//	privateExponent   INTEGER, --d
//	prime1            INTEGER, --p
//	prime2            INTEGER, --q
//	exponent1         INTEGER, --d mod(p - 1)
//	exponent2         INTEGER, --d mod(q - 1)
//	coefficient       INTEGER, --(inverse of q) mod p
//	otherPrimeInfos   OtherPrimeInfos OPTIONAL
//}

class CRSAPrivateKey : public CASN1Sequence
{
public:
	//Costruttori
	CRSAPrivateKey(UUCBufferedReader& reader);
	
	CRSAPrivateKey(const CASN1Object& obj);
	
	CRSAPrivateKey(const UUCByteArray& content);
	
	CRSAPrivateKey(const CASN1Integer& modulus, const CASN1Integer& pubExp, const CASN1Integer& priExp);

	CRSAPrivateKey(const BYTE* pbtRSAKey_MS, const DWORD dwLen);

	virtual ~CRSAPrivateKey();

	CASN1Integer getModulus() ;
	
	CASN1Integer getPublicExponent();
	
	CASN1Integer getPrivateExponent();
};
