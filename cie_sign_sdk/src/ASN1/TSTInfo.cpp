// TSTInfo.cpp: implementation of the CTSTInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "TSTInfo.h"
#include "ASN1OptionalField.h"
#include "ASN1Exception.h"
#include "ASN1UTCTime.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTSTInfo::CTSTInfo(UUCBufferedReader& reader)
:CASN1Sequence(reader)
{
/*
	addElement(new CVersion(reader));
	addElement(new CASN1ObjectIdentifier(reader));

	addElement(new CMessageImprint(reader));
	addElement(new CASN1Integer(reader));  // serial number
	addElement(new CASN1UTCTime(reader));  // time

	// Optional Field
	CASN1OptionalField* pAccuracy = NULL;
	if (CASN1OptionalField::isOptionaField(0x04, reader))
	{
		try
		{
			pAccuracy = new CASN1OptionalField(new CAccuracy(reader), 0x04); 
		}
		catch(CASN1ObjectNotFoundException* ex)
		{
			delete ex;
		
		}
	}
	addElement(pAccuracy);

	addElement(new CASN1Boolean(reader));
	
	reader.mark();

	// Optional Field
	CASN1OptionalField* pNonce = NULL;
	if (CASN1OptionalField::isOptionaField(0x00, reader))
	{
		try
		{
			pNonce = new CASN1OptionalField(new CASN1Integer(reader), 0x06); 
		}
		catch(CASN1ObjectNotFoundException* ex)
		{
			reader.reset();
			delete ex;
		
		}
	}
	addElement(pNonce);
/*
	reader.mark();

	// Optional tsa
	// Currently treated as Ignored
	CASN1OptionalField* pTsa = NULL;
	if (CASN1OptionalField::isOptionaField(0x00, reader))
	{
		try
		{
			pTsa
				= new CASN1OptionalField(new CASN1Ignored(reader), 0x00); 
		}
		catch(CASN1ObjectNotFoundException* ex)
		{
			reader.reset();
			delete ex;			
		}
	}

	addElement(pTsa);

	reader.mark();

	// Optional extensions
	// Currently treated as Ignored
	CASN1OptionalField* pExtensions = NULL;
	if (CASN1OptionalField::isOptionaField(0x01, reader))
	{
		try
		{
			pExtensions
				= new CASN1OptionalField(new CASN1Ignored(reader), 0x01); 
		}
		catch(CASN1ObjectNotFoundException* ex)
		{
			reader.reset();
			delete ex;			
		}
	}

	addElement(pExtensions);
*/
}

/*
CTSTInfo::CTSTInfo(const CASN1ObjectIdentifier& policyID, const CMessageImprint& messImprint,const CASN1Integer& serialNumber,const CASN1UTCTime& genTime)
{
	addElement(new CVersion(1));
	addElement(new CASN1ObjectIdentifier(policyID));

	addElement(new CMessageImprint(messImprint));
	addElement(new CASN1Integer(serialNumber));
	addElement(new CASN1UTCTime(genTime));
	addElement(NULL);
	addElement(new CASN1Boolean(FALSE));
	addElement(NULL);
//	addElement(NULL);
//	addElement(NULL);


}
*/

CTSTInfo::CTSTInfo(const CASN1Object& tstInfo)
:CASN1Sequence(tstInfo)
{
}

CTSTInfo::~CTSTInfo()
{
}

CASN1Sequence CTSTInfo::getMessageImprint()
{
	return elementAt(2);
}


CASN1UTCTime CTSTInfo::getUTCTime()
{
	return elementAt(4);
}

CAlgorithmIdentifier CTSTInfo::getDigestAlgorithn()
{
	CASN1Sequence messageImprint(elementAt(2));
	return (CAlgorithmIdentifier)messageImprint.elementAt(0);
}

CASN1Integer CTSTInfo::getSerialNumber()
{
	return elementAt(3);
}

CName CTSTInfo::getTSAName()
{
	int siz = size();
	for(int i = 1; i < siz; i++)
	{
		CASN1Object obj = elementAt(i);
		if(obj.getTag() == 0xA0)
		{
			CASN1Sequence val(obj);
			CASN1Sequence val1(val.elementAt(0));
			return val1.elementAt(0);
		}
	}
	
	throw -1;
}

