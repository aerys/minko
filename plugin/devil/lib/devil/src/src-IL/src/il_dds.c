//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 02/28/2009
//
// Filename: src-IL/src/il_dds.c
//
// Description: Reads from a DirectDraw Surface (.dds) file.
//
//-----------------------------------------------------------------------------


//
//
// Note:  Almost all this code is from nVidia's DDS-loading example at
//	http://www.nvidia.com/view.asp?IO=dxtc_decompression_code
//	and from the specs at
//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dx8_c/hh/dx8_c/graphics_using_0j03.asp
//	and
//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dx8_c/directx_cpp/Graphics/ProgrammersGuide/Appendix/DDSFileFormat/ovwDDSFileFormat.asp
//	However, some not really valid .dds files are also read, for example
//	Volume Textures without the COMPLEX bit set, so the specs aren't taken
//	too strictly while reading.


#include "il_internal.h"
#ifndef IL_NO_DDS
#include "il_dds.h"


// Global variables
static DDSHEAD	Head;				// Image header
static ILubyte	*CompData = NULL;	// Compressed data
static ILuint	CompSize;			// Compressed size
//static ILuint	CompFormat;			// Compressed format
static ILimage	*Image;
static ILint	Width, Height, Depth;
static ILboolean	Has16BitComponents;

ILuint CubemapDirections[CUBEMAP_SIDES] = {
	DDS_CUBEMAP_POSITIVEX,
	DDS_CUBEMAP_NEGATIVEX,
	DDS_CUBEMAP_POSITIVEY,
	DDS_CUBEMAP_NEGATIVEY,
	DDS_CUBEMAP_POSITIVEZ,
	DDS_CUBEMAP_NEGATIVEZ
};


//! Checks if the file specified in FileName is a valid .dds file.
ILboolean ilIsValidDds(ILconst_string FileName)
{
	ILHANDLE	DdsFile;
	ILboolean	bDds = IL_FALSE;

	if (!iCheckExtension(FileName, IL_TEXT("dds"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bDds;
	}

	DdsFile = iopenr(FileName);
	if (DdsFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bDds;
	}

	bDds = ilIsValidDdsF(DdsFile);
	icloser(DdsFile);

	return bDds;
}


//! Checks if the ILHANDLE contains a valid .dds file at the current position.
ILboolean ilIsValidDdsF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidDds();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Checks if Lump is a valid .dds lump.
ILboolean ilIsValidDdsL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidDds();
}


// Internal function used to get the .dds header from the current file.
ILboolean iGetDdsHead(DDSHEAD *Header)
{
	ILint i;

	iread(&Header->Signature, 1, 4);
	Header->Size1 = GetLittleUInt();
	Header->Flags1 = GetLittleUInt();
	Header->Height = GetLittleUInt();
	Header->Width = GetLittleUInt();
	Header->LinearSize = GetLittleUInt();
	Header->Depth = GetLittleUInt();
	Header->MipMapCount = GetLittleUInt();
	Header->AlphaBitDepth = GetLittleUInt();

	for (i = 0; i < 10; ++i)
		Header->NotUsed[i] = GetLittleUInt();

	Header->Size2 = GetLittleUInt();
	Header->Flags2 = GetLittleUInt();
	Header->FourCC = GetLittleUInt();
	Header->RGBBitCount = GetLittleUInt();
	Header->RBitMask = GetLittleUInt();
	Header->GBitMask = GetLittleUInt();
	Header->BBitMask = GetLittleUInt();
	Header->RGBAlphaBitMask = GetLittleUInt();
	Header->ddsCaps1 = GetLittleUInt();
	Header->ddsCaps2 = GetLittleUInt();
	Header->ddsCaps3 = GetLittleUInt();
	Header->ddsCaps4 = GetLittleUInt();
	Header->TextureStage = GetLittleUInt();

	if (Head.Depth == 0)
		Head.Depth = 1;

	return IL_TRUE;
}


// Internal function to get the header and check it.
ILboolean iIsValidDds()
{
	ILboolean	IsValid;
	DDSHEAD		Head;

	iGetDdsHead(&Head);
	iseek(-(ILint)sizeof(DDSHEAD), IL_SEEK_CUR);  // Go ahead and restore to previous state

	IsValid = iCheckDds(&Head);

	return IsValid;
}


// Internal function used to check if the HEADER is a valid .dds header.
ILboolean iCheckDds(DDSHEAD *Head)
{
	if (strncmp((const char*)Head->Signature, "DDS ", 4))
		return IL_FALSE;
	//note that if Size1 is "DDS " this is not a valid dds file according
	//to the file spec. Some broken tool out there seems to produce files
	//with this value in the size field, so we support reading them...
	if (Head->Size1 != 124 && Head->Size1 != IL_MAKEFOURCC('D', 'D', 'S', ' '))
		return IL_FALSE;
	if (Head->Size2 != 32)
		return IL_FALSE;
	if (Head->Width == 0 || Head->Height == 0)
		return IL_FALSE;
	return IL_TRUE;
}


//! Reads a .dds file
ILboolean ilLoadDds(ILconst_string FileName)
{
	ILHANDLE	DdsFile;
	ILboolean	bDds = IL_FALSE;

	DdsFile = iopenr(FileName);
	if (DdsFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bDds;
	}

	bDds = ilLoadDdsF(DdsFile);
	icloser(DdsFile);

	return bDds;
}


//! Reads an already-opened .dds file
ILboolean ilLoadDdsF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadDdsInternal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Reads from a memory "lump" that contains a .dds
ILboolean ilLoadDdsL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadDdsInternal();
}


void Check16BitComponents(DDSHEAD *Header)
{
	if (Header->RGBBitCount != 32)
		Has16BitComponents = IL_FALSE;
	// a2b10g10r10 format
	if (Header->RBitMask == 0x3FF00000 && Header->GBitMask == 0x000FFC00 && Header->BBitMask == 0x000003FF
		&& Header->RGBAlphaBitMask == 0xC0000000)
		Has16BitComponents = IL_TRUE;
	// a2r10g10b10 format
	else if (Header->RBitMask == 0x000003FF && Header->GBitMask == 0x000FFC00 && Header->BBitMask == 0x3FF00000
		&& Header->RGBAlphaBitMask == 0xC0000000)
		Has16BitComponents = IL_TRUE;
	else
		Has16BitComponents = IL_FALSE;
	return;
}


ILubyte iCompFormatToBpp(ILenum Format)
{
	//non-compressed (= non-FOURCC) codes
	if (Format == PF_LUMINANCE || Format == PF_LUMINANCE_ALPHA || Format == PF_ARGB)
		return Head.RGBBitCount/8;

	//fourcc formats
	else if (Format == PF_RGB || Format == PF_3DC || Format == PF_RXGB)
		return 3;
	else if (Format == PF_ATI1N)
		return 1;
	//else if (Format == PF_R16F)
	//	return 2;
	else if (Format == PF_A16B16G16R16 || Format == PF_A16B16G16R16F
		|| Format == PF_G32R32F)
		return 8;
	else if (Format == PF_A32B32G32R32F)
		return 16;
	else //if (Format == PF_G16R16F || Format == PF_R32F || dxt)
		return 4;
}


ILubyte iCompFormatToBpc(ILenum Format)
{
	if (Has16BitComponents)
		return 2;
	if (Format == PF_R16F || Format == PF_G16R16F || Format == PF_A16B16G16R16F)
		//DevIL has no internal half type, so these formats are converted to 32 bits
		return 4;
	else if (Format == PF_R32F || Format == PF_R16F || Format == PF_G32R32F || Format == PF_A32B32G32R32F)
		return 4;
	else if(Format == PF_A16B16G16R16)
		return 2;
	else
		return 1;
}


ILubyte iCompFormatToChannelCount(ILenum Format)
{
	if (Format == PF_RGB || Format == PF_3DC || Format == PF_RXGB)
		return 3;
	else if (Format == PF_LUMINANCE || /*Format == PF_R16F || Format == PF_R32F ||*/ Format == PF_ATI1N)
		return 1;
	else if (Format == PF_LUMINANCE_ALPHA /*|| Format == PF_G16R16F || Format == PF_G32R32F*/)
		return 2;
	else if (Format == PF_G16R16F || Format == PF_G32R32F || Format == PF_R32F || Format == PF_R16F)
		return 3;
	else //if(Format == PF_ARGB || dxt)
		return 4;
}


ILboolean iLoadDdsCubemapInternal(ILuint CompFormat)
{
	ILuint	i;
	ILubyte	Bpp, Channels, Bpc;
	ILimage *startImage;

	CompData = NULL;

	Bpp = iCompFormatToBpp(CompFormat);
	Channels = iCompFormatToChannelCount(CompFormat);
	Bpc = iCompFormatToBpc(CompFormat);
	if (CompFormat == PF_LUMINANCE && Head.RGBBitCount == 16 && Head.RBitMask == 0xFFFF) { //@TODO: This is a HACK.
		Bpc = 2; Bpp = 2;
	}

	startImage = Image;
	// Run through cube map possibilities
	for (i = 0; i < CUBEMAP_SIDES; i++) {
		// Reset each time
		Width = Head.Width;
		Height = Head.Height;
		Depth = Head.Depth;
		if (Head.ddsCaps2 & CubemapDirections[i]) {
			if (i != 0) {
				Image->Faces = ilNewImage(Width, Height, Depth, Channels, Bpc);
				if (Image->Faces == NULL)
					return IL_FALSE;

				Image = Image->Faces;

				if (CompFormat == PF_R16F
					|| CompFormat == PF_G16R16F
					|| CompFormat == PF_A16B16G16R16F
					|| CompFormat == PF_R32F
					|| CompFormat == PF_G32R32F
					|| CompFormat == PF_A32B32G32R32F) {
					// DevIL's format autodetection doesn't work for
					//  float images...correct this.
					Image->Type = IL_FLOAT;
					Image->Bpp = Channels;
				}

				ilBindImage(ilGetCurName()); // Set to parent image first.
				//ilActiveImage(i); //@TODO: now Image == iCurImage...globals SUCK, fix this!!!
				ilActiveFace(i);
			}

			if (!ReadData())
				return IL_FALSE;

			if (!AllocImage(CompFormat)) {
				if (CompData) {
					ifree(CompData);
					CompData = NULL;
				}
				return IL_FALSE;
			}

			Image->CubeFlags = CubemapDirections[i];

			if (!DdsDecompress(CompFormat)) {
				if (CompData) {
					ifree(CompData);
					CompData = NULL;
				}
				return IL_FALSE;
			}

			if (!ReadMipmaps(CompFormat)) {
				if (CompData) {
					ifree(CompData);
					CompData = NULL;
				}
				return IL_FALSE;
			}
		}
	}

	if (CompData) {
		ifree(CompData);
		CompData = NULL;
	}

	ilBindImage(ilGetCurName());  // Set to parent image first.
	return ilFixImage();
}


