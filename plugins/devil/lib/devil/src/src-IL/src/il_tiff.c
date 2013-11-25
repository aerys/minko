//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_tiff.c
//
// Description: Tiff (.tif) functions
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_TIF

#include "tiffio.h"

#include <time.h>
#include "il_manip.h"

#define MAGIC_HEADER1	0x4949
#define MAGIC_HEADER2	0x4D4D


#if (defined(_WIN32) || defined(_WIN64)) && defined(IL_USE_PRAGMA_LIBS)
	#if defined(_MSC_VER) || defined(__BORLANDC__)
		#ifndef _DEBUG
			#pragma comment(lib, "libtiff.lib")
		#else
			#pragma comment(lib, "libtiff-d.lib")
		#endif
	#endif
#endif


/*----------------------------------------------------------------------------*/

// No need for a separate header
static ILboolean iLoadTiffInternal(void);
static char*     iMakeString(void);
static TIFF*     iTIFFOpen(char *Mode);
static ILboolean iSaveTiffInternal(/*ILconst_string Filename*/);

/*----------------------------------------------------------------------------*/

ILboolean ilisValidTiffExtension(ILconst_string FileName)
{
	if (!iCheckExtension((ILstring)FileName, IL_TEXT("tif")) &&
		!iCheckExtension((ILstring)FileName, IL_TEXT("tiff")))
		return IL_FALSE;
	else
		return IL_TRUE;
}

/*----------------------------------------------------------------------------*/

//! Checks if the file specified in FileName is a valid tiff file.
ILboolean ilIsValidTiff(ILconst_string FileName)
{
	ILHANDLE	TiffFile;
	ILboolean	bTiff = IL_FALSE;

	if (!ilisValidTiffExtension((ILstring) FileName)) {
		ilSetError(IL_INVALID_EXTENSION);
		return bTiff;
	}

	TiffFile = iopenr((ILstring)FileName);
	if (TiffFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bTiff;
	}

	bTiff = ilIsValidTiffF(TiffFile);
	icloser(TiffFile);

	return bTiff;
}

/*----------------------------------------------------------------------------*/

ILboolean ilisValidTiffFunc()
{
	ILushort Header1, Header2;

	Header1 = GetLittleUShort();

	if (Header1 != MAGIC_HEADER1 && Header1 != MAGIC_HEADER2)
		return IL_FALSE;

	if (Header1 == MAGIC_HEADER1)
		Header2 = GetLittleUShort();
	else
		Header2 = GetBigUShort();

	if (Header2 != 42)
		return IL_FALSE;

	return IL_TRUE;
}

/*----------------------------------------------------------------------------*/

//! Checks if the ILHANDLE contains a valid tiff file at the current position.
ILboolean ilIsValidTiffF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = ilisValidTiffFunc();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}

/*----------------------------------------------------------------------------*/

//! Checks if Lump is a valid Tiff lump.
ILboolean ilIsValidTiffL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return ilisValidTiffFunc();
}

/*----------------------------------------------------------------------------*/

//! Reads a Tiff file
ILboolean ilLoadTiff(ILconst_string FileName)
{
	ILHANDLE	TiffFile;
	ILboolean	bTiff = IL_FALSE;

	TiffFile = iopenr(FileName);
	if (TiffFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
	}
	else {
		bTiff = ilLoadTiffF(TiffFile);
		icloser(TiffFile);
	}

	return bTiff;
}

/*----------------------------------------------------------------------------*/

//! Reads an already-opened Tiff file
ILboolean ilLoadTiffF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadTiffInternal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}

/*----------------------------------------------------------------------------*/

//! Reads from a memory "lump" that contains a Tiff
ILboolean ilLoadTiffL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadTiffInternal();
}

/*----------------------------------------------------------------------------*/

void warningHandler(const char* mod, const char* fmt, va_list ap)
{
	mod; fmt; ap;
	//char buff[1024];
	//vsnprintf(buff, 1024, fmt, ap);
}

void errorHandler(const char* mod, const char* fmt, va_list ap)
{
	mod; fmt; ap;
	//char buff[1024];
	//vsnprintf(buff, 1024, fmt, ap);
}

////

