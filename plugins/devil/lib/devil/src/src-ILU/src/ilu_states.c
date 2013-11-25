//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-ILU/src/ilu_states.c
//
// Description: The state machine
//
//-----------------------------------------------------------------------------


#include "ilu_internal.h"
#include "ilu_states.h"


ILconst_string _iluVendor	= IL_TEXT("Abysmal Software");
ILconst_string _iluVersion	= IL_TEXT("Developer's Image Library Utilities (ILU) 1.7.8");// IL_TEXT(__DATE__));


ILstring ILAPIENTRY iluGetString(ILenum StringName)
{
	switch (StringName)
	{
		case ILU_VENDOR:
			return (ILstring)_iluVendor;
		//changed 2003-09-04
		case ILU_VERSION_NUM:
			return (ILstring)_iluVersion;
		default:
			ilSetError(ILU_INVALID_PARAM);
			break;
	}
	return NULL;
}


void ILAPIENTRY iluGetIntegerv(ILenum Mode, ILint *Param)
{
	switch (Mode)
	{
		case ILU_VERSION_NUM:
			*Param = ILU_VERSION;
			break;

		case ILU_FILTER:
			*Param = iluFilter;
			break;

		default:
			ilSetError(ILU_INVALID_ENUM);
	}
	return;
}


ILint ILAPIENTRY iluGetInteger(ILenum Mode)
{
	ILint Temp;
	Temp = 0;
	iluGetIntegerv(Mode, &Temp);
	return Temp;
}


ILenum iluFilter = ILU_NEAREST;
ILenum iluPlacement = ILU_CENTER;

void ILAPIENTRY iluImageParameter(ILenum PName, ILenum Param)
{
	switch (PName)
	{
		case ILU_FILTER:
			switch (Param)
			{
				case ILU_NEAREST:
				case ILU_LINEAR:
				case ILU_BILINEAR:
				case ILU_SCALE_BOX:
				case ILU_SCALE_TRIANGLE:
				case ILU_SCALE_BELL:
				case ILU_SCALE_BSPLINE:
				case ILU_SCALE_LANCZOS3:
				case ILU_SCALE_MITCHELL:
					iluFilter = Param;
					break;
				default:
					ilSetError(ILU_INVALID_ENUM);
					return;
			}
			break;

		case ILU_PLACEMENT:
			switch (Param)
			{
				case ILU_LOWER_LEFT:
				case ILU_LOWER_RIGHT:
				case ILU_UPPER_LEFT:
				case ILU_UPPER_RIGHT:
				case ILU_CENTER:
					iluPlacement = Param;
					break;
				default:
					ilSetError(ILU_INVALID_ENUM);
					return;
			}
			break;

		default:
			ilSetError(ILU_INVALID_ENUM);
			return;
	}
	return;
}
