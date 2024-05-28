/*=============================================================================
Interface to Qualcomm Texture Conversion and Compression Library

                Copyright (c) 2013 Qualcomm Technologies, Inc.
                           All Rights Reserved.
=============================================================================*/

#ifndef _TEXTURECONVERTER_H_
#define _TEXTURECONVERTER_H_

#if defined(_WIN32) || defined(_WIN64)
#define DllImport __declspec ( dllexport )
#else
#define DllImport
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

DllImport unsigned short UByteToUInt16(unsigned char nByte);
DllImport short UByteToInt16(unsigned char nByte);

//=============================================================================
// CONSTS/ENUMS
//=============================================================================
//
// TQonvertImage.nFormat: Image format 
//
// Paletted formats are no longer supported and are included for legacy purposes only
DllImport enum 
{
	// General formats
    Q_FORMAT_RGBA_8UI = 1,
    Q_FORMAT_RGBA_8I,
    Q_FORMAT_RGB5_A1UI,
    Q_FORMAT_RGBA_4444,
    Q_FORMAT_RGBA_16UI,
    Q_FORMAT_RGBA_16I,
    Q_FORMAT_RGBA_32UI,
    Q_FORMAT_RGBA_32I,

    Q_FORMAT_PALETTE_8_RGBA_8888,
    Q_FORMAT_PALETTE_8_RGBA_5551,
    Q_FORMAT_PALETTE_8_RGBA_4444,
    Q_FORMAT_PALETTE_4_RGBA_8888,
    Q_FORMAT_PALETTE_4_RGBA_5551,
    Q_FORMAT_PALETTE_4_RGBA_4444,
    Q_FORMAT_PALETTE_1_RGBA_8888,
    Q_FORMAT_PALETTE_8_RGB_888,
    Q_FORMAT_PALETTE_8_RGB_565,
    Q_FORMAT_PALETTE_4_RGB_888,
    Q_FORMAT_PALETTE_4_RGB_565,

    Q_FORMAT_R2_GBA10UI,
    Q_FORMAT_RGB10_A2UI,
    Q_FORMAT_RGB10_A2I,
    Q_FORMAT_RGBA_F,
    Q_FORMAT_RGBA_HF,

    Q_FORMAT_RGB9_E5,   // Last five bits are exponent bits (Read following section in GLES3 spec: "3.8.17 Shared Exponent Texture Color Conversion")
    Q_FORMAT_RGB_8UI,
    Q_FORMAT_RGB_8I,
    Q_FORMAT_RGB_565,
    Q_FORMAT_RGB_16UI,
    Q_FORMAT_RGB_16I,
    Q_FORMAT_RGB_32UI,
    Q_FORMAT_RGB_32I,

    Q_FORMAT_RGB_F,
    Q_FORMAT_RGB_HF,
    Q_FORMAT_RGB_11_11_10_F,

    Q_FORMAT_RG_F,
    Q_FORMAT_RG_HF,
    Q_FORMAT_RG_32UI,
    Q_FORMAT_RG_32I,
    Q_FORMAT_RG_16I,               
    Q_FORMAT_RG_16UI,
    Q_FORMAT_RG_8I,
    Q_FORMAT_RG_8UI,
    Q_FORMAT_RG_S88, 

    Q_FORMAT_R_32UI,
    Q_FORMAT_R_32I,
    Q_FORMAT_R_F,                 
    Q_FORMAT_R_16F,                 
    Q_FORMAT_R_16I,                
    Q_FORMAT_R_16UI,                
    Q_FORMAT_R_8I,
    Q_FORMAT_R_8UI,

    Q_FORMAT_LUMINANCE_ALPHA_88,
    Q_FORMAT_LUMINANCE_8,
    Q_FORMAT_ALPHA_8,
    
    Q_FORMAT_LUMINANCE_ALPHA_F,
    Q_FORMAT_LUMINANCE_F,
    Q_FORMAT_ALPHA_F,
    Q_FORMAT_LUMINANCE_ALPHA_HF,
    Q_FORMAT_LUMINANCE_HF,
    Q_FORMAT_ALPHA_HF,
    Q_FORMAT_DEPTH_16,
    Q_FORMAT_DEPTH_24,
    Q_FORMAT_DEPTH_24_STENCIL_8,
    Q_FORMAT_DEPTH_32,
    
    Q_FORMAT_BGR_565,             
    Q_FORMAT_BGRA_8888,
    Q_FORMAT_BGRA_5551,           
    Q_FORMAT_BGRX_8888,           
    Q_FORMAT_BGRA_4444,           
    // Compressed formats
    Q_FORMAT_ATITC_RGBA,
    Q_FORMAT_ATC_RGBA_EXPLICIT_ALPHA = Q_FORMAT_ATITC_RGBA,
    Q_FORMAT_ATITC_RGB,
    Q_FORMAT_ATC_RGB = Q_FORMAT_ATITC_RGB,
    Q_FORMAT_ATC_RGBA_INTERPOLATED_ALPHA,
    Q_FORMAT_ETC1_RGB8,
    Q_FORMAT_3DC_X,
    Q_FORMAT_3DC_XY,
    
    Q_FORMAT_ETC2_RGB8,
    Q_FORMAT_ETC2_RGBA8,
    Q_FORMAT_ETC2_RGB8_PUNCHTHROUGH_ALPHA1,
    Q_FORMAT_ETC2_SRGB8,
    Q_FORMAT_ETC2_SRGB8_ALPHA8,
    Q_FORMAT_ETC2_SRGB8_PUNCHTHROUGH_ALPHA1,
    Q_FORMAT_EAC_R_SIGNED,
    Q_FORMAT_EAC_R_UNSIGNED,
    Q_FORMAT_EAC_RG_SIGNED,
    Q_FORMAT_EAC_RG_UNSIGNED,

    Q_FORMAT_S3TC_DXT1_RGB,
    Q_FORMAT_S3TC_DXT1_RGBA,
    Q_FORMAT_S3TC_DXT3_RGBA,
    Q_FORMAT_S3TC_DXT5_RGBA,
	
    // YUV formats
    Q_FORMAT_AYUV_32,
    Q_FORMAT_I444_24,
    Q_FORMAT_YUYV_16,
    Q_FORMAT_UYVY_16,
    Q_FORMAT_I420_12,
    Q_FORMAT_YV12_12,
    Q_FORMAT_NV21_12,
    Q_FORMAT_NV12_12,

    // ASTC Format
    Q_FORMAT_ASTC_8,
    Q_FORMAT_ASTC_16,
};

