unit OpenIL;

//------------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2003 by Denton Woods
// Last modified: 06/23/2002 <--Y2K Compliant! =]
//
// Filename: il/il.h
//
// Description:  The main include file for DevIL
//
//------------------------------------------------------------------------------

{******************************************************************************}
{ Converted to Delphi by Alexander Blach (alexander@abee.de)                   }
{   Version:       DevIL v1.6.6                                                }
{   Last modified: 08/08/2003                                                  }
{******************************************************************************}

interface
uses
  Windows;

//{$DEFINE IL_NO_BMP}
//{$DEFINE IL_NO_CUT}
//{$DEFINE IL_NO_CHEAD}
//{$DEFINE IL_NO_DCX}
//{$DEFINE IL_NO_DDS}
//{$DEFINE IL_NO_DOOM}
//{$DEFINE IL_NO_GIF}
//{$DEFINE IL_NO_ICO}
//{$DEFINE IL_NO_JPG}
//{$DEFINE IL_NO_LIF}
//{$DEFINE IL_NO_LBM}
//{$DEFINE IL_NO_MDL}
//{$DEFINE IL_NO_PCD}
//{$DEFINE IL_NO_PCX}
//{$DEFINE IL_NO_PIC}
//{$DEFINE IL_NO_PNG}
//{$DEFINE IL_NO_PNM}
//{$DEFINE IL_NO_PSD}
//{$DEFINE IL_NO_RAW}
//{$DEFINE IL_NO_SGI}
//{$DEFINE IL_NO_TGA}
//{$DEFINE IL_NO_TIF}
//{$DEFINE IL_NO_WAL}

//{$DEFINE IL_USE_IJL}

type
  TILenum = LongWord;
  TILboolean = Byte;
  TILbitfield = LongWord;
  TILbyte = ShortInt;
  TILshort = SmallInt;
  TILint = Integer;
  TILsizei = Integer;
  TILubyte = Byte;
  TILushort = Word;
  TILuint = LongWord;
  TILfloat = Single;
  TILclampf = Single;
  TILdouble = Double;
  TILclampd = Double;

  PILboolean = ^Byte;
  PILbitfield = ^LongWord;
  PILbyte = ^ShortInt;
  PILshort = ^SmallInt;
  PILint = ^Integer;
  PILsizei = ^Integer;
  PILubyte = ^Byte;
  PILushort = ^Word;
  PILuint = ^LongWord;
  PILfloat = ^Single;
  PILclampf = ^Single;
  PILdouble = ^Double;
  PILclampd = ^Double;

const
  IL_FALSE = 0;
  IL_TRUE = 1;


  // Matches OpenGL's right now.
  IL_COLOUR_INDEX = $1900;
  IL_COLOR_INDEX = $1900;
  IL_RGB = $1907;
  IL_RGBA = $1908;
  IL_BGR = $80E0;
  IL_BGRA = $80E1;
  IL_LUMINANCE = $1909;


  IL_BYTE = $1400;
  IL_UNSIGNED_BYTE = $1401;
  IL_SHORT = $1402;
  IL_UNSIGNED_SHORT = $1403;
  IL_INT = $1404;
  IL_UNSIGNED_INT = $1405;
  IL_FLOAT = $1406;
  IL_DOUBLE = $140A;


  IL_VENDOR = $1F00;


//
// IL-specific 's
//

  IL_VERSION_1_5_6 = 1;
  IL_VERSION = 156;
  IL_LOAD_EXT = $1F01;
  IL_SAVE_EXT = $1F02;



// Attribute Bits
  IL_ORIGIN_BIT = $00000001;
  IL_FILE_BIT = $00000002;
  IL_PAL_BIT = $00000004;
  IL_FORMAT_BIT = $00000008;
  IL_TYPE_BIT = $00000010;
  IL_COMPRESS_BIT = $00000020;
  IL_LOADFAIL_BIT = $00000040;
  IL_FORMAT_SPECIFIC_BIT = $00000080;
  IL_ALL_ATTRIB_BITS = $000FFFFF;


