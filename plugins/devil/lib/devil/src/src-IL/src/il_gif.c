//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_gif.c
//
// Description: Reads from a Graphics Interchange Format (.gif) file.
//
//  The LZW decompression code is based on code released to the public domain
//    by Javier Arevalo and can be found at
//    http://www.programmersheaven.com/zone10/cat452
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_GIF

#include "il_gif.h"


ILenum	GifType;

//! Checks if the file specified in FileName is a valid Gif file.
ILboolean ilIsValidGif(ILconst_string FileName)
{
	ILHANDLE	GifFile;
	ILboolean	bGif = IL_FALSE;

	if (!iCheckExtension(FileName, IL_TEXT("gif"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bGif;
	}

	GifFile = iopenr(FileName);
	if (GifFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bGif;
	}

	bGif = ilIsValidGifF(GifFile);
	icloser(GifFile);

	return bGif;
}


//! Checks if the ILHANDLE contains a valid Gif file at the current position.
ILboolean ilIsValidGifF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidGif();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Checks if Lump is a valid Gif lump.
ILboolean ilIsValidGifL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidGif();
}


// Internal function to get the header and check it.
ILboolean iIsValidGif()
{
	char Header[6];
	
	if (iread(Header, 1, 6) != 6)
		return IL_FALSE;
	iseek(-6, IL_SEEK_CUR);

	if (!strnicmp(Header, "GIF87A", 6))
		return IL_TRUE;
	if (!strnicmp(Header, "GIF89A", 6))
		return IL_TRUE;

	return IL_FALSE;
}


//! Reads a Gif file
ILboolean ilLoadGif(ILconst_string FileName)
{
	ILHANDLE	GifFile;
	ILboolean	bGif = IL_FALSE;

	GifFile = iopenr(FileName);
	if (GifFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bGif;
	}

	bGif = ilLoadGifF(GifFile);
	icloser(GifFile);

	return bGif;
}


//! Reads an already-opened Gif file
ILboolean ilLoadGifF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadGifInternal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Reads from a memory "lump" that contains a Gif
ILboolean ilLoadGifL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
   	return iLoadGifInternal();
}


