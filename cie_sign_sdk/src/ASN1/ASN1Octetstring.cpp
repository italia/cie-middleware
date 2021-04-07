

#include "ASN1Integer.h"
#include "ASN1Octetstring.h"
//#include "ASN1Exception.h"


const BYTE CASN1OctetString::TAG = 0x04;

	// distruttori
CASN1OctetString::~CASN1OctetString()
{ 
}

// costruttori
CASN1OctetString::CASN1OctetString(UUCBufferedReader& reader)
: CASN1Object(reader)
{		

}

CASN1OctetString::CASN1OctetString(const char* szOctetString)
: CASN1Object(TAG)
{
	UUCByteArray octetString((BYTE*)szOctetString, strlen(szOctetString));
	setValue(octetString);
}

CASN1OctetString::CASN1OctetString(const UUCByteArray& octetString)
: CASN1Object(TAG)
{	
	setValue(octetString);	
}

CASN1OctetString::CASN1OctetString(const BYTE* value, long len)
: CASN1Object(TAG)
{	
	setValue(value, len);	
}

CASN1OctetString::CASN1OctetString(const CASN1Object& octetString)
: CASN1Object(octetString)
{
}