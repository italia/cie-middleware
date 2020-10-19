using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CIEID.Controls
{
    class CarouselControl
    {

        private const int DOT_SIZE = 10;

        public class ButtonsEventArgs : EventArgs
        {
            public bool IsRightButton { get; set; }
            public bool IsEnabled { get; set; }
            public bool IsVisible { get; set; }

            public ButtonsEventArgs(bool isRightButton, bool isEnabled) : this(isRightButton, isEnabled, true)
            { }

            public ButtonsEventArgs(bool isRightButton, bool isEnabled, bool isVisible)
            {
                this.IsRightButton = isRightButton;
                this.IsEnabled = isEnabled;
                this.IsVisible = isVisible;
            }
        }

        private EventHandler<ButtonsEventArgs> onButtonsChanged;

        public event EventHandler<ButtonsEventArgs> ButtonsChanged
        {
            add
            {
                onButtonsChanged += value;
            }
            remove
            {
                onButtonsChanged -= value;
            }
        }

        public CircularViewList Items
        {
            get
            {
                return circularList;
            }
            set
            {
                if (value == null)
                    return;

                circularList = new CircularViewList();

                activeItemIndex = 1;

                if (value.Count == 1)
                {
                    circularList.Add(new CieModel());
                    circularList.Add(value.First());
                    circularList.Add(new CieModel());
                }
                else if (value.Count == 2)
                {
                    circularList.Add(new CieModel());
                    circularList.AddRange(value);

                    activeItemIndex = 0;
                }
                else
                {
                    circularList.AddRange(value);
                }

                UpdateButtons();
            }
        }

        public int CarouselItemsCount
        {
            get
            {
                return Items.Count(x => !x.IsEmpty);
            }
        }

        public CieModel ActiveCieModel
        {
            get
            {
                return Items[1];
            }
        }

        private CircularViewList circularList;
        private FlowLayoutPanel dotsGroup;
        private TableLayoutPanel mainContainer;

        private int activeItemIndex { get; set; }
        private bool dotsCreated = false;

        public CarouselControl(TableLayoutPanel mainContainer, FlowLayoutPanel dotsGroup)
        {
            this.mainContainer = mainContainer;
            this.dotsGroup = dotsGroup;
        }

        public void LoadData(CieCollection cieCollection)
        {
            Items = new CircularViewList(cieCollection.MyDictionary.Values);
            dotsCreated = false;
        }

        private void UpdateButtons()
        {
            if (CarouselItemsCount > 2)
            {
                onButtonsChanged?.Invoke(this, new ButtonsEventArgs(false, true));
                onButtonsChanged?.Invoke(this, new ButtonsEventArgs(true, true));
            }
            else
            if (CarouselItemsCount == 2)
            {
                onButtonsChanged?.Invoke(this, new ButtonsEventArgs(false, circularList.LastOrDefault().IsEmpty));
                onButtonsChanged?.Invoke(this, new ButtonsEventArgs(true, circularList.FirstOrDefault().IsEmpty));
            }
            else
            {
                onButtonsChanged?.Invoke(this, new ButtonsEventArgs(false, false, false));
                onButtonsChanged?.Invoke(this, new ButtonsEventArgs(true, false, false));
            }
        }

        public void ShiftLeft(int steps = 1)
        {
            while (steps-- > 0)
            {
                circularList.ShiftLeft();

                activeItemIndex++;

                if (activeItemIndex >= CarouselItemsCount)
                    activeItemIndex = 0;
            }

            UpdateLayout();
            UpdateButtons();
            UpdateDots();
        }

        public void ShiftRight(int steps = 1)
        {
            while (steps-- > 0)
            {
                circularList.ShiftRight();

                activeItemIndex--;

                if (activeItemIndex < 0)
                    activeItemIndex = CarouselItemsCount - 1;
            }

            UpdateLayout();
            UpdateButtons();
            UpdateDots();
        }

        private void UpdateDots()
        {
            var radioButton = (RadioButton)this.dotsGroup.Controls[activeItemIndex];

            radioButton.CheckedChanged -= dot_CheckedChanged;
            radioButton.Checked = true;
            radioButton.CheckedChanged += dot_CheckedChanged;
        }

        public void CreateDots()
        {
            this.dotsGroup.Controls.Clear();

            if (Items != null && CarouselItemsCount > 1)
            {
                for (int i = 0; i < CarouselItemsCount; i++)
                {
                    var dot = new CustomRadioButton
                    {
                        Name = String.Format("Dot#{0}", i),
                        Size = new System.Drawing.Size(DOT_SIZE, DOT_SIZE),
                        Anchor = (AnchorStyles.None),
                        Checked = activeItemIndex == i
                    };

                    dot.CheckedChanged += dot_CheckedChanged;

                    this.dotsGroup.Controls.Add(dot);
                }
            }
            else if(Items != null && CarouselItemsCount == 1)
            {
                var dot = new CustomRadioButton
                {
                    Name = "Dot1",
                    Size = new System.Drawing.Size(DOT_SIZE, DOT_SIZE),
                    Anchor = (AnchorStyles.None),
                    Checked = true
                };

                this.dotsGroup.Controls.Add(dot);
            }

            dotsCreated = true;
        }

        private void dot_CheckedChanged(object sender, EventArgs e)
        {
            var radioButton = (RadioButton)sender;

            if (radioButton.Checked)
            {
                var index = this.dotsGroup.Controls.GetChildIndex(radioButton);
                var diff = Math.Abs(activeItemIndex - index);

                if (activeItemIndex < index)
                {
                    ShiftLeft(diff);
                }
                else if (activeItemIndex > index)
                {
                    ShiftRight(diff);
                }

                activeItemIndex = index;
            }
        }

        public void UpdateLayout()
        {
            if (circularList != null && circularList.Count > 0)
            {
                var index = 0;

                foreach (var item in circularList.Take(3))
                {
                    UpdateCarouselItem(index, item);

                    index++;
                }
            }

            if (!dotsCreated)
                CreateDots();
        }

        private void UpdateCarouselItem(int index, CieModel model)
        {
            var carouselItem = mainContainer.Controls.Find(String.Format("carouselItem{0}", index), false).FirstOrDefault();

            if (model.IsEmpty)
            {
                carouselItem.Hide();
            } else
            {
                var labelCardNumberValue = carouselItem.Controls.Find(String.Format("labelCardNumberValue{0}", index), false).FirstOrDefault();
                var labelOwnerValue = carouselItem.Controls.Find(String.Format("labelOwnerValue{0}", index), false).FirstOrDefault();

                labelCardNumberValue.Text = model.SerialNumber;
                labelOwnerValue.Text = model.Owner;

                carouselItem.Show();
            }
        }
    }
}