#include "stdafx.h"
#include "safeDesktop.h"
#include <Sddl.h>

BOOL CreateMyDACL(SECURITY_ATTRIBUTES * pSA)
{
	TCHAR * szSD = TEXT("D:")		// Discretionary ACL
		TEXT("(D;OICI;GA;;;BG)")	// Deny access to built-in guests
		TEXT("(D;OICI;GA;;;AN)")	// Deny access to anonymous logon
		TEXT("(D;OICI;GA;;;AU)")	// Allow read/write/execute to authenticated users
		TEXT("(D;OICI;GA;;;BA)");	// Allow full control to administrators

	if (NULL == pSA)
		return FALSE;

	return ConvertStringSecurityDescriptorToSecurityDescriptor(
		szSD,
		SDDL_REVISION_1,
		&(pSA->lpSecurityDescriptor),
		NULL);
}

bool switchDesktop = false;

safeDesktop::safeDesktop(char *name) {
	if (!switchDesktop)
		return;
	hDeskCur = GetThreadDesktop(GetCurrentThreadId());
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	CreateMyDACL(&sa);

	hSecureDesktop = CreateDesktop(name, nullptr, nullptr, 0, DESKTOP_CREATEWINDOW | DESKTOP_CREATEMENU | DESKTOP_SWITCHDESKTOP, &sa);
	if (hSecureDesktop != nullptr) {
		ODS("hSecureDesktop != nullptr");
		auto ris = SetThreadDesktop(hSecureDesktop);
		if (ris != 0) {
			ODS("SetThreadDesktop!=0");
			ris = SwitchDesktop(hSecureDesktop);
			if (ris == 0) {
				ODS("SwitchDesktop==0");
				SwitchDesktop(hDeskCur);
				SetThreadDesktop(hDeskCur);
			}
			else
				ODS("SwitchDesktop!=0");
		}
	}
}

safeDesktop::operator HDESK() {
	return hSecureDesktop;
}

safeDesktop::~safeDesktop() {
	if (!switchDesktop)
		return;
	auto ris = SwitchDesktop(hDeskCur);
	ris = SetThreadDesktop(hDeskCur);
	ris = CloseDesktop(hSecureDesktop);
}