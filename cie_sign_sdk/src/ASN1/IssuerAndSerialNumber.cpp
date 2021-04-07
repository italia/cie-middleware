// IssuerAndSerialNumber.cpp: implementation of the CIssuerAndSerialNumber class.
//
//////////////////////////////////////////////////////////////////////


#include "IssuerAndSerialNumber.h"
#include "ASN1OptionalField.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIssuerAndSerialNumber::CIssuerAndSerialNumber(UUCBufferedReader& reader)
: CASN1Sequence(reader)
{
}
	
CIssuerAndSerialNumber::CIssuerAndSerialNumber(const CASN1Object& issuerAndSerNum)
: CASN1Sequence(issuerAndSerNum)
{

}

CIssuerAndSerialNumber::CIssuerAndSerialNumber(const CName& issuer, const CASN1Integer& serNum, bool contextSpecific)
{
	if(contextSpecific)
	{
		CASN1Sequence issuerField;
		issuerField.addElement(issuer);
	
		CASN1Sequence innerSequence;		
		innerSequence.addElement(CASN1OptionalField(issuerField, 0x04));
		
		addElement(innerSequence);
		addElement(serNum);
	}
	else 
	{
		addElement(issuer);
		addElement(serNum);
	}

	
	
}

CIssuerAndSerialNumber::~CIssuerAndSerialNumber()
{

}
