namespace StillsCSharp
{
    partial class Stills
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
            this.groupBoxCapture = new System.Windows.Forms.GroupBox();
            this.checkBoxInputValid = new System.Windows.Forms.CheckBox();
            this.label14 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.comboBoxCapturePixelFormat = new System.Windows.Forms.ComboBox();
            this.numericUpDownCaptureNumberStills = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.numericUpDownCaptureFrameInterval = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.comboBoxCaptureVideoMode = new System.Windows.Forms.ComboBox();
            this.comboBoxCaptureDevice = new System.Windows.Forms.ComboBox();
            this.checkBoxEnableFormatDetection = new System.Windows.Forms.CheckBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.buttonCaptureStartStop = new System.Windows.Forms.Button();
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.groupBoxPlayback = new System.Windows.Forms.GroupBox();
            this.label5 = new System.Windows.Forms.Label();
            this.comboBoxPlaybackVideoMode = new System.Windows.Forms.ComboBox();
            this.label11 = new System.Windows.Forms.Label();
            this.checkBoxLoopPlayback = new System.Windows.Forms.CheckBox();
            this.comboBoxPlaybackPixelFormat = new System.Windows.Forms.ComboBox();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.comboBoxPlaybackDevice = new System.Windows.Forms.ComboBox();
            this.numericUpDownPlaybackFrameInterval = new System.Windows.Forms.NumericUpDown();
            this.buttonPlaybackStartStop = new System.Windows.Forms.Button();
            this.label8 = new System.Windows.Forms.Label();
            this.buttonBrowseDirectory = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.comboBoxFileType = new System.Windows.Forms.ComboBox();
            this.label13 = new System.Windows.Forms.Label();
            this.textBoxDirectoryPath = new System.Windows.Forms.TextBox();
            this.panelStillsViewer = new System.Windows.Forms.Panel();
            this.groupBoxCapture.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownCaptureNumberStills)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownCaptureFrameInterval)).BeginInit();
            this.groupBoxPlayback.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPlaybackFrameInterval)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBoxCapture
            // 
            this.groupBoxCapture.Controls.Add(this.checkBoxInputValid);
            this.groupBoxCapture.Controls.Add(this.label14);
            this.groupBoxCapture.Controls.Add(this.label12);
            this.groupBoxCapture.Controls.Add(this.label7);
            this.groupBoxCapture.Controls.Add(this.comboBoxCapturePixelFormat);
            this.groupBoxCapture.Controls.Add(this.numericUpDownCaptureNumberStills);
            this.groupBoxCapture.Controls.Add(this.label6);
            this.groupBoxCapture.Controls.Add(this.numericUpDownCaptureFrameInterval);
            this.groupBoxCapture.Controls.Add(this.label4);
            this.groupBoxCapture.Controls.Add(this.comboBoxCaptureVideoMode);
            this.groupBoxCapture.Controls.Add(this.comboBoxCaptureDevice);
            this.groupBoxCapture.Controls.Add(this.checkBoxEnableFormatDetection);
            this.groupBoxCapture.Controls.Add(this.label3);
            this.groupBoxCapture.Controls.Add(this.label2);
            this.groupBoxCapture.Controls.Add(this.buttonCaptureStartStop);
            this.groupBoxCapture.Location = new System.Drawing.Point(12, 12);
            this.groupBoxCapture.Name = "groupBoxCapture";
            this.groupBoxCapture.Size = new System.Drawing.Size(317, 241);
            this.groupBoxCapture.TabIndex = 0;
            this.groupBoxCapture.TabStop = false;
            this.groupBoxCapture.Text = "Capture";
            // 
            // checkBoxInputValid
            // 
            this.checkBoxInputValid.AutoSize = true;
            this.checkBoxInputValid.Enabled = false;
            this.checkBoxInputValid.Location = new System.Drawing.Point(126, 133);
            this.checkBoxInputValid.Name = "checkBoxInputValid";
            this.checkBoxInputValid.Size = new System.Drawing.Size(15, 14);
            this.checkBoxInputValid.TabIndex = 20;
            this.checkBoxInputValid.UseVisualStyleBackColor = true;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(6, 133);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(87, 13);
            this.label14.TabIndex = 19;
            this.label14.Text = "Video Input Valid";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(6, 79);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(67, 13);
            this.label12.TabIndex = 18;
            this.label12.Text = "Pixel Format:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(6, 187);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(71, 13);
            this.label7.TabIndex = 14;
            this.label7.Text = "Number Stills:";
            // 
            // comboBoxCapturePixelFormat
            // 
            this.comboBoxCapturePixelFormat.FormattingEnabled = true;
            this.comboBoxCapturePixelFormat.Location = new System.Drawing.Point(126, 76);
            this.comboBoxCapturePixelFormat.Name = "comboBoxCapturePixelFormat";
            this.comboBoxCapturePixelFormat.Size = new System.Drawing.Size(185, 21);
            this.comboBoxCapturePixelFormat.TabIndex = 17;
            // 
            // numericUpDownCaptureNumberStills
            // 
            this.numericUpDownCaptureNumberStills.Location = new System.Drawing.Point(126, 185);
            this.numericUpDownCaptureNumberStills.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numericUpDownCaptureNumberStills.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownCaptureNumberStills.Name = "numericUpDownCaptureNumberStills";
            this.numericUpDownCaptureNumberStills.Size = new System.Drawing.Size(185, 20);
            this.numericUpDownCaptureNumberStills.TabIndex = 13;
            this.numericUpDownCaptureNumberStills.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 160);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(77, 13);
            this.label6.TabIndex = 12;
            this.label6.Text = "Frame Interval:";
            // 
            // numericUpDownCaptureFrameInterval
            // 
            this.numericUpDownCaptureFrameInterval.Location = new System.Drawing.Point(126, 158);
            this.numericUpDownCaptureFrameInterval.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numericUpDownCaptureFrameInterval.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownCaptureFrameInterval.Name = "numericUpDownCaptureFrameInterval";
            this.numericUpDownCaptureFrameInterval.Size = new System.Drawing.Size(185, 20);
            this.numericUpDownCaptureFrameInterval.TabIndex = 11;
            this.numericUpDownCaptureFrameInterval.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 52);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(67, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "Video Mode:";
            // 
            // comboBoxCaptureVideoMode
            // 
            this.comboBoxCaptureVideoMode.FormattingEnabled = true;
            this.comboBoxCaptureVideoMode.Location = new System.Drawing.Point(126, 49);
            this.comboBoxCaptureVideoMode.Name = "comboBoxCaptureVideoMode";
            this.comboBoxCaptureVideoMode.Size = new System.Drawing.Size(185, 21);
            this.comboBoxCaptureVideoMode.TabIndex = 7;
            this.comboBoxCaptureVideoMode.SelectedIndexChanged += new System.EventHandler(this.comboBoxCaptureVideoMode_SelectedIndexChanged);
            // 
            // comboBoxCaptureDevice
            // 
            this.comboBoxCaptureDevice.FormattingEnabled = true;
            this.comboBoxCaptureDevice.Location = new System.Drawing.Point(126, 22);
            this.comboBoxCaptureDevice.Name = "comboBoxCaptureDevice";
            this.comboBoxCaptureDevice.Size = new System.Drawing.Size(185, 21);
            this.comboBoxCaptureDevice.TabIndex = 5;
            this.comboBoxCaptureDevice.SelectedIndexChanged += new System.EventHandler(this.comboBoxCaptureDevice_SelectedIndexChanged);
            // 
            // checkBoxEnableFormatDetection
            // 
            this.checkBoxEnableFormatDetection.AutoSize = true;
            this.checkBoxEnableFormatDetection.Location = new System.Drawing.Point(126, 106);
            this.checkBoxEnableFormatDetection.Name = "checkBoxEnableFormatDetection";
            this.checkBoxEnableFormatDetection.Size = new System.Drawing.Size(15, 14);
            this.checkBoxEnableFormatDetection.TabIndex = 4;
            this.checkBoxEnableFormatDetection.UseVisualStyleBackColor = true;
            this.checkBoxEnableFormatDetection.CheckedChanged += new System.EventHandler(this.checkBoxEnableFormatDetection_CheckedChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 106);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(91, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "Format Detection:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 25);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(71, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Input Device:";
            // 
            // buttonCaptureStartStop
            // 
            this.buttonCaptureStartStop.Enabled = false;
            this.buttonCaptureStartStop.Location = new System.Drawing.Point(106, 211);
            this.buttonCaptureStartStop.Name = "buttonCaptureStartStop";
            this.buttonCaptureStartStop.Size = new System.Drawing.Size(95, 23);
            this.buttonCaptureStartStop.TabIndex = 1;
            this.buttonCaptureStartStop.Text = "Start Capture";
            this.buttonCaptureStartStop.UseVisualStyleBackColor = true;
            this.buttonCaptureStartStop.Click += new System.EventHandler(this.buttonStartCapture_Click);
            // 
            // groupBoxPlayback
            // 
            this.groupBoxPlayback.Controls.Add(this.label5);
            this.groupBoxPlayback.Controls.Add(this.comboBoxPlaybackVideoMode);
            this.groupBoxPlayback.Controls.Add(this.label11);
            this.groupBoxPlayback.Controls.Add(this.checkBoxLoopPlayback);
            this.groupBoxPlayback.Controls.Add(this.comboBoxPlaybackPixelFormat);
            this.groupBoxPlayback.Controls.Add(this.label9);
            this.groupBoxPlayback.Controls.Add(this.label10);
            this.groupBoxPlayback.Controls.Add(this.comboBoxPlaybackDevice);
            this.groupBoxPlayback.Controls.Add(this.numericUpDownPlaybackFrameInterval);
            this.groupBoxPlayback.Controls.Add(this.buttonPlaybackStartStop);
            this.groupBoxPlayback.Controls.Add(this.label8);
            this.groupBoxPlayback.Location = new System.Drawing.Point(12, 259);
            this.groupBoxPlayback.Name = "groupBoxPlayback";
            this.groupBoxPlayback.Size = new System.Drawing.Size(317, 185);
            this.groupBoxPlayback.TabIndex = 1;
            this.groupBoxPlayback.TabStop = false;
            this.groupBoxPlayback.Text = "Playback";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(6, 49);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(67, 13);
            this.label5.TabIndex = 18;
            this.label5.Text = "Video Mode:";
            // 
            // comboBoxPlaybackVideoMode
            // 
            this.comboBoxPlaybackVideoMode.FormattingEnabled = true;
            this.comboBoxPlaybackVideoMode.Location = new System.Drawing.Point(126, 46);
            this.comboBoxPlaybackVideoMode.Name = "comboBoxPlaybackVideoMode";
            this.comboBoxPlaybackVideoMode.Size = new System.Drawing.Size(185, 21);
            this.comboBoxPlaybackVideoMode.TabIndex = 17;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(6, 76);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(67, 13);
            this.label11.TabIndex = 16;
            this.label11.Text = "Pixel Format:";
            // 
            // checkBoxLoopPlayback
            // 
            this.checkBoxLoopPlayback.AutoSize = true;
            this.checkBoxLoopPlayback.Location = new System.Drawing.Point(126, 103);
            this.checkBoxLoopPlayback.Name = "checkBoxLoopPlayback";
            this.checkBoxLoopPlayback.Size = new System.Drawing.Size(15, 14);
            this.checkBoxLoopPlayback.TabIndex = 16;
            this.checkBoxLoopPlayback.UseVisualStyleBackColor = true;
            // 
            // comboBoxPlaybackPixelFormat
            // 
            this.comboBoxPlaybackPixelFormat.FormattingEnabled = true;
            this.comboBoxPlaybackPixelFormat.Location = new System.Drawing.Point(126, 73);
            this.comboBoxPlaybackPixelFormat.Name = "comboBoxPlaybackPixelFormat";
            this.comboBoxPlaybackPixelFormat.Size = new System.Drawing.Size(185, 21);
            this.comboBoxPlaybackPixelFormat.TabIndex = 15;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(6, 130);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(77, 13);
            this.label9.TabIndex = 16;
            this.label9.Text = "Frame Interval:";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(6, 103);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(81, 13);
            this.label10.TabIndex = 15;
            this.label10.Text = "Loop Playback:";
            // 
            // comboBoxPlaybackDevice
            // 
            this.comboBoxPlaybackDevice.FormattingEnabled = true;
            this.comboBoxPlaybackDevice.Location = new System.Drawing.Point(126, 19);
            this.comboBoxPlaybackDevice.Name = "comboBoxPlaybackDevice";
            this.comboBoxPlaybackDevice.Size = new System.Drawing.Size(185, 21);
            this.comboBoxPlaybackDevice.TabIndex = 16;
            this.comboBoxPlaybackDevice.SelectedIndexChanged += new System.EventHandler(this.comboBoxPlaybackDevice_SelectedIndexChanged);
            // 
            // numericUpDownPlaybackFrameInterval
            // 
            this.numericUpDownPlaybackFrameInterval.Location = new System.Drawing.Point(126, 128);
            this.numericUpDownPlaybackFrameInterval.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numericUpDownPlaybackFrameInterval.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownPlaybackFrameInterval.Name = "numericUpDownPlaybackFrameInterval";
            this.numericUpDownPlaybackFrameInterval.Size = new System.Drawing.Size(185, 20);
            this.numericUpDownPlaybackFrameInterval.TabIndex = 15;
            this.numericUpDownPlaybackFrameInterval.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // buttonPlaybackStartStop
            // 
            this.buttonPlaybackStartStop.Enabled = false;
            this.buttonPlaybackStartStop.Location = new System.Drawing.Point(106, 154);
            this.buttonPlaybackStartStop.Name = "buttonPlaybackStartStop";
            this.buttonPlaybackStartStop.Size = new System.Drawing.Size(95, 23);
            this.buttonPlaybackStartStop.TabIndex = 0;
            this.buttonPlaybackStartStop.Text = "Start Playback";
            this.buttonPlaybackStartStop.UseVisualStyleBackColor = true;
            this.buttonPlaybackStartStop.Click += new System.EventHandler(this.buttonStartPlayback_Click);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(6, 22);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(79, 13);
            this.label8.TabIndex = 15;
            this.label8.Text = "Output Device:";
            // 
            // buttonBrowseDirectory
            // 
            this.buttonBrowseDirectory.Location = new System.Drawing.Point(961, 11);
            this.buttonBrowseDirectory.Name = "buttonBrowseDirectory";
            this.buttonBrowseDirectory.Size = new System.Drawing.Size(75, 23);
            this.buttonBrowseDirectory.TabIndex = 3;
            this.buttonBrowseDirectory.Text = "Browse...";
            this.buttonBrowseDirectory.UseVisualStyleBackColor = true;
            this.buttonBrowseDirectory.Click += new System.EventHandler(this.buttonBrowseDirectory_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(348, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(49, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "File type:";
            // 
            // comboBoxFileType
            // 
            this.comboBoxFileType.FormattingEnabled = true;
            this.comboBoxFileType.Location = new System.Drawing.Point(403, 12);
            this.comboBoxFileType.Name = "comboBoxFileType";
            this.comboBoxFileType.Size = new System.Drawing.Size(134, 21);
            this.comboBoxFileType.TabIndex = 5;
            this.comboBoxFileType.SelectedIndexChanged += new System.EventHandler(this.comboBoxFileType_SelectedIndexChanged);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(568, 15);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(52, 13);
            this.label13.TabIndex = 6;
            this.label13.Text = "Directory:";
            // 
            // textBoxDirectoryPath
            // 
            this.textBoxDirectoryPath.Enabled = false;
            this.textBoxDirectoryPath.Location = new System.Drawing.Point(626, 13);
            this.textBoxDirectoryPath.Name = "textBoxDirectoryPath";
            this.textBoxDirectoryPath.Size = new System.Drawing.Size(329, 20);
            this.textBoxDirectoryPath.TabIndex = 7;
            // 
            // panelStillsViewer
            // 
            this.panelStillsViewer.Location = new System.Drawing.Point(335, 39);
            this.panelStillsViewer.Name = "panelStillsViewer";
            this.panelStillsViewer.Size = new System.Drawing.Size(701, 405);
            this.panelStillsViewer.TabIndex = 8;
            // 
            // Stills
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1048, 457);
            this.Controls.Add(this.panelStillsViewer);
            this.Controls.Add(this.textBoxDirectoryPath);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.comboBoxFileType);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.buttonBrowseDirectory);
            this.Controls.Add(this.groupBoxPlayback);
            this.Controls.Add(this.groupBoxCapture);
            this.Name = "Stills";
            this.Text = "Stills";
            this.Load += new System.EventHandler(this.Stills_Load);
            this.groupBoxCapture.ResumeLayout(false);
            this.groupBoxCapture.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownCaptureNumberStills)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownCaptureFrameInterval)).EndInit();
            this.groupBoxPlayback.ResumeLayout(false);
            this.groupBoxPlayback.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPlaybackFrameInterval)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBoxCapture;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
        private System.Windows.Forms.GroupBox groupBoxPlayback;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button buttonBrowseDirectory;
        private System.Windows.Forms.Button buttonPlaybackStartStop;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button buttonCaptureStartStop;
        private System.Windows.Forms.ComboBox comboBoxFileType;
        private System.Windows.Forms.CheckBox checkBoxEnableFormatDetection;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown numericUpDownCaptureNumberStills;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.NumericUpDown numericUpDownCaptureFrameInterval;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox comboBoxCaptureVideoMode;
        private System.Windows.Forms.ComboBox comboBoxCaptureDevice;
        private System.Windows.Forms.ComboBox comboBoxPlaybackDevice;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.CheckBox checkBoxLoopPlayback;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.NumericUpDown numericUpDownPlaybackFrameInterval;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.ComboBox comboBoxCapturePixelFormat;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ComboBox comboBoxPlaybackPixelFormat;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TextBox textBoxDirectoryPath;
        private System.Windows.Forms.CheckBox checkBoxInputValid;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Panel panelStillsViewer;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox comboBoxPlaybackVideoMode;
    }
}

