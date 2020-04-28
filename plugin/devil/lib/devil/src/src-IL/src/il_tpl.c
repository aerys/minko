//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_tpl.c
//
// Description: Reads from a Gamecube Texture Palette (.tpl).
//                Specifications were found at 
//                http://pabut.homeip.net:8000/yagcd/chap14.html.
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_TPL
#include "il_dds.h"


typedef struct TPLHEAD
{
	ILuint	Magic;
	ILuint	nTextures;
	ILuint	HeaderSize;
} TPLHEAD;

// Data formats
#define TPL_I4		0
#define TPL_I8		1
#define TPL_IA4		2
#define TPL_IA8		3
#define TPL_RGB565	4
#define TPL_RGB5A3	5
#define TPL_RGBA8	6
#define TPL_CI4		8
#define TPL_CI8		9
#define TPL_CI14X2	10
#define TPL_CMP		14

// Wrapping
#define TPL_CLAMP	0
#define	TPL_REPEAT	1
#define TPL_MIRROR	2

// Palette entries
#define TPL_PAL_IA8		0
#define TPL_PAL_RGB565	1
#define TPL_PAL_RGB5A3	2


ILboolean iIsValidTpl(void);
ILboolean iCheckTpl(TPLHEAD *Header);
ILboolean iLoadTplInternal(void);
ILboolean TplGetIndexImage(ILimage *Image, ILuint TexOff, ILuint DataFormat);


//! Checks if the file specified in FileName is a valid TPL file.
ILboolean ilIsValidTpl(ILconst_string FileName)
{
	ILHANDLE	TplFile;
	ILboolean	bTpl = IL_FALSE;
	
	if (!iCheckExtension(FileName, IL_TEXT("tpl"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bTpl;
	}
	
	TplFile = iopenr(FileName);
	if (TplFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bTpl;
	}
	
	bTpl = ilIsValidTplF(TplFile);
	icloser(TplFile);
	
	return bTpl;
}


//! Checks if the ILHANDLE contains a valid TPL file at the current position.
ILboolean ilIsValidTplF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;
	
	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidTpl();
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Checks if Lump is a valid TPL lump.
ILboolean ilIsValidTplL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidTpl();
}


// Internal function used to get the TPL header from the current file.
ILboolean iGetTplHead(TPLHEAD *Header)
{
	Header->Magic = GetBigUInt();
	Header->nTextures = GetBigUInt();
	Header->HeaderSize = GetBigUInt();
	return IL_TRUE;
}


// Internal function to get the header and check it.
ILboolean iIsValidTpl(void)
{
	TPLHEAD Header;

	if (!iGetTplHead(&Header))
		return IL_FALSE;
	iseek(-12, IL_SEEK_CUR);
	
	return iCheckTpl(&Header);
}


// Internal function used to check if the HEADER is a valid TPL header.
ILboolean iCheckTpl(TPLHEAD *Header)
{
	// The file signature is 0x0020AF30.
	if (Header->Magic != 0x0020AF30)
		return IL_FALSE;
	// Only valid header size is 0x0C.
	if (Header->HeaderSize != 0x0C)
		return IL_FALSE;
	// We have to have at least 1 texture.
	if (Header->nTextures == 0)
		return IL_FALSE;

	return IL_TRUE;
}


//! Reads a TPL file
ILboolean ilLoadTpl(ILconst_string FileName)
{
	ILHANDLE	TplFile;
	ILboolean	bTpl = IL_FALSE;
	
	TplFile = iopenr(FileName);
	if (TplFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bTpl;
	}

	bTpl = ilLoadTplF(TplFile);
	icloser(TplFile);

	return bTpl;
}


//! Reads an already-opened TPL file
ILboolean ilLoadTplF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;
	
	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadTplInternal();
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Reads from a memory "lump" that contains a TPL
ILboolean ilLoadTplL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadTplInternal();
}


