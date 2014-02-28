//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 11/07/2008
//
// Filename: src-IL/src/il_register.c
//
// Description: Allows the caller to specify user-defined callback functions
//				 to open files DevIL does not support, to parse files
//				 differently, or anything else a person can think up.
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#include "il_register.h"
#include <string.h>


// Linked lists of registered formats
iFormatL *LoadProcs = NULL;
iFormatS *SaveProcs = NULL;


ILboolean ILAPIENTRY ilRegisterLoad(ILconst_string Ext, IL_LOADPROC Load) {
	iFormatL *TempNode, *NewNode;

	TempNode = LoadProcs;
	if (TempNode != NULL) {
		while (TempNode->Next != NULL) {
			TempNode = TempNode->Next;
			if (!iStrCmp(TempNode->Ext, Ext)) {  // already registered
				return IL_TRUE;
			}
		}
	}

	NewNode = (iFormatL*)ialloc(sizeof(iFormatL));
	if (NewNode == NULL) {
		return IL_FALSE;
	}

	if (LoadProcs == NULL) {
		LoadProcs = NewNode;
	}
	else {
		TempNode->Next = NewNode;
	}

	NewNode->Ext = ilStrDup(Ext);
	NewNode->Load = Load;
	NewNode->Next = NULL;

	return IL_TRUE;
}


ILboolean ILAPIENTRY ilRegisterSave(ILconst_string Ext, IL_SAVEPROC Save)
{
	iFormatS *TempNode, *NewNode;

	TempNode = SaveProcs;
	if (TempNode != NULL) {
		while (TempNode->Next != NULL) {
			TempNode = TempNode->Next;
			if (!iStrCmp(TempNode->Ext, Ext)) {  // already registered
				return IL_TRUE;
			}
		}
	}

	NewNode = (iFormatS*)ialloc(sizeof(iFormatL));
	if (NewNode == NULL) {
		return IL_FALSE;
	}

	if (SaveProcs == NULL) {
		SaveProcs = NewNode;
	}
	else {
		TempNode->Next = NewNode;
	}

	NewNode->Ext = ilStrDup(Ext);
	NewNode->Save = Save;
	NewNode->Next = NULL;

	return IL_TRUE;
}


//! Unregisters a load extension - doesn't have to be called.
ILboolean ILAPIENTRY ilRemoveLoad(ILconst_string Ext)
{
	iFormatL *TempNode = LoadProcs, *PrevNode = NULL;

	while (TempNode != NULL) {
		if (!iStrCmp(Ext, TempNode->Ext)) {
			if (PrevNode == NULL) {  // first node in the list
				LoadProcs = TempNode->Next;
				ifree((void*)TempNode->Ext);
				ifree(TempNode);
			}
			else {
				PrevNode->Next = TempNode->Next;
				ifree((void*)TempNode->Ext);
				ifree(TempNode);
			}

			return IL_TRUE;
		}

		PrevNode = TempNode;
		TempNode = TempNode->Next;
	}

	return IL_FALSE;
}


//! Unregisters a save extension - doesn't have to be called.
ILboolean ILAPIENTRY ilRemoveSave(ILconst_string Ext)
{
	iFormatS *TempNode = SaveProcs, *PrevNode = NULL;

	while (TempNode != NULL) {
		if (!iStrCmp(Ext, TempNode->Ext)) {
			if (PrevNode == NULL) {  // first node in the list
				SaveProcs = TempNode->Next;
				ifree((void*)TempNode->Ext);
				ifree(TempNode);
			}
			else {
				PrevNode->Next = TempNode->Next;
				ifree((void*)TempNode->Ext);
				ifree(TempNode);
			}

			return IL_TRUE;
		}

		PrevNode = TempNode;
		TempNode = TempNode->Next;
	}

	return IL_FALSE;
}


// Automatically removes all registered formats.
void ilRemoveRegistered()
{
	iFormatL *TempNodeL = LoadProcs;
	iFormatS *TempNodeS = SaveProcs;

	while (LoadProcs != NULL) {
		TempNodeL = LoadProcs->Next;
		ifree((void*)LoadProcs->Ext);
		ifree(LoadProcs);
		LoadProcs = TempNodeL;
	}

	while (SaveProcs != NULL) {
		TempNodeS = SaveProcs->Next;
		ifree((void*)SaveProcs->Ext);
		ifree(SaveProcs);
		SaveProcs = TempNodeS;
	}

	return;
}


ILboolean iRegisterLoad(ILconst_string FileName)
{
	iFormatL	*TempNode = LoadProcs;
	ILstring	Ext = iGetExtension(FileName);
	ILenum		Error;

	if (!Ext)
		return IL_FALSE;

	while (TempNode != NULL) {
		if (!iStrCmp(Ext, TempNode->Ext)) {
			Error = TempNode->Load(FileName);
			if (Error == IL_NO_ERROR || Error == 0) {  // 0 and IL_NO_ERROR are both valid.
				return IL_TRUE;
			}
			else {
				ilSetError(Error);
				return IL_FALSE;
			}
		}
		TempNode = TempNode->Next;
	}

	return IL_FALSE;
}


ILboolean iRegisterSave(ILconst_string FileName)
{
	iFormatS	*TempNode = SaveProcs;
	ILstring	Ext = iGetExtension(FileName);
	ILenum		Error;

	if (!Ext)
		return IL_FALSE;

	while (TempNode != NULL) {
		if (!iStrCmp(Ext, TempNode->Ext)) {
			Error = TempNode->Save(FileName);
			if (Error == IL_NO_ERROR || Error == 0) {  // 0 and IL_NO_ERROR are both valid.
				return IL_TRUE;
			}
			else {
				ilSetError(Error);
				return IL_FALSE;
			}
		}
		TempNode = TempNode->Next;
	}

	return IL_FALSE;
}


