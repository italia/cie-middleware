


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Threading;

namespace CIEID
{
    public partial class MainForm : Form
    {
        public const int CKR_OK = 0x00000000;
        public const int CKR_CANCEL = 0x00000001;
        public const int CKR_TOKEN_NOT_PRESENT = 0x000000E0;
        public const int CKR_TOKEN_NOT_RECOGNIZED = 0x000000E1;
        public const int CKR_DEVICE_ERROR = 0x00000030;
        public const int CKR_GENERAL_ERROR = 0x00000005;
        public const int CKR_PIN_INCORRECT = 0x000000A0;
        public const int CKR_PIN_INVALID = 0x000000A1;
        public const int CKR_PIN_LEN_RANGE = 0x000000A2;

        /* CKR_PIN_EXPIRED and CKR_PIN_LOCKED are new for v2.0 */
        public const int CKR_PIN_EXPIRED = 0x000000A3;
        public const int CKR_PIN_LOCKED = 0x000000A4;

        public const int ENROLLED = 1;
        public const int NOT_ENROLLED = 0;

        delegate long ProgressCallback(int progress, string message);
        delegate long CompletedCallback(string pan, string name);

        [DllImport("ciepki.dll")]
        static extern long VerificaCIEAbilitata();

        [DllImport("ciepki.dll")]
        static extern int DisabilitaCIE();

        [DllImport("ciepki.dll", CallingConvention = CallingConvention.StdCall)]
        static extern long AbbinaCIE(string szPAN, string szPIN, int[] attempts, ProgressCallback progressCallBack, CompletedCallback completedCallBack);

        public MainForm()
        {
            InitializeComponent();

            //for (int i = 1; i < 9; i++)
            //{
            //    TextBox txtField = (TextBox)FindControlByTag(Controls, "" + i);

            //    txtField.Paint += new System.Windows.Forms.PaintEventHandler(this.TextBox_Paint);

            //    txtField.BorderStyle = BorderStyle.None;
            //}

            selectHome();
        }

