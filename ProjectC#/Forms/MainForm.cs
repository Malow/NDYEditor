﻿using System;
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
using System.Diagnostics;
using System.Text.RegularExpressions;

namespace Editor.Forms
{
    enum MODE
    {
        NONE = 0,
        SELECT = 1,
        MOVE = 2,
        PLACE = 3,
        RAISE = 4,
        LOWER = 5,
        PLACEBRUSH = 6,
        DRAWTEX = 7,
        SMOOTH = 8,
        DELETEENTITIES = 9,
        RESETGROUND = 10,
        AIGRID = 11,
        SHUFFLE = 12,
        WATER = 13
    }

    public partial class NDYEditor : Form
    {
        CppCLI m_GameEngine = null;
        MODE m_mode =  MODE.NONE;
        bool filePathKnown = false;
        bool worldSavedAndNotEdited = true;
        bool autoSwitchMove = false;
        Stopwatch autoSaveWatch;

        public NDYEditor()
        {
            InitializeComponent();
            m_GameEngine = new CppCLI(RenderBox.Handle);
            
            this.KeyPreview = true;
            this.KeyDown += new KeyEventHandler(ListenerKeyDown);
            this.KeyUp += new KeyEventHandler(ListenerKeyUp);

            autoSaveWatch = new Stopwatch();
            autoSaveWatch.Stop();
        }

        void ListenerKeyDown(object sender, KeyEventArgs e)
        {
            e.Handled = true;
            m_GameEngine.KeyDown((int)e.KeyCode);

            if (e.KeyCode == Keys.X)
            {
                this.m_GameEngine.LockMouseToCamera();
            }
            else if (e.KeyCode == Keys.Delete)
            {
                this.m_GameEngine.RemoveSelectedEntities();
            }
        }

        void ListenerKeyUp(object sender, KeyEventArgs e)
        {
            e.Handled = true;
            m_GameEngine.KeyUp((int)e.KeyCode);
            float x = 0, y = 0, z = 0;
            m_GameEngine.GetCameraInfo("Position", out x, out y, out z);
            this.Text = "BOOM! Editor | Camera Position: (" + x + ", " + y + ", " + z + ")";
            
            if ( e.Control && e.KeyCode == Keys.Z ) 
                e.Handled = true;

            if (e.Control && e.KeyCode == Keys.Y)
                e.Handled = true;
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
            // Run the GameEngine for one frame
            m_GameEngine.ProcessFrame();

            // Autosave
            if (filePathKnown && autoSaveWatch.ElapsedMilliseconds > 1000 * 60)
            {
                this.toolStripStatusLabel1.Text = "Saving!";
                Application.DoEvents();
                m_GameEngine.SaveWorld();
                UpdateSaveStatus();
                autoSaveWatch.Restart();
            }

            // Status
            string status="";

            // Number of entities in current sector
            if (m_GameEngine != null)
            {
                int numEntities;
                m_GameEngine.CountEntitiesInSector(out numEntities);

                // Print Number of Entities
                status = "Number of entities: " + numEntities;
            }

            // Save Status
            if (!worldSavedAndNotEdited) status = "Not Saved! " + status;
            if (worldSavedAndNotEdited) status = "Saved! " + status;
            this.toolStripStatusLabel1.Text = status;
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            CreateMap form = new CreateMap();
            if (form.ShowDialog() == DialogResult.OK)
            {
                if (form.GetReturnX() > 0 && form.GetReturnY() > 0)
                {
                    int xPos = form.GetReturnX();
                    int yPos = form.GetReturnY();

                    this.m_GameEngine.CreateWorld(xPos, yPos);
                    filePathKnown = false;
                    UpdateSaveStatus();
                }
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog fdlg = new OpenFileDialog();
            fdlg.Title = "Open File";
            fdlg.DefaultExt = "*.map";
            fdlg.InitialDirectory = Directory.GetCurrentDirectory();
            fdlg.Filter = "All files (*.*)|*.*|BOOM! Editor files (*.map)|*.map";
            fdlg.FilterIndex = 2;
            fdlg.RestoreDirectory = true;
            fdlg.AddExtension = true;
            if (fdlg.ShowDialog() == DialogResult.OK)
            {
                m_GameEngine.OpenWorld(fdlg.FileName);
                UpdateSaveStatus();
                this.filePathKnown = true;
            }
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog fdlg = new SaveFileDialog();
            fdlg.DefaultExt = "*.map";
            fdlg.Title = "Save File";
            fdlg.InitialDirectory = Directory.GetCurrentDirectory();
            fdlg.Filter = "All files (*.*)|*.*|BOOM! Editor files (*.map)|*.map";
            fdlg.FilterIndex = 2;
            fdlg.RestoreDirectory = true;
            fdlg.AddExtension = true;

            if (fdlg.ShowDialog() == DialogResult.OK)
            {
                this.toolStripStatusLabel1.Text = "Saving!";
                Application.DoEvents();
                m_GameEngine.SaveWorldAs(fdlg.FileName);
                this.filePathKnown = true;
                UpdateSaveStatus();
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!this.filePathKnown)
            {
                this.saveAsToolStripMenuItem_Click(sender, e);
            }
			else
            {
                this.toolStripStatusLabel1.Text = "Saving!";
                Application.DoEvents();
                m_GameEngine.SaveWorld();
                UpdateSaveStatus();
            }
        }

        private void RenderBox_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                this.RenderBox_LeftMouseDown(sender, e);
            }
            else if (e.Button == MouseButtons.Right)
            {
                this.RenderBox_RightMouseDown(sender, e);
            }
        }

