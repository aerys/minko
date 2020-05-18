//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_png.cpp
//
// Description: Portable network graphics file (.png) functions
//
//-----------------------------------------------------------------------------

// Most of the comments are left in this file from libpng's excellent example.c

#include "il_internal.h"
#ifndef IL_NO_PNG
#include <png.h>
#include <stdlib.h>
#if PNG_LIBPNG_VER < 10200
	#warning DevIL was designed with libpng 1.2.0 or higher in mind.  Consider upgrading at www.libpng.org.
#endif

#if (defined(_WIN32) || defined(_WIN64)) && defined(IL_USE_PRAGMA_LIBS)
	#if defined(_MSC_VER) || defined(__BORLANDC__)
		#ifndef _DEBUG
			#pragma comment(lib, "libpng16_static.lib")
			#pragma comment(lib, "zlibstatic.lib")
		#else
			#pragma comment(lib, "libpng16_staticd.lib")
			#pragma comment(lib, "zlibstatic.lib")
		#endif
	#endif
#endif


ILboolean	iIsValidPng(void);
ILboolean	iLoadPngInternal(void);
ILboolean	iSavePngInternal(void);

ILint		readpng_init(void);
ILboolean	readpng_get_image(ILdouble display_exponent);
void		readpng_cleanup(void);

png_structp png_ptr = NULL;
png_infop   info_ptr = NULL;
ILint		png_color_type;

#define GAMMA_CORRECTION 1.0  // Doesn't seem to be doing anything...


ILboolean ilIsValidPng(ILconst_string FileName)
{
	ILHANDLE	PngFile;
	ILboolean	bPng = IL_FALSE;

	if (!iCheckExtension(FileName, IL_TEXT("png"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bPng;
	}

	PngFile = iopenr(FileName);
	if (PngFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bPng;
	}

	bPng = ilIsValidPngF(PngFile);
	icloser(PngFile);

	return bPng;
}


ILboolean ilIsValidPngF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidPng();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


ILboolean ilIsValidPngL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidPng();
}


ILboolean iIsValidPng()
{
	ILubyte 	Signature[8];
	ILint		Read;

	Read = iread(Signature, 1, 8);
	iseek(-Read, IL_SEEK_CUR);

	return !png_sig_cmp(Signature, 0, 8);  // DW 5/2/2016: They changed the behavior of this function?
}


// Reads a file
ILboolean ilLoadPng(ILconst_string FileName)
{
	ILHANDLE	PngFile;
	ILboolean	bPng = IL_FALSE;

	PngFile = iopenr(FileName);
	if (PngFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bPng;
	}

	bPng = ilLoadPngF(PngFile);
	icloser(PngFile);

	return bPng;
}


// Reads an already-opened file
ILboolean ilLoadPngF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadPngInternal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


// Reads from a memory "lump"
ILboolean ilLoadPngL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadPngInternal();
}


ILboolean iLoadPngInternal()
{
	png_ptr = NULL;
	info_ptr = NULL;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}
	if (!iIsValidPng()) {
		ilSetError(IL_INVALID_VALUE);
		return IL_FALSE;
	}

	if (readpng_init())
		return IL_FALSE;
	if (!readpng_get_image(GAMMA_CORRECTION))
		return IL_FALSE;

	readpng_cleanup();

	return ilFixImage();
}


static void png_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
	(void)png_ptr;
	iread(data, 1, (ILuint)length);
	return;
}


static void png_error_func(png_structp png_ptr, png_const_charp message)
{
	ilSetError(IL_LIB_PNG_ERROR);

	/*
	  changed 20040224
	  From the libpng docs:
	  "Errors handled through png_error() are fatal, meaning that png_error()
	   should never return to its caller. Currently, this is handled via
	   setjmp() and longjmp()"
	*/
	//return;
	longjmp(png_jmpbuf(png_ptr), 1);
}

static void png_warn_func(png_structp png_ptr, png_const_charp message)
{
	return;
}


