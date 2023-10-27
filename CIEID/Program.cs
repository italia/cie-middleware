/*
 * CIE ID, l'applicazione per gestire la CIE
 * Author: Ugo Chirico - http://www.ugochirico.com
 * Data: 10/04/2019
 */


using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Windows.Forms;
using System.Threading;
using System.Runtime.InteropServices;

namespace CIEID
{
    static class Program
    {
        private static Mutex mutex = null;
        private static string LogConfigPrefixLib ="LIB_LOG_LEVEL";
        private static string LogConfigPrefixApp ="APP_LOG_LEVEL";
        // Valori di default per il logger
        public static LogLevel LogLevelApp = Logger.DefaultLogLevel;
        public static LogLevel LogLevelLib = Logger.DefaultLogLevel;


        [DllImport("user32.dll", SetLastError = true)]
        static extern bool SetProcessDPIAware();
        static public Logger Logger;

        /// <summary>
        /// Punto di ingresso principale dell'applicazione.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            const string appName = "CIEID";
            bool createdNew;

            Logger = new Logger(LogLevelApp);
            LogLevel[] LogConfigFile = LoadLogConfigFromFile();
            SetLogConfig(LogConfigFile[0], LogConfigFile[1]);
            Logger.Info("App CIE - logger inizializzato");
            Logger.Error($"Livello di log applicazione: {LogLevelApp}");
            Logger.Error($"Livello di log libreria: {LogLevelLib}");

            mutex = new Mutex(true, appName, out createdNew);

            if (!createdNew)
            {
                //app is already running! Exiting the application
                return;
            }

            if (Environment.OSVersion.Version.Major >= 6)
                SetProcessDPIAware();

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            //Properties.Settings.Default.firstTime = true;
            //Properties.Settings.Default.serialNumber = "";
            //Properties.Settings.Default.cardHolder = "";
            //Properties.Settings.Default.Save();

            if (Properties.Settings.Default.firstTime)
            {
                Application.Run(new Intro());
            }
            else
            {
                Application.Run(new mainForm(args.Length > 0 ? args[0] : null));
            }

        }

        static LogLevel[] LoadLogConfigFromFile()
        {
            LogLevel logLevelApp = Logger.DefaultLogLevel;
            LogLevel logLevelLib = Logger.DefaultLogLevel;
            bool _writeLogConfigFile = false;

            string programDataDir = System.Environment.GetEnvironmentVariable("PROGRAMDATA");
            string configFilePath = $"{programDataDir}\\CIEPKI\\config";
            try {
                Console.WriteLine("Leggo file configurazione log");
                string[] configFileLines = System.IO.File.ReadAllLines(configFilePath);
                foreach (var line in configFileLines) {
                    if (line.Contains(LogConfigPrefixApp))
                    {
                        Console.WriteLine($"Configurazione log applicazione: {line}");
                        string value = line.Split('=')[1];
                        try
                        {
                            int intValue = int.Parse(value);
                            if (Enum.IsDefined(typeof(LogLevel), intValue))
                            {
                                logLevelApp = (LogLevel)intValue;
                            }
                            else
                            {
                                Console.WriteLine($"valore '{intValue}' del livello di log applicazione fuori intervallo - uso default");
                                _writeLogConfigFile = true;
                            }
                        }
                        catch (FormatException e)
                        {
                            Console.WriteLine($"valore '{value}' del livello di log applicazione non valido - uso default");
                            _writeLogConfigFile = true;
                        }
                    }
                    else if (line.Contains(LogConfigPrefixLib))
                    {
                        Console.WriteLine($"Configurazione log libreria: {line}");
                        string value = line.Split('=')[1];
                        try
                        {
                            int intValue = int.Parse(value);
                            if (Enum.IsDefined(typeof(LogLevel), intValue))
                            {
                                logLevelLib = (LogLevel)intValue;
                            }
                            else
                            {
                                Console.WriteLine($"valore '{intValue}' del livello di log libreria fuori intervallo - uso default");
                                _writeLogConfigFile = true;
                            }
                        }
                        catch (FormatException)
                        {
                            Console.WriteLine($"valore '{value}' del livello di log libreria non valido - uso default");
                            _writeLogConfigFile = true;
                        }
                    }
                    else
                    {
                        Console.WriteLine($"Riga di configurazione log non valida: {line}");
                    }
              }
            }

            catch (FileNotFoundException)
            {
                Console.WriteLine($"File configurazione log non trovato, lo creo con valori di default");
                _writeLogConfigFile = true;
            }
            catch (DirectoryNotFoundException)
            {
                Console.WriteLine($"Directory di configurazione log non trovata, la creo");
                try
                {
                    System.IO.Directory.CreateDirectory($"{programDataDir}\\CIEPKI");
                    _writeLogConfigFile = true;
                }
                catch (Exception)
                {
                    Console.WriteLine($"Errore nel creare directory per il file di configurazione dei log");
                    _writeLogConfigFile = false;
                }
            }

            if (_writeLogConfigFile)
            {
                SaveLogConfigToFile(logLevelApp, logLevelLib);
            }

            return new LogLevel[] {logLevelApp, logLevelLib};
        }

        public static void SetLogConfig(LogLevel logLevelApp, LogLevel logLevelLib)
        {
            Logger.Info("SetLogConfig() - Inizia funzione");
            Logger.Debug($"SetLogConfig() - logLevelApp: {logLevelApp}");
            Logger.Debug($"SetLogConfig() - logLevelLib: {logLevelLib}");
            LogLevelApp = logLevelApp;
            LogLevelLib = logLevelLib;
            Logger.Level = LogLevelApp;
        }

        public static void SaveLogConfigToFile()
        {
            Logger.Info("SaveLogConfigToFile() - Inizia funzione");
            SaveLogConfigToFile(LogLevelApp, LogLevelLib);
        }

        static void SaveLogConfigToFile(LogLevel logLevelApp, LogLevel logLevelLib)
        {
            Logger.Info("SaveLogConfigToFile(LogLevel, LogLevel) - Inizia funzione");
            Logger.Debug($"SaveLogConfigToFile(LogLevel, LogLevel) - logLevelApp: {logLevelApp}");
            Logger.Debug($"SaveLogConfigToFile(LogLevel, LogLevel) - logLevelLib: {logLevelLib}");
            string programDataDir = System.Environment.GetEnvironmentVariable("PROGRAMDATA");
            string configFilePath = $"{programDataDir}\\CIEPKI\\config";
            string[] lines =
            {
                $"LIB_LOG_LEVEL={(int)logLevelLib}", $"APP_LOG_LEVEL={(int)logLevelApp}",
            };
            try
            {
                File.WriteAllLines(configFilePath, lines);
            }
            catch (Exception)
            {
                string message = $"Errore nel salvare il file di configurazione dei log";
                Logger.Error(message);
                Console.WriteLine(message);
            }
        }

    }
}
