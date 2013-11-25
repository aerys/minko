//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 02/01/2009
//
// Filename: src-IL/src/il_devil.c
//
// Description: Functions for working with the ILimage's and the current image
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#include <string.h>
#include <limits.h>
#include "il_manip.h"


ILAPI ILboolean ILAPIENTRY ilInitImage(ILimage *Image, ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp, ILenum Format, ILenum Type, void *Data)
{
	ILubyte BpcType = ilGetBpcType(Type);
	if (BpcType == 0) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	memset(Image, 0, sizeof(ILimage));

	////
	if (Width  == 0) Width = 1;
	if (Height == 0) Height = 1;
	if (Depth  == 0) Depth = 1;
	Image->Width	   = Width;
	Image->Height	   = Height;
	Image->Depth	   = Depth;
	Image->Bpp		   = Bpp;
	Image->Bpc		   = BpcType;
	Image->Bps		   = Width * Bpp * Image->Bpc;
	Image->SizeOfPlane = Image->Bps * Height;
	Image->SizeOfData  = Image->SizeOfPlane * Depth;
	Image->Format	   = Format;
	Image->Type 	   = Type;
	Image->Origin	   = IL_ORIGIN_LOWER_LEFT;
	Image->Pal.PalType = IL_PAL_NONE;
	Image->DxtcFormat  = IL_DXT_NO_COMP;
	Image->DxtcData    = NULL;

	Image->Data = (ILubyte*)ialloc(Image->SizeOfData);
	if (Image->Data == NULL) {
		return IL_FALSE;
	}

	if (Data != NULL) {
		memcpy(Image->Data, Data, Image->SizeOfData);
	}

	return IL_TRUE;
}



// Creates a new ILimage based on the specifications given
ILAPI ILimage* ILAPIENTRY ilNewImage(ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp, ILubyte Bpc)
{
	ILimage *Image;

	if (Bpp == 0 || Bpp > 4) {
		return NULL;
	}

	Image = (ILimage*)ialloc(sizeof(ILimage));
	if (Image == NULL) {
		return NULL;
	}

	if (!ilInitImage(Image, Width, Height, Depth, Bpp, ilGetFormatBpp(Bpp), ilGetTypeBpc(Bpc), NULL)) {
		if (Image->Data != NULL) {
			ifree(Image->Data);
		}
		ifree(Image);
		return NULL;
	}
	
	return Image;
}


// Same as above but allows specification of Format and Type
ILAPI ILimage* ILAPIENTRY ilNewImageFull(ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp, ILenum Format, ILenum Type, void *Data)
{
	ILimage *Image;

	if (Bpp == 0 || Bpp > 4) {
		return NULL;
	}

	Image = (ILimage*)ialloc(sizeof(ILimage));
	if (Image == NULL) {
		return NULL;
	}

	if (!ilInitImage(Image, Width, Height, Depth, Bpp, Format, Type, Data)) {
		if (Image->Data != NULL) {
			ifree(Image->Data);
		}
		ifree(Image);
		return NULL;
	}
	
	return Image;
}


//! Changes the current bound image to use these new dimensions (current data is destroyed).
/*! \param Width Specifies the new image width.  This cannot be 0.
	\param Height Specifies the new image height.  This cannot be 0.
	\param Depth Specifies the new image depth.  This cannot be 0.
	\param Bpp Number of channels (ex. 3 for RGB)
	\param Format Enum of the desired format.  Any format values are accepted.
	\param Type Enum of the desired type.  Any type values are accepted.
	\param Data Specifies data that should be copied to the new image. If this parameter is NULL, no data is copied, and the new image data consists of undefined values.
	\exception IL_ILLEGAL_OPERATION No currently bound image.
	\exception IL_INVALID_PARAM One of the parameters is incorrect, such as one of the dimensions being 0.
	\exception IL_OUT_OF_MEMORY Could not allocate enough memory.
	\return Boolean value of failure or success*/
ILboolean ILAPIENTRY ilTexImage(ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp, ILenum Format, ILenum Type, void *Data)
{
	return ilTexImage_(iCurImage, Width, Height, Depth, Bpp, Format, Type, Data);
}


