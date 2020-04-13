//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 02/28/2009
//
// Filename: src-IL/include/il_vtf.h
//
// Description: Reads from and writes to a Valve Texture Format (.vtf) file.
//                These are used in Valve's Source games.  VTF specs available
//                from http://developer.valvesoftware.com/wiki/VTF.
//
//-----------------------------------------------------------------------------


#ifndef VTF_H
#define VTF_H

#include "il_internal.h"

#ifdef _MSC_VER
#pragma pack(push, vtf_struct, 1)
#elif defined(MACOSX) || defined(__GNUC__)
#pragma pack(1)
#endif

typedef struct VTFHEAD
{
	ILubyte		Signature[4];		// File signature ("VTF\0").
	ILuint		Version[2];			// version[0].version[1] (currently 7.2).
	ILuint		HeaderSize;			// Size of the header struct (16 byte aligned; currently 80 bytes).
	ILushort	Width;				// Width of the largest mipmap in pixels. Must be a power of 2.
	ILushort	Height;				// Height of the largest mipmap in pixels. Must be a power of 2.
	ILuint		Flags;				// VTF flags.
	ILushort	Frames;				// Number of frames, if animated (1 for no animation).
	ILushort	FirstFrame;			// First frame in animation (0 based).
	ILubyte		Padding0[4];		// reflectivity padding (16 byte alignment).
	ILfloat		Reflectivity[3];	// reflectivity vector.
	ILubyte		Padding1[4];		// reflectivity padding (8 byte packing).
	ILfloat		BumpmapScale;		// Bumpmap scale.
	ILuint		HighResImageFormat;	// High resolution image format.
	ILubyte		MipmapCount;		// Number of mipmaps.
	ILuint		LowResImageFormat;	// Low resolution image format (always DXT1).
	ILubyte		LowResImageWidth;	// Low resolution image width.
	ILubyte		LowResImageHeight;	// Low resolution image height.
	ILushort	Depth;				// Depth of the largest mipmap in pixels.
									// Must be a power of 2. Can be 0 or 1 for a 2D texture (v7.2 only).
} IL_PACKSTRUCT VTFHEAD;

#if defined(MACOSX) || defined(__GNUC__)
#pragma pack()
#elif _MSC_VER
#pragma pack(pop, vtf_struct)
#endif

enum
{
	IMAGE_FORMAT_NONE = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888
};

enum
{
	TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
	TEXTUREFLAGS_TRILINEAR = 0x00000002,
	TEXTUREFLAGS_CLAMPS = 0x00000004,
	TEXTUREFLAGS_CLAMPT = 0x00000008,
	TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
	TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
	TEXTUREFLAGS_NOCOMPRESS = 0x00000040,
	TEXTUREFLAGS_NORMAL = 0x00000080,
	TEXTUREFLAGS_NOMIP = 0x00000100,
	TEXTUREFLAGS_NOLOD = 0x00000200,
	TEXTUREFLAGS_MINMIP = 0x00000400,
	TEXTUREFLAGS_PROCEDURAL = 0x00000800,
	TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
	TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,
	TEXTUREFLAGS_ENVMAP = 0x00004000,
	TEXTUREFLAGS_RENDERTARGET = 0x00008000,
	TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
	TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
	TEXTUREFLAGS_SINGLECOPY = 0x00040000,
	TEXTUREFLAGS_ONEOVERMIPLEVELINALPHA = 0x00080000,
	TEXTUREFLAGS_PREMULTCOLORBYONEOVERMIPLEVEL = 0x00100000,
	TEXTUREFLAGS_NORMALTODUDV = 0x00200000,
	TEXTUREFLAGS_ALPHATESTMIPGENERATION = 0x00400000,
	TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,
	TEXTUREFLAGS_NICEFILTERED = 0x01000000,
	TEXTUREFLAGS_CLAMPU = 0x02000000
};



// Internal functions
ILboolean	iIsValidVtf(void);
ILboolean	iGetVtfHead(VTFHEAD *Header);
ILboolean	iCheckVtf(VTFHEAD *Header);
ILboolean	iLoadVtfInternal(void);
ILboolean	VtfInitFacesMipmaps(ILimage *BaseImage, ILuint NumFaces, VTFHEAD *Header);
ILboolean	VtfInitMipmaps(ILimage *BaseImage, VTFHEAD *Header);
ILboolean	VtfReadData(void);
ILboolean	VtfDecompressDXT1(ILimage *Image);
ILboolean	VtfDecompressDXT5(ILimage *Image);
ILboolean	iSaveVtfInternal(void);

#endif//VTF_H
