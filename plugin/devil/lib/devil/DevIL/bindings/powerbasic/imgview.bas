'-----------------------------------------------------------------------------
' Minimal Image viewer using DevIL
' DevIL Sample program using Powerbasic.
'
' by Peter Scheutz, Scheutz & Clementsen Design
' Web: http://www.scheutz.dk
' e-mail: sourcecode@scheutz.dk
'
' Last modified: 06/22/2002
' Based on DevIL Ver. 1.6.1 PB incs
' Filename: imgview.bas
'
' Released under the GNU Lesser General Public License:
' http://www.gnu.org/copyleft/lesser.html
'
' For information on use and download of DevIL goto: http://openil.sourceforge.net/
' (Get docs and "End User Package")
'
' Report errors in the Powerbasic includes to e-mail above.
'
' For general help on Powerbasic and DevIL, try the forums at:
' http://www.powerbasic.com
'-----------------------------------------------------------------------------


#Register None
#Compile Exe "imgview.exe"

Option Explicit

#Include "win32api.inc"

#Include "il.inc"
#Include "ilu.inc"
#Include "ilut.inc"


Global hDCBmp As Long
Global pic As BITMAP

CallBack Function mainCallback()
    Local ps As PAINTSTRUCT
    Local hDC As Long
    Select Case CbMsg
        Case %WM_PAINT
            hDC = BeginPaint( CbHndl, ps )
            If hDCBmp<>0 Then
                BitBlt hDC, 0, 0, pic.bmWidth, pic.bmHeight  , hDCBmp, 0, 0, %SRCCOPY
            End If
            Call EndPaint( CbHndl, ps )

  End Select

End Function


Sub SetWindowClientSize (ByVal hWnd As Long,ByVal PixX As Long,ByVal PixY As Long)
   ' set window by client size in pixels
   ' centers window and clips to screen

    Local cRect As RECT
    Local diffX As Long
    Local diffY As Long


    GetWindowRect hWnd,cRect
    diffX=cRect.nRight-cRect.nLeft
    diffY=cRect.nBottom-cRect.nTop

    GetClientRect hWnd,cRect
    diffX=diffX-(cRect.nRight-cRect.nLeft)
    diffY=diffY-(cRect.nBottom-cRect.nTop)

    PixX=PixX+diffX
    PixY=PixY+diffY

    If PixX> GetSystemMetrics(%SM_CXSCREEN) Then   PixX = GetSystemMetrics(%SM_CXSCREEN)
    If PixY> GetSystemMetrics(%SM_CYSCREEN) Then   PixY = GetSystemMetrics(%SM_CYSCREEN)

    SetWindowPos hWnd, 0, (GetSystemMetrics(%SM_CXSCREEN) - PixX) / 2, _
    (GetSystemMetrics(%SM_CYSCREEN) - PixY) / 2, PixX, PixY, 0


End Sub



Function PbMain() As Long

    Local ILErr As Dword
    Local hBmp As Long

    Local hWnd As Long
    Local hDC As Long

    Local hBmpOld As Long
    Local errString As Asciiz*1024


    If  Len(Command$)=0 Then
        MsgBox "Usage: imgview.exe filename"
        Exit Function
    End If

    ilInit

    ilEnable %IL_FORMAT_SET
    ilEnable %IL_ORIGIN_SET
    ilEnable %IL_TYPE_SET

    ilFormatFunc %IL_RGB
    ilOriginFunc %IL_ORIGIN_LOWER_LEFT
    ilTypeFunc %IL_UNSIGNED_BYTE

    ilEnable %IL_CONV_PAL

    Do
        ILErr = ilGetError()
        If ILErr <>0 Then
            errString = iluErrorString(ILErr)
            MsgBox "Error in Set or enable:" & $CRLF &  errString
        End If
    Loop While (ILErr <> 0)





    Dialog New 0, "DevIL Mini imageviewer in PB " & Command$, , , 100, 100 , %WS_SYSMENU Or %WS_CAPTION To hWnd
    hDC = getDC (hWnd)
    If hDC<>0 Then
        hBmp = ilutWinLoadImage (Command$,getDC (hWnd) )
        ILErr=ilGetError()
        If ILErr<>0 Then
            errString = iluErrorString(ILErr)
            MsgBox "Error in ilutWinLoadImage:" & $CRLF &  errString
            Exit Function
        Else
            GetObject hBmp, SizeOf( pic ), pic
            SetWindowClientSize hWnd ,pic.bmWidth, pic.bmHeight

            hDCBmp = CreateCompatibleDC( hDC )
            hBmpOld = SelectObject( hDCBmp, hBmp )

            Dialog Show Modal hWnd  Call mainCallback
        End If


    End If

    If  hBmp < > 0  Then
        SelectObject hDCBmp, hBmpOld
        DeleteObject hBmp
        hBmp = 0
    End If





End Function







