//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 02/09/2009
//
// Filename: src-IL/src/il_dds-save.cpp
//
// Description: Saves a DirectDraw Surface (.dds) file.
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#include "il_dds.h"
#include <limits.h>


#ifndef IL_NO_DDS

//! Writes a Dds file
ILboolean ilSaveDds(const ILstring FileName)
{
	ILHANDLE	DdsFile;
	ILuint		DdsSize;

	if (ilGetBoolean(IL_FILE_MODE) == IL_FALSE) {
		if (iFileExists(FileName)) {
			ilSetError(IL_FILE_ALREADY_EXISTS);
			return IL_FALSE;
		}
	}

	DdsFile = iopenw(FileName);
	if (DdsFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	DdsSize = ilSaveDdsF(DdsFile);
	iclosew(DdsFile);

	if (DdsSize == 0)
		return IL_FALSE;
	return IL_TRUE;
}


//! Writes a Dds to an already-opened file
ILuint ilSaveDdsF(ILHANDLE File)
{
	ILuint Pos;
	iSetOutputFile(File);
	Pos = itellw();
	if (iSaveDdsInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


//! Writes a Dds to a memory "lump"
ILuint ilSaveDdsL(void *Lump, ILuint Size)
{
	ILuint Pos;
	iSetOutputLump(Lump, Size);
	Pos = itellw();
	if (iSaveDdsInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


//! Checks if an image is a cubemap
ILuint GetCubemapInfo(ILimage* image, ILint* faces)
{
	ILint	indices[] = { -1, -1, -1,  -1, -1, -1 }, i;
	ILimage	*img;
	ILuint	ret = 0, srcMipmapCount, srcImagesCount, mipmapCount;

	if (image == NULL)
		return 0;

	iGetIntegervImage(image, IL_NUM_IMAGES, (ILint*) &srcImagesCount);
	if (srcImagesCount != 5) //write only complete cubemaps (TODO?)
		return 0;

	img = image;
	iGetIntegervImage(image, IL_NUM_MIPMAPS, (ILint*) &srcMipmapCount);
	mipmapCount = srcMipmapCount;

	for (i = 0; i < 6; ++i) {
		switch (img->CubeFlags)
		{
			case DDS_CUBEMAP_POSITIVEX:
				indices[i] = 0;
				break;
			case DDS_CUBEMAP_NEGATIVEX:
				indices[i] = 1;
				break;
			case DDS_CUBEMAP_POSITIVEY:
				indices[i] = 2;
				break;
			case DDS_CUBEMAP_NEGATIVEY:
				indices[i] = 3;
				break;
			case DDS_CUBEMAP_POSITIVEZ:
				indices[i] = 4;
				break;
			case DDS_CUBEMAP_NEGATIVEZ:
				indices[i] = 5;
				break;
		}
        iGetIntegervImage(img, IL_NUM_MIPMAPS, (ILint*) &srcMipmapCount);
		if (srcMipmapCount != mipmapCount)
			return 0; //equal # of mipmaps required

		ret |= img->CubeFlags;
		img = img->Next;
	}

	for (i = 0; i < 6; ++i)
		if (indices[i] == -1)
			return 0; //one face not found

	if (ret != 0) //should always be true
		ret |= DDS_CUBEMAP;

	for (i = 0; i < 6; ++i)
		faces[indices[i]] = i;

	return ret;
}


// Internal function used to save the Dds.
ILboolean iSaveDdsInternal()
{
	ILenum	DXTCFormat;
	ILuint	counter, numMipMaps, image, numFaces, i;
	ILubyte	*CurData = NULL;
	ILint	CubeTable[6] = { 0 };
	ILuint	CubeFlags;

	CubeFlags = GetCubemapInfo(iCurImage, CubeTable);

	image = ilGetInteger(IL_CUR_IMAGE);
	DXTCFormat = iGetInt(IL_DXTC_FORMAT);
	WriteHeader(iCurImage, DXTCFormat, CubeFlags);

	if (CubeFlags != 0)
		numFaces = ilGetInteger(IL_NUM_FACES); // Should always be 5 for now
	else
		numFaces = 0;

	numMipMaps = ilGetInteger(IL_NUM_MIPMAPS); //this assumes all faces have same # of mipmaps

	for (i = 0; i <= numFaces; ++i) {
		for (counter = 0; counter <= numMipMaps; counter++) {
			ilBindImage(image);
			ilActiveImage(CubeTable[i]);
			ilActiveMipmap(counter);

			if (iCurImage->Origin != IL_ORIGIN_UPPER_LEFT) {
				CurData = iCurImage->Data;
				iCurImage->Data = iGetFlipped(iCurImage);
				if (iCurImage->Data == NULL) {
					iCurImage->Data = CurData;
					return IL_FALSE;
				}
			}

			if (!Compress(iCurImage, DXTCFormat))
				return IL_FALSE;

			if (iCurImage->Origin != IL_ORIGIN_UPPER_LEFT) {
				ifree(iCurImage->Data);
				iCurImage->Data = CurData;
			}
		}

	}

	return IL_TRUE;
}


// @TODO:  Finish this, as it is incomplete.
ILboolean WriteHeader(ILimage *Image, ILenum DXTCFormat, ILuint CubeFlags)
{
	ILuint i, FourCC, Flags1 = 0, Flags2 = 0, ddsCaps1 = 0,
	LinearSize, BlockSize, ddsCaps2 = 0;

	Flags1 |= DDS_LINEARSIZE | DDS_MIPMAPCOUNT 
			| DDS_WIDTH | DDS_HEIGHT | DDS_CAPS | DDS_PIXELFORMAT;
	Flags2 |= DDS_FOURCC;

	if (Image->Depth > 1)
		Flags1 |= DDS_DEPTH;

	// @TODO:  Fix the pre-multiplied alpha problem.
	if (DXTCFormat == IL_DXT2)
		DXTCFormat = IL_DXT3;
	else if (DXTCFormat == IL_DXT4)
		DXTCFormat = IL_DXT5;

	switch (DXTCFormat)
	{
		case IL_DXT1:
		case IL_DXT1A:
			FourCC = IL_MAKEFOURCC('D','X','T','1');
			break;
		case IL_DXT2:
			FourCC = IL_MAKEFOURCC('D','X','T','2');
			break;
		case IL_DXT3:
			FourCC = IL_MAKEFOURCC('D','X','T','3');
			break;
		case IL_DXT4:
			FourCC = IL_MAKEFOURCC('D','X','T','4');
			break;
		case IL_DXT5:
			FourCC = IL_MAKEFOURCC('D','X','T','5');
			break;
		case IL_ATI1N:
			FourCC = IL_MAKEFOURCC('A', 'T', 'I', '1');
			break;
		case IL_3DC:
			FourCC = IL_MAKEFOURCC('A','T','I','2');
			break;
		case IL_RXGB:
			FourCC = IL_MAKEFOURCC('R','X','G','B');
			break;
		default:
			// Error!
			ilSetError(IL_INTERNAL_ERROR);  // Should never happen, though.
			return IL_FALSE;
	}

	iwrite("DDS ", 1, 4);
	SaveLittleUInt(124);		// Size1
	SaveLittleUInt(Flags1);		// Flags1
	SaveLittleUInt(Image->Height);
	SaveLittleUInt(Image->Width);

	if (DXTCFormat == IL_DXT1 || DXTCFormat == IL_DXT1A || DXTCFormat == IL_ATI1N) {
		BlockSize = 8;
	}
	else {
		BlockSize = 16;
	}
	LinearSize = (((Image->Width + 3)/4) * ((Image->Height + 3)/4)) * BlockSize * Image->Depth;

	/*
	// doing this is actually wrong, linear size is only size of one cube face
	if (CubeFlags != 0) {
		ILint numFaces = 0;
		for (i = 0; i < 6; ++i)
			if (CubeFlags & CubemapDirections[i])
				++numFaces;

		LinearSize *= numFaces;
	}
	*/

	SaveLittleUInt(LinearSize);	// LinearSize (TODO: change this when uncompressed formats are supported)

	if (Image->Depth > 1) {
		SaveLittleUInt(Image->Depth);			// Depth
		ddsCaps2 |= DDS_VOLUME;
	}
	else
		SaveLittleUInt(0);						// Depth

	SaveLittleUInt(ilGetInteger(IL_NUM_MIPMAPS) + 1);  // MipMapCount
	SaveLittleUInt(0);			// AlphaBitDepth

	for (i = 0; i < 10; i++)
		SaveLittleUInt(0);		// Not used

	SaveLittleUInt(32);			// Size2
	SaveLittleUInt(Flags2);		// Flags2
	SaveLittleUInt(FourCC);		// FourCC
	SaveLittleUInt(0);			// RGBBitCount
	SaveLittleUInt(0);			// RBitMask
	SaveLittleUInt(0);			// GBitMask
	SaveLittleUInt(0);			// BBitMask
	SaveLittleUInt(0);			// RGBAlphaBitMask
	ddsCaps1 |= DDS_TEXTURE;
	//changed 20040516: set mipmap flag on mipmap images
	//(non-compressed .dds files still not supported,
	//though)
	if (ilGetInteger(IL_NUM_MIPMAPS) > 0)
		ddsCaps1 |= DDS_MIPMAP | DDS_COMPLEX;
	if (CubeFlags != 0) {
		ddsCaps1 |= DDS_COMPLEX;
		ddsCaps2 |= CubeFlags;
	}

	SaveLittleUInt(ddsCaps1);	// ddsCaps1

	SaveLittleUInt(ddsCaps2);	// ddsCaps2
	SaveLittleUInt(0);			// ddsCaps3
	SaveLittleUInt(0);			// ddsCaps4
	SaveLittleUInt(0);			// TextureStage

	return IL_TRUE;
}

#endif//IL_NO_DDS


ILuint ILAPIENTRY ilGetDXTCData(void *Buffer, ILuint BufferSize, ILenum DXTCFormat)
{
	ILubyte	*CurData = NULL;
	ILuint	retVal;
	ILint	BlockNum;

	if (Buffer == NULL) {  // Return the number that will be written with a subsequent call.
		BlockNum = ((iCurImage->Width + 3)/4) * ((iCurImage->Height + 3)/4)
					* iCurImage->Depth;

		switch (DXTCFormat)
		{
			case IL_DXT1:
			case IL_DXT1A:
			case IL_ATI1N:
				return BlockNum * 8;
			case IL_DXT3:
			case IL_DXT5:
			case IL_3DC:
			case IL_RXGB:
				return BlockNum * 16;
			default:
				ilSetError(IL_FORMAT_NOT_SUPPORTED);
				return 0;
		}
	}

	if (DXTCFormat == iCurImage->DxtcFormat && iCurImage->DxtcSize && iCurImage->DxtcData) {
		memcpy(Buffer, iCurImage->DxtcData, IL_MIN(BufferSize, iCurImage->DxtcSize));
		return IL_MIN(BufferSize, iCurImage->DxtcSize);
	}

	if (iCurImage->Origin != IL_ORIGIN_UPPER_LEFT) {
		CurData = iCurImage->Data;
		iCurImage->Data = iGetFlipped(iCurImage);
		if (iCurImage->Data == NULL) {
			iCurImage->Data = CurData;
			return 0;
		}
	}

	//@TODO: Is this the best way to do this?
	iSetOutputLump(Buffer, BufferSize);
	retVal = Compress(iCurImage, DXTCFormat);

	if (iCurImage->Origin != IL_ORIGIN_UPPER_LEFT) {
		ifree(iCurImage->Data);
		iCurImage->Data = CurData;
	}

	return retVal;
}


// Added the next two functions based on Charles Bloom's rant at
//  http://cbloomrants.blogspot.com/2008/12/12-08-08-dxtc-summary.html.
//  This code is by ryg and from the Molly Rocket forums:
//  https://mollyrocket.com/forums/viewtopic.php?t=392.
static ILint Mul8Bit(ILint a, ILint b)
{
	ILint t = a*b + 128;
	return (t + (t >> 8)) >> 8;
}

ILushort As16Bit(ILint r, ILint g, ILint b)
{
	return (Mul8Bit(r,31) << 11) + (Mul8Bit(g,63) << 5) + Mul8Bit(b,31);
}


ILushort *CompressTo565(ILimage *Image)
{
	ILimage		*TempImage;
	ILushort	*Data;
	ILuint		i, j;

	if ((Image->Type != IL_UNSIGNED_BYTE && Image->Type != IL_BYTE) || Image->Format == IL_COLOUR_INDEX) {
		TempImage = iConvertImage(iCurImage, IL_BGRA, IL_UNSIGNED_BYTE);  // @TODO: Needs to be BGRA.
		if (TempImage == NULL)
			return NULL;
	}
	else {
		TempImage = Image;
	}

	Data = (ILushort*)ialloc(iCurImage->Width * iCurImage->Height * 2 * iCurImage->Depth);
	if (Data == NULL) {
		if (TempImage != Image)
			ilCloseImage(TempImage);
		return NULL;
	}

	//changed 20040623: Use TempImages format :)
	switch (TempImage->Format)
	{
		case IL_RGB:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 3, j++) {
				/*Data[j]  = (TempImage->Data[i  ] >> 3) << 11;
				Data[j] |= (TempImage->Data[i+1] >> 2) << 5;
				Data[j] |=  TempImage->Data[i+2] >> 3;*/
				Data[j] = As16Bit(TempImage->Data[i], TempImage->Data[i+1], TempImage->Data[i+2]);
			}
			break;

		case IL_RGBA:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 4, j++) {
				/*Data[j]  = (TempImage->Data[i  ] >> 3) << 11;
				Data[j] |= (TempImage->Data[i+1] >> 2) << 5;
				Data[j] |=  TempImage->Data[i+2] >> 3;*/
				Data[j] = As16Bit(TempImage->Data[i], TempImage->Data[i+1], TempImage->Data[i+2]);
			}
			break;

		case IL_BGR:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 3, j++) {
				/*Data[j]  = (TempImage->Data[i+2] >> 3) << 11;
				Data[j] |= (TempImage->Data[i+1] >> 2) << 5;
				Data[j] |=  TempImage->Data[i  ] >> 3;*/
				Data[j] = As16Bit(TempImage->Data[i+2], TempImage->Data[i+1], TempImage->Data[i]);
			}
			break;

		case IL_BGRA:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 4, j++) {
				/*Data[j]  = (TempImage->Data[i+2] >> 3) << 11;
				Data[j] |= (TempImage->Data[i+1] >> 2) << 5;
				Data[j] |=  TempImage->Data[i  ] >> 3;*/
				Data[j] = As16Bit(TempImage->Data[i+2], TempImage->Data[i+1], TempImage->Data[i]);
			}
			break;

		case IL_LUMINANCE:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i++, j++) {
				//@TODO: Do better conversion here.
				/*Data[j]  = (TempImage->Data[i] >> 3) << 11;
				Data[j] |= (TempImage->Data[i] >> 2) << 5;
				Data[j] |=  TempImage->Data[i] >> 3;*/
				Data[j] = As16Bit(TempImage->Data[i], TempImage->Data[i], TempImage->Data[i]);
			}
			break;

		case IL_LUMINANCE_ALPHA:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 2, j++) {
				//@TODO: Do better conversion here.
				/*Data[j]  = (TempImage->Data[i] >> 3) << 11;
				Data[j] |= (TempImage->Data[i] >> 2) << 5;
				Data[j] |=  TempImage->Data[i] >> 3;*/
				Data[j] = As16Bit(TempImage->Data[i], TempImage->Data[i], TempImage->Data[i]);
			}
			break;

		case IL_ALPHA:
			memset(Data, 0, iCurImage->Width * iCurImage->Height * 2 * iCurImage->Depth);
			break;
	}

	if (TempImage != Image)
		ilCloseImage(TempImage);

	return Data;
}


