//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_vtf.cpp
//
// Description: Reads from and writes to a Valve Texture Format (.vtf) file.
//                These are used in Valve's Source games.  VTF specs available
//                from http://developer.valvesoftware.com/wiki/VTF.
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_VTF
#include "il_vtf.h"
#include "il_dds.h"


//@TODO: Get rid of these globals.
//static VTFHEAD Head;


//! Checks if the file specified in FileName is a valid VTF file.
ILboolean ilIsValidVtf(ILconst_string FileName)
{
	ILHANDLE	VtfFile;
	ILboolean	bVtf = IL_FALSE;

	if (!iCheckExtension(FileName, IL_TEXT("vtf"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bVtf;
	}

	VtfFile = iopenr(FileName);
	if (VtfFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bVtf;
	}

	bVtf = ilIsValidVtfF(VtfFile);
	icloser(VtfFile);
	
	return bVtf;
}


//! Checks if the ILHANDLE contains a valid VTF file at the current position.
ILboolean ilIsValidVtfF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidVtf();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Checks if Lump is a valid VTF lump.
ILboolean ilIsValidVtfL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidVtf();
}


// Internal function used to get the VTF header from the current file.
ILboolean iGetVtfHead(VTFHEAD *Header)
{
	iread(Header->Signature, 1, 4);
	Header->Version[0] = GetLittleUInt();
	Header->Version[1] = GetLittleUInt();
	Header->HeaderSize = GetLittleUInt();
	Header->Width = GetLittleUShort();
	Header->Height = GetLittleUShort();
	Header->Flags = GetLittleUInt();
	Header->Frames = GetLittleUShort();
	Header->FirstFrame = GetLittleUShort();
	iseek(4, IL_SEEK_CUR);  // Padding
	Header->Reflectivity[0] = GetLittleFloat();
	Header->Reflectivity[1] = GetLittleFloat();
	Header->Reflectivity[2] = GetLittleFloat();
	iseek(4, IL_SEEK_CUR);  // Padding
	Header->BumpmapScale = GetLittleFloat();
	Header->HighResImageFormat = GetLittleUInt();
	Header->MipmapCount = (ILubyte)igetc();
	Header->LowResImageFormat = GetLittleInt();
	Header->LowResImageWidth = (ILubyte)igetc();
	Header->LowResImageHeight = (ILubyte)igetc();
//@TODO: This is a hack for the moment.
	if (Header->HeaderSize == 64) {
		Header->Depth = igetc();
		if (Header->Depth == 0)
			Header->Depth = 1;
	}
	else {
		Header->Depth = GetLittleUShort();
		iseek(Header->HeaderSize - sizeof(VTFHEAD), IL_SEEK_CUR);
	}

	return IL_TRUE;
}


// Internal function to get the header and check it.
ILboolean iIsValidVtf(void)
{
	VTFHEAD Header;

	if (!iGetVtfHead(&Header))
		return IL_FALSE;
	iseek(-(ILint)sizeof(VTFHEAD), IL_SEEK_CUR);
	
	return iCheckVtf(&Header);
}


//@TODO: Add more checks.
// Should we check for Frames, MipmapCount and Depth != 0?

// Internal function used to check if the HEADER is a valid VTF header.
ILboolean iCheckVtf(VTFHEAD *Header)
{
	// The file signature is "VTF\0".
	if ((Header->Signature[0] != 'V') || (Header->Signature[1] != 'T') || (Header->Signature[2] != 'F')
		|| (Header->Signature[3] != 0))
		return IL_FALSE;
	// Are there other versions available yet?
	if (Header->Version[0] != 7)
		return IL_FALSE;
	// We have 7.0 through 7.4 as of 12/27/2008.
	if (Header->Version[1] > 4)
		return IL_FALSE;
	// May change in future version of the specifications.
	//  80 is through version 7.2, and 96/104 are through 7.4.
	//  This must be 16-byte aligned, but something is outputting headers with 104.
	if ((Header->HeaderSize != 80) && (Header->HeaderSize != 96) && (Header->HeaderSize != 104)
		&& (Header->HeaderSize != 64))
		return IL_FALSE;

	// 0 is an invalid dimension
	if (Header->Width == 0 || Header->Height == 0)
		return IL_FALSE;
	// Width and Height must be powers of 2.
	if ((ilNextPower2(Header->Width) != Header->Width) || (ilNextPower2(Header->Height) != Header->Height))
		return IL_FALSE;
	// It looks like width and height of zero are valid - i.e. no low res image.
	if (Header->LowResImageWidth != 0 && Header->LowResImageHeight != 0) {
		if ((ilNextPower2(Header->LowResImageWidth) != Header->LowResImageWidth)
			|| (ilNextPower2(Header->LowResImageHeight) != Header->LowResImageHeight))
			return IL_FALSE;
	}
	// In addition, the LowResImage has to have dimensions no greater than 16.
	if ((Header->LowResImageWidth > 16) || (Header->LowResImageHeight > 16))
		//|| (Header->LowResImageWidth == 0) || (Header->LowResImageHeight == 0))
		// It looks like width and height of zero are valid.
		return IL_FALSE;
	// And the LowResImage has to have dimensions less than or equal to the main image.
	if ((Header->LowResImageWidth > Header->Width) || (Header->LowResImageHeight > Header->Height))
		return IL_FALSE;
	// The LowResImage must be in DXT1 format, or if it does not exist, it is denoted by all bits set.
	if (Header->LowResImageFormat != IMAGE_FORMAT_DXT1 && Header->LowResImageFormat != 0xFFFFFFFF)
		return IL_FALSE;
	
	return IL_TRUE;
}


//! Reads a VTF file
ILboolean ilLoadVtf(ILconst_string FileName)
{
	ILHANDLE	VtfFile;
	ILboolean	bVtf = IL_FALSE;
	
	VtfFile = iopenr(FileName);
	if (VtfFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bVtf;
	}

	bVtf = ilLoadVtfF(VtfFile);
	icloser(VtfFile);

	return bVtf;
}


//! Reads an already-opened VTF file
ILboolean ilLoadVtfF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;
	
	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadVtfInternal();
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Reads from a memory "lump" that contains a VTF
ILboolean ilLoadVtfL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadVtfInternal();
}


