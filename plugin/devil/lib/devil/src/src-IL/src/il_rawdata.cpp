//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/rawdata.cpp
//
// Description: "Raw" file functions
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
//#ifndef IL_NO_DATA


ILboolean iLoadDataInternal(ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp);


//! Reads a raw data file
ILboolean ILAPIENTRY ilLoadData(ILconst_string FileName, ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp)
{
	ILHANDLE	RawFile;
	ILboolean	bRaw = IL_FALSE;

	// No need to check for raw data
	/*if (!iCheckExtension(FileName, "raw")) {
		ilSetError(IL_INVALID_EXTENSION);
		return bRaw;
	}*/

	RawFile = iopenr(FileName);
	if (RawFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bRaw;
	}

	bRaw = ilLoadDataF(RawFile, Width, Height, Depth, Bpp);
	icloser(RawFile);

	return bRaw;
}


//! Reads an already-opened raw data file
ILboolean ILAPIENTRY ilLoadDataF(ILHANDLE File, ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadDataInternal(Width, Height, Depth, Bpp);
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Reads from a raw data memory "lump"
ILboolean ILAPIENTRY ilLoadDataL(void *Lump, ILuint Size, ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp)
{
	iSetInputLump(Lump, Size);
	return iLoadDataInternal(Width, Height, Depth, Bpp);
}


// Internal function to load a raw data image
ILboolean iLoadDataInternal(ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp)
{
	if (iCurImage == NULL || ((Bpp != 1) && (Bpp != 3) && (Bpp != 4))) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	if (!ilTexImage(Width, Height, Depth, Bpp, 0, IL_UNSIGNED_BYTE, NULL)) {
		return IL_FALSE;
	}
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;

	// Tries to read the correct amount of data
	if (iread(iCurImage->Data, Width * Height * Depth * Bpp, 1) != 1)
		return IL_FALSE;

	if (iCurImage->Bpp == 1)
		iCurImage->Format = IL_LUMINANCE;
	else if (iCurImage->Bpp == 3)
		iCurImage->Format = IL_RGB;
	else  // 4
		iCurImage->Format = IL_RGBA;

	return ilFixImage();
}


//! Save the current image to FileName as raw data
ILboolean ILAPIENTRY ilSaveData(ILconst_string FileName)
{
	ILHANDLE DataFile;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	DataFile = iopenr(FileName);
	if (DataFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	iwrite(iCurImage->Data, 1, iCurImage->SizeOfData);
	icloser(DataFile);

	return IL_TRUE;
}


//#endif//IL_NO_DATA
