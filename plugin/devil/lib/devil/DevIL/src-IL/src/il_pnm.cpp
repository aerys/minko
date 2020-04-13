//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_pnm.cpp
//
// Description: Reads/writes to/from pbm/pgm/ppm formats (enough slashes? =)
//
//-----------------------------------------------------------------------------



#include "il_internal.h"
#ifndef IL_NO_PNM
#include "il_pnm.h"
#include <limits.h>  // for maximum values
#include <ctype.h>
#include "il_bits.h"

// According to the ppm specs, it's 70, but PSP
//  likes to output longer lines.
#define MAX_BUFFER 180  
static ILbyte LineBuffer[MAX_BUFFER];
static ILbyte SmallBuff[MAX_BUFFER];

// Can't read direct bits from a lump yet
ILboolean IsLump = IL_FALSE;


//! Checks if the file specified in FileName is a valid .pnm file.
ILboolean ilIsValidPnm(ILconst_string FileName)
{
	ILHANDLE	PnmFile;
	ILboolean	bPnm = IL_FALSE;

	if (   !iCheckExtension(FileName, IL_TEXT("pbm"))
		&& !iCheckExtension(FileName, IL_TEXT("pgm"))
		&& !iCheckExtension(FileName, IL_TEXT("ppm"))
		&& !iCheckExtension(FileName, IL_TEXT("pnm"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bPnm;
	}

	PnmFile = iopenr(FileName);
	if (PnmFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bPnm;
	}

	bPnm = ilIsValidPnmF(PnmFile);
	icloser(PnmFile);

	return bPnm;
}


//! Checks if the ILHANDLE contains a valid .pnm file at the current position.
ILboolean ilIsValidPnmF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidPnm();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Checks if Lump is a valid .pnm lump.
ILboolean ilIsValidPnmL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidPnm();
}


// Internal function to get the header and check it.
ILboolean iIsValidPnm()
{
	char	Head[2];
	ILint	Read;

	Read = iread(Head, 1, 2);
	iseek(-Read, IL_SEEK_CUR);  // Go ahead and restore to previous state
	if (Read != 2)
		return IL_FALSE;

	return iCheckPnm(Head);
}


// Internal function used to check if the HEADER is a valid .pnm header.
ILboolean iCheckPnm(char Header[2])
{
	if (Header[0] != 'P')
		return IL_FALSE;
	switch (Header[1])
	{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
			return IL_TRUE;
	}

	return IL_FALSE;
}


// Reads a file
ILboolean ilLoadPnm(ILconst_string FileName)
{
	ILHANDLE	PnmFile;
	ILboolean	bPnm = IL_FALSE;

	PnmFile = iopenr(FileName);
	if (PnmFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bPnm;
	}

	bPnm = ilLoadPnmF(PnmFile);
	icloser(PnmFile);

	return bPnm;
}


// Reads an already-opened file
ILboolean ilLoadPnmF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadPnmInternal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


// Reads from a memory "lump"
ILboolean ilLoadPnmL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadPnmInternal();
}


