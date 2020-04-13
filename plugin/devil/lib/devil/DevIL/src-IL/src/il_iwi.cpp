//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/04/2009
//
// Filename: src-IL/src/il_iwi.cpp
//
// Description: Reads from an Infinity Ward Image (.iwi) file from Call of Duty.
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_IWI
#include "il_dds.h"

typedef struct IWIHEAD
{
	ILuint		Signature;
	ILubyte		Format;
	ILubyte		Flags;
	ILushort	Width;
	ILushort	Height;
} IWIHEAD;

#define IWI_ARGB8	0x01
#define IWI_RGB8	0x02
#define IWI_ARGB4	0x03
#define IWI_A8		0x04
#define IWI_JPG		0x07
#define IWI_DXT1	0x0B
#define IWI_DXT3	0x0C
#define IWI_DXT5	0x0D

ILboolean iIsValidIwi(void);
ILboolean iCheckIwi(IWIHEAD *Header);
ILboolean iLoadIwiInternal(void);
ILboolean IwiInitMipmaps(ILimage *BaseImage, ILuint *NumMips);
ILboolean IwiReadImage(ILimage *BaseImage, IWIHEAD *Header, ILuint NumMips);
ILenum IwiGetFormat(ILubyte Format, ILubyte *Bpp);

//! Checks if the file specified in FileName is a valid IWI file.
ILboolean ilIsValidIwi(ILconst_string FileName)
{
	ILHANDLE	IwiFile;
	ILboolean	bIwi = IL_FALSE;
	
	if (!iCheckExtension(FileName, IL_TEXT("iwi"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bIwi;
	}
	
	IwiFile = iopenr(FileName);
	if (IwiFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bIwi;
	}
	
	bIwi = ilIsValidIwiF(IwiFile);
	icloser(IwiFile);
	
	return bIwi;
}


//! Checks if the ILHANDLE contains a valid IWI file at the current position.
ILboolean ilIsValidIwiF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;
	
	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidIwi();
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Checks if Lump is a valid IWI lump.
ILboolean ilIsValidIwiL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidIwi();
}


// Internal function used to get the IWI header from the current file.
ILboolean iGetIwiHead(IWIHEAD *Header)
{
	Header->Signature = GetLittleUInt();
	Header->Format = igetc();
	Header->Flags = igetc();  //@TODO: Find out what the flags mean.
	Header->Width = GetLittleUShort();
	Header->Height = GetLittleUShort();

	// @TODO: Find out what is in the rest of the header.
	iseek(18, IL_SEEK_CUR);

	return IL_TRUE;
}


// Internal function to get the header and check it.
ILboolean iIsValidIwi(void)
{
	IWIHEAD		Header;
	ILuint		Pos = itell();

	if (!iGetIwiHead(&Header))
		return IL_FALSE;
	// The length of the header varies, so we just go back to the original position.
	iseek(Pos, IL_SEEK_CUR);

	return iCheckIwi(&Header);
}


// Internal function used to check if the HEADER is a valid IWI header.
ILboolean iCheckIwi(IWIHEAD *Header)
{
	if (Header->Signature != 0x06695749 && Header->Signature != 0x05695749)  // 'IWi-' (version 6, and version 5 is the second).
		return IL_FALSE;
	if (Header->Width == 0 || Header->Height == 0)
		return IL_FALSE;
	// DXT images must have power-of-2 dimensions.
	if (Header->Format == IWI_DXT1 || Header->Format == IWI_DXT3 || Header->Format == IWI_DXT5)
		if (Header->Width != ilNextPower2(Header->Width) || Header->Height != ilNextPower2(Header->Height))
			return IL_FALSE;
	// 0x0B, 0x0C and 0x0D are DXT formats.
	if (Header->Format != IWI_ARGB4 && Header->Format != IWI_RGB8 && Header->Format != IWI_ARGB8 && Header->Format != IWI_A8 
		&& Header->Format != IWI_DXT1 && Header->Format != IWI_DXT3 && Header->Format != IWI_DXT5)
		return IL_FALSE;

	return IL_TRUE;
}


//! Reads a IWI file
ILboolean ilLoadIwi(ILconst_string FileName)
{
	ILHANDLE	IwiFile;
	ILboolean	bIwi = IL_FALSE;
	
	IwiFile = iopenr(FileName);
	if (IwiFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bIwi;
	}

	bIwi = ilLoadIwiF(IwiFile);
	icloser(IwiFile);

	return bIwi;
}


//! Reads an already-opened IWI file
ILboolean ilLoadIwiF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;
	
	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadIwiInternal();
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Reads from a memory "lump" that contains a IWI
ILboolean ilLoadIwiL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadIwiInternal();
}


