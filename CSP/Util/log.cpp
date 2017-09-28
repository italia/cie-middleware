#include "../stdafx.h"
#include "util.h"
#include "moduleinfo.h"
#include <vector>
#include "log.h"
#include "DbgHelp.h"
#include "UtilException.h"
#include "thread.h"
#include "IniSettings.h"


static char *szCompiledFile=__FILE__;

std::vector<CLog> logInit;
std::vector<CLog*> logToInit;

String logDirGlobal;
bool bInitLog=false;
bool bFirstGlobal=false;
bool bFunctionLog=false;
bool bGlobalParam=false;
const char *logGlobalVersion;
DWORD dwGlobalModuleNum=1;
DWORD dwGlobalCount;
DWORD dwGlobalDepth=0;
CLog Log;

enum logMode {
	LM_Single,	// un solo file
	LM_Module,	// un file per modulo
	LM_Thread,	// un file per thread
	LM_Module_Thread	// un file per modulo e per thread
} LogMode = logMode::LM_Module;

bool bMainInit=false;
bool bMainEnable=false;

void initLog(const char *iniFile,const char *version) {
	init_func_internal

	if (bMainInit)
		return;
	bMainInit=true;
	bInitLog=true;

	logGlobalVersion=version;
	
	OutputDebugString("File INI:");
	OutputDebugString(iniFile);
	OutputDebugString("\n");

	LogMode = (logMode)(IniSettingsInt("Log", "LogMode", (int)LM_Single, "Modalità di Log. Valori possibili:\n"
		"0 ;LM_Single,	// un solo file\n"
		"1 ;LM_Module,	// un file per modulo\n"
		"2 ;LM_Thread,	// un file per thread\n"
		"3 ;LM_Module_Thread	// un file per modulo e per thread\n")).GetValue((char*)iniFile);

	if (LogMode==-1) {
		LogMode=LM_Single;
	}

	bMainEnable=(IniSettingsBool("Log","LogEnable",false,"Abilitazione log globale")).GetValue((char*)iniFile);

	bFunctionLog = (IniSettingsBool("Log", "FunctionLog", false, "Abilitazione log delle chiamate a funzione")).GetValue((char*)iniFile);

	dwGlobalDepth = (IniSettingsInt("Log", "FunctionDepth", 10, "Definisce la profondità massima di log delle funzioni\n")).GetValue((char*)iniFile);

	bGlobalParam = (IniSettingsBool("Log", "ParamLog", false, "Abilitazione log dei parametri di input delle funzioni")).GetValue((char*)iniFile);

	(IniSettingsString("Log", "LogDir", "c:\\", "Definisce il path in cui salvare il file di log (con \\ finale)")).GetValue((char*)iniFile, logDirGlobal);

	char SectionName[30];
	int numMod=1;
	while (true) {
		sprintf_s(SectionName,30,"%s%i","LogModule",numMod);
		String modName;

		(IniSettingsString(SectionName, "Name", "", "Nome della sezione log di log")).GetValue((char*)iniFile, modName);

		if (modName[0]==0)
			break;

		CLog emptyLog;
		logInit.push_back(emptyLog);
		CLog &log=logInit[logInit.size()-1];
		log.logName=modName;

		log.bEnabled = (IniSettingsBool(SectionName, "LogEnable", bMainEnable, "Abilitazione log della sezione")).GetValue((char*)iniFile);

		(IniSettingsString(SectionName, "LogDir", logDirGlobal.lock(), "Definisce il path in cui salvare il file di log di questa sezione (con \\ finale). Default: directory di log globale")).GetValue((char*)iniFile, log.logDir);

		(IniSettingsString(SectionName, "LogFile", log.logName.lock(), "Definisce il nome del file in cui salvare il file di log di questa sezione (con \\ finale). Default: il nome della sezione di log")).GetValue((char*)iniFile, log.logFileName);

		log.bFunctionLog = (IniSettingsBool(SectionName, "FunctionLog", bFunctionLog, "Abilitazione log delle chiamate a funzione per questa sezione")).GetValue((char*)iniFile);

		log.bLogParam = (IniSettingsBool(SectionName, "ParamLog", bGlobalParam, "Abilitazione log dei parametri di input delle funzioni per questa sezione")).GetValue((char*)iniFile);

		log.bInitialized=true;
		numMod++;
	}
	if (logInit.size()==0) {
		OutputDebugString("Nessun LogModule definito. Impostare le sezioni [LogModule1]...[LogModuleN] con i valori:\n");
		OutputDebugString("Name,LogEnable,LogDir,LogFile,FunctionLog,ParamLog\n");
	}

	exit_func_internal
}

CLog::CLog() {
	init_func_internal
	bFirstLog=false;
	bInitialized=false;
	bEnabled=false;
	bLogParam=false;
	dwLogCount=0;
	exit_func_internal
}

CLog::~CLog() {
	bEnabled=false;
	bFirstLog=false;
}