// Internal version of ilTexImage.
ILAPI ILboolean ILAPIENTRY ilTexImage_(ILimage *Image, ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp, ILenum Format, ILenum Type, void *Data)
{
	if (Image == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	////

	// Not sure if we should be getting rid of the palette...
	if (Image->Pal.Palette && Image->Pal.PalSize && Image->Pal.PalType != IL_PAL_NONE) {
		ifree(Image->Pal.Palette);
	}

	ilCloseImage(Image->Mipmaps);
	ilCloseImage(Image->Next);
	ilCloseImage(Image->Faces);
	ilCloseImage(Image->Layers);

	if (Image->AnimList) ifree(Image->AnimList);
	if (Image->Profile)  ifree(Image->Profile);
	if (Image->DxtcData) ifree(Image->DxtcData);
	if (Image->Data)	 ifree(Image->Data);

	////

	//@TODO: Also check against format?
	/*if (Width == 0 || Height == 0 || Depth == 0 || Bpp == 0) {
		ilSetError(IL_INVALID_PARAM);
	return IL_FALSE;
	}*/

	return ilInitImage(Image, Width, Height, Depth, Bpp, Format, Type, Data);
}


//! Uploads Data of the same size to replace the current image's data.
/*! \param Data New image data to update the currently bound image
	\exception IL_ILLEGAL_OPERATION No currently bound image
	\exception IL_INVALID_PARAM Data was NULL.
	\return Boolean value of failure or success
*/
ILboolean ILAPIENTRY ilSetData(void *Data)
{
	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}
	
	return ilTexSubImage_(iCurImage, Data);
}


// Internal version of ilTexSubImage.
ILAPI ILboolean ILAPIENTRY ilTexSubImage_(ILimage *Image, void *Data)
{
	if (Image == NULL || Data == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}
	if (!Image->Data) {
		Image->Data = (ILubyte*)ialloc(Image->SizeOfData);
		if (Image->Data == NULL)
			return IL_FALSE;
	}
	memcpy(Image->Data, Data, Image->SizeOfData);
	return IL_TRUE;
}


//! Returns a pointer to the current image's data.
/*! The pointer to the image data returned by this function is only valid until any
    operations are done on the image.  After any operations, this function should be
	called again.  The pointer can be cast to other types for images that have more
	than one byte per channel for easier access to data.
	\exception IL_ILLEGAL_OPERATION No currently bound image
	\return ILubyte pointer to image data.*/
ILubyte* ILAPIENTRY ilGetData(void)
{
	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return NULL;
	}
	
	return iCurImage->Data;
}


//! Returns a pointer to the current image's palette data.
/*! The pointer to the image palette data returned by this function is only valid until
	any operations are done on the image.  After any operations, this function should be
	called again.
	\exception IL_ILLEGAL_OPERATION No currently bound image
	\return ILubyte pointer to image palette data.*/
ILubyte* ILAPIENTRY ilGetPalette(void)
{
	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return NULL;
	}
	
	return iCurImage->Pal.Palette;
}


//ILfloat ClearRed = 0.0f, ClearGreen = 0.0f, ClearBlue = 0.0f, ClearAlpha = 0.0f;

// Changed to the colour of the Universe
//	(http://www.newscientist.com/news/news.jsp?id=ns99991775)
//	*(http://www.space.com/scienceastronomy/universe_color_020308.html)*
//ILfloat ClearRed = 0.269f, ClearGreen = 0.388f, ClearBlue = 0.342f, ClearAlpha = 0.0f;
static ILfloat ClearRed   = 1.0f;
static ILfloat ClearGreen = 0.972549f;
static ILfloat ClearBlue  = 0.90588f;
static ILfloat ClearAlpha = 0.0f;
static ILfloat ClearLum   = 1.0f;

void ILAPIENTRY ilClearColour(ILclampf Red, ILclampf Green, ILclampf Blue, ILclampf Alpha)
{
	// Clamp to 0.0f - 1.0f.
	ClearRed	= Red < 0.0f ? 0.0f : (Red > 1.0f ? 1.0f : Red);
	ClearGreen	= Green < 0.0f ? 0.0f : (Green > 1.0f ? 1.0f : Green);
	ClearBlue	= Blue < 0.0f ? 0.0f : (Blue > 1.0f ? 1.0f : Blue);
	ClearAlpha	= Alpha < 0.0f ? 0.0f : (Alpha > 1.0f ? 1.0f : Alpha);
	
	if ((Red == Green) && (Red == Blue) && (Green == Blue)) {
		ClearLum = Red < 0.0f ? 0.0f : (Red > 1.0f ? 1.0f : Red);
	}
	else {
		ClearLum = 0.212671f * ClearRed + 0.715160f * ClearGreen + 0.072169f * ClearBlue;
		ClearLum = ClearLum < 0.0f ? 0.0f : (ClearLum > 1.0f ? 1.0f : ClearLum);
	}
	
	return;
}


