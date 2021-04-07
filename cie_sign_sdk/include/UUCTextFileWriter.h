// UUCTextFileWriter.h: interface for the UUCTextFileWriter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UUCTEXTFILEWRITER_H__F64974CA_4F95_4200_B7AE_4A53FB004B75__INCLUDED_)
#define AFX_UUCTEXTFILEWRITER_H__F64974CA_4F95_4200_B7AE_4A53FB004B75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <stdio.h>
#include "ASN1/UUCByteArray.h"

class UUCTextFileWriter  
{
public:
	UUCTextFileWriter(const char* szFilePath, bool bAppend = false);
	virtual ~UUCTextFileWriter();

	long writeLine(const char* szLine);
	long writeLine(const UUCByteArray& byteArray);

private:
	FILE* m_pf;
};

#endif // !defined(AFX_UUCTEXTFILEWRITER_H__F64974CA_4F95_4200_B7AE_4A53FB004B75__INCLUDED_)