        void TextBox_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics, ((Control)sender).DisplayRectangle, Color.LightGray, ButtonBorderStyle.Solid);
        }

        long Progress(int progress, string message)
        {
            this.Invoke((MethodInvoker)delegate
            {
                progressBar.Value = progress;
                labelProgressMessage.Text = message;
            });

            return 0;
        }

        long Completed(string pan, string name)
        {
            Properties.Settings.Default.serialNumber = pan;
            Properties.Settings.Default.cardHolder = name;
            Properties.Settings.Default.Save();

            return 0;
        }

        private void textBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar >= '0' && e.KeyChar <= '9')
            {
                TextBox textBox = (TextBox)sender;

                int tag = Int16.Parse((String)textBox.Tag);

                if (tag < 8)
                {
                    Control nextTextBox = FindControlByTag(this.Controls, "" + (tag + 1));
                    nextTextBox.Focus();
                }
            }
            else if (e.KeyChar == 8) // backspace
            {
                TextBox textBox = (TextBox)sender;

                int tag = Int16.Parse((String)textBox.Tag);


                if (tag > 1)
                {
                    Control previousTextBox = FindControlByTag(this.Controls, "" + (tag - 1));
                    previousTextBox.Focus();

                }
            }
            else if (e.KeyChar == 13) // enter
            {
                TextBox textBox = (TextBox)sender;

                int tag = Int16.Parse((String)textBox.Tag);

                if (tag == 8)
                    buttonAbbina_Click(sender, e);
            }
        }

        private Control FindControlByTag(Control.ControlCollection controls, object tag)
        {
            Control control = null;
            foreach (Control c in controls)
            {
                if (tag.Equals(c.Tag))
                {
                    control = c;
                }
                else
                {
                    if (c.HasChildren)
                        control = FindControlByTag(c.Controls, tag); //Recursively check all children controls as well; ie groupboxes or tabpages
                }

                if (control != null)
                    return control;
            }

            return null;
        }

        private void label5_Click(object sender, EventArgs e)
        {

        }

        private void labelSerialNumber_Click(object sender, EventArgs e)
        {

        }

        private void buttonHome_Click(object sender, EventArgs e)
        {
            selectHome();
        }

        private void selectHome()
        {
            if (Properties.Settings.Default.serialNumber.Equals(""))
            {
                tabControlMain.SelectedIndex = 0;
            }
            else
            {
                tabControlMain.SelectedIndex = 1;
                labelSerialNumber.Text = Properties.Settings.Default.serialNumber;
                labelCardHolder.Text = Properties.Settings.Default.cardHolder;
                tabControlMain.SelectedIndex = 1;
            }

            buttonHome.BackColor = Color.LightGray;
            buttonChangePIN.BackColor = Color.Transparent;
            buttonUnlock.BackColor = Color.Transparent;
            buttonTutorial.BackColor = Color.Transparent;
            buttonInfo.BackColor = Color.Transparent;
            buttonHelp.BackColor = Color.Transparent;
        }

        private void selectAbbinaProgress()
        {
            tabControlMain.SelectedIndex = 2;
            progressBar.Value = 0;
            progressBar.Maximum = 100;
        }

        private void buttonAbbina_Click(object sender, EventArgs e)
        {
            string pin = "";

            int i;
            for (i = 1; i < 9; i++)
            {
                TextBox txtField = (TextBox)FindControlByTag(Controls, "" + i);

                pin += txtField.Text;
            }

            if (pin.Length != 8)
            {
                MessageBox.Show("Il PIN deve essere composto da 8 numeri", "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            char c = pin[0];

            i = 1;
            for (i = 1; i < pin.Length && (c >= '0' && c <= '9'); i++)
            {
                c = pin[i];
            }

            if (i < pin.Length || !(c >= '0' && c <= '9'))
            {
                MessageBox.Show("Il PIN deve essere composto da 8 numeri", "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            for (i = 1; i < 9; i++)
            {
                TextBox txtField = (TextBox)FindControlByTag(Controls, "" + i);
                txtField.Text = "";
            }

            ((Control)sender).Enabled = false;

            tabControlMain.SelectedIndex = 2;

            ThreadStart processTaskThread = delegate { abbina(sender, pin); };

            new Thread(processTaskThread).Start();

        }

        private void abbina(object sender, string pin)
        {
            try
            {
                int[] attempts = new int[1];

                long ret = AbbinaCIE(null, pin, attempts, new ProgressCallback(Progress), new CompletedCallback(Completed));

                this.Invoke((MethodInvoker)delegate
                {
                    ((Control)sender).Enabled = false;

                    switch (ret)
                    {
                        case CKR_TOKEN_NOT_RECOGNIZED:
                            MessageBox.Show("CIE non presente sul lettore", "Abilitazione CIE", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            selectHome();
                            //[self showHomeFirstPage];
                            break;

                        case CKR_TOKEN_NOT_PRESENT:
                            MessageBox.Show("CIE non presente sul lettore", "Abilitazione CIE", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            selectHome();
                            break;

                        case CKR_PIN_INCORRECT:
                            MessageBox.Show(String.Format("Il PIN digitato è errato. rimangono %d tentativi", attempts[0]), "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            selectHome();
                            break;

                        case CKR_PIN_LOCKED:
                            MessageBox.Show("Munisciti del codice PUK e utilizza la funzione di sblocco carta per abilitarla", "Carta bloccata", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            selectHome();
                            break;

                        case CKR_GENERAL_ERROR:
                            MessageBox.Show("Errore inaspettato durante la comunicazione con la smart card", "Errore inaspettato", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            selectHome();
                            break;

                        case CKR_OK:
                            MessageBox.Show("L'abilitazione della CIE è avvennuta con successo", "CIE abilitata", MessageBoxButtons.OK, MessageBoxIcon.Information);
                            labelSerialNumber.Text = Properties.Settings.Default.serialNumber;
                            labelCardHolder.Text = Properties.Settings.Default.cardHolder;
                            tabControlMain.SelectedIndex = 1;
                            break;
                    }
                });
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        private void buttonDeleteCIE_Click(object sender, EventArgs e)
        {
            int ret = DisabilitaCIE();

            switch (ret)
            {
                case CKR_OK:
                    tabControlMain.SelectedIndex = 0;
                    MessageBox.Show("CIE disabilitata con successo", "CIE disabilitata", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    labelSerialNumber.Text = Properties.Settings.Default.serialNumber;
                    labelCardHolder.Text = Properties.Settings.Default.cardHolder;                    
                    Properties.Settings.Default.serialNumber = "";
                    Properties.Settings.Default.cardHolder = "";
                    Properties.Settings.Default.Save();
                    break;

                case CKR_TOKEN_NOT_PRESENT:
                    MessageBox.Show("CIE non presente sul lettore", "Disabilitazione CIE", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    break;

                default:
                    MessageBox.Show("Impossibile disabilitare la CIE", "CIE non disabilitata", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    break;
            }
        }
    }
//long ret = VerificaCIEAbilitata();

//            switch (ret)
//            {
//                case CKR_DEVICE_ERROR:
//                    break;

//                case CKR_TOKEN_NOT_PRESENT:
//                    break;

//                case CKR_GENERAL_ERROR:
//                    break;

//                case ENROLLED:
//                    break;

//                case NOT_ENROLLED:
//                    abbina();
//                    break;

//            }
}
