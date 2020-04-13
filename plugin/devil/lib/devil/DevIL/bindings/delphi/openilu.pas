unit OpenILU;

//------------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2002 by Denton Woods
// Last modified: 06/23/2002 <--Y2K Compliant! =]
//
// Filename: il/ilu.h
//
// Description: The main include file for ILU
//
//------------------------------------------------------------------------------

{******************************************************************************}
{ Converted to Delphi by Alexander Blach (alexander@abee.de)                   }
{   Version:       ILU v1.6.1                                                  }
{   Last modified: 06/23/2002                                                  }
{******************************************************************************}


interface
uses
  OpenIL,
  Windows;

const
  ILU_VERSION_1_5_6 = 1;
  ILU_VERSION = 156;

  ILU_FILTER = $2600;
  ILU_NEAREST = $2601;
  ILU_LINEAR = $2602;
  ILU_BILINEAR = $2603;
  ILU_SCALE_BOX = $2604;
  ILU_SCALE_TRIANGLE = $2605;
  ILU_SCALE_BELL = $2606;
  ILU_SCALE_BSPLINE = $2607;
  ILU_SCALE_LANCZOS3 = $2608;
  ILU_SCALE_MITCHELL = $2609;


// Error types
const
  ILU_INVALID_ENUM = $0501;
  ILU_OUT_OF_MEMORY = $0502;
  ILU_INTERNAL_ERROR = $0504;
  ILU_INVALID_VALUE = $0505;
  ILU_ILLEGAL_OPERATION = $0506;
  ILU_INVALID_PARAM = $0509;

// Values
const
  ILU_PLACEMENT = $0700;
  ILU_LOWER_LEFT = $0701;
  ILU_LOWER_RIGHT = $0702;
  ILU_UPPER_LEFT = $0703;
  ILU_UPPER_RIGHT = $0704;
  ILU_CENTER = $0705;
  ILU_CONVOLUTION_MATRIX = $0710;
  ILU_VERSION_NUM = $0DE2;

// Filters
const
  ILU_FILTER_BLUR = $0803;
  ILU_FILTER_GAUSSIAN_3x3 = $0804;
  ILU_FILTER_GAUSSIAN_5x5 = $0805;
  ILU_FILTER_EMBOSS1 = $0807;
  ILU_FILTER_EMBOSS2 = $0808;
  ILU_FILTER_LAPLACIAN1 = $080A;
  ILU_FILTER_LAPLACIAN2 = $080B;
  ILU_FILTER_LAPLACIAN3 = $080C;
  ILU_FILTER_LAPLACIAN4 = $080D;
  ILU_FILTER_SHARPEN1 = $080E;
  ILU_FILTER_SHARPEN2 = $080F;
  ILU_FILTER_SHARPEN3 = $0810;

type
  PILinfo = ^TILinfo;
  TILinfo = record
    Id: TILuint;		// the image's id
    Data: TILuint;		// the image's data
    Width: TILuint;             // the image's width
    Height: TILuint;            // the image's height
    Depth: TILuint;	        // the image's depth
    Bpp: TILubyte;              // bytes per pixel (not bits) of the image
    SizeOfData: TILuint;	// the total size of the data (in bytes)
    Format: TILenum;		// image format (in IL enum style)
    _Type: TILenum;		// image type (in IL enum style)
    Origin: TILenum;		// origin of the image
    Palette: PILubyte;		// the image's palette
    PalType: TILenum;		// palette type
    PalSize: TILuint;           // palette size
    CubeFlags: TILenum;         // flags for what cube map sides are present
    NumNext: TILuint;		// number of images following
    NumMips: TILuint;           // number of mipmaps
    NumLayers: TILuint;		// number of layers
  end;



const
{$IFDEF OPENIL_DEBUG}
  ILUDLL = 'ILU-d.DLL';
{$ELSE}
  ILUDLL = 'ILU.DLL';
{$ENDIF}


