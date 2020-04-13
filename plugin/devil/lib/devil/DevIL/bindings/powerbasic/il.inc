'-----------------------------------------------------------------------------
'
' ImageLib Sources
' Copyright (C) 2000-2002 by Denton Woods
' Last modified: 06/22/2002 <--Y2K Compliant! =]
'
' Filename: il/il.h
'
' Description: The main include file for DevIL
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
' Filename: il.inc
'
' Description: PB main include file for DevIL
'
' For information on use and download of DevIL goto: http://openil.sourceforge.net/
' (Get docs and "End User Package")
'
' Report errors in the Powerbasic includes to e-mail above.
'
' For general help on Powerbasic and DevIL, try the forums at:
' http://www.powerbasic.com
'-----------------------------------------------------------------------------

' #ifndef __il_h_
' #ifndef __IL_H__

' %__il_h_
' %__IL_H__

' #ifdef __cplusplus
' extern "C" {
' #endif

'%IL_NO_BMP
'%IL_NO_CUT
'%IL_NO_CHEAD
'%IL_NO_DDS
'%IL_NO_DOOM
'%IL_NO_ICO
'%IL_NO_JPG
'%IL_NO_LIF
'%IL_NO_LBM
'%IL_NO_MDL
'%IL_NO_PCD
'%IL_NO_PCX
'%IL_NO_PIC
'%IL_NO_PNG
'%IL_NO_PNM
'%IL_NO_RAW
'%IL_NO_SGI
'%IL_NO_TGA
'%IL_NO_TIF
'%IL_NO_WAL
'%IL_NO_DDS
'%IL_NO_DCX
'%IL_NO_PSD
'%IL_NO_PSP
'%IL_NO_PXR
'%IL_NO_RAW
'%IL_NO_SGI
'%IL_NO_TGA
'%IL_NO_TIF
'%IL_NO_WAL
'%IL_NO_XPM

'%IL_USE_IJL ' Whether we use the Intel Jpeg Library (else use libjpeg).


' #ifdef _WIN32
'%WIN32_LEAN_AND_MEAN ' Exclude rarely-used stuff from Windows headers
'#include <windows.h>
'  #ifdef _MSC_VER
'  #ifndef _IL_BUILD_LIBRARY
'  #ifdef _DEBUG
'  #pragma comment(lib, "devil-d.lib")
'  #else
'  #pragma comment(lib, "devil.lib")
'  #endif'_DEBUG
'  #endif'_IL_BUILD_LIBRARY
'  #endif'_MSC_VER
' #endif'_WIN32

' #include <stdio.h>

' typedef unsigned int ILenum;
' typedef unsigned char ILboolean;
' typedef unsigned int ILbitfield;
' typedef char ILbyte;
' typedef short ILshort;
' typedef int ILint;
' typedef int ILsizei;
' typedef unsigned char ILubyte;
' typedef unsigned short ILushort;
' typedef unsigned int ILuint;
' typedef float ILfloat;
' typedef float ILclampf;
' typedef double ILdouble;
' typedef double ILclampd;
' typedef void ILvoid;


%IL_FALSE = 0
%IL_TRUE = 1


' Matches OpenGL's right now.
%IL_COLOUR_INDEX        = &H1900&
%IL_COLOR_INDEX         = &H1900&
%IL_RGB         = &H1907&
%IL_RGBA        = &H1908&
%IL_BGR         = &H80E0&
%IL_BGRA        = &H80E1&
%IL_LUMINANCE       = &H1909&


%IL_BYTE        = &H1400&
%IL_UNSIGNED_BYTE       = &H1401&
%IL_SHORT       = &H1402&
%IL_UNSIGNED_SHORT      = &H1403&
%IL_INT         = &H1404&
%IL_UNSIGNED_INT        = &H1405&
%IL_FLOAT       = &H1406&
%GL_DOUBLE      = &H140A&


%IL_VENDOR      = &H1F00&


'
' IL-specific #define's
'

%IL_VERSION_1_5_6 = 1
%IL_VERSION = 156
%IL_LOAD_EXT = &H1F01&
%IL_SAVE_EXT = &H1F02&


' Attribute Bits
%IL_ORIGIN_BIT      = &H00000001&
%IL_FILE_BIT        = &H00000002&
%IL_PAL_BIT         = &H00000004&
%IL_FORMAT_BIT      = &H00000008&
%IL_TYPE_BIT        = &H00000010&
%IL_COMPRESS_BIT    = &H00000020&
%IL_LOADFAIL_BIT    = &H00000040&
%IL_ALL_ATTRIB_BITS = &H000FFFFF&


