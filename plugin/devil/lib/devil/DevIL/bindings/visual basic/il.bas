Attribute VB_Name = "Module1"
'-----------------------------------------------------------------------------
'
' ImageLib Sources
' Copyright (C) 2000-2002 by Denton Woods
' Converted from il.h by Rune Kock (rune@vupti.com)
' based on the earlier conversion by Timo Heister (Timo-Heister@gmx.de)
' Last modified:  22 June 2002, based on il.h dated 06/22/2002
'
' Filename: il.bas
'
' Description:  The main include file for DevIL
'
'-----------------------------------------------------------------------------


' I've converted the types as follows.  I have not dared use user-defined types, as I
' don't know how VB handles them internally.  This means, unfortunately, that the headers
' are less informative than the original.  So refer to il.h to see the original types.

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

Option Explicit

Public Const IL_FALSE = 0
Public Const IL_TRUE = 1


' Matches OpenGL's right now.
Public Const IL_COLOUR_INDEX = &H1900
Public Const IL_COLOR_INDEX = &H1900
Public Const IL_RGB = &H1907
Public Const IL_RGBA = &H1908
Public Const IL_BGR = &H80E0
Public Const IL_BGRA = &H80E1
Public Const IL_LUMINANCE = &H1909

Public Const IL_BYTE = &H1400
Public Const IL_UNSIGNED_BYTE = &H1401
Public Const IL_SHORT = &H1402
Public Const IL_UNSIGNED_SHORT = &H1403
Public Const IL_INT = &H1404
Public Const IL_UNSIGNED_INT = &H1405
Public Const IL_FLOAT = &H1406
Public Const IL_DOUBLE = &H140A

Public Const IL_VENDOR = &H1F00


'
' IL-specific public const's
'

Public Const IL_VERSION_1_6_0 = 1
Public Const IL_VERSION = 160
Public Const IL_LOAD_EXT = &H1F01
Public Const IL_SAVE_EXT = &H1F02


' Attribute Bits
Public Const IL_ORIGIN_BIT = &H1
Public Const IL_FILE_BIT = &H2
Public Const IL_PAL_BIT = &H4
Public Const IL_FORMAT_BIT = &H8
Public Const IL_TYPE_BIT = &H10
Public Const IL_COMPRESS_BIT = &H20
Public Const IL_LOADFAIL_BIT = &H40
Public Const IL_FORMAT_SPECIFIC_BIT = &H80
Public Const IL_ALL_ATTRIB_BITS = &HFFFFF

' Palette types
Public Const IL_PAL_NONE = &H400
Public Const IL_PAL_RGB24 = &H401
Public Const IL_PAL_RGB32 = &H402
Public Const IL_PAL_RGBA32 = &H403
Public Const IL_PAL_BGR24 = &H404
Public Const IL_PAL_BGR32 = &H405
Public Const IL_PAL_BGRA32 = &H406

' Image types
Public Const IL_TYPE_UNKNOWN = &H0
Public Const IL_BMP = &H420
Public Const IL_CUT = &H421
Public Const IL_DOOM = &H422
Public Const IL_DOOM_FLAT = &H423
Public Const IL_ICO = &H424
Public Const IL_JPG = &H425
Public Const IL_LBM = &H426
Public Const IL_PCD = &H427
Public Const IL_PCX = &H428
Public Const IL_PIC = &H429
Public Const IL_PNG = &H42A
Public Const IL_PNM = &H42B
Public Const IL_SGI = &H42C
Public Const IL_TGA = &H42D
Public Const IL_TIF = &H42E
Public Const IL_CHEAD = &H42F
Public Const IL_RAW = &H430
Public Const IL_MDL = &H431
Public Const IL_WAL = &H432
Public Const IL_OIL = &H433
Public Const IL_LIF = &H434
Public Const IL_MNG = &H435
Public Const IL_JNG = &H435
Public Const IL_GIF = &H436
Public Const IL_DDS = &H437
Public Const IL_DCX = &H438
Public Const IL_PSD = &H439
Public Const IL_EXIF = &H43A
Public Const IL_PSP = &H43B
Public Const IL_PIX = &H43C
Public Const IL_PXR = &H43D
Public Const IL_XPM = &H43E

Public Const IL_JASC_PAL = &H475

