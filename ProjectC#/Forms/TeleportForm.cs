using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Editor.Forms
{
    public partial class TeleportForm : Form
    {
        public float returnX, returnY;

        public TeleportForm()
        {
            InitializeComponent();
            this.FormBorderStyle = FormBorderStyle.FixedSingle;
            textBox1.Focus();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            float.TryParse(this.textBox1.Text, out returnX);
            float.TryParse(this.textBox2.Text, out returnY);

            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void FloatingNumbersInput(object sender, KeyPressEventArgs e)
        {
            if (!char.IsDigit(e.KeyChar))
            {
                e.Handled = true;
            }
        }

        public void SetCoordinates(float x, float y)
        {
            this.textBox1.Text = x.ToString();
            this.textBox2.Text = y.ToString();
        }
    }
}