/*
ILboolean iLoadTiffInternal (TIFF* tif, ILimage* Image)
{
	////

	uint16   photometric, planarconfig, orientation;
	uint16   samplesperpixel, bitspersample, *sampleinfo, extrasamples;
	uint32   w, h, d, linesize, tilewidth, tilelength;
	ILushort si;

	////

	TIFFSetDirectory(tif, directory);
	////

	// Process fields

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,  &w);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

	TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEDEPTH,      &d); //TODO: d is ignored...
	TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
	TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE,   &bitspersample);
	TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES,    &extrasamples, &sampleinfo);
	TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION,     &orientation);

	linesize = TIFFScanlineSize(tif);

	TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC,  &photometric);
	TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planarconfig);

	tilewidth = w; tilelength = h;
	TIFFGetFieldDefaulted(tif, TIFFTAG_TILEWIDTH,  &tilewidth);
	TIFFGetFieldDefaulted(tif, TIFFTAG_TILELENGTH, &tilelength);

	////

	if (extrasamples != 0) {
		return IL_FALSE;
	}

	////

	if (!Image) {
		int type = IL_UNSIGNED_BYTE;
		if (bitspersample == 16) type = IL_UNSIGNED_SHORT;
		if(!ilTexImage(w, h, 1, 1, IL_LUMINANCE, type, NULL)) {
			TIFFClose(tif);
			return IL_FALSE;
		}
		iCurImage->NumNext = 0;
		Image = iCurImage;
	}
	else {
		Image->Next = ilNewImage(w, h, 1, 1, 1);
		if(Image->Next == NULL) {
			TIFFClose(tif);
			return IL_FALSE;
		}
		Image = Image->Next;
		iCurImage->NumNext++;
	}
}
*/

////


