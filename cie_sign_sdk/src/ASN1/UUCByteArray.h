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
 
#pragma once
#include "definitions.h"

#define ERR_INDEX_OUT_OF_BOUND    0xC0001001L

class UUCByteArray  
{
public:
	UUCByteArray(const BYTE* pbtContent, const unsigned long unLen);
	UUCByteArray(const UUCByteArray& blob);
	UUCByteArray(const char* szHexString);
	UUCByteArray(const unsigned long nLen);
	UUCByteArray();

	void load(const char* szHexString);

	virtual ~UUCByteArray();

	const BYTE* getContent() const;
	unsigned long getLength() const;
	
	void reverse();
	void append(const BYTE btVal);
	void append(const BYTE* pbtVal, const unsigned int nLen);
	void append(const UUCByteArray& val);
	void append(const char* szHexString);
	BYTE get(const unsigned int index) const;// throw(long);
	void set(const unsigned int index, const BYTE btVal);// throw (long);
	BYTE operator [] (const unsigned int index) const;// throw(long);
	void remove(const unsigned int index);// throw(long);
	void removeAll();
	//void toHexString(char* szHex) const;	
	const char* toHexString();
	const char* toHexString(int nSize);

private:
	BYTE* m_pbtContent;
	unsigned long m_unLen;
	unsigned long m_nCapacity;
	char* m_szHex;
};
