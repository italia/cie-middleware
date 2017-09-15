#include "../StdAfx.h"
#include "cardlocker.h"

static char *szCompiledFile=__FILE__;

CCardLocker::CCardLocker(SCARDHANDLE card)
{
	init_func_internal
	hCard=card;
	Lock();
	exit_func_internal
}

CCardLocker::~CCardLocker(void)
{
	init_func_internal
	Unlock();
	exit_func_internal
}

void CCardLocker::Lock()
{
	init_func

	SCardBeginTransaction(hCard);

	exit_func
}

void CCardLocker::Unlock()
{
	init_func

	SCardEndTransaction(hCard,SCARD_LEAVE_CARD);

	exit_func
}

