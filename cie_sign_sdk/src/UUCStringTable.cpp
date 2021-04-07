// UUCStringTable.cpp: implementation of the UUCStringTable class.
//
//////////////////////////////////////////////////////////////////////

#include "UUCStringTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UUCStringTable::UUCStringTable(int initialCapacity, float loadFactor)
: UUCHashtable<char*, char*>(initialCapacity, loadFactor)
{

}

UUCStringTable::UUCStringTable(int initialCapacity)
: UUCHashtable<char*, char*>(initialCapacity)
{

}

UUCStringTable::UUCStringTable()
{

}


UUCStringTable::~UUCStringTable()
{
	removeAll();
/*
	// release memory in hashtable
	POS p = getFirstPosition();
	char* szKey;
	char* szContent;

	while(p != NULL)
	{
		p = getNextEntry(p, szKey, szContent);		
		delete szContent;
		delete szKey;
	}	
*/
}

void UUCStringTable::put(char* const& szKey, char* const& szValue)
{
	char* szOldValue = NULL;
	char* szOldKey = szKey;

	char* szNewValue;
	char* szNewKey;

	if(containsKey(szKey))
	{
		get(szOldKey, szOldValue);
	}
	else
	{
		szOldKey = NULL;
	}
	
	szNewValue = new char[strlen(szValue) + 1];
	strcpy(szNewValue, szValue);

	szNewKey = new char[strlen(szKey) + 1];
	strcpy(szNewKey, szKey);

	UUCHashtable<char*, char*>::put(szNewKey, szNewValue);

	if(szOldKey)
		delete szOldKey;
	if(szOldValue)
		delete szOldValue;
}

unsigned long UUCStringTable::getHashValue(char* const& szKey) const
{
	return UUCStringTable::getHash((const char*)szKey); 
}

unsigned long UUCStringTable::getHash(const char* szKey)
{
	int h = 0;
	int off = 0;
	char* val = (char*)szKey;
	int len = strlen((char*)szKey);

	if (len < 16) 
	{
 	    for (int i = len ; i > 0; i--) 
		{
 			h = (h * 37) + val[off++];
 	    }
 	} 
	else 
	{
 	    // only sample some characters
 	    int skip = len / 8;
 	    for (int i = len ; i > 0; i -= skip, off += skip) 
		{
 			h = (h * 39) + val[off];
 	    }
 	}

	return h;	
}

bool UUCStringTable::equal(char* const& szKey1, char* const& szKey2) const
{		
	return strcmp(szKey1,szKey2) == 0;
}

bool UUCStringTable::remove(char* const& szKey)
{
	char* szNewValue;

	char* szNewKey = szKey;;

	if(containsKey(szKey))
	{
		get(szNewKey, szNewValue);
		
		UUCHashtable<char*, char*>::remove(szNewKey);

		delete szNewKey;
		delete szNewValue;
		return true;
	}	

	return false;
}
