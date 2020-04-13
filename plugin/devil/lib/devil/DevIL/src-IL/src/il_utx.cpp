//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_utx.cpp
//
// Description: Reads from an Unreal and Unreal Tournament Texture (.utx) file.
//				Specifications can be found at
//				http://wiki.beyondunreal.com/Legacy:Package_File_Format.
//
//-----------------------------------------------------------------------------

#include "il_internal.h"
#ifndef IL_NO_UTX
#include "il_utx.h"


//! Reads a UTX file
ILboolean ilLoadUtx(ILconst_string FileName)
{
	ILHANDLE	UtxFile;
	ILboolean	bUtx = IL_FALSE;

	UtxFile = iopenr(FileName);
	if (UtxFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bUtx;
	}

	bUtx = ilLoadUtxF(UtxFile);
	icloser(UtxFile);

	return bUtx;
}


//! Reads an already-opened UTX file
ILboolean ilLoadUtxF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;
	
	iSetInputFile(File);
	FirstPos = itell();
	try {
		bRet = iLoadUtxInternal();
	}
	catch (bad_alloc &e) {
		e;
		ilSetError(IL_OUT_OF_MEMORY);
		return IL_FALSE;
	}
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Reads from a memory "lump" that contains a UTX
ILboolean ilLoadUtxL(const void *Lump, ILuint Size)
{
	try {
		iSetInputLump(Lump, Size);
	}
	catch (bad_alloc &e) {
		e;
		ilSetError(IL_OUT_OF_MEMORY);
		return IL_FALSE;
	}
	return iLoadUtxInternal();
}


ILboolean GetUtxHead(UTXHEADER &Header)
{
	Header.Signature = GetLittleUInt();
	Header.Version = GetLittleUShort();
	Header.LicenseMode = GetLittleUShort();
	Header.Flags = GetLittleUInt();
	Header.NameCount = GetLittleUInt();
	Header.NameOffset = GetLittleUInt();
	Header.ExportCount = GetLittleUInt();
	Header.ExportOffset = GetLittleUInt();
	Header.ImportCount = GetLittleUInt();
	Header.ImportOffset = GetLittleUInt();

	return IL_TRUE;
}


ILboolean CheckUtxHead(UTXHEADER &Header)
{
	// This signature signifies a UTX file.
	if (Header.Signature != 0x9E2A83C1)
		return IL_FALSE;
	// Unreal uses 61-63, and Unreal Tournament uses 67-69.
	if ((Header.Version < 61 || Header.Version > 69))
		return IL_FALSE;
	return IL_TRUE;
}


// Gets a name variable from the file.  Keep in mind that the return value must be freed.
string GetUtxName(UTXHEADER &Header)
{
#define NAME_MAX_LEN 256  //@TODO: Figure out if these can possibly be longer.
	char	Name[NAME_MAX_LEN];
	ILubyte	Length = 0;

	// New style (Unreal Tournament) name.  This has a byte at the beginning telling
	//  how long the string is (plus terminating 0), followed by the terminating 0. 
	if (Header.Version >= 64) {
		Length = igetc();
		if (iread(Name, Length, 1) != 1)
			return "";
		if (Name[Length-1] != 0)
			return "";
		return string(Name);
	}

	// Old style (Unreal) name.  This string length is unknown, but it is terminated
	//  by a 0.
	do {
		Name[Length++] = igetc();
	} while (!ieof() && Name[Length-1] != 0 && Length < NAME_MAX_LEN);

	// Never reached the terminating 0.
	if (Length == NAME_MAX_LEN && Name[Length-1] != 0)
		return "";

	return string(Name);
#undef NAME_MAX_LEN
}


bool GetUtxNameTable(vector <UTXENTRYNAME> &NameEntries, UTXHEADER &Header)
{
	ILuint	NumRead;

	// Go to the name table.
	iseek(Header.NameOffset, IL_SEEK_SET);

	NameEntries.resize(Header.NameCount);

	// Read in the name table.
	for (NumRead = 0; NumRead < Header.NameCount; NumRead++) {
		NameEntries[NumRead].Name = GetUtxName(Header);
		if (NameEntries[NumRead].Name == "")
			break;
		NameEntries[NumRead].Flags = GetLittleUInt();
	}

	// Did not read all of the entries (most likely GetUtxName failed).
	if (NumRead < Header.NameCount) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return false;
	}

	return true;
}


