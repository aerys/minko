Attribute VB_Name = "Module3"
'-----------------------------------------------------------------------------
'
' ImageLib Utility Toolkit Sources
' Copyright (C) 2000-2002 by Denton Woods
' Converted from ilut.h by Rune Kock (rune@vupti.com)
' based on the earlier conversion by Timo Heister (Timo-Heister@gmx.de)
' Last modified:  22 June 2002, based on ilut.h dated 06/22/2002
'
' Filename: ilut.bas
'
' Description:  The main include file for ILUT
'
'-----------------------------------------------------------------------------

' I've converted the types as follows.  I have not dared use user-defined types, as I
' don't know how VB handles them internally.  This means, unfortunately, that the headers
' are less informative than the original.  So refer to ilut.h to see the original types.

' ILenum      long
' ILboolean   byte
' ILbitfield  long
' ILbyte      byte
' ILshort     integer
' ILint       long
' ILsizei     long
' ILubyte     byte
' ILushort    integer
' ILuint      long
' ILfloat     single
' ILclampf    single
' ILdouble    double
' ILclampd    double
' ILvoid      n/a
' <all Windows handles>  integer


Option Explicit

Public Const ILUT_VERSION_1_6_0 = 1
Public Const ILUT_VERSION = 160


' Attribute Bits
Public Const ILUT_OPENGL_BIT = &H1
Public Const ILUT_D3D_BIT = &H2
Public Const ILUT_ALL_ATTRIB_BITS = &HFFFFF


' Error Types
Public Const ILUT_INVALID_ENUM = &H501
Public Const ILUT_OUT_OF_MEMORY = &H502
Public Const ILUT_INVALID_VALUE = &H505
Public Const ILUT_ILLEGAL_OPERATION = &H506
Public Const ILUT_INVALID_PARAM = &H509
Public Const ILUT_COULD_NOT_OPEN_FILE = &H50A
Public Const ILUT_STACK_OVERFLOW = &H50E
Public Const ILUT_STACK_UNDERFLOW = &H50F
Public Const ILUT_NOT_SUPPORTED = &H550


' State Definitions
Public Const ILUT_PALETTE_MODE = &H600
Public Const ILUT_OPENGL_CONV = &H610
Public Const ILUT_D3D_MIPLEVELS = &H620
Public Const ILUT_MAXTEX_WIDTH = &H630
Public Const ILUT_MAXTEX_HEIGHT = &H631
Public Const ILUT_MAXTEX_DEPTH = &H632
Public Const ILUT_GL_USE_S3TC = &H634
Public Const ILUT_D3D_USE_DXTC = &H634
Public Const ILUT_GL_GEN_S3TC = &H635
Public Const ILUT_D3D_GEN_DXTC = &H635
Public Const ILUT_S3TC_FORMAT = &H705
Public Const ILUT_DXTC_FORMAT = &H706


' Values
Public Const ILUT_VERSION_NUM = &HDE2


' ImageLib Utility Toolkit Functions
Public Declare Function ilutDisable Lib "ilut" (ByVal Mode As Long) As Byte
Public Declare Function ilutEnable Lib "ilut" (ByVal Mode As Long) As Byte
Public Declare Function ilutGetBoolean Lib "ilut" (ByVal Mode As Long) As Byte
Public Declare Sub ilutGetBooleanv Lib "ilut" (ByVal Mode As Long, ByRef Param As Byte)
Public Declare Function ilutGetInteger Lib "ilut" (ByVal Mode As Long) As Long
Public Declare Sub ilutGetIntegerv Lib "ilut" (ByVal Mode As Long, ByRef Param As Long)
Public Declare Function ilutGetString Lib "ilut" (ByVal StringName As Long) As Long
Public Declare Sub ilutInit Lib "ilut" ()
Public Declare Function ilutIsDisabled Lib "ilut" (ByVal Mode As Long) As Byte
Public Declare Function ilutIsEnabled Lib "ilut" (ByVal Mode As Long) As Byte
Public Declare Sub ilutPopAttrib Lib "ilut" ()
Public Declare Sub ilutPushAttrib Lib "ilut" (ByVal Bits As Long)
Public Declare Sub ilutSetInteger Lib "ilut" (ByVal Mode As Long, ByVal Param As Long)


Public Const ILUT_USE_ALLEGRO = False
Public Const ILUT_USE_WIN32 = True
Public Const ILUT_USE_OPENGL = True


' The different rendering api's...more to be added later?
Public Const ILUT_OPENGL = 0
Public Const ILUT_ALLEGRO = 1
Public Const ILUT_WIN32 = 2


Public Declare Function ilutRenderer Lib "ilut" (ByVal Renderer As Long) As Byte