ILAPI void ILAPIENTRY ilGetClear(void *Colours, ILenum Format, ILenum Type)
{
	ILubyte 	*BytePtr;
	ILushort	*ShortPtr;
	ILuint		*IntPtr;
	ILfloat 	*FloatPtr;
	ILdouble	*DblPtr;
	
	switch (Type)
	{
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
			BytePtr = (ILubyte*)Colours;
			switch (Format)
			{
				case IL_RGB:
					BytePtr[0] = (ILubyte)(ClearRed * UCHAR_MAX);
					BytePtr[1] = (ILubyte)(ClearGreen * UCHAR_MAX);
					BytePtr[2] = (ILubyte)(ClearBlue * UCHAR_MAX);
					break;
					
				case IL_RGBA:
					BytePtr[0] = (ILubyte)(ClearRed * UCHAR_MAX);
					BytePtr[1] = (ILubyte)(ClearGreen * UCHAR_MAX);
					BytePtr[2] = (ILubyte)(ClearBlue * UCHAR_MAX);
					BytePtr[3] = (ILubyte)(ClearAlpha * UCHAR_MAX);
					break;
					
				case IL_BGR:
					BytePtr[2] = (ILubyte)(ClearRed * UCHAR_MAX);
					BytePtr[1] = (ILubyte)(ClearGreen * UCHAR_MAX);
					BytePtr[0] = (ILubyte)(ClearBlue * UCHAR_MAX);
					BytePtr[3] = (ILubyte)(ClearAlpha * UCHAR_MAX);
					break;
					
				case IL_BGRA:
					BytePtr[2] = (ILubyte)(ClearRed * UCHAR_MAX);
					BytePtr[1] = (ILubyte)(ClearGreen * UCHAR_MAX);
					BytePtr[0] = (ILubyte)(ClearBlue * UCHAR_MAX);
					BytePtr[3] = (ILubyte)(ClearAlpha * UCHAR_MAX);
					break;
					
				case IL_LUMINANCE:
					BytePtr[0] = (ILubyte)(ClearAlpha * UCHAR_MAX);
					break;
					
				case IL_LUMINANCE_ALPHA:
					BytePtr[0] = (ILubyte)(ClearLum * UCHAR_MAX);
					BytePtr[1] = (ILubyte)(ClearAlpha * UCHAR_MAX);
					
				case IL_COLOUR_INDEX:
					BytePtr[0] = (ILubyte)(ClearAlpha * UCHAR_MAX);
					break;
					
				default:
					ilSetError(IL_INTERNAL_ERROR);
					return;
			}
				break;
			
		case IL_SHORT:
		case IL_UNSIGNED_SHORT:
			ShortPtr = (ILushort*)Colours;
			switch (Format)
			{
				case IL_RGB:
					ShortPtr[0] = (ILushort)(ClearRed * USHRT_MAX);
					ShortPtr[1] = (ILushort)(ClearGreen * USHRT_MAX);
					ShortPtr[2] = (ILushort)(ClearBlue * USHRT_MAX);
					break;
					
				case IL_RGBA:
					ShortPtr[0] = (ILushort)(ClearRed * USHRT_MAX);
					ShortPtr[1] = (ILushort)(ClearGreen * USHRT_MAX);
					ShortPtr[2] = (ILushort)(ClearBlue * USHRT_MAX);
					ShortPtr[3] = (ILushort)(ClearAlpha * USHRT_MAX);
					break;
					
				case IL_BGR:
					ShortPtr[2] = (ILushort)(ClearRed * USHRT_MAX);
					ShortPtr[1] = (ILushort)(ClearGreen * USHRT_MAX);
					ShortPtr[0] = (ILushort)(ClearBlue * USHRT_MAX);
					ShortPtr[3] = (ILushort)(ClearAlpha * USHRT_MAX);
					break;
					
				case IL_BGRA:
					ShortPtr[2] = (ILushort)(ClearRed * USHRT_MAX);
					ShortPtr[1] = (ILushort)(ClearGreen * USHRT_MAX);
					ShortPtr[0] = (ILushort)(ClearBlue * USHRT_MAX);
					ShortPtr[3] = (ILushort)(ClearAlpha * USHRT_MAX);
					break;
					
				case IL_LUMINANCE:
					ShortPtr[0] = (ILushort)(ClearAlpha * USHRT_MAX);
					break;
					
				case IL_LUMINANCE_ALPHA:
					ShortPtr[0] = (ILushort)(ClearLum * USHRT_MAX);
					ShortPtr[1] = (ILushort)(ClearAlpha * USHRT_MAX);
					break;
					
				case IL_COLOUR_INDEX:
					ShortPtr[0] = (ILushort)(ClearAlpha * USHRT_MAX);
					break;
					
				default:
					ilSetError(IL_INTERNAL_ERROR);
					return;
			}
				break;
			
		case IL_INT:
		case IL_UNSIGNED_INT:
			IntPtr = (ILuint*)Colours;
			switch (Format)
			{
				case IL_RGB:
					IntPtr[0] = (ILuint)(ClearRed * UINT_MAX);
					IntPtr[1] = (ILuint)(ClearGreen * UINT_MAX);
					IntPtr[2] = (ILuint)(ClearBlue * UINT_MAX);
					break;
					
				case IL_RGBA:
					IntPtr[0] = (ILuint)(ClearRed * UINT_MAX);
					IntPtr[1] = (ILuint)(ClearGreen * UINT_MAX);
					IntPtr[2] = (ILuint)(ClearBlue * UINT_MAX);
					IntPtr[3] = (ILuint)(ClearAlpha * UINT_MAX);
					break;
					
				case IL_BGR:
					IntPtr[2] = (ILuint)(ClearRed * UINT_MAX);
					IntPtr[1] = (ILuint)(ClearGreen * UINT_MAX);
					IntPtr[0] = (ILuint)(ClearBlue * UINT_MAX);
					IntPtr[3] = (ILuint)(ClearAlpha * UINT_MAX);
					break;
					
				case IL_BGRA:
					IntPtr[2] = (ILuint)(ClearRed * UINT_MAX);
					IntPtr[1] = (ILuint)(ClearGreen * UINT_MAX);
					IntPtr[0] = (ILuint)(ClearBlue * UINT_MAX);
					IntPtr[3] = (ILuint)(ClearAlpha * UINT_MAX);
					break;
					
				case IL_LUMINANCE:
					IntPtr[0] = (ILuint)(ClearAlpha * UINT_MAX);
					break;
					
				case IL_LUMINANCE_ALPHA:
					IntPtr[0] = (ILuint)(ClearLum * UINT_MAX);
					IntPtr[1] = (ILuint)(ClearAlpha * UINT_MAX);
					break;
					
				case IL_COLOUR_INDEX:
					IntPtr[0] = (ILuint)(ClearAlpha * UINT_MAX);
					break;
					
				default:
					ilSetError(IL_INTERNAL_ERROR);
					return;
			}
				break;
			
		case IL_FLOAT:
			FloatPtr = (ILfloat*)Colours;
			switch (Format)
			{
				case IL_RGB:
					FloatPtr[0] = ClearRed;
					FloatPtr[1] = ClearGreen;
					FloatPtr[2] = ClearBlue;
					break;
					
				case IL_RGBA:
					FloatPtr[0] = ClearRed;
					FloatPtr[1] = ClearGreen;
					FloatPtr[2] = ClearBlue;
					FloatPtr[3] = ClearAlpha;
					break;
					
				case IL_BGR:
					FloatPtr[2] = ClearRed;
					FloatPtr[1] = ClearGreen;
					FloatPtr[0] = ClearBlue;
					FloatPtr[3] = ClearAlpha;
					break;
					
				case IL_BGRA:
					FloatPtr[2] = ClearRed;
					FloatPtr[1] = ClearGreen;
					FloatPtr[0] = ClearBlue;
					FloatPtr[3] = ClearAlpha;
					break;
					
				case IL_LUMINANCE:
					FloatPtr[0] = ClearAlpha;
					break;
					
				case IL_LUMINANCE_ALPHA:
					FloatPtr[0] = ClearLum;
					FloatPtr[0] = ClearAlpha;
					break;
					
				case IL_COLOUR_INDEX:
					FloatPtr[0] = ClearAlpha;
					break;
					
				default:
					ilSetError(IL_INTERNAL_ERROR);
					return;
			}
				break;
			
		case IL_DOUBLE:
			DblPtr = (ILdouble*)Colours;
			switch (Format)
			{
				case IL_RGB:
					DblPtr[0] = ClearRed;
					DblPtr[1] = ClearGreen;
					DblPtr[2] = ClearBlue;
					break;
					
				case IL_RGBA:
					DblPtr[0] = ClearRed;
					DblPtr[1] = ClearGreen;
					DblPtr[2] = ClearBlue;
					DblPtr[3] = ClearAlpha;
					break;
					
				case IL_BGR:
					DblPtr[2] = ClearRed;
					DblPtr[1] = ClearGreen;
					DblPtr[0] = ClearBlue;
					DblPtr[3] = ClearAlpha;
					break;
					
				case IL_BGRA:
					DblPtr[2] = ClearRed;
					DblPtr[1] = ClearGreen;
					DblPtr[0] = ClearBlue;
					DblPtr[3] = ClearAlpha;
					break;
					
				case IL_LUMINANCE:
					DblPtr[0] = ClearAlpha;
					break;
					
				case IL_LUMINANCE_ALPHA:
					DblPtr[0] = ClearLum;
					DblPtr[1] = ClearAlpha;
					break;
					
				case IL_COLOUR_INDEX:
					DblPtr[0] = ClearAlpha;
					break;
					
				default:
					ilSetError(IL_INTERNAL_ERROR);
					return;
			}
				break;
			
		default:
			ilSetError(IL_INTERNAL_ERROR);
			return;
	}
	
	return;
}


