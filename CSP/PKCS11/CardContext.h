#pragma once

#if defined(WIN32) || defined(_WIN32)
#include <winscard.h>
#else
#include <PCSC/wintypes.h>
#include <PCSC/winscard.h>
#endif

class CCardContext
{
public:
	SCARDCONTEXT hContext;

	CCardContext(void);
	~CCardContext(void);

	operator SCARDCONTEXT();
	void validate();
	void renew();

private:
	void getContext();

};