' ImageLib Utility Toolkit's OpenGL Functions
Public Declare Function ilutGLBindTexImage Lib "ilut" () As Long
Public Declare Function ilutGLBindMipmaps Lib "ilut" () As Long
Public Declare Function ilutGLBuildMipmaps Lib "ilut" () As Byte
Public Declare Function ilutGLLoadImage Lib "ilut" (ByVal FileName As String) As Long
Public Declare Function ilutGLScreen Lib "ilut" () As Byte
Public Declare Function ilutGLScreenie Lib "ilut" () As Byte
Public Declare Function ilutGLSaveImage Lib "ilut" (ByVal FileName As String, ByVal TexID As Long) As Byte
Public Declare Function ilutGLSetTex Lib "ilut" (ByVal TexID As Long) As Byte
Public Declare Function ilutGLTexImage Lib "ilut" (ByVal Level As Long) As Byte


' Allegro and BeOS are probably irrelevant for VB, so they are left out...


' ImageLib Utility Toolkit's Win32 (DirectX/GDI) Functions
Public Declare Function ilutConvertToHBitmap Lib "ilut" (ByVal hDC As Integer) As Integer
' Public Declare Sub ilutGetBmpInfo Lib "ilut" (ByRef Info As BITMAPINFO)
' (The BITMAPINFO structure can probably not be ported to VB).
Public Declare Function ilutGetHPal Lib "ilut" () As Integer
Public Declare Function ilutGetPaddedData Lib "ilut" () As Long
Public Declare Function ilutGetWinClipboard Lib "ilut" () As Byte
Public Declare Function ilutLoadResource Lib "ilut" (ByVal hInst As Integer, ByVal ID As Long, ByVal ResourceType As String, ByVal IType As Long) As Byte
Public Declare Function ilutSetHBitmap Lib "ilut" (ByVal Bitmap As Integer) As Byte
Public Declare Function ilutSetHPal Lib "ilut" (ByVal Pal As Integer) As Byte
Public Declare Function ilutSetWinClipboard Lib "ilut" () As Byte
Public Declare Function ilutWinLoadImage Lib "ilut" (ByVal FileName As String, ByVal hDC As Integer) As Integer
Public Declare Function ilutWinLoadUrl Lib "ilut" (ByVal Url As String) As Byte
Public Declare Function ilutWinSaveImage Lib "ilut" (ByVal FileName As String, ByVal Bitmap As Integer) As Byte

' The DirectX 8 functions below might not be compiled into the DLL, so I've not
' bothered figuring out whether VB can be fooled into handling all those pointers.
'      ILAPI ILvoid  ILAPIENTRY ilutD3D8MipFunc(ILuint NumLevels);
'      ILAPI IDirect3DTexture8* ILAPIENTRY ilutD3D8Texture(IDirect3DDevice8 *Device);
'      ILAPI IDirect3DVolumeTexture8* ILAPIENTRY ilutD3D8VolumeTexture(IDirect3DDevice8 *Device);
'      ILAPI ILboolean ILAPIENTRY ilutD3D8TexFromFile(IDirect3DDevice8 *Device, char *FileName, IDirect3DTexture8 **Texture);
'      ILAPI ILboolean ILAPIENTRY ilutD3D8VolTexFromFile(IDirect3DDevice8 *Device, char *FileName, IDirect3DVolumeTexture8 **Texture);
'      ILAPI ILboolean ILAPIENTRY ilutD3D8TexFromFileInMemory(IDirect3DDevice8 *Device, ILvoid *Lump, ILuint Size, IDirect3DTexture8 **Texture);
'      ILAPI ILboolean ILAPIENTRY ilutD3D8VolTexFromFileInMemory(IDirect3DDevice8 *Device, ILvoid *Lump, ILuint Size, IDirect3DVolumeTexture8 **Texture);
'      ILAPI ILboolean ILAPIENTRY ilutD3D8TexFromFileHandle(IDirect3DDevice8 *Device, ILHANDLE File, IDirect3DTexture8 **Texture);
'      ILAPI ILboolean ILAPIENTRY ilutD3D8VolTexFromFileHandle(IDirect3DDevice8 *Device, ILHANDLE File, IDirect3DVolumeTexture8 **Texture);
'      // These two are not tested yet.
'      ILAPI ILboolean ILAPIENTRY ilutD3D8TexFromResource(IDirect3DDevice8 *Device, HMODULE SrcModule, char *SrcResource, IDirect3DTexture8 **Texture);
'      ILAPI ILboolean ILAPIENTRY ilutD3D8VolTexFromResource(IDirect3DDevice8 *Device, HMODULE SrcModule, char *SrcResource, IDirect3DVolumeTexture8 **Texture);
'
'      ILAPI ILboolean ILAPIENTRY ilutD3D8LoadSurface(IDirect3DDevice8 *Device, IDirect3DSurface8 *Surface);