        private void RenderBox_LeftMouseDown(object sender, MouseEventArgs e)
        {

            if (this.m_mode == MODE.PLACE) // This has to be in front of OnLeftMouseDown
            {
                this.m_GameEngine.SetEntityType(int.Parse(this.Combo_Model.Text.Split(':')[0]));
            }
            else if (this.m_mode == MODE.PLACEBRUSH) // This has to be in front of OnLeftMouseDown
            {
                this.m_GameEngine.SetEntityType(int.Parse(this.ComboBox_Model_Brush.Text.Split(':')[0]));
            }

            m_GameEngine.OnLeftMouseDown((uint)e.X, (uint)e.Y);
            UpdateSaveStatus();

            if (this.m_mode == MODE.SELECT)
            {
                GetAllSelectedInfo();

                // Check if anything has been selected
                int numSelected;
                m_GameEngine.GetNrOfSelectedEntities(out numSelected);

                if (numSelected > 0 && autoSwitchMove)
                {
                    this.m_mode = MODE.MOVE;
                    switchMode();
                    m_GameEngine.ChangeMode((int)m_mode);
                }
            }
            else if (this.m_mode == MODE.MOVE)
            {
                this.m_mode = MODE.SELECT;
                autoSwitchMove = true;
                switchMode();
                m_GameEngine.ChangeMode((int)m_mode);
                GetAllSelectedInfo();
            }
        }

        private void RenderBox_RightMouseDown(object sender, MouseEventArgs e)
        {
            m_GameEngine.OnRightMouseDown((uint)e.X, (uint)e.Y);

            if (this.m_mode == MODE.DRAWTEX)
            {
                string temp;
                m_GameEngine.GetBrushAttr("Tex0", out temp);
                this.ComboBox_Tex1.Enabled = false;
                this.ComboBox_Tex1.Text = temp;
                this.ComboBox_Tex1.Enabled = true;

                m_GameEngine.GetBrushAttr("Tex1", out temp);
                this.ComboBox_Tex2.Enabled = false;
                this.ComboBox_Tex2.Text = temp;
                this.ComboBox_Tex2.Enabled = true;

                m_GameEngine.GetBrushAttr("Tex2", out temp);
                this.ComboBox_Tex3.Enabled = false;
                this.ComboBox_Tex3.Text = temp;
                this.ComboBox_Tex3.Enabled = true;

                m_GameEngine.GetBrushAttr("Tex3", out temp);
                this.ComboBox_Tex4.Enabled = false;
                this.ComboBox_Tex4.Text = temp;
                this.ComboBox_Tex4.Enabled = true;

                m_GameEngine.GetBrushAttr("Tex4", out temp);
                this.ComboBox_Tex5.Enabled = false;
                this.ComboBox_Tex5.Text = temp;
                this.ComboBox_Tex5.Enabled = true;

                m_GameEngine.GetBrushAttr("Tex5", out temp);
                this.ComboBox_Tex6.Enabled = false;
                this.ComboBox_Tex6.Text = temp;
                this.ComboBox_Tex6.Enabled = true;

                m_GameEngine.GetBrushAttr("Tex6", out temp);
                this.ComboBox_Tex7.Enabled = false;
                this.ComboBox_Tex7.Text = temp;
                this.ComboBox_Tex7.Enabled = true;

                m_GameEngine.GetBrushAttr("Tex7", out temp);
                this.ComboBox_Tex8.Enabled = false;
                this.ComboBox_Tex8.Text = temp;
                this.ComboBox_Tex8.Enabled = true;

                this.Panel_Textures.Show();
                this.Panel_Textures.BringToFront();
            }
        }

