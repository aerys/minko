//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 02/21/2009
//
// Filename: src-ILU/src/ilu_mipmap.c
//
// Description: Generates mipmaps for the current image.
//
//-----------------------------------------------------------------------------


#include "ilu_internal.h"
//#include "ilu_mipmap.h"
//#include "ilu_states.h"


ILboolean iBuildMipmaps(ILimage *Parent, ILuint Width, ILuint Height, ILuint Depth)
{
	ILuint	x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	if (Parent->Width == 1 && Parent->Height == 1 && Parent->Depth == 1) {  // Already at the last mipmap
		return IL_TRUE;
	}

	if (Width == 0)
		Width = 1;
	if (Height == 0)
		Height = 1;
	if (Depth == 0)
		Depth = 1;

	Parent->Mipmaps = iluScale_(Parent, Width, Height, Depth);
	if (Parent->Mipmaps == NULL)
		return IL_FALSE;

	iBuildMipmaps(Parent->Mipmaps, Parent->Mipmaps->Width >> 1, Parent->Mipmaps->Height >> 1, Parent->Mipmaps->Depth >> 1);

	return IL_TRUE;
}


// Note: No longer changes all textures to powers of 2.
ILboolean ILAPIENTRY iluBuildMipmaps()
{
	iluCurImage = ilGetCurImage();
	if (iluCurImage == NULL) {
		ilSetError(ILU_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	// Get rid of any existing mipmaps.
	if (iluCurImage->Mipmaps) {
		ilCloseImage(iluCurImage->Mipmaps);
		iluCurImage->Mipmaps = NULL;
	}

	return iBuildMipmaps(iluCurImage, iluCurImage->Width >> 1, iluCurImage->Height >> 1, iluCurImage->Depth >> 1);
}