// Palette types
  IL_PAL_NONE = $0400;
  IL_PAL_RGB24 = $0401;
  IL_PAL_RGB32 = $0402;
  IL_PAL_RGBA32 = $0403;
  IL_PAL_BGR24 = $0404;
  IL_PAL_BGR32 = $0405;
  IL_PAL_BGRA32 = $0406;


// Image types
  IL_TYPE_UNKNOWN = $0000;
  IL_BMP = $0420;
  IL_CUT = $0421;
  IL_DOOM = $0422;
  IL_DOOM_FLAT = $0423;
  IL_ICO = $0424;
  IL_JPG = $0424;
  IL_LBM = $0426;
  IL_PCD = $0427;
  IL_PCX = $0428;
  IL_PIC = $0429;
  IL_PNG = $042A;
  IL_PNM = $042B;
  IL_SGI = $042C;
  IL_TGA = $042D;
  IL_TIF = $042E;
  IL_CHEAD = $042F;
  IL_RAW = $0430;
  IL_MDL = $0431;
  IL_WAL = $0432;
  IL_LIF = $0434;
  IL_MNG = $0435;
  IL_JNG = $0435;
  IL_GIF = $0436;
  IL_DDS = $0437;
  IL_DCX = $0438;
  IL_PSD = $0439;
  IL_EXIF = $043A;
  IL_PSP = $043B;
  IL_PIX = $043C;
  IL_PXR = $043D;
  IL_XPM = $043E;

  IL_JASC_PAL = $0475;


// Error Types
  IL_NO_ERROR = $0000;
  IL_INVALID_ENUM = $0501;
  IL_OUT_OF_MEMORY = $0502;
  IL_FORMAT_NOT_SUPPORTED = $0503;
  IL_INTERNAL_ERROR = $0504;
  IL_INVALID_VALUE = $0505;
  IL_ILLEGAL_OPERATION = $0506;
  IL_ILLEGAL_FILE_VALUE = $0507;
  IL_INVALID_FILE_HEADER = $0508;
  IL_INVALID_PARAM = $0509;
  IL_COULD_NOT_OPEN_FILE = $050A;
  IL_INVALID_EXTENSION = $050B;
  IL_FILE_ALREADY_EXISTS = $050C;
  IL_OUT_FORMAT_SAME = $050D;
  IL_STACK_OVERFLOW = $050E;
  IL_STACK_UNDERFLOW = $050F;
  IL_INVALID_CONVERSION = $0510;
  IL_BAD_DIMENSIONS = $0511;
  IL_FILE_READ_ERROR = $0512;

  IL_LIB_GIF_ERROR = $05E1;
  IL_LIB_JPEG_ERROR = $05E2;
  IL_LIB_PNG_ERROR = $05E3;
  IL_LIB_TIFF_ERROR = $05E4;
  IL_LIB_MNG_ERROR = $05E5;
  IL_UNKNOWN_ERROR = $05FF;


// Origin Definitions
  IL_ORIGIN_SET = $0600;
  IL_ORIGIN_LOWER_LEFT = $0601;
  IL_ORIGIN_UPPER_LEFT = $0602;
  IL_ORIGIN_MODE = $0603;


// Format and Type Mode Definitions
  IL_FORMAT_SET = $0610;
  IL_FORMAT_MODE = $0611;
  IL_TYPE_SET = $0612;
  IL_TYPE_MODE = $0613;


// File definitions
  IL_FILE_OVERWRITE = $0620;
  IL_FILE_MODE = $0621;


// Palette definitions
  IL_CONV_PAL = $0630;


// Load fail definitions
  IL_DEFAULT_ON_FAIL = $0632;


// Key colour definitions
  IL_USE_KEY_COLOUR = $0635;
  IL_USE_KEY_COLOR = $0635;


// Interlace definitions
  IL_SAVE_INTERLACED = $0639;
  IL_INTERLACE_MODE = $063A;


// Quantization definitions
  IL_QUANTIZATION_MODE = $0640;
  IL_WU_QUANT = $0641;
  IL_NEU_QUANT = $0642;
  IL_NEU_QUANT_SAMPLE = $0643;


