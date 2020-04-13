unit OpenILUT;

//------------------------------------------------------------------------------
//
// ImageLib Utility Toolkit Sources
// Copyright (C) 2000-2002 by Denton Woods
// Last modified: 06/23/2002 <--Y2K Compliant! =]
//
// Filename: il/ilut.h
//
// Description: The main include file for ILUT
//
//------------------------------------------------------------------------------

{******************************************************************************}
{ Converted to Delphi by Alexander Blach (alexander@abee.de)                   }
{   Version:       ILUT v1.6.1                                                 }
{   Last modified: 06/23/2002                                                  }
{                                                                              }
{   You will need the latest OpenGL units for Delphi. I recommend those on     }
{   http://www.delphi3d.net                                                    }
{******************************************************************************}


interface

// Note by ABee: I had to move this here so that I can look it up in "uses"
//{$DEFINE ILUT_USE_ALLEGRO)
{$DEFINE ILUT_USE_WIN32}
{$DEFINE ILUT_USE_OPENGL}

uses
  OpenIL,
  OpenILU,
{$IFDEF ILUT_USE_OPENGL}
  GL,
  //OpenGL12,
{$ENDIF}
  Windows;


const
  ILUT_VERSION_1_5_6 = 1;
  ILUT_VERSION = 156;

const
// Attribute Bits
  ILUT_OPENGL_BIT = $00000001;
  ILUT_D3D_BIT = $00000002;
  ILUT_ALL_ATTRIB_BITS = $000FFFFF;

// Error Types
  ILUT_INVALID_ENUM = $0501;
  ILUT_OUT_OF_MEMORY = $0502;
  ILUT_INVALID_VALUE = $0505;
  ILUT_ILLEGAL_OPERATION = $0506;
  ILUT_INVALID_PARAM = $0509;
  ILUT_COULD_NOT_OPEN_FILE = $050A;
  ILUT_STACK_OVERFLOW = $050E;
  ILUT_STACK_UNDERFLOW = $050F;
  ILUT_NOT_SUPPORTED = $0550;


// State Definitions
  ILUT_PALETTE_MODE = $0600;
  ILUT_OPENGL_CONV = $0610;
  ILUT_D3D_MIPLEVELS = $0620;
  ILUT_MAXTEX_WIDTH = $0630;
  ILUT_MAXTEX_HEIGHT = $0631;
  ILUT_MAXTEX_DEPTH = $0632;
  ILUT_GL_USE_S3TC = $0634;
  ILUT_D3D_USE_DXTC = $0634;
  ILUT_GL_GEN_S3TC = $0635;
  ILUT_D3D_GEN_DXTC = $0635;
  ILUT_S3TC_FORMAT = $0705;
  ILUT_DXTC_FORMAT = $0706;



// Values
  ILUT_VERSION_NUM = $0DE2;


const
{$IFDEF OPENIL_DEBUG}
  ILUTDLL = 'ILUT-d.DLL';
{$ELSE}
  ILUTDLL = 'ILUT.DLL';
{$ENDIF}

// ImageLib Utility Toolkit Functions
function ilutDisable(Mode: TILenum): TILboolean; stdcall; external ILUTDLL;
function ilutEnable(Mode: TILenum): TILboolean; stdcall; external ILUTDLL;
function ilutGetBoolean(Mode: TILenum): TILboolean; stdcall; external
  ILUTDLL;
procedure ilutGetBooleanv(Mode: TILenum; Param: PILboolean); stdcall; external
  ILUTDLL;
function ilutGetInteger(Mode: TILenum): TILint; stdcall; external
  ILUTDLL;
procedure ilutGetIntegerv(Mode: TILenum; Param: PILint); stdcall; external
  ILUTDLL;
function ilutGetString(StringName: TILenum): PChar; stdcall; external ILUTDLL;
procedure ilutInit; stdcall; external ILUTDLL;
function ilutIsDisabled(Mode: TILenum): TILboolean; stdcall; external ILUTDLL;
function ilutIsEnabled(Mode: TILenum): TILboolean; stdcall; external ILUTDLL;
procedure ilutPopAttrib; stdcall; external ILUTDLL;
procedure ilutPushAttrib(Bits: TILuint); stdcall; external ILUTDLL;
procedure ilutSetInteger(Mode: TILenum; Param: TILint); stdcall; external ILUTDLL;


// The different rendering api's...more to be added later?
const
  IL_OPENGL = 0;
  IL_ALLEGRO = 1;
  IL_WIN32 = 2;

function ilutRenderer(Renderer: TILenum): TILboolean; stdcall; external ILUTDLL;

// ImageLib Utility Toolkit's OpenGL Functions
{$IFDEF ILUT_USE_OPENGL}
function ilutGLBindTexImage: GLuint; stdcall; external ILUTDLL;
function ilutGLBindMipmaps: GLuint; stdcall; external ILUTDLL;
function ilutGLBuildMipmaps: TILboolean; stdcall; external ILUTDLL;
function ilutGLLoadImage(FileName: PChar): GLuint; stdcall; external ILUTDLL;
function ilutGLScreen: TILboolean; stdcall; external ILUTDLL;
function ilutGLScreenie: TILboolean; stdcall; external ILUTDLL;
function ilutGLSaveImage(FileName: PChar; TexID: GLuint): TILboolean;
 stdcall; external ILUTDLL;
function ilutGLSetTex(TexID: GLuint): TILboolean; stdcall; external ILUTDLL;
function ilutGLTexImage(Level: GLuint): TILboolean; stdcall; external ILUTDLL;
{$ENDIF}

// note by ABee: no Allegro in Delphi
// ImageLib Utility ToolKit's Allegro Functions
{$IFDEF ILUT_USE_ALLEGRO}
//function ilutConvertToAlleg(var Pal: PALETTE): PBitmap; stdcall; external
//ILUTDLL;
{$ENDIF}

// ImageLib Utility Toolkit's Win32 (DirectX/GDI) Functions
{$IFDEF ILUT_USE_WIN32}
function ilutConvertToHBitmap(DC: HDC): HBITMAP; stdcall; external ILUTDLL;
procedure ilutGetBmpInfo(Info: PBitmapInfo); stdcall; external ILUTDLL;
function ilutGetHPal: HPalette; stdcall; external ILUTDLL;
function ilutGetPaddedData: PILubyte; stdcall; external ILUTDLL;
function ilutGetWinClipboard: TILboolean; stdcall; external ILUTDLL;
function ilutLoadResource(hInst: LongWord; ID: TILint; ResourceType: PChar;
  _Type: TILenum): TILboolean; stdcall; external ILUTDLL;
function ilutSetHBitmap(Bitmap: HBITMAP): TILboolean; stdcall; external ILUTDLL;
function ilutSetHPal(Pal: HPALETTE): TILboolean; stdcall; external ILUTDLL;
function ilutSetWinClipboard: TILboolean; stdcall; external ILUTDLL;
function ilutWinLoadImage(FileName: PChar; DC: HDC): HBITMAP; stdcall;
 external ILUTDLL;
function ilutWinLoadUrl(Url: PChar): TILboolean; stdcall; external ILUTDLL;
function ilutWinSaveImage(FileName: PChar; Bitmap: HBITMAP): TILboolean;
 stdcall; external ILUTDLL;
{
		//#ifdef ILUT_USE_DIRECTX7
		//	LPDIRECTDRAWSURFACE7 ILAPIENTRY ilutDX7Surface(char *FileName, LPDIRECTDRAW7 DDraw);
		//#endif//ILUT_USE_DIRECTX7
}
{$ENDIF}

implementation

end.

