//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2001-2009 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_icns.c
//
// Description: Reads from a Mac OS X icon (.icns) file.
//		Credit for the format of .icns files goes to
//		http://www.macdisk.com/maciconen.php3 and
//		http://ezix.org/project/wiki/MacOSXIcons
//
//-----------------------------------------------------------------------------

//@TODO: Put ilSetError calls in when errors occur.
//@TODO: Should we clear the alpha channel just in case there isn't one in the file?
//@TODO: Checks on iread

#include "il_internal.h"
#ifndef IL_NO_ICNS
#include "il_icns.h"

#ifndef IL_NO_JP2
	#if defined(_WIN32) && defined(IL_USE_PRAGMA_LIBS)
		#if defined(_MSC_VER) || defined(__BORLANDC__)
			#ifndef _DEBUG
				#pragma comment(lib, "libjasper.lib")
			#else
				#pragma comment(lib, "libjasper-d.lib")
			#endif
		#endif
	#endif
#endif//IL_NO_JP2


//! Checks if the file specified in FileName is a valid .icns file.
ILboolean ilIsValidIcns(ILconst_string FileName)
{
	ILHANDLE	IcnsFile;
	ILboolean	bIcns = IL_FALSE;

	if (!iCheckExtension(FileName, IL_TEXT("icns"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bIcns;
	}

	IcnsFile = iopenr(FileName);
	if (IcnsFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bIcns;
	}

	bIcns = ilIsValidIcnsF(IcnsFile);
	icloser(IcnsFile);

	return bIcns;
}


//! Checks if the ILHANDLE contains a valid .icns file at the current position.
ILboolean ilIsValidIcnsF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidIcns();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Checks if Lump is a valid .icns lump.
ILboolean ilIsValidIcnsL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidIcns();
}


// Internal function to get the header and check it.
ILboolean iIsValidIcns()
{
	ICNSHEAD	Header;

	iread(Header.Head, 1, 4);
	iseek(-4, IL_SEEK_CUR);  // Go ahead and restore to previous state

	if (strncmp(Header.Head, "icns", 4))  // First 4 bytes have to be 'icns'.
		return IL_FALSE;

	return IL_TRUE;
}


//! Reads an icon file.
ILboolean ilLoadIcns(ILconst_string FileName)
{
	ILHANDLE	IcnsFile;
	ILboolean	bIcns = IL_FALSE;

	IcnsFile = iopenr(FileName);
	if (IcnsFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bIcns;
	}

	bIcns = ilLoadIcnsF(IcnsFile);
	icloser(IcnsFile);

	return bIcns;
}


//! Reads an already-opened icon file.
ILboolean ilLoadIcnsF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadIcnsInternal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Reads from a memory "lump" that contains an icon.
ILboolean ilLoadIcnsL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadIcnsInternal();
}


