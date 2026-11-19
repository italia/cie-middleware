
#include "SyncroMutex.h"
#include "util.h"

#ifdef WIN32

static char *szCompiledFile=__FILE__;

CSyncroMutex::CSyncroMutex(void)
{
	hMutex=NULL;
}

void CSyncroMutex::Create(void)
{
	init_func_internal
	hMutex=CreateMutex(NULL,FALSE,NULL);
	ER_ASSERT(hMutex!=NULL,	"Errore nella creazione del Mutex");
	exit_func_internal
}

void CSyncroMutex::Create(const char *szName)
{
	init_func_internal
	hMutex=OpenMutex(SYNCHRONIZE,FALSE,szName);
	if (hMutex==NULL) {
		HRESULT r=GetLastError();
		if (r==ERROR_FILE_NOT_FOUND) {
			SECURITY_ATTRIBUTES attr;
			SECURITY_DESCRIPTOR secDesc;
			DWORD dwACL=sizeof(ACL)+sizeof(ACCESS_ALLOWED_ACE)+sizeof(SID);
			ByteDynArray pbtACL(dwACL);
			PACL pACL=(PACL)pbtACL.data();

			InitializeAcl(pACL,dwACL,ACL_REVISION);
			PSID pSid;
			SID_IDENTIFIER_AUTHORITY worldSidAuth=SECURITY_WORLD_SID_AUTHORITY;
			AllocateAndInitializeSid(&worldSidAuth,1,SECURITY_WORLD_RID,0,0,0,0,0,0,0,&pSid);

			AddAccessAllowedAceEx(pACL, ACL_REVISION, INHERITED_ACE, SYNCHRONIZE, pSid);

			InitializeSecurityDescriptor(&secDesc,SECURITY_DESCRIPTOR_REVISION);
			SetSecurityDescriptorDacl(&secDesc,TRUE,pACL,FALSE);

			attr.bInheritHandle=FALSE;
			attr.lpSecurityDescriptor=&secDesc;

			hMutex=CreateMutex(&attr,FALSE,szName);
			ER_ASSERT(hMutex!=NULL,	"Errore nella creazione del Mutex con nome");
			FreeSid(pSid);
		}
		else {
			ER_ASSERT(FALSE,"Errore nella creazione del Mutex")
		}
	}
	exit_func_internal
}

CSyncroMutex::~CSyncroMutex(void)
{
	init_func_internal
	if (hMutex)
		CloseHandle(hMutex);
	exit_func_internal
}

void CSyncroMutex::Lock()
{
	init_func_internal
	DWORD res=WaitForSingleObject(hMutex,INFINITE);
	ER_ASSERT(res==S_OK || res==WAIT_ABANDONED,"Errore nel rilascio del mutex");
	exit_func_internal
}

void CSyncroMutex::Unlock()
{
	init_func_internal
	if (!ReleaseMutex(hMutex)) {
		ER_ASSERT(FALSE,"Errore nel rilascio del mutex")
	}
	exit_func_internal
}

CSyncroLocker::CSyncroLocker(CSyncroMutex &mutex)
{
	init_func_internal
	pMutex=&mutex;
	pMutex->Lock();
	exit_func_internal
}

CSyncroLocker::~CSyncroLocker()
{
	init_func_internal
	pMutex->Unlock();
	exit_func_internal
}

#else

CSyncroMutex::CSyncroMutex(void)
{
    hMutex=NULL;
}

void CSyncroMutex::Create(void)
{
//    init_func_internal
//    hMutex=CreateMutex(NULL,FALSE,NULL);
//    ER_ASSERT(hMutex!=NULL,    "Errore nella creazione del Mutex");
//    exit_func_internal
}

void CSyncroMutex::Create(const char *szName)
{
//    init_func_internal
//    hMutex=OpenMutex(SYNCHRONIZE,FALSE,szName);
//    if (hMutex==NULL) {
//        HRESULT r=GetLastError();
//        if (r==ERROR_FILE_NOT_FOUND) {
//            SECURITY_ATTRIBUTES attr;
//            SECURITY_DESCRIPTOR secDesc;
//            DWORD dwACL=sizeof(ACL)+sizeof(ACCESS_ALLOWED_ACE)+sizeof(SID);
//            ByteDynArray pbtACL(dwACL);
//            PACL pACL=(PACL)pbtACL.data();
//
//            InitializeAcl(pACL,dwACL,ACL_REVISION);
//            PSID pSid;
//            SID_IDENTIFIER_AUTHORITY worldSidAuth=SECURITY_WORLD_SID_AUTHORITY;
//            AllocateAndInitializeSid(&worldSidAuth,1,SECURITY_WORLD_RID,0,0,0,0,0,0,0,&pSid);
//
//            AddAccessAllowedAceEx(pACL, ACL_REVISION, INHERITED_ACE, SYNCHRONIZE, pSid);
//
//            InitializeSecurityDescriptor(&secDesc,SECURITY_DESCRIPTOR_REVISION);
//            SetSecurityDescriptorDacl(&secDesc,TRUE,pACL,FALSE);
//
//            attr.bInheritHandle=FALSE;
//            attr.lpSecurityDescriptor=&secDesc;
//
//            hMutex=CreateMutex(&attr,FALSE,szName);
//            ER_ASSERT(hMutex!=NULL,    "Errore nella creazione del Mutex con nome");
//            FreeSid(pSid);
//        }
//        else {
//            ER_ASSERT(FALSE,"Errore nella creazione del Mutex")
//        }
//    }
//    exit_func_internal
}

CSyncroMutex::~CSyncroMutex(void)
{
//    init_func_internal
//    if (hMutex)
//    CloseHandle(hMutex);
//    exit_func_internal
}

void CSyncroMutex::Lock()
{
//    init_func_internal
//    DWORD res=WaitForSingleObject(hMutex,INFINITE);
//    ER_ASSERT(res==S_OK || res==WAIT_ABANDONED,"Errore nel rilascio del mutex");
//    exit_func_internal
}

void CSyncroMutex::Unlock()
{
//    init_func_internal
//    if (!ReleaseMutex(hMutex)) {
//        ER_ASSERT(FALSE,"Errore nel rilascio del mutex")
//    }
//    exit_func_internal
}

CSyncroLocker::CSyncroLocker(CSyncroMutex &mutex)
{
//    init_func_internal
//    pMutex=&mutex;
//    pMutex->Lock();
//    exit_func_internal
}

CSyncroLocker::~CSyncroLocker()
{
//    init_func_internal
//    pMutex->Unlock();
//    exit_func_internal
}


#endif
