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

void iSwapUShort(ILushort *s)  {
	#ifdef USE_WIN32_ASM
		__asm {
			mov ebx, s
			mov al, [ebx+1]
			mov ah, [ebx  ]
			mov [ebx], ax
		}
	#else
	#ifdef GCC_X86_ASM
		asm("ror $8,%0"
			: "=r" (*s)
			: "0" (*s));
	#else
		*s = ((*s)>>8) | ((*s)<<8);
	#endif //GCC_X86_ASM
	#endif //USE_WIN32_ASM
}

void iSwapShort(ILshort *s) {
	iSwapUShort((ILushort*)s);
}

void iSwapUInt(ILuint *i) {
	#ifdef USE_WIN32_ASM
		__asm {
			mov ebx, i
			mov eax, [ebx]
			bswap eax
			mov [ebx], eax
		}
	#else
	#ifdef GCC_X86_ASM
			asm("bswap %0;"
				: "+r" (*i));
	#else
		*i = ((*i)>>24) | (((*i)>>8) & 0xff00) | (((*i)<<8) & 0xff0000) | ((*i)<<24);
	#endif //GCC_X86_ASM
	#endif //USE_WIN32_ASM
}

void iSwapInt(ILint *i) {
	iSwapUInt((ILuint*)i);
}

void iSwapFloat(ILfloat *f) {
	iSwapUInt((ILuint*)f);
}

void iSwapDouble(ILdouble *d) {
	#ifdef GCC_X86_ASM
	int *t = (int*)d;
	asm("bswap %2    \n"
		"bswap %3    \n"
		"movl  %2,%1 \n"
		"movl  %3,%0 \n"
		: "=g" (t[0]), "=g" (t[1])
		: "r"  (t[0]), "r"  (t[1]));
	#else
	ILubyte t,*b = (ILubyte*)d;
	#define dswap(x,y) t=b[x];b[x]=b[y];b[y]=b[x];
	dswap(0,7);
	dswap(1,6);
	dswap(2,5);
	dswap(3,4);
	#undef dswap
	#endif
}


ILushort GetLittleUShort() {
	ILushort s;
	iread(&s, sizeof(ILushort), 1);
#ifdef __BIG_ENDIAN__
	iSwapUShort(&s);
#endif
	return s;
}

ILshort GetLittleShort() {
	ILshort s;
	iread(&s, sizeof(ILshort), 1);
#ifdef __BIG_ENDIAN__
	iSwapShort(&s);
#endif
	return s;
}

ILuint GetLittleUInt() {
	ILuint i;
	iread(&i, sizeof(ILuint), 1);
#ifdef __BIG_ENDIAN__
	iSwapUInt(&i);
#endif
	return i;
}

ILint GetLittleInt() {
	ILint i;
	iread(&i, sizeof(ILint), 1);
#ifdef __BIG_ENDIAN__
	iSwapInt(&i);
#endif
	return i;
}

ILfloat GetLittleFloat() {
	ILfloat f;
	iread(&f, sizeof(ILfloat), 1);
#ifdef __BIG_ENDIAN__
	iSwapFloat(&f);
#endif
	return f;
}

ILdouble GetLittleDouble() {
	ILdouble d;
	iread(&d, sizeof(ILdouble), 1);
#ifdef __BIG_ENDIAN__
	iSwapDouble(&d);
#endif
	return d;
}

ILushort GetBigUShort() {
	ILushort s;
	iread(&s, sizeof(ILushort), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapUShort(&s);
#endif
	return s;
}

ILshort GetBigShort() {
	ILshort s;
	iread(&s, sizeof(ILshort), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapShort(&s);
#endif
	return s;
}

ILuint GetBigUInt() {
	ILuint i;
	iread(&i, sizeof(ILuint), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapUInt(&i);
#endif
	return i;
}

ILint GetBigInt() {
	ILint i;
	iread(&i, sizeof(ILint), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapInt(&i);
#endif
	return i;
}

ILfloat GetBigFloat() {
	ILfloat f;
	iread(&f, sizeof(ILfloat), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapFloat(&f);
#endif
	return f;
}

ILdouble GetBigDouble() {
	ILdouble d;
	iread(&d, sizeof(ILdouble), 1);
#ifdef __LITTLE_ENDIAN__
	iSwapDouble(&d);
#endif
	return d;
}

ILubyte SaveLittleUShort(ILushort s) {
#ifdef __BIG_ENDIAN__
	iSwapUShort(&s);
#endif
	return iwrite(&s, sizeof(ILushort), 1);
}

ILubyte SaveLittleShort(ILshort s) {
#ifdef __BIG_ENDIAN__
	iSwapShort(&s);
#endif
	return iwrite(&s, sizeof(ILshort), 1);
}

ILubyte SaveLittleUInt(ILuint i) {
#ifdef __BIG_ENDIAN__
	iSwapUInt(&i);
#endif
	return iwrite(&i, sizeof(ILuint), 1);
}

ILubyte SaveLittleInt(ILint i) {
#ifdef __BIG_ENDIAN__
	iSwapInt(&i);
#endif
	return iwrite(&i, sizeof(ILint), 1);
}

ILubyte SaveLittleFloat(ILfloat f) {
#ifdef __BIG_ENDIAN__
	iSwapFloat(&f);
#endif
	return iwrite(&f, sizeof(ILfloat), 1);
}

ILubyte SaveLittleDouble(ILdouble d) {
#ifdef __BIG_ENDIAN__
	iSwapDouble(&d);
#endif
	return iwrite(&d, sizeof(ILdouble), 1);
}


ILubyte SaveBigUShort(ILushort s) {
#ifdef __LITTLE_ENDIAN__
	iSwapUShort(&s);
#endif
	return iwrite(&s, sizeof(ILushort), 1);
}


ILubyte SaveBigShort(ILshort s) {
#ifdef __LITTLE_ENDIAN__
	iSwapShort(&s);
#endif
	return iwrite(&s, sizeof(ILshort), 1);
}


ILubyte SaveBigUInt(ILuint i) {
#ifdef __LITTLE_ENDIAN__
	iSwapUInt(&i);
#endif
	return iwrite(&i, sizeof(ILuint), 1);
}


ILubyte SaveBigInt(ILint i) {
#ifdef __LITTLE_ENDIAN__
	iSwapInt(&i);
#endif
	return iwrite(&i, sizeof(ILint), 1);
}


ILubyte SaveBigFloat(ILfloat f) {
#ifdef __LITTLE_ENDIAN__
	iSwapFloat(&f);
#endif
	return iwrite(&f, sizeof(ILfloat), 1);
}


ILubyte SaveBigDouble(ILdouble d) {
#ifdef __LITTLE_ENDIAN__
	iSwapDouble(&d);
#endif
	return iwrite(&d, sizeof(ILdouble), 1);
}

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
