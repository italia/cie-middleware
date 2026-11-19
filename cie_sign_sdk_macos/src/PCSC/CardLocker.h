#pragma once
#ifdef WIN32
#include <winscard.h>
#else
#include <PCSC/winscard.h>
#endif
#include "Token.h"
#include "../Util/SyncroMutex.h"

class CCardLocker
{
	SCARDHANDLE hCard;
public:
	CCardLocker(SCARDHANDLE card);
	~CCardLocker(void);
	void Lock();
	void Unlock();
};