// Internal function used to load the VTF.
ILboolean iLoadVtfInternal(void)
{
	ILboolean	bVtf = IL_TRUE;
	ILimage		*Image, *BaseImage;
	ILenum		Format, Type;
	ILint		Frame, Face, Mipmap;
	ILuint		SizeOfData, Channels, k;
	ILubyte		*CompData = NULL, SwapVal, *Data16Bit, *Temp, NumFaces;
	VTFHEAD		Head;
	ILuint		CurName;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}
	CurName = ilGetCurName();
	
	if (!iGetVtfHead(&Head))
		return IL_FALSE;
	if (!iCheckVtf(&Head)) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	if (Head.Flags & TEXTUREFLAGS_ENVMAP) {
		// If we have a spherical map in addition to the 6 sides of the cubemap, FirstFrame's bits
		//  are all set.  The only place I found out about this was VTFLib.
		NumFaces = Head.FirstFrame == 0xFFFF ? 6 : 7;
	}
	else
		NumFaces = 1;  // This is not an environmental map.

	// Skip the low resolution image.  This is just a thumbnail.
	//  The block size is 8, and the compression ratio is 6:1.
	SizeOfData = IL_MAX(Head.LowResImageWidth * Head.LowResImageHeight / 2, 8);
	if (Head.LowResImageWidth == 0 && Head.LowResImageHeight == 0)
		SizeOfData = 0;  // No low resolution image present.
	iseek(SizeOfData, IL_SEEK_CUR);

	//@TODO: Make this a helper function that set channels, bpc and format.
	switch (Head.HighResImageFormat)
	{
		case IMAGE_FORMAT_DXT1:  //@TODO: Should we make DXT1 channels = 3?
		case IMAGE_FORMAT_DXT1_ONEBITALPHA:
		case IMAGE_FORMAT_DXT3:
		case IMAGE_FORMAT_DXT5:
			Channels = 4;
			Format = IL_RGBA;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_BGR888:
		case IMAGE_FORMAT_BGR888_BLUESCREEN:
			Channels = 3;
			Format = IL_BGR;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_BGRA8888:
			Channels = 4;
			Format = IL_BGRA;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_BGRX8888:
			Channels = 3;
			Format = IL_BGR;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_RGB888:
		case IMAGE_FORMAT_RGB888_BLUESCREEN:
			Channels = 3;
			Format = IL_RGB;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_RGBA8888:
			Channels = 4;
			Format = IL_RGBA;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_RGBA16161616:  // 16-bit shorts
			Channels = 4;
			Format = IL_RGBA;
			Type = IL_UNSIGNED_SHORT;
			break;
		case IMAGE_FORMAT_RGBA16161616F:  // 16-bit floats
			Channels = 4;
			Format = IL_RGBA;
			Type = IL_FLOAT;
			break;
		case IMAGE_FORMAT_I8:  // 8-bit luminance data
			Channels = 1;
			Format = IL_LUMINANCE;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_IA88:  // 8-bit luminance and alpha data
			Channels = 2;
			Format = IL_LUMINANCE_ALPHA;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_A8:  // 8-bit alpha data
			Channels = 1;
			Format = IL_ALPHA;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_ARGB8888:
			Channels = 4;
			Format = IL_BGRA;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_ABGR8888:
			Channels = 4;
			Format = IL_RGBA;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_RGB565:
			Channels = 3;
			Format = IL_RGB;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_BGR565:
			Channels = 3;
			Format = IL_BGR;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_BGRA5551:
			Channels = 4;
			Format = IL_BGRA;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_BGRX5551:  // Unused alpha channel
			Channels = 3;
			Format = IL_BGR;
			Type = IL_UNSIGNED_BYTE;
			break;
		case IMAGE_FORMAT_BGRA4444:
			Channels = 4;
			Format = IL_BGRA;
			Type = IL_UNSIGNED_BYTE;
			break;

		default:
			ilSetError(IL_FORMAT_NOT_SUPPORTED);
			return IL_FALSE;
	}

	if (!ilTexImage(Head.Width, Head.Height, Head.Depth, Channels, Format, Type, NULL))
		return IL_FALSE;
	// The origin should be in the upper left.
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;
	// Create any mipmaps.
	VtfInitFacesMipmaps(iCurImage, NumFaces, &Head);

	// Create our animation chain
	BaseImage = Image = iCurImage;  // Top-level image
	for (Frame = 1; Frame < Head.Frames; Frame++) {
		Image->Next = ilNewImageFull(Head.Width, Head.Height, Head.Depth, Channels, Format, Type, NULL);
		if (Image->Next == NULL)
			return IL_FALSE;
		Image = Image->Next;
		// The origin should be in the upper left.
		Image->Origin = IL_ORIGIN_UPPER_LEFT;

		// Create our faces and mipmaps for each frame.
		VtfInitFacesMipmaps(Image, NumFaces, &Head);
	}

	// We want to put the smallest mipmap at the end, but it is first in the file, so we count backwards.
	for (Mipmap = Head.MipmapCount - 1; Mipmap >= 0; Mipmap--) {
		// Frames are in the normal order.
		for (Frame = 0; Frame < Head.Frames; Frame++) {
			// @TODO: Cubemap faces are always in the same order?
			for (Face = 0; Face < NumFaces; Face++) {
				//@TODO: Would probably be quicker to do the linked list traversal manually here.
				ilBindImage(CurName);
				ilActiveImage(Frame);
				ilActiveFace(Face);
				ilActiveMipmap(Mipmap);
				Image = iCurImage;

				switch (Head.HighResImageFormat)
				{
					// DXT1 compression
					case IMAGE_FORMAT_DXT1:
					case IMAGE_FORMAT_DXT1_ONEBITALPHA:
						// The block size is 8.
						SizeOfData = IL_MAX(Image->Width * Image->Height * Image->Depth / 2, 8);
						CompData = (ILubyte*)ialloc(SizeOfData);  // Gives a 6:1 compression ratio (or 8:1 for DXT1 with alpha)
						if (CompData == NULL)
							return IL_FALSE;
						iread(CompData, 1, SizeOfData);
						// Keep a copy of the DXTC data if the user wants it.
						if (ilGetInteger(IL_KEEP_DXTC_DATA) == IL_TRUE) {
							Image->DxtcSize = SizeOfData;
							Image->DxtcData = CompData;
							Image->DxtcFormat = IL_DXT5;
							CompData = NULL;
						}
						bVtf = DecompressDXT1(Image, CompData);
						break;

					// DXT3 compression
					case IMAGE_FORMAT_DXT3:
						// The block size is 16.
						SizeOfData = IL_MAX(Image->Width * Image->Height * Image->Depth, 16);
						CompData = (ILubyte*)ialloc(SizeOfData);  // Gives a 4:1 compression ratio
						if (CompData == NULL)
							return IL_FALSE;
						iread(CompData, 1, SizeOfData);
						// Keep a copy of the DXTC data if the user wants it.
						if (ilGetInteger(IL_KEEP_DXTC_DATA) == IL_TRUE) {
							Image->DxtcSize = SizeOfData;
							Image->DxtcData = CompData;
							Image->DxtcFormat = IL_DXT3;
							CompData = NULL;
						}
						bVtf = DecompressDXT3(Image, CompData);
						break;

					// DXT5 compression
					case IMAGE_FORMAT_DXT5:
						// The block size is 16.
						SizeOfData = IL_MAX(Image->Width * Image->Height * Image->Depth, 16);
						CompData = (ILubyte*)ialloc(SizeOfData);  // Gives a 4:1 compression ratio
						if (CompData == NULL)
							return IL_FALSE;
						iread(CompData, 1, SizeOfData);
						// Keep a copy of the DXTC data if the user wants it.
						if (ilGetInteger(IL_KEEP_DXTC_DATA) == IL_TRUE) {
							Image->DxtcSize = SizeOfData;
							Image->DxtcData = CompData;
							Image->DxtcFormat = IL_DXT5;
							CompData = NULL;
						}
						bVtf = DecompressDXT5(Image, CompData);
						break;

					// Uncompressed BGR(A) data (24-bit and 32-bit)
					case IMAGE_FORMAT_BGR888:
					case IMAGE_FORMAT_BGRA8888:
					// Uncompressed RGB(A) data (24-bit and 32-bit)
					case IMAGE_FORMAT_RGB888:
					case IMAGE_FORMAT_RGBA8888:
					// Uncompressed 16-bit shorts
					case IMAGE_FORMAT_RGBA16161616:
					// Luminance data only
					case IMAGE_FORMAT_I8:
					// Luminance and alpha data
					case IMAGE_FORMAT_IA88:
					// Alpha data only
					case IMAGE_FORMAT_A8:
					// We will ignore the part about the bluescreen right now.
					//   I could not find any information about it.
					case IMAGE_FORMAT_RGB888_BLUESCREEN:
					case IMAGE_FORMAT_BGR888_BLUESCREEN:
						// Just copy the data over - no compression.
						if (iread(Image->Data, 1, Image->SizeOfData) != Image->SizeOfData)
							bVtf = IL_FALSE;
						else
							bVtf = IL_TRUE;
						break;

					// Uncompressed 24-bit data with an unused alpha channel (we discard it)
					case IMAGE_FORMAT_BGRX8888:
						SizeOfData = Image->Width * Image->Height * Image->Depth * 3;
						Temp = CompData = (ILubyte*)ialloc(SizeOfData / 3 * 4);  // Not compressed data
						if (CompData == NULL)
							return IL_FALSE;
						if (iread(CompData, 1, SizeOfData / 3 * 4) != SizeOfData / 3 * 4) {
							bVtf = IL_FALSE;
							break;
						}
						for (k = 0; k < SizeOfData; k += 3) {
							Image->Data[k]   = Temp[0];
							Image->Data[k+1] = Temp[1];
							Image->Data[k+2] = Temp[2];
							Temp += 4;
						}

						break;

					// Uncompressed 16-bit floats (must be converted to 32-bit)
					case IMAGE_FORMAT_RGBA16161616F:
						SizeOfData = Image->Width * Image->Height * Image->Depth * Image->Bpp * 2;
						CompData = (ILubyte*)ialloc(SizeOfData);  // Not compressed data
						if (CompData == NULL)
							return IL_FALSE;
						if (iread(CompData, 1, SizeOfData) != SizeOfData) {
							bVtf = IL_FALSE;
							break;
						}
						bVtf = iConvFloat16ToFloat32((ILuint*)Image->Data, (ILushort*)CompData, SizeOfData / 2);
						break;

					// Uncompressed 32-bit ARGB and ABGR data.  DevIL does not handle this
					//   internally, so we have to swap values.
					case IMAGE_FORMAT_ARGB8888:
					case IMAGE_FORMAT_ABGR8888:
						if (iread(Image->Data, 1, Image->SizeOfData) != Image->SizeOfData) {
							bVtf = IL_FALSE;
							break;
						}
						else {
							bVtf = IL_TRUE;
						}
						// Swap the data
						for (k = 0; k < Image->SizeOfData; k += 4) {
							SwapVal = Image->Data[k];
							Image->Data[k]   = Image->Data[k+3];
							Image->Data[k+3] = SwapVal;
							SwapVal = Image->Data[k+1];
							Image->Data[k+1] = Image->Data[k+2];
							Image->Data[k+2] = SwapVal;
						}
						break;

					// Uncompressed 16-bit RGB and BGR data.  We have to expand this to 24-bit, since
					//   DevIL does not handle this internally.
					//   The data is in the file as: gggbbbbb rrrrrrggg
					case IMAGE_FORMAT_RGB565:
					case IMAGE_FORMAT_BGR565:
						SizeOfData = Image->Width * Image->Height * Image->Depth * 2;
						Data16Bit = CompData = (ILubyte*)ialloc(SizeOfData);  // Not compressed data
						if (CompData == NULL)
							return IL_FALSE;
						if (iread(CompData, 1, SizeOfData) != SizeOfData) {
							bVtf = IL_FALSE;
							break;
						}
						for (k = 0; k < Image->SizeOfData; k += 3) {
							Image->Data[k]   =  (Data16Bit[0] & 0x1F) << 3;
							Image->Data[k+1] = ((Data16Bit[1] & 0x07) << 5) | ((Data16Bit[0] & 0xE0) >> 3);
							Image->Data[k+2] =   Data16Bit[1] & 0xF8;
							Data16Bit += 2;
						}
						break;

					// Uncompressed 16-bit BGRA data (1-bit alpha).  We have to expand this to 32-bit,
					//   since DevIL does not handle this internally.
					//   Something seems strange with this one, but this is how VTFEdit outputs.
					//   The data is in the file as: gggbbbbb arrrrrgg
					case IMAGE_FORMAT_BGRA5551:
						SizeOfData = Image->Width * Image->Height * Image->Depth * 2;
						Data16Bit = CompData = (ILubyte*)ialloc(SizeOfData);  // Not compressed data
						if (CompData == NULL)
							return IL_FALSE;
						if (iread(CompData, 1, SizeOfData) != SizeOfData) {
							bVtf = IL_FALSE;
							break;
						}
						for (k = 0; k < Image->SizeOfData; k += 4) {
							Image->Data[k]   =  (Data16Bit[0] & 0x1F) << 3;
							Image->Data[k+1] = ((Data16Bit[0] & 0xE0) >> 2) | ((Data16Bit[1] & 0x03) << 6);
							Image->Data[k+2] =  (Data16Bit[1] & 0x7C) << 1;
							// 1-bit alpha is either off or on.
							Image->Data[k+3] = ((Data16Bit[0] & 0x80) == 0x80) ? 0xFF : 0x00;
							Data16Bit += 2;
						}
						break;

					// Same as above, but the alpha channel is unused.
					case IMAGE_FORMAT_BGRX5551:
						SizeOfData = Image->Width * Image->Height * Image->Depth * 2;
						Data16Bit = CompData = (ILubyte*)ialloc(SizeOfData);  // Not compressed data
						if (iread(CompData, 1, SizeOfData) != SizeOfData) {
							bVtf = IL_FALSE;
							break;
						}
						for (k = 0; k < Image->SizeOfData; k += 3) {
							Image->Data[k]   =  (Data16Bit[0] & 0x1F) << 3;
							Image->Data[k+1] = ((Data16Bit[0] & 0xE0) >> 2) | ((Data16Bit[1] & 0x03) << 6);
							Image->Data[k+2] =  (Data16Bit[1] & 0x7C) << 1;
							Data16Bit += 2;
						}
						break;

					// Data is reduced to a 4-bits per channel format.
					case IMAGE_FORMAT_BGRA4444:
						SizeOfData = Image->Width * Image->Height * Image->Depth * 4;
						Temp = CompData = (ILubyte*)ialloc(SizeOfData / 2);  // Not compressed data
						if (CompData == NULL)
							return IL_FALSE;
						if (iread(CompData, 1, SizeOfData / 2) != SizeOfData / 2) {
							bVtf = IL_FALSE;
							break;
						}
						for (k = 0; k < SizeOfData; k += 4) {
							// We double the data here.
							Image->Data[k]   = (Temp[0] & 0x0F) << 4 | (Temp[0] & 0x0F);
							Image->Data[k+1] = (Temp[0] & 0xF0) >> 4 | (Temp[0] & 0xF0);
							Image->Data[k+2] = (Temp[1] & 0x0F) << 4 | (Temp[1] & 0x0F);
							Image->Data[k+3] = (Temp[1] & 0xF0) >> 4 | (Temp[1] & 0xF0);
							Temp += 2;
						}
						break;
				}

				ifree(CompData);
				CompData = NULL;
				if (bVtf == IL_FALSE)  //@TODO: Do we need to do any cleanup here?
					return IL_FALSE;
			}
		}
	}

	ilBindImage(CurName);  // Set to parent image first.
	return ilFixImage();
}