ILboolean iLoadDdsInternal()
{
	ILuint	BlockSize = 0;
	ILuint	CompFormat;

	CompData = NULL;
	Image = NULL;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	if (!iGetDdsHead(&Head)) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}
	if (!iCheckDds(&Head)) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	BlockSize = DecodePixelFormat(&CompFormat);
	if (CompFormat == PF_UNKNOWN) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}
	Check16BitComponents(&Head);

	// Microsoft bug, they're not following their own documentation.
	if (!(Head.Flags1 & (DDS_LINEARSIZE | DDS_PITCH))
		|| Head.LinearSize == 0) {
		Head.Flags1 |= DDS_LINEARSIZE;
		Head.LinearSize = BlockSize;
	}

	Image = iCurImage;
	if (Head.ddsCaps1 & DDS_COMPLEX) {
		if (Head.ddsCaps2 & DDS_CUBEMAP) {
			if (!iLoadDdsCubemapInternal(CompFormat))
				return IL_FALSE;
			return IL_TRUE;
		}
	}

	Width = Head.Width;
	Height = Head.Height;
	Depth = Head.Depth;
	AdjustVolumeTexture(&Head, CompFormat);

	if (!ReadData())
		return IL_FALSE;
	if (!AllocImage(CompFormat)) {
		if (CompData) {
			ifree(CompData);
			CompData = NULL;
		}
		return IL_FALSE;
	}
	if (!DdsDecompress(CompFormat)) {
		if (CompData) {
			ifree(CompData);
			CompData = NULL;
		}
		return IL_FALSE;
	}

	if (!ReadMipmaps(CompFormat)) {
		if (CompData) {
			ifree(CompData);
			CompData = NULL;
		}
		return IL_FALSE;
	}

	if (CompData) {
		ifree(CompData);
		CompData = NULL;
	}

	ilBindImage(ilGetCurName());  // Set to parent image first.
	return ilFixImage();
}


ILuint DecodePixelFormat(ILuint *CompFormat)
{
	ILuint BlockSize;

	if (Head.Flags2 & DDS_FOURCC) {
		BlockSize = ((Head.Width + 3)/4) * ((Head.Height + 3)/4) * Head.Depth;
		switch (Head.FourCC)
		{
			case IL_MAKEFOURCC('D','X','T','1'):
				*CompFormat = PF_DXT1;
				BlockSize *= 8;
				break;

			case IL_MAKEFOURCC('D','X','T','2'):
				*CompFormat = PF_DXT2;
				BlockSize *= 16;
				break;

			case IL_MAKEFOURCC('D','X','T','3'):
				*CompFormat = PF_DXT3;
				BlockSize *= 16;
				break;

			case IL_MAKEFOURCC('D','X','T','4'):
				*CompFormat = PF_DXT4;
				BlockSize *= 16;
				break;

			case IL_MAKEFOURCC('D','X','T','5'):
				*CompFormat = PF_DXT5;
				BlockSize *= 16;
				break;

			case IL_MAKEFOURCC('A', 'T', 'I', '1'):
				*CompFormat = PF_ATI1N;
				BlockSize *= 8;
				break;

			case IL_MAKEFOURCC('A', 'T', 'I', '2'):
				*CompFormat = PF_3DC;
				BlockSize *= 16;
				break;

			case IL_MAKEFOURCC('R', 'X', 'G', 'B'):
				*CompFormat = PF_RXGB;
				BlockSize *= 16;
				break;

			case IL_MAKEFOURCC('$', '\0', '\0', '\0'):
				*CompFormat = PF_A16B16G16R16;
				BlockSize = Head.Width * Head.Height * Head.Depth * 8;
				break;

			case IL_MAKEFOURCC('o', '\0', '\0', '\0'):
				*CompFormat = PF_R16F;
				BlockSize = Head.Width * Head.Height * Head.Depth * 2;
				break;

			case IL_MAKEFOURCC('p', '\0', '\0', '\0'):
				*CompFormat = PF_G16R16F;
				BlockSize = Head.Width * Head.Height * Head.Depth * 4;
				break;

			case IL_MAKEFOURCC('q', '\0', '\0', '\0'):
				*CompFormat = PF_A16B16G16R16F;
				BlockSize = Head.Width * Head.Height * Head.Depth * 8;
				break;

			case IL_MAKEFOURCC('r', '\0', '\0', '\0'):
				*CompFormat = PF_R32F;
				BlockSize = Head.Width * Head.Height * Head.Depth * 4;
				break;

			case IL_MAKEFOURCC('s', '\0', '\0', '\0'):
				*CompFormat = PF_G32R32F;
				BlockSize = Head.Width * Head.Height * Head.Depth * 8;
				break;

			case IL_MAKEFOURCC('t', '\0', '\0', '\0'):
				*CompFormat = PF_A32B32G32R32F;
				BlockSize = Head.Width * Head.Height * Head.Depth * 16;
				break;

			default:
				*CompFormat = PF_UNKNOWN;
				BlockSize *= 16;
				break;
		}
	} else {
		// This dds texture isn't compressed so write out ARGB or luminance format
		if (Head.Flags2 & DDS_LUMINANCE) {
			if (Head.Flags2 & DDS_ALPHAPIXELS) {
				*CompFormat = PF_LUMINANCE_ALPHA;
			} else {
				*CompFormat = PF_LUMINANCE;
			}
		}
		else {
			if (Head.Flags2 & DDS_ALPHAPIXELS) {
				*CompFormat = PF_ARGB;
			} else {
				*CompFormat = PF_RGB;
			}
		}
		BlockSize = (Head.Width * Head.Height * Head.Depth * (Head.RGBBitCount >> 3));
	}

	return BlockSize;
}


// The few volume textures that I have don't have consistent LinearSize
//	entries, even though the DDS_LINEARSIZE flag is set.
void AdjustVolumeTexture(DDSHEAD *Head, ILuint CompFormat)
{
	if (Head->Depth <= 1)
		return;

	// All volume textures I've seem so far didn't have the DDS_COMPLEX flag set,
	// even though this is normally required. But because noone does set it,
	// also read images without it (TODO: check file size for 3d texture?)
	if (/*!(Head->ddsCaps1 & DDS_COMPLEX) ||*/ !(Head->ddsCaps2 & DDS_VOLUME)) {
		Head->Depth = 1;
		Depth = 1;
	}

	switch (CompFormat)
	{
		case PF_ARGB:
		case PF_RGB:
		case PF_LUMINANCE:
		case PF_LUMINANCE_ALPHA:
			//don't use the iCompFormatToBpp() function because this way
			//argb images with only 8 bits (eg. a1r2g3b2) work as well
			Head->LinearSize = IL_MAX(1,Head->Width) * IL_MAX(1,Head->Height) *
				(Head->RGBBitCount / 8);
			break;
	
		case PF_DXT1:

		case PF_ATI1N:
			Head->LinearSize = ((Head->Width+3)/4) * ((Head->Height+3)/4) * 8;
			break;

		case PF_DXT2:
		case PF_DXT3:
		case PF_DXT4:
		case PF_DXT5:
		case PF_3DC:
		case PF_RXGB:
			Head->LinearSize = ((Head->Width+3)/4) * ((Head->Height+3)/4) * 16;
			break;

		case PF_A16B16G16R16:
		case PF_R16F:
		case PF_G16R16F:
		case PF_A16B16G16R16F:
		case PF_R32F:
		case PF_G32R32F:
		case PF_A32B32G32R32F:
			Head->LinearSize = IL_MAX(1,Head->Width) * IL_MAX(1,Head->Height) *
				iCompFormatToBpp(CompFormat);
			break;
	}

	Head->Flags1 |= DDS_LINEARSIZE;
	Head->LinearSize *= Head->Depth;

	return;
}


// Reads the compressed data
ILboolean ReadData()
{
	ILuint	Bps;
	ILint	y, z;
	ILubyte	*Temp;

	if (CompData) {
		ifree(CompData);
		CompData = NULL;
	}

	if (Head.Flags1 & DDS_LINEARSIZE) {
		//Head.LinearSize = Head.LinearSize * Depth;

		CompData = (ILubyte*)ialloc(Head.LinearSize);
		if (CompData == NULL) {
			return IL_FALSE;
		}

		if (iread(CompData, 1, Head.LinearSize) != (ILuint)Head.LinearSize) {
			ifree(CompData);
			CompData = NULL;
			return IL_FALSE;
		}
	}
	else {
		Bps = Width * Head.RGBBitCount / 8;
		CompSize = Bps * Height * Depth;

		CompData = (ILubyte*)ialloc(CompSize);
		if (CompData == NULL) {
			return IL_FALSE;
		}

		Temp = CompData;
		for (z = 0; z < Depth; z++) {
			for (y = 0; y < Height; y++) {
				if (iread(Temp, 1, Bps) != Bps) {
					ifree(CompData);
					CompData = NULL;
					return IL_FALSE;
				}
				Temp += Bps;
			}
		}
	}

	return IL_TRUE;
}


