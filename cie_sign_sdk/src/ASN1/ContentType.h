#ifndef _CONTENTTYPE_H
#define _CONTENTTYPE_H


#include "ASN1ObjectIdentifier.h"

class CContentType : public CASN1ObjectIdentifier
{		
public:
	static const char* OID_TYPE_DATA;
	static const char* OID_TYPE_SIGNED;
	static const char* OID_TYPE_ENVELOPED;
	static const char* OID_TYPE_SIGNED_ENVELOPED;
	static const char* OID_TYPE_DIGEST;
	static const char* OID_TYPE_ENCRYPTED;
	static const char* OID_TYPE_TSTINFO;
/*						
	static const CContentType TYPE_DATA;
	static const CContentType TYPE_SIGNED;
	static const CContentType TYPE_ENVELOPED;
	static const CContentType TYPE_SIGNED_ENVELOPED;
	static const CContentType TYPE_DIGEST;
	static const CContentType TYPE_ENCRYPTED;
	static const CContentType TYPE_TSTINFO;
*/		
	CContentType(UUCBufferedReader& reader);		

	CContentType(const CASN1Object& contentType);

	CContentType(char* lpszOId);

	CContentType(const CASN1ObjectIdentifier& algoId);
	
	virtual ~CContentType();
	
	
};

#endif //_CONTENTTYPE_H