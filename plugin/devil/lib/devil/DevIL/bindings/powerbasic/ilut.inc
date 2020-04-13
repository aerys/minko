'-----------------------------------------------------------------------------
'
' ImageLib Utility Toolkit Sources
' Copyright (C) 2000-2002 by Denton Woods
' Last modified: 06/22/2002 <--Y2K Compliant! =]
'
' Filename: il/ilut.h
'
' Description: The main include file for ILUT
'
'-----------------------------------------------------------------------------
'-----------------------------------------------------------------------------
'
' Translated to Powerbasic by Peter Scheutz, Scheutz & Clementsen Design
' Web: http://www.scheutz.dk
' e-mail: sourcecode@scheutz.dk
'
' Last modified: 06/22/2002
' Based on DevIL Ver. 1.6.1 headers
'
' Filename: ilut.inc
'
' Description: PB main include file for OpenILUT
'
' For information on use and download of DevIL goto: http://openil.sourceforge.net/
' (Get docs and "End User Package")
'
' Report errors in the Powerbasic includes to e-mail above.
'
' For general help on Powerbasic and DevIL, try the forums at:
' http://www.powerbasic.com
'-----------------------------------------------------------------------------




#If Not %Def(%WINAPI)
    #Include "win32api.inc"
#EndIf


' #ifndef __ilut_h_
' #ifndef __ILUT_H__

' %__ilut_h_
' %__ILUT_H__

' #include <il/il.h>
' #include <il/ilu.h>

' #ifdef __cplusplus
' extern "C" {
' #endif


' #ifdef _WIN32
'  #ifdef _MSC_VER
'  #ifndef _ILUT_BUILD_LIBRARY
'  #ifdef _DEBUG
'  #pragma comment(lib, "ilut-d.lib")
'  #else
'  #pragma comment(lib, "ilut.lib")
'  #endif'_DEBUG
'  #endif'_ILUT_BUILD_LIBRARY
'  #endif'_MSC_VER
' #endif'_WIN32


%ILUT_VERSION_1_5_6 = 1
%ILUT_VERSION = 156


' Attribute Bits
%ILUT_OPENGL_BIT        = &H00000001&
%ILUT_D3D_BIT           = &H00000002&
%ILUT_ALL_ATTRIB_BITS   = &H000FFFFF&


' Error Types
%ILUT_INVALID_ENUM          = &H0501&
%ILUT_OUT_OF_MEMORY         = &H0502&
%ILUT_INVALID_VALUE         = &H0505&
%ILUT_ILLEGAL_OPERATION     = &H0506&
%ILUT_INVALID_PARAM         = &H0509&
%ILUT_COULD_NOT_OPEN_FILE   = &H050A&
%ILUT_STACK_OVERFLOW        = &H050E&
%ILUT_STACK_UNDERFLOW       = &H050F&
%ILUT_NOT_SUPPORTED         = &H0550&


' State Definitions
%ILUT_PALETTE_MODE      = &H0600&
%ILUT_OPENGL_CONV       = &H0610&
%ILUT_D3D_MIPLEVELS     = &H0620&
%ILUT_MAXTEX_WIDTH      = &H0630&
%ILUT_MAXTEX_HEIGHT     = &H0631&
%ILUT_MAXTEX_DEPTH      = &H0632&
%ILUT_GL_USE_S3TC       = &H0634
%ILUT_D3D_USE_DXTC      = &H0634
%ILUT_GL_GEN_S3TC       = &H0635
%ILUT_D3D_GEN_DXTC      = &H0635
%ILUT_S3TC_FORMAT       = &H0705
%ILUT_DXTC_FORMAT       = &H0706


' Values
%ILUT_VERSION_NUM       = &H0DE2&


' ImageLib Utility Toolkit Functions
Declare Function ilutDisable Lib "ilut.dll" Alias "ilutDisable" (ByVal pb_Mode As Dword) As Byte
Declare Function ilutEnable Lib "ilut.dll" Alias "ilutEnable" (ByVal pb_Mode As Dword) As Byte
Declare Function ilutGetBoolean Lib "ilut.dll" Alias "ilutGetBoolean" (ByVal pb_Mode As Dword) As Byte
Declare Sub ilutGetBooleanv Lib "ilut.dll" Alias "ilutGetBooleanv" (ByVal pb_Mode As Dword, ByRef pb_Param As Byte)
Declare Function ilutGetInteger Lib "ilut.dll" Alias "ilutGetInteger" (ByVal pb_Mode As Dword) As Long
Declare Sub ilutGetIntegerv Lib "ilut.dll" Alias "ilutGetIntegerv" (ByVal pb_Mode As Dword, ByRef pb_Param As Long)
Declare Function ilutGetString Lib "ilut.dll" Alias "ilutGetString" (ByVal pb_StringName As Dword) As Long
Declare Sub ilutInit Lib "ilut.dll" Alias "ilutInit" ()
Declare Function ilutIsDisabled Lib "ilut.dll" Alias "ilutIsDisabled" (ByVal pb_Mode As Dword) As Byte
Declare Function ilutIsEnabled Lib "ilut.dll" Alias "ilutIsEnabled" (ByVal pb_Mode As Dword) As Byte
Declare Sub ilutPopAttrib Lib "ilut.dll" Alias "ilutPopAttrib" ()
Declare Sub ilutPushAttrib Lib "ilut.dll" Alias "ilutPushAttrib" (ByVal pb_Bits As Dword)
Declare Sub ilutSetInteger Lib "ilut.dll" Alias "ilutSetInteger" (ByVal pb_Mode As Dword, ByVal pb_Param As Dword)


