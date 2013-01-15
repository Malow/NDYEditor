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
            this.shapeContainer1 = new Microsoft.VisualBasic.PowerPacks.ShapeContainer();
            this.ColorPrevBox = new Microsoft.VisualBasic.PowerPacks.RectangleShape();
            this.Label_SunLightColor = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.btn_Cancel = new System.Windows.Forms.Button();
            this.btn_ok = new System.Windows.Forms.Button();
            this.X = new System.Windows.Forms.TextBox();
            this.Y = new System.Windows.Forms.TextBox();
            this.Z = new System.Windows.Forms.TextBox();
            this.AmbientLightLabel = new System.Windows.Forms.Label();
            this.zAmbient = new System.Windows.Forms.TextBox();
            this.yAmbient = new System.Windows.Forms.TextBox();
            this.xAmbient = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // shapeContainer1
            // 
            this.shapeContainer1.Location = new System.Drawing.Point(0, 0);
            this.shapeContainer1.Margin = new System.Windows.Forms.Padding(0);
            this.shapeContainer1.Name = "shapeContainer1";
            this.shapeContainer1.Shapes.AddRange(new Microsoft.VisualBasic.PowerPacks.Shape[] {
            this.ColorPrevBox});
            this.shapeContainer1.Size = new System.Drawing.Size(406, 197);
            this.shapeContainer1.TabIndex = 3;
            this.shapeContainer1.TabStop = false;
            // 
            // ColorPrevBox
            // 
            this.ColorPrevBox.AccessibleName = "";
            this.ColorPrevBox.BackStyle = Microsoft.VisualBasic.PowerPacks.BackStyle.Opaque;
            this.ColorPrevBox.Location = new System.Drawing.Point(115, 19);
            this.ColorPrevBox.Name = "ColorPrevBox";
            this.ColorPrevBox.Size = new System.Drawing.Size(20, 20);
            this.ColorPrevBox.DoubleClick += new System.EventHandler(this.DoubleClickChangeColor);
            // 
            // Label_SunLightColor
            // 
            this.Label_SunLightColor.AutoSize = true;
            this.Label_SunLightColor.Location = new System.Drawing.Point(15, 20);
            this.Label_SunLightColor.Name = "Label_SunLightColor";
            this.Label_SunLightColor.Size = new System.Drawing.Size(96, 17);
            this.Label_SunLightColor.TabIndex = 4;
            this.Label_SunLightColor.Text = "Sunlight Color";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(15, 60);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(123, 17);
            this.label1.TabIndex = 5;
            this.label1.Text = "Sunlight Direction:";
            // 
            // btn_Cancel
            // 
            this.btn_Cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_Cancel.Location = new System.Drawing.Point(315, 162);
            this.btn_Cancel.Name = "btn_Cancel";
            this.btn_Cancel.Size = new System.Drawing.Size(75, 23);
            this.btn_Cancel.TabIndex = 7;
            this.btn_Cancel.Text = "Cancel";
            this.btn_Cancel.UseVisualStyleBackColor = true;
            this.btn_Cancel.Click += new System.EventHandler(this.btn_Cancel_Click);
            // 
            // btn_ok
            // 
            this.btn_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ok.Location = new System.Drawing.Point(234, 162);
            this.btn_ok.Name = "btn_ok";
            this.btn_ok.Size = new System.Drawing.Size(75, 23);
            this.btn_ok.TabIndex = 8;
            this.btn_ok.Text = "Ok";
            this.btn_ok.UseVisualStyleBackColor = true;
            this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
            // 
            // X
            // 
            this.X.Location = new System.Drawing.Point(144, 60);
            this.X.Name = "X";
            this.X.Size = new System.Drawing.Size(37, 22);
            this.X.TabIndex = 9;
            this.X.Text = "0";
            // 
            // Y
            // 
            this.Y.Location = new System.Drawing.Point(230, 60);
            this.Y.Name = "Y";
            this.Y.Size = new System.Drawing.Size(37, 22);
            this.Y.TabIndex = 10;
            this.Y.Text = "0";
            // 
            // Z
            // 
            this.Z.Location = new System.Drawing.Point(187, 60);
            this.Z.Name = "Z";
            this.Z.Size = new System.Drawing.Size(37, 22);
            this.Z.TabIndex = 11;
            this.Z.Text = "0";
            // 
            // AmbientLightLabel
            // 
            this.AmbientLightLabel.AutoSize = true;
            this.AmbientLightLabel.Location = new System.Drawing.Point(15, 98);
            this.AmbientLightLabel.Name = "AmbientLightLabel";
            this.AmbientLightLabel.Size = new System.Drawing.Size(98, 17);
            this.AmbientLightLabel.TabIndex = 12;
            this.AmbientLightLabel.Text = "Ambient Light:";
            // 
            // zAmbient
            // 
            this.zAmbient.Location = new System.Drawing.Point(162, 98);
            this.zAmbient.Name = "zAmbient";
            this.zAmbient.Size = new System.Drawing.Size(37, 22);
            this.zAmbient.TabIndex = 15;
            this.zAmbient.Text = "0";
            // 
            // yAmbient
            // 
            this.yAmbient.Location = new System.Drawing.Point(205, 98);
            this.yAmbient.Name = "yAmbient";
            this.yAmbient.Size = new System.Drawing.Size(37, 22);
            this.yAmbient.TabIndex = 14;
            this.yAmbient.Text = "0";
            // 
            // xAmbient
            // 
            this.xAmbient.Location = new System.Drawing.Point(119, 98);
            this.xAmbient.Name = "xAmbient";
            this.xAmbient.Size = new System.Drawing.Size(37, 22);
            this.xAmbient.TabIndex = 13;
            this.xAmbient.Text = "0";
            // 
            // ProjectProperties
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(406, 197);
            this.Controls.Add(this.zAmbient);
            this.Controls.Add(this.yAmbient);
            this.Controls.Add(this.xAmbient);
            this.Controls.Add(this.AmbientLightLabel);
            this.Controls.Add(this.Z);
            this.Controls.Add(this.Y);
            this.Controls.Add(this.X);
            this.Controls.Add(this.btn_ok);
            this.Controls.Add(this.btn_Cancel);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.Label_SunLightColor);
            this.Controls.Add(this.shapeContainer1);
            this.Name = "ProjectProperties";
            this.ShowInTaskbar = false;
            this.Text = "ProjectProperties";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ColorDialog colorDialog1;
        private Microsoft.VisualBasic.PowerPacks.ShapeContainer shapeContainer1;
        private Microsoft.VisualBasic.PowerPacks.RectangleShape ColorPrevBox;
        private System.Windows.Forms.Label Label_SunLightColor;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btn_Cancel;
        private System.Windows.Forms.Button btn_ok;
        private System.Windows.Forms.TextBox X;
        private System.Windows.Forms.TextBox Y;
        private System.Windows.Forms.TextBox Z;
        private System.Windows.Forms.Label AmbientLightLabel;
        private System.Windows.Forms.TextBox zAmbient;
        private System.Windows.Forms.TextBox yAmbient;
        private System.Windows.Forms.TextBox xAmbient;
    }
}