//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 01/24/2009
//
// Filename: src-IL/src/il_manip.cpp
//
// Description: Image manipulation
//
//-----------------------------------------------------------------------------


#include "il_internal.h"

ILfloat /*ILAPIENTRY*/ ilFloatToHalfOverflow() {
	ILfloat f = 1e10;
	ILint j;
	for (j = 0; j < 10; j++)
		f *= f;				// this will overflow before
	// the for loop terminates
	return f;
}

//-----------------------------------------------------
// Float-to-half conversion -- general case, including
// zeroes, denormalized numbers and exponent overflows.
//-----------------------------------------------------
ILushort ILAPIENTRY ilFloatToHalf(ILuint i) {
	//
	// Our floating point number, f, is represented by the bit
	// pattern in integer i.  Disassemble that bit pattern into
	// the sign, s, the exponent, e, and the significand, m.
	// Shift s into the position where it will go in in the
	// resulting half number.
	// Adjust e, accounting for the different exponent bias
	// of float and half (127 versus 15).
	//

	register int s =  (i >> 16) & 0x00008000;
	register int e = ((i >> 23) & 0x000000ff) - (127 - 15);
	register int m =   i        & 0x007fffff;

	//
	// Now reassemble s, e and m into a half:
	//

	if (e <= 0)
	{
		if (e < -10)
		{
			//
			// E is less than -10.  The absolute value of f is
			// less than HALF_MIN (f may be a small normalized
			// float, a denormalized float or a zero).
			//
			// We convert f to a half zero.
			//

			return 0;
		}

		//
		// E is between -10 and 0.  F is a normalized float,
		// whose magnitude is less than HALF_NRM_MIN.
		//
		// We convert f to a denormalized half.
		// 

		m = (m | 0x00800000) >> (1 - e);

		//
		// Round to nearest, round "0.5" up.
		//
		// Rounding may cause the significand to overflow and make
		// our number normalized.  Because of the way a half's bits
		// are laid out, we don't have to treat this case separately;
		// the code below will handle it correctly.
		// 

		if (m &  0x00001000)
			m += 0x00002000;

		//
		// Assemble the half from s, e (zero) and m.
		//

		return s | (m >> 13);
	}
	else if (e == 0xff - (127 - 15))
	{
		if (m == 0)
		{
			//
			// F is an infinity; convert f to a half
			// infinity with the same sign as f.
			//

			return s | 0x7c00;
		}
		else
		{
			//
			// F is a NAN; we produce a half NAN that preserves
			// the sign bit and the 10 leftmost bits of the
			// significand of f, with one exception: If the 10
			// leftmost bits are all zero, the NAN would turn 
			// into an infinity, so we have to set at least one
			// bit in the significand.
			//

			m >>= 13;
			return s | 0x7c00 | m | (m == 0);
		}
	}
	else
	{
		//
		// E is greater than zero.  F is a normalized float.
		// We try to convert f to a normalized half.
		//

		//
		// Round to nearest, round "0.5" up
		//

		if (m &  0x00001000)
		{
			m += 0x00002000;

			if (m & 0x00800000)
			{
				m =  0;		// overflow in significand,
				e += 1;		// adjust exponent
			}
		}

		//
		// Handle exponent overflow
		//

		if (e > 30)
		{
			ilFloatToHalfOverflow();	// Cause a hardware floating point overflow;
			return s | 0x7c00;	// if this returns, the half becomes an
		}   			// infinity with the same sign as f.

		//
		// Assemble the half from s, e and m.
		//

		return s | (e << 10) | (m >> 13);
	}
}

// Taken from OpenEXR
INLINE ILuint ILAPIENTRY ilHalfToFloat (ILushort y) {

	int s = (y >> 15) & 0x00000001;
	int e = (y >> 10) & 0x0000001f;
	int m =  y		  & 0x000003ff;

	if (e == 0)
	{
		if (m == 0)
		{
			//
			// Plus or minus zero
			//

			return s << 31;
		}
		else
		{
			//
			// Denormalized number -- renormalize it
			//

			while (!(m & 0x00000400))
			{
				m <<= 1;
				e -=  1;
			}

			e += 1;
			m &= ~0x00000400;
		}
	}
	else if (e == 31)
	{
		if (m == 0)
		{
			//
			// Positive or negative infinity
			//

			return (s << 31) | 0x7f800000;
		}
		else
		{
			//
			// Nan -- preserve sign and significand bits
			//

			return (s << 31) | 0x7f800000 | (m << 13);
		}
	}

	//
	// Normalized number
	//

	e = e + (127 - 15);
	m = m << 13;

	//
	// Assemble s, e and m.
	//

	return (s << 31) | (e << 23) | m;
}

