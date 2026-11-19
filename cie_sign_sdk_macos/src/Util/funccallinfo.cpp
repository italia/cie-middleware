
#include "funccallinfo.h"
#include <stdio.h>

thread_local size_t tlsCallDepth = 0;
thread_local std::unique_ptr<CFuncCallInfoList> callQueue = nullptr;
extern bool FunctionLog;
extern unsigned int GlobalDepth;
extern bool GlobalParam;
char szEmpty[]={'\0'};

CFuncCallInfo::CFuncCallInfo(const char *name, CLog &logInfo) : log(logInfo) {
	fName = name;
	//OutputDebugString(fName);
	if (FunctionLog) {
		if (tlsCallDepth < GlobalDepth) {
			LogNum = logInfo.write("%*sIN -> %s", (DWORD)tlsCallDepth, szEmpty, fName);
		}
	}

	//fName = name;
	tlsCallDepth = tlsCallDepth + 1;


//	auto head = callQueue.release();
//	callQueue = std::unique_ptr<CFuncCallInfoList>(new CFuncCallInfoList(this));
//	callQueue->next = std::unique_ptr<CFuncCallInfoList>(head);
}

CFuncCallInfo::~CFuncCallInfo() {
	//OutputDebugString(stdPrintf("OUT %s", fName).c_str());
	//fName = NULL;
	tlsCallDepth=tlsCallDepth-1;
	if (fName)
		log.write("%*sOUT -> %s (%u)",(DWORD)tlsCallDepth,szEmpty,fName,LogNum-1);

//	if (callQueue!=nullptr && callQueue->info == this) {
//		//auto head = callQueue->next.release();
////		callQueue = std::unique_ptr<CFuncCallInfoList>(head);
//	}
//	else {
//		callQueue = nullptr;
//		OutputDebugString("Errore nella sequenza delle funzioni");
//	}
}

const char *CFuncCallInfo::FunctionName() {
	return fName;
}
