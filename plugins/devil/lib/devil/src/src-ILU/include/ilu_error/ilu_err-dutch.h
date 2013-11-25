//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 11/08/2008
//
// Filename: src-ILU/include/ilu_error/ilu_err_dutch.h
//
// Description: Error functions in Dutch, translated by Bart De Lathouwer
//
//-----------------------------------------------------------------------------


#ifndef ILU_ERR_DUTCH_H
#define ILU_ERR_DUTCH_H

#include "ilu_internal.h"

ILconst_string iluErrorStringsDutch[IL_FILE_READ_ERROR - IL_INVALID_ENUM + 1] = {
	IL_TEXT("Ongeldige enumerant"),
	IL_TEXT("Geen vrij geheugen meer"),
	IL_TEXT("Format wordt nog niet ondersteund"),
	IL_TEXT("Interne fout"),
	IL_TEXT("Ongeldige waarde"),
	IL_TEXT("Foute bewerking"),
	IL_TEXT("Foute bestandswaarde"),
	IL_TEXT("Foute bestandsbegin"),
	IL_TEXT("Ongeldige parameter"),
	IL_TEXT("Kan het bestand niet openen"),
	IL_TEXT("ongeldige"),
	IL_TEXT("Bestand bestaat reeds"),
	IL_TEXT("uitgaand formaat equivalent"),
	IL_TEXT("stapel overstroming"),
	IL_TEXT("stapel onderstroming"),
	IL_TEXT("ongeldige omzetting"),
	IL_TEXT("slechte afmetingen"),
	IL_TEXT("Leesfout in bestand")
};

ILconst_string iluLibErrorStringsDutch[IL_LIB_EXR_ERROR - IL_LIB_GIF_ERROR + 1] = {
	IL_TEXT("fout in gif bibliotheek"),
	IL_TEXT("fout in jpeg bibliotheek"),
	IL_TEXT("fout in png bibliotheek"),
	IL_TEXT("fout in tiff bibliotheek"),
	IL_TEXT("fout in mng bibliotheek"),
	IL_TEXT("fout in jp2 bibliotheek"),
	IL_TEXT("fout in exr bibliotheek")
};

ILconst_string iluMiscErrorStringsDutch[2] = {
	IL_TEXT("geen fout"),
	IL_TEXT("onbekende fout")
};

#endif//ILU_ERR_DUTCH_H