ILint readpng_init()
{
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, png_error_func, png_warn_func);
	if (!png_ptr)
		return 4;	/* out of memory */

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return 4;	/* out of memory */
	}


	/* we could create a second info struct here (end_info), but it's only
	 * useful if we want to keep pre- and post-IDAT chunk info separated
	 * (mainly for PNG-aware image editors and converters) */


	/* setjmp() must be called in every function that calls a PNG-reading
	 * libpng function */

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return 2;
	}


	png_set_read_fn(png_ptr, NULL, png_read);
	png_set_error_fn(png_ptr, NULL, png_error_func, png_warn_func);

//	png_set_sig_bytes(png_ptr, 8);	/* we already read the 8 signature bytes */

	png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */


	/* alternatively, could make separate calls to png_get_image_width(),
	 * etc., but want bit_depth and png_color_type for later [don't care about
	 * compression_type and filter_type => NULLs] */

	/* OK, that's all we need for now; return happy */

	return 0;
}


/* display_exponent == LUT_exponent * CRT_exponent */

ILboolean readpng_get_image(ILdouble display_exponent)
{
	png_bytepp	row_pointers = NULL;
	png_uint_32 width, height; // Changed the type to fix AMD64 bit problems, thanks to Eric Werness
	ILdouble	screen_gamma = 1.0;
	ILuint		i, channels;
	ILenum		format;
	png_colorp	palette;
	ILint		num_palette, j, bit_depth;
#if _WIN32 || DJGPP
	ILdouble image_gamma;
#endif

	/* setjmp() must be called in every function that calls a PNG-reading
	 * libpng function */

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return IL_FALSE;
	}

	png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&width, (png_uint_32*)&height,
	             &bit_depth, &png_color_type, NULL, NULL, NULL);

	// Expand low-bit-depth grayscale images to 8 bits
	if (png_color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}

	// Expand RGB images with transparency to full alpha channels
	//	so the data will be available as RGBA quartets.
 	// But don't expand paletted images, since we want alpha palettes!
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) && !(png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE)))
		png_set_tRNS_to_alpha(png_ptr);

	//refresh information (added 20040224)
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&width, (png_uint_32*)&height,
	             &bit_depth, &png_color_type, NULL, NULL, NULL);

	if (bit_depth < 8) {	// Expanded earlier for grayscale, now take care of palette and rgb
		bit_depth = 8;
		png_set_packing(png_ptr);
	}

	// Perform gamma correction.
	// @TODO:  Determine if we should call png_set_gamma if image_gamma is 1.0.
#if _WIN32 || DJGPP
	screen_gamma = 2.2;
	if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
		png_set_gamma(png_ptr, screen_gamma, image_gamma);
#else
	screen_gamma = screen_gamma;
#endif

	//fix endianess
#ifdef __LITTLE_ENDIAN__
	if (bit_depth == 16)
		png_set_swap(png_ptr);