ILubyte *CompressTo88(ILimage *Image)
{
	ILimage		*TempImage;
	ILubyte		*Data;
	ILuint		i, j;

	if ((Image->Type != IL_UNSIGNED_BYTE && Image->Type != IL_BYTE) || Image->Format == IL_COLOUR_INDEX) {
		TempImage = iConvertImage(iCurImage, IL_BGR, IL_UNSIGNED_BYTE);  // @TODO: Needs to be BGRA.
		if (TempImage == NULL)
			return NULL;
	}
	else {
		TempImage = Image;
	}

	Data = (ILubyte*)ialloc(iCurImage->Width * iCurImage->Height * 2 * iCurImage->Depth);
	if (Data == NULL) {
		if (TempImage != Image)
			ilCloseImage(TempImage);
		return NULL;
	}

	//changed 20040623: Use TempImage's format :)
	switch (TempImage->Format)
	{
		case IL_RGB:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 3, j += 2) {
				Data[j  ] = TempImage->Data[i+1];
				Data[j+1] = TempImage->Data[i  ];
			}
			break;

		case IL_RGBA:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 4, j += 2) {
				Data[j  ] = TempImage->Data[i+1];
				Data[j+1] = TempImage->Data[i  ];
			}
			break;

		case IL_BGR:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 3, j += 2) {
				Data[j  ] = TempImage->Data[i+1];
				Data[j+1] = TempImage->Data[i+2];
			}
			break;

		case IL_BGRA:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 4, j += 2) {
				Data[j  ] = TempImage->Data[i+1];
				Data[j+1] = TempImage->Data[i+2];
			}
			break;

		case IL_LUMINANCE:
		case IL_LUMINANCE_ALPHA:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i++, j += 2) {
				Data[j  ] = Data[j+1] = 0; //??? Luminance is no normal map format...
			}
			break;
	}

	if (TempImage != Image)
		ilCloseImage(TempImage);

	return Data;
}

