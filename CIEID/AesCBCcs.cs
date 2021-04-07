using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace CIEID
{
    class AesCBCcs
    {
        private byte[] key;
        private byte[] IV;


        public AesCBCcs(byte[] key, byte[] IV)
        {
            this.key = key;
            this.IV = IV;
        }

        public string DecryptStringFromBytes_Aes(string Text)
        {
            if (Text == null || Text.Length <= 0)
                throw new ArgumentNullException("cipherText");
            if (this.key == null || this.key.Length <= 0)
                throw new ArgumentNullException("Key");
            if (this.IV == null || this.IV.Length <= 0)
                throw new ArgumentNullException("IV");

            string plaintext = null;
            byte[] cipherText = Convert.FromBase64String(Text.Replace(' ', '+'));

            using (AesCryptoServiceProvider aesAlg = new AesCryptoServiceProvider())
            {
                aesAlg.Key = this.key;
                aesAlg.IV = IV;
                aesAlg.Mode = CipherMode.CBC;
                aesAlg.Padding = PaddingMode.PKCS7;
                ICryptoTransform decryptor = aesAlg.CreateDecryptor(aesAlg.Key, aesAlg.IV);

                using (MemoryStream msDecrypt = new MemoryStream(cipherText))
                {
                    using (CryptoStream csDecrypt = new CryptoStream(msDecrypt, decryptor, CryptoStreamMode.Read))
                    {
                        using (StreamReader srDecrypt = new StreamReader(csDecrypt))
                        {
                            plaintext = srDecrypt.ReadToEnd();
                        }
                    }
                }
            }

            return plaintext;
        }

        public string EncryptStringToBytes_Aes(string plainText)
        {
            if (plainText == null || plainText.Length <= 0)
                throw new ArgumentNullException("plainText");
            if (this.key == null || this.key.Length <= 0)
                throw new ArgumentNullException("Key");
            if (this.IV == null || this.IV.Length <= 0)
                throw new ArgumentNullException("IV");

            byte[] encrypted;


            using (AesCryptoServiceProvider aesAlg = new AesCryptoServiceProvider())
            {
                aesAlg.Key = this.key;
                aesAlg.IV = this.IV;
                aesAlg.Mode = CipherMode.CBC;
                aesAlg.Padding = PaddingMode.PKCS7;

                ICryptoTransform encryptor = aesAlg.CreateEncryptor(aesAlg.Key, aesAlg.IV);

                using (MemoryStream msEncrypt = new MemoryStream())
                {
                    using (CryptoStream csEncrypt = new CryptoStream(msEncrypt, encryptor, CryptoStreamMode.Write))
                    {
                        using (StreamWriter swEncrypt = new StreamWriter(csEncrypt))
                        {
                            swEncrypt.Write(plainText);
                        }
                        encrypted = msEncrypt.ToArray();
                    }
                }
            }

            return Convert.ToBase64String(encrypted);
        }

    }
}
