#pragma once
#include <winscard.h>
#include "token.h"
#include "syncromutex.h"

class CCardLocker
{
	SCARDHANDLE hCard;
public:
	CCardLocker(SCARDHANDLE card);
	~CCardLocker(void);
	void Lock();
	void Unlock();
};
