#pragma once
#include "util.h"
#include "utilexception.h"

class CSyncroMutex
{
	HANDLE hMutex;
public:
	CSyncroMutex(void);
	void Create(void);
	void Create(const char *name);
	~CSyncroMutex(void);

	void Lock();
	void Unlock();
};

class CSyncroLocker
{
	CSyncroMutex *pMutex;
public:
	CSyncroLocker(CSyncroMutex &mutex);
	~CSyncroLocker();
};