ILuint GetFaceFlag(ILuint FaceNum)
{
	switch (FaceNum)
	{
		case 0:
			return IL_CUBEMAP_POSITIVEX;
		case 1:
			return IL_CUBEMAP_NEGATIVEX;
		case 2:
			return IL_CUBEMAP_POSITIVEY;
		case 3:
			return IL_CUBEMAP_NEGATIVEY;
		case 4:
			return IL_CUBEMAP_POSITIVEZ;
		case 5:
			return IL_CUBEMAP_NEGATIVEZ;
		case 6:
			return IL_SPHEREMAP;
	}

	return IL_SPHEREMAP;  // Should never reach here!
}


ILboolean VtfInitFacesMipmaps(ILimage *BaseImage, ILuint NumFaces, VTFHEAD *Header)
{
	ILimage	*Image;
	ILuint	Face;

	// Initialize mipmaps under the base image.
	VtfInitMipmaps(BaseImage, Header);
	Image = BaseImage;

	// We have an environment map.
	if (NumFaces != 1) {
		Image->CubeFlags = IL_CUBEMAP_POSITIVEX;
	}

	for (Face = 1; Face < NumFaces; Face++) {
		Image->Faces = ilNewImageFull(Image->Width, Image->Height, Image->Depth, Image->Bpp, Image->Format, Image->Type, NULL);
		if (Image->Faces == NULL)
			return IL_FALSE;
		Image = Image->Faces;

		// The origin should be in the upper left.
		Image->Origin = IL_ORIGIN_UPPER_LEFT;
		// Set the flags that tell which face this is.
		Image->CubeFlags = GetFaceFlag(Face);

		// Now we can initialize the mipmaps under each face.
		VtfInitMipmaps(Image, Header);
	}

	return IL_TRUE;
}


