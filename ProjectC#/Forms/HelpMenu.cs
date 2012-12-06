using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace Example
{
    public partial class HelpMenu : Form
    {
        public HelpMenu()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Random random = new Random();
            HelpMenu test = new HelpMenu();
            test.ShowDialog();
            test.Location = new Point(random.Next(500), random.Next(500));
            this.Close();
        }

        private void HelpMenu_Load(object sender, EventArgs e)
        {
            HelpMenu test = new HelpMenu();
            test.ShowDialog();
        }
    }
}
