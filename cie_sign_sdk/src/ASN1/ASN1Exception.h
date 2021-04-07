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
 

#ifndef _ASN1EXCEPTION_H
#define _ASN1EXCEPTION_H

class CASN1Exception
{
public:
	CASN1Exception(const char* lpszMsg)
		: m_lpszMsg(lpszMsg)
	{
		
	};

	virtual ~CASN1Exception(){};
		
	virtual bool GetErrorMessage(char* lpszError, UINT nMaxError)
	{
		if(nMaxError < strlen(m_lpszMsg))
			return false;

		strcpy(lpszError, m_lpszMsg);

		return true;
	};


public:
	const char* m_lpszMsg;
};

class CASN1ParsingException : public CASN1Exception
{
public:
	CASN1ParsingException()
		: CASN1Exception("Bad ASN1Object parsed")
	{};
		
	virtual ~CASN1ParsingException(){};
	
};


class CASN1ObjectNotFoundException: public CASN1Exception
{
public:
	CASN1ObjectNotFoundException(const char* lpszClass)
		:CASN1Exception(lpszClass)
	{};
		
	virtual ~CASN1ObjectNotFoundException(){};
};

class CASN1BadObjectIdException : public CASN1Exception
{
public:
	CASN1BadObjectIdException(const char* strClass)
		: CASN1Exception(strClass)
	{};
		
	virtual ~CASN1BadObjectIdException(){};
};

/*
class CFileNotFoundException : public CASN1Exception
{
public:
	CFileNotFoundException(LPCTSTR lpszFile)
		: CASN1Exception(lpszFile)
	{};
};
*/
class CBadContentTypeException : public CASN1Exception
{
public:
	CBadContentTypeException()
		: CASN1Exception("Bad Content Type")
	{};
		
	virtual ~CBadContentTypeException(){};
};


#endif //_ASN1EXCEPTION_H