void CompressToRXGB(ILimage *Image, ILushort** xgb, ILubyte** r)
{
	ILimage		*TempImage;
	ILuint		i, j;
	ILushort	*Data;
	ILubyte		*Alpha;

	*xgb = NULL;
	*r = NULL;

	if ((Image->Type != IL_UNSIGNED_BYTE && Image->Type != IL_BYTE) || Image->Format == IL_COLOUR_INDEX) {
		TempImage = iConvertImage(iCurImage, IL_BGR, IL_UNSIGNED_BYTE);  // @TODO: Needs to be BGRA.
		if (TempImage == NULL)
			return;
	}
	else {
		TempImage = Image;
	}

	*xgb = (ILushort*)ialloc(iCurImage->Width * iCurImage->Height * 2 * iCurImage->Depth);
	*r = (ILubyte*)ialloc(iCurImage->Width * iCurImage->Height * iCurImage->Depth);
	if (*xgb == NULL || *r == NULL) {
		if (TempImage != Image)
			ilCloseImage(TempImage);
		return;
	}

	//Alias pointers to be able to use copy'n'pasted code :)
	Data = *xgb;
	Alpha = *r;

	switch (TempImage->Format)
	{
		case IL_RGB:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 3, j++) {
				Alpha[j] = TempImage->Data[i];
				Data[j] = (TempImage->Data[i+1] >> 2) << 5;
				Data[j] |=  TempImage->Data[i+2] >> 3;
			}
			break;

		case IL_RGBA:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 4, j++) {
				Alpha[j]  = TempImage->Data[i];
				Data[j] = (TempImage->Data[i+1] >> 2) << 5;
				Data[j] |=  TempImage->Data[i+2] >> 3;
			}
			break;

		case IL_BGR:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 3, j++) {
				Alpha[j]  = TempImage->Data[i+2];
				Data[j] = (TempImage->Data[i+1] >> 2) << 5;
				Data[j] |=  TempImage->Data[i  ] >> 3;
			}
			break;

		case IL_BGRA:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 4, j++) {
				Alpha[j]  = TempImage->Data[i+2];
				Data[j] = (TempImage->Data[i+1] >> 2) << 5;
				Data[j] |=  TempImage->Data[i  ] >> 3;
			}
			break;

		case IL_LUMINANCE:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i++, j++) {
				Alpha[j]  = TempImage->Data[i];
				Data[j] = (TempImage->Data[i] >> 2) << 5;
				Data[j] |=  TempImage->Data[i] >> 3;
			}
			break;

		case IL_LUMINANCE_ALPHA:
			for (i = 0, j = 0; i < TempImage->SizeOfData; i += 2, j++) {
				Alpha[j]  = TempImage->Data[i];
				Data[j] = (TempImage->Data[i] >> 2) << 5;
				Data[j] |=  TempImage->Data[i] >> 3;
			}
			break;
	}

	if (TempImage != Image)
		ilCloseImage(TempImage);
}


