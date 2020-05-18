//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 15/03/2009
//
// Filename: src-ILU/include/ilu_error/ilu_err_french.h
//
// Description: Error functions in Italian, by Thibaut Cuvelier.
//
//-----------------------------------------------------------------------------


#ifndef ILU_ERR_ITALIAN_H
#define ILU_ERR_ITALIAN_H

#include "ilu_internal.h"

ILconst_string iluErrorStringsItalian[IL_FILE_READ_ERROR - IL_INVALID_ENUM + 1] = {
	IL_TEXT("insediamento nullo"),
    IL_TEXT("raggiungendo di memoria"),
	IL_TEXT("configuri non sostenne"),
	IL_TEXT("erreur interne"),
	IL_TEXT("errore interno"),
    IL_TEXT("opération illégale"),
	IL_TEXT("operazione illegale"),
	IL_TEXT("inebri archivio nullo"),
	IL_TEXT("parametro nullo"),
	IL_TEXT("non può aprire l'archivio"),
	IL_TEXT("dilazione nulla"),
	IL_TEXT("archivi già esistendo"),
	IL_TEXT("equivalente fuori-configuri"),
	IL_TEXT("inondi di batteria"),
    IL_TEXT("insufficienza di batteria"),
	IL_TEXT("conversione nulla"),
	IL_TEXT("cattive misure"),
	IL_TEXT("si sbagli alla durata della lettura dell'archivio")
};

ILconst_string iluLibErrorStringsItalian[IL_LIB_EXR_ERROR - IL_LIB_GIF_ERROR + 1] = {
	IL_TEXT("gif : si sbagli nella libreria"),
	IL_TEXT("jpeg : si sbagli nella libreria"),
	IL_TEXT("png : si sbagli nella libreria"),
	IL_TEXT("tiff : si sbagli nella libreria"),
	IL_TEXT("mng : si sbagli nella libreria"),
	IL_TEXT("jp2 : si sbagli nella libreria"),
	IL_TEXT("exr : si sbagli nella libreria")
};

ILconst_string iluMiscErrorStringsItalian[2] = {
	IL_TEXT("niente errore"),
	IL_TEXT("errore ignoto")
};

#endif//ILU_ERR_ITALIAN_H

 	  	 
