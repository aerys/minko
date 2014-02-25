//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 11/08/2008
//
// Filename: src-ILU/include/ilu_error/ilu_err_english.h
//
// Description: Error functions in English.
//
//-----------------------------------------------------------------------------


#ifndef ILU_ERR_ENGLISH_H
#define ILU_ERR_ENGLISH_H

#include "ilu_internal.h"

ILconst_string iluErrorStringsEnglish[IL_FILE_READ_ERROR - IL_INVALID_ENUM + 1] = {
	IL_TEXT("invalid enumerant"),
    IL_TEXT("out of memory"),
	IL_TEXT("format not supported yet"),
	IL_TEXT("internal error"),
	IL_TEXT("invalid value"),
    IL_TEXT("illegal operation"),
	IL_TEXT("illegal file value"),
	IL_TEXT("invalid file header"),
	IL_TEXT("invalid parameter"),
	IL_TEXT("could not open file"),
	IL_TEXT("invalid extension"),
	IL_TEXT("file already exists"),
	IL_TEXT("out format equivalent"),
	IL_TEXT("stack overflow"),
    IL_TEXT("stack underflow"),
	IL_TEXT("invalid conversion"),
	IL_TEXT("bad dimensions"),
	IL_TEXT("file read error")
};

ILconst_string iluLibErrorStringsEnglish[IL_LIB_EXR_ERROR - IL_LIB_GIF_ERROR + 1] = {
	IL_TEXT("gif library error"),
	IL_TEXT("jpeg library error"),
	IL_TEXT("png library error"),
	IL_TEXT("tiff library error"),
	IL_TEXT("mng library error"),
	IL_TEXT("jp2 library error"),
	IL_TEXT("exr library error")
};

ILconst_string iluMiscErrorStringsEnglish[2] = {
	IL_TEXT("no error"),
	IL_TEXT("unknown error")
};

#endif//ILU_ERR_ENGLISH_H