//! Clears the current bound image to the values specified in ilClearColour
ILboolean ILAPIENTRY ilClearImage()
{
	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}
	
	return ilClearImage_(iCurImage);
}


ILAPI ILboolean ILAPIENTRY ilClearImage_(ILimage *Image)
{
	ILuint		i, c, NumBytes;
	ILubyte 	Colours[32];  // Maximum is sizeof(double) * 4 = 32
	ILubyte 	*BytePtr;
	ILushort	*ShortPtr;
	ILuint		*IntPtr;
	ILfloat 	*FloatPtr;
	ILdouble	*DblPtr;
	
	NumBytes = Image->Bpp * Image->Bpc;
	ilGetClear(Colours, Image->Format, Image->Type);
	
	if (Image->Format != IL_COLOUR_INDEX) {
		switch (Image->Type)
		{
			case IL_BYTE:
			case IL_UNSIGNED_BYTE:
				BytePtr = (ILubyte*)Colours;
				for (c = 0; c < NumBytes; c += Image->Bpc) {
					for (i = c; i < Image->SizeOfData; i += NumBytes) {
						Image->Data[i] = BytePtr[c];
					}
				}
					break;
				
			case IL_SHORT:
			case IL_UNSIGNED_SHORT:
				ShortPtr = (ILushort*)Colours;
				for (c = 0; c < NumBytes; c += Image->Bpc) {
					for (i = c; i < Image->SizeOfData; i += NumBytes) {
						*((ILushort*)(Image->Data + i)) = ShortPtr[c / Image->Bpc];
					}
				}
					break;
				
			case IL_INT:
			case IL_UNSIGNED_INT:
				IntPtr = (ILuint*)Colours;
				for (c = 0; c < NumBytes; c += Image->Bpc) {
					for (i = c; i < Image->SizeOfData; i += NumBytes) {
						*((ILuint*)(Image->Data + i)) = IntPtr[c / Image->Bpc];
					}
				}
					break;
				
			case IL_FLOAT:
				FloatPtr = (ILfloat*)Colours;
				for (c = 0; c < NumBytes; c += Image->Bpc) {
					for (i = c; i < Image->SizeOfData; i += NumBytes) {
						*((ILfloat*)(Image->Data + i)) = FloatPtr[c / Image->Bpc];
					}
				}
					break;
				
			case IL_DOUBLE:
				DblPtr = (ILdouble*)Colours;
				for (c = 0; c < NumBytes; c += Image->Bpc) {
					for (i = c; i < Image->SizeOfData; i += NumBytes) {
						*((ILdouble*)(Image->Data + i)) = DblPtr[c / Image->Bpc];
					}
				}
					break;
		}
	}
	else {
		imemclear(Image->Data, Image->SizeOfData);
		
		if (Image->Pal.Palette)
			ifree(Image->Pal.Palette);
		Image->Pal.Palette = (ILubyte*)ialloc(4);
		if (Image->Pal.Palette == NULL) {
			return IL_FALSE;
		}
		
		Image->Pal.PalType = IL_PAL_RGBA32;
		Image->Pal.PalSize = 4;
		
		Image->Pal.Palette[0] = Colours[0] * UCHAR_MAX;
		Image->Pal.Palette[1] = Colours[1] * UCHAR_MAX;
		Image->Pal.Palette[2] = Colours[2] * UCHAR_MAX;
		Image->Pal.Palette[3] = Colours[3] * UCHAR_MAX;
	}
	
	return IL_TRUE;
}


