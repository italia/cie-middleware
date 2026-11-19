#pragma once

#include <memory>
#include <stdexcept>
#include "defines.h"

//#define UTILEX_BASE			0
//#define UTILEX_STRING		1
//#define UTILEX_SYSTEM		2
//#define UTILEX_WIN			3
//#define UTILEX_SCARD		4


class logged_error : public std::runtime_error {
public:
    logged_error(std::string message);
	logged_error(const char *message);
};

class scard_error : public logged_error {
public:
	StatusWord sw;
	scard_error(StatusWord sw);
};

class windows_error : logged_error {
public:
	windows_error(long  ris);
};

/*
class CBaseException
{
protected:
	virtual const char *ExceptionName();
	virtual std::unique_ptr<CBaseException> Clone() const;

	const CBaseException& operator=(const CBaseException&);

public:
	CBaseException();
	CBaseException(int line,char *fileName);
	CBaseException(const CBaseException& inner);
	CBaseException(const CBaseException& inner,int line,char *fileName);
	virtual ~CBaseException(void);
	std::unique_ptr<CBaseException> innerException;

	void DumpTree(std::string &dump);
	virtual void DumpError(std::string &dump);
	int type;
	int line;
	std::string fileName;
};

class CStringException : public CBaseException
{
public:
	CStringException();
	CStringException(int line,char *fileName);
	CStringException(const CBaseException& inner);
	CStringException(const CBaseException& inner,int line,char *fileName);
	CStringException(int line,char *fileName,const char *fmt,...);
	CStringException(const char *fmt,...);
	CStringException(const CBaseException& inner,const char *fmt,...);
	CStringException(const CBaseException& inner,int line,char *fileName,const char *fmt,...);

	virtual ~CStringException();
	void DumpError(std::string &dump) override;
protected:
	const char *ExceptionName() override;
	std::unique_ptr<CBaseException> Clone() const override;
	std::string description;
};

class CWinException : public CStringException
{
protected:
	virtual const char *ExceptionName();
	void Init(DWORD error);
	std::unique_ptr<CBaseException> Clone() const override;
public:
	DWORD errorCode;
	CWinException ();
	CWinException (int line,char *fileName);
	CWinException (DWORD errorCode);
	CWinException (int line,char *fileName,DWORD errorCode);
	CWinException (const CBaseException& inner);
	CWinException (const CBaseException& inner,int line,char *fileName);
	CWinException (const CBaseException& inner,DWORD errorCode);
	CWinException (const CBaseException& inner,int line,char *fileName,DWORD errorCode);
};

class CSCardException : public CStringException
{
protected:
	virtual const char *ExceptionName();
	void Init(WORD error);
	std::unique_ptr<CBaseException> Clone() const override;
public:
	WORD errorCode;
	CSCardException ();
	CSCardException (int line,char *fileName);
	CSCardException (WORD errorCode);
	CSCardException (int line,char *fileName,WORD errorCode);
	CSCardException (const CBaseException& inner,WORD errorCode);
	CSCardException (const CBaseException& inner,int line,char *fileName,WORD errorCode);

};

class CSystemException : public CStringException
{
protected:
	virtual const char *ExceptionName();
	void Init(unsigned int error,LPCONTEXT context);
	unsigned int errorCode;
	std::unique_ptr<CBaseException> Clone() const override;
public:
	CSystemException ();
	CSystemException (unsigned int errorCode,LPCONTEXT context);

	std::string stackWalk;
	virtual void DumpError(std::string & dump);
};
*/
