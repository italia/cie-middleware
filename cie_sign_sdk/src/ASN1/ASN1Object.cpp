
//#include "stdafx.h"
#include "ASN1Object.h"
#include <memory.h>
#include <math.h>
#include "ASN1Exception.h"


// costruttori
CASN1Object::CASN1Object()
: m_indefiniteLen(false), m_btLenRead(0)
{
}

CASN1Object::CASN1Object(const CASN1Object& obj)
: m_indefiniteLen(false), m_btLenRead(0)
{
	m_btTag = obj.getTag();
	m_value.append(*(obj.getValue()));
	//setValue(*(obj.getValue()));
}

CASN1Object::CASN1Object(BYTE btTag, const UUCByteArray& value)
: m_indefiniteLen(false), m_btLenRead(0)
{  
	m_btTag = btTag;
	m_value.append(value);
	//setValue(value);
}

CASN1Object::CASN1Object(BYTE btTag)
: m_indefiniteLen(false), m_btLenRead(0)
{  
	m_btTag = btTag;
}


CASN1Object::CASN1Object(UUCBufferedReader& reader)
: m_indefiniteLen(false), m_btLenRead(0)
{	
	fromReader(reader);
}


CASN1Object::CASN1Object(const UUCByteArray& content)
: m_indefiniteLen(false), m_btLenRead(0)
{
	fromByteArray(content);
}

CASN1Object::CASN1Object(const BYTE* value, long len)
: m_indefiniteLen(false), m_btLenRead(0)
{
	UUCBufferedReader reader(value, len);
	fromReader(reader);
}

// distruttore
CASN1Object::~CASN1Object()
{
//#ifdef DEBUG
	//NSLog(@"~CASN1Object()");
//#endif
}

//metodi

BYTE CASN1Object::getOrigLenLen() const
{
	return m_btLenRead;
}

BYTE CASN1Object::getTag() const 
{
    return m_btTag;
}
    
void CASN1Object::setTag(BYTE tag)
{
	m_btTag = tag;
}

UINT CASN1Object::getLength() const 
{
    return m_value.getLength();
}
    
const UUCByteArray* CASN1Object::getValue() const 
{
	return &m_value;
}

void CASN1Object::setValue(const UUCByteArray& value)
{
	m_value.removeAll();
	
    if(value.getLength() > 0)
    {
		m_value.append(value);
    }
}

void CASN1Object::setValue(const BYTE* value, long len)
{
	m_value.removeAll();
	
    if(len > 0)
    {
		m_value.append(value, len);
    }
}

int CASN1Object::getSerializedLength()
{
	return getSerializedLength(m_value.getLength(), m_indefiniteLen);
}

int CASN1Object::getSerializedLength(int nLen, bool indefiniteLen)
{
	//int nLen = getLength();
	int nTLVLen;

	if (nLen < 0x80)
	{
		nTLVLen    = 2 + nLen;	
	}
	else if (indefiniteLen)
	{
		nTLVLen = nLen + 4;
	}
	else //(nLen > 0x80)
	{
		int nLenNeeded;// = (int)(ceil((log((double)nLen) / log((double)2)) / 8));
		int nAuxLen = nLen;
		for(nLenNeeded = 0; nAuxLen > 0; nLenNeeded++, nAuxLen >>= 8);
		nTLVLen    = 2 + nLenNeeded + nLen;				
	}	
	
	return nTLVLen;
}
	
CASN1Object CASN1Object::operator = (const CASN1Object& obj)
{
	setValue(*obj.getValue());
	setTag(obj.getTag());
	return CASN1Object(obj);
}

bool CASN1Object::operator == (const CASN1Object& obj) const
{
		if(getTag() != obj.getTag())
		return false;
	
	if(getLength() != obj.getLength())
		return false;
	
	const BYTE* val1 = getValue()->getContent();
	const BYTE* val2 = obj.getValue()->getContent();
	int r = memcmp((void*)val1, (void*)val2, getLength()); 
	return  r == 0;
}

bool CASN1Object::operator != (const CASN1Object& obj) const
{
	return(!operator == (obj));
}