void ILAPIENTRY iFlipBuffer(ILubyte *buff, ILuint depth, ILuint line_size, ILuint line_num)
{
	ILubyte *StartPtr, *EndPtr;
	ILuint y, d;
	const ILuint size = line_num * line_size;

	for (d = 0; d < depth; d++) {
		StartPtr = buff + d * size;
		EndPtr   = buff + d * size + size;

		for (y = 0; y < (line_num/2); y++) {
			EndPtr -= line_size; 
			iMemSwap(StartPtr, EndPtr, line_size);
			StartPtr += line_size;
		}
	}
}

// Just created for internal use.
ILubyte* iFlipNewBuffer(ILubyte *buff, ILuint depth, ILuint line_size, ILuint line_num)
{
	ILubyte *data;
	ILubyte *s1, *s2;
	ILuint y, d;
	const ILuint size = line_num * line_size;

	if ((data = (ILubyte*)ialloc(depth*size)) == NULL)
		return IL_FALSE;

	for (d = 0; d < depth; d++) {
		s1 = buff + d * size;
		s2 = data + d * size+size;

		for (y = 0; y < line_num; y++) {
			s2 -= line_size; 
			memcpy(s2,s1,line_size);
			s1 += line_size;
		}
	}
	return data;
}


// Flips an image over its x axis
ILboolean ilFlipImage()
{
	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	iCurImage->Origin = (iCurImage->Origin == IL_ORIGIN_LOWER_LEFT) ?
						IL_ORIGIN_UPPER_LEFT : IL_ORIGIN_LOWER_LEFT;

	iFlipBuffer(iCurImage->Data,iCurImage->Depth,iCurImage->Bps,iCurImage->Height);

	return IL_TRUE;
}

// Just created for internal use.
ILubyte* ILAPIENTRY iGetFlipped(ILimage *img)
{
	if (img == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return NULL;
	}
	return iFlipNewBuffer(img->Data,img->Depth,img->Bps,img->Height);
}


//@JASON New routine created 28/03/2001
//! Mirrors an image over its y axis
ILboolean ILAPIENTRY iMirror() {
	ILubyte		*Data, *DataPtr, *Temp;
	ILuint		y, d, PixLine;
	ILint		x, c;
	ILushort	*ShortPtr, *TempShort;
	ILuint		*IntPtr, *TempInt;
	ILdouble	*DblPtr, *TempDbl;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	Data = (ILubyte*)ialloc(iCurImage->SizeOfData);
	if (Data == NULL)
		return IL_FALSE;

	PixLine = iCurImage->Bps / iCurImage->Bpc;
	switch (iCurImage->Bpc)
	{
		case 1:
			Temp = iCurImage->Data;
			for (d = 0; d < iCurImage->Depth; d++) {
				DataPtr = Data + d * iCurImage->SizeOfPlane;
				for (y = 0; y < iCurImage->Height; y++) {
					for (x = iCurImage->Width - 1; x >= 0; x--) {
						for (c = 0; c < iCurImage->Bpp; c++, Temp++) {
							DataPtr[y * PixLine + x * iCurImage->Bpp + c] = *Temp;
						}
					}
				}
			}
			break;

		case 2:
			TempShort = (ILushort*)iCurImage->Data;
			for (d = 0; d < iCurImage->Depth; d++) {
				ShortPtr = (ILushort*)(Data + d * iCurImage->SizeOfPlane);
				for (y = 0; y < iCurImage->Height; y++) {
					for (x = iCurImage->Width - 1; x >= 0; x--) {
						for (c = 0; c < iCurImage->Bpp; c++, TempShort++) {
							ShortPtr[y * PixLine + x * iCurImage->Bpp + c] = *TempShort;
						}
					}
				}
			}
			break;

		case 4:
			TempInt = (ILuint*)iCurImage->Data;
			for (d = 0; d < iCurImage->Depth; d++) {
				IntPtr = (ILuint*)(Data + d * iCurImage->SizeOfPlane);
				for (y = 0; y < iCurImage->Height; y++) {
					for (x = iCurImage->Width - 1; x >= 0; x--) {
						for (c = 0; c < iCurImage->Bpp; c++, TempInt++) {
							IntPtr[y * PixLine + x * iCurImage->Bpp + c] = *TempInt;
						}
					}
				}
			}
			break;

		case 8:
			TempDbl = (ILdouble*)iCurImage->Data;
			for (d = 0; d < iCurImage->Depth; d++) {
				DblPtr = (ILdouble*)(Data + d * iCurImage->SizeOfPlane);
				for (y = 0; y < iCurImage->Height; y++) {
					for (x = iCurImage->Width - 1; x >= 0; x--) {
						for (c = 0; c < iCurImage->Bpp; c++, TempDbl++) {
							DblPtr[y * PixLine + x * iCurImage->Bpp + c] = *TempDbl;
						}
					}
				}
			}
			break;
	}

	ifree(iCurImage->Data);
	iCurImage->Data = Data;

	return IL_TRUE;
}