ILboolean AllocImage(ILuint CompFormat)
{
	ILubyte channels = 4;
	ILenum format = IL_RGBA;

	switch (CompFormat)
	{
		case PF_RGB:
			if (!ilTexImage(Width, Height, Depth, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL))
				return IL_FALSE;
			break;
		case PF_ARGB:
			if (!ilTexImage(Width, Height, Depth, 4, IL_RGBA, Has16BitComponents ? IL_UNSIGNED_SHORT : IL_UNSIGNED_BYTE, NULL))
				return IL_FALSE;
			break;

		case PF_LUMINANCE:
			if (Head.RGBBitCount == 16 && Head.RBitMask == 0xFFFF) { //HACK
				if (!ilTexImage(Width, Height, Depth, 1, IL_LUMINANCE, IL_UNSIGNED_SHORT, NULL))
					return IL_FALSE;
			}
			else
				if (!ilTexImage(Width, Height, Depth, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, NULL))
					return IL_FALSE;
			break;

		case PF_LUMINANCE_ALPHA:
			if (!ilTexImage(Width, Height, Depth, 2, IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE, NULL))
				return IL_FALSE;
			break;

		case PF_ATI1N:
			//right now there's no OpenGL api to use the compressed 3dc data, so
			//throw it away (I don't know how DirectX works, though)?
			if (!ilTexImage(Width, Height, Depth, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, NULL))
				return IL_FALSE;
			break;

		case PF_3DC:
			//right now there's no OpenGL api to use the compressed 3dc data, so
			//throw it away (I don't know how DirectX works, though)?
			if (!ilTexImage(Width, Height, Depth, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL))
				return IL_FALSE;
			break;

		case PF_A16B16G16R16:
			if (!ilTexImage(Width, Height, Depth, iCompFormatToChannelCount(CompFormat),
				ilGetFormatBpp(iCompFormatToChannelCount(CompFormat)), IL_UNSIGNED_SHORT, NULL))
				return IL_FALSE;
			break;

		case PF_R16F:
		case PF_G16R16F:
		case PF_A16B16G16R16F:
		case PF_R32F:
		case PF_G32R32F:
		case PF_A32B32G32R32F:
			if (!ilTexImage(Width, Height, Depth, iCompFormatToChannelCount(CompFormat),
				ilGetFormatBpp(iCompFormatToChannelCount(CompFormat)), IL_FLOAT, NULL))
				return IL_FALSE;
			break;

		default:
			if (CompFormat == PF_RXGB) {
				channels = 3; //normal map
				format = IL_RGB;
			}

			if (!ilTexImage(Width, Height, Depth, channels, format, IL_UNSIGNED_BYTE, NULL))
				return IL_FALSE;
			if (ilGetInteger(IL_KEEP_DXTC_DATA) == IL_TRUE && CompData) {
				iCurImage->DxtcData = (ILubyte*)ialloc(Head.LinearSize);
				if (iCurImage->DxtcData == NULL)
					return IL_FALSE;
				iCurImage->DxtcFormat = CompFormat - PF_DXT1 + IL_DXT1;
				iCurImage->DxtcSize = Head.LinearSize;
				memcpy(iCurImage->DxtcData, CompData, iCurImage->DxtcSize);
			}
			break;
	}

	Image->Origin = IL_ORIGIN_UPPER_LEFT;
	
	return IL_TRUE;
}


/*
 * Assumes that the global variable CompFormat stores the format of the
 * global pointer CompData (that's the pointer to the compressed data).
 * Decompresses this data into Image->Data, returns if it was successful.
 * It also uses the globals Width and Height.
 *
 * Assumes that iCurImage has valid Width, Height, Depth, Data, SizeOfData,
 * Bpp, Bpc, Bps, SizeOfPlane, Format and Type fields. It is more or
 * less assumed that the image has u8 rgba data (perhaps not for float
 * images...)
 *
 *
 * @TODO: don't use globals, clean this function (and this file) up
 */
ILboolean DdsDecompress(ILuint CompFormat)
{
	switch (CompFormat)
	{
		case PF_ARGB:
		case PF_RGB:
		case PF_LUMINANCE:
		case PF_LUMINANCE_ALPHA:
			return DecompressARGB(CompFormat);

		case PF_DXT1:
			return DecompressDXT1(Image, CompData);

		case PF_DXT2:
			return DecompressDXT2(Image, CompData);

		case PF_DXT3:
			return DecompressDXT3(Image, CompData);

		case PF_DXT4:
			return DecompressDXT4(Image, CompData);

		case PF_DXT5:
			return DecompressDXT5(Image, CompData);

		case PF_ATI1N:
			return DecompressAti1n();

		case PF_3DC:
			return Decompress3Dc();

		case PF_RXGB:
			return DecompressRXGB();

		case PF_A16B16G16R16:
			memcpy(Image->Data, CompData, Image->SizeOfData);
			return IL_TRUE;

		case PF_R16F:
		case PF_G16R16F:
		case PF_A16B16G16R16F:
		case PF_R32F:
		case PF_G32R32F:
		case PF_A32B32G32R32F:
			return DecompressFloat(CompFormat);

		case PF_UNKNOWN:
			return IL_FALSE;
	}

	return IL_FALSE;
}


ILboolean ReadMipmaps(ILuint CompFormat)
{
	ILuint	i, CompFactor=0;
	ILubyte	Bpp, Channels, Bpc;
	ILimage	*StartImage, *TempImage;
	ILuint	LastLinear;
	ILuint	minW, minH;

	ILboolean isCompressed = IL_FALSE;

	Bpp = iCompFormatToBpp(CompFormat);
	Channels = iCompFormatToChannelCount(CompFormat);
	Bpc = iCompFormatToBpc(CompFormat);
	if (CompFormat == PF_LUMINANCE && Head.RGBBitCount == 16 && Head.RBitMask == 0xFFFF) { //HACK
		Bpc = 2; Bpp = 2;
	}

	//This doesn't work for images which first mipmap (= the image
	//itself) has width or height < 4
	//if (Head.Flags1 & DDS_LINEARSIZE) {
	//	CompFactor = (Width * Height * Depth * Bpp) / Head.LinearSize;
	//}
	switch (CompFormat)
	{
		case PF_DXT1:
			// This is officially 6, we have 8 here because DXT1 may contain alpha.
			CompFactor = 8;
			break;
		case PF_DXT2:
		case PF_DXT3:
		case PF_DXT4:
		case PF_DXT5:
			CompFactor = 4;
			break;
		case PF_RXGB:
		case PF_3DC:
			// This is officially 4 for 3dc, but that's bullshit :) There's no
			//  alpha data in 3dc images.
			CompFactor = 3;
			break;

		case PF_ATI1N:
			CompFactor = 2;
			break;
		default:
			CompFactor = 1;
	}

	StartImage = Image;

	if (!(Head.Flags1 & DDS_MIPMAPCOUNT) || Head.MipMapCount == 0) {
		//some .dds-files have their mipmap flag set,
		//but a mipmapcount of 0. Because mipMapCount is an uint, 0 - 1 gives
		//overflow - don't let this happen:
		Head.MipMapCount = 1;
	}

	LastLinear = Head.LinearSize;
	for (i = 0; i < Head.MipMapCount - 1; i++) {
		Depth = Depth / 2;
		Width = Width / 2;
		Height = Height / 2;

		if (Depth == 0) 
			Depth = 1;
		if (Width == 0) 
			Width = 1;
		if (Height == 0) 
			Height = 1;

		Image->Mipmaps = ilNewImage(Width, Height, Depth, Channels, Bpc);
		if (Image->Mipmaps == NULL)
			goto mip_fail;
		Image = Image->Mipmaps;
		Image->Origin = IL_ORIGIN_UPPER_LEFT;

		if (Head.Flags1 & DDS_LINEARSIZE) {
			if (CompFormat == PF_R16F
				|| CompFormat == PF_G16R16F
				|| CompFormat == PF_A16B16G16R16F
				|| CompFormat == PF_R32F
				|| CompFormat == PF_G32R32F
				|| CompFormat == PF_A32B32G32R32F) {
				Head.LinearSize = Width * Height * Depth * Bpp;

				//DevIL's format autodetection doesn't work for
				//float images...correct this
				Image->Type = IL_FLOAT;
				Image->Bpp = Channels;
			}
			else if (CompFormat == PF_A16B16G16R16)
				Head.LinearSize = Width * Height * Depth * Bpp;
			else if (CompFormat != PF_RGB && CompFormat != PF_ARGB
				&& CompFormat != PF_LUMINANCE
				&& CompFormat != PF_LUMINANCE_ALPHA) {

				//compressed format
				minW = (((Width+3)/4))*4;
				minH = (((Height+3)/4))*4;
				Head.LinearSize = (minW * minH * Depth * Bpp) / CompFactor;

				isCompressed = IL_TRUE;
			}
			else {
				//don't use Bpp to support argb images with less than 32 bits
				Head.LinearSize = Width * Height * Depth * (Head.RGBBitCount >> 3);
			}
		}
		else {
			Head.LinearSize >>= 1;
		}

		if (!ReadData())
			goto mip_fail;

		if (ilGetInteger(IL_KEEP_DXTC_DATA) == IL_TRUE && isCompressed == IL_TRUE && CompData) {
			Image->DxtcData = (ILubyte*)ialloc(Head.LinearSize);
			if (Image->DxtcData == NULL)
				return IL_FALSE;
			Image->DxtcFormat = CompFormat - PF_DXT1 + IL_DXT1;
			Image->DxtcSize = Head.LinearSize;
			memcpy(Image->DxtcData, CompData, Image->DxtcSize);
		}

		if (!DdsDecompress(CompFormat))
			goto mip_fail;
	}

	Head.LinearSize = LastLinear;
	Image = StartImage;

	return IL_TRUE;

mip_fail:
	Image = StartImage;
	StartImage = StartImage->Mipmaps;
	while (StartImage) {
		TempImage = StartImage;
		StartImage = StartImage->Mipmaps;
		ifree(TempImage);
	}

	Image->Mipmaps = NULL;
	return IL_FALSE;
}


void DxtcReadColors(const ILubyte* Data, Color8888* Out)
{
	ILubyte r0, g0, b0, r1, g1, b1;

	b0 = Data[0] & 0x1F;
	g0 = ((Data[0] & 0xE0) >> 5) | ((Data[1] & 0x7) << 3);
	r0 = (Data[1] & 0xF8) >> 3;

	b1 = Data[2] & 0x1F;
	g1 = ((Data[2] & 0xE0) >> 5) | ((Data[3] & 0x7) << 3);
	r1 = (Data[3] & 0xF8) >> 3;

	Out[0].r = r0 << 3 | r0 >> 2;
	Out[0].g = g0 << 2 | g0 >> 3;
	Out[0].b = b0 << 3 | b0 >> 2;

	Out[1].r = r1 << 3 | r1 >> 2;
	Out[1].g = g1 << 2 | g1 >> 3;
	Out[1].b = b1 << 3 | b1 >> 2;
}

