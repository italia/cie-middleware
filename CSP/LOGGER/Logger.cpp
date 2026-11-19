#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>
#include <cstdarg>

#ifdef WIN32
#include <Windows.h>
#include <Shlwapi.h>
#define stat _stat
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <pwd.h>
#endif


#include "Logger.h"

using namespace std;
using namespace CieIDLogger;


inline bool config_exists(const string& name) {
	ifstream f(name.c_str());
	return f.good();
}

Logger* Logger::m_Instance = 0;

//Folder path string
//char szLogDir[MAX_PATH];

static const char* level_strings[] = {
 "", "[DEBUG]", "[INFO]", "[ERROR]"
};


Logger::Logger()
{
	char pProcessInfo[MAX_PATH];
	string sConfig;

#ifdef WIN32
	char szModulePath[MAX_PATH];
	SYSTEMTIME  stTime;
	char* buf = nullptr;
	size_t sz = 0, l = 0;

	GetLocalTime(&stTime);
	_dupenv_s(&buf, &sz, "PROGRAMDATA");
	sprintf_s(pbLog, sizeof(pbLog), "%s/CIEPKI/CIEPKI_%04d-%02d-%02d.log", buf, stTime.wYear, stTime.wMonth, stTime.wDay);
#else
	time_t now = time(NULL);
	struct tm tm_now;
	localtime_r(&now, &tm_now);
	const char *home = getenv("HOME");
	if (!home) home = "/tmp";
	snprintf(pbLog, sizeof(pbLog), "%s/Library/Logs/CIEPKI/CIEPKI_%04d-%02d-%02d.log",
		home, tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday);
#endif

	int log_level = getLogConfig();

	if (log_level == LOG_STATUS_DISABLED) {
		disableLog();
	}
	else {

		m_File.open(pbLog, ios::out | ios::app);
		m_File << endl << "-----------------------------------------------------------------" << endl << endl;

		m_LogLevel = static_cast<LogLevel>(log_level);
		m_LogStatus = LOG_STATUS_ENABLED;

#ifdef WIN32
		l = GetModuleFileNameA(NULL, szModulePath, sizeof(szModulePath) - 1);
		szModulePath[l] = 0;
		sprintf_s(pProcessInfo, sizeof(pProcessInfo), "Process: '%s'", szModulePath);
#else
		snprintf(pProcessInfo, sizeof(pProcessInfo), "Process: pid %d", getpid());
#endif
		m_File << pProcessInfo << endl;
		m_File.flush();
		m_File.close();
	}

	m_LogType = FILE_LOG;

#ifdef WIN32
	InitializeCriticalSection(&m_Mutex);
#else
	int ret = 0;
	ret = pthread_mutexattr_init(&m_Attr);
	if (ret != 0)
	{
		printf("Logger::Logger() -- Mutex attr init failed!!\n");
		exit(0);
	}
	ret = pthread_mutexattr_settype(&m_Attr, PTHREAD_MUTEX_ERRORCHECK);
	if (ret != 0)
	{
		printf("Logger::Logger() -- Mutex attribute not initialize!!\n");
		exit(0);
	}
	ret = pthread_mutex_init(&m_Mutex, &m_Attr);
	if (ret != 0)
	{
		printf("Logger::Logger() -- Mutex not initialize!!\n");
		exit(0);
	}
#endif
}

Logger::~Logger()
{
	m_File.close();
#ifdef WIN32
	DeleteCriticalSection(&m_Mutex);
#else
	pthread_mutexattr_destroy(&m_Attr);
	pthread_mutex_destroy(&m_Mutex);
#endif
}

Logger* Logger::getInstance() throw ()
{
	if (m_Instance == 0)
	{
		m_Instance = new Logger();
	}

	int log_level = m_Instance->getLogConfig();

	if (log_level == LOG_STATUS_DISABLED) {
		m_Instance->disableLog();
	}
	else if(log_level >= 0 && log_level < 3){
		m_Instance->enableFileLogging();
		m_Instance->enableLog();
		m_Instance->updateLogLevel(static_cast<LogLevel>(log_level));
	}

	return m_Instance;
}


