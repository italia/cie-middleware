using System;
using System.Collections.Generic;
using System.Linq;

namespace CIEID.Controls
{
    class CircularViewList : List<CieModel>
    {
        public CircularViewList()
        { }

        public CircularViewList(IEnumerable<CieModel> controls)
        {
            AddRange(controls);
        }

        public void ShiftLeft()
        {
            var model = this.FirstOrDefault();

            if (model == null) return;

            this.Remove(model);
            this.Add(model);
        }

        public void ShiftRight()
        {
            var model = this.LastOrDefault();

            if (model == null) return;

            this.Remove(model);
            this.Insert(0, model);
        }
    }
}