// Internal function used to load the IWI.
ILboolean iLoadIwiInternal(void)
{
	IWIHEAD		Header;
	ILuint		NumMips = 0;
	ILboolean	HasMipmaps = IL_TRUE;
	ILenum		Format;
	ILubyte		Bpp;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	// Read the header and check it.
	if (!iGetIwiHead(&Header))
		return IL_FALSE;
	if (!iCheckIwi(&Header)) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	// From a post by Pointy on http://iwnation.com/forums/index.php?showtopic=27903,
	//  flags ending with 0x3 have no mipmaps.
	HasMipmaps = ((Header.Flags & 0x03) == 0x03) ? IL_FALSE : IL_TRUE;

	// Create the image, then create the mipmaps, then finally read the image.
	Format = IwiGetFormat(Header.Format, &Bpp);
	if (!ilTexImage(Header.Width, Header.Height, 1, Bpp, Format, IL_UNSIGNED_BYTE, NULL))
		return IL_FALSE;
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;
	if (HasMipmaps)
		if (!IwiInitMipmaps(iCurImage, &NumMips))
			return IL_FALSE;
	if (!IwiReadImage(iCurImage, &Header, NumMips))
		return IL_FALSE;

	return ilFixImage();
}


// Helper function to convert IWI formats to DevIL formats and Bpp.
ILenum IwiGetFormat(ILubyte Format, ILubyte *Bpp)
{
	switch (Format)
	{
		case IWI_ARGB8:
			*Bpp = 4;
			return IL_BGRA;
		case IWI_RGB8:
			*Bpp = 3;
			return IL_BGR;
		case IWI_ARGB4:
			*Bpp = 4;
			return IL_BGRA;
		case IWI_A8:
			*Bpp = 1;
			return IL_ALPHA;
		case IWI_DXT1:
			*Bpp = 4;
			return IL_RGBA;
		case IWI_DXT3:
			*Bpp = 4;
			return IL_RGBA;
		case IWI_DXT5:
			*Bpp = 4;
			return IL_RGBA;
	}

	return 0;  // Will never reach this.
}


// Function to intialize the mipmaps and determine the number of mipmaps.
ILboolean IwiInitMipmaps(ILimage *BaseImage, ILuint *NumMips)
{
	ILimage	*Image;
	ILuint	Width, Height, Mipmap;

	Image = BaseImage;
	Width = BaseImage->Width;  Height = BaseImage->Height;
	Image->Origin = IL_ORIGIN_UPPER_LEFT;

	for (Mipmap = 0; Width != 1 && Height != 1; Mipmap++) {
		// 1 is the smallest dimension possible.
		Width = (Width >> 1) == 0 ? 1 : (Width >> 1);
		Height = (Height >> 1) == 0 ? 1 : (Height >> 1);

		Image->Mipmaps = ilNewImageFull(Width, Height, 1, BaseImage->Bpp, BaseImage->Format, BaseImage->Type, NULL);
		if (Image->Mipmaps == NULL)
			return IL_FALSE;
		Image = Image->Mipmaps;

		// ilNewImage does not set these.
		Image->Format = BaseImage->Format;
		Image->Type = BaseImage->Type;
		// The origin is in the upper left.
		Image->Origin = IL_ORIGIN_UPPER_LEFT;
	}

	*NumMips = Mipmap;
	return IL_TRUE;
}