// Internal function used to load the Tiff.
ILboolean iLoadTiffInternal()
{
	TIFF	 *tif;
	uint16	 photometric, planarconfig, orientation;
	uint16	 samplesperpixel, bitspersample, *sampleinfo, extrasamples;
	uint32	 w, h, d, linesize, tilewidth, tilelength;
	ILubyte  *pImageData;
	ILuint	 i, ProfileLen, DirCount = 0;
	void	 *Buffer;
	ILimage  *Image, *TempImage;
	ILushort si;
        ILfloat  x_position, x_resolution, y_position, y_resolution;
	//TIFFRGBAImage img;
	//char emsg[1024];

	// to avoid high order bits garbage when used as shorts
	w = h = d = linesize = tilewidth = tilelength = 0;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	TIFFSetWarningHandler (NULL);
	TIFFSetErrorHandler   (NULL);

	//for debugging only
	//TIFFSetWarningHandler(warningHandler);
	//TIFFSetErrorHandler(errorHandler);

	tif = iTIFFOpen("r");
	if (tif == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	do {
		DirCount++;
	} while (TIFFReadDirectory(tif));

	/*
	 if (!ilTexImage(1, 1, 1, 1, IL_RGBA, IL_UNSIGNED_BYTE, NULL)) {
		 TIFFClose(tif);
		 return IL_FALSE;
	}
	Image = iCurImage;
	for (i = 1; i < DirCount; i++) {
		 Image->Next = ilNewImage(1, 1, 1, 1, 1);
		if (Image->Next == NULL) {
			TIFFClose(tif);
			return IL_FALSE;
		}
		Image = Image->Next;
	}
	iCurImage->NumNext = DirCount - 1;
	*/
	Image = NULL;
	for (i = 0; i < DirCount; i++) {
		TIFFSetDirectory(tif, (tdir_t)i);
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,  &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

		TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEDEPTH,		&d); //TODO: d is ignored...
		TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
		TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE,	&bitspersample);
		TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES,	&extrasamples, &sampleinfo);
		TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, 	&orientation);

		linesize = TIFFScanlineSize(tif);
		
		//added 2003-08-31
		//1 bpp tiffs are not neccessarily greyscale, they can
		//have a palette (photometric == 3)...get this information
		TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC,  &photometric);
		TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planarconfig);

		//special-case code for frequent data cases that may be read more
		//efficiently than with the TIFFReadRGBAImage() interface.
		
		//added 2004-05-12
		//Get tile sizes and use TIFFReadRGBAImage() for tiled images for now
		tilewidth = w; tilelength = h;
		TIFFGetFieldDefaulted(tif, TIFFTAG_TILEWIDTH,  &tilewidth);
		TIFFGetFieldDefaulted(tif, TIFFTAG_TILELENGTH, &tilelength);


		if (extrasamples == 0
			&& samplesperpixel == 1  //luminance or palette
			&& (bitspersample == 8 || bitspersample == 1 || bitspersample == 16)
			&& (photometric == PHOTOMETRIC_MINISWHITE
				|| photometric == PHOTOMETRIC_MINISBLACK
				|| photometric == PHOTOMETRIC_PALETTE)
			&& (orientation == ORIENTATION_TOPLEFT || orientation == ORIENTATION_BOTLEFT)
			&& tilewidth == w && tilelength == h
			) {
			ILubyte* strip;
			tsize_t stripsize;
			ILuint y;
			uint32 rowsperstrip, j, linesread;

			//TODO: 1 bit/pixel images should not be stored as 8 bits...
			//(-> add new format)
			if (!Image) {
				int type = IL_UNSIGNED_BYTE;
				if (bitspersample == 16) type = IL_UNSIGNED_SHORT;
				if (!ilTexImage(w, h, 1, 1, IL_LUMINANCE, type, NULL)) {
					TIFFClose(tif);
					return IL_FALSE;
				}
				Image = iCurImage;
			}
			else {
				Image->Next = ilNewImage(w, h, 1, 1, 1);
				if (Image->Next == NULL) {
					TIFFClose(tif);
					return IL_FALSE;
				}
				Image = Image->Next;
			}

			if (photometric == PHOTOMETRIC_PALETTE) { //read palette
				uint16 *red, *green, *blue;
				//ILboolean is16bitpalette = IL_FALSE;
				ILubyte *entry;
				uint32 count = 1 << bitspersample, j;
		
				TIFFGetField(tif, TIFFTAG_COLORMAP, &red, &green, &blue);

				Image->Format = IL_COLOUR_INDEX;
				Image->Pal.PalSize = (count)*3;
				Image->Pal.PalType = IL_PAL_RGB24;
				Image->Pal.Palette = (ILubyte*)ialloc(Image->Pal.PalSize);
				entry = Image->Pal.Palette;
				for (j = 0; j < count; ++j) {
					entry[0] = (ILubyte)(red[j] >> 8);
					entry[1] = (ILubyte)(green[j] >> 8);
					entry[2] = (ILubyte)(blue[j] >> 8);

					entry += 3;
				}
			}

			TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
			stripsize = TIFFStripSize(tif);

			strip = (ILubyte*)ialloc(stripsize);

			if (bitspersample == 8 || bitspersample == 16) {
				ILubyte *dat = Image->Data;
				for (y = 0; y < h; y += rowsperstrip) {
					//the last strip may contain less data if the image
					//height is not evenly divisible by rowsperstrip
					if (y + rowsperstrip > h) {
						stripsize = linesize*(h - y);
						linesread = h - y;
					}
					else
						linesread = rowsperstrip;

					if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, 0), strip, stripsize) == -1) {
						ilSetError(IL_LIB_TIFF_ERROR);
						ifree(strip);
						TIFFClose(tif);
						return IL_FALSE;
					}

					if (photometric == PHOTOMETRIC_MINISWHITE) { //invert channel
						uint32 k, t2;
						for (j = 0; j < linesread; ++j) {
							t2 = j*linesize;
							//this works for 16bit images as well: the two bytes
							//making up a pixel can be inverted independently
							for (k = 0; k < Image->Bps; ++k)
								dat[k] = ~strip[t2 + k];
							dat += w;
						}
					}
					else
						for(j = 0; j < linesread; ++j)
							memcpy(&Image->Data[(y + j)*Image->Bps], &strip[j*linesize], Image->Bps);
				}
			}
			else if (bitspersample == 1) {
				//TODO: add a native format to devil, so we don't have to
				//unpack the values here
				ILubyte mask, curr, *dat = Image->Data;
				uint32 k, sx, t2;
				for (y = 0; y < h; y += rowsperstrip) {
					//the last strip may contain less data if the image
					//height is not evenly divisible by rowsperstrip
					if (y + rowsperstrip > h) {
						stripsize = linesize*(h - y);
						linesread = h - y;
					}
					else
						linesread = rowsperstrip;

					if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, 0), strip, stripsize) == -1) {
						ilSetError(IL_LIB_TIFF_ERROR);
						ifree(strip);
						TIFFClose(tif);
						return IL_FALSE;
					}

					for (j = 0; j < linesread; ++j) {
						k = 0;
						sx = 0;
						t2 = j*linesize;
						while (k < w) {
							curr = strip[t2 + sx];
							if (photometric == PHOTOMETRIC_MINISWHITE)
								curr = ~curr;
							for (mask = 0x80; mask != 0 && k < w; mask >>= 1){
								if((curr & mask) != 0)
									dat[k] = 255;
								else
									dat[k] = 0;
								++k;
							}
							++sx;
						}
						dat += w;
					}
				}
			}

			ifree(strip);

			if(orientation == ORIENTATION_TOPLEFT)
				Image->Origin = IL_ORIGIN_UPPER_LEFT;
			else if(orientation == ORIENTATION_BOTLEFT)
				Image->Origin = IL_ORIGIN_LOWER_LEFT;
		}
		//for 16bit rgb images:
		else if (extrasamples == 0
			&& samplesperpixel == 3
			&& (bitspersample == 8 || bitspersample == 16)
			&& photometric == PHOTOMETRIC_RGB
			&& planarconfig == 1
			&& (orientation == ORIENTATION_TOPLEFT || orientation == ORIENTATION_BOTLEFT)
			&& tilewidth == w && tilelength == h
			) {
			ILubyte *strip, *dat;
			tsize_t stripsize;
			ILuint y;
			uint32 rowsperstrip, j, linesread;

			if (!Image) {
				int type = IL_UNSIGNED_BYTE;
				if (bitspersample == 16) type = IL_UNSIGNED_SHORT;
				if(!ilTexImage(w, h, 1, 3, IL_RGB, type, NULL)) {
					TIFFClose(tif);
					return IL_FALSE;
				}
				Image = iCurImage;
			}
			else {
				Image->Next = ilNewImage(w, h, 1, 1, 1);
				if(Image->Next == NULL) {
					TIFFClose(tif);
					return IL_FALSE;
				}
				Image = Image->Next;
			}

			TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
			stripsize = TIFFStripSize(tif);

			strip = (ILubyte*)ialloc(stripsize);

			dat = Image->Data;
			for (y = 0; y < h; y += rowsperstrip) {
				//the last strip may contain less data if the image
				//height is not evenly divisible by rowsperstrip
				if (y + rowsperstrip > h) {
					stripsize = linesize*(h - y);
					linesread = h - y;
				}
				else
					linesread = rowsperstrip;

				if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, 0), strip, stripsize) == -1) {
					ilSetError(IL_LIB_TIFF_ERROR);
					ifree(strip);
					TIFFClose(tif);
					return IL_FALSE;
				}

				for(j = 0; j < linesread; ++j)
						memcpy(&Image->Data[(y + j)*Image->Bps], &strip[j*linesize], Image->Bps);
			}

			ifree(strip);
			
			if(orientation == ORIENTATION_TOPLEFT)
				Image->Origin = IL_ORIGIN_UPPER_LEFT;
			else if(orientation == ORIENTATION_BOTLEFT)
				Image->Origin = IL_ORIGIN_LOWER_LEFT;
		}/*
		else if (extrasamples == 0 && samplesperpixel == 3  //rgb
					&& (bitspersample == 8 || bitspersample == 1 || bitspersample == 16)
					&& photometric == PHOTOMETRIC_RGB
					&& (planarconfig == PLANARCONFIG_CONTIG || planarcon
						&& (orientation == ORIENTATION_TOPLEFT || orientation == ORIENTATION_BOTLEFT)
					) {
		}
				*/
		else {
				//not direclty supported format
			if(!Image) {
				if(!ilTexImage(w, h, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL)) {
					TIFFClose(tif);
					return IL_FALSE;
				}
				Image = iCurImage;
			}
			else {
				Image->Next = ilNewImage(w, h, 1, 4, 1);
				if(Image->Next == NULL) {
					TIFFClose(tif);
					return IL_FALSE;
				}
				Image = Image->Next;
			}

			if (samplesperpixel == 4) {
				TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES, &extrasamples, &sampleinfo);
				if (!sampleinfo || sampleinfo[0] == EXTRASAMPLE_UNSPECIFIED) {
					si = EXTRASAMPLE_ASSOCALPHA;
					TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, &si);
				}
			}
			/*
			 if (!ilResizeImage(Image, Image->Width, Image->Height, 1, 4, 1)) {
				 TIFFClose(tif);
				 return IL_FALSE;
			 }
			 */
			Image->Format = IL_RGBA;
			Image->Type = IL_UNSIGNED_BYTE;
	
			// Siigron: added u_long cast to shut up compiler warning
			//2003-08-31: changed flag from 1 (exit on error) to 0 (keep decoding)
			//this lets me view text.tif, but can give crashes with unsupported
			//tiffs...
			//2003-09-04: keep flag 1 for official version for now
			if (!TIFFReadRGBAImage(tif, Image->Width, Image->Height, (uint32*)Image->Data, 0)) {
				TIFFClose(tif);
				ilSetError(IL_LIB_TIFF_ERROR);
				return IL_FALSE;
			}
			Image->Origin = IL_ORIGIN_LOWER_LEFT;  // eiu...dunno if this is right

