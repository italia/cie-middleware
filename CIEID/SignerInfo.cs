using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CIEID
{
    class SignerInfo
    {

        private FlowLayoutPanel verifyPanel;
        private string filePath;
        private verifyInfo_t[] vInfos;

        private enum revStatus
        {
            REVOCATION_STATUS_GOOD  = 0,
            REVOCATION_STATUS_REVOKED  = 1,
            REVOCATION_STATUS_SUSPENDED = 2,
            REVOCATION_STATUS_UNKNOWN  = 3
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct verifyInfo_t
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)]
            public string name;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)]
            public string surname;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)]
            public string cn;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)]
            public string signingTime;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)]
            public string cadn;
            public int CertRevocStatus;
            [MarshalAs(UnmanagedType.U1)]
            public bool isSignValid;
            [MarshalAs(UnmanagedType.U1)]
            public bool isCertValid;
        };


        [DllImport("ciepki.dll", CallingConvention = CallingConvention.StdCall)]
        static extern long verificaConCIE(string filePath, string proxyAddress, int proxyPort, string usrPass);
        [DllImport("ciepki.dll", CallingConvention = CallingConvention.StdCall)]
        static extern long getVerifyInfo(int index, ref verifyInfo_t vInfo);

        public SignerInfo(string filePath, FlowLayoutPanel verifyPanel)
        {
            this.verifyPanel = verifyPanel;
            this.filePath = filePath;
        }

        public System.Windows.Forms.TableLayoutPanel createTable(verifyInfo_t vInfo)
        {


            System.Windows.Forms.TableLayoutPanel tbVerifyInfo;
            System.Windows.Forms.Label lblVName;
            System.Windows.Forms.Label lblSignTime;
            System.Windows.Forms.Label lblSign;
            System.Windows.Forms.Label lblCertValid;
            System.Windows.Forms.Label lblCertRev;
            System.Windows.Forms.Label lblCn;
            System.Windows.Forms.PictureBox pbVName;
            System.Windows.Forms.PictureBox pbSignTime;
            System.Windows.Forms.PictureBox pbSign;
            System.Windows.Forms.PictureBox pbCertValid;
            System.Windows.Forms.PictureBox pbCertRev;
            System.Windows.Forms.PictureBox pbCn;

            tbVerifyInfo = new System.Windows.Forms.TableLayoutPanel();

            lblVName = new System.Windows.Forms.Label();
            lblSignTime = new System.Windows.Forms.Label();
            lblSign = new System.Windows.Forms.Label();
            lblCertValid = new System.Windows.Forms.Label();
            lblCertRev = new System.Windows.Forms.Label();
            lblCn = new System.Windows.Forms.Label();
            pbVName = new System.Windows.Forms.PictureBox();
            pbSignTime = new System.Windows.Forms.PictureBox();
            pbSign = new System.Windows.Forms.PictureBox();
            pbCertValid = new System.Windows.Forms.PictureBox();
            pbCertRev = new System.Windows.Forms.PictureBox();
            pbCn = new System.Windows.Forms.PictureBox();

            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form));
            string name = vInfo.name + " " + vInfo.surname + '\n' + vInfo.cn;
            string s_time = vInfo.signingTime;

            if (s_time == "")
            {
                s_time = "Attributo Signing Time non presente";
            }
            else
            {
                s_time = vInfo.signingTime.Substring(0, vInfo.signingTime.Length - 1);
                DateTime dateTime = DateTime.ParseExact(s_time, "yyMMddHHmmss", null);
                s_time = dateTime.ToString();
            }

            string s_cert = "";
            if (vInfo.isCertValid == true) 
            {
                pbCertValid.Image = CIEID.Properties.Resources.green_checkbox;
                pbCertValid.Update();
                s_cert = "Il certificato è valido";
            }
            else
            {
                pbCertValid.Image = CIEID.Properties.Resources.orange_checkbox;
                s_cert = "Il certificato non è valido";
            }

            string s_sign = "La firma non è valida";
            pbSign.Image = CIEID.Properties.Resources.orange_checkbox;
            if (vInfo.isSignValid == true)
            {
                s_sign = "La firma è valida";
                pbSign.Image = CIEID.Properties.Resources.green_checkbox;
            }

            
            string s_revoc = "Servizio di revoca non raggiungibile";
            pbCertRev.Image = CIEID.Properties.Resources.orange_checkbox;

            if (vInfo.CertRevocStatus == (int)revStatus.REVOCATION_STATUS_GOOD)
            {
                s_revoc = "Il certificato non è revocato";
                pbCertRev.Image = CIEID.Properties.Resources.green_checkbox;
            }
            else if (vInfo.CertRevocStatus == (int)revStatus.REVOCATION_STATUS_REVOKED)
            {
                s_revoc = "Il certificato è revocato";
            }
            else if (vInfo.CertRevocStatus == (int)revStatus.REVOCATION_STATUS_SUSPENDED)
            {
                s_revoc = "Il certificato è sospeso";
            }else if (vInfo.CertRevocStatus == (int)revStatus.REVOCATION_STATUS_UNKNOWN)
            {
                s_cert = "Certificato non verificato";
            }

            string s_cadn = vInfo.cadn; 

            // 
            // tbVerifyInfo
            // 
            tbVerifyInfo.ColumnCount = 2;
            tbVerifyInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 12));
            tbVerifyInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 88));
            tbVerifyInfo.Controls.Add(lblCn, 1, 5);
            tbVerifyInfo.Controls.Add(lblCertRev, 1, 4);
            tbVerifyInfo.Controls.Add(lblCertValid, 1, 3);
            tbVerifyInfo.Controls.Add(lblSign, 1, 2);
            tbVerifyInfo.Controls.Add(pbCn, 0, 5);
            tbVerifyInfo.Controls.Add(pbCertRev, 0, 4);
            tbVerifyInfo.Controls.Add(pbCertValid, 0, 3);
            tbVerifyInfo.Controls.Add(pbSign, 0, 2);
            tbVerifyInfo.Controls.Add(pbSignTime, 0, 1);
            tbVerifyInfo.Controls.Add(lblVName, 1, 0);
            tbVerifyInfo.Controls.Add(lblSignTime, 1, 1);
            tbVerifyInfo.Controls.Add(pbVName, 0, 0);
            tbVerifyInfo.Location = new System.Drawing.Point(3, 3);
            tbVerifyInfo.Name = "tbVerifyInfo";
            tbVerifyInfo.RowCount = 6;
            tbVerifyInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 15F));
            tbVerifyInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 15F));
            tbVerifyInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 15F));
            tbVerifyInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 15F));
            tbVerifyInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 15F));
            tbVerifyInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            //tbVerifyInfo.Size = new System.Drawing.Size(286, 244);
            tbVerifyInfo.Size = new System.Drawing.Size(verifyPanel.Width - 20, verifyPanel.Height);
            tbVerifyInfo.TabIndex = 0;
            //tbVerifyInfo.CellBorderStyle = TableLayoutPanelCellBorderStyle.Single;

            // 
            // lblVName
            // 
            lblVName.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            lblVName.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            lblVName.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            lblVName.Location = new System.Drawing.Point(38, 0);
            lblVName.Name = "lblVName";
            //lblVName.Padding = new System.Windows.Forms.Padding(0, 0, 0, 5);
            //lblVName.Size = new System.Drawing.Size(245, 37);
            lblVName.TabIndex = 0;
            lblVName.Text = name;
            lblVName.AutoSize = true;

            // 
            // pbVName
            // 
            pbVName.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            //pbVName.Padding = new System.Windows.Forms.Padding(15, 12, 15, 12);
            pbVName.Location = new System.Drawing.Point(3, 3);
            pbVName.Name = "pbVName";
            pbVName.Size = new System.Drawing.Size(29, 31);
            pbVName.TabIndex = 6;
            pbVName.TabStop = false;
            pbVName.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            pbVName.Image = CIEID.Properties.Resources.user;

            // 
            // lblSignTime
            // 
            lblSignTime.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            lblSignTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            lblSignTime.Location = new System.Drawing.Point(38, 43);
            lblSignTime.Name = "lblSignTime";
            //lblSignTime.Padding = new System.Windows.Forms.Padding(0, 0, 0, 5);
            //lblSignTime.Size = new System.Drawing.Size(245, 24);
            lblSignTime.TabIndex = 1;
            lblSignTime.AutoSize = true;
            lblSignTime.Text = s_time;

            // 
            // pbSignTime
            // 
            pbSignTime.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            //pbVName.Padding = new System.Windows.Forms.Padding(15, 12, 15, 12);
            pbSignTime.Location = new System.Drawing.Point(3, 3);
            pbSignTime.Name = "pbVName";
            pbSignTime.Size = new System.Drawing.Size(29, 31);
            pbSignTime.TabIndex = 6;
            pbSignTime.TabStop = false;
            pbSignTime.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            pbSignTime.Image = CIEID.Properties.Resources.calendar;

            // 
            // lblSign
            // 
            lblSign.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            lblSign.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            lblSign.Location = new System.Drawing.Point(38, 80);
            lblSign.Name = "lblSign";
            //lblSign.Padding = new System.Windows.Forms.Padding(0, 0, 0, 5);
            //lblSign.Size = new System.Drawing.Size(245, 24);
            lblSign.TabIndex = 12;
            lblSign.Text = s_sign;
            lblSignTime.AutoSize = true;

            // 
            // pbSign
            // 
            pbSign.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            //pbSign.Padding = new System.Windows.Forms.Padding(15, 12, 15, 12);
            pbSign.Location = new System.Drawing.Point(3, 77);
            pbSign.Name = "pbSign";
            pbSign.Size = new System.Drawing.Size(29, 31);
            pbSign.TabIndex = 8;
            pbSign.TabStop = false;
            pbSign.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;


            // 
            // lblCertValid
            // 
            lblCertValid.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            lblCertValid.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            lblCertValid.Location = new System.Drawing.Point(38, 117);
            lblCertValid.Name = "lblCertValid";
            //lblCertValid.Padding = new System.Windows.Forms.Padding(0, 0, 0, 5);
            //lblCertValid.Size = new System.Drawing.Size(245, 24);
            lblCertValid.TabIndex = 13;
            lblCertValid.Text = s_cert;
            lblCertValid.AutoSize = true;

            // 
            // pbCertValid
            // 
            pbCertValid.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            //pbCertValid.Padding = new System.Windows.Forms.Padding(15, 12, 15, 12);
            pbCertValid.Location = new System.Drawing.Point(3, 114);
            pbCertValid.Name = "pbCertValid";
            pbCertValid.Size = new System.Drawing.Size(29, 31);
            pbCertValid.TabIndex = 9;
            pbCertValid.TabStop = false;
            pbCertValid.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;

            // 
            // lblCertRev
            // 
            lblCertRev.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            lblCertRev.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            lblCertRev.Location = new System.Drawing.Point(38, 154);
            lblCertRev.Name = "lblCertRev";
            //lblCertRev.Padding = new System.Windows.Forms.Padding(0, 0, 0, 5);
            //lblCertRev.Size = new System.Drawing.Size(245, 24);
            lblCertRev.TabIndex = 14;
            lblCertRev.Text = s_revoc;
            lblCertRev.AutoSize = true;

            // 
            // pbCertRev
            // 
            pbCertRev.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            //pbCertRev.Padding = new System.Windows.Forms.Padding(15, 12, 15, 12);
            pbCertRev.Location = new System.Drawing.Point(3, 151);
            pbCertRev.Name = "pbCertRev";
            pbCertRev.Size = new System.Drawing.Size(29, 31);
            pbCertRev.TabIndex = 10;
            pbCertRev.TabStop = false;
            pbCertRev.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;

            // 
            // lblCn
            // 
            lblCn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            lblCn.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            lblCn.Location = new System.Drawing.Point(38, 188);
            lblCn.Name = "lblCn";
            //lblCn.Padding = new System.Windows.Forms.Padding(0, 0, 0, 5);
            //lblCn.Size = new System.Drawing.Size(245, 48);
            lblCn.TabIndex = 15;
            lblCn.AutoSize = true;
            lblCn.Text = s_cadn;

            // 
            // pbCn
            // 
            pbCn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            //pbCn.Padding = new System.Windows.Forms.Padding(15, 28, 15, 28);
            pbCn.Location = new System.Drawing.Point(3, 188);
            pbCn.Name = "pbCn";
            pbCn.Size = new System.Drawing.Size(29, 31);
            pbCn.TabIndex = 11;
            pbCn.TabStop = false;
            pbCn.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            pbCn.Image = CIEID.Properties.Resources.medal;

            return tbVerifyInfo;
        }


        public int verify()
        {
            string proxyAddress = null;
            string proxyCredentials = null;
            int proxyPort = -1;

            verifyPanel.Controls.Clear();

            verifyPanel.HorizontalScroll.Enabled = false;
            verifyPanel.HorizontalScroll.Visible = false;
            verifyPanel.HorizontalScroll.Maximum = 0;
            verifyPanel.AutoScroll = true;
            verifyPanel.FlowDirection = FlowDirection.TopDown;
            verifyPanel.WrapContents = false;

            if(Properties.Settings.Default.proxyURL != "")
            {
                proxyAddress = Properties.Settings.Default.proxyURL;
                proxyPort = Properties.Settings.Default.proxyPort;
                if(Properties.Settings.Default.credentials != "")
                {
                    string encryptedCredentials = Properties.Settings.Default.credentials;
                    ProxyInfoManager proxyInfoManager = new ProxyInfoManager();
                    string credentials = proxyInfoManager.getDecryptedCredentials(encryptedCredentials);
                    if (credentials.Substring(0, 5) == "cred=")
                    {
                        proxyCredentials = credentials.Substring(5);
                    }
                }
            }

            //Console.WriteLine("Verifica con CIE - Url: {0}, Port: {1}, credentials: {2}", proxyAddress, proxyPort, proxyCredentials);

            int n_sign = (int)verificaConCIE(filePath, proxyAddress, proxyPort, proxyCredentials);

            Console.WriteLine("n_sign: {0}", (UInt32) n_sign);
            if (n_sign <= 0)
            {
                return n_sign;
            }

            vInfos = new verifyInfo_t[n_sign];

            for (int i = 0; i<n_sign; i++)
            {
                getVerifyInfo(i, ref vInfos[i]);
                verifyPanel.Controls.Add(createTable(vInfos[i]));
            }

            return n_sign;
        }

    }
}
