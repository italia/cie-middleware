
#define __STDC_WANT_LIB_EXT1__ 1

#include <time.h>
#include "util.h"
#include "ModuleInfo.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include "log.h"
#include "UtilException.h"
//#include "Thread.h"
#include "IniSettings.h"
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include "UUCProperties.h"
#include <sys/stat.h>
#include <regex>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

std::string globalLogDir;
std::string globalLogName;
bool FunctionLog=false;
bool globalLogParam=false;
bool firstGlobal=false;
const char *logGlobalVersion;
unsigned int GlobalDepth = 0;
bool mainInit=false;
bool mainEnable=false;
unsigned int GlobalCount;

enum logMode {
	LM_Single,	// un solo file
	LM_Module,	// un file per modulo
	LM_Thread,	// un file per thread
	LM_Module_Thread	// un file per modulo e per thread
} LogMode = LM_Module;


void initLog(const char *moduleName, const char *iniFile,const char *version)
{

	if (mainInit)
		return;
	
    mainInit=true;

	logGlobalVersion=version;
	
	OutputDebugString("File INI:");
	OutputDebugString(iniFile);
	OutputDebugString("\n");

    UUCProperties settings;
    //settings.load(iniFile);
    
    LogMode = (logMode)(settings.getIntProperty("LogMode", (int)LM_Single));//, "Modalità di Log. Valori possibili:\n"
//        "0 ;LM_Single,    // un solo file\n"
//        "1 ;LM_Module,    // un file per modulo\n"
//        "2 ;LM_Thread,    // un file per thread\n"
//        "3 ;LM_Module_Thread    // un file per modulo e per thread\n")).GetValue((char*)iniFile);

	if (LogMode==-1) {
		LogMode=LM_Single;
	}

    mainEnable = settings.getIntProperty("LogEnable",1);//,"Abilitazione log globale")).GetValue((char*)iniFile);

    FunctionLog = settings.getIntProperty("FunctionLog", 1);//, "Abilitazione log delle chiamate a funzione")).GetValue((char*)iniFile);

    GlobalDepth = settings.getIntProperty("FunctionDepth", 10);//, "Definisce la profondità massima di log delle funzioni\n")).GetValue((char*)iniFile);

    globalLogParam = settings.getIntProperty("ParamLog", 1);//, "Abilitazione log dei parametri di input delle funzioni")).GetValue((char*)iniFile);
    
    globalLogName = moduleName;
    
    char* home = getenv("HOME");
    if(home == NULL)
    {
    	struct passwd *pw = getpwuid(getuid());

    	home = pw->pw_dir;
    	printf("home: %s", home);
    }

    std::string path(home);

	std::smatch match;
	std::regex_search(path, match, std::regex("^/(home|root)/"));
	std::string suffix = match.suffix();
	if(suffix.find("/") != std::string::npos)
		throw 1;

//    std::smatch match;
//    std::regex_search(path, match, std::regex("^/Users/"));
//    std::string suffix = match.suffix();
//    if(suffix.find("/") != std::string::npos)
//        throw 1;
    
    path.append("/.CIEPKI/");
    
    struct stat st = {0};
    
    if (stat(path.c_str(), &st) == -1) {
        mkdir(path.c_str(), 0700);
    }
    
    globalLogDir = settings.getProperty("LogDir", path.c_str()); //"Definisce il path in cui salvare il file di log (con / finale)"))
    
}

CLog::CLog() {
	
    init();
}

CLog::~CLog() {
	Enabled=false;
	FirstLog=false;
}

void CLog::init() {
	
	Enabled=mainEnable;
	LogParam=globalLogParam;
    LogCount=0;
    logName = globalLogName;
    logFileName = globalLogName;
    
    std::stringstream th;
    th << std::setw(8) << std::setfill('0');
    
    time_t T= time(NULL);
    struct tm t;
    struct  tm tm = *localtime_r(&T, &t);

    switch (LogMode) {
        case (LM_Single): {
            th << logFileName << "_" << std::setw(4) << tm.tm_year << "-" << std::setw(2) << tm.tm_mon << "-" << tm.tm_mday << ".log";
            break;
        }
        case (LM_Module): {
            th << std::setw(4) << tm.tm_year << "-" << std::setw(2) << tm.tm_mon << "-" << tm.tm_mday << "_" << logFileName << ".log";
            // log per modulo: il nome del file è yyyy-mm-gg_name.log, senza alcun path assegnato
            break;
        }
        case (LM_Thread): {
            th << std::setw(4) << tm.tm_year << "-" << std::setw(2) << tm.tm_mon << "-" << tm.tm_mday << "_00000000.log";
            // log per thread: il nome del file è yyyy-mm-gg_tttttttt.log, senza alcun path assegnato
            break;
        }
        case (LM_Module_Thread): {
            th << std::setw(4) << tm.tm_year << "-" << std::setw(2) << tm.tm_mon << "-" << tm.tm_mday << "_" << logFileName << "_00000000.log";
            // log per modulo e per thread: il nome del file è yyyy-mm-gg_name_tttttttt.log, senza alcun path assegnato
            break;
        }
    }
    
	logPath = th.str();

	if ((LogMode==LM_Module || LogMode==LM_Module_Thread) && logDir.length()!=0) {
		// se c'è un path specifico lo metto lì
		std::string path = logPath;
		logPath=logDir.append(path);
	}
	else if (!globalLogDir.empty()) {
		// se c'è un path globale lo metto lì
		std::string path=logPath;
		logPath = globalLogDir.append(path);
	}
	threadPos = logPath.begin()+logPath.length() - 12;
	Initialized=true;

	if (LogMode!=LM_Module && LogMode!=LM_Module_Thread && Enabled) writePure("Module %02i: %s",ModuleNum,logName.c_str());
	
}