#ifdef __BIG_ENDIAN__ //TIFFReadRGBAImage reads abgr on big endian, convert to rgba
			EndianSwapData(Image);
#endif

			/*
			 The following switch() should not be needed,
			 because we take care of the special cases that needed
			 these conversions. But since not all special cases
			 are handled right now, keep it :)
			 */
			//TODO: put switch into the loop??
			TempImage = iCurImage;
			iCurImage = Image; //ilConvertImage() converts iCurImage
			switch (samplesperpixel)
			{
				case 1:
					//added 2003-08-31 to keep palettized tiffs colored
					if(photometric != 3)
						ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
					else //strip alpha as tiff supports no alpha palettes
						ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
					break;
					
				case 3:
					//TODO: why the ifdef??
#ifdef __LITTLE_ENDIAN__
					ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
#endif			
					break; 
					
				case 4:
					pImageData = Image->Data;
					//removed on 2003-08-26...why was this here? libtiff should and does
					//take care of these things???
					/*			
					//invert alpha
#ifdef __LITTLE_ENDIAN__
					pImageData += 3;
#endif			
					for (i = Image->Width * Image->Height; i > 0; i--) {
						*pImageData ^= 255;
						pImageData += 4;
					}
					*/
					break;
			}
			iCurImage = TempImage;
			
		} //else not directly supported format

		if (TIFFGetField(tif, TIFFTAG_ICCPROFILE, &ProfileLen, &Buffer)) {
			if (Image->Profile && Image->ProfileSize)
				ifree(Image->Profile);
			Image->Profile = (ILubyte*)ialloc(ProfileLen);
			if (Image->Profile == NULL) {
				TIFFClose(tif);
				return IL_FALSE;
			}

			memcpy(Image->Profile, Buffer, ProfileLen);
			Image->ProfileSize = ProfileLen;

			//removed on 2003-08-24 as explained in bug 579574 on sourceforge
			//_TIFFfree(Buffer);
		}

                // dangelo: read offset from tiff tags.
                //If nothing is stored in these tags, then this must be an "uncropped" TIFF 
                //file, in which case, the "full size" width/height is the same as the 
                //"cropped" width and height
                //
                // the full size is currently not supported by DevIL
                //if (TIFFGetField(tif, TIFFTAG_PIXAR_IMAGEFULLWIDTH, &(c->full_width)) ==
                //        0)
                //    (c->full_width = c->cropped_width);
                //if (TIFFGetField(tif, TIFFTAG_PIXAR_IMAGEFULLLENGTH, &(c->full_height)) ==
                //        0)
                //    (c->full_height = c->cropped_height);

                if (TIFFGetField(tif, TIFFTAG_XPOSITION, &x_position) == 0)
                    x_position = 0;
                if (TIFFGetField(tif, TIFFTAG_XRESOLUTION, &x_resolution) == 0)
                    x_resolution = 0;
                if (TIFFGetField(tif, TIFFTAG_YPOSITION, &y_position) == 0)
                    y_position = 0;
                if (TIFFGetField(tif, TIFFTAG_YRESOLUTION, &y_resolution) == 0)
                    y_resolution = 0;

                //offset in pixels of "cropped" image from top left corner of 
                //full image (rounded to nearest integer)
                Image->OffX = (uint32) ((x_position * x_resolution) + 0.49);
                Image->OffY = (uint32) ((y_position * y_resolution) + 0.49);
                

		/*
		 Image = Image->Next;
		 if (Image == NULL)  // Should never happen except when we reach the end, but check anyway.
		 break;
		 */ 
	} //for tiff directories

	TIFFClose(tif);

	return ilFixImage();
}