// Internal function used to load the TPL.
ILboolean iLoadTplInternal(void)
{
	TPLHEAD		Header;
	ILimage		*Image/*, *BaseImage*/;
	ILuint		Pos, TexOff, PalOff, DataFormat, Bpp, DataOff, WrapS, WrapT;
	ILuint		x, y, xBlock, yBlock, i, j, k, n;
	ILenum		Format;
	ILushort	Width, Height, ShortPixel;
	ILubyte		BytePixel, CompData[8];
	Color8888	colours[4], *col;
	ILushort	color_0, color_1;
	ILuint		bitmask, Select;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}
	Image = iCurImage;  // Top-level image
	
	if (!iGetTplHead(&Header))
		return IL_FALSE;
	if (!iCheckTpl(&Header)) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	// Points to the beginning of the texture header directory.
	Pos = itell();

	for (n = 0; n < Header.nTextures; n++) {
		// Go back to the texture header directory for texture number n+1.
		iseek(Pos + n * 8, IL_SEEK_SET);
		TexOff = GetBigUInt();
		PalOff = GetBigUInt();
		// Go to the texture header.
		if (iseek(TexOff, IL_SEEK_SET))
			return IL_FALSE;

		Height = GetBigUShort();
		Width = GetBigUShort();
		// It looks like files actually have n-1 images, with the nth one having 0 height and width.
		if (Width == 0 || Height == 0) {
			// If this is our first image, however, we error out.
			if (Image == iCurImage) {
				ilSetError(IL_ILLEGAL_FILE_VALUE);
				return IL_FALSE;
			}
			// Break out of our for loop and run ilFixImage on the images.
			break;
		}

		DataFormat = GetBigUInt();
		TexOff = GetBigUInt();
		WrapS = GetBigUInt();
		WrapT = GetBigUInt();
		if (WrapS == TPL_REPEAT || WrapS == TPL_MIRROR) {
			// By the specs, repeated and mirrored textures must have dimensions of power of 2.
			if ((Width != ilNextPower2(Width)) || (Height != ilNextPower2(Height))) {
				ilSetError(IL_ILLEGAL_FILE_VALUE);
				return IL_FALSE;
			}
		}

		// Go to the actual texture data.
		if (iseek(TexOff, IL_SEEK_SET))
			return IL_FALSE;

		switch (DataFormat)
		{
			case TPL_I4:
			case TPL_I8:
				Format = IL_LUMINANCE;
				Bpp = 1;
				break;
			case TPL_IA4:
			case TPL_IA8:
				Format = IL_LUMINANCE_ALPHA;
				Bpp = 1;
				break;
			case TPL_RGB565:
				Format = IL_RGB;
				Bpp = 3;
				break;
			case TPL_RGB5A3:
				Format = IL_RGBA;
				Bpp = 4;
				break;
			case TPL_RGBA8:
				Format = IL_RGBA;
				Bpp = 4;
				break;
			case TPL_CI4:
			case TPL_CI8:
				Format = IL_COLOR_INDEX;
				Bpp = 1;
				break;
			case TPL_CI14X2:
				Format = IL_RGBA;
				Bpp = 3;
				break;
			case TPL_CMP:
				Format = IL_RGBA;
				Bpp = 4;
				break;

			default:
				ilSetError(IL_FORMAT_NOT_SUPPORTED);
				return IL_FALSE;
		}

		if (Image == iCurImage) {  // This is our first image.
			if (!ilTexImage(Width, Height, 1, Bpp, Format, IL_UNSIGNED_BYTE, NULL))
				return IL_FALSE;
		}
		else {
			Image->Next = ilNewImageFull(Width, Height, 1, Bpp, Format, IL_UNSIGNED_BYTE, NULL);
			if (Image->Next == NULL)
				return IL_FALSE;
			Image = Image->Next;
		}
		Image->Origin = IL_ORIGIN_UPPER_LEFT;  // Origins are always fixed here.

		switch (DataFormat)
		{
			case TPL_I4:
				// 8x8 tiles of 4-bit intensity values
				for (y = 0; y < Image->Height; y += 8) {
					for (x = 0; x < Image->Width; x += 8) {
						for (yBlock = 0; yBlock < 8; yBlock++) {
							if ((y + yBlock) >= Image->Height) {
								iseek(8, IL_SEEK_CUR);  // Entire row of pad bytes skipped.
								continue;
							}
							DataOff = Image->Bps * (y + yBlock) + Image->Bpp * x;
							for (xBlock = 0; xBlock < 8; xBlock += 2) {
								BytePixel = igetc();
								if ((x + xBlock) >= Image->Width)
									continue;  // Already read the pad byte.
								Image->Data[DataOff] = (BytePixel & 0xF0) | (BytePixel & 0xF0) >> 4;
								DataOff++;
								// We have to do this check again, so we do not go past the last pixel in the image (ex. 1 pixel wide image).
								if ((x + xBlock) >= Image->Width)
									continue;  // Already read the pad byte.
								Image->Data[DataOff+1] = (BytePixel & 0x0F) << 4 | (BytePixel & 0x0F);
								DataOff++;
							}
						}
					}
				}
				break;

			case TPL_I8:
				// 8x4 tiles of 8-bit intensity values
				for (y = 0; y < Image->Height; y += 4) {
					for (x = 0; x < Image->Width; x += 8) {
						for (yBlock = 0; yBlock < 4; yBlock++) {
							if ((y + yBlock) >= Image->Height) {
								iseek(8, IL_SEEK_CUR);  // Entire row of pad bytes skipped.
								continue;
							}
							DataOff = Image->Bps * (y + yBlock) + Image->Bpp * x;
							for (xBlock = 0; xBlock < 8; xBlock++) {
								if ((x + xBlock) >= Image->Width) {
									igetc();  // Skip the pad byte.
									continue;
								}
								Image->Data[DataOff] = igetc();  // Luminance value
								DataOff++;
							}
						}
					}
				}
				break;

			case TPL_IA4:
				// 8x4 tiles of 4-bit intensity and 4-bit alpha values
				for (y = 0; y < Image->Height; y += 4) {
					for (x = 0; x < Image->Width; x += 8) {
						for (yBlock = 0; yBlock < 4; yBlock++) {
							if ((y + yBlock) >= Image->Height) {
								iseek(8, IL_SEEK_CUR);  // Entire row of pad bytes skipped.
								continue;
							}
							DataOff = Image->Bps * (y + yBlock) + Image->Bpp * x;
							for (xBlock = 0; xBlock < 8; xBlock += 2) {
								BytePixel = igetc();
								if ((x + xBlock) >= Image->Width)
									continue;  // Already read the pad byte.
								Image->Data[DataOff] = (BytePixel & 0xF0) | (BytePixel & 0xF0) >> 4;
								Image->Data[DataOff+1] = (BytePixel & 0x0F) << 4 | (BytePixel & 0x0F);
								DataOff += 2;
							}
						}
					}
				}
				break;

			case TPL_IA8:
				// 4x4 tiles of 8-bit intensity and 8-bit alpha values
				for (y = 0; y < Image->Height; y += 4) {
					for (x = 0; x < Image->Width; x += 4) {
						for (yBlock = 0; yBlock < 4; yBlock++) {
							if ((y + yBlock) >= Image->Height) {
								iseek(8, IL_SEEK_CUR);  // Entire row of pad bytes skipped.
								continue;
							}
							DataOff = Image->Bps * (y + yBlock) + Image->Bpp * x;
							for (xBlock = 0; xBlock < 4; xBlock += 2) {
								if ((x + xBlock) >= Image->Width) {
									iseek(2, IL_SEEK_CUR);  // Skip the pad bytes.
									continue;
								}
								Image->Data[DataOff] = igetc();
								Image->Data[DataOff+1] = igetc();
								DataOff += 2;
							}
						}
					}
				}
				break;

			case TPL_RGB565:
				// 4x4 tiles of RGB565 data
				for (y = 0; y < Image->Height; y += 4) {
					for (x = 0; x < Image->Width; x += 4) {
						for (yBlock = 0; yBlock < 4; yBlock++) {
							if ((y + yBlock) >= Image->Height) {
								iseek(8, IL_SEEK_CUR);  // Entire row of pad bytes skipped.
								continue;
							}
							DataOff = Image->Bps * (y + yBlock) + Image->Bpp * x;
							for (xBlock = 0; xBlock < 4; xBlock++) {
								ShortPixel = GetBigUShort();
								if ((x + xBlock) >= Image->Width)
									continue;  // Already read the pad byte.
								Image->Data[DataOff] = ((ShortPixel & 0xF800) >> 8) | ((ShortPixel & 0xE000) >> 13); // Red
								Image->Data[DataOff+1] = ((ShortPixel & 0x7E0) >> 3) | ((ShortPixel & 0x600) >> 9); // Green
								Image->Data[DataOff+2] = ((ShortPixel & 0x1f) << 3) | ((ShortPixel & 0x1C) >> 2); // Blue
								DataOff += 3;
							}
						}
					}
				}
				break;

			case TPL_RGB5A3:
				// 4x4 tiles of either RGB5 or RGB4A3 depending on the MSB. 0x80
				for (y = 0; y < Image->Height; y += 4) {
					for (x = 0; x < Image->Width; x += 4) {
						for (yBlock = 0; yBlock < 4; yBlock++) {
							if ((y + yBlock) >= Image->Height) {
								iseek(8, IL_SEEK_CUR);  // Entire row of pad bytes skipped.
								continue;
							}
							DataOff = Image->Bps * (y + yBlock) + Image->Bpp * x;
							for (xBlock = 0; xBlock < 4; xBlock++) {
								ShortPixel = GetBigUShort();
								if ((x + xBlock) >= Image->Width)
									continue;  // Already read the pad byte.

								if (ShortPixel & 0x8000) {  // Check MSB.
									// We have RGB5.
									Image->Data[DataOff] = ((ShortPixel & 0x7C00) >> 7) | ((ShortPixel & 0x7000) >> 12); // Red
									Image->Data[DataOff+1] = ((ShortPixel & 0x3E0) >> 2) | ((ShortPixel & 0x380) >> 7); // Green
									Image->Data[DataOff+2] = ((ShortPixel & 0x1F) << 3) | ((ShortPixel & 0x1C) >> 2); // Blue
									Image->Data[DataOff+3] = 0xFF;  // I am just assuming that it is opaque.
								}
								else {
									// We have RGB4A3.
									Image->Data[DataOff] = ((ShortPixel & 0x7800) >> 7) | ((ShortPixel & 0x7800) >> 11); // Red
									Image->Data[DataOff+1] = ((ShortPixel & 0x0780) >> 3) | ((ShortPixel & 0x0780) >> 7); // Green
									Image->Data[DataOff+2] = ((ShortPixel & 0x0078) << 1) | ((ShortPixel & 0x0078) >> 3); // Blue
									Image->Data[DataOff+3] = ((ShortPixel & 0x07) << 5) | ((ShortPixel & 0x07) << 2) | (ShortPixel >> 1); // Alpha
								}
								DataOff += 3;
							}
						}
					}
				}
				break;

			case TPL_RGBA8:
				// 4x4 tiles of RGBA data
				for (y = 0; y < Image->Height; y += 4) {
					for (x = 0; x < Image->Width; x += 4) {
						for (yBlock = 0; yBlock < 4; yBlock++) {
							// Skip pad bytes at the bottom of the tile if any.
							if ((y + yBlock) >= Image->Height) {
								iseek(16, IL_SEEK_CUR);  // Entire row of pad bytes skipped
								continue;
							}

							// First it has the AR data.
							DataOff = Image->Bps * (y + yBlock) + Image->Bpp * x;
							for (xBlock = 0; xBlock < 4; xBlock++) {
								if ((x + xBlock) >= Image->Width) {
									iseek(2, IL_SEEK_CUR);  // Skip pad bytes.
									continue;
								}
								Image->Data[DataOff+3] = igetc();  // Alpha
								Image->Data[DataOff] = igetc();  // Red
								DataOff += 3;
							}

							// Then it has the GB data.
							DataOff = Image->Bps * (y + yBlock) + Image->Bpp * x;
							for (xBlock = 0; xBlock < 4 && x + xBlock < Image->Width; xBlock++) {
								if ((x + xBlock) >= Image->Width) {
									iseek(2, IL_SEEK_CUR);  // Skip pad bytes.
									continue;
								}
								Image->Data[DataOff+1] = igetc();  // Green
								Image->Data[DataOff+2] = igetc();  // Blue
								DataOff += 3;
							}
						}
					}
				}
				break;

			case TPL_CI4:
			case TPL_CI8:
			case TPL_CI14X2:
				// Seek to the palette header.
				if (iseek(PalOff, IL_SEEK_SET))
					return IL_FALSE;
				if (!TplGetIndexImage(Image, TexOff, DataFormat))
					return IL_FALSE;
				break;

			case TPL_CMP:
				// S3TC 2x2 blocks of 4x4 tiles.  I am assuming that this is DXT1, since it is not specified in the specs.
				//  Most of this ended up being copied from il_dds.c, from the DecompressDXT1 function.
				//@TODO: Make this/that code a bit more modular.
				for (y = 0; y < Image->Height; y += 8) {
					for (x = 0; x < Image->Width; x += 8) {
						for (yBlock = 0; yBlock < 8 && (y + yBlock) < Image->Height; yBlock += 4) {
							for (xBlock = 0; xBlock < 8 && (x + xBlock) < Image->Width; xBlock += 4) {
								if (iread(CompData, 1, 8) != 8)
									return IL_FALSE;  //@TODO: Need to do any cleanup here?
								color_0 = *((ILushort*)CompData);
								UShort(&color_0);
								color_1 = *((ILushort*)(CompData + 2));
								UShort(&color_1);
								DxtcReadColor(color_0, colours);
								DxtcReadColor(color_1, colours + 1);
								bitmask = ((ILuint*)CompData)[1];
								UInt(&bitmask);

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

										if (((x + xBlock + i) < Image->Width) && ((y + yBlock + j) < Image->Height)) {
											DataOff = (y + yBlock + j) * Image->Bps + (x + xBlock + i) * Image->Bpp;
											Image->Data[DataOff + 0] = col->r;
											Image->Data[DataOff + 1] = col->g;
											Image->Data[DataOff + 2] = col->b;
											Image->Data[DataOff + 3] = col->a;
										}
									}
								}
							}
						}
					}
				}
				break;
		}
	}

	return ilFixImage();
}