DWORD CLog::write(const char *format,...) {
 	va_list params;
	va_start (params, format);
	char pbtDate[20];
	unsigned int dummy = 0;
	unsigned int *Num = &dummy;

	if (Enabled && Initialized && mainEnable) {

		if (!firstGlobal && LogMode==LM_Single) {
			firstGlobal =true;
			write("Inizio Sessione - versione: %s",logGlobalVersion);
			writeModuleInfo();
		}
		if (!FirstLog && (LogMode==LM_Module || LogMode==LM_Module_Thread)) {
			FirstLog=true;
			write("%s - Inizio Sessione - versione file: %s",logName.c_str(), logVersion.c_str());
			writeModuleInfo();
		}

		//DWORD thNum;
		switch(LogMode) {
			case (LM_Module) : Num=&LogCount; break;
			case (LM_Module_Thread) :
			case (LM_Thread) : /*thNum=dwThreadCount;dwNum=&thNum;0*/ break;
			case (LM_Single) : Num=&GlobalCount; break;
		}
#ifdef WIN32
		SYSTEMTIME  stTime;
		GetLocalTime(&stTime);
		sprintf_s(pbtDate,sizeof(pbtDate),"%05u:[%02d:%02d:%02d.%03d]", *Num, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);	
#else
		time_t T= time(NULL);
		struct tm t;
		struct  tm tm = *localtime_r(&T, &t);

        snprintf(pbtDate,20,"%05u:[%02d:%02d:0%02d]", *Num, tm.tm_hour, tm.tm_min, tm.tm_sec);
#endif
		// se siamo in LM_thread devo scrivere il thread nel nome del file
		std::hash<std::thread::id> hasher;
		auto dwThreadID = hasher(std::this_thread::get_id());
		if (LogMode == LM_Thread || LogMode == LM_Module_Thread) {
			std::stringstream th;
			th << std::setiosflags(std::ios::hex | std::ios::uppercase);
			th << std::setw(8);
			th << dwThreadID << ".log";

			logPath.replace(threadPos, threadPos + 14, th.str());
		}
		FILE *lf=nullptr;
#ifdef WIN32
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
#else
        lf = fopen(logPath.c_str(), "a+t");
        if (lf) {

        	struct stat	lstat_buf;
			struct stat	fstat_buf;

			int r = lstat(logPath.c_str(), &lstat_buf);

			/* handle the case of the lstat failing first */
			if (r == -1)
			{
				fclose(lf);
				return ERROR_FILE_NOT_FOUND;
			}

			if (S_ISLNK(lstat_buf.st_mode))  {
				fclose(lf);
				return (long)ERROR_FILE_NOT_FOUND;
			}

			/* Get the properties of the opened file descriptor */
			r = stat(logPath.c_str(), &fstat_buf);
			if (r == -1)
			{
				fclose(lf);
				return (long)ERROR_FILE_NOT_FOUND;
			}

			if (lstat_buf.st_dev != fstat_buf.st_dev
				 || lstat_buf.st_ino != fstat_buf.st_ino
				 || (S_IFMT & lstat_buf.st_mode) != (S_IFMT & fstat_buf.st_mode))
			{
				fclose(lf);
				return (long)ERROR_FILE_NOT_FOUND;
			}

            switch(LogMode) {
                case (LM_Single) : fprintf(lf,"%s|%04i|%04i|%02i|", pbtDate, getpid(), dwThreadID, ModuleNum); break;
                case (LM_Module) : fprintf(lf,"%s|%04i|%04lx|", pbtDate, getpid(), dwThreadID); break;
                case (LM_Thread) : fprintf(lf,"%s|%04i|%02i|", pbtDate, getpid(), ModuleNum); break;
                case (LM_Module_Thread) : fprintf(lf,"%s|", pbtDate); break;
            }
            vfprintf(lf, format, params);
            fprintf(lf, "\n");
            fclose(lf);
        }
        
//        printf(format, params);
//        printf("\n", NULL);
#endif
		
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
		case (LM_Thread) : /*dwThreadCount=thNum+1;*/ break;
		case (LM_Single) : GlobalCount++; break;
	}
	return(*Num);
}

