#include "..\util.h"
#include ".\cardcontext.h"

static char *szCompiledFile=__FILE__;

RESULT CCardContext::getContext() {
	init_func
	HANDLE hSCardSystemEvent=SCardAccessStartedEvent();
	if (hSCardSystemEvent) {
		WaitForSingleObject(hSCardSystemEvent,INFINITE);
		SCardReleaseStartedEvent();
	}

	LONG _call_ris;
	if ((_call_ris=(SCardEstablishContext(SCARD_SCOPE_SYSTEM,NULL,NULL,&hContext)))!=S_OK) {
		throw CWinException();
	}

	_return(OK)
	exit_func
	_return(FAIL)
}

CCardContext::CCardContext(void)
{
	hContext=NULL;
	if (getContext()!=OK)
		throw;
}

CCardContext::~CCardContext(void)
{
	if (hContext)
		SCardReleaseContext(hContext);
}

CCardContext::operator SCARDCONTEXT() {
	return hContext;
}


void CCardContext::validate() {

	HANDLE hSCardSystemEvent=SCardAccessStartedEvent();
	if (hSCardSystemEvent) {
		WaitForSingleObject(hSCardSystemEvent,INFINITE);
		SCardReleaseStartedEvent();
	}

	if (hContext)
		if (SCardIsValidContext(hContext)!=SCARD_S_SUCCESS) 
			hContext=NULL;

	if (hContext==NULL) {
		if (getContext()!=OK)
			throw;
	}
}

RESULT CCardContext::renew() {
	init_func

	if (hContext)
		WIN_R_CALL(SCardReleaseContext(hContext), SCARD_S_SUCCESS);
	hContext=NULL;

	P11ER_CALL(getContext(),
		ERR_CANT_ESTABLISH_CONTEXT)

	_return(OK)
	exit_func
	_return(FAIL)
}
