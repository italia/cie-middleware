#pragma once

#include <winscard.h>

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
