using System;
using System.Windows.Forms;

namespace CIEID
{
    partial class mainForm
    {
        /// <summary>
        /// Variabile di progettazione necessaria.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Pulire le risorse in uso.
        /// </summary>
        /// <param name="disposing">ha valore true se le risorse gestite devono essere eliminate, false in caso contrario.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Codice generato da Progettazione Windows Form

        /// <summary>
        /// Metodo necessario per il supporto della finestra di progettazione. Non modificare
        /// il contenuto del metodo con l'editor di codice.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(mainForm));
            this.mainTabControl = new System.Windows.Forms.TabControl();
            this.CIEPairingTabPage = new System.Windows.Forms.TabPage();
            this.cancelButton = new System.Windows.Forms.Button();
            this.pairButton = new System.Windows.Forms.Button();
            this.PINDigit_TextBox8 = new System.Windows.Forms.TextBox();
            this.PINDigit_TextBox7 = new System.Windows.Forms.TextBox();
            this.PINDigit_TextBox6 = new System.Windows.Forms.TextBox();
            this.PINDigit_TextBox5 = new System.Windows.Forms.TextBox();
            this.PINDigit_TextBox4 = new System.Windows.Forms.TextBox();
            this.PINDigit_TextBox3 = new System.Windows.Forms.TextBox();
            this.PINDigit_TextBox2 = new System.Windows.Forms.TextBox();
            this.PINDigit_TextBox1 = new System.Windows.Forms.TextBox();
            this.insertPINLabel = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.CIEPairingDescriptionLabel = new System.Windows.Forms.Label();
            this.CIEPairingHeaderTextLabel = new System.Windows.Forms.Label();
            this.CIEPickerTabPage = new System.Windows.Forms.TabPage();
            this.signWithCIEWithoutPairingButton = new System.Windows.Forms.Button();
            this.selectSigningCIEButton = new System.Windows.Forms.Button();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.rightButton = new System.Windows.Forms.Button();
            this.leftButton = new System.Windows.Forms.Button();
            this.dotsGroup = new System.Windows.Forms.FlowLayoutPanel();
            this.tableLayoutPanelCarousel = new System.Windows.Forms.TableLayoutPanel();
            this.carouselItem0 = new System.Windows.Forms.FlowLayoutPanel();
            this.pictureBox2 = new System.Windows.Forms.PictureBox();
            this.labelCardNumber0 = new System.Windows.Forms.Label();
            this.labelCardNumberValue0 = new System.Windows.Forms.Label();
            this.labelOwner0 = new System.Windows.Forms.Label();
            this.labelOwnerValue0 = new System.Windows.Forms.Label();
            this.carouselItem1 = new System.Windows.Forms.FlowLayoutPanel();
            this.pictureBox12 = new System.Windows.Forms.PictureBox();
            this.label6 = new System.Windows.Forms.Label();
            this.labelCardNumberValue1 = new System.Windows.Forms.Label();
            this.label24 = new System.Windows.Forms.Label();
            this.labelOwnerValue1 = new System.Windows.Forms.Label();
            this.carouselItem2 = new System.Windows.Forms.FlowLayoutPanel();
            this.pictureBox14 = new System.Windows.Forms.PictureBox();
            this.label28 = new System.Windows.Forms.Label();
            this.labelCardNumberValue2 = new System.Windows.Forms.Label();
            this.label30 = new System.Windows.Forms.Label();
            this.labelOwnerValue2 = new System.Windows.Forms.Label();
            this.removeAllButton = new System.Windows.Forms.Button();
            this.addButton = new System.Windows.Forms.Button();
            this.deleteCIEButton = new System.Windows.Forms.Button();
            this.CIESuccessfullyPairedLabel = new System.Windows.Forms.Label();
            this.appNameCIEPickerHeaderTextLabel = new System.Windows.Forms.Label();
            this.CIEPairingInProgressTabPage = new System.Windows.Forms.TabPage();
            this.progressBar = new System.Windows.Forms.ProgressBar();
            this.labelProgressMessage = new System.Windows.Forms.Label();
            this.pictureBox3 = new System.Windows.Forms.PictureBox();
            this.labelPairingDescriptionPerformingOperation = new System.Windows.Forms.Label();
            this.labelPairYourCIE = new System.Windows.Forms.Label();
            this.tabPageChangePIN = new System.Windows.Forms.TabPage();
            this.labelPINPolicyDescription = new System.Windows.Forms.Label();
            this.changePINButton = new System.Windows.Forms.Button();
            this.textBoxNewPIN2 = new System.Windows.Forms.TextBox();
            this.labelRepeatNewPIN = new System.Windows.Forms.Label();
            this.textBoxNewPIN = new System.Windows.Forms.TextBox();
            this.labelTypeNewPIN = new System.Windows.Forms.Label();
            this.textBoxPIN = new System.Windows.Forms.TextBox();
            this.labelTypeOldPIN = new System.Windows.Forms.Label();
            this.pictureBox4 = new System.Windows.Forms.PictureBox();
            this.labelChangePINDescription = new System.Windows.Forms.Label();
            this.labelChangePINHeaderText = new System.Windows.Forms.Label();
            this.tabPageChangePINInProgress = new System.Windows.Forms.TabPage();
            this.progressBarChangePIN = new System.Windows.Forms.ProgressBar();
            this.progressLabelChangePIN = new System.Windows.Forms.Label();
            this.pictureBox5 = new System.Windows.Forms.PictureBox();
            this.labelInfoBoxAboutCIEPINSafety = new System.Windows.Forms.Label();
            this.labelChangePINPerformingOperationHeaderText = new System.Windows.Forms.Label();
            this.tabPageUnlockPIN = new System.Windows.Forms.TabPage();
            this.labelPINPolicyInPINUnlock = new System.Windows.Forms.Label();
            this.unlockPINButton = new System.Windows.Forms.Button();
            this.textBoxUnlockPIN2 = new System.Windows.Forms.TextBox();
            this.labelRepeatNewPINUnlockPIN = new System.Windows.Forms.Label();
            this.textBoxUnlockPIN = new System.Windows.Forms.TextBox();
            this.labelTypeNewPINUnlockPIN = new System.Windows.Forms.Label();
            this.textBoxPUK = new System.Windows.Forms.TextBox();
            this.labelTypePUK = new System.Windows.Forms.Label();
            this.pictureBox6 = new System.Windows.Forms.PictureBox();
            this.labelUnlockPINDescription = new System.Windows.Forms.Label();
            this.labelUnlockPINHeaderText = new System.Windows.Forms.Label();
            this.tabPageUnlockPINInProgress = new System.Windows.Forms.TabPage();
            this.progressBarUnlockPIN = new System.Windows.Forms.ProgressBar();
            this.progressLabelUnlockPIN = new System.Windows.Forms.Label();
            this.pictureBox7 = new System.Windows.Forms.PictureBox();
            this.labelUnlockPINDescriptionPerformingOperation = new System.Windows.Forms.Label();
            this.labelUnlockPINHeaderTextPerformingOperation = new System.Windows.Forms.Label();
            this.tabPageTutorialWebBrowser = new System.Windows.Forms.TabPage();
            this.webBrowserTutorial = new System.Windows.Forms.WebBrowser();
            this.tabPageHelpWebBrowser = new System.Windows.Forms.TabPage();
            this.pictureBox11 = new System.Windows.Forms.PictureBox();
            this.pictureBox13 = new System.Windows.Forms.PictureBox();
            this.webBrowserHelp = new System.Windows.Forms.WebBrowser();
            this.tabPageInformationWebBrowser = new System.Windows.Forms.TabPage();
            this.pictureBox10 = new System.Windows.Forms.PictureBox();
            this.pictureBox9 = new System.Windows.Forms.PictureBox();
            this.pictureBox8 = new System.Windows.Forms.PictureBox();
            this.webBrowserInfo = new System.Windows.Forms.WebBrowser();
            this.tabSelectFile = new System.Windows.Forms.TabPage();
            this.customizeGraphicSignatureLabel = new System.Windows.Forms.Label();
            this.pictureBox15 = new System.Windows.Forms.PictureBox();
            this.labelGraphicSignatureDescriptionInfoBox = new System.Windows.Forms.Label();
            this.panelChooseDoc = new System.Windows.Forms.Panel();
            this.selectDocument = new System.Windows.Forms.Button();
            this.labelChooseBetweenDragOrSelection = new System.Windows.Forms.Label();
            this.labelDragAndDropDocumentInformation = new System.Windows.Forms.Label();
            this.pbFolder = new System.Windows.Forms.PictureBox();
            this.digitalSignatureLabel = new System.Windows.Forms.Label();
            this.tabSelectOp = new System.Windows.Forms.TabPage();
            this.cancelOperationButton = new System.Windows.Forms.Button();
            this.panelChooseSignOrVerify = new System.Windows.Forms.Panel();
            this.signOperationOptionPanel = new System.Windows.Forms.Panel();
            this.signOptionChooserLabel = new System.Windows.Forms.Label();
            this.pictureBox17 = new System.Windows.Forms.PictureBox();
            this.verifyOptionPanel = new System.Windows.Forms.Panel();
            this.pictureBox18 = new System.Windows.Forms.PictureBox();
            this.verifyOptionChooserLabel = new System.Windows.Forms.Label();
            this.labelFileNamePathInOperationChooser = new System.Windows.Forms.Label();
            this.pictureBox16 = new System.Windows.Forms.PictureBox();
            this.labelDigitalSignatureHeaderTextChoosingOperation = new System.Windows.Forms.Label();
            this.tabSelectSignOp = new System.Windows.Forms.TabPage();
            this.proceedWithSignatureButton = new System.Windows.Forms.Button();
            this.labelDigitalSignatureDescriptionChoosingFormat = new System.Windows.Forms.Label();
            this.cancelSigningOperationButton = new System.Windows.Forms.Button();
            this.panelChooseSignatureType = new System.Windows.Forms.Panel();
            this.panelChoosePades = new System.Windows.Forms.Panel();
            this.enableGraphicSignatureCheckBox = new System.Windows.Forms.CheckBox();
            this.PAdESFormatDescriptionLabel = new System.Windows.Forms.Label();
            this.PAdESSignatureLabel = new System.Windows.Forms.Label();
            this.pbPades = new System.Windows.Forms.PictureBox();
            this.labelSelectSignatureFormatType = new System.Windows.Forms.Label();
            this.panelChooseCades = new System.Windows.Forms.Panel();
            this.CAdESFormatDescriptionLabel = new System.Windows.Forms.Label();
            this.CAdESSignatureLabel = new System.Windows.Forms.Label();
            this.CAdESP7MPictureBox = new System.Windows.Forms.PictureBox();
            this.labelFileNamePathInSigningFormatChooser = new System.Windows.Forms.Label();
            this.pictureBox21 = new System.Windows.Forms.PictureBox();
            this.labelDigitalSignatureHeaderTextChoosingFormat = new System.Windows.Forms.Label();
            this.graphicDigitalSignatureTab = new System.Windows.Forms.TabPage();
            this.labelDigitalSignatureDragSignatureBox = new System.Windows.Forms.Label();
            this.panelPreviewGraphicSignature = new System.Windows.Forms.Panel();
            this.downButton = new System.Windows.Forms.Button();
            this.upButton = new System.Windows.Forms.Button();
            this.panePreview = new System.Windows.Forms.Panel();
            this.labelFileNamePathInDragSignatureBox = new System.Windows.Forms.Label();
            this.pictureBox22 = new System.Windows.Forms.PictureBox();
            this.labelDigitalSignaturePlaceGraphicSignatureHeaderText = new System.Windows.Forms.Label();
            this.proceedSignPreviewButton = new System.Windows.Forms.Button();
            this.signPanelTabPage = new System.Windows.Forms.TabPage();
            this.closeButton = new System.Windows.Forms.Button();
            this.signButton = new System.Windows.Forms.Button();
            this.cancelSigningOperationPINPanelButton = new System.Windows.Forms.Button();
            this.placeCIEOnReaderLabel = new System.Windows.Forms.Label();
            this.panelTypePINAndSign = new System.Windows.Forms.Panel();
            this.panelSigningBoxPINArea = new System.Windows.Forms.Panel();
            this.pictureBox20 = new System.Windows.Forms.PictureBox();
            this.typePINLabel = new System.Windows.Forms.Label();
            this.documentSuccessfullySignedLabel = new System.Windows.Forms.Label();
            this.signPINDigit_TextBox1 = new System.Windows.Forms.TextBox();
            this.signProgressBar = new System.Windows.Forms.ProgressBar();
            this.signPINDigit_TextBox2 = new System.Windows.Forms.TextBox();
            this.digitalSignatureCompletedPictureBox = new System.Windows.Forms.PictureBox();
            this.signPINDigit_TextBox3 = new System.Windows.Forms.TextBox();
            this.signPINDigit_TextBox4 = new System.Windows.Forms.TextBox();
            this.signPINDigit_TextBox5 = new System.Windows.Forms.TextBox();
            this.signPINDigit_TextBox6 = new System.Windows.Forms.TextBox();
            this.signPINDigit_TextBox7 = new System.Windows.Forms.TextBox();
            this.signPINDigit_TextBox8 = new System.Windows.Forms.TextBox();
            this.labelFileNamePathSigningOperation = new System.Windows.Forms.Label();
            this.pictureBox19 = new System.Windows.Forms.PictureBox();
            this.labelDigitalSignatureHeaderText = new System.Windows.Forms.Label();
            this.graphicSignatureCustomizationTab = new System.Windows.Forms.TabPage();
            this.generateCustomGraphicSignatureButton = new System.Windows.Forms.Button();
            this.panelDefineYourGraphicSignature = new System.Windows.Forms.Panel();
            this.graphicDigitalSignaturePanel = new System.Windows.Forms.Panel();
            this.graphicSignatureCustomizationOverviewLabel = new System.Windows.Forms.Label();
            this.labelGraphicSignatureSecondDescription = new System.Windows.Forms.Label();
            this.selectFileForGraphicSignatureCustomizationButton = new System.Windows.Forms.Button();
            this.cancelCustomizationButton = new System.Windows.Forms.Button();
            this.labelDigitalSignatureHeaderTextCreatingGraphicSignature = new System.Windows.Forms.Label();
            this.documentVerifyTab = new System.Windows.Forms.TabPage();
            this.extractP7MButton = new System.Windows.Forms.Button();
            this.verifyPanel = new System.Windows.Forms.Panel();
            this.pnSignerInfo = new System.Windows.Forms.FlowLayoutPanel();
            this.signersCounterLabel = new System.Windows.Forms.Label();
            this.pictureBox23 = new System.Windows.Forms.PictureBox();
            this.fileNamePathVerifyLabel = new System.Windows.Forms.Label();
            this.verifyLoadingLabel = new System.Windows.Forms.Label();
            this.closeVerifyButton = new System.Windows.Forms.Button();
            this.labelVerifySignature = new System.Windows.Forms.Label();
            this.labelDigitalSignatureHeaderTextVerifyDocument = new System.Windows.Forms.Label();
            this.tabSettings = new System.Windows.Forms.TabPage();
            this.tabControlLogConfigurationOptions = new System.Windows.Forms.TabControl();
            this.tabConfigProxy = new System.Windows.Forms.TabPage();
            this.panelProxyConfig = new System.Windows.Forms.Panel();
            this.labelPortProxyConfiguration = new System.Windows.Forms.Label();
            this.txtPort = new System.Windows.Forms.TextBox();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.showPasswordCheckBox = new System.Windows.Forms.CheckBox();
            this.labelIPAddressProxyConfiguration = new System.Windows.Forms.Label();
            this.labelPasswordProxyConfiguration = new System.Windows.Forms.Label();
            this.txtUrl = new System.Windows.Forms.TextBox();
            this.labelUsernameProxyConfiguration = new System.Windows.Forms.Label();
            this.txtUsername = new System.Windows.Forms.TextBox();
            this.labelProxyConfigurationHeaderText = new System.Windows.Forms.Label();
            this.labelProxyConfigurationDescription = new System.Windows.Forms.Label();
            this.tabConfigLogging = new System.Windows.Forms.TabPage();
            this.labelLogConfigurationHeaderText = new System.Windows.Forms.Label();
            this.labelLogConfigurationDescription = new System.Windows.Forms.Label();
            this.panelLogConfiguration = new System.Windows.Forms.Panel();
            this.configLibraryLoggingGroupBox = new System.Windows.Forms.GroupBox();
            this.rbLoggingLibDebug = new System.Windows.Forms.RadioButton();
            this.rbLoggingLibInfo = new System.Windows.Forms.RadioButton();
            this.rbLoggingLibError = new System.Windows.Forms.RadioButton();
            this.rbLoggingLibNone = new System.Windows.Forms.RadioButton();
            this.configApplicationLoggingGroupBox = new System.Windows.Forms.GroupBox();
            this.rbLoggingAppError = new System.Windows.Forms.RadioButton();
            this.rbLoggingAppDebug = new System.Windows.Forms.RadioButton();
            this.rbLoggingAppInfo = new System.Windows.Forms.RadioButton();
            this.rbLoggingAppNone = new System.Windows.Forms.RadioButton();
            this.panelSetButtonsInOptions = new System.Windows.Forms.Panel();
            this.editSettingsButton = new System.Windows.Forms.Button();
            this.saveSettingsButton = new System.Windows.Forms.Button();
            this.menuHomeButton = new System.Windows.Forms.Button();
            this.pictureBoxLogo = new System.Windows.Forms.PictureBox();
            this.menuChangePINButton = new System.Windows.Forms.Button();
            this.menuUnlockPINButton = new System.Windows.Forms.Button();
            this.menuTutorialButton = new System.Windows.Forms.Button();
            this.menuHelpButton = new System.Windows.Forms.Button();
            this.menuInformationButton = new System.Windows.Forms.Button();
            this.menuDigitalSignatureButton = new System.Windows.Forms.Button();
            this.menuVerifyButton = new System.Windows.Forms.Button();
            this.menuSettingsButton = new System.Windows.Forms.Button();
            this.mainTabControl.SuspendLayout();
            this.CIEPairingTabPage.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.CIEPickerTabPage.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.tableLayoutPanelCarousel.SuspendLayout();
            this.carouselItem0.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
            this.carouselItem1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox12)).BeginInit();
            this.carouselItem2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox14)).BeginInit();
            this.CIEPairingInProgressTabPage.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox3)).BeginInit();
            this.tabPageChangePIN.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox4)).BeginInit();
            this.tabPageChangePINInProgress.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox5)).BeginInit();
            this.tabPageUnlockPIN.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox6)).BeginInit();
            this.tabPageUnlockPINInProgress.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox7)).BeginInit();
            this.tabPageTutorialWebBrowser.SuspendLayout();
            this.tabPageHelpWebBrowser.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox11)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox13)).BeginInit();
            this.tabPageInformationWebBrowser.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox10)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox9)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox8)).BeginInit();
            this.tabSelectFile.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox15)).BeginInit();
            this.panelChooseDoc.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbFolder)).BeginInit();
            this.tabSelectOp.SuspendLayout();
            this.panelChooseSignOrVerify.SuspendLayout();
            this.signOperationOptionPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox17)).BeginInit();
            this.verifyOptionPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox18)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox16)).BeginInit();
            this.tabSelectSignOp.SuspendLayout();
            this.panelChooseSignatureType.SuspendLayout();
            this.panelChoosePades.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbPades)).BeginInit();
            this.panelChooseCades.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CAdESP7MPictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox21)).BeginInit();
            this.graphicDigitalSignatureTab.SuspendLayout();
            this.panelPreviewGraphicSignature.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox22)).BeginInit();
            this.signPanelTabPage.SuspendLayout();
            this.panelTypePINAndSign.SuspendLayout();
            this.panelSigningBoxPINArea.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox20)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.digitalSignatureCompletedPictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox19)).BeginInit();
            this.graphicSignatureCustomizationTab.SuspendLayout();
            this.panelDefineYourGraphicSignature.SuspendLayout();
            this.documentVerifyTab.SuspendLayout();
            this.verifyPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox23)).BeginInit();
            this.tabSettings.SuspendLayout();
            this.tabControlLogConfigurationOptions.SuspendLayout();
            this.tabConfigProxy.SuspendLayout();
            this.panelProxyConfig.SuspendLayout();
            this.tabConfigLogging.SuspendLayout();
            this.panelLogConfiguration.SuspendLayout();
            this.configLibraryLoggingGroupBox.SuspendLayout();
            this.configApplicationLoggingGroupBox.SuspendLayout();
            this.panelSetButtonsInOptions.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxLogo)).BeginInit();
            this.SuspendLayout();
            // 
            // mainTabControl
            // 
            this.mainTabControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mainTabControl.Appearance = System.Windows.Forms.TabAppearance.Buttons;
            this.mainTabControl.Controls.Add(this.CIEPairingTabPage);
            this.mainTabControl.Controls.Add(this.CIEPickerTabPage);
            this.mainTabControl.Controls.Add(this.CIEPairingInProgressTabPage);
            this.mainTabControl.Controls.Add(this.tabPageChangePIN);
            this.mainTabControl.Controls.Add(this.tabPageChangePINInProgress);
            this.mainTabControl.Controls.Add(this.tabPageUnlockPIN);
            this.mainTabControl.Controls.Add(this.tabPageUnlockPINInProgress);
            this.mainTabControl.Controls.Add(this.tabPageTutorialWebBrowser);
            this.mainTabControl.Controls.Add(this.tabPageHelpWebBrowser);
            this.mainTabControl.Controls.Add(this.tabPageInformationWebBrowser);
            this.mainTabControl.Controls.Add(this.tabSelectFile);
            this.mainTabControl.Controls.Add(this.tabSelectOp);
            this.mainTabControl.Controls.Add(this.tabSelectSignOp);
            this.mainTabControl.Controls.Add(this.graphicDigitalSignatureTab);
            this.mainTabControl.Controls.Add(this.signPanelTabPage);
            this.mainTabControl.Controls.Add(this.graphicSignatureCustomizationTab);
            this.mainTabControl.Controls.Add(this.documentVerifyTab);
            this.mainTabControl.Controls.Add(this.tabSettings);
            this.mainTabControl.ItemSize = new System.Drawing.Size(0, 1);
            this.mainTabControl.Location = new System.Drawing.Point(148, -11);
            this.mainTabControl.Margin = new System.Windows.Forms.Padding(0);
            this.mainTabControl.Multiline = true;
            this.mainTabControl.Name = "mainTabControl";
            this.mainTabControl.SelectedIndex = 0;
            this.mainTabControl.Size = new System.Drawing.Size(657, 546);
            this.mainTabControl.TabIndex = 0;
            this.mainTabControl.SelectedIndexChanged += new System.EventHandler(this.mainTabControl_SelectedIndexChanged);
            // 
            // CIEPairingTabPage
            // 
            this.CIEPairingTabPage.BackColor = System.Drawing.Color.White;
            this.CIEPairingTabPage.Controls.Add(this.cancelButton);
            this.CIEPairingTabPage.Controls.Add(this.pairButton);
            this.CIEPairingTabPage.Controls.Add(this.PINDigit_TextBox8);
            this.CIEPairingTabPage.Controls.Add(this.PINDigit_TextBox7);
            this.CIEPairingTabPage.Controls.Add(this.PINDigit_TextBox6);
            this.CIEPairingTabPage.Controls.Add(this.PINDigit_TextBox5);
            this.CIEPairingTabPage.Controls.Add(this.PINDigit_TextBox4);
            this.CIEPairingTabPage.Controls.Add(this.PINDigit_TextBox3);
            this.CIEPairingTabPage.Controls.Add(this.PINDigit_TextBox2);
            this.CIEPairingTabPage.Controls.Add(this.PINDigit_TextBox1);
            this.CIEPairingTabPage.Controls.Add(this.insertPINLabel);
            this.CIEPairingTabPage.Controls.Add(this.pictureBox1);
            this.CIEPairingTabPage.Controls.Add(this.CIEPairingDescriptionLabel);
            this.CIEPairingTabPage.Controls.Add(this.CIEPairingHeaderTextLabel);
            this.CIEPairingTabPage.Location = new System.Drawing.Point(4, 9);
            this.CIEPairingTabPage.Margin = new System.Windows.Forms.Padding(2);
            this.CIEPairingTabPage.Name = "CIEPairingTabPage";
            this.CIEPairingTabPage.Padding = new System.Windows.Forms.Padding(2);
            this.CIEPairingTabPage.Size = new System.Drawing.Size(649, 533);
            this.CIEPairingTabPage.TabIndex = 0;
            this.CIEPairingTabPage.Text = "tabPage1";
            // 
            // cancelButton
            // 
            this.cancelButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.cancelButton.FlatAppearance.BorderSize = 0;
            this.cancelButton.ForeColor = System.Drawing.Color.White;
            this.cancelButton.Location = new System.Drawing.Point(101, 484);
            this.cancelButton.Margin = new System.Windows.Forms.Padding(2);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(156, 30);
            this.cancelButton.TabIndex = 21;
            this.cancelButton.Text = "Annulla";
            this.cancelButton.UseVisualStyleBackColor = false;
            this.cancelButton.Click += new System.EventHandler(this.CancelButton_Click);
            // 
            // pairButton
            // 
            this.pairButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.pairButton.FlatAppearance.BorderSize = 0;
            this.pairButton.ForeColor = System.Drawing.Color.White;
            this.pairButton.Location = new System.Drawing.Point(392, 484);
            this.pairButton.Margin = new System.Windows.Forms.Padding(2);
            this.pairButton.Name = "pairButton";
            this.pairButton.Size = new System.Drawing.Size(156, 30);
            this.pairButton.TabIndex = 20;
            this.pairButton.Text = "Abbina";
            this.pairButton.UseVisualStyleBackColor = false;
            this.pairButton.Click += new System.EventHandler(this.PairButton_Click);
            // 
            // PINDigit_TextBox8
            // 
            this.PINDigit_TextBox8.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.PINDigit_TextBox8.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PINDigit_TextBox8.Location = new System.Drawing.Point(528, 300);
            this.PINDigit_TextBox8.Margin = new System.Windows.Forms.Padding(2);
            this.PINDigit_TextBox8.Name = "PINDigit_TextBox8";
            this.PINDigit_TextBox8.Size = new System.Drawing.Size(20, 23);
            this.PINDigit_TextBox8.TabIndex = 8;
            this.PINDigit_TextBox8.Tag = "8";
            this.PINDigit_TextBox8.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.PINDigit_TextBox8.UseSystemPasswordChar = true;
            this.PINDigit_TextBox8.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PINDigit_TextBox_KeyPress);
            // 
            // PINDigit_TextBox7
            // 
            this.PINDigit_TextBox7.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.PINDigit_TextBox7.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PINDigit_TextBox7.Location = new System.Drawing.Point(504, 300);
            this.PINDigit_TextBox7.Margin = new System.Windows.Forms.Padding(2);
            this.PINDigit_TextBox7.Name = "PINDigit_TextBox7";
            this.PINDigit_TextBox7.Size = new System.Drawing.Size(20, 23);
            this.PINDigit_TextBox7.TabIndex = 7;
            this.PINDigit_TextBox7.Tag = "7";
            this.PINDigit_TextBox7.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.PINDigit_TextBox7.UseSystemPasswordChar = true;
            this.PINDigit_TextBox7.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PINDigit_TextBox_KeyPress);
            // 
            // PINDigit_TextBox6
            // 
            this.PINDigit_TextBox6.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.PINDigit_TextBox6.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PINDigit_TextBox6.Location = new System.Drawing.Point(480, 300);
            this.PINDigit_TextBox6.Margin = new System.Windows.Forms.Padding(2);
            this.PINDigit_TextBox6.Name = "PINDigit_TextBox6";
            this.PINDigit_TextBox6.Size = new System.Drawing.Size(20, 23);
            this.PINDigit_TextBox6.TabIndex = 6;
            this.PINDigit_TextBox6.Tag = "6";
            this.PINDigit_TextBox6.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.PINDigit_TextBox6.UseSystemPasswordChar = true;
            this.PINDigit_TextBox6.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PINDigit_TextBox_KeyPress);
            // 
            // PINDigit_TextBox5
            // 
            this.PINDigit_TextBox5.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.PINDigit_TextBox5.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PINDigit_TextBox5.Location = new System.Drawing.Point(456, 300);
            this.PINDigit_TextBox5.Margin = new System.Windows.Forms.Padding(2);
            this.PINDigit_TextBox5.Name = "PINDigit_TextBox5";
            this.PINDigit_TextBox5.Size = new System.Drawing.Size(20, 23);
            this.PINDigit_TextBox5.TabIndex = 5;
            this.PINDigit_TextBox5.Tag = "5";
            this.PINDigit_TextBox5.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.PINDigit_TextBox5.UseSystemPasswordChar = true;
            this.PINDigit_TextBox5.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PINDigit_TextBox_KeyPress);
            // 
            // PINDigit_TextBox4
            // 
            this.PINDigit_TextBox4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.PINDigit_TextBox4.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PINDigit_TextBox4.Location = new System.Drawing.Point(432, 300);
            this.PINDigit_TextBox4.Margin = new System.Windows.Forms.Padding(2);
            this.PINDigit_TextBox4.Name = "PINDigit_TextBox4";
            this.PINDigit_TextBox4.Size = new System.Drawing.Size(20, 23);
            this.PINDigit_TextBox4.TabIndex = 4;
            this.PINDigit_TextBox4.Tag = "4";
            this.PINDigit_TextBox4.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.PINDigit_TextBox4.UseSystemPasswordChar = true;
            this.PINDigit_TextBox4.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PINDigit_TextBox_KeyPress);
            // 
            // PINDigit_TextBox3
            // 
            this.PINDigit_TextBox3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.PINDigit_TextBox3.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PINDigit_TextBox3.Location = new System.Drawing.Point(408, 300);
            this.PINDigit_TextBox3.Margin = new System.Windows.Forms.Padding(2);
            this.PINDigit_TextBox3.Name = "PINDigit_TextBox3";
            this.PINDigit_TextBox3.Size = new System.Drawing.Size(20, 23);
            this.PINDigit_TextBox3.TabIndex = 3;
            this.PINDigit_TextBox3.Tag = "3";
            this.PINDigit_TextBox3.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.PINDigit_TextBox3.UseSystemPasswordChar = true;
            this.PINDigit_TextBox3.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PINDigit_TextBox_KeyPress);
            // 
            // PINDigit_TextBox2
            // 
            this.PINDigit_TextBox2.BackColor = System.Drawing.SystemColors.Window;
            this.PINDigit_TextBox2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.PINDigit_TextBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PINDigit_TextBox2.Location = new System.Drawing.Point(384, 300);
            this.PINDigit_TextBox2.Margin = new System.Windows.Forms.Padding(2);
            this.PINDigit_TextBox2.Name = "PINDigit_TextBox2";
            this.PINDigit_TextBox2.Size = new System.Drawing.Size(20, 23);
            this.PINDigit_TextBox2.TabIndex = 2;
            this.PINDigit_TextBox2.Tag = "2";
            this.PINDigit_TextBox2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.PINDigit_TextBox2.UseSystemPasswordChar = true;
            this.PINDigit_TextBox2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PINDigit_TextBox_KeyPress);
            // 
            // PINDigit_TextBox1
            // 
            this.PINDigit_TextBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PINDigit_TextBox1.ForeColor = System.Drawing.SystemColors.WindowText;
            this.PINDigit_TextBox1.Location = new System.Drawing.Point(360, 300);
            this.PINDigit_TextBox1.Margin = new System.Windows.Forms.Padding(2);
            this.PINDigit_TextBox1.Name = "PINDigit_TextBox1";
            this.PINDigit_TextBox1.Size = new System.Drawing.Size(20, 23);
            this.PINDigit_TextBox1.TabIndex = 1;
            this.PINDigit_TextBox1.Tag = "1";
            this.PINDigit_TextBox1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.PINDigit_TextBox1.UseSystemPasswordChar = true;
            this.PINDigit_TextBox1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PINDigit_TextBox_KeyPress);
            // 
            // insertPINLabel
            // 
            this.insertPINLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.insertPINLabel.Location = new System.Drawing.Point(374, 249);
            this.insertPINLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.insertPINLabel.Name = "insertPINLabel";
            this.insertPINLabel.Size = new System.Drawing.Size(156, 20);
            this.insertPINLabel.TabIndex = 11;
            this.insertPINLabel.Text = "Inserisci il PIN";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(89, 195);
            this.pictureBox1.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(207, 193);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox1.TabIndex = 10;
            this.pictureBox1.TabStop = false;
            // 
            // CIEPairingDescriptionLabel
            // 
            this.CIEPairingDescriptionLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CIEPairingDescriptionLabel.Location = new System.Drawing.Point(172, 85);
            this.CIEPairingDescriptionLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.CIEPairingDescriptionLabel.Name = "CIEPairingDescriptionLabel";
            this.CIEPairingDescriptionLabel.Size = new System.Drawing.Size(394, 45);
            this.CIEPairingDescriptionLabel.TabIndex = 9;
            this.CIEPairingDescriptionLabel.Text = "Dopo aver collegato e installato il lettore di smart card, posiziona la CIE sul l" +
    "ettore ed inserisci il PIN";
            this.CIEPairingDescriptionLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // CIEPairingHeaderTextLabel
            // 
            this.CIEPairingHeaderTextLabel.AutoSize = true;
            this.CIEPairingHeaderTextLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CIEPairingHeaderTextLabel.Location = new System.Drawing.Point(235, 49);
            this.CIEPairingHeaderTextLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.CIEPairingHeaderTextLabel.Name = "CIEPairingHeaderTextLabel";
            this.CIEPairingHeaderTextLabel.Size = new System.Drawing.Size(264, 36);
            this.CIEPairingHeaderTextLabel.TabIndex = 8;
            this.CIEPairingHeaderTextLabel.Text = "Abbina la tua CIE";
            // 
            // CIEPickerTabPage
            // 
            this.CIEPickerTabPage.BackColor = System.Drawing.Color.White;
            this.CIEPickerTabPage.Controls.Add(this.signWithCIEWithoutPairingButton);
            this.CIEPickerTabPage.Controls.Add(this.selectSigningCIEButton);
            this.CIEPickerTabPage.Controls.Add(this.tableLayoutPanel1);
            this.CIEPickerTabPage.Controls.Add(this.removeAllButton);
            this.CIEPickerTabPage.Controls.Add(this.addButton);
            this.CIEPickerTabPage.Controls.Add(this.deleteCIEButton);
            this.CIEPickerTabPage.Controls.Add(this.CIESuccessfullyPairedLabel);
            this.CIEPickerTabPage.Controls.Add(this.appNameCIEPickerHeaderTextLabel);
            this.CIEPickerTabPage.Location = new System.Drawing.Point(4, 9);
            this.CIEPickerTabPage.Margin = new System.Windows.Forms.Padding(2);
            this.CIEPickerTabPage.Name = "CIEPickerTabPage";
            this.CIEPickerTabPage.Size = new System.Drawing.Size(649, 533);
            this.CIEPickerTabPage.TabIndex = 1;
            this.CIEPickerTabPage.Text = "tabPage2";
            // 
            // signWithCIEWithoutPairingButton
            // 
            this.signWithCIEWithoutPairingButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.signWithCIEWithoutPairingButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.signWithCIEWithoutPairingButton.FlatAppearance.BorderSize = 0;
            this.signWithCIEWithoutPairingButton.ForeColor = System.Drawing.Color.White;
            this.signWithCIEWithoutPairingButton.Location = new System.Drawing.Point(338, 502);
            this.signWithCIEWithoutPairingButton.Margin = new System.Windows.Forms.Padding(2);
            this.signWithCIEWithoutPairingButton.Name = "signWithCIEWithoutPairingButton";
            this.signWithCIEWithoutPairingButton.Size = new System.Drawing.Size(200, 28);
            this.signWithCIEWithoutPairingButton.TabIndex = 23;
            this.signWithCIEWithoutPairingButton.Text = "Firma con CIE non associata";
            this.signWithCIEWithoutPairingButton.UseVisualStyleBackColor = false;
            this.signWithCIEWithoutPairingButton.Visible = false;
            this.signWithCIEWithoutPairingButton.Click += new System.EventHandler(this.SignWithCIEWithoutPairingButton_Click);
            // 
            // selectSigningCIEButton
            // 
            this.selectSigningCIEButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.selectSigningCIEButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.selectSigningCIEButton.FlatAppearance.BorderSize = 0;
            this.selectSigningCIEButton.ForeColor = System.Drawing.Color.White;
            this.selectSigningCIEButton.Location = new System.Drawing.Point(122, 502);
            this.selectSigningCIEButton.Margin = new System.Windows.Forms.Padding(2);
            this.selectSigningCIEButton.Name = "selectSigningCIEButton";
            this.selectSigningCIEButton.Size = new System.Drawing.Size(200, 28);
            this.selectSigningCIEButton.TabIndex = 22;
            this.selectSigningCIEButton.Text = "Firma con la CIE selezionata";
            this.selectSigningCIEButton.UseVisualStyleBackColor = false;
            this.selectSigningCIEButton.Visible = false;
            this.selectSigningCIEButton.Click += new System.EventHandler(this.SelectSigningCIEButton_Click);
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel1.AutoSize = true;
            this.tableLayoutPanel1.ColumnCount = 3;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 8.728943F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 83.61409F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 7.810107F));
            this.tableLayoutPanel1.Controls.Add(this.rightButton, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.leftButton, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.dotsGroup, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.tableLayoutPanelCarousel, 1, 0);
            this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 101);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 27F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(643, 392);
            this.tableLayoutPanel1.TabIndex = 21;
            // 
            // rightButton
            // 
            this.rightButton.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.rightButton.FlatAppearance.BorderSize = 0;
            this.rightButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.rightButton.Image = global::CIEID.Properties.Resources.forward_3x;
            this.rightButton.Location = new System.Drawing.Point(599, 161);
            this.rightButton.Name = "rightButton";
            this.rightButton.Size = new System.Drawing.Size(36, 43);
            this.rightButton.TabIndex = 3;
            this.rightButton.UseVisualStyleBackColor = true;
            this.rightButton.Click += new System.EventHandler(this.RightButton_Click);
            // 
            // leftButton
            // 
            this.leftButton.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.leftButton.FlatAppearance.BorderSize = 0;
            this.leftButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.leftButton.Image = global::CIEID.Properties.Resources.back_3x;
            this.leftButton.Location = new System.Drawing.Point(10, 161);
            this.leftButton.Name = "leftButton";
            this.leftButton.Size = new System.Drawing.Size(36, 43);
            this.leftButton.TabIndex = 2;
            this.leftButton.UseVisualStyleBackColor = true;
            this.leftButton.Click += new System.EventHandler(this.LeftButton_Click);
            // 
            // dotsGroup
            // 
            this.dotsGroup.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.dotsGroup.AutoSize = true;
            this.dotsGroup.Location = new System.Drawing.Point(324, 378);
            this.dotsGroup.Name = "dotsGroup";
            this.dotsGroup.Size = new System.Drawing.Size(0, 0);
            this.dotsGroup.TabIndex = 5;
            // 
            // tableLayoutPanelCarousel
            // 
            this.tableLayoutPanelCarousel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanelCarousel.AutoSize = true;
            this.tableLayoutPanelCarousel.BackColor = System.Drawing.Color.White;
            this.tableLayoutPanelCarousel.ColumnCount = 3;
            this.tableLayoutPanelCarousel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 28.57143F));
            this.tableLayoutPanelCarousel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 43.41373F));
            this.tableLayoutPanelCarousel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 27.88991F));
            this.tableLayoutPanelCarousel.Controls.Add(this.carouselItem0, 0, 0);
            this.tableLayoutPanelCarousel.Controls.Add(this.carouselItem1, 1, 0);
            this.tableLayoutPanelCarousel.Controls.Add(this.carouselItem2, 2, 0);
            this.tableLayoutPanelCarousel.Location = new System.Drawing.Point(59, 3);
            this.tableLayoutPanelCarousel.Name = "tableLayoutPanelCarousel";
            this.tableLayoutPanelCarousel.RowCount = 1;
            this.tableLayoutPanelCarousel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelCarousel.Size = new System.Drawing.Size(530, 359);
            this.tableLayoutPanelCarousel.TabIndex = 4;
            // 
            // carouselItem0
            // 
            this.carouselItem0.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.carouselItem0.Controls.Add(this.pictureBox2);
            this.carouselItem0.Controls.Add(this.labelCardNumber0);
            this.carouselItem0.Controls.Add(this.labelCardNumberValue0);
            this.carouselItem0.Controls.Add(this.labelOwner0);
            this.carouselItem0.Controls.Add(this.labelOwnerValue0);
            this.carouselItem0.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
            this.carouselItem0.Location = new System.Drawing.Point(4, 74);
            this.carouselItem0.Name = "carouselItem0";
            this.carouselItem0.Padding = new System.Windows.Forms.Padding(0, 12, 0, 12);
            this.carouselItem0.Size = new System.Drawing.Size(142, 211);
            this.carouselItem0.TabIndex = 0;
            // 
            // pictureBox2
            // 
            this.pictureBox2.Image = global::CIEID.Properties.Resources.cie;
            this.pictureBox2.Location = new System.Drawing.Point(3, 15);
            this.pictureBox2.Name = "pictureBox2";
            this.pictureBox2.Size = new System.Drawing.Size(134, 84);
            this.pictureBox2.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox2.TabIndex = 0;
            this.pictureBox2.TabStop = false;
            // 
            // labelCardNumber0
            // 
            this.labelCardNumber0.AutoSize = true;
            this.labelCardNumber0.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F);
            this.labelCardNumber0.ForeColor = System.Drawing.Color.LightGray;
            this.labelCardNumber0.Location = new System.Drawing.Point(3, 117);
            this.labelCardNumber0.Margin = new System.Windows.Forms.Padding(3, 15, 3, 0);
            this.labelCardNumber0.Name = "labelCardNumber0";
            this.labelCardNumber0.Padding = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.labelCardNumber0.Size = new System.Drawing.Size(74, 12);
            this.labelCardNumber0.TabIndex = 3;
            this.labelCardNumber0.Text = "Numero carta:";
            // 
            // labelCardNumberValue0
            // 
            this.labelCardNumberValue0.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelCardNumberValue0.ForeColor = System.Drawing.Color.LightGray;
            this.labelCardNumberValue0.Location = new System.Drawing.Point(3, 129);
            this.labelCardNumberValue0.Name = "labelCardNumberValue0";
            this.labelCardNumberValue0.Padding = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.labelCardNumberValue0.Size = new System.Drawing.Size(118, 16);
            this.labelCardNumberValue0.TabIndex = 1;
            this.labelCardNumberValue0.Text = "label6";
            // 
            // labelOwner0
            // 
            this.labelOwner0.AutoSize = true;
            this.labelOwner0.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F);
            this.labelOwner0.ForeColor = System.Drawing.Color.LightGray;
            this.labelOwner0.Location = new System.Drawing.Point(3, 150);
            this.labelOwner0.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.labelOwner0.Name = "labelOwner0";
            this.labelOwner0.Padding = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.labelOwner0.Size = new System.Drawing.Size(65, 12);
            this.labelOwner0.TabIndex = 2;
            this.labelOwner0.Text = "Intestatario:";
            // 
            // labelOwnerValue0
            // 
            this.labelOwnerValue0.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold);
            this.labelOwnerValue0.ForeColor = System.Drawing.Color.LightGray;
            this.labelOwnerValue0.Location = new System.Drawing.Point(3, 162);
            this.labelOwnerValue0.Name = "labelOwnerValue0";
            this.labelOwnerValue0.Padding = new System.Windows.Forms.Padding(4, 0, 0, 0);
            this.labelOwnerValue0.Size = new System.Drawing.Size(148, 30);
            this.labelOwnerValue0.TabIndex = 4;
            this.labelOwnerValue0.Text = "label27";
            // 
            // carouselItem1
            // 
            this.carouselItem1.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.carouselItem1.Controls.Add(this.pictureBox12);
            this.carouselItem1.Controls.Add(this.label6);
            this.carouselItem1.Controls.Add(this.labelCardNumberValue1);
            this.carouselItem1.Controls.Add(this.label24);
            this.carouselItem1.Controls.Add(this.labelOwnerValue1);
            this.carouselItem1.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
            this.carouselItem1.Location = new System.Drawing.Point(156, 38);
            this.carouselItem1.Name = "carouselItem1";
            this.carouselItem1.Padding = new System.Windows.Forms.Padding(0, 12, 0, 12);
            this.carouselItem1.Size = new System.Drawing.Size(219, 282);
            this.carouselItem1.TabIndex = 1;
            // 
            // pictureBox12
            // 
            this.pictureBox12.Image = global::CIEID.Properties.Resources.cie;
            this.pictureBox12.Location = new System.Drawing.Point(3, 15);
            this.pictureBox12.Name = "pictureBox12";
            this.pictureBox12.Size = new System.Drawing.Size(215, 136);
            this.pictureBox12.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox12.TabIndex = 0;
            this.pictureBox12.TabStop = false;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(3, 169);
            this.label6.Margin = new System.Windows.Forms.Padding(3, 15, 3, 0);
            this.label6.Name = "label6";
            this.label6.Padding = new System.Windows.Forms.Padding(10, 0, 10, 0);
            this.label6.Size = new System.Drawing.Size(105, 15);
            this.label6.TabIndex = 3;
            this.label6.Text = "Numero carta:";
            // 
            // labelCardNumberValue1
            // 
            this.labelCardNumberValue1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelCardNumberValue1.Location = new System.Drawing.Point(3, 184);
            this.labelCardNumberValue1.Name = "labelCardNumberValue1";
            this.labelCardNumberValue1.Padding = new System.Windows.Forms.Padding(10, 0, 10, 0);
            this.labelCardNumberValue1.Size = new System.Drawing.Size(228, 20);
            this.labelCardNumberValue1.TabIndex = 1;
            this.labelCardNumberValue1.Text = "label6";
            // 
            // label24
            // 
            this.label24.AutoSize = true;
            this.label24.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label24.Location = new System.Drawing.Point(3, 209);
            this.label24.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.label24.Name = "label24";
            this.label24.Padding = new System.Windows.Forms.Padding(10, 0, 10, 0);
            this.label24.Size = new System.Drawing.Size(90, 15);
            this.label24.TabIndex = 2;
            this.label24.Text = "Intestatario:";
            // 
            // labelOwnerValue1
            // 
            this.labelOwnerValue1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelOwnerValue1.Location = new System.Drawing.Point(3, 224);
            this.labelOwnerValue1.Name = "labelOwnerValue1";
            this.labelOwnerValue1.Padding = new System.Windows.Forms.Padding(10, 0, 5, 0);
            this.labelOwnerValue1.Size = new System.Drawing.Size(244, 41);
            this.labelOwnerValue1.TabIndex = 4;
            this.labelOwnerValue1.Text = "labelOwner";
            // 
            // carouselItem2
            // 
            this.carouselItem2.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.carouselItem2.Controls.Add(this.pictureBox14);
            this.carouselItem2.Controls.Add(this.label28);
            this.carouselItem2.Controls.Add(this.labelCardNumberValue2);
            this.carouselItem2.Controls.Add(this.label30);
            this.carouselItem2.Controls.Add(this.labelOwnerValue2);
            this.carouselItem2.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
            this.carouselItem2.Location = new System.Drawing.Point(385, 74);
            this.carouselItem2.Name = "carouselItem2";
            this.carouselItem2.Padding = new System.Windows.Forms.Padding(0, 12, 0, 12);
            this.carouselItem2.Size = new System.Drawing.Size(140, 211);
            this.carouselItem2.TabIndex = 5;
            // 
            // pictureBox14
            // 
            this.pictureBox14.Image = global::CIEID.Properties.Resources.cie;
            this.pictureBox14.Location = new System.Drawing.Point(3, 15);
            this.pictureBox14.Name = "pictureBox14";
            this.pictureBox14.Size = new System.Drawing.Size(134, 84);
            this.pictureBox14.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox14.TabIndex = 0;
            this.pictureBox14.TabStop = false;
            // 
            // label28
            // 
            this.label28.AutoSize = true;
            this.label28.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F);
            this.label28.ForeColor = System.Drawing.Color.LightGray;
            this.label28.Location = new System.Drawing.Point(3, 117);
            this.label28.Margin = new System.Windows.Forms.Padding(3, 15, 3, 0);
            this.label28.Name = "label28";
            this.label28.Padding = new System.Windows.Forms.Padding(4, 0, 5, 0);
            this.label28.Size = new System.Drawing.Size(73, 12);
            this.label28.TabIndex = 3;
            this.label28.Text = "Numero carta:";
            // 
            // labelCardNumberValue2
            // 
            this.labelCardNumberValue2.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold);
            this.labelCardNumberValue2.ForeColor = System.Drawing.Color.LightGray;
            this.labelCardNumberValue2.Location = new System.Drawing.Point(3, 129);
            this.labelCardNumberValue2.Name = "labelCardNumberValue2";
            this.labelCardNumberValue2.Padding = new System.Windows.Forms.Padding(4, 0, 5, 0);
            this.labelCardNumberValue2.Size = new System.Drawing.Size(118, 16);
            this.labelCardNumberValue2.TabIndex = 1;
            this.labelCardNumberValue2.Text = "label6";
            // 
            // label30
            // 
            this.label30.AutoSize = true;
            this.label30.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F);
            this.label30.ForeColor = System.Drawing.Color.LightGray;
            this.label30.Location = new System.Drawing.Point(3, 150);
            this.label30.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.label30.Name = "label30";
            this.label30.Padding = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.label30.Size = new System.Drawing.Size(65, 12);
            this.label30.TabIndex = 2;
            this.label30.Text = "Intestatario:";
            // 
            // labelOwnerValue2
            // 
            this.labelOwnerValue2.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Bold);
            this.labelOwnerValue2.ForeColor = System.Drawing.Color.LightGray;
            this.labelOwnerValue2.Location = new System.Drawing.Point(3, 162);
            this.labelOwnerValue2.Name = "labelOwnerValue2";
            this.labelOwnerValue2.Padding = new System.Windows.Forms.Padding(4, 0, 0, 0);
            this.labelOwnerValue2.Size = new System.Drawing.Size(148, 30);
            this.labelOwnerValue2.TabIndex = 4;
            this.labelOwnerValue2.Text = "label27";
            // 
            // removeAllButton
            // 
            this.removeAllButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.removeAllButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.removeAllButton.FlatAppearance.BorderSize = 0;
            this.removeAllButton.ForeColor = System.Drawing.Color.White;
            this.removeAllButton.Location = new System.Drawing.Point(239, 502);
            this.removeAllButton.Margin = new System.Windows.Forms.Padding(2);
            this.removeAllButton.Name = "removeAllButton";
            this.removeAllButton.Size = new System.Drawing.Size(157, 28);
            this.removeAllButton.TabIndex = 19;
            this.removeAllButton.Text = "Rimuovi tutte";
            this.removeAllButton.UseVisualStyleBackColor = false;
            this.removeAllButton.Click += new System.EventHandler(this.RemoveAllButton_Click);
            // 
            // addButton
            // 
            this.addButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.addButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.addButton.FlatAppearance.BorderSize = 0;
            this.addButton.ForeColor = System.Drawing.Color.White;
            this.addButton.Location = new System.Drawing.Point(435, 502);
            this.addButton.Margin = new System.Windows.Forms.Padding(2);
            this.addButton.Name = "addButton";
            this.addButton.Size = new System.Drawing.Size(157, 28);
            this.addButton.TabIndex = 18;
            this.addButton.Text = "Aggiungi Carta";
            this.addButton.UseVisualStyleBackColor = false;
            this.addButton.Click += new System.EventHandler(this.AddButton_Click);
            // 
            // deleteCIEButton
            // 
            this.deleteCIEButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.deleteCIEButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.deleteCIEButton.FlatAppearance.BorderSize = 0;
            this.deleteCIEButton.ForeColor = System.Drawing.Color.White;
            this.deleteCIEButton.Location = new System.Drawing.Point(58, 502);
            this.deleteCIEButton.Margin = new System.Windows.Forms.Padding(2);
            this.deleteCIEButton.Name = "deleteCIEButton";
            this.deleteCIEButton.Size = new System.Drawing.Size(157, 28);
            this.deleteCIEButton.TabIndex = 17;
            this.deleteCIEButton.Text = "Rimuovi carta selezionata";
            this.deleteCIEButton.UseVisualStyleBackColor = false;
            this.deleteCIEButton.Click += new System.EventHandler(this.DeleteCIEButton_Click);
            // 
            // CIESuccessfullyPairedLabel
            // 
            this.CIESuccessfullyPairedLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CIESuccessfullyPairedLabel.Location = new System.Drawing.Point(143, 53);
            this.CIESuccessfullyPairedLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.CIESuccessfullyPairedLabel.Name = "CIESuccessfullyPairedLabel";
            this.CIESuccessfullyPairedLabel.Size = new System.Drawing.Size(380, 45);
            this.CIESuccessfullyPairedLabel.TabIndex = 11;
            this.CIESuccessfullyPairedLabel.Text = "Carta d\'Identità Elettronica abbinata correttamente";
            this.CIESuccessfullyPairedLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // appNameCIEPickerHeaderTextLabel
            // 
            this.appNameCIEPickerHeaderTextLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.appNameCIEPickerHeaderTextLabel.Location = new System.Drawing.Point(71, 22);
            this.appNameCIEPickerHeaderTextLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.appNameCIEPickerHeaderTextLabel.Name = "appNameCIEPickerHeaderTextLabel";
            this.appNameCIEPickerHeaderTextLabel.Size = new System.Drawing.Size(513, 36);
            this.appNameCIEPickerHeaderTextLabel.TabIndex = 10;
            this.appNameCIEPickerHeaderTextLabel.Text = "CIE ID";
            this.appNameCIEPickerHeaderTextLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // CIEPairingInProgressTabPage
            // 
            this.CIEPairingInProgressTabPage.BackColor = System.Drawing.Color.White;
            this.CIEPairingInProgressTabPage.Controls.Add(this.progressBar);
            this.CIEPairingInProgressTabPage.Controls.Add(this.labelProgressMessage);
            this.CIEPairingInProgressTabPage.Controls.Add(this.pictureBox3);
            this.CIEPairingInProgressTabPage.Controls.Add(this.labelPairingDescriptionPerformingOperation);
            this.CIEPairingInProgressTabPage.Controls.Add(this.labelPairYourCIE);
            this.CIEPairingInProgressTabPage.Location = new System.Drawing.Point(4, 9);
            this.CIEPairingInProgressTabPage.Margin = new System.Windows.Forms.Padding(2);
            this.CIEPairingInProgressTabPage.Name = "CIEPairingInProgressTabPage";
            this.CIEPairingInProgressTabPage.Size = new System.Drawing.Size(649, 533);
            this.CIEPairingInProgressTabPage.TabIndex = 2;
            this.CIEPairingInProgressTabPage.Text = "tabPage3";
            // 
            // progressBar
            // 
            this.progressBar.ForeColor = System.Drawing.Color.Blue;
            this.progressBar.Location = new System.Drawing.Point(329, 296);
            this.progressBar.Margin = new System.Windows.Forms.Padding(2);
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(228, 13);
            this.progressBar.TabIndex = 16;
            this.progressBar.Value = 55;
            // 
            // labelProgressMessage
            // 
            this.labelProgressMessage.Font = new System.Drawing.Font("Microsoft Sans Serif", 13F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelProgressMessage.Location = new System.Drawing.Point(320, 267);
            this.labelProgressMessage.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelProgressMessage.Name = "labelProgressMessage";
            this.labelProgressMessage.Size = new System.Drawing.Size(237, 20);
            this.labelProgressMessage.TabIndex = 15;
            this.labelProgressMessage.Text = "Abbinamento in corso";
            this.labelProgressMessage.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // pictureBox3
            // 
            this.pictureBox3.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox3.Image")));
            this.pictureBox3.Location = new System.Drawing.Point(64, 193);
            this.pictureBox3.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox3.Name = "pictureBox3";
            this.pictureBox3.Size = new System.Drawing.Size(214, 214);
            this.pictureBox3.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox3.TabIndex = 14;
            this.pictureBox3.TabStop = false;
            // 
            // labelPairingDescriptionPerformingOperation
            // 
            this.labelPairingDescriptionPerformingOperation.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelPairingDescriptionPerformingOperation.Location = new System.Drawing.Point(121, 52);
            this.labelPairingDescriptionPerformingOperation.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelPairingDescriptionPerformingOperation.Name = "labelPairingDescriptionPerformingOperation";
            this.labelPairingDescriptionPerformingOperation.Size = new System.Drawing.Size(446, 72);
            this.labelPairingDescriptionPerformingOperation.TabIndex = 13;
            this.labelPairingDescriptionPerformingOperation.Text = "Dopo aver collegato e installato il lettore di smart card, posiziona la CIE sul l" +
    "ettore ed inserisci il PIN";
            this.labelPairingDescriptionPerformingOperation.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelPairYourCIE
            // 
            this.labelPairYourCIE.AutoSize = true;
            this.labelPairYourCIE.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelPairYourCIE.Location = new System.Drawing.Point(220, 16);
            this.labelPairYourCIE.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelPairYourCIE.Name = "labelPairYourCIE";
            this.labelPairYourCIE.Size = new System.Drawing.Size(264, 36);
            this.labelPairYourCIE.TabIndex = 12;
            this.labelPairYourCIE.Text = "Abbina la tua CIE";
            // 
            // tabPageChangePIN
            // 
            this.tabPageChangePIN.BackColor = System.Drawing.Color.White;
            this.tabPageChangePIN.Controls.Add(this.labelPINPolicyDescription);
            this.tabPageChangePIN.Controls.Add(this.changePINButton);
            this.tabPageChangePIN.Controls.Add(this.textBoxNewPIN2);
            this.tabPageChangePIN.Controls.Add(this.labelRepeatNewPIN);
            this.tabPageChangePIN.Controls.Add(this.textBoxNewPIN);
            this.tabPageChangePIN.Controls.Add(this.labelTypeNewPIN);
            this.tabPageChangePIN.Controls.Add(this.textBoxPIN);
            this.tabPageChangePIN.Controls.Add(this.labelTypeOldPIN);
            this.tabPageChangePIN.Controls.Add(this.pictureBox4);
            this.tabPageChangePIN.Controls.Add(this.labelChangePINDescription);
            this.tabPageChangePIN.Controls.Add(this.labelChangePINHeaderText);
            this.tabPageChangePIN.Location = new System.Drawing.Point(4, 9);
            this.tabPageChangePIN.Margin = new System.Windows.Forms.Padding(2);
            this.tabPageChangePIN.Name = "tabPageChangePIN";
            this.tabPageChangePIN.Size = new System.Drawing.Size(649, 533);
            this.tabPageChangePIN.TabIndex = 3;
            this.tabPageChangePIN.Text = "tabPage4";
            // 
            // labelPINPolicyDescription
            // 
            this.labelPINPolicyDescription.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelPINPolicyDescription.Location = new System.Drawing.Point(361, 345);
            this.labelPINPolicyDescription.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelPINPolicyDescription.Name = "labelPINPolicyDescription";
            this.labelPINPolicyDescription.Size = new System.Drawing.Size(219, 98);
            this.labelPINPolicyDescription.TabIndex = 25;
            this.labelPINPolicyDescription.Text = resources.GetString("labelPINPolicyDescription.Text");
            // 
            // changePINButton
            // 
            this.changePINButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.changePINButton.FlatAppearance.BorderSize = 0;
            this.changePINButton.ForeColor = System.Drawing.Color.White;
            this.changePINButton.Location = new System.Drawing.Point(249, 475);
            this.changePINButton.Margin = new System.Windows.Forms.Padding(2);
            this.changePINButton.Name = "changePINButton";
            this.changePINButton.Size = new System.Drawing.Size(134, 31);
            this.changePINButton.TabIndex = 24;
            this.changePINButton.Text = "Cambia PIN";
            this.changePINButton.UseVisualStyleBackColor = false;
            this.changePINButton.Click += new System.EventHandler(this.ChangePINButton_Click);
            // 
            // textBoxNewPIN2
            // 
            this.textBoxNewPIN2.Location = new System.Drawing.Point(365, 301);
            this.textBoxNewPIN2.Name = "textBoxNewPIN2";
            this.textBoxNewPIN2.Size = new System.Drawing.Size(217, 20);
            this.textBoxNewPIN2.TabIndex = 23;
            this.textBoxNewPIN2.UseSystemPasswordChar = true;
            this.textBoxNewPIN2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PINTextBox_KeyPress);
            // 
            // labelRepeatNewPIN
            // 
            this.labelRepeatNewPIN.AutoSize = true;
            this.labelRepeatNewPIN.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelRepeatNewPIN.Location = new System.Drawing.Point(362, 278);
            this.labelRepeatNewPIN.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelRepeatNewPIN.Name = "labelRepeatNewPIN";
            this.labelRepeatNewPIN.Size = new System.Drawing.Size(123, 17);
            this.labelRepeatNewPIN.TabIndex = 22;
            this.labelRepeatNewPIN.Text = "Ripeti il nuovo PIN";
            // 
            // textBoxNewPIN
            // 
            this.textBoxNewPIN.Location = new System.Drawing.Point(365, 249);
            this.textBoxNewPIN.Margin = new System.Windows.Forms.Padding(2);
            this.textBoxNewPIN.Name = "textBoxNewPIN";
            this.textBoxNewPIN.Size = new System.Drawing.Size(217, 20);
            this.textBoxNewPIN.TabIndex = 21;
            this.textBoxNewPIN.UseSystemPasswordChar = true;
            this.textBoxNewPIN.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PINTextBox_KeyPress);
            // 
            // labelTypeNewPIN
            // 
            this.labelTypeNewPIN.AutoSize = true;
            this.labelTypeNewPIN.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelTypeNewPIN.Location = new System.Drawing.Point(362, 225);
            this.labelTypeNewPIN.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelTypeNewPIN.Name = "labelTypeNewPIN";
            this.labelTypeNewPIN.Size = new System.Drawing.Size(138, 17);
            this.labelTypeNewPIN.TabIndex = 20;
            this.labelTypeNewPIN.Text = "Inserisci il nuovo PIN";
            // 
            // textBoxPIN
            // 
            this.textBoxPIN.Location = new System.Drawing.Point(364, 198);
            this.textBoxPIN.Margin = new System.Windows.Forms.Padding(2);
            this.textBoxPIN.Name = "textBoxPIN";
            this.textBoxPIN.Size = new System.Drawing.Size(217, 20);
            this.textBoxPIN.TabIndex = 19;
            this.textBoxPIN.UseSystemPasswordChar = true;
            this.textBoxPIN.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PINTextBox_KeyPress);
            // 
            // labelTypeOldPIN
            // 
            this.labelTypeOldPIN.AutoSize = true;
            this.labelTypeOldPIN.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelTypeOldPIN.Location = new System.Drawing.Point(362, 172);
            this.labelTypeOldPIN.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelTypeOldPIN.Name = "labelTypeOldPIN";
            this.labelTypeOldPIN.Size = new System.Drawing.Size(147, 17);
            this.labelTypeOldPIN.TabIndex = 18;
            this.labelTypeOldPIN.Text = "Inserisci il vecchio PIN";
            // 
            // pictureBox4
            // 
            this.pictureBox4.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox4.Image")));
            this.pictureBox4.Location = new System.Drawing.Point(64, 172);
            this.pictureBox4.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox4.Name = "pictureBox4";
            this.pictureBox4.Size = new System.Drawing.Size(214, 205);
            this.pictureBox4.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox4.TabIndex = 17;
            this.pictureBox4.TabStop = false;
            // 
            // labelChangePINDescription
            // 
            this.labelChangePINDescription.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelChangePINDescription.Location = new System.Drawing.Point(186, 66);
            this.labelChangePINDescription.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelChangePINDescription.Name = "labelChangePINDescription";
            this.labelChangePINDescription.Size = new System.Drawing.Size(289, 45);
            this.labelChangePINDescription.TabIndex = 16;
            this.labelChangePINDescription.Text = "Il PIN della tua CIE è un dato sensibile,  trattalo con cautela";
            this.labelChangePINDescription.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelChangePINHeaderText
            // 
            this.labelChangePINHeaderText.AutoSize = true;
            this.labelChangePINHeaderText.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelChangePINHeaderText.Location = new System.Drawing.Point(243, 24);
            this.labelChangePINHeaderText.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelChangePINHeaderText.Name = "labelChangePINHeaderText";
            this.labelChangePINHeaderText.Size = new System.Drawing.Size(183, 36);
            this.labelChangePINHeaderText.TabIndex = 15;
            this.labelChangePINHeaderText.Text = "Cambia PIN";
            // 
            // tabPageChangePINInProgress
            // 
            this.tabPageChangePINInProgress.BackColor = System.Drawing.Color.White;
            this.tabPageChangePINInProgress.Controls.Add(this.progressBarChangePIN);
            this.tabPageChangePINInProgress.Controls.Add(this.progressLabelChangePIN);
            this.tabPageChangePINInProgress.Controls.Add(this.pictureBox5);
            this.tabPageChangePINInProgress.Controls.Add(this.labelInfoBoxAboutCIEPINSafety);
            this.tabPageChangePINInProgress.Controls.Add(this.labelChangePINPerformingOperationHeaderText);
            this.tabPageChangePINInProgress.Location = new System.Drawing.Point(4, 9);
            this.tabPageChangePINInProgress.Margin = new System.Windows.Forms.Padding(2);
            this.tabPageChangePINInProgress.Name = "tabPageChangePINInProgress";
            this.tabPageChangePINInProgress.Size = new System.Drawing.Size(649, 533);
            this.tabPageChangePINInProgress.TabIndex = 4;
            this.tabPageChangePINInProgress.Text = "tabPage5";
            // 
            // progressBarChangePIN
            // 
            this.progressBarChangePIN.ForeColor = System.Drawing.Color.Blue;
            this.progressBarChangePIN.Location = new System.Drawing.Point(329, 292);
            this.progressBarChangePIN.Margin = new System.Windows.Forms.Padding(2);
            this.progressBarChangePIN.Name = "progressBarChangePIN";
            this.progressBarChangePIN.Size = new System.Drawing.Size(228, 13);
            this.progressBarChangePIN.TabIndex = 22;
            this.progressBarChangePIN.Value = 55;
            // 
            // progressLabelChangePIN
            // 
            this.progressLabelChangePIN.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.progressLabelChangePIN.Location = new System.Drawing.Point(300, 265);
            this.progressLabelChangePIN.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.progressLabelChangePIN.Name = "progressLabelChangePIN";
            this.progressLabelChangePIN.Size = new System.Drawing.Size(257, 20);
            this.progressLabelChangePIN.TabIndex = 21;
            this.progressLabelChangePIN.Text = "Abbinamento in corso";
            this.progressLabelChangePIN.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // pictureBox5
            // 
            this.pictureBox5.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox5.Image")));
            this.pictureBox5.Location = new System.Drawing.Point(64, 192);
            this.pictureBox5.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox5.Name = "pictureBox5";
            this.pictureBox5.Size = new System.Drawing.Size(214, 214);
            this.pictureBox5.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox5.TabIndex = 20;
            this.pictureBox5.TabStop = false;
            // 
            // labelInfoBoxAboutCIEPINSafety
            // 
            this.labelInfoBoxAboutCIEPINSafety.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelInfoBoxAboutCIEPINSafety.Location = new System.Drawing.Point(209, 60);
            this.labelInfoBoxAboutCIEPINSafety.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelInfoBoxAboutCIEPINSafety.Name = "labelInfoBoxAboutCIEPINSafety";
            this.labelInfoBoxAboutCIEPINSafety.Size = new System.Drawing.Size(244, 45);
            this.labelInfoBoxAboutCIEPINSafety.TabIndex = 18;
            this.labelInfoBoxAboutCIEPINSafety.Text = "Il PIN della tua CIE è un dato sensibile,  trattalo con cautela";
            this.labelInfoBoxAboutCIEPINSafety.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelChangePINPerformingOperationHeaderText
            // 
            this.labelChangePINPerformingOperationHeaderText.AutoSize = true;
            this.labelChangePINPerformingOperationHeaderText.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelChangePINPerformingOperationHeaderText.Location = new System.Drawing.Point(235, 16);
            this.labelChangePINPerformingOperationHeaderText.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelChangePINPerformingOperationHeaderText.Name = "labelChangePINPerformingOperationHeaderText";
            this.labelChangePINPerformingOperationHeaderText.Size = new System.Drawing.Size(183, 36);
            this.labelChangePINPerformingOperationHeaderText.TabIndex = 17;
            this.labelChangePINPerformingOperationHeaderText.Text = "Cambia PIN";
            // 
            // tabPageUnlockPIN
            // 
            this.tabPageUnlockPIN.BackColor = System.Drawing.Color.White;
            this.tabPageUnlockPIN.Controls.Add(this.labelPINPolicyInPINUnlock);
            this.tabPageUnlockPIN.Controls.Add(this.unlockPINButton);
            this.tabPageUnlockPIN.Controls.Add(this.textBoxUnlockPIN2);
            this.tabPageUnlockPIN.Controls.Add(this.labelRepeatNewPINUnlockPIN);
            this.tabPageUnlockPIN.Controls.Add(this.textBoxUnlockPIN);
            this.tabPageUnlockPIN.Controls.Add(this.labelTypeNewPINUnlockPIN);
            this.tabPageUnlockPIN.Controls.Add(this.textBoxPUK);
            this.tabPageUnlockPIN.Controls.Add(this.labelTypePUK);
            this.tabPageUnlockPIN.Controls.Add(this.pictureBox6);
            this.tabPageUnlockPIN.Controls.Add(this.labelUnlockPINDescription);
            this.tabPageUnlockPIN.Controls.Add(this.labelUnlockPINHeaderText);
            this.tabPageUnlockPIN.Location = new System.Drawing.Point(4, 9);
            this.tabPageUnlockPIN.Margin = new System.Windows.Forms.Padding(2);
            this.tabPageUnlockPIN.Name = "tabPageUnlockPIN";
            this.tabPageUnlockPIN.Size = new System.Drawing.Size(649, 533);
            this.tabPageUnlockPIN.TabIndex = 5;
            this.tabPageUnlockPIN.Text = "tabPage6";
            this.tabPageUnlockPIN.Click += new System.EventHandler(this.TabPageUnlockPIN_Click);
            // 
            // labelPINPolicyInPINUnlock
            // 
            this.labelPINPolicyInPINUnlock.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelPINPolicyInPINUnlock.Location = new System.Drawing.Point(361, 345);
            this.labelPINPolicyInPINUnlock.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelPINPolicyInPINUnlock.Name = "labelPINPolicyInPINUnlock";
            this.labelPINPolicyInPINUnlock.Size = new System.Drawing.Size(219, 98);
            this.labelPINPolicyInPINUnlock.TabIndex = 36;
            this.labelPINPolicyInPINUnlock.Text = resources.GetString("labelPINPolicyInPINUnlock.Text");
            // 
            // unlockPINButton
            // 
            this.unlockPINButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.unlockPINButton.FlatAppearance.BorderSize = 0;
            this.unlockPINButton.ForeColor = System.Drawing.Color.White;
            this.unlockPINButton.Location = new System.Drawing.Point(249, 475);
            this.unlockPINButton.Margin = new System.Windows.Forms.Padding(2);
            this.unlockPINButton.Name = "unlockPINButton";
            this.unlockPINButton.Size = new System.Drawing.Size(134, 31);
            this.unlockPINButton.TabIndex = 35;
            this.unlockPINButton.Text = "Sblocca carta";
            this.unlockPINButton.UseVisualStyleBackColor = false;
            this.unlockPINButton.Click += new System.EventHandler(this.UnlockPINButton_Click);
            // 
            // textBoxUnlockPIN2
            // 
            this.textBoxUnlockPIN2.Location = new System.Drawing.Point(365, 301);
            this.textBoxUnlockPIN2.Margin = new System.Windows.Forms.Padding(2);
            this.textBoxUnlockPIN2.Name = "textBoxUnlockPIN2";
            this.textBoxUnlockPIN2.Size = new System.Drawing.Size(217, 20);
            this.textBoxUnlockPIN2.TabIndex = 34;
            this.textBoxUnlockPIN2.UseSystemPasswordChar = true;
            this.textBoxUnlockPIN2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PUKTextBox_KeyPress);
            // 
            // labelRepeatNewPINUnlockPIN
            // 
            this.labelRepeatNewPINUnlockPIN.AutoSize = true;
            this.labelRepeatNewPINUnlockPIN.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelRepeatNewPINUnlockPIN.Location = new System.Drawing.Point(362, 278);
            this.labelRepeatNewPINUnlockPIN.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelRepeatNewPINUnlockPIN.Name = "labelRepeatNewPINUnlockPIN";
            this.labelRepeatNewPINUnlockPIN.Size = new System.Drawing.Size(123, 17);
            this.labelRepeatNewPINUnlockPIN.TabIndex = 33;
            this.labelRepeatNewPINUnlockPIN.Text = "Ripeti il nuovo PIN";
            // 
            // textBoxUnlockPIN
            // 
            this.textBoxUnlockPIN.Location = new System.Drawing.Point(365, 249);
            this.textBoxUnlockPIN.Margin = new System.Windows.Forms.Padding(2);
            this.textBoxUnlockPIN.Name = "textBoxUnlockPIN";
            this.textBoxUnlockPIN.Size = new System.Drawing.Size(217, 20);
            this.textBoxUnlockPIN.TabIndex = 32;
            this.textBoxUnlockPIN.UseSystemPasswordChar = true;
            this.textBoxUnlockPIN.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PUKTextBox_KeyPress);
            // 
            // labelTypeNewPINUnlockPIN
            // 
            this.labelTypeNewPINUnlockPIN.AutoSize = true;
            this.labelTypeNewPINUnlockPIN.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelTypeNewPINUnlockPIN.Location = new System.Drawing.Point(362, 225);
            this.labelTypeNewPINUnlockPIN.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelTypeNewPINUnlockPIN.Name = "labelTypeNewPINUnlockPIN";
            this.labelTypeNewPINUnlockPIN.Size = new System.Drawing.Size(138, 17);
            this.labelTypeNewPINUnlockPIN.TabIndex = 31;
            this.labelTypeNewPINUnlockPIN.Text = "Inserisci il nuovo PIN";
            // 
            // textBoxPUK
            // 
            this.textBoxPUK.Location = new System.Drawing.Point(364, 198);
            this.textBoxPUK.Margin = new System.Windows.Forms.Padding(2);
            this.textBoxPUK.Name = "textBoxPUK";
            this.textBoxPUK.Size = new System.Drawing.Size(217, 20);
            this.textBoxPUK.TabIndex = 30;
            this.textBoxPUK.UseSystemPasswordChar = true;
            this.textBoxPUK.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PUKTextBox_KeyPress);
            // 
            // labelTypePUK
            // 
            this.labelTypePUK.AutoSize = true;
            this.labelTypePUK.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelTypePUK.Location = new System.Drawing.Point(362, 172);
            this.labelTypePUK.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelTypePUK.Name = "labelTypePUK";
            this.labelTypePUK.Size = new System.Drawing.Size(101, 17);
            this.labelTypePUK.TabIndex = 29;
            this.labelTypePUK.Text = "Inserisci il PUK";
            // 
            // pictureBox6
            // 
            this.pictureBox6.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox6.Image")));
            this.pictureBox6.Location = new System.Drawing.Point(64, 172);
            this.pictureBox6.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox6.Name = "pictureBox6";
            this.pictureBox6.Size = new System.Drawing.Size(214, 205);
            this.pictureBox6.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox6.TabIndex = 28;
            this.pictureBox6.TabStop = false;
            // 
            // labelUnlockPINDescription
            // 
            this.labelUnlockPINDescription.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelUnlockPINDescription.Location = new System.Drawing.Point(186, 60);
            this.labelUnlockPINDescription.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelUnlockPINDescription.Name = "labelUnlockPINDescription";
            this.labelUnlockPINDescription.Size = new System.Drawing.Size(266, 45);
            this.labelUnlockPINDescription.TabIndex = 27;
            this.labelUnlockPINDescription.Text = "Utilizza il codice PUK ricevuto con la CIE";
            this.labelUnlockPINDescription.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelUnlockPINHeaderText
            // 
            this.labelUnlockPINHeaderText.AutoSize = true;
            this.labelUnlockPINHeaderText.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelUnlockPINHeaderText.Location = new System.Drawing.Point(212, 24);
            this.labelUnlockPINHeaderText.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelUnlockPINHeaderText.Name = "labelUnlockPINHeaderText";
            this.labelUnlockPINHeaderText.Size = new System.Drawing.Size(215, 36);
            this.labelUnlockPINHeaderText.TabIndex = 26;
            this.labelUnlockPINHeaderText.Text = "Sblocco Carta";
            // 
            // tabPageUnlockPINInProgress
            // 
            this.tabPageUnlockPINInProgress.BackColor = System.Drawing.Color.White;
            this.tabPageUnlockPINInProgress.Controls.Add(this.progressBarUnlockPIN);
            this.tabPageUnlockPINInProgress.Controls.Add(this.progressLabelUnlockPIN);
            this.tabPageUnlockPINInProgress.Controls.Add(this.pictureBox7);
            this.tabPageUnlockPINInProgress.Controls.Add(this.labelUnlockPINDescriptionPerformingOperation);
            this.tabPageUnlockPINInProgress.Controls.Add(this.labelUnlockPINHeaderTextPerformingOperation);
            this.tabPageUnlockPINInProgress.Location = new System.Drawing.Point(4, 9);
            this.tabPageUnlockPINInProgress.Margin = new System.Windows.Forms.Padding(2);
            this.tabPageUnlockPINInProgress.Name = "tabPageUnlockPINInProgress";
            this.tabPageUnlockPINInProgress.Size = new System.Drawing.Size(649, 533);
            this.tabPageUnlockPINInProgress.TabIndex = 6;
            this.tabPageUnlockPINInProgress.Text = "tabPage7";
            // 
            // progressBarUnlockPIN
            // 
            this.progressBarUnlockPIN.ForeColor = System.Drawing.Color.Blue;
            this.progressBarUnlockPIN.Location = new System.Drawing.Point(329, 292);
            this.progressBarUnlockPIN.Margin = new System.Windows.Forms.Padding(2);
            this.progressBarUnlockPIN.Name = "progressBarUnlockPIN";
            this.progressBarUnlockPIN.Size = new System.Drawing.Size(228, 13);
            this.progressBarUnlockPIN.TabIndex = 27;
            this.progressBarUnlockPIN.Value = 55;
            // 
            // progressLabelUnlockPIN
            // 
            this.progressLabelUnlockPIN.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.progressLabelUnlockPIN.Location = new System.Drawing.Point(321, 265);
            this.progressLabelUnlockPIN.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.progressLabelUnlockPIN.Name = "progressLabelUnlockPIN";
            this.progressLabelUnlockPIN.Size = new System.Drawing.Size(236, 20);
            this.progressLabelUnlockPIN.TabIndex = 26;
            this.progressLabelUnlockPIN.Text = "Abbinamento in corso";
            this.progressLabelUnlockPIN.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // pictureBox7
            // 
            this.pictureBox7.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox7.Image")));
            this.pictureBox7.Location = new System.Drawing.Point(63, 180);
            this.pictureBox7.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox7.Name = "pictureBox7";
            this.pictureBox7.Size = new System.Drawing.Size(214, 205);
            this.pictureBox7.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox7.TabIndex = 25;
            this.pictureBox7.TabStop = false;
            // 
            // labelUnlockPINDescriptionPerformingOperation
            // 
            this.labelUnlockPINDescriptionPerformingOperation.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelUnlockPINDescriptionPerformingOperation.Location = new System.Drawing.Point(186, 66);
            this.labelUnlockPINDescriptionPerformingOperation.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelUnlockPINDescriptionPerformingOperation.Name = "labelUnlockPINDescriptionPerformingOperation";
            this.labelUnlockPINDescriptionPerformingOperation.Size = new System.Drawing.Size(257, 44);
            this.labelUnlockPINDescriptionPerformingOperation.TabIndex = 24;
            this.labelUnlockPINDescriptionPerformingOperation.Text = "Utilizza il codice PUK ricevuto con la CIE";
            this.labelUnlockPINDescriptionPerformingOperation.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelUnlockPINHeaderTextPerformingOperation
            // 
            this.labelUnlockPINHeaderTextPerformingOperation.AutoSize = true;
            this.labelUnlockPINHeaderTextPerformingOperation.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelUnlockPINHeaderTextPerformingOperation.Location = new System.Drawing.Point(217, 22);
            this.labelUnlockPINHeaderTextPerformingOperation.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelUnlockPINHeaderTextPerformingOperation.Name = "labelUnlockPINHeaderTextPerformingOperation";
            this.labelUnlockPINHeaderTextPerformingOperation.Size = new System.Drawing.Size(209, 36);
            this.labelUnlockPINHeaderTextPerformingOperation.TabIndex = 23;
            this.labelUnlockPINHeaderTextPerformingOperation.Text = "Sblocco carta";
            // 
            // tabPageTutorialWebBrowser
            // 
            this.tabPageTutorialWebBrowser.BackColor = System.Drawing.Color.White;
            this.tabPageTutorialWebBrowser.Controls.Add(this.webBrowserTutorial);
            this.tabPageTutorialWebBrowser.Location = new System.Drawing.Point(4, 9);
            this.tabPageTutorialWebBrowser.Margin = new System.Windows.Forms.Padding(2);
            this.tabPageTutorialWebBrowser.Name = "tabPageTutorialWebBrowser";
            this.tabPageTutorialWebBrowser.Size = new System.Drawing.Size(649, 533);
            this.tabPageTutorialWebBrowser.TabIndex = 7;
            this.tabPageTutorialWebBrowser.Text = "tabPage8";
            // 
            // webBrowserTutorial
            // 
            this.webBrowserTutorial.Dock = System.Windows.Forms.DockStyle.Fill;
            this.webBrowserTutorial.Location = new System.Drawing.Point(0, 0);
            this.webBrowserTutorial.Margin = new System.Windows.Forms.Padding(2);
            this.webBrowserTutorial.MinimumSize = new System.Drawing.Size(15, 16);
            this.webBrowserTutorial.Name = "webBrowserTutorial";
            this.webBrowserTutorial.Size = new System.Drawing.Size(649, 533);
            this.webBrowserTutorial.TabIndex = 0;
            // 
            // tabPageHelpWebBrowser
            // 
            this.tabPageHelpWebBrowser.BackColor = System.Drawing.Color.White;
            this.tabPageHelpWebBrowser.Controls.Add(this.pictureBox11);
            this.tabPageHelpWebBrowser.Controls.Add(this.pictureBox13);
            this.tabPageHelpWebBrowser.Controls.Add(this.webBrowserHelp);
            this.tabPageHelpWebBrowser.Location = new System.Drawing.Point(4, 9);
            this.tabPageHelpWebBrowser.Margin = new System.Windows.Forms.Padding(2);
            this.tabPageHelpWebBrowser.Name = "tabPageHelpWebBrowser";
            this.tabPageHelpWebBrowser.Size = new System.Drawing.Size(649, 533);
            this.tabPageHelpWebBrowser.TabIndex = 8;
            this.tabPageHelpWebBrowser.Text = "tabPage9";
            // 
            // pictureBox11
            // 
            this.pictureBox11.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox11.Image")));
            this.pictureBox11.Location = new System.Drawing.Point(360, 444);
            this.pictureBox11.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox11.Name = "pictureBox11";
            this.pictureBox11.Size = new System.Drawing.Size(150, 67);
            this.pictureBox11.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox11.TabIndex = 7;
            this.pictureBox11.TabStop = false;
            // 
            // pictureBox13
            // 
            this.pictureBox13.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox13.Image")));
            this.pictureBox13.Location = new System.Drawing.Point(138, 444);
            this.pictureBox13.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox13.Name = "pictureBox13";
            this.pictureBox13.Size = new System.Drawing.Size(174, 67);
            this.pictureBox13.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox13.TabIndex = 5;
            this.pictureBox13.TabStop = false;
            // 
            // webBrowserHelp
            // 
            this.webBrowserHelp.Location = new System.Drawing.Point(0, 0);
            this.webBrowserHelp.Margin = new System.Windows.Forms.Padding(2);
            this.webBrowserHelp.MinimumSize = new System.Drawing.Size(15, 16);
            this.webBrowserHelp.Name = "webBrowserHelp";
            this.webBrowserHelp.Size = new System.Drawing.Size(652, 425);
            this.webBrowserHelp.TabIndex = 0;
            // 
            // tabPageInformationWebBrowser
            // 
            this.tabPageInformationWebBrowser.BackColor = System.Drawing.Color.White;
            this.tabPageInformationWebBrowser.Controls.Add(this.pictureBox10);
            this.tabPageInformationWebBrowser.Controls.Add(this.pictureBox9);
            this.tabPageInformationWebBrowser.Controls.Add(this.pictureBox8);
            this.tabPageInformationWebBrowser.Controls.Add(this.webBrowserInfo);
            this.tabPageInformationWebBrowser.Location = new System.Drawing.Point(4, 9);
            this.tabPageInformationWebBrowser.Margin = new System.Windows.Forms.Padding(2);
            this.tabPageInformationWebBrowser.Name = "tabPageInformationWebBrowser";
            this.tabPageInformationWebBrowser.Size = new System.Drawing.Size(649, 533);
            this.tabPageInformationWebBrowser.TabIndex = 9;
            this.tabPageInformationWebBrowser.Text = "tabPage10";
            // 
            // pictureBox10
            // 
            this.pictureBox10.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox10.Image")));
            this.pictureBox10.Location = new System.Drawing.Point(410, 439);
            this.pictureBox10.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox10.Name = "pictureBox10";
            this.pictureBox10.Size = new System.Drawing.Size(125, 65);
            this.pictureBox10.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox10.TabIndex = 4;
            this.pictureBox10.TabStop = false;
            // 
            // pictureBox9
            // 
            this.pictureBox9.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox9.Image")));
            this.pictureBox9.Location = new System.Drawing.Point(266, 439);
            this.pictureBox9.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox9.Name = "pictureBox9";
            this.pictureBox9.Size = new System.Drawing.Size(125, 65);
            this.pictureBox9.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox9.TabIndex = 3;
            this.pictureBox9.TabStop = false;
            // 
            // pictureBox8
            // 
            this.pictureBox8.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox8.Image")));
            this.pictureBox8.Location = new System.Drawing.Point(121, 439);
            this.pictureBox8.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox8.Name = "pictureBox8";
            this.pictureBox8.Size = new System.Drawing.Size(125, 65);
            this.pictureBox8.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox8.TabIndex = 2;
            this.pictureBox8.TabStop = false;
            // 
            // webBrowserInfo
            // 
            this.webBrowserInfo.Location = new System.Drawing.Point(0, 2);
            this.webBrowserInfo.Margin = new System.Windows.Forms.Padding(2);
            this.webBrowserInfo.MinimumSize = new System.Drawing.Size(15, 16);
            this.webBrowserInfo.Name = "webBrowserInfo";
            this.webBrowserInfo.Size = new System.Drawing.Size(652, 421);
            this.webBrowserInfo.TabIndex = 1;
            // 
            // tabSelectFile
            // 
            this.tabSelectFile.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.tabSelectFile.Controls.Add(this.customizeGraphicSignatureLabel);
            this.tabSelectFile.Controls.Add(this.pictureBox15);
            this.tabSelectFile.Controls.Add(this.labelGraphicSignatureDescriptionInfoBox);
            this.tabSelectFile.Controls.Add(this.panelChooseDoc);
            this.tabSelectFile.Controls.Add(this.digitalSignatureLabel);
            this.tabSelectFile.Location = new System.Drawing.Point(4, 9);
            this.tabSelectFile.Name = "tabSelectFile";
            this.tabSelectFile.Size = new System.Drawing.Size(649, 533);
            this.tabSelectFile.TabIndex = 10;
            this.tabSelectFile.Text = "tabPage11";
            // 
            // customizeGraphicSignatureLabel
            // 
            this.customizeGraphicSignatureLabel.AutoSize = true;
            this.customizeGraphicSignatureLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.customizeGraphicSignatureLabel.ForeColor = System.Drawing.SystemColors.Highlight;
            this.customizeGraphicSignatureLabel.Location = new System.Drawing.Point(469, 452);
            this.customizeGraphicSignatureLabel.Name = "customizeGraphicSignatureLabel";
            this.customizeGraphicSignatureLabel.Size = new System.Drawing.Size(89, 17);
            this.customizeGraphicSignatureLabel.TabIndex = 22;
            this.customizeGraphicSignatureLabel.Text = "Personalizza";
            this.customizeGraphicSignatureLabel.Click += new System.EventHandler(this.CustomizeGraphicSignatureLabel_Click);
            this.customizeGraphicSignatureLabel.MouseEnter += new System.EventHandler(this.CustomizeGraphicSignatureLabel_MouseEnter);
            this.customizeGraphicSignatureLabel.MouseLeave += new System.EventHandler(this.CustomizeGraphicSignatureLabel_MouseLeave);
            // 
            // pictureBox15
            // 
            this.pictureBox15.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox15.Image")));
            this.pictureBox15.InitialImage = null;
            this.pictureBox15.Location = new System.Drawing.Point(23, 440);
            this.pictureBox15.Name = "pictureBox15";
            this.pictureBox15.Size = new System.Drawing.Size(79, 41);
            this.pictureBox15.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox15.TabIndex = 21;
            this.pictureBox15.TabStop = false;
            // 
            // labelGraphicSignatureDescriptionInfoBox
            // 
            this.labelGraphicSignatureDescriptionInfoBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelGraphicSignatureDescriptionInfoBox.Location = new System.Drawing.Point(107, 423);
            this.labelGraphicSignatureDescriptionInfoBox.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelGraphicSignatureDescriptionInfoBox.Name = "labelGraphicSignatureDescriptionInfoBox";
            this.labelGraphicSignatureDescriptionInfoBox.Size = new System.Drawing.Size(357, 73);
            this.labelGraphicSignatureDescriptionInfoBox.TabIndex = 20;
            this.labelGraphicSignatureDescriptionInfoBox.Text = "Abbiamo creato per te una firma grafica, ma se preferisci puoi personalizzarla. Q" +
    "uesto passaggio non è indispensabile, ma ti consentirà di dare un tocco personal" +
    "e ai documenti firmati.";
            this.labelGraphicSignatureDescriptionInfoBox.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // panelChooseDoc
            // 
            this.panelChooseDoc.AllowDrop = true;
            this.panelChooseDoc.Controls.Add(this.selectDocument);
            this.panelChooseDoc.Controls.Add(this.labelChooseBetweenDragOrSelection);
            this.panelChooseDoc.Controls.Add(this.labelDragAndDropDocumentInformation);
            this.panelChooseDoc.Controls.Add(this.pbFolder);
            this.panelChooseDoc.Location = new System.Drawing.Point(77, 116);
            this.panelChooseDoc.Name = "panelChooseDoc";
            this.panelChooseDoc.Size = new System.Drawing.Size(481, 275);
            this.panelChooseDoc.TabIndex = 17;
            this.panelChooseDoc.DragDrop += new System.Windows.Forms.DragEventHandler(this.PanelChooseDoc_dragDrop);
            this.panelChooseDoc.DragEnter += new System.Windows.Forms.DragEventHandler(this.PanelChooseDoc_dragEnter);
            this.panelChooseDoc.DragLeave += new System.EventHandler(this.PanelChooseDoc_dragLeave);
            this.panelChooseDoc.Paint += new System.Windows.Forms.PaintEventHandler(this.Panel1_Paint);
            // 
            // selectDocument
            // 
            this.selectDocument.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.selectDocument.BackColor = System.Drawing.Color.CornflowerBlue;
            this.selectDocument.FlatAppearance.BorderSize = 0;
            this.selectDocument.ForeColor = System.Drawing.Color.White;
            this.selectDocument.Location = new System.Drawing.Point(171, 229);
            this.selectDocument.Margin = new System.Windows.Forms.Padding(2);
            this.selectDocument.Name = "selectDocument";
            this.selectDocument.Size = new System.Drawing.Size(157, 28);
            this.selectDocument.TabIndex = 21;
            this.selectDocument.Text = "Seleziona un documento";
            this.selectDocument.UseVisualStyleBackColor = false;
            this.selectDocument.Click += new System.EventHandler(this.SelectDocument_Click);
            // 
            // labelChooseBetweenDragOrSelection
            // 
            this.labelChooseBetweenDragOrSelection.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.labelChooseBetweenDragOrSelection.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelChooseBetweenDragOrSelection.Location = new System.Drawing.Point(58, 184);
            this.labelChooseBetweenDragOrSelection.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelChooseBetweenDragOrSelection.Name = "labelChooseBetweenDragOrSelection";
            this.labelChooseBetweenDragOrSelection.Size = new System.Drawing.Size(377, 25);
            this.labelChooseBetweenDragOrSelection.TabIndex = 20;
            this.labelChooseBetweenDragOrSelection.Text = "oppure";
            this.labelChooseBetweenDragOrSelection.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelDragAndDropDocumentInformation
            // 
            this.labelDragAndDropDocumentInformation.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelDragAndDropDocumentInformation.Location = new System.Drawing.Point(58, 124);
            this.labelDragAndDropDocumentInformation.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelDragAndDropDocumentInformation.Name = "labelDragAndDropDocumentInformation";
            this.labelDragAndDropDocumentInformation.Size = new System.Drawing.Size(377, 45);
            this.labelDragAndDropDocumentInformation.TabIndex = 19;
            this.labelDragAndDropDocumentInformation.Text = "Trascina i tuoi documenti qui dentro per firmarli o per verificare una firma elet" +
    "tronica esistente";
            this.labelDragAndDropDocumentInformation.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // pbFolder
            // 
            this.pbFolder.Image = ((System.Drawing.Image)(resources.GetObject("pbFolder.Image")));
            this.pbFolder.InitialImage = null;
            this.pbFolder.Location = new System.Drawing.Point(201, 14);
            this.pbFolder.Name = "pbFolder";
            this.pbFolder.Size = new System.Drawing.Size(85, 104);
            this.pbFolder.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pbFolder.TabIndex = 0;
            this.pbFolder.TabStop = false;
            // 
            // digitalSignatureLabel
            // 
            this.digitalSignatureLabel.AutoSize = true;
            this.digitalSignatureLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.digitalSignatureLabel.Location = new System.Drawing.Point(200, 18);
            this.digitalSignatureLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.digitalSignatureLabel.Name = "digitalSignatureLabel";
            this.digitalSignatureLabel.Size = new System.Drawing.Size(255, 36);
            this.digitalSignatureLabel.TabIndex = 16;
            this.digitalSignatureLabel.Text = "Firma Elettronica";
            // 
            // tabSelectOp
            // 
            this.tabSelectOp.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.tabSelectOp.Controls.Add(this.cancelOperationButton);
            this.tabSelectOp.Controls.Add(this.panelChooseSignOrVerify);
            this.tabSelectOp.Controls.Add(this.labelDigitalSignatureHeaderTextChoosingOperation);
            this.tabSelectOp.Location = new System.Drawing.Point(4, 9);
            this.tabSelectOp.Name = "tabSelectOp";
            this.tabSelectOp.Size = new System.Drawing.Size(649, 533);
            this.tabSelectOp.TabIndex = 11;
            this.tabSelectOp.Text = "tabPage11";
            // 
            // cancelOperationButton
            // 
            this.cancelOperationButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.cancelOperationButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.cancelOperationButton.FlatAppearance.BorderSize = 0;
            this.cancelOperationButton.ForeColor = System.Drawing.Color.White;
            this.cancelOperationButton.Location = new System.Drawing.Point(256, 484);
            this.cancelOperationButton.Margin = new System.Windows.Forms.Padding(2);
            this.cancelOperationButton.Name = "cancelOperationButton";
            this.cancelOperationButton.Size = new System.Drawing.Size(157, 28);
            this.cancelOperationButton.TabIndex = 20;
            this.cancelOperationButton.Text = "Annulla";
            this.cancelOperationButton.UseVisualStyleBackColor = false;
            this.cancelOperationButton.Click += new System.EventHandler(this.CancelOperationButton_Click);
            // 
            // panelChooseSignOrVerify
            // 
            this.panelChooseSignOrVerify.Controls.Add(this.signOperationOptionPanel);
            this.panelChooseSignOrVerify.Controls.Add(this.verifyOptionPanel);
            this.panelChooseSignOrVerify.Controls.Add(this.labelFileNamePathInOperationChooser);
            this.panelChooseSignOrVerify.Controls.Add(this.pictureBox16);
            this.panelChooseSignOrVerify.Location = new System.Drawing.Point(73, 104);
            this.panelChooseSignOrVerify.Name = "panelChooseSignOrVerify";
            this.panelChooseSignOrVerify.Size = new System.Drawing.Size(501, 362);
            this.panelChooseSignOrVerify.TabIndex = 18;
            // 
            // signOperationOptionPanel
            // 
            this.signOperationOptionPanel.Controls.Add(this.signOptionChooserLabel);
            this.signOperationOptionPanel.Controls.Add(this.pictureBox17);
            this.signOperationOptionPanel.Location = new System.Drawing.Point(133, 128);
            this.signOperationOptionPanel.Name = "signOperationOptionPanel";
            this.signOperationOptionPanel.Size = new System.Drawing.Size(247, 99);
            this.signOperationOptionPanel.TabIndex = 3;
            this.signOperationOptionPanel.Click += new System.EventHandler(this.SignOperationOptionPanel_MouseClick);
            this.signOperationOptionPanel.MouseEnter += new System.EventHandler(this.SignOperationOptionPanel_MouseEnter);
            this.signOperationOptionPanel.MouseLeave += new System.EventHandler(this.SignOperationOptionPanel_MouseLeave);
            // 
            // signOptionChooserLabel
            // 
            this.signOptionChooserLabel.AutoSize = true;
            this.signOptionChooserLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.signOptionChooserLabel.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.signOptionChooserLabel.Location = new System.Drawing.Point(116, 43);
            this.signOptionChooserLabel.Name = "signOptionChooserLabel";
            this.signOptionChooserLabel.Size = new System.Drawing.Size(128, 25);
            this.signOptionChooserLabel.TabIndex = 3;
            this.signOptionChooserLabel.Text = "Firma           >";
            this.signOptionChooserLabel.Click += new System.EventHandler(this.SignOperationOptionPanel_MouseClick);
            this.signOptionChooserLabel.MouseEnter += new System.EventHandler(this.SignOperationOptionPanel_MouseEnter);
            this.signOptionChooserLabel.MouseLeave += new System.EventHandler(this.SignOperationOptionPanel_MouseLeave);
            // 
            // pictureBox17
            // 
            this.pictureBox17.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox17.Image")));
            this.pictureBox17.InitialImage = null;
            this.pictureBox17.Location = new System.Drawing.Point(9, 14);
            this.pictureBox17.Name = "pictureBox17";
            this.pictureBox17.Size = new System.Drawing.Size(65, 82);
            this.pictureBox17.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox17.TabIndex = 2;
            this.pictureBox17.TabStop = false;
            // 
            // verifyOptionPanel
            // 
            this.verifyOptionPanel.Controls.Add(this.pictureBox18);
            this.verifyOptionPanel.Controls.Add(this.verifyOptionChooserLabel);
            this.verifyOptionPanel.Location = new System.Drawing.Point(133, 242);
            this.verifyOptionPanel.Name = "verifyOptionPanel";
            this.verifyOptionPanel.Size = new System.Drawing.Size(247, 99);
            this.verifyOptionPanel.TabIndex = 4;
            this.verifyOptionPanel.Click += new System.EventHandler(this.pnVerificaOp_MouseClick);
            this.verifyOptionPanel.MouseEnter += new System.EventHandler(this.pnVerificaOp_MouseEnter);
            this.verifyOptionPanel.MouseLeave += new System.EventHandler(this.pnVerificaOp_MouseLeave);
            // 
            // pictureBox18
            // 
            this.pictureBox18.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox18.Image")));
            this.pictureBox18.InitialImage = null;
            this.pictureBox18.Location = new System.Drawing.Point(9, 14);
            this.pictureBox18.Name = "pictureBox18";
            this.pictureBox18.Size = new System.Drawing.Size(65, 82);
            this.pictureBox18.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox18.TabIndex = 5;
            this.pictureBox18.TabStop = false;
            // 
            // verifyOptionChooserLabel
            // 
            this.verifyOptionChooserLabel.AutoSize = true;
            this.verifyOptionChooserLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.verifyOptionChooserLabel.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.verifyOptionChooserLabel.Location = new System.Drawing.Point(116, 43);
            this.verifyOptionChooserLabel.Name = "verifyOptionChooserLabel";
            this.verifyOptionChooserLabel.Size = new System.Drawing.Size(129, 25);
            this.verifyOptionChooserLabel.TabIndex = 4;
            this.verifyOptionChooserLabel.Text = "Verifica        >";
            this.verifyOptionChooserLabel.Click += new System.EventHandler(this.pnVerificaOp_MouseClick);
            this.verifyOptionChooserLabel.MouseEnter += new System.EventHandler(this.pnVerificaOp_MouseEnter);
            this.verifyOptionChooserLabel.MouseLeave += new System.EventHandler(this.pnVerificaOp_MouseLeave);
            // 
            // labelFileNamePathInOperationChooser
            // 
            this.labelFileNamePathInOperationChooser.AutoEllipsis = true;
            this.labelFileNamePathInOperationChooser.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelFileNamePathInOperationChooser.Location = new System.Drawing.Point(142, 23);
            this.labelFileNamePathInOperationChooser.Name = "labelFileNamePathInOperationChooser";
            this.labelFileNamePathInOperationChooser.Size = new System.Drawing.Size(320, 51);
            this.labelFileNamePathInOperationChooser.TabIndex = 2;
            this.labelFileNamePathInOperationChooser.Text = "label32label32label32label32label32label32label32label32label32label32label32labe" +
    "l32label32label32label32label32label32label32";
            // 
            // pictureBox16
            // 
            this.pictureBox16.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox16.Image")));
            this.pictureBox16.InitialImage = null;
            this.pictureBox16.Location = new System.Drawing.Point(61, 3);
            this.pictureBox16.Name = "pictureBox16";
            this.pictureBox16.Size = new System.Drawing.Size(75, 89);
            this.pictureBox16.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox16.TabIndex = 1;
            this.pictureBox16.TabStop = false;
            // 
            // labelDigitalSignatureHeaderTextChoosingOperation
            // 
            this.labelDigitalSignatureHeaderTextChoosingOperation.AutoSize = true;
            this.labelDigitalSignatureHeaderTextChoosingOperation.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelDigitalSignatureHeaderTextChoosingOperation.Location = new System.Drawing.Point(200, 18);
            this.labelDigitalSignatureHeaderTextChoosingOperation.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelDigitalSignatureHeaderTextChoosingOperation.Name = "labelDigitalSignatureHeaderTextChoosingOperation";
            this.labelDigitalSignatureHeaderTextChoosingOperation.Size = new System.Drawing.Size(255, 36);
            this.labelDigitalSignatureHeaderTextChoosingOperation.TabIndex = 17;
            this.labelDigitalSignatureHeaderTextChoosingOperation.Text = "Firma Elettronica";
            // 
            // tabSelectSignOp
            // 
            this.tabSelectSignOp.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.tabSelectSignOp.Controls.Add(this.proceedWithSignatureButton);
            this.tabSelectSignOp.Controls.Add(this.labelDigitalSignatureDescriptionChoosingFormat);
            this.tabSelectSignOp.Controls.Add(this.cancelSigningOperationButton);
            this.tabSelectSignOp.Controls.Add(this.panelChooseSignatureType);
            this.tabSelectSignOp.Controls.Add(this.labelDigitalSignatureHeaderTextChoosingFormat);
            this.tabSelectSignOp.Location = new System.Drawing.Point(4, 9);
            this.tabSelectSignOp.Name = "tabSelectSignOp";
            this.tabSelectSignOp.Size = new System.Drawing.Size(649, 533);
            this.tabSelectSignOp.TabIndex = 12;
            this.tabSelectSignOp.Text = "tabPage11";
            // 
            // proceedWithSignatureButton
            // 
            this.proceedWithSignatureButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.proceedWithSignatureButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.proceedWithSignatureButton.Enabled = false;
            this.proceedWithSignatureButton.FlatAppearance.BorderSize = 0;
            this.proceedWithSignatureButton.ForeColor = System.Drawing.Color.White;
            this.proceedWithSignatureButton.Location = new System.Drawing.Point(392, 484);
            this.proceedWithSignatureButton.Margin = new System.Windows.Forms.Padding(2);
            this.proceedWithSignatureButton.Name = "proceedWithSignatureButton";
            this.proceedWithSignatureButton.Size = new System.Drawing.Size(157, 28);
            this.proceedWithSignatureButton.TabIndex = 25;
            this.proceedWithSignatureButton.Text = "PROSEGUI";
            this.proceedWithSignatureButton.UseVisualStyleBackColor = false;
            this.proceedWithSignatureButton.Click += new System.EventHandler(this.ProceedWithSignatureButton_Click);
            // 
            // labelDigitalSignatureDescriptionChoosingFormat
            // 
            this.labelDigitalSignatureDescriptionChoosingFormat.AutoSize = true;
            this.labelDigitalSignatureDescriptionChoosingFormat.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelDigitalSignatureDescriptionChoosingFormat.ForeColor = System.Drawing.SystemColors.ControlDark;
            this.labelDigitalSignatureDescriptionChoosingFormat.Location = new System.Drawing.Point(250, 65);
            this.labelDigitalSignatureDescriptionChoosingFormat.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelDigitalSignatureDescriptionChoosingFormat.Name = "labelDigitalSignatureDescriptionChoosingFormat";
            this.labelDigitalSignatureDescriptionChoosingFormat.Size = new System.Drawing.Size(148, 20);
            this.labelDigitalSignatureDescriptionChoosingFormat.TabIndex = 24;
            this.labelDigitalSignatureDescriptionChoosingFormat.Text = "Firma documento";
            // 
            // cancelSigningOperationButton
            // 
            this.cancelSigningOperationButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.cancelSigningOperationButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.cancelSigningOperationButton.FlatAppearance.BorderSize = 0;
            this.cancelSigningOperationButton.ForeColor = System.Drawing.Color.White;
            this.cancelSigningOperationButton.Location = new System.Drawing.Point(101, 484);
            this.cancelSigningOperationButton.Margin = new System.Windows.Forms.Padding(2);
            this.cancelSigningOperationButton.Name = "cancelSigningOperationButton";
            this.cancelSigningOperationButton.Size = new System.Drawing.Size(157, 28);
            this.cancelSigningOperationButton.TabIndex = 23;
            this.cancelSigningOperationButton.Text = "Annulla";
            this.cancelSigningOperationButton.UseVisualStyleBackColor = false;
            this.cancelSigningOperationButton.Click += new System.EventHandler(this.CancelSigningOperationButton_Click);
            // 
            // panelChooseSignatureType
            // 
            this.panelChooseSignatureType.Controls.Add(this.panelChoosePades);
            this.panelChooseSignatureType.Controls.Add(this.labelSelectSignatureFormatType);
            this.panelChooseSignatureType.Controls.Add(this.panelChooseCades);
            this.panelChooseSignatureType.Controls.Add(this.labelFileNamePathInSigningFormatChooser);
            this.panelChooseSignatureType.Controls.Add(this.pictureBox21);
            this.panelChooseSignatureType.Location = new System.Drawing.Point(73, 104);
            this.panelChooseSignatureType.Name = "panelChooseSignatureType";
            this.panelChooseSignatureType.Size = new System.Drawing.Size(501, 362);
            this.panelChooseSignatureType.TabIndex = 22;
            // 
            // panelChoosePades
            // 
            this.panelChoosePades.Controls.Add(this.enableGraphicSignatureCheckBox);
            this.panelChoosePades.Controls.Add(this.PAdESFormatDescriptionLabel);
            this.panelChoosePades.Controls.Add(this.PAdESSignatureLabel);
            this.panelChoosePades.Controls.Add(this.pbPades);
            this.panelChoosePades.Location = new System.Drawing.Point(256, 160);
            this.panelChoosePades.Name = "panelChoosePades";
            this.panelChoosePades.Size = new System.Drawing.Size(220, 180);
            this.panelChoosePades.TabIndex = 26;
            this.panelChoosePades.Click += new System.EventHandler(this.PanelChoosePades_MouseClick);
            this.panelChoosePades.MouseEnter += new System.EventHandler(this.PanelChoosePades_MouseEnter);
            this.panelChoosePades.MouseLeave += new System.EventHandler(this.PanelChoosePades_MouseLeave);
            // 
            // enableGraphicSignatureCheckBox
            // 
            this.enableGraphicSignatureCheckBox.AutoSize = true;
            this.enableGraphicSignatureCheckBox.ForeColor = System.Drawing.SystemColors.GrayText;
            this.enableGraphicSignatureCheckBox.Location = new System.Drawing.Point(21, 160);
            this.enableGraphicSignatureCheckBox.Name = "enableGraphicSignatureCheckBox";
            this.enableGraphicSignatureCheckBox.Size = new System.Drawing.Size(127, 17);
            this.enableGraphicSignatureCheckBox.TabIndex = 6;
            this.enableGraphicSignatureCheckBox.Text = "Aggiungi firma grafica";
            this.enableGraphicSignatureCheckBox.UseVisualStyleBackColor = true;
            this.enableGraphicSignatureCheckBox.CheckedChanged += new System.EventHandler(this.EnableGraphicSignatureCheckBox_CheckedChanged);
            this.enableGraphicSignatureCheckBox.Click += new System.EventHandler(this.PanelChoosePades_MouseClick);
            this.enableGraphicSignatureCheckBox.MouseEnter += new System.EventHandler(this.PanelChoosePades_MouseEnter);
            this.enableGraphicSignatureCheckBox.MouseLeave += new System.EventHandler(this.PanelChoosePades_MouseLeave);
            // 
            // PAdESFormatDescriptionLabel
            // 
            this.PAdESFormatDescriptionLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PAdESFormatDescriptionLabel.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.PAdESFormatDescriptionLabel.Location = new System.Drawing.Point(59, 51);
            this.PAdESFormatDescriptionLabel.Name = "PAdESFormatDescriptionLabel";
            this.PAdESFormatDescriptionLabel.Size = new System.Drawing.Size(155, 106);
            this.PAdESFormatDescriptionLabel.TabIndex = 4;
            this.PAdESFormatDescriptionLabel.Text = "Si appone su documenti PDF nella versione grafica oppure in maniera invisibile. I" +
    "l documento firmato avrà estensione .pdf.";
            this.PAdESFormatDescriptionLabel.Click += new System.EventHandler(this.PanelChoosePades_MouseClick);
            this.PAdESFormatDescriptionLabel.MouseEnter += new System.EventHandler(this.PanelChoosePades_MouseEnter);
            this.PAdESFormatDescriptionLabel.MouseLeave += new System.EventHandler(this.PanelChoosePades_MouseLeave);
            // 
            // PAdESSignatureLabel
            // 
            this.PAdESSignatureLabel.AutoSize = true;
            this.PAdESSignatureLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PAdESSignatureLabel.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.PAdESSignatureLabel.Location = new System.Drawing.Point(59, 23);
            this.PAdESSignatureLabel.Name = "PAdESSignatureLabel";
            this.PAdESSignatureLabel.Size = new System.Drawing.Size(100, 18);
            this.PAdESSignatureLabel.TabIndex = 3;
            this.PAdESSignatureLabel.Text = "Firma PADES";
            this.PAdESSignatureLabel.Click += new System.EventHandler(this.PanelChoosePades_MouseClick);
            this.PAdESSignatureLabel.MouseEnter += new System.EventHandler(this.PanelChoosePades_MouseEnter);
            this.PAdESSignatureLabel.MouseLeave += new System.EventHandler(this.PanelChoosePades_MouseLeave);
            // 
            // pbPades
            // 
            this.pbPades.Image = global::CIEID.Properties.Resources.pdf_2x_gray;
            this.pbPades.InitialImage = null;
            this.pbPades.Location = new System.Drawing.Point(3, 11);
            this.pbPades.Name = "pbPades";
            this.pbPades.Size = new System.Drawing.Size(47, 59);
            this.pbPades.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pbPades.TabIndex = 2;
            this.pbPades.TabStop = false;
            this.pbPades.Click += new System.EventHandler(this.PanelChoosePades_MouseClick);
            this.pbPades.MouseEnter += new System.EventHandler(this.PanelChoosePades_MouseEnter);
            this.pbPades.MouseLeave += new System.EventHandler(this.PanelChoosePades_MouseLeave);
            // 
            // labelSelectSignatureFormatType
            // 
            this.labelSelectSignatureFormatType.AutoSize = true;
            this.labelSelectSignatureFormatType.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelSelectSignatureFormatType.ForeColor = System.Drawing.SystemColors.ControlDark;
            this.labelSelectSignatureFormatType.Location = new System.Drawing.Point(156, 121);
            this.labelSelectSignatureFormatType.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelSelectSignatureFormatType.Name = "labelSelectSignatureFormatType";
            this.labelSelectSignatureFormatType.Size = new System.Drawing.Size(200, 20);
            this.labelSelectSignatureFormatType.TabIndex = 25;
            this.labelSelectSignatureFormatType.Text = "Seleziona il tipo di firma";
            // 
            // panelChooseCades
            // 
            this.panelChooseCades.Controls.Add(this.CAdESFormatDescriptionLabel);
            this.panelChooseCades.Controls.Add(this.CAdESSignatureLabel);
            this.panelChooseCades.Controls.Add(this.CAdESP7MPictureBox);
            this.panelChooseCades.Location = new System.Drawing.Point(25, 160);
            this.panelChooseCades.Name = "panelChooseCades";
            this.panelChooseCades.Size = new System.Drawing.Size(210, 180);
            this.panelChooseCades.TabIndex = 3;
            this.panelChooseCades.Click += new System.EventHandler(this.PanelChooseCades_MouseClick);
            this.panelChooseCades.MouseEnter += new System.EventHandler(this.PanelChooseCades_MouseEnter);
            this.panelChooseCades.MouseLeave += new System.EventHandler(this.PanelChooseCades_MouseLeave);
            // 
            // CAdESFormatDescriptionLabel
            // 
            this.CAdESFormatDescriptionLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CAdESFormatDescriptionLabel.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.CAdESFormatDescriptionLabel.Location = new System.Drawing.Point(56, 51);
            this.CAdESFormatDescriptionLabel.Name = "CAdESFormatDescriptionLabel";
            this.CAdESFormatDescriptionLabel.Size = new System.Drawing.Size(145, 108);
            this.CAdESFormatDescriptionLabel.TabIndex = 4;
            this.CAdESFormatDescriptionLabel.Text = "Si appone su una qualsiasi tipologia di documento e prevede la generazione di una" +
    " busta crittografica. Il documento firmato avrà estensione .p7m.";
            this.CAdESFormatDescriptionLabel.Click += new System.EventHandler(this.PanelChooseCades_MouseClick);
            this.CAdESFormatDescriptionLabel.MouseEnter += new System.EventHandler(this.PanelChooseCades_MouseEnter);
            this.CAdESFormatDescriptionLabel.MouseLeave += new System.EventHandler(this.PanelChooseCades_MouseLeave);
            // 
            // CAdESSignatureLabel
            // 
            this.CAdESSignatureLabel.AutoSize = true;
            this.CAdESSignatureLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CAdESSignatureLabel.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.CAdESSignatureLabel.Location = new System.Drawing.Point(59, 23);
            this.CAdESSignatureLabel.Name = "CAdESSignatureLabel";
            this.CAdESSignatureLabel.Size = new System.Drawing.Size(101, 18);
            this.CAdESSignatureLabel.TabIndex = 3;
            this.CAdESSignatureLabel.Text = "Firma CADES";
            // 
            // CAdESP7MPictureBox
            // 
            this.CAdESP7MPictureBox.Image = global::CIEID.Properties.Resources.p7m_2x_gray;
            this.CAdESP7MPictureBox.InitialImage = null;
            this.CAdESP7MPictureBox.Location = new System.Drawing.Point(3, 11);
            this.CAdESP7MPictureBox.Name = "CAdESP7MPictureBox";
            this.CAdESP7MPictureBox.Size = new System.Drawing.Size(47, 59);
            this.CAdESP7MPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.CAdESP7MPictureBox.TabIndex = 2;
            this.CAdESP7MPictureBox.TabStop = false;
            this.CAdESP7MPictureBox.Click += new System.EventHandler(this.PanelChooseCades_MouseClick);
            this.CAdESP7MPictureBox.MouseEnter += new System.EventHandler(this.PanelChooseCades_MouseEnter);
            this.CAdESP7MPictureBox.MouseLeave += new System.EventHandler(this.PanelChooseCades_MouseLeave);
            // 
            // labelFileNamePathInSigningFormatChooser
            // 
            this.labelFileNamePathInSigningFormatChooser.AutoEllipsis = true;
            this.labelFileNamePathInSigningFormatChooser.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelFileNamePathInSigningFormatChooser.Location = new System.Drawing.Point(142, 23);
            this.labelFileNamePathInSigningFormatChooser.Name = "labelFileNamePathInSigningFormatChooser";
            this.labelFileNamePathInSigningFormatChooser.Size = new System.Drawing.Size(320, 51);
            this.labelFileNamePathInSigningFormatChooser.TabIndex = 2;
            this.labelFileNamePathInSigningFormatChooser.Text = "label32";
            // 
            // pictureBox21
            // 
            this.pictureBox21.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox21.Image")));
            this.pictureBox21.InitialImage = null;
            this.pictureBox21.Location = new System.Drawing.Point(61, 3);
            this.pictureBox21.Name = "pictureBox21";
            this.pictureBox21.Size = new System.Drawing.Size(75, 89);
            this.pictureBox21.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox21.TabIndex = 1;
            this.pictureBox21.TabStop = false;
            // 
            // labelDigitalSignatureHeaderTextChoosingFormat
            // 
            this.labelDigitalSignatureHeaderTextChoosingFormat.AutoSize = true;
            this.labelDigitalSignatureHeaderTextChoosingFormat.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelDigitalSignatureHeaderTextChoosingFormat.Location = new System.Drawing.Point(200, 18);
            this.labelDigitalSignatureHeaderTextChoosingFormat.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelDigitalSignatureHeaderTextChoosingFormat.Name = "labelDigitalSignatureHeaderTextChoosingFormat";
            this.labelDigitalSignatureHeaderTextChoosingFormat.Size = new System.Drawing.Size(255, 36);
            this.labelDigitalSignatureHeaderTextChoosingFormat.TabIndex = 21;
            this.labelDigitalSignatureHeaderTextChoosingFormat.Text = "Firma Elettronica";
            // 
            // graphicDigitalSignatureTab
            // 
            this.graphicDigitalSignatureTab.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.graphicDigitalSignatureTab.Controls.Add(this.labelDigitalSignatureDragSignatureBox);
            this.graphicDigitalSignatureTab.Controls.Add(this.panelPreviewGraphicSignature);
            this.graphicDigitalSignatureTab.Controls.Add(this.labelDigitalSignaturePlaceGraphicSignatureHeaderText);
            this.graphicDigitalSignatureTab.Controls.Add(this.proceedSignPreviewButton);
            this.graphicDigitalSignatureTab.Location = new System.Drawing.Point(4, 9);
            this.graphicDigitalSignatureTab.Name = "graphicDigitalSignatureTab";
            this.graphicDigitalSignatureTab.Size = new System.Drawing.Size(649, 533);
            this.graphicDigitalSignatureTab.TabIndex = 13;
            this.graphicDigitalSignatureTab.Text = "tabPage11";
            // 
            // labelDigitalSignatureDragSignatureBox
            // 
            this.labelDigitalSignatureDragSignatureBox.AutoSize = true;
            this.labelDigitalSignatureDragSignatureBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelDigitalSignatureDragSignatureBox.ForeColor = System.Drawing.SystemColors.ControlDark;
            this.labelDigitalSignatureDragSignatureBox.Location = new System.Drawing.Point(53, 65);
            this.labelDigitalSignatureDragSignatureBox.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelDigitalSignatureDragSignatureBox.Name = "labelDigitalSignatureDragSignatureBox";
            this.labelDigitalSignatureDragSignatureBox.Size = new System.Drawing.Size(532, 20);
            this.labelDigitalSignatureDragSignatureBox.TabIndex = 28;
            this.labelDigitalSignatureDragSignatureBox.Text = "Trascina la firma in un punto desiderato all\'interno del documento";
            // 
            // panelPreviewGraphicSignature
            // 
            this.panelPreviewGraphicSignature.Controls.Add(this.downButton);
            this.panelPreviewGraphicSignature.Controls.Add(this.upButton);
            this.panelPreviewGraphicSignature.Controls.Add(this.panePreview);
            this.panelPreviewGraphicSignature.Controls.Add(this.labelFileNamePathInDragSignatureBox);
            this.panelPreviewGraphicSignature.Controls.Add(this.pictureBox22);
            this.panelPreviewGraphicSignature.Location = new System.Drawing.Point(73, 104);
            this.panelPreviewGraphicSignature.Name = "panelPreviewGraphicSignature";
            this.panelPreviewGraphicSignature.Size = new System.Drawing.Size(501, 375);
            this.panelPreviewGraphicSignature.TabIndex = 27;
            // 
            // downButton
            // 
            this.downButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.downButton.BackColor = System.Drawing.Color.White;
            this.downButton.FlatAppearance.BorderSize = 0;
            this.downButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.downButton.ForeColor = System.Drawing.Color.White;
            this.downButton.Image = ((System.Drawing.Image)(resources.GetObject("downButton.Image")));
            this.downButton.Location = new System.Drawing.Point(450, 298);
            this.downButton.Margin = new System.Windows.Forms.Padding(2);
            this.downButton.Name = "downButton";
            this.downButton.Size = new System.Drawing.Size(34, 35);
            this.downButton.TabIndex = 31;
            this.downButton.UseVisualStyleBackColor = false;
            this.downButton.Click += new System.EventHandler(this.DownButton_Click);
            // 
            // upButton
            // 
            this.upButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.upButton.BackColor = System.Drawing.Color.White;
            this.upButton.FlatAppearance.BorderSize = 0;
            this.upButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.upButton.ForeColor = System.Drawing.Color.White;
            this.upButton.Image = ((System.Drawing.Image)(resources.GetObject("upButton.Image")));
            this.upButton.Location = new System.Drawing.Point(450, 168);
            this.upButton.Margin = new System.Windows.Forms.Padding(2);
            this.upButton.Name = "upButton";
            this.upButton.Size = new System.Drawing.Size(34, 26);
            this.upButton.TabIndex = 30;
            this.upButton.UseVisualStyleBackColor = false;
            this.upButton.Click += new System.EventHandler(this.UpButton_Click);
            // 
            // panePreview
            // 
            this.panePreview.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.panePreview.Location = new System.Drawing.Point(44, 108);
            this.panePreview.Name = "panePreview";
            this.panePreview.Size = new System.Drawing.Size(401, 264);
            this.panePreview.TabIndex = 0;
            // 
            // labelFileNamePathInDragSignatureBox
            // 
            this.labelFileNamePathInDragSignatureBox.AutoEllipsis = true;
            this.labelFileNamePathInDragSignatureBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelFileNamePathInDragSignatureBox.Location = new System.Drawing.Point(142, 23);
            this.labelFileNamePathInDragSignatureBox.Name = "labelFileNamePathInDragSignatureBox";
            this.labelFileNamePathInDragSignatureBox.Size = new System.Drawing.Size(320, 51);
            this.labelFileNamePathInDragSignatureBox.TabIndex = 2;
            this.labelFileNamePathInDragSignatureBox.Text = "label32";
            // 
            // pictureBox22
            // 
            this.pictureBox22.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox22.Image")));
            this.pictureBox22.InitialImage = null;
            this.pictureBox22.Location = new System.Drawing.Point(61, 3);
            this.pictureBox22.Name = "pictureBox22";
            this.pictureBox22.Size = new System.Drawing.Size(75, 89);
            this.pictureBox22.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox22.TabIndex = 1;
            this.pictureBox22.TabStop = false;
            // 
            // labelDigitalSignaturePlaceGraphicSignatureHeaderText
            // 
            this.labelDigitalSignaturePlaceGraphicSignatureHeaderText.AutoSize = true;
            this.labelDigitalSignaturePlaceGraphicSignatureHeaderText.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelDigitalSignaturePlaceGraphicSignatureHeaderText.Location = new System.Drawing.Point(200, 18);
            this.labelDigitalSignaturePlaceGraphicSignatureHeaderText.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelDigitalSignaturePlaceGraphicSignatureHeaderText.Name = "labelDigitalSignaturePlaceGraphicSignatureHeaderText";
            this.labelDigitalSignaturePlaceGraphicSignatureHeaderText.Size = new System.Drawing.Size(255, 36);
            this.labelDigitalSignaturePlaceGraphicSignatureHeaderText.TabIndex = 26;
            this.labelDigitalSignaturePlaceGraphicSignatureHeaderText.Text = "Firma Elettronica";
            // 
            // proceedSignPreviewButton
            // 
            this.proceedSignPreviewButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.proceedSignPreviewButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.proceedSignPreviewButton.FlatAppearance.BorderSize = 0;
            this.proceedSignPreviewButton.ForeColor = System.Drawing.Color.White;
            this.proceedSignPreviewButton.Location = new System.Drawing.Point(233, 484);
            this.proceedSignPreviewButton.Margin = new System.Windows.Forms.Padding(2);
            this.proceedSignPreviewButton.Name = "proceedSignPreviewButton";
            this.proceedSignPreviewButton.Size = new System.Drawing.Size(157, 28);
            this.proceedSignPreviewButton.TabIndex = 29;
            this.proceedSignPreviewButton.Text = "PROSEGUI";
            this.proceedSignPreviewButton.UseVisualStyleBackColor = false;
            this.proceedSignPreviewButton.Click += new System.EventHandler(this.ProceedSignPreviewButton_Click);
            // 
            // signPanelTabPage
            // 
            this.signPanelTabPage.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.signPanelTabPage.Controls.Add(this.closeButton);
            this.signPanelTabPage.Controls.Add(this.signButton);
            this.signPanelTabPage.Controls.Add(this.cancelSigningOperationPINPanelButton);
            this.signPanelTabPage.Controls.Add(this.placeCIEOnReaderLabel);
            this.signPanelTabPage.Controls.Add(this.panelTypePINAndSign);
            this.signPanelTabPage.Controls.Add(this.labelDigitalSignatureHeaderText);
            this.signPanelTabPage.Location = new System.Drawing.Point(4, 9);
            this.signPanelTabPage.Name = "signPanelTabPage";
            this.signPanelTabPage.Size = new System.Drawing.Size(649, 533);
            this.signPanelTabPage.TabIndex = 14;
            // 
            // closeButton
            // 
            this.closeButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.closeButton.FlatAppearance.BorderSize = 0;
            this.closeButton.ForeColor = System.Drawing.Color.White;
            this.closeButton.Location = new System.Drawing.Point(240, 484);
            this.closeButton.Margin = new System.Windows.Forms.Padding(2);
            this.closeButton.Name = "closeButton";
            this.closeButton.Size = new System.Drawing.Size(156, 30);
            this.closeButton.TabIndex = 32;
            this.closeButton.Text = "CONCLUDI";
            this.closeButton.UseVisualStyleBackColor = false;
            this.closeButton.Visible = false;
            this.closeButton.Click += new System.EventHandler(this.CloseButton_Click);
            // 
            // signButton
            // 
            this.signButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.signButton.Enabled = false;
            this.signButton.FlatAppearance.BorderSize = 0;
            this.signButton.ForeColor = System.Drawing.Color.White;
            this.signButton.Location = new System.Drawing.Point(392, 484);
            this.signButton.Margin = new System.Windows.Forms.Padding(2);
            this.signButton.Name = "signButton";
            this.signButton.Size = new System.Drawing.Size(156, 30);
            this.signButton.TabIndex = 31;
            this.signButton.Text = "FIRMA";
            this.signButton.UseVisualStyleBackColor = false;
            this.signButton.Click += new System.EventHandler(this.SignButton_Click);
            // 
            // cancelSigningOperationPINPanelButton
            // 
            this.cancelSigningOperationPINPanelButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.cancelSigningOperationPINPanelButton.FlatAppearance.BorderSize = 0;
            this.cancelSigningOperationPINPanelButton.ForeColor = System.Drawing.Color.White;
            this.cancelSigningOperationPINPanelButton.Location = new System.Drawing.Point(101, 484);
            this.cancelSigningOperationPINPanelButton.Margin = new System.Windows.Forms.Padding(2);
            this.cancelSigningOperationPINPanelButton.Name = "cancelSigningOperationPINPanelButton";
            this.cancelSigningOperationPINPanelButton.Size = new System.Drawing.Size(156, 30);
            this.cancelSigningOperationPINPanelButton.TabIndex = 30;
            this.cancelSigningOperationPINPanelButton.Text = "Annulla";
            this.cancelSigningOperationPINPanelButton.UseVisualStyleBackColor = false;
            this.cancelSigningOperationPINPanelButton.Click += new System.EventHandler(this.CancelButton_Click);
            // 
            // placeCIEOnReaderLabel
            // 
            this.placeCIEOnReaderLabel.AutoSize = true;
            this.placeCIEOnReaderLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.placeCIEOnReaderLabel.ForeColor = System.Drawing.SystemColors.ControlDark;
            this.placeCIEOnReaderLabel.Location = new System.Drawing.Point(210, 65);
            this.placeCIEOnReaderLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.placeCIEOnReaderLabel.Name = "placeCIEOnReaderLabel";
            this.placeCIEOnReaderLabel.Size = new System.Drawing.Size(235, 20);
            this.placeCIEOnReaderLabel.TabIndex = 31;
            this.placeCIEOnReaderLabel.Text = "Appoggia la carta sul lettore";
            // 
            // panelTypePINAndSign
            // 
            this.panelTypePINAndSign.Controls.Add(this.panelSigningBoxPINArea);
            this.panelTypePINAndSign.Controls.Add(this.labelFileNamePathSigningOperation);
            this.panelTypePINAndSign.Controls.Add(this.pictureBox19);
            this.panelTypePINAndSign.Location = new System.Drawing.Point(73, 104);
            this.panelTypePINAndSign.Name = "panelTypePINAndSign";
            this.panelTypePINAndSign.Size = new System.Drawing.Size(501, 362);
            this.panelTypePINAndSign.TabIndex = 30;
            // 
            // panelSigningBoxPINArea
            // 
            this.panelSigningBoxPINArea.Controls.Add(this.pictureBox20);
            this.panelSigningBoxPINArea.Controls.Add(this.typePINLabel);
            this.panelSigningBoxPINArea.Controls.Add(this.documentSuccessfullySignedLabel);
            this.panelSigningBoxPINArea.Controls.Add(this.signPINDigit_TextBox1);
            this.panelSigningBoxPINArea.Controls.Add(this.signProgressBar);
            this.panelSigningBoxPINArea.Controls.Add(this.signPINDigit_TextBox2);
            this.panelSigningBoxPINArea.Controls.Add(this.digitalSignatureCompletedPictureBox);
            this.panelSigningBoxPINArea.Controls.Add(this.signPINDigit_TextBox3);
            this.panelSigningBoxPINArea.Controls.Add(this.signPINDigit_TextBox4);
            this.panelSigningBoxPINArea.Controls.Add(this.signPINDigit_TextBox5);
            this.panelSigningBoxPINArea.Controls.Add(this.signPINDigit_TextBox6);
            this.panelSigningBoxPINArea.Controls.Add(this.signPINDigit_TextBox7);
            this.panelSigningBoxPINArea.Controls.Add(this.signPINDigit_TextBox8);
            this.panelSigningBoxPINArea.Location = new System.Drawing.Point(3, 108);
            this.panelSigningBoxPINArea.Name = "panelSigningBoxPINArea";
            this.panelSigningBoxPINArea.Size = new System.Drawing.Size(495, 239);
            this.panelSigningBoxPINArea.TabIndex = 31;
            // 
            // pictureBox20
            // 
            this.pictureBox20.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox20.Image")));
            this.pictureBox20.Location = new System.Drawing.Point(14, 38);
            this.pictureBox20.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox20.Name = "pictureBox20";
            this.pictureBox20.Size = new System.Drawing.Size(153, 153);
            this.pictureBox20.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox20.TabIndex = 26;
            this.pictureBox20.TabStop = false;
            // 
            // typePINLabel
            // 
            this.typePINLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.typePINLabel.Location = new System.Drawing.Point(190, 68);
            this.typePINLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.typePINLabel.Name = "typePINLabel";
            this.typePINLabel.Size = new System.Drawing.Size(276, 20);
            this.typePINLabel.TabIndex = 27;
            this.typePINLabel.Text = "Inserisci le ultime 4 cifre del PIN";
            // 
            // documentSuccessfullySignedLabel
            // 
            this.documentSuccessfullySignedLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.documentSuccessfullySignedLabel.Location = new System.Drawing.Point(240, 102);
            this.documentSuccessfullySignedLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.documentSuccessfullySignedLabel.Name = "documentSuccessfullySignedLabel";
            this.documentSuccessfullySignedLabel.Size = new System.Drawing.Size(226, 20);
            this.documentSuccessfullySignedLabel.TabIndex = 30;
            this.documentSuccessfullySignedLabel.Text = "File firmato con successo";
            this.documentSuccessfullySignedLabel.Visible = false;
            // 
            // signPINDigit_TextBox1
            // 
            this.signPINDigit_TextBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.signPINDigit_TextBox1.ForeColor = System.Drawing.SystemColors.WindowText;
            this.signPINDigit_TextBox1.Location = new System.Drawing.Point(267, 117);
            this.signPINDigit_TextBox1.Margin = new System.Windows.Forms.Padding(2);
            this.signPINDigit_TextBox1.Name = "signPINDigit_TextBox1";
            this.signPINDigit_TextBox1.Size = new System.Drawing.Size(20, 23);
            this.signPINDigit_TextBox1.TabIndex = 22;
            this.signPINDigit_TextBox1.Tag = "9";
            this.signPINDigit_TextBox1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.signPINDigit_TextBox1.UseSystemPasswordChar = true;
            this.signPINDigit_TextBox1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBoxSignPin_KeyPress);
            // 
            // signProgressBar
            // 
            this.signProgressBar.ForeColor = System.Drawing.Color.Blue;
            this.signProgressBar.Location = new System.Drawing.Point(202, 112);
            this.signProgressBar.Margin = new System.Windows.Forms.Padding(2);
            this.signProgressBar.Name = "signProgressBar";
            this.signProgressBar.Size = new System.Drawing.Size(228, 13);
            this.signProgressBar.TabIndex = 29;
            this.signProgressBar.Value = 55;
            this.signProgressBar.Visible = false;
            // 
            // signPINDigit_TextBox2
            // 
            this.signPINDigit_TextBox2.BackColor = System.Drawing.SystemColors.Window;
            this.signPINDigit_TextBox2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.signPINDigit_TextBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.signPINDigit_TextBox2.Location = new System.Drawing.Point(291, 117);
            this.signPINDigit_TextBox2.Margin = new System.Windows.Forms.Padding(2);
            this.signPINDigit_TextBox2.Name = "signPINDigit_TextBox2";
            this.signPINDigit_TextBox2.Size = new System.Drawing.Size(20, 23);
            this.signPINDigit_TextBox2.TabIndex = 23;
            this.signPINDigit_TextBox2.Tag = "10";
            this.signPINDigit_TextBox2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.signPINDigit_TextBox2.UseSystemPasswordChar = true;
            this.signPINDigit_TextBox2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBoxSignPin_KeyPress);
            // 
            // digitalSignatureCompletedPictureBox
            // 
            this.digitalSignatureCompletedPictureBox.Image = ((System.Drawing.Image)(resources.GetObject("digitalSignatureCompletedPictureBox.Image")));
            this.digitalSignatureCompletedPictureBox.InitialImage = null;
            this.digitalSignatureCompletedPictureBox.Location = new System.Drawing.Point(194, 90);
            this.digitalSignatureCompletedPictureBox.Name = "digitalSignatureCompletedPictureBox";
            this.digitalSignatureCompletedPictureBox.Size = new System.Drawing.Size(41, 42);
            this.digitalSignatureCompletedPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.digitalSignatureCompletedPictureBox.TabIndex = 28;
            this.digitalSignatureCompletedPictureBox.TabStop = false;
            this.digitalSignatureCompletedPictureBox.Visible = false;
            // 
            // signPINDigit_TextBox3
            // 
            this.signPINDigit_TextBox3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.signPINDigit_TextBox3.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.signPINDigit_TextBox3.Location = new System.Drawing.Point(315, 117);
            this.signPINDigit_TextBox3.Margin = new System.Windows.Forms.Padding(2);
            this.signPINDigit_TextBox3.Name = "signPINDigit_TextBox3";
            this.signPINDigit_TextBox3.Size = new System.Drawing.Size(20, 23);
            this.signPINDigit_TextBox3.TabIndex = 24;
            this.signPINDigit_TextBox3.Tag = "11";
            this.signPINDigit_TextBox3.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.signPINDigit_TextBox3.UseSystemPasswordChar = true;
            this.signPINDigit_TextBox3.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBoxSignPin_KeyPress);
            // 
            // signPINDigit_TextBox4
            // 
            this.signPINDigit_TextBox4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.signPINDigit_TextBox4.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.signPINDigit_TextBox4.Location = new System.Drawing.Point(339, 117);
            this.signPINDigit_TextBox4.Margin = new System.Windows.Forms.Padding(2);
            this.signPINDigit_TextBox4.Name = "signPINDigit_TextBox4";
            this.signPINDigit_TextBox4.Size = new System.Drawing.Size(20, 23);
            this.signPINDigit_TextBox4.TabIndex = 25;
            this.signPINDigit_TextBox4.Tag = "12";
            this.signPINDigit_TextBox4.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.signPINDigit_TextBox4.UseSystemPasswordChar = true;
            this.signPINDigit_TextBox4.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBoxSignPin_KeyPress);
            // 
            // signPINDigit_TextBox5
            // 
            this.signPINDigit_TextBox5.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.signPINDigit_TextBox5.ForeColor = System.Drawing.SystemColors.WindowText;
            this.signPINDigit_TextBox5.Location = new System.Drawing.Point(363, 117);
            this.signPINDigit_TextBox5.Margin = new System.Windows.Forms.Padding(2);
            this.signPINDigit_TextBox5.Name = "signPINDigit_TextBox5";
            this.signPINDigit_TextBox5.Size = new System.Drawing.Size(20, 23);
            this.signPINDigit_TextBox5.TabIndex = 26;
            this.signPINDigit_TextBox5.Tag = "13";
            this.signPINDigit_TextBox5.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.signPINDigit_TextBox5.UseSystemPasswordChar = true;
            this.signPINDigit_TextBox5.Visible = false;
            this.signPINDigit_TextBox5.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBoxSignPin_KeyPress);
            // 
            // signPINDigit_TextBox6
            // 
            this.signPINDigit_TextBox6.BackColor = System.Drawing.SystemColors.Window;
            this.signPINDigit_TextBox6.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.signPINDigit_TextBox6.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.signPINDigit_TextBox6.Location = new System.Drawing.Point(387, 117);
            this.signPINDigit_TextBox6.Margin = new System.Windows.Forms.Padding(2);
            this.signPINDigit_TextBox6.Name = "signPINDigit_TextBox6";
            this.signPINDigit_TextBox6.Size = new System.Drawing.Size(20, 23);
            this.signPINDigit_TextBox6.TabIndex = 27;
            this.signPINDigit_TextBox6.Tag = "14";
            this.signPINDigit_TextBox6.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.signPINDigit_TextBox6.UseSystemPasswordChar = true;
            this.signPINDigit_TextBox6.Visible = false;
            this.signPINDigit_TextBox6.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBoxSignPin_KeyPress);
            // 
            // signPINDigit_TextBox7
            // 
            this.signPINDigit_TextBox7.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.signPINDigit_TextBox7.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.signPINDigit_TextBox7.Location = new System.Drawing.Point(411, 117);
            this.signPINDigit_TextBox7.Margin = new System.Windows.Forms.Padding(2);
            this.signPINDigit_TextBox7.Name = "signPINDigit_TextBox7";
            this.signPINDigit_TextBox7.Size = new System.Drawing.Size(20, 23);
            this.signPINDigit_TextBox7.TabIndex = 28;
            this.signPINDigit_TextBox7.Tag = "15";
            this.signPINDigit_TextBox7.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.signPINDigit_TextBox7.UseSystemPasswordChar = true;
            this.signPINDigit_TextBox7.Visible = false;
            this.signPINDigit_TextBox7.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBoxSignPin_KeyPress);
            // 
            // signPINDigit_TextBox8
            // 
            this.signPINDigit_TextBox8.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.signPINDigit_TextBox8.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.signPINDigit_TextBox8.Location = new System.Drawing.Point(435, 117);
            this.signPINDigit_TextBox8.Margin = new System.Windows.Forms.Padding(2);
            this.signPINDigit_TextBox8.Name = "signPINDigit_TextBox8";
            this.signPINDigit_TextBox8.Size = new System.Drawing.Size(20, 23);
            this.signPINDigit_TextBox8.TabIndex = 29;
            this.signPINDigit_TextBox8.Tag = "16";
            this.signPINDigit_TextBox8.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.signPINDigit_TextBox8.UseSystemPasswordChar = true;
            this.signPINDigit_TextBox8.Visible = false;
            this.signPINDigit_TextBox8.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBoxSignPin_KeyPress);
            // 
            // labelFileNamePathSigningOperation
            // 
            this.labelFileNamePathSigningOperation.AutoEllipsis = true;
            this.labelFileNamePathSigningOperation.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelFileNamePathSigningOperation.Location = new System.Drawing.Point(142, 23);
            this.labelFileNamePathSigningOperation.Name = "labelFileNamePathSigningOperation";
            this.labelFileNamePathSigningOperation.Size = new System.Drawing.Size(320, 51);
            this.labelFileNamePathSigningOperation.TabIndex = 2;
            this.labelFileNamePathSigningOperation.Text = "label32";
            // 
            // pictureBox19
            // 
            this.pictureBox19.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox19.Image")));
            this.pictureBox19.InitialImage = null;
            this.pictureBox19.Location = new System.Drawing.Point(61, 3);
            this.pictureBox19.Name = "pictureBox19";
            this.pictureBox19.Size = new System.Drawing.Size(75, 89);
            this.pictureBox19.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox19.TabIndex = 1;
            this.pictureBox19.TabStop = false;
            // 
            // labelDigitalSignatureHeaderText
            // 
            this.labelDigitalSignatureHeaderText.AutoSize = true;
            this.labelDigitalSignatureHeaderText.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelDigitalSignatureHeaderText.Location = new System.Drawing.Point(200, 18);
            this.labelDigitalSignatureHeaderText.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelDigitalSignatureHeaderText.Name = "labelDigitalSignatureHeaderText";
            this.labelDigitalSignatureHeaderText.Size = new System.Drawing.Size(255, 36);
            this.labelDigitalSignatureHeaderText.TabIndex = 29;
            this.labelDigitalSignatureHeaderText.Text = "Firma Elettronica";
            // 
            // graphicSignatureCustomizationTab
            // 
            this.graphicSignatureCustomizationTab.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.graphicSignatureCustomizationTab.Controls.Add(this.generateCustomGraphicSignatureButton);
            this.graphicSignatureCustomizationTab.Controls.Add(this.panelDefineYourGraphicSignature);
            this.graphicSignatureCustomizationTab.Controls.Add(this.selectFileForGraphicSignatureCustomizationButton);
            this.graphicSignatureCustomizationTab.Controls.Add(this.cancelCustomizationButton);
            this.graphicSignatureCustomizationTab.Controls.Add(this.labelDigitalSignatureHeaderTextCreatingGraphicSignature);
            this.graphicSignatureCustomizationTab.Location = new System.Drawing.Point(4, 9);
            this.graphicSignatureCustomizationTab.Name = "graphicSignatureCustomizationTab";
            this.graphicSignatureCustomizationTab.Size = new System.Drawing.Size(649, 533);
            this.graphicSignatureCustomizationTab.TabIndex = 15;
            this.graphicSignatureCustomizationTab.Text = "tabPage11";
            // 
            // generateCustomGraphicSignatureButton
            // 
            this.generateCustomGraphicSignatureButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.generateCustomGraphicSignatureButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.generateCustomGraphicSignatureButton.FlatAppearance.BorderSize = 0;
            this.generateCustomGraphicSignatureButton.ForeColor = System.Drawing.Color.White;
            this.generateCustomGraphicSignatureButton.Location = new System.Drawing.Point(425, 484);
            this.generateCustomGraphicSignatureButton.Margin = new System.Windows.Forms.Padding(2);
            this.generateCustomGraphicSignatureButton.Name = "generateCustomGraphicSignatureButton";
            this.generateCustomGraphicSignatureButton.Size = new System.Drawing.Size(130, 28);
            this.generateCustomGraphicSignatureButton.TabIndex = 36;
            this.generateCustomGraphicSignatureButton.Text = "Crea firma";
            this.generateCustomGraphicSignatureButton.UseVisualStyleBackColor = false;
            this.generateCustomGraphicSignatureButton.Click += new System.EventHandler(this.GenerateCustomGraphicSignatureButton_Click);
            // 
            // panelDefineYourGraphicSignature
            // 
            this.panelDefineYourGraphicSignature.Controls.Add(this.graphicDigitalSignaturePanel);
            this.panelDefineYourGraphicSignature.Controls.Add(this.graphicSignatureCustomizationOverviewLabel);
            this.panelDefineYourGraphicSignature.Controls.Add(this.labelGraphicSignatureSecondDescription);
            this.panelDefineYourGraphicSignature.Location = new System.Drawing.Point(73, 104);
            this.panelDefineYourGraphicSignature.Name = "panelDefineYourGraphicSignature";
            this.panelDefineYourGraphicSignature.Size = new System.Drawing.Size(501, 362);
            this.panelDefineYourGraphicSignature.TabIndex = 35;
            // 
            // graphicDigitalSignaturePanel
            // 
            this.graphicDigitalSignaturePanel.Location = new System.Drawing.Point(19, 30);
            this.graphicDigitalSignaturePanel.Name = "graphicDigitalSignaturePanel";
            this.graphicDigitalSignaturePanel.Size = new System.Drawing.Size(463, 100);
            this.graphicDigitalSignaturePanel.TabIndex = 32;
            // 
            // graphicSignatureCustomizationOverviewLabel
            // 
            this.graphicSignatureCustomizationOverviewLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.graphicSignatureCustomizationOverviewLabel.Location = new System.Drawing.Point(23, 191);
            this.graphicSignatureCustomizationOverviewLabel.Name = "graphicSignatureCustomizationOverviewLabel";
            this.graphicSignatureCustomizationOverviewLabel.Size = new System.Drawing.Size(456, 49);
            this.graphicSignatureCustomizationOverviewLabel.TabIndex = 33;
            this.graphicSignatureCustomizationOverviewLabel.Text = "Abbiamo creato per te una firma grafica, ma se preferisci puoi personalizzarla. Q" +
    "uesto passaggio non è indispensabile, ma ti consentirà di dare un tocco personal" +
    "e ai documenti firmati.";
            // 
            // labelGraphicSignatureSecondDescription
            // 
            this.labelGraphicSignatureSecondDescription.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelGraphicSignatureSecondDescription.Location = new System.Drawing.Point(23, 288);
            this.labelGraphicSignatureSecondDescription.Name = "labelGraphicSignatureSecondDescription";
            this.labelGraphicSignatureSecondDescription.Size = new System.Drawing.Size(456, 57);
            this.labelGraphicSignatureSecondDescription.TabIndex = 34;
            this.labelGraphicSignatureSecondDescription.Text = "Puoi caricare un file in formato PNG, se non hai un file contenente una firma gra" +
    "fica puoi realizzarne uno utilizzanto l\'app CieSign disponibile per smartphone i" +
    "OS o Android";
            // 
            // selectFileForGraphicSignatureCustomizationButton
            // 
            this.selectFileForGraphicSignatureCustomizationButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.selectFileForGraphicSignatureCustomizationButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.selectFileForGraphicSignatureCustomizationButton.FlatAppearance.BorderSize = 0;
            this.selectFileForGraphicSignatureCustomizationButton.ForeColor = System.Drawing.Color.White;
            this.selectFileForGraphicSignatureCustomizationButton.Location = new System.Drawing.Point(264, 484);
            this.selectFileForGraphicSignatureCustomizationButton.Margin = new System.Windows.Forms.Padding(2);
            this.selectFileForGraphicSignatureCustomizationButton.Name = "selectFileForGraphicSignatureCustomizationButton";
            this.selectFileForGraphicSignatureCustomizationButton.Size = new System.Drawing.Size(130, 28);
            this.selectFileForGraphicSignatureCustomizationButton.TabIndex = 31;
            this.selectFileForGraphicSignatureCustomizationButton.Text = "Seleziona un file";
            this.selectFileForGraphicSignatureCustomizationButton.UseVisualStyleBackColor = false;
            this.selectFileForGraphicSignatureCustomizationButton.Click += new System.EventHandler(this.SelectFileForGraphicSignatureCustomizationButton_Click);
            // 
            // cancelCustomizationButton
            // 
            this.cancelCustomizationButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.cancelCustomizationButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.cancelCustomizationButton.FlatAppearance.BorderSize = 0;
            this.cancelCustomizationButton.ForeColor = System.Drawing.Color.White;
            this.cancelCustomizationButton.Location = new System.Drawing.Point(92, 484);
            this.cancelCustomizationButton.Margin = new System.Windows.Forms.Padding(2);
            this.cancelCustomizationButton.Name = "cancelCustomizationButton";
            this.cancelCustomizationButton.Size = new System.Drawing.Size(130, 28);
            this.cancelCustomizationButton.TabIndex = 30;
            this.cancelCustomizationButton.Text = "Indietro";
            this.cancelCustomizationButton.UseVisualStyleBackColor = false;
            this.cancelCustomizationButton.Click += new System.EventHandler(this.CancelCustomizationButton_Click);
            // 
            // labelDigitalSignatureHeaderTextCreatingGraphicSignature
            // 
            this.labelDigitalSignatureHeaderTextCreatingGraphicSignature.AutoSize = true;
            this.labelDigitalSignatureHeaderTextCreatingGraphicSignature.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelDigitalSignatureHeaderTextCreatingGraphicSignature.Location = new System.Drawing.Point(200, 18);
            this.labelDigitalSignatureHeaderTextCreatingGraphicSignature.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelDigitalSignatureHeaderTextCreatingGraphicSignature.Name = "labelDigitalSignatureHeaderTextCreatingGraphicSignature";
            this.labelDigitalSignatureHeaderTextCreatingGraphicSignature.Size = new System.Drawing.Size(255, 36);
            this.labelDigitalSignatureHeaderTextCreatingGraphicSignature.TabIndex = 27;
            this.labelDigitalSignatureHeaderTextCreatingGraphicSignature.Text = "Firma Elettronica";
            // 
            // documentVerifyTab
            // 
            this.documentVerifyTab.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.documentVerifyTab.Controls.Add(this.extractP7MButton);
            this.documentVerifyTab.Controls.Add(this.verifyPanel);
            this.documentVerifyTab.Controls.Add(this.verifyLoadingLabel);
            this.documentVerifyTab.Controls.Add(this.closeVerifyButton);
            this.documentVerifyTab.Controls.Add(this.labelVerifySignature);
            this.documentVerifyTab.Controls.Add(this.labelDigitalSignatureHeaderTextVerifyDocument);
            this.documentVerifyTab.Location = new System.Drawing.Point(4, 9);
            this.documentVerifyTab.Name = "documentVerifyTab";
            this.documentVerifyTab.Size = new System.Drawing.Size(649, 533);
            this.documentVerifyTab.TabIndex = 16;
            this.documentVerifyTab.Text = "tabPage11";
            // 
            // extractP7MButton
            // 
            this.extractP7MButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.extractP7MButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.extractP7MButton.FlatAppearance.BorderSize = 0;
            this.extractP7MButton.ForeColor = System.Drawing.Color.White;
            this.extractP7MButton.Location = new System.Drawing.Point(85, 496);
            this.extractP7MButton.Margin = new System.Windows.Forms.Padding(2);
            this.extractP7MButton.Name = "extractP7MButton";
            this.extractP7MButton.Size = new System.Drawing.Size(157, 28);
            this.extractP7MButton.TabIndex = 35;
            this.extractP7MButton.Text = "Estrai";
            this.extractP7MButton.UseVisualStyleBackColor = false;
            this.extractP7MButton.Click += new System.EventHandler(this.ExtractP7MButton_Click);
            // 
            // verifyPanel
            // 
            this.verifyPanel.Controls.Add(this.pnSignerInfo);
            this.verifyPanel.Controls.Add(this.signersCounterLabel);
            this.verifyPanel.Controls.Add(this.pictureBox23);
            this.verifyPanel.Controls.Add(this.fileNamePathVerifyLabel);
            this.verifyPanel.Location = new System.Drawing.Point(50, 104);
            this.verifyPanel.Name = "verifyPanel";
            this.verifyPanel.Size = new System.Drawing.Size(547, 384);
            this.verifyPanel.TabIndex = 34;
            // 
            // pnSignerInfo
            // 
            this.pnSignerInfo.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.pnSignerInfo.Location = new System.Drawing.Point(104, 123);
            this.pnSignerInfo.Name = "pnSignerInfo";
            this.pnSignerInfo.Size = new System.Drawing.Size(341, 258);
            this.pnSignerInfo.TabIndex = 32;
            // 
            // signersCounterLabel
            // 
            this.signersCounterLabel.AutoSize = true;
            this.signersCounterLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.signersCounterLabel.Location = new System.Drawing.Point(335, 89);
            this.signersCounterLabel.Name = "signersCounterLabel";
            this.signersCounterLabel.Size = new System.Drawing.Size(54, 17);
            this.signersCounterLabel.TabIndex = 31;
            this.signersCounterLabel.Text = "label41";
            // 
            // pictureBox23
            // 
            this.pictureBox23.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox23.Image")));
            this.pictureBox23.InitialImage = null;
            this.pictureBox23.Location = new System.Drawing.Point(61, 3);
            this.pictureBox23.Name = "pictureBox23";
            this.pictureBox23.Size = new System.Drawing.Size(75, 89);
            this.pictureBox23.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox23.TabIndex = 29;
            this.pictureBox23.TabStop = false;
            // 
            // fileNamePathVerifyLabel
            // 
            this.fileNamePathVerifyLabel.AutoEllipsis = true;
            this.fileNamePathVerifyLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.fileNamePathVerifyLabel.Location = new System.Drawing.Point(142, 23);
            this.fileNamePathVerifyLabel.Name = "fileNamePathVerifyLabel";
            this.fileNamePathVerifyLabel.Size = new System.Drawing.Size(320, 51);
            this.fileNamePathVerifyLabel.TabIndex = 30;
            this.fileNamePathVerifyLabel.Text = "label32";
            // 
            // verifyLoadingLabel
            // 
            this.verifyLoadingLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.verifyLoadingLabel.Location = new System.Drawing.Point(200, 239);
            this.verifyLoadingLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.verifyLoadingLabel.Name = "verifyLoadingLabel";
            this.verifyLoadingLabel.Size = new System.Drawing.Size(279, 47);
            this.verifyLoadingLabel.TabIndex = 31;
            this.verifyLoadingLabel.Text = "Verifica in corso...";
            // 
            // closeVerifyButton
            // 
            this.closeVerifyButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.closeVerifyButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.closeVerifyButton.FlatAppearance.BorderSize = 0;
            this.closeVerifyButton.ForeColor = System.Drawing.Color.White;
            this.closeVerifyButton.Location = new System.Drawing.Point(388, 493);
            this.closeVerifyButton.Margin = new System.Windows.Forms.Padding(2);
            this.closeVerifyButton.Name = "closeVerifyButton";
            this.closeVerifyButton.Size = new System.Drawing.Size(157, 28);
            this.closeVerifyButton.TabIndex = 28;
            this.closeVerifyButton.Text = "CONCLUDI";
            this.closeVerifyButton.UseVisualStyleBackColor = false;
            this.closeVerifyButton.Click += new System.EventHandler(this.CloseVerifyButton_Click);
            // 
            // labelVerifySignature
            // 
            this.labelVerifySignature.AutoSize = true;
            this.labelVerifySignature.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelVerifySignature.ForeColor = System.Drawing.SystemColors.ControlDark;
            this.labelVerifySignature.Location = new System.Drawing.Point(225, 65);
            this.labelVerifySignature.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelVerifySignature.Name = "labelVerifySignature";
            this.labelVerifySignature.Size = new System.Drawing.Size(205, 20);
            this.labelVerifySignature.TabIndex = 27;
            this.labelVerifySignature.Text = "Verifica firma elettronica";
            // 
            // labelDigitalSignatureHeaderTextVerifyDocument
            // 
            this.labelDigitalSignatureHeaderTextVerifyDocument.AutoSize = true;
            this.labelDigitalSignatureHeaderTextVerifyDocument.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelDigitalSignatureHeaderTextVerifyDocument.Location = new System.Drawing.Point(200, 18);
            this.labelDigitalSignatureHeaderTextVerifyDocument.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelDigitalSignatureHeaderTextVerifyDocument.Name = "labelDigitalSignatureHeaderTextVerifyDocument";
            this.labelDigitalSignatureHeaderTextVerifyDocument.Size = new System.Drawing.Size(255, 36);
            this.labelDigitalSignatureHeaderTextVerifyDocument.TabIndex = 26;
            this.labelDigitalSignatureHeaderTextVerifyDocument.Text = "Firma Elettronica";
            // 
            // tabSettings
            // 
            this.tabSettings.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.tabSettings.Controls.Add(this.tabControlLogConfigurationOptions);
            this.tabSettings.Controls.Add(this.panelSetButtonsInOptions);
            this.tabSettings.Location = new System.Drawing.Point(4, 9);
            this.tabSettings.Name = "tabSettings";
            this.tabSettings.Size = new System.Drawing.Size(649, 533);
            this.tabSettings.TabIndex = 17;
            // 
            // tabControlLogConfigurationOptions
            // 
            this.tabControlLogConfigurationOptions.Controls.Add(this.tabConfigProxy);
            this.tabControlLogConfigurationOptions.Controls.Add(this.tabConfigLogging);
            this.tabControlLogConfigurationOptions.Location = new System.Drawing.Point(16, 8);
            this.tabControlLogConfigurationOptions.Margin = new System.Windows.Forms.Padding(16, 8, 16, 8);
            this.tabControlLogConfigurationOptions.Name = "tabControlLogConfigurationOptions";
            this.tabControlLogConfigurationOptions.SelectedIndex = 0;
            this.tabControlLogConfigurationOptions.Size = new System.Drawing.Size(617, 477);
            this.tabControlLogConfigurationOptions.TabIndex = 39;
            // 
            // tabConfigProxy
            // 
            this.tabConfigProxy.Controls.Add(this.panelProxyConfig);
            this.tabConfigProxy.Controls.Add(this.labelProxyConfigurationHeaderText);
            this.tabConfigProxy.Controls.Add(this.labelProxyConfigurationDescription);
            this.tabConfigProxy.Location = new System.Drawing.Point(4, 22);
            this.tabConfigProxy.Name = "tabConfigProxy";
            this.tabConfigProxy.Padding = new System.Windows.Forms.Padding(3);
            this.tabConfigProxy.Size = new System.Drawing.Size(609, 451);
            this.tabConfigProxy.TabIndex = 0;
            this.tabConfigProxy.Text = "Proxy";
            this.tabConfigProxy.UseVisualStyleBackColor = true;
            // 
            // panelProxyConfig
            // 
            this.panelProxyConfig.Controls.Add(this.labelPortProxyConfiguration);
            this.panelProxyConfig.Controls.Add(this.txtPort);
            this.panelProxyConfig.Controls.Add(this.txtPassword);
            this.panelProxyConfig.Controls.Add(this.showPasswordCheckBox);
            this.panelProxyConfig.Controls.Add(this.labelIPAddressProxyConfiguration);
            this.panelProxyConfig.Controls.Add(this.labelPasswordProxyConfiguration);
            this.panelProxyConfig.Controls.Add(this.txtUrl);
            this.panelProxyConfig.Controls.Add(this.labelUsernameProxyConfiguration);
            this.panelProxyConfig.Controls.Add(this.txtUsername);
            this.panelProxyConfig.Location = new System.Drawing.Point(56, 74);
            this.panelProxyConfig.Name = "panelProxyConfig";
            this.panelProxyConfig.Size = new System.Drawing.Size(501, 362);
            this.panelProxyConfig.TabIndex = 36;
            // 
            // labelPortProxyConfiguration
            // 
            this.labelPortProxyConfiguration.AutoSize = true;
            this.labelPortProxyConfiguration.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelPortProxyConfiguration.Location = new System.Drawing.Point(334, 94);
            this.labelPortProxyConfiguration.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelPortProxyConfiguration.Name = "labelPortProxyConfiguration";
            this.labelPortProxyConfiguration.Size = new System.Drawing.Size(47, 20);
            this.labelPortProxyConfiguration.TabIndex = 37;
            this.labelPortProxyConfiguration.Text = "Porta";
            // 
            // txtPort
            // 
            this.txtPort.Enabled = false;
            this.txtPort.Location = new System.Drawing.Point(338, 116);
            this.txtPort.Margin = new System.Windows.Forms.Padding(2);
            this.txtPort.Name = "txtPort";
            this.txtPort.Size = new System.Drawing.Size(54, 20);
            this.txtPort.TabIndex = 30;
            this.txtPort.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtPort_KeyPress);
            // 
            // txtPassword
            // 
            this.txtPassword.Enabled = false;
            this.txtPassword.Location = new System.Drawing.Point(89, 260);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.Size = new System.Drawing.Size(217, 20);
            this.txtPassword.TabIndex = 35;
            this.txtPassword.UseSystemPasswordChar = true;
            // 
            // showPasswordCheckBox
            // 
            this.showPasswordCheckBox.AutoSize = true;
            this.showPasswordCheckBox.Enabled = false;
            this.showPasswordCheckBox.Location = new System.Drawing.Point(329, 263);
            this.showPasswordCheckBox.Name = "showPasswordCheckBox";
            this.showPasswordCheckBox.Size = new System.Drawing.Size(120, 17);
            this.showPasswordCheckBox.TabIndex = 36;
            this.showPasswordCheckBox.Text = "Visualizza password";
            this.showPasswordCheckBox.UseVisualStyleBackColor = true;
            this.showPasswordCheckBox.CheckedChanged += new System.EventHandler(this.ShowPasswordCheckBox_CheckedChanged);
            // 
            // labelIPAddressProxyConfiguration
            // 
            this.labelIPAddressProxyConfiguration.AutoSize = true;
            this.labelIPAddressProxyConfiguration.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelIPAddressProxyConfiguration.Location = new System.Drawing.Point(87, 91);
            this.labelIPAddressProxyConfiguration.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelIPAddressProxyConfiguration.Name = "labelIPAddressProxyConfiguration";
            this.labelIPAddressProxyConfiguration.Size = new System.Drawing.Size(208, 20);
            this.labelIPAddressProxyConfiguration.TabIndex = 27;
            this.labelIPAddressProxyConfiguration.Text = "Indirizzo (URL o indirizzo IP)";
            // 
            // labelPasswordProxyConfiguration
            // 
            this.labelPasswordProxyConfiguration.AutoSize = true;
            this.labelPasswordProxyConfiguration.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelPasswordProxyConfiguration.Location = new System.Drawing.Point(87, 237);
            this.labelPasswordProxyConfiguration.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelPasswordProxyConfiguration.Name = "labelPasswordProxyConfiguration";
            this.labelPasswordProxyConfiguration.Size = new System.Drawing.Size(78, 20);
            this.labelPasswordProxyConfiguration.TabIndex = 34;
            this.labelPasswordProxyConfiguration.Text = "Password";
            // 
            // txtUrl
            // 
            this.txtUrl.Enabled = false;
            this.txtUrl.Location = new System.Drawing.Point(89, 117);
            this.txtUrl.Margin = new System.Windows.Forms.Padding(2);
            this.txtUrl.Name = "txtUrl";
            this.txtUrl.Size = new System.Drawing.Size(217, 20);
            this.txtUrl.TabIndex = 28;
            // 
            // labelUsernameProxyConfiguration
            // 
            this.labelUsernameProxyConfiguration.AutoSize = true;
            this.labelUsernameProxyConfiguration.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelUsernameProxyConfiguration.Location = new System.Drawing.Point(87, 167);
            this.labelUsernameProxyConfiguration.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelUsernameProxyConfiguration.Name = "labelUsernameProxyConfiguration";
            this.labelUsernameProxyConfiguration.Size = new System.Drawing.Size(83, 20);
            this.labelUsernameProxyConfiguration.TabIndex = 33;
            this.labelUsernameProxyConfiguration.Text = "Username";
            // 
            // txtUsername
            // 
            this.txtUsername.Enabled = false;
            this.txtUsername.Location = new System.Drawing.Point(89, 189);
            this.txtUsername.Margin = new System.Windows.Forms.Padding(2);
            this.txtUsername.Name = "txtUsername";
            this.txtUsername.Size = new System.Drawing.Size(217, 20);
            this.txtUsername.TabIndex = 32;
            // 
            // labelProxyConfigurationHeaderText
            // 
            this.labelProxyConfigurationHeaderText.AutoSize = true;
            this.labelProxyConfigurationHeaderText.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelProxyConfigurationHeaderText.Location = new System.Drawing.Point(89, 3);
            this.labelProxyConfigurationHeaderText.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelProxyConfigurationHeaderText.Name = "labelProxyConfigurationHeaderText";
            this.labelProxyConfigurationHeaderText.Size = new System.Drawing.Size(416, 36);
            this.labelProxyConfigurationHeaderText.TabIndex = 25;
            this.labelProxyConfigurationHeaderText.Text = "Configurazione server proxy";
            // 
            // labelProxyConfigurationDescription
            // 
            this.labelProxyConfigurationDescription.AutoSize = true;
            this.labelProxyConfigurationDescription.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelProxyConfigurationDescription.ForeColor = System.Drawing.SystemColors.ControlDark;
            this.labelProxyConfigurationDescription.Location = new System.Drawing.Point(52, 40);
            this.labelProxyConfigurationDescription.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelProxyConfigurationDescription.Name = "labelProxyConfigurationDescription";
            this.labelProxyConfigurationDescription.Size = new System.Drawing.Size(478, 20);
            this.labelProxyConfigurationDescription.TabIndex = 26;
            this.labelProxyConfigurationDescription.Text = "Inserisci l\'indirizzo del server proxy ed eventuali credenziali";
            // 
            // tabConfigLogging
            // 
            this.tabConfigLogging.Controls.Add(this.labelLogConfigurationHeaderText);
            this.tabConfigLogging.Controls.Add(this.labelLogConfigurationDescription);
            this.tabConfigLogging.Controls.Add(this.panelLogConfiguration);
            this.tabConfigLogging.Location = new System.Drawing.Point(4, 22);
            this.tabConfigLogging.Name = "tabConfigLogging";
            this.tabConfigLogging.Padding = new System.Windows.Forms.Padding(3);
            this.tabConfigLogging.Size = new System.Drawing.Size(609, 451);
            this.tabConfigLogging.TabIndex = 1;
            this.tabConfigLogging.Text = "Log";
            this.tabConfigLogging.UseVisualStyleBackColor = true;
            // 
            // labelLogConfigurationHeaderText
            // 
            this.labelLogConfigurationHeaderText.AutoSize = true;
            this.labelLogConfigurationHeaderText.Font = new System.Drawing.Font("Microsoft Sans Serif", 22F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelLogConfigurationHeaderText.Location = new System.Drawing.Point(114, 3);
            this.labelLogConfigurationHeaderText.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelLogConfigurationHeaderText.Name = "labelLogConfigurationHeaderText";
            this.labelLogConfigurationHeaderText.Size = new System.Drawing.Size(411, 36);
            this.labelLogConfigurationHeaderText.TabIndex = 27;
            this.labelLogConfigurationHeaderText.Text = "Configurazione livello di log";
            // 
            // labelLogConfigurationDescription
            // 
            this.labelLogConfigurationDescription.AutoSize = true;
            this.labelLogConfigurationDescription.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelLogConfigurationDescription.ForeColor = System.Drawing.SystemColors.ControlDark;
            this.labelLogConfigurationDescription.Location = new System.Drawing.Point(46, 39);
            this.labelLogConfigurationDescription.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelLogConfigurationDescription.Name = "labelLogConfigurationDescription";
            this.labelLogConfigurationDescription.Size = new System.Drawing.Size(519, 20);
            this.labelLogConfigurationDescription.TabIndex = 28;
            this.labelLogConfigurationDescription.Text = "Seleziona il livello desiderato per applicazione desktop e libreria";
            // 
            // panelLogConfiguration
            // 
            this.panelLogConfiguration.Controls.Add(this.configLibraryLoggingGroupBox);
            this.panelLogConfiguration.Controls.Add(this.configApplicationLoggingGroupBox);
            this.panelLogConfiguration.Location = new System.Drawing.Point(56, 74);
            this.panelLogConfiguration.Name = "panelLogConfiguration";
            this.panelLogConfiguration.Size = new System.Drawing.Size(501, 362);
            this.panelLogConfiguration.TabIndex = 0;
            // 
            // configLibraryLoggingGroupBox
            // 
            this.configLibraryLoggingGroupBox.Controls.Add(this.rbLoggingLibDebug);
            this.configLibraryLoggingGroupBox.Controls.Add(this.rbLoggingLibInfo);
            this.configLibraryLoggingGroupBox.Controls.Add(this.rbLoggingLibError);
            this.configLibraryLoggingGroupBox.Controls.Add(this.rbLoggingLibNone);
            this.configLibraryLoggingGroupBox.Enabled = false;
            this.configLibraryLoggingGroupBox.Location = new System.Drawing.Point(270, 42);
            this.configLibraryLoggingGroupBox.Name = "configLibraryLoggingGroupBox";
            this.configLibraryLoggingGroupBox.Size = new System.Drawing.Size(192, 256);
            this.configLibraryLoggingGroupBox.TabIndex = 2;
            this.configLibraryLoggingGroupBox.TabStop = false;
            this.configLibraryLoggingGroupBox.Text = "Libreria";
            // 
            // rbLoggingLibDebug
            // 
            this.rbLoggingLibDebug.AutoSize = true;
            this.rbLoggingLibDebug.Location = new System.Drawing.Point(64, 202);
            this.rbLoggingLibDebug.Name = "rbLoggingLibDebug";
            this.rbLoggingLibDebug.Size = new System.Drawing.Size(55, 17);
            this.rbLoggingLibDebug.TabIndex = 3;
            this.rbLoggingLibDebug.TabStop = true;
            this.rbLoggingLibDebug.Text = "debug";
            this.rbLoggingLibDebug.UseVisualStyleBackColor = true;
            // 
            // rbLoggingLibInfo
            // 
            this.rbLoggingLibInfo.AutoSize = true;
            this.rbLoggingLibInfo.Location = new System.Drawing.Point(64, 147);
            this.rbLoggingLibInfo.Name = "rbLoggingLibInfo";
            this.rbLoggingLibInfo.Size = new System.Drawing.Size(42, 17);
            this.rbLoggingLibInfo.TabIndex = 2;
            this.rbLoggingLibInfo.TabStop = true;
            this.rbLoggingLibInfo.Text = "info";
            this.rbLoggingLibInfo.UseVisualStyleBackColor = true;
            // 
            // rbLoggingLibError
            // 
            this.rbLoggingLibError.AutoSize = true;
            this.rbLoggingLibError.Location = new System.Drawing.Point(64, 92);
            this.rbLoggingLibError.Name = "rbLoggingLibError";
            this.rbLoggingLibError.Size = new System.Drawing.Size(46, 17);
            this.rbLoggingLibError.TabIndex = 1;
            this.rbLoggingLibError.TabStop = true;
            this.rbLoggingLibError.Text = "error";
            this.rbLoggingLibError.UseVisualStyleBackColor = true;
            // 
            // rbLoggingLibNone
            // 
            this.rbLoggingLibNone.AutoSize = true;
            this.rbLoggingLibNone.Location = new System.Drawing.Point(64, 37);
            this.rbLoggingLibNone.Name = "rbLoggingLibNone";
            this.rbLoggingLibNone.Size = new System.Drawing.Size(65, 17);
            this.rbLoggingLibNone.TabIndex = 0;
            this.rbLoggingLibNone.TabStop = true;
            this.rbLoggingLibNone.Text = "nessuno";
            this.rbLoggingLibNone.UseVisualStyleBackColor = true;
            // 
            // configApplicationLoggingGroupBox
            // 
            this.configApplicationLoggingGroupBox.Controls.Add(this.rbLoggingAppError);
            this.configApplicationLoggingGroupBox.Controls.Add(this.rbLoggingAppDebug);
            this.configApplicationLoggingGroupBox.Controls.Add(this.rbLoggingAppInfo);
            this.configApplicationLoggingGroupBox.Controls.Add(this.rbLoggingAppNone);
            this.configApplicationLoggingGroupBox.Enabled = false;
            this.configApplicationLoggingGroupBox.Location = new System.Drawing.Point(38, 42);
            this.configApplicationLoggingGroupBox.Name = "configApplicationLoggingGroupBox";
            this.configApplicationLoggingGroupBox.Size = new System.Drawing.Size(192, 256);
            this.configApplicationLoggingGroupBox.TabIndex = 1;
            this.configApplicationLoggingGroupBox.TabStop = false;
            this.configApplicationLoggingGroupBox.Text = "Applicazione desktop";
            // 
            // rbLoggingAppError
            // 
            this.rbLoggingAppError.AutoSize = true;
            this.rbLoggingAppError.Location = new System.Drawing.Point(64, 92);
            this.rbLoggingAppError.Name = "rbLoggingAppError";
            this.rbLoggingAppError.Size = new System.Drawing.Size(46, 17);
            this.rbLoggingAppError.TabIndex = 1;
            this.rbLoggingAppError.TabStop = true;
            this.rbLoggingAppError.Text = "error";
            this.rbLoggingAppError.UseVisualStyleBackColor = true;
            // 
            // rbLoggingAppDebug
            // 
            this.rbLoggingAppDebug.AutoSize = true;
            this.rbLoggingAppDebug.Location = new System.Drawing.Point(64, 202);
            this.rbLoggingAppDebug.Name = "rbLoggingAppDebug";
            this.rbLoggingAppDebug.Size = new System.Drawing.Size(55, 17);
            this.rbLoggingAppDebug.TabIndex = 3;
            this.rbLoggingAppDebug.TabStop = true;
            this.rbLoggingAppDebug.Text = "debug";
            this.rbLoggingAppDebug.UseVisualStyleBackColor = true;
            // 
            // rbLoggingAppInfo
            // 
            this.rbLoggingAppInfo.AutoSize = true;
            this.rbLoggingAppInfo.Location = new System.Drawing.Point(64, 147);
            this.rbLoggingAppInfo.Name = "rbLoggingAppInfo";
            this.rbLoggingAppInfo.Size = new System.Drawing.Size(42, 17);
            this.rbLoggingAppInfo.TabIndex = 1;
            this.rbLoggingAppInfo.TabStop = true;
            this.rbLoggingAppInfo.Text = "info";
            this.rbLoggingAppInfo.UseVisualStyleBackColor = true;
            // 
            // rbLoggingAppNone
            // 
            this.rbLoggingAppNone.AutoSize = true;
            this.rbLoggingAppNone.Location = new System.Drawing.Point(64, 37);
            this.rbLoggingAppNone.Name = "rbLoggingAppNone";
            this.rbLoggingAppNone.Size = new System.Drawing.Size(65, 17);
            this.rbLoggingAppNone.TabIndex = 0;
            this.rbLoggingAppNone.TabStop = true;
            this.rbLoggingAppNone.Text = "nessuno";
            this.rbLoggingAppNone.UseVisualStyleBackColor = true;
            // 
            // panelSetButtonsInOptions
            // 
            this.panelSetButtonsInOptions.Controls.Add(this.editSettingsButton);
            this.panelSetButtonsInOptions.Controls.Add(this.saveSettingsButton);
            this.panelSetButtonsInOptions.Location = new System.Drawing.Point(107, 492);
            this.panelSetButtonsInOptions.Name = "panelSetButtonsInOptions";
            this.panelSetButtonsInOptions.Size = new System.Drawing.Size(440, 28);
            this.panelSetButtonsInOptions.TabIndex = 38;
            // 
            // editSettingsButton
            // 
            this.editSettingsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.editSettingsButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.editSettingsButton.FlatAppearance.BorderSize = 0;
            this.editSettingsButton.ForeColor = System.Drawing.Color.White;
            this.editSettingsButton.Location = new System.Drawing.Point(271, 2);
            this.editSettingsButton.Margin = new System.Windows.Forms.Padding(2);
            this.editSettingsButton.Name = "editSettingsButton";
            this.editSettingsButton.Size = new System.Drawing.Size(157, 28);
            this.editSettingsButton.TabIndex = 38;
            this.editSettingsButton.Text = "Modifica";
            this.editSettingsButton.UseVisualStyleBackColor = false;
            this.editSettingsButton.Click += new System.EventHandler(this.EditSettingsButton_Click);
            // 
            // saveSettingsButton
            // 
            this.saveSettingsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.saveSettingsButton.BackColor = System.Drawing.Color.CornflowerBlue;
            this.saveSettingsButton.Enabled = false;
            this.saveSettingsButton.FlatAppearance.BorderSize = 0;
            this.saveSettingsButton.ForeColor = System.Drawing.Color.White;
            this.saveSettingsButton.Location = new System.Drawing.Point(16, 2);
            this.saveSettingsButton.Margin = new System.Windows.Forms.Padding(2);
            this.saveSettingsButton.Name = "saveSettingsButton";
            this.saveSettingsButton.Size = new System.Drawing.Size(157, 28);
            this.saveSettingsButton.TabIndex = 37;
            this.saveSettingsButton.Text = "Salva";
            this.saveSettingsButton.UseVisualStyleBackColor = false;
            this.saveSettingsButton.Click += new System.EventHandler(this.SaveSettingsButton_Click);
            // 
            // menuHomeButton
            // 
            this.menuHomeButton.AutoSize = true;
            this.menuHomeButton.BackColor = System.Drawing.Color.Silver;
            this.menuHomeButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.menuHomeButton.FlatAppearance.BorderColor = System.Drawing.SystemColors.ButtonFace;
            this.menuHomeButton.FlatAppearance.BorderSize = 0;
            this.menuHomeButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.menuHomeButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.menuHomeButton.Image = ((System.Drawing.Image)(resources.GetObject("menuHomeButton.Image")));
            this.menuHomeButton.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.menuHomeButton.Location = new System.Drawing.Point(6, 114);
            this.menuHomeButton.Margin = new System.Windows.Forms.Padding(2);
            this.menuHomeButton.Name = "menuHomeButton";
            this.menuHomeButton.Size = new System.Drawing.Size(140, 36);
            this.menuHomeButton.TabIndex = 1;
            this.menuHomeButton.Text = "  Home";
            this.menuHomeButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.menuHomeButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.menuHomeButton.UseVisualStyleBackColor = false;
            this.menuHomeButton.Click += new System.EventHandler(this.MenuHomeButton_Click);
            // 
            // pictureBoxLogo
            // 
            this.pictureBoxLogo.Image = ((System.Drawing.Image)(resources.GetObject("pictureBoxLogo.Image")));
            this.pictureBoxLogo.Location = new System.Drawing.Point(29, 10);
            this.pictureBoxLogo.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBoxLogo.Name = "pictureBoxLogo";
            this.pictureBoxLogo.Size = new System.Drawing.Size(82, 89);
            this.pictureBoxLogo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBoxLogo.TabIndex = 2;
            this.pictureBoxLogo.TabStop = false;
            // 
            // menuChangePINButton
            // 
            this.menuChangePINButton.AutoSize = true;
            this.menuChangePINButton.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.menuChangePINButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.menuChangePINButton.FlatAppearance.BorderColor = System.Drawing.SystemColors.ButtonFace;
            this.menuChangePINButton.FlatAppearance.BorderSize = 0;
            this.menuChangePINButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.menuChangePINButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.menuChangePINButton.Image = ((System.Drawing.Image)(resources.GetObject("menuChangePINButton.Image")));
            this.menuChangePINButton.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.menuChangePINButton.Location = new System.Drawing.Point(6, 234);
            this.menuChangePINButton.Margin = new System.Windows.Forms.Padding(2);
            this.menuChangePINButton.Name = "menuChangePINButton";
            this.menuChangePINButton.Size = new System.Drawing.Size(140, 36);
            this.menuChangePINButton.TabIndex = 3;
            this.menuChangePINButton.Text = "  Cambia PIN";
            this.menuChangePINButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.menuChangePINButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.menuChangePINButton.UseVisualStyleBackColor = false;
            this.menuChangePINButton.Click += new System.EventHandler(this.MenuChangePINButton_Click);
            // 
            // menuUnlockPINButton
            // 
            this.menuUnlockPINButton.AutoSize = true;
            this.menuUnlockPINButton.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.menuUnlockPINButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.menuUnlockPINButton.FlatAppearance.BorderColor = System.Drawing.SystemColors.ButtonFace;
            this.menuUnlockPINButton.FlatAppearance.BorderSize = 0;
            this.menuUnlockPINButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.menuUnlockPINButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.menuUnlockPINButton.Image = ((System.Drawing.Image)(resources.GetObject("menuUnlockPINButton.Image")));
            this.menuUnlockPINButton.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.menuUnlockPINButton.Location = new System.Drawing.Point(6, 277);
            this.menuUnlockPINButton.Margin = new System.Windows.Forms.Padding(2);
            this.menuUnlockPINButton.Name = "menuUnlockPINButton";
            this.menuUnlockPINButton.Size = new System.Drawing.Size(140, 36);
            this.menuUnlockPINButton.TabIndex = 4;
            this.menuUnlockPINButton.Text = "  Sblocca Carta";
            this.menuUnlockPINButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.menuUnlockPINButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.menuUnlockPINButton.UseVisualStyleBackColor = false;
            this.menuUnlockPINButton.Click += new System.EventHandler(this.MenuUnlockPINButton_Click);
            // 
            // menuTutorialButton
            // 
            this.menuTutorialButton.AutoSize = true;
            this.menuTutorialButton.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.menuTutorialButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.menuTutorialButton.FlatAppearance.BorderColor = System.Drawing.SystemColors.ButtonFace;
            this.menuTutorialButton.FlatAppearance.BorderSize = 0;
            this.menuTutorialButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.menuTutorialButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.menuTutorialButton.Image = ((System.Drawing.Image)(resources.GetObject("menuTutorialButton.Image")));
            this.menuTutorialButton.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.menuTutorialButton.Location = new System.Drawing.Point(6, 320);
            this.menuTutorialButton.Margin = new System.Windows.Forms.Padding(2);
            this.menuTutorialButton.Name = "menuTutorialButton";
            this.menuTutorialButton.Size = new System.Drawing.Size(140, 36);
            this.menuTutorialButton.TabIndex = 5;
            this.menuTutorialButton.Text = "  Tutorial";
            this.menuTutorialButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.menuTutorialButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.menuTutorialButton.UseVisualStyleBackColor = false;
            this.menuTutorialButton.Click += new System.EventHandler(this.MenuTutorialButton_Click);
            // 
            // menuHelpButton
            // 
            this.menuHelpButton.AutoSize = true;
            this.menuHelpButton.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.menuHelpButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.menuHelpButton.FlatAppearance.BorderColor = System.Drawing.SystemColors.ButtonFace;
            this.menuHelpButton.FlatAppearance.BorderSize = 0;
            this.menuHelpButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.menuHelpButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.menuHelpButton.Image = ((System.Drawing.Image)(resources.GetObject("menuHelpButton.Image")));
            this.menuHelpButton.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.menuHelpButton.Location = new System.Drawing.Point(6, 363);
            this.menuHelpButton.Margin = new System.Windows.Forms.Padding(2);
            this.menuHelpButton.Name = "menuHelpButton";
            this.menuHelpButton.Size = new System.Drawing.Size(140, 36);
            this.menuHelpButton.TabIndex = 6;
            this.menuHelpButton.Text = "  Aiuto";
            this.menuHelpButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.menuHelpButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.menuHelpButton.UseVisualStyleBackColor = false;
            this.menuHelpButton.Click += new System.EventHandler(this.MenuHelpButton_Click);
            // 
            // menuInformationButton
            // 
            this.menuInformationButton.AutoSize = true;
            this.menuInformationButton.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.menuInformationButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.menuInformationButton.FlatAppearance.BorderColor = System.Drawing.SystemColors.ButtonFace;
            this.menuInformationButton.FlatAppearance.BorderSize = 0;
            this.menuInformationButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.menuInformationButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.menuInformationButton.Image = ((System.Drawing.Image)(resources.GetObject("menuInformationButton.Image")));
            this.menuInformationButton.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.menuInformationButton.Location = new System.Drawing.Point(6, 403);
            this.menuInformationButton.Margin = new System.Windows.Forms.Padding(2);
            this.menuInformationButton.Name = "menuInformationButton";
            this.menuInformationButton.Size = new System.Drawing.Size(140, 36);
            this.menuInformationButton.TabIndex = 7;
            this.menuInformationButton.Text = "  Informazioni";
            this.menuInformationButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.menuInformationButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.menuInformationButton.UseVisualStyleBackColor = false;
            this.menuInformationButton.Click += new System.EventHandler(this.MenuInformationButton_Click);
            // 
            // menuDigitalSignatureButton
            // 
            this.menuDigitalSignatureButton.AutoSize = true;
            this.menuDigitalSignatureButton.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.menuDigitalSignatureButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.menuDigitalSignatureButton.FlatAppearance.BorderColor = System.Drawing.SystemColors.ButtonFace;
            this.menuDigitalSignatureButton.FlatAppearance.BorderSize = 0;
            this.menuDigitalSignatureButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.menuDigitalSignatureButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.menuDigitalSignatureButton.Image = ((System.Drawing.Image)(resources.GetObject("menuDigitalSignatureButton.Image")));
            this.menuDigitalSignatureButton.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.menuDigitalSignatureButton.Location = new System.Drawing.Point(6, 156);
            this.menuDigitalSignatureButton.Margin = new System.Windows.Forms.Padding(2);
            this.menuDigitalSignatureButton.Name = "menuDigitalSignatureButton";
            this.menuDigitalSignatureButton.Size = new System.Drawing.Size(140, 38);
            this.menuDigitalSignatureButton.TabIndex = 8;
            this.menuDigitalSignatureButton.Text = "Firma Elettronica";
            this.menuDigitalSignatureButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.menuDigitalSignatureButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.menuDigitalSignatureButton.UseVisualStyleBackColor = false;
            this.menuDigitalSignatureButton.Click += new System.EventHandler(this.MenuDigitalSignatureButton_Click);
            // 
            // menuVerifyButton
            // 
            this.menuVerifyButton.AutoSize = true;
            this.menuVerifyButton.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.menuVerifyButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.menuVerifyButton.FlatAppearance.BorderColor = System.Drawing.SystemColors.ButtonFace;
            this.menuVerifyButton.FlatAppearance.BorderSize = 0;
            this.menuVerifyButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.menuVerifyButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.menuVerifyButton.Image = ((System.Drawing.Image)(resources.GetObject("menuVerifyButton.Image")));
            this.menuVerifyButton.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.menuVerifyButton.Location = new System.Drawing.Point(6, 194);
            this.menuVerifyButton.Margin = new System.Windows.Forms.Padding(2);
            this.menuVerifyButton.Name = "menuVerifyButton";
            this.menuVerifyButton.Size = new System.Drawing.Size(140, 38);
            this.menuVerifyButton.TabIndex = 9;
            this.menuVerifyButton.Text = "Verifica firma";
            this.menuVerifyButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.menuVerifyButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.menuVerifyButton.UseVisualStyleBackColor = false;
            this.menuVerifyButton.Click += new System.EventHandler(this.MenuVerifyButton_Click);
            // 
            // menuSettingsButton
            // 
            this.menuSettingsButton.AutoSize = true;
            this.menuSettingsButton.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.menuSettingsButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.menuSettingsButton.FlatAppearance.BorderColor = System.Drawing.SystemColors.ButtonFace;
            this.menuSettingsButton.FlatAppearance.BorderSize = 0;
            this.menuSettingsButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.menuSettingsButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.menuSettingsButton.Image = ((System.Drawing.Image)(resources.GetObject("menuSettingsButton.Image")));
            this.menuSettingsButton.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.menuSettingsButton.Location = new System.Drawing.Point(6, 443);
            this.menuSettingsButton.Margin = new System.Windows.Forms.Padding(2);
            this.menuSettingsButton.Name = "menuSettingsButton";
            this.menuSettingsButton.Size = new System.Drawing.Size(140, 36);
            this.menuSettingsButton.TabIndex = 10;
            this.menuSettingsButton.Text = "  Impostazioni";
            this.menuSettingsButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.menuSettingsButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.menuSettingsButton.UseVisualStyleBackColor = false;
            this.menuSettingsButton.Click += new System.EventHandler(this.menuSettingsButton_Click);
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(799, 530);
            this.Controls.Add(this.menuSettingsButton);
            this.Controls.Add(this.menuDigitalSignatureButton);
            this.Controls.Add(this.menuVerifyButton);
            this.Controls.Add(this.menuInformationButton);
            this.Controls.Add(this.menuHelpButton);
            this.Controls.Add(this.menuTutorialButton);
            this.Controls.Add(this.menuUnlockPINButton);
            this.Controls.Add(this.menuChangePINButton);
            this.Controls.Add(this.pictureBoxLogo);
            this.Controls.Add(this.menuHomeButton);
            this.Controls.Add(this.mainTabControl);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MaximizeBox = false;
            this.Name = "mainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "CIE ID";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.mainTabControl.ResumeLayout(false);
            this.CIEPairingTabPage.ResumeLayout(false);
            this.CIEPairingTabPage.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.CIEPickerTabPage.ResumeLayout(false);
            this.CIEPickerTabPage.PerformLayout();
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.tableLayoutPanelCarousel.ResumeLayout(false);
            this.carouselItem0.ResumeLayout(false);
            this.carouselItem0.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
            this.carouselItem1.ResumeLayout(false);
            this.carouselItem1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox12)).EndInit();
            this.carouselItem2.ResumeLayout(false);
            this.carouselItem2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox14)).EndInit();
            this.CIEPairingInProgressTabPage.ResumeLayout(false);
            this.CIEPairingInProgressTabPage.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox3)).EndInit();
            this.tabPageChangePIN.ResumeLayout(false);
            this.tabPageChangePIN.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox4)).EndInit();
            this.tabPageChangePINInProgress.ResumeLayout(false);
            this.tabPageChangePINInProgress.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox5)).EndInit();
            this.tabPageUnlockPIN.ResumeLayout(false);
            this.tabPageUnlockPIN.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox6)).EndInit();
            this.tabPageUnlockPINInProgress.ResumeLayout(false);
            this.tabPageUnlockPINInProgress.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox7)).EndInit();
            this.tabPageTutorialWebBrowser.ResumeLayout(false);
            this.tabPageHelpWebBrowser.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox11)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox13)).EndInit();
            this.tabPageInformationWebBrowser.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox10)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox9)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox8)).EndInit();
            this.tabSelectFile.ResumeLayout(false);
            this.tabSelectFile.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox15)).EndInit();
            this.panelChooseDoc.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pbFolder)).EndInit();
            this.tabSelectOp.ResumeLayout(false);
            this.tabSelectOp.PerformLayout();
            this.panelChooseSignOrVerify.ResumeLayout(false);
            this.signOperationOptionPanel.ResumeLayout(false);
            this.signOperationOptionPanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox17)).EndInit();
            this.verifyOptionPanel.ResumeLayout(false);
            this.verifyOptionPanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox18)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox16)).EndInit();
            this.tabSelectSignOp.ResumeLayout(false);
            this.tabSelectSignOp.PerformLayout();
            this.panelChooseSignatureType.ResumeLayout(false);
            this.panelChooseSignatureType.PerformLayout();
            this.panelChoosePades.ResumeLayout(false);
            this.panelChoosePades.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbPades)).EndInit();
            this.panelChooseCades.ResumeLayout(false);
            this.panelChooseCades.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CAdESP7MPictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox21)).EndInit();
            this.graphicDigitalSignatureTab.ResumeLayout(false);
            this.graphicDigitalSignatureTab.PerformLayout();
            this.panelPreviewGraphicSignature.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox22)).EndInit();
            this.signPanelTabPage.ResumeLayout(false);
            this.signPanelTabPage.PerformLayout();
            this.panelTypePINAndSign.ResumeLayout(false);
            this.panelSigningBoxPINArea.ResumeLayout(false);
            this.panelSigningBoxPINArea.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox20)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.digitalSignatureCompletedPictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox19)).EndInit();
            this.graphicSignatureCustomizationTab.ResumeLayout(false);
            this.graphicSignatureCustomizationTab.PerformLayout();
            this.panelDefineYourGraphicSignature.ResumeLayout(false);
            this.documentVerifyTab.ResumeLayout(false);
            this.documentVerifyTab.PerformLayout();
            this.verifyPanel.ResumeLayout(false);
            this.verifyPanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox23)).EndInit();
            this.tabSettings.ResumeLayout(false);
            this.tabControlLogConfigurationOptions.ResumeLayout(false);
            this.tabConfigProxy.ResumeLayout(false);
            this.tabConfigProxy.PerformLayout();
            this.panelProxyConfig.ResumeLayout(false);
            this.panelProxyConfig.PerformLayout();
            this.tabConfigLogging.ResumeLayout(false);
            this.tabConfigLogging.PerformLayout();
            this.panelLogConfiguration.ResumeLayout(false);
            this.configLibraryLoggingGroupBox.ResumeLayout(false);
            this.configLibraryLoggingGroupBox.PerformLayout();
            this.configApplicationLoggingGroupBox.ResumeLayout(false);
            this.configApplicationLoggingGroupBox.PerformLayout();
            this.panelSetButtonsInOptions.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxLogo)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TabControl mainTabControl;
        private System.Windows.Forms.TabPage CIEPairingTabPage;
        private System.Windows.Forms.TabPage CIEPickerTabPage;
        private System.Windows.Forms.Button menuHomeButton;
        private System.Windows.Forms.PictureBox pictureBoxLogo;
        private System.Windows.Forms.Button menuChangePINButton;
        private System.Windows.Forms.Button menuUnlockPINButton;
        private System.Windows.Forms.Button menuTutorialButton;
        private System.Windows.Forms.Button menuHelpButton;
        private System.Windows.Forms.Button menuInformationButton;
        private System.Windows.Forms.Label CIEPairingDescriptionLabel;
        private System.Windows.Forms.Label CIEPairingHeaderTextLabel;
        private System.Windows.Forms.TextBox PINDigit_TextBox8;
        private System.Windows.Forms.TextBox PINDigit_TextBox7;
        private System.Windows.Forms.TextBox PINDigit_TextBox6;
        private System.Windows.Forms.TextBox PINDigit_TextBox5;
        private System.Windows.Forms.TextBox PINDigit_TextBox4;
        private System.Windows.Forms.TextBox PINDigit_TextBox3;
        private System.Windows.Forms.TextBox PINDigit_TextBox2;
        private System.Windows.Forms.TextBox PINDigit_TextBox1;
        private System.Windows.Forms.Label insertPINLabel;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label CIESuccessfullyPairedLabel;
        private System.Windows.Forms.Label appNameCIEPickerHeaderTextLabel;
        private System.Windows.Forms.TabPage CIEPairingInProgressTabPage;
        private System.Windows.Forms.TabPage tabPageChangePIN;
        private System.Windows.Forms.TabPage tabPageChangePINInProgress;
        private System.Windows.Forms.TabPage tabPageUnlockPIN;
        private System.Windows.Forms.TabPage tabPageUnlockPINInProgress;
        private System.Windows.Forms.TabPage tabPageTutorialWebBrowser;
        private System.Windows.Forms.TabPage tabPageHelpWebBrowser;
        private System.Windows.Forms.TabPage tabPageInformationWebBrowser;
        private System.Windows.Forms.Button deleteCIEButton;
        private System.Windows.Forms.ProgressBar progressBar;
        private System.Windows.Forms.Label labelProgressMessage;
        private System.Windows.Forms.PictureBox pictureBox3;
        private System.Windows.Forms.Label labelPairingDescriptionPerformingOperation;
        private System.Windows.Forms.Label labelPairYourCIE;
        private System.Windows.Forms.Button pairButton;
        private System.Windows.Forms.PictureBox pictureBox4;
        private System.Windows.Forms.Label labelChangePINDescription;
        private System.Windows.Forms.Label labelChangePINHeaderText;
        private System.Windows.Forms.TextBox textBoxNewPIN2;
        private System.Windows.Forms.Label labelRepeatNewPIN;
        private System.Windows.Forms.TextBox textBoxNewPIN;
        private System.Windows.Forms.Label labelTypeNewPIN;
        private System.Windows.Forms.TextBox textBoxPIN;
        private System.Windows.Forms.Label labelTypeOldPIN;
        private System.Windows.Forms.Button changePINButton;
        private System.Windows.Forms.Label labelPINPolicyDescription;
        private System.Windows.Forms.ProgressBar progressBarChangePIN;
        private System.Windows.Forms.Label progressLabelChangePIN;
        private System.Windows.Forms.PictureBox pictureBox5;
        private System.Windows.Forms.Label labelInfoBoxAboutCIEPINSafety;
        private System.Windows.Forms.Label labelChangePINPerformingOperationHeaderText;
        private System.Windows.Forms.Label labelPINPolicyInPINUnlock;
        private System.Windows.Forms.Button unlockPINButton;
        private System.Windows.Forms.TextBox textBoxUnlockPIN2;
        private System.Windows.Forms.Label labelRepeatNewPINUnlockPIN;
        private System.Windows.Forms.TextBox textBoxUnlockPIN;
        private System.Windows.Forms.Label labelTypeNewPINUnlockPIN;
        private System.Windows.Forms.TextBox textBoxPUK;
        private System.Windows.Forms.Label labelTypePUK;
        private System.Windows.Forms.PictureBox pictureBox6;
        private System.Windows.Forms.Label labelUnlockPINDescription;
        private System.Windows.Forms.Label labelUnlockPINHeaderText;
        private System.Windows.Forms.ProgressBar progressBarUnlockPIN;
        private System.Windows.Forms.Label progressLabelUnlockPIN;
        private System.Windows.Forms.PictureBox pictureBox7;
        private System.Windows.Forms.Label labelUnlockPINDescriptionPerformingOperation;
        private System.Windows.Forms.Label labelUnlockPINHeaderTextPerformingOperation;
        private System.Windows.Forms.WebBrowser webBrowserTutorial;
        private System.Windows.Forms.WebBrowser webBrowserHelp;
        private System.Windows.Forms.WebBrowser webBrowserInfo;
        private System.Windows.Forms.PictureBox pictureBox10;
        private System.Windows.Forms.PictureBox pictureBox9;
        private System.Windows.Forms.PictureBox pictureBox8;
        private System.Windows.Forms.PictureBox pictureBox11;
        private System.Windows.Forms.PictureBox pictureBox13;
        private System.Windows.Forms.Button addButton;
        private System.Windows.Forms.Button removeAllButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.Button menuDigitalSignatureButton;
        private System.Windows.Forms.Button menuVerifyButton;
        private System.Windows.Forms.TabPage tabSelectFile;
        private System.Windows.Forms.Panel panelChooseDoc;
        private System.Windows.Forms.Button selectDocument;
        private System.Windows.Forms.Label labelChooseBetweenDragOrSelection;
        private System.Windows.Forms.Label labelDragAndDropDocumentInformation;
        private System.Windows.Forms.PictureBox pbFolder;
        private System.Windows.Forms.Label digitalSignatureLabel;
        private System.Windows.Forms.Label customizeGraphicSignatureLabel;
        private System.Windows.Forms.PictureBox pictureBox15;
        private System.Windows.Forms.Label labelGraphicSignatureDescriptionInfoBox;
        private TabPage tabSelectOp;
        private Panel panelChooseSignOrVerify;
        private Panel verifyOptionPanel;
        private Panel signOperationOptionPanel;
        private Label labelFileNamePathInOperationChooser;
        private PictureBox pictureBox16;
        private Label labelDigitalSignatureHeaderTextChoosingOperation;
        private Button cancelOperationButton;
        private Label verifyOptionChooserLabel;
        private Label signOptionChooserLabel;
        private PictureBox pictureBox17;
        private TabPage tabSelectSignOp;
        private Label labelDigitalSignatureDescriptionChoosingFormat;
        private Button cancelSigningOperationButton;
        private Panel panelChooseSignatureType;
        private Label labelFileNamePathInSigningFormatChooser;
        private PictureBox pictureBox21;
        private Label labelDigitalSignatureHeaderTextChoosingFormat;
        private Button proceedWithSignatureButton;
        private Panel panelChoosePades;
        private Label PAdESFormatDescriptionLabel;
        private Label PAdESSignatureLabel;
        private PictureBox pbPades;
        private Label labelSelectSignatureFormatType;
        private Panel panelChooseCades;
        private Label CAdESFormatDescriptionLabel;
        private Label CAdESSignatureLabel;
        private PictureBox CAdESP7MPictureBox;
        private CheckBox enableGraphicSignatureCheckBox;
        private PictureBox pictureBox18;
        private TabPage graphicDigitalSignatureTab;
        private Label labelDigitalSignatureDragSignatureBox;
        private Panel panelPreviewGraphicSignature;
        private Panel panePreview;
        private Label labelFileNamePathInDragSignatureBox;
        private PictureBox pictureBox22;
        private Label labelDigitalSignaturePlaceGraphicSignatureHeaderText;
        private Button proceedSignPreviewButton;
        private Button upButton;
        private Button downButton;
        private TabPage signPanelTabPage;
        private Button closeButton;
        private Button signButton;
        private Button cancelSigningOperationPINPanelButton;
        private Label placeCIEOnReaderLabel;
        private Panel panelTypePINAndSign;
        private TextBox signPINDigit_TextBox4;
        private TextBox signPINDigit_TextBox3;
        private TextBox signPINDigit_TextBox2;
        private TextBox signPINDigit_TextBox1;
        private Label typePINLabel;
        private PictureBox pictureBox20;
        private Label labelFileNamePathSigningOperation;
        private PictureBox pictureBox19;
        private Label labelDigitalSignatureHeaderText;
        private PictureBox digitalSignatureCompletedPictureBox;
        private ProgressBar signProgressBar;
        private Label documentSuccessfullySignedLabel;
        private TabPage graphicSignatureCustomizationTab;
        private Label labelDigitalSignatureHeaderTextCreatingGraphicSignature;
        private TabPage documentVerifyTab;
        private Button closeVerifyButton;
        private Label labelVerifySignature;
        private Label labelDigitalSignatureHeaderTextVerifyDocument;
        private Label verifyLoadingLabel;
        private Panel verifyPanel;
        private FlowLayoutPanel pnSignerInfo;
        private Label signersCounterLabel;
        private PictureBox pictureBox23;
        private Label fileNamePathVerifyLabel;
        private Button selectSigningCIEButton;
        private Label labelGraphicSignatureSecondDescription;
        private Label graphicSignatureCustomizationOverviewLabel;
        private Panel graphicDigitalSignaturePanel;
        private Button selectFileForGraphicSignatureCustomizationButton;
        private Button cancelCustomizationButton;
        private Panel panelDefineYourGraphicSignature;
        private Panel panelSigningBoxPINArea;
        private Button generateCustomGraphicSignatureButton;
        private TabPage tabSettings;
        private Label labelProxyConfigurationDescription;
        private Label labelProxyConfigurationHeaderText;
        private Panel panelProxyConfig;
        private TextBox txtPassword;
        private CheckBox showPasswordCheckBox;
        private Label labelIPAddressProxyConfiguration;
        private Label labelPasswordProxyConfiguration;
        private TextBox txtUrl;
        private Label labelUsernameProxyConfiguration;
        private TextBox txtUsername;
        private Button menuSettingsButton;
        private Panel panelSetButtonsInOptions;
        private Button editSettingsButton;
        private Button saveSettingsButton;
        private TextBox txtPort;
        private Label labelPortProxyConfiguration;
        private Button extractP7MButton;
        private TabControl tabControlLogConfigurationOptions;
        private TabPage tabConfigProxy;
        private TabPage tabConfigLogging;
        private Label labelLogConfigurationHeaderText;
        private Label labelLogConfigurationDescription;
        private Panel panelLogConfiguration;
        private GroupBox configLibraryLoggingGroupBox;
        private RadioButton rbLoggingLibDebug;
        private RadioButton rbLoggingLibInfo;
        private RadioButton rbLoggingLibError;
        private RadioButton rbLoggingLibNone;
        private GroupBox configApplicationLoggingGroupBox;
        private RadioButton rbLoggingAppDebug;
        private RadioButton rbLoggingAppInfo;
        private RadioButton rbLoggingAppError;
        private RadioButton rbLoggingAppNone;
        private TextBox signPINDigit_TextBox5;
        private TextBox signPINDigit_TextBox6;
        private TextBox signPINDigit_TextBox7;
        private TextBox signPINDigit_TextBox8;
        private Button signWithCIEWithoutPairingButton;
        private TableLayoutPanel tableLayoutPanel1;
        private Button rightButton;
        private Button leftButton;
        private FlowLayoutPanel dotsGroup;
        private TableLayoutPanel tableLayoutPanelCarousel;
        private System.Windows.Forms.FlowLayoutPanel carouselItem0;
        private System.Windows.Forms.FlowLayoutPanel carouselItem1;
        private System.Windows.Forms.FlowLayoutPanel carouselItem2;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label labelCardNumberValue1;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.Label labelOwnerValue1;
        private System.Windows.Forms.PictureBox pictureBox2;
        private System.Windows.Forms.Label labelCardNumber0;
        private System.Windows.Forms.Label labelCardNumberValue0;
        private System.Windows.Forms.Label labelOwner0;
        private System.Windows.Forms.Label labelOwnerValue0;
        private System.Windows.Forms.PictureBox pictureBox14;
        private System.Windows.Forms.Label label28;
        private System.Windows.Forms.Label labelCardNumberValue2;
        private System.Windows.Forms.Label label30;
        private System.Windows.Forms.Label labelOwnerValue2;
        private System.Windows.Forms.PictureBox pictureBox12;
    }
}