//@TODO: Probably not safe on Big Endian.
void DxtcReadColor(ILushort Data, Color8888* Out)
{
	ILubyte r, g, b;

	b = Data & 0x1f;
	g = (Data & 0x7E0) >> 5;
	r = (Data & 0xF800) >> 11;

	Out->r = r << 3 | r >> 2;
	Out->g = g << 2 | g >> 3;
	Out->b = b << 3 | r >> 2;
}

ILboolean DecompressDXT1(ILimage *lImage, ILubyte *lCompData)
{
	ILuint		x, y, z, i, j, k, Select;
	ILubyte		*Temp;
	Color8888	colours[4], *col;
	ILushort	color_0, color_1;
	ILuint		bitmask, Offset;

	if (!lCompData)
		return IL_FALSE;

	Temp = lCompData;
	colours[0].a = 0xFF;
	colours[1].a = 0xFF;
	colours[2].a = 0xFF;
	//colours[3].a = 0xFF;
	for (z = 0; z < lImage->Depth; z++) {
		for (y = 0; y < lImage->Height; y += 4) {
			for (x = 0; x < lImage->Width; x += 4) {
				color_0 = *((ILushort*)Temp);
				UShort(&color_0);
				color_1 = *((ILushort*)(Temp + 2));
				UShort(&color_1);
				DxtcReadColor(color_0, colours);
				DxtcReadColor(color_1, colours + 1);
				bitmask = ((ILuint*)Temp)[1];
				UInt(&bitmask);
				Temp += 8;

				if (color_0 > color_1) {
					// Four-color block: derive the other two colors.
					// 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
					// These 2-bit codes correspond to the 2-bit fields 
					// stored in the 64-bit block.
					colours[2].b = (2 * colours[0].b + colours[1].b + 1) / 3;
					colours[2].g = (2 * colours[0].g + colours[1].g + 1) / 3;
					colours[2].r = (2 * colours[0].r + colours[1].r + 1) / 3;
					//colours[2].a = 0xFF;

					colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
					colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
					colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;
					colours[3].a = 0xFF;
				}
				else { 
					// Three-color block: derive the other color.
					// 00 = color_0,  01 = color_1,  10 = color_2,
					// 11 = transparent.
					// These 2-bit codes correspond to the 2-bit fields 
					// stored in the 64-bit block. 
					colours[2].b = (colours[0].b + colours[1].b) / 2;
					colours[2].g = (colours[0].g + colours[1].g) / 2;
					colours[2].r = (colours[0].r + colours[1].r) / 2;
					//colours[2].a = 0xFF;

					colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
					colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
					colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;
					colours[3].a = 0x00;
				}

				for (j = 0, k = 0; j < 4; j++) {
					for (i = 0; i < 4; i++, k++) {
						Select = (bitmask & (0x03 << k*2)) >> k*2;
						col = &colours[Select];

						if (((x + i) < lImage->Width) && ((y + j) < lImage->Height)) {
							Offset = z * lImage->SizeOfPlane + (y + j) * lImage->Bps + (x + i) * lImage->Bpp;
							lImage->Data[Offset + 0] = col->r;
							lImage->Data[Offset + 1] = col->g;
							lImage->Data[Offset + 2] = col->b;
							lImage->Data[Offset + 3] = col->a;
						}
					}
				}
			}
		}
	}

	return IL_TRUE;
}


ILboolean DecompressDXT2(ILimage *lImage, ILubyte *lCompData)
{
	// Can do color & alpha same as dxt3, but color is pre-multiplied 
	//   so the result will be wrong unless corrected. 
	if (!DecompressDXT3(Image, CompData))
		return IL_FALSE;
	CorrectPreMult();

	return IL_TRUE;
}


ILboolean DecompressDXT3(ILimage *lImage, ILubyte *lCompData)
{
	ILuint		x, y, z, i, j, k, Select;
	ILubyte		*Temp;
	//Color565	*color_0, *color_1;
	Color8888	colours[4], *col;
	ILuint		bitmask, Offset;
	ILushort	word;
	ILubyte		*alpha;

	if (!lCompData)
		return IL_FALSE;

	Temp = lCompData;
	for (z = 0; z < lImage->Depth; z++) {
		for (y = 0; y < lImage->Height; y += 4) {
			for (x = 0; x < lImage->Width; x += 4) {
				alpha = Temp;
				Temp += 8;
				DxtcReadColors(Temp, colours);
				bitmask = ((ILuint*)Temp)[1];
				UInt(&bitmask);
				Temp += 8;

				// Four-color block: derive the other two colors.    
				// 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
				// These 2-bit codes correspond to the 2-bit fields 
				// stored in the 64-bit block.
				colours[2].b = (2 * colours[0].b + colours[1].b + 1) / 3;
				colours[2].g = (2 * colours[0].g + colours[1].g + 1) / 3;
				colours[2].r = (2 * colours[0].r + colours[1].r + 1) / 3;
				//colours[2].a = 0xFF;

				colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
				colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
				colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;
				//colours[3].a = 0xFF;

				k = 0;
				for (j = 0; j < 4; j++) {
					for (i = 0; i < 4; i++, k++) {
						Select = (bitmask & (0x03 << k*2)) >> k*2;
						col = &colours[Select];

						if (((x + i) < lImage->Width) && ((y + j) < lImage->Height)) {
							Offset = z * lImage->SizeOfPlane + (y + j) * lImage->Bps + (x + i) * lImage->Bpp;
							lImage->Data[Offset + 0] = col->r;
							lImage->Data[Offset + 1] = col->g;
							lImage->Data[Offset + 2] = col->b;
						}
					}
				}

				for (j = 0; j < 4; j++) {
					word = alpha[2*j] + 256*alpha[2*j+1];
					for (i = 0; i < 4; i++) {
						if (((x + i) < lImage->Width) && ((y + j) < lImage->Height)) {
							Offset = z * lImage->SizeOfPlane + (y + j) * lImage->Bps + (x + i) * lImage->Bpp + 3;
							lImage->Data[Offset] = word & 0x0F;
							lImage->Data[Offset] = lImage->Data[Offset] | (lImage->Data[Offset] << 4);
						}
						word >>= 4;
					}
				}

			}
		}
	}

	return IL_TRUE;
}


ILboolean DecompressDXT4(ILimage *lImage, ILubyte *lCompData)
{
	// Can do color & alpha same as dxt5, but color is pre-multiplied 
	//   so the result will be wrong unless corrected. 
	if (!DecompressDXT5(Image, CompData))
		return IL_FALSE;
	CorrectPreMult();

	return IL_FALSE;
}


ILboolean DecompressDXT5(ILimage *lImage, ILubyte *lCompData)
{
	ILuint		x, y, z, i, j, k, Select;
	ILubyte		*Temp; //, r0, g0, b0, r1, g1, b1;
	Color8888	colours[4], *col;
	ILuint		bitmask, Offset;
	ILubyte		alphas[8], *alphamask;
	ILuint		bits;

	if (!lCompData)
		return IL_FALSE;

	Temp = lCompData;
	for (z = 0; z < lImage->Depth; z++) {
		for (y = 0; y < lImage->Height; y += 4) {
			for (x = 0; x < lImage->Width; x += 4) {
				if (y >= lImage->Height || x >= lImage->Width)
					break;
				alphas[0] = Temp[0];
				alphas[1] = Temp[1];
				alphamask = Temp + 2;
				Temp += 8;

				DxtcReadColors(Temp, colours);
				bitmask = ((ILuint*)Temp)[1];
				UInt(&bitmask);
				Temp += 8;

				// Four-color block: derive the other two colors.    
				// 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
				// These 2-bit codes correspond to the 2-bit fields 
				// stored in the 64-bit block.
				colours[2].b = (2 * colours[0].b + colours[1].b + 1) / 3;
				colours[2].g = (2 * colours[0].g + colours[1].g + 1) / 3;
				colours[2].r = (2 * colours[0].r + colours[1].r + 1) / 3;
				//colours[2].a = 0xFF;

				colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
				colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
				colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;
				//colours[3].a = 0xFF;

				k = 0;
				for (j = 0; j < 4; j++) {
					for (i = 0; i < 4; i++, k++) {

						Select = (bitmask & (0x03 << k*2)) >> k*2;
						col = &colours[Select];

						// only put pixels out < width or height
						if (((x + i) < lImage->Width) && ((y + j) < lImage->Height)) {
							Offset = z * lImage->SizeOfPlane + (y + j) * lImage->Bps + (x + i) * lImage->Bpp;
							lImage->Data[Offset + 0] = col->r;
							lImage->Data[Offset + 1] = col->g;
							lImage->Data[Offset + 2] = col->b;
						}
					}
				}

				// 8-alpha or 6-alpha block?    
				if (alphas[0] > alphas[1]) {    
					// 8-alpha block:  derive the other six alphas.    
					// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
					alphas[2] = (6 * alphas[0] + 1 * alphas[1] + 3) / 7;	// bit code 010
					alphas[3] = (5 * alphas[0] + 2 * alphas[1] + 3) / 7;	// bit code 011
					alphas[4] = (4 * alphas[0] + 3 * alphas[1] + 3) / 7;	// bit code 100
					alphas[5] = (3 * alphas[0] + 4 * alphas[1] + 3) / 7;	// bit code 101
					alphas[6] = (2 * alphas[0] + 5 * alphas[1] + 3) / 7;	// bit code 110
					alphas[7] = (1 * alphas[0] + 6 * alphas[1] + 3) / 7;	// bit code 111
				}
				else {
					// 6-alpha block.
					// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
					alphas[2] = (4 * alphas[0] + 1 * alphas[1] + 2) / 5;	// Bit code 010
					alphas[3] = (3 * alphas[0] + 2 * alphas[1] + 2) / 5;	// Bit code 011
					alphas[4] = (2 * alphas[0] + 3 * alphas[1] + 2) / 5;	// Bit code 100
					alphas[5] = (1 * alphas[0] + 4 * alphas[1] + 2) / 5;	// Bit code 101
					alphas[6] = 0x00;										// Bit code 110
					alphas[7] = 0xFF;										// Bit code 111
				}

				// Note: Have to separate the next two loops,
				//	it operates on a 6-byte system.

				// First three bytes
				//bits = *((ILint*)alphamask);
				bits = (alphamask[0]) | (alphamask[1] << 8) | (alphamask[2] << 16);
				for (j = 0; j < 2; j++) {
					for (i = 0; i < 4; i++) {
						// only put pixels out < width or height
						if (((x + i) < lImage->Width) && ((y + j) < lImage->Height)) {
							Offset = z * lImage->SizeOfPlane + (y + j) * lImage->Bps + (x + i) * lImage->Bpp + 3;
							lImage->Data[Offset] = alphas[bits & 0x07];
						}
						bits >>= 3;
					}
				}

				// Last three bytes
				//bits = *((ILint*)&alphamask[3]);
				bits = (alphamask[3]) | (alphamask[4] << 8) | (alphamask[5] << 16);
				for (j = 2; j < 4; j++) {
					for (i = 0; i < 4; i++) {
						// only put pixels out < width or height
						if (((x + i) < lImage->Width) && ((y + j) < lImage->Height)) {
							Offset = z * lImage->SizeOfPlane + (y + j) * lImage->Bps + (x + i) * lImage->Bpp + 3;
							lImage->Data[Offset] = alphas[bits & 0x07];
						}
						bits >>= 3;
					}
				}
			}
		}
	}

	return IL_TRUE;
}


