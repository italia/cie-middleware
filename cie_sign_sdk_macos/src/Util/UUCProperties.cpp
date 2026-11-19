/* UUCProperties.cpp: implementation of the UUCProperties class.
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

#define SAFEDELETE(pointer) try { if(pointer) { delete pointer; pointer = NULL;}} catch(...) {}

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

		char* szName;
		char* szValue;

		long nEOF = -1;

		UUCByteArray line;
		long nRes = textFileReader.readLine(line);

		char* szLine	= (char*)line.getContent();
        char* szSavePtr;
		size_t dwLineLen = line.getLength();

		while(nRes != nEOF)
		{
			if(szLine[0] != '#' && szLine[0] != '[')  // salta i commenti
			{
				szName  = strtok_r(szLine, "=", &szSavePtr);
				szValue = strtok_r(NULL, "\n", &szSavePtr);
				putProperty(szName, szValue);
			}

			line.removeAll();
			nRes	  = textFileReader.readLine(line);
			szLine	  = (char*)line.getContent();
			dwLineLen = line.getLength();
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

long UUCProperties::load(const UUCByteArray& props)
{
	char* szName;
	char* szValue;
	char* szEqual;
    char* szSavePtr;
	char* szProps = (char*)props.getContent();
	char* szLine	= strtok_r(szProps, "\r\n", &szSavePtr);

	while(szLine && (szLine) < (szProps + props.getLength()))
	{
		if(szLine[0] != '#' && szLine[0] != '[')  // salta i commenti
		{
			szEqual = strstr(szLine, "=");
            if (szEqual && strlen(szEqual) > 1){
                szEqual[0] = 0;
                szName  = szLine;
                szValue = szEqual + 1;
                putProperty(szName, szValue);
            }
			szLine = strtok_r(NULL, "\r\n", &szSavePtr);
		}
		else
		{
			szLine = strtok_r(NULL, "\r\n", &szSavePtr);//strlen(szLine) + 1;
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
            size_t l = strlen(szHeader) + 3;
			szLine = new char[l];
			snprintf(szLine, l, "#%s", szHeader);
			textFileWriter.writeLine(szLine);
			delete[] szLine;
		}

		time_t ltime;
		TZSET();
		time( &ltime );

		szLine = new char[255];
		snprintf(szLine, 255, "#%s", ctime( &ltime ) );
		textFileWriter.writeLine(szLine);
		delete[] szLine;

		// iterate in the hashtable
		char* szName;
		char* szValue;

		POS p = m_pStringTable->getFirstPosition();

		while(p != NULL)
		{
			p = m_pStringTable->getNextEntry(p, szName, szValue);

            size_t l  = strlen(szName) + strlen(szValue) + 2;
			szLine = new char[l];
			snprintf(szLine, l, "%s=%s", szName, szValue);
			textFileWriter.writeLine(szLine);
			delete[] szLine;
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
            size_t l = strlen(szHeader) + 4;
			szLine = new char[l];
			snprintf(szLine, l, "#%s\r\n", szHeader);
			props.append((BYTE*)szLine, (int)strlen(szLine));
			delete[] szLine;
		}

		time_t ltime;
		TZSET();
		time( &ltime );

		szLine = new char[255];
		snprintf(szLine, 255, "#%s\r\n", ctime( &ltime ) );
		props.append((BYTE*)szLine, (int)strlen(szLine));
		delete[] szLine;

		// iterate in the hashtable
		char* szName;
		char* szValue;

		POS p = m_pStringTable->getFirstPosition();

		while(p != NULL)
		{
			p = m_pStringTable->getNextEntry(p, szName, szValue);

            size_t l = strlen(szName) + strlen(szValue) + 2 + 3;
			szLine = new char[l];
			snprintf(szLine, l, "%s=%s\r\n", szName, szValue);
			props.append((BYTE*)szLine, (int)strlen(szLine));
			delete[] szLine;
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

int UUCProperties::getIntProperty(const char* szName, int nDefaultValue /*= NULL*/) const
{

    const char* szVal = getProperty(szName, NULL);
    if(szVal)
        return atoi(szVal);
    else
        return nDefaultValue;
}

const char* UUCProperties::getProperty(const char* szName, const char* szDefaultValue /*= NULL*/) const
{
	char* szValue;
	char* szName1 = (char*)szName;
	if(m_pStringTable->containsKey(szName1))
	{
		m_pStringTable->get(szName1, szValue);
		return szValue;
	}
	else
	{
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