/*----------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
// Extension to load tiff files from memory
// Marco Fabbricatore (fabbrica@ai-lab.fh-furtwangen.de)
/////////////////////////////////////////////////////////////////////////////////////////

static tsize_t 
_tiffFileReadProc(thandle_t fd, tdata_t pData, tsize_t tSize)
{
	fd;
	return iread(pData, 1, tSize);
}

/*----------------------------------------------------------------------------*/

// We have trouble sometimes reading when writing a file.  Specifically, the only time
//  I have seen libtiff call this is at the beginning of writing a tiff, before
//  anything is ever even written!  Also, we have to return 0, no matter what tSize
//  is.  If we return tSize like would be expected, then TIFFClientOpen fails.
static tsize_t 
_tiffFileReadProcW(thandle_t fd, tdata_t pData, tsize_t tSize)
{
	fd;
	return 0;
}

/*----------------------------------------------------------------------------*/

static tsize_t 
_tiffFileWriteProc(thandle_t fd, tdata_t pData, tsize_t tSize)
{
	fd;
	return iwrite(pData, 1, tSize);
}

/*----------------------------------------------------------------------------*/

static toff_t
_tiffFileSeekProc(thandle_t fd, toff_t tOff, int whence)
{
	fd;
	/* we use this as a special code, so avoid accepting it */
	if (tOff == 0xFFFFFFFF)
		return 0xFFFFFFFF;

	iseek(tOff, whence);
	return itell();
	//return tOff;
}