ILboolean Decompress3Dc()
{
	int			x, y, z, i, j, k, t1, t2;
	ILubyte		*Temp, *Temp2;
	ILubyte		XColours[8], YColours[8];
	ILuint		bitmask, bitmask2, Offset, CurrOffset;

	if (!CompData)
		return IL_FALSE;

	Temp = CompData;
	Offset = 0;
	for (z = 0; z < Depth; z++) {
		for (y = 0; y < Height; y += 4) {
			for (x = 0; x < Width; x += 4) {
				Temp2 = Temp + 8;

				//Read Y palette
				t1 = YColours[0] = Temp[0];
				t2 = YColours[1] = Temp[1];
				Temp += 2;
				if (t1 > t2)
					for (i = 2; i < 8; ++i)
						YColours[i] = t1 + ((t2 - t1)*(i - 1))/7;
				else {
					for (i = 2; i < 6; ++i)
						YColours[i] = t1 + ((t2 - t1)*(i - 1))/5;
					YColours[6] = 0;
					YColours[7] = 255;
				}

				// Read X palette
				t1 = XColours[0] = Temp2[0];
				t2 = XColours[1] = Temp2[1];
				Temp2 += 2;
				if (t1 > t2)
					for (i = 2; i < 8; ++i)
						XColours[i] = t1 + ((t2 - t1)*(i - 1))/7;
				else {
					for (i = 2; i < 6; ++i)
						XColours[i] = t1 + ((t2 - t1)*(i - 1))/5;
					XColours[6] = 0;
					XColours[7] = 255;
				}

				//decompress pixel data
				CurrOffset = Offset;
				for (k = 0; k < 4; k += 2) {
					// First three bytes
					bitmask = ((ILuint)(Temp[0]) << 0) | ((ILuint)(Temp[1]) << 8) | ((ILuint)(Temp[2]) << 16);
					bitmask2 = ((ILuint)(Temp2[0]) << 0) | ((ILuint)(Temp2[1]) << 8) | ((ILuint)(Temp2[2]) << 16);
					for (j = 0; j < 2; j++) {
						// only put pixels out < height
						if ((y + k + j) < Height) {
							for (i = 0; i < 4; i++) {
								// only put pixels out < width
								if (((x + i) < Width)) {
									ILint t, tx, ty;

									t1 = CurrOffset + (x + i)*3;
									Image->Data[t1 + 1] = ty = YColours[bitmask & 0x07];
									Image->Data[t1 + 0] = tx = XColours[bitmask2 & 0x07];

									//calculate b (z) component ((r/255)^2 + (g/255)^2 + (b/255)^2 = 1
									t = 127*128 - (tx - 127)*(tx - 128) - (ty - 127)*(ty - 128);
									if (t > 0)
										Image->Data[t1 + 2] = (ILubyte)(iSqrt(t) + 128);
									else
										Image->Data[t1 + 2] = 0x7F;
								}
								bitmask >>= 3;
								bitmask2 >>= 3;
							}
							CurrOffset += Image->Bps;
						}
					}
					Temp += 3;
					Temp2 += 3;
				}

				//skip bytes that were read via Temp2
				Temp += 8;
			}
			Offset += Image->Bps*4;
		}
	}

	return IL_TRUE;
}


ILboolean DecompressAti1n()
{
	int			x, y, z, i, j, k, t1, t2;
	ILubyte		*Temp;
	ILubyte		Colours[8];
	ILuint		bitmask, Offset, CurrOffset;

	if (!CompData)
		return IL_FALSE;

	Temp = CompData;
	Offset = 0;
	for (z = 0; z < Depth; z++) {
		for (y = 0; y < Height; y += 4) {
			for (x = 0; x < Width; x += 4) {
				//Read palette
				t1 = Colours[0] = Temp[0];
				t2 = Colours[1] = Temp[1];
				Temp += 2;
				if (t1 > t2)
					for (i = 2; i < 8; ++i)
						Colours[i] = t1 + ((t2 - t1)*(i - 1))/7;
				else {
					for (i = 2; i < 6; ++i)
						Colours[i] = t1 + ((t2 - t1)*(i - 1))/5;
					Colours[6] = 0;
					Colours[7] = 255;
				}

				//decompress pixel data
				CurrOffset = Offset;
				for (k = 0; k < 4; k += 2) {
					// First three bytes
					bitmask = ((ILuint)(Temp[0]) << 0) | ((ILuint)(Temp[1]) << 8) | ((ILuint)(Temp[2]) << 16);
					for (j = 0; j < 2; j++) {
						// only put pixels out < height
						if ((y + k + j) < Height) {
							for (i = 0; i < 4; i++) {
								// only put pixels out < width
								if (((x + i) < Width)) {
									t1 = CurrOffset + (x + i);
									Image->Data[t1] = Colours[bitmask & 0x07];
								}
								bitmask >>= 3;
							}
							CurrOffset += Image->Bps;
						}
					}
					Temp += 3;
				}
			}
			Offset += Image->Bps*4;
		}
	}

	return IL_TRUE;
}


//This is nearly exactly the same as DecompressDXT5...
//I have to clean up this file (put common code in
//helper functions etc)
ILboolean DecompressRXGB()
{
	int			x, y, z, i, j, k, Select;
	ILubyte		*Temp;
	Color565	*color_0, *color_1;
	Color8888	colours[4], *col;
	ILuint		bitmask, Offset;
	ILubyte		alphas[8], *alphamask;
	ILuint		bits;

	if (!CompData)
		return IL_FALSE;

	Temp = CompData;
	for (z = 0; z < Depth; z++) {
		for (y = 0; y < Height; y += 4) {
			for (x = 0; x < Width; x += 4) {
				if (y >= Height || x >= Width)
					break;
				alphas[0] = Temp[0];
				alphas[1] = Temp[1];
				alphamask = Temp + 2;
				Temp += 8;
				color_0 = ((Color565*)Temp);
				color_1 = ((Color565*)(Temp+2));
				bitmask = ((ILuint*)Temp)[1];
				Temp += 8;

				colours[0].r = color_0->nRed << 3;
				colours[0].g = color_0->nGreen << 2;
				colours[0].b = color_0->nBlue << 3;
				colours[0].a = 0xFF;

				colours[1].r = color_1->nRed << 3;
				colours[1].g = color_1->nGreen << 2;
				colours[1].b = color_1->nBlue << 3;
				colours[1].a = 0xFF;

				// Four-color block: derive the other two colors.    
				// 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
				// These 2-bit codes correspond to the 2-bit fields 
				// stored in the 64-bit block.
				colours[2].b = (2 * colours[0].b + colours[1].b + 1) / 3;
				colours[2].g = (2 * colours[0].g + colours[1].g + 1) / 3;
				colours[2].r = (2 * colours[0].r + colours[1].r + 1) / 3;
				colours[2].a = 0xFF;

				colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
				colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
				colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;
				colours[3].a = 0xFF;

				k = 0;
				for (j = 0; j < 4; j++) {
					for (i = 0; i < 4; i++, k++) {

						Select = (bitmask & (0x03 << k*2)) >> k*2;
						col = &colours[Select];

						// only put pixels out < width or height
						if (((x + i) < Width) && ((y + j) < Height)) {
							Offset = z * Image->SizeOfPlane + (y + j) * Image->Bps + (x + i) * Image->Bpp;
							Image->Data[Offset + 0] = col->r;
							Image->Data[Offset + 1] = col->g;
							Image->Data[Offset + 2] = col->b;
						}
					}
				}

				// 8-alpha or 6-alpha block?    
				if (alphas[0] > alphas[1]) {    
					// 8-alpha block:  derive the other six alphas.    
					// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
					alphas[2] = (6 * alphas[0] + 1 * alphas[1] + 3) / 7;	// bit code 010
					alphas[3] = (5 * alphas[0] + 2 * alphas[1] + 3) / 7;	// bit code 011
					alphas[4] = (4 * alphas[0] + 3 * alphas[1] + 3) / 7;	// bit code 100
					alphas[5] = (3 * alphas[0] + 4 * alphas[1] + 3) / 7;	// bit code 101
					alphas[6] = (2 * alphas[0] + 5 * alphas[1] + 3) / 7;	// bit code 110
					alphas[7] = (1 * alphas[0] + 6 * alphas[1] + 3) / 7;	// bit code 111
				}
				else {
					// 6-alpha block.
					// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
					alphas[2] = (4 * alphas[0] + 1 * alphas[1] + 2) / 5;	// Bit code 010
					alphas[3] = (3 * alphas[0] + 2 * alphas[1] + 2) / 5;	// Bit code 011
					alphas[4] = (2 * alphas[0] + 3 * alphas[1] + 2) / 5;	// Bit code 100
					alphas[5] = (1 * alphas[0] + 4 * alphas[1] + 2) / 5;	// Bit code 101
					alphas[6] = 0x00;										// Bit code 110
					alphas[7] = 0xFF;										// Bit code 111
				}

				// Note: Have to separate the next two loops,
				//	it operates on a 6-byte system.
				// First three bytes
				bits = *((ILint*)alphamask);
				for (j = 0; j < 2; j++) {
					for (i = 0; i < 4; i++) {
						// only put pixels out < width or height
						if (((x + i) < Width) && ((y + j) < Height)) {
							Offset = z * Image->SizeOfPlane + (y + j) * Image->Bps + (x + i) * Image->Bpp + 0;
							Image->Data[Offset] = alphas[bits & 0x07];
						}
						bits >>= 3;
					}
				}

				// Last three bytes
				bits = *((ILint*)&alphamask[3]);
				for (j = 2; j < 4; j++) {
					for (i = 0; i < 4; i++) {
						// only put pixels out < width or height
						if (((x + i) < Width) && ((y + j) < Height)) {
							Offset = z * Image->SizeOfPlane + (y + j) * Image->Bps + (x + i) * Image->Bpp + 0;
							Image->Data[Offset] = alphas[bits & 0x07];
						}
						bits >>= 3;
					}
				}
			}
		}
	}

	return IL_TRUE;
}