' Error Types
Public Const IL_NO_ERROR = &H0
Public Const IL_INVALID_ENUM = &H501
Public Const IL_OUT_OF_MEMORY = &H502
Public Const IL_FORMAT_NOT_SUPPORTED = &H503
Public Const IL_INTERNAL_ERROR = &H504
Public Const IL_INVALID_VALUE = &H505
Public Const IL_ILLEGAL_OPERATION = &H506
Public Const IL_ILLEGAL_FILE_VALUE = &H507
Public Const IL_INVALID_FILE_HEADER = &H508
Public Const IL_INVALID_PARAM = &H509
Public Const IL_COULD_NOT_OPEN_FILE = &H50A
Public Const IL_INVALID_EXTENSION = &H50B
Public Const IL_FILE_ALREADY_EXISTS = &H50C
Public Const IL_OUT_FORMAT_SAME = &H50D
Public Const IL_STACK_OVERFLOW = &H50E
Public Const IL_STACK_UNDERFLOW = &H50F
Public Const IL_INVALID_CONVERSION = &H510
Public Const IL_BAD_DIMENSIONS = &H511
Public Const IL_FILE_READ_ERROR = &H512

Public Const IL_LIB_GIF_ERROR = &H5E1
Public Const IL_LIB_JPEG_ERROR = &H5E2
Public Const IL_LIB_PNG_ERROR = &H5E3
Public Const IL_LIB_TIFF_ERROR = &H5E4
Public Const IL_LIB_MNG_ERROR = &H5E5
Public Const IL_UNKNOWN_ERROR = &H5FF

' Origin Definitions
Public Const IL_ORIGIN_SET = &H600
Public Const IL_ORIGIN_LOWER_LEFT = &H601
Public Const IL_ORIGIN_UPPER_LEFT = &H602
Public Const IL_ORIGIN_MODE = &H603

' Format and Type Mode Definitions
Public Const IL_FORMAT_SET = &H610
Public Const IL_FORMAT_MODE = &H611
Public Const IL_TYPE_SET = &H612
Public Const IL_TYPE_MODE = &H613

' File definitions
Public Const IL_FILE_OVERWRITE = &H620
Public Const IL_FILE_MODE = &H621

' Palette definitions
Public Const IL_CONV_PAL = &H630

' Load fail definitions
Public Const IL_DEFAULT_ON_FAIL = &H632

' Key colour definitions
Public Const IL_USE_KEY_COLOUR = &H635
Public Const IL_USE_KEY_COLOR = &H635

' Interlace definitions
Public Const IL_SAVE_INTERLACED = &H639
Public Const IL_INTERLACE_MODE = &H63A

' Quantization definitions
Public Const IL_QUANTIZATION_MODE = &H640
Public Const IL_WU_QUANT = &H641
Public Const IL_NEU_QUANT = &H642
Public Const IL_NEU_QUANT_SAMPLE = &H643

' Hints
Public Const IL_FASTEST = &H660
Public Const IL_LESS_MEM = &H661
Public Const IL_DONT_CARE = &H662
Public Const IL_MEM_SPEED_HINT = &H665
Public Const IL_USE_COMPRESSION = &H666
Public Const IL_NO_COMPRESSION = &H667
Public Const IL_COMPRESSION_HINT = &H668

' Subimage types
Public Const IL_SUB_NEXT = &H680
Public Const IL_SUB_MIPMAP = &H681
Public Const IL_SUB_LAYER = &H682

' Compression definitions (mostly for .oil)
Public Const IL_COMPRESS_MODE = &H700
Public Const IL_COMPRESS_NONE = &H701
Public Const IL_COMPRESS_RLE = &H702
Public Const IL_COMPRESS_LZO = &H703
Public Const IL_COMPRESS_ZLIB = &H704

' File format-specific values
Public Const IL_TGA_CREATE_STAMP = &H710
Public Const IL_JPG_QUALITY = &H711
Public Const IL_PNG_INTERLACE = &H712
Public Const IL_TGA_RLE = &H713
Public Const IL_BMP_RLE = &H714
Public Const IL_SGI_RLE = &H715
Public Const IL_TGA_ID_STRING = &H717
Public Const IL_TGA_AUTHNAME_STRING = &H718
Public Const IL_TGA_AUTHCOMMENT_STRING = &H719
Public Const IL_PNG_AUTHNAME_STRING = &H71A
Public Const IL_PNG_TITLE_STRING = &H71B
Public Const IL_PNG_DESCRIPTION_STRING = &H71C
Public Const IL_TIF_DESCRIPTION_STRING = &H71D
Public Const IL_TIF_HOSTCOMPUTER_STRING = &H71E
Public Const IL_TIF_DOCUMENTNAME_STRING = &H71F
Public Const IL_TIF_AUTHNAME_STRING = &H720
Public Const IL_JPG_SAVE_FORMAT = &H721
Public Const IL_CHEAD_HEADER_STRING = &H722
Public Const IL_PCD_PICNUM = &H723

