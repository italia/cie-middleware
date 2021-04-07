
#ifndef _ASN1OBJECT_
#define _ASN1OBJECT_

#include "definitions.h"
#include <stdio.h>
#include "UUCBufferedReader.h"
#include "UUCByteArray.h"


class CASN1Object
{	

public:

	// Constructors
	CASN1Object();
	CASN1Object(const CASN1Object& obj);
	CASN1Object(BYTE btTag, const UUCByteArray& value);
	CASN1Object(BYTE btTag);
	CASN1Object(UUCBufferedReader& reader);
	CASN1Object(const UUCByteArray& content);
	CASN1Object(const BYTE* value, long len);

	virtual ~CASN1Object();

	virtual BYTE  getTag()    const;		
	UINT  getLength() const;	

	const UUCByteArray* getValue() const;	
	void setValue(const UUCByteArray& value);
	void setValue(const BYTE* value, long len);
	
	void setTag(BYTE tag);
	
	int getSerializedLength();
	static int getSerializedLength(int nLen, bool indefiniteLen);
	
	void toByteArray(UUCByteArray& byteArray) const;
	void fromByteArray(const UUCByteArray& content);
	void fromByteArray(const BYTE* pContent, int iLen);
	static int parseLen(UUCBufferedReader& reader, BYTE* pbtTag, UUCByteArray* pValue, BYTE* pbtLenRead, bool* pbIndefiniteLen);
	void fromReader(UUCBufferedReader& reader);

	CASN1Object operator = (const CASN1Object& obj);
	
	bool operator == (const CASN1Object& obj) const;
	bool operator != (const CASN1Object& obj) const;

	const char* toHexString();

	BYTE getOrigLenLen() const; // number of bytes to represent len

protected:		
	BYTE   m_btTag;
    UUCByteArray  m_value;
	
	static int parseBER(UUCBufferedReader& reader, UUCByteArray& buffer);
	
	bool m_indefiniteLen;
	BYTE m_btLenRead;
	UUCByteArray m_der;
};

#endif //_ASN1OBJECT_