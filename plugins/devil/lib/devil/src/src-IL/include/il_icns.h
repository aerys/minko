//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 08/23/2008
//
// Filename: src-IL/include/il_icns.h
//
// Description: Reads from a Mac OS X icon (.icns) file.
//
//-----------------------------------------------------------------------------


#ifndef ICNS_H
#define ICNS_H

#include "il_internal.h"

#ifdef _WIN32
	#pragma pack(push, icns_struct, 1)
#endif
typedef struct ICNSHEAD
{
	char		Head[4];	// Must be 'ICNS'
	ILint		Size;		// Total size of the file (including header)
} IL_PACKSTRUCT ICNSHEAD;

typedef struct ICNSDATA
{
	char		ID[4];		// Identifier ('it32', 'il32', etc.)
	ILint		Size;		// Total size of the entry (including identifier)
} IL_PACKSTRUCT ICNSDATA;

#ifdef _WIN32
	#pragma pack(pop, icns_struct)
#endif

ILboolean iIsValidIcns();
ILboolean iLoadIcnsInternal();
ILboolean iIcnsReadData(ILboolean *BaseCreated, ILboolean IsAlpha, ILint Width, ICNSDATA *Entry, ILimage **Image);

#endif//ICNS_H