//! Overlays the image found in Src on top of the current bound image at the coords specified.
ILboolean ILAPIENTRY ilOverlayImage(ILuint Source, ILint XCoord, ILint YCoord, ILint ZCoord)
{
	ILuint	Width, Height, Depth;
	ILuint	Dest;
	
	Dest = ilGetCurName();
	ilBindImage(Source);
	Width = iCurImage->Width;  Height = iCurImage->Height;  Depth = iCurImage->Depth;
	ilBindImage(Dest);

	return ilBlit(Source, XCoord, YCoord, ZCoord, 0, 0, 0, Width, Height, Depth);
}

//@NEXT DestX,DestY,DestZ must be set to ILuint
ILboolean ILAPIENTRY ilBlit(ILuint Source, ILint DestX,  ILint DestY,   ILint DestZ, 
                                           ILuint SrcX,  ILuint SrcY,   ILuint SrcZ,
                                           ILuint Width, ILuint Height, ILuint Depth)
{
	ILuint 		x, y, z, ConvBps, ConvSizePlane;
	ILimage 	*Dest,*Src;
	ILubyte 	*Converted;
	ILuint		DestName = ilGetCurName();
	ILuint		c;
	ILuint		StartX, StartY, StartZ;
	ILboolean	DestFlipped = IL_FALSE;
	ILubyte 	*SrcTemp;
	ILfloat		Back;

	// Check if the desiination image really exists
	if (DestName == 0 || iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}
	Dest = iCurImage;
	
	// set the destination image to upper left origin
	if (Dest->Origin == IL_ORIGIN_LOWER_LEFT) {  // Dest
		DestFlipped = IL_TRUE;
		ilFlipImage();
	}
	//DestOrigin = Dest->Origin;
	ilBindImage(Source);
	
	// Check if the source image really exists
	if (iCurImage == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}
	
	Src = iCurImage;
	
	//@TODO test if coordinates are inside the images (hard limit for source)
	
	// set the source image to upper left origin
	if (Src->Origin == IL_ORIGIN_LOWER_LEFT)
	{
		SrcTemp = iGetFlipped(iCurImage);
		if (SrcTemp == NULL)
		{
			ilBindImage(DestName);
			if (DestFlipped)
				ilFlipImage();
			return IL_FALSE;
		}
	}
	else
	{
		SrcTemp = iCurImage->Data;
	}
	
	// convert source image to match the destination image type and format
	Converted = (ILubyte*)ilConvertBuffer(Src->SizeOfData, Src->Format, Dest->Format, Src->Type, Dest->Type, NULL, SrcTemp);
	if (Converted == NULL)
		return IL_FALSE;
	
	ConvBps 	  = Dest->Bpp * Src->Width;
	ConvSizePlane = ConvBps   * Src->Height;
	
	//@NEXT in next version this would have to be removed since Dest* will be unsigned
	StartX = DestX >= 0 ? 0 : -DestX;
	StartY = DestY >= 0 ? 0 : -DestY;
	StartZ = DestZ >= 0 ? 0 : -DestZ;
	
	// Limit the copy of data inside of the destination image
	if (Width  + DestX > Dest->Width)  Width  = Dest->Width  - DestX;
	if (Height + DestY > Dest->Height) Height = Dest->Height - DestY;
	if (Depth  + DestZ > Dest->Depth)  Depth  = Dest->Depth  - DestZ;
	
	//@TODO: non funziona con rgba
	if (Src->Format == IL_RGBA || Src->Format == IL_BGRA || Src->Format == IL_LUMINANCE_ALPHA) {
		const ILuint bpp_without_alpha = Dest->Bpp - 1;
		for (z = 0; z < Depth; z++) {
			for (y = 0; y < Height; y++) {
				for (x = 0; x < Width; x++) {
					const ILuint  SrcIndex  = (z+SrcZ)*ConvSizePlane + (y+SrcY)*ConvBps + (x+SrcX)*Dest->Bpp;
					const ILuint  DestIndex = (z+DestZ)*Dest->SizeOfPlane + (y+DestY)*Dest->Bps + (x+DestX)*Dest->Bpp;
					const ILuint  AlphaIdx = SrcIndex + bpp_without_alpha;
					ILfloat Front = 0;
					
					switch (Dest->Type)
					{
						case IL_BYTE:
						case IL_UNSIGNED_BYTE:
							Front = Converted[AlphaIdx]/((float)IL_MAX_UNSIGNED_BYTE);
							break;
						case IL_SHORT:
						case IL_UNSIGNED_SHORT:
							Front = ((ILshort*)Converted)[AlphaIdx]/((float)IL_MAX_UNSIGNED_SHORT);
							break;
						case IL_INT:
						case IL_UNSIGNED_INT:
							Front = ((ILint*)Converted)[AlphaIdx]/((float)IL_MAX_UNSIGNED_INT);
							break;
						case IL_FLOAT:
							Front = ((ILfloat*)Converted)[AlphaIdx];
							break;
						case IL_DOUBLE:
							Front = (ILfloat)(((ILdouble*)Converted)[AlphaIdx]);
							break;
					}
					Back = 1.0f - Front;
					// In case of Alpha channel, the data is blended. Keeps the original alpha.
					if (ilIsEnabled(IL_BLIT_BLEND)) {
						for (c = 0; c < bpp_without_alpha; c++)
						{
							Dest->Data[DestIndex + c] = 
								(ILubyte)(Converted[SrcIndex + c] * Front
											+ Dest->Data[DestIndex + c] * Back);
						}
					}
					else {
						for (c = 0; c < Dest->Bpp; c++)
						{
							Dest->Data[DestIndex + c] = (ILubyte)(Converted[SrcIndex + c]);
						}
					}
				}
			}
		}
	} else {
		for( z = 0; z < Depth; z++ ) {
			for( y = 0; y < Height; y++ ) {
				for( x = 0; x < Width; x++ ) {
					for( c = 0; c < Dest->Bpp; c++ ) {
						Dest->Data[(z+DestZ)*Dest->SizeOfPlane + (y+DestY)*Dest->Bps + (x+DestX)*Dest->Bpp + c] =
						 Converted[(z+SrcZ)*ConvSizePlane + (y+SrcY)*ConvBps + (x+SrcX)*Dest->Bpp + c];
					}
				}
			}
		}
	}
	
	if (SrcTemp != iCurImage->Data)
		ifree(SrcTemp);
	
	ilBindImage(DestName);
	if (DestFlipped)
		ilFlipImage();
	
	ifree(Converted);
	
	return IL_TRUE;
}


