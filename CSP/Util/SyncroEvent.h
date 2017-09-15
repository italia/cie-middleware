#pragma once

class CSyncroEvent
{
#ifdef WIN32
	HANDLE hEvent;
#endif
public:
	CSyncroEvent(void);
	CSyncroEvent(const char *name);
	~CSyncroEvent(void);

	void Signal();
	void Wait();
};

class CSyncroSignaler
{
	CSyncroEvent *pEvent;
public:
	CSyncroSignaler(CSyncroEvent &event);
	~CSyncroSignaler();

	void detach();
};
