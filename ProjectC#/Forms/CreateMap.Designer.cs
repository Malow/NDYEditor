namespace Example
{
    partial class CreateMap
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
            this.CreateButton = new System.Windows.Forms.Button();
            this.CloseButton = new System.Windows.Forms.Button();
            this.WidthLable = new System.Windows.Forms.Label();
            this.widthSize = new System.Windows.Forms.TextBox();
            this.heightSize = new System.Windows.Forms.TextBox();
            this.HeightLable = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // CreateButton
            // 
            this.CreateButton.Location = new System.Drawing.Point(112, 174);
            this.CreateButton.Name = "CreateButton";
            this.CreateButton.Size = new System.Drawing.Size(75, 23);
            this.CreateButton.TabIndex = 0;
            this.CreateButton.Text = "Create";
            this.CreateButton.UseVisualStyleBackColor = true;
            this.CreateButton.Click += new System.EventHandler(this.CreateButton_Click);
            // 
            // CloseButton
            // 
            this.CloseButton.Location = new System.Drawing.Point(193, 174);
            this.CloseButton.Name = "CloseButton";
            this.CloseButton.Size = new System.Drawing.Size(75, 23);
            this.CloseButton.TabIndex = 1;
            this.CloseButton.Text = "Close";
            this.CloseButton.UseVisualStyleBackColor = true;
            this.CloseButton.Click += new System.EventHandler(this.CloseButton_Click);
            // 
            // WidthLable
            // 
            this.WidthLable.AutoSize = true;
            this.WidthLable.Location = new System.Drawing.Point(13, 43);
            this.WidthLable.Name = "WidthLable";
            this.WidthLable.Size = new System.Drawing.Size(44, 17);
            this.WidthLable.TabIndex = 2;
            this.WidthLable.Text = "Width";
            // 
            // widthSize
            // 
            this.widthSize.Location = new System.Drawing.Point(66, 43);
            this.widthSize.Name = "widthSize";
            this.widthSize.Size = new System.Drawing.Size(100, 22);
            this.widthSize.TabIndex = 3;
            this.widthSize.Text = "1";
            // 
            // heightSize
            // 
            this.heightSize.Location = new System.Drawing.Point(66, 71);
            this.heightSize.Name = "heightSize";
            this.heightSize.Size = new System.Drawing.Size(100, 22);
            this.heightSize.TabIndex = 5;
            this.heightSize.Text = "1";
            // 
            // HeightLable
            // 
            this.HeightLable.AutoSize = true;
            this.HeightLable.Location = new System.Drawing.Point(13, 71);
            this.HeightLable.Name = "HeightLable";
            this.HeightLable.Size = new System.Drawing.Size(49, 17);
            this.HeightLable.TabIndex = 4;
            this.HeightLable.Text = "Height";
            // 
            // CreateMap
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(280, 205);
            this.Controls.Add(this.heightSize);
            this.Controls.Add(this.HeightLable);
            this.Controls.Add(this.widthSize);
            this.Controls.Add(this.WidthLable);
            this.Controls.Add(this.CloseButton);
            this.Controls.Add(this.CreateButton);
            this.Name = "CreateMap";
            this.Text = "CreateMap";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button CreateButton;
        private System.Windows.Forms.Button CloseButton;
        private System.Windows.Forms.Label WidthLable;
        private System.Windows.Forms.TextBox widthSize;
        private System.Windows.Forms.TextBox heightSize;
        private System.Windows.Forms.Label HeightLable;
    }
}