ILboolean iCopySubImage(ILimage *Dest, ILimage *Src)
{
	ILimage *DestTemp, *SrcTemp;
	
	DestTemp = Dest;
	SrcTemp = Src;
	
	do {
		ilCopyImageAttr(DestTemp, SrcTemp);
		DestTemp->Data = (ILubyte*)ialloc(SrcTemp->SizeOfData);
		if (DestTemp->Data == NULL) {
			return IL_FALSE;
		}
		memcpy(DestTemp->Data, SrcTemp->Data, SrcTemp->SizeOfData);
		
		if (SrcTemp->Next) {
			DestTemp->Next = (ILimage*)icalloc(1, sizeof(ILimage));
			if (!DestTemp->Next) {
				return IL_FALSE;
			}
		}
		else {
			DestTemp->Next = NULL;
		}
		
		DestTemp = DestTemp->Next;
		SrcTemp = SrcTemp->Next;
	} while (SrcTemp);
	
	return IL_TRUE;
}


ILboolean iCopySubImages(ILimage *Dest, ILimage *Src)
{
	if (Src->Faces) {
		Dest->Faces = (ILimage*)icalloc(1, sizeof(ILimage));
		if (!Dest->Faces) {
			return IL_FALSE;
		}
		if (!iCopySubImage(Dest->Faces, Src->Faces))
			return IL_FALSE;
	}
	
	if (Src->Layers) {
		Dest->Layers = (ILimage*)icalloc(1, sizeof(ILimage));
		if (!Dest->Layers) {
			return IL_FALSE;
		}
		if (!iCopySubImage(Dest->Layers, Src->Layers))
			return IL_FALSE;
	}

	if (Src->Mipmaps) {
		Dest->Mipmaps = (ILimage*)icalloc(1, sizeof(ILimage));
		if (!Dest->Mipmaps) {
			return IL_FALSE;
		}
		if (!iCopySubImage(Dest->Mipmaps, Src->Mipmaps))
			return IL_FALSE;
	}

	if (Src->Next) {
		Dest->Next = (ILimage*)icalloc(1, sizeof(ILimage));
		if (!Dest->Next) {
			return IL_FALSE;
		}
		if (!iCopySubImage(Dest->Next, Src->Next))
			return IL_FALSE;
	}

	return IL_TRUE;
}


