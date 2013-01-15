namespace Example
{
    partial class ProjectProperties
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.ColorPrevBox = new Microsoft.VisualBasic.PowerPacks.RectangleShape();
            this.Label_SunLightColor = new System.Windows.Forms.Label();
            this.btn_Cancel = new System.Windows.Forms.Button();
            this.btn_ok = new System.Windows.Forms.Button();
            this.X = new System.Windows.Forms.TextBox();
            this.Z = new System.Windows.Forms.TextBox();
            this.Y = new System.Windows.Forms.TextBox();
            this.AmbientLightLabel = new System.Windows.Forms.Label();
            this.zAmbient = new System.Windows.Forms.TextBox();
            this.yAmbient = new System.Windows.Forms.TextBox();
            this.xAmbient = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.shapeContainer2 = new Microsoft.VisualBasic.PowerPacks.ShapeContainer();
            this.Light = new System.Windows.Forms.GroupBox();
            this.groupBox1.SuspendLayout();
            this.Light.SuspendLayout();
            this.SuspendLayout();
            // 
            // ColorPrevBox
            // 
            this.ColorPrevBox.AccessibleName = "";
            this.ColorPrevBox.BackStyle = Microsoft.VisualBasic.PowerPacks.BackStyle.Opaque;
            this.ColorPrevBox.Location = new System.Drawing.Point(76, 78);
            this.ColorPrevBox.Name = "ColorPrevBox";
            this.ColorPrevBox.Size = new System.Drawing.Size(20, 20);
            this.ColorPrevBox.DoubleClick += new System.EventHandler(this.DoubleClickChangeColor);
            // 
            // Label_SunLightColor
            // 
            this.Label_SunLightColor.AutoSize = true;
            this.Label_SunLightColor.Location = new System.Drawing.Point(5, 96);
            this.Label_SunLightColor.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.Label_SunLightColor.Name = "Label_SunLightColor";
            this.Label_SunLightColor.Size = new System.Drawing.Size(72, 13);
            this.Label_SunLightColor.TabIndex = 4;
            this.Label_SunLightColor.Text = "Sunlight Color";
            // 
            // btn_Cancel
            // 
            this.btn_Cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_Cancel.Location = new System.Drawing.Point(147, 260);
            this.btn_Cancel.Margin = new System.Windows.Forms.Padding(2);
            this.btn_Cancel.Name = "btn_Cancel";
            this.btn_Cancel.Size = new System.Drawing.Size(56, 19);
            this.btn_Cancel.TabIndex = 7;
            this.btn_Cancel.Text = "Cancel";
            this.btn_Cancel.UseVisualStyleBackColor = true;
            this.btn_Cancel.Click += new System.EventHandler(this.btn_Cancel_Click);
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(87, 260);
            this.btn_ok.Margin = new System.Windows.Forms.Padding(2);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(56, 19);
            this.btn_ok.TabIndex = 8;
            this.btn_ok.Text = "Ok";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // X
            // 
            this.X.Location = new System.Drawing.Point(68, 18);
            this.X.Margin = new System.Windows.Forms.Padding(2);
            this.X.Name = "X";
            this.X.Size = new System.Drawing.Size(29, 20);
            this.X.TabIndex = 9;
            this.X.Text = "0";
            // 
            // Z
            // 
            this.Z.Location = new System.Drawing.Point(68, 66);
            this.Z.Margin = new System.Windows.Forms.Padding(2);
            this.Z.Name = "Z";
            this.Z.Size = new System.Drawing.Size(29, 20);
            this.Z.TabIndex = 10;
            this.Z.Text = "0";
            // 
            // Y
            // 
            this.Y.Location = new System.Drawing.Point(68, 42);
            this.Y.Margin = new System.Windows.Forms.Padding(2);
            this.Y.Name = "Y";
            this.Y.Size = new System.Drawing.Size(29, 20);
            this.Y.TabIndex = 11;
            this.Y.Text = "0";
            // 
            // AmbientLightLabel
            // 
            this.AmbientLightLabel.AutoSize = true;
            this.AmbientLightLabel.Location = new System.Drawing.Point(5, 21);
            this.AmbientLightLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.AmbientLightLabel.Name = "AmbientLightLabel";
            this.AmbientLightLabel.Size = new System.Drawing.Size(74, 13);
            this.AmbientLightLabel.TabIndex = 12;
            this.AmbientLightLabel.Text = "Ambient Light:";
            // 
            // zAmbient
            // 
            this.zAmbient.Location = new System.Drawing.Point(149, 18);
            this.zAmbient.Margin = new System.Windows.Forms.Padding(2);
            this.zAmbient.Name = "zAmbient";
            this.zAmbient.Size = new System.Drawing.Size(29, 20);
            this.zAmbient.TabIndex = 15;
            this.zAmbient.Text = "0";
            // 
            // yAmbient
            // 
            this.yAmbient.Location = new System.Drawing.Point(116, 18);
            this.yAmbient.Margin = new System.Windows.Forms.Padding(2);
            this.yAmbient.Name = "yAmbient";
            this.yAmbient.Size = new System.Drawing.Size(29, 20);
            this.yAmbient.TabIndex = 14;
            this.yAmbient.Text = "0";
            // 
            // xAmbient
            // 
            this.xAmbient.Location = new System.Drawing.Point(83, 18);
            this.xAmbient.Margin = new System.Windows.Forms.Padding(2);
            this.xAmbient.Name = "xAmbient";
            this.xAmbient.Size = new System.Drawing.Size(29, 20);
            this.xAmbient.TabIndex = 13;
            this.xAmbient.Text = "0";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.Z);
            this.groupBox1.Controls.Add(this.X);
            this.groupBox1.Controls.Add(this.Y);
            this.groupBox1.Controls.Add(this.Label_SunLightColor);
            this.groupBox1.Controls.Add(this.shapeContainer2);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(190, 125);
            this.groupBox1.TabIndex = 16;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Sun Settings";
            this.groupBox1.Enter += new System.EventHandler(this.groupBox1_Enter);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(5, 69);
            this.label4.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(59, 13);
            this.label4.TabIndex = 19;
            this.label4.Text = "Direction Z";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(5, 45);
            this.label3.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(59, 13);
            this.label3.TabIndex = 18;
            this.label3.Text = "Direction Y";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(5, 20);
            this.label2.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(59, 13);
            this.label2.TabIndex = 17;
            this.label2.Text = "Direction X";
            // 
            // shapeContainer2
            // 
            this.shapeContainer2.Location = new System.Drawing.Point(3, 16);
            this.shapeContainer2.Margin = new System.Windows.Forms.Padding(0);
            this.shapeContainer2.Name = "shapeContainer2";
            this.shapeContainer2.Shapes.AddRange(new Microsoft.VisualBasic.PowerPacks.Shape[] {
            this.ColorPrevBox});
            this.shapeContainer2.Size = new System.Drawing.Size(184, 106);
            this.shapeContainer2.TabIndex = 20;
            this.shapeContainer2.TabStop = false;
            // 
            // Light
            // 
            this.Light.Controls.Add(this.AmbientLightLabel);
            this.Light.Controls.Add(this.xAmbient);
            this.Light.Controls.Add(this.zAmbient);
            this.Light.Controls.Add(this.yAmbient);
            this.Light.Location = new System.Drawing.Point(12, 143);
            this.Light.Name = "Light";
            this.Light.Size = new System.Drawing.Size(190, 105);
            this.Light.TabIndex = 17;
            this.Light.TabStop = false;
            this.Light.Text = "Light";
            // 
            // ProjectProperties
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(215, 288);
            this.Controls.Add(this.Light);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.btn_ok);
            this.Controls.Add(this.btn_Cancel);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MaximizeBox = false;
            this.Name = "ProjectProperties";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "World Settings";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.Light.ResumeLayout(false);
            this.Light.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ColorDialog colorDialog1;
        private Microsoft.VisualBasic.PowerPacks.RectangleShape ColorPrevBox;
        private System.Windows.Forms.Label Label_SunLightColor;
        private System.Windows.Forms.Button btn_Cancel;
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.TextBox X;
        private System.Windows.Forms.TextBox Z;
        private System.Windows.Forms.TextBox Y;
        private System.Windows.Forms.Label AmbientLightLabel;
        private System.Windows.Forms.TextBox zAmbient;
        private System.Windows.Forms.TextBox yAmbient;
        private System.Windows.Forms.TextBox xAmbient;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private Microsoft.VisualBasic.PowerPacks.ShapeContainer shapeContainer2;
        private System.Windows.Forms.GroupBox Light;
    }
}