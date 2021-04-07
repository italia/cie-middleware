using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CIEID
{
    class CieModel
    {
        public String SerialNumber { get; set; }
        public String Owner { get; set; }
        public String Pan { get; set; }
        public bool isCustomSign { get; set; }

        public bool IsEmpty
        {
            get
            {
                return String.IsNullOrEmpty(SerialNumber) &&
                    String.IsNullOrEmpty(Owner) &&
                    String.IsNullOrEmpty(Pan);
            }
        }

        public CieModel()
        { }

        public CieModel(String serialNumber, String owner)
        {
            this.SerialNumber = serialNumber;
            this.Owner = owner;
            this.isCustomSign = false;
        }

        public CieModel(String serialNumber, String owner, String pan)
        {
            this.SerialNumber = serialNumber;
            this.Owner = owner;
            this.Pan = pan;
            this.isCustomSign = false;
        }
    }
}
