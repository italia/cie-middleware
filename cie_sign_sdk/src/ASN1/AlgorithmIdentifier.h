#ifndef _ALGORITHMIDENTIFIER_H
#define _ALGORITHMIDENTIFIER_H

#include "ASN1ObjectIdentifier.h"
#include "ASN1Sequence.h"

class CAlgorithmIdentifier : public CASN1Sequence
{	
public:
	CAlgorithmIdentifier(const CASN1Object& algoId);

	CAlgorithmIdentifier(UUCBufferedReader& reader);			

	CAlgorithmIdentifier(const char* szObjId);
	
	CAlgorithmIdentifier(const CASN1ObjectIdentifier& objId);
	
	CASN1ObjectIdentifier getOID();

	CASN1Object getParameters();
};

#endif // _ALGORITHMIDENTIFIER_H