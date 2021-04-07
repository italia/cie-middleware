// UUCProperties.cpp: implementation of the UUCProperties class.
//
//////////////////////////////////////////////////////////////////////

#include "UUCProperties.h"
#include "UUCTextFileReader.h"
#include "UUCTextFileWriter.h"
#include <stdlib.h>

#include <time.h>

#ifdef WIN32
#define TZSET _tzset
#else
#define TZSET tzset
#endif

#include "UUCLogger.h"
USE_LOG;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MAX_ALLOC_SIZE  512

UUCProperties::UUCProperties()
{
	m_pStringTable = new UUCStringTable();
	m_bAllocated = true;
}

UUCProperties::UUCProperties(const UUCProperties& defaults)
: m_pStringTable(defaults.m_pStringTable)
{
	m_bAllocated = false;
}

UUCProperties::~UUCProperties()
{	
	if(m_bAllocated)
		SAFEDELETE(m_pStringTable);

	m_pStringTable = NULL;
}

long UUCProperties::load(const char* szFilePath)
{
	try
	{
		UUCTextFileReader textFileReader(szFilePath);

		//LOG_DBG((0, "UUCProperties::load", "read OK %s", szFilePath));

		char* szName;
		char* szValue;
		
		long nEOF = -1;

		UUCByteArray line;
		long nRes = textFileReader.readLine(line);

		char* szLine	= (char*)line.getContent();
        
		//LOG_DBG((0, "UUCProperties::line", "%s", szLine ));

		DWORD dwLineLen = line.getLength();

		while(nRes != nEOF)
		{
			if(szLine[0] != '\0' && szLine[0] != '#' && szLine[0] != '[')  // salta i commenti
			{
				szName  = strtok(szLine, "=");
				szValue = strtok(NULL, "\n");
				putProperty(szName, szValue);			
			}

			line.removeAll();
			nRes	  = textFileReader.readLine(line);
			szLine	  = (char*)line.getContent();
			dwLineLen = line.getLength();

			//LOG_DBG((0, "UUCProperties::line", "%s", szLine ));
		}				
	}
	catch(long nErr)
	{
		LOG_ERR((0, "UUCProperties::exception", "%d", nErr));

		return nErr;
	}
	catch(...)
	{
#ifdef WIN32
		return GetLastError();
#else
		return -1;
#endif
	}

	return 0;
}

long UUCProperties::load(const UUCByteArray& props)
{
	char* szName;
	char* szValue;
	long nEOF = -1;
	char* szEqual;
	char* szProps = (char*)props.getContent();
	char* szLine	= strtok(szProps, "\r\n");
	
	while(szLine)
	{
		if(szLine[0] != '#' && szLine[0] != '[')  // salta i commenti
		{
			szEqual = strstr(szLine, "=");
			szEqual[0] = 0;
			szName  = szLine;			
			szValue = szEqual + 1;
			putProperty(szName, szValue);
			szLine = strtok(NULL, "\r\n");
		}
		else
		{
			szLine = strtok(NULL, "\r\n");//strlen(szLine) + 1;
			//szProps += strlen(szLine) + 1;
		}
	}

	return 0;
}
	
long UUCProperties::save(const char* szFilePath, const char* szHeader) const
{
	char* szLine;

	try
	{
		UUCTextFileWriter textFileWriter(szFilePath);

		if (szHeader != NULL) 
		{			
			szLine = new char[strlen(szHeader) + 3];
			sprintf(szLine, "#%s", szHeader);
			textFileWriter.writeLine(szLine);
			delete szLine;
		}

		time_t ltime;
		TZSET();
		time( &ltime );
		
		szLine = new char[255];
		sprintf(szLine, "#%s", ctime( &ltime ) );		
		textFileWriter.writeLine(szLine);
		delete szLine;

		// iterate in the hashtable
		char* szName;
		char* szValue;

		POS p = m_pStringTable->getFirstPosition();
		
		while(p != NULL)
		{
			p = m_pStringTable->getNextEntry(p, szName, szValue);		
				
			szLine = new char[strlen(szName) + strlen(szValue) + 2];
			sprintf(szLine, "%s=%s", szName, szValue);	    	   
			textFileWriter.writeLine(szLine);
			delete szLine; 
		}				
	}
	catch(long nErr)
	{
		return nErr;
	}
	catch(...)
	{
		#ifdef WIN32
		return GetLastError();
#else
		return -1;
#endif
	}

	return 0;
}

long UUCProperties::save(UUCByteArray& props, const char* szHeader) const
{
	char* szLine;

	try
	{
		if (szHeader != NULL) 
		{			
			szLine = new char[strlen(szHeader) + 4];
			sprintf(szLine, "#%s\r\n", szHeader);
			props.append((BYTE*)szLine, strlen(szLine));
			delete szLine;
		}

		time_t ltime;
		TZSET();
		time( &ltime );
		
		szLine = new char[255];
		sprintf(szLine, "#%s\r\n", ctime( &ltime ) );		
		props.append((BYTE*)szLine, strlen(szLine));
		delete szLine;

		// iterate in the hashtable
		char* szName;
		char* szValue;

		POS p = m_pStringTable->getFirstPosition();
		
		while(p != NULL)
		{
			p = m_pStringTable->getNextEntry(p, szName, szValue);		
				
			szLine = new char[strlen(szName) + strlen(szValue) + 2 + 3];
			sprintf(szLine, "%s=%s\r\n", szName, szValue);	    	   
			props.append((BYTE*)szLine, strlen(szLine));
			delete szLine; 
		}				
	}
	catch(long nErr)
	{
		return nErr;
	}
	catch(...)
	{
		#ifdef WIN32
		return GetLastError();
#else
		return -1;
#endif
	}

	return 0;
}

const char* UUCProperties::getProperty(const char* szName, const char* szDefaultValue /*= NULL*/) const
{
	char* szValue;
	char* szName1 = (char*)szName;
	//strcpy(szName1, szName);
	if(m_pStringTable->containsKey(szName1))
	{
		m_pStringTable->get(szName1, szValue);
		//delete szName1;
		return szValue;
	}
	else
	{
		//delete szName1;
		return szDefaultValue;
	}
}
	
void UUCProperties::putProperty(const char* szName, const char* szValue)
{	
	m_pStringTable->put((char*)szName, (char*)szValue);	
}

UUCStringTable* UUCProperties::getPropertyTable() const
{
	return m_pStringTable;
}

bool UUCProperties::contains(const char* szName) const
{
	return m_pStringTable->containsKey((char*)szName);
}



void UUCProperties::remove(const char *szName)
{
	m_pStringTable->remove((char*)szName);
}

void UUCProperties::removeAll()
{
	m_pStringTable->removeAll();
}

int UUCProperties::size() const
{
	return m_pStringTable->size();
}