// Hints
  IL_FASTEST = $0660;
  IL_LESS_MEM = $0661;
  IL_DONT_CARE = $0662;
  IL_MEM_SPEED_HINT = $0665;
  IL_USE_COMPRESSION = $0666;
  IL_NO_COMPRESSION = $0667;
  IL_COMPRESSION_HINT = $0668;


// Subimage
  IL_SUB_NEXT = $0680;
  IL_SUB_MIPMAP = $0681;
  IL_SUB_LAYER = $0682;


// Compression definitions (mostly for .oil)
  IL_COMPRESS_MODE = $0700;
  IL_COMPRESS_NONE = $0701;
  IL_COMPRESS_RLE = $0702;
  IL_COMPRESS_LZO = $0703;
  IL_COMPRESS_ZLIB = $0704;


// File format-specific values
  IL_TGA_CREATE_STAMP = $0710;
  IL_JPG_QUALITY = $0711;
  IL_PNG_INTERLACE = $0712;
  IL_TGA_RLE = $0713;
  IL_BMP_RLE = $0714;
  IL_SGI_RLE = $0715;
  IL_TGA_ID_STRING = $0717;
  IL_TGA_AUTHNAME_STRING = $0718;
  IL_TGA_AUTHCOMMENT_STRING = $0719;
  IL_PNG_AUTHNAME_STRING = $071A;
  IL_PNG_TITLE_STRING = $071B;
  IL_PNG_DESCRIPTION_STRING = $071C;
  IL_TIF_DESCRIPTION_STRING = $071D;
  IL_TIF_HOSTCOMPUTER_STRING = $071E;
  IL_TIF_DOCUMENTNAME_STRING = $071F;
  IL_TIF_AUTHNAME_STRING = $0720;
  IL_JPG_SAVE_FORMAT = $0721;
  IL_CHEAD_HEADER_STRING = $0722;
  IL_PCD_PICNUM = $0723;


// DXTC definitions
  IL_DXTC_FORMAT = $0705;
  IL_DXT1 = $0706;
  IL_DXT2 = $0707;
  IL_DXT3 = $0708;
  IL_DXT4 = $0709;
  IL_DXT5 = $070A;
  IL_DXT_NO_COMP = $070B;
  IL_KEEP_DXTC_DATA = $070C;
  IL_DXTC_DATA_FORMAT = $070D;


// Cube map definitions
  IL_CUBEMAP_POSITIVEX = $00000400;
  IL_CUBEMAP_NEGATIVEX = $00000800;
  IL_CUBEMAP_POSITIVEY = $00001000;
  IL_CUBEMAP_NEGATIVEY = $00002000;
  IL_CUBEMAP_POSITIVEZ = $00004000;
  IL_CUBEMAP_NEGATIVEZ = $00008000;


// Values
  IL_VERSION_NUM = $0DE2;
  IL_IMAGE_WIDTH = $0DE4;
  IL_IMAGE_HEIGHT = $0DE5;
  IL_IMAGE_DEPTH = $0DE6;
  IL_IMAGE_SIZE_OF_DATA = $0DE7;
  IL_IMAGE_BPP = $0DE8;
  IL_IMAGE_BYTES_PER_PIXEL = $0DE8;
  IL_IMAGE_BITS_PER_PIXEL = $0DE9;
  IL_IMAGE_FORMAT = $0DEA;
  IL_IMAGE_TYPE = $0DEB;
  IL_PALETTE_TYPE = $0DEC;
  IL_PALETTE_SIZE = $0DED;
  IL_PALETTE_BPP = $0DEE;
  IL_PALETTE_NUM_COLS = $0DEF;
  IL_PALETTE_BASE_TYPE = $0DF0;
  IL_NUM_IMAGES = $0DF1;
  IL_NUM_MIPMAPS = $0DF2;
  IL_NUM_LAYERS = $0DF3;
  IL_ACTIVE_IMAGE = $0DF4;
  IL_ACTIVE_MIPMAP = $0DF5;
  IL_ACTIVE_LAYER = $0DF6;
  IL_CUR_IMAGE = $0DF7;
  IL_IMAGE_DURATION = $0DF8;
  IL_IMAGE_PLANESIZE = $0DF9;
  IL_IMAGE_BPC = $0DFA;
  IL_IMAGE_OFFX = $0DFB;
  IL_IMAGE_OFFY = $0DFC;
  IL_IMAGE_CUBEFLAGS = $0DFD;


