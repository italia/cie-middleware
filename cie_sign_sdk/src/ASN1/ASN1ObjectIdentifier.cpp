
#include "ASN1ObjectIdentifier.h"
#include <math.h>
//#include "ASN1Exception.h"
#include <stdlib.h>
#include "UUCBufferedReader.h"



const  BYTE CASN1ObjectIdentifier::TAG = 0x06;
	
CASN1ObjectIdentifier::~CASN1ObjectIdentifier()
{	
	
}

CASN1ObjectIdentifier::CASN1ObjectIdentifier(UUCBufferedReader& reader)
: CASN1Object(reader)
{					
	
}
	
CASN1ObjectIdentifier::CASN1ObjectIdentifier(const CASN1Object &objId)
: CASN1Object(objId)
{
	//m_pObjId = new CBlob(*(objId.m_pObjId));	
}
	
CASN1ObjectIdentifier::CASN1ObjectIdentifier(const char* strObjId)
:CASN1Object(TAG)
{					
	BYTE out[256];
	int nIndex = 0;
	int nVal;
	int nAux;
	char* szTok;
	char* szOID = new char[strlen(strObjId) + 2];
	strcpy(szOID, strObjId);

	szTok = strtok(szOID, ".");
	
	UINT nFirst = 40 * atoi(szTok) + atoi(strtok(NULL, "."));
	if(nFirst > 0xff)
	{
		delete szOID;
		throw -1;//new CASN1BadObjectIdException(strObjId);
	}
	out[nIndex] = nFirst;
	nIndex++;
		
	int i = 0;
		
	while ((szTok = strtok(NULL, ".")) != NULL)
	{
		nVal = atoi(szTok);
		if(nVal == 0)
		{
			out[nIndex] = 0x00;
			nIndex++;					
		}
		else if (nVal == 1)
		{
			out[nIndex] = 0x01;
			nIndex++;					
		}
		else
		{
			i = (int)ceil((log((double)abs(nVal)) / log((double)2)) / 7); // base 128
			while (nVal != 0)
			{
				nAux = (int)(floor(nVal / pow((float)128, i - 1)));
				nVal = nVal - (int)(pow((float)128, i - 1) * nAux);				

				// next value (or with 0x80)
				if(nVal != 0)
					nAux |= 0x80;
					
				out[nIndex] = nAux;
				nIndex++;					
				
				i--;
			}
		}
	}
				
	
	setValue(UUCByteArray(out, nIndex));
	
	delete szOID;
	
}
		
bool CASN1ObjectIdentifier::equals(const CASN1ObjectIdentifier& objid)
{
	if(getLength() != objid.getLength())
		return false;
	
	const BYTE* val1 = getValue()->getContent();
	const BYTE* val2 = objid.getValue()->getContent();
	
	int r = memcmp(val1, val2, getLength());
	
	return r == 0;
}
/*
string CASN1ObjectIdentifier::ToOidString()
{
	long value = 0;
	bool first = true;
	char szValue[256];
	
	const UUCByteArray* pValue = getValue();
	int len = pValue->getLength();
	const BYTE* pVal = pValue->getContent();
	string objId;

	for (int i = 0; i != len; i++)
	{
		int b = pVal[i];

//		if (value < 0x80000000000000L)
//		{
			value = value * 128 + (b & 0x7f);
			if ((b & 0x80) == 0)             // end of number reached
			{
				if (first)
				{
					switch ((int)value / 40)
					{
						case 0:
							objId.append("0");
							break;

						case 1:
							objId.append("1");
							value -= 40;
							break;
						default:
							objId.append("2");
							value -= 80;
							break;
					}
					first = false;
				}

				objId.append(".");
				sprintf(szValue, "%d", value);
				objId.append(szValue);
				value = 0;
			}
		//}
	}

	return objId;
}
*/

void CASN1ObjectIdentifier::ToOidString(UUCByteArray& objId)
{
	long value = 0;
	bool first = true;
	char szValue[256];
	
	const UUCByteArray* pValue = getValue();
	int len = pValue->getLength();
	const BYTE* pVal = pValue->getContent();
	
	for (int i = 0; i != len; i++)
	{
		int b = pVal[i];

//		if (value < 0x80000000000000L)
//		{
			value = value * 128 + (b & 0x7f);
			if ((b & 0x80) == 0)             // end of number reached
			{
				if (first)
				{
					switch ((int)value / 40)
					{
						case 0:
							objId.append('0');
							break;

						case 1:
							objId.append('1');
							value -= 40;
							break;
						default:
							objId.append('2');
							value -= 80;
							break;
					}
					first = false;
				}

				objId.append('.');
				sprintf(szValue, "%d", value);
				objId.append((BYTE*)szValue, strlen(szValue));
				value = 0;
			}
	}

	objId.append((BYTE)'\0');

	//return objId.ToString();
}

