/*
 *  Copyright (c) 2000-2006 by Ugo Chirico - http://www.ugosweb.com
 *  All Rights Reserved 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include "UUCByteArray.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>


#define DEFAULT_CAPACITY  100
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UUCByteArray::UUCByteArray(const BYTE* pbtContent, const unsigned long unLen)
: m_szHex(NULL)
{	
	m_unLen = unLen;
	
	if(m_unLen > 0)
	{
		m_nCapacity = m_unLen;	
		m_pbtContent = (BYTE*)malloc(m_nCapacity);	
		if(m_pbtContent == NULL)
			throw -5L;
		
		memcpy(m_pbtContent, pbtContent, m_unLen);
	}
	else
	{
		m_unLen = 0;
		m_nCapacity = DEFAULT_CAPACITY;
		m_pbtContent = (BYTE*)malloc(m_nCapacity);
		if(m_pbtContent == NULL)
			throw -5L;
		
	}	
}

UUCByteArray::UUCByteArray(const UUCByteArray& blob)
: m_szHex(NULL)
{
	m_unLen = blob.getLength();
	
	if(m_unLen > 0)
	{
		m_nCapacity = m_unLen;	
		//m_pbtContent = (BYTE*)GlobalAlloc(GPTR, m_nCapacity);
		m_pbtContent = (BYTE*)malloc(m_nCapacity);
		if(m_pbtContent == NULL)
			throw -5L;
		
		memcpy(m_pbtContent, blob.getContent(), m_unLen);
	}
	else
	{
		m_unLen = 0;
		m_nCapacity = DEFAULT_CAPACITY;
		//m_pbtContent = (BYTE*)GlobalAlloc(GPTR, m_nCapacity);	
		m_pbtContent = (BYTE*)malloc(m_nCapacity);
		if(m_pbtContent == NULL)
			throw -5L;
		
	}	
	

	/*
	m_unLen = blob.getLength();
	if(m_unLen > 0)
	{
		m_nCapacity = m_unLen;	
		m_pbtContent = (BYTE*)malloc(m_nCapacity);	
		memcpy(m_pbtContent, blob.getContent(), m_unLen);
	}
	else
	{
		m_unLen = 0;
		m_nCapacity = DEFAULT_CAPACITY;
		m_pbtContent = (BYTE*)malloc(m_nCapacity);		
	}
	*/
}

UUCByteArray::UUCByteArray(const char* szHexString)
: m_szHex(NULL)
{
	m_unLen = (strlen(szHexString)/2);
	m_nCapacity = m_unLen;	
	//m_pbtContent = (BYTE*)GlobalAlloc(GPTR, m_nCapacity);	
	m_pbtContent = (BYTE*)malloc(m_nCapacity);
	if(m_pbtContent == NULL)
		throw -5L;
	
	for(unsigned int i = 0; i < m_unLen; i++)
	{
		m_pbtContent[i] = atox((char*)szHexString + (i * 2));
	}	
}

UUCByteArray::UUCByteArray(const unsigned long nLen)
: m_szHex(NULL)
{
	//m_unLen = nLen;
	m_unLen = 0;
	m_nCapacity = nLen;	
	//m_pbtContent = (BYTE*)GlobalAlloc(GPTR, m_nCapacity);	
	m_pbtContent = (BYTE*)malloc(m_nCapacity);
	if(m_pbtContent == NULL)
		throw -5L;
	
}

UUCByteArray::UUCByteArray()
: m_szHex(NULL)
{
	m_unLen = 0;
	m_nCapacity = DEFAULT_CAPACITY;
	//m_pbtContent = (BYTE*)GlobalAlloc(GPTR, m_nCapacity);
	m_pbtContent = (BYTE*)malloc(m_nCapacity);
	if(m_pbtContent == NULL)
		throw -5L;
	
}

UUCByteArray::~UUCByteArray()
{
	//GlobalFree(m_pbtContent);
	free(m_pbtContent);
	if(m_szHex)
		delete m_szHex;
}

void UUCByteArray::load(const char* szHexString)
{
	m_unLen = (strlen(szHexString)/2);
	
	//GlobalFree(m_pbtContent);
	free(m_pbtContent);
	m_nCapacity = m_unLen;	
	//m_pbtContent = (BYTE*)GlobalAlloc(GPTR, m_nCapacity);
	m_pbtContent = (BYTE*)malloc(m_nCapacity);

	for(unsigned int i = 0; i < m_unLen; i++)
	{
		m_pbtContent[i] = atox((char*)szHexString + (i * 2));
	}
}

const BYTE* UUCByteArray::getContent() const
{
	return m_pbtContent;
}

unsigned long UUCByteArray::getLength() const
{
	return m_unLen;
}

BYTE UUCByteArray::operator [] (const unsigned int index) const //throw(long)
{
	return get(index);
}

