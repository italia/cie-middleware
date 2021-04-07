// UUCStringTable.h: interface for the UUCStringTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UUCSTRINGTABLE_H__4392B6C2_89AA_436D_8291_A3D22CFF877B__INCLUDED_)
#define AFX_UUCSTRINGTABLE_H__4392B6C2_89AA_436D_8291_A3D22CFF877B__INCLUDED_

#pragma once

#include "UUCHashtable.h"

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
