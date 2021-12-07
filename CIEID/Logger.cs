using System;
using System.IO;

namespace CIEID
{
    public enum LogLevel
    {
        NONE = 0,
        DEBUG = 1,
        INFO = 2,
        ERROR = 3,
    };

    public class Logger
    {
        public static LogLevel DefaultLogLevel = LogLevel.NONE;
        private LogLevel _level;
        public LogLevel Level
        {
            get => _level;
            set => _level = value;
        }

        // The default argument value should be `Logger.DefaultLogLevel`
        // rather than an explicit LogLevel enum value.
        public Logger(LogLevel logLevel = LogLevel.NONE)
        {
            Info($"Init della classe Logger, livello di log {logLevel}");
            Level = logLevel;
        }

        // Public functions
        public void Debug(string message)
        {
            Log($"[D] {message}", LogLevel.DEBUG);
        }

        public void Info(string message)
        {
            Log($"[I] {message}", LogLevel.INFO);
        }

        public void Error(string message)
        {
            Log($"[E] {message}", LogLevel.ERROR);
        }

        public void Log(string message, LogLevel messageLevel)
        {
            if (Level > 0 && Level <= messageLevel)
            {
                Write(message);
            }
        }

        private void Write(string message)
        {
            string programDataDir = System.Environment.GetEnvironmentVariable("PROGRAMDATA");
            string currentDate = DateTime.Now.ToString("yyyy-MM-dd");
            string timestamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff");
            string logFilePath = $"{programDataDir}\\CIEPKI\\CIEID_{currentDate}.log";
            try
            {
                StreamWriter streamWriter = new StreamWriter(logFilePath, append: true);
                streamWriter.WriteLineAsync($"{timestamp}  {message}");
                streamWriter.Close();
            }
            catch(Exception)
            {
                Console.WriteLine($"{timestamp}  {message}");
            }
        }
    }
}
