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
            this.shouldCreateMap = true;
            this.Close();
        }
    }
}