        void UpdateSaveStatus()
        {
            int temp = 0;
            m_GameEngine.HasWorldBeenSaved(out temp);

            if ( worldSavedAndNotEdited && temp == 0 )
            {
                autoSaveWatch.Restart();
            }

            worldSavedAndNotEdited = temp != 0;
        }

        void GetAllSelectedInfo()
        {
            TextBox_Pos_X.Enabled = false;
            TextBox_Pos_Y.Enabled = false;
            TextBox_Pos_Z.Enabled = false;

            TextBox_Rot_X.Enabled = false;
            TextBox_Rot_Y.Enabled = false;
            TextBox_Rot_Z.Enabled = false;

            TextBox_Scale_X.Enabled = false;
            TextBox_Scale_Y.Enabled = false;
            TextBox_Scale_Z.Enabled = false;

            float x = 0;
            float y = 0;
            float z = 0;

            m_GameEngine.GetSelectedInfo("pos", out x, out y, out z);
            TextBox_Pos_X.Text = x.ToString();
            TextBox_Pos_Y.Text = y.ToString();
            TextBox_Pos_Z.Text = z.ToString();

            m_GameEngine.GetSelectedInfo("rot", out x, out y, out z);
            TextBox_Rot_X.Text = x.ToString();
            TextBox_Rot_Y.Text = y.ToString();
            TextBox_Rot_Z.Text = z.ToString();

            m_GameEngine.GetSelectedInfo("scale", out x, out y, out z);
            TextBox_Scale_X.Text = x.ToString();
            TextBox_Scale_Y.Text = y.ToString();
            TextBox_Scale_Z.Text = z.ToString();

            TextBox_Pos_X.Enabled = true;
            TextBox_Pos_Y.Enabled = true;
            TextBox_Pos_Z.Enabled = true;

            TextBox_Rot_X.Enabled = true;
            TextBox_Rot_Y.Enabled = true;
            TextBox_Rot_Z.Enabled = true;

            TextBox_Scale_X.Enabled = true;
            TextBox_Scale_Y.Enabled = true;
            TextBox_Scale_Z.Enabled = true;
        }

        private void NDYEditor_Activated(object sender, EventArgs e)
        {
            if (this.m_GameEngine != null)
            {
                this.m_GameEngine.SetWindowFocused(false);
            }
        }

        private void NDYEditor_Deactivate(object sender, EventArgs e)
        {
            if (this.m_GameEngine != null)
            {
                this.m_GameEngine.SetWindowFocused(true);
            }
        }

