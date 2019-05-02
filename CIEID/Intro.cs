/*
 * CIE ID, l'applicazione per gestire la CIE
 * Author: Ugo Chirico - http://www.ugochirico.com
 * Data: 10/04/2019 
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CIEID
{
    public partial class Intro : Form
    {
        public Intro()
        {
            InitializeComponent();
        }


        private void buttonStart_Click(object sender, EventArgs e)
        {
            if(checkBoxNoMore.Checked)
            {
                Properties.Settings.Default.firstTime = false;
                Properties.Settings.Default.Save(); // Saves settings in application configuration file                
            }

            MainForm mainForm = new MainForm(null);
            mainForm.Show();

            Hide();
            //Close();

        }

        private void buttonContine_Click(object sender, EventArgs e)
        {
            tabControl.SelectedIndex = 1;
            tabControl.Refresh();
        }
    }
}