// Should we add type to the parameter list?
// Copies a 1d block of pixels to the buffer pointed to by Data.
ILboolean ilCopyPixels1D(ILuint XOff, ILuint Width, void *Data)
{
	ILuint	x, c, NewBps, NewOff, PixBpp;
	ILubyte	*Temp = (ILubyte*)Data, *TempData = iCurImage->Data;

	if (ilIsEnabled(IL_ORIGIN_SET)) {
		if ((ILenum)ilGetInteger(IL_ORIGIN_MODE) != iCurImage->Origin) {
			TempData = iGetFlipped(iCurImage);
			if (TempData == NULL)
				return IL_FALSE;
		}
	}

	PixBpp = iCurImage->Bpp * iCurImage->Bpc;

	if (iCurImage->Width < XOff + Width) {
		NewBps = (iCurImage->Width - XOff) * PixBpp;
	}
	else {
		NewBps = Width * PixBpp;
	}
	NewOff = XOff * PixBpp;

	for (x = 0; x < NewBps; x += PixBpp) {
		for (c = 0; c < PixBpp; c++) {
			Temp[x + c] = TempData[(x + NewOff) + c];
		}
	}

	if (TempData != iCurImage->Data)
		ifree(TempData);

	return IL_TRUE;
}


// Copies a 2d block of pixels to the buffer pointed to by Data.
ILboolean ilCopyPixels2D(ILuint XOff, ILuint YOff, ILuint Width, ILuint Height, void *Data)
{
	ILuint	x, y, c, NewBps, DataBps, NewXOff, NewHeight, PixBpp;
	ILubyte	*Temp = (ILubyte*)Data, *TempData = iCurImage->Data;

	if (ilIsEnabled(IL_ORIGIN_SET)) {
		if ((ILenum)ilGetInteger(IL_ORIGIN_MODE) != iCurImage->Origin) {
			TempData = iGetFlipped(iCurImage);
			if (TempData == NULL)
				return IL_FALSE;
		}
	}

	PixBpp = iCurImage->Bpp * iCurImage->Bpc;

	if (iCurImage->Width < XOff + Width)
		NewBps = (iCurImage->Width - XOff) * PixBpp;
	else
		NewBps = Width * PixBpp;

	if (iCurImage->Height < YOff + Height)
		NewHeight = iCurImage->Height - YOff;
	else
		NewHeight = Height;

	DataBps = Width * PixBpp;
	NewXOff = XOff * PixBpp;

	for (y = 0; y < NewHeight; y++) {
		for (x = 0; x < NewBps; x += PixBpp) {
			for (c = 0; c < PixBpp; c++) {
				Temp[y * DataBps + x + c] = 
					TempData[(y + YOff) * iCurImage->Bps + x + NewXOff + c];
			}
		}
	}

	if (TempData != iCurImage->Data)
		ifree(TempData);

	return IL_TRUE;
}