// ImageLib Utility Functions
function iluAlienify: TILboolean; stdcall; external ILUDLL;
function iluBitFilter1: TILboolean; stdcall; external ILUDLL;
function iluBitFilter2: TILboolean; stdcall; external ILUDLL;
function iluBitFilter3: TILboolean; stdcall; external ILUDLL;
function iluBlurAvg(Iter: TILuint): TILboolean; stdcall; external ILUDLL;
function iluBlurGaussian(Iter: TILuint): TILboolean; stdcall; external ILUDLL;
function iluBuildMipmaps: TILboolean; stdcall; external ILUDLL;
function iluColoursUsed: TILuint; stdcall; external ILUDLL;
function iluColorsUsed: TILuint; stdcall; external ILUDLL name 'iluColoursUsed';
function iluCompareImage(Comp: TILuint): TILboolean; stdcall; external ILUDLL;
function iluContrast(Contrast: TILfloat): TILboolean; stdcall; external ILUDLL;
function iluCrop(XOff, YOff, ZOff, Width, Height, Depth: TILuint): TILboolean;
  stdcall; external ILUDLL;
procedure iluDeleteImage(Id: TILuint); stdcall; external ILUDLL;
function iluEdgeDetectE: TILboolean; stdcall; external ILUDLL;
function iluEdgeDetectP: TILboolean; stdcall; external ILUDLL;
function iluEdgeDetectS: TILboolean; stdcall; external ILUDLL;
function iluEmboss: TILboolean; stdcall; external ILUDLL;
function iluEnlargeCanvas(Width, Height, Depth: TILuint): TILboolean; stdcall;
  external ILUDLL;
function iluEnlargeImage(XDim, YDim, ZDim: TILfloat): TILboolean; stdcall;
  external ILUDLL;
function iluEqualize: TILboolean; stdcall; external ILUDLL;
function iluErrorString(Error: TILenum): PChar; stdcall; external ILUDLL;
function iluFlipImage: TILboolean; stdcall; external ILUDLL;
function iluGammaCorrect(Gamma: TILfloat): TILboolean; stdcall; external ILUDLL;
function iluGenImage: TILuint; stdcall; external ILUDLL;
procedure iluGetImageInfo(Info: PILinfo); stdcall; external ILUDLL;
function iluGetInteger(Mode: TILenum): TILint; stdcall; external ILUDLL;
procedure iluGetIntegerv(Mode: TILenum; var Param: TILint); stdcall; external ILUDLL;
function iluGetString(StringName: TILenum): PChar; stdcall; external ILUDLL;
procedure iluImageParameter(PName, Param: TILenum); stdcall; external ILUDLL;
procedure iluInit; stdcall; external ILUDLL;
function iluInvertAlpha: TILboolean; stdcall; external ILUDLL;
function iluLoadImage(FileName: PChar): TILuint; stdcall; external ILUDLL;
function iluMirror: TILboolean; stdcall; external ILUDLL;
function iluNegative: TILboolean; stdcall; external ILUDLL;
function iluNoisify(Factor: TILubyte): TILboolean; stdcall; external ILUDLL;
function iluPixelize(PixSize: TILuint): TILboolean; stdcall; external ILUDLL;
//procedure iluRegionf(ULx, ULy, BRx, BRy: TILfloat); stdcall; external ILUDLL;
//procedure iluRegioni(ULx, ULy, BRx, BRy: TILuint); stdcall; external ILUDLL;
function iluReplaceColour(Red, Green, Blue: TILubyte; Tolerance: TILfloat):
 TILboolean; stdcall; external ILUDLL;
function iluReplaceColor(Red, Green, Blue: TILubyte; Tolerance: TILfloat):
 TILboolean; stdcall; external ILUDLL name 'iluReplaceColour';
function iluRotate(Angle: TILfloat): TILboolean; stdcall; external ILUDLL;
function iluRotate3D(x, y, z, Angle: TILfloat): TILboolean; stdcall;
  external ILUDLL;
function iluSaturate1f(Saturation: TILfloat): TILboolean; stdcall; external ILUDLL;
function iluSaturate4f(r, g, b, Saturation: TILfloat): TILboolean; stdcall;
  external ILUDLL;
function iluScale(Width, Height, Depth: TILuint): TILboolean;
  stdcall; external ILUDLL;
function iluScaleColours(r, g, b: TILfloat): TILboolean; stdcall; external ILUDLL;
function iluScaleColors(r, g, b: TILfloat): TILboolean; stdcall;
  external ILUDLL name 'iluScaleColours';
function iluSharpen(Factor: TILfloat; Iter: TILuint): TILboolean; stdcall; external
  ILUDLL;
function iluSwapColours: TILboolean; stdcall; external ILUDLL;
function iluSwapColors: TILboolean; stdcall; external ILUDLL name 'iluSwapColours';
function iluWave(Angle: TILfloat): TILboolean; stdcall; external ILUDLL;

implementation

end.