' DXTC definitions
Public Const IL_DXTC_FORMAT = &H705
Public Const IL_DXT1 = &H706
Public Const IL_DXT2 = &H707
Public Const IL_DXT3 = &H708
Public Const IL_DXT4 = &H709
Public Const IL_DXT5 = &H70A
Public Const IL_DXT_NO_COMP = &H70B
Public Const IL_KEEP_DXTC_DATA = &H70C
Public Const IL_DXTC_DATA_FORMAT = &H70D

' Cube map definitions
Public Const IL_CUBEMAP_POSITIVEX = &H400
Public Const IL_CUBEMAP_NEGATIVEX = &H800
Public Const IL_CUBEMAP_POSITIVEY = &H1000
Public Const IL_CUBEMAP_NEGATIVEY = &H2000
Public Const IL_CUBEMAP_POSITIVEZ = &H4000
Public Const IL_CUBEMAP_NEGATIVEZ = &H8000

' Values
Public Const IL_VERSION_NUM = &HDE2
Public Const IL_IMAGE_WIDTH = &HDE4
Public Const IL_IMAGE_HEIGHT = &HDE5
Public Const IL_IMAGE_DEPTH = &HDE6
Public Const IL_IMAGE_SIZE_OF_DATA = &HDE7
Public Const IL_IMAGE_BPP = &HDE8
Public Const IL_IMAGE_BYTES_PER_PIXEL = &HDE8
Public Const IL_IMAGE_BITS_PER_PIXEL = &HDE9
Public Const IL_IMAGE_FORMAT = &HDEA
Public Const IL_IMAGE_TYPE = &HDEB
Public Const IL_PALETTE_TYPE = &HDEC
Public Const IL_PALETTE_SIZE = &HDED
Public Const IL_PALETTE_BPP = &HDEE
Public Const IL_PALETTE_NUM_COLS = &HDEF
Public Const IL_PALETTE_BASE_TYPE = &HDF0
Public Const IL_NUM_IMAGES = &HDF1
Public Const IL_NUM_MIPMAPS = &HDF2
Public Const IL_NUM_LAYERS = &HDF3
Public Const IL_ACTIVE_IMAGE = &HDF4
Public Const IL_ACTIVE_MIPMAP = &HDF5
Public Const IL_ACTIVE_LAYER = &HDF6
Public Const IL_CUR_IMAGE = &HDF7
Public Const IL_IMAGE_DURATION = &HDF8
Public Const IL_IMAGE_PLANESIZE = &HDF9
Public Const IL_IMAGE_BPC = &HDFA
Public Const IL_IMAGE_OFFX = &HDFB
Public Const IL_IMAGE_OFFY = &HDFC
Public Const IL_IMAGE_CUBEFLAGS = &HDFD

Public Const IL_SEEK_SET = 0
Public Const IL_SEEK_CUR = 1
Public Const IL_SEEK_END = 2
Public Const IL_EOF = -1


