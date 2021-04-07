
#ifndef _ASN1BOOLEAN_H
#define _ASN1BOOLEAN_H

#include "ASN1Integer.h"


class CASN1Boolean : public CASN1Object
{
private:
	static const BYTE TAG;
	
public:
	    // costruttori	

	 CASN1Boolean(bool val);

	 CASN1Boolean(const CASN1Object&);
	
	CASN1Boolean(UUCBufferedReader& reader);
		// distruttori

 	~CASN1Boolean();

	bool getBoolValue() const;
};

#endif //ASN1BOOLEAN