// UUCProperties.h: interface for the UUCProperties class.
//
//////////////////////////////////////////////////////////////////////

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