void CLog::writePure(const char *format,...) {
 	va_list params;
	va_start (params, format);
//    char pbtDate[0x800]={NULL};
	if (Enabled && Initialized && mainEnable) {
		if (!firstGlobal && LogMode==LM_Single) {
			firstGlobal =true;
			write("Inizio Sessione - versione: %s",logGlobalVersion);
			writeModuleInfo();
		}
		if (!FirstLog && (LogMode==LM_Module || LogMode==LM_Module_Thread)) {
			FirstLog=true;
			write("%s - Inizio Sessione - versione file: %s",logName.c_str(), logVersion.c_str());
			writeModuleInfo();
		}

		// se siamo in LM_thread devo scrivere il thread nel nome del file
		std::hash<std::thread::id> hasher;
		auto dwThreadID = hasher(std::this_thread::get_id());
		if (LogMode == LM_Thread || LogMode == LM_Module_Thread) {
			std::stringstream th;
			th << std::setiosflags(std::ios::hex | std::ios::uppercase);
			th << std::setw(8);
			th << dwThreadID << ".log";

			logPath.replace(threadPos, threadPos + 14, th.str());
		}
		FILE *lf = nullptr;
#ifdef WIN32
		fopen_s(&lf,logPath.c_str(), "a+t");
#else
        lf = fopen(logPath.c_str(), "a+t");
#endif
		if (lf) {

			struct stat	lstat_buf;
			struct stat	fstat_buf;

			int r = lstat(logPath.c_str(), &lstat_buf);

			/* handle the case of the lstat failing first */
			if (r == -1)
			{
				fclose(lf);
				return;
			}

			if (S_ISLNK(lstat_buf.st_mode))  {
				fclose(lf);
				return;
			}

			/* Get the properties of the opened file descriptor */
			r = stat(logPath.c_str(), &fstat_buf);
			if (r == -1)
			{
				fclose(lf);
				return;
			}

			if (lstat_buf.st_dev != fstat_buf.st_dev
				 || lstat_buf.st_ino != fstat_buf.st_ino
				 || (S_IFMT & lstat_buf.st_mode) != (S_IFMT & fstat_buf.st_mode))
			{
				fclose(lf);
				return;
			}

			vfprintf(lf, format, params);
			fprintf(lf, "\n");
			fclose(lf);
		}
        
//        printf(format, params);
//        printf("\n", NULL);
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
	if (!Enabled || !Initialized || !mainEnable) return;
	if (!firstGlobal && LogMode==LM_Single) {
		firstGlobal =true;
		write("Inizio Sessione - versione: %s",logGlobalVersion);
		writeModuleInfo();
	}
	if (!FirstLog && (LogMode==LM_Module || LogMode==LM_Module_Thread)) {
		FirstLog=true;
		write("%s - Inizio Sessione - versione file: %s",logName.c_str(), logVersion.c_str());
		writeModuleInfo();
	}

//    char pbtDate[0x800]={NULL};

	// se siamo in LM_thread devo scrivere il thread nel nome del file
	std::hash<std::thread::id> hasher;
	auto dwThreadID = hasher(std::this_thread::get_id());
	if (LogMode == LM_Thread || LogMode == LM_Module_Thread) {
		std::stringstream th;
		th << std::setiosflags(std::ios::hex | std::ios::uppercase);
		th << std::setw(8);
		th << dwThreadID << ".log";

		logPath.replace(threadPos, threadPos + 14, th.str());
	}

	FILE *lf = nullptr;
#ifdef WIN32
	fopen_s(&lf,logPath.c_str(), "a+t");
#else
    lf = fopen(logPath.c_str(), "a+t");
#endif
	if (lf) {

		struct stat	lstat_buf;
		struct stat	fstat_buf;

		int r = lstat(logPath.c_str(), &lstat_buf);

		/* handle the case of the lstat failing first */
		if (r == -1)
		{
			fclose(lf);
			return;
		}

		if (S_ISLNK(lstat_buf.st_mode))  {
			fclose(lf);
			return;
		}

		/* Get the properties of the opened file descriptor */
		r = stat(logPath.c_str(), &fstat_buf);
		if (r == -1)
		{
			fclose(lf);
			return;
		}

		if (lstat_buf.st_dev != fstat_buf.st_dev
			 || lstat_buf.st_ino != fstat_buf.st_ino
			 || (S_IFMT & lstat_buf.st_mode) != (S_IFMT & fstat_buf.st_mode))
		{
			fclose(lf);
			return;
		}


		if (datalen>100) datalen=100;
		for (size_t i=0;i<datalen;i++)
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


CLog Log;
