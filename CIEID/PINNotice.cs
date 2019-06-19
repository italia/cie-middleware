using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CIEID
{
    public partial class PINNotice : Form
    {
        private int timerCountdown = 5;

        public PINNotice()
        {
            InitializeComponent();
            buttonOK.Text = "OK (" + timerCountdown + ")";
            countdownTimer.Start();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {

        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            countdownTimer.Stop();
            Hide();
                        
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            timerCountdown--;
            if (timerCountdown < 0)
            {
                buttonOK_Click(sender, e);
            }
            else
            {
                buttonOK.Text = "OK (" + timerCountdown + ")";
            }
            
        }
    }
}