void Logger::writeConfigFile(string& filePath, string& sConfig) throw() {
	m_ConfigFile.open(filePath, ios::out);
	m_ConfigFile << sConfig;
	m_ConfigFile.close();
}

#ifdef WIN32
BOOL IsElevated() {
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}
#else
static bool IsElevated() {
	return getuid() == 0;
}
#endif


int Logger::getLogConfig() throw() {
	char pbConfig[MAX_PATH];
	string sConfig;
	int log_level;
	struct stat result;
	char pbPathConfig[MAX_PATH];

#ifdef WIN32
	char* buf = nullptr;
	size_t sz = 0;
	_dupenv_s(&buf, &sz, "PROGRAMDATA");
	snprintf(pbPathConfig, sizeof(pbPathConfig), "%s/CIEPKI", buf);
	snprintf(pbConfig, sizeof(pbConfig), "%s/config", pbPathConfig);
#else
	const char *home = getenv("HOME");
	if (!home) home = "/tmp";
	snprintf(pbPathConfig, sizeof(pbPathConfig), "%s/Library/Application Support/CIEPKI", home);
	snprintf(pbConfig, sizeof(pbConfig), "%s/config", pbPathConfig);
#endif

	if (IsElevated()) {
		return LOG_STATUS_DISABLED;
	}

#ifdef WIN32
	if (!PathFileExists(pbPathConfig)) {
		CreateDirectory(pbPathConfig, nullptr);
	}
#else
	struct stat st;
	if (stat(pbPathConfig, &st) != 0) {
		string logDir = string(home) + "/Library/Logs/CIEPKI";
		mkdir(logDir.c_str(), 0755);
		mkdir(pbPathConfig, 0755);
	}
#endif

	if (!config_exists(pbConfig)) {
		sConfig = "LIB_LOG_LEVEL=2";
		string configPath(pbConfig);
		writeConfigFile(configPath, sConfig);
		t64configTime = 0;
	}
	
	volatile int stat_res = stat(pbConfig, &result);

	if (stat_res == 0) {
		if (t64configTime < result.st_mtime) {
			t64configTime = result.st_mtime;
			m_ConfigFile.open(pbConfig, ios::in);
			m_ConfigFile >> sConfig;
			m_ConfigFile.close();

			sscanf(sConfig.data(), "LIB_LOG_LEVEL=%d", &log_level);

			if (log_level < 0 && log_level > 5) {
				log_level = 0;
				sConfig = "LIB_LOG_LEVEL=2";
				string configPath2(pbConfig);
				writeConfigFile(configPath2, sConfig);
			}

			m_LogLevel = static_cast<LogLevel>(log_level);
		}
	}

	return m_LogLevel;
}

void Logger::lock()
{
#ifdef WIN32
	EnterCriticalSection(&m_Mutex);
#else
	pthread_mutex_lock(&m_Mutex);
#endif
}

void Logger::unlock()
{
#ifdef WIN32
	LeaveCriticalSection(&m_Mutex);
#else
	pthread_mutex_unlock(&m_Mutex);
#endif
}

void Logger::logIntoFile(string& data)
{
	lock();
	m_File << getCurrentTime() << "  " << data << endl;
	unlock();
}

void Logger::logOnConsole(string& data)
{
	cout << getCurrentTime() << "  " << data << endl;
}

