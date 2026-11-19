/*
 *  TimeStampRequest.h
 *  iDigitalSApp
 *
 *  Created by svp on 22/03/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include "TimeStampToken.h"

class CTimeStampRequest : public CASN1Sequence  
{
public:
	CTimeStampRequest(UUCBufferedReader& reader);
	
	CTimeStampRequest(const CASN1Object& timeStampToken);
	
	CTimeStampRequest(const char* szHashAlgoOID, UUCByteArray& digest, const char* szPolicyOID, CASN1Integer& nounce);
	
	
	virtual ~CTimeStampRequest();
	
};