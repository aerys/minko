//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 09/01/2003 <--Y2K Compliant! =]
//
// Filename: src-IL/include/il_bmp.h
//
// Description: Reads and writes to a bitmap (.bmp) file.
//
//-----------------------------------------------------------------------------


#ifndef BMP_H
#define BMP_H

#include "il_internal.h"

#ifdef _WIN32
	#pragma pack(push, bmp_struct, 1)
#endif
typedef struct BMPHEAD {
	ILushort	bfType;
	ILint		bfSize;
	ILuint		bfReserved;
	ILint		bfDataOff;
	ILint		biSize;
	ILint		biWidth;
	ILint		biHeight;
	ILshort		biPlanes;
	ILshort		biBitCount;
	ILint		biCompression;
	ILint		biSizeImage;
	ILint		biXPelsPerMeter;
	ILint		biYPelsPerMeter;
	ILint		biClrUsed;
	ILint		biClrImportant;
} IL_PACKSTRUCT BMPHEAD;

typedef struct OS2_HEAD
{
	// Bitmap file header.
	ILushort	bfType;
	ILuint		biSize;
	ILshort		xHotspot;
	ILshort		yHotspot;
	ILuint		DataOff;

	// Bitmap core header.
	ILuint		cbFix;
	//2003-09-01: changed cx, cy to ushort according to MSDN
	ILushort		cx;
	ILushort		cy;
	ILushort	cPlanes;
	ILushort	cBitCount;
} IL_PACKSTRUCT OS2_HEAD;
#ifdef _WIN32
	#pragma pack(pop, bmp_struct)
#endif

// Internal functions
ILboolean	iGetBmpHead(BMPHEAD * const Header);
ILboolean	iGetOS2Head(OS2_HEAD * const Header);
ILboolean	iIsValidBmp();
ILboolean	iCheckBmp(const BMPHEAD *CONST_RESTRICT Header);
ILboolean	iCheckOS2(const OS2_HEAD *CONST_RESTRICT Header);
ILboolean	iLoadBitmapInternal();
ILboolean	iSaveBitmapInternal();
ILboolean	ilReadUncompBmp(BMPHEAD *Info);
ILboolean	ilReadRLE8Bmp(BMPHEAD *Info);
ILboolean	ilReadRLE4Bmp(BMPHEAD *Info);
ILboolean	iGetOS2Bmp(OS2_HEAD *Header);

#ifdef IL_BMP_C
#undef NOINLINE
#undef INLINE
#define INLINE
#endif

#ifndef NOINLINE
INLINE void GetShiftFromMask(const ILuint Mask, ILuint * CONST_RESTRICT ShiftLeft, ILuint * CONST_RESTRICT ShiftRight) {
	ILuint Temp, i;

	if( Mask == 0 ) {
		*ShiftLeft = *ShiftRight = 0;
		return;
	}

	Temp = Mask;
	for( i = 0; i < 32; i++, Temp >>= 1 ) {
		if( Temp & 1 )
			break;
	}
	*ShiftRight = i;

	// Temp is preserved, so use it again:
	for( i = 0; i < 8; i++, Temp >>= 1 ) {
		if( !(Temp & 1) )
			break;
	}
	*ShiftLeft = 8 - i;

	return;
}
#endif

#endif//BMP_H
