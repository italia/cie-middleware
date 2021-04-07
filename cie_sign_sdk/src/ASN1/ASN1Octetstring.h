#ifndef _ASN1OCTECTSTRING_H
#define _ASN1OCTECTSTRING_H


#include "ASN1Object.h"

class CASN1OctetString : public CASN1Object
{
private:
		const static BYTE TAG;
	
public: 

	//Costruttore
	CASN1OctetString(UUCBufferedReader& reader);
			
	CASN1OctetString(const UUCByteArray& bOctetString);

	CASN1OctetString(const char* szOctetString);

	CASN1OctetString(const CASN1Object& octetString);
	
	CASN1OctetString(const BYTE* value, long len);

	//Distruttore
	~CASN1OctetString();
};

#endif //ASN1OCTECTSTRING