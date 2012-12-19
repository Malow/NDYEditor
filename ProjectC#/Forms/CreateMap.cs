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
    public partial class CreateMap : Form
    {
        public bool shouldCreateMap = false;
        public CreateMap()
        {
            InitializeComponent();
        }
        public bool GetShouldCreateMap()
        {
            return shouldCreateMap;
        }
        public int GetReturnX()
        {
            return Convert.ToInt32(widthSize.Text);
        }
        public int GetReturnY()
        {
            return Convert.ToInt32(heightSize.Text);
        }
        private void CloseButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void CreateButton_Click(object sender, EventArgs e)
        {
            if(heightSize.Text == "" || widthSize.Text == "")
            {
                MessageBox.Show("Width or Height is not higher than 0", "Value Error");
            }
            else if(Convert.ToInt32(heightSize.Text) > 0 && Convert.ToInt32(widthSize.Text) > 0)
            {
                this.shouldCreateMap = true;
                this.Close();
            }
        }

        private void NoNumberKeyPress(object sender, KeyPressEventArgs e)
        {
            if (!char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar))
            {
                e.Handled = true;
            }
        }
    }
}