' ImageLib Functions
Public Declare Function ilActiveImage Lib "devil" (ByVal Number As Long) As Byte
Public Declare Function ilActiveLayer Lib "devil" (ByVal Number As Long) As Byte
Public Declare Function ilActiveMipmap Lib "devil" (ByVal Number As Long) As Byte
Public Declare Function ilApplyPal Lib "devil" (ByVal FileName As String) As Byte
Public Declare Function ilApplyProfile Lib "devil" (ByVal InProfile As String, ByVal OutProfile As String) As Byte
Public Declare Sub ilBindImage Lib "devil" (ByVal Image As Long)
Public Declare Function ilBlit Lib "devil" (ByVal Src As Long, ByVal DestX As Long, ByVal DestY As Long, ByVal DestZ As Long, ByVal SrcX As Long, ByVal SrcY As Long, ByVal SrcZ As Long, ByVal width As Long, ByVal height As Long, ByVal Depth As Long) As Byte
Public Declare Sub ilClearColour Lib "devil" (ByVal Red As Single, ByVal Green As Single, ByVal Blue As Single, ByVal Alpha As Single)
Public Declare Function ilClearImage Lib "devil" () As Byte
Public Declare Function ilCloneCurImage Lib "devil" () As Long
Public Declare Function ilCompressFunc Lib "devil" (ByVal Mode As Long) As Byte
Public Declare Function ilConvertImage Lib "devil" (ByVal DestFormat As Long, ByVal DestType As Long) As Byte
Public Declare Function ilConvertPal Lib "devil" (ByVal DestFormat As Long) As Byte
Public Declare Function ilCopyImage Lib "devil" (ByVal Src As Long) As Byte
Public Declare Sub ilCopyPixels Lib "devil" (ByVal XOff As Long, ByVal YOff As Long, ByVal ZOff As Long, ByVal width As Long, ByVal height As Long, ByVal Depth As Long, ByVal OutFormat As Long, ByVal OutType As Long, ByRef OutData As Byte)
Public Declare Function ilCreateSubImage Lib "devil" (ByVal IType As Long, ByVal Num As Long) As Long
Public Declare Function ilDefaultImage Lib "devil" () As Boolean
Public Declare Sub ilDeleteImages Lib "devil" (ByVal Num As Long, ByRef Images As Long)
Public Declare Function ilDisable Lib "devil" (ByVal Mode As Long) As Byte
Public Declare Function ilEnable Lib "devil" (ByVal Mode As Long) As Byte
Public Declare Function ilFormatFunc Lib "devil" (ByVal Mode As Long) As Byte
Public Declare Sub ilGenImages Lib "devil" (ByVal Num As Long, ByRef Images As Long)
Public Declare Function ilGetBoolean Lib "devil" (ByVal Mode As Long) As Byte
Public Declare Sub ilGetBooleanv Lib "devil" (ByVal Mode As Long, ByRef Param As Byte)
Public Declare Function ilGetData Lib "devil" () As Long
Public Declare Function ilGetError Lib "devil" () As Long
Public Declare Function ilGetInteger Lib "devil" (ByVal Mode As Long) As Long
Public Declare Sub ilGetIntegerv Lib "devil" (ByVal Mode As Long, ByRef Param As Long)
Public Declare Function ilGetLumpPos Lib "devil" () As Long
Public Declare Function ilGetPalette Lib "devil" () As Long
Public Declare Function ilGetString Lib "devil" (StringName As Long) As Long
Public Declare Sub ilHint Lib "devil" (ByVal Target As Long, ByVal Mode As Long)
Public Declare Sub ilInit Lib "devil" ()
Public Declare Function ilIsDisabled Lib "devil" (ByVal Mode As Long) As Byte
Public Declare Function ilIsEnabled Lib "devil" (ByVal Mode As Long) As Byte
Public Declare Function ilIsImage Lib "devil" (ByVal Image As Long) As Byte
Public Declare Function ilIsValid Lib "devil" (ByVal IType As Long, ByVal FileName As String) As Byte
Public Declare Function ilIsValidF Lib "devil" (ByVal IType As Long, ByVal File As Long) As Byte
Public Declare Function ilIsValidL Lib "devil" (ByVal IType As Long, ByRef Lump As Byte, ByVal Size As Long) As Byte
Public Declare Sub ilKeyColour Lib "devil" (ByVal Red As Single, ByVal Green As Single, ByVal Blue As Single, ByVal Alpha As Single)
Public Declare Function ilLoad Lib "devil" (ByVal IType As Long, ByVal FileName As String) As Byte
Public Declare Function ilLoadF Lib "devil" (ByVal IType As Long, ByVal File As Long) As Byte
Public Declare Function ilLoadImage Lib "devil" (ByVal FileName As String) As Byte
Public Declare Function ilLoadL Lib "devil" (ByVal IType As Long, ByRef Lump As Byte, ByVal Size As Long)
Public Declare Function ilLoadPal Lib "devil" (ByVal FileName As String) As Byte
Public Declare Function ilOriginFunc Lib "devil" (ByVal Mode As Long) As Byte
Public Declare Function ilOverlayImage Lib "devil" (ByVal Src As Long, ByVal XCoord As Long, ByVal YCoord As Long, ByVal ZCoord As Long) As Byte
Public Declare Sub ilPopAttrib Lib "devil" ()
Public Declare Sub ilPushAttrib Lib "devil" (ByVal Bits As Long)
Public Declare Sub ilRegisterFormat Lib "devil" (ByVal Format As Long)
Public Declare Function ilRegisterLoad Lib "devil" (ByVal Ext As String, ByVal LoadProc As Long) As Byte
Public Declare Function ilRegisterMipNum Lib "devil" (ByVal Num As Long) As Byte
Public Declare Function ilRegisterNumImages Lib "devil" (ByVal Num As Long) As Byte
Public Declare Sub ilRegisterOrigin Lib "devil" (ByVal Origin As Long)
Public Declare Sub ilRegisterPal Lib "devil" (ByRef Pal As Byte, ByVal Size As Long, ByVal IType As Long)
Public Declare Function ilRegisterSave Lib "devil" (ByVal Ext As String, ByVal SaveProc As Long) As Byte
Public Declare Sub ilRegisterType Lib "devil" (ByVal IType As Long)
Public Declare Function ilRemoveLoad Lib "devil" (ByVal Ext As String) As Byte
Public Declare Function ilRemoveSave Lib "devil" (ByVal Ext As String) As Byte
Public Declare Sub ilResetRead Lib "devil" ()
Public Declare Sub ilResetWrite Lib "devil" ()
Public Declare Function ilSave Lib "devil" (ByVal IType As Long, ByVal FileName As String) As Byte
Public Declare Function ilSaveF Lib "devil" (ByVal IType As Long, ByVal File As Long) As Byte
Public Declare Function ilSaveImage Lib "devil" (ByVal FileName As String) As Byte
Public Declare Function ilSaveL Lib "devil" (ByVal IType As Long, ByRef Lump As Byte, ByVal Size As Long) As Byte
Public Declare Function ilSavePal Lib "devil" (ByVal FileName As String) As Byte
Public Declare Function ilSetData Lib "devil" (ByRef Data As Byte) As Byte
Public Declare Function ilSetDuration Lib "devil" (ByVal Duration As Long) As Byte
Public Declare Sub ilSetInteger Lib "devil" (ByVal Mode As Long, ByVal Param As Long)
Public Declare Sub ilSetPixels Lib "devil" (ByVal XOff As Long, ByVal YOff As Long, ByVal ZOff As Long, ByVal width As Long, ByVal height As Long, ByVal Depth As Long, ByVal Format As Long, ByVal IType As Long, ByRef Data As Byte)
Public Declare Sub ilSetRead Lib "devil" (ByVal fOpenRProc As Long, ByVal fCloseRProc As Long, ByVal fEofProc As Long, ByVal fGetcProc As Long, ByVal fReadProc As Long, ByVal fSeekRProc As Long, ByVal fTellRProc As Long)
Public Declare Sub ilSetString Lib "devil" (ByVal Mode As Long, ByVal FileName As String)
Public Declare Sub ilSetWrite Lib "devil" (ByVal fOpenWProc As Long, ByVal fCloseWProc As Long, ByVal fPutcProc As Long, ByVal fSeekWProc As Long, ByVal fTellWProc As Long, ByVal fWriteProc As Long)
Public Declare Function ilTexImage Lib "devil" (ByVal width As Long, ByVal height As Long, ByVal Depth As Long, ByVal Bpp As Byte, ByVal Format As Long, ByVal IType As Long, ByRef Data As Byte) As Byte
Public Declare Function ilTexImage0 Lib "devil" Alias "ilTexImage" (ByVal width As Long, ByVal height As Long, ByVal Depth As Long, ByVal Bpp As Byte, ByVal Format As Long, ByVal IType As Long, ByVal DataPointer As Long) As Byte
' used if you want to pass a NULL-pointer (zero).
Public Declare Function ilTypeFunc Lib "devil" (ByVal Mode As Long) As Byte

Public Declare Function ilLoadData Lib "devil" (ByVal FileName As String, ByVal width As Long, ByVal height As Long, ByVal Depth As Long, ByVal Bpp As Byte) As Byte
Public Declare Function ilLoadDataF Lib "devil" (ByVal File As Long, ByVal width As Long, ByVal height As Long, ByVal Depth As Long, ByVal Bpp As Byte) As Byte
Public Declare Function ilLoadDataL Lib "devil" (ByRef Lump As Long, ByVal Size As Long, ByVal width As Long, ByVal height As Long, ByVal Depth As Long, ByVal Bpp As Byte) As Byte
Public Declare Function ilSaveData Lib "devil" (ByVal FileName As String) As Byte

' For all those weirdos that spell "colour" without the 'u'.
Public Declare Sub ilClearColor Lib "devil" Alias "ilClearColour" (ByVal Red As Single, ByVal Green As Single, ByVal Blue As Single, ByVal Alpha As Single)
Public Declare Sub ilKeyColor Lib "devil" Alias "ilKeyColour" (ByVal Red As Single, ByVal Green As Single, ByVal Blue As Single, ByVal Alpha As Single)
