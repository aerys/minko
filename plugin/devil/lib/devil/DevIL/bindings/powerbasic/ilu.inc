'-----------------------------------------------------------------------------
'
' ImageLib Utility Sources
' Copyright (C) 2000-2002 by Denton Woods
' Last modified: 06/22/2002 <--Y2K Compliant! =]
'
' Filename: il/ilu.h
'
' Description: The main include file for OpenILU
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
' Filename: ilu.inc
'
' Description: PB main include file for OpenILU
'
' For information on use and download of OpenIL goto: http://openil.sourceforge.net/
' (Get docs and "End User Package")
'
' Report errors in the Powerbasic includes to e-mail above.
'
' For general help on Powerbasic and OpenIL, try the forums at:
' http://www.powerbasic.com
'-----------------------------------------------------------------------------

' #ifndef __ilu_h_
' #ifndef __ILU_H__

' %__ilu_h_
' %__ILU_H__

' #include <il/il.h>

' #ifdef __cplusplus
' extern "C" {
' #endif


' #ifdef _WIN32
'  #ifdef _MSC_VER
'  #ifndef _ILU_BUILD_LIBRARY
'  #ifdef _DEBUG
'  #pragma comment(lib, "ilu-d.lib")
'  #else
'  #pragma comment(lib, "ilu.lib")
'  #endif'_DEBUG
'  #endif'_ILU_BUILD_LIBRARY
'  #endif'_MSC_VER
' #endif'_WIN32


%ILU_VERSION_1_5_6 = 1
%ILU_VERSION = 156


%ILU_FILTER         = &H2600&
%ILU_NEAREST        = &H2601&
%ILU_LINEAR         = &H2602&
%ILU_BILINEAR       = &H2603&
%ILU_SCALE_BOX      = &H2604&
%ILU_SCALE_TRIANGLE = &H2605&
%ILU_SCALE_BELL     = &H2606&
%ILU_SCALE_BSPLINE  = &H2607&
%ILU_SCALE_LANCZOS3 = &H2608&
%ILU_SCALE_MITCHELL = &H2609&


' Error types
%ILU_INVALID_ENUM       = &H0501&
%ILU_OUT_OF_MEMORY      = &H0502&
%ILU_INTERNAL_ERROR     = &H0504&
%ILU_INVALID_VALUE      = &H0505&
%ILU_ILLEGAL_OPERATION  = &H0506&
%ILU_INVALID_PARAM      = &H0509&


' Values
%ILU_PLACEMENT          = &H0700&
%ILU_LOWER_LEFT         = &H0701&
%ILU_LOWER_RIGHT        = &H0702&
%ILU_UPPER_LEFT         = &H0703&
%ILU_UPPER_RIGHT        = &H0704&
%ILU_CENTER             = &H0705&
%ILU_CONVOLUTION_MATRIX = &H0710&
%ILU_VERSION_NUM        = &H0DE2&


' Filters
%ILU_FILTER_BLUR         = &H0803&
%ILU_FILTER_GAUSSIAN_3x3 = &H0804&
%ILU_FILTER_GAUSSIAN_5X5 = &H0805&
%ILU_FILTER_EMBOSS1      = &H0807&
%ILU_FILTER_EMBOSS2      = &H0808&
%ILU_FILTER_LAPLACIAN1   = &H080A&
%ILU_FILTER_LAPLACIAN2   = &H080B&
%ILU_FILTER_LAPLACIAN3   = &H080C&
%ILU_FILTER_LAPLACIAN4   = &H080D&
%ILU_FILTER_SHARPEN1     = &H080E&
%ILU_FILTER_SHARPEN2     = &H080F&
%ILU_FILTER_SHARPEN3     = &H0810&