ILboolean IwiReadImage(ILimage *BaseImage, IWIHEAD *Header, ILuint NumMips)
{
	ILimage	*Image;
	ILuint	SizeOfData;
	ILubyte	*CompData = NULL;
	ILint	i, j, k, m;

	for (i = NumMips; i >= 0; i--) {
		Image = BaseImage;
		// Go to the ith mipmap level.
		//  The mipmaps go from smallest to the largest.
		for (j = 0; j < i; j++)
			Image = Image->Mipmaps;

		switch (Header->Format)
		{
			case IWI_ARGB8: // These are all
			case IWI_RGB8:  //  uncompressed data,
			case IWI_A8:    //  so just read it.
				if (iread(Image->Data, 1, Image->SizeOfData) != Image->SizeOfData)
					return IL_FALSE;
				break;

			case IWI_ARGB4:  //@TODO: Find some test images for this.
				// Data is in ARGB4 format - 4 bits per component.
				SizeOfData = Image->Width * Image->Height * 2;
				CompData = (ILubyte*)ialloc(SizeOfData);  // Not really compressed - just in ARGB4 format.
				if (CompData == NULL)
					return IL_FALSE;
				if (iread(CompData, 1, SizeOfData) != SizeOfData) {
					ifree(CompData);
					return IL_FALSE;
				}
				for (k = 0, m = 0; k < (ILint)Image->SizeOfData; k += 4, m += 2) {
					// @TODO: Double the image data into the low and high nibbles for a better range of values.
					Image->Data[k+0] = CompData[m] & 0xF0;
					Image->Data[k+1] = (CompData[m] & 0x0F) << 4;
					Image->Data[k+2] = CompData[m+1] & 0xF0;
					Image->Data[k+3] = (CompData[m+1] & 0x0F) << 4;
				}
				break;

			case IWI_DXT1:
				// DXT1 data has at least 8 bytes, even for one pixel.
				SizeOfData = IL_MAX(Image->Width * Image->Height / 2, 8);
				CompData = (ILubyte*)ialloc(SizeOfData);  // Gives a 6:1 compression ratio (or 8:1 for DXT1 with alpha)
				if (CompData == NULL)
					return IL_FALSE;
				if (iread(CompData, 1, SizeOfData) != SizeOfData) {
					ifree(CompData);
					return IL_FALSE;
				}

				// Decompress the DXT1 data into Image (ith mipmap).
				if (!DecompressDXT1(Image, CompData)) {
					ifree(CompData);
					return IL_FALSE;
				}

				// Keep a copy of the DXTC data if the user wants it.
				if (ilGetInteger(IL_KEEP_DXTC_DATA) == IL_TRUE) {
					Image->DxtcSize = SizeOfData;
					Image->DxtcData = CompData;
					Image->DxtcFormat = IL_DXT1;
					CompData = NULL;
				}

				break;

			case IWI_DXT3:
				// DXT3 data has at least 16 bytes, even for one pixel.
				SizeOfData = IL_MAX(Image->Width * Image->Height, 16);
				CompData = (ILubyte*)ialloc(SizeOfData);  // Gives a 4:1 compression ratio
				if (CompData == NULL)
					return IL_FALSE;
				if (iread(CompData, 1, SizeOfData) != SizeOfData) {
					ifree(CompData);
					return IL_FALSE;
				}

				// Decompress the DXT3 data into Image (ith mipmap).
				if (!DecompressDXT3(Image, CompData)) {
					ifree(CompData);
					return IL_FALSE;
				}
				break;

			case IWI_DXT5:
				// DXT5 data has at least 16 bytes, even for one pixel.
				SizeOfData = IL_MAX(Image->Width * Image->Height, 16);
				CompData = (ILubyte*)ialloc(SizeOfData);  // Gives a 4:1 compression ratio
				if (CompData == NULL)
					return IL_FALSE;
				if (iread(CompData, 1, SizeOfData) != SizeOfData) {
					ifree(CompData);
					return IL_FALSE;
				}

				// Decompress the DXT5 data into Image (ith mipmap).
				if (!DecompressDXT5(Image, CompData)) {
					ifree(CompData);
					return IL_FALSE;
				}
				break;
		}
	
		ifree(CompData);
	}

	return IL_TRUE;
}


#endif//IL_NO_IWI
