
#ifndef _ASN1NULL_H
#define _ASN1NULL_H
#include "ASN1Object.h"

class CASN1Null : public CASN1Object
{
private:
	static const BYTE TAG;

public:
	    // costruttori
	 CASN1Null(UUCBufferedReader& reader);	

	 CASN1Null();

	// distruttori

	~CASN1Null();
};

#endif //ASN1BOOLEAN