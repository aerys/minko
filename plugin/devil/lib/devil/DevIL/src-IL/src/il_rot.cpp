//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 02/15/2009
//
// Filename: src-IL/src/il_rot.cpp
//
// Description: Reads from a Homeworld 2 - Relic Texture (.rot) file.
//
//-----------------------------------------------------------------------------

// @TODO:
// Note: I am not certain about which is DXT3 and which is DXT5.  According to
//  http://forums.relicnews.com/showthread.php?t=20512, DXT3 is 1030, and DXT5
//  is 1029.  However, neither way seems to work quite right for the alpha.

#include "il_internal.h"
#ifndef IL_NO_ROT
#include "il_dds.h"

ILboolean iLoadRotInternal(void);

#define ROT_RGBA32	1024
#define ROT_DXT1	1028
#define ROT_DXT3	1029
#define ROT_DXT5	1030


//! Reads a ROT file
ILboolean ilLoadRot(ILconst_string FileName)
{
	ILHANDLE	RotFile;
	ILboolean	bRot = IL_FALSE;
	
	RotFile = iopenr(FileName);
	if (RotFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bRot;
	}

	bRot = ilLoadRotF(RotFile);
	icloser(RotFile);

	return bRot;
}


//! Reads an already-opened ROT file
ILboolean ilLoadRotF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;
	
	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadRotInternal();
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Reads from a memory "lump" that contains a ROT
ILboolean ilLoadRotL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadRotInternal();
}


