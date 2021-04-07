// RelativeDistinguishedName.cpp: implementation of the CRelativeDistinguishedName class.
//
//////////////////////////////////////////////////////////////////////


#include "RelativeDistinguishedName.h"
//#include "ASN1Exception.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRelativeDistinguishedName::CRelativeDistinguishedName()
{

}

CRelativeDistinguishedName::CRelativeDistinguishedName(UUCBufferedReader& reader)
: CASN1SetOf(reader)
{
	
}
	
CRelativeDistinguishedName::CRelativeDistinguishedName(const CASN1Object& name)
: CASN1SetOf(name)
{
}

/*
void CRelativeDistinguishedName::addAttributeValue(const CAttributeValueAssertion& value)
{
	addElement(new CAttributeValueAssertion(value));
}
*/

CRelativeDistinguishedName::~CRelativeDistinguishedName()
{

}
