//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 03/02/2009
//
// Filename: src-IL/src/il_utx.c
//
// Description: Reads from an Unreal and Unreal Tournament Texture (.utx) file.
//				Specifications can be found at
//				http://wiki.beyondunreal.com/Legacy:Package_File_Format.
//
//-----------------------------------------------------------------------------

#include "il_internal.h"
#ifndef IL_NO_UTX
#include "il_utx.h"
#include "il_dds.h"


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
	bRet = iLoadUtxInternal();
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Reads from a memory "lump" that contains a UTX
ILboolean ilLoadUtxL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadUtxInternal();
}


ILboolean GetUtxHead(UTXHEADER *Header)
{
	Header->Signature = GetLittleUInt();
	Header->Version = GetLittleUShort();
	Header->LicenseMode = GetLittleUShort();
	Header->Flags = GetLittleUInt();
	Header->NameCount = GetLittleUInt();
	Header->NameOffset = GetLittleUInt();
	Header->ExportCount = GetLittleUInt();
	Header->ExportOffset = GetLittleUInt();
	Header->ImportCount = GetLittleUInt();
	Header->ImportOffset = GetLittleUInt();

	return IL_TRUE;
}


ILboolean CheckUtxHead(UTXHEADER *Header)
{
	// This signature signifies a UTX file.
	if (Header->Signature != 0x9E2A83C1)
		return IL_FALSE;
	// Unreal uses 61-63, and Unreal Tournament uses 67-69.
	if ((Header->Version < 61 || Header->Version > 69))
		return IL_FALSE;
	return IL_TRUE;
}


// Gets a name variable from the file.  Keep in mind that the return value must be freed.
char *GetUtxName(UTXHEADER *Header)
{
#define NAME_MAX_LEN 256  //@TODO: Figure out if these can possibly be longer.
	char	*Name, OldName[NAME_MAX_LEN];
	ILubyte	Length = 0;

	// New style (Unreal Tournament) name.  This has a byte at the beginning telling
	//  how long the string is (plus terminating 0), followed by the terminating 0. 
	if (Header->Version >= 64) {
		Length = igetc();
		Name = (char*)ialloc(Length);
		if (Name == NULL)
			return NULL;
		if (iread(Name, Length, 1) != 1) {
			ifree(Name);
			return NULL;
		}
		return Name;
	}

	// Old style (Unreal) name.  This string length is unknown, but it is terminated
	//  by a 0.
	do {
		OldName[Length++] = igetc();
	} while (!ieof() && OldName[Length-1] != 0 && Length < NAME_MAX_LEN);

	// Never reached the terminating 0.
	if (Length == NAME_MAX_LEN && OldName[Length-1] != 0)
		return NULL;

	// Just copy the string and return it.
	Name = (char*)ialloc(Length);
	if (Name == NULL)
		return NULL;
	memcpy(Name, OldName, Length);

	return Name;

#undef NAME_MAX_LEN
}


UTXENTRYNAME *GetUtxNameTable(UTXHEADER *Header)
{
	UTXENTRYNAME *NameEntries;
	ILuint	i, NumRead;

	// Go to the name table.
	iseek(Header->NameOffset, IL_SEEK_SET);

	// Allocate the name table.
	NameEntries = (UTXENTRYNAME*)ialloc(Header->NameCount * sizeof(UTXENTRYNAME));
	if (NameEntries == NULL)
		return NULL;

	// Read in the name table.
	for (NumRead = 0; NumRead < Header->NameCount; NumRead++) {
		NameEntries[NumRead].Name = GetUtxName(Header);
		if (NameEntries[NumRead].Name == NULL)
			break;
		NameEntries[NumRead].Flags = GetLittleUInt();
	}

	// Did not read all of the entries (most likely GetUtxName failed).
	if (NumRead < Header->NameCount) {
		// So we have to free all of the memory we allocated here.
		for (i = 0; i < NumRead; i++) {
			ifree(NameEntries[NumRead].Name);
		}
		ilSetError(IL_INVALID_FILE_HEADER);
		return NULL;
	}

	return NameEntries;
}


