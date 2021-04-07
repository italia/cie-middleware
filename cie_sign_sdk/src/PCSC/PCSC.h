#include <winscard.h>
#include "Array.h"
#include <vector>
#include <thread>

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