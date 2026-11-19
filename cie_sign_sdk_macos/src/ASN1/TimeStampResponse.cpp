/*
 *  TimeStampResponse.cpp
 *  iDigitalSApp
 *
 *  Created by svp on 19/03/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "TimeStampResponse.h"
#include "TimeStampToken.h"
#include "ContentInfo.h"
#include "ContentType.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeStampResponse::CTimeStampResponse(UUCBufferedReader& reader)
: CASN1Sequence(reader)
{
	
}

CTimeStampResponse::CTimeStampResponse(const CASN1Object& timeStampresponse)
: CASN1Sequence(timeStampresponse)
{
	
}

CTimeStampResponse::CTimeStampResponse(const BYTE* content, int length)
: CASN1Sequence(content, length)
{

}

CTimeStampResponse::~CTimeStampResponse()
{
	
}

CTimeStampToken CTimeStampResponse::getTimeStampToken()
{
	return elementAt(1);
}

CPKIStatusInfo CTimeStampResponse::getPKIStatusInfo()
{
	return elementAt(0);
}

int CTimeStampResponse::verify()
{
	return verify(NULL);
}

int CTimeStampResponse::verify(const char* szDateTime)
{
	CTimeStampToken tst(elementAt(1));	
	return tst.verify(szDateTime, NULL);
}

