// UUCTextFile.h: interface for the UUCTextFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UUCTEXTFILE_H__CD3660A5_B4C5_4CD4_99AC_69AC96D1460F__INCLUDED_)
#define AFX_UUCTEXTFILE_H__CD3660A5_B4C5_4CD4_99AC_69AC96D1460F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include "ASN1/UUCByteArray.h"

class UUCTextFileReader 
{
public:
	UUCTextFileReader(const char* szFilePath);
	virtual ~UUCTextFileReader();

	long readLine(char* szLine, unsigned long nLen);// throw (long);
	long readLine(UUCByteArray& line);
private:

	FILE* m_pf;
};

#endif // !defined(AFX_UUCTEXTFILE_H__CD3660A5_B4C5_4CD4_99AC_69AC96D1460F__INCLUDED_)