' typedef struct ILinfo
Type ILinfo  Dword
    iId As Dword'ILuint Id; ' the image's id
    iData As Dword '(should this be Byte Ptr? )'ILubyte *Data; ' the image's data
    iWidth As Dword 'ILuint Width; ' the image's width
    iHeight As Dword 'ILuint Height; ' the image's height
    iDepth As Dword 'ILuint Depth; ' the image's depth
    iBpp As Byte'ILubyte Bpp; ' bytes per pixel (not bits) of the image
    iSizeOfData As Dword 'ILuint SizeOfData; ' the total size of the data (in bytes)
    iFormat As Dword 'ILenum Format; ' image format (in IL enum style)
    iType As Dword 'ILenum Type; ' image type (in IL enum style)
    iOrigin As Dword 'ILenum Origin; ' origin of the image
    iPalette As Long 'ILubyte *Palette; ' the image's palette
    iPalType As Dword 'ILenum PalType; ' palette type
    iPalSize As Dword 'ILuint PalSize; ' palette size
    iNumNext As Dword'ILuint NumNext; ' number of images following
    iNumMips As Dword 'ILuint NumMips; ' number of mipmaps
    iNumLayers As Dword'ILuint NumLayers; ' number of layers
End Type


' ImageLib Utility Functions
Declare Function iluAlienify Lib "ilu.dll" Alias "iluAlienify" () As Byte
Declare Function iluApplyProfile Lib "ilu.dll" Alias "iluApplyProfile" (ByRef pb_InProfile As Asciiz, ByRef pb_OutProfile As Asciiz) As Byte
Declare Function iluBlurAvg Lib "ilu.dll" Alias "iluBlurAvg" (ByVal pb_Iter As Dword) As Byte
Declare Function iluBlurGaussian Lib "ilu.dll" Alias "iluBlurGaussian" (ByVal pb_Iter As Dword) As Byte
Declare Function iluBuildMipmaps Lib "ilu.dll" Alias "iluBuildMipmaps" () As Byte
Declare Function iluColoursUsed Lib "ilu.dll" Alias "iluColoursUsed" () As Dword
Declare Function iluCompareImage Lib "ilu.dll" Alias "iluCompareImage" (ByVal pb_Comp As Dword) As Byte
Declare Function iluContrast Lib "ilu.dll" Alias "iluContrast" (ByVal pb_Contrast As Single) As Byte
Declare Function iluCrop Lib "ilu.dll" Alias "iluCrop" (ByVal pb_XOff As Dword, ByVal pb_YOff As Dword, ByVal pb_ZOff As Dword, ByVal pb_Width As Dword, ByVal pb_Height As Dword, ByVal pb_Depth As Dword) As Byte
Declare Sub iluDeleteImage Lib "ilu.dll" Alias "iluDeleteImage" (ByVal pb_Id As Dword)
Declare Function iluEdgeDetectE Lib "ilu.dll" Alias "iluEdgeDetectE" () As Byte
Declare Function iluEdgeDetectP Lib "ilu.dll" Alias "iluEdgeDetectP" () As Byte
Declare Function iluEdgeDetectS Lib "ilu.dll" Alias "iluEdgeDetectS" () As Byte
Declare Function iluEmboss Lib "ilu.dll" Alias "iluEmboss" () As Byte
Declare Function iluEnlargeCanvas Lib "ilu.dll" Alias "iluEnlargeCanvas" (ByVal pb_Width As Dword, ByVal pb_Height As Dword, ByVal pb_Depth As Dword) As Byte
Declare Function iluEnlargeImage Lib "ilu.dll" Alias "iluEnlargeImage" (ByVal pb_XDim As Single, ByVal pb_YDim As Single, ByVal pb_ZDim As Single) As Byte
Declare Function iluEqualize Lib "ilu.dll" Alias "iluEqualize" () As Byte
Declare Function iluErrorString Lib "ilu.dll" Alias "iluErrorString" (ByVal pb_Error As Dword) As String
Declare Function iluFlipImage Lib "ilu.dll" Alias "iluFlipImage" () As Byte
Declare Function iluGammaCorrect Lib "ilu.dll" Alias "iluGammaCorrect" (ByVal pb_Gamma As Single) As Byte
Declare Function iluGenImage Lib "ilu.dll" Alias "iluGenImage" () As Dword
Declare Sub iluGetImageInfo Lib "ilu.dll" Alias "iluGetImageInfo" (ByRef pb_Info As ILinfo)
Declare Function iluGetInteger Lib "ilu.dll" Alias "iluGetInteger" (ByVal pb_Mode As Dword) As Long
Declare Sub iluGetIntegerv Lib "ilu.dll" Alias "iluGetIntegerv" (ByVal pb_Mode As Dword, ByRef pb_Param As Long)
Declare Function iluGetString Lib "ilu.dll" Alias "iluGetString" (ByVal pb_StringName As Dword) As String
Declare Sub iluInit Lib "ilu.dll" Alias "iluInit" ()
Declare Function iluInvertAlpha Lib "ilu.dll" Alias "iluInvertAlpha" () As Byte
Declare Function iluLoadImage Lib "ilu.dll" Alias "iluLoadImage" (ByRef pb_FileName As Asciiz) As Dword
Declare Sub iluImageParameter Lib "ilu.dll" Alias "iluImageParameter" (ByVal pb_PName As Dword, ByVal pb_Param As Dword)
Declare Function iluMirror Lib "ilu.dll" Alias "iluMirror" () As Byte
Declare Function iluNegative Lib "ilu.dll" Alias "iluNegative" () As Byte
Declare Function iluNoisify Lib "ilu.dll" Alias "iluNoisify" (ByVal pb_Tolerance As Single) As Byte
Declare Function iluPixelize Lib "ilu.dll" Alias "iluPixelize" (ByVal pb_PixSize As Dword) As Byte
'Declare Sub iluRegionf Lib "ilu.dll" Alias "iluRegionf" (ByVal pb_ULx As Single, ByVal pb_ULy As Single, ByVal pb_BRx As Single, ByVal pb_BRy As Single)
'Declare Sub iluRegioni Lib "ilu.dll" Alias "iluRegioni" (ByVal pb_ULx As Dword, ByVal pb_ULy As Dword, ByVal pb_BRx As Dword, ByVal pb_BRy As Dword)
Declare Function iluReplaceColour Lib "ilu.dll" Alias "iluReplaceColour" (ByVal pb_Red As Byte, ByVal pb_Green As Byte, ByVal pb_Blue As Byte, ByVal pb_Tolerance As Single) As Byte
Declare Function iluRotate Lib "ilu.dll" Alias "iluRotate" (ByVal pb_Angle As Single) As Byte
Declare Function iluRotate3D Lib "ilu.dll" Alias "iluRotate3D" (ByVal pb_x As Single, ByVal pb_y As Single, ByVal pb_z As Single, ByVal pb_Angle As Single) As Byte
Declare Function iluSaturate1f Lib "ilu.dll" Alias "iluSaturate1f" (ByVal pb_Saturation As Single) As Byte
Declare Function iluSaturate4f Lib "ilu.dll" Alias "iluSaturate4f" (ByVal pb_r As Single, ByVal pb_g As Single, ByVal pb_b As Single, ByVal pb_Saturation As Single) As Byte
Declare Function iluScale Lib "ilu.dll" Alias "iluScale" (ByVal pb_Width As Dword, ByVal pb_Height As Dword, ByVal pb_Depth As Dword) As Byte
Declare Function iluScaleColours Lib "ilu.dll" Alias "iluScaleColours" (ByVal pb_r As Single, ByVal pb_g As Single, ByVal pb_b As Single) As Byte
Declare Function iluSharpen Lib "ilu.dll" Alias "iluSharpen" (ByVal pb_Factor As Single, ByVal pb_Iter As Dword) As Byte
Declare Function iluSwapColours Lib "ilu.dll" Alias "iluSwapColours" () As Byte
Declare Function iluWave Lib "ilu.dll" Alias "iluWave" (ByVal pb_Angle As Single) As Byte

'%iluColorsUsed = iluColoursUsed
'%iluSwapColors = iluSwapColours


' #ifdef __cplusplus
' }
' #endif

' #endif /* __ILU_H__ */
' #endif /* __ilu_h_ */
