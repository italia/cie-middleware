#include "stdafx.h"
#include <windows.h>
#include ".\utilexception.h"
#include <imagehlp.h>
#include <stdio.h>


char *CBaseException::ExceptionName() {
	return "Base Exception";
}

CBaseException::CBaseException() {
	type=UTILEX_BASE;
	innerException=NULL;
}

CBaseException::CBaseException(int _line,char *_fileName) {
	line=_line;
	fileName=_fileName;
	type=UTILEX_BASE;
	innerException=NULL;
}


CBaseException::CBaseException(CBaseException& inner) {
	type=UTILEX_BASE;
	innerException=inner.Clone();
	inner.innerException=NULL;
}

CBaseException::CBaseException(CBaseException& inner,int _line,char *_fileName) {
	line=_line;
	fileName=_fileName;
	type=UTILEX_BASE;
	innerException=inner.Clone();
	inner.innerException=NULL;
}

CBaseException *CBaseException::Clone() {
	CBaseException* cb=new CBaseException();
	*cb=*this;
	return cb;
}

const CBaseException& CBaseException::operator=(const CBaseException& orig) {
	if (orig.fileName.pbtData!=NULL)
		fileName=orig.fileName;
	else
		fileName.clear();
	type=orig.type;
	line=orig.line;
	innerException=orig.innerException;
	
	return *this;
}

CBaseException::~CBaseException(void)
{
	if (innerException)
		delete innerException;
}

void CBaseException::DumpError(String &dump) {
	dump="Eccezione generica";
}

void CBaseException::DumpTree(String &dump) 
{
	CBaseException *exc=this;
	dump="";
	while(exc!=NULL) {
		String desc;
		exc->DumpError(desc);
		String pos="";
		if (exc->fileName.pbtData!=NULL)
			pos.printf("%s(%i)",exc->fileName.stringlock(),exc->line);
		char *exName=exc->ExceptionName();
		int sz=dump.size()-1;
		dump.printf("%s : %s:%s\n", pos.lock(), exName, desc.lock());
		exc=exc->innerException;
	}
}

CStringException::CStringException() : CBaseException() {
	type=UTILEX_STRING;
}

CStringException::CStringException(int line,char *fileName) : CBaseException(line,fileName) {
	type=UTILEX_STRING;
}

CStringException::CStringException(CBaseException& inner) : CBaseException(inner) {
	type=UTILEX_STRING;
}

CStringException::CStringException(CBaseException& inner,int line,char *fileName) : CBaseException(inner,line,fileName) {
	type=UTILEX_STRING;
}

CStringException::CStringException(const char *fmt,...) : CBaseException() {
	type=UTILEX_STRING;
	va_list args;
	va_start (args, fmt);
	description.printfList(fmt,args);
	va_end (args);
}

CStringException::CStringException(int line,char *fileName,const char *fmt,...) : CBaseException(line,fileName) {
	type=UTILEX_STRING;
	va_list args;
	va_start (args, fmt);
	description.printfList(fmt,args);
	va_end (args);
}

CStringException::CStringException(CBaseException& inner,const char *fmt,...) : CBaseException(inner)  {
	type=UTILEX_STRING;
	va_list args;
	va_start (args, fmt);
	description.printfList(fmt,args);
	va_end (args);
}

CStringException::CStringException(CBaseException& inner,int line,char *fileName,const char *fmt,...) : CBaseException(inner,line,fileName)  {
	type=UTILEX_STRING;
	va_list args;
	va_start (args, fmt);
	description.printfList(fmt,args);
	va_end (args);
}

CStringException::~CStringException(void)
{
}

void CStringException::DumpError(String &dump) {
	dump=description;
}

CBaseException *CStringException::Clone() {
	CStringException* cb=new CStringException();
	*cb=*this;
	return cb;
}


void CWinException::Init(DWORD error) {
	errorCode=error;
	static char szWinErrBuffer[300];
	FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,NULL,errorCode,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),szWinErrBuffer,300,NULL);
	description=szWinErrBuffer;
}