ILuint Compress(ILimage *Image, ILenum DXTCFormat)
{
	ILushort	*Data, Block[16], ex0, ex1, *Runner16, t0, t1;
	ILuint		x, y, z, i, BitMask, DXTCSize;//, Rms1, Rms2;
	ILubyte		*Alpha, AlphaBlock[16], AlphaBitMask[6], /*AlphaOut[16],*/ a0, a1;
	ILboolean	HasAlpha;
	ILuint		Count = 0;
	ILubyte		*Data3Dc, *Runner8, *ByteData, *BlockData;

	if (DXTCFormat == IL_3DC) {
		Data3Dc = CompressTo88(Image);
		if (Data3Dc == NULL)
			return 0;

		Runner8 = Data3Dc;

		for (z = 0; z < Image->Depth; z++) {
			for (y = 0; y < Image->Height; y += 4) {
				for (x = 0; x < Image->Width; x += 4) {
					Get3DcBlock(AlphaBlock, Runner8, Image, x, y, 0);
					ChooseAlphaEndpoints(AlphaBlock, &a0, &a1);
					GenAlphaBitMask(a0, a1, AlphaBlock, AlphaBitMask, NULL);
					iputc(a0);
					iputc(a1);
					iwrite(AlphaBitMask, 1, 6);

					Get3DcBlock(AlphaBlock, Runner8, Image, x, y, 1);
					ChooseAlphaEndpoints(AlphaBlock, &a0, &a1);
					GenAlphaBitMask(a0, a1, AlphaBlock, AlphaBitMask, NULL);
					iputc(a0);
					iputc(a1);
					iwrite(AlphaBitMask, 1, 6);

					Count += 16;
				}
			}
			Runner8 += Image->Width * Image->Height * 2;
		}
		ifree(Data3Dc);
	}

	else if (DXTCFormat == IL_ATI1N)
	{
		ILimage *TempImage;

		if (Image->Bpp != 1) {
			TempImage = iConvertImage(iCurImage, IL_LUMINANCE, IL_UNSIGNED_BYTE);
			if (TempImage == NULL)
				return 0;
		}
		else {
			TempImage = Image;
		}

		Runner8 = TempImage->Data;

		for (z = 0; z < Image->Depth; z++) {
			for (y = 0; y < Image->Height; y += 4) {
				for (x = 0; x < Image->Width; x += 4) {
					GetAlphaBlock(AlphaBlock, Runner8, Image, x, y);
					ChooseAlphaEndpoints(AlphaBlock, &a0, &a1);
					GenAlphaBitMask(a0, a1, AlphaBlock, AlphaBitMask, NULL);
					iputc(a0);
					iputc(a1);
					iwrite(AlphaBitMask, 1, 6);
					Count += 8;
				}
			}
			Runner8 += Image->Width * Image->Height;
		}

		if (TempImage != Image)
			ilCloseImage(TempImage);
	}
	else
	{
		// We want to try nVidia compression first, because it is the fastest.
#ifdef IL_USE_DXTC_NVIDIA
		if (ilIsEnabled(IL_NVIDIA_COMPRESS) && Image->Depth == 1) {  // See if we need to use the nVidia Texture Tools library.
			if (DXTCFormat == IL_DXT1 || DXTCFormat == IL_DXT1A || DXTCFormat == IL_DXT3 || DXTCFormat == IL_DXT5) {
				// NVTT needs data as BGRA 32-bit.
				if (Image->Format != IL_BGRA || Image->Type != IL_UNSIGNED_BYTE) {  // No need to convert if already this format/type.
					ByteData = ilConvertBuffer(Image->SizeOfData, Image->Format, IL_BGRA, Image->Type, IL_UNSIGNED_BYTE, NULL, Image->Data);
					if (ByteData == NULL)
						return 0;
				}
				else
					ByteData = Image->Data;

				// Here's where all the compression and writing goes on.
				if (!ilNVidiaCompressDXTFile(ByteData, Image->Width, Image->Height, 1, DXTCFormat))
					return 0;

				if (ByteData != Image->Data)
					ifree(ByteData);

				return Image->Width * Image->Height * 4;  // Either compresses all or none.
			}
		}
#endif//IL_USE_DXTC_NVIDIA

		// libsquish generates better quality output than DevIL does, so we try it next.
#ifdef IL_USE_DXTC_SQUISH
		if (ilIsEnabled(IL_SQUISH_COMPRESS) && Image->Depth == 1) {  // See if we need to use the nVidia Texture Tools library.
			if (DXTCFormat == IL_DXT1 || DXTCFormat == IL_DXT1A || DXTCFormat == IL_DXT3 || DXTCFormat == IL_DXT5) {
				// libsquish needs data as RGBA 32-bit.
				if (Image->Format != IL_RGBA || Image->Type != IL_UNSIGNED_BYTE) {  // No need to convert if already this format/type.
					ByteData = (ILubyte*)ilConvertBuffer(Image->SizeOfData, Image->Format, IL_RGBA, Image->Type, IL_UNSIGNED_BYTE, NULL, Image->Data);
					if (ByteData == NULL)
						return 0;
				}
				else
					ByteData = Image->Data;

				// Get compressed data here.
				BlockData = ilSquishCompressDXT(ByteData, Image->Width, Image->Height, 1, DXTCFormat, &DXTCSize);
				if (BlockData == NULL)
					return 0;

				if (iwrite(BlockData, 1, DXTCSize) != DXTCSize) {
					if (ByteData != Image->Data)
						ifree(ByteData);
					ifree(BlockData);
					return 0;
				}

				if (ByteData != Image->Data)
					ifree(ByteData);
				ifree(BlockData);

				return Image->Width * Image->Height * 4;  // Either compresses all or none.
			}
		}
#endif//IL_USE_DXTC_SQUISH

		if (DXTCFormat != IL_RXGB) {
			Data = CompressTo565(Image);
			if (Data == NULL)
				return 0;

			Alpha = ilGetAlpha(IL_UNSIGNED_BYTE);
			if (Alpha == NULL) {
				ifree(Data);
				return 0;
			}
		}
		else {
			CompressToRXGB(Image, &Data, &Alpha);
			if (Data == NULL || Alpha == NULL) {
				if (Data != NULL)
					ifree(Data);
				if (Alpha != NULL)
					ifree(Alpha);
				return 0;
			}
		}

		Runner8 = Alpha;
		Runner16 = Data;

		switch (DXTCFormat)
		{
			case IL_DXT1:
			case IL_DXT1A:
				for (z = 0; z < Image->Depth; z++) {
					for (y = 0; y < Image->Height; y += 4) {
						for (x = 0; x < Image->Width; x += 4) {
							GetAlphaBlock(AlphaBlock, Runner8, Image, x, y);
							HasAlpha = IL_FALSE;
							for (i = 0 ; i < 16; i++) {
								if (AlphaBlock[i] < 128) {
									HasAlpha = IL_TRUE;
									break;
								}
							}

							GetBlock(Block, Runner16, Image, x, y);
							ChooseEndpoints(Block, &ex0, &ex1);
							CorrectEndDXT1(&ex0, &ex1, HasAlpha);
							SaveLittleUShort(ex0);
							SaveLittleUShort(ex1);
							if (HasAlpha)
								BitMask = GenBitMask(ex0, ex1, 3, Block, AlphaBlock, NULL);
							else
								BitMask = GenBitMask(ex0, ex1, 4, Block, NULL, NULL);
							SaveLittleUInt(BitMask);
							Count += 8;
						}
					}

					Runner16 += Image->Width * Image->Height;
					Runner8 += Image->Width * Image->Height;
				}
				break;

			/*case IL_DXT2:
				for (y = 0; y < Image->Height; y += 4) {
					for (x = 0; x < Image->Width; x += 4) {
						GetAlphaBlock(AlphaBlock, Alpha, Image, x, y);
						for (i = 0; i < 16; i += 2) {
							iputc((ILubyte)(((AlphaBlock[i] >> 4) << 4) | (AlphaBlock[i+1] >> 4)));
						}

						GetBlock(Block, Data, Image, x, y);
						PreMult(Block, AlphaBlock);
						ChooseEndpoints(Block, &ex0, &ex1);
						SaveLittleUShort(ex0);
						SaveLittleUShort(ex1);
						BitMask = GenBitMask(ex0, ex1, 4, Block, NULL, NULL);
						SaveLittleUInt(BitMask);
					}		
				}
				break;*/

			case IL_DXT3:
				for (z = 0; z < Image->Depth; z++) {
					for (y = 0; y < Image->Height; y += 4) {
						for (x = 0; x < Image->Width; x += 4) {
							GetAlphaBlock(AlphaBlock, Runner8, Image, x, y);
							for (i = 0; i < 16; i += 2) {
								iputc((ILubyte)(((AlphaBlock[i+1] >> 4) << 4) | (AlphaBlock[i] >> 4)));
							}

							GetBlock(Block, Runner16, Image, x, y);
							ChooseEndpoints(Block, &t0, &t1);
							ex0 = IL_MAX(t0, t1);
							ex1 = IL_MIN(t0, t1);
							CorrectEndDXT1(&ex0, &ex1, 0);
							SaveLittleUShort(ex0);
							SaveLittleUShort(ex1);
							BitMask = GenBitMask(ex0, ex1, 4, Block, NULL, NULL);
							SaveLittleUInt(BitMask);
							Count += 16;
						}
					}

					Runner16 += Image->Width * Image->Height;
					Runner8 += Image->Width * Image->Height;
				}
				break;

			case IL_RXGB:
			case IL_DXT5:
				for (z = 0; z < Image->Depth; z++) {
					for (y = 0; y < Image->Height; y += 4) {
						for (x = 0; x < Image->Width; x += 4) {
							GetAlphaBlock(AlphaBlock, Runner8, Image, x, y);
							ChooseAlphaEndpoints(AlphaBlock, &a0, &a1);
							GenAlphaBitMask(a0, a1, AlphaBlock, AlphaBitMask, NULL/*AlphaOut*/);
							/*Rms2 = RMSAlpha(AlphaBlock, AlphaOut);
							GenAlphaBitMask(a0, a1, 8, AlphaBlock, AlphaBitMask, AlphaOut);
							Rms1 = RMSAlpha(AlphaBlock, AlphaOut);
							if (Rms2 <= Rms1) {  // Yeah, we have to regenerate...
								GenAlphaBitMask(a0, a1, 6, AlphaBlock, AlphaBitMask, AlphaOut);
								Rms2 = a1;  // Just reuse Rms2 as a temporary variable...
								a1 = a0;
								a0 = Rms2;
							}*/
							iputc(a0);
							iputc(a1);
							iwrite(AlphaBitMask, 1, 6);

							GetBlock(Block, Runner16, Image, x, y);
							ChooseEndpoints(Block, &t0, &t1);
							ex0 = IL_MAX(t0, t1);
							ex1 = IL_MIN(t0, t1);
							CorrectEndDXT1(&ex0, &ex1, 0);
							SaveLittleUShort(ex0);
							SaveLittleUShort(ex1);
							BitMask = GenBitMask(ex0, ex1, 4, Block, NULL, NULL);
							SaveLittleUInt(BitMask);
							Count += 16;
						}
					}

					Runner16 += Image->Width * Image->Height;
					Runner8 += Image->Width * Image->Height;
				}
				break;
		}

		ifree(Data);
		ifree(Alpha);
	} //else no 3DC

	return Count;  // Returns 0 if no compression was done.
}


