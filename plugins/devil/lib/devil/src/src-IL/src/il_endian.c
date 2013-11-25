//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Last modified: 12/06/2006
//
// Filename: src-IL/src/il_endian.c
//
// Description: Takes care of endian issues
//
//-----------------------------------------------------------------------------


#define IL_ENDIAN_C

#include "il_endian.h"

void EndianSwapData(void *_Image)
{
	ILuint		i;
	ILubyte		*temp, *s, *d;
	ILushort	*ShortS, *ShortD;
	ILuint		*IntS, *IntD;
	ILfloat		*FltS, *FltD;
	ILdouble	*DblS, *DblD;

	ILimage *Image = (ILimage*)_Image;

	switch (Image->Type) {
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
			switch (Image->Bpp) {
				case 3:
					temp = (ILubyte*)ialloc(Image->SizeOfData);
					if (temp == NULL)
						return;
					s = Image->Data;
					d = temp;

					for( i = Image->Width * Image->Height; i > 0; i-- ) {
						*d++ = *(s+2);
						*d++ = *(s+1);
						*d++ = *s;
						s += 3;
					}

					ifree(Image->Data);
					Image->Data = temp;
					break;

				case 4:
					temp = (ILubyte*)ialloc(Image->SizeOfData);
					if (temp == NULL)
						return;
					s = Image->Data;
					d = temp;

					for (i = Image->Width * Image->Height; i > 0; i--) {
						*d++ = *(s+3);
						*d++ = *(s+2);
						*d++ = *(s+1);
						*d++ = *s;
						s += 4;
					}

					ifree(Image->Data);
					Image->Data = temp;
					break;
			}
			break;

		case IL_SHORT:
		case IL_UNSIGNED_SHORT:
			switch (Image->Bpp) {
				case 3:
					temp = (ILubyte*)ialloc(Image->SizeOfData);
					if (temp == NULL)
						return;
					ShortS = (ILushort*)Image->Data;
					ShortD = (ILushort*)temp;

					for (i = Image->Width * Image->Height; i > 0; i--) {
						*ShortD = *ShortS++; iSwapUShort(ShortD++);
						*ShortD = *ShortS++; iSwapUShort(ShortD++);
						*ShortD = *ShortS++; iSwapUShort(ShortD++);
					}

					ifree(Image->Data);
					Image->Data = temp;
					break;

				case 4:
					temp = (ILubyte*)ialloc(Image->SizeOfData);
					if (temp == NULL)
						return;
					ShortS = (ILushort*)Image->Data;
					ShortD = (ILushort*)temp;

					for (i = Image->Width * Image->Height; i > 0; i--) {
						*ShortD = *ShortS++; iSwapUShort(ShortD++);
						*ShortD = *ShortS++; iSwapUShort(ShortD++);
						*ShortD = *ShortS++; iSwapUShort(ShortD++);
						*ShortD = *ShortS++; iSwapUShort(ShortD++);
					}

					ifree(Image->Data);
					Image->Data = temp;
					break;
			}
			break;

		case IL_INT:
		case IL_UNSIGNED_INT:
			switch (Image->Bpp)
			{
				case 3:
					temp = (ILubyte*)ialloc(Image->SizeOfData);
					if (temp == NULL)
						return;
					IntS = (ILuint*)Image->Data;
					IntD = (ILuint*)temp;

					for (i = Image->Width * Image->Height; i > 0; i--) {
						*IntD = *IntS++; iSwapUInt(IntD++);
						*IntD = *IntS++; iSwapUInt(IntD++);
						*IntD = *IntS++; iSwapUInt(IntD++);
					}

					ifree(Image->Data);
					Image->Data = temp;
					break;

				case 4:
					temp = (ILubyte*)ialloc(Image->SizeOfData);
					if (temp == NULL)
						return;
					IntS = (ILuint*)Image->Data;
					IntD = (ILuint*)temp;

					for (i = Image->Width * Image->Height; i > 0; i--) {
						*IntD = *IntS++; iSwapUInt(IntD++);
						*IntD = *IntS++; iSwapUInt(IntD++);
						*IntD = *IntS++; iSwapUInt(IntD++);
						*IntD = *IntS++; iSwapUInt(IntD++);
					}

					ifree(Image->Data);
					Image->Data = temp;
					break;
			}
			break;

		case IL_FLOAT:
			switch (Image->Bpp)
			{
				case 3:
					temp = (ILubyte*)ialloc(Image->SizeOfData);
					if (temp == NULL)
						return;
					FltS = (ILfloat*)Image->Data;
					FltD = (ILfloat*)temp;

					for (i = Image->Width * Image->Height; i > 0; i--) {
						*FltD = *FltS++; iSwapFloat(FltD++);
						*FltD = *FltS++; iSwapFloat(FltD++);
						*FltD = *FltS++; iSwapFloat(FltD++);
					}

					ifree(Image->Data);
					Image->Data = temp;
					break;

				case 4:
					temp = (ILubyte*)ialloc(Image->SizeOfData);
					if (temp == NULL)
						return;
					FltS = (ILfloat*)Image->Data;
					FltD = (ILfloat*)temp;

					for (i = Image->Width * Image->Height; i > 0; i--) {
						*FltD = *FltS++; iSwapFloat(FltD++);
						*FltD = *FltS++; iSwapFloat(FltD++);
						*FltD = *FltS++; iSwapFloat(FltD++);
						*FltD = *FltS++; iSwapFloat(FltD++);
					}

					ifree(Image->Data);
					Image->Data = temp;
					break;
			}
			break;

		case IL_DOUBLE:
			switch (Image->Bpp)
			{
				case 3:
					temp = (ILubyte*)ialloc(Image->SizeOfData);
					if (temp == NULL)
						return;
					DblS = (ILdouble*)Image->Data;
					DblD = (ILdouble*)temp;

					for (i = Image->Width * Image->Height; i > 0; i--) {
						*DblD = *DblS++; iSwapDouble(DblD++);
						*DblD = *DblS++; iSwapDouble(DblD++);
						*DblD = *DblS++; iSwapDouble(DblD++);
					}

					ifree(Image->Data);
					Image->Data = temp;
					break;

				case 4:
					temp = (ILubyte*)ialloc(Image->SizeOfData);
					if (temp == NULL)
						return;
					DblS = (ILdouble*)Image->Data;
					DblD = (ILdouble*)temp;

					for (i = Image->Width * Image->Height; i > 0; i--) {
						*DblD = *DblS++; iSwapDouble(DblD++);
						*DblD = *DblS++; iSwapDouble(DblD++);
						*DblD = *DblS++; iSwapDouble(DblD++);
						*DblD = *DblS++; iSwapDouble(DblD++);
					}

					ifree(Image->Data);
					Image->Data = temp;
					break;
			}
			break;
	}

	if( iCurImage->Format == IL_COLOUR_INDEX ) {
		switch (iCurImage->Pal.PalType) {
			case IL_PAL_RGB24:
			case IL_PAL_BGR24:
				temp = (ILubyte*)ialloc(Image->Pal.PalSize);
				if (temp == NULL)
					return;
				s = Image->Pal.Palette;
				d = temp;

				for (i = Image->Pal.PalSize / 3; i > 0; i--) {
					*d++ = *(s+2);
					*d++ = *(s+1);
					*d++ = *s;
					s += 3;
				}

				ifree(Image->Pal.Palette);
				Image->Pal.Palette = temp;
				break;

			case IL_PAL_RGBA32:
			case IL_PAL_RGB32:
			case IL_PAL_BGRA32:
			case IL_PAL_BGR32:
				temp = (ILubyte*)ialloc(Image->Pal.PalSize);
				if (temp == NULL)
					return;
				s = Image->Pal.Palette;
				d = temp;

				for (i = Image->Pal.PalSize / 4; i > 0; i--) {
					*d++ = *(s+3);
					*d++ = *(s+2);
					*d++ = *(s+1);
					*d++ = *s;
					s += 4;
				}

				ifree(Image->Pal.Palette);
				Image->Pal.Palette = temp;
				break;
		}
	}
	return;
}
