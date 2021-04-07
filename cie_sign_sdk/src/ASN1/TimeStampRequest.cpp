/*
 *  TimeStampRequest.cpp
 *  iDigitalSApp
 *
 *  Created by svp on 22/03/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "TimeStampRequest.h"
#include "ASN1Boolean.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTimeStampRequest::CTimeStampRequest(UUCBufferedReader& reader)
:CASN1Sequence(reader)
{
	
}

CTimeStampRequest::CTimeStampRequest(const CASN1Object& timeStampToken)
:CASN1Sequence(timeStampToken)
{
	
}

CTimeStampRequest::CTimeStampRequest(const char* szHashAlgoOID, UUCByteArray& digest, const char* szPolicyOID, CASN1Integer& nounce)
: CASN1Sequence()
{
	addElement(CASN1Integer(1));
	CASN1Sequence messageImprint;
	
	messageImprint.addElement(CAlgorithmIdentifier(szHashAlgoOID));
	messageImprint.addElement(CASN1OctetString(digest));
	addElement(messageImprint);
	
	if(szPolicyOID != NULL && strlen(szPolicyOID) > 0)
	{
		CASN1ObjectIdentifier policyOid(szPolicyOID);//"1.3.6.1.4.1.29741.1.1.6");
		addElement(policyOid);
	}
	
	addElement(nounce);
	addElement(CASN1Boolean(true)); // certReq
}

CTimeStampRequest::~CTimeStampRequest()
{
	
}