// This following code is from http://wiki.beyondunreal.com/Legacy:Package_File_Format/Data_Details.
/// <summary>Reads a compact integer from the FileReader.
/// Bytes read differs, so do not make assumptions about
/// physical data being read from the stream. (If you have
/// to, get the difference of FileReader.BaseStream.Position
/// before and after this is executed.)</summary>
/// <returns>An "uncompacted" signed integer.</returns>
/// <remarks>FileReader is a System.IO.BinaryReader mapped
/// to a file. Also, there may be better ways to implement
/// this, but this is fast, and it works.</remarks>
ILint UtxReadCompactInteger()
{
        int output = 0;
        ILboolean sign = IL_FALSE;
		int i;
		ILubyte x;
        for(i = 0; i < 5; i++)
        {
                x = igetc();
                // First byte
                if(i == 0)
                {
                        // Bit: X0000000
                        if((x & 0x80) > 0)
                                sign = IL_TRUE;
                        // Bits: 00XXXXXX
                        output |= (x & 0x3F);
                        // Bit: 0X000000
                        if((x & 0x40) == 0)
                                break;
                }
                // Last byte
                else if(i == 4)
                {
                        // Bits: 000XXXXX -- the 0 bits are ignored
                        // (hits the 32 bit boundary)
                        output |= (x & 0x1F) << (6 + (3 * 7));
                }
                // Middle bytes
                else
                {
                        // Bits: 0XXXXXXX
                        output |= (x & 0x7F) << (6 + ((i - 1) * 7));
                        // Bit: X0000000
                        if((x & 0x80) == 0)
                                break;
                }
        }
        // multiply by negative one here, since the first 6+ bits could be 0
        if (sign)
                output *= -1;
        return output;
}


void ChangeObjectReference(ILint *ObjRef, ILboolean *IsImported)
{
	if (*ObjRef < 0) {
		*IsImported = IL_TRUE;
		*ObjRef = -*ObjRef - 1;
	}
	else if (*ObjRef > 0) {
		*IsImported = IL_FALSE;
		*ObjRef = *ObjRef - 1;  // This is an object reference, so we have to do this conversion.
	}
	else {
		*ObjRef = -1;  // "NULL" pointer
	}

	return;
}


bool GetUtxExportTable(vector <UTXEXPORTTABLE> &ExportTable, UTXHEADER &Header)
{
	ILuint i;

	// Go to the name table.
	iseek(Header.ExportOffset, IL_SEEK_SET);

	// Create ExportCount elements in our array.
	ExportTable.resize(Header.ExportCount);

	for (i = 0; i < Header.ExportCount; i++) {
		ExportTable[i].Class = UtxReadCompactInteger();
		ExportTable[i].Super = UtxReadCompactInteger();
		ExportTable[i].Group = GetLittleUInt();
		ExportTable[i].ObjectName = UtxReadCompactInteger();
		ExportTable[i].ObjectFlags = GetLittleUInt();
		ExportTable[i].SerialSize = UtxReadCompactInteger();
		ExportTable[i].SerialOffset = UtxReadCompactInteger();

		ChangeObjectReference(&ExportTable[i].Class, &ExportTable[i].ClassImported);
		ChangeObjectReference(&ExportTable[i].Super, &ExportTable[i].SuperImported);
		ChangeObjectReference(&ExportTable[i].Group, &ExportTable[i].GroupImported);
	}

	return true;
}


bool GetUtxImportTable(vector <UTXIMPORTTABLE> &ImportTable, UTXHEADER &Header)
{
	ILuint i;

	// Go to the name table.
	iseek(Header.ImportOffset, IL_SEEK_SET);

	// Allocate the name table.
	ImportTable.resize(Header.ImportCount);

	for (i = 0; i < Header.ImportCount; i++) {
		ImportTable[i].ClassPackage = UtxReadCompactInteger();
		ImportTable[i].ClassName = UtxReadCompactInteger();
		ImportTable[i].Package = GetLittleUInt();
		ImportTable[i].ObjectName = UtxReadCompactInteger();

		ChangeObjectReference(&ImportTable[i].Package, &ImportTable[i].PackageImported);
	}

	return true;
}