// Copies a 3d block of pixels to the buffer pointed to by Data.
ILboolean ilCopyPixels3D(ILuint XOff, ILuint YOff, ILuint ZOff, ILuint Width, ILuint Height, ILuint Depth, void *Data)
{
	ILuint	x, y, z, c, NewBps, DataBps, NewSizePlane, NewH, NewD, NewXOff, PixBpp;
	ILubyte	*Temp = (ILubyte*)Data, *TempData = iCurImage->Data;

	if (ilIsEnabled(IL_ORIGIN_SET)) {
		if ((ILenum)ilGetInteger(IL_ORIGIN_MODE) != iCurImage->Origin) {
			TempData = iGetFlipped(iCurImage);
			if (TempData == NULL)
				return IL_FALSE;
		}
	}

	PixBpp = iCurImage->Bpp * iCurImage->Bpc;

	if (iCurImage->Width < XOff + Width)
		NewBps = (iCurImage->Width - XOff) * PixBpp;
	else
		NewBps = Width * PixBpp;

	if (iCurImage->Height < YOff + Height)
		NewH = iCurImage->Height - YOff;
	else
		NewH = Height;

	if (iCurImage->Depth < ZOff + Depth)
		NewD = iCurImage->Depth - ZOff;
	else
		NewD = Depth;

	DataBps = Width * PixBpp;
	NewSizePlane = NewBps * NewH;

	NewXOff = XOff * PixBpp;

	for (z = 0; z < NewD; z++) {
		for (y = 0; y < NewH; y++) {
			for (x = 0; x < NewBps; x += PixBpp) {
				for (c = 0; c < PixBpp; c++) {
					Temp[z * NewSizePlane + y * DataBps + x + c] = 
						TempData[(z + ZOff) * iCurImage->SizeOfPlane + (y + YOff) * iCurImage->Bps + x + NewXOff + c];
						//TempData[(z + ZOff) * iCurImage->SizeOfPlane + (y + YOff) * iCurImage->Bps + (x + XOff) * iCurImage->Bpp + c];
				}
			}
		}
	}

	if (TempData != iCurImage->Data)
		ifree(TempData);

	return IL_TRUE;
}


ILuint ILAPIENTRY ilCopyPixels(ILuint XOff, ILuint YOff, ILuint ZOff, ILuint Width, ILuint Height, ILuint Depth, ILenum Format, ILenum Type, void *Data)
{
	void	*Converted = NULL;
	ILubyte	*TempBuff = NULL;
	ILuint	SrcSize, DestSize;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return 0;
	}
	DestSize = Width * Height * Depth * ilGetBppFormat(Format) * ilGetBpcType(Type);
	if (DestSize == 0) {
		return DestSize;
	}
	if (Data == NULL || Format == IL_COLOUR_INDEX) {
		ilSetError(IL_INVALID_PARAM);
		return 0;
	}
	SrcSize = Width * Height * Depth * iCurImage->Bpp * iCurImage->Bpc;

	if (Format == iCurImage->Format && Type == iCurImage->Type) {
		TempBuff = (ILubyte*)Data;
	}
	else {
		TempBuff = (ILubyte*)ialloc(SrcSize);
		if (TempBuff == NULL) {
			return 0;
		}
	}

	if (YOff + Height <= 1) {
		if (!ilCopyPixels1D(XOff, Width, TempBuff)) {
			goto failed;
		}
	}
	else if (ZOff + Depth <= 1) {
		if (!ilCopyPixels2D(XOff, YOff, Width, Height, TempBuff)) {
			goto failed;
		}
	}
	else {
		if (!ilCopyPixels3D(XOff, YOff, ZOff, Width, Height, Depth, TempBuff)) {
			goto failed;
		}
	}

	if (Format == iCurImage->Format && Type == iCurImage->Type) {
		return DestSize;
	}

	Converted = ilConvertBuffer(SrcSize, iCurImage->Format, Format, iCurImage->Type, Type, &iCurImage->Pal, TempBuff);
	if (Converted == NULL)
		goto failed;

	memcpy(Data, Converted, DestSize);

	ifree(Converted);
	if (TempBuff != Data)
		ifree(TempBuff);

	return DestSize;

failed:
	if (TempBuff != Data)
		ifree(TempBuff);
	ifree(Converted);
	return 0;
}


ILboolean ilSetPixels1D(ILint XOff, ILuint Width, void *Data)
{
	ILuint	c, SkipX = 0, PixBpp;
	ILint	x, NewWidth;
	ILubyte	*Temp = (ILubyte*)Data, *TempData = iCurImage->Data;

	if (ilIsEnabled(IL_ORIGIN_SET)) {
		if ((ILenum)ilGetInteger(IL_ORIGIN_MODE) != iCurImage->Origin) {
			TempData = iGetFlipped(iCurImage);
			if (TempData == NULL)
				return IL_FALSE;
		}
	}

	PixBpp = iCurImage->Bpp * iCurImage->Bpc;

	if (XOff < 0) {
		SkipX = abs(XOff);
		XOff = 0;
	}

	if (iCurImage->Width < XOff + Width) {
		NewWidth = iCurImage->Width - XOff;
	}
	else {
		NewWidth = Width;
	}

	NewWidth -= SkipX;

	for (x = 0; x < NewWidth; x++) {
		for (c = 0; c < PixBpp; c++) {
			TempData[(x + XOff) * PixBpp + c] = Temp[(x + SkipX) * PixBpp + c];
		}
	}

	if (TempData != iCurImage->Data) {
		ifree(iCurImage->Data);
		iCurImage->Data = TempData;
	}

	return IL_TRUE;
}