CWinException::CWinException () : CStringException() {
	type=UTILEX_WIN;
	Init(GetLastError());
}

CWinException::CWinException (int line,char *fileName) : CStringException(line,fileName) {
	type=UTILEX_WIN;
	Init(GetLastError());
}

CWinException::CWinException (DWORD _errorCode) : CStringException()  {
	type=UTILEX_WIN;
	Init(_errorCode);
}

CWinException::CWinException (int line,char *fileName,DWORD _errorCode) : CStringException(line,fileName)  {
	type=UTILEX_WIN;
	Init(_errorCode);
}

CWinException::CWinException (CBaseException& inner) : CStringException(inner) {
	type=UTILEX_WIN;
	Init(GetLastError());
}

CWinException::CWinException (CBaseException& inner,int line,char *fileName) : CStringException(inner,line,fileName) {
	type=UTILEX_WIN;
	Init(GetLastError());
}

CWinException::CWinException (CBaseException& inner,DWORD _errorCode) : CStringException(inner) {
	type=UTILEX_WIN;
	Init(_errorCode);
}

CWinException::CWinException (CBaseException& inner,int line,char *fileName,DWORD _errorCode) : CStringException(inner,line,fileName) {
	type=UTILEX_WIN;
	Init(_errorCode);
}

CBaseException *CWinException::Clone() {
	CWinException* cb=new CWinException();
	*cb=*this;
	return cb;
}

void CSCardException::Init(WORD error) {
	errorCode=error;
	description=CardErr(errorCode);
}

CSCardException::CSCardException () : CStringException() {
	type=UTILEX_SCARD;
}

CSCardException::CSCardException (int line,char *fileName) : CStringException(line,fileName) {
	type=UTILEX_SCARD;
}

CSCardException::CSCardException (WORD _errorCode) : CStringException()  {
	type=UTILEX_SCARD;
	Init(_errorCode);
}

CSCardException::CSCardException (int line,char *fileName,WORD _errorCode) : CStringException(line,fileName)  {
	type=UTILEX_SCARD;
	Init(_errorCode);
}

CSCardException::CSCardException (CBaseException& inner,WORD _errorCode) : CStringException(inner) {
	type=UTILEX_SCARD;
	Init(_errorCode);
}

CSCardException::CSCardException (CBaseException& inner,int line,char *fileName,WORD _errorCode) : CStringException(inner,line,fileName) {
	type=UTILEX_SCARD;
	Init(_errorCode);
}

CBaseException *CSCardException::Clone() {
	CSCardException* cb=new CSCardException();
	*cb=*this;
	return cb;
}

char *CStringException::ExceptionName() {
	return "Exception";
}

char *CWinException::ExceptionName() {
	return "Windows Error";
}

char *CSCardException::ExceptionName() {
	return "SCard Error";
}

char *CSystemException::ExceptionName() {
	return "System Error";
}

void CSystemException::Init(unsigned int error,LPCONTEXT context) {
	errorCode=error;
	description=SystemErr(error);
	//if (context)
		//UtilStackWalk(context,stackWalk,0);
}

CSystemException::CSystemException () : CStringException() {
	type=UTILEX_SYSTEM;
}

CSystemException::CSystemException (unsigned int _errorCode,LPCONTEXT context) : CStringException()  {
	type=UTILEX_SYSTEM;
	Init(_errorCode,context);
}

CBaseException *CSystemException::Clone() {
	CSystemException* cb=new CSystemException();
	*cb=*this;
	return cb;
}

char *sep="-----------------\n";
void CSystemException::DumpError(String &dump) {
	dump.printf("%s\n%s%s%s",description.lock(),sep,stackWalk.lock(),sep);
}

void exceptionTranslator( unsigned int u, _EXCEPTION_POINTERS* pExp ) {
	throw CSystemException(u,pExp->ContextRecord);
}
