//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_wbmp.c
//
// Description: Reads from a Wireless Bitmap (.wbmp) file.  Specs available from
//				http://www.ibm.com/developerworks/wireless/library/wi-wbmp/
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_WBMP
#include "il_bits.h"


ILboolean	iLoadWbmpInternal(void);
ILuint		WbmpGetMultibyte(void);
ILboolean	iSaveWbmpInternal(void);

// Reads a .wbmp file
ILboolean ilLoadWbmp(ILconst_string FileName)
{
	ILHANDLE	WbmpFile;
	ILboolean	bWbmp = IL_FALSE;

	WbmpFile = iopenr(FileName);
	if (WbmpFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bWbmp;
	}

	iSetInputFile(WbmpFile);

	bWbmp = ilLoadWbmpF(WbmpFile);

	icloser(WbmpFile);

	return bWbmp;
}


//! Reads an already-opened .wbmp file
ILboolean ilLoadWbmpF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadWbmpInternal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Reads from a memory "lump" that contains a .wbmp
ILboolean ilLoadWbmpL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadWbmpInternal();
}


ILboolean iLoadWbmpInternal(void)
{
	ILuint	Width, Height, BitPadding, i;
	BITFILE	*File;
	ILubyte	Padding[8];

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}
	
	if (igetc() != 0 || igetc() != 0) {  // The first two bytes have to be 0 (the "header")
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	Width = WbmpGetMultibyte();  // Next follows the width and height.
	Height = WbmpGetMultibyte();

	if (Width == 0 || Height == 0) {  // Must have at least some height and width.
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	if (!ilTexImage(Width, Height, 1, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, NULL))
		return IL_FALSE;
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;  // Always has origin in the upper left.

	BitPadding = (8 - (Width % 8)) % 8;  // Has to be aligned on a byte boundary.  The rest is padding.
	File = bfile(iGetFile());
	if (File == NULL)
		return IL_FALSE;  //@TODO: Error?

	//@TODO: Do this without bread?  Would be faster, since we would not have to do
	//  the second loop.

	// Reads the bits
	for (i = 0; i < iCurImage->Height; i++) {
		bread(&iCurImage->Data[iCurImage->Width * i], 1, iCurImage->Width, File);
		//bseek(File, BitPadding, IL_SEEK_CUR);  //@TODO: This function does not work correctly.
		bread(Padding, 1, BitPadding, File);  // Skip padding bits.
	}
	// Converts bit value of 1 to white and leaves 0 at 0 (2-colour images only).
	for (i = 0; i < iCurImage->SizeOfData; i++) {
		if (iCurImage->Data[i] == 1)
			iCurImage->Data[i] = 0xFF;  // White
	}

	bclose(File);

	return IL_TRUE;
}


ILuint WbmpGetMultibyte()
{
	ILuint Val = 0, i;
	ILubyte Cur;

	for (i = 0; i < 5; i++) {  // Should not be more than 5 bytes.
		Cur = igetc();
		Val = (Val << 7) | (Cur & 0x7F);  // Drop the MSB of Cur.
		if (!(Cur & 0x80)) {  // Check the MSB and break if 0.
			break;
		}
	}

	return Val;
}


ILboolean WbmpPutMultibyte(ILuint Val)
{
	ILint	i, NumBytes = 0;
	ILuint	MultiVal = Val;

	do {
		MultiVal >>= 7;
		NumBytes++;
	} while (MultiVal != 0);

	for (i = NumBytes - 1; i >= 0; i--) {
		MultiVal = (Val >> (i * 7)) & 0x7F;
		if (i != 0)
			MultiVal |= 0x80;
		iputc(MultiVal);
	}

	return IL_TRUE;
}


//! Writes a Wbmp file
ILboolean ilSaveWbmp(const ILstring FileName)
{
	ILHANDLE	WbmpFile;
	ILuint		WbmpSize;

	if (ilGetBoolean(IL_FILE_MODE) == IL_FALSE) {
		if (iFileExists(FileName)) {
			ilSetError(IL_FILE_ALREADY_EXISTS);
			return IL_FALSE;
		}
	}

	WbmpFile = iopenw(FileName);
	if (WbmpFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	WbmpSize = ilSaveWbmpF(WbmpFile);
	iclosew(WbmpFile);

	if (WbmpSize == 0)
		return IL_FALSE;
	return IL_TRUE;
}


//! Writes a .wbmp to an already-opened file
ILuint ilSaveWbmpF(ILHANDLE File)
{
	ILuint Pos;
	iSetOutputFile(File);
	Pos = itellw();
	if (iSaveWbmpInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


//! Writes a .wbmp to a memory "lump"
ILuint ilSaveWbmpL(void *Lump, ILuint Size)
{
	ILuint Pos;
	iSetOutputLump(Lump, Size);
	Pos = itellw();
	if (iSaveWbmpInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


// In il_quantizer.c
ILimage *iQuantizeImage(ILimage *Image, ILuint NumCols);
// In il_neuquant.c
ILimage *iNeuQuant(ILimage *Image, ILuint NumCols);


// Internal function used to save the Wbmp.
ILboolean iSaveWbmpInternal()
{
	ILimage	*TempImage = NULL;
	ILuint	i, j;
	ILint	k;
	ILubyte	Val;
	ILubyte	*TempData;

	iputc(0);  // First two header values
	iputc(0);  //  must be 0.

	WbmpPutMultibyte(iCurImage->Width);  // Write the width
	WbmpPutMultibyte(iCurImage->Height); //  and the height.

	//TempImage = iConvertImage(iCurImage, IL_LUMINANCE, IL_UNSIGNED_BYTE);
	if (iGetInt(IL_QUANTIZATION_MODE) == IL_NEU_QUANT)
		TempImage = iNeuQuant(iCurImage, 2);
	else // Assume IL_WU_QUANT otherwise.
		TempImage = iQuantizeImage(iCurImage, 2);

	if (TempImage == NULL)
		return IL_FALSE;

	if (TempImage->Origin != IL_ORIGIN_UPPER_LEFT) {
		TempData = iGetFlipped(TempImage);
		if (TempData == NULL) {
			ilCloseImage(TempImage);
			return IL_FALSE;
		}
	} else {
		TempData = TempImage->Data;
	}

	for (i = 0; i < TempImage->Height; i++) {
		for (j = 0; j < TempImage->Width; j += 8) {
			Val = 0;
			for (k = 0; k < 8; k++) {
				if (j + k < TempImage->Width) {
					//Val |= ((TempData[TempImage->Width * i + j + k] > 0x7F) ? (0x80 >> k) : 0x00);
					Val |= ((TempData[TempImage->Width * i + j + k] == 1) ? (0x80 >> k) : 0x00);
				}
			}
			iputc(Val);
		}
	}

	if (TempData != TempImage->Data)
		ifree(TempData);
	ilCloseImage(TempImage);

	return IL_TRUE;
}

#endif//IL_NO_WBMP

