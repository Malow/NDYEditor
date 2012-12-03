using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Interop;

namespace Example
{
    enum MODE
    {
        NONE,
        SELECT,
        MOVE,
        ROT,
    }
    public partial class NDYEditor : Form
    {
        CppCLI m_GameEngine = null;
        bool m_APILoaded = false;
        MODE mode =  MODE.NONE;

        int SelectedObject = 1; // 1 just for testing. Should otherwise be init to "-1"

        public NDYEditor()
        {
            InitializeComponent();

            m_GameEngine = new CppCLI();
            m_GameEngine.Init(RenderBox.Handle, RenderBox.Width, RenderBox.Height);
            this.ResizeEnd += new EventHandler(form1_ResizeEnd);
            this.Resize += new EventHandler(form1_Resize);


            

        }

        public void GameLoop()
        {
            while (this.Created)
            {
                Run();
                Application.DoEvents();
            }
        }

        //This is our update / Renderloop
        private void Run()
        {
            if (m_APILoaded)
            {
                //Run the GameEngine for one frame
                m_GameEngine.ProcessFrame();
            }
        }

        void form1_Resize(object sender, EventArgs e)
        {
            //Hantera när maximize knappen trycks
            if (this.WindowState == FormWindowState.Maximized)
            {
                m_GameEngine.OnResize(RenderBox.Width, RenderBox.Height);
            }

            //När man återgår till "normal" state igen så hantera de också
            else if (this.WindowState == FormWindowState.Normal)
            {
                m_GameEngine.OnResize(RenderBox.Width, RenderBox.Height);
            }
        }

        void form1_ResizeEnd(object sender, EventArgs e)
        {
            m_GameEngine.OnResize(RenderBox.Width, RenderBox.Height);
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            CreateMap form = new CreateMap();
            form.ShowDialog();
            if (form.shouldCreateMap && form.GetReturnX() > 0 && form.GetReturnY() > 0)
            {
                int xPos = form.GetReturnX();
                int yPos = form.GetReturnY();

                this.m_GameEngine.CreateWorld(xPos, yPos);
            }
            else
            {
                MessageBox.Show("Both values must be above \"0\"", "Value Error");

            }

            // talk to game engine here and make it create "a whole new world" :)

        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string path = "";
            OpenFileDialog fdlg = new OpenFileDialog();
            fdlg.Title = "Open File";
            fdlg.InitialDirectory = @"c:\";
            fdlg.Filter = "All files (*.*)|*.*|All files (*.*)|*.*";
            fdlg.FilterIndex = 2;
            fdlg.RestoreDirectory = true;
            if (fdlg.ShowDialog() == DialogResult.OK)
            {
                path = fdlg.FileName;
            }
            if (path != "")
            {
                path += ".map";

                m_GameEngine.OpenWorld(path);
            }
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string path = "";
            SaveFileDialog fdlg = new SaveFileDialog();
            fdlg.Title = "Save File";
            fdlg.InitialDirectory = @"c:\";
            fdlg.Filter = "All files (*.*)|*.*|All files (*.*)|*.*";
            fdlg.FilterIndex = 2;
            fdlg.RestoreDirectory = true;
            if (fdlg.ShowDialog() == DialogResult.OK)
            {
                path = fdlg.FileName;
            }
            if (path != "")
            {
                path += ".map";

                m_GameEngine.SaveWorld(path);
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.saveAsToolStripMenuItem_Click(sender, e);
        }

        private void form1_Load(object sender, EventArgs e)
        {

        }

        
        private void RenderBox_Paint(object sender, PaintEventArgs e)
        {

        }

        private void RenderBox_MouseDown(object sender, MouseEventArgs e)
        {
            m_GameEngine.OnLeftMouseDown((uint)e.X, (uint)e.Y);
        }

        private void MoveTool_Click(object sender, EventArgs e)
        {
            if (this.SelectedObject != -1)
            {
                this.Panel_Info.Show();
                this.Panel_ObjectInfo.Show();
                this.mode = MODE.MOVE;
                m_GameEngine.ModeSelect((int)this.mode);
            }
            else
            {
                this.Panel_Info.Hide();
                this.Panel_ObjectInfo.Hide();
                this.mode = MODE.SELECT;
                m_GameEngine.ModeSelect((int)this.mode);
            }
        }

        private void SelectTool_Click(object sender, EventArgs e)
        {
            this.Panel_Info.Hide();
            this.Panel_ObjectInfo.Hide();
            this.mode = MODE.SELECT;
            m_GameEngine.ModeSelect((int)this.mode);
        }

        private void btnRotate_Click(object sender, EventArgs e)
        {
            if (this.SelectedObject != -1)
            {
                this.Panel_Info.Show();
                this.Panel_ObjectInfo.Show();
                this.mode = MODE.ROT;
                m_GameEngine.ModeSelect((int)this.mode);
            }
            else
            {
                this.Panel_Info.Hide();
                this.Panel_ObjectInfo.Hide();
                this.mode = MODE.SELECT;
                m_GameEngine.ModeSelect((int)this.mode);
            }
        }
    }
}
