#include "../stdafx.h"
#include "funccallinfo.h"
#include <stdio.h>

static char *szCompiledFile=__FILE__;

DWORD tlsCallDepth;
//extern CThreadLocalStorage tlsCallDepth;
extern bool FunctionLog;
extern unsigned int GlobalDepth;
extern bool GlobalParam;
char szEmpty[]={NULL};

void CFuncCallInfo::startCall() {
	tlsCallDepth=0;
}

CFuncCallInfo::CFuncCallInfo(char *name,CLog &logInfo) : log(logInfo) {
	if (FunctionLog) {
		if (tlsCallDepth<GlobalDepth) {
			fName=name;
			LogNum=logInfo.write("%*sIN -> %s",(DWORD)tlsCallDepth,szEmpty,fName);
		}
		else fName=NULL;
	}
	else fName=NULL;
	tlsCallDepth=tlsCallDepth+1;
}

CFuncCallInfo::~CFuncCallInfo() {
	tlsCallDepth=tlsCallDepth-1;
	if (fName)
		log.write("%*sOUT -> %s (%u)",(DWORD)tlsCallDepth,szEmpty,fName,LogNum-1);
}

void CFuncCallInfo::logRet(DWORD val,unsigned int line) {
	if (fName)
		log.write("%*s%s ret:%u (line %u)",(DWORD)tlsCallDepth,szEmpty,fName,val,line);
}
void CFuncCallInfo::logRet(char *val,unsigned int line) {
	if (fName)
		log.write("%*s%.10s ret:%u (line %u)",(DWORD)tlsCallDepth,szEmpty,fName,val,line);
}
void CFuncCallInfo::logRet(ByteArray &val,unsigned int line) {
	// TODO
}
void CFuncCallInfo::logRet(BOOL val,unsigned int line) {
	if (fName)
		log.write("%*s%s ret:%i (line %u)",(DWORD)tlsCallDepth,szEmpty,fName,val,line);
}
void CFuncCallInfo::logRet(void *val,unsigned int line) {
	if (fName)
		log.write("%*s%s ret:%p (line %u)",(DWORD)tlsCallDepth,szEmpty,fName,val,line);
}
void CFuncCallInfo::logRet(unsigned int line) {
	if (fName)
		log.write("%*s%s ret void (line %u)",(DWORD)tlsCallDepth,szEmpty,fName,line);
}
void CFuncCallInfo::logRet() {
	if (fName)
		log.write("%*s%s ret void",(DWORD)tlsCallDepth,szEmpty,fName);
}
void CFuncCallInfo::logParameter(DWORD val) {
	if (fName && GlobalParam && log.LogParam)
		log.writePure("param (%05u): %i",LogNum,val);
}
void CFuncCallInfo::logParameter(DWORD *val) {
	if (fName && GlobalParam && log.LogParam) {
		if (val)
			log.writePure("param (%05u): %p(%u)",LogNum,val,*val);
		else
			log.writePure("param (%05u): NULL",LogNum);
	}
}
void CFuncCallInfo::logParameter(void *val) {
	if (fName && GlobalParam && log.LogParam)
		log.writePure("param (%05u): %p",LogNum,val);
}

void CFuncCallInfo::logParameter(char *val) {
	if (fName && GlobalParam && log.LogParam) {
		if (val)
			log.writePure("param (%05u): \"%.10s\" ",LogNum,val);
		else
			log.writePure("param (%05u): NULL",LogNum);
	}
}

void CFuncCallInfo::logParameter(char *val, unsigned long len) {
	logParameter(val, (size_t)len);
}

void CFuncCallInfo::logParameter(unsigned char *val, unsigned long len) {
	logParameter(val, (size_t)len);
}

void CFuncCallInfo::logParameter(void *val, unsigned long len) {
	logParameter(val, (size_t)len);
}

void CFuncCallInfo::logParameter(char *val, unsigned long *len) {
	logParameter(val, *len);
}

void CFuncCallInfo::logParameter(unsigned char *val, unsigned long *len) {
	logParameter(val, *len);
}

void CFuncCallInfo::logParameter(void *val, unsigned long *len) {
	logParameter(val, *len);
}


void CFuncCallInfo::logParameter(char *val, size_t len) {
	logParameter(val, &len);
}

void CFuncCallInfo::logParameter(unsigned char *val, size_t len) {
	logParameter(val, &len);
}

void CFuncCallInfo::logParameter(void *val, size_t len) {
	logParameter(val, &len);
}

void CFuncCallInfo::logParameter(unsigned char *val, size_t *len) {
	if (fName && GlobalParam && log.LogParam) {
		auto maxlen = min(10, *len);
		if (val) {
			char buf[31];
			for (DWORD i = 0; i<maxlen; i++)
				sprintf_s(buf + i * 3, 4, "%02x ", (int)((BYTE*)(val))[i]);
			log.writePure("param (%05u): %s (len %i)", LogNum, buf, len);
		}
		else {
			log.writePure("param (%05u): NULL (len %i)", LogNum, len);
		}
	}
}

void CFuncCallInfo::logParameter(char *val, size_t *len) {
	if (fName && GlobalParam && log.LogParam) {
		if (val) {
			auto maxlen = min(10, *len);
			log.writePure("param (%05u): %.*s (len %i)",LogNum,maxlen,val,*len);
		}
		else 
			log.writePure("param (%05u): NULL (len %i)",LogNum,*len);
	}
}

void CFuncCallInfo::logParameter(void *val, size_t *len) {
	if (fName && GlobalParam && log.LogParam) {
		if (val != nullptr) {
			if (len != nullptr) {
				auto maxlen = min(10, *len);
				char buf[31];
				for (size_t i = 0; i < maxlen; i++)
					sprintf_s(buf + i * 3, 4, "%02x ", (int)((BYTE*)(val))[i]);
				log.writePure("param (%05u): %s (len %i)", LogNum, buf, *len);
			}
			else
				log.writePure("param (%05u): NULL (len NULL)", LogNum);
		}
		else
			log.writePure("param (%05u): NULL (len %i)", LogNum, *len);
	}
}

void CFuncCallInfo::logParameterHide(char *val, size_t len) {
	if (fName && GlobalParam && log.LogParam) {
		log.writePure("param (%05u): *** (len %i)",LogNum,len);
	}
}

void CFuncCallInfo::logParameterHide(void *val, size_t len) {
	if (fName && GlobalParam && log.LogParam) {
		log.writePure("param (%05u): *** (len %i)",LogNum,len);
	}
}

void CFuncCallInfo::logParameterHide(char *val, size_t *len) {
	if (fName && GlobalParam && log.LogParam) {
		if (len)
			log.writePure("param (%05u): *** (len %i)",LogNum,*len);
		else
			log.writePure("param (%05u): *** (len NULL)",LogNum);
	}
}

void CFuncCallInfo::logParameterHide(void *val, size_t *len) {
	if (fName && GlobalParam && log.LogParam) {
		if (len)
			log.writePure("param (%05u): *** (len %i)",LogNum,*len);
		else
			log.writePure("param (%05u): *** (len NULL)",LogNum);
	}
}
