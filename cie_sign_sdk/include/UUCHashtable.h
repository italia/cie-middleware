/* CHashtable.hpp
 *
 *  Copyright (c) 2000-2006 by Ugo Chirico - http://www.ugosweb.com
 *  All Rights Reserved 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
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
#ifndef _UUHASHTABLE_HPP_
#define _UUHASHTABLE_HPP_

#define POS void*
#define INVALID (void*)-1l

#ifndef _DEFAULT_CAPACITY 
#define _DEFAULT_CAPACITY 100
#define _DEFAULT_LOADFACTOR 0.75
#endif

#define ERR_INDEX_OUT_OF_BOUND    0xC0001001L
#define ERR_TABLE_IS_EMPTY		  0xC0001002L
#include <stdlib.h>
#include "definitions.h"

template<class KEY, class VALUE>
class UUCHashtable
{	
private:
	struct HashtableEntry
	{
		HashtableEntry(){};
		unsigned int hash;
		KEY   key; 
		VALUE value;
		HashtableEntry* next;
	};

	HashtableEntry** m_table;

	unsigned int m_nSize;

    unsigned int m_nCount;
	unsigned int m_nIndex;		
    unsigned int m_threshold;

    float m_loadFactor;
	
	void rehash() 
	{				
		HashtableEntry** oldTable = m_table;
		// search for old first entry
		int nIndex;
		for(nIndex = m_nSize - 1; nIndex > 0 && oldTable[nIndex] == NULL; nIndex--);
		HashtableEntry* e = oldTable[nIndex];	
		
		// new capacity
		m_nSize = (unsigned int )(m_nSize * (m_loadFactor + 1));
		
		init();
		
		HashtableEntry* e1;
		// reput all entries
		while(e != NULL)
		{
			e1 = e;
			put(e->key, e->value);
			e = e->next;
			if(e == NULL)
			{			
				if(nIndex == 0) 
				{
					e = NULL;
				}
				else
				{
					for(nIndex--; nIndex > 0 && oldTable[nIndex] == NULL; nIndex--);
					e = oldTable[nIndex];
				}
			}
			
			free(e1);
		}

		free(oldTable);		
    };

	void init()
	{
		m_table = (HashtableEntry**)malloc(sizeof(HashtableEntry*) * m_nSize);
		m_threshold = (unsigned int)(m_nSize * m_loadFactor);		
		m_nCount = 0;
		m_nIndex = 0;
		for(unsigned int i = 0; i < m_nSize; i++)
		{
			m_table[i] = NULL;
		}		
	}
	
protected:
	virtual unsigned long getHashValue(const KEY& key) const
	{
		// default identity hash - works for most primitive values
		return (unsigned long)key;
	};

	virtual bool equal(const KEY& key1, const KEY& key2) const
	{		
		return key1 == key2;
	};

public:
	
	UUCHashtable(unsigned int initialCapacity, float loadFactor) 
	{
		m_loadFactor = loadFactor;
		m_nSize = initialCapacity;		
		init();
    };

    UUCHashtable(unsigned int initialCapacity) 
	{
		m_loadFactor = _DEFAULT_LOADFACTOR;
		m_nSize = initialCapacity;

		init();		
    };

    UUCHashtable() 
	{
		m_loadFactor = _DEFAULT_LOADFACTOR;
		m_nSize = _DEFAULT_CAPACITY;

		init();		
    };

    virtual ~UUCHashtable() 
	{
		removeAll();
		if(m_table)
			free(m_table);
	};

	// la scansione degli elementi non ha niente a che fare con la tabella
	POS getFirstPosition()
	{
		for(m_nIndex = m_nSize - 1; m_nIndex > 0 && m_table[m_nIndex] == NULL; m_nIndex--);

		return m_table[m_nIndex];		
	}

	POS getNextEntry(POS pos, KEY& key, VALUE& value)
	{
		if(pos)
		{
			key = ((HashtableEntry*)pos)->key;
			value = ((HashtableEntry*)pos)->value;
			pos = ((HashtableEntry*)pos)->next;
			if(pos == NULL)
			{			
				if(m_nIndex == 0) 
				{
					pos = NULL;
				}
				else
				{
					for(m_nIndex--; m_nIndex > 0 && m_table[m_nIndex] == NULL; m_nIndex--);
					pos = m_table[m_nIndex];
				}
			}

			return pos;
		}
		else
		{
			key = NULL;
			value = NULL;
			return NULL;
		}
	}

	unsigned int size() const
	{
		return m_nCount;
    };

    bool isEmpty() const
	{
		return m_nCount == 0;
    };

    bool containsKey(const KEY& key) const
	{
		unsigned int hash = getHashValue(key);
		long index = (hash & 0x7FFFFFFF) % m_nSize;

		for(HashtableEntry* e = m_table[index]; e != NULL; e = e->next)
		{
			if(e->hash == hash && equal(e->key,key))
			{
				return true;
			}
		}

		return false;
    };
	
    virtual bool get(KEY& key, VALUE& value) const
	{
		UINT hash = getHashValue(key);
		int index = (hash & 0x7FFFFFFF) % m_nSize;

		for(HashtableEntry* e = m_table[index]; e != NULL; e = e->next)
		{
			if(e->hash == hash && equal(e->key,key))
			{
				value = e->value;
				key = e->key;
				return true;
			}
		}

		return false;
    };
		
    virtual void put(const KEY& key, const VALUE& value) 
	{
		//if (key == NULL || value == NULL)
		//	throw 1;

		// Makes sure the key is not already in the hashtable.
		UINT hash = getHashValue(key);
		int index = (hash & 0x7FFFFFFF) % m_nSize;
		HashtableEntry* e;
		for(e = m_table[index]; e != NULL; e = e->next)
		{
			if(e->hash == hash && equal(e->key,key))
			{
				e->value = value;
				e->key = key;
				return;
			}
		}

		if (m_nCount >= m_threshold) 
		{
			// Rehash the table if the threshold is exceeded
			rehash();
			put(key, value);
			return;
		}

		// Creates the new entry.
		e = (HashtableEntry*)malloc(sizeof(HashtableEntry));
		e->hash  = hash;		
		e->key   = key;		
		e->value = value;
		e->next  = m_table[index];
		m_table[index] = e;
		
		m_nCount++;		
    };

	virtual void removeAll()
	{
		KEY* keys = new KEY[size()];
		KEY key;
		VALUE value;
		POS pos = getFirstPosition();
		unsigned int i = 0;
		while(pos)
		{	
			pos = getNextEntry(pos, key, value);
			keys[i] = key;	
			i++;
		}
		
		for(unsigned int j = 0; j < i; j++)
		{			
			remove(keys[j]);												
		}

		m_nCount = 0;

		delete keys;
	}


    virtual bool remove(const KEY& key) 
	{
		UINT hash = getHashValue(key);
		int index = (hash & 0x7FFFFFFF) % m_nSize;
		
		HashtableEntry* prev = NULL;

		for(HashtableEntry* e = m_table[index]; e != NULL; e = e->next)
		{
			if(e->hash == hash && equal(e->key,key))
			{
				if(prev == NULL)
				{
					m_table[index] = e->next;
					e->next = NULL;
				}
				else
				{
					prev->next = e->next;
					m_table[index] = prev;
				}		
							
				free(e);
				m_nCount--;
				return true;
			}

			prev = e;
		}

		return false;
    };
};

#endif //_UUHASHTABLE_HPP_
