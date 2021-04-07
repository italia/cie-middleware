// UUCTextFileWriter.cpp: implementation of the UUCTextFileWriter class.
//
//////////////////////////////////////////////////////////////////////

#include "UUCTextFileWriter.h"
//#include "UUCException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UUCTextFileWriter::UUCTextFileWriter(const char* szFilePath, bool bAppend /*= false*/)
{
	if(bAppend)
		m_pf = fopen(szFilePath, "a+t");
	else 
		m_pf = fopen(szFilePath, "wt");
	
	if(!m_pf)
		throw (long)ERROR_FILE_NOT_FOUND;
}

UUCTextFileWriter::~UUCTextFileWriter()
{
	fclose(m_pf);
}

long UUCTextFileWriter::writeLine(const char* szLine)
{
	if(fprintf(m_pf, "%s\n", szLine) < 0)
		return -1;	

	fflush(m_pf);

	return 0;
}

long UUCTextFileWriter::writeLine(const UUCByteArray& byteArray)
{
	char* pszLine = new char[byteArray.getLength() + 1];
	memcpy(pszLine, byteArray.getContent(), byteArray.getLength());

	if(fprintf(m_pf, "%s\n", pszLine) < 0)
	{
		delete pszLine;
		#ifdef WIN32
		return GetLastError();
#else
		return -1;
#endif
	}

	delete pszLine;
	fflush(m_pf);

	return 0;
}

