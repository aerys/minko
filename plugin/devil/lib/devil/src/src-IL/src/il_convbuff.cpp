//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 01/08/2007
//
// Filename: src-IL/src/il_convbuff.cpp
//
// Description: Converts between several image formats
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifdef ALTIVEC_GCC
#include "altivec_typeconversion.h"
#endif
#include <limits.h>


void* ILAPIENTRY iSwitchTypes(ILuint SizeOfData, ILenum SrcType, ILenum DestType, void *Buffer);

//ILushort ILAPIENTRY ilFloatToHalf(ILuint i);
//ILuint   ILAPIENTRY ilHalfToFloat (ILushort y);
//ILfloat  /*ILAPIENTRY*/ ilFloatToHalfOverflow();

ILimage *iConvertPalette(ILimage *Image, ILenum DestFormat);

#define CHECK_ALLOC() 	if (NewData == NULL) { \
							if (Data != Buffer) \
								ifree(Data); \
							return IL_FALSE; \
						}

ILAPI void* ILAPIENTRY ilConvertBuffer(ILuint SizeOfData, ILenum SrcFormat, ILenum DestFormat, ILenum SrcType, ILenum DestType, ILpal *SrcPal, void *Buffer)
{
	//static const	ILfloat LumFactor[3] = { 0.299f, 0.587f, 0.114f };  // Used for conversion to luminance
	//static const	ILfloat LumFactor[3] = { 0.3086f, 0.6094f, 0.0820f };  // http://www.sgi.com/grafica/matrix/index.html
	static const	ILfloat LumFactor[3] = { 0.212671f, 0.715160f, 0.072169f };  // http://www.inforamp.net/~poynton/ and libpng's libpng.txt

	ILubyte		*NewData = NULL;
	ILuint		i, j, c, Size;
	ILfloat		Resultf;
	ILdouble	Resultd;
	ILuint		NumPix;  // Really number of pixels * bpp.
	ILuint		BpcDest;
	void		*Data = NULL;
	ILimage		*PalImage = NULL, *TempImage = NULL;

	if (SizeOfData == 0 || Buffer == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return NULL;
	}

	Data = iSwitchTypes(SizeOfData, SrcType, DestType, Buffer);
	if (Data == NULL)
		return NULL;

	BpcDest = ilGetBpcType(DestType);
	NumPix = SizeOfData / ilGetBpcType(SrcType);

	if (DestFormat == SrcFormat) {
		NewData = (ILubyte*)ialloc(NumPix * BpcDest);
		if (NewData == NULL) {
			return IL_FALSE;
		}
		memcpy(NewData, Data, NumPix * BpcDest);
		if (Data != Buffer)
			ifree(Data);

		return NewData;
	}

	// Colour-indexed images are special here
	if (SrcFormat == IL_COLOUR_INDEX) {
		// We create a temporary palette image so that we can send it to iConvertPalette.
		PalImage = (ILimage*)icalloc(1, sizeof(ILimage));  // Much better to have it all set to 0.
		if (PalImage == NULL)
			return NULL;
		// Populate the temporary palette image.
		PalImage->Pal.Palette = SrcPal->Palette;
		PalImage->Pal.PalSize = SrcPal->PalSize;
		PalImage->Pal.PalType = SrcPal->PalType;
		PalImage->Width = NumPix;
		PalImage->Height = 1;
		PalImage->Depth = 1;
		PalImage->Format = IL_COLOUR_INDEX;
		PalImage->Type = IL_UNSIGNED_BYTE;
		PalImage->Data = (ILubyte*)Buffer;
		PalImage->Bpp = 1;
		PalImage->SizeOfData = SizeOfData;

		// Convert the paletted image to a different format.
		TempImage = iConvertPalette(PalImage, DestFormat);
		if (TempImage == NULL) {
			// So that we do not delete the original palette or data.
			PalImage->Pal.Palette = NULL;
			PalImage->Data = NULL;
			ilCloseImage(PalImage);
			return NULL;
		}

		// Set TempImage->Data to NULL so that we can preserve it via NewData, or
		//  else it would get wiped out by ilCloseImage.
		NewData = TempImage->Data;
		TempImage->Data = NULL;
		// So that we do not delete the original palette or data.
		PalImage->Pal.Palette = NULL;
		PalImage->Data = NULL;

		// Clean up here.
		ilCloseImage(PalImage);
		ilCloseImage(TempImage);
		return NewData;
	}
	
	switch (SrcFormat)
	{
		case IL_RGB:
			switch (DestFormat)
			{
				case IL_BGR:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
						#ifdef ALTIVEC_GCC
							abc2cba_byte((ILubyte*)Data,NumPix * BpcDest,NewData);
						#else
							for (i = 0; i < NumPix; i += 3) {
								NewData[i] = ((ILubyte*)(Data))[i+2];
								NewData[i+1] = ((ILubyte*)(Data))[i+1];
								NewData[i+2] = ((ILubyte*)(Data))[i];
							}
						#endif
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
						#ifdef ALTIVEC_GCC
							abc2cba_short((ILushort*)Data,NumPix * BpcDest,(ILushort*)NewData);
						#else
							for (i = 0; i < NumPix; i += 3) {
								((ILushort*)(NewData))[i] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[i+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[i+2] = ((ILushort*)(Data))[i];
							}
						#endif
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
						#ifdef ALTIVEC_GCC
							abc2cba_int((ILuint*)Data,NumPix * BpcDest,(ILuint*)NewData);
						#else
							for (i = 0; i < NumPix; i += 3) {
								((ILuint*)(NewData))[i] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[i+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[i+2] = ((ILuint*)(Data))[i];
							}
						#endif
							break;
						case IL_FLOAT:
						#ifdef ALTIVEC_GCC
							abc2cba_float(Data,NumPix * BpcDest,NewData);
						#else
							for (i = 0; i < NumPix; i += 3) {
								((ILfloat*)(NewData))[i] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[i+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[i+2] = ((ILfloat*)(Data))[i];
							}
						#endif
							break;
						case IL_DOUBLE:
						#ifdef ALTIVEC_GCC
							abc2cba_double((ILdouble*)Data,NumPix * BpcDest,(ILdouble*)NewData);
						#else
							for (i = 0; i < NumPix; i += 3) {
								((ILdouble*)(NewData))[i] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[i+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[i+2] = ((ILdouble*)(Data))[i];
							}
							break;
						#endif
					}
					break;

				case IL_RGBA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 4 / 3);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								NewData[j] = ((ILubyte*)(Data))[i];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i+2];
								NewData[j+3] = UCHAR_MAX;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+3] = USHRT_MAX;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+3] = UINT_MAX;
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+3] = 1.0f;
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+3] = 1.0;
							}
							break;
					}
					break;

				case IL_BGRA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 4 / 3);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								NewData[j] = ((ILubyte*)(Data))[i+2];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i];
								NewData[j+3] = UCHAR_MAX;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+3] = USHRT_MAX;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+3] = UINT_MAX;
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+3] = 1.0f;
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+3] = 1.0f;
							}
							break;
					}
					break;

				case IL_LUMINANCE:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 3);
					CHECK_ALLOC();
					Size = NumPix / 3;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILubyte*)(Data))[i * 3 + c] * LumFactor[c];
								}
								NewData[i] = (ILubyte)Resultf;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILushort*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILushort*)(NewData))[i] = (ILushort)Resultf;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILuint*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILuint*)(NewData))[i] = (ILuint)Resultf;
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILfloat*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILfloat*)(NewData))[i] = Resultf;
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								Resultd = 0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILdouble*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILdouble*)(NewData))[i] = Resultd;
							}
							break;
					}
					break;

				case IL_LUMINANCE_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 3 * 2);
					CHECK_ALLOC();
					Size = NumPix / 3;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILubyte*)(Data))[i * 3 + c] * LumFactor[c];
								}
								NewData[i*2] = (ILubyte)Resultf;
								NewData[i*2+1] = UCHAR_MAX;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILushort*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILushort*)(NewData))[i*2] = (ILushort)Resultf;
								((ILushort*)(NewData))[i*2+1] = USHRT_MAX;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILuint*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILuint*)(NewData))[i*2] = (ILuint)Resultf;
								((ILuint*)(NewData))[i*2+1] = UINT_MAX;
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILfloat*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILfloat*)(NewData))[i*2] = Resultf;
								((ILfloat*)(NewData))[i*2+1] = 1.0f;
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								Resultd = 0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILdouble*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILdouble*)(NewData))[i*2] = Resultd;
								((ILdouble*)(NewData))[i*2+1] = 1.0;
							}
							break;
					}
					break;

				case IL_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 3);
					CHECK_ALLOC();
					memset(NewData, 0, NumPix * BpcDest);
					break;

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;

		case IL_RGBA:
			switch (DestFormat)
			{
				case IL_BGRA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
						#ifdef ALTIVEC_GCC
							abcd2cbad_byte(NewData,iCurImage->SizeOfData,NewData);
						#else
							for (i = 0; i < NumPix; i += 4) {
								NewData[i] = ((ILubyte*)(Data))[i+2];
								NewData[i+1] = ((ILubyte*)(Data))[i+1];
								NewData[i+2] = ((ILubyte*)(Data))[i];
								NewData[i+3] = ((ILubyte*)(Data))[i+3];
							}
						#endif
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
						#ifdef ALTIVEC_GCC
							abcd2cbad_short((ILushort*)Data,iCurImage->SizeOfData,(ILushort*)NewData);
						#else
							for (i = 0; i < NumPix; i += 4) {
								((ILushort*)(NewData))[i] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[i+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[i+2] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[i+3] = ((ILushort*)(Data))[i+3];
							}
						#endif
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
						#ifdef ALTIVEC_GCC
							abcd2cbad_int((ILuint*)Data,iCurImage->SizeOfData,(ILuint*)NewData);
						#else
							for (i = 0; i < NumPix; i += 4) {
								((ILuint*)(NewData))[i] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[i+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[i+2] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[i+3] = ((ILuint*)(Data))[i+3];
							}
						#endif
							break;
						case IL_FLOAT:
						#ifdef ALTIVEC_GCC
							abcd2cbad_float(Data,iCurImage->SizeOfData,NewData);
						#else
							for (i = 0; i < NumPix; i += 4) {
								((ILfloat*)(NewData))[i] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[i+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[i+2] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[i+3] = ((ILfloat*)(Data))[i+3];
							}
						#endif
							break;
						case IL_DOUBLE:
						#ifdef ALTIVEC_GCC
							abcd2cbad_double((ILdouble*)Data,iCurImage->SizeOfData,(ILdouble*)NewData);
						#else
							for (i = 0; i < NumPix; i += 4) {
								((ILdouble*)(NewData))[i] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[i+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[i+2] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[i+3] = ((ILdouble*)(Data))[i+3];
							}
						#endif
							break;
					}
					break;

				case IL_RGB:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 3 / 4);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								NewData[j] = ((ILubyte*)(Data))[i];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i+2];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i+2];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i+2];
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i+2];
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i+2];
							}
							break;
					}
					break;

				case IL_BGR:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 3 / 4);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								NewData[j] = ((ILubyte*)(Data))[i+2];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i];
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i];
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i];
							}
							break;
					}
					break;

				case IL_LUMINANCE:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 4);
					CHECK_ALLOC();
					Size = NumPix / 4;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILubyte*)(Data))[i * 4 + c] * LumFactor[c];
								}
								NewData[i] = (ILubyte)Resultf;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILushort*)(Data))[i * 4 + c] * LumFactor[c];
								}
								((ILushort*)(NewData))[i] = (ILushort)Resultf;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILuint*)(Data))[i * 4 + c] * LumFactor[c];
								}
								((ILuint*)(NewData))[i] = (ILuint)Resultd;
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILfloat*)(Data))[i * 4 + c] * LumFactor[c];
								}
								((ILfloat*)(NewData))[i] = (ILfloat)Resultd;
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILdouble*)(Data))[i * 4 + c] * LumFactor[c];
								}
								((ILdouble*)(NewData))[i] = Resultd;
							}
							break;
					}
					break;

				case IL_LUMINANCE_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 4 * 2);
					CHECK_ALLOC();
					Size = NumPix / 4 * 2;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i += 2) {
								Resultf = 0.0f;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILubyte*)(Data))[i * 2 + c] * LumFactor[c];
								}
								NewData[i] = (ILubyte)Resultf;
								NewData[i+1] = ((ILubyte*)(Data))[i * 2 + 3];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < Size; i += 2) {
								Resultf = 0.0f;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILushort*)(Data))[i * 2 + c] * LumFactor[c];
								}
								((ILushort*)(NewData))[i] = (ILushort)Resultf;
								((ILushort*)(NewData))[i+1] = ((ILushort*)(Data))[i * 2 + 3];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i += 2) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILuint*)(Data))[i * 2 + c] * LumFactor[c];
								}
								((ILuint*)(NewData))[i] = (ILuint)Resultd;
								((ILuint*)(NewData))[i+1] = ((ILuint*)(Data))[i * 2 + 3];
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i += 2) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILfloat*)(Data))[i * 2 + c] * LumFactor[c];
								}
								((ILfloat*)(NewData))[i] = (ILfloat)Resultd;
								((ILfloat*)(NewData))[i+1] = ((ILfloat*)(Data))[i * 2 + 3];
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i += 2) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILdouble*)(Data))[i * 2 + c] * LumFactor[c];
								}
								((ILdouble*)(NewData))[i] = Resultd;
								((ILdouble*)(NewData))[i+1] = ((ILdouble*)(Data))[i * 2 + 3];
							}
							break;
					}
					break;

				case IL_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 4);
					CHECK_ALLOC();
					Size = NumPix / 4;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								NewData[i] = ((ILubyte*)(Data))[i * 4 + 3];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < Size; i++) {
								((ILushort*)(NewData))[i] = ((ILushort*)(Data))[i * 4 + 3];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								((ILuint*)(NewData))[i] = ((ILuint*)(Data))[i * 4 + 3];
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								((ILfloat*)(NewData))[i] = ((ILfloat*)(Data))[i * 4 + 3];
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								((ILdouble*)(NewData))[i] = ((ILdouble*)(Data))[i * 4 + 3];
							}
							break;
					}
					break;

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;

		case IL_BGR:
			switch (DestFormat)
			{
				case IL_RGB:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
						#ifdef ALTIVEC_GCC
							abc2cba_byte(((ILubyte*)Data),NumPix * BpcDest,NewData);
						#else
							for (i = 0; i < NumPix; i += 3) {
								NewData[i] = ((ILubyte*)(Data))[i+2];
								NewData[i+1] = ((ILubyte*)(Data))[i+1];
								NewData[i+2] = ((ILubyte*)(Data))[i];
							}
						#endif
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
						#ifdef ALTIVEC_GCC
							abc2cba_short((ILushort*)Data,NumPix * BpcDest,(ILushort*)NewData);
						#else
							for (i = 0; i < NumPix; i += 3) {
								((ILushort*)(NewData))[i] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[i+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[i+2] = ((ILushort*)(Data))[i];
							}
						#endif
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
						#ifdef ALTIVEC_GCC
							abc2cba_int((ILuint*)Data,NumPix * BpcDest,(ILuint*)NewData);
						#else
							for (i = 0; i < NumPix; i += 3) {
								((ILuint*)(NewData))[i] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[i+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[i+2] = ((ILuint*)(Data))[i];
							}
						#endif
							break;
						case IL_FLOAT:
						#ifdef ALTIVEC_GCC
							abc2cba_float(Data,NumPix * BpcDest,NewData);
						#else
							for (i = 0; i < NumPix; i += 3) {
								((ILfloat*)(NewData))[i] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[i+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[i+2] = ((ILfloat*)(Data))[i];
							}
						#endif
							break;
						case IL_DOUBLE:
						#ifdef ALTIVEC_GCC
							abc2cba_double((ILdouble*)Data,iCurImage->SizeOfData,(ILdouble*)NewData);
						#else
							for (i = 0; i < NumPix; i += 3) {
								((ILdouble*)(NewData))[i] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[i+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[i+2] = ((ILdouble*)(Data))[i];
							}
						#endif
							break;
					}
					break;

				case IL_BGRA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 4 / 3);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								NewData[j] = ((ILubyte*)(Data))[i];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i+2];
								NewData[j+3] = UCHAR_MAX;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+3] = USHRT_MAX;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+3] = UINT_MAX;
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+3] = 1.0f;
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+3] = 1.0;
							}
							break;
					}
					break;

				case IL_RGBA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 4 / 3);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								NewData[j] = ((ILubyte*)(Data))[i+2];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i];
								NewData[j+3] = UCHAR_MAX;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+3] = USHRT_MAX;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+3] = UINT_MAX;
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+3] = 1.0f;
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+3] = 1.0;
							}
							break;
					}
					break;

				case IL_LUMINANCE:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 3);
					CHECK_ALLOC();
					Size = NumPix / 3;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultf += ((ILubyte*)(Data))[i * 3 + c] * LumFactor[j];
								}
								NewData[i] = (ILubyte)Resultf;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultf += ((ILushort*)(Data))[i * 3 + c] * LumFactor[j];
								}
								((ILushort*)(NewData))[i] = (ILushort)Resultf;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILuint*)(Data))[i * 3 + c] * LumFactor[j];
								}
								((ILuint*)(NewData))[i] = (ILuint)Resultd;
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILfloat*)(Data))[i * 3 + c] * LumFactor[j];
								}
								((ILfloat*)(NewData))[i] = (ILfloat)Resultd;
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILdouble*)(Data))[i * 3 + c] * LumFactor[j];
								}
								((ILdouble*)(NewData))[i] = Resultd;
							}
							break;
					}
					break;

				case IL_LUMINANCE_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 3 * 2);
					CHECK_ALLOC();
					Size = NumPix / 3;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILubyte*)(Data))[i * 3 + c] * LumFactor[c];
								}
								NewData[i*2] = (ILubyte)Resultf;
								NewData[i*2+1] = UCHAR_MAX;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILushort*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILushort*)(NewData))[i*2] = (ILushort)Resultf;
								((ILushort*)(NewData))[i*2+1] = USHRT_MAX;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILuint*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILuint*)(NewData))[i*2] = (ILuint)Resultf;
								((ILuint*)(NewData))[i*2+1] = UINT_MAX;
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILfloat*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILfloat*)(NewData))[i*2] = Resultf;
								((ILfloat*)(NewData))[i*2+1] = 1.0f;
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								Resultd = 0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILdouble*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILdouble*)(NewData))[i*2] = Resultd;
								((ILdouble*)(NewData))[i*2+1] = 1.0;
							}
							break;
					}
					break;

				case IL_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 3);
					CHECK_ALLOC();
					memset(NewData, 0, NumPix * BpcDest / 3);
					break;

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;

		case IL_BGRA:
			switch (DestFormat)
			{
				case IL_RGBA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
						#ifdef ALTIVEC_GCC
							abcd2cbad_byte(NewData,iCurImage->SizeOfData,NewData);
						#else
							for (i = 0; i < NumPix; i += 4) {
								NewData[i] = ((ILubyte*)(Data))[i+2];
								NewData[i+1] = ((ILubyte*)(Data))[i+1];
								NewData[i+2] = ((ILubyte*)(Data))[i];
								NewData[i+3] = ((ILubyte*)(Data))[i+3];
							}
						#endif
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
						#ifdef ALTIVEC_GCC
							abcd2cbad_short((ILushort*)Data,iCurImage->SizeOfData,(ILushort*)NewData);
						#else
							for (i = 0; i < NumPix; i += 4) {
								((ILushort*)(NewData))[i] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[i+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[i+2] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[i+3] = ((ILushort*)(Data))[i+3];
							}
						#endif
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
						#ifdef ALTIVEC_GCC
							abcd2cbad_int((ILuint*)NewData,iCurImage->SizeOfData,(ILuint*)NewData);
						#else
							for (i = 0; i < NumPix; i += 4) {
								((ILuint*)(NewData))[i] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[i+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[i+2] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[i+3] = ((ILuint*)(Data))[i+3];
							}
						#endif
							break;
						case IL_FLOAT:
						#ifdef ALTIVEC_GCC
							abcd2cbad_float(NewData,iCurImage->SizeOfData,NewData);
						#else
							for (i = 0; i < NumPix; i += 4) {
								((ILfloat*)(NewData))[i] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[i+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[i+2] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[i+3] = ((ILfloat*)(Data))[i+3];
							}
						#endif
							break;
						case IL_DOUBLE:
						#ifdef ALTIVEC_GCC
							abcd2cbad_double((ILdouble*)Data,iCurImage->SizeOfData,(ILdouble*)NewData);
						#else
							for (i = 0; i < NumPix; i += 4) {
								((ILdouble*)(NewData))[i] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[i+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[i+2] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[i+3] = ((ILdouble*)(Data))[i+3];
							}
						#endif
							break;
					}
					break;

				case IL_BGR:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 3 / 4);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								NewData[j] = ((ILubyte*)(Data))[i];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i+2];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i+2];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i+2];
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i+2];
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i+2];
							}
							break;
					}
					break;

				case IL_RGB:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 3 / 4);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								NewData[j] = ((ILubyte*)(Data))[i+2];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i];
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i];
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i];
							}
							break;
					}
					break;

				case IL_LUMINANCE:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 4);
					CHECK_ALLOC();
					Size = NumPix / 4;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultf += ((ILubyte*)(Data))[i * 4 + c] * LumFactor[j];
								}
								NewData[i] = (ILubyte)Resultf;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultf += ((ILushort*)(Data))[i * 4 + c] * LumFactor[j];
								}
								((ILushort*)(NewData))[i] = (ILushort)Resultf;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILuint*)(Data))[i * 4 + c] * LumFactor[j];
								}
								((ILuint*)(NewData))[i] = (ILuint)Resultd;
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILfloat*)(Data))[i * 4 + c] * LumFactor[j];
								}
								((ILfloat*)(NewData))[i] = (ILfloat)Resultd;
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILdouble*)(Data))[i * 4 + c] * LumFactor[j];
								}
								((ILdouble*)(NewData))[i] = Resultd;
							}
							break;
					}
					break;

				case IL_LUMINANCE_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 4 * 2);
					CHECK_ALLOC();
					Size = NumPix / 4 * 2;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i += 2) {
								Resultf = 0.0f;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILubyte*)(Data))[i * 2 + c] * LumFactor[c];
								}
								NewData[i] = (ILubyte)Resultf;
								NewData[i+1] = ((ILubyte*)(Data))[i * 2 + 3];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < Size; i += 2) {
								Resultf = 0.0f;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILushort*)(Data))[i * 2 + c] * LumFactor[c];
								}
								((ILushort*)(NewData))[i] = (ILushort)Resultf;
								((ILushort*)(NewData))[i+1] = ((ILushort*)(Data))[i * 2 + 3];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i += 2) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILuint*)(Data))[i * 2 + c] * LumFactor[c];
								}
								((ILuint*)(NewData))[i] = (ILuint)Resultd;
								((ILuint*)(NewData))[i+1] = ((ILuint*)(Data))[i * 2 + 3];
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i += 2) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILfloat*)(Data))[i * 2 + c] * LumFactor[c];
								}
								((ILfloat*)(NewData))[i] = (ILfloat)Resultd;
								((ILfloat*)(NewData))[i+1] = ((ILfloat*)(Data))[i * 2 + 3];
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i += 2) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILdouble*)(Data))[i * 2 + c] * LumFactor[c];
								}
								((ILdouble*)(NewData))[i] = Resultd;
								((ILdouble*)(NewData))[i+1] = ((ILdouble*)(Data))[i * 2 + 3];
							}
							break;
					}
					break;

				case IL_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 4);
					CHECK_ALLOC();
					Size = NumPix / 4;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								NewData[i] = ((ILubyte*)(Data))[i * 4 + 3];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < Size; i++) {
								((ILushort*)(NewData))[i] = ((ILushort*)(Data))[i * 4 + 3];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								((ILuint*)(NewData))[i] = ((ILuint*)(Data))[i * 4 + 3];
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								((ILfloat*)(NewData))[i] = ((ILfloat*)(Data))[i * 4 + 3];
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								((ILdouble*)(NewData))[i] = ((ILdouble*)(Data))[i * 4 + 3];
							}
							break;
					}
					break;

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;


		case IL_LUMINANCE:
			switch (DestFormat)
			{
				case IL_RGB:
				case IL_BGR:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 3);
					CHECK_ALLOC();

					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i++, j += 3) {
								for (c = 0; c < 3; c++) {
									NewData[j + c] = ((ILubyte*)(Data))[i];
								}
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i++, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILushort*)(NewData))[j + c] = ((ILushort*)(Data))[i];
								}
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i++, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILuint*)(NewData))[j + c] = ((ILuint*)(Data))[i];
								}
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i++, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILfloat*)(NewData))[j + c] = ((ILfloat*)(Data))[i];
								}
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i++, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILdouble*)(NewData))[j + c] = ((ILdouble*)(Data))[i];
								}
							}
							break;
					}
					break;

				case IL_RGBA:
				case IL_BGRA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 4);
					CHECK_ALLOC();

					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									NewData[j + c] = ((ILubyte*)(Data))[i];
								}
								NewData[j + 3] = UCHAR_MAX;  // Full opacity
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILushort*)(NewData))[j + c] = ((ILushort*)(Data))[i];
								}
								((ILushort*)(NewData))[j + 3] = USHRT_MAX;  // Full opacity
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILuint*)(NewData))[j + c] = ((ILuint*)(Data))[i];
								}
								((ILuint*)(NewData))[j + 3] = UINT_MAX;  // Full opacity
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILfloat*)(NewData))[j + c] = ((ILfloat*)(Data))[i];
								}
								((ILfloat*)(NewData))[j + 3] = 1.0f;  // Full opacity
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILdouble*)(NewData))[j + c] = ((ILdouble*)(Data))[i];
								}
								((ILdouble*)(NewData))[j + 3] = 1.0;  // Full opacity
							}
							break;
					}
					break;

				case IL_LUMINANCE_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 2);
					CHECK_ALLOC();

					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < NumPix; i++) {
								NewData[i * 2] = ((ILubyte*)(Data))[i];
								NewData[i * 2 + 1] = UCHAR_MAX;  // Full opacity
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < NumPix; i++) {
								((ILushort*)(NewData))[i * 2] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[i * 2 + 1] = USHRT_MAX;  // Full opacity
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < NumPix; i++) {
								((ILuint*)(NewData))[i * 2] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[i * 2 + 1] = UINT_MAX;  // Full opacity
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < NumPix; i++) {
								((ILfloat*)(NewData))[i * 2] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[i * 2 + 1] = 1.0f;  // Full opacity
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < NumPix; i++) {
								((ILdouble*)(NewData))[i * 2] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[i * 2 + 1] = 1.0;  // Full opacity
							}
							break;
					}
					break;

				case IL_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest);
					CHECK_ALLOC();
					memset(NewData, 0, NumPix * BpcDest);
					break;

				/*case IL_COLOUR_INDEX:
					NewData = (ILubyte*)ialloc(iCurImage->SizeOfData);
					NewImage->Pal.Palette = (ILubyte*)ialloc(768);
					if (NewData == NULL || NewImage->Pal.Palette) {
						ifree(NewImage);
						return IL_FALSE;
					}

					// Fill the palette
					for (i = 0; i < 256; i++) {
						for (c = 0; c < 3; c++) {
							NewImage->Pal.Palette[i * 3 + c] = (ILubyte)i;
						}
					}
					// Copy the data
					for (i = 0; i < iCurImage->SizeOfData; i++) {
						NewData[i] = iCurImage->Data[i];
					}
					break;*/

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;


		case IL_LUMINANCE_ALPHA:
			switch (DestFormat)
			{
				case IL_RGB:
				case IL_BGR:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 2 * 3);
					CHECK_ALLOC();

					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 2, j += 3) {
								for (c = 0; c < 3; c++) {
									NewData[j + c] = ((ILubyte*)(Data))[i];
								}
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i += 2, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILushort*)(NewData))[j + c] = ((ILushort*)(Data))[i];
								}
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 2, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILuint*)(NewData))[j + c] = ((ILuint*)(Data))[i];
								}
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 2, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILfloat*)(NewData))[j + c] = ((ILfloat*)(Data))[i];
								}
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 2, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILdouble*)(NewData))[j + c] = ((ILdouble*)(Data))[i];
								}
							}
							break;
					}
					break;

				case IL_RGBA:
				case IL_BGRA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 2 * 4);
					CHECK_ALLOC();

					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 2, j += 4) {
								for (c = 0; c < 3; c++) {
									NewData[j + c] = ((ILubyte*)(Data))[i];
								}
								NewData[j + 3] = ((ILubyte*)(Data))[i+1];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i += 2, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILushort*)(NewData))[j + c] = ((ILushort*)(Data))[i];
								}
								((ILushort*)(NewData))[j + 3] = ((ILushort*)(Data))[i+1];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 2, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILuint*)(NewData))[j + c] = ((ILuint*)(Data))[i];
								}
								((ILuint*)(NewData))[j + 3] = ((ILuint*)(Data))[i+1];
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 2, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILfloat*)(NewData))[j + c] = ((ILfloat*)(Data))[i];
								}
								((ILfloat*)(NewData))[j + 3] = ((ILfloat*)(Data))[i+1];
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 2, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILdouble*)(NewData))[j + c] = ((ILdouble*)(Data))[i];
								}
								((ILdouble*)(NewData))[j + 3] = ((ILdouble*)(Data))[i+1];
							}
							break;
					}
					break;

				case IL_LUMINANCE:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 2);
					CHECK_ALLOC();

					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 2, j++) {
								NewData[j] = ((ILubyte*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i += 2, j++) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 2, j++) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i];
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 2, j++) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i];
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 2, j++) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i];
							}
							break;
					}
					break;

				case IL_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 2);
					CHECK_ALLOC();
					Size = NumPix / 2;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								NewData[i] = ((ILubyte*)(Data))[i * 2 + 3];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < Size; i++) {
								((ILushort*)(NewData))[i] = ((ILushort*)(Data))[i * 2 + 3];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								((ILuint*)(NewData))[i] = ((ILuint*)(Data))[i * 2 + 3];
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								((ILfloat*)(NewData))[i] = ((ILfloat*)(Data))[i * 2 + 3];
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								((ILdouble*)(NewData))[i] = ((ILdouble*)(Data))[i * 2 + 3];
							}
							break;
					}
					break;

				/*case IL_COLOUR_INDEX:
					NewData = (ILubyte*)ialloc(iCurImage->SizeOfData);
					NewImage->Pal.Palette = (ILubyte*)ialloc(768);
					if (NewData == NULL || NewImage->Pal.Palette) {
						ifree(NewImage);
						return IL_FALSE;
					}

					// Fill the palette
					for (i = 0; i < 256; i++) {
						for (c = 0; c < 3; c++) {
							NewImage->Pal.Palette[i * 3 + c] = (ILubyte)i;
						}
					}
					// Copy the data
					for (i = 0; i < iCurImage->SizeOfData; i++) {
						NewData[i] = iCurImage->Data[i];
					}
					break;*/

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;


		case IL_ALPHA:
			switch (DestFormat)
			{
				case IL_RGB:
				case IL_BGR:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 3);
					CHECK_ALLOC();

					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
						case IL_UNSIGNED_INT:
						case IL_INT:
						case IL_FLOAT:
						case IL_DOUBLE:
							memset(NewData, 0, NumPix * BpcDest * 3);  // Easy enough
							break;
						//@TODO: Do we need to a check for default: (error)?
					}
					break;

				case IL_RGBA:
				case IL_BGRA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 4);
					CHECK_ALLOC();

					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									NewData[j + c] = 0;
								}
								NewData[j + 3] = ((ILubyte*)(Data))[i];  // Only value that matters
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILushort*)(NewData))[j + c] = 0;
								}
								((ILushort*)(NewData))[j + 3] = ((ILushort*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILuint*)(NewData))[j + c] = 0;
								}
								((ILuint*)(NewData))[j + 3] = ((ILuint*)(Data))[i];
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILfloat*)(NewData))[j + c] = 0;
								}
								((ILfloat*)(NewData))[j + 3] = ((ILfloat*)(Data))[i];
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILdouble*)(NewData))[j + c] = 0;
								}
								((ILdouble*)(NewData))[j + 3] = ((ILdouble*)(Data))[i];
							}
							break;
					}
					break;

				case IL_LUMINANCE_ALPHA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 2);
					CHECK_ALLOC();

					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < NumPix; i++) {
								NewData[i * 2] = 0;
								NewData[i * 2 + 1] = ((ILubyte*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
						case IL_HALF:
							for (i = 0; i < NumPix; i++) {
								((ILushort*)(NewData))[i * 2] = 0;
								((ILushort*)(NewData))[i * 2 + 1] = ((ILushort*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < NumPix; i++) {
								((ILuint*)(NewData))[i * 2] = 0;
								((ILuint*)(NewData))[i * 2 + 1] = ((ILuint*)(Data))[i];
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < NumPix; i++) {
								((ILfloat*)(NewData))[i * 2] = 0;
								((ILfloat*)(NewData))[i * 2 + 1] = ((ILfloat*)(Data))[i];
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < NumPix; i++) {
								((ILdouble*)(NewData))[i * 2] = 0;
								((ILdouble*)(NewData))[i * 2 + 1] = ((ILdouble*)(Data))[i];
							}
							break;
					}
					break;


				/*case IL_COLOUR_INDEX:
					NewData = (ILubyte*)ialloc(iCurImage->SizeOfData);
					NewImage->Pal.Palette = (ILubyte*)ialloc(768);
					if (NewData == NULL || NewImage->Pal.Palette) {
						ifree(NewImage);
						return IL_FALSE;
					}

					// Fill the palette
					for (i = 0; i < 256; i++) {
						for (c = 0; c < 3; c++) {
							NewImage->Pal.Palette[i * 3 + c] = (ILubyte)i;
						}
					}
					// Copy the data
					for (i = 0; i < iCurImage->SizeOfData; i++) {
						NewData[i] = iCurImage->Data[i];
					}
					break;*/

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;
	}

	if (Data != Buffer)
		ifree(Data);

	return NewData;
}


// Really shouldn't have to check for default, as in above ilConvertBuffer().
//  This now converts better from lower bpp to higher bpp.  For example, when
//  converting from 8 bpp to 16 bpp, if the value is 0xEC, the new value is 0xECEC
//  instead of 0xEC00.
void* ILAPIENTRY iSwitchTypes(ILuint SizeOfData, ILenum SrcType, ILenum DestType, void *Buffer)
{
	ILuint		BpcSrc, BpcDest, Size, i;
	ILubyte		*NewData, *BytePtr;
	ILushort	*ShortPtr;
	ILuint		*IntPtr;
	ILfloat		*FloatPtr, tempFloat;
	ILdouble	*DblPtr, tempDouble;
	ILushort	*HalfPtr;

	BpcSrc = ilGetBpcType(SrcType);
	BpcDest = ilGetBpcType(DestType);

	if (BpcSrc == 0 || BpcDest == 0) {
		ilSetError(IL_INTERNAL_ERROR);
		return IL_FALSE;
	}

	Size = SizeOfData / BpcSrc;

	//if (BpcSrc == BpcDest) {
	if (SrcType == DestType) {
		return Buffer;
	}

	NewData = (ILubyte*)ialloc(Size * BpcDest);
	if (NewData == NULL) {
		return IL_FALSE;
	}

	switch (DestType)
	{
		case IL_UNSIGNED_BYTE:
		case IL_BYTE:
			BytePtr = (ILubyte*)NewData;
			switch (SrcType)
			{
				case IL_UNSIGNED_SHORT:
				case IL_SHORT:
					for (i = 0; i < Size; i++) {
						BytePtr[i] = ((ILushort*)Buffer)[i] >> 8;
					}
					break;
				case IL_UNSIGNED_INT:
				case IL_INT:
					for (i = 0; i < Size; i++) {
						BytePtr[i] = ((ILuint*)Buffer)[i] >> 24;
					}
					break;
				case IL_FLOAT:
					for (i = 0; i < Size; i++) {
						#if CLAMP_FLOATS
							tempFloat = IL_CLAMP(((ILfloat*)Buffer)[i]);
							BytePtr[i] = (ILubyte)(tempFloat * UCHAR_MAX);
						#else
							BytePtr[i] = (ILubyte)(((ILfloat*)Buffer)[i] * UCHAR_MAX);
						#endif
					}
					break;
				case IL_HALF:
					for (i = 0; i < Size; i++) {
						#if CLAMP_HALF
							*((ILuint*)&tempFloat) = ilHalfToFloat(((ILushort*)Buffer)[i]);
							tempFloat = IL_CLAMP(tempFloat);					
							BytePtr[i] = (ILubyte)(tempFloat * UCHAR_MAX);
						#else
							*((ILuint*)&tempFloat) = ilHalfToFloat(((ILushort*)Buffer)[i]);
							BytePtr[i] = (ILubyte)(tempFloat * UCHAR_MAX);
						#endif
					}
					break;
				case IL_DOUBLE:
					for (i = 0; i < Size; i++) {
						#if CLAMP_DOUBLES
							tempDouble = IL_CLAMP(((ILdouble*)Buffer)[i]);
							BytePtr[i] = (ILubyte)(tempDouble * UCHAR_MAX);
						#else
							BytePtr[i] = (ILubyte)( ((ILdouble*)Buffer)[i] * UCHAR_MAX);
						#endif
					}
					break;
			}
			break;

		case IL_UNSIGNED_SHORT:
		case IL_SHORT:
			ShortPtr = (ILushort*)NewData;
			switch (SrcType)
			{
				case IL_UNSIGNED_BYTE:
				case IL_BYTE:
					for (i = 0; i < Size; i++) {
						ShortPtr[i] = (((ILubyte*)Buffer)[i] << 8) | ((ILubyte*)Buffer)[i];
					}
					break;
				case IL_UNSIGNED_INT:
				case IL_INT:
					for (i = 0; i < Size; i++) {
						ShortPtr[i] = ((ILuint*)Buffer)[i] >> 16;
					}
					break;
				case IL_FLOAT:
					for (i = 0; i < Size; i++) {
						#if CLAMP_FLOATS
							tempFloat = IL_CLAMP(((ILfloat*)Buffer)[i]);
							ShortPtr[i] = (ILushort)(tempFloat * USHRT_MAX);
						#else
							ShortPtr[i] = (ILushort)( ((ILfloat*)Buffer)[i] * USHRT_MAX);
						#endif
					}
					break;
				case IL_HALF:
					for (i = 0; i < Size; i++) {
						#if CLAMP_FLOATS
							*((ILuint*)&tempFloat) = ilHalfToFloat(((ILushort*)Buffer)[i]);
							tempFloat = IL_CLAMP(tempFloat);					
							ShortPtr[i] = (ILushort)(tempFloat * USHRT_MAX);
						#else
							*((ILuint*)&tempFloat) = ilHalfToFloat(((ILushort*)Buffer)[i]);
							ShortPtr[i] = (ILushort)(tempFloat * USHRT_MAX);
						#endif
					}
					break;
				case IL_DOUBLE:
					for (i = 0; i < Size; i++) {
						#if CLAMP_DOUBLES
							tempDouble = IL_CLAMP(((ILdouble*)Buffer)[i]);
							ShortPtr[i] = (ILushort)(tempDouble * USHRT_MAX);
						#else
							ShortPtr[i] = (ILushort)( ((ILdouble*)Buffer)[i] * USHRT_MAX);
						#endif
					}
					break;
			}
			break;

		case IL_UNSIGNED_INT:
		case IL_INT:
			IntPtr = (ILuint*)NewData;
			switch (SrcType)
			{
				case IL_UNSIGNED_BYTE:
				case IL_BYTE:
					for (i = 0; i < Size; i++) {
						IntPtr[i] = (((ILubyte*)Buffer)[i] << 24) | (((ILubyte*)Buffer)[i] << 16) | 
							(((ILubyte*)Buffer)[i] << 8) | ((ILubyte*)Buffer)[i];
					}
					break;
				case IL_UNSIGNED_SHORT:
				case IL_SHORT:
					for (i = 0; i < Size; i++) {
						IntPtr[i] = (((ILushort*)Buffer)[i] << 16) | ((ILushort*)Buffer)[i];
					}
					break;
				case IL_FLOAT:
					for (i = 0; i < Size; i++) {
						#if CLAMP_FLOATS
							tempFloat = IL_CLAMP(((ILfloat*)Buffer)[i]);
							IntPtr[i] = (ILuint)(tempFloat * UINT_MAX);
						#else
							IntPtr[i] = (ILuint)( ((ILfloat*)Buffer)[i] * UINT_MAX);
						#endif
					}
					break;
				case IL_HALF:
					for (i = 0; i < Size; i++) {
						#if CLAMP_FLOATS
							*((ILuint*)&tempFloat) = ilHalfToFloat(((ILushort*)Buffer)[i]);
							tempFloat = IL_CLAMP(tempFloat);
							IntPtr[i] = (ILuint)(tempFloat * UINT_MAX);
						#else
						*((ILuint*)&tempFloat) = ilHalfToFloat(((ILushort*)Buffer)[i]);
						IntPtr[i] = (ILuint)(tempFloat * UINT_MAX);
						#endif
					}
					break;
				case IL_DOUBLE:
					for (i = 0; i < Size; i++) {
						#if CLAMP_DOUBLES
							tempDouble = IL_CLAMP(((ILdouble*)Buffer)[i]);
							IntPtr[i] = (ILuint)(tempDouble * UINT_MAX);
						#else
							IntPtr[i] = (ILuint)( ((ILdouble*)Buffer)[i] * UINT_MAX);
						#endif
					}
					break;
			}
			break;

		// @TODO:  Handle signed better.
		case IL_FLOAT:
			FloatPtr = (ILfloat*)NewData;
			switch (SrcType)
			{
				case IL_UNSIGNED_BYTE:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = ((ILubyte*)Buffer)[i] / (ILfloat)UCHAR_MAX;
					}
					break;
				case IL_BYTE:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = ((ILbyte*)Buffer)[i] / (ILfloat)UCHAR_MAX;
					}
					break;
				case IL_UNSIGNED_SHORT:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = ((ILushort*)Buffer)[i] / (ILfloat)USHRT_MAX;
					}
					break;
				case IL_SHORT:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = ((ILshort*)Buffer)[i] / (ILfloat)USHRT_MAX;
					}
					break;
				case IL_UNSIGNED_INT:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = (ILfloat)((ILuint*)Buffer)[i] / (ILfloat)UINT_MAX;
					}
					break;
				case IL_INT:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = (ILfloat)((ILint*)Buffer)[i] / (ILfloat)UINT_MAX;
					}
					break;
				case IL_HALF:
					for (i = 0; i < Size; i++) {
						*((ILuint*)&FloatPtr[i]) = ilHalfToFloat(((ILushort*)Buffer)[i]);
					}
					break;
				case IL_DOUBLE:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = (ILfloat)((ILdouble*)Buffer)[i];
					}
					break;
			}
			break;

		case IL_DOUBLE:
			DblPtr = (ILdouble*)NewData;
			switch (SrcType)
			{
				case IL_UNSIGNED_BYTE:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILubyte*)Buffer)[i] / (ILdouble)UCHAR_MAX;
					}
					break;
				case IL_BYTE:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILbyte*)Buffer)[i] / (ILdouble)UCHAR_MAX;
					}
					break;
				case IL_UNSIGNED_SHORT:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILushort*)Buffer)[i] / (ILdouble)USHRT_MAX;
					}
					break;
				case IL_SHORT:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILshort*)Buffer)[i] / (ILdouble)USHRT_MAX;
					}
					break;
				case IL_UNSIGNED_INT:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILuint*)Buffer)[i] / (ILdouble)UINT_MAX;
					}
					break;
				case IL_INT:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILint*)Buffer)[i] / (ILdouble)UINT_MAX;
					}
					break;
				case IL_HALF:
					for (i = 0; i < Size; i++) {
						*(ILuint*)&tempFloat = ilHalfToFloat(((ILushort*)Buffer)[i]);
						DblPtr[i] = tempFloat;
					}
					break;
				case IL_FLOAT:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILfloat*)Buffer)[i];
					}
					break;
			}
			break;
	
		case IL_HALF:
			HalfPtr = (ILushort*)NewData;
			switch (SrcType)
			{
			case IL_UNSIGNED_BYTE:
				for (i = 0; i < Size; i++) {
					tempFloat = ((ILubyte*)Buffer)[i] / (ILfloat)UCHAR_MAX;
					*((ILushort*)&HalfPtr[i]) = ilFloatToHalf(*(ILuint*)&tempFloat);
				}
				break;
			case IL_BYTE:
				for (i = 0; i < Size; i++) {
					tempFloat = ((ILbyte*)Buffer)[i] / (ILfloat)UCHAR_MAX;
					*((ILushort*)&HalfPtr[i]) = ilFloatToHalf(*(ILuint*)&tempFloat);
				}
				break;
			case IL_UNSIGNED_SHORT:
				for (i = 0; i < Size; i++) {
					tempFloat = ((ILushort*)Buffer)[i] / (ILfloat)USHRT_MAX;
					*((ILushort*)&HalfPtr[i]) = ilFloatToHalf(*(ILuint*)&tempFloat);
				}
				break;
			case IL_SHORT:
				for (i = 0; i < Size; i++) {
					tempFloat = ((ILshort*)Buffer)[i] / (ILfloat)USHRT_MAX;
					*((ILushort*)&HalfPtr[i]) = ilFloatToHalf(*(ILuint*)&tempFloat);
				}
				break;
			case IL_UNSIGNED_INT:
				for (i = 0; i < Size; i++) {
					tempFloat = ((ILuint*)Buffer)[i] / (ILfloat)UINT_MAX;
					*((ILushort*)&HalfPtr[i]) = ilFloatToHalf(*(ILuint*)&tempFloat);
				}
				break;
			case IL_INT:
				for (i = 0; i < Size; i++) {
					tempFloat = ((ILint*)Buffer)[i] / (ILfloat)UINT_MAX;
					*((ILushort*)&HalfPtr[i]) = ilFloatToHalf(*(ILuint*)&tempFloat);
				}
				break;
			case IL_DOUBLE:
				for (i = 0; i < Size; i++) {
					tempFloat = (ILfloat)((ILdouble*)Buffer)[i];
					*((ILushort*)&HalfPtr[i]) = ilFloatToHalf(*(ILuint*)&tempFloat);
				}
				break;
			case IL_FLOAT:
				for (i = 0; i < Size; i++) {
					tempFloat = ((ILfloat*)Buffer)[i];
					*((ILushort*)&HalfPtr[i]) = ilFloatToHalf(*(ILuint*)&tempFloat);
				}
				break;
			}
			break;
	}


	return NewData;
}




