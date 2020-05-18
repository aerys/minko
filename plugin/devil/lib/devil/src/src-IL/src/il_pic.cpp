//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_pic.cpp
//
// Description: Softimage Pic (.pic) functions
//	Lots of this code is taken from Paul Bourke's Softimage Pic code at
//	http://local.wasp.uwa.edu.au/~pbourke/dataformats/softimagepic/
//
//-----------------------------------------------------------------------------

#include "il_internal.h"
#ifndef IL_NO_PIC
#include "il_pic.h"
#include <string.h>


//! Checks if the file specified in FileName is a valid .pic file.
ILboolean ilIsValidPic(ILconst_string FileName)
{
	ILHANDLE	PicFile;
	ILboolean	bPic = IL_FALSE;

	if (!iCheckExtension(FileName, IL_TEXT("pic"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bPic;
	}

	PicFile = iopenr(FileName);
	if (PicFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bPic;
	}

	bPic = ilIsValidPicF(PicFile);
	icloser(PicFile);

	return bPic;
}


//! Checks if the ILHANDLE contains a valid .pic file at the current position.
ILboolean ilIsValidPicF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidPic();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Checks if Lump is a valid .pic lump.
ILboolean ilIsValidPicL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidPic();
}


// Internal function used to get the .pic header from the current file.
ILboolean iGetPicHead(PIC_HEAD *Header)
{
	Header->Magic = GetBigInt();
	Header->Version = GetBigFloat();
	iread(Header->Comment, 1, 80);
	iread(Header->Id, 1, 4);
	Header->Width = GetBigShort();
	Header->Height = GetBigShort();
	Header->Ratio = GetBigFloat();
	Header->Fields = GetBigShort();
	Header->Padding = GetBigShort();

	return IL_TRUE;
}


// Internal function to get the header and check it.
ILboolean iIsValidPic()
{
	PIC_HEAD	Head;

	if (!iGetPicHead(&Head))
		return IL_FALSE;
	iseek(-(ILint)sizeof(PIC_HEAD), IL_SEEK_CUR);  // Go ahead and restore to previous state

	return iCheckPic(&Head);
}


// Internal function used to check if the header is a valid .pic header.
ILboolean iCheckPic(PIC_HEAD *Header)
{
	if (Header->Magic != 0x5380F634)
		return IL_FALSE;
	if (strncmp((const char*)Header->Id, "PICT", 4))
		return IL_FALSE;
	if (Header->Width == 0)
		return IL_FALSE;
	if (Header->Height == 0)
		return IL_FALSE;

	return IL_TRUE;
}


//! Reads a .pic file
ILboolean ilLoadPic(ILconst_string FileName)
{
	ILHANDLE	PicFile;
	ILboolean	bPic = IL_FALSE;

	PicFile = iopenr(FileName);
	if (PicFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bPic;
	}

	bPic = ilLoadPicF(PicFile);
	icloser(PicFile);

	return bPic;
}


//! Reads an already-opened .pic file
ILboolean ilLoadPicF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadPicInternal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Reads from a memory "lump" that contains a .pic
ILboolean ilLoadPicL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadPicInternal();
}


// Internal function used to load the .pic
ILboolean iLoadPicInternal()
{
	ILuint		Alpha = IL_FALSE;
	ILubyte		Chained;
	CHANNEL		*Channel = NULL, *Channels = NULL, *Prev;
	PIC_HEAD	Header;
	ILboolean	Read;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	if (!iGetPicHead(&Header))
		return IL_FALSE;
	if (!iCheckPic(&Header)) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	// Read channels
	do {
		if (Channel == NULL) {
			Channel = Channels = (CHANNEL*)ialloc(sizeof(CHANNEL));
			if (Channels == NULL)
				return IL_FALSE;
		}
		else {
			Channels->Next = (CHANNEL*)ialloc(sizeof(CHANNEL));
			if (Channels->Next == NULL) {
				// Clean up the list before erroring out.
				while (Channel) {
					Prev = Channel;
					Channel = (CHANNEL*)Channel->Next;
					ifree(Prev);
				}
				return IL_FALSE;
			}
			Channels = (CHANNEL*)Channels->Next;
		}
		Channels->Next = NULL;

		Chained = igetc();
		Channels->Size = igetc();
		Channels->Type = igetc();
		Channels->Chan = igetc();
		if (ieof()) {
			Read = IL_FALSE;
			goto finish;
		}
		
		// See if we have an alpha channel in there
		if (Channels->Chan & PIC_ALPHA_CHANNEL)
			Alpha = IL_TRUE;
		
	} while (Chained);

	if (Alpha) {  // Has an alpha channel
		if (!ilTexImage(Header.Width, Header.Height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL)) {
			Read = IL_FALSE;
			goto finish;  // Have to destroy Channels first.
		}
	}
	else {  // No alpha channel
		if (!ilTexImage(Header.Width, Header.Height, 1, 3, IL_RGBA, IL_UNSIGNED_BYTE, NULL)) {
			Read = IL_FALSE;
			goto finish;  // Have to destroy Channels first.
		}
	}
	iCurImage->Origin = IL_ORIGIN_LOWER_LEFT;

	Read = readScanlines((ILuint*)iCurImage->Data, Header.Width, Header.Height, Channel, Alpha);

finish:
	// Destroy channels
	while (Channel) {
		Prev = Channel;
		Channel = (CHANNEL*)Channel->Next;
		ifree(Prev);
	}

	if (Read == IL_FALSE)
		return IL_FALSE;

	return ilFixImage();
}


