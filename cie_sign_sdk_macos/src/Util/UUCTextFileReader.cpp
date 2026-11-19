/* UUCTextFile.cpp: implementation of the UUCTextFile class.
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

#include "UUCTextFileReader.h"
#include <sys/stat.h>
#include <fcntl.h>



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UUCTextFileReader::UUCTextFileReader(const char* szFilePath)
{
	//LOG_DBG((0, "UUCTextFileReader", szFilePath));
	m_pf = fopen(szFilePath, "rt");
	if(!m_pf)
	{
		//LOG_DBG((0, "UUCTextFileReader", "fopen not found: %s", szFilePath ));
		throw (long)ERROR_FILE_NOT_FOUND;
	}

	struct stat	lstat_buf;
	struct stat	fstat_buf;

	int r = lstat(szFilePath, &lstat_buf);

	/* handle the case of the lstat failing first */
	if (r == -1)
	{
		fclose(m_pf);
		throw (long)ERROR_FILE_NOT_FOUND;
	}

	if (S_ISLNK(lstat_buf.st_mode))  {
		fclose(m_pf);
		throw (long)ERROR_FILE_NOT_FOUND;
	}

	/* Get the properties of the opened file descriptor */
	r = stat(szFilePath, &fstat_buf);
	if (r == -1)
	{
		fclose(m_pf);
		throw (long)ERROR_FILE_NOT_FOUND;
	}

	if (lstat_buf.st_dev != fstat_buf.st_dev
		 || lstat_buf.st_ino != fstat_buf.st_ino
		 || (S_IFMT & lstat_buf.st_mode) != (S_IFMT & fstat_buf.st_mode))
	{
		fclose(m_pf);
		throw (long)ERROR_FILE_NOT_FOUND;
	}
	//LOG_DBG((0, "UUCTextFileReader", "read OK %s", szFilePath));
}

UUCTextFileReader::~UUCTextFileReader()
{
	fclose(m_pf);
}

long UUCTextFileReader::readLine(UUCByteArray& line)
{
	char szLine[2];
	unsigned int i = 0;
	while((fread(szLine, 1, 1, m_pf) > 0) && (szLine[0] != '\n') )
	{
		i++;
		line.append(szLine[0]);
	}

	if(i > 0)
	{
		line.append((BYTE)0);
		return 0;
	}
	else if((i == 0) && szLine[0] == '\n')
	{
		return readLine(line);
	}
	else
	{
		return -1;
	}
}

long UUCTextFileReader::readLine(char* szLine, unsigned long nLen) // throw (long)
{
	unsigned int i = 0;
	while((fread(szLine + i, 1, 1, m_pf) > 0) && (szLine[i] != '\n') )
	{
		i++;
		if(i == nLen)
		{
			//SetLastError(ERROR_MORE_DATA);
			throw (long)ERROR_MORE_DATA;
		}
	}

	if(i > 0)
	{
		szLine[i] = 0;
		return 0;
	}
	else if((i == 0) && szLine[i] == '\n')
	{
		return readLine(szLine, nLen);
	}
	else
	{
		return -1;
	}
}
