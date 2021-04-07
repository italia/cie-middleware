using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CIEID
{
    class PdfPreview
    {
        private Panel prPanel;
        private string filePath;
        private string tmpFolderName;
        private string signImagePath;
        private string mutoolPath;
        private int pageNumber;
        private int pdfNumPages;
        private PictureBox pbPdfPreview;
        private MoveablePictureBox signPicture;

        private int getPdfNumPages(string fileName)
        {

            int pdf_page_number = 0;
            var proc = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = mutoolPath,
                    Arguments = string.Format("{0} \"{1}\"", "info", fileName),
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    CreateNoWindow = true
                }
            };

            proc.Start();

            while (!proc.StandardOutput.EndOfStream)
            {
                string line = proc.StandardOutput.ReadLine();
                //Console.WriteLine(line);

                //Pattern espressione regolare
                string pattern = "Pages: (\\d+)";

                //Associo il pattern alla regex
                Regex r = new Regex(pattern, RegexOptions.IgnoreCase);

                //Trovo il matching
                Match m = r.Match(line);
                if (m.Success)
                {
                    //Ottengo il numero delle pagine dal primo gruppo
                    pdf_page_number = int.Parse(m.Groups[1].Value);

                    Console.WriteLine("------------> Numero di pagine del pdf: {0}", pdf_page_number);

                    proc.Close();
                    break;
                }
            }
            return pdf_page_number;

        }

        public int getPdfPages()
        {
            return this.pdfNumPages;
        }

        public string getSignImagePath()
        {
            return signImagePath;
        }

        public int getPdfPageNumber()
        {
            return this.pageNumber;
        }

        public PdfPreview(Panel previewPanel, string pdfPath, string imagePath)
        {
            this.prPanel = previewPanel;
            this.filePath = pdfPath;
            this.signImagePath = imagePath;
            this.tmpFolderName = Path.GetTempPath() + "IPZS";
            this.pageNumber = 1;

            if (!System.IO.Directory.Exists(tmpFolderName))
            {
                System.IO.Directory.CreateDirectory(tmpFolderName);
            }

            this.mutoolPath = Path.Combine(string.Format("{0}IPZS", Path.GetTempPath()), "mutool.exe");
            Console.WriteLine("mutool path {0}", mutoolPath);

            if (!File.Exists(mutoolPath))
                File.WriteAllBytes(mutoolPath, CIEID.Properties.Resources.mutool);

            pdfNumPages = getPdfNumPages(filePath);


            Bitmap signImage = new Bitmap(Image.FromFile(signImagePath), 90, 27);
            signImage.MakeTransparent();

            signPicture = new MoveablePictureBox();
            signPicture.Image = signImage;
            signPicture.BackColor = Color.Transparent;
            signPicture.Width = signImage.Width;
            signPicture.Height = signImage.Height;
            signPicture.BringToFront();
            signPicture.BorderStyle = BorderStyle.FixedSingle;

            pbPdfPreview = new PictureBox();

            pbPdfPreview.Controls.Add(signPicture);
            pbPdfPreview.Update();

            prPanel.Controls.Add(pbPdfPreview);
            freeTempFolder();
            showPreview();
        }

        public void pdfPreviewRemoveObjects()
        {
            this.signPicture.Hide();
            this.pbPdfPreview.Controls.Remove(signPicture);
            this.pbPdfPreview.Hide();
            this.prPanel.Controls.Remove(pbPdfPreview);
        }

        public Dictionary<string,float> getSignImageInfos()
        {
            Dictionary<string, float> signImageInfo = new Dictionary<string, float>();

            //X, Y, Width e Height signImage
            //page number

            float x = (float)signPicture.Left / (float)pbPdfPreview.Width;
            float y = (float)(signPicture.Bottom) / (float)pbPdfPreview.Height;
            float w = ((float)signPicture.Width / (float)pbPdfPreview.Width);
            float h = ((float)signPicture.Height / (float)pbPdfPreview.Height);

            signImageInfo["x"] = x;
            signImageInfo["y"] = y;
            signImageInfo["w"] = w;
            signImageInfo["h"] = h;
            signImageInfo["pageNumber"] = pageNumber - 1;

            return signImageInfo;
        }

        public void pageUp()
        {
            if (pageNumber - 1 >= 1)
            {
                pageNumber -= 1;
            }

            showPreview();
        }

        public void pageDown()
        {
            if (this.pdfNumPages >= pageNumber + 1)
            {
                pageNumber += 1;
            }

            showPreview();
        }

        public void freeTempFolder()
        {
            System.GC.Collect();
            System.GC.WaitForPendingFinalizers();

            string[] Files = Directory.GetFiles(tmpFolderName);

            string prefix = ".png";

            foreach (string file in Files)
            {
                if (file.ToLower().Contains(prefix))
                {
                    File.Delete(file);
                }
            }
        }

        public void showPreview()
        {
            var proc1 = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = mutoolPath,
                    Arguments = string.Format("{0} {1} \"{2}\" {3}", "draw -o", tmpFolderName + "\\page%d.png", filePath, pageNumber),
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    CreateNoWindow = true
                }
            };
            
            string img_file_name = string.Format("{0}\\page{1}.png", tmpFolderName, pageNumber);

            if (!File.Exists(img_file_name))
            {
                proc1.Start();
                proc1.WaitForExit();
            }

            Image img = Image.FromFile(img_file_name);
            if(img.Width > img.Height)
            {
                pbPdfPreview.Width = pbPdfPreview.Parent.Width - 10;
                pbPdfPreview.Height = pbPdfPreview.Parent.Height;
                pbPdfPreview.Left = 5;
                pbPdfPreview.Top = 0;
            }
            else
            {
                pbPdfPreview.Width = (pbPdfPreview.Parent.Width)/2;
                pbPdfPreview.Height = pbPdfPreview.Parent.Height;
                pbPdfPreview.Left = (pbPdfPreview.Parent.Width) / 4;
                pbPdfPreview.Top = 0;
            }

            Bitmap croppedImage = new Bitmap(img, pbPdfPreview.Width, pbPdfPreview.Height);
            pbPdfPreview.Image = croppedImage;
            pbPdfPreview.SendToBack();
        }

    }
}