//
// TFormatFlags.nEncodeFlag: Image encoding flags 
//
DllImport enum TEncodeFlag
{
    Q_FLAG_ENCODE_NONE = 0,
    Q_FLAG_ENCODE_DEFAULT = 0,
    Q_FLAG_ENCODE_ATITC_FAST,
};

//
// TFormatFlags.nScaleFilter: Scaling option to use when creating scaled output (for example for mipmap generation)
//
DllImport enum TScaleFilterFlag
{
    Q_FLAG_SCALEFILTER_DEFAULT = 0,
    Q_FLAG_SCALEFILTER_NEAREST,
    Q_FLAG_SCALEFILTER_MEAN, 
    Q_FLAG_SCALEFILTER_BILINEAR,
    Q_FLAG_SCALEFILTER_BICUBIC,
    Q_FLAG_SCALEFILTER_KAISER,
};

//
// TFormatFlags.nNormalMap: Describes the algorithm to use for creating normal maps (for bumpmapping)
//
DllImport enum TNormalMapFlag
{
    Q_FLAG_NORMALMAP_NONE = 0,
    Q_FLAG_NORMALMAP_ROBERTSCROSS,
    Q_FLAG_NORMALMAP_SOBEL,
    Q_FLAG_NORMALMAP_PREWITTGRADIENT,
};

//
// TFormatFlags.nDebugFlag: Debug options
//
DllImport enum TDebugFlags
{
    Q_FLAG_DEBUG_DEFAULT         = 0,
    Q_FLAG_DEBUG_DISABLE_VERBOSE = 0x00000001,  // Disable verbose error output to stderr
    Q_FLAG_DEBUG_VERSION         = 0x00000002,  // Return the library version (MAJOR<<16 | MINOR) as an unsigned int in pOutput->pData
};

//
// Qonvert function Success and Error return values
//
DllImport enum TReturnCode
{
    Q_SUCCESS = 0,
    Q_ERROR_UNSUPPORTED_DIMENSIONS,
    Q_ERROR_UNSUPPORTED_SRC_FORMAT,
    Q_ERROR_UNSUPPORTED_DST_FORMAT,
    Q_ERROR_UNSUPPORTED_SRC_FORMAT_FLAG,
    Q_ERROR_UNSUPPORTED_DST_FORMAT_FLAG,
    Q_ERROR_INCORRECT_SRC_PARAMETER,
    Q_ERROR_INCORRECT_DST_PARAMETER,
    Q_ERROR_INCORRECT_DATASIZE,
    Q_ERROR_OTHER,
};

//=============================================================================
// TYPEDEFS 
//=============================================================================

//
// Additional format flags, leaving any value to 0 means the library will use a proper default value
//
DllImport typedef struct _TFormatFlags
{
    unsigned int nStride;           // Specify if the image stride is different then the default (bpp*width)
    unsigned int nMaskRed;          // Specify which of the bits in a color are red, can be used for swizzled color modes like (BGR instead of RGB)
    unsigned int nMaskGreen;        // Specify which of the bits in a color are green, can be used for swizzled color modes like (BGR instead of RGB)
    unsigned int nMaskBlue;         // Specify which of the bits in a color are blue, can be used for swizzled color modes like (BGR instead of RGB)
    unsigned int nMaskAlpha;        // Specify which of the bits in a color are alpha, can be used for swizzled color modes like (BGR instead of RGB)
    unsigned int nFlipX;            // Flip image horizontally
    unsigned int nFlipY;            // Flip image vertically
    unsigned int nEncodeFlag;       // Specifying image encoding options (see TEncodeFlag above)
    unsigned int nScaleFilterFlag;  // TScaleFilterFlag specifying the scale filter used when scaling the source image to the destination image (for mipmapping)
    unsigned int nNormalMapFlag;    // TNormalMapFlag specify if a normal map should be created (for destination images only) 
    unsigned int nNormalMapScale;   // Specify the scale when creating a normalmap (0 = no scaling)
    unsigned int nNormalMapWrap;    // Specify if wrapping is enabled when creating a normalmap (0 = clamp to border, 1 = wrap)
    unsigned int nDebugFlags;       // Debug flags (logical OR TDebugFlags)
} TFormatFlags;

