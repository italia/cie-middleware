/*
 * CIE ID, l'applicazione per gestire la CIE
 * Author: Ugo Chirico - http://www.ugochirico.com
 * Data: 10/04/2019 
 */


using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Threading;

namespace CIEID
{
    static class Program
    {
        private static Mutex mutex = null;

        /// <summary>
        /// Punto di ingresso principale dell'applicazione.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            const string appName = "CIEID";
            bool createdNew;

            mutex = new Mutex(true, appName, out createdNew);

            if (!createdNew)
            {
                //app is already running! Exiting the application  
                return;
            }

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
                Application.Run(new MainForm(args.Length > 0 ? args[0] : null));
            }

        }
    }
}
