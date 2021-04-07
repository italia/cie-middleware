#pragma once
#include "ASN1Object.h"
#include "ASN1Integer.h"
#include "ASN1Sequence.h"

class CRSAPublicKey : public CASN1Sequence
{
public:
	//Costruttori
	CRSAPublicKey(UUCBufferedReader& reader);
	
	CRSAPublicKey(const CASN1Object& obj);
	
	//CRSAPublicKey(const UUCByteArray& content);
	
	CRSAPublicKey(const CASN1Integer& modulus, const CASN1Integer& exponent);

	virtual ~CRSAPublicKey();

	CASN1Integer getModulus() ;
	
	CASN1Integer getExponent();

private:
	
};