#endif


	png_read_update_info(png_ptr, info_ptr);
	channels = (ILint)png_get_channels(png_ptr, info_ptr);
	//added 20040224: update png_color_type so that it has the correct value
	//in iLoadPngInternal (globals rule...)
	png_color_type = png_get_color_type(png_ptr, info_ptr);

	//determine internal format
	switch(png_color_type)
	{
		case PNG_COLOR_TYPE_PALETTE:
			format = IL_COLOUR_INDEX;
			break;
		case PNG_COLOR_TYPE_GRAY:
			format = IL_LUMINANCE;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			format = IL_LUMINANCE_ALPHA;
			break;
		case PNG_COLOR_TYPE_RGB:
			format = IL_RGB;
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			format = IL_RGBA;
			break;
		default:
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return IL_FALSE;
	}

	if (!ilTexImage(width, height, 1, (ILubyte)channels, format, ilGetTypeBpc((ILubyte)(bit_depth >> 3)), NULL)) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return IL_FALSE;
	}
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;

	//copy palette
	if (format == IL_COLOUR_INDEX) {
		int chans;
		png_bytep trans = NULL;
		int  num_trans = -1;
		if (!png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette)) {
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return IL_FALSE;
		}

		chans = 3;
		iCurImage->Pal.PalType = IL_PAL_RGB24;

		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
			png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, NULL);
			iCurImage->Pal.PalType = IL_PAL_RGBA32;
			chans = 4;
		}

		iCurImage->Pal.PalSize = num_palette * chans;

		iCurImage->Pal.Palette = (ILubyte*)ialloc(iCurImage->Pal.PalSize);

		for (j = 0; j < num_palette; ++j) {
			iCurImage->Pal.Palette[chans*j + 0] = palette[j].red;
			iCurImage->Pal.Palette[chans*j + 1] = palette[j].green;
			iCurImage->Pal.Palette[chans*j + 2] = palette[j].blue;
			if (trans!=NULL) {
				if (j<num_trans)
					iCurImage->Pal.Palette[chans*j + 3] = trans[j];
				else
					iCurImage->Pal.Palette[chans*j + 3] = 255;
			}
		}
	}

	//allocate row pointers
	if ((row_pointers = (png_bytepp)ialloc(height * sizeof(png_bytep))) == NULL) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return IL_FALSE;
	}


	// Set the individual row_pointers to point at the correct offsets */
	for (i = 0; i < height; i++)
		row_pointers[i] = iCurImage->Data + i * iCurImage->Bps;


	// Now we can go ahead and just read the whole image
	png_read_image(png_ptr, row_pointers);


	/* and we're done!	(png_read_end() can be omitted if no processing of
	 * post-IDAT text/time/etc. is desired) */
	//png_read_end(png_ptr, NULL);
	ifree(row_pointers);

	return IL_TRUE;
}


void readpng_cleanup()
{
	if (png_ptr && info_ptr) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		png_ptr = NULL;
		info_ptr = NULL;
	}
}