void CASN1Object::toByteArray(UUCByteArray& byteArray) const
{	
	//UUCByteArray serializedForm;
	BYTE* pbtSerialized = NULL;
	int nTLVLen;
	unsigned int nLen = getLength();
		
	//if (nLen < 0x00000080)
	//{
		// indefinite length
		//To Do
	//}	
	if(nLen < 0x80)	
	{
		// Short Form
		nTLVLen    = 2 + nLen;
		pbtSerialized = new BYTE[nTLVLen+1];
		
		pbtSerialized[0] = getTag();
		pbtSerialized[1] = (BYTE)nLen;
					
		memcpy((pbtSerialized + 2), getValue()->getContent(), nLen);	
	
	}
	else //if (nLen >= 0x80)
	{
		// Long Form
		//int nLenNeeded = (int)(ceil((log((double)nLen) / log((double)2)) / 8));
		int nLenNeeded = 0;
		int nAuxLen = nLen;
		for(nLenNeeded = 0; nAuxLen > 0; nLenNeeded++, nAuxLen >>= 8);

		nTLVLen    = 2 + nLenNeeded + nLen;
		
		pbtSerialized = new BYTE[nTLVLen];
		
		pbtSerialized[0] = getTag();
		pbtSerialized[1] = (BYTE)(0x80 + nLenNeeded);
		int nDigit = 0;
		int i = 0;
		int nAux = nLen;
		for(i = 0; i < nLenNeeded; i++)				
		{
			nDigit = nAux >> (256 * i);			
			pbtSerialized[2 + (nLenNeeded - i - 1)] = (BYTE)nDigit;
			nAux = nAux / 256;					
		}					
									
		memcpy((pbtSerialized + 2 + (nLenNeeded)), getValue()->getContent(), nLen);												
	}	
	
	byteArray.append(pbtSerialized, nTLVLen);

	delete pbtSerialized;

	//return serializedForm;
}


void CASN1Object::fromByteArray(const UUCByteArray& content)
{
	UUCBufferedReader reader(content);
	
	fromReader(reader);
}

void CASN1Object::fromByteArray(const BYTE* pContent, int iLen)
{
	UUCBufferedReader reader(pContent, iLen);

	fromReader(reader);
}


void CASN1Object::fromReader(UUCBufferedReader& reader)
{
	CASN1Object::parseLen(reader, &m_btTag, &m_value, &m_btLenRead, NULL);

}



int CASN1Object::parseLen(UUCBufferedReader& reader, BYTE* pbtTag, UUCByteArray* pValue, BYTE* pbtLenRead, bool* pbIndefiniteLen)
{
	BYTE btTag = 0;
	BYTE btLenRead;
	UINT nLen = 0;
	BYTE btHexLen[10];
	if (pbIndefiniteLen)
		*pbIndefiniteLen = false;

	if (pbtLenRead)
		*pbtLenRead = 0;
	if (!pbtTag)
		pbtTag = &btTag;

	//Read Tag
	if (!reader.read(pbtTag, 1))
		throw CASN1ObjectNotFoundException("");

	// Read Len
	if (!reader.read(&btLenRead, 1))
		throw CASN1ParsingException();


	if (btLenRead == 0x80)  // indefinite-length
	{
		UUCByteArray buffer;

		parseBER(reader, buffer);

		if (pValue)
			pValue->append(buffer.getContent(), buffer.getLength());

		if (pbIndefiniteLen)
			*pbIndefiniteLen = true;

		if (pbtLenRead)
			*pbtLenRead = 0;
		return buffer.getLength();
	}
	else if ((btLenRead & 0x80) == 0x80)
	{
		// Long Form		
		btLenRead = btLenRead & 0x7F;

		if (reader.read(btHexLen, btLenRead) != btLenRead)
		{
			throw CASN1ParsingException();
		}

		for (BYTE i = 0; i < btLenRead; i++)
		{
			nLen += (UINT)(btHexLen[btLenRead - i - 1] * pow((double)256, i));
		}

		if (pbtLenRead)
			*pbtLenRead = btLenRead;
	}
	else
	{
		// Short Form
		nLen = btLenRead & 0x000000FF;
		if (pbtLenRead)
			*pbtLenRead = 0;
	}

	// legge il resto dei 

	if (pValue)
	{

		BYTE* pbtVal = new BYTE[nLen];
		unsigned int n;
		if ((n = reader.read(pbtVal, nLen)) < nLen)
		{
			delete pbtVal;
			throw CASN1ParsingException();
		}

		pValue->append(pbtVal, nLen);

		delete pbtVal;
	}
	return nLen;
}



const char* CASN1Object::toHexString()
{
	toByteArray(m_der);
	return m_der.toHexString();
}

int CASN1Object::parseBER(UUCBufferedReader& reader, UUCByteArray& buffer)
{
	int begin = reader.getPosition();
	
	CASN1Object obj(reader);
	
	int end = reader.getPosition();
	
	obj.toByteArray(buffer);
	
	BYTE btEnd[2];
	int n;
	
	//reader.mark();
	
	if((n = reader.read(btEnd, 2)) < 2)
	{
		
		throw CASN1ParsingException();										
	}
	
	if(btEnd[0] == 0x00 && btEnd[1] == 0x00)
	{
		return end - begin;
	}
	else 
	{
		reader.setPosition(end);//reset();
		return end - begin + parseBER(reader,  buffer);//(reader).getSerializedLength();
	}

}













