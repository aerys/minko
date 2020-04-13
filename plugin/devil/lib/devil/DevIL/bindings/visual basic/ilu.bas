Attribute VB_Name = "DevIL_manipulate"
'-----------------------------------------------------------------------------
'
' ImageLib Utility Sources
' Copyright (C) 2000-2002 by Denton Woods
' Converted from ilu.h by Rune Kock (rune@vupti.com)
' based on the earlier conversion by Timo Heister (Timo-Heister@gmx.de)
' Last modified:  22 June 2002, based on ilu.h dated 06/22/2002
'
' Filename: ilu.bas
'
' Description:  The main include file for ILU
'
'-----------------------------------------------------------------------------

' I've converted the types as follows.  I have not dared use user-defined types, as I
' don't know how VB handles them internally.  This means, unfortunately, that the headers
' are less informative than the original.  So refer to ilu.h to see the original types.

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
' ILHANDLE    long


Option Explicit

Public Const ILU_VERSION_1_6_0 = 1
Public Const ILU_VERSION = 160


Public Const ILU_FILTER = &H2600
Public Const ILU_NEAREST = &H2601
Public Const ILU_LINEAR = &H2602
Public Const ILU_BILINEAR = &H2603
Public Const ILU_SCALE_BOX = &H2604
Public Const ILU_SCALE_TRIANGLE = &H2605
Public Const ILU_SCALE_BELL = &H2606
Public Const ILU_SCALE_BSPLINE = &H2607
Public Const ILU_SCALE_LANCZOS3 = &H2608
Public Const ILU_SCALE_MITCHELL = &H2609


' Error types
Public Const ILU_INVALID_ENUM = &H501
Public Const ILU_OUT_OF_MEMORY = &H502
Public Const ILU_INTERNAL_ERROR = &H504
Public Const ILU_INVALID_VALUE = &H505
Public Const ILU_ILLEGAL_OPERATION = &H506
Public Const ILU_INVALID_PARAM = &H509


' Values
Public Const ILU_PLACEMENT = &H700
Public Const ILU_LOWER_LEFT = &H701
Public Const ILU_LOWER_RIGHT = &H702
Public Const ILU_UPPER_LEFT = &H703
Public Const ILU_UPPER_RIGHT = &H704
Public Const ILU_CENTER = &H705
Public Const ILU_CONVOLUTION_MATRIX = &H710
Public Const ILU_VERSION_NUM = &HDE2


' Filters
Public Const ILU_FILTER_BLUR = &H803
Public Const ILU_FILTER_GAUSSIAN_3x3 = &H804
Public Const ILU_FILTER_GAUSSIAN_5X5 = &H805
Public Const ILU_FILTER_EMBOSS1 = &H807
Public Const ILU_FILTER_EMBOSS2 = &H808
Public Const ILU_FILTER_LAPLACIAN1 = &H80A
Public Const ILU_FILTER_LAPLACIAN2 = &H80B
Public Const ILU_FILTER_LAPLACIAN3 = &H80C
Public Const ILU_FILTER_LAPLACIAN4 = &H80D
Public Const ILU_FILTER_SHARPEN1 = &H80E
Public Const ILU_FILTER_SHARPEN2 = &H80F
Public Const ILU_FILTER_SHARPEN3 = &H810


Public Type ILinfo
  ID As Long          ' the image's id
  DataPointer As Long ' pointer to the image's data (useless in VB)
  width As Long       ' the image's width
  height As Long      ' the image's height
  Depth As Long       ' the image's depth
  Bpp As Byte         ' bytes per pixel (not bits) of the image
  SizeOfData As Long  ' the total size of the data (in bytes)
  Format As Long      ' image format (in IL enum style)
  IType As Long       ' image type (in IL enum style)
  Origin As Long      ' origin of the image
  PalettePointer As Long ' pointer to the image's palette (useless in VB)
  PalType As Long     ' palette type
  PalSize As Long     ' palette size
  NumNext As Long     ' number of images following
  NumMips As Long     ' number of mipmaps
  NumLayers  As Long  ' number of layers
End Type