ILboolean ilSetPixels2D(ILint XOff, ILint YOff, ILuint Width, ILuint Height, void *Data)
{
	ILuint	c, SkipX = 0, SkipY = 0, NewBps, PixBpp;
	ILint	x, y, NewWidth, NewHeight;
	ILubyte	*Temp = (ILubyte*)Data, *TempData = iCurImage->Data;

	if (ilIsEnabled(IL_ORIGIN_SET)) {
		if ((ILenum)ilGetInteger(IL_ORIGIN_MODE) != iCurImage->Origin) {
			TempData = iGetFlipped(iCurImage);
			if (TempData == NULL)
				return IL_FALSE;
		}
	}

	PixBpp = iCurImage->Bpp * iCurImage->Bpc;

	if (XOff < 0) {
		SkipX = abs(XOff);
		XOff = 0;
	}
	if (YOff < 0) {
		SkipY = abs(YOff);
		YOff = 0;
	}

	if (iCurImage->Width < XOff + Width)
		NewWidth = iCurImage->Width - XOff;
	else
		NewWidth = Width;
	NewBps = Width * PixBpp;

	if (iCurImage->Height < YOff + Height)
		NewHeight = iCurImage->Height - YOff;
	else
		NewHeight = Height;

	NewWidth -= SkipX;
	NewHeight -= SkipY;

	for (y = 0; y < NewHeight; y++) {
		for (x = 0; x < NewWidth; x++) {
			for (c = 0; c < PixBpp; c++) {
				TempData[(y + YOff) * iCurImage->Bps + (x + XOff) * PixBpp + c] =
					Temp[(y + SkipY) * NewBps + (x + SkipX) * PixBpp + c];					
			}
		}
	}

	if (TempData != iCurImage->Data) {
		ifree(iCurImage->Data);
		iCurImage->Data = TempData;
	}

	return IL_TRUE;
}


ILboolean ilSetPixels3D(ILint XOff, ILint YOff, ILint ZOff, ILuint Width, ILuint Height, ILuint Depth, void *Data)
{
	ILuint	SkipX = 0, SkipY = 0, SkipZ = 0, c, NewBps, NewSizePlane, PixBpp;
	ILint	x, y, z, NewW, NewH, NewD;
	ILubyte	*Temp = (ILubyte*)Data, *TempData = iCurImage->Data;

	if (ilIsEnabled(IL_ORIGIN_SET)) {
		if ((ILenum)ilGetInteger(IL_ORIGIN_MODE) != iCurImage->Origin) {
			TempData = iGetFlipped(iCurImage);
			if (TempData == NULL)
				return IL_FALSE;
		}
	}

	PixBpp = iCurImage->Bpp * iCurImage->Bpc;

	if (XOff < 0) {
		SkipX = abs(XOff);
		XOff = 0;
	}
	if (YOff < 0) {
		SkipY = abs(YOff);
		YOff = 0;
	}
	if (ZOff < 0) {
		SkipZ = abs(ZOff);
		ZOff = 0;
	}

	if (iCurImage->Width < XOff + Width)
		NewW = iCurImage->Width - XOff;
	else
		NewW = Width;
	NewBps = Width * PixBpp;

	if (iCurImage->Height < YOff + Height)
		NewH = iCurImage->Height - YOff;
	else
		NewH = Height;

	if (iCurImage->Depth < ZOff + Depth)
		NewD = iCurImage->Depth - ZOff;
	else
		NewD = Depth;
	NewSizePlane = NewBps * Height;

	NewW -= SkipX;
	NewH -= SkipY;
	NewD -= SkipZ;

	for (z = 0; z < NewD; z++) {
		for (y = 0; y < NewH; y++) {
			for (x = 0; x < NewW; x++) {
				for (c = 0; c < PixBpp; c++) {
					TempData[(z + ZOff) * iCurImage->SizeOfPlane + (y + YOff) * iCurImage->Bps + (x + XOff) * PixBpp + c] =
						Temp[(z + SkipZ) * NewSizePlane + (y + SkipY) * NewBps + (x + SkipX) * PixBpp + c];
				}
			}
		}
	}

	if (TempData != iCurImage->Data) {
		ifree(iCurImage->Data);
		iCurImage->Data = TempData;
	}

	return IL_TRUE;
}


