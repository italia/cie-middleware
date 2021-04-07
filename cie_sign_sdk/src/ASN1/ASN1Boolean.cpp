
#include "ASN1Boolean.h"

const BYTE CASN1Boolean::TAG = 0x01;

	// Distruttori
CASN1Boolean::~CASN1Boolean()
{
	
}

	//Costruttori
CASN1Boolean::CASN1Boolean(UUCBufferedReader& reader)
: CASN1Object(reader)
{	
	
}

CASN1Boolean::CASN1Boolean(bool b)
{
	setTag(TAG);
	UUCByteArray val;
	val.append((BYTE) (b ? 0xFF : 0));
	setValue(val);
}

CASN1Boolean::CASN1Boolean(const CASN1Object& obj)
: CASN1Object(obj)
{
	setTag(TAG);
}
	
bool CASN1Boolean::getBoolValue() const
{
	return getValue()->getContent()[0] == 1;
}	


