/* UUCProperties.h: interface for the UUCProperties class.
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

#if !defined(AFX_UUCPROPERTIES_H__715BAE3B_069E_4D31_9FBF_54EA38AAEFEC__INCLUDED_)
#define AFX_UUCPROPERTIES_H__715BAE3B_069E_4D31_9FBF_54EA38AAEFEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "UUCByteArray.h"
#include "UUCStringTable.h"
#include <stdio.h>

class UUCProperties
{
public:
	UUCProperties();
	UUCProperties(const UUCProperties& defaults);

	virtual ~UUCProperties();

	long load(const char* szFilePath);
	long load(const UUCByteArray& props);
	long save(const char* szFilePath, const char* szHeader) const;
	long save(UUCByteArray& props, const char* szHeader) const;
	void putProperty(const char* szName, const char* szValue);
	//void putProperty(char* szName, char* szValue);
	const char* getProperty(const char* szName, const char* szDefaultValue = NULL) const;
    int getIntProperty(const char* szName, int nDefaultValue = NULL) const;

    void remove(const char* szName);
	void removeAll();

	UUCStringTable* getPropertyTable() const;

	bool contains(const char* szName) const;

	int size() const;

protected:
	UUCStringTable* m_pStringTable;

	bool m_bAllocated;
};

#endif // !defined(AFX_UUCPROPERTIES_H__715BAE3B_069E_4D31_9FBF_54EA38AAEFEC__INCLUDED_)
