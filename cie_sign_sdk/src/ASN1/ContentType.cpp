
#include "ContentType.h"
#include "ASN1Exception.h"

const char* CContentType::OID_TYPE_DATA  				= "1.2.840.113549.1.7.1";
const char* CContentType::OID_TYPE_SIGNED  			= "1.2.840.113549.1.7.2";
const char* CContentType::OID_TYPE_ENVELOPED  		= "1.2.840.113549.1.7.3";
const char* CContentType::OID_TYPE_SIGNED_ENVELOPED  	= "1.2.840.113549.1.7.4";
const char* CContentType::OID_TYPE_DIGEST  			= "1.2.840.113549.1.7.5";
const char* CContentType::OID_TYPE_ENCRYPTED 			= "1.2.840.113549.1.7.6";
const char* CContentType::OID_TYPE_TSTINFO 			= "1.2.840.113549.1.9.16.1.4";
/*					
const CContentType CContentType::TYPE_DATA = CContentType(CASN1ObjectIdentifier(OID_TYPE_DATA));
const CContentType CContentType::TYPE_SIGNED = CContentType(CASN1ObjectIdentifier(OID_TYPE_SIGNED));
const CContentType CContentType::TYPE_ENVELOPED  = CContentType(CASN1ObjectIdentifier(OID_TYPE_ENVELOPED));
const CContentType CContentType::TYPE_SIGNED_ENVELOPED = CContentType(CASN1ObjectIdentifier(OID_TYPE_SIGNED_ENVELOPED));
const CContentType CContentType::TYPE_DIGEST = CContentType(CASN1ObjectIdentifier(OID_TYPE_DIGEST));
const CContentType CContentType::TYPE_ENCRYPTED = CContentType(CASN1ObjectIdentifier(OID_TYPE_ENCRYPTED)); 		
const CContentType CContentType::TYPE_TSTINFO = CContentType(CASN1ObjectIdentifier(OID_TYPE_TSTINFO)); 		
*/	
CContentType::CContentType(const CASN1ObjectIdentifier& algoId)
:CASN1ObjectIdentifier(algoId)
{}

CContentType::CContentType(char* lpszOId)
:CASN1ObjectIdentifier(lpszOId)
{}
	
CContentType::CContentType(UUCBufferedReader& reader)
:CASN1ObjectIdentifier(reader)
{		
/*	if(!(equals(CContentType(OID_TYPE_DATA)) || equals(CContentType(OID_TYPE_SIGNED)) ||
		 equals(CContentType(OID_TYPE_ENVELOPED)) || equals(CContentType(OID_TYPE_SIGNED_ENVELOPED)) ||
		 equals(CContentType(OID_TYPE_DIGEST)) || equals(CContentType(OID_TYPE_ENCRYPTED)) || equals(CContentType(OID_TYPE_TSTINFO))))
		 throw new CBadContentTypeException();
*/
 }	

CContentType::CContentType(const CASN1Object& contentType)
:CASN1ObjectIdentifier(contentType)
{
}

CContentType::~CContentType()
{
}

/*
BOOL CContentType::equals(const CContentType& type) const
{
	return CASN1ObjectIdentifier::equals(type);
}
 */




