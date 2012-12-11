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
using System.IO;


namespace Example
{
    enum MODE
    {
        NONE,
        SELECT,
        MOVE,
        ROT,
        PLACE
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

            this.KeyPreview = true;
            this.KeyDown += new KeyEventHandler(ListenerKeyDown);
            this.KeyUp += new KeyEventHandler(ListenerKeyUp);
        }
        void ListenerKeyDown(object sender, KeyEventArgs e)
        {
            e.Handled = true;
            m_GameEngine.KeyDown((int)e.KeyCode);

            if (e.KeyCode == Keys.X)
            {
                this.m_GameEngine.LockMouseToCamera();
            }
        }
        void ListenerKeyUp(object sender, KeyEventArgs e)
        {
            e.Handled = true;
            m_GameEngine.KeyUp((int)e.KeyCode);
        }
        public void GameLoop()
        {
            m_GameEngine.OnResize(RenderBox.Width, RenderBox.Height);
            while (this.Created)
            {
                //Run();
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
                    filePathKnown = false;
                }
                else
                {

                }
            }

            // talk to game engine here and make it create "a whole new world" :)

        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog fdlg = new OpenFileDialog();
            fdlg.Title = "Open File";
            fdlg.DefaultExt = "*.map";
            fdlg.InitialDirectory = System.IO.Path.Combine(System.IO.Path.GetDirectoryName(Application.ExecutablePath), @"YourSubDirectoryName");
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
            if (!this.filePathKnown)
            {
                this.saveAsToolStripMenuItem_Click(sender, e);
                return;
            }
			else
            {
                m_GameEngine.SaveWorld();
            }
        }

        
        private void RenderBox_Paint(object sender, PaintEventArgs e)
        {

        }

        private void RenderBox_MouseDown(object sender, MouseEventArgs e)
        {
            if (this.m_mode == MODE.PLACE)
            {
                this.m_GameEngine.SetCreateModelPath("Media/" + this.Combo_Model.SelectedText);
            }
            m_GameEngine.OnLeftMouseDown((uint)e.X, (uint)e.Y);
        }

        private void MoveTool_Click(object sender, EventArgs e)
        {
            if (this.SelectedObject != -1)
            {
                this.m_mode = MODE.MOVE;
                switchMode();
                m_GameEngine.ChangeMode((int)this.m_mode);
            }
            else
            {
                this.m_mode = MODE.SELECT;
                switchMode();
                m_GameEngine.ChangeMode((int)this.m_mode);
            }
        }

        private void SelectTool_Click(object sender, EventArgs e)
        {
            this.m_mode = MODE.SELECT;
            switchMode();
            m_GameEngine.ChangeMode((int)this.m_mode);
        }

        private void btnRotate_Click(object sender, EventArgs e)
        {
            if (this.SelectedObject != -1)
            {
                this.m_mode = MODE.ROT;
                switchMode();
                m_GameEngine.ChangeMode((int)this.m_mode);
            }
            else
            {
                this.m_mode = MODE.SELECT;
                switchMode();
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

        private void hideAll()
        {
            this.Panel_Info.Hide();
            this.Panel_ObjectInfo.Hide();
            this.Panel_PlaceObject.Hide();
            this.Panel_Info.SendToBack();
            this.Panel_ObjectInfo.SendToBack();
            this.Panel_PlaceObject.SendToBack();
        }

        private void NDYEditor_Load(object sender, EventArgs e)
        {
            this.Load_Models();
        }

        private void Load_Models()
        {
            DirectoryInfo di = new DirectoryInfo("Media/");
            FileInfo[] files = di.GetFiles("*obj");

            for (int i = 0; i < files.Length; i++)
            {
                //if(files[i].ToString() != "scale.obj")
                this.Combo_Model.Items.Add(files[i].ToString());
            }
        }

        private void btnPlaceObject_Click(object sender, EventArgs e)
        {
            this.m_mode = MODE.PLACE;
            switchMode();
            this.m_GameEngine.ChangeMode((int)this.m_mode);
        }

        private void switchMode()
        {
            if (this.m_mode == MODE.NONE || this.m_mode == MODE.SELECT)
            {
                this.hideAll();
            }
            if (this.m_mode == MODE.ROT || m_mode == MODE.MOVE)
            {
                this.hideAll();

                this.Panel_Info.Show();
                this.Panel_ObjectInfo.Show();

                this.Panel_Info.BringToFront();
                this.Panel_ObjectInfo.BringToFront();
            }
            if (this.m_mode == MODE.PLACE)
            {
                this.hideAll();

                this.Panel_PlaceObject.Show();
                this.Panel_PlaceObject.BringToFront();
            }
        }

        private void btnFPS_Click(object sender, EventArgs e)
        {
            m_GameEngine.ChangeCameraMode("FPS");
        }

        private void btnRTS_Click(object sender, EventArgs e)
        {
            m_GameEngine.ChangeCameraMode("RTS");
            this.m_mode = MODE.SELECT;
            switchMode();
            m_GameEngine.ChangeMode((int)this.m_mode);
        }
    }
}
