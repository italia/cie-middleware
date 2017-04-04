#pragma once

class safeDesktop {
	HDESK hSecureDesktop;
	HDESK hDeskCur;
public:
	safeDesktop(char *name);
	operator HDESK();
	~safeDesktop();
};