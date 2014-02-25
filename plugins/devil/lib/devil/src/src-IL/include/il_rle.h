//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2002 by Denton Woods
// Last modified: 05/25/2001 <--Y2K Compliant! =]
//
// Filename: src-IL/include/il_rle.h
//
// Description: Functions for run-length encoding
//
//-----------------------------------------------------------------------------

#ifndef RLE_H
#define RLE_H

#include "il_internal.h"

#define TGA_MAX_RUN 128
#define SGI_MAX_RUN 127
#define BMP_MAX_RUN 127

#ifdef IL_RLE_C
#undef NOINLINE
#undef INLINE
#define INLINE
#endif

#ifndef NOINLINE
INLINE ILuint GetPix(ILubyte *p, ILuint bpp) {
	ILuint Pixel;
	Pixel = (ILuint)*p++;
	
	while( bpp-- > 1 ) {
		Pixel <<= 8;
		Pixel |= (ILuint)*p++;
	}
	return Pixel;
}

INLINE ILint CountDiffPixels(ILubyte *p, ILuint bpp, ILuint pixCnt) {
	ILuint	pixel;
	ILuint	nextPixel = 0;
	ILint	n;

	n = 0;
	if (pixCnt == 1)
		return pixCnt;
	pixel = GetPix(p, bpp);

	while (pixCnt > 1) {
		p += bpp;
		nextPixel = GetPix(p, bpp);
		if (nextPixel == pixel)
			break;
		pixel = nextPixel;
		++n;
		--pixCnt;
	}

	if (nextPixel == pixel)
		return n;
	return n + 1;
}


INLINE ILint CountSamePixels(ILubyte *p, ILuint bpp, ILuint pixCnt) {
	ILuint	pixel;
	ILuint	nextPixel;
	ILint	n;

	n = 1;
	pixel = GetPix(p, bpp);
	pixCnt--;

	while (pixCnt > 0) {
		p += bpp;
		nextPixel = GetPix(p, bpp);
		if (nextPixel != pixel)
			break;
		++n;
		--pixCnt;
	}

	return n;
}
#endif

ILuint GetPix(ILubyte *p, ILuint bpp);
ILint CountDiffPixels(ILubyte *p, ILuint bpp, ILuint pixCnt);
ILint CountSamePixels(ILubyte *p, ILuint bpp, ILuint pixCnt);

#endif//RLE_H