void ILAPIENTRY ilSetPixels(ILint XOff, ILint YOff, ILint ZOff, ILuint Width, ILuint Height, ILuint Depth, ILenum Format, ILenum Type, void *Data)
{
	void *Converted;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return;
	}
	if (Data == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return;
	}

	if (Format == iCurImage->Format && Type == iCurImage->Type) {
		Converted = (void*)Data;
	}
	else {
		Converted = ilConvertBuffer(Width * Height * Depth * ilGetBppFormat(Format) * ilGetBpcType(Type), Format, iCurImage->Format, Type, iCurImage->Type, NULL, Data);
		if (!Converted)
			return;
	}

	if (YOff + Height <= 1) {
		ilSetPixels1D(XOff, Width, Converted);
	}
	else if (ZOff + Depth <= 1) {
		ilSetPixels2D(XOff, YOff, Width, Height, Converted);
	}
	else {
		ilSetPixels3D(XOff, YOff, ZOff, Width, Height, Depth, Converted);
	}

	if (Format == iCurImage->Format && Type == iCurImage->Type) {
		return;
	}

	if (Converted != Data)
		ifree(Converted);

	return;
}



//	Ripped from Platinum (Denton's sources)
//	This could very well easily be changed to a 128x128 image instead...needed?

//! Creates an ugly 64x64 black and yellow checkerboard image.
ILboolean ILAPIENTRY ilDefaultImage()
{
	ILubyte *TempData;
	ILubyte Yellow[3] = { 18, 246, 243 };
	ILubyte Black[3]  = { 0, 0, 0 };
	ILubyte *ColorPtr = Yellow;  // The start color
	ILboolean Color = IL_TRUE;

	// Loop Variables
	ILint v, w, x, y;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	if (!ilTexImage(64, 64, 1, 3, IL_BGR, IL_UNSIGNED_BYTE, NULL)) {
		return IL_FALSE;
	}
	iCurImage->Origin = IL_ORIGIN_LOWER_LEFT;
	TempData = iCurImage->Data;

	for (v = 0; v < 8; v++) {
		// We do this because after a "block" line ends, the next row of blocks
		// above starts with the ending colour, but the very inner loop switches them.
		if (Color) {
			Color = IL_FALSE;
			ColorPtr = Black;
		}
		else {
			Color = IL_TRUE;
			ColorPtr = Yellow;
		}

		for (w = 0; w < 8; w++) {
			for (x = 0; x < 8; x++) {
				for (y = 0; y < 8; y++, TempData += iCurImage->Bpp) {
					TempData[0] = ColorPtr[0];
					TempData[1] = ColorPtr[1];
					TempData[2] = ColorPtr[2];
				}

				// Switch to alternate between black and yellow
				if (Color) {
					Color = IL_FALSE;
					ColorPtr = Black;
				}
				else {
					Color = IL_TRUE;
					ColorPtr = Yellow;
				}
			}
		}
	}

	return IL_TRUE;
}


