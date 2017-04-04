#pragma once

#include "util.h"

#define UTILEX_BASE			0
#define UTILEX_STRING		1
#define UTILEX_SYSTEM		2
#define UTILEX_WIN			3
#define UTILEX_SCARD		4

class CBaseException
{
protected:
	virtual char *ExceptionName();
	virtual CBaseException *Clone();

	const CBaseException& operator=(const CBaseException&);

public:
	CBaseException();
	CBaseException(int line,char *fileName);
	CBaseException(CBaseException& inner);
	CBaseException(CBaseException& inner,int line,char *fileName);
	virtual ~CBaseException(void);
	CBaseException* innerException;

	void DumpTree(String &dump);
	virtual void DumpError(String &dump);
	int type;
	int line;
	String fileName;
};

class CStringException : public CBaseException
{
public:
	CStringException();
	CStringException(int line,char *fileName);
	CStringException(CBaseException& inner);
	CStringException(CBaseException& inner,int line,char *fileName);
	CStringException(int line,char *fileName,const char *fmt,...);
	CStringException(const char *fmt,...);
	CStringException(CBaseException& inner,const char *fmt,...);
	CStringException(CBaseException& inner,int line,char *fileName,const char *fmt,...);

	virtual ~CStringException();
	virtual void DumpError(String &dump);
protected:
	virtual char *ExceptionName();
	virtual CBaseException *Clone();
	String description;
};

class CWinException : public CStringException
{
protected:
	virtual char *ExceptionName();
	void Init(DWORD error);
	virtual CBaseException *Clone();
public:
	DWORD errorCode;
	CWinException ();
	CWinException (int line,char *fileName);
	CWinException (DWORD errorCode);
	CWinException (int line,char *fileName,DWORD errorCode);
	CWinException (CBaseException& inner);
	CWinException (CBaseException& inner,int line,char *fileName);
	CWinException (CBaseException& inner,DWORD errorCode);
	CWinException (CBaseException& inner,int line,char *fileName,DWORD errorCode);
};

class CSCardException : public CStringException
{
protected:
	virtual char *ExceptionName();
	void Init(WORD error);
	virtual CBaseException *Clone();
public:
	WORD errorCode;
	CSCardException ();
	CSCardException (int line,char *fileName);
	CSCardException (WORD errorCode);
	CSCardException (int line,char *fileName,WORD errorCode);
	CSCardException (CBaseException& inner,WORD errorCode);
	CSCardException (CBaseException& inner,int line,char *fileName,WORD errorCode);

};

class CSystemException : public CStringException
{
protected:
	virtual char *ExceptionName();
	void Init(unsigned int error,LPCONTEXT context);
	unsigned int errorCode;
	virtual CBaseException *Clone();
public:
	CSystemException ();
	CSystemException (unsigned int errorCode,LPCONTEXT context);

	String stackWalk;
	virtual void DumpError(String & dump);
};
