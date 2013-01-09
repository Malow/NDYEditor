using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Example
{
    public partial class ExitForm : Form
    {
        string text = "Close";
        public ExitForm()
        {
            InitializeComponent();
        }
        public string GetText()
        {
            return text;
        }
        private void button1_Click(object sender, EventArgs e)
        {
            text = "Save";
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            text = "DontSave";
            this.Close();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            text = "Close";
            this.Close();
        }
    }
}
