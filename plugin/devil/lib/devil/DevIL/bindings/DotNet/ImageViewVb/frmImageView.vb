Public Class frmImageView
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents mainMenu As System.Windows.Forms.MainMenu
    Friend WithEvents mnuFile As System.Windows.Forms.MenuItem
    Friend WithEvents mnuFileOpen As System.Windows.Forms.MenuItem
    Friend WithEvents mnuFileSaveAs As System.Windows.Forms.MenuItem
    Friend WithEvents mnuFileSep1 As System.Windows.Forms.MenuItem
    Friend WithEvents mnuFileExit As System.Windows.Forms.MenuItem
    Friend WithEvents mnuHelp As System.Windows.Forms.MenuItem
    Friend WithEvents mnuHelpAbout As System.Windows.Forms.MenuItem
    Friend WithEvents pictureBox As System.Windows.Forms.PictureBox
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.mainMenu = New System.Windows.Forms.MainMenu()
        Me.mnuFile = New System.Windows.Forms.MenuItem()
        Me.mnuFileOpen = New System.Windows.Forms.MenuItem()
        Me.mnuFileSaveAs = New System.Windows.Forms.MenuItem()
        Me.mnuFileSep1 = New System.Windows.Forms.MenuItem()
        Me.mnuFileExit = New System.Windows.Forms.MenuItem()
        Me.mnuHelp = New System.Windows.Forms.MenuItem()
        Me.mnuHelpAbout = New System.Windows.Forms.MenuItem()
        Me.pictureBox = New System.Windows.Forms.PictureBox()
        Me.SuspendLayout()
        '
        'mainMenu
        '
        Me.mainMenu.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuFile, Me.mnuHelp})
        '
        'mnuFile
        '
        Me.mnuFile.Index = 0
        Me.mnuFile.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuFileOpen, Me.mnuFileSaveAs, Me.mnuFileSep1, Me.mnuFileExit})
        Me.mnuFile.Text = "&File"
        '
        'mnuFileOpen
        '
        Me.mnuFileOpen.Index = 0
        Me.mnuFileOpen.Text = "&Open..."
        '
        'mnuFileSaveAs
        '
        Me.mnuFileSaveAs.Index = 1
        Me.mnuFileSaveAs.Text = "Save &as..."
        '
        'mnuFileSep1
        '
        Me.mnuFileSep1.Index = 2
        Me.mnuFileSep1.Text = "-"
        '
        'mnuFileExit
        '
        Me.mnuFileExit.Index = 3
        Me.mnuFileExit.Text = "E&xit"
        '
        'mnuHelp
        '
        Me.mnuHelp.Index = 1
        Me.mnuHelp.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.mnuHelpAbout})
        Me.mnuHelp.Text = "&?"
        '
        'mnuHelpAbout
        '
        Me.mnuHelpAbout.Index = 0
        Me.mnuHelpAbout.Text = "&About..."
        '
        'pictureBox
        '
        Me.pictureBox.Name = "pictureBox"
        Me.pictureBox.Size = New System.Drawing.Size(720, 414)
        Me.pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize
        Me.pictureBox.TabIndex = 1
        Me.pictureBox.TabStop = False
        '
        'frmImageView
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(720, 434)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.pictureBox})
        Me.Menu = Me.mainMenu
        Me.Name = "frmImageView"
        Me.Text = "ImageView VB.NET"
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private m_bmp As System.Drawing.Bitmap


    Private Sub mnuFileOpen_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles mnuFileOpen.Click
        Dim ofd As New System.Windows.Forms.OpenFileDialog()
        ofd.Filter = "All Image Files(*.BMP;*.CUT;*.DCX;*.DDS;*.ICO;*.GIF;*.JPG;*.LBM;*.LIF;*.MDL;*.PCD;*.PCX;*.PIC;*.PNG;*.PNM;*.PSD;*.PSP;*.RAW;*.SGI;*.TGA;*.TIF;*.WAL;*.ACT;*.PAL;)|*.BMP;*.CUT;*.DCX;*.DDS;*.ICO;*.GIF;*.JPG;*.LBM;*.LIF;*.MDL;*.PCD;*.PCX;*.PIC;*.PNG;*.PNM;*.PSD;*.PSP;*.RAW;*.SGI;*.TGA;*.TIF;*.WAL;*.ACT;*.PAL|All files (*.*)|*.*"
        ofd.Filter += "|BMP files (*.BMP)|*.BMP"
        ofd.Filter += "|CUT files (*.CUT)|*.CUT"
        ofd.Filter += "|DCX files (*.DCX)|*.DCX"
        ofd.Filter += "|DDS files (*.DDS)|*.DDS"
        ofd.Filter += "|ICO files (*.ICO)|*.ICO"
        ofd.Filter += "|GIF files (*.GIF)|*.GIF"
        ofd.Filter += "|JPG files (*.JPG)|*.JPG"
        ofd.Filter += "|LBM files (*.LBM)|*.LBM"
        ofd.Filter += "|LIF files (*.LIF)|*.LIF"
        ofd.Filter += "|MDL files (*.MDL)|*.MDL"
        ofd.Filter += "|PCD files (*.PCD)|*.PCD"
        ofd.Filter += "|PCX files (*.PCX)|*.PCX"
        ofd.Filter += "|PIC files (*.PIC)|*.PIC"
        ofd.Filter += "|PNG files (*.PNG)|*.PNG"
        ofd.Filter += "|PNM files (*.PNM)|*.PNM"
        ofd.Filter += "|PSD files (*.PSD)|*.PSD"
        ofd.Filter += "|PSP files (*.PSP)|*.PSP"
        ofd.Filter += "|RAW files (*.RAW)|*.RAW"
        ofd.Filter += "|SGI files (*.SGI)|*.SGI"
        ofd.Filter += "|TGA files (*.TGA)|*.TGA"
        ofd.Filter += "|TIF files (*.TIF)|*.TIF"
        ofd.Filter += "|WAL files (*.WAL)|*.WAL"
        ofd.Filter += "|ACT files (*.ACT)|*.ACT"
        ofd.Filter += "|PAL files (*.PAL)|*.PAL"
        ofd.Filter += "|All files (*.*)|*.*"

        If (ofd.ShowDialog() = DialogResult.OK) Then
            m_bmp = DevIL.DevIL.LoadBitmap(ofd.FileName)
            If Not (m_bmp Is Nothing) Then
                pictureBox.Image = m_bmp
            End If
        End If
    End Sub

    Private Sub mnuFileSaveAs_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles mnuFileSaveAs.Click
        If Not (m_bmp Is Nothing) Then
            Dim sfd As New System.Windows.Forms.SaveFileDialog()
            sfd.Filter = "BMP format (*.BMP)|*.BMP"
            sfd.Filter += "|DDS format (*.DDS)|*.DDS"
            sfd.Filter += "|JPG format (*.JPG)|*.JPG"
            sfd.Filter += "|PCX format (*.PCX)|*.PCX"
            sfd.Filter += "|PNG format (*.PNG)|*.PNG"
            sfd.Filter += "|PNM format (*.PNM)|*.PNM"
            sfd.Filter += "|RAW format (*.RAW)|*.RAW"
            sfd.Filter += "|SGI format (*.SGI)|*.SGI"
            sfd.Filter += "|TGA format (*.TGA)|*.TGA"
            sfd.Filter += "|TIF format (*.TIF)|*.TIF"
            sfd.Filter += "|PAL format (*.PAL)|*.PAL"

            If (sfd.ShowDialog() = DialogResult.OK) Then
                DevIL.DevIL.SaveBitmap(sfd.FileName, m_bmp)
            End If
        End If
    End Sub

    Private Sub mnuFileExit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles mnuFileExit.Click
        Me.Close()
    End Sub

    Private Sub mnuHelpAbout_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles mnuHelpAbout.Click
        MessageBox.Show("Program by Marco Mastropaolo\n\nThis is a VB.NET example for the DevIL.NET library.\n\nhttp://www.mastropaolo.com", "About")
    End Sub
End Class