ILboolean VtfInitMipmaps(ILimage *BaseImage, VTFHEAD *Header)
{
	ILimage	*Image;
	ILuint	Width, Height, Depth, Mipmap;

	Image = BaseImage;
	Width = BaseImage->Width;  Height = BaseImage->Height;  Depth = BaseImage->Depth;

	for (Mipmap = 1; Mipmap < Header->MipmapCount; Mipmap++) {
		// 1 is the smallest dimension possible.
		Width = (Width >> 1) == 0 ? 1 : (Width >> 1);
		Height = (Height >> 1) == 0 ? 1 : (Height >> 1);
		Depth = (Depth >> 1) == 0 ? 1 : (Depth >> 1);

		Image->Mipmaps = ilNewImageFull(Width, Height, Depth, BaseImage->Bpp, BaseImage->Format, BaseImage->Type, NULL);
		if (Image->Mipmaps == NULL)
			return IL_FALSE;
		Image = Image->Mipmaps;

		// ilNewImage does not set these.
		Image->Format = BaseImage->Format;
		Image->Type = BaseImage->Type;
		// The origin should be in the upper left.
		Image->Origin = IL_ORIGIN_UPPER_LEFT;
	}

	return IL_TRUE;
}



ILboolean CheckDimensions()
{
	if ((ilNextPower2(iCurImage->Width) != iCurImage->Width) || (ilNextPower2(iCurImage->Height) != iCurImage->Height)) {
		ilSetError(IL_BAD_DIMENSIONS);
		return IL_FALSE;
	}
	return IL_TRUE;
}