string Logger::getCurrentTime()
{
	char pbtDate[0x1000];
#ifdef WIN32
	SYSTEMTIME  stTime;
	GetLocalTime(&stTime);
	sprintf_s(pbtDate, sizeof(pbtDate), "%04d-%02d-%02d %02d:%02d:%02d:%03d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct tm tm_now;
	localtime_r(&tv.tv_sec, &tm_now);
	snprintf(pbtDate, sizeof(pbtDate), "%04d-%02d-%02d %02d:%02d:%02d:%03d",
		tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday,
		tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, (int)(tv.tv_usec / 1000));
#endif
	return pbtDate;
}


void Logger::log_log(ostream& out, LogLevel level, const char* text) throw() {
	if (m_LogStatus == LOG_STATUS_ENABLED) {

		if (level < m_LogLevel) {
			return;
		}

		string data;
		data.append(level_strings[level]);
		data.append(" ");
		data.append(text);

		lock();
		m_File.open(pbLog, ios::out | ios::app);
		m_File << getCurrentTime() << "  " << data << endl;
		m_File.flush();
		m_File.close();
		unlock();

	}
}

// Interface for Debug Log
void Logger::debug(const char* fmt, ...) throw()
{
	char buffer[1024];
	va_list args;
	va_start(args, fmt);

	vsprintf(buffer, fmt, args);
	va_end(args);

	switch (m_LogType)
	{
	case FILE_LOG:
		log_log(m_File, LOG_LEVEL_DEBUG, buffer);
		break;
	case CONSOLE:
		log_log(cout, LOG_LEVEL_DEBUG, buffer);
	default:
		break;
	}
}

void Logger::debug(string& text) throw()
{
	debug(text.data());
}

void Logger::debug(ostringstream& stream) throw()
{
	string text = stream.str();
	debug(text.data());
}


// Interface for Info Log
void Logger::info(const char* fmt, ...) throw()
{
	char buffer[1024];
	va_list args;
	va_start(args, fmt);

	vsprintf(buffer, fmt, args);
	va_end(args);

	switch (m_LogType)
	{
	case FILE_LOG:
		log_log(m_File, LOG_LEVEL_INFO, buffer);
		break;
	case CONSOLE:
		log_log(cout, LOG_LEVEL_INFO, buffer);
	default:
		break;
	}
}

void Logger::info(string& text) throw()
{
	info(text.data());
}

void Logger::info(ostringstream& stream) throw()
{
	string text = stream.str();
	info(text.data());
}


// Interface for Error Log
int Logger::error(const char* fmt, ...) throw()
{
	char buffer[1024];
	va_list args;
	va_start(args, fmt);

	vsprintf(buffer, fmt, args);
	va_end(args);

	switch (m_LogType)
	{
	case FILE_LOG:
		log_log(m_File, LOG_LEVEL_ERROR, buffer);
		break;
	case CONSOLE:
		log_log(cout, LOG_LEVEL_ERROR, buffer);
	default:
		break;
	}

	return -1;
}

int Logger::error(string& text) throw()
{
	return error(text.data());
}

int Logger::error(ostringstream& stream) throw()
{
	string text = stream.str();
	return error(text.data());
}


// Interface for Buffer Log 
void Logger::buffer(uint8_t* buff, size_t buff_size) throw()
{
	size_t index = 0;

	if (m_LogLevel == LOG_LEVEL_DEBUG) {
		switch (m_LogType)
		{
		case FILE_LOG:
			print_bytes(m_File, buff, buff_size, true);
			break;
		case CONSOLE:
			print_bytes(cout, buff, buff_size, true);
		default:
			break;
		}
	}
}

void Logger::print_bytes(ostream& out, uint8_t* data, size_t dataLen, bool format) {
	size_t index = 0;


	lock();
	m_File.open(pbLog, ios::out | ios::app);

	m_File << setfill('0');
	m_File << endl;

	m_File << "0x" << hex << setw(8) << index << "\t";

	for (size_t index = 0; index < dataLen; index++) {

		if (index) {
			if ((index % 16) == 0) {
				m_File << "\n0x" << hex << setw(8) << index << "\t";
			}
			else if ((index % 8) == 0) {
				m_File << " -  ";
			}
		}

		m_File << hex << setw(2) << (int)data[index] << " ";
	}
	m_File << endl << endl;


	m_File.close();
	unlock();
}

#if 0
void Logger::buffer(string& text) throw()
{
	buffer(text.data());
}

void Logger::buffer(ostringstream& stream) throw()
{
	string text = stream.str();
	buffer(text.data());
}
#endif

void Logger::updateLogLevel(LogLevel logLevel)
{
	m_LogLevel = logLevel;
}

void Logger::enableLog()
{
	m_LogStatus = LOG_STATUS_ENABLED;
}

void Logger::disableLog()
{
	m_LogStatus = LOG_STATUS_DISABLED;
}

void Logger::updateLogType(LogType logType)
{
	m_LogType = logType;
}

void Logger::enableConsoleLogging()
{
	m_LogType = CONSOLE;
}

void Logger::enableFileLogging()
{
	m_LogType = FILE_LOG;
}

