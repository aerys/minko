//-----------------------------------------------------------------------------
// Description: Functions for run-length encoding
//-----------------------------------------------------------------------------

// RLE code from TrueVision's TGA sample code available as Tgautils.zip at
//	ftp://ftp.truevision.com/pub/TGA.File.Format.Spec/PC.Version

#define IL_RLE_C

#include "il_internal.h"
#include "il_rle.h"

ILboolean ilRleCompressLine(ILubyte *p, ILuint n, ILubyte bpp,
			ILubyte *q, ILuint *DestWidth, ILenum CompressMode) {
	
	ILint		DiffCount;		// pixel count until two identical
	ILint		SameCount;		// number of identical adjacent pixels
	ILint		RLEBufSize = 0; // count of number of bytes encoded
	ILint		MaxRun;
	const ILint bmp_pad_to_even = (ILint)(1 - ((size_t)q - *DestWidth) % 2);

	switch( CompressMode ) {
		case IL_TGACOMP:
			MaxRun = TGA_MAX_RUN;
			break;
		case IL_SGICOMP:
			MaxRun = SGI_MAX_RUN;
			break;
		case IL_BMPCOMP:
			MaxRun = BMP_MAX_RUN;
			break;
		default:
			ilSetError(IL_INVALID_PARAM);
			return IL_FALSE;
	}

	while( n > 0 ) {	
		// Analyze pixels
		DiffCount = CountDiffPixels(p, bpp, (ILint)n > MaxRun ? MaxRun : n);
		SameCount = CountSamePixels(p, bpp, (ILint)n > MaxRun ? MaxRun : n);

		if( CompressMode == IL_BMPCOMP ) {
			ILint remaining_data = n - DiffCount - SameCount;
			if( remaining_data < 3  ) { // check if the run has gone near the end
				// no absolute run can be done
				// complete the line adding 0x01 + pixel, for each pixel
				while( remaining_data > 0 ) {
					*q++ = 0x01;
					*q++ = *p++;
					remaining_data--;
				}
				DiffCount = 0;
				SameCount = 0;
				n = 0;
			}
		}

		if( DiffCount > 0 ) { // create a raw packet (bmp absolute run)
			switch(CompressMode) {
				case IL_TGACOMP:
					*q++ = (ILbyte)(DiffCount - 1);
					break;
				case IL_BMPCOMP:
					*q++ = 0x00; RLEBufSize++;
					*q++ = (ILbyte)DiffCount;
					break;
				case IL_SGICOMP:
					*q++ = (ILbyte)(DiffCount | 0x80);
					break;
			}
			n -= DiffCount;
			RLEBufSize += (DiffCount * bpp) + 1;

			while( DiffCount > 0 ) {
				switch(bpp) {
					case 4:	*q++ = *p++;
					case 3: *q++ = *p++;
					case 2: *q++ = *p++;
					case 1: *q++ = *p++;
				}
				DiffCount--;
			}
		
			if( CompressMode == IL_BMPCOMP ) {
				if( (size_t)q % 2 == bmp_pad_to_even ) {
					*q++ = 0x00; // insert padding
				}
			}
		}

		if( SameCount > 1 ) { // create a RLE packet
			switch(CompressMode) {
				case IL_TGACOMP:
					*q++ = (ILbyte)((SameCount - 1) | 0x80);
					break;
				case IL_SGICOMP:
				case IL_BMPCOMP:
					*q++ = (ILbyte)(SameCount);
					break;
			}
			n -= SameCount;
			RLEBufSize += bpp + 1;
			p += (SameCount - 1) * bpp;
			*q++ = *p++;
			switch(bpp) {
				case 4:	*q++ = *p++;
				case 3: *q++ = *p++;
				case 2: *q++ = *p++;
				case 1: *q++ = *p++;
			}
		}
	}

	// write line termination code
	switch(CompressMode) {
		case IL_SGICOMP:
			++RLEBufSize;
			*q++ = 0;
			break;
		case IL_BMPCOMP: 
			*q++ = 0x00; RLEBufSize++;
			*q++ = 0x00; RLEBufSize++;
			break;
	}
	*DestWidth = RLEBufSize;
	
	return IL_TRUE;
}


// Compresses an entire image using run-length encoding
ILuint ilRleCompress(ILubyte *Data, ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp,
		ILubyte *Dest, ILenum CompressMode, ILuint *ScanTable) {
	ILuint DestW = 0, i, j, LineLen, Bps = Width * Bpp, SizeOfPlane = Width * Height * Bpp;

	if( ScanTable )
		imemclear(ScanTable,Depth*Height*sizeof(ILuint));
	for( j = 0; j < Depth; j++ ) {
		for( i = 0; i < Height; i++ ) {
			if( ScanTable )
				*ScanTable++ = DestW;
			ilRleCompressLine(Data + j * SizeOfPlane + i * Bps, Width, Bpp, Dest + DestW, &LineLen, CompressMode);
			DestW += LineLen;
		}
	}
	
	if( CompressMode == IL_BMPCOMP ) { // add end of image
		*(Data+DestW) = 0x00; DestW++;
		*(Data+DestW) = 0x01; DestW++;
	}

	return DestW;
}