ILboolean TplGetIndexImage(ILimage *Image, ILuint TexOff, ILuint DataFormat)
{
	ILushort	NumPal, ShortPixel;
	ILubyte		LumVal, BytePixel;
	ILuint		PalFormat, PalOff, PalBpp, DataOff;
	ILuint		x, y, xBlock, yBlock, i;

	NumPal = GetBigUShort();
	iseek(2, IL_SEEK_CUR);  // Do we need to do anything with the 'unpacked' entry?  I see nothing in the specs about it.
	PalFormat = GetBigUInt();

	// Now we have to find out where the actual palette data is stored.
	//@TODO: Do we need to set any errors here?
	PalOff = GetBigUInt();
	if (iseek(PalOff, IL_SEEK_SET))
		return IL_FALSE;

	switch (PalFormat)
	{
		case TPL_PAL_IA8:
			Image->Pal.Palette = (ILubyte*)ialloc(NumPal * 4);
			if (Image->Pal.Palette == NULL)
				return IL_FALSE;
			Image->Pal.PalType = IL_PAL_RGBA32;  //@TODO: Use this format natively.
			Image->Pal.PalSize = NumPal * 4;
			PalBpp = 4;

			for (i = 0; i < NumPal; i++) {
				LumVal = igetc();
				//@TODO: Do proper conversion of luminance, or support this format natively.
				Image->Pal.Palette[i * 4] = LumVal;  // Assign the luminance value.
				Image->Pal.Palette[i * 4 + 1] = LumVal;
				Image->Pal.Palette[i * 4 + 2] = LumVal;
				Image->Pal.Palette[i * 4 + 3] = igetc();  // Get alpha value.
			}
			break;

		case TPL_PAL_RGB565:
			Image->Pal.Palette = (ILubyte*)ialloc(NumPal * 3);
			if (Image->Pal.Palette == NULL)
				return IL_FALSE;
			Image->Pal.PalType = IL_PAL_RGB24;
			Image->Pal.PalSize = NumPal * 3;
			PalBpp = 3;

			for (i = 0; i < NumPal; i++) {
				ShortPixel = GetBigUShort();
				// This is mostly the same code as in the TPL_RGB565 case.
				Image->Pal.Palette[i*3] = ((ShortPixel & 0xF800) >> 8) | ((ShortPixel & 0xE000) >> 13); // Red
				Image->Pal.Palette[i*3+1] = ((ShortPixel & 0x7E0) >> 3) | ((ShortPixel & 0x600) >> 9); // Green
				Image->Pal.Palette[i*3+2] = ((ShortPixel & 0x1f) << 3) | ((ShortPixel & 0x1C) >> 2); // Blue
			}
			break;

		case TPL_PAL_RGB5A3:
			Image->Pal.Palette = (ILubyte*)ialloc(NumPal * 4);
			if (Image->Pal.Palette == NULL)
				return IL_FALSE;
			Image->Pal.PalType = IL_PAL_RGBA32;
			Image->Pal.PalSize = NumPal * 4;
			PalBpp = 4;

			for (i = 0; i < NumPal; i++) {
				ShortPixel = GetBigUShort();
				// This is mostly the same code as in the TPL_RGB565 case.
				if (ShortPixel & 0x8000) {  // Check MSB.
					// We have RGB5.
					Image->Pal.Palette[i*4] = ((ShortPixel & 0x7C00) >> 7) | ((ShortPixel & 0x7000) >> 12); // Red
					Image->Pal.Palette[i*4+1] = ((ShortPixel & 0x3E0) >> 2) | ((ShortPixel & 0x380) >> 7); // Green
					Image->Pal.Palette[i*4+2] = ((ShortPixel & 0x1F) << 3) | ((ShortPixel & 0x1C) >> 2); // Blue
					Image->Pal.Palette[i*4+3] = 0xFF;  // I am just assuming that it is opaque.
				}
				else {
					// We have RGB4A3.
					Image->Pal.Palette[i*4] = ((ShortPixel & 0x7800) >> 7) | ((ShortPixel & 0x7800) >> 11); // Red
					Image->Pal.Palette[i*4+1] = ((ShortPixel & 0x0780) >> 3) | ((ShortPixel & 0x0780) >> 7); // Green
					Image->Pal.Palette[i*4+2] = ((ShortPixel & 0x0078) << 1) | ((ShortPixel & 0x0078) >> 3); // Blue
					Image->Pal.Palette[i*4+3] = ((ShortPixel & 0x07) << 5) | ((ShortPixel & 0x07) << 2) | (ShortPixel >> 1); // Alpha
				}
			}
			break;

		default:
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			return IL_FALSE;
	}

	// Go back to the texture data.
	if (iseek(TexOff, IL_SEEK_SET))
		return IL_FALSE;

	switch (DataFormat)
	{
		case TPL_CI4:
			// 8x8 tiles of 4-bit color indices
			//  This is the exact same code as the TPL_I4 case.
			for (y = 0; y < Image->Height; y += 8) {
				for (x = 0; x < Image->Width; x += 8) {
					for (yBlock = 0; yBlock < 8; yBlock++) {
						if ((y + yBlock) >= Image->Height) {
							iseek(8, IL_SEEK_CUR);  // Entire row of pad bytes skipped.
							continue;
						}
						DataOff = Image->Bps * (y + yBlock) + Image->Bpp * x;
						for (xBlock = 0; xBlock < 8; xBlock += 2) {
							BytePixel = igetc();
							if ((x + xBlock) >= Image->Width)
								continue;  // Already read the pad byte.
							Image->Data[DataOff] = (BytePixel & 0xF0) | (BytePixel & 0xF0) >> 4;
							DataOff++;
							// We have to do this check again, so we do not go past the last pixel in the image (ex. 1 pixel wide image).
							if ((x + xBlock) >= Image->Width)
								continue;  // Already read the pad byte.
							Image->Data[DataOff+1] = (BytePixel & 0x0F) << 4 | (BytePixel & 0x0F);
							DataOff++;
						}
					}
				}
			}
			break;

		case TPL_CI8:
			// 8x4 tiles of 8-bit intensity values
			//  This is the exact same code as the TPL_I8 case.
			for (y = 0; y < Image->Height; y += 4) {
				for (x = 0; x < Image->Width; x += 8) {
					for (yBlock = 0; yBlock < 4; yBlock++) {
						if ((y + yBlock) >= Image->Height) {
							iseek(8, IL_SEEK_CUR);  // Entire row of pad bytes skipped.
							continue;
						}
						DataOff = Image->Bps * (y + yBlock) + Image->Bpp * x;
						for (xBlock = 0; xBlock < 8; xBlock++) {
							if ((x + xBlock) >= Image->Width) {
								igetc();  // Skip the pad byte.
								continue;
							}
							Image->Data[DataOff] = igetc();  // Color index
							DataOff++;
						}
					}
				}
			}
			break;


		//@TODO: Convert to more formats than just RGBA.
		case TPL_CI14X2:
			// 4x4 tiles of 14-bit indices into a palette.  Not supported at all in DevIL, since
			//  it has a huge palette (> 256 colors).  Convert to RGBA.
			for (y = 0; y < Image->Height; y += 4) {
				for (x = 0; x < Image->Width; x += 4) {
					for (yBlock = 0; yBlock < 4; yBlock++) {
						if ((y + yBlock) >= Image->Height) {
							iseek(8, IL_SEEK_CUR);  // Entire row of pad bytes skipped.
							continue;
						}
						DataOff = Image->Bps * (y + yBlock) + Image->Bpp * x;
						for (xBlock = 0; xBlock < 4; xBlock++) {
							if ((x + xBlock) >= Image->Width) {
								GetBigUShort();  // Skip the pad short.
								continue;
							}
							ShortPixel = GetBigUShort();
							ShortPixel >>= 2;  // Lower 2 bits are padding bits.
							Image->Data[DataOff] = Image->Pal.Palette[ShortPixel * PalBpp];
							Image->Data[DataOff+1] = Image->Pal.Palette[ShortPixel * PalBpp + 1];
							Image->Data[DataOff+2] = Image->Pal.Palette[ShortPixel * PalBpp + 2];
							if (PalFormat == TPL_PAL_RGB565)
								Image->Data[DataOff+3] = 0xFF;
							else
								Image->Data[DataOff+3] = Image->Pal.Palette[ShortPixel * PalBpp + 3];
							DataOff++;
						}
					}
				}
			}
			// Get rid of the palette, since we no longer need it.
			ifree(Image->Pal.Palette);
			Image->Pal.PalType = IL_PAL_NONE;
			Image->Pal.PalSize = 0;
			break;

	}

	return IL_TRUE;
}
#endif//IL_NO_TPL