void CLog::initParam(CLog &log) {
	init_func_internal
	bEnabled=log.bEnabled;
	bLogParam=log.bLogParam;

	dwModuleNum=dwGlobalModuleNum;
	dwGlobalModuleNum++;

	SYSTEMTIME  stTime;
	GetLocalTime(&stTime);

	switch (LogMode) {
		case (LM_Single): {
			// log singolo: il nome del file è yyyy-mm-gg.log, senza alcun path assegnato
			logPath.printf("%s_%04i-%02i-%02i.log",log.logFileName.lock(),stTime.wYear,stTime.wMonth,stTime.wDay);
			break;
		}
		case (LM_Module): {
			// log per modulo: il nome del file è yyyy-mm-gg_name.log, senza alcun path assegnato
			logPath.printf("%04i-%02i-%02i_%s.log",stTime.wYear,stTime.wMonth,stTime.wDay,log.logFileName.lock());
			break;
		}
		case (LM_Thread): {
			// log per thread: il nome del file è yyyy-mm-gg_tttttttt.log, senza alcun path assegnato
			logPath.printf("%04i-%02i-%02i_00000000.log",stTime.wYear,stTime.wMonth,stTime.wDay);
			break;
		}
		case (LM_Module_Thread): {
			// log per modulo e per thread: il nome del file è yyyy-mm-gg_name_tttttttt.log, senza alcun path assegnato
			logPath.printf("%04i-%02i-%02i_%s_00000000.log",stTime.wYear,stTime.wMonth,stTime.wDay,log.logFileName.lock());
			break;
		}
	}
	if ((LogMode==LM_Module || LogMode==LM_Module_Thread) && log.logDir.strlen()!=0) {
		// se c'è un path specifico lo metto lì
		String path=logPath;
		logPath.printf("%s\\%s",log.logDir.stringlock(),path.stringlock());
	}
	else if (!logDirGlobal.isEmpty()) {
		// se c'è un path globale lo metto lì
		String path=logPath;
		logPath.printf("%s\\%s",logDirGlobal.stringlock(),path.stringlock());
	}
	threadPos=logPath.lock()+logPath.strlen()-12;
	bInitialized=true;

	if (LogMode!=LM_Module && LogMode!=LM_Module_Thread && bEnabled) writePure("Module %02i: %s",dwModuleNum,logName.lock());
	exit_func_internal
}

void CLog::initModule(const char *name,char *version) {
	init_func_internal
	
	logName=name;
	logVersion=version;

	if (!bInitLog)
	// verrà inizializzato dopo, quando chiamo logInit
		logToInit.push_back(this);
	else {
		// vediamo se ce l'ho...
		for (DWORD i=0;i<logInit.size();i++) {
			if (strcmp(logInit[i].logName.lock(),logName.lock())==0) {
				initParam(logInit[i]);
			}
		}
	}

	exit_func_internal
}

DWORD CLog::write(const char *format,...) {
 	va_list params;
	va_start (params, format);
	char pbtDate[0x800];
	DWORD dummy=0;
	DWORD *dwNum=&dummy;

	if (bEnabled && bInitialized && bMainEnable) {

		if (!bFirstGlobal && LogMode==LM_Single) {
			bFirstGlobal =true;
			write("Inizio Sessione - versione: %s",logGlobalVersion);
			writeModuleInfo();
		}
		if (!bFirstLog && (LogMode==LM_Module || LogMode==LM_Module_Thread)) {
			bFirstLog=true;
			write("%s - Inizio Sessione - versione file: %s",logName.lock(),logVersion);
			writeModuleInfo();
		}

		//DWORD thNum;
		switch(LogMode) {
			case (LM_Module) : dwNum=&dwLogCount; break;
			case (LM_Module_Thread) :
			//case (LM_Thread) : thNum=dwThreadCount;dwNum=&thNum; break;
			case (LM_Single) : dwNum=&dwGlobalCount; break;
		}

		SYSTEMTIME  stTime;
		GetLocalTime(&stTime);
		sprintf_s(pbtDate,sizeof(pbtDate),"%05u:[%02d:%02d:%02d.%03d]", *dwNum, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);	
	 
		// se siamo in LM_thread devo scrivere il thread nel nome del file
		DWORD dwThreadID=CThread::getID();
		if (LogMode==LM_Thread || LogMode==LM_Module_Thread)
			sprintf_s(threadPos,14,"%08x.log",dwThreadID);
		FILE *lf=nullptr;
		fopen_s(&lf,logPath.lock(), "a+t");
		if (lf) {
			switch(LogMode) {
				case (LM_Single) : fprintf(lf,"%s|%04i|%04i|%02i|", pbtDate, GetCurrentProcessId(), dwThreadID, dwModuleNum); break;
				case (LM_Module) : fprintf(lf,"%s|%04i|%04x|", pbtDate, GetCurrentProcessId(), dwThreadID); break;
				case (LM_Thread) : fprintf(lf,"%s|%04i|%02i|", pbtDate, GetCurrentProcessId(), dwModuleNum); break;
				case (LM_Module_Thread) : fprintf(lf,"%s|", pbtDate); break;
			}
			vfprintf(lf, format, params);
			fprintf(lf, "\n");
			fclose(lf);
		}
	}

#ifdef _DEBUG
#ifdef WIN32
	vsprintf_s(pbtDate, format, params);
	int dtLen = (int)strnlen(pbtDate, sizeof(pbtDate));
	sprintf_s(pbtDate + dtLen, 2048 - dtLen, "|thread:%08x|%s|", GetCurrentThreadId(), logName.lock());
	dtLen = (int)strnlen(pbtDate, sizeof(pbtDate));
	sprintf_s(pbtDate+ dtLen, 2048 - dtLen , "\n");
	OutputDebugString(pbtDate);
#else
	puts(pbtDate);
#endif
#endif
 	va_end(params);
	switch(LogMode) {
		case (LM_Module) : dwLogCount++; break;
		case (LM_Module_Thread) :
		//case (LM_Thread) : dwThreadCount=thNum+1; break;
		case (LM_Single) : dwGlobalCount++; break;
	}
	return(*dwNum);
}