//
// Basic image format description
//
DllImport typedef struct _TQonvertImage
{
//    DllImport _TQonvertImage & operator=(const _TQonvertImage &rhs);
    
    unsigned int    nWidth;          // Image width
    unsigned int    nHeight;         // Image height
    unsigned int    nFormat;         // Image format, see Q_FORMAT_xxx enums above.
    TFormatFlags*   pFormatFlags;    // Points to additional image format flags, can be NULL (will assume default values).
    unsigned int    nDataSize;       // Size of the pData buffer
    unsigned char*  pData;           // Image data
    void *compressionOptions;
} TQonvertImage;

DllImport _TQonvertImage * NewTQonvertImage(_TQonvertImage *);

//=============================================================================
// FUNCTION PROTOTYPES
//=============================================================================

//-----------------------------------------------------------------------------
// Qonvert
//
// Convert the input texture to the output texture format. Typically used to convert
// simple image formats into OpenGL ES supported compression formats, like ATITC
// supported on the Conversion Platform MSM series chips.
//
// If pOutput->pData is set to NULL, the pOutput->nDataSize will be calculated based 
// on the pOutput format information. With this information the application is expected
// to create a memory block for pOutput->pData. 
// 
// Return: 
//   0     == SUCCESS
//   other == ERROR CODE
//-----------------------------------------------------------------------------
DllImport extern unsigned int Qonvert( 
    const TQonvertImage*    pSrcImg,  // source image
    TQonvertImage*          pDstImg,  // destination image
    void *                  options = NULL
);

DllImport enum CompressionOptionsType
{
    CompType_None = 0,
    CompType_ASTC,
    CompType_ASTC_16,
    CompType_Count
};

DllImport enum ASTCCompressionOptionsSpeed{
    ASTC_EXHAUSTIVE = 0,
    ASTC_THOROUGH,
    ASTC_MEDIUM,
    ASTC_FAST,
    ASTC_VERY_FAST  
};

DllImport enum ASTCCompressionOptionsMode{
    ASTC_HDR = 0,
    ASTC_SRGB,
    ASTC_LINEAR
};

DllImport enum ASTCCompressionOptionsDefaultOrCustom{
    DEFAULT = 0,
    CUSTOM
};

DllImport enum ASTCCompressionOptionsUseBitRate{
    USE_BITRATE = 0,
    USE_BLOCK_DIMENSIONS
};

DllImport enum ASTCCompressionOptionsCommand{
    COMPRESS = 0,
    DECOMPRESS
};

DllImport enum ASTC_BIT_FIDELITY{
	ASTC_8_BIT = 1,
	ASTC_16_BIT,
	ASTC_32_BIT,
};

DllImport typedef struct _ASTCOptions
{
    //These variables are ASTC specific. 
    int     CompressionSpeed;
    int     CompressionMode;
    int     UseBitRate;
    float   BitRate;
    int     xDimension;
    int     yDimension;

    int     UseBuiltInOptimizations;
    int     ApplyPSNROptimization;
    int     ApplyPerceptualOptimization;
    int     ApplyMaskOptimization;
    int     ApplyAlphaOptimization;
    int     ApplyHDROptimization;
    int     ApplyHDRWithAlphaChannelOptimization;
    int     ApplyHDRForLogErrorOptimization;
    int     ApplyHDRWithAlphaChannelForLogErrorOptimization;

    int                     BitFidelity;
    int                     ASTCCompressionMode;
    unsigned char*          astc_pData;
    const TQonvertImage*	astc_pSrc;
	TQonvertImage *			astc_pDest;
} ASTCOptions;

DllImport typedef struct _StandardOptions
{
    int     ImageType;          //Is this a CompressedImage or a DifferenceImage (cacheing only)
    int     CompressionType;    //For help with typecasting the pointer below.
    void *  CompressionOptions; //The pointer to the compression Options.
}StandardOptions;

DllImport _ASTCOptions *NewASTCOptions();
DllImport _ASTCOptions *NewASTCOptionsCopy(_ASTCOptions *);
DllImport bool ASTCOptionsEqual(_ASTCOptions *me, _ASTCOptions *other);
DllImport _StandardOptions *NewStandardOptions(int,int,void *);
DllImport _StandardOptions *NewStandardOptionsCopy(_StandardOptions *options);

#ifdef __cplusplus
};
#endif

#endif  //_TEXTURECONVERTER_H_