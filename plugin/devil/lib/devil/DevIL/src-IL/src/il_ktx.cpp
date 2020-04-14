//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2016 by Denton Woods
// Last modified: 05/15/2016
//
// Filename: src-IL/src/il_ktx.cpp
//
// Description: Reads a Khronos Texture .ktx file
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_KTX
#include "il_bits.h"


ILboolean	iIsValidKtx(void);
ILboolean	iLoadKtxInternal();


#ifdef _MSC_VER
#pragma pack(push, packed_struct, 1)
#endif
typedef struct KTX_HEAD
{
	ILubyte	identifier[12];
	ILuint	endianness;
	ILuint	glType;
	ILuint	glTypeSize;
	ILuint	glFormat;
	ILuint	glInternalFormat;
	ILuint	glBaseInternalFormat;
	ILuint	pixelWidth;
	ILuint	pixelHeight;
	ILuint	pixelDepth;
	ILuint	numberOfArrayElements;
	ILuint	numberOfFaces;
	ILuint	numberOfMipmapLevels;
	ILuint	bytesOfKeyValueData;
} IL_PACKSTRUCT KTX_HEAD;
#ifdef _MSC_VER
#pragma pack(pop,  packed_struct)
#endif


// From GL/GL.h
#define I_GL_BYTE                           0x1400
#define I_GL_UNSIGNED_BYTE                  0x1401
#define I_GL_SHORT                          0x1402
#define I_GL_UNSIGNED_SHORT                 0x1403
#define I_GL_INT                            0x1404
#define I_GL_UNSIGNED_INT                   0x1405
#define I_GL_FLOAT                          0x1406
//#define I_GL_2_BYTES                        0x1407
//#define I_GL_3_BYTES                        0x1408
//#define I_GL_4_BYTES                        0x1409
#define I_GL_DOUBLE                         0x140A
#define I_GL_ALPHA                          0x1906
#define I_GL_RGB                            0x1907
#define I_GL_RGBA                           0x1908
#define I_GL_LUMINANCE                      0x1909
#define I_GL_LUMINANCE_ALPHA                0x190A



ILboolean ilIsValidKtx(ILconst_string FileName)
{
	ILHANDLE	KtxFile;
	ILboolean	bKtx = IL_FALSE;

	if (!iCheckExtension(FileName, IL_TEXT("ktx"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bKtx;
	}

	KtxFile = iopenr(FileName);
	if (KtxFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bKtx;
	}

	bKtx = ilIsValidKtxF(KtxFile);
	icloser(KtxFile);

	return bKtx;
}


ILboolean ilIsValidKtxF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidKtx();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


ILboolean ilIsValidKtxL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidKtx();
}


ILboolean iIsValidKtx()
{
	ILubyte 	Signature[8];
	ILint		Read;

	/*Read = iread(Signature, 1, 8);
	iseek(-Read, IL_SEEK_CUR);*/

	return IL_FALSE;
}


//! Reads a .ktx file
ILboolean ilLoadKtx(ILconst_string FileName)
{
	ILHANDLE	KtxFile;
	ILboolean	bKtx = IL_FALSE;

	KtxFile = iopenr(FileName);
	if (KtxFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bKtx;
	}

	bKtx = ilLoadKtxF(KtxFile);
	icloser(KtxFile);

	return bKtx;
}


//! Reads an already-opened .ktx file
ILboolean ilLoadKtxF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadKtxInternal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Reads from a memory "lump" that contains a .ktx
ILboolean ilLoadKtxL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadKtxInternal();
}


// Note:  .Ktx support has not been tested yet!
//  https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
ILboolean iLoadKtxInternal()
{
	KTX_HEAD	Header;
	ILuint		imageSize;
	ILenum		Format;
	ILubyte		Bpp;
	char		FileIdentifier[12] = {
		//0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
		'«', 'K', 'T', 'X', ' ', '1', '1', '»', '\r', '\n', '\x1A', '\n'
	};

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	/*Header.Width = GetLittleShort();
	Header.Height = GetLittleShort();
	Header.Dummy = GetLittleInt();*/

	if (iread(Header.identifier, 1, 12) != 12)
		return IL_FALSE;
	Header.endianness = GetLittleUInt();
	Header.glType = GetLittleUInt();
	Header.glTypeSize = GetLittleUInt();
	Header.glFormat = GetLittleUInt();
	Header.glInternalFormat = GetLittleUInt();
	Header.glBaseInternalFormat = GetLittleUInt();
	Header.pixelWidth = GetLittleUInt();
	Header.pixelHeight = GetLittleUInt();
	Header.pixelDepth = GetLittleUInt();
	Header.numberOfArrayElements = GetLittleUInt();
	Header.numberOfFaces = GetLittleUInt();
	Header.numberOfMipmapLevels = GetLittleUInt();
	Header.bytesOfKeyValueData = GetLittleUInt();


	if (memcmp(Header.identifier, FileIdentifier, 12) || Header.endianness != 0x04030201) {
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		return IL_FALSE;
	}
	//@TODO: Additional types
	if (Header.glType != I_GL_UNSIGNED_BYTE || Header.glTypeSize != 1) {
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		return IL_FALSE;
	}
	//@TODO: Additional formats
	if (Header.glFormat <= I_GL_ALPHA || Header.glFormat >= I_GL_LUMINANCE_ALPHA || Header.glInternalFormat != Header.glFormat /*|| Header.glBaseInternalFormat != Header.glFormat*/) {
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		return IL_FALSE;
	}

	//@TODO: Mipmaps, etc.
	if (Header.numberOfArrayElements != 0 || Header.numberOfFaces != 1 || Header.numberOfMipmapLevels != 1) {
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		return IL_FALSE;
	}
	//@TODO: Parse this data
	if (iseek(Header.bytesOfKeyValueData, IL_SEEK_CUR))
		return IL_FALSE;

	switch (Header.glFormat)
	{
		case I_GL_LUMINANCE:
			Bpp = 1;
			Format = IL_LUMINANCE;
			break;
		case IL_LUMINANCE_ALPHA:
			Bpp = 2;
			Format = IL_LUMINANCE_ALPHA;
			break;
		case I_GL_RGB:
			Bpp = 3;
			Format = IL_RGB;
			break;
		case I_GL_RGBA:
			Bpp = 4;
			Format = IL_RGBA;
			break;
		default:
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			return IL_FALSE;
	}

	//@TODO: More than just RGBA
	if (!ilTexImage(Header.pixelWidth, Header.pixelHeight, 1, Bpp, Format, IL_UNSIGNED_BYTE, NULL)) {
		return IL_FALSE;
	}
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;

	imageSize = GetLittleUInt();
	if (imageSize != Header.pixelWidth * Header.pixelHeight * Bpp) {
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		return IL_FALSE;
	}

	if (iread(iCurImage->Data, Bpp, Header.pixelWidth * Header.pixelHeight) != Header.pixelWidth * Header.pixelHeight)
		return IL_FALSE;

	return ilFixImage();
}


#endif//IL_NO_KTX