        private void btnSelect_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)Keys.Escape)
            {
                this.m_mode = MODE.NONE;
                this.m_GameEngine.ChangeMode((int)this.m_mode);
            }
        }

        private void NDYEditor_Load(object sender, EventArgs e)
        {
            this.Load_Settings();
            this.Load_Models();
            this.Load_Textures();
            this.Load_Shuffle();
        }

        private void Load_Settings()
        {
            
			string path = "EditorConfig.cfg";
            
            if (File.Exists(path))
            {
				string[] lines = System.IO.File.ReadAllLines("EditorConfig.cfg");
                textBox_InnerCircle_Terrain.Text = lines[0];
                textBox_OuterCircle_Terrain.Text = lines[1];
                TextBox_Terrain_Strength.Text = lines[2];
                TextBox_InnerCircle_Smooth.Text = lines[3];
                TextBox_OuterCircle_Smooth.Text = lines[4];
                TextBox_Strength_Smooth.Text = lines[5];
                textBox1.Text = lines[6];
                TextBox_DeleteCircle_Inner.Text = lines[7];
                TextBox_AIGrid_Inner.Text = lines[8];
                TextBox_BrushPlace_Inner.Text = lines[9];
                TextBox_StrengthCircle.Text = lines[10];
                TextBox_BothCircles_Size.Text = lines[11];
                TextBox_Strength_RaiseLower.Text = lines[12];
            }
            else
            {
                Save_Settings();
            }
        }
        private void Save_Settings()
        {
            System.IO.StreamWriter file = new System.IO.StreamWriter("EditorConfig.cfg");

            file.WriteLine(textBox_InnerCircle_Terrain.Text);
            file.WriteLine(textBox_OuterCircle_Terrain.Text);
            file.WriteLine(TextBox_Terrain_Strength.Text);
            file.WriteLine(TextBox_InnerCircle_Smooth.Text);
            file.WriteLine(TextBox_OuterCircle_Smooth.Text);
            file.WriteLine(TextBox_Strength_Smooth.Text);
            file.WriteLine(textBox1.Text);
            file.WriteLine(TextBox_DeleteCircle_Inner.Text);
            file.WriteLine(TextBox_AIGrid_Inner.Text);
            file.WriteLine(TextBox_BrushPlace_Inner.Text);
            file.WriteLine(TextBox_StrengthCircle.Text);
            file.WriteLine(TextBox_BothCircles_Size.Text);
            file.WriteLine(TextBox_Strength_RaiseLower.Text);

            file.Close();
        }
        private void Load_Shuffle()
        {
            StreamReader sr = new StreamReader("Shuffle.txt");
            String line = "";
            String nr = "";
            while (!sr.EndOfStream)
            {
                line = sr.ReadLine();
                if (line.StartsWith("Number:"))
                {
                    string[] lineSplit = line.Split(':');
                    nr = lineSplit[lineSplit.Length - 1];
                }
                else if (line.StartsWith("Name:"))
                {
                    string[] lineSplit = line.Split(':');
                    string name = lineSplit[lineSplit.Length - 1].ToString();
                    comboBox_shuffle.Items.Add(nr + ": " + name);
                }
            }

            sr.Close();
            this.comboBox_shuffle.SelectedIndex = 0;
        }

        private void Load_Models()
        {
            StreamReader sr = new StreamReader("Entities.txt");
            String line = "";
            String nr = "";
            while (!sr.EndOfStream)
            {
                line = sr.ReadLine();
                if (line.StartsWith("Number:"))
                {
                    string[] lineSplit = line.Split(':');
                    nr = lineSplit[lineSplit.Length - 1];
                }
                else if (line.StartsWith("Name:"))
                {
                    string[] lineSplit = line.Split(':');
                    this.Combo_Model.Items.Add(nr + ": " + lineSplit[lineSplit.Length-1].ToString());
                    this.ComboBox_Model_Brush.Items.Add(nr + ": " + lineSplit[lineSplit.Length - 1].ToString());
                }
            }
            sr.Close();
            this.Combo_Model.SelectedIndex = 0;
            this.ComboBox_Model_Brush.SelectedIndex = 0;
        }

        private void Load_Textures()
        {
            DirectoryInfo di = new DirectoryInfo("Media/Textures/");
            FileInfo[] files = di.GetFiles();

            this.ComboBox_Tex1.Enabled = false;
            this.ComboBox_Tex2.Enabled = false;
            this.ComboBox_Tex3.Enabled = false;
            this.ComboBox_Tex4.Enabled = false;
            this.ComboBox_Tex5.Enabled = false;
            this.ComboBox_Tex6.Enabled = false;
            this.ComboBox_Tex7.Enabled = false;
            this.ComboBox_Tex8.Enabled = false;

            for (int i = 0; i < files.Length; i++)
            {
                this.ComboBox_Tex1.Items.Add(files[i].ToString());
                this.ComboBox_Tex2.Items.Add(files[i].ToString());
                this.ComboBox_Tex3.Items.Add(files[i].ToString());
                this.ComboBox_Tex4.Items.Add(files[i].ToString());
                this.ComboBox_Tex5.Items.Add(files[i].ToString());
                this.ComboBox_Tex6.Items.Add(files[i].ToString());
                this.ComboBox_Tex7.Items.Add(files[i].ToString());
                this.ComboBox_Tex8.Items.Add(files[i].ToString());
            }

            this.ComboBox_Tex1.Enabled = true;
            this.ComboBox_Tex2.Enabled = true;
            this.ComboBox_Tex3.Enabled = true;
            this.ComboBox_Tex4.Enabled = true;
            this.ComboBox_Tex5.Enabled = true;
            this.ComboBox_Tex6.Enabled = true;
            this.ComboBox_Tex7.Enabled = true;
            this.ComboBox_Tex8.Enabled = true;
        }

        private void hideAll()
        {
            this.Panel_Info.Hide();
            this.Panel_ObjectInfo.Hide();
            this.Panel_PlaceObject.Hide();
            this.Panel_Lower_Raise_Ground.Hide();
            this.Panel_Tex_Picker.Hide();
            this.Panel_Textures.Hide();
            this.Panel_PlaceBrush.Hide();
            this.Panel_SmoothTool.Hide();
            this.panel_DeleteCircle.Hide();
            this.Panel_AIGrid.Hide();
            this.Panel_SliderPropBars.Hide();
            this.Panel_Switch.Hide();
            this.Panel_Info.SendToBack();
            this.Panel_ObjectInfo.SendToBack();
            this.Panel_PlaceObject.SendToBack();
            this.Panel_Lower_Raise_Ground.SendToBack();
            this.Panel_Tex_Picker.SendToBack();
            this.Panel_Textures.SendToBack();
            this.Panel_PlaceBrush.SendToBack();
            this.Panel_SmoothTool.SendToBack();
            this.panel_DeleteCircle.SendToBack();
            this.Panel_AIGrid.SendToBack();
            this.Panel_SliderPropBars.SendToBack();
            this.Panel_Switch.SendToBack();
        }

        private void switchMode()
        {
            if (this.m_mode == MODE.NONE)
            {
                bnt_None.Focus();
                this.hideAll();
            }
            else if (this.m_mode == MODE.SELECT)
            {
                btn_Select.Focus();
                this.hideAll();
                this.Panel_Info.Show();
                this.Panel_Info.BringToFront();

                this.Panel_SliderPropBars.Show();
                this.Panel_SliderPropBars.SendToBack();

            }
            else if (m_mode == MODE.MOVE)
            {
                btn_Move.Focus();
                this.hideAll();
                this.Panel_Info.Show();
                this.Panel_Info.BringToFront();

                this.Panel_SliderPropBars.Show();
                this.Panel_SliderPropBars.SendToBack();
            }
            else if (this.m_mode == MODE.PLACE)
            {
                btn_PlaceObject.Focus();
                this.hideAll();
                this.Panel_PlaceObject.Show();
                this.Panel_PlaceObject.BringToFront();
            }
            else if (this.m_mode == MODE.LOWER)
            {
                btn_LowerGround.Focus();
                this.hideAll();
                this.Panel_Lower_Raise_Ground.Show();
                this.Panel_Lower_Raise_Ground.BringToFront();

                if(TextBox_BothCircles_Size.Text != "")
                    m_GameEngine.SetBrushAttr("InnerCircle", float.Parse(this.TextBox_BothCircles_Size.Text));

                m_GameEngine.SetBrushAttr("OuterCircle", 0.0f);

                if(TextBox_Strength_RaiseLower.Text != "")
                    m_GameEngine.SetBrushAttr("Strength", float.Parse(this.TextBox_Strength_RaiseLower.Text));
            }
            else if (this.m_mode == MODE.RAISE)
            {
                btn_RaiseGround.Focus();
                this.hideAll();

                this.Panel_Lower_Raise_Ground.Show();
                this.Panel_Lower_Raise_Ground.BringToFront();

                if(TextBox_BothCircles_Size.Text != "")
                    m_GameEngine.SetBrushAttr("InnerCircle", float.Parse(this.TextBox_BothCircles_Size.Text));

                m_GameEngine.SetBrushAttr("OuterCircle", 0.0f);

                if(TextBox_Strength_RaiseLower.Text != "")
                    m_GameEngine.SetBrushAttr("Strength", float.Parse(this.TextBox_Strength_RaiseLower.Text));
            }
            else if (this.m_mode == MODE.PLACEBRUSH)
            {
                btn_PlaceBrush.Focus();
                this.hideAll();

                this.Panel_PlaceBrush.Show();
                this.Panel_PlaceBrush.BringToFront();

                if(TextBox_BrushPlace_Inner.Text != "")
                    m_GameEngine.SetBrushAttr("InnerCircle", float.Parse(this.TextBox_BrushPlace_Inner.Text));

                m_GameEngine.SetBrushAttr("OuterCircle", 0.0f);

                if(TextBox_StrengthCircle.Text != "")
                    m_GameEngine.SetBrushAttr("Strength", float.Parse(this.TextBox_StrengthCircle.Text));
            }
            else if (this.m_mode == MODE.DRAWTEX)
            {
                btn_DrawTex.Focus();
                this.hideAll();

                this.Panel_Tex_Picker.Show();
                this.Panel_Tex_Picker.BringToFront();
                if(textBox_InnerCircle_Terrain.Text != "")
                    m_GameEngine.SetBrushAttr("InnerCircle", float.Parse(textBox_InnerCircle_Terrain.Text));
                if(textBox_OuterCircle_Terrain.Text != "")
                    m_GameEngine.SetBrushAttr("OuterCircle", float.Parse(textBox_OuterCircle_Terrain.Text));
                if(TextBox_Terrain_Strength.Text != "")
                    m_GameEngine.SetBrushAttr("Strength", float.Parse(TextBox_Terrain_Strength.Text));
            }
            else if (this.m_mode == MODE.SMOOTH)
            {
                this.btn_Smooth.Focus();
                this.hideAll();

                this.Panel_SmoothTool.Show();
                this.Panel_SmoothTool.BringToFront();
                if(TextBox_InnerCircle_Smooth.Text != "")
                    m_GameEngine.SetBrushAttr("InnerCircle", float.Parse(TextBox_InnerCircle_Smooth.Text));
                if(TextBox_OuterCircle_Smooth.Text != "")
                    m_GameEngine.SetBrushAttr("OuterCircle", float.Parse(TextBox_OuterCircle_Smooth.Text));
                if(TextBox_Strength_Smooth.Text != "")
                    m_GameEngine.SetBrushAttr("Strength", float.Parse(TextBox_Strength_Smooth.Text));
            }

            else if (this.m_mode == MODE.DELETEENTITIES)
            {
                this.btn_DeleteEntity.Focus();
                this.hideAll();

                this.panel_DeleteCircle.Show();
                this.panel_DeleteCircle.BringToFront();

                if(TextBox_DeleteCircle_Inner.Text != "")
                    m_GameEngine.SetBrushAttr("InnerCircle", float.Parse(TextBox_DeleteCircle_Inner.Text));

                m_GameEngine.SetBrushAttr("OuterCircle", 0.0f);
            }
            else if (this.m_mode == MODE.RESETGROUND)
            {
                this.btn_ResetGround.Focus();
                this.hideAll();

                this.panel_DeleteCircle.Show();
                this.panel_DeleteCircle.BringToFront();

                if (TextBox_DeleteCircle_Inner.Text != "")
                    m_GameEngine.SetBrushAttr("InnerCircle", float.Parse(TextBox_DeleteCircle_Inner.Text));

                m_GameEngine.SetBrushAttr("OuterCircle", 0.0f);
            }
            else if (this.m_mode == MODE.AIGRID)
            {
                this.btn_AIGrid.Focus();
                this.hideAll();

                this.Panel_AIGrid.Show();
                this.Panel_AIGrid.BringToFront();

                if (TextBox_DeleteCircle_Inner.Text != "")
                    m_GameEngine.SetBrushAttr("InnerCircle", float.Parse(TextBox_AIGrid_Inner.Text));

                m_GameEngine.SetBrushAttr("OuterCircle", 0.0f);
            }
            else if (this.m_mode == MODE.SHUFFLE)
            {
                this.btn_Shuffle.Focus();
                this.hideAll();

                this.Panel_Switch.Show();
                this.Panel_Switch.BringToFront();

                m_GameEngine.SetBrushAttr("InnerCircle", 1.0f);
                m_GameEngine.SetBrushAttr("OuterCircle", 0.0f);
            }
            else if (this.m_mode == MODE.WATER)
            {
                btn_Move.Focus();
                this.hideAll();
            }
        }
        private void setDrawTex(object sender, EventArgs e)
        {
            m_GameEngine.SetBrushAttr("DrawTex", float.Parse((sender as RadioButton).AccessibleName));
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

        private void RenderBox_MouseMove(object sender, MouseEventArgs e)
        {
            m_GameEngine.MouseMove(e.X, e.Y);
        }

		private void RenderBox_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                m_GameEngine.OnLeftMouseUp((uint)e.X, (uint)e.Y);
            }
            else if (e.Button == MouseButtons.Right)
            {
                m_GameEngine.OnRightMouseUp((uint)e.X, (uint)e.Y);
            }
            else if (e.Button == MouseButtons.Middle)
            {
                m_GameEngine.OnMiddleMouseUp((uint)e.X, (uint)e.Y);
            }
        }

        private void NDYEditor_FormClosing(object sender, FormClosingEventArgs e)
        {
            if ( !worldSavedAndNotEdited )
            {
                ExitForm form = new ExitForm();
                form.ShowDialog();

                string temp = form.GetText();
                if (temp == "Close")
                {
                    e.Cancel = true;
                }
                else if (temp == "Save")
                {
                    if (!this.filePathKnown)
                    {
                        this.saveAsToolStripMenuItem_Click(sender, e);
                    }
                    else
                    {
                        this.toolStripStatusLabel1.Text = "Saving!";
                        Application.DoEvents();
                        m_GameEngine.SaveWorld();
                    }
                }
            }
            Save_Settings();
        }

        private void NDYEditor_FormClosed(object sender, FormClosedEventArgs e)
        {
            m_GameEngine = null;
            GC.WaitForFullGCComplete();
        }

        private void NoNumberKeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == ',' && (sender as TextBox).Text == "")
            {
                e.Handled = true;
            }
            else if (!char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar) && e.KeyChar != ',')
            {
                e.Handled = true;
            }
            else if (e.KeyChar == ',' && (sender as TextBox).Text.IndexOf(',') > -1)
            {
                e.Handled = true;
            }
        }

        private void SetSelecetedObjectInfo(object sender, EventArgs e)
        {
            if ((sender as TextBox).Enabled == true)
            {
                string info = (sender as TextBox).AccessibleName;
                if (info == "pos")
                {
                    float x, y, z;
                    bool boolX, boolY, boolZ;
                    boolX = float.TryParse(TextBox_Pos_X.Text, out x);
                    boolY = float.TryParse(TextBox_Pos_Y.Text, out y);
                    boolZ = float.TryParse(TextBox_Pos_Z.Text, out z);

                    if(boolX && boolY && boolZ)
                        m_GameEngine.SetSelectedObjectInfo(info, x, y, z);
                }

                if (info == "rot")
                {
                    float x, y, z;
                    bool boolX, boolY, boolZ;
                    boolX = float.TryParse(TextBox_Rot_X.Text, out x);
                    boolY = float.TryParse(TextBox_Rot_Y.Text, out y);
                    boolZ = float.TryParse(TextBox_Rot_Z.Text, out z);

                    if (boolX && boolY && boolZ)
                        m_GameEngine.SetSelectedObjectInfo(info, x, y, z);
                }

                if (info == "scale")
                {
                    float x, y, z;
                    bool boolX, boolY, boolZ;
                    boolX = float.TryParse(TextBox_Scale_X.Text, out x);
                    boolY = float.TryParse(TextBox_Scale_Y.Text, out y);
                    boolZ = float.TryParse(TextBox_Scale_Z.Text, out z);

                    if (boolX && boolY && boolZ)
                        m_GameEngine.SetSelectedObjectInfo(info, x, y, z);
                }
            }
        }

        private void SetBrushAttr(object sender, EventArgs e)
        {
            if(sender is TextBox)
                if ((sender as TextBox).Text != "")
                    m_GameEngine.SetBrushAttr((sender as TextBox).AccessibleName, float.Parse((sender as TextBox).Text));
                else
                    m_GameEngine.SetBrushAttr((sender as TextBox).AccessibleName, 0.0f);
        }

        private void combobox_Texture_changed(object sender, EventArgs e)
        {
            if ((sender as ComboBox).Enabled == true)
            {
                m_GameEngine.SetBrushAttr("Tex" + (sender as ComboBox).AccessibleName, (sender as ComboBox).Text);
                RenderBox.Focus();
            }
            UpdateSaveStatus();
        }

        private void combobox_textures_ignore(object sender, EventArgs e)
        {
            RenderBox.Focus();
        }

        private void RenderBox_Resize(object sender, EventArgs e)
        {
            if (m_GameEngine != null)
                m_GameEngine.OnResize(RenderBox.Size.Width, RenderBox.Size.Height);
        }

        private void btn_Change_Mode(object sender, EventArgs e)
        {
            this.m_mode = (MODE)int.Parse((sender as Button).AccessibleName);
            if (this.m_mode == MODE.MOVE)
            {
                int numSelected = 0;
                m_GameEngine.GetNrOfSelectedEntities(out numSelected);
                if ( numSelected <= 0 ) this.m_mode = MODE.SELECT;
                autoSwitchMove = true;
            }
            else
            {
                autoSwitchMove = false;
            }
            
            switchMode();
            this.m_GameEngine.ChangeMode((int)this.m_mode);
        }

        private void settingToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ProjectProperties form = new ProjectProperties();

            float x = 0, y = 0, z = 0;

            m_GameEngine.GetSunInfo("Dir", out x, out y, out z);
            form.SetSunDir(x, y, z);

            m_GameEngine.GetSunInfo("Color", out x, out y, out z);
            form.SetColorBox((int)Math.Ceiling(x*255.0f), (int)Math.Ceiling(y*255.0f), (int)Math.Ceiling(z*255.0f));

            m_GameEngine.GetAmbientLight("Color", out x, out y, out z);
            form.SetAmbientLightColor(x*255.0f, y*255.0f, z*255.0f);

            if (form.ShowDialog() == DialogResult.OK)
            {
                ReturnVector3 temp = form.GetSunDir();
                m_GameEngine.SetSunInfo("Dir", temp.mX, temp.mY, temp.mZ);

                temp = form.GetSunColor();
                m_GameEngine.SetSunInfo("Color", temp.mX / 255, temp.mY / 255, temp.mZ / 255);

                temp = form.GetAmbientLight();
                m_GameEngine.SetAmbientLight("Color", temp.mX / 255, temp.mY / 255, temp.mZ / 255);
            }

            UpdateSaveStatus();
        }

        private void RenderBox_Click(object sender, EventArgs e)
        {
            RenderBox.Focus();
        }

        private void btn_MakeAI_Click(object sender, EventArgs e)
        {
            m_GameEngine.CalculateAIGrid();
        }

        private void shadowOnOffToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void worldToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void teleportToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TeleportForm form = new TeleportForm();

            float width, height;
            m_GameEngine.GetWorldSize(out width, out height);
            form.SetCoordinates(width / 2.0f, height / 2.0f);

            if (form.ShowDialog() == DialogResult.OK)
            {
                float x = form.returnX, y = form.returnY;
                m_GameEngine.TeleportTo(x, 2.0f, y);
            }
        }

        private void shadowOnOffToolStripMenuItem_Click_1(object sender, EventArgs e)
        {
            (sender as ToolStripMenuItem).Checked = !(sender as ToolStripMenuItem).Checked;
            m_GameEngine.ToggleShadows((sender as ToolStripMenuItem).Checked);
        }

        private void trackBar_ValueChanged(object sender, EventArgs e)
        {
            
        }

        private void trackBar_ValueChanged(object sender, MouseEventArgs e)
        {
            string info = "rot";
            if (radioButton_pos_slider.Checked == true)
                info = "pos";
            else if (radioButton_scale_slider.Checked == true)
                info = "scale";

            m_GameEngine.IncSelectedObjectInfo(info,
                float.Parse(textBox1.Text) * ((float)trackBar_x.Value / (float)trackBar_x.Maximum),
                float.Parse(textBox1.Text) * ((float)trackBar_y.Value / (float)trackBar_y.Maximum),
                float.Parse(textBox1.Text) * ((float)trackBar_z.Value / (float)trackBar_z.Maximum));

            trackBar_x.Value = 0;
            trackBar_y.Value = 0;
            trackBar_z.Value = 0;

            GetAllSelectedInfo();
        }

        private void Shuffle(object sender, EventArgs e)
        {
            string[] parameters = comboBox_shuffle.SelectedItem.ToString().Split(':');
        }

        private void comboBox_shuffle_SelectionChangeCommitted(object sender, EventArgs e)
        {
            this.m_GameEngine.SetShuffleGroup(uint.Parse(this.comboBox_shuffle.Text.Split(':')[0]));
        }

        private void grassToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            (sender as ToolStripMenuItem).Checked = !(sender as ToolStripMenuItem).Checked;
            m_GameEngine.ToggleGrass((sender as ToolStripMenuItem).Checked);
        }

       /* private void textBox1_TextChanged(object sender, EventArgs e)
        {
            int x = 0;
            if ((sender as TextBox).Text == "" || int.TryParse((sender as TextBox).Text, out x) == false)
                return;
            if (x == 0)
                return;

            trackBar_x.Maximum = int.Parse((sender as TextBox).Text);
            trackBar_y.Maximum = int.Parse((sender as TextBox).Text);
            trackBar_z.Maximum = int.Parse((sender as TextBox).Text);

            trackBar_x.Minimum = 0 - int.Parse((sender as TextBox).Text);
            trackBar_y.Minimum = 0 - int.Parse((sender as TextBox).Text);
            trackBar_z.Minimum = 0 - int.Parse((sender as TextBox).Text);
        }*/
    }
}
