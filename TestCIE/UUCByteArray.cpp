/*
 *  Copyright (c) 2000-2018 by Ugo Chirico - http://www.ugochirico.com
 *  All Rights Reserved 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
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
 
#include "pch.h"
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

long UUCByteArray::load(const char* szHexString)
{
	m_unLen = (strlen(szHexString)/2);
	
	//GlobalFree(m_pbtContent);
	free(m_pbtContent);
	m_nCapacity = m_unLen;	
	//m_pbtContent = (BYTE*)GlobalAlloc(GPTR, m_nCapacity);
	m_pbtContent = (BYTE*)malloc(m_nCapacity);
    if(m_pbtContent == NULL)
        return ERROR_UNABLE_TO_ALLOCATE;
        
	for(unsigned int i = 0; i < m_unLen; i++)
	{
		m_pbtContent[i] = atox((char*)szHexString + (i * 2));
	}
    
    return S_OK;
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
		throw (long)ERROR_INDEX_OUT_OF_BOUND;
	}
	return m_pbtContent[index];
}

void UUCByteArray::set(const unsigned int index, const BYTE btVal) //throw(long)
{
	if(index >= m_unLen)
	{
		//SetLastError(ERR_INDEX_OUT_OF_BOUND);
		throw (long)ERROR_INDEX_OUT_OF_BOUND;
	}
	m_pbtContent[index] = btVal;
}

void UUCByteArray::remove(const unsigned int index) //throw(long)
{
	if(index >= m_unLen)
	{
		//SetLastError((long)ERR_INDEX_OUT_OF_BOUND);
		throw (long)ERROR_INDEX_OUT_OF_BOUND;
	}

	for(unsigned int i = index; i < m_unLen - 1; i++)
		m_pbtContent[i] = m_pbtContent[i + 1];

	m_unLen--;
}

void UUCByteArray::removeAll()
{
	memset(m_pbtContent,0, m_nCapacity);
	m_unLen = 0;
}

long UUCByteArray::append(const BYTE btVal)
{
	if(m_unLen == m_nCapacity)
	{
		m_nCapacity += DEFAULT_CAPACITY;
        BYTE* buffer = (BYTE*)realloc(m_pbtContent, m_nCapacity);
        if(buffer == NULL)
            return ERROR_UNABLE_TO_ALLOCATE;
        
        m_pbtContent = buffer;
	}
		
	m_pbtContent[m_unLen] = btVal;

	m_unLen++;
    
    return S_OK;
}

long UUCByteArray::append(const BYTE* pbtVal, const unsigned long nLen)
{
	if(m_unLen + nLen > m_nCapacity)
	{
		m_nCapacity += nLen;
        BYTE* buffer = (BYTE*)realloc(m_pbtContent, m_nCapacity);
        if(buffer == NULL)
            return ERROR_UNABLE_TO_ALLOCATE;
        
        m_pbtContent = buffer;
	}

	for(unsigned int i = 0; i < nLen; i++)
	{
		m_pbtContent[m_unLen] = pbtVal[i];
		m_unLen++;		
	}
    
    return S_OK;
}

long UUCByteArray::append(const UUCByteArray& val)
{
	return append(val.getContent(), val.getLength());
}

long UUCByteArray::append(const char* szHexString)
{
	UUCByteArray ba(szHexString);
	return append(ba.getContent(), ba.getLength());
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

long UUCByteArray::reverse()
{
	BYTE* pbtContent = (BYTE*)malloc(m_unLen);
    if(m_pbtContent == NULL)
        return ERROR_UNABLE_TO_ALLOCATE;
    
	for(unsigned long i = 0; i < m_unLen; i++)
	{
		pbtContent[i] = m_pbtContent[m_unLen - i - 1];
	}

	memcpy(m_pbtContent, pbtContent, m_unLen);

	free(pbtContent);
    
    return S_OK;
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
		nSize = (int)m_unLen;
	}
	
	m_szHex = new char[(nSize + 1) * 2];	

	try
	{
		char szDigit[3];
        memset(m_szHex, 0, (nSize + 1) * 2);
		for(int i = 0; i < nSize; i++)
		{
			snprintf(szDigit, 3, "%02X", m_pbtContent[i]);
			strncat_s(m_szHex, 3, szDigit, 2);
		}	

		return m_szHex;
	}
	catch(...)
	{
		//SetLastError(-3);
		throw -3L;//new UUCException("UUCByteArray:toHexString:Access Violation");
	}

	
}

int atox(const char* szVal)
{
    int nVal = 0;
    
    if(szVal[1] >='0' && szVal[1] <= '9')
    {
        nVal = szVal[1] - '0';
    }
    else if(szVal[1] >='a' && szVal[1] <= 'f')
    {
        nVal = szVal[1] - 'a' + 10;
    }
    else if(szVal[1] >='A' && szVal[1] <= 'F')
    {
        nVal = szVal[1] - 'A' + 10;
    }
    else
    {
        throw(-1);
    }
    
    
    if(szVal[0] >='0' && szVal[0] <= '9')
    {
        nVal += (szVal[0] - '0') * 16;
    }
    else if(szVal[0] >='a' && szVal[0] <= 'f')
    {
        nVal += (szVal[0] - 'a' + 10) * 16;
    }
    else if(szVal[0] >='A' && szVal[0] <= 'F')
    {
        nVal += (szVal[0] - 'A' + 10) * 16;
    }
    else
    {
        throw(-1);
    }
    
    return nVal;
}