' ImageLib Utility Functions
Public Declare Function iluAlienify Lib "ilu" () As Byte
Public Declare Function iluBitFilter1 Lib "ilu" () As Byte
Public Declare Function iluBitFilter2 Lib "ilu" () As Byte
Public Declare Function iluBitFilter3 Lib "ilu" () As Byte
Public Declare Function iluBlurAvg Lib "ilu" (ByVal Iter As Long) As Byte
Public Declare Function iluBlurGaussian Lib "ilu" (ByVal Iter As Long) As Byte
Public Declare Function iluBuildMipmaps Lib "ilu" () As Byte
Public Declare Function iluColoursUsed Lib "ilu" () As Long
Public Declare Function iluCompareImage Lib "ilu" (ByVal Comp As Long) As Byte
Public Declare Function iluContrast Lib "ilu" (ByVal Contrast As Single) As Byte
Public Declare Function iluCrop Lib "ilu" (ByVal XOff As Long, ByVal YOff As Long, ByVal ZOff As Long, ByVal width As Long, ByVal height As Long, ByVal Depth As Long) As Byte
Public Declare Sub iluDeleteImage Lib "ilu" (ByVal ID As Long)
Public Declare Function iluEdgeDetectE Lib "ilu" () As Byte
Public Declare Function iluEdgeDetectP Lib "ilu" () As Byte
Public Declare Function iluEdgeDetectS Lib "ilu" () As Byte
Public Declare Function iluEmboss Lib "ilu" () As Byte
Public Declare Function iluEnlargeCanvas Lib "ilu" (ByVal width As Long, ByVal height As Long, ByVal Depth As Long) As Byte
Public Declare Function iluEnlargeImage Lib "ilu" (ByVal XDim As Single, ByVal YDim As Single, ByVal ZDim As Single) As Byte
Public Declare Function iluEqualize Lib "ilu" () As Byte
Public Declare Function iluErrorString Lib "ilu" (ByVal Error As Long) As Long
Public Declare Function iluFlipImage Lib "ilu" () As Byte
Public Declare Function iluGammaCorrect Lib "ilu" (ByVal Gamma As Single) As Byte
Public Declare Function iluGenImage Lib "ilu" () As Long
Public Declare Sub iluGetImageInfo Lib "ilu" (ByRef Info As ILinfo)
Public Declare Function iluGetInteger Lib "ilu" (ByVal Mode As Long) As Long
Public Declare Sub iluGetIntegerv Lib "ilu" (ByVal Mode As Long, ByRef Param As Long)
Public Declare Function iluGetString Lib "ilu" (ByVal StringName As Long) As Long
Public Declare Sub iluImageParameter Lib "ilu" (ByVal PName As Long, ByVal Param As Long)
Public Declare Sub iluInit Lib "ilu" ()
Public Declare Function iluLoadImage Lib "ilu" (ByVal FileName As String) As Long
Public Declare Function iluMirror Lib "ilu" () As Byte
Public Declare Function iluNegative Lib "ilu" () As Byte
Public Declare Function iluNoisify Lib "ilu" (ByVal Tolerance As Single) As Byte
Public Declare Function iluPixelize Lib "ilu" (ByVal PixSize As Long) As Byte
' Public Declare Sub iluRegionf Lib "ilu" (ByVal ULx As Single, ByVal ULy As Single, ByVal BRx As Single, ByVal BRy As Single)
' Public Declare Sub iluRegioni Lib "ilu" (ByVal ULx As Long, ByVal ULy As Long, ByVal BRx As Long, ByVal BRy As Long)
Public Declare Function iluReplaceColour Lib "ilu" (ByVal Red As Byte, ByVal Green As Byte, ByVal Blue As Byte, ByVal Tolerance As Single) As Byte
Public Declare Function iluRotate Lib "ilu" (ByVal Angle As Single) As Byte
Public Declare Function iluRotate3D Lib "ilu" (ByVal x As Single, ByVal y As Single, ByVal z As Single, ByVal Angle As Single) As Byte
Public Declare Function iluSaturate1f Lib "ilu" (ByVal Saturation As Single) As Byte
Public Declare Function iluSaturate4f Lib "ilu" (ByVal r As Single, ByVal g As Single, ByVal b As Single, ByVal Saturation As Single) As Byte
Public Declare Function iluScale Lib "ilu" (ByVal width As Long, ByVal height As Long, ByVal Depth As Long) As Byte
Public Declare Function iluScaleColours Lib "ilu" (ByVal r As Single, ByVal g As Single, ByVal b As Single) As Byte
Public Declare Function iluSharpen Lib "ilu" (ByVal Factor As Single, ByVal Iter As Long) As Byte
Public Declare Function iluSwapColours Lib "ilu" () As Byte
Public Declare Function iluWave Lib "ilu" (ByVal Angle As Single) As Byte

Public Declare Function iluColorsUsed Lib "ilu" Alias "iluColoursUsed" () As Long
Public Declare Function iluSwapColors Lib "ilu" Alias "iluSwapColours" () As Byte
Public Declare Function iluReplaceColor Lib "ilu" Alias "iluReplaceColour" (ByVal Red As Byte, ByVal Green As Byte, ByVal Blue As Byte, ByVal Tolerance As Single) As Byte
Public Declare Function iluScaleColors Lib "ilu" Alias "iluScaleColours" (ByVal r As Single, ByVal g As Single, ByVal b As Single) As Byte