// Assumed to be 16-bit (5:6:5).
ILboolean GetBlock(ILushort *Block, ILushort *Data, ILimage *Image, ILuint XPos, ILuint YPos)
{
    ILuint x, y, i = 0, Offset = YPos * Image->Width + XPos;

	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
		    if (XPos + x < Image->Width && YPos + y < Image->Height)
				Block[i++] = Data[Offset + x];
			else
				// Variant of bugfix from https://sourceforge.net/forum/message.php?msg_id=5486779.
				//  If we are out of bounds of the image, just copy the adjacent data.
			    Block[i++] = Data[Offset];
		}
		// We do not want to read past the end of the image.
		if (YPos + y + 1 < Image->Height)
			Offset += Image->Width;
	}

	return IL_TRUE;
}


ILboolean GetAlphaBlock(ILubyte *Block, ILubyte *Data, ILimage *Image, ILuint XPos, ILuint YPos)
{
	ILuint x, y, i = 0, Offset = YPos * Image->Width + XPos;

	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
		    if (XPos + x < Image->Width && YPos + y < Image->Height)
				Block[i++] = Data[Offset + x];
			else
				// Variant of bugfix from https://sourceforge.net/forum/message.php?msg_id=5486779.
				//  If we are out of bounds of the image, just copy the adjacent data.
			    Block[i++] = Data[Offset];
		}
		// We do not want to read past the end of the image.
		if (YPos + y + 1 < Image->Height)
			Offset += Image->Width;
	}

	return IL_TRUE;
}

