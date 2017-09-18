#include "../stdAfx.h"
#include "syncroEvent.h"

static char *szCompiledFile=__FILE__;

CSyncroEvent::CSyncroEvent(void)
{
	init_func_internal
#ifdef WIN32
	hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	if (hEvent==NULL) {
		throw CStringException("Impossibile create l'evento");
	}
#endif
	exit_func_internal
}

CSyncroEvent::CSyncroEvent(const char *szName)
{
	init_func_internal
#ifdef WIN32
	hEvent=OpenEvent(SYNCHRONIZE,FALSE,szName);
	if (hEvent==NULL) {
		HRESULT r=GetLastError();
		if (r==ERROR_FILE_NOT_FOUND) {
			SECURITY_ATTRIBUTES attr;
			SECURITY_DESCRIPTOR secDesc;
			DWORD dwACL=sizeof(ACL)+sizeof(ACCESS_ALLOWED_ACE)+sizeof(SID);
			ByteDynArray pbtACL(dwACL);
			PACL pACL=(PACL)pbtACL.lock();

			InitializeAcl(pACL,dwACL,ACL_REVISION);
			PSID pSid;
			SID_IDENTIFIER_AUTHORITY worldSidAuth=SECURITY_WORLD_SID_AUTHORITY;
			AllocateAndInitializeSid(&worldSidAuth,1,SECURITY_WORLD_RID,0,0,0,0,0,0,0,&pSid);

			AddAccessAllowedAceEx(pACL, ACL_REVISION, INHERITED_ACE,SYNCHRONIZE, pSid);

			InitializeSecurityDescriptor(&secDesc,SECURITY_DESCRIPTOR_REVISION);
			SetSecurityDescriptorDacl(&secDesc,TRUE,pACL,FALSE);

			attr.bInheritHandle=FALSE;
			attr.lpSecurityDescriptor=&secDesc;

			hEvent=CreateEvent(&attr,FALSE,FALSE,szName);
			if (hEvent==NULL) {
				throw CStringException("Impossibile create l'evento");
			}
			FreeSid(pSid);
		}
		else {
			throw CStringException("Impossibile create l'evento");
		}
	}
#endif
	exit_func_internal
}

CSyncroEvent::~CSyncroEvent(void)
{
	init_func_internal
#ifdef WIN32
	CloseHandle(hEvent);
#endif
	exit_func_internal
}

void CSyncroEvent::Wait()
{
	init_func_internal
#ifdef WIN32
	HRESULT hr;
	if ((hr=WaitForSingleObject(hEvent,INFINITE))!=S_OK) {
		throw CWinException(hr);
	}
#endif
	exit_func_internal
}

void CSyncroEvent::Signal()
{
	init_func_internal
#ifdef WIN32
	if (!SetEvent(hEvent)) {
		throw CWinException();
	}
#endif
	exit_func_internal
}

CSyncroSignaler::CSyncroSignaler(CSyncroEvent &event)
{
	init_func_internal
	pEvent=&event;
	exit_func_internal
}

void CSyncroSignaler::detach()
{
	init_func_internal
	pEvent=NULL;
	exit_func_internal
}

CSyncroSignaler::~CSyncroSignaler()
{
	init_func_internal
	if (pEvent)
		pEvent->Signal();
	exit_func_internal
}
