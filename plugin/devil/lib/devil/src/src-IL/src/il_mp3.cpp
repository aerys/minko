//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/05/2009
//
// Filename: src-IL/src/il_mp3.cpp
//
// MimeType: Reads from an MPEG-1 Audio Layer 3 (.mp3) file.
//
//-----------------------------------------------------------------------------

#include "il_internal.h"
#ifndef IL_NO_MP3

typedef struct MP3HEAD
{
	char	Signature[3];
	ILubyte	VersionMajor;
	ILubyte	VersionMinor;
	ILubyte	Flags;
	ILuint	Length;
} MP3HEAD;

#define MP3_NONE 0
#define MP3_JPG  1
#define MP3_PNG  2

ILboolean iLoadMp3Internal(void);
ILboolean iIsValidMp3(void);
ILboolean iCheckMp3(MP3HEAD *Header);
ILboolean iLoadJpegInternal(void);
ILboolean iLoadPngInternal(void);


//! Checks if the file specified in FileName is a valid MP3 file.
ILboolean ilIsValidMp3(ILconst_string FileName)
{
	ILHANDLE	Mp3File;
	ILboolean	bMp3 = IL_FALSE;
	
	if (!iCheckExtension(FileName, IL_TEXT("mp3"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bMp3;
	}
	
	Mp3File = iopenr(FileName);
	if (Mp3File == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bMp3;
	}
	
	bMp3 = ilIsValidMp3F(Mp3File);
	icloser(Mp3File);
	
	return bMp3;
}


//! Checks if the ILHANDLE contains a valid MP3 file at the current position.
ILboolean ilIsValidMp3F(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;
	
	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidMp3();
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Checks if Lump is a valid MP3 lump.
ILboolean ilIsValidMp3L(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidMp3();
}


ILuint GetSynchInt()
{
	ILuint SynchInt;

	SynchInt = GetBigUInt();

	SynchInt = ((SynchInt & 0x7F000000) >> 3) | ((SynchInt & 0x7F0000) >> 2)
				| ((SynchInt & 0x7F00) >> 1) | (SynchInt & 0x7F);

	return SynchInt;
}


// Internal function used to get the MP3 header from the current file.
ILboolean iGetMp3Head(MP3HEAD *Header)
{
	if (iread(Header->Signature, 3, 1) != 1)
		return IL_FALSE;
	Header->VersionMajor = igetc();
	Header->VersionMinor = igetc();
	Header->Flags = igetc();
	Header->Length = GetSynchInt();

	return IL_TRUE;
}


// Internal function to get the header and check it.
ILboolean iIsValidMp3(void)
{
	MP3HEAD		Header;
	ILuint		Pos = itell();

	if (!iGetMp3Head(&Header))
		return IL_FALSE;
	// The length of the header varies, so we just go back to the original position.
	iseek(Pos, IL_SEEK_CUR);

	return iCheckMp3(&Header);
}


// Internal function used to check if the HEADER is a valid MP3 header.
ILboolean iCheckMp3(MP3HEAD *Header)
{
	if (strncmp(Header->Signature, "ID3", 3))
		return IL_FALSE;
	if (Header->VersionMajor != 3 && Header->VersionMinor != 4)
		return IL_FALSE;

	return IL_TRUE;
}


ILuint iFindMp3Pic(MP3HEAD *Header)
{
	char	ID[4];
	ILuint	FrameSize;
	ILubyte	TextEncoding;
	ILubyte	MimeType[65], Description[65];
	ILubyte	PicType;
	ILuint	i;
	ILuint	Type = MP3_NONE;

	do {
		if (iread(ID, 4, 1) != 1)
			return MP3_NONE;
		if (Header->VersionMajor == 3)
			FrameSize = GetBigUInt();
		else
			FrameSize = GetSynchInt();

		GetBigUShort();  // Skip the flags.

		//@TODO: Support multiple APIC entries in an mp3 file.
		if (!strncmp(ID, "APIC", 4)) {
			//@TODO: Use TextEncoding properly - UTF16 strings starting with FFFE or FEFF.
			TextEncoding = igetc();
			// Get the MimeType (read until we hit 0).
			for (i = 0; i < 65; i++) {
				MimeType[i] = igetc();
				if (MimeType[i] == 0)
					break;
			}
			// The MimeType must be terminated by 0 in the file by the specs.
			if (MimeType[i] != 0)
				return MP3_NONE;
			if (!strcmp((char*)MimeType, "image/jpeg"))
				Type = MP3_JPG;
			else if (!strcmp((char*)MimeType, "image/png"))
				Type = MP3_PNG;
			else
				Type = MP3_NONE;

			PicType = igetc();  // Whether this is a cover, band logo, etc.

			// Skip the description.
			for (i = 0; i < 65; i++) {
				Description[i] = igetc();
				if (Description[i] == 0)
					break;
			}
			if (Description[i] != 0)
				return MP3_NONE;
			return Type;
		}
		else {
			iseek(FrameSize, IL_SEEK_CUR);
		}

		//if (!strncmp(MimeType, "
	} while (!ieof() && itell() < Header->Length);

	return Type;
}


//! Reads a MP3 file
ILboolean ilLoadMp3(ILconst_string FileName)
{
	ILHANDLE	Mp3File;
	ILboolean	bMp3 = IL_FALSE;

	Mp3File = iopenr(FileName);
	if (Mp3File == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bMp3;
	}

	bMp3 = ilLoadMp3F(Mp3File);
	icloser(Mp3File);

	return bMp3;
}


//! Reads an already-opened MP3 file
ILboolean ilLoadMp3F(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadMp3Internal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Reads from a memory "lump" that contains a MP3
ILboolean ilLoadMp3L(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadMp3Internal();
}


// Internal function used to load the MP3.
ILboolean iLoadMp3Internal(void)
{
	MP3HEAD	Header;
	ILuint	Type;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	if (!iGetMp3Head(&Header))
		return IL_FALSE;
	if (!iCheckMp3(&Header))
		return IL_FALSE;
	Type = iFindMp3Pic(&Header);
	
	switch (Type)
	{
#ifndef IL_NO_JPG
		case MP3_JPG:
			return iLoadJpegInternal();
#endif//IL_NO_JPG

#ifndef IL_NO_PNG
		case MP3_PNG:
			return iLoadPngInternal();
#endif//IL_NO_PNG

		// Either a picture was not found, or the MIME type was not recognized.
		default:
			ilSetError(IL_INVALID_FILE_HEADER);
	}

	return IL_FALSE;
}

#endif//IL_NO_MP3

