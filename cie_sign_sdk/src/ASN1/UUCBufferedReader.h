/* BufferedReader.h: interface for the CBufferedReader class.
 *
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
#include <stdio.h>
#include "UUCByteArray.h"

class UUCBufferedReader  
{
public:
	unsigned int getPosition();
	void setPosition(unsigned int index);
	//UUCBufferedReader(const char* szFilePath);	
	UUCBufferedReader(const UUCByteArray& buffer);
	UUCBufferedReader(const BYTE* pbtBuffer, int len);
	
	virtual ~UUCBufferedReader();

	unsigned int read(BYTE* pbtBuffer, unsigned int  nLen);
	unsigned int read(UUCByteArray& byteArray);

	void mark();
	void reset();	
	void releaseMark();

private:
	//unsigned int readNext();

	//FILE* m_pf;
	BYTE* m_pbtBuffer;
	//UUCByteArray* m_pByteArray;
	unsigned int m_nBufPos;
	unsigned int m_nBufLen;
	unsigned int m_nIndex;
	bool m_bEOF;

	unsigned int* m_pnStack;
	unsigned int  m_nStackSize;
	int  m_nTop;
};
