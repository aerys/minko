//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 05/25/2002
//
// Filename: src-ILU/src/ilu_rotate.cpp
//
// Description: Rotates an image.
//
//-----------------------------------------------------------------------------


#include "ilu_internal.h"
#include "ilu_states.h"


ILboolean ILAPIENTRY iluRotate(ILfloat Angle)
{
	ILimage	*Temp, *Temp1, *CurImage = NULL;
	ILenum	PalType = 0;

	iluCurImage = ilGetCurImage();
	if (iluCurImage == NULL) {
		ilSetError(ILU_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	if (iluCurImage->Format == IL_COLOUR_INDEX) {
		PalType = iluCurImage->Pal.PalType;
		CurImage = iluCurImage;
		iluCurImage = iConvertImage(iluCurImage, ilGetPalBaseType(CurImage->Pal.PalType), IL_UNSIGNED_BYTE);
	}

	Temp = iluRotate_(iluCurImage, Angle);
	if (Temp != NULL) {
		if (PalType != 0) {
			ilCloseImage(iluCurImage);
			Temp1 = iConvertImage(Temp, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE);
			ilCloseImage(Temp);
			Temp = Temp1;
			ilSetCurImage(CurImage);
		}
		ilTexImage(Temp->Width, Temp->Height, Temp->Depth, Temp->Bpp, Temp->Format, Temp->Type, Temp->Data);
		if (PalType != 0) {
			iluCurImage = ilGetCurImage();
			iluCurImage->Pal.PalSize = Temp->Pal.PalSize;
			iluCurImage->Pal.PalType = Temp->Pal.PalType;
			iluCurImage->Pal.Palette = (ILubyte*)ialloc(Temp->Pal.PalSize);
			if (iluCurImage->Pal.Palette == NULL) {
				ilCloseImage(Temp);
				return IL_FALSE;
			}
			memcpy(iluCurImage->Pal.Palette, Temp->Pal.Palette, Temp->Pal.PalSize);
		}

		iluCurImage->Origin = Temp->Origin;
		ilCloseImage(Temp);
		return IL_TRUE;
	}
	return IL_FALSE;
}


ILboolean ILAPIENTRY iluRotate3D(ILfloat x, ILfloat y, ILfloat z, ILfloat Angle)
{
	ILimage *Temp;

// return IL_FALSE;

	iluCurImage = ilGetCurImage();
	Temp = iluRotate3D_(iluCurImage, x, y, z, Angle);
	if (Temp != NULL) {
		ilTexImage(Temp->Width, Temp->Height, Temp->Depth, Temp->Bpp, Temp->Format, Temp->Type, Temp->Data);
		iluCurImage->Origin = Temp->Origin;
		ilSetPal(&Temp->Pal);
		ilCloseImage(Temp);
		return IL_TRUE;
	}
	return IL_FALSE;
}


//! Rotates a bitmap any angle.
//  Code help comes from http://www.leunen.com/cbuilder/rotbmp.html.
ILAPI ILimage* ILAPIENTRY iluRotate_(ILimage *Image, ILfloat Angle)
{
	ILimage		*Rotated = NULL;
	ILint		x, y, c;
	ILdouble	Cos, Sin;
	ILuint		RotOffset, ImgOffset;
	ILint		MinX, MinY, MaxX, MaxY;
	ILushort	*ShortPtr;
	ILuint		*IntPtr;
	ILdouble	*DblPtr;
	ILdouble	Point1x, Point1y, Point2x, Point2y, Point3x, Point3y;
	ILint		SrcX, SrcY;

	// Multiples of 90 are special.
	Angle = (ILfloat)fmod((ILdouble)Angle, 360.0);
	if (Angle < 0)
		Angle = 360.0f + Angle;

	Cos = (ILdouble)cos((IL_PI * Angle) / 180.0);
	Sin = (ILdouble)sin((IL_PI * Angle) / 180.0);

	Point1x = (-(ILint)Image->Height * Sin);
	Point1y = (Image->Height * Cos);
	Point2x = (Image->Width * Cos - Image->Height * Sin);
	Point2y = (Image->Height * Cos + Image->Width * Sin);
	Point3x = (Image->Width * Cos);
	Point3y = (Image->Width * Sin);

	MinX = (ILint)IL_MIN(0, IL_MIN(Point1x, IL_MIN(Point2x, Point3x)));
	MinY = (ILint)IL_MIN(0, IL_MIN(Point1y, IL_MIN(Point2y, Point3y)));
	MaxX = (ILint)IL_MAX(Point1x, IL_MAX(Point2x, Point3x));
	MaxY = (ILint)IL_MAX(Point1y, IL_MAX(Point2y, Point3y));

	Rotated = (ILimage*)icalloc(1, sizeof(ILimage));
	if (Rotated == NULL)
		return NULL;
	if (ilCopyImageAttr(Rotated, Image) == IL_FALSE) {
		ilCloseImage(Rotated);
		return NULL;
	}

	if (ilResizeImage(Rotated, (ILuint)ceil(fabs(MaxX) - MinX), (ILuint)ceil(fabs(MaxY) - MinY), 1, Image->Bpp, Image->Bpc) == IL_FALSE) {
		ilCloseImage(Rotated);
		return IL_FALSE;
	}

	ilClearImage_(Rotated);

	ShortPtr = (ILushort*)iluCurImage->Data;
	IntPtr = (ILuint*)iluCurImage->Data;
	DblPtr = (ILdouble*)iluCurImage->Data;

	//if (iluFilter == ILU_NEAREST) {
	switch (iluCurImage->Bpc)
	{
		case 1:  // Byte-based (most images)
			if (Angle == 90.0) {
				for (x = 0; x < (ILint)Image->Width; x++) {
					for (y = 0; y < (ILint)Image->Height; y++) {
						RotOffset = x * Rotated->Bps + (Image->Width - 1 - y) * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							Rotated->Data[RotOffset + c] = Image->Data[ImgOffset + c];
						}
					} 
				} 
			}
			else if (Angle == 180.0) {
				for (x = 0; x < (ILint)Image->Width; x++) {
					for (y = 0; y < (ILint)Image->Height; y++) {
						RotOffset = (Image->Height - 1 - y) * Rotated->Bps + x * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							Rotated->Data[RotOffset + c] = Image->Data[ImgOffset + c];
						}
					} 
				} 
			}
			else if (Angle == 270.0) {
				for (x = 0; x < (ILint)Image->Width; x++) {
					for (y = 0; y < (ILint)Image->Height; y++) {
						RotOffset = (Image->Height - 1 - x) * Rotated->Bps + y * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							Rotated->Data[RotOffset + c] = Image->Data[ImgOffset + c];
						}
					} 
				} 
			}
			else {
				for (x = 0; x < (ILint)Rotated->Width; x++) {
					for (y = 0; y < (ILint)Rotated->Height; y++) {
						SrcX = (ILint)((x + MinX) * Cos + (y + MinY) * Sin);
						SrcY = (ILint)((y + MinY) * Cos - (x + MinX) * Sin);
						if (SrcX >= 0 && SrcX < (ILint)Image->Width && SrcY >= 0 && SrcY < (ILint)Image->Height) {
							RotOffset = y * Rotated->Bps + x * Rotated->Bpp;
							ImgOffset = (ILuint)SrcY * Image->Bps + (ILuint)SrcX * Image->Bpp;
							for (c = 0; c < Rotated->Bpp; c++) {
								Rotated->Data[RotOffset + c] = Image->Data[ImgOffset + c];
							}
						}
					}
				}
			}
			break;

		case 2:  // Short-based
			Image->Bps /= 2;   // Makes it easier to just
			Rotated->Bps /= 2; //   cast to short.

			if (Angle == 90.0) {
				for (x = 0; x < (ILint)Image->Width; x++) {
					for (y = 0; y < (ILint)Image->Height; y++) {
						RotOffset = x * Rotated->Bps + (Image->Width - 1 - y) * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							((ILushort*)(Rotated->Data))[RotOffset + c] = ShortPtr[ImgOffset + c];
						}
					} 
				} 
			}
			else if (Angle == 180.0) {
				for (x = 0; x < (ILint)Image->Width; x++) {
					for (y = 0; y < (ILint)Image->Height; y++) {
						RotOffset = (Image->Height - 1 - y) * Rotated->Bps + x * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							((ILushort*)(Rotated->Data))[RotOffset + c] = ShortPtr[ImgOffset + c];
						}
					} 
				} 
			}
			else if (Angle == 270.0) {
				for (x = 0; x < (ILint)Image->Width; x++) {
					for (y = 0; y < (ILint)Image->Height; y++) {
						RotOffset = (Image->Height - 1 - x) * Rotated->Bps + y * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							((ILushort*)(Rotated->Data))[RotOffset + c] = ShortPtr[ImgOffset + c];
						}
					} 
				} 
			}
			else {
				for (x = 0; x < (ILint)Rotated->Width; x++) {
					for (y = 0; y < (ILint)Rotated->Height; y++) {
						SrcX = (ILint)((x + MinX) * Cos + (y + MinY) * Sin);
						SrcY = (ILint)((y + MinY) * Cos - (x + MinX) * Sin);
						if (SrcX >= 0 && SrcX < (ILint)Image->Width && SrcY >= 0 && SrcY < (ILint)Image->Height) {
							RotOffset = y * Rotated->Bps + x * Rotated->Bpp;
							ImgOffset = (ILuint)SrcY * Image->Bps + (ILuint)SrcX * Image->Bpp;
							for (c = 0; c < Rotated->Bpp; c++) {
								((ILushort*)(Rotated->Data))[RotOffset + c] = ShortPtr[ImgOffset + c];
							}
						}
					}
				}
			}
			Image->Bps *= 2;
			Rotated->Bps *= 2;
			break;

		case 4:  // Floats or 32-bit integers
			Image->Bps /= 4;
			Rotated->Bps /= 4;

			if (Angle == 90.0) {
				for (x = 0; x < (ILint)Image->Width; x++) {
					for (y = 0; y < (ILint)Image->Height; y++) {
						RotOffset = x * Rotated->Bps + (Image->Width - 1 - y) * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							((ILuint*)(Rotated->Data))[RotOffset + c] = IntPtr[ImgOffset + c];
						}
					} 
				} 
			}
			else if (Angle == 180.0) {
				for (x = 0; x < (ILint)Image->Width; x++) {
					for (y = 0; y < (ILint)Image->Height; y++) {
						RotOffset = (Image->Height - 1 - y) * Rotated->Bps + x * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							((ILuint*)(Rotated->Data))[RotOffset + c] = IntPtr[ImgOffset + c];
						}
					} 
				} 
			}
			else if (Angle == 270.0) {
				for (x = 0; x < (ILint)Image->Width; x++) {
					for (y = 0; y < (ILint)Image->Height; y++) {
						RotOffset = (Image->Height - 1 - x) * Rotated->Bps + y * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							((ILuint*)(Rotated->Data))[RotOffset + c] = IntPtr[ImgOffset + c];
						}
					} 
				} 
			}
			else {
				for (x = 0; x < (ILint)Rotated->Width; x++) {
					for (y = 0; y < (ILint)Rotated->Height; y++) {
						SrcX = (ILint)((x + MinX) * Cos + (y + MinY) * Sin);
						SrcY = (ILint)((y + MinY) * Cos - (x + MinX) * Sin);
						if (SrcX >= 0 && SrcX < (ILint)Image->Width && SrcY >= 0 && SrcY < (ILint)Image->Height) {
							RotOffset = y * Rotated->Bps + x * Rotated->Bpp;
							ImgOffset = (ILuint)SrcY * Image->Bps + (ILuint)SrcX * Image->Bpp;
							for (c = 0; c < Rotated->Bpp; c++) {
								((ILuint*)(Rotated->Data))[RotOffset + c] = IntPtr[ImgOffset + c];
							}
						}
					}
				}
			}
			Image->Bps *= 4;
			Rotated->Bps *= 4;
			break;

		case 8:  // Double or 64-bit integers
			Image->Bps /= 8;
			Rotated->Bps /= 8;

			if (Angle == 90.0) {
				for (x = 0; x < (ILint)Image->Width; x++) {
					for (y = 0; y < (ILint)Image->Height; y++) {
						RotOffset = x * Rotated->Bps + (Image->Width - 1 - y) * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							((ILdouble*)(Rotated->Data))[RotOffset + c] = DblPtr[ImgOffset + c];
						}
					} 
				} 
			}
			else if (Angle == 180.0) {
				for (x = 0; x < (ILint)Image->Width; x++) { 
					for (y = 0; y < (ILint)Image->Height; y++) { 
						RotOffset = (Image->Height - 1 - y) * Rotated->Bps + x * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							((ILdouble*)(Rotated->Data))[RotOffset + c] = DblPtr[ImgOffset + c];
						}
					} 
				} 
			}
			else if (Angle == 270.0) {
				for (x = 0; x < (ILint)Image->Width; x++) {
					for (y = 0; y < (ILint)Image->Height; y++) {
						RotOffset = (Image->Height - 1 - x) * Rotated->Bps + y * Rotated->Bpp;
						ImgOffset = y * Image->Bps + x * Image->Bpp;
						for (c = 0; c < Rotated->Bpp; c++) {
							((ILdouble*)(Rotated->Data))[RotOffset + c] = DblPtr[ImgOffset + c];
						}
					} 
				} 
			}
			else {
				for (x = 0; x < (ILint)Rotated->Width; x++) {
					for (y = 0; y < (ILint)Rotated->Height; y++) {
						SrcX = (ILint)((x + MinX) * Cos + (y + MinY) * Sin);
						SrcY = (ILint)((y + MinY) * Cos - (x + MinX) * Sin);
						if (SrcX >= 0 && SrcX < (ILint)Image->Width && SrcY >= 0 && SrcY < (ILint)Image->Height) {
							RotOffset = y * Rotated->Bps + x * Rotated->Bpp;
							ImgOffset = (ILuint)SrcY * Image->Bps + (ILuint)SrcX * Image->Bpp;
							for (c = 0; c < Rotated->Bpp; c++) {
								((ILdouble*)(Rotated->Data))[RotOffset + c] = DblPtr[ImgOffset + c];
							}
						}
					}
				}
			}
			Image->Bps *= 8;
			Rotated->Bps *= 8;
			break;
	}

	return Rotated;
}


ILAPI ILimage* ILAPIENTRY iluRotate3D_(ILimage *Image, ILfloat x, ILfloat y, ILfloat z, ILfloat Angle)
{
	Image; x; y; z; Angle;
	return NULL;
}
