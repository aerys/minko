from ctypes import *

_libraries = {}
_libraries['/usr/lib/libIL.so'] = CDLL('/usr/lib/libIL.so')
STRING = c_char_p


ILboolean = c_ubyte
ILuint = c_uint
ilActiveImage = _libraries['/usr/lib/libIL.so'].ilActiveImage
ilActiveImage.restype = ILboolean
ilActiveImage.argtypes = [ILuint]
ilActiveLayer = _libraries['/usr/lib/libIL.so'].ilActiveLayer
ilActiveLayer.restype = ILboolean
ilActiveLayer.argtypes = [ILuint]
ilActiveMipmap = _libraries['/usr/lib/libIL.so'].ilActiveMipmap
ilActiveMipmap.restype = ILboolean
ilActiveMipmap.argtypes = [ILuint]
ilApplyPal = _libraries['/usr/lib/libIL.so'].ilApplyPal
ilApplyPal.restype = ILboolean
ilApplyPal.argtypes = [STRING]
ilApplyProfile = _libraries['/usr/lib/libIL.so'].ilApplyProfile
ilApplyProfile.restype = ILboolean
ilApplyProfile.argtypes = [STRING, STRING]
ilBindImage = _libraries['/usr/lib/libIL.so'].ilBindImage
ilBindImage.restype = None
ilBindImage.argtypes = [ILuint]
ILint = c_int
ilBlit = _libraries['/usr/lib/libIL.so'].ilBlit
ilBlit.restype = ILboolean
ilBlit.argtypes = [ILuint, ILint, ILint, ILint, ILuint, ILuint, ILuint, ILuint, ILuint, ILuint]
ILclampf = c_float
ilClearColour = _libraries['/usr/lib/libIL.so'].ilClearColour
ilClearColour.restype = None
ilClearColour.argtypes = [ILclampf, ILclampf, ILclampf, ILclampf]
ilClearImage = _libraries['/usr/lib/libIL.so'].ilClearImage
ilClearImage.restype = ILboolean
ilClearImage.argtypes = []
ilCloneCurImage = _libraries['/usr/lib/libIL.so'].ilCloneCurImage
ilCloneCurImage.restype = ILuint
ilCloneCurImage.argtypes = []
ILenum = c_uint
ilCompressFunc = _libraries['/usr/lib/libIL.so'].ilCompressFunc
ilCompressFunc.restype = ILboolean
ilCompressFunc.argtypes = [ILenum]
ilConvertImage = _libraries['/usr/lib/libIL.so'].ilConvertImage
ilConvertImage.restype = ILboolean
ilConvertImage.argtypes = [ILenum, ILenum]
ilConvertPal = _libraries['/usr/lib/libIL.so'].ilConvertPal
ilConvertPal.restype = ILboolean
ilConvertPal.argtypes = [ILenum]
ilCopyImage = _libraries['/usr/lib/libIL.so'].ilCopyImage
ilCopyImage.restype = ILboolean
ilCopyImage.argtypes = [ILuint]
ilCopyPixels = _libraries['/usr/lib/libIL.so'].ilCopyPixels
ilCopyPixels.restype = ILuint
ilCopyPixels.argtypes = [ILuint, ILuint, ILuint, ILuint, ILuint, ILuint, ILenum, ILenum, c_void_p]
ilCreateSubImage = _libraries['/usr/lib/libIL.so'].ilCreateSubImage
ilCreateSubImage.restype = ILuint
ilCreateSubImage.argtypes = [ILenum, ILuint]
ilDefaultImage = _libraries['/usr/lib/libIL.so'].ilDefaultImage
ilDefaultImage.restype = ILboolean
ilDefaultImage.argtypes = []
ilDeleteImage = _libraries['/usr/lib/libIL.so'].ilDeleteImage
ilDeleteImage.restype = None
ilDeleteImage.argtypes = [ILuint]
size_t = c_ulong
ILsizei = size_t
ilDeleteImages = _libraries['/usr/lib/libIL.so'].ilDeleteImages
ilDeleteImages.restype = None
ilDeleteImages.argtypes = [ILsizei, POINTER(ILuint)]
ilDisable = _libraries['/usr/lib/libIL.so'].ilDisable
ilDisable.restype = ILboolean
ilDisable.argtypes = [ILenum]
ilEnable = _libraries['/usr/lib/libIL.so'].ilEnable
ilEnable.restype = ILboolean
ilEnable.argtypes = [ILenum]
ilFormatFunc = _libraries['/usr/lib/libIL.so'].ilFormatFunc
ilFormatFunc.restype = ILboolean
ilFormatFunc.argtypes = [ILenum]
ilGenImages = _libraries['/usr/lib/libIL.so'].ilGenImages
ilGenImages.restype = None
ilGenImages.argtypes = [ILsizei, POINTER(ILuint)]
ilGenImage = _libraries['/usr/lib/libIL.so'].ilGenImage
ilGenImage.restype = ILuint
ilGenImage.argtypes = []
ILubyte = c_ubyte
ilGetAlpha = _libraries['/usr/lib/libIL.so'].ilGetAlpha
ilGetAlpha.restype = POINTER(ILubyte)
ilGetAlpha.argtypes = [ILenum]
ilGetBoolean = _libraries['/usr/lib/libIL.so'].ilGetBoolean
ilGetBoolean.restype = ILboolean
ilGetBoolean.argtypes = [ILenum]
ilGetBooleanv = _libraries['/usr/lib/libIL.so'].ilGetBooleanv
ilGetBooleanv.restype = None
ilGetBooleanv.argtypes = [ILenum, POINTER(ILboolean)]
ilGetData = _libraries['/usr/lib/libIL.so'].ilGetData
ilGetData.restype = POINTER(ILubyte)
ilGetData.argtypes = []
ilGetDXTCData = _libraries['/usr/lib/libIL.so'].ilGetDXTCData
ilGetDXTCData.restype = ILuint
ilGetDXTCData.argtypes = [c_void_p, ILuint, ILenum]
ilGetError = _libraries['/usr/lib/libIL.so'].ilGetError
ilGetError.restype = ILenum
ilGetError.argtypes = []
ilGetInteger = _libraries['/usr/lib/libIL.so'].ilGetInteger
ilGetInteger.restype = ILint
ilGetInteger.argtypes = [ILenum]
ilGetIntegerv = _libraries['/usr/lib/libIL.so'].ilGetIntegerv
ilGetIntegerv.restype = None
ilGetIntegerv.argtypes = [ILenum, POINTER(ILint)]
ilGetLumpPos = _libraries['/usr/lib/libIL.so'].ilGetLumpPos
ilGetLumpPos.restype = ILuint
ilGetLumpPos.argtypes = []
ilGetPalette = _libraries['/usr/lib/libIL.so'].ilGetPalette
ilGetPalette.restype = POINTER(ILubyte)
ilGetPalette.argtypes = []
ilGetString = _libraries['/usr/lib/libIL.so'].ilGetString
ilGetString.restype = STRING
ilGetString.argtypes = [ILenum]
ilHint = _libraries['/usr/lib/libIL.so'].ilHint
ilHint.restype = None
ilHint.argtypes = [ILenum, ILenum]
ilInit = _libraries['/usr/lib/libIL.so'].ilInit
ilInit.restype = None
ilInit.argtypes = []
ilIsDisabled = _libraries['/usr/lib/libIL.so'].ilIsDisabled
ilIsDisabled.restype = ILboolean
ilIsDisabled.argtypes = [ILenum]
ilIsEnabled = _libraries['/usr/lib/libIL.so'].ilIsEnabled
ilIsEnabled.restype = ILboolean
ilIsEnabled.argtypes = [ILenum]
ILHANDLE = c_void_p
ilDetermineTypeF = _libraries['/usr/lib/libIL.so'].ilDetermineTypeF
ilDetermineTypeF.restype = ILenum
ilDetermineTypeF.argtypes = [ILHANDLE]
ilIsImage = _libraries['/usr/lib/libIL.so'].ilIsImage
ilIsImage.restype = ILboolean
ilIsImage.argtypes = [ILuint]
ilIsValid = _libraries['/usr/lib/libIL.so'].ilIsValid
ilIsValid.restype = ILboolean
ilIsValid.argtypes = [ILenum, STRING]
ilIsValidF = _libraries['/usr/lib/libIL.so'].ilIsValidF
ilIsValidF.restype = ILboolean
ilIsValidF.argtypes = [ILenum, ILHANDLE]
ilIsValidL = _libraries['/usr/lib/libIL.so'].ilIsValidL
ilIsValidL.restype = ILboolean
ilIsValidL.argtypes = [ILenum, c_void_p, ILuint]
ilKeyColour = _libraries['/usr/lib/libIL.so'].ilKeyColour
ilKeyColour.restype = None
ilKeyColour.argtypes = [ILclampf, ILclampf, ILclampf, ILclampf]
ilLoad = _libraries['/usr/lib/libIL.so'].ilLoad
ilLoad.restype = ILboolean
ilLoad.argtypes = [ILenum, STRING]
ilLoadF = _libraries['/usr/lib/libIL.so'].ilLoadF
ilLoadF.restype = ILboolean
ilLoadF.argtypes = [ILenum, ILHANDLE]
ilLoadImage = _libraries['/usr/lib/libIL.so'].ilLoadImage
ilLoadImage.restype = ILboolean
ilLoadImage.argtypes = [STRING]
ilLoadL = _libraries['/usr/lib/libIL.so'].ilLoadL
ilLoadL.restype = ILboolean
ilLoadL.argtypes = [ILenum, c_void_p, ILuint]
ilLoadPal = _libraries['/usr/lib/libIL.so'].ilLoadPal
ilLoadPal.restype = ILboolean
ilLoadPal.argtypes = [STRING]
ILdouble = c_double
ilModAlpha = _libraries['/usr/lib/libIL.so'].ilModAlpha
ilModAlpha.restype = None
ilModAlpha.argtypes = [ILdouble]
ilOriginFunc = _libraries['/usr/lib/libIL.so'].ilOriginFunc
ilOriginFunc.restype = ILboolean
ilOriginFunc.argtypes = [ILenum]
ilOverlayImage = _libraries['/usr/lib/libIL.so'].ilOverlayImage
ilOverlayImage.restype = ILboolean
ilOverlayImage.argtypes = [ILuint, ILint, ILint, ILint]
ilPopAttrib = _libraries['/usr/lib/libIL.so'].ilPopAttrib
ilPopAttrib.restype = None
ilPopAttrib.argtypes = []
ilPushAttrib = _libraries['/usr/lib/libIL.so'].ilPushAttrib
ilPushAttrib.restype = None
ilPushAttrib.argtypes = [ILuint]
ilRegisterFormat = _libraries['/usr/lib/libIL.so'].ilRegisterFormat
ilRegisterFormat.restype = None
ilRegisterFormat.argtypes = [ILenum]
IL_LOADPROC = CFUNCTYPE(ILenum, STRING)
ilRegisterLoad = _libraries['/usr/lib/libIL.so'].ilRegisterLoad
ilRegisterLoad.restype = ILboolean
ilRegisterLoad.argtypes = [STRING, IL_LOADPROC]
ilRegisterMipNum = _libraries['/usr/lib/libIL.so'].ilRegisterMipNum
ilRegisterMipNum.restype = ILboolean
ilRegisterMipNum.argtypes = [ILuint]
ilRegisterNumImages = _libraries['/usr/lib/libIL.so'].ilRegisterNumImages
ilRegisterNumImages.restype = ILboolean
ilRegisterNumImages.argtypes = [ILuint]
ilRegisterOrigin = _libraries['/usr/lib/libIL.so'].ilRegisterOrigin
ilRegisterOrigin.restype = None
ilRegisterOrigin.argtypes = [ILenum]
ilRegisterPal = _libraries['/usr/lib/libIL.so'].ilRegisterPal
ilRegisterPal.restype = None
ilRegisterPal.argtypes = [c_void_p, ILuint, ILenum]
IL_SAVEPROC = CFUNCTYPE(ILenum, STRING)
ilRegisterSave = _libraries['/usr/lib/libIL.so'].ilRegisterSave
ilRegisterSave.restype = ILboolean
ilRegisterSave.argtypes = [STRING, IL_SAVEPROC]
ilRegisterType = _libraries['/usr/lib/libIL.so'].ilRegisterType
ilRegisterType.restype = None
ilRegisterType.argtypes = [ILenum]
ilRemoveLoad = _libraries['/usr/lib/libIL.so'].ilRemoveLoad
ilRemoveLoad.restype = ILboolean
ilRemoveLoad.argtypes = [STRING]
ilRemoveSave = _libraries['/usr/lib/libIL.so'].ilRemoveSave
ilRemoveSave.restype = ILboolean
ilRemoveSave.argtypes = [STRING]
ilResetMemory = _libraries['/usr/lib/libIL.so'].ilResetMemory
ilResetMemory.restype = None
ilResetMemory.argtypes = []
ilResetRead = _libraries['/usr/lib/libIL.so'].ilResetRead
ilResetRead.restype = None
ilResetRead.argtypes = []
ilResetWrite = _libraries['/usr/lib/libIL.so'].ilResetWrite
ilResetWrite.restype = None
ilResetWrite.argtypes = []
ilSave = _libraries['/usr/lib/libIL.so'].ilSave
ilSave.restype = ILboolean
ilSave.argtypes = [ILenum, STRING]
ilSaveF = _libraries['/usr/lib/libIL.so'].ilSaveF
ilSaveF.restype = ILuint
ilSaveF.argtypes = [ILenum, ILHANDLE]
ilSaveImage = _libraries['/usr/lib/libIL.so'].ilSaveImage
ilSaveImage.restype = ILboolean
ilSaveImage.argtypes = [STRING]
ilSaveL = _libraries['/usr/lib/libIL.so'].ilSaveL
ilSaveL.restype = ILuint
ilSaveL.argtypes = [ILenum, c_void_p, ILuint]
ilSavePal = _libraries['/usr/lib/libIL.so'].ilSavePal
ilSavePal.restype = ILboolean
ilSavePal.argtypes = [STRING]
ilSetAlpha = _libraries['/usr/lib/libIL.so'].ilSetAlpha
ilSetAlpha.restype = ILboolean
ilSetAlpha.argtypes = [ILdouble]
ilSetData = _libraries['/usr/lib/libIL.so'].ilSetData
ilSetData.restype = ILboolean
ilSetData.argtypes = [c_void_p]
ilSetDuration = _libraries['/usr/lib/libIL.so'].ilSetDuration
ilSetDuration.restype = ILboolean
ilSetDuration.argtypes = [ILuint]
ilSetInteger = _libraries['/usr/lib/libIL.so'].ilSetInteger
ilSetInteger.restype = None
ilSetInteger.argtypes = [ILenum, ILint]
mAlloc = CFUNCTYPE(c_void_p, ILsizei)
mFree = CFUNCTYPE(None, c_void_p)
ilSetMemory = _libraries['/usr/lib/libIL.so'].ilSetMemory
ilSetMemory.restype = None
ilSetMemory.argtypes = [mAlloc, mFree]
ilSetPixels = _libraries['/usr/lib/libIL.so'].ilSetPixels
ilSetPixels.restype = None
ilSetPixels.argtypes = [ILint, ILint, ILint, ILuint, ILuint, ILuint, ILenum, ILenum, c_void_p]
fOpenRProc = CFUNCTYPE(ILHANDLE, STRING)
fCloseRProc = CFUNCTYPE(None, ILHANDLE)
fEofProc = CFUNCTYPE(ILboolean, ILHANDLE)
fGetcProc = CFUNCTYPE(ILint, ILHANDLE)
fReadProc = CFUNCTYPE(ILint, c_void_p, ILuint, ILuint, ILHANDLE)
fSeekRProc = CFUNCTYPE(ILint, ILHANDLE, ILint, ILint)
fTellRProc = CFUNCTYPE(ILint, ILHANDLE)
ilSetRead = _libraries['/usr/lib/libIL.so'].ilSetRead
ilSetRead.restype = None
ilSetRead.argtypes = [fOpenRProc, fCloseRProc, fEofProc, fGetcProc, fReadProc, fSeekRProc, fTellRProc]
ilSetString = _libraries['/usr/lib/libIL.so'].ilSetString
ilSetString.restype = None
ilSetString.argtypes = [ILenum, STRING]
fOpenWProc = CFUNCTYPE(ILHANDLE, STRING)
fCloseWProc = CFUNCTYPE(None, ILHANDLE)
fPutcProc = CFUNCTYPE(ILint, ILubyte, ILHANDLE)
fSeekWProc = CFUNCTYPE(ILint, ILHANDLE, ILint, ILint)
fTellWProc = CFUNCTYPE(ILint, ILHANDLE)
fWriteProc = CFUNCTYPE(ILint, c_void_p, ILuint, ILuint, ILHANDLE)
ilSetWrite = _libraries['/usr/lib/libIL.so'].ilSetWrite
ilSetWrite.restype = None
ilSetWrite.argtypes = [fOpenWProc, fCloseWProc, fPutcProc, fSeekWProc, fTellWProc, fWriteProc]
ilShutDown = _libraries['/usr/lib/libIL.so'].ilShutDown
ilShutDown.restype = None
ilShutDown.argtypes = []
ilTexImage = _libraries['/usr/lib/libIL.so'].ilTexImage
ilTexImage.restype = ILboolean
ilTexImage.argtypes = [ILuint, ILuint, ILuint, ILubyte, ILenum, ILenum, c_void_p]
ilTypeFromExt = _libraries['/usr/lib/libIL.so'].ilTypeFromExt
ilTypeFromExt.restype = ILenum
ilTypeFromExt.argtypes = [STRING]
ilTypeFunc = _libraries['/usr/lib/libIL.so'].ilTypeFunc
ilTypeFunc.restype = ILboolean
ilTypeFunc.argtypes = [ILenum]
ilLoadData = _libraries['/usr/lib/libIL.so'].ilLoadData
ilLoadData.restype = ILboolean
ilLoadData.argtypes = [STRING, ILuint, ILuint, ILuint, ILubyte]
ilLoadDataF = _libraries['/usr/lib/libIL.so'].ilLoadDataF
ilLoadDataF.restype = ILboolean
ilLoadDataF.argtypes = [ILHANDLE, ILuint, ILuint, ILuint, ILubyte]
ilLoadDataL = _libraries['/usr/lib/libIL.so'].ilLoadDataL
ilLoadDataL.restype = ILboolean
ilLoadDataL.argtypes = [c_void_p, ILuint, ILuint, ILuint, ILuint, ILubyte]
ilSaveData = _libraries['/usr/lib/libIL.so'].ilSaveData
ilSaveData.restype = ILboolean
ilSaveData.argtypes = [STRING]
IL_LIB_JPEG_ERROR = 1506 # Variable c_int
IL_FLOAT = 5126 # Variable c_int
IL_RXGB = 1807 # Variable c_int
IL_DOOM = 1058 # Variable c_int
IL_PAL_BIT = 4 # Variable c_int
IL_TYPE_SET = 1554 # Variable c_int
IL_NUM_MIPMAPS = 3570 # Variable c_int
IL_IMAGE_CUBEFLAGS = 3581 # Variable c_int
IL_STACK_UNDERFLOW = 1295 # Variable c_int
IL_SHORT = 5122 # Variable c_int
IL_KEEP_DXTC_DATA = 1804 # Variable c_int
IL_IMAGE_WIDTH = 3556 # Variable c_int
IL_DEFAULT_ON_FAIL = 1586 # Variable c_int
IL_ATI1N = 1808 # Variable c_int
IL_COMPRESS_LZO = 1795 # Variable c_int
IL_SEEK_END = 2 # Variable c_int
IL_MNG = 1077 # Variable c_int
IL_PSD = 1081 # Variable c_int
IL_DOUBLE = 5130 # Variable c_int
IL_JP2 = 1089 # Variable c_int
IL_PALETTE_BASE_TYPE = 3568 # Variable c_int
IL_CUBEMAP_POSITIVEZ = 16384 # Variable c_int
IL_CUBEMAP_POSITIVEY = 4096 # Variable c_int
IL_CUBEMAP_POSITIVEX = 1024 # Variable c_int
IL_PALETTE_NUM_COLS = 3567 # Variable c_int
IL_IMAGE_DURATION = 3576 # Variable c_int
IL_JPG_PROGRESSIVE = 1829 # Variable c_int
IL_OUT_FORMAT_SAME = 1293 # Variable c_int
IL_PCX = 1064 # Variable c_int
IL_HDR = 1087 # Variable c_int
IL_SUB_MIPMAP = 1665 # Variable c_int
IL_USE_COMPRESSION = 1638 # Variable c_int
IL_IMAGE_CHANNELS = 3583 # Variable c_int
IL_CUBEMAP_NEGATIVEZ = 32768 # Variable c_int
IL_CUBEMAP_NEGATIVEY = 8192 # Variable c_int
IL_CUBEMAP_NEGATIVEX = 2048 # Variable c_int
IL_IMAGE_BITS_PER_PIXEL = 3561 # Variable c_int
IL_TGA = 1069 # Variable c_int
IL_MEM_SPEED_HINT = 1637 # Variable c_int
IL_WU_QUANT = 1601 # Variable c_int
IL_INT = 5124 # Variable c_int
IL_LIB_JP2_ERROR = 1510 # Variable c_int
IL_MAX_QUANT_INDEXS = 1604 # Variable c_int
IL_3DC = 1806 # Variable c_int
IL_TYPE_UNKNOWN = 0 # Variable c_int
IL_TYPE_BIT = 16 # Variable c_int
IL_LOADFAIL_BIT = 64 # Variable c_int
IL_FILE_OVERWRITE = 1568 # Variable c_int
IL_FORMAT_SPECIFIC_BIT = 128 # Variable c_int
IL_CHEAD = 1071 # Variable c_int
IL_INVALID_FILE_HEADER = 1288 # Variable c_int
IL_RAW = 1072 # Variable c_int
IL_FILE_READ_ERROR = 1298 # Variable c_int
IL_ILLEGAL_OPERATION = 1286 # Variable c_int
IL_INTERLACE_MODE = 1594 # Variable c_int
IL_XPM = 1086 # Variable c_int
IL_LIB_TIFF_ERROR = 1508 # Variable c_int
IL_WDP = 1091 # Variable c_int
IL_LOAD_EXT = 7937 # Variable c_int
IL_PAL_RGB24 = 1025 # Variable c_int
IL_BMP_RLE = 1812 # Variable c_int
IL_COMPRESS_RLE = 1794 # Variable c_int
IL_ACTIVE_IMAGE = 3572 # Variable c_int
IL_IMAGE_DEPTH = 3558 # Variable c_int
IL_IMAGE_BYTES_PER_PIXEL = 3560 # Variable c_int
IL_ORIGIN_LOWER_LEFT = 1537 # Variable c_int
IL_USE_KEY_COLOR = 1589 # Variable c_int
IL_ACTIVE_LAYER = 3574 # Variable c_int
IL_GIF = 1078 # Variable c_int
IL_NEU_QUANT = 1602 # Variable c_int
IL_CONV_PAL = 1584 # Variable c_int
IL_DXT5 = 1802 # Variable c_int
IL_PNG_AUTHNAME_STRING = 1818 # Variable c_int
IL_DXT1 = 1798 # Variable c_int
IL_PAL_BGR32 = 1029 # Variable c_int
IL_DXT3 = 1800 # Variable c_int
IL_NUM_LAYERS = 3571 # Variable c_int
IL_BYTE = 5120 # Variable c_int
IL_PIX = 1084 # Variable c_int
IL_PIC = 1065 # Variable c_int
IL_JFIF = 1061 # Variable c_int
IL_PAL_BGRA32 = 1030 # Variable c_int
IL_ILLEGAL_FILE_VALUE = 1287 # Variable c_int
IL_UNSIGNED_BYTE = 5121 # Variable c_int
IL_FORMAT_MODE = 1553 # Variable c_int
IL_PALETTE_SIZE = 3565 # Variable c_int
IL_EXIF = 1082 # Variable c_int
IL_COMPRESSION_HINT = 1640 # Variable c_int
IL_INVALID_VALUE = 1285 # Variable c_int
IL_DCX = 1080 # Variable c_int
IL_DOOM_FLAT = 1059 # Variable c_int
IL_BAD_DIMENSIONS = 1297 # Variable c_int
IL_USE_KEY_COLOUR = 1589 # Variable c_int
IL_SGI_RLE = 1813 # Variable c_int
IL_UNKNOWN_ERROR = 1535 # Variable c_int
IL_VENDOR = 7936 # Variable c_int
IL_DONT_CARE = 1634 # Variable c_int
IL_COMPRESS_NONE = 1793 # Variable c_int
IL_COMPRESS_ZLIB = 1796 # Variable c_int
IL_BLIT_BLEND = 1590 # Variable c_int
IL_QUANTIZATION_MODE = 1600 # Variable c_int
IL_CUR_IMAGE = 3575 # Variable c_int
IL_INVALID_ENUM = 1281 # Variable c_int
IL_FORMAT_BIT = 8 # Variable c_int
IL_TIF_HOSTCOMPUTER_STRING = 1822 # Variable c_int
IL_BMP = 1056 # Variable c_int
IL_TIF_DESCRIPTION_STRING = 1821 # Variable c_int
IL_TIF_DOCUMENTNAME_STRING = 1823 # Variable c_int
IL_SUB_NEXT = 1664 # Variable c_int
IL_IMAGE_TYPE = 3563 # Variable c_int
IL_INVALID_PARAM = 1289 # Variable c_int
IL_ICNS = 1088 # Variable c_int
IL_ALPHA = 6411 # Variable c_int
IL_TGA_CREATE_STAMP = 1808 # Variable c_int
IL_PAL_BGR24 = 1028 # Variable c_int
IL_DXTC_DATA_FORMAT = 1805 # Variable c_int
IL_PSP = 1083 # Variable c_int
IL_TGA_ID_STRING = 1815 # Variable c_int
IL_IMAGE_SIZE_OF_DATA = 3559 # Variable c_int
IL_RGBA = 6408 # Variable c_int
IL_FORMAT_NOT_SUPPORTED = 1283 # Variable c_int
IL_FILE_MODE = 1569 # Variable c_int
IL_LBM = 1062 # Variable c_int
IL_ORIGIN_MODE = 1539 # Variable c_int
IL_TIF = 1070 # Variable c_int
IL_VERSION_NUM = 3554 # Variable c_int
IL_PALETTE_BPP = 3566 # Variable c_int
IL_LUMINANCE_ALPHA = 6410 # Variable c_int
IL_VERSION_1_7_5 = 1 # Variable c_int
IL_PAL_NONE = 1024 # Variable c_int
IL_PCD = 1063 # Variable c_int
IL_PAL_RGBA32 = 1027 # Variable c_int
IL_PXR = 1085 # Variable c_int
IL_NUM_IMAGES = 3569 # Variable c_int
IL_JPG_QUALITY = 1809 # Variable c_int
IL_FALSE = 0 # Variable c_int
IL_SEEK_CUR = 1 # Variable c_int
IL_SEEK_SET = 0 # Variable c_int
IL_JPG = 1061 # Variable c_int
IL_ORIGIN_SET = 1536 # Variable c_int
IL_DDS = 1079 # Variable c_int
IL_RGB = 6407 # Variable c_int
IL_TGA_RLE = 1811 # Variable c_int
IL_IMAGE_ORIGIN = 3582 # Variable c_int
IL_IMAGE_BPC = 3578 # Variable c_int
IL_COMPRESS_MODE = 1792 # Variable c_int
IL_COMPRESS_BIT = 32 # Variable c_int
IL_NEU_QUANT_SAMPLE = 1603 # Variable c_int
IL_JASC_PAL = 1141 # Variable c_int
IL_IMAGE_BPP = 3560 # Variable c_int
IL_HALF = 5131 # Variable c_int
IL_DXT2 = 1799 # Variable c_int
IL_NO_COMPRESSION = 1639 # Variable c_int
IL_COULD_NOT_OPEN_FILE = 1290 # Variable c_int
IL_PNM = 1067 # Variable c_int
IL_PNG = 1066 # Variable c_int
IL_TGA_AUTHCOMMENT_STRING = 1817 # Variable c_int
IL_JNG = 1077 # Variable c_int
IL_LUMINANCE = 6409 # Variable c_int
IL_ICO = 1060 # Variable c_int
IL_IMAGE_OFFX = 3579 # Variable c_int
IL_IMAGE_OFFY = 3580 # Variable c_int
IL_IMAGE_PLANESIZE = 3577 # Variable c_int
IL_ORIGIN_UPPER_LEFT = 1538 # Variable c_int
IL_TGA_AUTHNAME_STRING = 1816 # Variable c_int
IL_PCD_PICNUM = 1827 # Variable c_int
IL_TYPE_MODE = 1555 # Variable c_int
IL_SUB_LAYER = 1666 # Variable c_int
IL_CUT = 1057 # Variable c_int
IL_CHEAD_HEADER_STRING = 1826 # Variable c_int
IL_PNG_TITLE_STRING = 1819 # Variable c_int
IL_DXT_NO_COMP = 1803 # Variable c_int
IL_LIF = 1076 # Variable c_int
IL_DXTC_FORMAT = 1797 # Variable c_int
IL_TRUE = 1 # Variable c_int
IL_TIF_AUTHNAME_STRING = 1824 # Variable c_int
IL_WAL = 1074 # Variable c_int
IL_ACTIVE_MIPMAP = 3573 # Variable c_int
IL_PNG_DESCRIPTION_STRING = 1820 # Variable c_int
IL_INTERNAL_ERROR = 1284 # Variable c_int
IL_OUT_OF_MEMORY = 1282 # Variable c_int
IL_MDL = 1073 # Variable c_int
IL_IMAGE_HEIGHT = 3557 # Variable c_int
IL_PALETTE_TYPE = 3564 # Variable c_int
IL_BGR = 32992 # Variable c_int
IL_PNG_INTERLACE = 1810 # Variable c_int
IL_UNSIGNED_INT = 5125 # Variable c_int
IL_LIB_GIF_ERROR = 1505 # Variable c_int
IL_FILE_WRITE_ERROR = 1298 # Variable c_int
IL_ORIGIN_BIT = 1 # Variable c_int
IL_VTF = 1092 # Variable c_int
IL_INVALID_CONVERSION = 1296 # Variable c_int
IL_LESS_MEM = 1633 # Variable c_int
IL_LIB_PNG_ERROR = 1507 # Variable c_int
IL_SGI = 1068 # Variable c_int
IL_UNSIGNED_SHORT = 5123 # Variable c_int
IL_COLOUR_INDEX = 6400 # Variable c_int
IL_STACK_OVERFLOW = 1294 # Variable c_int
IL_FILE_ALREADY_EXISTS = 1292 # Variable c_int
IL_COLOR_INDEX = 6400 # Variable c_int
IL_BGRA = 32993 # Variable c_int
IL_PNG_ALPHA_INDEX = 1828 # Variable c_int
IL_EOF = -1 # Variable c_int
IL_JPG_SAVE_FORMAT = 1825 # Variable c_int
IL_LIB_MNG_ERROR = 1509 # Variable c_int
IL_INVALID_EXTENSION = 1291 # Variable c_int
IL_VERSION = 175 # Variable c_int
IL_IMAGE_FORMAT = 3562 # Variable c_int
IL_SAVE_EXT = 7938 # Variable c_int
IL_FILE_BIT = 2 # Variable c_int
IL_FORMAT_SET = 1552 # Variable c_int
IL_FASTEST = 1632 # Variable c_int
IL_EXR = 1090 # Variable c_int
IL_PAL_RGB32 = 1026 # Variable c_int
IL_DXT4 = 1801 # Variable c_int
IL_ALL_ATTRIB_BITS = 1048575 # Variable c_int
IL_SAVE_INTERLACED = 1593 # Variable c_int
IL_NO_ERROR = 0 # Variable c_int
__all__ = ['ilActiveImage', 'ILclampf', 'IL_PAL_RGB24',
           'IL_LIB_JPEG_ERROR', 'ilLoadImage', 'IL_VERSION_1_7_5',
           'IL_TIF_AUTHNAME_STRING', 'ilBindImage',
           'IL_TGA_CREATE_STAMP', 'IL_UNSIGNED_SHORT', 'ilConvertPal',
           'IL_PAL_BGR24', 'IL_FLOAT', 'ilLoad', 'IL_ACTIVE_MIPMAP',
           'size_t', 'ILHANDLE', 'IL_DOUBLE', 'IL_PALETTE_BASE_TYPE',
           'IL_NUM_IMAGES', 'IL_INTERNAL_ERROR', 'IL_LBM',
           'IL_PNG_DESCRIPTION_STRING', 'IL_CUBEMAP_POSITIVEZ',
           'IL_COMPRESSION_HINT', 'ilSetString', 'ilRemoveLoad',
           'IL_RXGB', 'IL_CUBEMAP_POSITIVEY', 'IL_NO_ERROR',
           'IL_LIB_TIFF_ERROR', 'fTellRProc', 'IL_TGA_ID_STRING',
           'IL_IMAGE_SIZE_OF_DATA', 'ILenum', 'IL_RGBA',
           'IL_TGA_AUTHNAME_STRING', 'fOpenRProc', 'IL_COMPRESS_NONE',
           'IL_JASC_PAL', 'IL_DOOM', 'IL_COMPRESS_RLE', 'IL_ICO',
           'IL_ACTIVE_IMAGE', 'ilRegisterMipNum', 'ilSetRead',
           'fEofProc', 'IL_PAL_BIT', 'ilLoadDataL', 'IL_PSP',
           'IL_TYPE_SET', 'IL_PNG_INTERLACE', 'IL_STACK_UNDERFLOW',
           'IL_VERSION_NUM', 'IL_FORMAT_NOT_SUPPORTED', 'ilSetMemory',
           'IL_IMAGE_DEPTH', 'IL_NUM_MIPMAPS', 'IL_SUB_LAYER',
           'IL_BAD_DIMENSIONS', 'IL_IMAGE_CUBEFLAGS', 'ilShutDown',
           'IL_ORIGIN_LOWER_LEFT', 'fCloseRProc', 'ilGetBoolean',
           'IL_ACTIVE_LAYER', 'IL_WAL', 'ilRegisterNumImages',
           'ILdouble', 'ilApplyProfile', 'IL_DXTC_DATA_FORMAT',
           'IL_FASTEST', 'IL_GIF', 'ilResetWrite', 'ilSaveData',
           'IL_KEEP_DXTC_DATA', 'ilGetIntegerv', 'ILuint',
           'IL_IMAGE_WIDTH', 'IL_PNG_ALPHA_INDEX', 'IL_SHORT',
           'IL_FILE_ALREADY_EXISTS', 'ilDeleteImages', 'IL_ATI1N',
           'ilOverlayImage', 'ilResetRead', 'IL_ORIGIN_MODE',
           'ILubyte', 'IL_NEU_QUANT', 'ilCompressFunc',
           'IL_COMPRESS_LZO', 'ilGetBooleanv', 'IL_FORMAT_SET',
           'IL_SEEK_END', 'IL_MNG', 'IL_IMAGE_TYPE', 'IL_PSD',
           'ilDeleteImage', 'IL_CONV_PAL', 'ilGetDXTCData',
           'IL_PAL_RGBA32', 'IL_TIF_DESCRIPTION_STRING',
           'IL_PALETTE_BPP', 'IL_JP2', 'ilClearImage', 'ilGetLumpPos',
           'IL_DXT1', 'IL_PAL_RGB32', 'IL_DXT3', 'IL_PIX',
           'IL_LESS_MEM', 'ilSaveF', 'IL_NUM_LAYERS', 'IL_BYTE',
           'IL_UNSIGNED_INT', 'IL_INVALID_EXTENSION', 'ilFormatFunc',
           'ilRegisterSave', 'ilLoadL', 'IL_CUBEMAP_POSITIVEX',
           'fGetcProc', 'IL_PAL_NONE', 'ilGetPalette',
           'IL_PALETTE_NUM_COLS', 'ilLoadF', 'IL_DXT4',
           'IL_IMAGE_BPC', 'ilTexImage', 'IL_PIC', 'ilGetString',
           'IL_COULD_NOT_OPEN_FILE', 'IL_FILE_WRITE_ERROR', 'IL_JFIF',
           'IL_IMAGE_DURATION', 'ilGetError', 'IL_PAL_BGRA32',
           'IL_JPG_PROGRESSIVE', 'IL_FILE_OVERWRITE', 'ILboolean',
           'ilSetInteger', 'IL_CHEAD_HEADER_STRING',
           'IL_COLOUR_INDEX', 'ilGenImages', 'IL_OUT_FORMAT_SAME',
           'IL_UNSIGNED_BYTE', 'IL_PCX', 'IL_HDR', 'ilPopAttrib',
           'IL_COLOR_INDEX', 'IL_MEM_SPEED_HINT', 'IL_JPG_QUALITY',
           'ilGetData', 'ilActiveLayer', 'IL_SUB_MIPMAP', 'IL_FALSE',
           'ilSetDuration', 'ilGetInteger', 'ilIsValid',
           'IL_LIB_GIF_ERROR', 'IL_USE_COMPRESSION', 'ilGetAlpha',
           'IL_FORMAT_SPECIFIC_BIT', 'IL_DXT2', 'IL_USE_KEY_COLOR',
           'IL_ILLEGAL_FILE_VALUE', 'IL_SEEK_CUR', 'fPutcProc',
           'IL_SEEK_SET', 'ILsizei', 'IL_IMAGE_CHANNELS',
           'ilApplyPal', 'IL_TRUE', 'fReadProc', 'IL_XPM', 'ilSave',
           'IL_JPG', 'IL_ORIGIN_BIT', 'IL_DEFAULT_ON_FAIL', 'mFree',
           'ilGenImage', 'IL_CUBEMAP_NEGATIVEZ',
           'IL_CUBEMAP_NEGATIVEY', 'IL_CUBEMAP_NEGATIVEX', 'IL_DCX',
           'ilRegisterLoad', 'IL_ORIGIN_SET', 'IL_DOOM_FLAT',
           'ilDefaultImage', 'IL_IMAGE_BYTES_PER_PIXEL',
           'IL_IMAGE_BITS_PER_PIXEL', 'IL_USE_KEY_COLOUR',
           'IL_IMAGE_FORMAT', 'IL_PNM', 'IL_PCD', 'ilIsDisabled',
           'IL_TGA', 'IL_BGR', 'IL_PALETTE_TYPE', 'IL_DDS',
           'fTellWProc', 'IL_SAVE_INTERLACED', 'IL_VENDOR', 'IL_EXIF',
           'IL_VERSION', 'IL_QUANTIZATION_MODE', 'fSeekWProc',
           'IL_IMAGE_HEIGHT', 'IL_MDL', 'IL_LOAD_EXT',
           'ilRegisterType', 'IL_UNKNOWN_ERROR', 'IL_STACK_OVERFLOW',
           'IL_LIF', 'IL_SGI_RLE', 'IL_WU_QUANT', 'IL_TGA_RLE',
           'IL_INT', 'IL_INTERLACE_MODE', 'IL_LIB_JP2_ERROR',
           'IL_FORMAT_MODE', 'fWriteProc', 'IL_EOF', 'IL_BGRA',
           'IL_COMPRESS_MODE', 'IL_COMPRESS_BIT', 'ILint',
           'ilCloneCurImage', 'ilRegisterFormat',
           'IL_ALL_ATTRIB_BITS', 'IL_NEU_QUANT_SAMPLE',
           'IL_DONT_CARE', 'IL_MAX_QUANT_INDEXS', 'IL_IMAGE_BPP',
           'IL_3DC', 'IL_TYPE_UNKNOWN', 'ilTypeFunc', 'mAlloc',
           'IL_ORIGIN_UPPER_LEFT', 'IL_PALETTE_SIZE',
           'ilCreateSubImage', 'ilIsEnabled', 'IL_PXR', 'ilSetPixels',
           'IL_FILE_MODE', 'ilSetAlpha', 'IL_COMPRESS_ZLIB', 'IL_RGB',
           'IL_TYPE_BIT', 'IL_BLIT_BLEND', 'IL_JPG_SAVE_FORMAT',
           'IL_LOADFAIL_BIT', 'fOpenWProc', 'IL_VTF', 'IL_SAVEPROC',
           'ilSaveImage', 'IL_LOADPROC', 'ilSavePal', 'IL_TIF',
           'IL_TGA_AUTHCOMMENT_STRING', 'IL_FORMAT_BIT',
           'IL_CUR_IMAGE', 'IL_CHEAD', 'IL_LUMINANCE', 'fCloseWProc',
           'ilCopyPixels', 'IL_IMAGE_OFFX', 'IL_IMAGE_OFFY',
           'IL_INVALID_FILE_HEADER', 'IL_IMAGE_PLANESIZE',
           'ilDisable', 'IL_INVALID_ENUM', 'ilOriginFunc',
           'ilRegisterOrigin', 'ilPushAttrib', 'ilSetWrite',
           'ilTypeFromExt', 'IL_TIF_HOSTCOMPUTER_STRING', 'IL_SGI',
           'IL_IMAGE_ORIGIN', 'ilCopyImage', 'ilEnable',
           'IL_PCD_PICNUM', 'IL_SAVE_EXT', 'IL_EXR', 'IL_PNG',
           'IL_FILE_BIT', 'IL_CUT', 'IL_BMP', 'IL_RAW',
           'IL_INVALID_CONVERSION', 'ilRegisterPal', 'IL_BMP_RLE',
           'ilBlit', 'IL_TIF_DOCUMENTNAME_STRING', 'IL_SUB_NEXT',
           'IL_TYPE_MODE', 'IL_PNG_AUTHNAME_STRING',
           'IL_FILE_READ_ERROR', 'ilRemoveSave', 'IL_LUMINANCE_ALPHA',
           'IL_ILLEGAL_OPERATION', 'IL_WDP', 'IL_DXT5', 'IL_JNG',
           'IL_NO_COMPRESSION', 'IL_PAL_BGR32', 'ilIsValidL',
           'ilLoadDataF', 'ilLoadData', 'fSeekRProc',
           'IL_LIB_MNG_ERROR', 'ilIsValidF', 'IL_PNG_TITLE_STRING',
           'ilDetermineTypeF', 'ilSaveL', 'ilSetData', 'ilInit',
           'IL_INVALID_PARAM', 'IL_LIB_PNG_ERROR', 'ilClearColour',
           'ilActiveMipmap', 'IL_DXT_NO_COMP', 'IL_OUT_OF_MEMORY',
           'ilLoadPal', 'ilModAlpha', 'IL_INVALID_VALUE', 'IL_ICNS',
           'IL_DXTC_FORMAT', 'IL_HALF', 'ilHint', 'ilIsImage',
           'ilConvertImage', 'IL_ALPHA', 'ilKeyColour',
           'ilResetMemory']
