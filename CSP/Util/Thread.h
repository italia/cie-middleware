#pragma once

#include "util.h"


class CThread
{
//public:
	CThread(void);
	~CThread(void);
#ifdef WIN32
	HANDLE hThread;
	DWORD dwThreadID;
#endif
	void createThread(void *threadFunc,void *threadData);
	DWORD joinThread(DWORD timeout);
	void terminateThread();
	void exitThread(DWORD dwCode);
	void close();
	inline static DWORD getID() {return GetCurrentThreadId();}
};
