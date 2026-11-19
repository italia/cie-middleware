/*
 *  TimeStampResponse.h
 *  iDigitalSApp
 *
 *  Created by svp on 19/03/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "TimeStampToken.h"
#include "PKIStatusInfo.h"

class CTimeStampResponse : public CASN1Sequence  
{
public:
	CTimeStampResponse(UUCBufferedReader& reader);
	
	CTimeStampResponse(const CASN1Object& timeStampresponse);

	CTimeStampResponse(const BYTE* content, int length);
	
	virtual ~CTimeStampResponse();
	
	CTimeStampToken getTimeStampToken();
	
	CPKIStatusInfo getPKIStatusInfo();
	
	int verify(const char* szDateTime);

	int verify();
	
};