ILboolean Get3DcBlock(ILubyte *Block, ILubyte *Data, ILimage *Image, ILuint XPos, ILuint YPos, int channel)
{
	ILuint x, y, i = 0, Offset = 2*(YPos * Image->Width + XPos) + channel;

	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
			if (x < Image->Width && y < Image->Height)
                Block[i++] = Data[Offset + 2*x];
            else
                Block[i++] = Data[Offset];
		}
        Offset += 2*Image->Width;
	}

	return IL_TRUE;
}


void ShortToColor565(ILushort Pixel, Color565 *Colour)
{
	Colour->nRed   = (Pixel & 0xF800) >> 11;
	Colour->nGreen = (Pixel & 0x07E0) >> 5;
	Colour->nBlue  = (Pixel & 0x001F);
	return;
}


void ShortToColor888(ILushort Pixel, Color888 *Colour)
{
	Colour->r = ((Pixel & 0xF800) >> 11) << 3;
	Colour->g = ((Pixel & 0x07E0) >> 5)  << 2;
	Colour->b = ((Pixel & 0x001F))       << 3;
	return;
}


ILushort Color565ToShort(Color565 *Colour)
{
	return (Colour->nRed << 11) | (Colour->nGreen << 5) | (Colour->nBlue);
}


ILushort Color888ToShort(Color888 *Colour)
{
	return ((Colour->r >> 3) << 11) | ((Colour->g >> 2) << 5) | (Colour->b >> 3);
}


