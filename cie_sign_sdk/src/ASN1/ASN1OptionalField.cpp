
#include "ASN1OptionalField.h"
#include "ASN1Exception.h"


const BYTE CASN1OptionalField::TAG = 0xA0;
			
CASN1OptionalField::CASN1OptionalField(const CASN1Object& asn1Obj, const BYTE& btClass)
: CASN1Object(asn1Obj), m_btClass(btClass)
{	
	
}

CASN1OptionalField::CASN1OptionalField(const CASN1Object& opt)
: CASN1Object(opt)
{	
}

CASN1OptionalField::CASN1OptionalField(UUCBufferedReader& reader)
: CASN1Object(reader)
{	
}

CASN1OptionalField::~CASN1OptionalField()
{
	
}

BYTE CASN1OptionalField::getTag() const
{
	return (BYTE)(TAG | m_btClass);
}

/*
BOOL CASN1OptionalField::isOptionaField(const BYTE& btClass, CBufferedReader& reader)
{
	reader.mark();
	try
	{
		TL* pTl = getTL(reader);
		if (pTl->getTag() == (TAG | btClass))
		{
			//reader.mark(0);
			//reader.reset();
			delete pTl;
			return true;
		}
		delete pTl;
	}
	catch(CASN1Exception* ex)
	{
	}
	
	reader.reset();
	
	

	return false;	
}
*/