const
{$IFDEF OPENIL_DEBUG}
  OPENILDLL = 'DevIL-d.DLL';
{$ELSE}
  OPENILDLL = 'DevIL.DLL';
{$ENDIF}

  IL_SEEK_SET = 0;
  IL_SEEK_CUR = 1;
  IL_SEEK_END = 2;
  IL_EOF = -1;

// Callback functions for file reading
type
  TILHandle = Pointer;
// Callback functions for file reading
  TfCloseRProc = procedure(Handle: TILHandle); stdcall;
  TfEofProc   = function(Handle: TILHandle): TILboolean; stdcall;
  TfGetcProc  = function(Handle: TILHandle): TILint; stdcall;
  TfOpenRProc  = function(FileName: PChar): TILHandle; stdcall;
  TfReadProc  = function(Buffer: Pointer; Size, Count: TILint;
    Handle: TILHandle): TILint; stdcall;
  TfSeekRProc  = function(Handle: TILHandle; Offset, Origin: TILint): TILint;
    stdcall;
  TfTellRProc  = function(Handle: TILHandle): TILint; stdcall;

// Callback function for file writing
  TfCloseWProc = procedure(Handle: TILHandle); stdcall;
  TfOpenWProc  = function(FileName: PChar): TILHandle; stdcall;
  TfPutcProc   = function(c: TILubyte; Handle: TILHandle): TILint; stdcall;
  TfSeekWProc  = function(Handle: TILHandle; Offset, Origin: TILint): TILint;
    stdcall;
  TfTellWProc  = function(Handle: TILHandle): TILint; stdcall;
  TfWriteProc  = function(Buffer: Pointer; Size: TILuint; Count: TILuint;
    Handle: TILHandle): TILint; stdcall;

// Registered format procedures
  TIL_LOADPROC = function(FileName: PChar): TILboolean; stdcall;
  TIL_SAVEPROC = function(FileName: PChar): TILboolean; stdcall;


// ImageLib Functions
function ilActiveImage(Number: TILuint): TILboolean; stdcall;
  external OPENILDLL;
function ilActiveLayer(Number: TILuint): TILboolean; stdcall;
  external OPENILDLL;
function ilActiveMipmap(Number: TILuint): TILboolean; stdcall;
  external OPENILDLL;
function ilApplyPal(FileName: PChar): TILboolean; stdcall; external OPENILDLL;
function ilApplyProfile(InProfile, OutProfile: PChar): TILboolean; stdcall; external OPENILDLL;
procedure ilBindImage(Image: TILuint); stdcall; external
  OPENILDLL;
function ilBlit(Src: TILuint; DestX, DestY, DestZ: TILint; SrcX, SrcY, SrcZ, Width, Height, Depth: TILuint): TILboolean; stdcall; external OPENILDLL;
procedure ilClearColour(Red, Green, Blue, Alpha: TILclampf); stdcall; external OPENILDLL;
procedure ilClearColor(Red, Green, Blue, Alpha: TILclampf); stdcall; external OPENILDLL name 'ilClearColour';
function ilClearImage: TILboolean; stdcall; external OPENILDLL;
function ilCloneCurImage: TILuint; stdcall; external OPENILDLL;
function ilCompressFunc(Mode: TILenum): TILboolean; stdcall; external OPENILDLL;
function ilConvertImage(DestFormat, DestType: TILenum): TILboolean; stdcall; external
  OPENILDLL;
function ilConvertPal(DestFormat: TILenum): TILboolean; stdcall; external
  OPENILDLL;