//! Writes a Vtf file
ILboolean ilSaveVtf(const ILstring FileName)
{
	ILHANDLE	VtfFile;
	ILuint		VtfSize;

	if (!CheckDimensions())
		return IL_FALSE;

	if (ilGetBoolean(IL_FILE_MODE) == IL_FALSE) {
		if (iFileExists(FileName)) {
			ilSetError(IL_FILE_ALREADY_EXISTS);
			return IL_FALSE;
		}
	}

	VtfFile = iopenw(FileName);
	if (VtfFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	VtfSize = ilSaveVtfF(VtfFile);
	iclosew(VtfFile);

	if (VtfSize == 0)
		return IL_FALSE;
	return IL_TRUE;
}


//! Writes a .vtf to an already-opened file
ILuint ilSaveVtfF(ILHANDLE File)
{
	ILuint Pos;
	if (!CheckDimensions())
		return 0;
	iSetOutputFile(File);
	Pos = itellw();
	if (iSaveVtfInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


//! Writes a .vtf to a memory "lump"
ILuint ilSaveVtfL(void *Lump, ILuint Size)
{
	ILuint Pos;
	if (!CheckDimensions())
		return 0;
	iSetOutputLump(Lump, Size);
	Pos = itellw();
	if (iSaveVtfInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


// Internal function used to save the Vtf.
ILboolean iSaveVtfInternal()
{
	ILimage	*TempImage = iCurImage;
	ILubyte	*TempData, *CompData;
	ILuint	Format, i, CompSize;
	ILenum	Compression;

	// Find out if the user has specified to use DXT compression.
	Compression = ilGetInteger(IL_VTF_COMP);

	//@TODO: Other formats
	if (Compression == IL_DXT_NO_COMP) {
		switch (TempImage->Format)
		{
			case IL_RGB:
				Format = IMAGE_FORMAT_RGB888;
				break;
			case IL_RGBA:
				Format = IMAGE_FORMAT_RGBA8888;
				break;
			case IL_BGR:
				Format = IMAGE_FORMAT_BGR888;
				break;
			case IL_BGRA:
				Format = IMAGE_FORMAT_BGRA8888;
				break;
			case IL_LUMINANCE:
				Format = IMAGE_FORMAT_I8;
				break;
			case IL_ALPHA:
				Format = IMAGE_FORMAT_A8;
				break;
			case IL_LUMINANCE_ALPHA:
				Format = IMAGE_FORMAT_IA88;
				break;
			//case IL_COLOUR_INDEX:
			default:
				Format = IMAGE_FORMAT_BGRA8888;
				TempImage = iConvertImage(iCurImage, IL_BGRA, IL_UNSIGNED_BYTE);
				if (TempImage == NULL)
					return IL_FALSE;
		}

		//@TODO: When we have the half format available internally, also use IMAGE_FORMAT_RGBA16161616F.
		if (TempImage->Format == IL_RGBA && TempImage->Type == IL_UNSIGNED_SHORT) {
			Format = IMAGE_FORMAT_RGBA16161616;
		}
		else if (TempImage->Type != IL_UNSIGNED_BYTE) {  //@TODO: Any possibility for shorts, etc. to be used?
			TempImage = iConvertImage(iCurImage, Format, IL_UNSIGNED_BYTE);
			if (TempImage == NULL)
				return IL_FALSE;
		}
	}
	else {  // We are using DXT compression.
		switch (Compression)
		{
			case IL_DXT1:
				Format = IMAGE_FORMAT_DXT1_ONEBITALPHA;//IMAGE_FORMAT_DXT1;
				break;
			case IL_DXT3:
				Format = IMAGE_FORMAT_DXT3;
				break;
			case IL_DXT5:
				Format = IMAGE_FORMAT_DXT5;
				break;
			default:  // Should never reach this point.
				ilSetError(IL_INTERNAL_ERROR);
				Format = IMAGE_FORMAT_DXT5;
		}
	}

	if (TempImage->Origin != IL_ORIGIN_UPPER_LEFT) {
		TempData = iGetFlipped(TempImage);
		if (TempData == NULL) {
			ilCloseImage(TempImage);
			return IL_FALSE;
		}
	} else {
		TempData = TempImage->Data;
	}

	// Write the file signature.
	iwrite("VTF", 1, 4);
	// Write the file version - currently using 7.2 specs.
	SaveLittleUInt(7);
	SaveLittleUInt(2);
	// Write the header size.
	SaveLittleUInt(80);
	// Now we write the width and height of the image.
	SaveLittleUShort(TempImage->Width);
	SaveLittleUShort(TempImage->Height);
	//@TODO: This is supposed to be the flags used.  What should we use here?  Let users specify?
	SaveLittleUInt(0);
	// Number of frames in the animation. - @TODO: Change to use animations.
	SaveLittleUShort(1);
	// First frame in the animation
	SaveLittleUShort(0);
	// Padding
	SaveLittleUInt(0);
	// Reflectivity (3 floats) - @TODO: Use what values?  User specified?
	SaveLittleFloat(0.0f);
	SaveLittleFloat(0.0f);
	SaveLittleFloat(0.0f);
	// Padding
	SaveLittleUInt(0);
	// Bumpmap scale
	SaveLittleFloat(0.0f);
	// Image format
	SaveLittleUInt(Format);
	// Mipmap count - @TODO: Use mipmaps
	iputc(1);
	// Low resolution image format - @TODO: Create low resolution image.
	SaveLittleUInt(0xFFFFFFFF);
	// Low resolution image width and height
	iputc(0);
	iputc(0);
	// Depth of the image - @TODO: Support for volumetric images.
	SaveLittleUShort(1);

	// Write final padding for the header (out to 80 bytes).
	for (i = 0; i < 15; i++) {
		iputc(0);
	}

	if (Compression == IL_DXT_NO_COMP) {
		// We just write the image data directly.
		if (iwrite(TempImage->Data, TempImage->SizeOfData, 1) != 1)
			return IL_FALSE;
	}
	else {  // Do DXT compression here and write.
		// We have to find out how much we are writing first.
		CompSize = ilGetDXTCData(NULL, 0, Compression);
		if (CompSize == 0) {
			ilSetError(IL_INTERNAL_ERROR);
			if (TempData != TempImage->Data)
				ifree(TempData);
			return IL_FALSE;
		}
		CompData = (ILubyte*)ialloc(CompSize);
		if (CompData == NULL) {
			if (TempData != TempImage->Data)
				ifree(TempData);
			return IL_FALSE;
		}

		// DXT compress the data.
		CompSize = ilGetDXTCData(CompData, CompSize, Compression);
		if (CompSize == 0) {
			ilSetError(IL_INTERNAL_ERROR);
			if (TempData != TempImage->Data)
				ifree(TempData);
			return IL_FALSE;
		}
		// Finally write the data.
		if (iwrite(CompData, CompSize, 1) != 1) {
			ifree(CompData);
			if (TempData != TempImage->Data)
				ifree(TempData);
			return IL_FALSE;
		}
	}

	if (TempData != TempImage->Data)
		ifree(TempData);
	if (TempImage != iCurImage)
		ilCloseImage(TempImage);

	return IL_TRUE;
}


#endif//IL_NO_VTF