//! Writes a Png file
ILboolean ilSavePng(const ILstring FileName)
{
	ILHANDLE	PngFile;
	ILuint		PngSize;

	if (ilGetBoolean(IL_FILE_MODE) == IL_FALSE) {
		if (iFileExists(FileName)) {
			ilSetError(IL_FILE_ALREADY_EXISTS);
			return IL_FALSE;
		}
	}

	PngFile = iopenw(FileName);
	if (PngFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	PngSize = ilSavePngF(PngFile);
	iclosew(PngFile);

	if (PngSize == 0)
		return IL_FALSE;
	return IL_TRUE;
}


//! Writes a Png to an already-opened file
ILuint ilSavePngF(ILHANDLE File)
{
	ILuint Pos;
	iSetOutputFile(File);
	Pos = itellw();
	if (iSavePngInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


//! Writes a Png to a memory "lump"
ILuint ilSavePngL(void *Lump, ILuint Size)
{
	ILuint Pos;
	iSetOutputLump(Lump, Size);
	Pos = itellw();
	if (iSavePngInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


void png_write(png_structp png_ptr, png_bytep data, png_size_t length)
{
	(void)png_ptr;
	iwrite(data, 1, (ILuint)length);
	return;
}

void flush_data(png_structp png_ptr)
{
	return;
}


// Internal function used to save the Png.
ILboolean iSavePngInternal()
{
	png_structp png_ptr;
	png_infop	info_ptr;
	png_text	text[4];
	ILenum		PngType;
	ILuint		BitDepth, i, j;
	ILubyte 	**RowPtr = NULL;
	ILimage 	*Temp = NULL;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	/* Create and initialize the png_struct with the desired error handler
	* functions.  If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters.  We also check that
	* the library version is compatible with the one used at compile time,
	* in case we are using dynamically linked libraries.  REQUIRED.
	*/
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, png_error_func, png_warn_func);
	if (png_ptr == NULL) {
		ilSetError(IL_LIB_PNG_ERROR);
		return IL_FALSE;
	}

	// Allocate/initialize the image information data.	REQUIRED
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		ilSetError(IL_LIB_PNG_ERROR);
		goto error_label;
	}

	/*// Set error handling.  REQUIRED if you aren't supplying your own
	//	error handling functions in the png_create_write_struct() call.
	if (setjmp(png_jmpbuf(png_ptr))) {
		// If we get here, we had a problem reading the file
		png_destroy_write_struct(&png_ptr, &info_ptr);
		ilSetError(IL_LIB_PNG_ERROR);
		return IL_FALSE;
	}*/

//	png_init_io(png_ptr, PngFile);
	png_set_write_fn(png_ptr, NULL, png_write, flush_data);

	switch (iCurImage->Type)
	{
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
			Temp = iCurImage;
			BitDepth = 8;
			break;
		case IL_SHORT:
		case IL_UNSIGNED_SHORT:
			Temp = iCurImage;
			BitDepth = 16;
			break;
		case IL_INT:
		case IL_UNSIGNED_INT:
			Temp = iConvertImage(iCurImage, iCurImage->Format, IL_UNSIGNED_SHORT);
			if (Temp == NULL) {
				png_destroy_write_struct(&png_ptr, &info_ptr);
				return IL_FALSE;
			}
			BitDepth = 16;
			break;
		default:
			ilSetError(IL_INTERNAL_ERROR);
			goto error_label;
	}

	switch (iCurImage->Format)
	{
		case IL_COLOUR_INDEX:
			PngType = PNG_COLOR_TYPE_PALETTE;
			break;
		case IL_LUMINANCE:
			PngType = PNG_COLOR_TYPE_GRAY;
			break;
		case IL_LUMINANCE_ALPHA: //added 20050328
			PngType = PNG_COLOR_TYPE_GRAY_ALPHA;
			break;
		case IL_RGB:
		case IL_BGR:
			PngType = PNG_COLOR_TYPE_RGB;
			break;
		case IL_RGBA:
		case IL_BGRA:
			PngType = PNG_COLOR_TYPE_RGB_ALPHA;
			break;
		default:
			ilSetError(IL_INTERNAL_ERROR);
			goto error_label;
	}

	// Set the image information here.	Width and height are up to 2^31,
	//	bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
	//	the png_color_type selected. png_color_type is one of PNG_COLOR_TYPE_GRAY,
	//	PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
	//	or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
	//	PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
	//	currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
	if (iGetInt(IL_PNG_INTERLACE) == IL_TRUE) {
		png_set_IHDR(png_ptr, info_ptr, iCurImage->Width, iCurImage->Height, BitDepth, PngType,
			PNG_INTERLACE_ADAM7, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	}
	else {
		png_set_IHDR(png_ptr, info_ptr, iCurImage->Width, iCurImage->Height, BitDepth, PngType,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	}

    // set the palette if there is one.  REQUIRED for indexed-color images.
	if (iCurImage->Format == IL_COLOUR_INDEX) {
	    ILpal		*TempPal = NULL;
        ILint       palType;
        ILint numCols;

        numCols = ilGetInteger(IL_PALETTE_NUM_COLS);
        if(numCols>256) {
            numCols = 256;  // png maximum
        }

		TempPal = iConvertPal(&iCurImage->Pal, IL_PAL_RGB24);
		png_set_PLTE(png_ptr, info_ptr, (png_colorp)TempPal->Palette, numCols);
        ilClosePal(TempPal);

        palType = ilGetInteger(IL_PALETTE_TYPE);
        if( palType==IL_PAL_RGBA32 || palType==IL_PAL_BGRA32 ) {
            // the palette has transparency, so we need a tRNS chunk.
            png_byte trans[256];    // png supports up to 256 palette entries
            int maxTrans = -1;
            int i;
            for(i=0; i<numCols; ++i) {
                ILubyte alpha = iCurImage->Pal.Palette[(4*i) + 3];
                trans[i] = alpha;
                if(alpha<255) {
                    // record the highest non-opaque index
                    maxTrans = i;
                }
            }
            // only write tRNS chunk if we've got some non-opaque colours
            if(maxTrans!=-1) {
                png_set_tRNS(png_ptr, info_ptr, trans, maxTrans+1, 0);
            }
        }
	}

	/*
	// optional significant bit chunk
	// if we are dealing with a grayscale image then 
	sig_bit.gray = true_bit_depth;
	// otherwise, if we are dealing with a color image then
	sig_bit.red = true_red_bit_depth;
	sig_bit.green = true_green_bit_depth;
	sig_bit.blue = true_blue_bit_depth;
	// if the image has an alpha channel then
	sig_bit.alpha = true_alpha_bit_depth;
	png_set_sBIT(png_ptr, info_ptr, sig_bit);*/


	/* Optional gamma chunk is strongly suggested if you have any guess
	* as to the correct gamma of the image.
	*/
	//png_set_gAMA(png_ptr, info_ptr, gamma);

	// Optionally write comments into the image.
	imemclear(text, sizeof(png_text) * 4);
	text[0].key = (png_charp)"Generated by";
	text[0].text = (png_charp)"Generated by the Developer's Image Library (DevIL)";
	text[0].compression = PNG_TEXT_COMPRESSION_NONE;
	text[1].key = (png_charp)"Author";
	text[1].text = (png_charp)iGetString(IL_PNG_AUTHNAME_STRING);  // Will not actually be modified!
	text[1].compression = PNG_TEXT_COMPRESSION_NONE;
	text[2].key = (png_charp)"Description";
	text[2].text = iGetString(IL_PNG_DESCRIPTION_STRING);
	text[2].compression = PNG_TEXT_COMPRESSION_NONE;
	text[3].key = (png_charp)"Title";
	text[3].text = iGetString(IL_PNG_TITLE_STRING);
	text[3].compression = PNG_TEXT_COMPRESSION_NONE;
	png_set_text(png_ptr, info_ptr, text, 3);

	// Write the file header information.  REQUIRED.
	png_write_info(png_ptr, info_ptr);

	// Free up our user-defined text.
	if (text[1].text)
		ifree(text[1].text);
	if (text[2].text)
		ifree(text[2].text);
	if (text[3].text)
		ifree(text[3].text);

	/* Shift the pixels up to a legal bit depth and fill in
	* as appropriate to correctly scale the image.
	*/
	//png_set_shift(png_ptr, &sig_bit);

	/* pack pixels into bytes */
	//png_set_packing(png_ptr);

	// swap location of alpha bytes from ARGB to RGBA
	//png_set_swap_alpha(png_ptr);

	// flip BGR pixels to RGB
	if (iCurImage->Format == IL_BGR || iCurImage->Format == IL_BGRA)
		png_set_bgr(png_ptr);

	// swap bytes of 16-bit files to most significant byte first
	#ifdef	__LITTLE_ENDIAN__
	png_set_swap(png_ptr);
	#endif//__LITTLE_ENDIAN__

	RowPtr = (ILubyte**)ialloc(iCurImage->Height * sizeof(ILubyte*));
	if (RowPtr == NULL)
		goto error_label;
	if (iCurImage->Origin == IL_ORIGIN_UPPER_LEFT) {
		for (i = 0; i < iCurImage->Height; i++) {
			RowPtr[i] = Temp->Data + i * Temp->Bps;
		}
	}
	else {
		j = iCurImage->Height - 1;
		for (i = 0; i < iCurImage->Height; i++, j--) {
			RowPtr[i] = Temp->Data + j * Temp->Bps;
		}
	}

	// Writes the image.
	png_write_image(png_ptr, RowPtr);

	// It is REQUIRED to call this to finish writing the rest of the file
	png_write_end(png_ptr, info_ptr);

	// clean up after the write, and ifree any memory allocated
	png_destroy_write_struct(&png_ptr, &info_ptr);

	ifree(RowPtr);

	if (Temp != iCurImage)
		ilCloseImage(Temp);

	return IL_TRUE;

error_label:
	png_destroy_write_struct(&png_ptr, &info_ptr);
	ifree(RowPtr);
	if (Temp != iCurImage)
		ilCloseImage(Temp);
	return IL_FALSE;
}


#endif//IL_NO_PNG
