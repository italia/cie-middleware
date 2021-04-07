/*
 *  PKIStatusInfo.h
 *  iDigitalSApp
 *
 *  Created by svp on 19/03/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PKISTATUSINFO_H
#define _PKISTATUSINFO_H

#include "TimeStampToken.h"


class CPKIStatusInfo : public CASN1Sequence  
{
public:
	CPKIStatusInfo(UUCBufferedReader& reader);
	
	CPKIStatusInfo(const CASN1Object& PKIStatusInfo);
	
	virtual ~CPKIStatusInfo();
	
	CASN1Integer getStatus();
};

#endif