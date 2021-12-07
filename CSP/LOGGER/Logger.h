#ifndef _LOGGER_H_
#define _LOGGER_H_

// C++ Header File(s)
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#ifdef WIN32
// Win Socket Header File(s)
#include <Windows.h>
#include <process.h>
#else
// POSIX Socket Header File(s)
#include <errno.h>
#include <pthread.h>
#endif


namespace CieIDLogger
{
// Direct Interface for logging into log file or console using MACRO(s)
#define LOG_DEBUG(...)			Logger::getInstance()->debug(__VA_ARGS__)
#define LOG_INFO(...)			Logger::getInstance()->info( __VA_ARGS__)
#define LOG_ERROR(...)			Logger::getInstance()->error( __VA_ARGS__)
#define LOG_BUFFER(data, len)	Logger::getInstance()->buffer(data, len);

	typedef enum LOG_STATUS
	{
		LOG_STATUS_DISABLED = 0,
		LOG_STATUS_ENABLED = 1
	}LogStatus;

	// enum for LOG_LEVEL
	typedef enum LOG_LEVEL
	{
		LOG_LEVEL_DEBUG = 1,
		LOG_LEVEL_INFO = 2,
		LOG_LEVEL_ERROR = 3
	}LogLevel;

	// enum for LOG_TYPE
	typedef enum LOG_TYPE
	{
		NO_LOG = 1,
		CONSOLE = 2,
		FILE_LOG = 3,
	}LogType;

	class Logger
	{
	public:
		static Logger* getInstance() throw ();

		// Interfaces to control log levels
		void updateLogLevel(LogLevel logLevel);
		void enableLog();  // Enable all log levels
		void disableLog(); // Disable all log levels, except error and alarm

		// Interfaces to control log Types
		void updateLogType(LogType logType);

		void enableConsoleLogging();
		void enableFileLogging();


		// Interface for Debug log 
		void debug(const char* fmt, ...) throw();
		void debug(std::string& text) throw();
		void debug(std::ostringstream& stream) throw();

		// Interface for Info Log 
		void info(const char* fmt, ...) throw();
		void info(std::string& text) throw();
		void info(std::ostringstream& stream) throw();
		
		// Interface for Error Log 
		int error(const char* fmt, ...) throw();
		int error(std::string& text) throw();
		int error(std::ostringstream& stream) throw();
		
		// Interface for Buffer Log 
		void buffer(uint8_t* buff, size_t buff_size) throw();
		//void buffer(std::string& text) throw();
		//void buffer(std::ostringstream& stream) throw();

	protected:
		Logger();
		~Logger();

		// Wrapper function for lock/unlock
		// For Extensible feature, lock and unlock should be in protected
		void lock();
		void unlock();

		std::string getCurrentTime();
		int getLogConfig()throw();

	private:
		void logIntoFile(std::string& data);
		void logOnConsole(std::string& data);
		void log_log(std::ostream& out, LogLevel level, const char* text)throw();
		void writeConfigFile(std::string& filePath, std::string& sConfig)throw();
		void operator=(const Logger& obj) {}

		void print_bytes(std::ostream& out, uint8_t* data, size_t dataLen, bool format);

	private:
		static Logger* m_Instance;
		std::ofstream m_File;
		std::fstream m_ConfigFile;
		char pbLog[MAX_PATH];
		char pbConfig[MAX_PATH];
		__time64_t t64configTime;

#ifdef	WIN32
		CRITICAL_SECTION        m_Mutex;
#else
		pthread_mutexattr_t     m_Attr;
		pthread_mutex_t         m_Mutex;
#endif

		LogLevel                m_LogLevel;
		LogType                 m_LogType;
		LogStatus				m_LogStatus;
	};

} // End of namespace

#endif // End of _LOGGER_H_