void UtxDestroyNameEntries(UTXENTRYNAME *NameEntries, UTXHEADER *Header)
{
	ILuint i;

	for (i = 0; i < Header->NameCount; i++) {
		ifree(NameEntries[i].Name);
	}
	ifree(NameEntries);

	return;
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

UTXEXPORTTABLE *GetUtxExportTable(UTXHEADER *Header)
{
	UTXEXPORTTABLE	*ExportTable;
	ILuint			i;

	// Go to the name table.
	iseek(Header->ExportOffset, IL_SEEK_SET);

	// Allocate the name table.
	ExportTable = (UTXEXPORTTABLE*)ialloc(Header->ExportCount * sizeof(UTXEXPORTTABLE));
	if (ExportTable == NULL)
		return NULL;

	for (i = 0; i < Header->ExportCount; i++) {
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

	return ExportTable;
}


void UtxDestroyExportTable(UTXEXPORTTABLE *ExportTable, UTXHEADER *Header)
{
	ifree(ExportTable);
	return;
}


UTXIMPORTTABLE *GetUtxImportTable(UTXHEADER *Header)
{
	UTXIMPORTTABLE	*ImportTable;
	ILuint			i;

	// Go to the name table.
	iseek(Header->ImportOffset, IL_SEEK_SET);

	// Allocate the name table.
	ImportTable = (UTXIMPORTTABLE*)ialloc(Header->ImportCount * sizeof(UTXIMPORTTABLE));
	if (ImportTable == NULL)
		return NULL;

	for (i = 0; i < Header->ImportCount; i++) {
		ImportTable[i].ClassPackage = UtxReadCompactInteger();
		ImportTable[i].ClassName = UtxReadCompactInteger();
		ImportTable[i].Package = GetLittleUInt();
		ImportTable[i].ObjectName = UtxReadCompactInteger();

		ChangeObjectReference(&ImportTable[i].Package, &ImportTable[i].PackageImported);
	}

	return ImportTable;
}


void UtxDestroyImportTable(UTXIMPORTTABLE *ImportTable, UTXHEADER *Header)
{
	ifree(ImportTable);
	return;
}


void UtxDestroyPalettes(UTXPALETTE *Palettes, ILuint NumPal)
{
	ILuint i;
	for (i = 0; i < NumPal; i++) {
		//ifree(Palettes[i].Name);
		ifree(Palettes[i].Pal);
	}
	ifree(Palettes);
}


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
	UTXENTRYNAME	*NameEntries;
	UTXEXPORTTABLE	*ExportTable;
	UTXIMPORTTABLE	*ImportTable;
	UTXPALETTE		*Palettes;
	ILimage			*Image;
	ILuint			NumPal = 0, i, j = 0;
ILint Name;
ILubyte Type;
ILint	Val;
ILint	Size;
ILint	Width, Height, PalEntry;
ILboolean	BaseCreated = IL_FALSE, HasPal;
ILuint	Pos;
ILint	Format;
ILubyte	*CompData = NULL;
ILubyte ExtraData[9];

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	if (!GetUtxHead(&Header))
		return IL_FALSE;
	if (!CheckUtxHead(&Header))
		return IL_FALSE;

	// Now we grab the name table.
	NameEntries = GetUtxNameTable(&Header);
	if (NameEntries == NULL)
		return IL_FALSE;
	// Then we get the export table.
	ExportTable = GetUtxExportTable(&Header);
	if (ExportTable == NULL) {
		UtxDestroyNameEntries(NameEntries, &Header);
		return IL_FALSE;
	}
	// Then the last table is the import table.
	ImportTable = GetUtxImportTable(&Header);
	if (ImportTable == NULL) {
		UtxDestroyNameEntries(NameEntries, &Header);
		UtxDestroyExportTable(ExportTable, &Header);
		return IL_FALSE;
	}

	for (i = 0; i < Header.ExportCount; i++) {
		if (!strcmp(NameEntries[ImportTable[ExportTable[i].Class].ObjectName].Name, "Palette"))
			NumPal++;
	}
	if (NumPal == 0)  //@TODO: Take care of UTX files without paletted data.
		return IL_FALSE;
	Palettes = (UTXPALETTE*)ialloc(NumPal * sizeof(UTXPALETTE));
	if (Palettes == NULL) {
		UtxDestroyNameEntries(NameEntries, &Header);
		UtxDestroyExportTable(ExportTable, &Header);
		UtxDestroyImportTable(ImportTable, &Header);
		return IL_FALSE;
	}
	NumPal = 0;
	for (i = 0; i < Header.ExportCount; i++) {
		if (!strcmp(NameEntries[ImportTable[ExportTable[i].Class].ObjectName].Name, "Palette")) {
			//Palettes[i].Name = strdup(NameEntries[ExportTable[i].ObjectName].Name);
			Palettes[NumPal].Name = i;//ExportTable[i].ObjectName;
			iseek(ExportTable[NumPal].SerialOffset, IL_SEEK_SET);
Name = igetc();  // Skip the 2.
			Palettes[NumPal].Count = UtxReadCompactInteger();
			Palettes[NumPal].Pal = (ILubyte*)ialloc(Palettes[NumPal].Count * 4);
			if (/*Palettes[NumPal].Name == NULL || */Palettes[NumPal].Pal == NULL) {
				UtxDestroyNameEntries(NameEntries, &Header);
				UtxDestroyExportTable(ExportTable, &Header);
				UtxDestroyImportTable(ImportTable, &Header);
				UtxDestroyPalettes(Palettes, NumPal);
				return IL_FALSE;
			}
			if (iread(Palettes[NumPal].Pal, Palettes[NumPal].Count * 4, 1) != 1)
				//@TODO: Deallocations here!
				return IL_FALSE;
			NumPal++;
		}
	}

	for (i = 0; i < Header.ExportCount; i++) {
		if (!strcmp(NameEntries[ImportTable[ExportTable[i].Class].ObjectName].Name, "Texture")) {
			iseek(ExportTable[i].SerialOffset, IL_SEEK_SET);
			Width = -1;  Height = -1;  PalEntry = NumPal;  HasPal = IL_FALSE;  Format = -1;
++j;
j = j;

			do {
Pos = itell();
				Name = UtxReadCompactInteger();
				if (!strcmp(NameEntries[Name].Name, "None"))
					break;
				Type = igetc();
				Size = (Type & 0x70) >> 4;

				if (/*Name == 0 && */Type == 0xA2)
					igetc();  // Byte is 1 here...

				switch (Type & 0x0F)
				{
					case 1:
						Val = igetc();
						break;

					case 2:
						Val = GetLittleUInt();
						break;

					case 3:  // Boolean value is in the info byte.
						igetc();
						break;

					case 4:
						GetLittleFloat();
						break;

					case 5:
					case 6:
						Val = itell();
						Val = UtxReadCompactInteger();
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
						Val = Val;
						break;
				}

				//@TODO: What should we do if Name >= Header.NameCount?
				if ((ILuint)Name < Header.NameCount) {
					if (!strcmp(NameEntries[Name].Name, "Palette")) {
						Val--;
						if (HasPal == IL_FALSE) {
							for (PalEntry = 0; (ILuint)PalEntry < NumPal; PalEntry++) {
								if (Val == Palettes[PalEntry].Name) {
									HasPal = IL_TRUE;
									break;
								}
							}
						}
					}
					if (!strcmp(NameEntries[Name].Name, "Format"))
						if (Format == -1)
							Format = Val;
					if (!strcmp(NameEntries[Name].Name, "USize"))
						if (Width == -1)
							Width = Val;
					if (!strcmp(NameEntries[Name].Name, "VSize"))
						if (Height == -1)
							Height = Val;
				}

			} while (!ieof());

			if (Format == -1)
				Format = UTX_P8;
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

			iseek(5, IL_SEEK_CUR);
			UtxReadCompactInteger();

			switch (Format)
			{
				case UTX_P8:
					Image->Pal.PalSize = Palettes[PalEntry].Count * 4;
					Image->Pal.Palette = (ILubyte*)ialloc(Image->Pal.PalSize);
					if (Image->Pal.Palette == NULL)
						//@TODO: Do all the deallocations needed here!
						return IL_FALSE;
					memcpy(Image->Pal.Palette, Palettes[PalEntry].Pal, Image->Pal.PalSize);
					Image->Pal.PalType = IL_PAL_RGBA32;

					if (iread(Image->Data, Image->SizeOfData, 1) != 1)
						return IL_FALSE;  //@TODO: Deallocations...
					break;

				case UTX_DXT1:
//
//
// HACK!!!
//
//
//igetc();
//ExtraData[0] = igetc();
//if (ExtraData[0] == 0x0C)
	//iseek(8, IL_SEEK_CUR);
//	iread(ExtraData+1, 8, 1);
//else
	//iseek(7, IL_SEEK_CUR);
//	iread(ExtraData+1, 7, 1);
//if (igetc() == 0x80)
//	igetc();
					Image->DxtcSize = IL_MAX(Image->Width * Image->Height / 2, 8);
					CompData = (ILubyte*)ialloc(Image->DxtcSize);
					if (CompData == NULL)
						//@TODO: Do all the deallocations needed here!
						return IL_FALSE;

					if (iread(CompData, Image->DxtcSize, 1) != 1) {
						ifree(CompData);
						return IL_FALSE;  //@TODO: Deallocations...
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


	UtxDestroyNameEntries(NameEntries, &Header);
	UtxDestroyExportTable(ExportTable, &Header);
	UtxDestroyImportTable(ImportTable, &Header);
	UtxDestroyPalettes(Palettes, NumPal);


	//return IL_FALSE;
	return ilFixImage();
}

#endif//IL_NO_UTX