//
// "Reporting" functions
//

void ILAPIENTRY ilRegisterOrigin(ILenum Origin)
{
	switch (Origin)
	{
		case IL_ORIGIN_LOWER_LEFT:
		case IL_ORIGIN_UPPER_LEFT:
			iCurImage->Origin = Origin;
			break;
		default:
			ilSetError(IL_INVALID_ENUM);
	}
	return;
}


void ILAPIENTRY ilRegisterFormat(ILenum Format)
{
	switch (Format)
	{
		case IL_COLOUR_INDEX:
		case IL_RGB:
		case IL_RGBA:
		case IL_BGR:
		case IL_BGRA:
		case IL_LUMINANCE:
                case IL_LUMINANCE_ALPHA:
			iCurImage->Format = Format;
			break;
		default:
			ilSetError(IL_INVALID_ENUM);
	}
	return;
}


ILboolean ILAPIENTRY ilRegisterNumFaces(ILuint Num)
{
	ILimage *Next, *Prev;

	ilBindImage(ilGetCurName());  // Make sure the current image is actually bound.
	ilCloseImage(iCurImage->Faces);  // Close any current mipmaps.

	iCurImage->Faces = NULL;
	if (Num == 0)  // Just gets rid of all the mipmaps.
		return IL_TRUE;

	iCurImage->Faces = ilNewImage(1, 1, 1, 1, 1);
	if (iCurImage->Faces == NULL)
		return IL_FALSE;
	Next = iCurImage->Faces;
	Num--;

	while (Num) {
		Next->Faces = ilNewImage(1, 1, 1, 1, 1);
		if (Next->Faces == NULL) {
			// Clean up before we error out.
			Prev = iCurImage->Faces;
			while (Prev) {
				Next = Prev->Faces;
				ilCloseImage(Prev);
				Prev = Next;
			}
			return IL_FALSE;
		}
		Next = Next->Faces;
		Num--;
	}

	return IL_TRUE;
}


ILboolean ILAPIENTRY ilRegisterMipNum(ILuint Num)
{
	ILimage *Next, *Prev;

	ilBindImage(ilGetCurName());  // Make sure the current image is actually bound.
	ilCloseImage(iCurImage->Mipmaps);  // Close any current mipmaps.

	iCurImage->Mipmaps = NULL;
	if (Num == 0)  // Just gets rid of all the mipmaps.
		return IL_TRUE;

	iCurImage->Mipmaps = ilNewImage(1, 1, 1, 1, 1);
	if (iCurImage->Mipmaps == NULL)
		return IL_FALSE;
	Next = iCurImage->Mipmaps;
	Num--;

	while (Num) {
		Next->Next = ilNewImage(1, 1, 1, 1, 1);
		if (Next->Next == NULL) {
			// Clean up before we error out.
			Prev = iCurImage->Mipmaps;
			while (Prev) {
				Next = Prev->Next;
				ilCloseImage(Prev);
				Prev = Next;
			}
			return IL_FALSE;
		}
		Next = Next->Next;
		Num--;
	}

	return IL_TRUE;
}


ILboolean ILAPIENTRY ilRegisterNumImages(ILuint Num)
{
	ILimage *Next, *Prev;

	ilBindImage(ilGetCurName());  // Make sure the current image is actually bound.
	ilCloseImage(iCurImage->Next);  // Close any current "next" images.

	iCurImage->Next = NULL;
	if (Num == 0)  // Just gets rid of all the "next" images.
		return IL_TRUE;

	iCurImage->Next = ilNewImage(1, 1, 1, 1, 1);
	if (iCurImage->Next == NULL)
		return IL_FALSE;
	Next = iCurImage->Next;
	Num--;

	while (Num) {
		Next->Next = ilNewImage(1, 1, 1, 1, 1);
		if (Next->Next == NULL) {
			// Clean up before we error out.
			Prev = iCurImage->Next;
			while (Prev) {
				Next = Prev->Next;
				ilCloseImage(Prev);
				Prev = Next;
			}
			return IL_FALSE;
		}
		Next = Next->Next;
		Num--;
	}

	return IL_TRUE;
}


void ILAPIENTRY ilRegisterType(ILenum Type)
{
	switch (Type)
	{
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
		case IL_SHORT:
		case IL_UNSIGNED_SHORT:
		case IL_INT:
		case IL_UNSIGNED_INT:
		case IL_FLOAT:
		case IL_DOUBLE:
			iCurImage->Type = Type;
			break;
		default:
			ilSetError(IL_INVALID_ENUM);
	}

	return;
}


void ILAPIENTRY ilRegisterPal(void *Pal, ILuint Size, ILenum Type)
{
	if (!iCurImage->Pal.Palette || !iCurImage->Pal.PalSize || iCurImage->Pal.PalType != IL_PAL_NONE) {
		ifree(iCurImage->Pal.Palette);
	}

	iCurImage->Pal.PalSize = Size;
	iCurImage->Pal.PalType = Type;
	iCurImage->Pal.Palette = (ILubyte*)ialloc(Size);
	if (iCurImage->Pal.Palette == NULL)
		return;

	if (Pal != NULL) {
		memcpy(iCurImage->Pal.Palette, Pal, Size);
	}
	else {
		ilSetError(IL_INVALID_PARAM);
	}
	
	return;
}


ILboolean ILAPIENTRY ilSetDuration(ILuint Duration)
{
	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	iCurImage->Duration = Duration;

	return IL_TRUE;
}
