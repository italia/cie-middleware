
#include "ASN1Sequence.h"

const BYTE CASN1Sequence::TAG = 0x30;


CASN1Sequence::CASN1Sequence()
: CASN1GenericSequence(TAG)
{
}

CASN1Sequence::CASN1Sequence(UUCBufferedReader& reader)
: CASN1GenericSequence(reader)
{
	setTag(TAG);
}

CASN1Sequence::CASN1Sequence(const CASN1Object& obj)
: CASN1GenericSequence(obj)
{
	setTag(TAG);
}

CASN1Sequence::CASN1Sequence(const BYTE* value, long len)
: CASN1GenericSequence(value, len)
{

}

CASN1Sequence::CASN1Sequence(const UUCByteArray& content)
: CASN1GenericSequence(content)
{

}

CASN1Sequence::~CASN1Sequence()
{
	//NSLog(@"~CASN1Sequence()");
}
