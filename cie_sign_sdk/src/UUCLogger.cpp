#include "UUCLogger.h"
#include <time.h>
#include <string>
#include <stdio.h>

#ifdef __ANDROID__
#include <android/log.h>
//__android_log_print(ANDROID_LOG_INFO, "MYPROG", "errno = %d, %s", errno, strerror(errno));
//__android_log_print(ANDROID_LOG_INFO, "MYPROG", "Hellowrold");
#endif

logFunc pfnCrashliticsLog = NULL;


UUCLogger::UUCLogger(void)
: m_nLogLevel(1)
{
}

UUCLogger::UUCLogger(const char* szLogFileName)
: m_nLogLevel(1)
{
	setLogFile(szLogFileName);
}

UUCLogger::~UUCLogger(void)
{
}

void UUCLogger::setLogLevel(int loglevel)
{
	m_nLogLevel = loglevel;
}

void UUCLogger::setLogFile(const char* szLogFileName)
{
	strcpy(m_szLogFileName, szLogFileName);
/*
#ifdef WIN32
	if(strstr(szLogFileName, ":\\") == (szLogFileName + 1) || 
	   strstr(szLogFileName, "\\") == szLogFileName ||
	   strstr(szLogFileName, "..\\") == szLogFileName || 
	   strstr(szLogFileName, ".\\") == szLogFileName)
	{
		// path assoluto
		strcpy(m_szLogFileName, szLogFileName);
	}
	else
	{
		char szTempPath[MAX_PATH];
		GetTempPath(MAX_PATH, szTempPath);
		strcat(szTempPath, szLogFileName);
		strcpy(m_szLogFileName, szTempPath);
	}
#else
	// path assoluto
	strcpy(m_szLogFileName, szLogFileName);
#endif
	*/
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
	if(nType > m_nLogLevel)
		return;

#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_DEBUG, "DigitSign", "%d, %X, %s, %s%", nType, nID, szModuleName, szMsg);	
#else
    /*
	if(m_szLogFileName[0] == NULL)
		return;

	FILE* f = fopen(m_szLogFileName, "a+t");
	if(f == NULL)
	{
		return;
	}

	/* Get UNIX-style time and display as number and string. */
	time_t ltime;
    time( &ltime );
	tm* pCurTime = localtime(&ltime);
	
	char* szTime = asctime(pCurTime);
	szTime[strlen(szTime) - 1] = 0;

	
    
    char szLogMsg[5000];
    sprintf(szLogMsg, "[%s], %d, %X, %s, %s\n", szTime, nType, nID, szModuleName, szMsg);
    printf(szLogMsg);
    if(pfnCrashliticsLog != NULL)
        pfnCrashliticsLog(szLogMsg);
    
//    fprintf(f, "[%s], %d, %X, %s, %s\n", szTime, nType, nID, szModuleName, szMsg);
//
//	fclose(f);
    
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