//Taken from OpenEXR
unsigned int
halfToFloat (unsigned short y)
{
	int s = (y >> 15) & 0x00000001;
	int e = (y >> 10) & 0x0000001f;
	int m =  y		  & 0x000003ff;

	if (e == 0)
	{
		if (m == 0)
		{
			//
			// Plus or minus zero
			//
			return s << 31;
		}
		else
		{
			//
			// Denormalized number -- renormalize it
			//
			while (!(m & 0x00000400))
			{
				m <<= 1;
				e -=  1;
			}

			e += 1;
			m &= ~0x00000400;
		}
	}
	else if (e == 31)
	{
		if (m == 0)
		{
			//
			// Positive or negative infinity
			//
			return (s << 31) | 0x7f800000;
		}
		else
		{
			//
			// Nan -- preserve sign and significand bits
			//
			return (s << 31) | 0x7f800000 | (m << 13);
		}
	}

	//
	// Normalized number
	//
	e = e + (127 - 15);
	m = m << 13;

	//
	// Assemble s, e and m.
	//
	return (s << 31) | (e << 23) | m;
}


ILboolean iConvFloat16ToFloat32(ILuint* dest, ILushort* src, ILuint size)
{
	ILuint i;
	for (i = 0; i < size; ++i, ++dest, ++src) {
		//float: 1 sign bit, 8 exponent bits, 23 mantissa bits
		//half: 1 sign bit, 5 exponent bits, 10 mantissa bits
		*dest = halfToFloat(*src);
	}

	return IL_TRUE;
}


// Same as iConvFloat16ToFloat32, but we have to set the blue channel to 1.0f.
//  The destination format is RGB, and the source is R16G16 (little endian).
ILboolean iConvG16R16ToFloat32(ILuint* dest, ILushort* src, ILuint size)
{
	ILuint i;
	for (i = 0; i < size; i += 3) {
		//float: 1 sign bit, 8 exponent bits, 23 mantissa bits
		//half: 1 sign bit, 5 exponent bits, 10 mantissa bits
		*dest++ = halfToFloat(*src++);
		*dest++ = halfToFloat(*src++);
		*((ILfloat*)dest++) = 1.0f;
	}

	return IL_TRUE;
}


// Same as iConvFloat16ToFloat32, but we have to set the green and blue channels
//  to 1.0f.  The destination format is RGB, and the source is R16.
ILboolean iConvR16ToFloat32(ILuint* dest, ILushort* src, ILuint size)
{
	ILuint i;
	for (i = 0; i < size; i += 3) {
		//float: 1 sign bit, 8 exponent bits, 23 mantissa bits
		//half: 1 sign bit, 5 exponent bits, 10 mantissa bits
		*dest++ = halfToFloat(*src++);
		*((ILfloat*)dest++) = 1.0f;
		*((ILfloat*)dest++) = 1.0f;
	}

	return IL_TRUE;
}


ILboolean DecompressFloat(ILuint lCompFormat)
{
	ILuint i, j, Size;

	switch (lCompFormat)
	{
		case PF_R32F:  // Red float, green = blue = max
			Size = Image->Width * Image->Height * Image->Depth * 3;
			for (i = 0, j = 0; i < Size; i += 3, j++) {
				((ILfloat*)Image->Data)[i] = ((ILfloat*)CompData)[j];
				((ILfloat*)Image->Data)[i+1] = 1.0f;
				((ILfloat*)Image->Data)[i+2] = 1.0f;
			}
			return IL_TRUE;
		case PF_A32B32G32R32F:  // Direct copy of float RGBA data
			memcpy(Image->Data, CompData, Image->SizeOfData);
			return IL_TRUE;
		case PF_G32R32F:  // Red float, green float, blue = max
			Size = Image->Width * Image->Height * Image->Depth * 3;
			for (i = 0, j = 0; i < Size; i += 3, j += 2) {
				((ILfloat*)Image->Data)[i] = ((ILfloat*)CompData)[j];
				((ILfloat*)Image->Data)[i+1] = ((ILfloat*)CompData)[j+1];
				((ILfloat*)Image->Data)[i+2] = 1.0f;
			}
			return IL_TRUE;
		case PF_R16F:  // Red float, green = blue = max
			return iConvR16ToFloat32((ILuint*)Image->Data, (ILushort*)CompData,
				Image->Width * Image->Height * Image->Depth * Image->Bpp);
		case PF_A16B16G16R16F:  // Just convert from half to float.
			return iConvFloat16ToFloat32((ILuint*)Image->Data, (ILushort*)CompData,
				Image->Width * Image->Height * Image->Depth * Image->Bpp);
		case PF_G16R16F:  // Convert from half to float, set blue = max.
			return iConvG16R16ToFloat32((ILuint*)Image->Data, (ILushort*)CompData,
				Image->Width * Image->Height * Image->Depth * Image->Bpp);
		default:
			return IL_FALSE;
	}
}


void CorrectPreMult()
{
	ILuint i;

	for (i = 0; i < Image->SizeOfData; i += 4) {
		if (Image->Data[i+3] != 0) {  // Cannot divide by 0.
			Image->Data[i]   = (ILubyte)(((ILuint)Image->Data[i]   << 8) / Image->Data[i+3]);
			Image->Data[i+1] = (ILubyte)(((ILuint)Image->Data[i+1] << 8) / Image->Data[i+3]);
			Image->Data[i+2] = (ILubyte)(((ILuint)Image->Data[i+2] << 8) / Image->Data[i+3]);
		}
	}

	return;
}


ILboolean DecompressARGB(ILuint CompFormat)
{
	ILuint ReadI = 0, TempBpp, i;
	ILuint RedL, RedR;
	ILuint GreenL, GreenR;
	ILuint BlueL, BlueR;
	ILuint AlphaL, AlphaR;
	ILubyte	*Temp;

	if (Has16BitComponents)
		return DecompressARGB16(CompFormat);

	if (!CompData)
		return IL_FALSE;

	if (CompFormat == PF_LUMINANCE && Head.RGBBitCount == 16 && Head.RBitMask == 0xFFFF) { //HACK
		memcpy(Image->Data, CompData, Image->SizeOfData);
		return IL_TRUE;
	}

	GetBitsFromMask(Head.RBitMask, &RedL, &RedR);
	GetBitsFromMask(Head.GBitMask, &GreenL, &GreenR);
	GetBitsFromMask(Head.BBitMask, &BlueL, &BlueR);
	GetBitsFromMask(Head.RGBAlphaBitMask, &AlphaL, &AlphaR);
	Temp = CompData;
	TempBpp = Head.RGBBitCount / 8;

	for (i = 0; i < Image->SizeOfData; i += Image->Bpp) {

		//@TODO: This is SLOOOW...
		//but the old version crashed in release build under
		//winxp (and xp is right to stop this code - I always
		//wondered that it worked the old way at all)
		if (Image->SizeOfData - i < 4) { //less than 4 byte to write?
			if (TempBpp == 3) { //this branch is extra-SLOOOW
				ReadI =
					*Temp
					| ((*(Temp + 1)) << 8)
					| ((*(Temp + 2)) << 16);
			}
			else if (TempBpp == 1)
				ReadI = *((ILubyte*)Temp);
			else if (TempBpp == 2)
				ReadI = Temp[0] | (Temp[1] << 8);
		}
		else
			ReadI = Temp[0] | (Temp[1] << 8) | (Temp[2] << 16) | (Temp[3] << 24);
		Temp += TempBpp;

		Image->Data[i] = ((ReadI & Head.RBitMask) >> RedR) << RedL;

		if (Image->Bpp >= 3) {
			Image->Data[i+1] = ((ReadI & Head.GBitMask) >> GreenR) << GreenL;
			Image->Data[i+2] = ((ReadI & Head.BBitMask) >> BlueR) << BlueL;

			if (Image->Bpp == 4) {
				Image->Data[i+3] = ((ReadI & Head.RGBAlphaBitMask) >> AlphaR) << AlphaL;
				if (AlphaL >= 7) {
					Image->Data[i+3] = Image->Data[i+3] ? 0xFF : 0x00;
				}
				else if (AlphaL >= 4) {
					Image->Data[i+3] = Image->Data[i+3] | (Image->Data[i+3] >> 4);
				}
			}
		}
		else if (Image->Bpp == 2) {
			Image->Data[i+1] = ((ReadI & Head.RGBAlphaBitMask) >> AlphaR) << AlphaL;
			if (AlphaL >= 7) {
				Image->Data[i+1] = Image->Data[i+1] ? 0xFF : 0x00;
			}
			else if (AlphaL >= 4) {
				Image->Data[i+1] = Image->Data[i+1] | (Image->Data[i+3] >> 4);
			}
		}
	}

	return IL_TRUE;
}


// This function simply counts how many contiguous bits are in the mask.
ILuint CountBitsFromMask(ILuint Mask)
{
	ILuint		i, TestBit = 0x01, Count = 0;
	ILboolean	FoundBit = IL_FALSE;

	for (i = 0; i < 32; i++, TestBit <<= 1) {
		if (Mask & TestBit) {
			if (!FoundBit)
				FoundBit = IL_TRUE;
			Count++;
		}
		else if (FoundBit)
			return Count;
	}

	return Count;
}


