#pragma once
#include "util.h"
#include "utilexception.h"
#ifndef WIN32
#include <mutex>
#endif

class CSyncroMutex
{
#ifdef WIN32
	HANDLE hMutex;
#else
	std::mutex mtx;
	bool created;
#endif
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
