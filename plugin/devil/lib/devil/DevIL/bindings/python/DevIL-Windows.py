from ctypes import *

_stdcall_libraries = {}
_stdcall_libraries['DevIL'] = WinDLL('DevIL')
STRING = c_char_p


IL_LOAD_EXT = 7937 # Variable c_int
IL_TGA_CREATE_STAMP = 1808 # Variable c_int
IL_PAL_RGB24 = 1025 # Variable c_int
IL_LIB_JPEG_ERROR = 1506 # Variable c_int
IL_DXTC_DATA_FORMAT = 1805 # Variable c_int
IL_TIF_AUTHNAME_STRING = 1824 # Variable c_int
IL_PSP = 1083 # Variable c_int
IL_PALETTE_BASE_TYPE = 3568 # Variable c_int
IL_PAL_BGR24 = 1028 # Variable c_int
IL_WAL = 1074 # Variable c_int
IL_ACTIVE_MIPMAP = 3573 # Variable c_int
IL_PSD = 1081 # Variable c_int
IL_VERSION = 175 # Variable c_int
IL_NUM_IMAGES = 3569 # Variable c_int
IL_INTERNAL_ERROR = 1284 # Variable c_int
IL_FORMAT_SET = 1552 # Variable c_int
IL_PNG_DESCRIPTION_STRING = 1820 # Variable c_int
IL_COULD_NOT_OPEN_FILE = 1290 # Variable c_int
IL_SEEK_SET = 0 # Variable c_int
IL_RXGB = 1807 # Variable c_int
IL_TGA_ID_STRING = 1815 # Variable c_int
IL_IMAGE_SIZE_OF_DATA = 3559 # Variable c_int
IL_RGBA = 6408 # Variable c_int
IL_PALETTE_TYPE = 3564 # Variable c_int
IL_BGR = 32992 # Variable c_int
IL_DOOM = 1058 # Variable c_int
IL_COMPRESS_RLE = 1794 # Variable c_int
IL_ACTIVE_IMAGE = 3572 # Variable c_int
IL_PAL_BIT = 4 # Variable c_int
IL_TYPE_SET = 1554 # Variable c_int
IL_PNG_INTERLACE = 1810 # Variable c_int
IL_DXTC_FORMAT = 1797 # Variable c_int
IL_CUBEMAP_POSITIVEZ = 16384 # Variable c_int
IL_FORMAT_NOT_SUPPORTED = 1283 # Variable c_int
IL_IMAGE_DEPTH = 3558 # Variable c_int
IL_NUM_MIPMAPS = 3570 # Variable c_int
IL_IMAGE_BYTES_PER_PIXEL = 3560 # Variable c_int
IL_IMAGE_CUBEFLAGS = 3581 # Variable c_int
IL_STACK_UNDERFLOW = 1295 # Variable c_int
IL_ORIGIN_LOWER_LEFT = 1537 # Variable c_int
IL_USE_KEY_COLOR = 1589 # Variable c_int
IL_UNSIGNED_INT = 5125 # Variable c_int
IL_USE_KEY_COLOUR = 1589 # Variable c_int
IL_ILLEGAL_OPERATION = 1286 # Variable c_int
IL_GIF = 1078 # Variable c_int
IL_FLOAT = 5126 # Variable c_int
IL_KEEP_DXTC_DATA = 1804 # Variable c_int
IL_FILE_WRITE_ERROR = 1298 # Variable c_int
IL_IMAGE_WIDTH = 3556 # Variable c_int
IL_DEFAULT_ON_FAIL = 1586 # Variable c_int
IL_USE_COMPRESSION = 1638 # Variable c_int
IL_JASC_PAL = 1141 # Variable c_int
IL_LBM = 1062 # Variable c_int
IL_ORIGIN_MODE = 1539 # Variable c_int
IL_NEU_QUANT = 1602 # Variable c_int
IL_VENDOR = 7936 # Variable c_int
IL_ACTIVE_LAYER = 3574 # Variable c_int
IL_SEEK_END = 2 # Variable c_int
IL_MNG = 1077 # Variable c_int
IL_TIF = 1070 # Variable c_int
IL_INVALID_CONVERSION = 1296 # Variable c_int
IL_DOUBLE = 5130 # Variable c_int
IL_VERSION_NUM = 3554 # Variable c_int
IL_PAL_RGBA32 = 1027 # Variable c_int
IL_TIF_DESCRIPTION_STRING = 1821 # Variable c_int
IL_PALETTE_BPP = 3566 # Variable c_int
IL_IMAGE_DURATION = 3576 # Variable c_int
IL_JP2 = 1089 # Variable c_int
IL_DXT4 = 1801 # Variable c_int
IL_DXT1 = 1798 # Variable c_int
IL_PAL_BGR32 = 1029 # Variable c_int
IL_DXT3 = 1800 # Variable c_int
IL_PIX = 1084 # Variable c_int
IL_LESS_MEM = 1633 # Variable c_int
IL_NUM_LAYERS = 3571 # Variable c_int
IL_BYTE = 5120 # Variable c_int
IL_INVALID_EXTENSION = 1291 # Variable c_int
IL_CUBEMAP_POSITIVEY = 4096 # Variable c_int
IL_CUBEMAP_POSITIVEX = 1024 # Variable c_int
IL_PCD = 1063 # Variable c_int
IL_IMAGE_BPC = 3578 # Variable c_int
IL_PIC = 1065 # Variable c_int
IL_SUB_LAYER = 1666 # Variable c_int
IL_UNSIGNED_SHORT = 5123 # Variable c_int
IL_PAL_BGRA32 = 1030 # Variable c_int
IL_JPG_PROGRESSIVE = 1829 # Variable c_int
IL_FILE_OVERWRITE = 1568 # Variable c_int
IL_SAVE_INTERLACED = 1593 # Variable c_int
IL_COLOUR_INDEX = 6400 # Variable c_int
IL_ILLEGAL_FILE_VALUE = 1287 # Variable c_int
IL_OUT_FORMAT_SAME = 1293 # Variable c_int
IL_UNSIGNED_BYTE = 5121 # Variable c_int
IL_PCX = 1064 # Variable c_int
IL_HDR = 1087 # Variable c_int
IL_LIB_MNG_ERROR = 1509 # Variable c_int
IL_COLOR_INDEX = 6400 # Variable c_int
IL_STACK_OVERFLOW = 1294 # Variable c_int
IL_JPG_QUALITY = 1809 # Variable c_int
IL_VERSION_1_7_5 = 1 # Variable c_int
IL_SUB_MIPMAP = 1665 # Variable c_int
IL_FALSE = 0 # Variable c_int
IL_FORMAT_MODE = 1553 # Variable c_int
IL_LIB_GIF_ERROR = 1505 # Variable c_int
IL_EXIF = 1082 # Variable c_int
IL_PXR = 1085 # Variable c_int
IL_SEEK_CUR = 1 # Variable c_int
IL_OUT_OF_MEMORY = 1282 # Variable c_int
IL_IMAGE_TYPE = 3563 # Variable c_int
IL_IMAGE_CHANNELS = 3583 # Variable c_int
IL_TRUE = 1 # Variable c_int
IL_COMPRESSION_HINT = 1640 # Variable c_int
IL_FILE_ALREADY_EXISTS = 1292 # Variable c_int
IL_JPG = 1061 # Variable c_int
IL_ORIGIN_BIT = 1 # Variable c_int
IL_LIB_PNG_ERROR = 1507 # Variable c_int
IL_INVALID_VALUE = 1285 # Variable c_int
IL_CUBEMAP_NEGATIVEZ = 32768 # Variable c_int
IL_CUBEMAP_NEGATIVEX = 2048 # Variable c_int
IL_DCX = 1080 # Variable c_int
IL_ORIGIN_SET = 1536 # Variable c_int
IL_DOOM_FLAT = 1059 # Variable c_int
IL_BAD_DIMENSIONS = 1297 # Variable c_int
IL_IMAGE_BITS_PER_PIXEL = 3561 # Variable c_int
IL_ATI1N = 1808 # Variable c_int
IL_JFIF = 1061 # Variable c_int
IL_SUB_NEXT = 1664 # Variable c_int
IL_TGA = 1069 # Variable c_int
IL_MEM_SPEED_HINT = 1637 # Variable c_int
IL_DDS = 1079 # Variable c_int
IL_QUANTIZATION_MODE = 1600 # Variable c_int
IL_IMAGE_HEIGHT = 3557 # Variable c_int
IL_MDL = 1073 # Variable c_int
IL_INVALID_PARAM = 1289 # Variable c_int
IL_BGRA = 32993 # Variable c_int
IL_PNG_ALPHA_INDEX = 1828 # Variable c_int
IL_WU_QUANT = 1601 # Variable c_int
IL_TGA_RLE = 1811 # Variable c_int
IL_IMAGE_ORIGIN = 3582 # Variable c_int
IL_SGI_RLE = 1813 # Variable c_int
IL_EOF = -1 # Variable c_int
IL_UNKNOWN_ERROR = 1535 # Variable c_int
IL_COMPRESS_MODE = 1792 # Variable c_int
IL_COMPRESS_BIT = 32 # Variable c_int
IL_COMPRESS_LZO = 1795 # Variable c_int
IL_NEU_QUANT_SAMPLE = 1603 # Variable c_int
IL_DONT_CARE = 1634 # Variable c_int
IL_MAX_QUANT_INDEXS = 1604 # Variable c_int
IL_IMAGE_BPP = 3560 # Variable c_int
IL_IMAGE_PLANESIZE = 3577 # Variable c_int
IL_3DC = 1806 # Variable c_int
IL_TYPE_UNKNOWN = 0 # Variable c_int
IL_COMPRESS_NONE = 1793 # Variable c_int
IL_XPM = 1086 # Variable c_int
IL_ORIGIN_UPPER_LEFT = 1538 # Variable c_int
IL_PALETTE_SIZE = 3565 # Variable c_int
IL_SHORT = 5122 # Variable c_int
IL_FILE_MODE = 1569 # Variable c_int
IL_IMAGE_FORMAT = 3562 # Variable c_int
IL_COMPRESS_ZLIB = 1796 # Variable c_int
IL_RGB = 6407 # Variable c_int
IL_TYPE_BIT = 16 # Variable c_int
IL_BLIT_BLEND = 1590 # Variable c_int
IL_JPG_SAVE_FORMAT = 1825 # Variable c_int
IL_LOADFAIL_BIT = 64 # Variable c_int
IL_PNM = 1067 # Variable c_int
IL_VTF = 1092 # Variable c_int
IL_PAL_NONE = 1024 # Variable c_int
IL_FORMAT_SPECIFIC_BIT = 128 # Variable c_int
IL_PNG = 1066 # Variable c_int
IL_TGA_AUTHCOMMENT_STRING = 1817 # Variable c_int
IL_CUR_IMAGE = 3575 # Variable c_int
IL_JNG = 1077 # Variable c_int
IL_LUMINANCE = 6409 # Variable c_int
IL_ICO = 1060 # Variable c_int
IL_IMAGE_OFFX = 3579 # Variable c_int
IL_IMAGE_OFFY = 3580 # Variable c_int
IL_INVALID_FILE_HEADER = 1288 # Variable c_int
IL_INVALID_ENUM = 1281 # Variable c_int
IL_INT = 5124 # Variable c_int
IL_CHEAD = 1071 # Variable c_int
IL_FORMAT_BIT = 8 # Variable c_int
IL_CONV_PAL = 1584 # Variable c_int
IL_NO_COMPRESSION = 1639 # Variable c_int
IL_SGI = 1068 # Variable c_int
IL_LIB_JP2_ERROR = 1510 # Variable c_int
IL_SAVE_EXT = 7938 # Variable c_int
IL_EXR = 1090 # Variable c_int
IL_FILE_BIT = 2 # Variable c_int
IL_BMP = 1056 # Variable c_int
IL_RAW = 1072 # Variable c_int
IL_BMP_RLE = 1812 # Variable c_int
IL_TGA_AUTHNAME_STRING = 1816 # Variable c_int
IL_TIF_DOCUMENTNAME_STRING = 1823 # Variable c_int
IL_PCD_PICNUM = 1827 # Variable c_int
IL_TYPE_MODE = 1555 # Variable c_int
IL_FILE_READ_ERROR = 1298 # Variable c_int
IL_LUMINANCE_ALPHA = 6410 # Variable c_int
IL_FASTEST = 1632 # Variable c_int
IL_WDP = 1091 # Variable c_int
IL_DXT5 = 1802 # Variable c_int
IL_CUBEMAP_NEGATIVEY = 8192 # Variable c_int
IL_CUT = 1057 # Variable c_int
IL_PAL_RGB32 = 1026 # Variable c_int
IL_PNG_AUTHNAME_STRING = 1818 # Variable c_int
IL_INTERLACE_MODE = 1594 # Variable c_int
IL_CHEAD_HEADER_STRING = 1826 # Variable c_int
IL_PNG_TITLE_STRING = 1819 # Variable c_int
IL_ALL_ATTRIB_BITS = 1048575 # Variable c_int
IL_DXT_NO_COMP = 1803 # Variable c_int
IL_DXT2 = 1799 # Variable c_int
IL_LIF = 1076 # Variable c_int
IL_TIF_HOSTCOMPUTER_STRING = 1822 # Variable c_int
IL_ICNS = 1088 # Variable c_int
IL_HALF = 5131 # Variable c_int
IL_LIB_TIFF_ERROR = 1508 # Variable c_int
IL_ALPHA = 6411 # Variable c_int
IL_NO_ERROR = 0 # Variable c_int
IL_PALETTE_NUM_COLS = 3567 # Variable c_int
ILboolean = c_ubyte
ILuint = c_uint
ilActiveImage = _stdcall_libraries['DevIL'].ilActiveImage
ilActiveImage.restype = ILboolean
ilActiveImage.argtypes = [ILuint]
ilActiveLayer = _stdcall_libraries['DevIL'].ilActiveLayer
ilActiveLayer.restype = ILboolean
ilActiveLayer.argtypes = [ILuint]
ilActiveMipmap = _stdcall_libraries['DevIL'].ilActiveMipmap
ilActiveMipmap.restype = ILboolean
ilActiveMipmap.argtypes = [ILuint]
ilApplyPal = _stdcall_libraries['DevIL'].ilApplyPal
ilApplyPal.restype = ILboolean
ilApplyPal.argtypes = [STRING]
ilApplyProfile = _stdcall_libraries['DevIL'].ilApplyProfile
ilApplyProfile.restype = ILboolean
ilApplyProfile.argtypes = [STRING, STRING]
ilBindImage = _stdcall_libraries['DevIL'].ilBindImage
ilBindImage.restype = None
ilBindImage.argtypes = [ILuint]
ILint = c_int
ilBlit = _stdcall_libraries['DevIL'].ilBlit
ilBlit.restype = ILboolean
ilBlit.argtypes = [ILuint, ILint, ILint, ILint, ILuint, ILuint, ILuint, ILuint, ILuint, ILuint]
ILclampf = c_float
ilClearColour = _stdcall_libraries['DevIL'].ilClearColour
ilClearColour.restype = None
ilClearColour.argtypes = [ILclampf, ILclampf, ILclampf, ILclampf]
ilClearImage = _stdcall_libraries['DevIL'].ilClearImage
ilClearImage.restype = ILboolean
ilClearImage.argtypes = []
ilCloneCurImage = _stdcall_libraries['DevIL'].ilCloneCurImage
ilCloneCurImage.restype = ILuint
ilCloneCurImage.argtypes = []
ILenum = c_uint
ilCompressFunc = _stdcall_libraries['DevIL'].ilCompressFunc
ilCompressFunc.restype = ILboolean
ilCompressFunc.argtypes = [ILenum]
ilConvertImage = _stdcall_libraries['DevIL'].ilConvertImage
ilConvertImage.restype = ILboolean
ilConvertImage.argtypes = [ILenum, ILenum]
ilConvertPal = _stdcall_libraries['DevIL'].ilConvertPal
ilConvertPal.restype = ILboolean
ilConvertPal.argtypes = [ILenum]
ilCopyImage = _stdcall_libraries['DevIL'].ilCopyImage
ilCopyImage.restype = ILboolean
ilCopyImage.argtypes = [ILuint]
ilCopyPixels = _stdcall_libraries['DevIL'].ilCopyPixels
ilCopyPixels.restype = ILuint
ilCopyPixels.argtypes = [ILuint, ILuint, ILuint, ILuint, ILuint, ILuint, ILenum, ILenum, c_void_p]
ilCreateSubImage = _stdcall_libraries['DevIL'].ilCreateSubImage
ilCreateSubImage.restype = ILuint
ilCreateSubImage.argtypes = [ILenum, ILuint]
ilDefaultImage = _stdcall_libraries['DevIL'].ilDefaultImage
ilDefaultImage.restype = ILboolean
ilDefaultImage.argtypes = []
ilDeleteImage = _stdcall_libraries['DevIL'].ilDeleteImage
ilDeleteImage.restype = None
ilDeleteImage.argtypes = [ILuint]
size_t = c_uint
ILsizei = size_t
ilDeleteImages = _stdcall_libraries['DevIL'].ilDeleteImages
ilDeleteImages.restype = None
ilDeleteImages.argtypes = [ILsizei, POINTER(ILuint)]
ilDisable = _stdcall_libraries['DevIL'].ilDisable
ilDisable.restype = ILboolean
ilDisable.argtypes = [ILenum]
ilEnable = _stdcall_libraries['DevIL'].ilEnable
ilEnable.restype = ILboolean
ilEnable.argtypes = [ILenum]
ilFormatFunc = _stdcall_libraries['DevIL'].ilFormatFunc
ilFormatFunc.restype = ILboolean
ilFormatFunc.argtypes = [ILenum]
ilGenImages = _stdcall_libraries['DevIL'].ilGenImages
ilGenImages.restype = None
ilGenImages.argtypes = [ILsizei, POINTER(ILuint)]
ilGenImage = _stdcall_libraries['DevIL'].ilGenImage
ilGenImage.restype = ILuint
ilGenImage.argtypes = []
ILubyte = c_ubyte
ilGetAlpha = _stdcall_libraries['DevIL'].ilGetAlpha
ilGetAlpha.restype = POINTER(ILubyte)
ilGetAlpha.argtypes = [ILenum]
ilGetBoolean = _stdcall_libraries['DevIL'].ilGetBoolean
ilGetBoolean.restype = ILboolean
ilGetBoolean.argtypes = [ILenum]
ilGetBooleanv = _stdcall_libraries['DevIL'].ilGetBooleanv
ilGetBooleanv.restype = None
ilGetBooleanv.argtypes = [ILenum, POINTER(ILboolean)]
ilGetData = _stdcall_libraries['DevIL'].ilGetData
ilGetData.restype = POINTER(ILubyte)
ilGetData.argtypes = []
ilGetDXTCData = _stdcall_libraries['DevIL'].ilGetDXTCData
ilGetDXTCData.restype = ILuint
ilGetDXTCData.argtypes = [c_void_p, ILuint, ILenum]
ilGetError = _stdcall_libraries['DevIL'].ilGetError
ilGetError.restype = ILenum
ilGetError.argtypes = []
ilGetInteger = _stdcall_libraries['DevIL'].ilGetInteger
ilGetInteger.restype = ILint
ilGetInteger.argtypes = [ILenum]
ilGetIntegerv = _stdcall_libraries['DevIL'].ilGetIntegerv
ilGetIntegerv.restype = None
ilGetIntegerv.argtypes = [ILenum, POINTER(ILint)]
ilGetLumpPos = _stdcall_libraries['DevIL'].ilGetLumpPos
ilGetLumpPos.restype = ILuint
ilGetLumpPos.argtypes = []
ilGetPalette = _stdcall_libraries['DevIL'].ilGetPalette
ilGetPalette.restype = POINTER(ILubyte)
ilGetPalette.argtypes = []
ilGetString = _stdcall_libraries['DevIL'].ilGetString
ilGetString.restype = STRING
ilGetString.argtypes = [ILenum]
ilHint = _stdcall_libraries['DevIL'].ilHint
ilHint.restype = None
ilHint.argtypes = [ILenum, ILenum]
ilInit = _stdcall_libraries['DevIL'].ilInit
ilInit.restype = None
ilInit.argtypes = []
ilIsDisabled = _stdcall_libraries['DevIL'].ilIsDisabled
ilIsDisabled.restype = ILboolean
ilIsDisabled.argtypes = [ILenum]
ilIsEnabled = _stdcall_libraries['DevIL'].ilIsEnabled
ilIsEnabled.restype = ILboolean
ilIsEnabled.argtypes = [ILenum]
ILHANDLE = c_void_p
ilDetermineTypeF = _stdcall_libraries['DevIL'].ilDetermineTypeF
ilDetermineTypeF.restype = ILenum
ilDetermineTypeF.argtypes = [ILHANDLE]
ilIsImage = _stdcall_libraries['DevIL'].ilIsImage
ilIsImage.restype = ILboolean
ilIsImage.argtypes = [ILuint]
ilIsValid = _stdcall_libraries['DevIL'].ilIsValid
ilIsValid.restype = ILboolean
ilIsValid.argtypes = [ILenum, STRING]
ilIsValidF = _stdcall_libraries['DevIL'].ilIsValidF
ilIsValidF.restype = ILboolean
ilIsValidF.argtypes = [ILenum, ILHANDLE]
ilIsValidL = _stdcall_libraries['DevIL'].ilIsValidL
ilIsValidL.restype = ILboolean
ilIsValidL.argtypes = [ILenum, c_void_p, ILuint]
ilKeyColour = _stdcall_libraries['DevIL'].ilKeyColour
ilKeyColour.restype = None
ilKeyColour.argtypes = [ILclampf, ILclampf, ILclampf, ILclampf]
ilLoad = _stdcall_libraries['DevIL'].ilLoad
ilLoad.restype = ILboolean
ilLoad.argtypes = [ILenum, STRING]
ilLoadF = _stdcall_libraries['DevIL'].ilLoadF
ilLoadF.restype = ILboolean
ilLoadF.argtypes = [ILenum, ILHANDLE]
ilLoadImage = _stdcall_libraries['DevIL'].ilLoadImage
ilLoadImage.restype = ILboolean
ilLoadImage.argtypes = [STRING]
ilLoadL = _stdcall_libraries['DevIL'].ilLoadL
ilLoadL.restype = ILboolean
ilLoadL.argtypes = [ILenum, c_void_p, ILuint]
ilLoadPal = _stdcall_libraries['DevIL'].ilLoadPal
ilLoadPal.restype = ILboolean
ilLoadPal.argtypes = [STRING]
ILdouble = c_double
ilModAlpha = _stdcall_libraries['DevIL'].ilModAlpha
ilModAlpha.restype = None
ilModAlpha.argtypes = [ILdouble]
ilOriginFunc = _stdcall_libraries['DevIL'].ilOriginFunc
ilOriginFunc.restype = ILboolean
ilOriginFunc.argtypes = [ILenum]
ilOverlayImage = _stdcall_libraries['DevIL'].ilOverlayImage
ilOverlayImage.restype = ILboolean
ilOverlayImage.argtypes = [ILuint, ILint, ILint, ILint]
ilPopAttrib = _stdcall_libraries['DevIL'].ilPopAttrib
ilPopAttrib.restype = None
ilPopAttrib.argtypes = []
ilPushAttrib = _stdcall_libraries['DevIL'].ilPushAttrib
ilPushAttrib.restype = None
ilPushAttrib.argtypes = [ILuint]
ilRegisterFormat = _stdcall_libraries['DevIL'].ilRegisterFormat
ilRegisterFormat.restype = None
ilRegisterFormat.argtypes = [ILenum]
IL_LOADPROC = WINFUNCTYPE(ILenum, STRING)
ilRegisterLoad = _stdcall_libraries['DevIL'].ilRegisterLoad
ilRegisterLoad.restype = ILboolean
ilRegisterLoad.argtypes = [STRING, IL_LOADPROC]
ilRegisterMipNum = _stdcall_libraries['DevIL'].ilRegisterMipNum
ilRegisterMipNum.restype = ILboolean
ilRegisterMipNum.argtypes = [ILuint]
ilRegisterNumImages = _stdcall_libraries['DevIL'].ilRegisterNumImages
ilRegisterNumImages.restype = ILboolean
ilRegisterNumImages.argtypes = [ILuint]
ilRegisterOrigin = _stdcall_libraries['DevIL'].ilRegisterOrigin
ilRegisterOrigin.restype = None
ilRegisterOrigin.argtypes = [ILenum]
ilRegisterPal = _stdcall_libraries['DevIL'].ilRegisterPal
ilRegisterPal.restype = None
ilRegisterPal.argtypes = [c_void_p, ILuint, ILenum]
IL_SAVEPROC = WINFUNCTYPE(ILenum, STRING)
ilRegisterSave = _stdcall_libraries['DevIL'].ilRegisterSave
ilRegisterSave.restype = ILboolean
ilRegisterSave.argtypes = [STRING, IL_SAVEPROC]
ilRegisterType = _stdcall_libraries['DevIL'].ilRegisterType
ilRegisterType.restype = None
ilRegisterType.argtypes = [ILenum]
ilRemoveLoad = _stdcall_libraries['DevIL'].ilRemoveLoad
ilRemoveLoad.restype = ILboolean
ilRemoveLoad.argtypes = [STRING]
ilRemoveSave = _stdcall_libraries['DevIL'].ilRemoveSave
ilRemoveSave.restype = ILboolean
ilRemoveSave.argtypes = [STRING]
ilResetMemory = _stdcall_libraries['DevIL'].ilResetMemory
ilResetMemory.restype = None
ilResetMemory.argtypes = []
ilResetRead = _stdcall_libraries['DevIL'].ilResetRead
ilResetRead.restype = None
ilResetRead.argtypes = []
ilResetWrite = _stdcall_libraries['DevIL'].ilResetWrite
ilResetWrite.restype = None
ilResetWrite.argtypes = []
ilSave = _stdcall_libraries['DevIL'].ilSave
ilSave.restype = ILboolean
ilSave.argtypes = [ILenum, STRING]
ilSaveF = _stdcall_libraries['DevIL'].ilSaveF
ilSaveF.restype = ILuint
ilSaveF.argtypes = [ILenum, ILHANDLE]
ilSaveImage = _stdcall_libraries['DevIL'].ilSaveImage
ilSaveImage.restype = ILboolean
ilSaveImage.argtypes = [STRING]
ilSaveL = _stdcall_libraries['DevIL'].ilSaveL
ilSaveL.restype = ILuint
ilSaveL.argtypes = [ILenum, c_void_p, ILuint]
ilSavePal = _stdcall_libraries['DevIL'].ilSavePal
ilSavePal.restype = ILboolean
ilSavePal.argtypes = [STRING]
ilSetAlpha = _stdcall_libraries['DevIL'].ilSetAlpha
ilSetAlpha.restype = ILboolean
ilSetAlpha.argtypes = [ILdouble]
ilSetData = _stdcall_libraries['DevIL'].ilSetData
ilSetData.restype = ILboolean
ilSetData.argtypes = [c_void_p]
ilSetDuration = _stdcall_libraries['DevIL'].ilSetDuration
ilSetDuration.restype = ILboolean
ilSetDuration.argtypes = [ILuint]
ilSetInteger = _stdcall_libraries['DevIL'].ilSetInteger
ilSetInteger.restype = None
ilSetInteger.argtypes = [ILenum, ILint]
mAlloc = WINFUNCTYPE(c_void_p, c_uint)
mFree = WINFUNCTYPE(None, c_void_p)
ilSetMemory = _stdcall_libraries['DevIL'].ilSetMemory
ilSetMemory.restype = None
ilSetMemory.argtypes = [mAlloc, mFree]
ilSetPixels = _stdcall_libraries['DevIL'].ilSetPixels
ilSetPixels.restype = None
ilSetPixels.argtypes = [ILint, ILint, ILint, ILuint, ILuint, ILuint, ILenum, ILenum, c_void_p]
fOpenRProc = WINFUNCTYPE(ILHANDLE, STRING)
fCloseRProc = WINFUNCTYPE(None, c_void_p)
fEofProc = WINFUNCTYPE(ILboolean, c_void_p)
fGetcProc = WINFUNCTYPE(ILint, c_void_p)
fReadProc = WINFUNCTYPE(ILint, c_void_p, c_uint, c_uint, c_void_p)
fSeekRProc = WINFUNCTYPE(ILint, c_void_p, c_int, c_int)
fTellRProc = WINFUNCTYPE(ILint, c_void_p)
ilSetRead = _stdcall_libraries['DevIL'].ilSetRead
ilSetRead.restype = None
ilSetRead.argtypes = [fOpenRProc, fCloseRProc, fEofProc, fGetcProc, fReadProc, fSeekRProc, fTellRProc]
ilSetString = _stdcall_libraries['DevIL'].ilSetString
ilSetString.restype = None
ilSetString.argtypes = [ILenum, STRING]
fOpenWProc = WINFUNCTYPE(ILHANDLE, STRING)
fCloseWProc = WINFUNCTYPE(None, c_void_p)
fPutcProc = WINFUNCTYPE(ILint, c_ubyte, c_void_p)
fSeekWProc = WINFUNCTYPE(ILint, c_void_p, c_int, c_int)
fTellWProc = WINFUNCTYPE(ILint, c_void_p)
fWriteProc = WINFUNCTYPE(ILint, c_void_p, c_uint, c_uint, c_void_p)
ilSetWrite = _stdcall_libraries['DevIL'].ilSetWrite
ilSetWrite.restype = None
ilSetWrite.argtypes = [fOpenWProc, fCloseWProc, fPutcProc, fSeekWProc, fTellWProc, fWriteProc]
ilShutDown = _stdcall_libraries['DevIL'].ilShutDown
ilShutDown.restype = None
ilShutDown.argtypes = []
ilTexImage = _stdcall_libraries['DevIL'].ilTexImage
ilTexImage.restype = ILboolean
ilTexImage.argtypes = [ILuint, ILuint, ILuint, ILubyte, ILenum, ILenum, c_void_p]
ilTypeFromExt = _stdcall_libraries['DevIL'].ilTypeFromExt
ilTypeFromExt.restype = ILenum
ilTypeFromExt.argtypes = [STRING]
ilTypeFunc = _stdcall_libraries['DevIL'].ilTypeFunc
ilTypeFunc.restype = ILboolean
ilTypeFunc.argtypes = [ILenum]
ilLoadData = _stdcall_libraries['DevIL'].ilLoadData
ilLoadData.restype = ILboolean
ilLoadData.argtypes = [STRING, ILuint, ILuint, ILuint, ILubyte]
ilLoadDataF = _stdcall_libraries['DevIL'].ilLoadDataF
ilLoadDataF.restype = ILboolean
ilLoadDataF.argtypes = [ILHANDLE, ILuint, ILuint, ILuint, ILubyte]
ilLoadDataL = _stdcall_libraries['DevIL'].ilLoadDataL
ilLoadDataL.restype = ILboolean
ilLoadDataL.argtypes = [c_void_p, ILuint, ILuint, ILuint, ILuint, ILubyte]
ilSaveData = _stdcall_libraries['DevIL'].ilSaveData
ilSaveData.restype = ILboolean
ilSaveData.argtypes = [STRING]
__all__ = ['IL_LOAD_EXT', 'IL_TGA_CREATE_STAMP', 'IL_PAL_RGB24',
           'IL_LIB_JPEG_ERROR', 'IL_DXTC_DATA_FORMAT',
           'IL_TIF_AUTHNAME_STRING', 'ilBindImage', 'IL_PSP',
           'ilConvertPal', 'IL_PAL_BGR24', 'IL_WAL', 'ilLoad',
           'IL_ACTIVE_MIPMAP', 'size_t', 'IL_PSD', 'IL_VERSION',
           'IL_NUM_IMAGES', 'IL_PNG_DESCRIPTION_STRING',
           'IL_INTERNAL_ERROR', 'IL_SEEK_SET', 'ilSetString',
           'ilRemoveLoad', 'IL_RXGB', 'IL_MDL', 'IL_NO_ERROR',
           'ILdouble', 'IL_TGA_ID_STRING', 'IL_IMAGE_SIZE_OF_DATA',
           'ILenum', 'IL_RGBA', 'IL_PALETTE_TYPE', 'fOpenRProc',
           'fReadProc', 'IL_BGR', 'IL_DOOM', 'IL_COMPRESS_RLE',
           'IL_ACTIVE_IMAGE', 'ilRegisterMipNum', 'ilLoadImage',
           'ilGenImages', 'ilSaveImage', 'IL_PAL_BIT',
           'IL_TGA_AUTHCOMMENT_STRING', 'ilLoadDataL', 'IL_TYPE_SET',
           'IL_PNG_INTERLACE', 'IL_DXTC_FORMAT', 'IL_RGB',
           'IL_FORMAT_NOT_SUPPORTED', 'ilSetMemory', 'ilClearImage',
           'IL_IMAGE_DEPTH', 'IL_NUM_MIPMAPS', 'ilModAlpha',
           'IL_BAD_DIMENSIONS', 'IL_IMAGE_CUBEFLAGS',
           'IL_STACK_UNDERFLOW', 'IL_ORIGIN_LOWER_LEFT',
           'IL_USE_KEY_COLOR', 'ilGetBoolean', 'IL_ACTIVE_LAYER',
           'ilRegisterNumImages', 'IL_SHORT', 'ilSave',
           'IL_UNSIGNED_INT', 'IL_FASTEST', 'IL_GIF', 'IL_FLOAT',
           'ilSaveData', 'IL_KEEP_DXTC_DATA', 'IL_FILE_WRITE_ERROR',
           'ILuint', 'IL_IMAGE_WIDTH', 'IL_DEFAULT_ON_FAIL',
           'ilLoadL', 'ilDeleteImages', 'IL_ATI1N', 'ilOverlayImage',
           'IL_LBM', 'IL_ORIGIN_MODE', 'ILubyte', 'IL_NEU_QUANT',
           'ilCompressFunc', 'IL_VENDOR', 'ilGetBooleanv',
           'ilSetAlpha', 'ilTypeFunc', 'IL_SEEK_END', 'IL_MNG',
           'IL_TIF', 'ILHANDLE', 'IL_DOUBLE', 'IL_VERSION_NUM',
           'ilGetDXTCData', 'IL_PAL_RGBA32',
           'IL_TIF_DESCRIPTION_STRING', 'IL_PALETTE_BPP',
           'ilResetRead', 'IL_JP2', 'IL_DXT4', 'ilLoadData',
           'ilGetLumpPos', 'IL_DXT1', 'IL_PAL_RGB32', 'IL_DXT3',
           'IL_PIX', 'IL_LESS_MEM', 'IL_PALETTE_BASE_TYPE',
           'IL_NUM_LAYERS', 'IL_BYTE', 'ilApplyProfile',
           'ilPopAttrib', 'fTellRProc', 'ilFormatFunc',
           'IL_CUBEMAP_POSITIVEZ', 'IL_CUBEMAP_POSITIVEY',
           'IL_CUBEMAP_POSITIVEX', 'fGetcProc', 'IL_PAL_NONE',
           'ilGetPalette', 'IL_JPG_PROGRESSIVE', 'ilLoadF',
           'fCloseRProc', 'IL_PIC', 'ilGetString', 'IL_LOADFAIL_BIT',
           'IL_UNSIGNED_SHORT', 'IL_JFIF', 'IL_IMAGE_DURATION',
           'ilGetError', 'IL_PAL_BGRA32', 'IL_PCD', 'IL_TGA',
           'ILboolean', 'ilSetInteger', 'ilIsValidF',
           'IL_COLOUR_INDEX', 'IL_ILLEGAL_FILE_VALUE',
           'IL_OUT_FORMAT_SAME', 'IL_UNSIGNED_BYTE', 'IL_PCX',
           'IL_HDR', 'ilGetAlpha', 'fTellWProc', 'IL_STACK_OVERFLOW',
           'IL_JPG_QUALITY', 'IL_VERSION_1_7_5', 'ilActiveLayer',
           'IL_SUB_MIPMAP', 'IL_FALSE', 'ilCopyPixels',
           'IL_FORMAT_MODE', 'ilIsValid', 'IL_LIB_GIF_ERROR',
           'ilRegisterSave', 'IL_USE_COMPRESSION', 'ilGetIntegerv',
           'IL_DXT2', 'IL_VTF', 'IL_PXR', 'IL_SEEK_CUR', 'IL_PNG',
           'ilShutDown', 'IL_OUT_OF_MEMORY', 'ILsizei',
           'IL_IMAGE_CHANNELS', 'ilApplyPal', 'IL_TRUE',
           'IL_COMPRESSION_HINT', 'IL_INVALID_VALUE',
           'IL_FILE_ALREADY_EXISTS', 'IL_JPG', 'mFree', 'ilGenImage',
           'IL_CUBEMAP_NEGATIVEZ', 'IL_CUBEMAP_NEGATIVEY',
           'IL_CUBEMAP_NEGATIVEX', 'IL_DCX', 'ilDisable', 'ilInit',
           'IL_ORIGIN_SET', 'IL_DOOM_FLAT', 'ilDefaultImage',
           'IL_IMAGE_BYTES_PER_PIXEL', 'IL_IMAGE_BITS_PER_PIXEL',
           'IL_USE_KEY_COLOUR', 'IL_SUB_NEXT', 'ilDetermineTypeF',
           'IL_PNM', 'ilIsDisabled', 'IL_LIB_PNG_ERROR',
           'IL_MEM_SPEED_HINT', 'IL_DDS', 'IL_COLOR_INDEX', 'IL_EXIF',
           'ilGetData', 'IL_JNG', 'IL_IMAGE_HEIGHT', 'ilLoadPal',
           'ilLoadDataF', 'IL_INVALID_FILE_HEADER', 'IL_BGRA',
           'ilSaveF', 'ilRegisterLoad', 'ilKeyColour',
           'IL_PNG_ALPHA_INDEX', 'IL_CONV_PAL',
           'IL_INVALID_EXTENSION', 'IL_TGA_RLE', 'IL_INT',
           'IL_IMAGE_ORIGIN', 'ilGetInteger', 'IL_IMAGE_BPC',
           'IL_EOF', 'IL_UNKNOWN_ERROR', 'IL_COMPRESS_MODE',
           'IL_COMPRESS_BIT', 'IL_ORIGIN_BIT', 'ilCloneCurImage',
           'ilRegisterFormat', 'ilSetWrite', 'ilActiveMipmap',
           'IL_NEU_QUANT_SAMPLE', 'IL_DONT_CARE',
           'IL_MAX_QUANT_INDEXS', 'IL_DXT_NO_COMP', 'IL_IMAGE_BPP',
           'ilBlit', 'IL_3DC', 'ilSetDuration', 'IL_TYPE_UNKNOWN',
           'IL_COMPRESS_NONE', 'mAlloc', 'ilActiveImage',
           'IL_PALETTE_SIZE', 'ilCreateSubImage', 'ilIsEnabled',
           'ilSetPixels', 'IL_FILE_MODE', 'ilTexImage',
           'IL_INVALID_ENUM', 'IL_COMPRESS_ZLIB', 'IL_SGI_RLE',
           'IL_TYPE_BIT', 'IL_BLIT_BLEND', 'IL_JPG_SAVE_FORMAT',
           'ilPushAttrib', 'IL_COULD_NOT_OPEN_FILE',
           'IL_FILE_OVERWRITE', 'IL_LIB_MNG_ERROR', 'IL_SAVEPROC',
           'IL_FORMAT_SPECIFIC_BIT', 'IL_QUANTIZATION_MODE',
           'ilSavePal', 'fSeekWProc', 'IL_SAVE_INTERLACED',
           'ilRegisterOrigin', 'IL_CUR_IMAGE', 'IL_WU_QUANT',
           'IL_LUMINANCE', 'fCloseWProc', 'IL_ICO', 'IL_IMAGE_OFFX',
           'IL_IMAGE_OFFY', 'IL_JASC_PAL', 'IL_IMAGE_PLANESIZE',
           'ilDeleteImage', 'IL_IMAGE_FORMAT', 'ilOriginFunc',
           'ilSetRead', 'IL_FORMAT_BIT', 'IL_ORIGIN_UPPER_LEFT',
           'IL_FILE_READ_ERROR', 'IL_NO_COMPRESSION',
           'IL_TIF_HOSTCOMPUTER_STRING', 'ilIsValidL', 'IL_SGI',
           'IL_LIB_JP2_ERROR', 'ilCopyImage', 'ilEnable',
           'ilRegisterType', 'IL_PCD_PICNUM', 'IL_SAVE_EXT',
           'IL_FILE_BIT', 'IL_BMP', 'IL_RAW', 'fEofProc',
           'ilRegisterPal', 'IL_BMP_RLE', 'IL_TGA_AUTHNAME_STRING',
           'IL_TIF_DOCUMENTNAME_STRING', 'IL_FORMAT_SET',
           'IL_TYPE_MODE', 'IL_SUB_LAYER', 'ilRemoveSave',
           'IL_LUMINANCE_ALPHA', 'IL_ILLEGAL_OPERATION', 'IL_EXR',
           'IL_DXT5', 'IL_CUT', 'IL_PAL_BGR32',
           'IL_PNG_AUTHNAME_STRING', 'IL_INTERLACE_MODE',
           'IL_IMAGE_TYPE', 'fSeekRProc', 'IL_CHEAD_HEADER_STRING',
           'fWriteProc', 'IL_PNG_TITLE_STRING', 'IL_COMPRESS_LZO',
           'ilSaveL', 'ilSetData', 'ilConvertImage', 'ILclampf',
           'ILint', 'IL_PALETTE_NUM_COLS', 'IL_INVALID_PARAM',
           'ilClearColour', 'ilTypeFromExt', 'fPutcProc',
           'IL_ALL_ATTRIB_BITS', 'IL_CHEAD', 'ilResetWrite',
           'IL_INVALID_CONVERSION', 'IL_LIF', 'IL_XPM', 'IL_ICNS',
           'IL_HALF', 'ilHint', 'fOpenWProc', 'ilIsImage',
           'IL_LIB_TIFF_ERROR', 'IL_ALPHA', 'IL_WDP', 'ilResetMemory',
           'IL_LOADPROC']
