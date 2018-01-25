#include "../util/util.h"
#include "cardcontext.h"

static char *szCompiledFile=__FILE__;

void CCardContext::getContext() {
	init_func
	HANDLE hSCardSystemEvent=SCardAccessStartedEvent();
	if (hSCardSystemEvent) {
		WaitForSingleObject(hSCardSystemEvent,INFINITE);
		SCardReleaseStartedEvent();
	}

	LONG _call_ris;
	if ((_call_ris=(SCardEstablishContext(SCARD_SCOPE_SYSTEM,NULL,NULL,&hContext)))!=S_OK) {
		throw windows_error(_call_ris);
	}
}

CCardContext::CCardContext(void)
{
	hContext=NULL;
	getContext();
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
		getContext();
	}
}

void CCardContext::renew() {
	init_func
	
	LONG ris;
	if (hContext)
		if ((ris=SCardReleaseContext(hContext)) != SCARD_S_SUCCESS)
			throw windows_error(ris);
	hContext=NULL;

	getContext();

}