ILuint GenBitMask(ILushort ex0, ILushort ex1, ILuint NumCols, ILushort *In, ILubyte *Alpha, Color888 *OutCol)
{
	ILuint		i, j, Closest, Dist, BitMask = 0;
	ILubyte		Mask[16];
	Color888	c, Colours[4];

	ShortToColor888(ex0, &Colours[0]);
	ShortToColor888(ex1, &Colours[1]);
	if (NumCols == 3) {
		Colours[2].r = (Colours[0].r + Colours[1].r) / 2;
		Colours[2].g = (Colours[0].g + Colours[1].g) / 2;
		Colours[2].b = (Colours[0].b + Colours[1].b) / 2;
		Colours[3].r = (Colours[0].r + Colours[1].r) / 2;
		Colours[3].g = (Colours[0].g + Colours[1].g) / 2;
		Colours[3].b = (Colours[0].b + Colours[1].b) / 2;
	}
	else {  // NumCols == 4
		Colours[2].r = (2 * Colours[0].r + Colours[1].r + 1) / 3;
		Colours[2].g = (2 * Colours[0].g + Colours[1].g + 1) / 3;
		Colours[2].b = (2 * Colours[0].b + Colours[1].b + 1) / 3;
		Colours[3].r = (Colours[0].r + 2 * Colours[1].r + 1) / 3;
		Colours[3].g = (Colours[0].g + 2 * Colours[1].g + 1) / 3;
		Colours[3].b = (Colours[0].b + 2 * Colours[1].b + 1) / 3;
	}

	for (i = 0; i < 16; i++) {
		if (Alpha) {  // Test to see if we have 1-bit transparency
			if (Alpha[i] < 128) {
				Mask[i] = 3;  // Transparent
				if (OutCol) {
					OutCol[i].r = Colours[3].r;
					OutCol[i].g = Colours[3].g;
					OutCol[i].b = Colours[3].b;
				}
				continue;
			}
		}

		// If no transparency, try to find which colour is the closest.
		Closest = UINT_MAX;
		ShortToColor888(In[i], &c);
		for (j = 0; j < NumCols; j++) {
			Dist = Distance(&c, &Colours[j]);
			if (Dist < Closest) {
				Closest = Dist;
				Mask[i] = j;
				if (OutCol) {
					OutCol[i].r = Colours[j].r;
					OutCol[i].g = Colours[j].g;
					OutCol[i].b = Colours[j].b;
				}
			}
		}
	}

	for (i = 0; i < 16; i++) {
		BitMask |= (Mask[i] << (i*2));
	}

	return BitMask;
}


void GenAlphaBitMask(ILubyte a0, ILubyte a1, ILubyte *In, ILubyte *Mask, ILubyte *Out)
{
	ILubyte Alphas[8], M[16];
	ILuint	i, j, Closest, Dist;

	Alphas[0] = a0;
	Alphas[1] = a1;

	// 8-alpha or 6-alpha block?
	if (a0 > a1) {
		// 8-alpha block:  derive the other six alphas.
		// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
		Alphas[2] = (6 * Alphas[0] + 1 * Alphas[1] + 3) / 7;	// bit code 010
		Alphas[3] = (5 * Alphas[0] + 2 * Alphas[1] + 3) / 7;	// bit code 011
		Alphas[4] = (4 * Alphas[0] + 3 * Alphas[1] + 3) / 7;	// bit code 100
		Alphas[5] = (3 * Alphas[0] + 4 * Alphas[1] + 3) / 7;	// bit code 101
		Alphas[6] = (2 * Alphas[0] + 5 * Alphas[1] + 3) / 7;	// bit code 110
		Alphas[7] = (1 * Alphas[0] + 6 * Alphas[1] + 3) / 7;	// bit code 111
	}
	else {
		// 6-alpha block.
		// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
		Alphas[2] = (4 * Alphas[0] + 1 * Alphas[1] + 2) / 5;	// Bit code 010
		Alphas[3] = (3 * Alphas[0] + 2 * Alphas[1] + 2) / 5;	// Bit code 011
		Alphas[4] = (2 * Alphas[0] + 3 * Alphas[1] + 2) / 5;	// Bit code 100
		Alphas[5] = (1 * Alphas[0] + 4 * Alphas[1] + 2) / 5;	// Bit code 101
		Alphas[6] = 0x00;										// Bit code 110
		Alphas[7] = 0xFF;										// Bit code 111
	}

	for (i = 0; i < 16; i++) {
		Closest = UINT_MAX;
		for (j = 0; j < 8; j++) {
			Dist = abs((ILint)In[i] - (ILint)Alphas[j]);
			if (Dist < Closest) {
				Closest = Dist;
				M[i] = j;
			}
		}
	}

	if (Out) {
		for (i = 0; i < 16; i++) {
			Out[i] = Alphas[M[i]];
		}
	}

	//this was changed 20040623. There was a shift bug in here. Now the code
	//produces much higher quality images.
	// First three bytes.
	Mask[0] = (M[0]) | (M[1] << 3) | ((M[2] & 0x03) << 6);
	Mask[1] = ((M[2] & 0x04) >> 2) | (M[3] << 1) | (M[4] << 4) | ((M[5] & 0x01) << 7);
	Mask[2] = ((M[5] & 0x06) >> 1) | (M[6] << 2) | (M[7] << 5);

	// Second three bytes.
	Mask[3] = (M[8]) | (M[9] << 3) | ((M[10] & 0x03) << 6);
	Mask[4] = ((M[10] & 0x04) >> 2) | (M[11] << 1) | (M[12] << 4) | ((M[13] & 0x01) << 7);
	Mask[5] = ((M[13] & 0x06) >> 1) | (M[14] << 2) | (M[15] << 5);

	return;
}


ILuint RMSAlpha(ILubyte *Orig, ILubyte *Test)
{
	ILuint	RMS = 0, i;
	ILint	d;

	for (i = 0; i < 16; i++) {
		d = Orig[i] - Test[i];
		RMS += d*d;
	}

	//RMS /= 16;

	return RMS;
}


ILuint Distance(Color888 *c1, Color888 *c2)
{
	return  (c1->r - c2->r) * (c1->r - c2->r) +
			(c1->g - c2->g) * (c1->g - c2->g) +
			(c1->b - c2->b) * (c1->b - c2->b);
}

#define Sum(c) ((c)->r + (c)->g + (c)->b)
#define NormSquared(c) ((c)->r * (c)->r + (c)->g * (c)->g + (c)->b * (c)->b)