/*void UtxDestroyPalettes(UTXPALETTE *Palettes, ILuint NumPal)
{
	ILuint i;
	for (i = 0; i < NumPal; i++) {
		//ifree(Palettes[i].Name);
		ifree(Palettes[i].Pal);
	}
	ifree(Palettes);
}*/


ILenum UtxFormatToDevIL(ILuint Format)
{
	switch (Format)
	{
		case UTX_P8:
			return IL_COLOR_INDEX;
		case UTX_DXT1:
			return IL_RGBA;
	}
	return IL_BGRA;  // Should never reach here.
}


ILuint UtxFormatToBpp(ILuint Format)
{
	switch (Format)
	{
		case UTX_P8:
			return 1;
		case UTX_DXT1:
			return 4;
	}
	return 4;  // Should never reach here.
}


// Internal function used to load the UTX.
ILboolean iLoadUtxInternal(void)
{
	UTXHEADER		Header;
	vector <UTXENTRYNAME> NameEntries;
	vector <UTXEXPORTTABLE> ExportTable;
	vector <UTXIMPORTTABLE> ImportTable;
	vector <UTXPALETTE> Palettes;
	ILimage		*Image;
	ILuint		NumPal = 0, i, j = 0;
	ILint		Name;
	ILubyte		Type;
	ILint		Val;
	ILint		Size;
	ILint		Width, Height, PalEntry;
	ILboolean	BaseCreated = IL_FALSE, HasPal;
	ILint		Format;
	ILubyte		*CompData = NULL;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	if (!GetUtxHead(Header))
		return IL_FALSE;
	if (!CheckUtxHead(Header))
		return IL_FALSE;

	// We grab the name table.
	if (!GetUtxNameTable(NameEntries, Header))
		return IL_FALSE;
	// Then we get the export table.
	if (!GetUtxExportTable(ExportTable, Header))
		return IL_FALSE;
	// Then the last table is the import table.
	if (!GetUtxImportTable(ImportTable, Header))
		return IL_FALSE;

	// Find the number of palettes in the export table.
	for (i = 0; i < Header.ExportCount; i++) {
		if (NameEntries[ImportTable[ExportTable[i].Class].ObjectName].Name == "Palette")
			NumPal++;
	}
	Palettes.resize(NumPal);

	// Read in all of the palettes.
	NumPal = 0;
	for (i = 0; i < Header.ExportCount; i++) {
		if (NameEntries[ImportTable[ExportTable[i].Class].ObjectName].Name == "Palette") {
			Palettes[NumPal].Name = i;
			iseek(ExportTable[NumPal].SerialOffset, IL_SEEK_SET);
			Name = igetc();  // Skip the 2.  @TODO: Can there be more in front of the palettes?
			Palettes[NumPal].Count = UtxReadCompactInteger();
			Palettes[NumPal].Pal = new ILubyte[Palettes[NumPal].Count * 4];
			if (iread(Palettes[NumPal].Pal, Palettes[NumPal].Count * 4, 1) != 1)
				return IL_FALSE;
			NumPal++;
		}
	}

	for (i = 0; i < Header.ExportCount; i++) {
		// Find textures in the file.
		if (NameEntries[ImportTable[ExportTable[i].Class].ObjectName].Name == "Texture") {
			iseek(ExportTable[i].SerialOffset, IL_SEEK_SET);
			Width = -1;  Height = -1;  PalEntry = NumPal;  HasPal = IL_FALSE;  Format = -1;

			do {
				// Always starts with a comptact integer that gives us an entry into the name table.
				Name = UtxReadCompactInteger();
				if (NameEntries[Name].Name == "None")
					break;
				Type = igetc();
				Size = (Type & 0x70) >> 4;

				if (Type == 0xA2)
					igetc();  // Byte is 1 here...

				switch (Type & 0x0F)
				{
					case 1:  // Get a single byte.
						Val = igetc();
						break;

					case 2:  // Get unsigned integer.
						Val = GetLittleUInt();
						break;

					case 3:  // Boolean value is in the info byte.
						igetc();
						break;

					case 4:  // Skip flaots for right now.
						GetLittleFloat();
						break;

					case 5:
					case 6:  // Get a compact integer - an object reference.
						Val = UtxReadCompactInteger();
						Val--;
						break;

					case 10:
						Val = igetc();
						switch (Size)
						{
							case 0:
								iseek(1, IL_SEEK_CUR);
								break;
							case 1:
								iseek(2, IL_SEEK_CUR);
								break;
							case 2:
								iseek(4, IL_SEEK_CUR);
								break;
							case 3:
								iseek(12, IL_SEEK_CUR);
								break;
						}
						break;

					default:  // Uhm...
						break;
				}

				//@TODO: What should we do if Name >= Header.NameCount?
				if ((ILuint)Name < Header.NameCount) {  // Don't want to go past the end of our array.
					if (NameEntries[Name].Name == "Palette") {
						// If it has references to more than one palette, just use the first one.
						if (HasPal == IL_FALSE) {
							// We go through the palette list here to match names.
							for (PalEntry = 0; (ILuint)PalEntry < NumPal; PalEntry++) {
								if (Val == Palettes[PalEntry].Name) {
									HasPal = IL_TRUE;
									break;
								}
							}
						}
					}
					if (NameEntries[Name].Name == "Format")  // Not required for P8 images but can be present.
						if (Format == -1)
							Format = Val;
					if (NameEntries[Name].Name == "USize")  // Width of the image
						if (Width == -1)
							Width = Val;
					if (NameEntries[Name].Name == "VSize")  // Height of the image
						if (Height == -1)
							Height = Val;
				}
			} while (!ieof());

			// If the format property is not present, it is a paletted (P8) image.
			if (Format == -1)
				Format = UTX_P8;
			// Just checks for everything being proper.  If the format is P8, we check to make sure that a palette was found.
			if (Width == -1 || Height == -1 || (PalEntry == NumPal && Format != UTX_DXT1) || (Format != UTX_P8 && Format != UTX_DXT1))
				return IL_FALSE;
			if (BaseCreated == IL_FALSE) {
				BaseCreated = IL_TRUE;
				ilTexImage(Width, Height, 1, UtxFormatToBpp(Format), UtxFormatToDevIL(Format), IL_UNSIGNED_BYTE, NULL);
				Image = iCurImage;
			}
			else {
				Image->Next = ilNewImageFull(Width, Height, 1, UtxFormatToBpp(Format), UtxFormatToDevIL(Format), IL_UNSIGNED_BYTE, NULL);
				if (Image->Next == NULL)
					return IL_FALSE;
				Image = Image->Next;
			}

			// Skip the mipmap count, width offset and mipmap size entries.
			//@TODO: Implement mipmaps.
			iseek(5, IL_SEEK_CUR);
			UtxReadCompactInteger();

			switch (Format)
			{
				case UTX_P8:
					Image->Pal.PalSize = Palettes[PalEntry].Count * 4;
					Image->Pal.Palette = (ILubyte*)ialloc(Image->Pal.PalSize);
					if (Image->Pal.Palette == NULL)
						return IL_FALSE;
					memcpy(Image->Pal.Palette, Palettes[PalEntry].Pal, Image->Pal.PalSize);
					Image->Pal.PalType = IL_PAL_RGBA32;

					if (iread(Image->Data, Image->SizeOfData, 1) != 1)
						return IL_FALSE;
					break;

				case UTX_DXT1:
					Image->DxtcSize = IL_MAX(Image->Width * Image->Height / 2, 8);
					CompData = (ILubyte*)ialloc(Image->DxtcSize);
					if (CompData == NULL)
						return IL_FALSE;

					if (iread(CompData, Image->DxtcSize, 1) != 1) {
						ifree(CompData);
						return IL_FALSE;
					}
					// Keep a copy of the DXTC data if the user wants it.
					if (ilGetInteger(IL_KEEP_DXTC_DATA) == IL_TRUE) {
						Image->DxtcData = CompData;
						Image->DxtcFormat = IL_DXT1;
						CompData = NULL;
					}
					if (DecompressDXT1(Image, CompData) == IL_FALSE) {
						ifree(CompData);
						return IL_FALSE;
					}
					ifree(CompData);
					break;
			}
			Image->Origin = IL_ORIGIN_UPPER_LEFT;
		}
	}

	return ilFixImage();
}

#endif//IL_NO_UTX