ILubyte* ILAPIENTRY ilGetAlpha(ILenum Type)
{
	ILimage		*TempImage;
	ILubyte		*Alpha;
	ILushort	*AlphaShort;
	ILuint		*AlphaInt;
	ILdouble	*AlphaDbl;
	ILuint		i, j, Bpc, Size, AlphaOff;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	Bpc = ilGetBpcType(Type);
	if (Bpc == 0) {
		ilSetError(IL_INVALID_PARAM);
		return NULL;
	}

	if (iCurImage->Type == Type) {
		TempImage = iCurImage;
	} else {
		TempImage = iConvertImage(iCurImage, iCurImage->Format, Type);
		if (TempImage == NULL)
			return NULL;
	}

	Size = iCurImage->Width * iCurImage->Height * iCurImage->Depth * TempImage->Bpp;
	Alpha = (ILubyte*)ialloc(Size / TempImage->Bpp * Bpc);
	if (Alpha == NULL) {
		if (TempImage != iCurImage)
			ilCloseImage(TempImage);
		return NULL;
	}

	switch (TempImage->Format)
	{
		case IL_RGB:
		case IL_BGR:
		case IL_LUMINANCE:
		case IL_COLOUR_INDEX:  // @TODO: Make IL_COLOUR_INDEX separate.
			memset(Alpha, 0xFF, Size / TempImage->Bpp * Bpc);
			if (TempImage != iCurImage)
				ilCloseImage(TempImage);
			return Alpha;
	}

	// If our format is alpha, just return a copy.
	if (TempImage->Format == IL_ALPHA) {
		memcpy(Alpha, TempImage->Data, TempImage->SizeOfData);
		return Alpha;
	}
		
	if (TempImage->Format == IL_LUMINANCE_ALPHA)
		AlphaOff = 2;
	else
		AlphaOff = 4;

	switch (TempImage->Type)
	{
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
			for (i = AlphaOff-1, j = 0; i < Size; i += AlphaOff, j++)
				Alpha[j] = TempImage->Data[i];
			break;

		case IL_SHORT:
		case IL_UNSIGNED_SHORT:
			AlphaShort = (ILushort*)Alpha;
			for (i = AlphaOff-1, j = 0; i < Size; i += AlphaOff, j++)
				AlphaShort[j] = ((ILushort*)TempImage->Data)[i];
			break;

		case IL_INT:
		case IL_UNSIGNED_INT:
		case IL_FLOAT:  // Can throw float in here, because it's the same size.
			AlphaInt = (ILuint*)Alpha;
			for (i = AlphaOff-1, j = 0; i < Size; i += AlphaOff, j++)
				AlphaInt[j] = ((ILuint*)TempImage->Data)[i];
			break;

		case IL_DOUBLE:
			AlphaDbl = (ILdouble*)Alpha;
			for (i = AlphaOff-1, j = 0; i < Size; i += AlphaOff, j++)
				AlphaDbl[j] = ((ILdouble*)TempImage->Data)[i];
			break;
	}

	if (TempImage != iCurImage)
		ilCloseImage(TempImage);

	return Alpha;
}

