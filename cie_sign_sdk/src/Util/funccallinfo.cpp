#include "stdafx.h"
#include "funccallinfo.h"
#include <stdio.h>

static char *szCompiledFile=__FILE__;

thread_local size_t tlsCallDepth = 0;
thread_local std::unique_ptr<CFuncCallInfoList> callQueue = nullptr;
extern bool FunctionLog;
extern unsigned int GlobalDepth;
extern bool GlobalParam;
char szEmpty[]={NULL};

CFuncCallInfo::CFuncCallInfo(char *name, CLog &logInfo) : log(logInfo) {
	sName = name;
	//OutputDebugString(fName);
	//if (FunctionLog) {
	//	if (tlsCallDepth < GlobalDepth) {
			LogNum = logInfo.write("%u IN -> %s", (DWORD)tlsCallDepth, sName.c_str());
		//}
	//}

	//fName = name;
	tlsCallDepth = tlsCallDepth + 1;


	auto head = callQueue.release();
	callQueue = std::unique_ptr<CFuncCallInfoList>(new CFuncCallInfoList(this));
	callQueue->next = std::unique_ptr<CFuncCallInfoList>(head);
}

CFuncCallInfo::~CFuncCallInfo() {
	//OutputDebugString(stdPrintf("OUT %s", fName).c_str());
	//fName = NULL;
	tlsCallDepth=tlsCallDepth-1;
	//if (sName)
//		log.write("%u OUT -> %s (%u)",(DWORD)tlsCallDepth,sName.c_str(),LogNum-1);

	if (callQueue!=nullptr && callQueue->info == this) {
		auto head = callQueue->next.release();
		callQueue = std::unique_ptr<CFuncCallInfoList>(head);
	}
	else {
		callQueue = nullptr;
		OutputDebugString("Errore nella sequenza delle funzioni");
	}
}

const char *CFuncCallInfo::FunctionName() {
	return sName.c_str();
}
