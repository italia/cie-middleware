#ifndef _CONTENTINFO_H
#define _CONTENTINFO_H


#include "ASN1Sequence.h"
#include "ContentType.h"

class CContentInfo : public CASN1Sequence
{
	// Defined as 
	// ContentInfo ::= SEQUENCE {
	// 		contentType ContentType,
	//		content [0] EXPLICIT ANY DEFINED BY contentType OPTIONAL}
	
public:	
	CContentInfo(const CContentType& contentType, const CASN1Object& content);
	
	CContentInfo(const CContentType& contentType);

	CContentInfo(UUCBufferedReader& reader);

	CContentInfo(const CASN1Object& contentInfo);
	
	virtual ~CContentInfo();
	
	void setContent(const CASN1Object& content);

	CContentType getContentType();
	
	CASN1Object getContent();

};

#endif //_CONTENTINFO_H