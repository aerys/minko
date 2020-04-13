using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace ImageViewCs
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class frmImageView : System.Windows.Forms.Form
	{
		private System.Windows.Forms.MainMenu mainMenu;
		private System.Windows.Forms.PictureBox pictureBox;
		private System.Windows.Forms.MenuItem mnuFile;
		private System.Windows.Forms.MenuItem mnuFileOpen;
		private System.Windows.Forms.MenuItem mnuFileSaveAs;
		private System.Windows.Forms.MenuItem mnuFileSep1;
		private System.Windows.Forms.MenuItem mnuFileExit;
		private System.Windows.Forms.MenuItem mnuHelp;
		private System.Windows.Forms.MenuItem mnuHelpAbout;
		private System.Windows.Forms.Panel panel;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public frmImageView()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.mainMenu = new System.Windows.Forms.MainMenu();
			this.mnuFile = new System.Windows.Forms.MenuItem();
			this.mnuFileOpen = new System.Windows.Forms.MenuItem();
			this.mnuFileSaveAs = new System.Windows.Forms.MenuItem();
			this.mnuFileSep1 = new System.Windows.Forms.MenuItem();
			this.mnuFileExit = new System.Windows.Forms.MenuItem();
			this.mnuHelp = new System.Windows.Forms.MenuItem();
			this.mnuHelpAbout = new System.Windows.Forms.MenuItem();
			this.panel = new System.Windows.Forms.Panel();
			this.pictureBox = new System.Windows.Forms.PictureBox();
			this.panel.SuspendLayout();
			this.SuspendLayout();
			// 
			// mainMenu
			// 
			this.mainMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.mnuFile,
																					 this.mnuHelp});
			// 
			// mnuFile
			// 
			this.mnuFile.Index = 0;
			this.mnuFile.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.mnuFileOpen,
																					this.mnuFileSaveAs,
																					this.mnuFileSep1,
																					this.mnuFileExit});
			this.mnuFile.Text = "&File";
			// 
			// mnuFileOpen
			// 
			this.mnuFileOpen.Index = 0;
			this.mnuFileOpen.Text = "&Open...";
			this.mnuFileOpen.Click += new System.EventHandler(this.mnuFileOpen_Click);
			// 
			// mnuFileSaveAs
			// 
			this.mnuFileSaveAs.Index = 1;
			this.mnuFileSaveAs.Text = "Save &as...";
			this.mnuFileSaveAs.Click += new System.EventHandler(this.mnuFileSaveAs_Click);
			// 
			// mnuFileSep1
			// 
			this.mnuFileSep1.Index = 2;
			this.mnuFileSep1.Text = "-";
			// 
			// mnuFileExit
			// 
			this.mnuFileExit.Index = 3;
			this.mnuFileExit.Text = "E&xit";
			this.mnuFileExit.Click += new System.EventHandler(this.mnuFileExit_Click);
			// 
			// mnuHelp
			// 
			this.mnuHelp.Index = 1;
			this.mnuHelp.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.mnuHelpAbout});
			this.mnuHelp.Text = "&?";
			// 
			// mnuHelpAbout
			// 
			this.mnuHelpAbout.Index = 0;
			this.mnuHelpAbout.Text = "&About...";
			this.mnuHelpAbout.Click += new System.EventHandler(this.mnuHelpAbout_Click);
			// 
			// panel
			// 
			this.panel.AutoScroll = true;
			this.panel.Controls.AddRange(new System.Windows.Forms.Control[] {
																				this.pictureBox});
			this.panel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel.Name = "panel";
			this.panel.Size = new System.Drawing.Size(720, 414);
			this.panel.TabIndex = 1;
			// 
			// pictureBox
			// 
			this.pictureBox.Name = "pictureBox";
			this.pictureBox.Size = new System.Drawing.Size(720, 414);
			this.pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
			this.pictureBox.TabIndex = 0;
			this.pictureBox.TabStop = false;
			// 
			// frmImageView
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(720, 414);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.panel});
			this.Menu = this.mainMenu;
			this.Name = "frmImageView";
			this.Text = "ImageView C#";
			this.panel.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new frmImageView());
		}

		private System.Drawing.Bitmap m_bmp;

		private void mnuFileOpen_Click(object sender, System.EventArgs e)
		{
			System.Windows.Forms.OpenFileDialog ofd = new System.Windows.Forms.OpenFileDialog();
			ofd.Filter = "All Image Files(*.BMP;*.CUT;*.DCX;*.DDS;*.ICO;*.GIF;*.JPG;*.LBM;*.LIF;*.MDL;*.PCD;*.PCX;*.PIC;*.PNG;*.PNM;*.PSD;*.PSP;*.RAW;*.SGI;*.TGA;*.TIF;*.WAL;*.ACT;*.PAL;)|*.BMP;*.CUT;*.DCX;*.DDS;*.ICO;*.GIF;*.JPG;*.LBM;*.LIF;*.MDL;*.PCD;*.PCX;*.PIC;*.PNG;*.PNM;*.PSD;*.PSP;*.RAW;*.SGI;*.TGA;*.TIF;*.WAL;*.ACT;*.PAL|All files (*.*)|*.*";
			ofd.Filter += "|BMP files (*.BMP)|*.BMP";
			ofd.Filter += "|CUT files (*.CUT)|*.CUT";
			ofd.Filter += "|DCX files (*.DCX)|*.DCX";
			ofd.Filter += "|DDS files (*.DDS)|*.DDS";
			ofd.Filter += "|ICO files (*.ICO)|*.ICO";
			ofd.Filter += "|GIF files (*.GIF)|*.GIF";
			ofd.Filter += "|JPG files (*.JPG)|*.JPG";
			ofd.Filter += "|LBM files (*.LBM)|*.LBM";
			ofd.Filter += "|LIF files (*.LIF)|*.LIF";
			ofd.Filter += "|MDL files (*.MDL)|*.MDL";
			ofd.Filter += "|PCD files (*.PCD)|*.PCD";
			ofd.Filter += "|PCX files (*.PCX)|*.PCX";
			ofd.Filter += "|PIC files (*.PIC)|*.PIC";
			ofd.Filter += "|PNG files (*.PNG)|*.PNG";
			ofd.Filter += "|PNM files (*.PNM)|*.PNM";
			ofd.Filter += "|PSD files (*.PSD)|*.PSD";
			ofd.Filter += "|PSP files (*.PSP)|*.PSP";
			ofd.Filter += "|RAW files (*.RAW)|*.RAW";
			ofd.Filter += "|SGI files (*.SGI)|*.SGI";
			ofd.Filter += "|TGA files (*.TGA)|*.TGA";
			ofd.Filter += "|TIF files (*.TIF)|*.TIF";
			ofd.Filter += "|WAL files (*.WAL)|*.WAL";
			ofd.Filter += "|ACT files (*.ACT)|*.ACT";
			ofd.Filter += "|PAL files (*.PAL)|*.PAL";
			ofd.Filter += "|All files (*.*)|*.*";

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				m_bmp = DevIL.DevIL.LoadBitmap(ofd.FileName);
				if (m_bmp != null)
				{
					pictureBox.Image = m_bmp;
				}
			}
		}

		private void mnuFileExit_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}

		private void mnuHelpAbout_Click(object sender, System.EventArgs e)
		{
			MessageBox.Show("Program by Marco Mastropaolo\n\nThis is a C# example for the DevIL.NET library.\n\nhttp://www.mastropaolo.com", "About");
		}

		private void mnuFileSaveAs_Click(object sender, System.EventArgs e)
		{
			if (m_bmp != null)
			{
				System.Windows.Forms.SaveFileDialog sfd = new System.Windows.Forms.SaveFileDialog();
				sfd.Filter = "BMP format (*.BMP)|*.BMP";
				sfd.Filter += "|DDS format (*.DDS)|*.DDS";
				sfd.Filter += "|JPG format (*.JPG)|*.JPG";
				sfd.Filter += "|PCX format (*.PCX)|*.PCX";
				sfd.Filter += "|PNG format (*.PNG)|*.PNG";
				sfd.Filter += "|PNM format (*.PNM)|*.PNM";
				sfd.Filter += "|RAW format (*.RAW)|*.RAW";
				sfd.Filter += "|SGI format (*.SGI)|*.SGI";
				sfd.Filter += "|TGA format (*.TGA)|*.TGA";
				sfd.Filter += "|TIF format (*.TIF)|*.TIF";
				sfd.Filter += "|PAL format (*.PAL)|*.PAL";
				
				if (sfd.ShowDialog() == DialogResult.OK)
				{
					DevIL.DevIL.SaveBitmap(sfd.FileName, m_bmp);
				}
			}
		}
	}
}
