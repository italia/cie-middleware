
#include "ASN1Setof.h"

const BYTE CASN1SetOf::TAG = 0x31;

CASN1SetOf::CASN1SetOf()
: CASN1GenericSequence(TAG)
{
}

CASN1SetOf::CASN1SetOf(const CASN1Object& obj)
: CASN1GenericSequence(obj)
{
	setTag(TAG);
}
 
CASN1SetOf::~CASN1SetOf()
{
	
}