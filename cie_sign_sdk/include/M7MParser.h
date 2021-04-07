/*
 *  M7MParser.h
 *  iDigitalSApp
 *
 *  Created by svp on 05/09/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _M7MPARSER_H_
#define _M7MPARSER_H_

#include <string>
#include "ASN1/UUCByteArray.h"

using namespace std;

class M7MParser
{
public:
	M7MParser();
	
	int Load(const char* m7m, int m7mlen);
	
	int GetP7M(UUCByteArray& p7m);
	
	int GetTSR(UUCByteArray& tsr);
	
private:
	UUCByteArray m_p7m;
	UUCByteArray m_tsr;
};



#endif //_M7MPARSER_H_