' Palette types
%IL_PAL_NONE        = &H0400&
%IL_PAL_RGB24       = &H0401&
%IL_PAL_RGB32       = &H0402&
%IL_PAL_RGBA32      = &H0403&
%IL_PAL_BGR24       = &H0404&
%IL_PAL_BGR32       = &H0405&
%IL_PAL_BGRA32      = &H0406&


' Image types
%IL_TYPE_UNKNOWN	= &H0000&
%IL_BMP         = &H0420&
%IL_CUT         = &H0421&
%IL_DOOM        = &H0422&
%IL_DOOM_FLAT   = &H0423&
%IL_ICO         = &H0424&
%IL_JPG         = &H0425&
%IL_LBM         = &H0426&
%IL_PCD         = &H0427&
%IL_PCX         = &H0428&
%IL_PIC         = &H0429&
%IL_PNG         = &H042A&
%IL_PNM         = &H042B&
%IL_SGI         = &H042C&
%IL_TGA         = &H042D&
%IL_TIF         = &H042E&
%IL_CHEAD       = &H042F&
%IL_RAW         = &H0430&
%IL_MDL         = &H0431&
%IL_WAL         = &H0432&
%IL_LIF         = &H0434&
%IL_MNG         = &H0435&
%IL_JNG         = &H0435&
%IL_GIF         = &H0436&
%IL_DDS         = &H0437&
%IL_DCX			= &H0438&
%IL_PSD			= &H0439&
%IL_EXIF		= &H043A&
%IL_PSP			= &H043B&
%IL_PIX			= &H043C&
%IL_PXR			= &H043D&
%IL_XPM			= &H043E&

%IL_JASC_PAL	= &H0475&


' Error Types
%IL_NO_ERROR				= &H0000&
%IL_INVALID_ENUM			= &H0501&
%IL_OUT_OF_MEMORY			= &H0502&
%IL_FORMAT_NOT_SUPPORTED	= &H0503&
%IL_INTERNAL_ERROR			= &H0504&
%IL_INVALID_VALUE			= &H0505&
%IL_ILLEGAL_OPERATION		= &H0506&
%IL_ILLEGAL_FILE_VALUE		= &H0507&
%IL_INVALID_FILE_HEADER		= &H0508&
%IL_INVALID_PARAM			= &H0509&
%IL_COULD_NOT_OPEN_FILE		= &H050A&
%IL_INVALID_EXTENSION		= &H050B&
%IL_FILE_ALREADY_EXISTS		= &H050C&
%IL_OUT_FORMAT_SAME			= &H050D&
%IL_STACK_OVERFLOW			= &H050E&
%IL_STACK_UNDERFLOW			= &H050F&
%IL_INVALID_CONVERSION		= &H0510&
%IL_BAD_DIMENSIONS			= &H0511&
%IL_FILE_READ_ERROR			= &H0512&

%IL_LIB_GIF_ERROR			= &H05E1&
%IL_LIB_JPEG_ERROR			= &H05E2&
%IL_LIB_PNG_ERROR			= &H05E3&
%IL_LIB_TIFF_ERROR			= &H05E4&
%IL_LIB_MNG_ERROR			= &H05E5&
%IL_UNKNOWN_ERROR			= &H05FF&


' Origin Definitions
%IL_ORIGIN_SET			= &H0600&
%IL_ORIGIN_LOWER_LEFT	= &H0601&
%IL_ORIGIN_UPPER_LEFT	= &H0602&
%IL_ORIGIN_MODE			= &H0603&


' Format and Type Mode Definitions
%IL_FORMAT_SET			= &H0610&
%IL_FORMAT_MODE			= &H0611&
%IL_TYPE_SET			= &H0612&
%IL_TYPE_MODE			= &H0613&


' File definitions
%IL_FILE_OVERWRITE		= &H0620&
%IL_FILE_MODE			= &H0621&


' Palette definitions
%IL_CONV_PAL			= &H0630&


' Load fail definitions
%IL_DEFAULT_ON_FAIL		= &H0632&


' Key colour definitions
%IL_USE_KEY_COLOUR      = &H0635&
%IL_USE_KEY_COLOR       = &H0635&


' Interlace definitions
%IL_SAVE_INTERLACED		= &H0639&
%IL_INTERLACE_MODE		= &H063A&