/*----------------------------------------------------------------------------*/

static toff_t
_tiffFileSeekProcW(thandle_t fd, toff_t tOff, int whence)
{
	/* we use this as a special code, so avoid accepting it */
	if (tOff == 0xFFFFFFFF)
		return 0xFFFFFFFF;

	iseekw(tOff, whence);
	return itellw();
	//return tOff;
}

/*----------------------------------------------------------------------------*/

static int
_tiffFileCloseProc(thandle_t fd)
{
	fd;
	return (0);
}

/*----------------------------------------------------------------------------*/

static toff_t
_tiffFileSizeProc(thandle_t fd)
{
	ILint Offset, Size;
	Offset = itell();
	iseek(0, IL_SEEK_END);
	Size = itell();
	iseek(Offset, IL_SEEK_SET);

	fd;

	return Size;
}

/*----------------------------------------------------------------------------*/

static toff_t
_tiffFileSizeProcW(thandle_t fd)
{
	ILint Offset, Size;
	Offset = itellw();
	iseekw(0, IL_SEEK_END);
	Size = itellw();
	iseekw(Offset, IL_SEEK_SET);

	return Size;
}

/*----------------------------------------------------------------------------*/

#ifdef __BORLANDC__
#pragma argsused
#endif
static int
_tiffDummyMapProc(thandle_t fd, tdata_t* pbase, toff_t* psize)
{
	fd; pbase; psize;
	return 0;
}

/*----------------------------------------------------------------------------*/

#ifdef __BORLANDC__
#pragma argsused
#endif
static void
_tiffDummyUnmapProc(thandle_t fd, tdata_t base, toff_t size)
{
	fd; base; size;
	return;
}

/*----------------------------------------------------------------------------*/

