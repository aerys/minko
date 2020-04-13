'-----------------------------------------------------------------------------
' Convert Image using DevIL
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
#Compile Exe "imgconv.exe"
Option Explicit


#Include "il.inc"
#Include "ilu.inc"
#Include "ilut.inc"

$If Not %Def(%TRUE)
    %TRUE = 1
    %FALSE = 0
#EndIf



$If Not %Def(%MAX_PATH   )
    %MAX_PATH    = 260
#EndIf

%OVERWRITE = %False '%True

Function PbMain() As Long

    Local ILErr As Dword
    Local info As  ILinfo
    Local ID As Dword
    Local errString As Asciiz*1024
    Local Infile As Asciiz*%MAX_PATH
    Local Outfile As Asciiz*%MAX_PATH

    Infile="C:\test.bmp"    ' OBS: Change this !
    Outfile="C:\test.tga"   ' OBS: Change this !

    ilInit

    ilPushAttrib %IL_ALL_ATTRIB_BITS
    ILErr=ilGetError()
    If  ILErr <> 0 Then
            errString = iluErrorString(ILErr)
            MsgBox "Error in ilPushAttrib" & $CRLF &  errString
            Exit Function
    End If

    ilGenImages ByVal 1, ID
    ILErr=ilGetError()
    If  ILErr <> 0 Then
            errString = iluErrorString(ILErr)
            MsgBox "Error in ilGenImages" & $CRLF &  errString
            Exit Function
    End If


    ilBindImage ID
    ILErr=ilGetError()
    If  ILErr <> 0 Then
            errString = iluErrorString(ILErr)
            MsgBox "Error in ilBindImage" & $CRLF &  errString
            GoTo done
    End If

'    ilEnable %IL_FORMAT_SET
'    ilEnable %IL_ORIGIN_SET
'    ilEnable %IL_TYPE_SET

'    ilFormatFunc %IL_RGB
'    ilOriginFunc %IL_ORIGIN_LOWER_LEFT
'    ilTypeFunc %IL_UNSIGNED_BYTE
'
'    ilEnable %IL_CONV_PAL

'    Do
'        ILErr = ilGetError()
'        If ILErr<>0 Then
'            errString = iluErrorString(ILErr)
'            MsgBox "Error in ..Func or ilEnable" & $CRLF &  errString
'        End If
'    Loop While (ILErr <> 0)



    ilLoadImage  infile
    ILErr=ilGetError()
    If  ILErr <> 0 Then
            errString = iluErrorString(ILErr)
            MsgBox "Error in ilLoadImage" & $CRLF &  errString
            GoTo done
    End If

    ' unrem to test these effects.

'    iluRotate 50
'    iluPixelize 5
'    iluEnlargeImage 2,2,4
'    iluBlurGaussian 4

'    Do
'        ILErr = ilGetError()
'        If ILErr<>0 Then
'            errString = iluErrorString(ILErr)
'            MsgBox "Error in Effect" & $CRLF &  errString
'        End If
'    Loop While (ILErr <> 0)





    If %OVERWRITE=%True  And ilIsDisabled(%IL_FILE_OVERWRITE)  Then
        ilEnable %IL_FILE_OVERWRITE
        ilSaveImage outfile
        'ilDisable %IL_FILE_OVERWRITE   ' done by   ilPushAttrib/ilPopAttrib
    Else
        ilSaveImage outfile

    End If

    ILErr=ilGetError()
    If  ILErr <> 0 Then
        errString = iluErrorString(ILErr)
        MsgBox "Error in ilSaveImage" & $CRLF &  errString
        GoTo done
    Else
        MsgBox "Picture converted"
    End If

done:
    ilDeleteImages 1 , ID

    ilPopAttrib


    ' clear additional errors
    Do
        ILErr = ilGetError()
    Loop While (ILErr <> 0)





End Function







                                                           