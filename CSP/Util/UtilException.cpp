#include "../stdafx.h"
#include <windows.h>
#include "utilexception.h"
#include <imagehlp.h>
#include <stdio.h>


const char *CBaseException::ExceptionName() {
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


CBaseException::CBaseException(const CBaseException& inner) {
	type=UTILEX_BASE;
	innerException=inner.Clone();
}

CBaseException::CBaseException(const CBaseException& inner,int _line,char *_fileName) {
	line=_line;
	fileName=_fileName;
	type=UTILEX_BASE;
	innerException=inner.Clone();
}

std::unique_ptr<CBaseException> CBaseException::Clone() const {
	auto cb=std::unique_ptr<CBaseException>(new CBaseException());
	*cb=*this;
	return cb;
}

const CBaseException& CBaseException::operator=(const CBaseException& orig) {
	if (!orig.fileName.empty())
		fileName=orig.fileName;
	else
		fileName.clear();
	type=orig.type;
	line=orig.line;
	if (orig.innerException)
		innerException=orig.innerException->Clone();
	else
		innerException.reset();
	
	return *this;
}

CBaseException::~CBaseException(void)
{
}

void CBaseException::DumpError(std::string &dump) {
	dump="Eccezione generica";
}

void CBaseException::DumpTree(std::string &dump)
{
	CBaseException *exc=this;
	dump="";
	while(exc!=NULL) {
		std::string  desc;
		exc->DumpError(desc);
		std::string pos = "";
		if (!exc->fileName.empty())
			pos = exc->fileName.append("(").append(std::to_string(exc->line)).append(")");
		const char *exName=exc->ExceptionName();
		size_t sz=dump.size()-1;
		dump = pos.append(" : ").append(exName).append(desc).append("\n");
		exc=exc->innerException.get();
	}
}

CStringException::CStringException() : CBaseException() {
	type=UTILEX_STRING;
}

CStringException::CStringException(int line,char *fileName) : CBaseException(line,fileName) {
	type=UTILEX_STRING;
}

CStringException::CStringException(const CBaseException& inner) : CBaseException(inner) {
	type=UTILEX_STRING;
}

CStringException::CStringException(const CBaseException& inner,int line,char *fileName) : CBaseException(inner,line,fileName) {
	type=UTILEX_STRING;
}

CStringException::CStringException(const char *fmt,...) : CBaseException() {
	type=UTILEX_STRING;
	va_list args;
	va_start (args, fmt);
	int size = _vscprintf(fmt, args) + 1;
	description.resize(size);
	vsprintf_s(&description[0], size, fmt, args);
	va_end (args);
}

CStringException::CStringException(int line,char *fileName,const char *fmt,...) : CBaseException(line,fileName) {
	type=UTILEX_STRING;
	va_list args;
	va_start(args, fmt);
	int size = _vscprintf(fmt, args) + 1;
	description.resize(size);
	vsprintf_s(&description[0], size, fmt, args);
	va_end(args);
}

CStringException::CStringException(const CBaseException& inner,const char *fmt,...) : CBaseException(inner)  {
	type=UTILEX_STRING;
	va_list args;
	va_start (args, fmt);
	int size = _vscprintf(fmt, args) + 1;
	description.resize(size);
	vsprintf_s(&description[0], size, fmt, args);
	va_end (args);
}

CStringException::CStringException(const CBaseException& inner,int line,char *fileName,const char *fmt,...) : CBaseException(inner,line,fileName)  {
	type=UTILEX_STRING;
	va_list args;
	va_start(args, fmt);
	int size = _vscprintf(fmt, args) + 1;
	description.resize(size);
	vsprintf_s(&description[0], size, fmt, args);
	va_end(args);
}

CStringException::~CStringException(void)
{
}

void CStringException::DumpError(std::string &dump) {
	dump=description;
}

std::unique_ptr<CBaseException> CStringException::Clone() const {
	auto cb=std::unique_ptr<CStringException>(new CStringException());
	*cb=*this;
	return std::unique_ptr<CBaseException>(cb.release());
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

CWinException::CWinException (const CBaseException& inner) : CStringException(inner) {
	type=UTILEX_WIN;
	Init(GetLastError());
}

CWinException::CWinException (const CBaseException& inner,int line,char *fileName) : CStringException(inner,line,fileName) {
	type=UTILEX_WIN;
	Init(GetLastError());
}

CWinException::CWinException (const CBaseException& inner,DWORD _errorCode) : CStringException(inner) {
	type=UTILEX_WIN;
	Init(_errorCode);
}

CWinException::CWinException (const CBaseException& inner,int line,char *fileName,DWORD _errorCode) : CStringException(inner,line,fileName) {
	type=UTILEX_WIN;
	Init(_errorCode);
}

std::unique_ptr<CBaseException> CWinException::Clone() const {
	auto cb=std::unique_ptr<CWinException>(new CWinException());
	*cb=*this;
	return std::unique_ptr<CBaseException>(cb.release());
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

CSCardException::CSCardException (const CBaseException& inner,WORD _errorCode) : CStringException(inner) {
	type=UTILEX_SCARD;
	Init(_errorCode);
}

CSCardException::CSCardException (const CBaseException& inner,int line,char *fileName,WORD _errorCode) : CStringException(inner,line,fileName) {
	type=UTILEX_SCARD;
	Init(_errorCode);
}

std::unique_ptr<CBaseException> CSCardException::Clone() const {
	auto cb=std::unique_ptr<CSCardException>(new CSCardException());
	*cb=*this;
	return std::unique_ptr<CBaseException>(cb.release());
}

const char *CStringException::ExceptionName() {
	return "Exception";
}

const char *CWinException::ExceptionName() {
	return "Windows Error";
}

const char *CSCardException::ExceptionName() {
	return "SCard Error";
}

const char *CSystemException::ExceptionName() {
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

std::unique_ptr<CBaseException> CSystemException::Clone() const {
	auto cb=std::unique_ptr<CSystemException>(new CSystemException());
	*cb=*this;
	return std::unique_ptr<CBaseException>(cb.release());
}

char *sep="-----------------\n";
void CSystemException::DumpError(std::string &dump) {
	dump = description.append("\n").append(sep).append(stackWalk).append(sep);
}

void exceptionTranslator( unsigned int u, _EXCEPTION_POINTERS* pExp ) {
	throw CSystemException(u,pExp->ContextRecord);
}