TIFF *iTIFFOpen(char *Mode)
{
	TIFF *tif;

	if (Mode[0] == 'w')
		tif = TIFFClientOpen("TIFFMemFile", Mode,
							NULL,
							_tiffFileReadProcW, _tiffFileWriteProc,
							_tiffFileSeekProcW, _tiffFileCloseProc,
							_tiffFileSizeProcW, _tiffDummyMapProc,
							_tiffDummyUnmapProc);
	else
		tif = TIFFClientOpen("TIFFMemFile", Mode,
							NULL,
							_tiffFileReadProc, _tiffFileWriteProc,
							_tiffFileSeekProc, _tiffFileCloseProc,
							_tiffFileSizeProc, _tiffDummyMapProc,
							_tiffDummyUnmapProc);
	
	return tif;
}

/*----------------------------------------------------------------------------*/


//! Writes a Tiff file
ILboolean ilSaveTiff(const ILstring FileName)
{
	ILHANDLE	TiffFile;
	ILuint		TiffSize;

	if (ilGetBoolean(IL_FILE_MODE) == IL_FALSE) {
		if (iFileExists(FileName)) {
			ilSetError(IL_FILE_ALREADY_EXISTS);
			return IL_FALSE;
		}
	}

	TiffFile = iopenw(FileName);
	if (TiffFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	TiffSize = ilSaveTiffF(TiffFile);
	iclosew(TiffFile);

	if (TiffSize == 0)
		return IL_FALSE;
	return IL_TRUE;
}


//! Writes a Tiff to an already-opened file
ILuint ilSaveTiffF(ILHANDLE File)
{
	ILuint Pos;
	iSetOutputFile(File);
	Pos = itellw();
	if (iSaveTiffInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


//! Writes a Tiff to a memory "lump"
ILuint ilSaveTiffL(void *Lump, ILuint Size)
{
	ILuint Pos = itellw();
	iSetOutputLump(Lump, Size);
	if (iSaveTiffInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


// @TODO:  Accept palettes!

// Internal function used to save the Tiff.
ILboolean iSaveTiffInternal(/*ILconst_string Filename*/)
{
	ILenum	Format;
	ILenum	Compression;
	ILuint	ixLine;
	TIFF	*File;
	char	Description[512];
	ILimage *TempImage;
	const char	*str;
	ILboolean SwapColors;
	ILubyte *OldData;

	if(iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

#if 1
	TIFFSetWarningHandler (NULL);
	TIFFSetErrorHandler   (NULL);
#else
	//for debugging only
	TIFFSetWarningHandler(warningHandler);
	TIFFSetErrorHandler(errorHandler);
#endif
	if (iGetHint(IL_COMPRESSION_HINT) == IL_USE_COMPRESSION)
		Compression = COMPRESSION_LZW;
	else
		Compression = COMPRESSION_NONE;

	if (iCurImage->Format == IL_COLOUR_INDEX) {
		if (ilGetBppPal(iCurImage->Pal.PalType) == 4)  // Preserve the alpha.
			TempImage = iConvertImage(iCurImage, IL_RGBA, IL_UNSIGNED_BYTE);
		else
			TempImage = iConvertImage(iCurImage, IL_RGB, IL_UNSIGNED_BYTE);
		
		if (TempImage == NULL) {
			return IL_FALSE;
		}
	}
	else {
		TempImage = iCurImage;
	}

	/*#ifndef _UNICODE
		File = TIFFOpen(Filename, "w");
	#else
		File = TIFFOpenW(Filename, "w");
	#endif*/

	// Control writing functions ourself.
	File = iTIFFOpen("w");
	if (File == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	sprintf(Description, "Tiff generated by %s", ilGetString(IL_VERSION_NUM));

	TIFFSetField(File, TIFFTAG_IMAGEWIDTH, TempImage->Width);
	TIFFSetField(File, TIFFTAG_IMAGELENGTH, TempImage->Height);
	TIFFSetField(File, TIFFTAG_COMPRESSION, Compression);
	TIFFSetField(File, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(File, TIFFTAG_BITSPERSAMPLE, TempImage->Bpc << 3);
	TIFFSetField(File, TIFFTAG_SAMPLESPERPIXEL, TempImage->Bpp);
	if (TempImage->Bpp == 4) //TODO: LUMINANCE, LUMINANCE_ALPHA
		TIFFSetField(File, TIFFTAG_MATTEING, 1);
	TIFFSetField(File, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(File, TIFFTAG_ROWSPERSTRIP, 1);
	TIFFSetField(File, TIFFTAG_SOFTWARE, ilGetString(IL_VERSION_NUM));
	/*TIFFSetField(File, TIFFTAG_DOCUMENTNAME,
		iGetString(IL_TIF_DOCUMENTNAME_STRING) ?
		iGetString(IL_TIF_DOCUMENTNAME_STRING) : FileName);
*/
	str = iGetString(IL_TIF_DOCUMENTNAME_STRING);
	if (str) {
		TIFFSetField(File, TIFFTAG_DOCUMENTNAME, str);
		ifree(str);
	}


	str = iGetString(IL_TIF_AUTHNAME_STRING);
	if (iGetString(IL_TIF_AUTHNAME_STRING)) {
		TIFFSetField(File, TIFFTAG_ARTIST, str);
		ifree(str);
	}

	str = iGetString(IL_TIF_HOSTCOMPUTER_STRING);
	if (str) {
		TIFFSetField(File, TIFFTAG_HOSTCOMPUTER, str);
		ifree(str);
	}

	str = iGetString(IL_TIF_HOSTCOMPUTER_STRING);
	if (str) {
		TIFFSetField(File, TIFFTAG_IMAGEDESCRIPTION, str);
		ifree(str);
	}

	// Set the date and time string.
	TIFFSetField(File, TIFFTAG_DATETIME, iMakeString());

	// 24/4/2003
	// Orientation flag is not always supported (Photoshop, ...), orient the image data 
	// and set it always to normal view
	TIFFSetField(File, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	if (TempImage->Origin != IL_ORIGIN_UPPER_LEFT) {
		ILubyte* Data = iGetFlipped(TempImage);
		OldData = TempImage->Data;
		TempImage->Data = Data;
	}
	else
		OldData = TempImage->Data;

	/*
	 TIFFSetField(File, TIFFTAG_ORIENTATION,
				  TempImage->Origin == IL_ORIGIN_UPPER_LEFT ? ORIENTATION_TOPLEFT : ORIENTATION_BOTLEFT);
	 */

	Format = TempImage->Format;
	SwapColors = (Format == IL_BGR || Format == IL_BGRA);
	if (SwapColors)
 		ilSwapColours();

	for (ixLine = 0; ixLine < TempImage->Height; ++ixLine) {
		if (TIFFWriteScanline(File, TempImage->Data + ixLine * TempImage->Bps, ixLine, 0) < 0) {
			TIFFClose(File);
			ilSetError(IL_LIB_TIFF_ERROR);
			if (SwapColors)
				ilSwapColours();
			if (TempImage->Data != OldData) {
				ifree( TempImage->Data );
				TempImage->Data = OldData;
			}
			if (TempImage != iCurImage)
				ilCloseImage(TempImage);
			return IL_FALSE;
		}
	}

	if (SwapColors)
 		ilSwapColours();

	if (TempImage->Data != OldData) {
		ifree(TempImage->Data);
		TempImage->Data = OldData;
	}

	if (TempImage != iCurImage)
		ilCloseImage(TempImage);

	TIFFClose(File);

	return IL_TRUE;
}

/*----------------------------------------------------------------------------*/
// Makes a neat date string for the date field.
// From http://www.awaresystems.be/imaging/tiff/tifftags/datetime.html :
// The format is: "YYYY:MM:DD HH:MM:SS", with hours like those on
// a 24-hour clock, and one space character between the date and the
// time. The length of the string, including the terminating NUL, is
// 20 bytes.)
char *iMakeString()
{
	static char TimeStr[20];
	time_t		Time;
	struct tm	*CurTime;

	imemclear(TimeStr, 20);

	time(&Time);
#ifdef _WIN32
	_tzset();
#endif
	CurTime = localtime(&Time);

	strftime(TimeStr, 20, "%Y:%m:%d %H:%M:%S", CurTime);
	
	return TimeStr;
}

/*----------------------------------------------------------------------------*/

#endif//IL_NO_TIF