' #ifdef DJGPP
'  %ILUT_USE_ALLEGRO
' #elif _WIN32
'  %ILUT_USE_WIN32
'  %ILUT_USE_OPENGL
' #else
'  %ILUT_USE_OPENGL
' #endif

'%ILUT_USE_ALLEGRO
'%ILUT_USE_WIN32
'%ILUT_USE_OPENGL


' The different rendering api's...more to be added later?
%ILUT_OPENGL = 0
%ILUT_ALLEGRO = 1
%ILUT_WIN32 = 2


Declare Function ilutRenderer Lib "ilut.dll" Alias "ilutRenderer" (ByVal pb_Renderer As Dword) As Byte


' ImageLib Utility Toolkit's OpenGL Functions
' #ifdef ILUT_USE_OPENGL
'  #ifdef _MSC_VER
'  %WIN32_LEAN_AND_MEAN
'  #include <windows.h>
'  #endif'_MSC_VER
'  #include <GL/gl.h>
'  #include <GL/glu.h>
'  #include <GL/glext.h>
'  #ifdef _MSC_VER
'  #pragma comment(lib, "opengl32.lib")
'  #pragma comment(lib, "glu32.lib")
'  #endif'_MSC_VER
Declare Function ilutGLBindTexImage Lib "ilut.dll" Alias "ilutGLBindTexImage" () As Dword
Declare Function ilutGLBindMipCompressed Lib "ilut.dll" Alias "ilutGLBindMipCompressed" () As Dword
Declare Function ilutGLBuildMipmaps Lib "ilut.dll" Alias "ilutGLBuildMipmaps" () As Byte
Declare Function ilutGLLoadImage Lib "ilut.dll" Alias "ilutGLLoadImage" (ByRef pb_FileName As Asciiz) As Dword
Declare Function ilutGLScreen Lib "ilut.dll" Alias "ilutGLScreen" () As Byte
Declare Function ilutGLScreenie Lib "ilut.dll" Alias "ilutGLScreenie" () As Byte
Declare Function ilutGLSaveImage Lib "ilut.dll" Alias "ilutGLSaveImage" (ByRef pb_FileName As Asciiz, ByVal pb_TexID As Dword) As Byte
Declare Function ilutGLSetTex Lib "ilut.dll" Alias "ilutGLSetTex" (ByVal pb_TexID As Dword) As Byte
Declare Function ilutGLTexImage Lib "ilut.dll" Alias "ilutGLTexImage" (ByVal pb_Level As Dword) As Byte
' #endif'ILUT_USE_OPENGL


' ImageLib Utility ToolKit's Allegro Functions
' #ifdef ILUT_USE_ALLEGRO
'  #include <allegro.h>
Declare Function ilutAllegLoadImage Lib "ilut.dll" Alias "ilutAllegLoadImage" (ByRef pb_FileName As Asciiz) As Long
'Declare Function ilutConvertToAlleg Lib "ilut.dll" Alias "ilutConvertToAlleg" (ByVal pb_Pal As PALETTE) As Long
' #endif'ILUT_USE_ALLEGRO


' ImageLib Utility Toolkit's Win32 (DirectX/GDI) Functions
' #ifdef ILUT_USE_WIN32
'  #ifdef _WIN32
'  %WIN32_LEAN_AND_MEAN
'  #include <windows.h>
Declare Function ilutConvertToHBitmap Lib "ilut.dll" Alias "ilutConvertToHBitmap" (ByVal pb_hDC As Long) As Long
Declare Sub ilutGetBmpInfo Lib "ilut.dll" Alias "ilutGetBmpInfo" (ByRef pb_Info As BITMAPINFO)
Declare Function ilutGetHPal Lib "ilut.dll" Alias "ilutGetHPal" () As Long
Declare Function ilutGetPaddedData Lib "ilut.dll" Alias "ilutGetPaddedData" () As Long
Declare Function ilutGetWinClipboard Lib "ilut.dll" Alias "ilutGetWinClipboard" () As Byte
Declare Function ilutLoadResource Lib "ilut.dll" Alias "ilutLoadResource" (ByVal pb_hInst As Long, ByVal pb_ID As Long, ByRef pb_ResourceType As Asciiz, ByVal pb_Type As Dword) As Byte
Declare Function ilutSetHBitmap Lib "ilut.dll" Alias "ilutSetHBitmap" (ByVal pb_Bitmap As Long) As Byte
Declare Function ilutSetHPal Lib "ilut.dll" Alias "ilutSetHPal" (ByVal pb_Pal As Long) As Byte
Declare Function ilutSetWinClipboard Lib "ilut.dll" Alias "ilutSetWinClipboard" () As Byte
Declare Function ilutWinLoadImage Lib "ilut.dll" Alias "ilutWinLoadImage" (ByRef pb_FileName As Asciiz, ByVal pb_hDC As Long) As Long
Declare Function ilutWinLoadUrl Lib "ilut.dll" Alias "ilutWinLoadUrl" (ByRef pb_Url As Asciiz) As Byte
Declare Function ilutWinSaveImage Lib "ilut.dll" Alias "ilutWinSaveImage" (ByRef pb_FileName As Asciiz, ByVal pb_Bitmap As Long) As Byte

'  #ifdef ILUT_USE_DIRECTX7
'  LPDIRECTDRAWSURFACE7 ILAPIENTRY ilutDX7Surface(char *FileName, LPDIRECTDRAW7 DDraw);
'  #endif'ILUT_USE_DIRECTX7
'  #endif'_WIN32
' #endif'ILUT_USE_WIN32


' #ifdef __cplusplus
' }
' #endif

' #endif /* __ILUT_H__ */
' #endif /* __ilut_h_ */
