//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 05/25/2001 <--Y2K Compliant! =]
//
// Filename: src-IL/include/il_pal.h
//
// Description: Loads palettes from different file formats
//
//-----------------------------------------------------------------------------


#ifndef IL_PAL_H
#define IL_PAL_H

#include "il_internal.h"

#define BUFFLEN	256
#define PALBPP	3

#ifdef _MSC_VER
#pragma pack(push, packed_struct, 1)
#endif
typedef struct HALOHEAD
{
	ILushort	Id;  // 'AH'
	ILshort		Version;
	ILshort		Size;
	ILbyte		Filetype;
	ILbyte		Subtype;
	//ILshort	Brdid, Grmode;
	ILint		Ignored;
	ILushort	MaxIndex;  // Colors = maxindex + 1
	ILushort	MaxRed;
	ILushort	MaxGreen;
	ILushort	MaxBlue;
	/*ILbyte	Signature[8];
	ILbyte		Filler[12];*/
	ILbyte		Filler[20];  // Always 0 by PSP 4
} IL_PACKSTRUCT HALOHEAD;
#ifdef _MSC_VER
#pragma pack(pop,  packed_struct)
#endif

ILboolean	ilLoadJascPal(ILconst_string FileName);
ILboolean	ilSaveJascPal(ILconst_string FileName);
char		*iFgetw(ILubyte *Buff, ILint MaxLen, FILE *File);
ILboolean	ilLoadHaloPal(ILconst_string FileName);
ILboolean	ilLoadColPal(ILconst_string FileName);
ILboolean	ilLoadActPal(ILconst_string FileName);
ILboolean	ilLoadPltPal(ILconst_string FileName);

#endif//IL_PAL_H
