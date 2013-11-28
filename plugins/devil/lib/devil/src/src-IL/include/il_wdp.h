//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 09/26/2008
//
// Filename: src-IL/include/il_hdr.h
//
// Description: Reads a Microsoft HD Photo (.wdp)
//
//-----------------------------------------------------------------------------


#ifndef WDP_H
#define WDP_H

#include "il_internal.h"


//
// Structures
//
typedef struct WDPHEAD
{
	ILubyte		Encoding[2];
	ILubyte		UniqueID;
	ILubyte		Version;
	ILuint		Offset;
} WDPHEAD;

typedef struct WDPIFD
{
	ILushort	Tag;
	ILushort	Type;
	ILuint		Count;
	ILuint		ValOff;
	ILuint		NextOff;
} WDPIFD;

typedef struct WDPGUID
{
	ILuint		First;
	ILushort	Second;
	ILushort	Third;
	ILuint		Fourth;
	ILuint		Fifth;
} WDPGUID;

typedef struct WDPIMGHEAD
{
	ILuint		GDISignature[2];
	ILuint		Codec;
	ILubyte		Flags[2];
	ILubyte		Format;
	ILuint		Width, Height;  // Can either be short or 32-bit int
	ILuint		VertTiles, HorzTiles;  // 12-bits each
	ILushort	*TileWidth, *TileHeight;  // 8 or 16-bits each
	ILubyte		*TileStretch;  // 8-bits each
	ILubyte		ExtraPixels[3];
} WDPIMGHEAD;

typedef struct WDPIMGPLANE
{
	ILubyte		Flags1;
	ILubyte		Color;
	ILubyte		Bayer;
	ILubyte		ShiftBits;
	ILubyte		Mantissa;
	ILubyte		Expbias;
	ILubyte		Flags2;

	ILubyte		NumChannels;
} WDPIMGPLANE;

typedef struct WDPDCQUANT
{
	ILubyte		ChMode;
	ILubyte		DcQuant;
	ILubyte		DcQuantY;
	ILubyte		DcQuantUV;
	ILubyte		DcQuantChan;
} WDPDCQUANT;

typedef struct WDPTILE
{
	ILuint		StartCode;
	ILubyte		HashAndType;
} WDPTILE;

//
// Image header defines
//

// Codec and sub-codec
#define WDP_CODEC			0xF0
#define WDP_SUBCODEC		0x0F

// First set of flags
#define WDP_TILING_FLAG		0x80
#define WDP_BITSTREAM_FMT	0x40
#define WDP_ORIENTATION		0x38
#define WDP_INDEXTABLE		0x04
#define WDP_OVERLAP			0x03

// Second set of flags
#define WDP_SHORT_HEADER	0x80
#define WDP_LONG_WORD		0x40
#define WDP_WINDOWING		0x20
#define WDP_TRIM_FLEXBITS	0x10
#define WDP_TILE_STRETCH	0x08
#define WDP_ALPHACHANNEL	0x01

// Format and bit-depth
#define WDP_FORMAT			0xF0
#define WDP_BITDEPTH		0x0F
#define WDP_Y_ONLY			0x00
#define WDP_YUV_420			0x01
#define WDP_YUV_422			0x02
#define WDP_YUV_444			0x03
#define WDP_CMYK			0x04
#define WDP_BAYER			0x05
#define WDP_N_CHANNEL		0x06
#define WDP_RGB				0x07
#define WDP_RGBE			0x08

// Bitdepth
#define WDP_BD_1_WHITE		0x00
#define WDP_BD_8			0x01
#define WDP_BD_16			0x02
#define WDP_BD_16S			0x03
#define WDP_BD_16F			0x04
#define WDP_BD_32			0x05
#define WDP_BD_32S			0x06
#define WDP_BD_32F			0x07
#define WDP_BD_5			0x08
#define WDP_BD_10			0x09
#define WDP_BD_565			0x0A
#define WDP_BD_1_BLACK		0x0F


//
// Image plane header defines
//

// First set of flags
#define WDP_CLR_FMT			0xE0
#define WDP_NO_SCALED		0x10
#define WDP_BANDS_PRESENT	0x0F

// Defines for n-channels
#define WDP_NUM_CHANS		0xF0
#define WDP_COLOR_INTERP	0x0F

// Second set of flags
#define WDP_DC_FRAME		0x80

// Channel modes
#define WDP_CH_UNIFORM		0x00
#define WDP_CH_SEPARATE		0x01
#define WDP_CH_INDEPENDENT	0x02

// Tile types
#define WDP_TILE_HASH		0xF8
#define WDP_TILE_TYPE		0x03
#define WDP_SPATIAL_TILE	0x00
#define WDP_DC_TILE			0x01
#define WDP_LOWPASS_TILE	0x02
#define WDP_HIGHPASS_TILE	0x03
#define WDP_FLEXBITS_TILE	0x04

// Bands present
#define WDP_SB_ALL			0x00
#define WDP_SB_NO_FLEXBITS	0x01
#define WDP_SB_NO_HIGHPASS	0x02
#define WDP_SB_DC_ONLY		0x03
#define WDP_SB_ISOLATED		0x04


//
// Internal functions
//
ILboolean	iIsValidWdp();
ILboolean	iCheckWdp(WDPHEAD *Header);
ILboolean	iLoadWdpInternal();
ILuint		VLWESC();


#endif//WDP_H
