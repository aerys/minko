'-----------------------------------------------------------------------------
' Get Image info using DevIL
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
#Compile Exe "imginfo.exe"
Option Explicit


#Include "il.inc"
#Include "ilu.inc"
#Include "ilut.inc"


Function OpenIL_TypeString(ByVal il_const As Dword) As String

    Select Case il_const
        Case %IL_BYTE : Function = "IL_BYTE"
        Case %IL_UNSIGNED_BYTE : Function = "IL_UNSIGNED_BYTE"
        Case %IL_SHORT : Function = "IL_SHORT"
        Case %IL_UNSIGNED_SHORT : Function = "IL_UNSIGNED_SHORT"
        Case %IL_INT : Function = "IL_INT"
        Case %IL_UNSIGNED_INT : Function = "IL_UNSIGNED_INT"
        Case %IL_FLOAT : Function = "IL_FLOAT"
        Case %GL_DOUBLE : Function = "GL_DOUBLE"

        Case Else : Function = "Type not defined"


    End Select

End Function




Function OpenIL_FormatString(ByVal il_const  As Dword) As String

    Select Case il_const
        Case %IL_COLOUR_INDEX : Function = "IL_COLOUR_INDEX"
        Case %IL_COLOR_INDEX : Function = "IL_COLOR_INDEX"
        Case %IL_RGB : Function = "IL_RGB"
        Case %IL_RGBA : Function = "IL_RGBA"
        Case %IL_BGR : Function = "IL_BGR"
        Case %IL_BGRA : Function = "IL_BGRA"
        Case %IL_LUMINANCE : Function = "IL_LUMINANCE"

        Case Else : Function = "Format not defined"


    End Select

End Function



Function OpenIL_OriginString(ByVal il_const As Dword) As String

    ' Origin Definitions
    Select Case il_const
        Case %IL_ORIGIN_LOWER_LEFT : Function = "IL_ORIGIN_LOWER_LEFT"
        Case %IL_ORIGIN_UPPER_LEFT : Function = "IL_ORIGIN_UPPER_LEFT"

        Case Else : Function = "Origin type not defined"
    End Select

End Function



Function OpenIL_PalTypeString(ByVal il_const As Dword) As String

    ' Palette types
    Select Case il_const
        Case %IL_PAL_NONE : Function = "IL_PAL_NONE"
        Case %IL_PAL_RGB24 : Function = "IL_PAL_RGB24"
        Case %IL_PAL_RGB32 : Function = "IL_PAL_RGB32"
        Case %IL_PAL_RGBA32 : Function = "IL_PAL_RGBA32"
        Case %IL_PAL_BGR24 : Function = "IL_PAL_BGR24"
        Case %IL_PAL_BGR32 : Function = "IL_PAL_BGR32"
        Case %IL_PAL_BGRA32 : Function = "IL_PAL_BGRA32"

        Case Else : Function = "Pallete type not defined"
    End Select

End Function


Function PbMain() As Long

    Local ILErr As Dword
    Local info As  ILinfo
    Local ID As Dword
    Local errString As Asciiz*1024


    If  Len(Command$)=0 Then
        MsgBox "Usage: imginfo.exe filename"
        Exit Function
    End If

    ilInit

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



   ' ilEnable %IL_FORMAT_SET
   ' ilEnable %IL_ORIGIN_SET
   ' ilEnable %IL_TYPE_SET
   '
   ' ilFormatFunc %IL_RGB
   ' ilOriginFunc %IL_ORIGIN_LOWER_LEFT
   ' ilTypeFunc %IL_UNSIGNED_BYTE
   '
   ' ilEnable %IL_CONV_PAL
   ' Do
   '     ILErr = ilGetError()
   ' Loop While (ILErr <> 0)





    ilLoadImage  Command$
    ILErr=ilGetError()
    If  ILErr <> 0 Then
            errString = iluErrorString(ILErr)
            MsgBox "Error in ilLoadImage" & $CRLF &  errString
            GoTo done
    End If


    iluGetImageInfo info
    ILErr=ilGetError()
    If  ILErr <> 0 Then
            errString = iluErrorString(ILErr)
            MsgBox "Error in iluGetImageInfo" & $CRLF &  errString
            GoTo done
    Else

        MsgBox "DevIL Image info: " & $CRLF & $CRLF & _
        "Id: " & $TAB & $TAB & Format$(info.iId) & $CRLF & _
        "Data Pointer"  & $TAB & Format$(info.iData) & $CRLF & _
        "Width: " & $TAB & $TAB & Format$(info.iWidth) & $CRLF & _
        "Height: " & $TAB & $TAB & Format$(info.iHeight) & $CRLF & _
        "Depth: " & $TAB & $TAB & Format$(info.iDepth) & $CRLF & _
        "Bpp: " & $TAB & $TAB & Format$(info.iBpp) & $CRLF & _
        "SizeOfData: " & $TAB & Format$(info.iSizeOfData) & $CRLF & _
        "Format: " & $TAB & $TAB & OpenIL_FormatString(info.iFormat) & $CRLF & _
        "Type: " & $TAB & $TAB & OpenIL_TypeString(info.iType) & $CRLF & _
        "Origin: " & $TAB & $TAB & OpenIL_OriginString(info.iOrigin) & $CRLF & _
        "PalType: " & $TAB & $TAB & OpenIL_PalTypeString(info.iPalType) & $CRLF & _
        "PalSize: " & $TAB & $TAB & Format$(info.iPalSize) & $CRLF & _
        "NumNext: "  & $TAB & Format$(info.iNumNext) & $CRLF & _
        "NumMips: " & $TAB & Format$(info.iNumMips) & $CRLF & _
        "NumLayers: "  & $TAB & Format$(info.iNumLayers)

    End If



done:
    ilDeleteImages 1 , ID

    ' clear additional errors
    Do
        ILErr = ilGetError()
    Loop While (ILErr <> 0)



End Function






                                                               