ILboolean readScanlines(ILuint *image, ILint width, ILint height, CHANNEL *channel, ILuint alpha)
{
	ILint	i;
	ILuint	*scan;

	(void)alpha;
	
	for (i = height - 1; i >= 0; i--) {
		scan = image + i * width;

		if (!readScanline((ILubyte *)scan, width, channel, alpha ? 4 : 3)) {
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			return IL_FALSE;
		}
	}

	return IL_TRUE;
}


ILuint readScanline(ILubyte *scan, ILint width, CHANNEL *channel, ILint bytes)
{
	ILint		noCol;
	ILint		off[4];
	ILuint		status=0;

	while (channel) {
		noCol = 0;
		if(channel->Chan & PIC_RED_CHANNEL) {
			off[noCol] = 0;
			noCol++;
		}
		if(channel->Chan & PIC_GREEN_CHANNEL) {
			off[noCol] = 1;
			noCol++;
		}
		if(channel->Chan & PIC_BLUE_CHANNEL) {
			off[noCol] = 2;
			noCol++;
		}
		if(channel->Chan & PIC_ALPHA_CHANNEL) {
			off[noCol] = 3;
			noCol++;
			//@TODO: Find out if this is possible.
			if (bytes == 3)  // Alpha channel in a 24-bit image.  Do not know what to do with this.
				return 0;
		}

		switch(channel->Type & 0x0F)
		{
			case PIC_UNCOMPRESSED:
				status = channelReadRaw(scan, width, noCol, off, bytes);
				break;
			case PIC_PURE_RUN_LENGTH:
				status = channelReadPure(scan, width, noCol, off, bytes);
				break;
			case PIC_MIXED_RUN_LENGTH:
				status = channelReadMixed(scan, width, noCol, off, bytes);
				break;
		}
		if (!status)
			break;

		channel = (CHANNEL*)channel->Next;
	}
	return status;
}


ILboolean channelReadRaw(ILubyte *scan, ILint width, ILint noCol, ILint *off, ILint bytes)
{
	ILint i, j;

	for (i = 0; i < width; i++) {
		if (ieof())
			return IL_FALSE;
		for (j = 0; j < noCol; j++)
			if (iread(&scan[off[j]], 1, 1) != 1)
				return IL_FALSE;
		scan += bytes;
	}
	return IL_TRUE;
}


ILboolean channelReadPure(ILubyte *scan, ILint width, ILint noCol, ILint *off, ILint bytes)
{
	ILubyte		col[4];
	ILint		count;
	int			i, j, k;

	for (i = width; i > 0; ) {
		count = igetc();
		if (count == IL_EOF)
			return IL_FALSE;
		if (count > width)
			count = width;
		i -= count;
		
		if (ieof())
			return IL_FALSE;
		
		for (j = 0; j < noCol; j++)
			if (iread(&col[j], 1, 1) != 1)
				return IL_FALSE;
		
		for (k = 0; k < count; k++, scan += bytes) {
			for(j = 0; j < noCol; j++)
				scan[off[j] + k] = col[j];
		}
	}
	return IL_TRUE;
}


ILboolean channelReadMixed(ILubyte *scan, ILint width, ILint noCol, ILint *off, ILint bytes)
{
	ILint	count;
	int		i, j, k;
	ILubyte	col[4];

	for(i = 0; i < width; i += count) {
		if (ieof())
			return IL_FALSE;

		count = igetc();
		if (count == IL_EOF)
			return IL_FALSE;

		if (count >= 128) {  // Repeated sequence
			if (count == 128) {  // Long run
				count = GetBigUShort();
				if (ieof()) {
					ilSetError(IL_FILE_READ_ERROR);
					return IL_FALSE;
				}
			}
			else
				count -= 127;
			
			// We've run past...
			if ((i + count) > width) {
				//fprintf(stderr, "ERROR: FF_PIC_load(): Overrun scanline (Repeat) [%d + %d > %d] (NC=%d)\n", i, count, width, noCol);
				ilSetError(IL_ILLEGAL_FILE_VALUE);
				return IL_FALSE;
			}

			for (j = 0; j < noCol; j++)
				if (iread(&col[j], 1, 1) != 1) {
					ilSetError(IL_FILE_READ_ERROR);
					return IL_FALSE;
				}

			for (k = 0; k < count; k++, scan += bytes) {
				for (j = 0; j < noCol; j++)
					scan[off[j]] = col[j];
			}
		} else {				// Raw sequence
			count++;
			if ((i + count) > width) {
				//fprintf(stderr, "ERROR: FF_PIC_load(): Overrun scanline (Raw) [%d + %d > %d] (NC=%d)\n", i, count, width, noCol);
				ilSetError(IL_ILLEGAL_FILE_VALUE);
				return IL_FALSE;
			}
			
			for (k = count; k > 0; k--, scan += bytes) {
				for (j = 0; j < noCol; j++)
					if (iread(&scan[off[j]], 1, 1) != 1) {
						ilSetError(IL_FILE_READ_ERROR);
						return IL_FALSE;
					}
			}
		}
	}

	return IL_TRUE;
}


#endif//IL_NO_PIC