' Quantization definitions
%IL_QUANTIZATION_MODE	= &H0640&
%IL_WU_QUANT			= &H0641&
%IL_NEU_QUANT			= &H0642&
%IL_NEU_QUANT_SAMPLE	= &H0643&


' Hints
%IL_FASTEST				= &H0660&
%IL_LESS_MEM			= &H0661&
%IL_DONT_CARE			= &H0662&
%IL_MEM_SPEED_HINT		= &H0665&
%IL_USE_COMPRESSION		= &H0666&
%IL_NO_COMPRESSION		= &H0667&
%IL_COMPRESSION_HINT	= &H0668&


' Subimage types
%IL_SUB_NEXT			= &H0680&
%IL_SUB_MIPMAP			= &H0681&
%IL_SUB_LAYER			= &H0682&


' Compression definitions
%IL_COMPRESS_MODE       = &H0700&
%IL_COMPRESS_NONE       = &H0701&
%IL_COMPRESS_RLE        = &H0702&
%IL_COMPRESS_LZO        = &H0703&
%IL_COMPRESS_ZLIB       = &H0704&


' File format-specific values
%IL_TGA_CREATE_STAMP		= &H0710&
%IL_JPG_QUALITY				= &H0711&
%IL_PNG_INTERLACE			= &H0712&
%IL_TGA_RLE					= &H0713&
%IL_BMP_RLE					= &H0714&
%IL_SGI_RLE					= &H0715&
%IL_TGA_CREATE_STAMP		= &H0710&
%IL_JPG_QUALITY				= &H0711&
%IL_PNG_INTERLACE			= &H0712&
%IL_TGA_RLE					= &H0713&
%IL_BMP_RLE					= &H0714&
%IL_SGI_RLE					= &H0715&
%IL_TGA_ID_STRING			= &H0717&
%IL_TGA_AUTHNAME_STRING		= &H0718&
%IL_TGA_AUTHCOMMENT_STRING	= &H0719&
%IL_PNG_AUTHNAME_STRING		= &H071A&
%IL_PNG_TITLE_STRING		= &H071B&
%IL_PNG_DESCRIPTION_STRING	= &H071C&
%IL_TIF_DESCRIPTION_STRING	= &H071D&
%IL_TIF_HOSTCOMPUTER_STRING	= &H071E&
%IL_TIF_DOCUMENTNAME_STRING	= &H071F&
%IL_TIF_AUTHNAME_STRING		= &H0720&
%IL_JPG_SAVE_FORMAT			= &H0721&
%IL_CHEAD_HEADER_STRING		= &H0722&
%IL_PCD_PICNUM				= &H0723&


' DXTC definitions
%IL_DXTC_FORMAT				= &H0705&
%IL_DXT1					= &H0706&
%IL_DXT2					= &H0707&
%IL_DXT3					= &H0708&
%IL_DXT4					= &H0709&
%IL_DXT5					= &H070A&
%IL_DXT_NO_COMP				= &H070B&
%IL_KEEP_DXTC_DATA			= &H070C&
%IL_DXTC_DATA_FORMAT		= &H070D&


' Cube map definitions
%IL_CUBEMAP_POSITIVEX		= &H00000400&
%IL_CUBEMAP_NEGATIVEX		= &H00000800&
%IL_CUBEMAP_POSITIVEY		= &H00001000&
%IL_CUBEMAP_NEGATIVEY		= &H00002000&
%IL_CUBEMAP_POSITIVEZ		= &H00004000&
%IL_CUBEMAP_NEGATIVEZ		= &H00008000&


' Values
%IL_VERSION_NUM				= &H0DE2&
%IL_IMAGE_WIDTH				= &H0DE4&
%IL_IMAGE_HEIGHT			= &H0DE5&
%IL_IMAGE_DEPTH				= &H0DE6&
%IL_IMAGE_SIZE_OF_DATA		= &H0DE7&
%IL_IMAGE_BPP				= &H0DE8&
%IL_IMAGE_BYTES_PER_PIXEL	= &H0DE8&
%IL_IMAGE_BITS_PER_PIXEL	= &H0DE9&
%IL_IMAGE_FORMAT	        = &H0DEA&
%IL_IMAGE_TYPE			    = &H0DEB&
%IL_PALETTE_TYPE			= &H0DEC&
%IL_PALETTE_SIZE			= &H0DED&
%IL_PALETTE_BPP				= &H0DEE&
%IL_PALETTE_NUM_COLS		= &H0DEF&
%IL_PALETTE_BASE_TYPE		= &H0DF0&
%IL_NUM_IMAGES				= &H0DF1&
%IL_NUM_MIPMAPS				= &H0DF2&
%IL_NUM_LAYERS				= &H0DF3&
%IL_ACTIVE_IMAGE			= &H0DF4&
%IL_ACTIVE_MIPMAP			= &H0DF5&
%IL_ACTIVE_LAYER			= &H0DF6&
%IL_CUR_IMAGE				= &H0DF7&
%IL_IMAGE_DURATION			= &H0DF8&


