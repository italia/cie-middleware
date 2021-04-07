
#include "RSAPublicKey.h"

CRSAPublicKey::CRSAPublicKey(UUCBufferedReader& reader)
: CASN1Sequence(reader)
{
}
	
CRSAPublicKey::CRSAPublicKey(const CASN1Object& obj)
: CASN1Sequence(obj)
{

}
/*	
CRSAPublicKey::CRSAPublicKey(const UUCByteArray& content)
: CASN1Sequence(content)
{

}
 */
CRSAPublicKey::CRSAPublicKey(const CASN1Integer& modulus, const CASN1Integer& exponent)
{
	addElement(modulus);
	addElement(exponent);
}

CRSAPublicKey::~CRSAPublicKey(void)
{
	
}

CASN1Integer CRSAPublicKey::getModulus() 
{
	return CASN1Integer(elementAt(0));
}

CASN1Integer CRSAPublicKey::getExponent() 
{
	return CASN1Integer(elementAt(1));
}

