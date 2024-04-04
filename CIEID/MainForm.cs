using CIEID.Controls;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing.Text;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace CIEID
{
    public partial class mainForm : Form
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
        public const int CARD_ALREADY_ENABLED = 0x000000F0;
        public const int CARD_PAN_MISMATCH = 0x000000F1;
        public const UInt32 INVALID_FILE_TYPE = 0x84000005;

        /* CKR_PIN_EXPIRED and CKR_PIN_LOCKED are new for v2.0 */
        public const int CKR_PIN_EXPIRED = 0x000000A3;
        public const int CKR_PIN_LOCKED = 0x000000A4;

        public const int ENROLLED = 1;
        public const int NOT_ENROLLED = 0;

        private PdfPreview pdfPreview = null;
        private enum OperationSelectedState
        {
            NO_OP = 0,
            PADES_SIGNATURE = 1,
            CADES_SIGNATURE = 2,
            VERIFY = 3,
        }

        private int signFontSize = 120;

        private OperationSelectedState signOp = OperationSelectedState.NO_OP;

        delegate long ProgressCallback(int progress, string message);
        delegate long CompletedCallback(string pan, string name, string ef_seriale);
        delegate long SignCompletedCallback(int retValue);

        [DllImport("ciepki.dll")]
        static extern int VerificaCIEAbilitata(string pan);

        [DllImport("ciepki.dll")]
        static extern int DisabilitaCIE(string pan);

        [DllImport("ciepki.dll", CallingConvention = CallingConvention.StdCall)]
        static extern int isCIEEnrolled(StringBuilder pan);

        [DllImport("ciepki.dll", CallingConvention = CallingConvention.StdCall)]
        static extern int AbbinaCIE(string szPAN, string szPIN, int[] attempts, ProgressCallback progressCallBack, CompletedCallback completedCallBack);

        [DllImport("ciepki.dll", CallingConvention = CallingConvention.StdCall)]
        static extern int ChangePIN(string szPIN, string szNewPIN, int[] attempts, ProgressCallback progressCallBack);

        [DllImport("ciepki.dll", CallingConvention = CallingConvention.StdCall)]
        static extern int UnlockPIN(string szPUK, string szNewPIN, int[] attempts, ProgressCallback progressCallBack);

        [DllImport("ciepki.dll", CallingConvention = CallingConvention.StdCall)]
        static extern int firmaConCIE(string inFilePath, string type, string pin, string pan, int page, float x, float y, float w, float h, string imagePathFile, string outFilePath, ProgressCallback progressCallBack, SignCompletedCallback signCompletedCallBack);

        [DllImport("ciepki.dll", CallingConvention = CallingConvention.StdCall)]
        static extern int estraiP7m(string inFilePath, string outFilePath);

        private bool shouldSignWithoutCIEPairing = false;

        private String PANForOneShotSigning = String.Empty;

        private CieCollection cieColl;

        internal CieCollection CieColl { get => cieColl; set => cieColl = value; }

        private CarouselControl carouselControl;

        private Logger Logger;

        public mainForm(string arg)
        {
            Logger = Program.Logger;
            Logger.Info("Inizializzo form principale");

            InitializeComponent();

            if ("unlock".Equals(arg))
            {
                SelectUnlockPIN();
            }
            else if ("changepin".Equals(arg))
            {
                SelectChangePIN();
            }
            else
            {
                SelectHome();
            }
        }

        void TextBox_Paint(object sender, PaintEventArgs e)
        {
            ControlPaint.DrawBorder(e.Graphics, ((Control)sender).DisplayRectangle, Color.LightGray, ButtonBorderStyle.Solid);
        }

        long PairingProgress(int progress, string message)
        {
            this.Invoke((MethodInvoker)delegate
            {
                progressBar.Value = progress;
                labelProgressMessage.Text = message;
            });

            return 0;
        }

        long SignProgress(int progress, string message)
        {
            this.Invoke((MethodInvoker)delegate
            {
                signProgressBar.Value = progress;
            });

            return 0;
        }

        private string getSignImagePath(string efSeriale)
        {
            Logger.Info("getSignImagePath() - Inizia funzione");
            string appdataPath = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
            return string.Format("{0}\\IPZS\\{1}_default.png", appdataPath, efSeriale);
        }

        long SignCompleted(int retValue)
        {
            Logger.Info("SignCompleted() - Inizia funzione");
            this.Invoke((MethodInvoker)delegate
            {
                if (retValue != 0)
                {
                    documentSuccessfullySignedLabel.Text = "Si è verificato un errore";
                    Logger.Debug(documentSuccessfullySignedLabel.Text);
                    digitalSignatureCompletedPictureBox.Image = Properties.Resources.cross;
                    digitalSignatureCompletedPictureBox.SizeMode = PictureBoxSizeMode.StretchImage;
                }
                else
                {
                    documentSuccessfullySignedLabel.Text = "File firmato con successo";
                    Logger.Debug(documentSuccessfullySignedLabel.Text);
                    digitalSignatureCompletedPictureBox.Image = Properties.Resources.check;
                    digitalSignatureCompletedPictureBox.SizeMode = PictureBoxSizeMode.StretchImage;
                }

                signProgressBar.Hide();
                typePINLabel.Hide();
                placeCIEOnReaderLabel.Hide();
                cancelSigningOperationPINPanelButton.Hide();
                signButton.Hide();
                documentSuccessfullySignedLabel.Show();
                documentSuccessfullySignedLabel.Update();
                digitalSignatureCompletedPictureBox.Show();
                digitalSignatureCompletedPictureBox.Update();
                closeButton.Show();
                closeButton.Update();
            });

            return 0;
        }

        long PairingCompleted(string pan, string name, string efSeriale)
        {
            Logger.Info("PairingCompleted() - Inizia funzione");

            string defaultSignImagePath = getSignImagePath(efSeriale);
            CieColl.addCie(pan, new CieModel(efSeriale, name, pan));

            if (shouldSignWithoutCIEPairing)
                PANForOneShotSigning = pan;

            Properties.Settings.Default.cieList = JsonConvert.SerializeObject(CieColl.MyDictionary);
            Properties.Settings.Default.Save();

            Console.WriteLine("Immagine firma salvata in: {0}", defaultSignImagePath);

            TextInfo nameInfo = new CultureInfo("it-IT", false).TextInfo;
            DrawText(nameInfo.ToTitleCase(name.ToLower()), Color.Black, defaultSignImagePath);

            Console.WriteLine("CIE abbinate dopo aggiunta: " + Properties.Settings.Default.cieList);

            return 0;
        }

        long ChangePINProgress(int progress, string message)
        {
            this.Invoke((MethodInvoker)delegate
            {
                progressBarChangePIN.Value = progress;
                progressLabelChangePIN.Text = message;
            });

            return 0;
        }

        long UnlockPINProgress(int progress, string message)
        {
            this.Invoke((MethodInvoker)delegate
            {
                progressBarUnlockPIN.Value = progress;
                progressLabelUnlockPIN.Text = message;
            });

            return 0;
        }

        private void PINDigit_TextBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            TextBox textBox = (TextBox)sender;
            int tag = Int16.Parse((String)textBox.Tag);

            if (e.KeyChar >= '0' && e.KeyChar <= '9')
            {
                if (tag < 8)
                {
                    Control nextTextBox = FindControlByTag(this.Controls, "" + (tag + 1));
                    nextTextBox.Focus();
                }
            }

            else if (e.KeyChar == 8) // backspace
            {
                if (tag > 1)
                {
                    Control previousTextBox = FindControlByTag(this.Controls, "" + (tag - 1));
                    previousTextBox.Focus();
                }
            }

            else if (e.KeyChar == 13) // enter
            {
                if (tag == 8)
                    PairButton_Click(sender, e);
            }

            else
                e.Handled = true;

            if (textBox.Text != String.Empty)
                textBox.Text = String.Empty;

        }

        private void TextBoxSignPin_KeyPress(object sender, KeyPressEventArgs e)
        {
            byte numPINDigits = (byte)((shouldSignWithoutCIEPairing) ? 4 : 0);

            TextBox textBox = (TextBox)sender;
            int tag = Int16.Parse((String)textBox.Tag);

            if (e.KeyChar >= '0' && e.KeyChar <= '9')
            {
                if (tag < 12 + numPINDigits)
                {
                    Control nextTextBox = FindControlByTag(this.Controls, "" + (tag + 1));
                    nextTextBox.Focus();
                }
                else
                {
                    signButton.Enabled = true;
                }
            }

            else if (e.KeyChar == 8) // Backspace
            {
                if (tag > 9)
                {
                    Control previousTextBox = FindControlByTag(this.Controls, "" + (tag - 1));
                    previousTextBox.Focus();
                }
            }

            else if (e.KeyChar == 13) // Enter
            {
                if (tag == 12 + numPINDigits)
                {
                    SignButton_Click(sender, e);
                }
            }

            else
                e.Handled = true;

            if (textBox.Text != String.Empty)
                textBox.Text = String.Empty;
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


        private void ConfigureHomeButtons(CieCollection cieColl)
        {
            Logger.Info("configureHomeButtons() - Inizia funzione");

            if (cieColl.MyDictionary.Count > 1)
            {
                int size_x = tableLayoutPanel1.Size.Width;
                int size_y = tableLayoutPanel1.Size.Height;
                int remaining_space = (size_x - (3 * (deleteCIEButton.Width))) / 4;

                removeAllButton.Location = new Point(remaining_space, tableLayoutPanel1.Location.Y + size_y + 10);
                deleteCIEButton.Location = new Point(remaining_space + removeAllButton.Width + removeAllButton.Location.X, removeAllButton.Location.Y);
                addButton.Location = new Point(remaining_space + removeAllButton.Width + deleteCIEButton.Location.X, removeAllButton.Location.Y);
                removeAllButton.Visible = true;
            }
            else
            {
                int size_x = tableLayoutPanel1.Size.Width;
                int size_y = tableLayoutPanel1.Size.Height;
                int remaining_space = (size_x - (2 * (deleteCIEButton.Width))) / 3;

                removeAllButton.Visible = false;
                deleteCIEButton.Location = new Point(remaining_space, tableLayoutPanel1.Location.Y + size_y + 10);
                addButton.Location = new Point(2 * remaining_space + deleteCIEButton.Width, deleteCIEButton.Location.Y);
            }

            if (CieColl.MyDictionary.Count >= 1)
            {
                int size_x = CIEPairingTabPage.Size.Width;
                int size_y = CIEPairingTabPage.Size.Height;
                int remaining_space = (size_x - 2 * ((pairButton.Width))) / 3;

                int height = size_y - pairButton.Height - 30;

                cancelButton.Location = new Point(remaining_space, height);
                pairButton.Location = new Point(2 * remaining_space + cancelButton.Width, cancelButton.Location.Y);
                cancelButton.Visible = true;
            }
            else
            {
                int size_x = CIEPairingTabPage.Size.Width;
                int size_y = CIEPairingTabPage.Size.Height;
                int remaining_space = (size_x - ((pairButton.Width))) / 2;
                int height = size_y - pairButton.Height - 30;

                pairButton.Location = new Point(remaining_space, height);
                cancelButton.Visible = false;
            }
        }

        private void MenuHomeButton_Click(object sender, EventArgs e)
        {
            Logger.Info("MenuHomeButton_Click() - Inizia funzione");
            SelectHome();
        }

        private void ChangeHomeObjects()
        {
            Logger.Info("ChangeHomeObjects() - Inizia funzione");

            appNameCIEPickerHeaderTextLabel.Text = "Firma Elettronica";
            CIESuccessfullyPairedLabel.Text = "Seleziona la CIE da usare";

            deleteCIEButton.Visible = false;
            removeAllButton.Visible = false;
            addButton.Visible = false;
            selectSigningCIEButton.Visible = true;
            signWithCIEWithoutPairingButton.Visible = true;

            mainTabControl.SelectedIndex = 1;
        }

        private void CreateImages(CieCollection CieColl)
        {
            Logger.Info("createImages() - Inizia funzione");


            /*
            string defaultSignImagePath = getSignImagePath(efSeriale);
            CieColl.addCie(pan, new CieModel(efSeriale, name, pan));

            Properties.Settings.Default.cieList = JsonConvert.SerializeObject(CieColl.MyDictionary);
            Properties.Settings.Default.Save();

            Console.WriteLine("Immagine firma salvata in: {0}", defaultSignImagePath);

            TextInfo nameInfo = new CultureInfo("it-IT", false).TextInfo;
            DrawText(nameInfo.ToTitleCase(name.ToLower()), Color.Black, defaultSignImagePath);
            */
        }

        private void SelectHome()
        {
            Logger.Info("SelectHome() - Inizia funzione");

            deleteCIEButton.Visible = true;
            removeAllButton.Visible = true;
            addButton.Visible = true;
            selectSigningCIEButton.Visible = false;
            signWithCIEWithoutPairingButton.Visible = false;

            appNameCIEPickerHeaderTextLabel.Text = "CIE ID";
            CIESuccessfullyPairedLabel.Text = "Carta d'Identità Elettronica abbinata correttamente";

            CieColl = new CieCollection(Properties.Settings.Default.cieList);

            if (!Properties.Settings.Default.cardHolder.Equals(""))
            {
                CieColl.addCie(Properties.Settings.Default.serialNumber, new CieModel(Properties.Settings.Default.efSeriale, Properties.Settings.Default.cardHolder, Properties.Settings.Default.serialNumber));

                Properties.Settings.Default.serialNumber = "";
                Properties.Settings.Default.cardHolder = "";
                Properties.Settings.Default.efSeriale = "";
                Properties.Settings.Default.Save();
            }

            ConfigureHomeButtons(cieColl);

            Console.WriteLine("Lista CIE abbinate: " + Properties.Settings.Default.cieList);

            if (CieColl.MyDictionary.Count == 0)
            {
                mainTabControl.SelectedIndex = 0;
            }
            else
            {
                if (carouselControl == null)
                {
                    carouselControl = new CarouselControl(tableLayoutPanelCarousel, dotsGroup);
                    carouselControl.ButtonsChanged += CarouselControl_ButtonsChanged;
                }

                carouselControl.LoadData(CieColl);
                carouselControl.UpdateLayout();

                mainTabControl.SelectedIndex = 1;
            }

            menuHomeButton.BackColor = Color.LightGray;
            menuChangePINButton.BackColor = Color.Transparent;
            menuUnlockPINButton.BackColor = Color.Transparent;
            menuTutorialButton.BackColor = Color.Transparent;
            menuInformationButton.BackColor = Color.Transparent;
            menuHelpButton.BackColor = Color.Transparent;
            menuDigitalSignatureButton.BackColor = Color.Transparent;
            menuVerifyButton.BackColor = Color.Transparent;
            menuSettingsButton.BackColor = Color.Transparent;
        }

        private PrivateFontCollection LoadCustomFont()
        {
            Logger.Info("loadCustomFont() - Inizia funzione");
            //Create your private font collection object.
            PrivateFontCollection pfc = new PrivateFontCollection();

            //Select your font from the resources.
            //My font here is "Digireu.ttf"
            int fontLength = Properties.Resources.Allura_Regular.Length;

            // create a buffer to read in to
            byte[] fontdata = Properties.Resources.Allura_Regular;

            // create an unsafe memory block for the font data
            System.IntPtr data = Marshal.AllocCoTaskMem(fontLength);

            // copy the bytes to the unsafe memory block
            Marshal.Copy(fontdata, 0, data, fontLength);

            // pass the font to the font collection
            pfc.AddMemoryFont(data, fontLength);

            return pfc;
        }

        private void DrawText(String text, Color textColor, String path)
        {
            PrivateFontCollection pfc = LoadCustomFont();

            Font font = new Font(pfc.Families[0], signFontSize);

            //first, create a dummy bitmap just to get a graphics object
            Image img = new Bitmap(1, 1);
            Graphics drawing = Graphics.FromImage(img);
            //measure the string to see how big the image needs to be
            SizeF textSize = drawing.MeasureString(text, font);

            //set the stringformat flags to rtl
            StringFormat sf = new StringFormat();
            //uncomment the next line for right to left languages
            //sf.FormatFlags = StringFormatFlags.DirectionRightToLeft;
            sf.Trimming = StringTrimming.Word;
            //free up the dummy image and old graphics object
            img.Dispose();
            drawing.Dispose();

            //create a new image of the right size
            img = new Bitmap((int)textSize.Width, (int)textSize.Height);

            drawing = Graphics.FromImage(img);
            //Adjust for high quality
            drawing.CompositingQuality = CompositingQuality.HighQuality;
            drawing.InterpolationMode = InterpolationMode.HighQualityBilinear;
            drawing.PixelOffsetMode = PixelOffsetMode.HighQuality;
            drawing.SmoothingMode = SmoothingMode.HighQuality;
            drawing.TextRenderingHint = TextRenderingHint.AntiAliasGridFit;

            //paint the background
            drawing.Clear(Color.White);

            //create a brush for the text
            Brush textBrush = new SolidBrush(textColor);

            drawing.DrawString(text, font, textBrush, new RectangleF(0, 0, textSize.Width, textSize.Height), sf);
            drawing.Save();

            textBrush.Dispose();
            drawing.Dispose();
            img.Save(path, ImageFormat.Png);
            img.Dispose();
        }

        private void SelectPairingProgress()
        {
            mainTabControl.SelectedIndex = 2;
            progressBar.Value = 0;
            progressBar.Maximum = 100;
        }

        private void PairButton_Click(object sender, EventArgs e)
        {
            Logger.Info("PairButton_Click() - Inizia funzione");
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

            mainTabControl.SelectedIndex = 2;

            ThreadStart processTaskThread = delegate { CIEPairing(sender, pin, false); };

            new Thread(processTaskThread).Start();
        }

        private int CIEPairing(object sender, string pin, bool oneShotSignature)
        {
            int ret = -1;

            Logger.Info("CIEPairing() - Inizia funzione");
            try
            {
                int[] attempts = new int[1];

                ret = AbbinaCIE(null, pin, attempts, new ProgressCallback(PairingProgress), new CompletedCallback(PairingCompleted));

                this.Invoke((MethodInvoker)delegate
                {
                    ((Control)sender).Enabled = true;
                    switch (ret)
                    {
                        case CKR_TOKEN_NOT_RECOGNIZED:
                            Logger.Debug("CIE non presente sul lettore - CKR_TOKEN_NOT_RECOGNIZED");
                            MessageBox.Show("CIE non presente sul lettore", "Abilitazione CIE", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            SelectHome();
                            break;

                        case CKR_TOKEN_NOT_PRESENT:
                            Logger.Debug("CIE non presente sul lettore - CKR_TOKEN_NOT_PRESENT");
                            MessageBox.Show("CIE non presente sul lettore", "Abilitazione CIE", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            SelectHome();
                            break;

                        case CKR_PIN_INCORRECT:
                            Logger.Debug("Il PIN digitato è errato. - CKR_PIN_INCORRECT");
                            MessageBox.Show(String.Format("Il PIN digitato è errato. Rimangono {0} tentativi", attempts[0]), "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            SelectHome();
                            break;

                        case CKR_PIN_LOCKED:
                            Logger.Debug("Carta bloccata - CKR_PIN_LOCKED");
                            MessageBox.Show("Munisciti del codice PUK e utilizza la funzione di sblocco carta per abilitarla", "Carta bloccata", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            SelectHome();
                            break;

                        case CKR_GENERAL_ERROR:
                            Logger.Debug("Errore inaspettato durante la comunicazione con la smart card - CKR_GENERAL_ERROR");
                            MessageBox.Show("Errore inaspettato durante la comunicazione con la smart card", "Errore inaspettato", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            SelectHome();
                            break;
                     
                        case CKR_OK:
                            Logger.Debug("L'abilitazione della CIE è avvenuta con successo - CKR_OK");
                            if (!oneShotSignature)
                            {
                                MessageBox.Show("L'abilitazione della CIE è avvenuta con successo", "CIE abilitata", MessageBoxButtons.OK, MessageBoxIcon.Information);
                                SelectHome();
                            }
                            break;

                        case CARD_ALREADY_ENABLED:
                            Logger.Debug("Carta già abilitata - CARD_ALREADY_ENABLED");
                            MessageBox.Show("Carta già abilitata", "Carta già abilitata", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                            SelectHome();
                            break;
                    }
                });

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }

            return ret;
        }

        private void RemoveExistingGraphicSignature(String serialNumber)
        {
            if (File.Exists(getSignImagePath(serialNumber)))
            {
                File.Delete(getSignImagePath(serialNumber));
            }
        }

        private void RemoveCIEFromCollection(string PAN)
        {
            int ret = DisabilitaCIE(PAN);

            switch (ret)
            {
                case CKR_OK:
                    Logger.Debug("CIE disabilitata con successo");

                    try
                    {
                        var model = CieColl.MyDictionary[PAN];
                        RemoveExistingGraphicSignature(model.SerialNumber);
                    }

                    catch
                    {
                        Console.WriteLine("Failed to erase graphic signature");
                        Logger.Debug("RemoveCIEFromCollection() - Failed to remove graphic signature image for CIE PAN: " + PAN);
                    }


                    CieColl.removeCie(PAN);
                    Properties.Settings.Default.cieList = JsonConvert.SerializeObject(CieColl.MyDictionary);
                    Properties.Settings.Default.Save();

                    if (!shouldSignWithoutCIEPairing)
                    { 
                        MessageBox.Show("CIE disabilitata con successo", "CIE disabilitata", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        SelectHome();
                    }

                    break;

                case CKR_TOKEN_NOT_PRESENT:
                    Logger.Debug("CIE non presente sul lettore");
                    MessageBox.Show("CIE non presente sul lettore", "Disabilitazione CIE", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    break;

                default:
                    Logger.Debug("Impossibile disabilitare la CIE");
                    MessageBox.Show("Impossibile disabilitare la CIE", "CIE non disabilitata", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    break;
            }
        }

        private void DeleteCIEButton_Click(object sender, EventArgs e)
        {
            Logger.Info("DeleteCIEButton_Click() - Inizia funzione");
            var model = carouselControl.ActiveCieModel;

            if (MessageBox.Show(
                        String.Format("Stai rimuovendo la Carta di Identità di {0} dal sistema, per utilizzarla nuovamente dovrai ripetere l'abbinamento.", model.Owner),
                        "Disabilita CIE", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.Cancel)
                return;

            RemoveCIEFromCollection(model.Pan);
        }

        private void ChangePINButton_Click(object sender, EventArgs e)
        {
            Logger.Info("ChangePINButton_Click() - Inizia funzione");
            string pin = textBoxPIN.Text;
            string newpin = textBoxNewPIN.Text;
            string newpin2 = textBoxNewPIN2.Text;

            int i;

            if (pin.Length != 8)
            {
                Logger.Debug("Il PIN attuale deve essere composto da 8 numeri");
                MessageBox.Show("Il PIN deve essere composto da 8 numeri", "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            if (newpin.Length != 8)
            {
                Logger.Debug("Il nuovo PIN deve essere composto da 8 numeri");
                MessageBox.Show("Il PIN deve essere composto da 8 numeri", "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }


            char c = pin[0];

            for (i = 1; i < pin.Length && (c >= '0' && c <= '9'); i++)
            {
                c = pin[i];
            }

            if (i < pin.Length || !(c >= '0' && c <= '9'))
            {
                Logger.Debug("Il PIN attuale deve essere composto da 8 numeri");
                MessageBox.Show("Il PIN deve essere composto da 8 numeri", "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            c = newpin[0];

            for (i = 1; i < newpin.Length && (c >= '0' && c <= '9'); i++)
            {
                c = newpin[i];
            }

            if (i < newpin.Length || !(c >= '0' && c <= '9'))
            {
                Logger.Debug("Il nuovo PIN deve essere composto da 8 numeri");
                MessageBox.Show("Il PIN deve essere composto da 8 numeri", "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            if (!newpin.Equals(newpin2))
            {
                Logger.Debug("I PIN non corrispondono");
                MessageBox.Show("I PIN non corrispondono", "PIN non corrispondenti", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            c = newpin[0];
            char lastchar = c;

            for (i = 1; i < newpin.Length && c == lastchar; i++)
            {
                lastchar = c;
                c = newpin[i];
            }

            if (c == lastchar)
            {
                Logger.Debug("Il nuovo PIN non deve essere composto da cifre uguali");
                MessageBox.Show("Il nuovo PIN non deve essere composto da cifre uguali", "PIN non valido", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            c = newpin[0];
            lastchar = (char)((int)c - 1);

            for (i = 1; i < newpin.Length && c == lastchar + 1; i++)
            {
                lastchar = c;
                c = newpin[i];
            }

            if (c == lastchar + 1)
            {
                Logger.Debug("Il nuovo PIN non deve essere composto da cifre consecutive");
                MessageBox.Show("Il nuovo PIN non deve essere composto da cifre consecutive", "PIN non valido", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            c = newpin[0];
            lastchar = (char)(c + 1);

            for (i = 1; i < newpin.Length && c == lastchar - 1; i++)
            {
                lastchar = c;
                c = newpin[i];
            }

            if (c == lastchar - 1)
            {
                Logger.Debug("Il nuovo PIN non deve essere composto da cifre consecutive");
                MessageBox.Show("Il nuovo PIN non deve essere composto da cifre consecutive", "PIN non valido", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            textBoxPIN.Text = "";
            textBoxNewPIN2.Text = "";
            textBoxNewPIN.Text = "";

            ((Control)sender).Enabled = false;

            mainTabControl.SelectedIndex = 4;

            ThreadStart processTaskThread = delegate { ChangePIN(sender, pin, newpin); };

            new Thread(processTaskThread).Start();
        }

        private void ChangePIN(object sender, string pin, string newpin)
        {
            Logger.Info("ChangePIN() - Inizia funzione");
            int[] attempts = new int[1];

            int ret = ChangePIN(pin, newpin, attempts, ChangePINProgress);

            this.Invoke((MethodInvoker)delegate
            {
                ((Control)sender).Enabled = true;
                switch (ret)
                {
                case CKR_TOKEN_NOT_RECOGNIZED:
                    Logger.Debug("CIE non presente sul lettore - CKR_TOKEN_NOT_RECOGNIZED");
                    MessageBox.Show("CIE non presente sul lettore", "Cambio PIN", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    SelectHome();
                    break;

                case CKR_TOKEN_NOT_PRESENT:
                    Logger.Debug("CIE non presente sul lettore - CKR_TOKEN_NOT_PRESENT");
                    MessageBox.Show("CIE non presente sul lettore", "Cambio PIN", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    SelectHome();
                    break;

                case CKR_PIN_INCORRECT:
                    Logger.Debug("Il PIN digitato è errato - CKR_PIN_INCORRECT");
                    MessageBox.Show(String.Format("Il PIN digitato è errato. Rimangono {0} tentativi", attempts[0]), "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    SelectHome();
                    break;

                case CKR_PIN_LOCKED:
                    Logger.Debug("Carta bloccata - CKR_PIN_LOCKED");
                    MessageBox.Show("Munisciti del codice PUK e utilizza la funzione di sblocco carta per abilitarla", "Carta bloccata", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    SelectHome();
                    break;

                case CKR_GENERAL_ERROR:
                    Logger.Debug("Errore inaspettato durante la comunicazione con la smart card - CKR_GENERAL_ERROR");
                    MessageBox.Show("Errore inaspettato durante la comunicazione con la smart card", "Errore inaspettato", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    SelectHome();
                    break;

                case CKR_OK:
                    Logger.Debug("Il PIN è stato modificato con successo - CKR_OK");
                    MessageBox.Show("Il PIN è stato modificato con successo", "Operazione completata", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    SelectHome();
                    new PINNotice().ShowDialog();
                    break;
                }
            });
        }

        private void UnlockPIN(object sender, string puk, string newpin)
        {
            Logger.Info("UnlockPIN() - Inizia funzione");
            int[] attempts = new int[1];

            long ret = UnlockPIN(puk, newpin, attempts, UnlockPINProgress);

            this.Invoke((MethodInvoker)delegate
            {
                ((Control)sender).Enabled = true;

                switch (ret)
                {
                case CKR_TOKEN_NOT_RECOGNIZED:
                    Logger.Debug("CIE non presente sul lettore - CKR_TOKEN_NOT_RECOGNIZED");
                    MessageBox.Show("CIE non presente sul lettore", "Sblocca CIE", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    SelectHome();
                    break;

                case CKR_TOKEN_NOT_PRESENT:
                    Logger.Debug("CIE non presente sul lettore - CKR_TOKEN_NOT_PRESENT");
                    MessageBox.Show("CIE non presente sul lettore", "Sblocca CIE", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    SelectHome();
                    break;

                case CKR_PIN_INCORRECT:
                    Logger.Debug("Il PUK digitato è errato. - CKR_PIN_INCORRECT");
                    MessageBox.Show(String.Format("Il PUK digitato è errato. Rimangono {0} tentativi", attempts[0]), "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    SelectHome();
                    break;

                case CKR_PIN_LOCKED:
                    Logger.Debug("PUK bloccato. - CKR_PIN_LOCKED");
                    MessageBox.Show("PUK bloccato. La tua CIE deve essere sostituita", "Carta bloccata", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    SelectHome();
                    break;

                case CKR_GENERAL_ERROR:
                    Logger.Debug("Errore inaspettato durante la comunicazione con la smart card - CKR_GENERAL_ERROR");
                    MessageBox.Show("Errore inaspettato durante la comunicazione con la smart card", "Errore inaspettato", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    SelectHome();
                    break;
                case CKR_DEVICE_ERROR:
                    Logger.Debug("Errore inaspettato durante la comunicazione con la smart card - CKR_DEVICE_ERROR");
                    MessageBox.Show("Errore inaspettato durante la comunicazione con la smart card", "Errore inaspettato", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    SelectHome();
                    break;
                case CKR_OK:
                    Logger.Debug("La CIE è stata sbloccata con successo - CKR_OK");
                    MessageBox.Show("La CIE è stata sbloccata con successo", "Operazione completata", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    SelectHome();
                    new PINNotice().ShowDialog();
                    break;
                }
            });
        }

        private void MenuChangePINButton_Click(object sender, EventArgs e)
        {
            Logger.Info("MenuChangePINButton_Click() - Inizia funzione");
            SelectChangePIN();
        }

        private void SelectChangePIN()
        {
            Logger.Info("SelectChangePIN() - Inizia funzione");
            mainTabControl.SelectedIndex = 3;

            menuHomeButton.BackColor = Color.Transparent;
            menuChangePINButton.BackColor = Color.LightGray;
            menuUnlockPINButton.BackColor = Color.Transparent;
            menuTutorialButton.BackColor = Color.Transparent;
            menuInformationButton.BackColor = Color.Transparent;
            menuHelpButton.BackColor = Color.Transparent;
            menuDigitalSignatureButton.BackColor = Color.Transparent;
            menuVerifyButton.BackColor = Color.Transparent;
            menuSettingsButton.BackColor = Color.Transparent;
        }

        private void MenuUnlockPINButton_Click(object sender, EventArgs e)
        {
            Logger.Info("MenuUnlockPINButton_Click() - Inizia funzione");
            SelectUnlockPIN();
        }

        private void SelectUnlockPIN()
        {
            Logger.Info("SelectUnlockPIN() - Inizia funzione");
            mainTabControl.SelectedIndex = 5;

            menuHomeButton.BackColor = Color.Transparent;
            menuChangePINButton.BackColor = Color.Transparent;
            menuUnlockPINButton.BackColor = Color.LightGray;
            menuTutorialButton.BackColor = Color.Transparent;
            menuInformationButton.BackColor = Color.Transparent;
            menuHelpButton.BackColor = Color.Transparent;
            menuDigitalSignatureButton.BackColor = Color.Transparent;
            menuVerifyButton.BackColor = Color.Transparent;
            menuSettingsButton.BackColor = Color.Transparent;
        }

        private void UnlockPINButton_Click(object sender, EventArgs e)
        {
            Logger.Info("UnlockPINButton_Click() - Inizia funzione");
            string puk = textBoxPUK.Text;
            string newpin = textBoxUnlockPIN.Text;
            string newpin2 = textBoxUnlockPIN2.Text;

            int i;

            if (puk.Length != 8)
            {
                Logger.Debug("Il PUK deve essere composto da 8 numeri");
                MessageBox.Show("Il PUK deve essere composto da 8 numeri", "PUK non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            if (newpin.Length != 8)
            {
                Logger.Debug("Il PIN deve essere composto da 8 numeri");
                MessageBox.Show("Il PIN deve essere composto da 8 numeri", "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }


            char c = puk[0];

            i = 1;
            for (i = 1; i < puk.Length && (c >= '0' && c <= '9'); i++)
            {
                c = puk[i];
            }

            if (i < puk.Length || !(c >= '0' && c <= '9'))
            {
                Logger.Debug("Il PUK deve essere composto da 8 numeri");
                MessageBox.Show("Il PUK deve essere composto da 8 numeri", "PUK non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            c = newpin[0];

            i = 1;
            for (i = 1; i < newpin.Length && (c >= '0' && c <= '9'); i++)
            {
                c = newpin[i];
            }

            if (i < newpin.Length || !(c >= '0' && c <= '9'))
            {
                Logger.Debug("Il PIN deve essere composto da 8 numeri");
                MessageBox.Show("Il PIN deve essere composto da 8 numeri", "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            if (!newpin.Equals(newpin2))
            {
                Logger.Debug("I PIN non corrispondono");
                MessageBox.Show("I PIN non corrispondono", "PIN non corrispondenti", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            c = newpin[0];
            char lastchar = c;

            for (i = 1; i < newpin.Length && c == lastchar; i++)
            {
                lastchar = c;
                c = newpin[i];
            }

            if (c == lastchar)
            {
                Logger.Debug("Il nuovo PIN non deve essere composto da cifre uguali");
                MessageBox.Show("Il nuovo PIN non deve essere composto da cifre uguali", "PIN non valido", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            c = newpin[0];
            lastchar = (char)((int)c - 1);

            for (i = 1; i < newpin.Length && c == lastchar + 1; i++)
            {
                lastchar = c;
                c = newpin[i];
            }

            if (c == lastchar + 1)
            {
                Logger.Debug("Il nuovo PIN non deve essere composto da cifre consecutive");
                MessageBox.Show("Il nuovo PIN non deve essere composto da cifre consecutive", "PIN non valido", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            c = newpin[0];
            lastchar = (char)(c + 1);

            for (i = 1; i < newpin.Length && c == lastchar - 1; i++)
            {
                lastchar = c;
                c = newpin[i];
            }

            if (c == lastchar - 1)
            {
                Logger.Debug("Il nuovo PIN non deve essere composto da cifre consecutive");
                MessageBox.Show("Il nuovo PIN non deve essere composto da cifre consecutive", "PIN non valido", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            textBoxPUK.Text = "";
            textBoxUnlockPIN.Text = "";
            textBoxUnlockPIN2.Text = "";

            ((Control)sender).Enabled = false;

            mainTabControl.SelectedIndex = 6;

            ThreadStart processTaskThread = delegate { UnlockPIN(sender, puk, newpin); };

            new Thread(processTaskThread).Start();
        }

        private void MenuTutorialButton_Click(object sender, EventArgs e)
        {
            Logger.Info("MenuTutorialButton_Click() - Inizia funzione");
            mainTabControl.SelectedIndex = 7;

            menuHomeButton.BackColor = Color.Transparent;
            menuChangePINButton.BackColor = Color.Transparent;
            menuUnlockPINButton.BackColor = Color.Transparent;
            menuTutorialButton.BackColor = Color.LightGray;
            menuInformationButton.BackColor = Color.Transparent;
            menuHelpButton.BackColor = Color.Transparent;
            menuDigitalSignatureButton.BackColor = Color.Transparent;
            menuVerifyButton.BackColor = Color.Transparent;
            menuSettingsButton.BackColor = Color.Transparent;
            webBrowserTutorial.Navigate("https://idserver.servizicie.interno.gov.it/idp/tutorial_win.jsp");
        }

        private void MenuHelpButton_Click(object sender, EventArgs e)
        {
            Logger.Info("MenuHelpButton_Click() - Inizia funzione");
            mainTabControl.SelectedIndex = 8;

            menuHomeButton.BackColor = Color.Transparent;
            menuChangePINButton.BackColor = Color.Transparent;
            menuUnlockPINButton.BackColor = Color.Transparent;
            menuTutorialButton.BackColor = Color.Transparent;
            menuInformationButton.BackColor = Color.Transparent;
            menuHelpButton.BackColor = Color.LightGray;
            menuDigitalSignatureButton.BackColor = Color.Transparent;
            menuVerifyButton.BackColor = Color.Transparent;
            menuSettingsButton.BackColor = Color.Transparent;

            webBrowserHelp.Navigate("https://idserver.servizicie.interno.gov.it/idp/aiuto.jsp");
        }

        private void MenuInformationButton_Click(object sender, EventArgs e)
        {
            Logger.Info("MenuInformationButton_Click() - Inizia funzione");
            mainTabControl.SelectedIndex = 9;

            menuHomeButton.BackColor = Color.Transparent;
            menuChangePINButton.BackColor = Color.Transparent;
            menuUnlockPINButton.BackColor = Color.Transparent;
            menuTutorialButton.BackColor = Color.Transparent;
            menuInformationButton.BackColor = Color.LightGray;
            menuHelpButton.BackColor = Color.Transparent;
            menuDigitalSignatureButton.BackColor = Color.Transparent;
            menuVerifyButton.BackColor = Color.Transparent;
            menuSettingsButton.BackColor = Color.Transparent;

            webBrowserInfo.Navigate("https://idserver.servizicie.interno.gov.it/idp/privacy.jsp");
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            Logger.Info("MainForm_FormClosed() - Inizia funzione");
            Application.Exit();
        }

        private void PINTextBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 13) // enter
            {
                ChangePINButton_Click(sender, e);
            }
        }

        private void PUKTextBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 13) // enter
            {
                UnlockPINButton_Click(sender, e);
            }
        }

        private void AddButton_Click(object sender, EventArgs e)
        {
            Logger.Info("AddButton_Click() - Inizia funzione");
            mainTabControl.SelectedIndex = 0;
        }

        private void RemoveAllButton_Click(object sender, EventArgs e)
        {
            Logger.Info("RemoveAllButton_Click() - Inizia funzione");
            if (MessageBox.Show(String.Format("Rimuovere tutte le Carte di Identità attualmente abbinate?"),
                                "Disabilita tutte le CIE", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.Cancel)
                return;

            String[] arrayCIE = new String[CieColl.MyDictionary.Count];

            for (int i = 0; i < arrayCIE.Count(); i++)
            {
                arrayCIE[i] = CieColl.MyDictionary.ElementAt(i).Key;
            }

            for (int i = 0; i < arrayCIE.Count(); i++)
            {

                int ret = DisabilitaCIE(arrayCIE[i]);

                if (ret != CKR_OK)
                {
                    var cieModel = CieColl.MyDictionary.ElementAt(i).Value;
                    MessageBox.Show("Impossibile disabilitare la CIE numero " + cieModel.SerialNumber, "CIE non disabilitata", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    Properties.Settings.Default.cieList = JsonConvert.SerializeObject(CieColl.MyDictionary);
                    Properties.Settings.Default.Save();
                    SelectHome();
                    return;
                }

                CieColl.removeCie(arrayCIE[i]);
                Console.WriteLine("CIE con chiave " + arrayCIE[i] + " disabilitata");
            }

            Properties.Settings.Default.cieList = JsonConvert.SerializeObject(CieColl.MyDictionary);
            Properties.Settings.Default.Save();

            Console.WriteLine("Cie Rimanenti: " + Properties.Settings.Default.cieList);

            MessageBox.Show("CIE disabilitate con successo", "CIE disabilitate", MessageBoxButtons.OK, MessageBoxIcon.Information);
            SelectHome();
        }

        private void LeftButton_Click(object sender, EventArgs e)
        {
            Logger.Info("LeftButton_Click() - Inizia funzione");
            carouselControl.ShiftRight();
        }

        private void RightButton_Click(object sender, EventArgs e)
        {
            Logger.Info("RightButton_Click() - Inizia funzione");
            carouselControl.ShiftLeft();
        }

        private void CarouselControl_ButtonsChanged(object sender, Controls.CarouselControl.ButtonsEventArgs e)
        {
            Logger.Info("CarouselControl_ButtonsChanged() - Inizia funzione");
            if (e.IsRightButton)
            {
                rightButton.Enabled = e.IsEnabled;
                ToggleButtonVisibility(rightButton, e.IsVisible);
            }
            else
            {
                leftButton.Enabled = e.IsEnabled;
                ToggleButtonVisibility(leftButton, e.IsVisible);
            }
        }

        private void ToggleButtonVisibility(Button button, bool show)
        {
            Logger.Info("toggleButtonVisibility() - Inizia funzione");
            if (show)
            {
                button.Show();
            }
            else
            {
                button.Hide();
            }
        }

        private void CancelButton_Click(object sender, EventArgs e)
        {
            Logger.Info("CancelButton_Click() - Inizia funzione");
            SelectHome();
        }

        private void TabPageUnlockPIN_Click(object sender, EventArgs e)
        {
            Logger.Info("TabPageUnlockPIN_Click() - Inizia funzione");
        }

        private void Panel1_Paint(object sender, PaintEventArgs e)
        {
            Logger.Info("panel1_Paint() - Inizia funzione");
            Panel panel = (Panel)sender;
            float width = (float)5.0;
            Pen pen = new Pen(SystemColors.ControlDark, width);
            pen.DashStyle = DashStyle.Dash;
            e.Graphics.DrawLine(pen, 0, 0, 0, panel.Height - 0);
            e.Graphics.DrawLine(pen, 0, 0, panel.Width - 0, 0);
            e.Graphics.DrawLine(pen, panel.Width - 1, panel.Height - 1, 0, panel.Height - 1);
            e.Graphics.DrawLine(pen, panel.Width - 1, panel.Height - 1, panel.Width - 1, 0);
        }

        private void MenuDigitalSignatureButton_Click(object sender, EventArgs e)
        {
            Logger.Info("menuDigitalSignatureButton_Click() - Inizia funzione");
            menuHomeButton.BackColor = Color.Transparent;
            menuChangePINButton.BackColor = Color.Transparent;
            menuUnlockPINButton.BackColor = Color.Transparent;
            menuTutorialButton.BackColor = Color.Transparent;
            menuInformationButton.BackColor = Color.Transparent;
            menuHelpButton.BackColor = Color.Transparent;
            menuDigitalSignatureButton.BackColor = Color.LightGray;
            menuVerifyButton.BackColor = Color.Transparent;
            menuSettingsButton.BackColor = Color.Transparent;

            signOp = OperationSelectedState.NO_OP;
            digitalSignatureLabel.Text = "Firma Elettronica";
            
            if (CieColl.MyDictionary.Count > 0)
            {
                ChangeHomeObjects();
                Console.WriteLine("ChangeHomeObjects called");
            }

            else
            {
                SignWithCIEWithoutPairingButton_Click(null, null);
                Console.WriteLine("SignCIEW/OPairing called");
            }
        }

        private void MenuVerifyButton_Click(object sender, EventArgs e)
        {
            Logger.Info("menuVerifyButton_Click() - Inizia funzione");
            menuHomeButton.BackColor = Color.Transparent;
            menuChangePINButton.BackColor = Color.Transparent;
            menuUnlockPINButton.BackColor = Color.Transparent;
            menuTutorialButton.BackColor = Color.Transparent;
            menuInformationButton.BackColor = Color.Transparent;
            menuHelpButton.BackColor = Color.Transparent;
            menuDigitalSignatureButton.BackColor = Color.Transparent;
            menuVerifyButton.BackColor = Color.LightGray;
            menuSettingsButton.BackColor = Color.Transparent;

            signOp = OperationSelectedState.VERIFY;
            digitalSignatureLabel.Text = "Verifica firma";
            DisplayFileSelectionTab();
        }

        private void CustomizeGraphicSignatureLabel_Click(object sender, EventArgs e)
        {
            Logger.Info("CustomizeGraphicSignatureLabel_Click() - Inizia funzione");

            var model = carouselControl.ActiveCieModel;

            string signImagePath = getSignImagePath(model.SerialNumber);

            if (graphicDigitalSignaturePanel.Controls.Count > 0 && graphicDigitalSignaturePanel.Controls[0] != null)
            {
                graphicDigitalSignaturePanel.Controls[0].Dispose();
            }

            if (!System.IO.File.Exists(signImagePath))
            {
                TextInfo nameInfo = new CultureInfo("it-IT", false).TextInfo;
                string name = model.Owner;
                DrawText(nameInfo.ToTitleCase(name.ToLower()), Color.Black, signImagePath);
            }

            PictureBox signPicture = new PictureBox();
            signPicture.BackColor = Color.Transparent;
            signPicture.Width = graphicDigitalSignaturePanel.Width;
            signPicture.Height = graphicDigitalSignaturePanel.Height;

            Image image;
            using (Stream stream = File.OpenRead(signImagePath))
            {
                image = Image.FromStream(stream);
            }

            Bitmap signImage = new Bitmap(image, signPicture.Width, signPicture.Height);
            signImage.MakeTransparent();
            signPicture.Image = (Image)signImage.Clone();

            signPicture.Update();
            graphicDigitalSignaturePanel.Controls.Add(signPicture);


            if (model.isCustomSign)
            {
                graphicSignatureCustomizationOverviewLabel.Text = "Una tua firma grafica personalizzata è già stata caricata. Vuoi aggiornarla?";
                graphicSignatureCustomizationOverviewLabel.Update();

                generateCustomGraphicSignatureButton.Enabled = true;

            }
            else
            {
                graphicSignatureCustomizationOverviewLabel.Text = "Abbiamo creato per te una firma grafica, ma se preferisci puoi personalizzarla. " +
                                                "Questo passaggio non è indispensabile, ma ti consentirà di dare un tocco personale ai documenti firmati.";

                graphicSignatureCustomizationOverviewLabel.Update();

                generateCustomGraphicSignatureButton.Enabled = false;
            }

            mainTabControl.SelectedIndex = 15;
        }

        private void CustomizeGraphicSignatureLabel_MouseEnter(object sender, EventArgs e)
        {
            customizeGraphicSignatureLabel.Font = new Font(customizeGraphicSignatureLabel.Font, FontStyle.Underline);
        }

        private void CustomizeGraphicSignatureLabel_MouseLeave(object sender, EventArgs e)
        {
            customizeGraphicSignatureLabel.Font = new Font(customizeGraphicSignatureLabel.Font, FontStyle.Regular);
        }

        void PanelChooseDoc_dragEnter(object sender, DragEventArgs e)
        {
            Logger.Info("panelChooseDoc_dragEnter() - Inizia funzione");
            Console.WriteLine("Panel_DragEnter");
            panelChooseDoc.BackColor = Color.LightGray;
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
                e.Effect = DragDropEffects.Copy;
        }

        void PanelChooseDoc_dragLeave(object sender, EventArgs e)
        {
            Logger.Info("panelChooseDoc_dragLeave() - Inizia funzione");
            Console.WriteLine("Panel_DragLeave");
            panelChooseDoc.BackColor = Color.Transparent;
        }

        private void GoToSelectSignOp(string file_name)
        {
            Logger.Info("goToSelectSignOp() - Inizia funzione");
            labelFileNamePathInOperationChooser.Text = file_name;

            if (signOp == OperationSelectedState.VERIFY)
            {
                pnVerificaOp_MouseClick(null, null);
            }
            else
            {
                SignOperationOptionPanel_MouseClick(null, null);
            }
        }

        void PanelChooseDoc_dragDrop(object sender, DragEventArgs e)
        {
            Logger.Info("PanelChooseDoc_dragDrop() - Inizia funzione");

            panelChooseDoc.BackColor = Color.Transparent;
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
            Logger.Debug($"File drop: {files[0]}");
            GoToSelectSignOp(files[0]);
        }

        private void SelectDocument_Click(object sender, EventArgs e)
        {
            Logger.Info("SelectDocument_Click() - Inizia funzione");
            OpenFileDialog openFile = new OpenFileDialog();

            //deleteTmpFiles();
            //page_index = 1;

            if (openFile.ShowDialog() == DialogResult.OK)
            {
                string file_name = openFile.FileName;
                Logger.Debug($"Selected file: {file_name}");

                GoToSelectSignOp(file_name);
            }
        }

        private void DisplaySigningOperationTab()
        {
            Logger.Info("displaySigningOperationTab() - Inizia funzione");
            labelFileNamePathInSigningFormatChooser.Text = labelFileNamePathInOperationChooser.Text;

            signOp = OperationSelectedState.NO_OP;
            proceedWithSignatureButton.Enabled = false;

            PAdESSignatureLabel.ForeColor = SystemColors.ControlDarkDark;
            PAdESFormatDescriptionLabel.ForeColor = SystemColors.ControlDarkDark;

            CAdESSignatureLabel.ForeColor = SystemColors.ControlDarkDark;
            CAdESFormatDescriptionLabel.ForeColor = SystemColors.ControlDarkDark;

            CAdESP7MPictureBox.Image = CIEID.Properties.Resources.p7m_2x_gray;
            pbPades.Image = CIEID.Properties.Resources.pdf_2x_gray;

            enableGraphicSignatureCheckBox.Checked = false;

            if (labelFileNamePathInSigningFormatChooser.Text.EndsWith(".pdf") && !shouldSignWithoutCIEPairing)
            {
                enableGraphicSignatureCheckBox.Enabled = true;
                enableGraphicSignatureCheckBox.Visible = true;
            }
            else
            {
                enableGraphicSignatureCheckBox.Enabled = false;
                enableGraphicSignatureCheckBox.Visible = false;
            }

            mainTabControl.SelectedIndex = 12;
        }

        private void SignOperationOptionPanel_MouseClick(object sender, EventArgs e)
        {
            Logger.Info("signOperationOptionPanel_MouseClick() - Inizia funzione");
            DisplaySigningOperationTab();
        }

        private void SignOperationOptionPanel_MouseEnter(object sender, EventArgs e)
        {
            signOptionChooserLabel.ForeColor = SystemColors.Highlight;
        }

        private void SignOperationOptionPanel_MouseLeave(object sender, EventArgs e)
        {
            signOptionChooserLabel.ForeColor = SystemColors.ControlDarkDark;
        }

        private void pnVerificaOp_MouseClick(object sender, EventArgs e)
        {
            Logger.Info("pnVerificaOp_MouseClick() - Inizia funzione");

            fileNamePathVerifyLabel.Text = labelFileNamePathInOperationChooser.Text;
            SignerInfo sInfo = new SignerInfo(fileNamePathVerifyLabel.Text, pnSignerInfo);
            int n_sott = sInfo.verify();

            if (n_sott == 0)
            {
                MessageBox.Show("Il file selezionato non contiene firme", "Verifica completata", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                mainTabControl.SelectedIndex = 10;
            }

            else if ((UInt32)n_sott == INVALID_FILE_TYPE)
            {
                MessageBox.Show("Il file selezionato non è un file valido. È" +
                    " possibile verificare solo file con estensione .p7m o .pdf", "Errore nella verifica", MessageBoxButtons.OK, MessageBoxIcon.Error);
                mainTabControl.SelectedIndex = 10;
            }

            else if (n_sott < 0)
            {
                MessageBox.Show("Errore nella verifica del file", "Errore nella verifica", MessageBoxButtons.OK, MessageBoxIcon.Error);
                mainTabControl.SelectedIndex = 10;
            }
            else
            {
                signersCounterLabel.Text = string.Format("Numero di sottoscrittori: {0}", n_sott);
                signersCounterLabel.Update();

                verifyPanel.Visible = true;

                if (Path.GetExtension(fileNamePathVerifyLabel.Text) == ".p7m")
                {
                    extractP7MButton.Enabled = true;
                }
                else
                {
                    extractP7MButton.Enabled = false;
                }

                mainTabControl.SelectedIndex = 16;
            }
        }

        private void pnVerificaOp_MouseEnter(object sender, EventArgs e)
        {
            verifyOptionChooserLabel.ForeColor = SystemColors.Highlight;
        }

        private void pnVerificaOp_MouseLeave(object sender, EventArgs e)
        {
            verifyOptionChooserLabel.ForeColor = SystemColors.ControlDarkDark;
        }

        private void CancelOperationButton_Click(object sender, EventArgs e)
        {
            Logger.Info("CancelOperationButton_Click() - Inizia funzione");
            mainTabControl.SelectedIndex = 10;
        }

        private void EnableGraphicSignatureCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Logger.Info("EnableGraphicSignatureCheckBox_CheckedChanged() - Inizia funzione");
            if (labelFileNamePathInSigningFormatChooser.Text.EndsWith(".pdf"))
            {
                if (enableGraphicSignatureCheckBox.Checked == true)
                {
                    enableGraphicSignatureCheckBox.ForeColor = SystemColors.Highlight;
                }
                else
                {
                    enableGraphicSignatureCheckBox.ForeColor = SystemColors.GrayText;
                }
            }
        }

        private void CancelSigningOperationButton_Click(object sender, EventArgs e)
        {
            Logger.Info("CancelSigningOperationButton_Click() - Inizia funzione");

            signOp = OperationSelectedState.NO_OP;
            proceedWithSignatureButton.Enabled = false;

            PAdESSignatureLabel.ForeColor = SystemColors.ControlDarkDark;
            PAdESFormatDescriptionLabel.ForeColor = SystemColors.ControlDarkDark;

            CAdESSignatureLabel.ForeColor = SystemColors.ControlDarkDark;
            CAdESFormatDescriptionLabel.ForeColor = SystemColors.ControlDarkDark;

            CAdESP7MPictureBox.Image = CIEID.Properties.Resources.p7m_2x_gray;
            pbPades.Image = CIEID.Properties.Resources.pdf_2x_gray;

            enableGraphicSignatureCheckBox.Checked = false;

            mainTabControl.SelectedIndex = 11;
        }

        private void PanelChoosePades_MouseEnter(object sender, EventArgs e)
        {
            panelChoosePades.BorderStyle = BorderStyle.FixedSingle;
        }

        private void PanelChoosePades_MouseLeave(object sender, EventArgs e)
        {
            panelChoosePades.BorderStyle = BorderStyle.None;
        }

        private void PanelChooseCades_MouseEnter(object sender, EventArgs e)
        {
            panelChooseCades.BorderStyle = BorderStyle.FixedSingle;
        }

        private void PanelChooseCades_MouseLeave(object sender, EventArgs e)
        {
            panelChooseCades.BorderStyle = BorderStyle.None;
        }

        private void PanelChoosePades_MouseClick(object sender, EventArgs e)
        {
            Logger.Info("panelChoosePades_MouseClick() - Inizia funzione");
            if (labelFileNamePathInSigningFormatChooser.Text.EndsWith(".pdf"))
            {
                PAdESSignatureLabel.ForeColor = Color.Red;
                PAdESFormatDescriptionLabel.ForeColor = Color.Black;

                CAdESSignatureLabel.ForeColor = SystemColors.ControlDarkDark;
                CAdESFormatDescriptionLabel.ForeColor = SystemColors.ControlDarkDark;

                CAdESP7MPictureBox.Image = CIEID.Properties.Resources.p7m_2x_gray;
                pbPades.Image = CIEID.Properties.Resources.pdf_2x;

                signOp = OperationSelectedState.PADES_SIGNATURE;
                proceedWithSignatureButton.Enabled = true;
            }
        }

        private void PanelChooseCades_MouseClick(object sender, EventArgs e)
        {
            Logger.Info("panelChooseCades_MouseClick() - Inizia funzione");
            CAdESSignatureLabel.ForeColor = SystemColors.Highlight;
            CAdESFormatDescriptionLabel.ForeColor = Color.Black;

            PAdESSignatureLabel.ForeColor = SystemColors.ControlDarkDark;
            PAdESFormatDescriptionLabel.ForeColor = SystemColors.ControlDarkDark;
            enableGraphicSignatureCheckBox.Checked = false;

            CAdESP7MPictureBox.Image = CIEID.Properties.Resources.p7m_2x;
            pbPades.Image = CIEID.Properties.Resources.pdf_2x_gray;

            signOp = OperationSelectedState.CADES_SIGNATURE;
            proceedWithSignatureButton.Enabled = true;
        }

        private void ProceedWithSignatureButton_Click(object sender, EventArgs e)
        {
            Logger.Info("proceedWithSignatureButton_Click() - Inizia funzione");

            if ((enableGraphicSignatureCheckBox.Checked == true) && (signOp == OperationSelectedState.PADES_SIGNATURE))
            {
                var model = carouselControl.ActiveCieModel;
                string signImagePath = getSignImagePath(model.SerialNumber);

                if (!System.IO.File.Exists(signImagePath))
                {
                    TextInfo nameInfo = new CultureInfo("it-IT", false).TextInfo;
                    string name = model.Owner;
                    DrawText(nameInfo.ToTitleCase(name.ToLower()), Color.Black, signImagePath);
                }

                labelFileNamePathInDragSignatureBox.Text = labelFileNamePathInSigningFormatChooser.Text;

                upButton.Enabled = true;
                downButton.Enabled = true;

                if (pdfPreview != null)
                {
                    pdfPreview.pdfPreviewRemoveObjects();
                }

                pdfPreview = new PdfPreview(panePreview, labelFileNamePathInDragSignatureBox.Text, signImagePath);
                if (pdfPreview.getPdfPages() <= 1)
                {
                    upButton.Enabled = false;
                    downButton.Enabled = false;
                }

                mainTabControl.SelectedIndex = 13;
            }
            else
            {
                labelFileNamePathSigningOperation.Text = labelFileNamePathInSigningFormatChooser.Text;
                mainTabControl.SelectedIndex = 14;
            }

            ChangeSignPINObjects();
        }

        private void UpButton_Click(object sender, EventArgs e)
        {
            Logger.Info("upButton_Click() - Inizia funzione");
            pdfPreview.pageUp();
        }

        private void DownButton_Click(object sender, EventArgs e)
        {
            Logger.Info("downButton_Click() - Inizia funzione");
            pdfPreview.pageDown();
        }

        private void ChangeSignPINObjects()
        {
            Logger.Info("ChangeSignPINObjects() - Inizia funzione");

            Console.WriteLine("ShouldSignWithoutPairing: " + shouldSignWithoutCIEPairing);

            byte tagIndexStart = 9;
            byte tagIndexEnd = 17;
            byte spacingDiffPINLabelAndInput = 77;

            byte numDigitsPIN = (byte)((shouldSignWithoutCIEPairing) ? 8 : 4);
            typePINLabel.Text = "Inserire le " + ((numDigitsPIN == 4) ? "ultime 4" : "8") + " cifre del PIN";
            typePINLabel.TextAlign = ContentAlignment.MiddleLeft;
            typePINLabel.Show();

            for (int i = tagIndexStart; i < tagIndexEnd; i++)
            {
                TextBox txtField = (TextBox)FindControlByTag(Controls, "" + i);
                txtField.Hide();
                txtField.Text = "";

                txtField.Location = new Point(typePINLabel.Location.X + ((shouldSignWithoutCIEPairing) ? 8 : spacingDiffPINLabelAndInput) + 24 * (i - tagIndexStart), txtField.Location.Y);

                if(i < tagIndexStart + numDigitsPIN)
                    txtField.Show();
            }

            cancelSigningOperationPINPanelButton.Show();
            cancelSigningOperationPINPanelButton.Enabled = true;
            signButton.Show();
            signButton.Enabled = false;
            closeButton.Hide();

            placeCIEOnReaderLabel.Show();
            digitalSignatureCompletedPictureBox.Hide();
            documentSuccessfullySignedLabel.Hide();
            signProgressBar.Hide();
        }

        private void CloseButton_Click(object sender, EventArgs e)
        {
            Logger.Info("CloseButton_Click() - Inizia funzione");

            ChangeSignPINObjects();

            MenuDigitalSignatureButton_Click(null, null);
        }

        private void GoToSignPIN(string fileToSignPath)
        {
            Logger.Info("GoToSignPIN() - Inizia funzione");
            labelFileNamePathSigningOperation.Text = fileToSignPath;
            mainTabControl.SelectedIndex = 14;
        }

        private void ProceedSignPreviewButton_Click(object sender, EventArgs e)
        {
            Logger.Info("ProceedSignPreviewButton_Click() - Inizia funzione");
            GoToSignPIN(labelFileNamePathInDragSignatureBox.Text);
        }

        private void SignButton_Click(object sender, EventArgs e)
        {
            Logger.Info("SignButton_Click() - Inizia funzione");
            string PIN = "";
            byte numDigitsPIN = (byte)((shouldSignWithoutCIEPairing) ? 8 : 4);

            for (int i = 9; i < 9 + numDigitsPIN; i++)
            {
                TextBox txtField = (TextBox)FindControlByTag(Controls, "" + i);

                PIN += txtField.Text;
            }

            if (PIN.Length != numDigitsPIN)
            {
                MessageBox.Show("Inserire le " + ((numDigitsPIN == 4) ? "ultime 4": "8") + " cifre del PIN", "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            string fileName = "";

            SaveFileDialog saveFileDialog1 = new SaveFileDialog();

            if (signOp == OperationSelectedState.PADES_SIGNATURE)
            {
                fileName = Path.GetFileNameWithoutExtension(labelFileNamePathSigningOperation.Text) + "-signed";
                saveFileDialog1.Filter = "File (*.pdf) | *.pdf";
            }
            else
            {
                fileName = Path.GetFileName(labelFileNamePathSigningOperation.Text) + "-signed";
                fileName = fileName.Replace(".p7m", "");
                saveFileDialog1.Filter = "File (*.p7m) | *.p7m";
            }

            saveFileDialog1.FileName = fileName;
            string sfdname = saveFileDialog1.FileName;
            string pathToSaveFile = "";
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                pathToSaveFile = Path.GetFullPath(saveFileDialog1.FileName);
                Console.WriteLine("Path save file: {0}", pathToSaveFile);
            }
            else
                return;

            for (int i = 9; i < 9 + numDigitsPIN; i++)
            {
                TextBox txtField = (TextBox)FindControlByTag(Controls, "" + i);
                txtField.Hide();
            }

            cancelSigningOperationPINPanelButton.Enabled = false;
            signButton.Enabled = false;

            signProgressBar.Value = 0;
            signProgressBar.Show();
            typePINLabel.TextAlign = ContentAlignment.MiddleCenter;
            typePINLabel.Text = "Firma in corso...";
            typePINLabel.Update();

            //PairCIE and Sign

            ((Control)sender).Enabled = false;
            ThreadStart processTaskThread = delegate
            {
                CieModel model = null;

                if (shouldSignWithoutCIEPairing)
                {
                    int tmpPairRes = CIEPairing(sender, PIN, true);

                    if (tmpPairRes == CARD_ALREADY_ENABLED)
                    {
                        MessageBox.Show("La CIE risulta essere già stata associata precedentemente, per cui l'operazione di firma è stata annullata. Ripetere il procedimento, selezionando la CIE dal selettore presente in 'Firma Elettronica'.", "CIE già associata", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        shouldSignWithoutCIEPairing = false;
                        return;
                    }

                    else if (tmpPairRes != CKR_OK)
                    {
                        MessageBox.Show("Si è verificato un errore durante la lettura dei dati della CIE.", "Errore durante la firma", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        shouldSignWithoutCIEPairing = false;
                        return;
                    }

                    PIN = PIN.Substring(PIN.Length - 4);
                }

                else
                    model = carouselControl.ActiveCieModel;

                int ret = 0;
                if (enableGraphicSignatureCheckBox.Checked && (signOp == OperationSelectedState.PADES_SIGNATURE))
                {
                    Console.WriteLine("Pades con grafica");
                    Dictionary<string, float> signImageInfo = pdfPreview.getSignImageInfos();
                    ret = firmaConCIE(labelFileNamePathSigningOperation.Text, "pdf", PIN, (shouldSignWithoutCIEPairing) ? PANForOneShotSigning : model.Pan, (int)signImageInfo["pageNumber"], signImageInfo["x"], signImageInfo["y"], signImageInfo["w"], signImageInfo["h"],
                                      pdfPreview.getSignImagePath(), pathToSaveFile, new ProgressCallback(SignProgress), new SignCompletedCallback(SignCompleted));
                }
                else if (signOp == OperationSelectedState.PADES_SIGNATURE)
                {
                    Console.WriteLine("Pades senza grafica");
                    ret = firmaConCIE(labelFileNamePathSigningOperation.Text, "pdf", PIN, (shouldSignWithoutCIEPairing) ? PANForOneShotSigning : model.Pan, 0, 0.0f, 0.0f, 0.0f, 0.0f, null, pathToSaveFile, new ProgressCallback(SignProgress), new SignCompletedCallback(SignCompleted));
                }
                else if (signOp == OperationSelectedState.CADES_SIGNATURE)
                {
                    Console.WriteLine("Cades");
                    ret = firmaConCIE(labelFileNamePathSigningOperation.Text, "p7m", PIN, (shouldSignWithoutCIEPairing) ? PANForOneShotSigning : model.Pan, 0, 0.0f, 0.0f, 0.0f, 0.0f, null, pathToSaveFile, new ProgressCallback(SignProgress), new SignCompletedCallback(SignCompleted));
                }

                this.Invoke((MethodInvoker)delegate
                {
                    ((Control)sender).Enabled = true;
                    switch (ret)
                    {
                        case CKR_TOKEN_NOT_RECOGNIZED:
                            MessageBox.Show("CIE non presente sul lettore", "Abilitazione CIE", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            ChangeSignPINObjects();
                            break;

                        case CKR_TOKEN_NOT_PRESENT:
                            MessageBox.Show("CIE non presente sul lettore", "Abilitazione CIE", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            ChangeSignPINObjects();
                            break;

                        case CKR_PIN_INCORRECT:
                            MessageBox.Show(String.Format("Il PIN digitato è errato."), "PIN non corretto", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            ChangeSignPINObjects();
                            break;

                        case CKR_PIN_LOCKED:
                            MessageBox.Show("Munisciti del codice PUK e utilizza la funzione di sblocco carta per abilitarla", "Carta bloccata", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            ChangeSignPINObjects();
                            break;

                        case CARD_PAN_MISMATCH:
                            MessageBox.Show("CIE selezionata diversa da quella presente sul lettore", "CIE non corrispondente", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            ChangeSignPINObjects();
                            break;
                    }

                    if (shouldSignWithoutCIEPairing)
                        RemoveCIEFromCollection(PANForOneShotSigning);

                    PANForOneShotSigning = String.Empty;
                    shouldSignWithoutCIEPairing = false;
                });
            };

            new Thread(processTaskThread).Start();
        }

        private void CancelCustomizationButton_Click(object sender, EventArgs e)
        {
            Logger.Info("CancelCustomizationButton_Click() - Inizia funzione");

            var model = carouselControl.ActiveCieModel;

            if (model.isCustomSign)
            {
                customizeGraphicSignatureLabel.Text = "Aggiorna";
                labelGraphicSignatureDescriptionInfoBox.Text = "Firma personalizzata correttamente";
            }

            mainTabControl.SelectedIndex = 10;
        }

        private void SelectFileForGraphicSignatureCustomizationButton_Click(object sender, EventArgs e)
        {
            Logger.Info("SelectFileForGraphicSignatureCustomizationButton_Click() - Inizia funzione");
            OpenFileDialog openFile = new OpenFileDialog();

            openFile.Filter = "File (*.png) | *.png";

            if (openFile.ShowDialog() == DialogResult.OK)
            {
                string file_name = openFile.FileName;
                Console.WriteLine("PNG Selected file: {0}", file_name);

                var model = carouselControl.ActiveCieModel;

                File.Copy(file_name, getSignImagePath(model.SerialNumber), true);
                CieColl.MyDictionary[model.Pan].isCustomSign = true;
                Properties.Settings.Default.cieList = JsonConvert.SerializeObject(CieColl.MyDictionary);
                Properties.Settings.Default.Save();

                Image image;
                using (Stream stream = File.OpenRead(getSignImagePath(model.SerialNumber)))
                {
                    image = Image.FromStream(stream);
                }

                PictureBox signPicture = (PictureBox)graphicDigitalSignaturePanel.Controls[0];
                Bitmap signImage = new Bitmap(image, signPicture.Width, signPicture.Height);
                signImage.MakeTransparent();
                signPicture.Image = signImage;
                signPicture.Update();

                graphicSignatureCustomizationOverviewLabel.Text = "Una tua firma grafica personalizzata è già stata caricata. Vuoi aggiornarla?";
                graphicSignatureCustomizationOverviewLabel.Update();

                generateCustomGraphicSignatureButton.Enabled = true;
            }
        }

        private void CloseVerifyButton_Click(object sender, EventArgs e)
        {
            Logger.Info("CloseVerifyButton_Click() - Inizia funzione");
            mainTabControl.SelectedIndex = 10;
        }

        private void DisplayFileSelectionTab()
        {
            Logger.Info("DisplayFileSelectionTab() - Inizia funzione");

            if (signOp == OperationSelectedState.VERIFY)
            {
                pictureBox15.Visible = false;
                customizeGraphicSignatureLabel.Visible = false;
                labelGraphicSignatureDescriptionInfoBox.Visible = false;
                labelDragAndDropDocumentInformation.Text = "Trascina i tuoi documenti qui per verificare una firma elettronica esistente";
            }

            else
            {
                labelDragAndDropDocumentInformation.Text = "Trascina i tuoi documenti qui dentro per firmarli";

                if (shouldSignWithoutCIEPairing)
                {
                    pictureBox15.Visible = false;
                    customizeGraphicSignatureLabel.Visible = false;
                    labelGraphicSignatureDescriptionInfoBox.Visible = false;
                }

                else
                {
                    var model = carouselControl.ActiveCieModel;

                    pictureBox15.Visible = true;

                    customizeGraphicSignatureLabel.Visible = true;
                    labelGraphicSignatureDescriptionInfoBox.Visible = true;

                    if (model.isCustomSign)
                    {
                        customizeGraphicSignatureLabel.Text = "Aggiorna";
                        labelGraphicSignatureDescriptionInfoBox.Text = "Firma personalizzata correttamente";
                    }

                    else
                    {
                        customizeGraphicSignatureLabel.Text = "Personalizza";
                        labelGraphicSignatureDescriptionInfoBox.Text = "Abbiamo creato per te una firma grafica, ma se preferisci puoi personalizzarla. " +
                                       "Questo passaggio non è indispensabile, ma ti consentirà di dare un tocco personale ai documenti firmati.";
                    }
                }
            }

            mainTabControl.SelectedIndex = 10;
        }

        private void SelectSigningCIEButton_Click(object sender, EventArgs e)
        {
            Logger.Info("SelectSigningCIEButton_Click() - Inizia funzione");
            Console.WriteLine("Disabling flag");
            shouldSignWithoutCIEPairing = false;
            DisplayFileSelectionTab();
        }

        private void GenerateCustomGraphicSignatureButton_Click(object sender, EventArgs e)
        {
            Logger.Info("GenerateCustomGraphicSignatureButton_Click() - Inizia funzione");

            var model = carouselControl.ActiveCieModel;

            string defaultSignImagePath = getSignImagePath(model.SerialNumber);

            Console.WriteLine("Immagine firma salvata in: {0}", defaultSignImagePath);

            TextInfo nameInfo = new CultureInfo("it-IT", false).TextInfo;
            DrawText(nameInfo.ToTitleCase(model.Owner.ToLower()), Color.Black, defaultSignImagePath);

            Image image;
            using (Stream stream = File.OpenRead(getSignImagePath(model.SerialNumber)))
            {
                image = Image.FromStream(stream);
            }

            PictureBox signPicture = (PictureBox)graphicDigitalSignaturePanel.Controls[0];
            Bitmap signImage = new Bitmap(image, signPicture.Width, signPicture.Height);
            signImage.MakeTransparent();
            signPicture.Image = signImage;
            signPicture.Update();

            CieColl.MyDictionary[model.Pan].isCustomSign = false;
            Properties.Settings.Default.cieList = JsonConvert.SerializeObject(CieColl.MyDictionary);
            Properties.Settings.Default.Save();

            labelGraphicSignatureDescriptionInfoBox.Text = "Abbiamo creato per te una firma grafica, ma se preferisci puoi personalizzarla. " +
                           "Questo passaggio non è indispensabile, ma ti consentirà di dare un tocco personale ai documenti firmati.";

            customizeGraphicSignatureLabel.Text = "Personalizza";

            graphicSignatureCustomizationOverviewLabel.Text = labelGraphicSignatureDescriptionInfoBox.Text;
            graphicSignatureCustomizationOverviewLabel.Update();

            generateCustomGraphicSignatureButton.Enabled = false;
        }

        private void menuSettingsButton_Click(object sender, EventArgs e)
        {
            Logger.Info("menuSettingsButton_Click() - Inizia funzione");

            menuHomeButton.BackColor = Color.Transparent;
            menuChangePINButton.BackColor = Color.Transparent;
            menuUnlockPINButton.BackColor = Color.Transparent;
            menuTutorialButton.BackColor = Color.Transparent;
            menuInformationButton.BackColor = Color.Transparent;
            menuHelpButton.BackColor = Color.Transparent;
            menuDigitalSignatureButton.BackColor = Color.Transparent;
            menuVerifyButton.BackColor = Color.Transparent;
            menuSettingsButton.BackColor = Color.LightGray;

            // If btnEditSettings is enabled then the config pane is *not* in edit mode
            if (editSettingsButton.Enabled)
            {
                Logger.Debug("menuSettingsButton_Click() - Configurazione non in modifica, carico valori dalle preferenze");
                if (Properties.Settings.Default.proxyURL != "")
                {
                    Logger.Debug("menuSettingsButton_Click() - Impostazione proxy presente");
                    if (Properties.Settings.Default.credentials == "")
                    {
                        Logger.Debug("menuSettingsButton_Click() - Impostazioni credenziali presenti");
                        txtUsername.Text = "";
                        txtPassword.Text = "";
                    }
                    else
                    {
                        Logger.Debug("menuSettingsButton_Click() - Impostazioni credenziali non presenti");
                        string encryptedCredentials = Properties.Settings.Default.credentials;
                        ProxyInfoManager proxyInfoManager = new ProxyInfoManager();

                        string credentials = proxyInfoManager.getDecryptedCredentials(encryptedCredentials);

                        if (credentials.Substring(0, 5) == "cred=")
                        {
                            string[] infos = credentials.Substring(5).Split(':');
                            txtUsername.Text = infos[0];
                            txtPassword.Text = infos[1];
                        }

                    }
                    txtUrl.Text = Properties.Settings.Default.proxyURL;
                    txtPort.Text = Properties.Settings.Default.proxyPort.ToString();
                }

                Logger.Debug($"menuSettingsButton_Click() - radio button app: {Program.LogLevelApp}  lib: {Program.LogLevelLib}");
                rbLoggingAppNone.Checked = (Program.LogLevelApp == LogLevel.NONE);
                rbLoggingAppError.Checked = (Program.LogLevelApp == LogLevel.ERROR);
                rbLoggingAppInfo.Checked = (Program.LogLevelApp == LogLevel.INFO);
                rbLoggingAppDebug.Checked = (Program.LogLevelApp == LogLevel.DEBUG);

                rbLoggingLibNone.Checked = (Program.LogLevelLib == LogLevel.NONE);
                rbLoggingLibError.Checked = (Program.LogLevelLib == LogLevel.ERROR);
                rbLoggingLibInfo.Checked = (Program.LogLevelLib == LogLevel.INFO);
                rbLoggingLibDebug.Checked = (Program.LogLevelLib == LogLevel.DEBUG);
            }

            mainTabControl.SelectedIndex = 17;
        }

        private void ShowPasswordCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            Logger.Info("ShowPasswordCheckBox_CheckedChanged() - Inizia funzione");
            if (showPasswordCheckBox.Checked == true)
            {
                txtPassword.UseSystemPasswordChar = false;
            }
            else
            {
                txtPassword.UseSystemPasswordChar = true;
            }
        }

        private void SaveSettingsButton_Click(object sender, EventArgs e)
        {
            Logger.Info("SaveSettingsButton_Click() - Inizia funzione");

            if ((String.IsNullOrEmpty(txtUsername.Text) && !String.IsNullOrEmpty(txtPassword.Text)) || (!String.IsNullOrEmpty(txtUsername.Text) && String.IsNullOrEmpty(txtPassword.Text)))
            {
                Logger.Debug("SaveSettingsButton_Click() - Campo username o password mancante");
                MessageBox.Show("Campo username o password mancante", "Credenziali proxy mancanti", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }
            if ((String.IsNullOrEmpty(txtPort.Text) && !String.IsNullOrEmpty(txtUrl.Text)) || (!String.IsNullOrEmpty(txtPort.Text) && String.IsNullOrEmpty(txtUrl.Text)))
            {
                Logger.Debug("SaveSettingsButton_Click() - Indirizzo o porta del proxy mancante");
                MessageBox.Show("Indirizzo o porta del proxy mancante", "Informazioni proxy mancanti", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }
            if ((String.IsNullOrEmpty(txtUsername.Text)))
            {
                Logger.Debug("SaveSettingsButton_Click() - Elimina credenziali proxy");
                Properties.Settings.Default.credentials = "";
            }
            else
            {
                Logger.Debug($"SaveSettingsButton_Click() - Conserva credenziali per username: '{txtUsername.Text}'");
                string credentials = String.Format("cred={0}:{1}", txtUsername.Text, txtPassword.Text);
                //Console.WriteLine("Credentials: {0}", credentials);

                ProxyInfoManager proxyInfoManager = new ProxyInfoManager();
                string encryptedCredentials = proxyInfoManager.getEncryptedCredentials(credentials);
                //Console.WriteLine("Credentials: {0}", credentials);
                Properties.Settings.Default.credentials = encryptedCredentials;
            }
            Logger.Debug($"SaveSettingsButton_Click() - Conserva credenziali per proxyURL: '{txtUrl.Text}'");
            Properties.Settings.Default.proxyURL = txtUrl.Text;
            if (String.IsNullOrEmpty(txtPort.Text))
            {
                Logger.Debug($"SaveSettingsButton_Click() - proxyPort: {0}");
                Properties.Settings.Default.proxyPort = 0;
            }
            else
            {
                Int32 value = Int32.Parse(txtPort.Text);
                Logger.Debug($"SaveSettingsButton_Click() - proxyPort: {value}");
                Properties.Settings.Default.proxyPort = value;
            }

            Logger.Debug("SaveSettingsButton_Click() - Registra configurazione di log");
            LogLevel logLevelApp = Logger.DefaultLogLevel;
            LogLevel logLevelLib = Logger.DefaultLogLevel;

            if (rbLoggingAppNone.Checked == true)
            {
                logLevelApp = LogLevel.NONE;
            }
            else if (rbLoggingAppError.Checked == true)
            {
                logLevelApp = LogLevel.ERROR;
            }
            else if (rbLoggingAppInfo.Checked == true)
            {
                logLevelApp = LogLevel.INFO;
            }
            else if (rbLoggingAppDebug.Checked == true)
            {
                logLevelApp = LogLevel.DEBUG;
            }

            if (rbLoggingLibNone.Checked == true)
            {
                logLevelLib = LogLevel.NONE;
            }
            else if (rbLoggingLibError.Checked == true)
            {
                logLevelLib = LogLevel.ERROR;
            }
            else if (rbLoggingLibInfo.Checked == true)
            {
                logLevelLib = LogLevel.INFO;
            }
            else if (rbLoggingLibDebug.Checked == true)
            {
                logLevelLib = LogLevel.DEBUG;
            }

            Program.SetLogConfig(logLevelApp, logLevelLib);

            Logger.Debug("SaveSettingsButton_Click() - Esci dalla modalità di modifica");
            txtUrl.Enabled = false;
            txtUsername.Enabled = false;
            txtPassword.Enabled = false;
            txtPort.Enabled = false;
            showPasswordCheckBox.Enabled = false;
            showPasswordCheckBox.Checked = false;

            configLibraryLoggingGroupBox.Enabled = false;
            configApplicationLoggingGroupBox.Enabled = false;

            saveSettingsButton.Enabled = false;
            editSettingsButton.Enabled = true;

            Logger.Debug("SaveSettingsButton_Click() - Salva configurazione");
            Program.SaveLogConfigToFile();
            Properties.Settings.Default.Save();
            Logger.Debug("SaveSettingsButton_Click() - Configurazione salvata");
        }

        private void EditSettingsButton_Click(object sender, EventArgs e)
        {
            Logger.Info("editSettingsButton_Click() - Inizia funzione");

            txtUrl.Enabled = true;
            txtUsername.Enabled = true;
            txtPassword.Enabled = true;
            txtPort.Enabled = true;
            showPasswordCheckBox.Enabled = true;
            showPasswordCheckBox.Checked = false;

            configLibraryLoggingGroupBox.Enabled = true;
            configApplicationLoggingGroupBox.Enabled = true;

            saveSettingsButton.Enabled = true;
            editSettingsButton.Enabled = false;
        }

        private void txtPort_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar))
            {
                e.Handled = true;
            }
        }

        private void ExtractP7MButton_Click(object sender, EventArgs e)
        {
            Logger.Info("ExtractP7MButton_Click() - Inizia funzione");
            string fileName = Path.GetFileNameWithoutExtension(fileNamePathVerifyLabel.Text);

            fileName = fileName.Replace("-signed", "");

            SaveFileDialog saveFileDialog1 = new SaveFileDialog();
            saveFileDialog1.FileName = fileName;
            string fileExt = Path.GetExtension(fileName);
            saveFileDialog1.Filter = string.Format("{0}{1}{2}{3}", "File(*", fileExt, ") | *", fileExt);

            string pathToSaveFile = "";

            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                pathToSaveFile = Path.GetFullPath(saveFileDialog1.FileName);

                Console.WriteLine("File extension: {0}", fileExt);
                long res = estraiP7m(fileNamePathVerifyLabel.Text, pathToSaveFile);
                Console.WriteLine("Res: {0}", (UInt32)res);

                if (res == 0)
                {
                    MessageBox.Show("File estratto correttamente", "Estrazione file completata", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    MessageBox.Show("Impossibile estrarre il file", "Estrazione file completata", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                return;
            }

        }

        private void SignWithCIEWithoutPairingButton_Click(object sender, EventArgs e)
        {
            //Enable 8 PIN digits
            shouldSignWithoutCIEPairing = true;
            ChangeSignPINObjects();
            DisplayFileSelectionTab();
        }

        private void mainTabControl_SelectedIndexChanged(object sender, EventArgs e)
        {
            Console.WriteLine("mainTabControl_SelectedIndex: " + mainTabControl.SelectedIndex.ToString());

            if (mainTabControl.SelectedIndex == 0 || mainTabControl.SelectedIndex == 1)
                shouldSignWithoutCIEPairing = false;
        }
    }
}
