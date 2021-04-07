
#include "ASN1Null.h"

const BYTE CASN1Null::TAG = 0x05;

	// Distruttori
CASN1Null::~CASN1Null()
{	
}

	//Costruttori
CASN1Null::CASN1Null(UUCBufferedReader& reader)
: CASN1Object(reader)
{		
}

CASN1Null::CASN1Null()
: CASN1Object(TAG)
{
	
}


