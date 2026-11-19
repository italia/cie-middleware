/*
 *  TimeStampData.cpp
 *  iDigitalSApp
 *
 *  Created by svp on 22/03/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


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

#include "TimeStampData.h"
#include "ASN1OptionalField.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTimeStampData::CTimeStampData(UUCBufferedReader& reader)
:CContentInfo(reader)
{
	
}

CTimeStampData::CTimeStampData(const CASN1Object& timeStampData)
:CContentInfo(timeStampData)
{
	
}

CTimeStampData::CTimeStampData(UUCByteArray& content, CTimeStampToken& tst)
: CContentInfo(CContentType(szTimeStampDataOID))
{
	CASN1Sequence timeStampData;
	timeStampData.addElement(CASN1Integer(1)); // version
	
	timeStampData.addElement(CASN1OctetString(content)); // content
	
	CASN1Sequence evidence;
	CASN1Sequence tsAndCrl;
	
	tsAndCrl.addElement(tst);
	evidence.addElement(tsAndCrl);
	timeStampData.addElement(CASN1OptionalField(evidence, 0));

	
	setContent(timeStampData);
}

CTimeStampData::~CTimeStampData()
{
	//NSLog(@"~CASN1Object()");
}

int CTimeStampData::verify(REVOCATION_INFO* pRevocationInfo)
{
	return verify(NULL, pRevocationInfo);
}

int CTimeStampData::verify(const char* szDateTime, REVOCATION_INFO* pRevocationInfo)
{
	CTimeStampToken tst(getTimeStampToken());
	
	return tst.verify(szDateTime, pRevocationInfo);
}

CTimeStampToken CTimeStampData::getTimeStampToken()
{
	CASN1Sequence timeStampData(getContent());

	int size = timeStampData.size();

	CASN1Sequence evidence(timeStampData.elementAt(size -1));
	CASN1Sequence tsAndCrl(evidence.elementAt(0));
	
	CTimeStampToken tst(tsAndCrl.elementAt(0));
	
	return tst;
}

CASN1OctetString CTimeStampData::getTimeStampDataContent()
{
	CASN1Sequence timeStampData(getContent());

	int size = timeStampData.size();

	return timeStampData.elementAt(size - 2);
}