// Copies everything but the Data from Src to Dest.
ILAPI ILboolean ILAPIENTRY ilCopyImageAttr(ILimage *Dest, ILimage *Src)
{
	if (Dest == NULL || Src == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}
	
	if (Dest->Pal.Palette && Dest->Pal.PalSize && Dest->Pal.PalType != IL_PAL_NONE) {
		ifree(Dest->Pal.Palette);
		Dest->Pal.Palette = NULL;
	}
	if (Dest->Faces) {
		ilCloseImage(Dest->Faces);
		Dest->Faces = NULL;
	}
	if (Dest->Layers) {
		ilCloseImage(Dest->Layers);
		Dest->Layers = NULL;
	}
	if (Dest->Mipmaps) {
		ilCloseImage(Dest->Mipmaps);
		Dest->Mipmaps = NULL;
	}
	if (Dest->Next) {
		ilCloseImage(Dest->Next);
		Dest->Next = NULL;
	}
	if (Dest->Profile) {
		ifree(Dest->Profile);
		Dest->Profile = NULL;
		Dest->ProfileSize = 0;
	}
	if (Dest->DxtcData) {
		ifree(Dest->DxtcData);
		Dest->DxtcData = NULL;
		Dest->DxtcFormat = IL_DXT_NO_COMP;
		Dest->DxtcSize = 0;
	}
	
	if (Src->AnimList && Src->AnimSize) {
		Dest->AnimList = (ILuint*)ialloc(Src->AnimSize * sizeof(ILuint));
		if (Dest->AnimList == NULL) {
			return IL_FALSE;
		}
		memcpy(Dest->AnimList, Src->AnimList, Src->AnimSize * sizeof(ILuint));
	}
	if (Src->Profile) {
		Dest->Profile = (ILubyte*)ialloc(Src->ProfileSize);
		if (Dest->Profile == NULL) {
			return IL_FALSE;
		}
		memcpy(Dest->Profile, Src->Profile, Src->ProfileSize);
		Dest->ProfileSize = Src->ProfileSize;
	}
	if (Src->Pal.Palette) {
		Dest->Pal.Palette = (ILubyte*)ialloc(Src->Pal.PalSize);
		if (Dest->Pal.Palette == NULL) {
			return IL_FALSE;
		}
		memcpy(Dest->Pal.Palette, Src->Pal.Palette, Src->Pal.PalSize);
	}
	else {
		Dest->Pal.Palette = NULL;
	}
	
	Dest->Pal.PalSize = Src->Pal.PalSize;
	Dest->Pal.PalType = Src->Pal.PalType;
	Dest->Width = Src->Width;
	Dest->Height = Src->Height;
	Dest->Depth = Src->Depth;
	Dest->Bpp = Src->Bpp;
	Dest->Bpc = Src->Bpc;
	Dest->Bps = Src->Bps;
	Dest->SizeOfPlane = Src->SizeOfPlane;
	Dest->SizeOfData = Src->SizeOfData;
	Dest->Format = Src->Format;
	Dest->Type = Src->Type;
	Dest->Origin = Src->Origin;
	Dest->Duration = Src->Duration;
	Dest->CubeFlags = Src->CubeFlags;
	Dest->AnimSize = Src->AnimSize;
	Dest->OffX = Src->OffX;
	Dest->OffY = Src->OffY;
	
	return IL_TRUE/*iCopySubImages(Dest, Src)*/;
}


