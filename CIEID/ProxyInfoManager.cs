using System;
using System.Collections.Generic;
using System.Linq;
using System.Management;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace CIEID
{
    class ProxyInfoManager
    {
        private AesCBCcs aesCipher;
        private byte[] key;
        private byte[] IV;

        public ProxyInfoManager()
        {
            ManagementClass managClass = new ManagementClass("win32_processor");
            ManagementObjectCollection managCollec = managClass.GetInstances();
            string seed = "";

            foreach (ManagementObject managObj in managCollec)
            {
                seed = managObj.Properties["processorID"].Value.ToString();
                break;
            }

            Console.WriteLine("Seed generated -> {0} ", seed);

            SHA256 hash = SHA256.Create();

            this.key = hash.ComputeHash(Encoding.UTF8.GetBytes(seed));
            this.IV = Encoding.ASCII.GetBytes(@"9/\~V).A,lY&=t2b");

            Console.WriteLine("Generated key -> {0}", Convert.ToBase64String(key));
            aesCipher =  new AesCBCcs(key, IV);
        }

        public string getEncryptedCredentials(string credentials)
        {
            return aesCipher.EncryptStringToBytes_Aes(credentials);
        }

        public string getDecryptedCredentials(string encryptedCredentials)
        {
            return aesCipher.DecryptStringFromBytes_Aes(encryptedCredentials);
        }
    }
}
