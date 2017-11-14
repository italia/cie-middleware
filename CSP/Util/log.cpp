#include "../stdafx.h"
#include "util.h"
#include "moduleinfo.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include "log.h"
#include "DbgHelp.h"
#include "UtilException.h"
#include "thread.h"
#include "IniSettings.h"


static char *szCompiledFile=__FILE__;

std::vector<CLog> logInit;
std::vector<CLog*> logToInit;

std::string logDirGlobal;
bool InitLog=false;
bool FirstGlobal=false;
bool FunctionLog=false;
bool GlobalParam=false;
const char *logGlobalVersion;
unsigned int GlobalModuleNum=1;
unsigned int GlobalCount;
unsigned int GlobalDepth = 0;
CLog Log;

enum logMode {
	LM_Single,	// un solo file
	LM_Module,	// un file per modulo
	LM_Thread,	// un file per thread
	LM_Module_Thread	// un file per modulo e per thread
} LogMode = LM_Module;

bool MainInit=false;
bool MainEnable=false;

void initLog(const char *iniFile,const char *version) {
	init_func_internal

	if (MainInit)
		return;
	MainInit=true;
	InitLog=true;

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

	MainEnable=(IniSettingsBool("Log","LogEnable",false,"Abilitazione log globale")).GetValue((char*)iniFile);

	FunctionLog = (IniSettingsBool("Log", "FunctionLog", false, "Abilitazione log delle chiamate a funzione")).GetValue((char*)iniFile);

	GlobalDepth = (IniSettingsInt("Log", "FunctionDepth", 10, "Definisce la profondità massima di log delle funzioni\n")).GetValue((char*)iniFile);

	GlobalParam = (IniSettingsBool("Log", "ParamLog", false, "Abilitazione log dei parametri di input delle funzioni")).GetValue((char*)iniFile);

	(IniSettingsString("Log", "LogDir", "c:\\", "Definisce il path in cui salvare il file di log (con \\ finale)")).GetValue((char*)iniFile, logDirGlobal);

	char SectionName[30];
	int numMod=1;
	while (true) {
		sprintf_s(SectionName,30,"%s%i","LogModule",numMod);
		std::string modName;

		(IniSettingsString(SectionName, "Name", "", "Nome della sezione log di log")).GetValue((char*)iniFile, modName);

		if (modName[0]==0)
			break;

		CLog emptyLog;
		logInit.push_back(emptyLog);
		CLog &log=logInit[logInit.size()-1];
		log.logName=modName;

		log.Enabled = (IniSettingsBool(SectionName, "LogEnable", MainEnable, "Abilitazione log della sezione")).GetValue((char*)iniFile);

		(IniSettingsString(SectionName, "LogDir", logDirGlobal.c_str(), "Definisce il path in cui salvare il file di log di questa sezione (con \\ finale). Default: directory di log globale")).GetValue((char*)iniFile, log.logDir);

		(IniSettingsString(SectionName, "LogFile", log.logName.c_str(), "Definisce il nome del file in cui salvare il file di log di questa sezione (con \\ finale). Default: il nome della sezione di log")).GetValue((char*)iniFile, log.logFileName);

		log.FunctionLog = (IniSettingsBool(SectionName, "FunctionLog", FunctionLog, "Abilitazione log delle chiamate a funzione per questa sezione")).GetValue((char*)iniFile);

		log.LogParam = (IniSettingsBool(SectionName, "ParamLog", GlobalParam, "Abilitazione log dei parametri di input delle funzioni per questa sezione")).GetValue((char*)iniFile);

		log.Initialized=true;
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
	FirstLog=false;
	Initialized=false;
	Enabled=false;
	LogParam=false;
	LogCount=0;
	exit_func_internal
}

CLog::~CLog() {
	Enabled=false;
	FirstLog=false;
}

void CLog::initParam(CLog &log) {
	init_func_internal
	Enabled=log.Enabled;
	LogParam=log.LogParam;

	ModuleNum=GlobalModuleNum;
	GlobalModuleNum++;

	SYSTEMTIME  stTime;
	GetLocalTime(&stTime);

	std::stringstream th;
	th << std::setw(8) << std::setfill('0');

	switch (LogMode) {
		case (LM_Single): {
			th << log.logFileName << "_" << std::setw(4) << stTime.wYear << "-" << std::setw(2) << stTime.wMonth << "-" << stTime.wDay << ".log";
			break;
		}
		case (LM_Module): {
			th << std::setw(4) << stTime.wYear << "-" << std::setw(2) << stTime.wMonth << "-" << stTime.wDay << "_" << logFileName << ".log";
			// log per modulo: il nome del file è yyyy-mm-gg_name.log, senza alcun path assegnato
			break;
		}
		case (LM_Thread): {
			th << std::setw(4) << stTime.wYear << "-" << std::setw(2) << stTime.wMonth << "-" << stTime.wDay << "_00000000.log";
			// log per thread: il nome del file è yyyy-mm-gg_tttttttt.log, senza alcun path assegnato
			break;
		}
		case (LM_Module_Thread): {
			th << std::setw(4) << stTime.wYear << "-" << std::setw(2) << stTime.wMonth << "-" << stTime.wDay << "_" << logFileName << "_00000000.log";
			// log per modulo e per thread: il nome del file è yyyy-mm-gg_name_tttttttt.log, senza alcun path assegnato
			break;
		}
	}
	logPath = th.str();

	if ((LogMode==LM_Module || LogMode==LM_Module_Thread) && log.logDir.length()!=0) {
		// se c'è un path specifico lo metto lì
		std::string path = logPath;
		logPath=log.logDir.append("\\").append(path);
	}
	else if (!logDirGlobal.empty()) {
		// se c'è un path globale lo metto lì
		std::string path=logPath;
		logPath = logDirGlobal.append("\\").append(path);
	}
	threadPos = logPath.begin()+logPath.length() - 12;
	Initialized=true;

	if (LogMode!=LM_Module && LogMode!=LM_Module_Thread && Enabled) writePure("Module %02i: %s",ModuleNum,logName.c_str());
	exit_func_internal
}

void CLog::initModule(const char *name, const char *version) {
	init_func_internal
	
	logName=name;
	logVersion=version;

	if (!InitLog)
	// verrà inizializzato dopo, quando chiamo logInit
		logToInit.push_back(this);
	else {
		// vediamo se ce l'ho...
		for (DWORD i=0;i<logInit.size();i++) {
			if (logInit[i].logName==logName) {
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
	unsigned int dummy = 0;
	unsigned int *Num = &dummy;

	if (Enabled && Initialized && MainEnable) {

		if (!FirstGlobal && LogMode==LM_Single) {
			FirstGlobal =true;
			write("Inizio Sessione - versione: %s",logGlobalVersion);
			writeModuleInfo();
		}
		if (!FirstLog && (LogMode==LM_Module || LogMode==LM_Module_Thread)) {
			FirstLog=true;
			write("%s - Inizio Sessione - versione file: %s",logName.c_str(),logVersion);
			writeModuleInfo();
		}

		//DWORD thNum;
		switch(LogMode) {
			case (LM_Module) : Num=&LogCount; break;
			case (LM_Module_Thread) :
			//case (LM_Thread) : thNum=dwThreadCount;dwNum=&thNum; break;
			case (LM_Single) : Num=&GlobalCount; break;
		}

		SYSTEMTIME  stTime;
		GetLocalTime(&stTime);
		sprintf_s(pbtDate,sizeof(pbtDate),"%05u:[%02d:%02d:%02d.%03d]", *Num, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);	
	 
		// se siamo in LM_thread devo scrivere il thread nel nome del file
		DWORD dwThreadID=CThread::getID();
		if (LogMode == LM_Thread || LogMode == LM_Module_Thread) {
			std::stringstream th;
			th << std::setiosflags(std::ios::hex | std::ios::uppercase);
			th << std::setw(8);
			th << dwThreadID << ".log";

			logPath.replace(threadPos, threadPos + 14, th.str());
		}
		FILE *lf=nullptr;
		fopen_s(&lf,logPath.c_str(), "a+t");
		if (lf) {
			switch(LogMode) {
				case (LM_Single) : fprintf(lf,"%s|%04i|%04i|%02i|", pbtDate, GetCurrentProcessId(), dwThreadID, ModuleNum); break;
				case (LM_Module) : fprintf(lf,"%s|%04i|%04x|", pbtDate, GetCurrentProcessId(), dwThreadID); break;
				case (LM_Thread) : fprintf(lf,"%s|%04i|%02i|", pbtDate, GetCurrentProcessId(), ModuleNum); break;
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
	sprintf_s(pbtDate + dtLen, 2048 - dtLen, "|thread:%08x|%s|", GetCurrentThreadId(), logName.c_str());
	dtLen = (int)strnlen(pbtDate, sizeof(pbtDate));
	sprintf_s(pbtDate+ dtLen, 2048 - dtLen , "\n");
	OutputDebugString(pbtDate);
#else
	puts(pbtDate);
#endif
#endif
 	va_end(params);
	switch(LogMode) {
		case (LM_Module) : LogCount++; break;
		case (LM_Module_Thread) :
		//case (LM_Thread) : dwThreadCount=thNum+1; break;
		case (LM_Single) : GlobalCount++; break;
	}
	return(*Num);
}

void CLog::writePure(const char *format,...) {
 	va_list params;
	va_start (params, format);
	char pbtDate[0x800]={NULL};
	if (Enabled && Initialized && MainEnable) {
		if (!FirstGlobal && LogMode==LM_Single) {
			FirstGlobal =true;
			write("Inizio Sessione - versione: %s",logGlobalVersion);
			writeModuleInfo();
		}
		if (!FirstLog && (LogMode==LM_Module || LogMode==LM_Module_Thread)) {
			FirstLog=true;
			write("%s - Inizio Sessione - versione file: %s",logName.c_str(),logVersion);
			writeModuleInfo();
		}

		// se siamo in LM_thread devo scrivere il thread nel nome del file
		DWORD dwThreadID=CThread::getID();
		if (LogMode == LM_Thread || LogMode == LM_Module_Thread) {
			std::stringstream th;
			th << std::setiosflags(std::ios::hex | std::ios::uppercase);
			th << std::setw(8);
			th << dwThreadID << ".log";

			logPath.replace(threadPos, threadPos + 14, th.str());
		}
		FILE *lf = nullptr;
		fopen_s(&lf,logPath.c_str(), "a+t");
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

void CLog::writeBinData(BYTE *data, size_t datalen) {
	if (!Enabled || !Initialized || !MainEnable) return;
	if (!FirstGlobal && LogMode==LM_Single) {
		FirstGlobal =true;
		write("Inizio Sessione - versione: %s",logGlobalVersion);
		writeModuleInfo();
	}
	if (!FirstLog && (LogMode==LM_Module || LogMode==LM_Module_Thread)) {
		FirstLog=true;
		write("%s - Inizio Sessione - versione file: %s",logName.c_str(),logVersion);
		writeModuleInfo();
	}

	char pbtDate[0x800]={NULL};

	// se siamo in LM_thread devo scrivere il thread nel nome del file
	DWORD dwThreadID=CThread::getID();
	if (LogMode == LM_Thread || LogMode == LM_Module_Thread) {
		std::stringstream th;
		th << std::setiosflags(std::ios::hex | std::ios::uppercase);
		th << std::setw(8);
		th << dwThreadID << ".log";

		logPath.replace(threadPos, threadPos + 14, th.str());
	}

	FILE *lf = nullptr;
	fopen_s(&lf,logPath.c_str(), "a+t");
	if (lf) {
		if (datalen>100) datalen=100;
		for (int i=0;i<datalen;i++)
			fprintf(lf, "%02x ", data[i]);
		fprintf(lf, "\n");
		fclose(lf);
	}
}

void CLog::writeModuleInfo() {
	if (!Enabled) return;
	CModuleInfo module;
	HANDLE mainModule = module.getApplicationModule();
	module.init(mainModule);
	write("Applicazione chiamante: %s",module.szModuleName.c_str());
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
