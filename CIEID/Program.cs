using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace CIEID
{
    static class Program
    {
        /// <summary>
        /// Punto di ingresso principale dell'applicazione.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            //Properties.Settings.Default.serialNumber = "";
            //Properties.Settings.Default.cardHolder = "";
            //Properties.Settings.Default.Save();

            if (Properties.Settings.Default.firstTime)
            {
                Properties.Settings.Default.firstTime = false;
                Properties.Settings.Default.Save(); // Saves settings in application configuration file
                Application.Run(new Intro());
            }
            else
            {
                Application.Run(new MainForm());
            }

        }
    }
}
