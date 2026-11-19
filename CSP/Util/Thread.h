#pragma once

#include "util.h"


class CThread
{
//public:
	CThread(void);
	~CThread(void);
#if defined(WIN32) || defined(_WIN32)
	HANDLE hThread;
	DWORD dwThreadID;
#else
	pthread_t hThread;
	DWORD dwThreadID;
#endif
	void createThread(void *threadFunc,void *threadData);
	DWORD joinThread(DWORD timeout);
	void terminateThread();
	void exitThread(DWORD dwCode);
	void close();
	inline static DWORD getID() {return (DWORD)GetCurrentThreadId();}
};
