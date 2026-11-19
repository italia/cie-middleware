/*
 *  TimeStampData.h
 *  iDigitalSApp
 *
 *  Created by svp on 22/03/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "TimeStampToken.h"
#include "PKIStatusInfo.h"
#include "ASN1Octetstring.h"
#include "disigonsdk.h"

/*
 ContentInfo ::= SEQUENCE {
	contentType ContentType,
	content [0] EXPLICIT ANY DEFINED BY contentType }

ContentType ::= OBJECT IDENTIFIER
id-ct-timestampedData OBJECT IDENTIFIER ::= {
	iso(1) member-body(2) us(840) rsadsi(113549) pkcs(1)
	pkcs9(9) id-smime(16) id-ct(1) 31 }

This particular OID signals that the content field of the ContentInfo
has the following syntax:

TimeStampedData ::= SEQUENCE {
	version              INTEGER { v1(1) },
	dataUri              IA5String OPTIONAL,
	metaData             MetaData OPTIONAL,
	content              OCTET STRING OPTIONAL,
	temporalEvidence     Evidence
}

MetaData ::= SEQUENCE {
	hashProtected        BOOLEAN,
	fileName             UTF8String OPTIONAL,
	mediaType            IA5String OPTIONAL,
	otherMetaData        Attributes OPTIONAL
}

Attributes ::=
SET SIZE(1..MAX) OF Attribute -- according to RFC 5652

Evidence ::= CHOICE {
	tstEvidence    [0] TimeStampTokenEvidence,   -- see RFC 3161
	ersEvidence    [1] EvidenceRecord,           -- see RFC 4998
	otherEvidence  [2] OtherEvidence
}

OtherEvidence ::= SEQUENCE {
	oeType               OBJECT IDENTIFIER,
	oeValue              ANY DEFINED BY oeType }

TimeStampTokenEvidence ::=
SEQUENCE SIZE(1..MAX) OF TimeStampAndCRL

TimeStampAndCRL ::= SEQUENCE {
	timeStamp   TimeStampToken,          -- according to RFC 3161
	crl         CertificateList OPTIONAL -- according to RFC 5280
}	
*/

class CTimeStampData : public CContentInfo  
{
public:
	CTimeStampData(UUCBufferedReader& reader);
	
	CTimeStampData(const CASN1Object& timeStampData);
	
	CTimeStampData(UUCByteArray& content, CTimeStampToken& tst);
		
	virtual ~CTimeStampData();
	
	int verify(REVOCATION_INFO* pRevocationInfo);
	
	int verify(const char* szDateTime, REVOCATION_INFO* pRevocationInfo);
	
	CTimeStampToken getTimeStampToken();

	CASN1OctetString getTimeStampDataContent();
	
};
