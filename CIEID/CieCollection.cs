using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace CIEID
{
    class CieCollection
    {
        private Dictionary<string, CieModel> mydictionary;


        public CieCollection()
        {
            mydictionary = new Dictionary<string, CieModel>();
        }

        public CieCollection(string collectionString)
        {
            if(collectionString.Equals(""))
            {
                mydictionary = new Dictionary<string, CieModel>();
            }
            else
            {
                mydictionary = JsonConvert.DeserializeObject<Dictionary<string, CieModel>>(collectionString);
            }
        }

        public Dictionary<string, CieModel> MyDictionary
        {
            get { return mydictionary; }
            set { mydictionary = value; }
        }

        public void addCie(String pan, CieModel cieModel)
        {
            if (mydictionary != null)
                mydictionary.Add(pan, cieModel);
        }

        public void removeCie(String pan)
        {
            if (mydictionary != null)
                mydictionary.Remove(pan);
        }

        public void removeAllCie()
        {
            if (mydictionary != null)
                mydictionary.Clear();
        }
    }
}