function ilCopyImage(Src: TILuint): TILboolean; stdcall; external OPENILDLL;
procedure ilCopyPixels(XOff, YOff, ZOff, Width, Height, Depth: TILuint;
  Format, _Type: TILenum; Data: Pointer); stdcall; external OPENILDLL;
procedure ilCreateSubImage(SubType: TILenum; Num: TILuint); stdcall; external OPENILDLL;
procedure ilDefaultImage; stdcall; external OPENILDLL;
procedure ilDeleteImages(Num: TILsizei; const Images: PILuint); stdcall;
  external OPENILDLL;
function ilDisable(Mode: TILenum): TILboolean; stdcall; external OPENILDLL;
function ilEnable(Mode: TILenum): TILboolean; stdcall; external OPENILDLL;
function ilFormatFunc(Mode: TILenum): TILboolean; stdcall; external OPENILDLL;
procedure ilGenImages(Num: TILsizei; Images: PILuint); stdcall; external
  OPENILDLL;
function ilGetAlpha(_Type: TILenum): PILubyte; stdcall; external OPENILDLL;
function ilGetBoolean(Mode: TILenum): TILboolean; stdcall; external OPENILDLL;
procedure ilGetBooleanv(Mode: TILenum; Param: PILboolean); stdcall; external
  OPENILDLL;
function ilGetData: PILubyte; stdcall; external OPENILDLL;
function ilGetDXTCData(Buffer: PILubyte; BufferSize: TILuint; DXTCFormat: TILenum):
  TILuint; stdcall; external OPENILDLL;
function ilGetError: TILenum; stdcall; external OPENILDLL;
function ilGetInteger(Mode: TILenum): TILint; stdcall; external OPENILDLL;
procedure ilGetIntegerv(Mode: TILenum; Param: PILint); stdcall; external
  OPENILDLL;
function ilGetLumpPos: TILuint; stdcall; external OPENILDLL;
function ilGetPalette: PILubyte; stdcall; external OPENILDLL;
function ilGetString(StringName: TILenum): PChar; stdcall; external OPENILDLL;
procedure ilHint(Target, Mode: TILenum); stdcall; external OPENILDLL;
procedure ilInit; stdcall; external OPENILDLL;
function ilIsDisabled(Mode: TILenum): TILboolean; stdcall; external OPENILDLL;
function ilIsEnabled(Mode: TILenum): TILboolean; stdcall; external OPENILDLL;
function ilIsImage(Image: TILuint): TILboolean; stdcall; external OPENILDLL;
function ilIsValid(_Type: TILenum; FileName: PChar): TILboolean; stdcall;
  external OPENILDLL;
function ilIsValidF(_Type: TILenum; _File: TILHandle): TILboolean; stdcall;
  external OPENILDLL;
function ilIsValidL(_Type: TILenum; Lump: Pointer; Size: TILuint): TILboolean;
  stdcall; external OPENILDLL;
procedure ilKeyColor(Red: TILubyte; Green: TILubyte; Blue: TILubyte; PalNum: TILubyte);
  stdcall; external OPENILDLL name 'ilKeyColour';
procedure ilKeyColour(Red: TILubyte; Green: TILubyte; Blue: TILubyte; PalNum: TILubyte);
  stdcall; external OPENILDLL;
function ilLoad(_Type: TILenum; FileName: PChar): TILboolean; stdcall;
  external OPENILDLL;
function ilLoadF(_Type: TILenum; _File: TILHandle): TILboolean; stdcall;
  external OPENILDLL;
function ilLoadImage(FileName: PChar): TILboolean; stdcall; external OPENILDLL;
function ilLoadL(_Type: TILenum; Lump: Pointer; Size: TILuint): TILboolean; stdcall;
  external OPENILDLL;
function ilLoadPal(FileName: PChar): TILboolean; stdcall; external OPENILDLL;
function ilOriginFunc(Mode: TILenum): TILboolean; stdcall; external OPENILDLL;
function ilOverlayImage(Src: TILuint; XCoord, YCoord, ZCoord: TILint):
  TILboolean; stdcall; external OPENILDLL;
