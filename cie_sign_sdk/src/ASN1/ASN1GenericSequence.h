
#ifndef _ASN1GENERICSEQUENCE_H
#define _ASN1GENERICSEQUENCE_H

#include "ASN1Object.h"
#define MAXSIZE 100

class CASN1GenericSequence : public CASN1Object
{
public:
	
	CASN1GenericSequence(BYTE btTag);
	CASN1GenericSequence(UUCBufferedReader& reader);
	CASN1GenericSequence(const UUCByteArray& content);
	CASN1GenericSequence(const CASN1Object& obj);
	CASN1GenericSequence(const CASN1GenericSequence& obj);
	CASN1GenericSequence(const BYTE* value, long len);

	virtual ~CASN1GenericSequence();

    CASN1GenericSequence& operator = (const CASN1GenericSequence&);
    
	void addElement(const CASN1Object& obj);
	void addElementAt(const CASN1Object& pObj, int nPos);
    CASN1Object elementAt(int nPos);
    CASN1Object nextElement();
    CASN1Object elementAtOpt(int nPos);
    CASN1Object nextElementOpt();
	void setElementAt(const CASN1Object& obj, int nPos);
	void removeElementAt(int nPos);
	void removeAll();
	bool isPresent(int nPos) const;
	unsigned int size() const;
	void fromByteArray(const UUCByteArray& content);

    //void init(unsigned int initialsize);
    //void autogrow(unsigned int additionalsize);
    
protected:

private:
	//CASN1GenericSequence();

	//unsigned int getOffset(int nPos) const;
	unsigned int m_nextOffset;

	unsigned int * m_pnOffsets;
	unsigned int m_nOffsetsMax;
	int m_nSize;

	int makeOffset();
};

#endif // _ASN1GENERICSEQUENCE_H