BYTE UUCByteArray::get(const unsigned int index) const //throw(long)
{
	if(index >= m_unLen)
	{
		//SetLastError(ERR_INDEX_OUT_OF_BOUND);
		throw (long)ERR_INDEX_OUT_OF_BOUND;
	}
	return m_pbtContent[index];
}

void UUCByteArray::set(const unsigned int index, const BYTE btVal) //throw(long)
{
	if(index >= m_unLen)
	{
		//SetLastError(ERR_INDEX_OUT_OF_BOUND);
		throw (long)ERR_INDEX_OUT_OF_BOUND;
	}
	m_pbtContent[index] = btVal;
}

void UUCByteArray::remove(const unsigned int index) //throw(long)
{
	if(index >= m_unLen)
	{
		//SetLastError((long)ERR_INDEX_OUT_OF_BOUND);
		throw (long)ERR_INDEX_OUT_OF_BOUND;
	}

	for(unsigned int i = index; i < m_unLen - 1; i++)
		m_pbtContent[i] = m_pbtContent[i + 1];

	m_unLen--;
}

void UUCByteArray::removeAll()
{
	memset(m_pbtContent,0, m_nCapacity);
	m_unLen = 0;

	//m_nCapacity = DEFAULT_CAPACITY;
	//GlobalFree(m_pbtContent);
	//m_pbtContent = (BYTE*)GlobalAlloc(GPTR, m_nCapacity);	
	//m_pbtContent = (BYTE*)malloc(m_nCapacity);
}

void UUCByteArray::append(const BYTE btVal)
{
	if(m_unLen == m_nCapacity)
	{
		m_nCapacity += DEFAULT_CAPACITY;
		//m_pbtContent = (BYTE*)GlobalReAlloc(m_pbtContent, m_nCapacity, GMEM_ZEROINIT);
		m_pbtContent = (BYTE*)realloc(m_pbtContent, m_nCapacity);
		if(m_pbtContent == NULL)
			throw -5L;
		
	}
		
	m_pbtContent[m_unLen] = btVal;

	m_unLen++;
}

void UUCByteArray::append(const BYTE* pbtVal, const unsigned int nLen)
{
	if(m_unLen + nLen > m_nCapacity)
	{
		m_nCapacity += nLen;
		//m_pbtContent = (BYTE*)GlobalReAlloc(m_pbtContent, m_nCapacity, GMEM_ZEROINIT);
		m_pbtContent = (BYTE*)realloc(m_pbtContent, m_nCapacity);
		if(m_pbtContent == NULL)
			throw -5L;
		
	}

	for(unsigned int i = 0; i < nLen; i++)
	{
		m_pbtContent[m_unLen] = pbtVal[i];
		m_unLen++;
		//append(pbtVal[i]);
	}	
}

void UUCByteArray::append(const UUCByteArray& val)
{
	append(val.getContent(), val.getLength());
}

void UUCByteArray::append(const char* szHexString)
{
	UUCByteArray ba(szHexString);
	append(ba.getContent(), ba.getLength());
}

/*
void UUCByteArray::toHexString(char* szHexString) const
{
	try
	{
		char szDigit[3];
		strcpy(szHexString, "");
		for(unsigned int i = 0; i < m_unLen; i++)
		{
			sprintf(szDigit, "%02X", m_pbtContent[i]);
			strcat(szHexString, szDigit);
		}	
	}
	catch(...)
	{
		throw new UUCException("UUCByteArray:toHexString:Access Violation");
	}
}
*/

void UUCByteArray::reverse()
{
	BYTE* pbtContent = (BYTE*)malloc(m_unLen);
	if(m_pbtContent == NULL)
		throw -5L;
	
	for(int i = 0; i < m_unLen; i++)
	{
		pbtContent[i] = m_pbtContent[m_unLen - i - 1];
	}

	memcpy(m_pbtContent, pbtContent, m_unLen);

	free(pbtContent);
}

const char* UUCByteArray::toHexString() 
{
	return toHexString(0);
}

const char* UUCByteArray::toHexString(int nSize) 
{
	if(m_szHex)
	{
		delete m_szHex;
		m_szHex = NULL;
	}

	if(nSize == 0 || nSize > m_unLen)
	{
		nSize = m_unLen;
	}

	//m_szHex = new char[(m_unLen + 1) * 2];
	m_szHex = new char[(nSize + 1) * 2];	

	try
	{
		char szDigit[3];
		strcpy(m_szHex, "");
		for(unsigned int i = 0; i < nSize; i++)
		{
			sprintf(szDigit, "%02X", m_pbtContent[i]);
			strcat(m_szHex, szDigit);
		}	

		return m_szHex;
	}
	catch(...)
	{
		//SetLastError(-3);
		throw -3L;//new UUCException("UUCByteArray:toHexString:Access Violation");
	}

	
}
