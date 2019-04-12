﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CIEID
{
    public partial class Intro : Form
    {
        public Intro()
        {
            InitializeComponent();
        }


        private void buttonStart_Click(object sender, EventArgs e)
        {
            Hide();


            MainForm mainForm = new MainForm();

            mainForm.Show();

        }

        private void buttonContine_Click(object sender, EventArgs e)
        {
            tabControl.SelectedIndex = 1;
            tabControl.Refresh();
        }
    }
}