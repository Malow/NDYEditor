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
    public struct ReturnVector3
    {
        public float mX;
        public float mY;
        public float mZ;
    }
    
    public partial class ProjectProperties : Form
    {
        public ProjectProperties()
        {
            InitializeComponent();
        }
        public void SetColorBox(int x, int y, int z)
        {
            this.ColorPrevBox.BackColor = Color.FromArgb(x, y, z);
        }
        public void SetSunDir(float x, float y, float z)
        {
            this.X.Text = x.ToString();
            this.Y.Text = y.ToString();
            this.Z.Text = z.ToString();
        }
        public void SetAmbientLightColor(float x, float y, float z)
        {
            this.xAmbient.Text = x.ToString();
            this.yAmbient.Text = y.ToString();
            this.zAmbient.Text = z.ToString();
        }
        public ReturnVector3 GetSunDir()
        {
            ReturnVector3 returnVal;

            returnVal.mX = float.Parse(X.Text);
            returnVal.mY = float.Parse(Y.Text);
            returnVal.mZ = float.Parse(Z.Text);

            return returnVal;
        }
        public ReturnVector3 GetAmbientLight()
        {
            ReturnVector3 returnVal;

            returnVal.mX = float.Parse(xAmbient.Text);
            returnVal.mY = float.Parse(yAmbient.Text);
            returnVal.mZ = float.Parse(zAmbient.Text);

            return returnVal;
        }
        public ReturnVector3 GetSunColor()
        {
            ReturnVector3 returnVal;
            Color tempColor = this.ColorPrevBox.BackColor;
            returnVal.mX = (float)tempColor.R;
            returnVal.mY = (float)tempColor.G;
            returnVal.mZ = (float)tempColor.B;

            return returnVal;
        }
        private void DoubleClickChangeColor(object sender, EventArgs e)
        {
            this.colorDialog1.ShowDialog();
            Color temp = this.colorDialog1.Color;

            this.ColorPrevBox.BackColor = temp;
        }

        private void btn_ok_Click(object sender, EventArgs e)
        {
            if ((X.Text == "" || Y.Text == "" || Z.Text == ""))
            {
                MessageBox.Show("One of the 3 values isnt defined", "Value Error!");
                return;
            }
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void btn_Cancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}
