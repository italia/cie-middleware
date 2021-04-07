
#include "AlgorithmIdentifier.h"
#include "ASN1Null.h"


CAlgorithmIdentifier::CAlgorithmIdentifier(const CASN1Object& algoId)
: CASN1Sequence(algoId)
{
}


CAlgorithmIdentifier::CAlgorithmIdentifier(const char* lpszObjId)
{		
	addElement(CASN1ObjectIdentifier(lpszObjId));
	addElement(CASN1Null());
}
	
CAlgorithmIdentifier::CAlgorithmIdentifier(UUCBufferedReader& reader)
: CASN1Sequence(reader)
{	
}

CASN1ObjectIdentifier CAlgorithmIdentifier::getOID()
{
	return (CASN1ObjectIdentifier)elementAt(0);
}

CASN1Object CAlgorithmIdentifier::getParameters() 
{
	return (CASN1Object)elementAt(1);
}