//! Copies everything from Src to the current bound image.
ILboolean ILAPIENTRY ilCopyImage(ILuint Src)
{
	ILuint DestName = ilGetCurName();
	ILimage *DestImage = iCurImage, *SrcImage;
	
	if (iCurImage == NULL || DestName == 0) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}
	
	ilBindImage(Src);
	SrcImage = iCurImage;
	ilBindImage(DestName);
	ilTexImage(SrcImage->Width, SrcImage->Height, SrcImage->Depth, SrcImage->Bpp, SrcImage->Format, SrcImage->Type, SrcImage->Data);
	ilCopyImageAttr(DestImage, SrcImage);
	
	return IL_TRUE;
}


// Creates a copy of Src and returns it.
ILAPI ILimage* ILAPIENTRY ilCopyImage_(ILimage *Src)
{
	ILimage *Dest;
	
	if (Src == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return NULL;
	}
	
	Dest = ilNewImage(Src->Width, Src->Height, Src->Depth, Src->Bpp, Src->Bpc);
	if (Dest == NULL) {
		return NULL;
	}
	
	if (ilCopyImageAttr(Dest, Src) == IL_FALSE)
		return NULL;
	
	memcpy(Dest->Data, Src->Data, Src->SizeOfData);
	
	return Dest;
}


ILuint ILAPIENTRY ilCloneCurImage()
{
	ILuint Id;
	ILimage *CurImage;
	
	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return 0;
	}
	
	ilGenImages(1, &Id);
	if (Id == 0)
		return 0;
	
	CurImage = iCurImage;
	
	ilBindImage(Id);
	ilTexImage(CurImage->Width, CurImage->Height, CurImage->Depth, CurImage->Bpp, CurImage->Format, CurImage->Type, CurImage->Data);
	ilCopyImageAttr(iCurImage, CurImage);
	
	iCurImage = CurImage;
	
	return Id;
}


// Like ilTexImage but doesn't destroy the palette.
ILAPI ILboolean ILAPIENTRY ilResizeImage(ILimage *Image, ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp, ILubyte Bpc)
{
	if (Image == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}
	
	if (Image->Data != NULL)
		ifree(Image->Data);
	
	Image->Depth = Depth;
	Image->Width = Width;
	Image->Height = Height;
	Image->Bpp = Bpp;
	Image->Bpc = Bpc;
	Image->Bps = Bpp * Bpc * Width;
	Image->SizeOfPlane = Image->Bps * Height;
	Image->SizeOfData = Image->SizeOfPlane * Depth;
	
	Image->Data = (ILubyte*)ialloc(Image->SizeOfData);
	if (Image->Data == NULL) {
		return IL_FALSE;
	}
	
	return IL_TRUE;
}