// Internal function used to load the icon.
ILboolean iLoadIcnsInternal()
{
	ICNSHEAD	Header;
	ICNSDATA	Entry;
	ILimage		*Image = NULL;
	ILboolean	BaseCreated = IL_FALSE;


	if (iCurImage == NULL)
	{
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	iread(Header.Head, 4, 1);
	Header.Size = GetBigInt();

	if (strncmp(Header.Head, "icns", 4))  // First 4 bytes have to be 'icns'.
		return IL_FALSE;

	while ((ILint)itell() < Header.Size && !ieof())
	{
		iread(Entry.ID, 4, 1);
		Entry.Size = GetBigInt();

		if (!strncmp(Entry.ID, "it32", 4))  // 128x128 24-bit
		{
			if (iIcnsReadData(&BaseCreated, IL_FALSE, 128, &Entry, &Image) == IL_FALSE)
				goto icns_error;
		}
		else if (!strncmp(Entry.ID, "t8mk", 4))  // 128x128 alpha mask
		{
			if (iIcnsReadData(&BaseCreated, IL_TRUE, 128, &Entry, &Image) == IL_FALSE)
				goto icns_error;
		}
		else if (!strncmp(Entry.ID, "ih32", 4))  // 48x48 24-bit
		{
			if (iIcnsReadData(&BaseCreated, IL_FALSE, 48, &Entry, &Image) == IL_FALSE)
				goto icns_error;
		}
		else if (!strncmp(Entry.ID, "h8mk", 4))  // 48x48 alpha mask
		{
			if (iIcnsReadData(&BaseCreated, IL_TRUE, 48, &Entry, &Image) == IL_FALSE)
				goto icns_error;
		}
		else if (!strncmp(Entry.ID, "il32", 4))  // 32x32 24-bit
		{
			if (iIcnsReadData(&BaseCreated, IL_FALSE, 32, &Entry, &Image) == IL_FALSE)
				goto icns_error;
		}
		else if (!strncmp(Entry.ID, "l8mk", 4))  // 32x32 alpha mask
		{
			if (iIcnsReadData(&BaseCreated, IL_TRUE, 32, &Entry, &Image) == IL_FALSE)
				goto icns_error;
		}
		else if (!strncmp(Entry.ID, "is32", 4))  // 16x16 24-bit
		{
			if (iIcnsReadData(&BaseCreated, IL_FALSE, 16, &Entry, &Image) == IL_FALSE)
				goto icns_error;
		}
		else if (!strncmp(Entry.ID, "s8mk", 4))  // 16x16 alpha mask
		{
			if (iIcnsReadData(&BaseCreated, IL_TRUE, 16, &Entry, &Image) == IL_FALSE)
				goto icns_error;
		}
#ifndef IL_NO_JP2
		else if (!strncmp(Entry.ID, "ic09", 4))  // 512x512 JPEG2000 encoded - Uses JasPer
		{
			if (iIcnsReadData(&BaseCreated, IL_FALSE, 512, &Entry, &Image) == IL_FALSE)
				goto icns_error;
		}
		else if (!strncmp(Entry.ID, "ic08", 4))  // 256x256 JPEG2000 encoded - Uses JasPer
		{
			if (iIcnsReadData(&BaseCreated, IL_FALSE, 256, &Entry, &Image) == IL_FALSE)
				goto icns_error;
		}
#endif//IL_NO_JP2
		else  // Not a valid format or one that we can use
		{
			iseek(Entry.Size - 8, IL_SEEK_CUR);
		}
	}

	return ilFixImage();

icns_error:
	return IL_FALSE;
}

ILboolean iIcnsReadData(ILboolean *BaseCreated, ILboolean IsAlpha, ILint Width, ICNSDATA *Entry, ILimage **Image)
{
	ILint		Position = 0, RLEPos = 0, Channel, i;
	ILubyte		RLERead, *Data = NULL;
	ILimage		*TempImage = NULL;
	ILboolean	ImageAlreadyExists = IL_FALSE;

	// The .icns format stores the alpha and RGB as two separate images, so this
	//  checks to see if one exists for that particular size.  Unfortunately,
	//	there is no guarantee that they are in any particular order.
	if (*BaseCreated && iCurImage != NULL)
	{
		TempImage = iCurImage;
		while (TempImage != NULL)
		{
			if ((ILuint)Width == TempImage->Width)
			{
				ImageAlreadyExists = IL_TRUE;
				break;
			}
			TempImage = TempImage->Next;
		}
	}

	Data = ialloc(Entry->Size - 8);
	if (Data == NULL)
		return IL_FALSE;

	if (!ImageAlreadyExists)
	{
		if (!*BaseCreated)  // Create base image
		{
			ilTexImage(Width, Width, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
			iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;
			*Image = iCurImage;
			*BaseCreated = IL_TRUE;
		}
		else  // Create next image in list
		{
			(*Image)->Next = ilNewImage(Width, Width, 1, 4, 1);
			*Image = (*Image)->Next;
			(*Image)->Format = IL_RGBA;
			(*Image)->Origin = IL_ORIGIN_UPPER_LEFT;
		}

		TempImage = *Image;
	}

	if (IsAlpha)  // Alpha is never compressed.
	{
		iread(Data, Entry->Size - 8, 1);  // Size includes the header
		if (Entry->Size - 8 != Width * Width)
		{
			ifree(Data);
			return IL_FALSE;
		}

		for (i = 0; i < Width * Width; i++)
		{
			TempImage->Data[(i * 4) + 3] = Data[i];
		}
	}
	else if (Width == 256 || Width == 512)  // JPEG2000 encoded - uses JasPer
	{
#ifndef IL_NO_JP2
		iread(Data, Entry->Size - 8, 1);  // Size includes the header
		if (ilLoadJp2LInternal(Data, Entry->Size - 8, TempImage) == IL_FALSE)
		{
			ifree(Data);
			ilSetError(IL_LIB_JP2_ERROR);
			return IL_TRUE;
		}
#else  // Cannot handle this size.
		ilSetError(IL_LIB_JP2_ERROR);  //@TODO: Handle this better...just skip the data.
		return IL_FALSE;
#endif//IL_NO_JP2
	}
	else  // RGB data
	{
		iread(Data, Entry->Size - 8, 1);  // Size includes the header
		if (Width == 128)
			RLEPos += 4;  // There are an extra 4 bytes here of zeros.
		//@TODO: Should we check to make sure they are all 0?

		if (Entry->Size - 8 == Width * Width * 4) // Uncompressed
		{
			//memcpy(TempImage->Data, Data, Entry->Size);
			for (i = 0; i < Width * Width; i++, Position += 4)
			{
				TempImage->Data[i * 4 + 0] = Data[Position+1];
				TempImage->Data[i * 4 + 1] = Data[Position+2];
				TempImage->Data[i * 4 + 2] = Data[Position+3];
			}
		}
		else  // RLE decoding
		{
			for (Channel = 0; Channel < 3; Channel++)
			{
				Position = 0;
				while (Position < Width * Width)
				{
					RLERead = Data[RLEPos];
					RLEPos++;

					if (RLERead >= 128)
					{
						for (i = 0; i < RLERead - 125 && (Position + i) < Width * Width; i++)
						{
							TempImage->Data[Channel + (Position + i) * 4] = Data[RLEPos];
						}
						RLEPos++;
						Position += RLERead - 125;
					}
					else
					{
						for (i = 0; i < RLERead + 1 && (Position + i) < Width * Width; i++)
						{
							TempImage->Data[Channel + (Position + i) * 4] = Data[RLEPos + i];
						}
						RLEPos += RLERead + 1;
						Position += RLERead + 1;
					}
				}
			}
		}
	}

	ifree(Data);
	return IL_TRUE;
}

#endif//IL_NO_ICNS
