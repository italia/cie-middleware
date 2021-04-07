
#include "ContentInfo.h"
//#include "SignedData.h"
//#include "ASN1Exception.h"
#include "ASN1OptionalField.h"
//#include "Data.h"
//#include "TSTInfo.h"

	
CContentInfo::~CContentInfo()
{
	//NSLog(@"~CContentInfo()");
}


CContentInfo::CContentInfo(const CASN1Object& contentInfo)
: CASN1Sequence(contentInfo)
{
}

CContentInfo::CContentInfo(UUCBufferedReader& reader)
: CASN1Sequence(reader)
{
}

CContentInfo::CContentInfo(const CContentType& contentType)
{
	addElement(contentType);
}	

CContentInfo::CContentInfo(const CContentType& contentType, const CASN1Object& content)
{
	addElement(contentType);

	CASN1Sequence innerContent;
	innerContent.addElement(content);
	addElement(CASN1OptionalField(innerContent, 0));
	
	/*
	CASN1Object* pContent = NULL;

	try
	{
		// Content 
		if(contentType.equals(CContentType(CContentType::OID_TYPE_DATA)))
		{			
			pContent = new CASN1OptionalField(new CData(*(CData*)(&content)), 0x00);
		}
		else if(contentType.equals(CContentType(CContentType::OID_TYPE_DIGEST)))
		{
		}
		else if(contentType.equals(CContentType(CContentType::OID_TYPE_ENCRYPTED)))
		{
		}
		else if(contentType.equals(CContentType(CContentType::OID_TYPE_ENVELOPED)))
		{
		}
		else if(contentType.equals(CContentType(CContentType::OID_TYPE_SIGNED)))
		{
			pContent = new CASN1OptionalField(new CSignedData(*(CSignedData*)(&content)),0x00);
		}
		else if(contentType.equals(CContentType(CContentType::OID_TYPE_SIGNED_ENVELOPED)))
		{
		}
		else if(contentType.equals(CContentType(CContentType::OID_TYPE_TSTINFO)))
		{
			pContent = new CASN1OptionalField(new CTSTInfo(*(CTSTInfo*)(&content)),0x00);
		}
		else
		{
			throw new CBadContentTypeException();
		}
	}
	catch(CASN1BadObjectIdException* ex)
	{
		throw ex;
	}
	catch(...)
	{
		throw new CASN1ObjectNotFoundException("CContentType");
	}

	addElement(*pContent);
	
	delete pContent;
*/
	
}

void CContentInfo::setContent(const CASN1Object& content)
{
	CASN1Sequence innerContent;
	innerContent.addElement(content);
	if(size() < 2)
		addElement(CASN1OptionalField(innerContent, 0));
	else 
	{
		setElementAt(CASN1OptionalField(innerContent, 0), 1);
	}
}

CContentType CContentInfo::getContentType()
{
	return CContentType(elementAt(0));
}
	
CASN1Object CContentInfo::getContent() {
	// il getValue è necessario perchè il content è dichiarato explicit [0] 
	return CASN1Object(*elementAt(1).getValue());
}