'
' Section shamelessly modified from the glut header.
'

' This is from Win32's <windef.h>
' #if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__) || defined(__LCC__)
'  = %ILAPIENTRY = __stdcall
'  %IL_PACKSTRUCT
' #else
'  %ILAPIENTRY
'  = %IL_PACKSTRUCT = __attribute__ = ((packed))
' #endif
' This is from Win32's <wingdi.h> and <winnt.h>
' #if defined(__LCC__)
'  = %ILAPI = __stdcall
' #elif _WIN32
'  #ifdef _IL_BUILD_LIBRARY
'  = %ILAPI = __declspec(dllexport)
'  #else
'  = %ILAPI = __declspec(dllimport)
'  #endif
' #else
'  %ILAPI
' #endif


%IL_SEEK_SET = 0
%IL_SEEK_CUR = 1
%IL_SEEK_END = 2
%IL_EOF = -1


' Callback functions for file reading
' typedef void* ILHANDLE;
' typedef ILvoid (ILAPIENTRY *fCloseRProc) (ILHANDLE);
' typedef ILboolean (ILAPIENTRY *fEofProc) (ILHANDLE);
' typedef ILint (ILAPIENTRY *fGetcProc) (ILHANDLE);
' typedef ILHANDLE (ILAPIENTRY *fOpenRProc) (char*);
' typedef ILint (ILAPIENTRY *fReadProc) (void*, ILuint, ILuint, ILHANDLE);
' typedef ILint (ILAPIENTRY *fSeekRProc) (ILHANDLE, ILint, ILint);
' typedef ILint (ILAPIENTRY *fTellRProc) (ILHANDLE);

' Callback functions for file writing
' typedef ILvoid (ILAPIENTRY *fCloseWProc) (ILHANDLE);
' typedef ILHANDLE (ILAPIENTRY *fOpenWProc) (char*);
' typedef ILint (ILAPIENTRY *fPutcProc) (ILubyte, ILHANDLE);
' typedef ILint (ILAPIENTRY *fSeekWProc) (ILHANDLE, ILint, ILint);
' typedef ILint (ILAPIENTRY *fTellWProc) (ILHANDLE);
' typedef ILint (ILAPIENTRY *fWriteProc) (void*, ILuint, ILuint, ILHANDLE);


' Registered format procedures
' typedef ILboolean (ILAPIENTRY *IL_LOADPROC) (char*);
' typedef ILboolean (ILAPIENTRY *IL_SAVEPROC) (char*);


