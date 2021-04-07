#pragma once

#include "disigonsdk.h"
#include <stdarg.h>

#define MAX_LOG_SIZE			5000

#define LOG_TYPE_ERROR			1
#define LOG_TYPE_WARNING		2
#define LOG_TYPE_MESSAGE		3
#define LOG_TYPE_DEBUG			4

// macro 
#define DECLARE_LOG UUCLogger g_log
#define USE_LOG extern UUCLogger g_log
#define SET_LOG_FILE(file) g_log.setLogFile(file)
#define SET_LOG_LEVEL(level) g_log.setLogLevel(level)

#define LOG_MSG(params) g_log.logMessage params 
#define LOG_ERR(params) g_log.logError params
#define LOG_WAR(params) g_log.logWarning params
#define LOG_DBG(params) g_log.logDebug params



class UUCLogger
{
public:
	UUCLogger(void);
	UUCLogger(const char* szLogFileName);
	virtual ~UUCLogger(void);

	virtual void logMessage(const unsigned int nID, const char *szModuleName, const char* szMsg, ...);
	virtual void logWarning(const unsigned int nID, const char *szModuleName, const char* szMsg, ...);
	virtual void logError(const unsigned int nID, const char *szModuleName, const char* szMsg, ...);
	virtual void logDebug(const unsigned int nID, const char *szModuleName, const char* szMsg, ...);

	virtual void setLogFile(const char* szLogFileName);
	virtual void setLogLevel(int logLevel);
private:
	void log(const unsigned int nType, const char *szMsg, const unsigned int nID, const char *szModuleName);
	void log(const unsigned int nType, const char* szMsg, const unsigned int nID, const char *szModuleName, va_list args);

	char m_szLogFileName[MAX_PATH];
	int m_nLogLevel;
	char m_szBuffer[MAX_LOG_SIZE];
};
