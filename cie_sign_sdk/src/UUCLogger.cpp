#include "UUCLogger.h"
#include <time.h>
#include <string>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#ifdef __ANDROID__
#include <android/log.h>
//__android_log_print(ANDROID_LOG_INFO, "MYPROG", "errno = %d, %s", errno, strerror(errno));
//__android_log_print(ANDROID_LOG_INFO, "MYPROG", "Hellowrorld");
#endif

logFunc pfnCrashliticsLog = NULL;

UUCLogger::UUCLogger(void)
: m_nLogLevel(m_nLogLevel) // Set to DEBUG to capture all messages by default
{
	initializeLogFile();
}

UUCLogger::UUCLogger(const char* szLogFileName)
: m_nLogLevel(m_nLogLevel)
{
	// If a specific filename is provided, use it; otherwise initialize with default
	if (szLogFileName && strlen(szLogFileName) > 0) {
		setLogFile(szLogFileName);
	} else {
		initializeLogFile();
	}
}

UUCLogger::~UUCLogger(void)
{
}

void UUCLogger::initializeLogFile()
{
#ifdef _WIN32
	// Get PROGRAMDATA directory path
	char szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath))) {
		// Create CIEPKI directory if it doesn't exist
		strcat_s(szPath, MAX_PATH, "\\CIEPKI");
		_mkdir(szPath);
		
		// Get current date for filename
		time_t now = time(NULL);
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);
		
		// Create filename with format: CIE_SIGN_YYYY-MM-DD.log
		char szFileName[MAX_PATH];
		sprintf_s(szFileName, MAX_PATH, "\\CIE_SIGN_SDK_%04d-%02d-%02d.log",
			timeinfo.tm_year + 1900,
			timeinfo.tm_mon + 1,
			timeinfo.tm_mday);
		
		strcat_s(szPath, MAX_PATH, szFileName);
		strcpy_s(m_szLogFileName, MAX_PATH, szPath);
	} else {
		// Fallback to current directory if PROGRAMDATA is not accessible
		m_szLogFileName[0] = '\0';
	}
#else
	// For non-Windows platforms, use current directory
	m_szLogFileName[0] = '\0';
#endif
}

void UUCLogger::setLogLevel(int loglevel)
{
	m_nLogLevel = loglevel;
}

void UUCLogger::setLogFile(const char* szLogFileName)
{
	strcpy(m_szLogFileName, szLogFileName);
}

void UUCLogger::log(const unsigned int nType, const char* szMsg, const unsigned int nID, const char *szModuleName, va_list args)
{
	try
	{
		vsnprintf( m_szBuffer, 5000, szMsg, args ); // C4996
		log(nType, m_szBuffer, nID, szModuleName);	
	}
	catch(...)
	{

	}
}

void UUCLogger::log(const unsigned int nType, const char *szMsg, const unsigned int nID, const char *szModuleName)
{
	if(nType > m_nLogLevel || nType == LOG_TYPE_NONE)
		return;

#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_DEBUG, "DigitSign", "%d, %X, %s, %s", nType, nID, szModuleName, szMsg);	
#else

#ifdef _WIN32
	// Write to log file if path is set
	if(m_szLogFileName[0] != '\0')
	{
		FILE* f = nullptr;
		errno_t err = fopen_s(&f, m_szLogFileName, "a+t");
		if(err == 0 && f != NULL)
		{
			// Get current time with milliseconds
			SYSTEMTIME st;
			GetLocalTime(&st);
			
			// Determine log type string
			const char* szType = "";
			switch(nType) {
				case LOG_TYPE_ERROR:   szType = "[ERROR]"; break;
				case LOG_TYPE_WARNING: szType = "[WARNING]"; break;
				case LOG_TYPE_MESSAGE: szType = "[MESSAGE]"; break;
				case LOG_TYPE_DEBUG:   szType = "[DEBUG]"; break;
				default:               szType = "[UNKNOWN]"; break;
			}
			
			// Write log entry with timestamp
			fprintf(f, "%04d-%02d-%02d %02d:%02d:%02d.%03d  %s %s: %s\n",
				st.wYear, st.wMonth, st.wDay,
				st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
				szType, szModuleName, szMsg);
			
			fclose(f);
		}
	}
#endif

	// Also print to console for debugging
	char szLogMsg[5000];
	time_t ltime;
	time( &ltime );
	tm* pCurTime = localtime(&ltime);
	
	char* szTime = asctime(pCurTime);
	if (szTime) {
		szTime[strlen(szTime) - 1] = 0;
	}
	
	sprintf(szLogMsg, "[%s], %d, %X, %s, %s\n", szTime ? szTime : "", nType, nID, szModuleName, szMsg);
	printf("%s", szLogMsg);
	
	if(pfnCrashliticsLog != NULL)
		pfnCrashliticsLog(szLogMsg);
    
#endif
}


void UUCLogger::logMessage(const unsigned int nID, const char *szModuleName, const char* szMsg, ...)
{
	if(LOG_TYPE_MESSAGE > m_nLogLevel)
		return;

	va_list args;	
	
	szMsg = (szMsg == NULL) ? (char*)"" : szMsg;
		
	va_start (args, szMsg);

	log(LOG_TYPE_MESSAGE, szMsg, nID, szModuleName, args);

	va_end(args);				
}

void UUCLogger::logWarning(const unsigned int nID, const char *szModuleName, const char* szMsg, ...)
{
	if(LOG_TYPE_WARNING > m_nLogLevel)
		return;

	va_list args;	
	
	szMsg = (szMsg == NULL) ? (char*)"" : szMsg;
		
	va_start (args, szMsg);

	log(LOG_TYPE_WARNING, szMsg, nID, szModuleName, args);

	va_end(args);				

}
void UUCLogger::logError(const unsigned int nID, const char *szModuleName, const char* szMsg, ...)
{
	if(LOG_TYPE_ERROR > m_nLogLevel)
		return;

	va_list args;	
	
	szMsg = (szMsg == NULL) ? (char*)"" : szMsg;
		
	va_start (args, szMsg);

	log(LOG_TYPE_ERROR, szMsg, nID, szModuleName, args);

	va_end(args);				

}

void UUCLogger::logDebug(const unsigned int nID, const char *szModuleName, const char* szMsg, ...)
{
	if(LOG_TYPE_DEBUG > m_nLogLevel)
		return;

	va_list args;	
	
	szMsg = (szMsg == NULL) ? (char*)"" : szMsg;
		
	va_start (args, szMsg);

	log(LOG_TYPE_DEBUG, szMsg, nID, szModuleName, args);

	va_end(args);				

}