' ImageLib Functions
Declare Function ilActiveImage Lib "DevIL.dll" Alias "ilActiveImage" (ByVal pb_Number As Dword) As Byte
Declare Function ilActiveLayer Lib "DevIL.dll" Alias "ilActiveLayer" (ByVal pb_Number As Dword) As Byte
Declare Function ilActiveMipmap Lib "DevIL.dll" Alias "ilActiveMipmap" (ByVal pb_Number As Dword) As Byte
Declare Function ilApplyPal Lib "DevIL.dll" Alias "ilApplyPal" (ByRef pb_FileName As Asciiz) As Byte
Declare Function ilApplyProfile Lib "DevIL.dll" Alias "ilApplyProfile" (ByRef pb_InProfile
As Asciiz, ByRef pb_OutProfile As Asciiz) As Byte
Declare Sub ilBindImage Lib "DevIL.dll" Alias "ilBindImage" (ByVal pb_Image As Dword)
Declare Function ilBlit Lib "DevIL.dll" Alias "ilBlit" (ByVal pb_Src As Long, ByVal pb_DestX As Long, ByVal pb_DestY As Long, ByVal pb_DestZ as Dword, ByVal pb_SrcX As Dword, ByVal pb_SrcY As Dword, ByVal pb_SrcZ As Dword, ByVal pb_Width As Dword, ByVal pb_Height As Dword, ByVal pb_Depth As Dword) As Byte
Declare Sub ilClearColour Lib "DevIL.dll" Alias "ilClearColour" (ByVal pb_Red As Single, ByVal pb_Green As Single, ByVal pb_Blue As Single, ByVal pb_Alpha As Single)
Declare Function ilClearImage Lib "DevIL.dll" Alias "ilClearImage" () As Byte
Declare Function ilCloneCurImage Lib "DevIL.dll" Alias "ilCloneCurImage" () As Dword
Declare Function ilCompressFunc Lib "DevIL.dll" Alias "ilCompressFunc" (ByVal pb_Mode As Dword) As Byte
Declare Function ilConvertImage Lib "DevIL.dll" Alias "ilConvertImage" (ByVal pb_DestFormat As Dword, ByVal pb_DestType As Dword) As Byte
Declare Function ilConvertPal Lib "DevIL.dll" Alias "ilConvertPal" (ByVal pb_DestFormat As Dword) As Byte
Declare Function ilCopyImage Lib "DevIL.dll" Alias "ilCopyImage" (ByVal pb_Src As Dword) As Byte
Declare Function ilCopyPixels Lib "DevIL.dll" Alias "ilCopyPixels" (ByVal pb_XOff As Dword, ByVal pb_YOff As Dword, ByVal pb_ZOff As Dword, ByVal pb_Width As Dword, ByVal pb_Height As Dword, ByVal pb_Depth As Dword, ByVal pb_Format As Dword, ByVal pb_Type As Dword, ByRef pb_Data As Long) As Dword
Declare Function ilCreateSubImage Lib "DevIL.dll" Alias "ilCreateSubImage" (ByVal pb_Type As Dword, ByVal pb_Num As Dword) As Dword
Declare Function ilDefaultImage Lib "DevIL.dll" Alias "ilDefaultImage" () As Byte
Declare Sub ilDeleteImages Lib "DevIL.dll" Alias "ilDeleteImages" (ByVal pb_Num As Long, ByRef pb_Images As Dword)
Declare Function ilDisable Lib "DevIL.dll" Alias "ilDisable" (ByVal pb_Mode As Dword) As Byte
Declare Function ilEnable Lib "DevIL.dll" Alias "ilEnable" (ByVal pb_Mode As Dword) As Byte
Declare Function ilFormatFunc Lib "DevIL.dll" Alias "ilFormatFunc" (ByVal pb_Mode As Dword) As Byte
Declare Sub ilGenImages Lib "DevIL.dll" Alias "ilGenImages" (ByVal pb_Num As Long, ByRef pb_Images As Dword)
Declare Function ilGetAlpha Lib "DevIL.dll" Alias "ilGetAlpha" (ByVal pb_Type As Dword) As Long
Declare Function ilGetBoolean Lib "DevIL.dll" Alias "ilGetBoolean" (ByVal pb_Mode As Dword) As Byte
Declare Sub ilGetBooleanv Lib "DevIL.dll" Alias "ilGetBooleanv" (ByVal pb_Mode As Dword, ByRef pb_Param As Byte)
Declare Function ilGetData Lib "DevIL.dll" Alias "ilGetData" () As Long
Declare Function ilGetError Lib "DevIL.dll" Alias "ilGetError" () As Dword
Declare Function ilGetInteger Lib "DevIL.dll" Alias "ilGetInteger" (ByVal pb_Mode As Dword) As Long
Declare Sub ilGetIntegerv Lib "DevIL.dll" Alias "ilGetIntegerv" (ByVal pb_Mode As Dword, ByRef pb_Param As Long)
Declare Function ilGetLumpPos Lib "DevIL.dll" Alias "ilGetLumpPos" () As Dword
Declare Function ilGetPalette Lib "DevIL.dll" Alias "ilGetPalette" () As Long
Declare Function ilGetString Lib "DevIL.dll" Alias "ilGetString" (ByVal pb_StringName As Dword) As String
Declare Sub ilHint Lib "DevIL.dll" Alias "ilHint" (ByVal pb_Target As Dword, ByVal pb_Mode As Dword)
Declare Sub ilInit Lib "DevIL.dll" Alias "ilInit" ()
Declare Function ilIsDisabled Lib "DevIL.dll" Alias "ilIsDisabled" (ByVal pb_Mode As Dword) As Byte
Declare Function ilIsEnabled Lib "DevIL.dll" Alias "ilIsEnabled" (ByVal pb_Mode As Dword) As Byte
Declare Function ilIsImage Lib "DevIL.dll" Alias "ilIsImage" (ByVal pb_Image As Dword) As Byte
Declare Function ilIsValid Lib "DevIL.dll" Alias "ilIsValid" (ByVal pb_Type As Dword, ByRef pb_FileName As Asciiz) As Byte
Declare Function ilIsValidF Lib "DevIL.dll" Alias "ilIsValidF" (ByVal pb_Type As Dword, ByVal pb_File As Long) As Byte
Declare Function ilIsValidL Lib "DevIL.dll" Alias "ilIsValidL" (ByVal pb_Type As Dword, ByRef pb_Lump As Long, ByVal pb_Size As Dword) As Byte
Declare Sub ilKeyColour Lib "DevIL.dll" Alias "ilKeyColour" (ByVal pb_Red As Single, ByVal pb_Green As Single, ByVal pb_Blue As Single, ByVal pb_Alpha As Single)
Declare Function ilLoad Lib "DevIL.dll" Alias "ilLoad" (ByVal pb_Type As Dword, ByRef pb_FileName As Asciiz) As Byte
Declare Function ilLoadF Lib "DevIL.dll" Alias "ilLoadF" (ByVal pb_Type As Dword, ByVal pb_File As Long) As Byte
Declare Function ilLoadImage Lib "DevIL.dll" Alias "ilLoadImage" (ByRef pb_FileName As Asciiz) As Byte
Declare Function ilLoadL Lib "DevIL.dll" Alias "ilLoadL" (ByVal pb_Type As Dword, ByRef pb_Lump As Long, ByVal pb_Size As Dword) As Byte
Declare Function ilLoadPal Lib "DevIL.dll" Alias "ilLoadPal" (ByRef pb_FileName As Asciiz) As Byte
Declare Function ilOriginFunc Lib "DevIL.dll" Alias "ilOriginFunc" (ByVal pb_Mode As Dword) As Byte
Declare Function ilOverlayImage Lib "DevIL.dll" Alias "ilOverlayImage" (ByVal pb_Src As Dword, ByVal pb_XCoord As Long, ByVal pb_YCoord As Long, ByVal pb_ZCoord As Long) As Byte
Declare Sub ilPopAttrib Lib "DevIL.dll" Alias "ilPopAttrib" ()
Declare Sub ilPushAttrib Lib "DevIL.dll" Alias "ilPushAttrib" (ByVal pb_Bits As Dword)
Declare Sub ilRegisterFormat Lib "DevIL.dll" Alias "ilRegisterFormat" (ByVal pb_Format As Dword)
Declare Function ilRegisterLoad Lib "DevIL.dll" Alias "ilRegisterLoad" (ByRef pb_Ext As Asciiz, ByVal pb_Load As Dword) As Byte
Declare Function ilRegisterMipNum Lib "DevIL.dll" Alias "ilRegisterMipNum" (ByVal pb_Num As Dword) As Byte
Declare Function ilRegisterNumImages Lib "DevIL.dll" Alias "ilRegisterNumImages" (ByVal pb_Num As Dword) As Byte
Declare Sub ilRegisterOrigin Lib "DevIL.dll" Alias "ilRegisterOrigin" (ByVal pb_Origin As Dword)
Declare Sub ilRegisterPal Lib "DevIL.dll" Alias "ilRegisterPal" (ByRef pb_Pal As Long, ByVal pb_Size As Dword, ByVal pb_Type As Dword)
Declare Function ilRegisterSave Lib "DevIL.dll" Alias "ilRegisterSave" (ByRef pb_Ext As Asciiz, ByVal pb_Save As Dword) As Byte
Declare Sub ilRegisterType Lib "DevIL.dll" Alias "ilRegisterType" (ByVal pb_Type As Dword)
Declare Function ilRemoveLoad Lib "DevIL.dll" Alias "ilRemoveLoad" (ByRef pb_Ext As Asciiz) As Byte
Declare Function ilRemoveSave Lib "DevIL.dll" Alias "ilRemoveSave" (ByRef pb_Ext As Asciiz) As Byte
Declare Sub ilResetRead Lib "DevIL.dll" Alias "ilResetRead" ()
Declare Sub ilResetWrite Lib "DevIL.dll" Alias "ilResetWrite" ()
Declare Function ilSave Lib "DevIL.dll" Alias "ilSave" (ByVal pb_Type As Dword, ByRef pb_FileName As Asciiz) As Byte
Declare Function ilSaveF Lib "DevIL.dll" Alias "ilSaveF" (ByVal pb_Type As Dword, ByVal pb_File As Long) As Byte
Declare Function ilSaveImage Lib "DevIL.dll" Alias "ilSaveImage" (ByRef pb_FileName As Asciiz) As Byte
Declare Function ilSaveL Lib "DevIL.dll" Alias "ilSaveL" (ByVal pb_Type As Dword, ByRef pb_Lump As Long, ByVal pb_Size As Dword) As Byte
Declare Function ilSavePal Lib "DevIL.dll" Alias "ilSavePal" (ByRef pb_FileName As Asciiz) As Byte
Declare Function ilSetData Lib "DevIL.dll" Alias "ilSetData" (ByRef pb_Data As Long) As Byte
Declare Function ilSetDuration Lib "DevIL.dll" Alias "ilSetDuration" (ByVal pb_Duration As Dword) As Byte
Declare Sub ilSetInteger Lib "DevIL.dll" Alias "ilSetInteger" (ByVal pb_Mode As Dword, ByVal pb_Param As Dword)
Declare Sub ilSetPixels Lib "DevIL.dll" Alias "ilSetPixels" (ByVal pb_XOff As Dword, ByVal pb_YOff As Dword, ByVal pb_ZOff As Dword, ByVal pb_Width As Dword, ByVal pb_Height As Dword, ByVal pb_Depth As Dword, ByVal pb_Format As Dword, ByVal pb_Type As  _
Dword, ByRef pb_Data As Long)
Declare Sub ilSetRead Lib "DevIL.dll" Alias "ilSetRead" (ByVal pb_fOpenRProc As Dword, ByVal pb_fCloseRProc As Dword, ByVal pb_fEofProc As Dword, ByVal pb_fGetcProc As Dword, ByVal pb_fReadProc As Dword, _
ByVal pb_fSeekRProc As  Dword, ByVal pb_fTellRProc As Dword)
Declare Sub ilSetWrite Lib "DevIL.dll" Alias "ilSetWrite" (ByVal pb_fOpenWProc As Dword, ByVal pb_fCloseWProc As Dword, ByVal pb_fPutcProc As Dword, ByVal pb_fSeekWProc As Dword, _
ByVal pb_fTellWProc As Dword, ByVal pb_fWriteProc As  Dword)
Declare Function ilTexImage Lib "DevIL.dll" Alias "ilTexImage" (ByVal pb_Width As Dword, ByVal pb_Height As Dword, ByVal pb_Depth As Dword, ByVal pb_Bpp As Byte, ByVal pb_Format As Dword, ByVal pb_Type As Dword, ByRef pb_Data As Long) As Byte
Declare Function ilTypeFunc Lib "DevIL.dll" Alias "ilTypeFunc" (ByVal pb_Mode As Dword) As Byte

