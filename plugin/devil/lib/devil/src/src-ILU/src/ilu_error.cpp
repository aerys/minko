//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/16/2009
//
// Filename: src-ILU/src/ilu_error.cpp
//
// Description: Error functions
//
//-----------------------------------------------------------------------------


#include "ilu_internal.h"
#include "ilu_error/ilu_err-arabic.h"
#include "ilu_error/ilu_err-dutch.h"
#include "ilu_error/ilu_err-english.h"
#include "ilu_error/ilu_err-french.h"
#include "ilu_error/ilu_err-german.h"
#include "ilu_error/ilu_err-italian.h"
#include "ilu_error/ilu_err-japanese.h"
#include "ilu_error/ilu_err-spanish.h"


ILconst_string *iluErrors;
ILconst_string *iluLibErrors;
ILconst_string *iluMiscErrors;
#define ILU_NUM_LANGUAGES 8

ILconst_string *iluErrorStrings[ILU_NUM_LANGUAGES] = {
	iluErrorStringsEnglish,
	iluErrorStringsArabic,
	iluErrorStringsDutch,
	iluErrorStringsFrench,
	iluErrorStringsJapanese,
	iluErrorStringsSpanish,
	iluErrorStringsGerman,
	iluErrorStringsItalian
};

ILconst_string *iluLibErrorStrings[ILU_NUM_LANGUAGES] = {
	iluLibErrorStringsEnglish,
	iluLibErrorStringsArabic,
	iluLibErrorStringsDutch,
	iluLibErrorStringsFrench,
	iluLibErrorStringsJapanese,
	iluLibErrorStringsSpanish,
	iluLibErrorStringsGerman,
	iluLibErrorStringsItalian
};

ILconst_string *iluMiscErrorStrings[ILU_NUM_LANGUAGES] = {
	iluMiscErrorStringsEnglish,
	iluMiscErrorStringsArabic,
	iluMiscErrorStringsDutch,
	iluMiscErrorStringsFrench,
	iluMiscErrorStringsJapanese,
	iluMiscErrorStringsSpanish,
	iluMiscErrorStringsGerman,
	iluMiscErrorStringsItalian
};


ILconst_string ILAPIENTRY iluErrorString(ILenum Error)
{
	// Now we are dealing with Unicode strings.
	if (Error == IL_NO_ERROR) {
		return iluMiscErrors[0];
	}
	if (Error == IL_UNKNOWN_ERROR) {
		return iluMiscErrors[1];
	}
	if (Error >= IL_INVALID_ENUM && Error <= IL_FILE_READ_ERROR) {
		return (ILstring)iluErrors[Error - IL_INVALID_ENUM];
	}
	if (Error >= IL_LIB_GIF_ERROR && Error <= IL_LIB_EXR_ERROR) {
		return (ILstring)iluLibErrors[Error - IL_LIB_GIF_ERROR];
	}

	return iluMiscErrors[0];
}


ILboolean ILAPIENTRY iluSetLanguage(ILenum Language)
{
	switch (Language)
	{
		case ILU_ENGLISH:
		case ILU_ARABIC:
		case ILU_DUTCH:
		case ILU_FRENCH:
		case ILU_JAPANESE:
		case ILU_SPANISH:
		case ILU_GERMAN:
		case ILU_ITALIAN:
			iluErrors = iluErrorStrings[Language - ILU_ENGLISH];
			iluLibErrors = iluLibErrorStrings[Language - ILU_ENGLISH];
			iluMiscErrors = iluMiscErrorStrings[Language - ILU_ENGLISH];
			break;

		default:
			ilSetError(IL_INVALID_ENUM);
			return IL_FALSE;
	}

	return IL_TRUE;
}

 	  	 
