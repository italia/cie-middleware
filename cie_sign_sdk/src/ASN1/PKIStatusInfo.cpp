/*
 *  PKIStatusInfo.cpp
 *  iDigitalSApp
 *
 *  Created by svp on 19/03/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "PKIStatusInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPKIStatusInfo::CPKIStatusInfo(UUCBufferedReader& reader)
: CASN1Sequence(reader)
{
	
}

CPKIStatusInfo::CPKIStatusInfo(const CASN1Object& pkiStatusInfo)
: CASN1Sequence(pkiStatusInfo)
{
	
}

CPKIStatusInfo::~CPKIStatusInfo()
{
	
}


CASN1Integer CPKIStatusInfo::getStatus()
{
	return elementAt(0);
}