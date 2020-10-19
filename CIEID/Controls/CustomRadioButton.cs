using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing.Drawing2D;

namespace CIEID.Controls
{
    class CustomRadioButton : RadioButton
    {
        private Color onColor = Color.FromArgb(0, 122, 255);
        private Color offColor = Color.FromArgb(0, 122, 255);

        public CustomRadioButton()
        {
            SetStyle(ControlStyles.SupportsTransparentBackColor, true);
            BackColor = Color.Transparent;
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            var graphics = e.Graphics;
            graphics.Clear(Parent.BackColor);
            graphics.SmoothingMode = SmoothingMode.AntiAlias;

            var dotDiameter = ClientRectangle.Width - 2;
            var innerRect = new RectangleF(ClientRectangle.X, ClientRectangle.Y, dotDiameter, dotDiameter);

            var pen = new Pen(offColor, 1);
            graphics.DrawEllipse(pen, innerRect);

            if (this.Checked)
            {
                graphics.FillEllipse(new SolidBrush(onColor), innerRect);
            }
        }

    }
}