// Internal function used to load the Gif.
ILboolean iLoadGifInternal()
{
	GIFHEAD	Header;
	ILpal	GlobalPal;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	GlobalPal.Palette = NULL;
	GlobalPal.PalSize = 0;

	//read header
	iread(&Header.Sig, 1, 6);
  	Header.Width = GetLittleUShort();
    Header.Height = GetLittleUShort();
	Header.ColourInfo = igetc();
	Header.Background = igetc();
	Header.Aspect = igetc();
		  
	if (!strnicmp(Header.Sig, "GIF87A", 6)) {
		GifType = GIF87A;
	}
	else if (!strnicmp(Header.Sig, "GIF89A", 6)) {
		GifType = GIF89A;
	}
	else {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	if (!ilTexImage(Header.Width, Header.Height, 1, 1, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE, NULL))
		return IL_FALSE;
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;

	// Check for a global colour map.
	if (Header.ColourInfo & (1 << 7)) {
		if (!iGetPalette(Header.ColourInfo, &GlobalPal, IL_FALSE, NULL)) {
			return IL_FALSE;
		}
	}

	if (!GetImages(&GlobalPal, &Header))
		return IL_FALSE;

	if (GlobalPal.Palette && GlobalPal.PalSize)
		ifree(GlobalPal.Palette);
	GlobalPal.Palette = NULL;
	GlobalPal.PalSize = 0;

	return ilFixImage();
}


ILboolean iGetPalette(ILubyte Info, ILpal *Pal, ILboolean UsePrevPal, ILimage *PrevImage)
{
	ILuint PalSize, PalOffset = 0;

	// If we have a local palette and have to use the previous frame as well,
	//  we have to copy the palette from the previous frame, in addition
	//  to the data, which we copy in GetImages.

	// The ld(palettes bpp - 1) is stored in the lower
	// 3 bits of Info (weird gif format ... :) )
	PalSize = (1 << ((Info & 0x7) + 1)) * 3;
	if (UsePrevPal) {
		if (PrevImage == NULL) {  // Cannot use the previous palette if it does not exist.
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			return IL_FALSE;
		}
		PalSize = PalSize + PrevImage->Pal.PalSize;
		PalOffset = PrevImage->Pal.PalSize;
	}
	if (PalSize > 256 * 3) {
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		return IL_FALSE;
	}
	Pal->PalSize = PalSize;

	Pal->PalType = IL_PAL_RGB24;
	//Pal->Palette = (ILubyte*)ialloc(Pal->PalSize);
	Pal->Palette = (ILubyte*)ialloc(256 * 3);
	if (Pal->Palette == NULL)
		return IL_FALSE;
	if (UsePrevPal)
		memcpy(Pal->Palette, PrevImage->Pal.Palette, PrevImage->Pal.PalSize);  // Copy the old palette over.
	if (iread(Pal->Palette + PalOffset, 1, Pal->PalSize) != Pal->PalSize) {  // Read the new palette.
		ifree(Pal->Palette);
		Pal->Palette = NULL;
		return IL_FALSE;
	}

	return IL_TRUE;
}


ILboolean GetImages(ILpal *GlobalPal, GIFHEAD *GifHead)
{
	IMAGEDESC	ImageDesc, OldImageDesc;
	GFXCONTROL	Gfx;
	ILboolean	BaseImage = IL_TRUE;
	ILimage		*Image = iCurImage, *TempImage = NULL, *PrevImage = NULL;
	ILuint		NumImages = 0, i;
	ILint		input;
	ILuint		PalOffset;

	OldImageDesc.ImageInfo = 0; // to initialize the data with an harmless value 
	Gfx.Used = IL_TRUE;

	while (!ieof()) {
		ILubyte DisposalMethod = 1;
		
		i = itell();
		if (!SkipExtensions(&Gfx))
			goto error_clean;
		i = itell();

		if (!Gfx.Used)
			DisposalMethod = (Gfx.Packed & 0x1C) >> 2;

		//read image descriptor
		ImageDesc.Separator = igetc();
		if (ImageDesc.Separator != 0x2C) //end of image
			break;
		ImageDesc.OffX = GetLittleUShort();
		ImageDesc.OffY = GetLittleUShort();
		ImageDesc.Width = GetLittleUShort();
		ImageDesc.Height = GetLittleUShort();
		ImageDesc.ImageInfo = igetc();

		if (ieof()) {
			ilGetError();  // Gets rid of the IL_FILE_READ_ERROR that inevitably results.
			break;
		}


		if (!BaseImage) {
			NumImages++;
			Image->Next = ilNewImage(iCurImage->Width, iCurImage->Height, 1, 1, 1);
			if (Image->Next == NULL)
				goto error_clean;
			//20040612: DisposalMethod controls how the new images data is to be combined
			//with the old image. 0 means that it doesn't matter how they are combined,
			//1 means keep the old image, 2 means set to background color, 3 is
			//load the image that was in place before the current (this is not implemented
			//here! (TODO?))
			if (DisposalMethod == 2 || DisposalMethod == 3)
				//Note that this is actually wrong, too: If the image has a local
				//color table, we should really search for the best fit of the
				//background color table and use that index (?). Furthermore,
				//we should only memset the part of the image that is not read
				//later (if we are sure that no parts of the read image are transparent).
				if (!Gfx.Used && Gfx.Packed & 0x1)
					memset(Image->Next->Data, Gfx.Transparent, Image->SizeOfData);
				else
					memset(Image->Next->Data, GifHead->Background, Image->SizeOfData);
			else if (DisposalMethod == 1 || DisposalMethod == 0)
				memcpy(Image->Next->Data, Image->Data, Image->SizeOfData);
			//Interlacing has to be removed after the image was copied (line above)
			if (OldImageDesc.ImageInfo & (1 << 6)) {  // Image is interlaced.
				if (!RemoveInterlace(Image))
					goto error_clean;
			}

			PrevImage = Image;
			Image = Image->Next;
			Image->Format = IL_COLOUR_INDEX;
			Image->Origin = IL_ORIGIN_UPPER_LEFT;
		} else {
			BaseImage = IL_FALSE;
			if (!Gfx.Used && Gfx.Packed & 0x1)
				memset(Image->Data, Gfx.Transparent, Image->SizeOfData);
			else
			    memset(Image->Data, GifHead->Background, Image->SizeOfData);
		}

		Image->OffX = ImageDesc.OffX;
		Image->OffY = ImageDesc.OffY;
		PalOffset = 0;

		// Check to see if the image has its own palette.
		if (ImageDesc.ImageInfo & (1 << 7)) {
			ILboolean UsePrevPal = IL_FALSE;
			if (DisposalMethod == 1 && NumImages != 0) {  // Cannot be the first image for this.
				PalOffset = PrevImage->Pal.PalSize;
				UsePrevPal = IL_TRUE;
			}
			if (!iGetPalette(ImageDesc.ImageInfo, &Image->Pal, UsePrevPal, PrevImage)) {
				goto error_clean;
			}
		} else {
			if (!iCopyPalette(&Image->Pal, GlobalPal)) {
				goto error_clean;
			}
		}

		if (!GifGetData(Image, Image->Data + ImageDesc.OffX + ImageDesc.OffY*Image->Width, Image->SizeOfData,
				ImageDesc.Width, ImageDesc.Height, Image->Width, PalOffset, &Gfx)) {
			memset(Image->Data, 0, Image->SizeOfData);  //@TODO: Remove this.  For debugging purposes right now.
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			goto error_clean;
		}

		// See if there was a valid graphics control extension.
		if (!Gfx.Used) {
			Gfx.Used = IL_TRUE;
			Image->Duration = Gfx.Delay * 10;  // We want it in milliseconds.

			// See if a transparent colour is defined.
			if (Gfx.Packed & 1) {
				if (!ConvertTransparent(Image, Gfx.Transparent)) {
					goto error_clean;
				}
	    	}
		}
		i = itell();
		// Terminates each block.
		if((input = igetc()) == IL_EOF)
			goto error_clean;

		if (input != 0x00)
		    iseek(-1, IL_SEEK_CUR);
		//	break;

		OldImageDesc = ImageDesc;
	}

	//Deinterlace last image
	if (OldImageDesc.ImageInfo & (1 << 6)) {  // Image is interlaced.
		if (!RemoveInterlace(Image))
			goto error_clean;
	}

	if (BaseImage)  // Was not able to load any images in...
		return IL_FALSE;

	return IL_TRUE;

error_clean:
	Image = iCurImage->Next;
    /*	while (Image) {
		TempImage = Image;
		Image = Image->Next;
		ilCloseImage(TempImage);
	}*/
	return IL_FALSE;
}


ILboolean SkipExtensions(GFXCONTROL *Gfx)
{
	ILint	Code;
	ILint	Label;
	ILint	Size;

	// DW (06-03-2002):  Apparently there can be...
	//if (GifType == GIF87A)
	//	return IL_TRUE;  // No extensions in the GIF87a format.

	do {
		if((Code = igetc()) == IL_EOF)
			return IL_FALSE;

		if (Code != 0x21) {
			iseek(-1, IL_SEEK_CUR);
			return IL_TRUE;
		}

		if((Label = igetc()) == IL_EOF)
			return IL_FALSE;

		switch (Label)
		{
			case 0xF9:
				Gfx->Size = igetc();
				Gfx->Packed = igetc();
				Gfx->Delay = GetLittleUShort();
				Gfx->Transparent = igetc();
				Gfx->Terminator = igetc();
				if (ieof())
					return IL_FALSE;
				Gfx->Used = IL_FALSE;
				break;
			/*case 0xFE:
				break;

			case 0x01:
				break;*/
			default:
				do {
					if((Size = igetc()) == IL_EOF)
						return IL_FALSE;
					iseek(Size, IL_SEEK_CUR);
				} while (!ieof() && Size != 0);
		}

		// @TODO:  Handle this better.
		if (ieof()) {
			ilSetError(IL_FILE_READ_ERROR);
			return IL_FALSE;
		}
	} while (1);

	return IL_TRUE;
}


#define MAX_CODES 4096

ILint	curr_size, clear, ending, newcodes, top_slot, slot, navail_bytes = 0, nbits_left = 0;
ILubyte	b1;
ILubyte	byte_buff[257];
ILubyte	*pbytes;
ILubyte	*stack;
ILubyte	*suffix;
ILshort	*prefix;

ILboolean success;

ILuint code_mask[13] =
{
   0L,
   0x0001L, 0x0003L,
   0x0007L, 0x000FL,
   0x001FL, 0x003FL,
   0x007FL, 0x00FFL,
   0x01FFL, 0x03FFL,
   0x07FFL, 0x0FFFL
};


ILint get_next_code(void) {
	ILint	i, t;
	ILuint	ret;

	//20050102: Tests for IL_EOF were added because this function
	//crashed sometimes if igetc() returned IL_EOF
	//(for example "table-add-column-before-active.gif" included in the
	//mozilla source package)

	if (!nbits_left) {
		if (navail_bytes <= 0) {
			pbytes = byte_buff;
			navail_bytes = igetc();

			if(navail_bytes == IL_EOF) {
				success = IL_FALSE;
				return ending;
			}

			if (navail_bytes) {
				for (i = 0; i < navail_bytes; i++) {
					if((t = igetc()) == IL_EOF) {
						success = IL_FALSE;
						return ending;
					}
					byte_buff[i] = t;
				}
			}
		}
		b1 = *pbytes++;
		nbits_left = 8;
		navail_bytes--;
	}

	ret = b1 >> (8 - nbits_left);
	while (curr_size > nbits_left) {
		if (navail_bytes <= 0) {
			pbytes = byte_buff;
			navail_bytes = igetc();

			if(navail_bytes == IL_EOF) {
				success = IL_FALSE;
				return ending;
			}

			if (navail_bytes) {
				for (i = 0; i < navail_bytes; i++) {
					if((t = igetc()) == IL_EOF) {
						success = IL_FALSE;
						return ending;
					}
					byte_buff[i] = t;
				}
			}
		}
		b1 = *pbytes++;
		ret |= b1 << nbits_left;
		nbits_left += 8;
		navail_bytes--;
	}
	nbits_left -= curr_size;

	return (ret & code_mask[curr_size]);
}

static void cleanUpGifLoadState()
{
	ifree(stack);
	ifree(suffix);
	ifree(prefix);

}

ILboolean GifGetData(ILimage *Image, ILubyte *Data, ILuint ImageSize, ILuint Width, ILuint Height, ILuint Stride, ILuint PalOffset, GFXCONTROL *Gfx)
{
	ILubyte	*sp;
	ILint	code, fc, oc;
	ILubyte	DisposalMethod = 0;
	ILint	c, size;
	ILuint	i = 0, Read = 0, j = 0;
	ILubyte	*DataPtr = Data;

	if (!Gfx->Used)
		DisposalMethod = (Gfx->Packed & 0x1C) >> 2;
	if((size = igetc()) == IL_EOF)
		return IL_FALSE;

	if (size < 2 || 9 < size) {
		return IL_FALSE;
	}

	stack  = (ILubyte*)ialloc(MAX_CODES + 1);
	suffix = (ILubyte*)ialloc(MAX_CODES + 1);
	prefix = (ILshort*)ialloc(sizeof(*prefix) * (MAX_CODES + 1));
	if (!stack || !suffix || !prefix)
	{
		cleanUpGifLoadState();
		return IL_FALSE;
	}

	curr_size = size + 1;
	top_slot = 1 << curr_size;
	clear = 1 << size;
	ending = clear + 1;
	slot = newcodes = ending + 1;
	navail_bytes = nbits_left = 0;
	oc = fc = 0;
	sp = stack;

	while ((c = get_next_code()) != ending && Read < Height) {
		if (c == clear)
		{
			curr_size = size + 1;
			slot = newcodes;
			top_slot = 1 << curr_size;
			while ((c = get_next_code()) == clear);
			if (c == ending)
				break;
			if (c >= slot)
				c = 0;
			oc = fc = c;

			if (DisposalMethod == 1 && !Gfx->Used && Gfx->Transparent == c && (Gfx->Packed & 0x1) != 0)
				i++;
			else if (i < Width)
				DataPtr[i++] = c + PalOffset;

			if (i == Width)
			{
				DataPtr += Stride;
				i = 0;
				Read += 1;
                ++j;
                if (j >= Height) {
                   cleanUpGifLoadState();
                   return IL_FALSE;
                }
			}
		}
		else
		{
			code = c;
            //BG-2007-01-10: several fixes for incomplete GIFs
			if (code >= slot)
			{
				code = oc;
                if (sp >= stack + MAX_CODES) {
                   cleanUpGifLoadState();
                   return IL_FALSE;
                }
                *sp++ = fc;
			}

            if (code >= MAX_CODES)
                return IL_FALSE; 
                while (code >= newcodes)
				{
                    if (sp >= stack + MAX_CODES)
					{
                        cleanUpGifLoadState();
                        return IL_FALSE;
                    }
                    *sp++ = suffix[code];
                    code = prefix[code];
                }
            
                if (sp >= stack + MAX_CODES) {
                cleanUpGifLoadState();
                return IL_FALSE;
            }

            *sp++ = (ILbyte)code;
			if (slot < top_slot)
			{
				fc = code;
				suffix[slot]   = fc;
				prefix[slot++] = oc;
				oc = c;
			}
			if (slot >= top_slot && curr_size < 12)
			{
				top_slot <<= 1;
				curr_size++;
			}
			while (sp > stack)
			{
				sp--;
				if (DisposalMethod == 1 && !Gfx->Used && Gfx->Transparent == *sp && (Gfx->Packed & 0x1) != 0)
					i++;
				else if (i < Width)
					DataPtr[i++] = *sp + PalOffset;

				if (i == Width)
				{
					i = 0;
					Read += 1;
                    j = (j+1) % Height;
					// Needs to start from Data, not Image->Data.
					DataPtr = Data + j * Stride;
				}
			}
		}
	}
	cleanUpGifLoadState();
	return IL_TRUE;
}


/*From the GIF spec:

  The rows of an Interlaced images are arranged in the following order:

      Group 1 : Every 8th. row, starting with row 0.              (Pass 1)
      Group 2 : Every 8th. row, starting with row 4.              (Pass 2)
      Group 3 : Every 4th. row, starting with row 2.              (Pass 3)
      Group 4 : Every 2nd. row, starting with row 1.              (Pass 4)
*/

ILboolean RemoveInterlace(ILimage *image)
{
	ILubyte *NewData;
	ILuint	i, j = 0;

	NewData = (ILubyte*)ialloc(image->SizeOfData);
	if (NewData == NULL)
		return IL_FALSE;

	//changed 20041230: images with offsety != 0 were not
	//deinterlaced correctly before...
	for (i = 0; i < image->OffY; i++, j++) {
		memcpy(&NewData[i * image->Bps], &image->Data[j * image->Bps], image->Bps);
	}

	for (i = 0 + image->OffY; i < image->Height; i += 8, j++) {
		memcpy(&NewData[i * image->Bps], &image->Data[j * image->Bps], image->Bps);
	}

	for (i = 4 + image->OffY; i < image->Height; i += 8, j++) {
		memcpy(&NewData[i * image->Bps], &image->Data[j * image->Bps], image->Bps);
	}

	for (i = 2 + image->OffY; i < image->Height; i += 4, j++) {
		memcpy(&NewData[i * image->Bps], &image->Data[j * image->Bps], image->Bps);
	}

	for (i = 1 + image->OffY; i < image->Height; i += 2, j++) {
		memcpy(&NewData[i * image->Bps], &image->Data[j * image->Bps], image->Bps);
	}

	ifree(image->Data);
	image->Data = NewData;

	return IL_TRUE;
}


// Uses the transparent colour index to make an alpha channel.
ILboolean ConvertTransparent(ILimage *Image, ILubyte TransColour)
{
	ILubyte	*Palette;
	ILuint	i, j;

	if (!Image->Pal.Palette || !Image->Pal.PalSize) {
		ilSetError(IL_INTERNAL_ERROR);
		return IL_FALSE;
	}

	Palette = (ILubyte*)ialloc(Image->Pal.PalSize / 3 * 4);
	if (Palette == NULL)
		return IL_FALSE;

	for (i = 0, j = 0; i < Image->Pal.PalSize; i += 3, j += 4) {
		Palette[j  ] = Image->Pal.Palette[i  ];
		Palette[j+1] = Image->Pal.Palette[i+1];
		Palette[j+2] = Image->Pal.Palette[i+2];
		if (j/4 == TransColour)
			Palette[j+3] = 0x00;
		else
			Palette[j+3] = 0xFF;
	}

	ifree(Image->Pal.Palette);
	Image->Pal.Palette = Palette;
	Image->Pal.PalSize = Image->Pal.PalSize / 3 * 4;
	Image->Pal.PalType = IL_PAL_RGBA32;

	return IL_TRUE;
}

#endif //IL_NO_GIF