// Load either a pgm or a ppm
ILboolean iLoadPnmInternal()
{
	ILimage		*PmImage = NULL;
	PPMINFO		Info;
//	ILuint		LineInc = 0, SmallInc = 0;

	Info.Type = 0;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	// Find out what type of pgm/ppm this is
	if (iGetWord(IL_FALSE) == IL_FALSE)
		return IL_FALSE;

	if (SmallBuff[0] != 'P') {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	switch( SmallBuff[1] ) {
		case '1':
			Info.Type = IL_PBM_ASCII;
			break;
		case '2':
			Info.Type = IL_PGM_ASCII;
			break;
		case '3':
			Info.Type = IL_PPM_ASCII;
			break;
		case '4':
			Info.Type = IL_PBM_BINARY;
			if (IsLump) {
				ilSetError(IL_FORMAT_NOT_SUPPORTED);
				return IL_FALSE;
			}
			break;
		case '5':
			Info.Type = IL_PGM_BINARY;
			break;
		case '6':
			Info.Type = IL_PPM_BINARY;
			break;
		default:
			ilSetError(IL_INVALID_FILE_HEADER);
			return IL_FALSE;
	}
	
	// Retrieve the width and height
	if (iGetWord(IL_FALSE) == IL_FALSE)
		return IL_FALSE;
	Info.Width = atoi((const char*)SmallBuff);
	if (Info.Width == 0) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	if (iGetWord(IL_FALSE) == IL_FALSE)
		return IL_FALSE;
	Info.Height = atoi((const char*)SmallBuff);
	if (Info.Height == 0) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	// Retrieve the maximum colour component value
	if (Info.Type != IL_PBM_ASCII && Info.Type != IL_PBM_BINARY) {
		if (iGetWord(IL_TRUE) == IL_FALSE)
			return IL_FALSE;
		if ((Info.MaxColour = atoi((const char*)SmallBuff)) == 0) {
			ilSetError(IL_INVALID_FILE_HEADER);
			return IL_FALSE;
		}
	} else {
		Info.MaxColour = 1;
	}
	
	if (Info.Type == IL_PBM_ASCII || Info.Type == IL_PBM_BINARY ||
		Info.Type == IL_PGM_ASCII || Info.Type == IL_PGM_BINARY) {
		if (Info.Type == IL_PGM_ASCII) {
			Info.Bpp = Info.MaxColour < 256 ? 1 : 2;
		} else {
			Info.Bpp = 1;
		}
	} else {
		Info.Bpp = 3;
	}
	
	switch (Info.Type) {
		case IL_PBM_ASCII:
		case IL_PGM_ASCII:
		case IL_PPM_ASCII:
			PmImage = ilReadAsciiPpm(&Info);
			break;
		case IL_PBM_BINARY:
			PmImage = ilReadBitPbm(&Info);
			break;
		case IL_PGM_BINARY:
		case IL_PPM_BINARY:
			PmImage = ilReadBinaryPpm(&Info);
			break;
		default:
			return IL_FALSE;
	}

	if (PmImage == NULL) {
	    iCurImage->Format = ilGetFormatBpp(iCurImage->Bpp);
	    ilSetError(IL_FILE_READ_ERROR);
	    return IL_FALSE;
	}

	// Is this conversion needed?  Just 0's and 1's shows up as all black
	if (Info.Type == IL_PBM_ASCII) {
		PbmMaximize(PmImage);
	}

	if (Info.MaxColour > 255)
		PmImage->Type = IL_UNSIGNED_SHORT;
	PmImage->Origin = IL_ORIGIN_UPPER_LEFT;
	if (Info.Type == IL_PBM_ASCII || Info.Type == IL_PBM_BINARY ||
		Info.Type == IL_PGM_ASCII || Info.Type == IL_PGM_BINARY)
		PmImage->Format = IL_LUMINANCE;
	else
		PmImage->Format = IL_RGB;
	PmImage->Origin = IL_ORIGIN_UPPER_LEFT;

	if (PmImage == NULL)
		return IL_FALSE;
	return ilFixImage();
}



ILimage *ilReadAsciiPpm(PPMINFO *Info)
{
	ILint	LineInc = 0, SmallInc = 0, DataInc = 0, Size;
//	ILint	BytesRead = 0;

	if (Info->MaxColour > 255)
		Info->Bpp *= 2;

	Size = Info->Width * Info->Height * Info->Bpp;

	if (!ilTexImage(Info->Width, Info->Height, 1, (ILubyte)(Info->Bpp), 0, IL_UNSIGNED_BYTE, NULL)) {
		return IL_FALSE;
	}
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;
	if (Info->MaxColour > 255)
		iCurImage->Type = IL_UNSIGNED_SHORT;

	while (DataInc < Size) {  // && !feof(File)) {
		LineInc = 0;

		if (iFgets((char *)LineBuffer, MAX_BUFFER) == NULL) {
			//ilSetError(IL_ILLEGAL_FILE_VALUE);
			//return NULL;
			//return iCurImage;
			break;
		}
		if (LineBuffer[0] == '#') {  // Comment
			continue;
		}

		while ((LineBuffer[LineInc] != NUL) && (LineBuffer[LineInc] != '\n')) {

			SmallInc = 0;
			while (!isalnum(LineBuffer[LineInc])) {  // Skip any whitespace
				LineInc++;
			}
			while (isalnum(LineBuffer[LineInc])) {
				SmallBuff[SmallInc] = LineBuffer[LineInc];
				SmallInc++;
				LineInc++;
			}
			SmallBuff[SmallInc] = NUL;
			iCurImage->Data[DataInc] = atoi((const char*)SmallBuff);  // Convert from string to colour

			// PSP likes to put whitespace at the end of lines...figures. =/
			while (!isalnum(LineBuffer[LineInc]) && LineBuffer[LineInc] != NUL) {  // Skip any whitespace
				LineInc++;
			}

			// We should set some kind of state flag that enables this
			//Image->Data[DataInc] *= (ILubyte)(255 / Info->MaxColour);  // Scales to 0-255		
			if (Info->MaxColour > 255)
				DataInc++;
			DataInc++;
		}
	}

	// If we read less than what we should have...
	if (DataInc < Size) {
		//ilCloseImage(iCurImage);
		//ilSetCurImage(NULL);
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		return NULL;
	}

	return iCurImage;
}


ILimage *ilReadBinaryPpm(PPMINFO *Info)
{
	ILuint	Size;

	Size = Info->Width * Info->Height * Info->Bpp;

	if (!ilTexImage(Info->Width, Info->Height, 1, (ILubyte)(Info->Bpp), 0, IL_UNSIGNED_BYTE, NULL)) {
		return IL_FALSE;
	}
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;

	/* 4/3/2007 Dario Meloni
	 Here it seems we have eaten too much bytes and it is needed to fix 
	 the starting point
	 works well on various images
	
	No more need of this workaround. fixed iGetWord
	iseek(0,IL_SEEK_END);
	ILuint size = itell();
	iseek(size-Size,IL_SEEK_SET);
	*/
	if (iread(iCurImage->Data, 1, Size ) != Size) {
		ilCloseImage(iCurImage);	
		return NULL;
	}
	return iCurImage;
}


ILimage *ilReadBitPbm(PPMINFO *Info)
{
	ILuint	m, j, x, CurrByte;

	if (!ilTexImage(Info->Width, Info->Height, 1, (ILubyte)(Info->Bpp), 0, IL_UNSIGNED_BYTE, NULL)) {
		return IL_FALSE;
	}
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;

	x = 0;
	for (j = 0; j < iCurImage->SizeOfData;) {
		CurrByte = igetc();
		for (m = 128; m > 0 && x < Info->Width; m >>= 1, ++x, ++j) {
			iCurImage->Data[j] = (CurrByte & m)?255:0;
		}
		if (x == Info->Width)
			x = 0;
	}

	return iCurImage;
}


ILboolean iGetWord(ILboolean final)
{
	ILint WordPos = 0;
	ILint Current = 0;
	ILboolean Started = IL_FALSE;
	ILboolean Looping = IL_TRUE;

	if (ieof())
		return IL_FALSE;

	while (Looping) {
		while ((Current = igetc()) != IL_EOF && Current != '\n' && Current != '#' && Current != ' ') {
			if (WordPos >= MAX_BUFFER)  // We have hit the maximum line length.
				return IL_FALSE;

			if (!isalnum(Current)) {
				if (Started) {
					Looping = IL_FALSE;
					break;
				}
				continue;
			}

			if (Looping)
				SmallBuff[WordPos++] = Current;
		}
		if (Current == IL_EOF)
			return IL_FALSE;
		SmallBuff[WordPos] = 0; // 08-17-2008 - was NULL, changed to avoid warning
		if (final == IL_TRUE)
	        break;

		if (!Looping)
			break;

		if (Current == '#') {  // '#' is a comment...read until end of line
			while ((Current = igetc()) != IL_EOF && Current != '\n');
		}

		// Get rid of any erroneous spaces
		while ((Current = igetc()) != IL_EOF) {
			if (Current != ' ')
				break;
		}
		iseek(-1, IL_SEEK_CUR);

		if (WordPos > 0)
			break;
	}

	if (Current == -1 || WordPos == 0) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	return IL_TRUE;
}


ILstring FName = NULL;

//! Writes a Pnm file
ILboolean ilSavePnm(const ILstring FileName)
{
	ILHANDLE	PnmFile;
	ILuint		PnmSize;

	if (ilGetBoolean(IL_FILE_MODE) == IL_FALSE) {
		if (iFileExists(FileName)) {
			ilSetError(IL_FILE_ALREADY_EXISTS);
			return IL_FALSE;
		}
	}

	PnmFile = iopenw(FileName);
	if (PnmFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	PnmSize = ilSavePnmF(PnmFile);
	iclosew(PnmFile);

	if (PnmSize == 0)
		return IL_FALSE;
	return IL_TRUE;
}


//! Writes a Pnm to an already-opened file
ILuint ilSavePnmF(ILHANDLE File)
{
	ILuint Pos;
	iSetOutputFile(File);
	Pos = itellw();
	if (iSavePnmInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


//! Writes a Pnm to a memory "lump"
ILuint ilSavePnmL(void *Lump, ILuint Size)
{
	ILuint Pos;
	FName = NULL;
	iSetOutputLump(Lump, Size);
	Pos = itellw();
	if (iSavePnmInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


// Internal function used to save the Pnm.
ILboolean iSavePnmInternal()
{
	ILuint		Bpp, MaxVal = UCHAR_MAX, i = 0, j, k;
	ILenum		Type = 0;
	ILuint		LinePos = 0;  // Cannot exceed 70 for pnm's!
	ILboolean	Binary;
	ILimage		*TempImage;
	ILubyte		*TempData;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	if (iCheckExtension(FName, IL_TEXT("pbm")))
		Type = IL_PBM_ASCII;
	else if (iCheckExtension(FName, IL_TEXT("pgm")))
		Type = IL_PGM_ASCII;
	else if (iCheckExtension(FName, IL_TEXT("ppm")))
		Type = IL_PPM_ASCII;
	else
		Type = IL_PPM_ASCII;

	/*if (!Type) {
		ilSetError(IL_INVALID_EXTENSION);
		return IL_FALSE;
	}*/

	if (iGetHint(IL_COMPRESSION_HINT) == IL_USE_COMPRESSION) {
		Type += 3;
		Binary = IL_TRUE;
	}
	else {
		Binary = IL_FALSE;
	}

	if (iCurImage->Type == IL_UNSIGNED_BYTE) {
		MaxVal = UCHAR_MAX;
	}
	else if (iCurImage->Type == IL_UNSIGNED_SHORT) {
		MaxVal = USHRT_MAX;
	}
	else {
		ilSetError(IL_FORMAT_NOT_SUPPORTED);
		return IL_FALSE;
	}
	if (MaxVal > UCHAR_MAX && Type >= IL_PBM_BINARY) {  // binary cannot be higher than 255
		ilSetError(IL_FORMAT_NOT_SUPPORTED);
		return IL_FALSE;
	}

	switch (Type)
	{
		case IL_PBM_ASCII:
			Bpp = 1;
			ilprintf("P1\n");
			TempImage = iConvertImage(iCurImage, IL_LUMINANCE, IL_UNSIGNED_BYTE);
			break;
		//case IL_PBM_BINARY:  // Don't want to mess with saving bits just yet...
			//Bpp = 1;
			//ilprintf("P4\n");
			//break;
		case IL_PBM_BINARY:
			ilSetError(IL_FORMAT_NOT_SUPPORTED);
			return IL_FALSE;
		case IL_PGM_ASCII:
			Bpp = 1;
			ilprintf("P2\n");
			TempImage = iConvertImage(iCurImage, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE);
			break;
		case IL_PGM_BINARY:
			Bpp = 1;
			ilprintf("P5\n");
			TempImage = iConvertImage(iCurImage, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE);
			break;
		case IL_PPM_ASCII:
			Bpp = 3;
			ilprintf("P3\n");
			TempImage = iConvertImage(iCurImage, IL_RGB, IL_UNSIGNED_BYTE);
			break;
		case IL_PPM_BINARY:
			Bpp = 3;
			ilprintf("P6\n");
			TempImage = iConvertImage(iCurImage, IL_RGB, IL_UNSIGNED_BYTE);
			break;
		default:
			ilSetError(IL_INTERNAL_ERROR);
			return IL_FALSE;
	}

	if (TempImage == NULL)
		return IL_FALSE;

	if (Bpp != TempImage->Bpp) {
		ilSetError(IL_INVALID_CONVERSION);
		return IL_FALSE;
	}

	if (TempImage->Origin != IL_ORIGIN_UPPER_LEFT) {
		TempData = iGetFlipped(TempImage);
		if (TempData == NULL) {
			ilCloseImage(TempImage);
			return IL_FALSE;
		}
	}
	else {
		TempData = TempImage->Data;
	}

	ilprintf("%d %d\n", TempImage->Width, TempImage->Height);
	if (Type != IL_PBM_BINARY && Type != IL_PBM_ASCII)  // not needed for .pbm's (only 0 and 1)
		ilprintf("%d\n", MaxVal);

	while (i < TempImage->SizeOfPlane) {
		for (j = 0; j < Bpp; j++) {
			if (Binary) {
				if (Type == IL_PBM_BINARY) {
					iputc((ILubyte)(TempData[i] > 127 ? 1 : 0));
				}
				else {
					iputc(TempData[i]);
				}
			}
			else {
				if (TempImage->Type == IL_UNSIGNED_BYTE)
					k = TempData[i];
				else  // IL_UNSIGNED_SHORT
					k = *((ILushort*)TempData + i);
				if (Type == IL_PBM_ASCII) {
					LinePos += ilprintf("%d ", TempData[i] > 127 ? 1 : 0);
				}
				else {
					LinePos += ilprintf("%d ", TempData[i]);
				}
			}

			if (TempImage->Type == IL_UNSIGNED_SHORT)
				i++;
			i++;
		}

		if (LinePos > 65) {  // Just a good number =]
			ilprintf("\n");
			LinePos = 0;
		}
	}

	if (TempImage->Origin != IL_ORIGIN_UPPER_LEFT)
		ifree(TempData);
	ilCloseImage(TempImage);

	return IL_TRUE;
}


// Converts a .pbm to something viewable.
void PbmMaximize(ILimage *Image)
{
	ILuint i = 0;
	for (i = 0; i < Image->SizeOfPlane; i++)
		if (Image->Data[i] == 1)
			Image->Data[i] = 0xFF;
	return;
}


#endif//IL_NO_PNM
