#ifndef _ASN1OPTIONALFIELD_H
#define _ASN1OPTIONALFIELD_H


#include "ASN1Object.h"

class CASN1OptionalField : public CASN1Object
{
	
public:
	
	CASN1OptionalField(UUCBufferedReader& reader);
	
	CASN1OptionalField(const CASN1Object& pAsn1Obj, const BYTE& btClass);

	CASN1OptionalField(const CASN1Object& opt);

	~CASN1OptionalField();

	//CASN1Object getObject() const;

	BYTE getTag() const;
	//static BOOL isOptionaField(const BYTE& btClass, UUCBufferedReader& reader);
	
private:
	
	static const BYTE TAG;
	BYTE m_btClass;
};


#endif  //_ASN1OPTIONALFIELD_H