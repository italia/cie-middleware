#include "../stdafx.h"
#include "thread.h"
#include "util.h"
#include "utilexception.h"

static char *szCompiledFile=__FILE__;

CThread::CThread(void)
{
	hThread=0;
	dwThreadID=0;
}

CThread::~CThread(void)
{
	dwThreadID=0;
#if defined(WIN32) || defined(_WIN32)
	if (hThread)
		CloseHandle(hThread);
#endif
}

void CThread::close()
{
	dwThreadID = 0;
#if defined(WIN32) || defined(_WIN32)
	if (hThread)
		CloseHandle(hThread);
#endif
	hThread = 0;
}

void CThread::createThread(void *threadFunc,void *threadData)
{
	init_func
#if defined(WIN32) || defined(_WIN32)
	if (dwThreadID != 0 || hThread!=nullptr)
		throw CStringException("Thread non ancora chiuso");
	hThread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)threadFunc,threadData,0,&dwThreadID);
	if (!hThread) {
		throw CWinException();
	}
#else
	if (dwThreadID != 0)
		throw logged_error("Thread non ancora chiuso");
	typedef void*(*pthread_fn)(void*);
	if (pthread_create(&hThread, nullptr, (pthread_fn)threadFunc, threadData) != 0)
		throw logged_error("pthread_create failed");
	dwThreadID = 1;
#endif
	exit_func
}

DWORD CThread::joinThread(DWORD timeout)
{
	init_func
#if defined(WIN32) || defined(_WIN32)
	DWORD ret=WaitForSingleObject(hThread,timeout);
	if (ret == 0)
		return 0;
	if (ret==WAIT_FAILED) {
		if (GetLastError()==ERROR_INVALID_HANDLE)
			return 0;
		return 1;
	}
	return 1;
#else
	(void)timeout;
	if (hThread) {
		pthread_join(hThread, nullptr);
		return 0;
	}
	return 0;
#endif
	exit_func
}

void CThread::exitThread(DWORD dwCode)
{
	init_func
#if defined(WIN32) || defined(_WIN32)
	if (hThread) {
		if (!CloseHandle(hThread))
			throw CWinException();
		hThread=NULL;
		ExitThread(dwCode);
		dwThreadID=0;
	}
#else
	(void)dwCode;
	hThread = 0;
	dwThreadID = 0;
	pthread_exit(nullptr);
#endif
	exit_func
}

void CThread::terminateThread()
{
	init_func
#if defined(WIN32) || defined(_WIN32)
	dwThreadID=0;
#pragma warning(suppress: 6258)
	BOOL ris = TerminateThread(hThread, 0);
	if (!ris)
		throw CWinException();
#else
	dwThreadID = 0;
	if (hThread) {
		pthread_cancel(hThread);
		hThread = 0;
	}
#endif
	exit_func
}