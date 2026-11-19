/* UUCStringTable.h: interface for the UUCStringTable class.
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

#if !defined(AFX_UUCSTRINGTABLE_H__4392B6C2_89AA_436D_8291_A3D22CFF877B__INCLUDED_)
#define AFX_UUCSTRINGTABLE_H__4392B6C2_89AA_436D_8291_A3D22CFF877B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "UUCHashtable.hpp"

class UUCStringTable : public UUCHashtable<char*, char*>
{
public:
	void remove();
	// contructors
	UUCStringTable();
	UUCStringTable(int initialCapacity, float loadFactor);
	UUCStringTable(int initialCapacity);

	virtual void put(char* const& szKey, char* const& szValue);
	virtual bool remove(char* const& szKey);

	static unsigned long getHash(const char* szKey);

	// destructor
	virtual ~UUCStringTable();

protected:
	//virtual unsigned long getHashValue(unsigned long szKey);
	virtual unsigned long getHashValue(char* const& szKey) const;
	virtual bool equal(char* const& szKey1, char* const& szKey2) const;

	//virtual UINT getHashValue(const char*& szKey);
};

#endif // !defined(AFX_UUCSTRINGTABLE_H__4392B6C2_89AA_436D_8291_A3D22CFF877B__INCLUDED_)
