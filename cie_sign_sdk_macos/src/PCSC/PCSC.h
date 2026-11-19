#ifdef WIN32
#include <winscard.h>
#else
#include <PCSC/winscard.h>
#include "wintypes.h"
#endif
#include "../Util/Array.h"
#include <vector>
#include <thread>

#ifndef SCARD_PROTOCOL_Tx
#define SCARD_PROTOCOL_Tx SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1
#endif

class safeConnection {
public:
	SCARDCONTEXT hContext;
	SCARDHANDLE hCard;
	safeConnection(SCARDCONTEXT hContext, LPCSTR szReader, DWORD dwShareMode);
	safeConnection(SCARDHANDLE hCard);
	~safeConnection();
	operator SCARDHANDLE();
};

class safeTransaction{
	SCARDHANDLE hCard;
	bool locked;
	DWORD dwDisposition;
public:
	safeTransaction(safeConnection &conn, DWORD dwDisposition);
	void unlock();
	bool isLocked();
	~safeTransaction();
};

class readerMonitor {
	SCARDCONTEXT hContext;
	std::thread Thread;
	void *appData;
	void(*readerEvent)(std::string &reader, bool insert,void *appData);
	bool stopMonitor;
public:
	readerMonitor(void(*readerEvent)(std::string &reader, bool insert, void* appData), void* appData);
	~readerMonitor();
};
