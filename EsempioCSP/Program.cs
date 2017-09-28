using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Net;
using System.IO;

namespace EsempioCSP
{
    static class Program
    {

        /* Pagina Register.aspx:
         
         <% @Page Language="C#" %>{
            CF : '<%=Request.ClientCertificate["SUBJECTCN"].Split('/')[0]%>',
            Nome : '<%=Request.ClientCertificate["SUBJECTG"]%>',
            Cognome : '<%=Request.ClientCertificate["SUBJECTSN"]%>'
        }
         * 
        */

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {

            // imposta i parametri del CSP per ottenere il container della chiave sulla CIE
            var cspParameters=new CspParameters(1,"Microsoft Base Smart Card Crypto Provider");
            cspParameters.Flags = CspProviderFlags.UseDefaultKeyContainer;
            
            // il middleware CIE espone un container di tipo Signature
            cspParameters.KeyNumber = (int)KeyNumber.Signature;
            RSACryptoServiceProvider rsa = new RSACryptoServiceProvider(cspParameters);
            // dal nome del container ricavo l'IdServizi. 
            var idSerivizi=rsa.CspKeyContainerInfo.UniqueKeyContainerName.Replace("CIE-", "/");
            rsa.Dispose();

            // apro lo store dei certificati personali
            var store=new X509Store(StoreName.My);
            store.Open(OpenFlags.ReadOnly);
            
            // cerco nello store un certificato che abbia l'IdServizi nel Subject name
            var cc = store.Certificates.Find(X509FindType.FindBySubjectName, idSerivizi,false);

            // evito la validazione del certificato del server. In ambienti di produzione è necessario verificare che il server sia trusted
            ServicePointManager.ServerCertificateValidationCallback += (sender, certificate, chain, sslPolicyErrors) => true;

            // creo una web request  impostando il certificato della CIE come ClienCertificate
            var req = HttpWebRequest.Create("https://localhost/register.aspx") as HttpWebRequest;
            req.ClientCertificates.Add(cc[0]);

            // al momento dell'invio della request verrà richiesto il PIN
            var response = new StreamReader(req.GetResponse().GetResponseStream()).ReadToEnd();
        }
    }
}
