// ASN1BitString.cpp: implementation of the CASN1BitString class.
//
//////////////////////////////////////////////////////////////////////

#include "ASN1BitString.h"
#include "ASN1Exception.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const BYTE CASN1BitString::TAG = 0x03;
		
CASN1BitString::~CASN1BitString()
{
	
}

CASN1BitString::CASN1BitString(UUCBufferedReader& reader)
: CASN1Object(reader)
{	
	/*
	// check unused bit	
	reader.mark();

	reader.read(&m_unusedbit, 1);
		
	// try encapsulated first	
	try
	{
		m_btVal.m_pSeq = new CASN1IntegerSequence(reader);
		m_bEncapsulated = true;
		reader.releaseMark();
	}
	catch(CASN1Exception* ex)
	{	
		delete ex;
		reader.reset();
		m_bEncapsulated = false;
		m_btVal.m_pbtVal = deserialize(reader);	
	}
	 */
	
}

CASN1BitString::CASN1BitString(const CASN1Object& obj)
: CASN1Object(obj)
{
		
}