// Same as DecompressARGB, but it works on images with more than 8 bits
//  per channel, such as a2r10g10b10 and a2b10g10r10.
ILboolean DecompressARGB16(ILuint CompFormat)
{
	ILuint ReadI = 0, TempBpp, i;
	ILuint RedL, RedR;
	ILuint GreenL, GreenR;
	ILuint BlueL, BlueR;
	ILuint AlphaL, AlphaR;
	ILuint RedPad, GreenPad, BluePad, AlphaPad;
	ILubyte	*Temp;

	if (!CompData)
		return IL_FALSE;

	GetBitsFromMask(Head.RBitMask, &RedL, &RedR);
	GetBitsFromMask(Head.GBitMask, &GreenL, &GreenR);
	GetBitsFromMask(Head.BBitMask, &BlueL, &BlueR);
	GetBitsFromMask(Head.RGBAlphaBitMask, &AlphaL, &AlphaR);
	RedPad   = 16 - CountBitsFromMask(Head.RBitMask);
	GreenPad = 16 - CountBitsFromMask(Head.GBitMask);
	BluePad  = 16 - CountBitsFromMask(Head.BBitMask);
	AlphaPad = 16 - CountBitsFromMask(Head.RGBAlphaBitMask);

	RedL = RedL + RedPad;
	GreenL = GreenL + GreenPad;
	BlueL = BlueL + BluePad;
	AlphaL = AlphaL + AlphaPad;

	Temp = CompData;
	TempBpp = Head.RGBBitCount / 8;

	for (i = 0; i < Image->SizeOfData / 2; i += Image->Bpp) {

		//@TODO: This is SLOOOW...
		//but the old version crashed in release build under
		//winxp (and xp is right to stop this code - I always
		//wondered that it worked the old way at all)
		if (Image->SizeOfData - i < 4) { //less than 4 byte to write?
			if (TempBpp == 3) { //this branch is extra-SLOOOW
				ReadI =
					*Temp
					| ((*(Temp + 1)) << 8)
					| ((*(Temp + 2)) << 16);
			}
			else if (TempBpp == 1)
				ReadI = *((ILubyte*)Temp);
			else if (TempBpp == 2)
				ReadI = Temp[0] | (Temp[1] << 8);
		}
		else
			ReadI = Temp[0] | (Temp[1] << 8) | (Temp[2] << 16) | (Temp[3] << 24);
		Temp += TempBpp;

		((ILushort*)Image->Data)[i+2] = ((ReadI & Head.RBitMask) >> RedR) << RedL;

		if (Image->Bpp >= 3) {
			((ILushort*)Image->Data)[i+1] = ((ReadI & Head.GBitMask) >> GreenR) << GreenL;
			((ILushort*)Image->Data)[i] = ((ReadI & Head.BBitMask) >> BlueR) << BlueL;

			if (Image->Bpp == 4) {
				((ILushort*)Image->Data)[i+3] = ((ReadI & Head.RGBAlphaBitMask) >> AlphaR) << AlphaL;
				if (AlphaL >= 7) {
					((ILushort*)Image->Data)[i+3] = ((ILushort*)Image->Data)[i+3] ? 0xFF : 0x00;
				}
				else if (AlphaL >= 4) {
					((ILushort*)Image->Data)[i+3] = ((ILushort*)Image->Data)[i+3] | (((ILushort*)Image->Data)[i+3] >> 4);
				}
			}
		}
		else if (Image->Bpp == 2) {
			((ILushort*)Image->Data)[i+1] = ((ReadI & Head.RGBAlphaBitMask) >> AlphaR) << AlphaL;
			if (AlphaL >= 7) {
				((ILushort*)Image->Data)[i+1] = ((ILushort*)Image->Data)[i+1] ? 0xFF : 0x00;
			}
			else if (AlphaL >= 4) {
				((ILushort*)Image->Data)[i+1] = ((ILushort*)Image->Data)[i+1] | (Image->Data[i+3] >> 4);
			}
		}
	}

	return IL_TRUE;
}


// @TODO:  Look at using the BSF/BSR operands for inline ASM here.
void GetBitsFromMask(ILuint Mask, ILuint *ShiftLeft, ILuint *ShiftRight)
{
	ILuint Temp, i;

	if (Mask == 0) {
		*ShiftLeft = *ShiftRight = 0;
		return;
	}

	Temp = Mask;
	for (i = 0; i < 32; i++, Temp >>= 1) {
		if (Temp & 1)
			break;
	}
	*ShiftRight = i;

	// Temp is preserved, so use it again:
	for (i = 0; i < 8; i++, Temp >>= 1) {
		if (!(Temp & 1))
			break;
	}
	*ShiftLeft = 8 - i;

	return;
}


//
//
// DXT extension code
//
//
ILubyte* ILAPIENTRY ilGetDxtcData()
{
	if (iCurImage == NULL) {
		ilSetError(IL_INTERNAL_ERROR);
		return NULL;
	}
	return iCurImage->DxtcData;
}

void ilFreeSurfaceDxtcData()
{
	if (iCurImage != NULL && iCurImage->DxtcData != NULL) {
		ifree(iCurImage->DxtcData);
		iCurImage->DxtcData = NULL;
		iCurImage->DxtcSize = 0;
		iCurImage->DxtcFormat = IL_DXT_NO_COMP;
	}
}

void ilFreeImageDxtcData()
{
	ILint i, j;
	ILuint ImgID = ilGetInteger(IL_CUR_IMAGE);
	ILint ImgCount = ilGetInteger(IL_NUM_IMAGES);
	ILint MipCount;

	for(i = 0; i <= ImgCount; ++i) {
		ilBindImage(ImgID);
		ilActiveImage(i);

		MipCount = ilGetInteger(IL_NUM_MIPMAPS);
		for(j = 0; j <= MipCount; ++j) {
			ilBindImage(ImgID);
			ilActiveImage(i);
			ilActiveMipmap(j);

			ilFreeSurfaceDxtcData();
		}
	}
}

/*
 * This assumes DxtcData, DxtcFormat, width, height, and depth are valid
 */