void CLog::writePure(const char *format,...) {
 	va_list params;
	va_start (params, format);
	char pbtDate[0x800]={NULL};
	if (bEnabled && bInitialized && bMainEnable) {
		if (!bFirstGlobal && LogMode==LM_Single) {
			bFirstGlobal =true;
			write("Inizio Sessione - versione: %s",logGlobalVersion);
			writeModuleInfo();
		}
		if (!bFirstLog && (LogMode==LM_Module || LogMode==LM_Module_Thread)) {
			bFirstLog=true;
			write("%s - Inizio Sessione - versione file: %s",logName.lock(),logVersion);
			writeModuleInfo();
		}

		// se siamo in LM_thread devo scrivere il thread nel nome del file
		DWORD dwThreadID=CThread::getID();
		if (LogMode==LM_Thread || LogMode==LM_Module_Thread)
			sprintf_s(threadPos,14,"%08x.log",dwThreadID);

		FILE *lf = nullptr;
		fopen_s(&lf,logPath.lock(), "a+t");
		if (lf) {
			vfprintf(lf, format, params);
			fprintf(lf, "\n");
			fclose(lf);
		}
	}
#ifdef _DEBUG
#ifdef WIN32
	int dtLen = (int)strnlen(pbtDate, sizeof(pbtDate));
	vsprintf_s(pbtDate+dtLen,2048-dtLen, format, params);
	dtLen = (int)strnlen(pbtDate, sizeof(pbtDate));
	sprintf_s(pbtDate + dtLen, 2048 - dtLen , "\n");
	OutputDebugString(pbtDate);
#else
	puts(pbtDate);
#endif
#endif
 	va_end(params);
}

void CLog::writeBinData(BYTE *data,int datalen) {
	if (!bEnabled || !bInitialized || !bMainEnable) return;
	if (!bFirstGlobal && LogMode==LM_Single) {
		bFirstGlobal =true;
		write("Inizio Sessione - versione: %s",logGlobalVersion);
		writeModuleInfo();
	}
	if (!bFirstLog && (LogMode==LM_Module || LogMode==LM_Module_Thread)) {
		bFirstLog=true;
		write("%s - Inizio Sessione - versione file: %s",logName.lock(),logVersion);
		writeModuleInfo();
	}

	char pbtDate[0x800]={NULL};

	// se siamo in LM_thread devo scrivere il thread nel nome del file
	DWORD dwThreadID=CThread::getID();
	if (LogMode==LM_Thread || LogMode==LM_Module_Thread)
		sprintf_s(threadPos,14,"%08x.log",dwThreadID);

	FILE *lf = nullptr;
	fopen_s(&lf,logPath.lock(), "a+t");
	if (lf) {
		if (datalen>100) datalen=100;
		for (int i=0;i<datalen;i++)
			fprintf(lf, "%02x ", data[i]);
		fprintf(lf, "\n");
		fclose(lf);
	}
}

void CLog::writeModuleInfo() {
	if (!bEnabled) return;
	CModuleInfo module;
	HANDLE mainModule = module.getApplicationModule();
	module.init(mainModule);
	write("Applicazione chiamante: %s",module.szModuleName.stringlock());
}

void CLog::__dumpStack() {
#ifdef _DUMP_STACK
 //#ifdef _DEBUG
	_threadData thData;
	DuplicateHandle (GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),&thData.hHandle,0,TRUE,DUPLICATE_SAME_ACCESS);
	thData.dwId=GetCurrentThreadId();
	CThread dumpThread;
	dumpThread.createThread(___dumpStack,&thData);
	stackSem.Wait();
	CloseHandle(thData.hHandle);
	dumpThread.joinThread(0);
 //#endif
#endif
}