Declare Function ilLoadData Lib "DevIL.dll" Alias "ilLoadData" (ByRef pb_FileName As Asciiz, ByVal pb_Width As Dword, ByVal pb_Height As Dword, ByVal pb_Depth As Dword, ByVal pb_Bpp As Byte) As Byte
Declare Function ilLoadDataF Lib "DevIL.dll" Alias "ilLoadDataF" (ByVal pb_File As Long, ByVal pb_Width As Dword, ByVal pb_Height As Dword, ByVal pb_Depth As Dword, ByVal pb_Bpp As Byte) As Byte
Declare Function ilLoadDataL Lib "DevIL.dll" Alias "ilLoadDataL" (ByRef pb_Lump As Long, ByVal pb_Size As Dword, ByVal pb_Width As Dword, ByVal pb_Height As Dword, ByVal pb_Depth As Dword, ByVal pb_Bpp As Byte) As Byte
Declare Function ilSaveData Lib "DevIL.dll" Alias "ilSaveData" (ByRef pb_FileName As Asciiz) As Byte

'%ilClearColor = ilClearColour
'%ilKeyColor = ilKeyColour


' #if (DJGPP || LINUX)
'  static void GccMain() __attribute__((constructor));
' #endif


' #ifdef __cplusplus
' }
' #endif

' #endif /* __IL_H__ */
' #endif /* __il_h_ */