// Internal function used to load the ROT.
ILboolean iLoadRotInternal(void)
{
	ILubyte		Form[4], FormName[4];
	ILuint		FormLen, Width, Height, Format, Channels, CompSize;
	ILuint		MipSize, MipLevel, MipWidth, MipHeight;
	ILenum		FormatIL;
	ILimage		*Image;
	ILboolean	BaseCreated = IL_FALSE;
	ILubyte		*CompData = NULL;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	// The first entry in the file must be 'FORM', 0x20 in a big endian integer and then 'HEAD'.
	iread(Form, 1, 4);
	FormLen = GetBigUInt();
	iread(FormName, 1, 4);
	if (strncmp((char*)Form, "FORM", 4) || FormLen != 0x14 || strncmp((char*)FormName, "HEAD", 4)) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	// Next follows the width, height and format in the header.
	Width = GetLittleUInt();
	Height = GetLittleUInt();
	Format = GetLittleUInt();

	//@TODO: More formats.
	switch (Format)
	{
		case ROT_RGBA32:  // 32-bit RGBA format
			Channels = 4;
			FormatIL = IL_RGBA;
			break;

		case ROT_DXT1:  // DXT1 (no alpha)
			Channels = 4;
			FormatIL = IL_RGBA;
			break;

		case ROT_DXT3:  // DXT3
		case ROT_DXT5:  // DXT5
			Channels = 4;
			FormatIL = IL_RGBA;
			// Allocates the maximum needed (the first width/height given in the file).
			CompSize = ((Width + 3) / 4) * ((Height + 3) / 4) * 16;
			CompData = (ILubyte*)ialloc(CompSize);
			if (CompData == NULL)
				return IL_FALSE;
			break;

		default:
			ilSetError(IL_INVALID_FILE_HEADER);
			return IL_FALSE;
	}

	if (Width == 0 || Height == 0) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	//@TODO: Find out what this is.
	GetLittleUInt();  // Skip this for the moment.  This appears to be the number of channels.

	// Next comes 'FORM', a length and 'MIPS'.
	iread(Form, 1, 4);
	FormLen = GetBigUInt();
	iread(FormName, 1, 4);
	//@TODO: Not sure if the FormLen has to be anything specific here.
	if (strncmp((char*)Form, "FORM", 4) || strncmp((char*)FormName, "MIPS", 4)) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	//@TODO: Can these mipmap levels be in any order?  Some things may be easier if the answer is no.
	Image = iCurImage;
	do {
		// Then we have 'FORM' again.
		iread(Form, 1, 4);
		// This is the size of the mipmap data.
		MipSize = GetBigUInt();
		iread(FormName, 1, 4);
		if (strncmp((char*)Form, "FORM", 4)) {
			if (!BaseCreated) {  // Our file is malformed.
				ilSetError(IL_INVALID_FILE_HEADER);
				return IL_FALSE;
			}
			// We have reached the end of the mipmap data.
			break;
		}
		if (strncmp((char*)FormName, "MLVL", 4)) {
			ilSetError(IL_INVALID_FILE_HEADER);
			return IL_FALSE;
		}

		// Next is the mipmap attributes (level number, width, height and length)
		MipLevel = GetLittleUInt();
		MipWidth = GetLittleUInt();
		MipHeight = GetLittleUInt();
		MipSize = GetLittleUInt();  // This is the same as the previous size listed -20 (for attributes).

		// Lower level mipmaps cannot be larger than the main image.
		if (MipWidth > Width || MipHeight > Height || MipSize > CompSize) {
			ilSetError(IL_INVALID_FILE_HEADER);
			return IL_FALSE;
		}

		// Just create our images here.
		if (!BaseCreated) {
			if (!ilTexImage(MipWidth, MipHeight, 1, Channels, FormatIL, IL_UNSIGNED_BYTE, NULL))
				return IL_FALSE;
			BaseCreated = IL_TRUE;
		}
		else {
			Image->Mipmaps = ilNewImageFull(MipWidth, MipHeight, 1, Channels, FormatIL, IL_UNSIGNED_BYTE, NULL);
			Image = Image->Mipmaps;
		}

		switch (Format)
		{
			case ROT_RGBA32:  // 32-bit RGBA format
				if (iread(Image->Data, Image->SizeOfData, 1) != 1)
					return IL_FALSE;
				break;

			case ROT_DXT1:
				// Allocates the size of the compressed data.
				CompSize = ((MipWidth + 3) / 4) * ((MipHeight + 3) / 4) * 8;
				if (CompSize != MipSize) {
					ilSetError(IL_INVALID_FILE_HEADER);
					return IL_FALSE;
				}
				CompData = (ILubyte*)ialloc(CompSize);
				if (CompData == NULL)
					return IL_FALSE;

				// Read in the DXT1 data...
				if (iread(CompData, CompSize, 1) != 1)
					return IL_FALSE;
				// ...and decompress it.
				if (!DecompressDXT1(Image, CompData)) {
					ifree(CompData);
					return IL_FALSE;
				}
				if (ilGetInteger(IL_KEEP_DXTC_DATA) == IL_TRUE) {
					Image->DxtcSize = CompSize;
					Image->DxtcData = CompData;
					Image->DxtcFormat = IL_DXT1;
					CompData = NULL;
				}
				break;

			case ROT_DXT3:
				// Allocates the size of the compressed data.
				CompSize = ((MipWidth + 3) / 4) * ((MipHeight + 3) / 4) * 16;
				if (CompSize != MipSize) {
					ilSetError(IL_INVALID_FILE_HEADER);
					return IL_FALSE;
				}
				CompData = (ILubyte*)ialloc(CompSize);
				if (CompData == NULL)
					return IL_FALSE;

				// Read in the DXT3 data...
				if (iread(CompData, MipSize, 1) != 1)
					return IL_FALSE;
				// ...and decompress it.
				if (!DecompressDXT3(Image, CompData)) {
					ifree(CompData);
					return IL_FALSE;
				}
				if (ilGetInteger(IL_KEEP_DXTC_DATA) == IL_TRUE) {
					Image->DxtcSize = CompSize;
					Image->DxtcData = CompData;
					Image->DxtcFormat = IL_DXT3;
					CompData = NULL;
				}
				break;

			case ROT_DXT5:
				// Allocates the size of the compressed data.
				CompSize = ((MipWidth + 3) / 4) * ((MipHeight + 3) / 4) * 16;
				if (CompSize != MipSize) {
					ilSetError(IL_INVALID_FILE_HEADER);
					return IL_FALSE;
				}
				CompData = (ILubyte*)ialloc(CompSize);
				if (CompData == NULL)
					return IL_FALSE;

				// Read in the DXT5 data...
				if (iread(CompData, MipSize, 1) != 1)
					return IL_FALSE;
				// ...and decompress it.
				if (!DecompressDXT5(Image, CompData)) {
					ifree(CompData);
					return IL_FALSE;
				}
				// Keeps a copy
				if (ilGetInteger(IL_KEEP_DXTC_DATA) == IL_TRUE) {
					Image->DxtcSize = CompSize;
					Image->DxtcData = CompData;
					Image->DxtcFormat = IL_DXT5;
					CompData = NULL;
				}
				break;
		}
		ifree(CompData);  // Free it if it was not saved.
	} while (!ieof());  //@TODO: Is there any other condition that should end this?

	return ilFixImage();
}

#endif//IL_NO_ROT

