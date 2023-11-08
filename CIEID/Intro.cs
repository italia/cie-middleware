using System;
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

            mainForm mainForm = new mainForm(null);
            mainForm.Show();

            Hide();
        }

        private void buttonContinue_Click(object sender, EventArgs e)
        {
            tabControl.SelectedIndex = 1;
            tabControl.Refresh();
        }
    }
}