// sets the Alpha value to a specific value for each pixel in the image
ILboolean ILAPIENTRY ilSetAlpha(ILdouble AlphaValue)
{
	ILboolean	ret = IL_TRUE;
	ILuint		i,Size;
	ILimage		*Image = iCurImage;
	ILuint		AlphaOff;

	if (Image == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	AlphaValue = IL_CLAMP(AlphaValue);

	switch (Image->Format)
	{
		case IL_RGB:
			ret = ilConvertImage(IL_RGBA, Image->Type);
		case IL_RGBA:
			AlphaOff = 4;
		break;
		case IL_BGR:
			ret = ilConvertImage(IL_BGRA, Image->Type);
		case IL_BGRA:
			AlphaOff = 4;
			break;
		case IL_LUMINANCE:
			ret = ilConvertImage(IL_LUMINANCE_ALPHA, Image->Type);
		case IL_LUMINANCE_ALPHA:
			AlphaOff = 2;
			break;
		case IL_ALPHA:
			AlphaOff = 1;
		case IL_COLOUR_INDEX: //@TODO use palette with alpha
			ret = ilConvertImage(IL_RGBA, Image->Type);
			AlphaOff = 4;
			break;
	}
	if (ret == IL_FALSE) {
		// Error has been set by ilConvertImage.
		return IL_FALSE;
	}
	Size = Image->Width * Image->Height * Image->Depth * Image->Bpp;

	switch (iCurImage->Type)
	{
		case IL_BYTE: 
		case IL_UNSIGNED_BYTE: {
			const ILbyte alpha = (ILubyte)(AlphaValue * IL_MAX_UNSIGNED_BYTE + .5);
			for (i = AlphaOff-1; i < Size; i += AlphaOff)
				Image->Data[i] = alpha;
			break;
		}
		case IL_SHORT:
		case IL_UNSIGNED_SHORT: {
			const ILushort alpha = (ILushort)(AlphaValue * IL_MAX_UNSIGNED_SHORT + .5);
			for (i = AlphaOff-1; i < Size; i += AlphaOff)
				((ILushort*)Image->Data)[i] = alpha;
			break;
		}
		case IL_INT:
		case IL_UNSIGNED_INT: {
			const ILushort alpha = (ILushort)(AlphaValue * IL_MAX_UNSIGNED_INT + .5);
			for (i = AlphaOff-1; i < Size; i += AlphaOff)
				((ILuint*)Image->Data)[i] = alpha;
			break;
		}
		case IL_FLOAT: {
			const ILfloat alpha = (ILfloat)AlphaValue;
			for (i = AlphaOff-1; i < Size; i += AlphaOff)
				((ILfloat*)Image->Data)[i] = alpha;
			break;
		}
		case IL_DOUBLE: {
			const ILdouble alpha  = AlphaValue;
			for (i = AlphaOff-1; i < Size; i += AlphaOff)
				((ILdouble*)Image->Data)[i] = alpha;
			break;
		}
	}
	
	return IL_TRUE;
}

void ILAPIENTRY ilModAlpha(ILdouble AlphaValue)
{
    ILuint AlphaOff = 0;
    ILboolean ret = IL_FALSE;
    ILuint i,j,Size;

    union {
        ILubyte alpha_byte;
        ILushort alpha_short;
        ILuint alpha_int;
        ILfloat alpha_float;
        ILdouble alpha_double;
    } Alpha;

    
    if (iCurImage == NULL) {
        ilSetError(IL_ILLEGAL_OPERATION);
        return;
    }
    
    switch (iCurImage->Format)
	{
            case IL_RGB:
                ret = ilConvertImage(IL_RGBA,iCurImage->Type);
                AlphaOff = 4;
                break;
            case IL_BGR:
                ret = ilConvertImage(IL_BGRA,iCurImage->Type);
                AlphaOff = 4;
                break;
            case IL_LUMINANCE:
                ret = ilConvertImage(IL_LUMINANCE_ALPHA,iCurImage->Type);
                AlphaOff = 2;
                break;
            case IL_COLOUR_INDEX:
                ret = ilConvertImage(IL_RGBA,iCurImage->Type);
                AlphaOff = 4;
                break;
    }    
    Size = iCurImage->Width * iCurImage->Height * iCurImage->Depth * iCurImage->Bpp;
    
    if (!ret)
		return;

    switch (iCurImage->Type)
	{
        case IL_BYTE:
        case IL_UNSIGNED_BYTE:
            Alpha.alpha_byte = (ILubyte)(AlphaValue * 0x000000FF + .5);
            for (i = AlphaOff-1, j = 0; i < Size; i += AlphaOff, j++)
                iCurImage->Data[i] = Alpha.alpha_byte;
            break;
        case IL_SHORT:
        case IL_UNSIGNED_SHORT:
            Alpha.alpha_short = (ILushort)(AlphaValue * 0x0000FFFF + .5);
            for (i = AlphaOff-1, j = 0; i < Size; i += AlphaOff, j++)
                ((ILushort*)iCurImage->Data)[i] = Alpha.alpha_short;
            break;
        case IL_INT:
        case IL_UNSIGNED_INT:
            Alpha.alpha_int = (ILuint)(AlphaValue * 0xFFFFFFFF + .5);
            for (i = AlphaOff-1, j = 0; i < Size; i += AlphaOff, j++)
                ((ILuint*)iCurImage->Data)[i] = Alpha.alpha_int;
            break;
        case IL_FLOAT:
            Alpha.alpha_float = (ILfloat)AlphaValue;
            for (i = AlphaOff-1, j = 0; i < Size; i += AlphaOff, j++)
                ((ILfloat*)iCurImage->Data)[i] = Alpha.alpha_float;
            break;
        case IL_DOUBLE:
            Alpha.alpha_double = AlphaValue;
            for (i = AlphaOff-1, j = 0; i < Size; i += AlphaOff, j++)
                ((ILdouble*)iCurImage->Data)[i] = Alpha.alpha_double;
            break;
    }

	return;
}


//! Clamps data values of unsigned bytes from 16 to 235 for display on an
//   NTSC television.  Reasoning for this is given at
//   http://msdn.microsoft.com/en-us/library/bb174608.aspx.
ILboolean ILAPIENTRY ilClampNTSC(void)
{
	ILuint x, y, z, c;
	ILuint Offset = 0;

    if (iCurImage == NULL) {
        ilSetError(IL_ILLEGAL_OPERATION);
        return IL_FALSE;
    }

	if (iCurImage->Type != IL_UNSIGNED_BYTE)  // Should we set an error here?
		return IL_FALSE;

	for (z = 0; z < iCurImage->Depth; z++) {
		for (y = 0; y < iCurImage->Height; y++) {
			for (x = 0; x < iCurImage->Width; x++) {
				for (c = 0; c < iCurImage->Bpp; c++) {
					iCurImage->Data[Offset + c] = IL_LIMIT(iCurImage->Data[Offset + c], 16, 235);
				}
			Offset += iCurImage->Bpp;
			}
		}
	}

	return IL_TRUE;
}