void ChooseEndpoints(ILushort *Block, ILushort *ex0, ILushort *ex1)
{
	ILuint		i;
	Color888	Colours[16];
	ILint		Lowest=0, Highest=0;

	for (i = 0; i < 16; i++) {
		ShortToColor888(Block[i], &Colours[i]);
	
		if (NormSquared(&Colours[i]) < NormSquared(&Colours[Lowest]))
			Lowest = i;
		if (NormSquared(&Colours[i]) > NormSquared(&Colours[Highest]))
 			Highest = i;
	}
	*ex0 = Block[Highest];
	*ex1 = Block[Lowest];
}

#undef Sum
#undef NormSquared


void ChooseAlphaEndpoints(ILubyte *Block, ILubyte *a0, ILubyte *a1)
{
	ILuint	i, Lowest = 0xFF, Highest = 0;

	for (i = 0; i < 16; i++) {
		if (Block[i] < Lowest)
			Lowest = Block[i];
		if (Block[i] > Highest)
			Highest = Block[i];
	}

	*a0 = Lowest;
	*a1 = Highest;
}


void CorrectEndDXT1(ILushort *ex0, ILushort *ex1, ILboolean HasAlpha)
{
	ILushort Temp;

	if (HasAlpha) {
		if (*ex0 > *ex1) {
			Temp = *ex0;
			*ex0 = *ex1;
			*ex1 = Temp;
		}
	}
	else {
		if (*ex0 < *ex1) {
			Temp = *ex0;
			*ex0 = *ex1;
			*ex1 = Temp;
		}
	}

	return;
}


void PreMult(ILushort *Data, ILubyte *Alpha)
{
	Color888	Colour;
	ILuint		i;

	for (i = 0; i < 16; i++) {
		ShortToColor888(Data[i], &Colour);
		Colour.r = (ILubyte)(((ILuint)Colour.r * Alpha[i]) >> 8);
		Colour.g = (ILubyte)(((ILuint)Colour.g * Alpha[i]) >> 8);
		Colour.b = (ILubyte)(((ILuint)Colour.b * Alpha[i]) >> 8);

		/*Colour.r = (ILubyte)(Colour.r * (Alpha[i] / 255.0));
		Colour.g = (ILubyte)(Colour.g * (Alpha[i] / 255.0));
		Colour.b = (ILubyte)(Colour.b * (Alpha[i] / 255.0));*/

		Data[i] = Color888ToShort(&Colour);
		ShortToColor888(Data[i], &Colour);
	}

	return;
}


//! Compresses data to a DXT format using different methods.
//  The data must be in unsigned byte RGBA or BGRA format.  Only DXT1, DXT3 and DXT5 are supported.
ILAPI ILubyte* ILAPIENTRY ilCompressDXT(ILubyte *Data, ILuint Width, ILuint Height, ILuint Depth, ILenum DXTCFormat, ILuint *DXTCSize)
{
	ILimage *TempImage, *CurImage = iCurImage;
	ILuint	BuffSize;
	ILubyte	*Buffer;

	if ((DXTCFormat != IL_DXT1 && DXTCFormat != IL_DXT1A && DXTCFormat != IL_DXT3 && DXTCFormat != IL_DXT5)
		|| Data == NULL || Width == 0 || Height == 0 || Depth == 0) {
		ilSetError(IL_INVALID_PARAM);
		return NULL;
	}

	// We want to try nVidia compression first, because it is the fastest.
#ifdef IL_USE_DXTC_NVIDIA
	if (ilIsEnabled(IL_NVIDIA_COMPRESS) && Depth == 1) {  // See if we need to use the nVidia Texture Tools library.
		// NVTT needs data as BGRA 32-bit.
		// Here's where all the compression and writing goes on.
		return ilNVidiaCompressDXT(Data, Width, Height, 1, DXTCFormat, DXTCSize);
	}
#endif//IL_USE_DXTC_NVIDIA

	// libsquish generates better quality output than DevIL does, so we try it next.
#ifdef IL_USE_DXTC_SQUISH
	if (ilIsEnabled(IL_SQUISH_COMPRESS) && Depth == 1) {  // See if we need to use the nVidia Texture Tools library.
		if (DXTCFormat == IL_DXT1 || DXTCFormat == IL_DXT1A || DXTCFormat == IL_DXT3 || DXTCFormat == IL_DXT5) {
			// libsquish needs data as RGBA 32-bit.
			// Get compressed data here.
			return ilSquishCompressDXT(Data, Width, Height, 1, DXTCFormat, DXTCSize);
		}
	}
#endif//IL_USE_DXTC_SQUISH

	TempImage = (ILimage*)ialloc(sizeof(ILimage));
	memset(TempImage, 0, sizeof(ILimage));
	TempImage->Width = Width;
	TempImage->Height = Height;
	TempImage->Depth = Depth;
	TempImage->Bpp = 4;  // RGBA or BGRA
	TempImage->Format = IL_BGRA;
	TempImage->Bpc = 1;  // Unsigned bytes only
	TempImage->Type = IL_UNSIGNED_BYTE;
	TempImage->SizeOfPlane = TempImage->Bps * Height;
	TempImage->SizeOfData  = TempImage->SizeOfPlane * Depth;
	TempImage->Origin = IL_ORIGIN_UPPER_LEFT;
	TempImage->Data = Data;

	BuffSize = ilGetDXTCData(NULL, 0, DXTCFormat);
	if (BuffSize == 0)
		return NULL;
	Buffer = (ILubyte*)ialloc(BuffSize);
	if (Buffer == NULL)
		return NULL;

	if (ilGetDXTCData(Buffer, BuffSize, DXTCFormat) != BuffSize) {
		ifree(Buffer);
		return NULL;
	}
	*DXTCSize = BuffSize;

	// Restore backup of iCurImage.
	iCurImage = CurImage;
	TempImage->Data = NULL;
	ilCloseImage(TempImage);

	return Buffer;
}
