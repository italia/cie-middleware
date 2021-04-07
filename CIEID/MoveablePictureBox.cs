using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CIEID
{
    public class MoveablePictureBox : PictureBox
    {
        Point Location;

        public MoveablePictureBox()
        {
            //container.Add(this);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            Location = e.Location;
            base.OnMouseDown(e);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                int tempLeft = this.Left + (e.X - Location.X);
                int tempTop = this.Top + (e.Y - Location.Y);

                if (tempLeft < 0)
                    tempLeft = 0;

                if (tempTop < 0)
                    tempTop = 0;

                if (tempLeft > this.Parent.Width - this.Width)
                    tempLeft = this.Parent.Width - this.Width;

                if (tempTop > this.Parent.Height - this.Height)
                    tempTop = this.Parent.Height - this.Height;

                this.Left = tempLeft;
                this.Top = tempTop;

            }

            base.OnMouseMove(e);
        }
    }
}
