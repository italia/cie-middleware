#include "../stdafx.h"
#include "funccallinfo.h"
#include <stdio.h>

static char *szCompiledFile=__FILE__;

DWORD tlsCallDepth;
//extern CThreadLocalStorage tlsCallDepth;
extern bool bFunctionLog;
extern DWORD dwGlobalDepth;
extern bool bGlobalParam;
char szEmpty[]={NULL};

void CFuncCallInfo::startCall() {
	tlsCallDepth=0;
}

CFuncCallInfo::CFuncCallInfo(char *name,CLog &logInfo) : log(logInfo) {
	if (bFunctionLog) {
		if (tlsCallDepth<dwGlobalDepth) {
			fName=name;
			dwLogNum=logInfo.write("%*sIN -> %s",(DWORD)tlsCallDepth,szEmpty,fName);
		}
		else fName=NULL;
	}
	else fName=NULL;
	tlsCallDepth=tlsCallDepth+1;
}

CFuncCallInfo::~CFuncCallInfo() {
	tlsCallDepth=tlsCallDepth-1;
	if (fName)
		log.write("%*sOUT -> %s (%u)",(DWORD)tlsCallDepth,szEmpty,fName,dwLogNum-1);
}

void CFuncCallInfo::logRet(DWORD val,DWORD line) {
	if (fName)
		log.write("%*s%s ret:%u (line %u)",(DWORD)tlsCallDepth,szEmpty,fName,val,(DWORD)line);
}
void CFuncCallInfo::logRet(char *val,DWORD line) {
	if (fName)
		log.write("%*s%.10s ret:%u (line %u)",(DWORD)tlsCallDepth,szEmpty,fName,val,(DWORD)line);
}
void CFuncCallInfo::logRet(ByteArray &val,DWORD line) {
	// TODO
}
void CFuncCallInfo::logRet(BOOL val,DWORD line) {
	if (fName)
		log.write("%*s%s ret:%i (line %u)",(DWORD)tlsCallDepth,szEmpty,fName,val,(DWORD)line);
}
void CFuncCallInfo::logRet(void *val,DWORD line) {
	if (fName)
		log.write("%*s%s ret:%p (line %u)",(DWORD)tlsCallDepth,szEmpty,fName,val,(DWORD)line);
}
void CFuncCallInfo::logRet(DWORD line) {
	if (fName)
		log.write("%*s%s ret void (line %u)",(DWORD)tlsCallDepth,szEmpty,fName,(DWORD)line);
}
void CFuncCallInfo::logRet() {
	if (fName)
		log.write("%*s%s ret void",(DWORD)tlsCallDepth,szEmpty,fName);
}
void CFuncCallInfo::logParameter(DWORD val) {
	if (fName && bGlobalParam && log.bLogParam)
		log.writePure("param (%05u): %i",dwLogNum,val);
}
void CFuncCallInfo::logParameter(DWORD *val) {
	if (fName && bGlobalParam && log.bLogParam) {
		if (val)
			log.writePure("param (%05u): %p(%u)",dwLogNum,val,*val);
		else
			log.writePure("param (%05u): NULL",dwLogNum);
	}
}
void CFuncCallInfo::logParameter(void *val) {
	if (fName && bGlobalParam && log.bLogParam)
		log.writePure("param (%05u): %p",dwLogNum,val);
}
void CFuncCallInfo::logParameter(char *val) {
	if (fName && bGlobalParam && log.bLogParam) {
		if (val)
			log.writePure("param (%05u): \"%.10s\" ",dwLogNum,val);
		else
			log.writePure("param (%05u): NULL",dwLogNum);
	}
}

void CFuncCallInfo::logParameter(char *val,DWORD len) {
	if (fName && bGlobalParam && log.bLogParam) {
		DWORD maxlen=min(10,len);
		if (val)
			log.writePure("param (%05u): %.*s (len %i)",dwLogNum,maxlen,val,len);
		else
			log.writePure("param (%05u): NULL (len %i)",dwLogNum,len);
	}
}

void CFuncCallInfo::logParameter(void *val,DWORD len) {
	if (fName && bGlobalParam && log.bLogParam) {
		DWORD maxlen=min(10,len);
		if (val) {
			char buf[31];
			for (DWORD i=0;i<maxlen;i++)
				sprintf_s(buf+i*3,4,"%02x ",(int)((BYTE*)(val))[i]);
			log.writePure("param (%05u): %s (len %i)",dwLogNum,buf,len);
		}
		else {
			log.writePure("param (%05u): NULL (len %i)",dwLogNum,len);
		}
	}
}

void CFuncCallInfo::logParameter(char *val,DWORD *len) {
	if (fName && bGlobalParam && log.bLogParam) {
		if (val) {
			DWORD maxlen=min(10,*len);
			log.writePure("param (%05u): %.*s (len %i)",dwLogNum,maxlen,val,*len);
		}
		else 
			log.writePure("param (%05u): NULL (len %i)",dwLogNum,*len);
	}
}

void CFuncCallInfo::logParameter(void *val,DWORD *len) {
	if (fName && bGlobalParam && log.bLogParam) {
		if (val) {
			if (len) {
				DWORD maxlen=min(10,*len);
				char buf[31];
				for (DWORD i=0;i<maxlen;i++)
					sprintf_s(buf+i*3,4,"%02x ",(int)((BYTE*)(val))[i]);
				log.writePure("param (%05u): %s (len %i)",dwLogNum,buf,*len);
			}
			else
				log.writePure("param (%05u): NULL (len NULL)",dwLogNum);
		}
		else
			log.writePure("param (%05u): NULL (len %i)",dwLogNum,*len);
	}
}

void CFuncCallInfo::logParameterHide(char *val,DWORD len) {
	if (fName && bGlobalParam && log.bLogParam) {
		log.writePure("param (%05u): *** (len %i)",dwLogNum,len);
	}
}

void CFuncCallInfo::logParameterHide(void *val,DWORD len) {
	if (fName && bGlobalParam && log.bLogParam) {
		log.writePure("param (%05u): *** (len %i)",dwLogNum,len);
	}
}

void CFuncCallInfo::logParameterHide(char *val,DWORD *len) {
	if (fName && bGlobalParam && log.bLogParam) {
		if (len)
			log.writePure("param (%05u): *** (len %i)",dwLogNum,*len);
		else
			log.writePure("param (%05u): *** (len NULL)",dwLogNum);
	}
}

void CFuncCallInfo::logParameterHide(void *val,DWORD *len) {
	if (fName && bGlobalParam && log.bLogParam) {
		if (len)
			log.writePure("param (%05u): *** (len %i)",dwLogNum,*len);
		else
			log.writePure("param (%05u): *** (len NULL)",dwLogNum);
	}
}
