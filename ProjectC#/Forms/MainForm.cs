using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Interop;
using System.Runtime.InteropServices;


namespace Example
{
    enum MODE
    {
        NONE,
        SELECT,
        MOVE,
        ROT,
        PLACETREE
    }
    public partial class NDYEditor : Form
    {

        CppCLI m_GameEngine = null;
        bool m_APILoaded = false;
        MODE m_mode =  MODE.NONE;
        bool filePathKnown = false;

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
                this.m_GameEngine.Update();
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
            if (form.GetShouldCreateMap())
            {
                if (form.shouldCreateMap && form.GetReturnX() > 0 && form.GetReturnY() > 0)
                {
                    int xPos = form.GetReturnX();
                    int yPos = form.GetReturnY();

                    this.m_GameEngine.CreateWorld(xPos, yPos);
                }
                else
                {

                }
            }

            // talk to game engine here and make it create "a whole new world" :)

        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string path = "";
            OpenFileDialog fdlg = new OpenFileDialog();
            fdlg.Title = "Open File";
            fdlg.DefaultExt = "*.map";
            fdlg.InitialDirectory = @"c:\";
            fdlg.Filter = "All files (*.*)|*.*|BOOM! Editor files (*.map)|*.map";
            fdlg.FilterIndex = 2;
            fdlg.RestoreDirectory = true;
            fdlg.AddExtension = true;
            if (fdlg.ShowDialog() == DialogResult.OK)
            {
                m_GameEngine.OpenWorld(fdlg.FileName);
                this.filePathKnown = true;
            }
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string path = "";
            SaveFileDialog fdlg = new SaveFileDialog();
            fdlg.DefaultExt = "*.map";
            fdlg.Title = "Save File";
            fdlg.InitialDirectory = @"c:\";
            fdlg.Filter = "All files (*.*)|*.*|BOOM! Editor files (*.map)|*.map";
            fdlg.FilterIndex = 2;
            fdlg.RestoreDirectory = true;
            fdlg.AddExtension = true;
            if (fdlg.ShowDialog() == DialogResult.OK)
            {
                path = fdlg.FileName;
                m_GameEngine.SaveWorldAs(path);
                this.filePathKnown = true;
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (this.filePathKnown)
            {
                this.saveAsToolStripMenuItem_Click(sender, e);
                return;
            }

            string path = "";
            SaveFileDialog fdlg = new SaveFileDialog();
            fdlg.Title = "Save File";
            fdlg.InitialDirectory = @"c:\";
            fdlg.DefaultExt = "*.map";
            fdlg.Filter = "All files (*.*)|*.*|BOOM! Editor files (*.map)|*.map";
            fdlg.FilterIndex = 2;
            fdlg.RestoreDirectory = true;
            fdlg.AddExtension = true;
            if (fdlg.ShowDialog() == DialogResult.OK)
            {
                m_GameEngine.SaveWorld(fdlg.FileName);
                this.filePathKnown = true;
            }
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
            if (this.m_mode == MODE.PLACETREE)
            {
                this.m_mode = MODE.MOVE;
                this.m_GameEngine.ChangeMode((int)this.m_mode);
            }
        }

        private void MoveTool_Click(object sender, EventArgs e)
        {
            if (this.SelectedObject != -1)
            {
                this.Panel_Info.Show();
                this.Panel_ObjectInfo.Show();
                this.m_mode = MODE.MOVE;
                m_GameEngine.ChangeMode((int)this.m_mode);
            }
            else
            {
                this.Panel_Info.Hide();
                this.Panel_ObjectInfo.Hide();
                this.m_mode = MODE.SELECT;
                m_GameEngine.ChangeMode((int)this.m_mode);
            }
        }

        private void SelectTool_Click(object sender, EventArgs e)
        {
            this.Panel_Info.Hide();
            this.Panel_ObjectInfo.Hide();
            this.m_mode = MODE.SELECT;
            m_GameEngine.ChangeMode((int)this.m_mode);
        }

        private void btnRotate_Click(object sender, EventArgs e)
        {
            if (this.SelectedObject != -1)
            {
                this.Panel_Info.Show();
                this.Panel_ObjectInfo.Show();
                this.m_mode = MODE.ROT;
                m_GameEngine.ChangeMode((int)this.m_mode);
            }
            else
            {
                this.Panel_Info.Hide();
                this.Panel_ObjectInfo.Hide();
                this.m_mode = MODE.SELECT;
                m_GameEngine.ChangeMode((int)this.m_mode);
            }
        }

        private void NDYEditor_Activated(object sender, EventArgs e)
        {
            this.m_GameEngine.SetWindowFocused(false);
        }

        private void NDYEditor_Deactivate(object sender, EventArgs e)
        {
            this.m_GameEngine.SetWindowFocused(true);
        }

        private void btnSelect_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)Keys.Escape)
            {
                this.m_mode = MODE.NONE;
                this.m_GameEngine.ChangeMode((int)this.m_mode);
            }
        }

        private void helpToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            HelpMenu test = new HelpMenu();
            test.Show();
        }

        private void Test_Click(object sender, EventArgs e)
        {
            this.m_mode = MODE.PLACETREE;
            this.m_GameEngine.ChangeMode((int)this.m_mode);
            this.m_GameEngine.SetCreateModelPath("Media/Fern_02_v01.obj");
        }
    }
}