procedure ilPopAttrib; stdcall; external OPENILDLL;
procedure ilPushAttrib(Bits: TILuint); stdcall; external OPENILDLL;
procedure ilRegisterFormat(Format: TILenum); stdcall; external OPENILDLL;
function ilRegisterLoad(Ext: PChar; Load: TIL_LOADPROC): TILboolean; stdcall;
  external OPENILDLL;
function ilRegisterMipNum(Num: TILuint): TILboolean; stdcall; external OPENILDLL;
function ilRegisterNumImages(Num: TILuint): TILboolean;  stdcall;
  external OPENILDLL;
procedure ilRegisterOrigin(Origin: TILenum); stdcall; external OPENILDLL;
procedure ilRegisterPal(Pal: Pointer; Size: TILuint; _Type: TILenum); stdcall;
  external OPENILDLL;
function ilRegisterSave(Ext: PChar; Save: TIL_SAVEPROC): TILboolean; stdcall;
  external OPENILDLL;
procedure ilRegisterType(_Type: TILenum); stdcall; external OPENILDLL;
function ilRemoveLoad(Ext: PChar): TILboolean; stdcall; external OPENILDLL;
function ilRemoveSave(Ext: PChar): TILboolean; stdcall; external OPENILDLL;
procedure ilResetRead; stdcall; external OPENILDLL;
procedure ilResetWrite; stdcall; external OPENILDLL;
function ilSave(_Type: TILenum; FileName: PChar): TILboolean; stdcall;
  external OPENILDLL;
function ilSaveF(_Type: TILenum; _File: TILHandle): TILboolean; stdcall;
  external OPENILDLL;
function ilSaveImage(FileName: PChar): TILboolean; stdcall; external OPENILDLL;
function ilSaveL(_Type: TILenum; Lump: Pointer; Size: TILuint): TILboolean;
  stdcall; external OPENILDLL;
function ilSavePal(FileName: PChar): TILboolean; stdcall; external
  OPENILDLL;
function ilSetData(Data: Pointer): TILboolean; stdcall; external OPENILDLL;
function ilSetDuration(Duration: TILuint): TILboolean; stdcall;
  external OPENILDLL;
procedure ilSetInteger(Mode: TILenum; Param: TILint); stdcall; external OPENILDLL;
procedure ilSetPixels(XOff, YOff, ZOff, Width, Height, Depth: TILuint;
  Format, _Type: TILenum; Data: Pointer); stdcall; external OPENILDLL;
procedure ilSetRead(Open: TfOpenRProc; Close: TfCloseRProc;
  Eof: TfEofProc; Getc: TfGetcProc; Read: TfReadProc; Seek: TfSeekRProc;
  Tell: TfTellRProc); stdcall; external OPENILDLL;
procedure ilSetString(Mode: TILenum; Param: PChar); stdcall; external OPENILDLL; 
procedure ilSetWrite(Open: TfOpenWProc; Close: TfCloseWProc;
  Putc: TfPutcProc; Seek: TfSeekWProc;
  Tell: TfTellWProc; Write: TfWriteProc); stdcall; external OPENILDLL;
function ilTexImage(Width, Height, Depth: TILuint; Bpp: TILubyte; Format, _Type:
  TILenum; Data: Pointer): TILboolean; stdcall; external OPENILDLL;
function ilTypeFunc(Mode: TILenum): TILboolean; stdcall; external OPENILDLL;
function ilLoadData(FileName: PChar; Width, Height, Depth: TILuint;
  Bpp: TILubyte): TILboolean; stdcall; external OPENILDLL;
function ilLoadDataF(_File: TILHandle; Width, Height, Depth: TILuint;
  Bpp: TILubyte): TILboolean; stdcall; external OPENILDLL;
function ilLoadDataL(Lump: Pointer; Size: TILuint; Width, Height, Depth: TILuint;
  Bpp: TILubyte): TILboolean; stdcall; external OPENILDLL;
function ilSaveData(FileName: PChar): TILboolean; stdcall; external OPENILDLL;

implementation

end.

