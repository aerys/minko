//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 03/03/2009
//
// Filename: src-ILU/include/ilu_error/ilu_err_french.h
//
// Description: Error functions in French, by Thibaut Cuvelier.
//
//-----------------------------------------------------------------------------


#ifndef ILU_ERR_FRENCH_H
#define ILU_ERR_FRENCH_H

#include "ilu_internal.h"

ILconst_string iluErrorStringsFrench[IL_FILE_READ_ERROR - IL_INVALID_ENUM + 1] = {
	IL_TEXT("énumération invalide"),
    IL_TEXT("dépassement de mémoire"),
	IL_TEXT("format non supporté"),
	IL_TEXT("erreur interne"),
	IL_TEXT("valeur illégale"),
    IL_TEXT("opération illégale"),
	IL_TEXT("valeur de fichier illégale"),
	IL_TEXT("en-tête de fichier invalide"),
	IL_TEXT("paramètre invalide"),
	IL_TEXT("ne peut pas ouvrir le fichier"),
	IL_TEXT("extension invalide"),
	IL_TEXT("fichier déjà existant"),
	IL_TEXT("équivalent hors-format"),
	IL_TEXT("stack overflow"),	//often used like this in French, so no need to translate
    IL_TEXT("stack underflow"),
	IL_TEXT("conversion invalide"),
	IL_TEXT("mauvaises dimensions"),
	IL_TEXT("erreur lors de la lecture du fichier")
};

ILconst_string iluLibErrorStringsFrench[IL_LIB_EXR_ERROR - IL_LIB_GIF_ERROR + 1] = {
	IL_TEXT("gif : erreur dans la librairie"),	//or: gif : erreur dans la bibliothèque (but less used)
	IL_TEXT("jpeg : erreur dans la librairie"),
	IL_TEXT("png : erreur dans la librairie"),
	IL_TEXT("tiff : erreur dans la librairie"),
	IL_TEXT("mng : erreur dans la librairie"),
	IL_TEXT("jp2 : erreur dans la librairie"),
	IL_TEXT("exr : erreur dans la librairie")
};

ILconst_string iluMiscErrorStringsFrench[2] = {
	IL_TEXT("pas d'erreur"),
	IL_TEXT("erreur inconnue")
};

#endif//ILU_ERR_FRENCH_H

 	  	 
