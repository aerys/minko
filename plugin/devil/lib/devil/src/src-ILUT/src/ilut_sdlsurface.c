//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2002 by Denton Woods
// Copyright (C) 2002 Nelson Rush.
// Last modified: 05/18/2002
//
// Filename: src-ILUT/src/ilut_sdlsurface.c
//
// Description:  SDL Surface functions for images
//
//-----------------------------------------------------------------------------


#include "ilut_internal.h"
#ifdef ILUT_USE_SDL
#include <SDL.h>
//#include "endian.h"

#ifdef  _MSC_VER
	#pragma comment(lib, "sdl.lib")
#endif//_MSC_VER

int isBigEndian;
int rmask, gmask, bmask, amask;

void InitSDL()
{
//#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#ifdef __BIG_ENDIAN__
	isBigEndian = 1;
    rmask = 0xFF000000;
    gmask = 0x00FF0000;
    bmask = 0x0000FF00;
    amask = 0x000000FF;
#else
	isBigEndian = 0;
    rmask = 0x000000FF;
    gmask = 0x0000FF00;
    bmask = 0x00FF0000;
    amask = 0xFF000000;
#endif
	return;
}

//ILboolean ilConvertPal(ILenum DestFormat);

//ILpal *Pal;

// Does not account for converting luminance...
SDL_Surface *ILAPIENTRY ilutConvertToSDLSurface(unsigned int flags)
{
	SDL_Surface *Bitmap = NULL;
	ILuint		i = 0, Pad, BppPal;
	ILubyte		*Dest, *Data;
	ILimage		*Image;

	Image = ilutCurImage = ilGetCurImage();
	if (ilutCurImage == NULL) {
		ilSetError(ILUT_ILLEGAL_OPERATION);
		return NULL;
	}

	InitSDL();

	// Should be IL_BGR(A).
	if (ilutCurImage->Format == IL_RGB || ilutCurImage->Format == IL_RGBA) {
		if (!isBigEndian) {
			//iluSwapColours();  // No need to swap colors.  Just use the bitmasks.
			rmask = 0x00FF0000;
			gmask = 0x0000FF00;
			bmask = 0x000000FF;
		}
	}
	else if (ilutCurImage->Format == IL_BGR || ilutCurImage->Format == IL_BGRA) {
		if (isBigEndian) {
			rmask = 0x0000FF00;
			gmask = 0x00FF0000;
			bmask = 0xFF000000;
		}
	}
	else if (Image->Format != IL_COLOR_INDEX) {  // We have to convert the image.
		#ifdef __BIG_ENDIAN__
		Image = iConvertImage(Image, IL_RGBA, IL_UNSIGNED_BYTE);
		#else
		Image = iConvertImage(Image, IL_BGRA, IL_UNSIGNED_BYTE);
		#endif
		if (Image == NULL)
			return NULL;
	}

	if (Image->Type != IL_UNSIGNED_BYTE) {
		// We do not have to worry about Image != iCurImage at this point, because if it was converted,
		//  it was converted to a type of unsigned byte.
		Image = iConvertImage(Image, Image->Format, IL_UNSIGNED_BYTE);
		if (Image == NULL)
			return NULL;
	}

	Data = Image->Data;
	if (Image->Origin == IL_ORIGIN_LOWER_LEFT) {
		Data = iGetFlipped(Image);
		if (Data == NULL)
			goto done;
	}

	Bitmap = SDL_CreateRGBSurface(flags, Image->Width, Image->Height, Image->Bpp * 8,
					rmask,gmask,bmask,amask);
	if (Bitmap == NULL)
		goto done;

	if (SDL_MUSTLOCK(Bitmap))
		SDL_LockSurface(Bitmap);

	Pad = Bitmap->pitch - Image->Bps;
	if (Pad == 0) {
		memcpy(Bitmap->pixels, Data, Image->SizeOfData);
	}
	else {  // Must pad the lines on some images.
		Dest = Bitmap->pixels;
		for (i = 0; i < Image->Height; i++) {
			memcpy(Dest, Data + i * Image->Bps, Image->Bps);
			imemclear(Dest + Image->Bps, Pad);
			Dest += Bitmap->pitch;
		}
	}

	if (SDL_MUSTLOCK(Bitmap))
		SDL_UnlockSurface(Bitmap);

	if (Image->Format == IL_COLOR_INDEX) {
		BppPal = ilGetBppPal(Image->Pal.PalType);
		switch (ilutCurImage->Pal.PalType)
		{
			case IL_PAL_RGB24:
			case IL_PAL_RGB32:
			case IL_PAL_RGBA32:
				for (i = 0; i < ilutCurImage->Pal.PalSize / BppPal; i++) {
					(Bitmap->format)->palette->colors[i].r = ilutCurImage->Pal.Palette[i*BppPal+0];
					(Bitmap->format)->palette->colors[i].g = ilutCurImage->Pal.Palette[i*BppPal+1];
					(Bitmap->format)->palette->colors[i].b = ilutCurImage->Pal.Palette[i*BppPal+2];
					(Bitmap->format)->palette->colors[i].unused = 0xFF;
				}
				break;
			case IL_PAL_BGR24:
			case IL_PAL_BGR32:
			case IL_PAL_BGRA32:
				for (i = 0; i < ilutCurImage->Pal.PalSize / BppPal; i++) {
					(Bitmap->format)->palette->colors[i].b = ilutCurImage->Pal.Palette[i*BppPal+0];
					(Bitmap->format)->palette->colors[i].g = ilutCurImage->Pal.Palette[i*BppPal+1];
					(Bitmap->format)->palette->colors[i].r = ilutCurImage->Pal.Palette[i*BppPal+2];
					(Bitmap->format)->palette->colors[i].unused = 0xFF;
				}
				break;
			default:
				ilSetError(IL_INTERNAL_ERROR);  // Do anything else?
		}
	}

done:
	if (Data != Image->Data)
		ifree(Data);  // This is flipped data.
	if (Image != ilutCurImage)
		ilCloseImage(Image);  // This is a converted image.
	return Bitmap;  // This is NULL if there was an error.
}


#ifndef _WIN32_WCE
SDL_Surface* ILAPIENTRY ilutSDLSurfaceLoadImage(ILstring FileName)
{
	SDL_Surface *Surface;

	iBindImageTemp();
	if (!ilLoadImage(FileName)) {
		return NULL;
	}

	Surface = ilutConvertToSDLSurface(SDL_SWSURFACE);

	return Surface;
}
#endif//_WIN32_WCE


// Unfinished
ILboolean ILAPIENTRY ilutSDLSurfaceFromBitmap(SDL_Surface *Bitmap)
{
	ilutCurImage = ilGetCurImage();
	if (ilutCurImage == NULL) {
		ilSetError(ILUT_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	if (Bitmap == NULL || Bitmap->w == 0 || Bitmap->h == 0) {
		ilSetError(ILUT_INVALID_PARAM);
		return IL_FALSE;
	}

	if (!ilTexImage(Bitmap->w, Bitmap->h, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL))
		return IL_FALSE;

	return IL_TRUE;
}


#endif//ILUT_USE_SDL
