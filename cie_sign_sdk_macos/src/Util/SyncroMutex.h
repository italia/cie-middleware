#pragma once
#include "util.h"
#include "UtilException.h"

class CSyncroMutex
{
	HANDLE hMutex;
public:
	void Create(void);

	CSyncroMutex(void);
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