ILAPI ILboolean ILAPIENTRY ilDxtcDataToSurface()
{
	ILuint CompFormat;

	if (iCurImage == NULL || iCurImage->DxtcData == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	if (!(iCurImage->DxtcFormat == IL_DXT1 || iCurImage->DxtcFormat == IL_DXT3
		|| iCurImage->DxtcFormat == IL_DXT5)) {
		ilSetError(IL_INVALID_PARAM); //TODO
		return IL_FALSE;
	}

	//@TODO: is this right for all dxt formats? works for
	//  DXT1, 3, 5
	iCurImage->Bpp = 4;
	iCurImage->Bpc = 1;
	iCurImage->Bps = iCurImage->Width*iCurImage->Bpp*iCurImage->Bpc;
	iCurImage->SizeOfPlane = iCurImage->Height*iCurImage->Bps;
	iCurImage->Format = IL_RGBA;
	iCurImage->Type = IL_UNSIGNED_BYTE;

	if (iCurImage->SizeOfData != iCurImage->SizeOfPlane*iCurImage->Depth) {
		iCurImage->SizeOfData = iCurImage->Depth*iCurImage->SizeOfPlane;
		if (iCurImage->Data != NULL)
			ifree(iCurImage->Data);
		iCurImage->Data = NULL;
	}

	if (iCurImage->Data == NULL) {
		iCurImage->Data = ialloc(iCurImage->SizeOfData);
	}

	Image = iCurImage;
	Width = iCurImage->Width;
	Height = iCurImage->Height;
	Depth = iCurImage->Depth;
	switch(iCurImage->DxtcFormat)
	{
		case IL_DXT1: CompFormat = PF_DXT1; break;
		case IL_DXT3: CompFormat = PF_DXT3; break;
		case IL_DXT5: CompFormat = PF_DXT5; break;
	}
	CompData = iCurImage->DxtcData;
	DdsDecompress(CompFormat); //globals suck...fix this

	//@TODO: origin should be set in Decompress()...
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;
	return ilFixCur();
}


ILAPI ILboolean ILAPIENTRY ilDxtcDataToImage()
{
	ILint i, j;
	ILuint ImgID = ilGetInteger(IL_CUR_IMAGE);
	ILint ImgCount = ilGetInteger(IL_NUM_IMAGES);
	ILint MipCount;
	ILboolean ret = IL_TRUE;

	for(i = 0; i <= ImgCount; ++i) {
		ilBindImage(ImgID);
		ilActiveImage(i);

		MipCount = ilGetInteger(IL_NUM_MIPMAPS);
		for(j = 0; j <= MipCount; ++j) {
			ilBindImage(ImgID);
			ilActiveImage(i);
			ilActiveMipmap(j);

			if (!ilDxtcDataToSurface())
				ret = IL_FALSE;
		}
	}
    ilBindImage(ImgID);

	return ret;
}


ILAPI ILboolean ILAPIENTRY ilSurfaceToDxtcData(ILenum Format)
{
	ILuint Size;
	void* Data;
	ilFreeSurfaceDxtcData();

	Size = ilGetDXTCData(NULL, 0, Format);
	if (Size == 0) {
		return IL_FALSE;
	}

	Data = ialloc(Size);
    
	if (Data == NULL)
		return IL_FALSE;
            
	ilGetDXTCData(Data, Size, Format);

	//These have to be after the call to ilGetDXTCData()
	iCurImage->DxtcData = Data;
	iCurImage->DxtcFormat = Format;
	iCurImage->DxtcSize = Size;

	return IL_TRUE;
}


ILAPI ILboolean ILAPIENTRY ilImageToDxtcData(ILenum Format)
{
	ILint i, j;
	ILuint ImgID = ilGetInteger(IL_CUR_IMAGE);
	ILint ImgCount = ilGetInteger(IL_NUM_IMAGES);
	ILint MipCount;
	ILboolean ret = IL_TRUE;

	for (i = 0; i <= ImgCount; ++i) {
		ilBindImage(ImgID);
		ilActiveImage(i);

		MipCount = ilGetInteger(IL_NUM_MIPMAPS);
		for(j = 0; j <= MipCount; ++j) {
			ilBindImage(ImgID);
			ilActiveImage(i);
			ilActiveMipmap(j);

			if (!ilSurfaceToDxtcData(Format))
				ret = IL_FALSE;
		}
	}

	return ret;
}


//works like ilTexImage(), ie. destroys mipmaps etc (which sucks, but
//is consistent. There should be a ilTexSurface() and ilTexSurfaceDxtc()
//functions as well, but for now this is sufficient)
ILAPI ILboolean ILAPIENTRY ilTexImageDxtc(ILint w, ILint h, ILint d, ILenum DxtFormat, const ILubyte* data)
{
	ILimage* Image = iCurImage;

	ILint xBlocks, yBlocks, BlockSize, LineSize, DataSize;


	//The next few lines are copied from ilTexImage() and ilInitImage() -
	//should be factored in more reusable functions...
	if (Image == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	////

	// Not sure if we should be getting rid of the palette...
	if (Image->Pal.Palette && Image->Pal.PalSize && Image->Pal.PalType != IL_PAL_NONE) {
		ifree(Image->Pal.Palette);
	}

	// These are set NULL later by the memset call.
	ilCloseImage(Image->Mipmaps);
	ilCloseImage(Image->Next);
	ilCloseImage(Image->Faces);
	ilCloseImage(Image->Layers);

	if (Image->AnimList) ifree(Image->AnimList);
	if (Image->Profile)  ifree(Image->Profile);
	if (Image->DxtcData) ifree(Image->DxtcData);
	if (Image->Data)	 ifree(Image->Data);


	////

	memset(Image, 0, sizeof(ILimage));
	Image->Width	   = w;
	Image->Height	   = h;
	Image->Depth	   = d;

	//TODO: What about origin with dxtc data?
	Image->Origin	   = IL_ORIGIN_LOWER_LEFT;
	Image->Pal.PalType = IL_PAL_NONE;

    // Allocate DXT data buffer
	xBlocks = (w + 3)/4;
	yBlocks = (h + 3)/4;
	if (DxtFormat == IL_DXT1)
		BlockSize = 8;
	else
		BlockSize = 16;

	LineSize = xBlocks * BlockSize;

	DataSize = yBlocks * LineSize * d;

	Image->DxtcFormat  = DxtFormat;
        Image->DxtcSize = DataSize;
	Image->DxtcData    = ialloc(DataSize);

	if (Image->DxtcData == NULL) {
		return IL_FALSE;
	}

	if (data != NULL)
		memcpy(Image->DxtcData, data, DataSize);

	return IL_TRUE;
}


/* ------------------------------------------------------------------- */

void iFlipColorBlock(ILubyte *data)
{
    ILubyte tmp;

    tmp = data[4];
    data[4] = data[7];
    data[7] = tmp;

    tmp = data[5];
    data[5] = data[6];
    data[6] = tmp;
}

void iFlipSimpleAlphaBlock(ILushort *data)
{
	ILushort tmp;

	tmp = data[0];
	data[0] = data[3];
	data[3] = tmp;

	tmp = data[1];
	data[1] = data[2];
	data[2] = tmp;
}

void iComplexAlphaHelper(ILubyte* Data)
{
	ILushort tmp[2];

	//one 4 pixel line is 12 bit, copy each line into
	//a ushort, swap them and copy back
	tmp[0] = (Data[0] | (Data[1] << 8)) & 0xfff;
	tmp[1] = ((Data[1] >> 4) | (Data[2] << 4)) & 0xfff;

	Data[0] = (ILubyte)tmp[1];
	Data[1] = (tmp[1] >> 8) | (tmp[0] << 4);
	Data[2] = tmp[0] >> 4;
}

void iFlipComplexAlphaBlock(ILubyte *Data)
{
	ILubyte tmp[3];
	Data += 2; //Skip 'palette'

	//swap upper two rows with lower two rows
	memcpy(tmp, Data, 3);
	memcpy(Data, Data + 3, 3);
	memcpy(Data + 3, tmp, 3);

	//swap 1st with 2nd row, 3rd with 4th
	iComplexAlphaHelper(Data);
	iComplexAlphaHelper(Data + 3);
}

void iFlipDxt1(ILubyte* data, ILuint count)
{
	ILuint i;

	for (i = 0; i < count; ++i) {
		iFlipColorBlock(data);
		data += 8; //advance to next block
	}
}

void iFlipDxt3(ILubyte* data, ILuint count)
{
	ILuint i;
	for (i = 0; i < count; ++i) {
		iFlipSimpleAlphaBlock((ILushort*)data);
		iFlipColorBlock(data + 8);
		data += 16; //advance to next block
	}
}

void iFlipDxt5(ILubyte* data, ILuint count)
{
	ILuint i;
	for (i = 0; i < count; ++i) {
		iFlipComplexAlphaBlock(data);
		iFlipColorBlock(data + 8);
		data += 16; //advance to next block
	}
}

void iFlip3dc(ILubyte* data, ILuint count)
{
	ILuint i;
	for (i = 0; i < count; ++i) {
		iFlipComplexAlphaBlock(data);
		iFlipComplexAlphaBlock(data + 8);
		data += 16; //advance to next block
	}
}


ILAPI void ILAPIENTRY ilFlipSurfaceDxtcData()
{
	ILuint y, z;
	ILuint BlockSize, LineSize;
	ILubyte *Temp, *Runner, *Top, *Bottom;
	ILuint numXBlocks, numYBlocks;
	void (*FlipBlocks)(ILubyte* data, ILuint count);

	if (iCurImage == NULL || iCurImage->DxtcData == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return;
	}

	numXBlocks = (iCurImage->Width + 3)/4;
	numYBlocks = (iCurImage->Height + 3)/4;
	
	switch (iCurImage->DxtcFormat)
	{
		case IL_DXT1:
			BlockSize = 8;
			FlipBlocks = iFlipDxt1;
			break;
		case IL_DXT2:
		case IL_DXT3:
			BlockSize = 16;
			FlipBlocks = iFlipDxt3;
			break;
		case IL_DXT4:
		case IL_DXT5:
		case IL_RXGB:
			BlockSize = 16;
			FlipBlocks = iFlipDxt5;
			break;
		case IL_3DC:
			BlockSize = 16;
			FlipBlocks = iFlip3dc;
			break;
		default:
			ilSetError(IL_INVALID_PARAM);
			return;
	}
	
	LineSize = numXBlocks * BlockSize;
	Temp = ialloc(LineSize);

	if (Temp == NULL)
	    return;

	Runner = iCurImage->DxtcData;
	for (z = 0; z < iCurImage->Depth; ++z) {
		Top = Runner;
		Bottom = Runner + (numYBlocks - 1)*LineSize;
	    
		for (y = 0; y < numYBlocks/2; ++y) {
			//swap block row
			memcpy(Temp, Top, LineSize);
			memcpy(Top, Bottom, LineSize);
			memcpy(Bottom, Temp, LineSize);


			//swap blocks
			FlipBlocks(Top, numXBlocks);
			FlipBlocks(Bottom, numXBlocks);

			Top += LineSize;
			Bottom -= LineSize;
		}

		//middle line
		if (numYBlocks % 2 != 0)
			FlipBlocks(Top, numXBlocks);

		Runner += LineSize * numYBlocks;
	}

	ifree(Temp);
}

/**********************************************************************/

void iInvertDxt3Alpha(ILubyte *data)
{
	ILint i;

	for (i = 0; i < 8; ++i) {
		/*
		ILubyte b, t1, t2;
		b = data[i];

		t1 = b & 0xf;
		t1 = 15 - t1;
		t2 = b >> 4;
		t2 = 15 - t2;

		data[i] = (t2 << 4) | t1;
		*/
		//simpler:
		data[i] = ~data[i];
	}
}

void iInvertDxt5Alpha(ILubyte *data)
{
	ILubyte a0, a1;
	ILint i, j;
	const ILubyte map1[] = { 1, 0, 7, 6, 5, 4, 3, 2 };
	const ILubyte map2[] = { 1, 0, 5, 4, 3, 2, 7, 6 };


	a0 = data[0];
	a1 = data[1];

	//a0 > a1 <=> 255 - a0 < 255 - a1. Because of this,
	//a1 and a2 have to be swapped, and the indices
	//have to be changed as well.

	//invert and swap alpha
	data[0] = 255 - a1;
	data[1] = 255 - a0;
	data += 2;

	//fix indices
	for (i = 0; i < 6; i += 3) {
		ILuint in = data[i] | (data[i+1] << 8) | (data[i+2] << 16);
		ILuint out = 0;

		for (j = 0; j < 24; j += 3) {
			ILubyte b = (in >> j) & 0x7;

			if (a0 > a1)
				b = map1[b];
			else
				b = map2[b];

			out |= b << j;
		}

		data[i] = out;
		data[i+1] = out >> 8;
		data[i+2] = out >> 16;
	}
}


ILAPI ILboolean ILAPIENTRY ilInvertSurfaceDxtcDataAlpha()
{
	ILint i;
	ILuint BlockSize;
	ILubyte *Runner;
	ILint numXBlocks, numYBlocks, numBlocks;
	void (*InvertAlpha)(ILubyte* data);

	if (iCurImage == NULL || iCurImage->DxtcData == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	numXBlocks = (iCurImage->Width + 3)/4;
	numYBlocks = (iCurImage->Height + 3)/4;
	numBlocks = numXBlocks*numYBlocks*iCurImage->Depth;
	BlockSize = 16;

	switch (iCurImage->DxtcFormat)
	{
		case IL_DXT3:
			InvertAlpha = iInvertDxt3Alpha;
			break;
		case IL_DXT5:
			InvertAlpha = iInvertDxt5Alpha;
			break;
		default:
			//DXT2/4 are not supported yet because nobody
			//uses them anyway and I would have to change
			//the color blocks as well...
			//DXT1 is not supported because DXT1 alpha is
			//seldom used and it's not easily invertable.
			ilSetError(IL_INVALID_PARAM);
			return IL_FALSE;
	}

	Runner = iCurImage->DxtcData;
	for (i = 0; i < numBlocks; ++i, Runner += BlockSize) {
		InvertAlpha(Runner);
	}

	return IL_TRUE;
}




#endif//IL_NO_DDS
