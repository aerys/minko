//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 11/08/2008
//
// Filename: src-ILU/include/ilu_error/ilu_err-spanish.h
//
// Description: Error functions in Spanish, translated by Carlos Aragonés
//
//-----------------------------------------------------------------------------


#ifndef ILU_ERR_SPANISH_H
#define ILU_ERR_SPANISH_H

#include "ilu_internal.h"

ILconst_string iluErrorStringsSpanish[IL_FILE_READ_ERROR - IL_INVALID_ENUM + 1] = {
	IL_TEXT("enumerador incorrecto"),
    IL_TEXT("no queda memoria disponible"),
	IL_TEXT("formato no soportado todavía"),
	IL_TEXT("error interno"),
	IL_TEXT("valor incorrecto"),
    IL_TEXT("operación ilegaIL_TEXT("),
	IL_TEXT("valor de fichero ilegaIL_TEXT("),
	IL_TEXT("cabecera incorrecta"),
	IL_TEXT("parámetro incorrecto"),
	IL_TEXT("no se puede abrir el fichero"),
	IL_TEXT("extensión desconocida"),
	IL_TEXT("el fichero ya existe"),
	IL_TEXT("formato de salida equivalente"),
	IL_TEXT("desbordamiento superior de pila"),
    IL_TEXT("desbordamiento inferior de pila"),
	IL_TEXT("conversión incorrecta"),
	IL_TEXT("número de dimensiones incorrecto"),
	IL_TEXT("error de lectura en el fichero")
};

ILconst_string iluLibErrorStringsSpanish[IL_LIB_EXR_ERROR - IL_LIB_GIF_ERROR + 1] = {
	IL_TEXT("error en la librería gif"),  // the correct translation will be 'error en la biblioteca gif', but nobody in spain uses the word 'biblioteca' for the translation the word 'library'. Almost all Spanish programmers translate 'library' incorrectly to 'librería'.
	IL_TEXT("error en la librería jpeg"),
	IL_TEXT("error en la librería png"),
	IL_TEXT("error en la librería tiff"),
	IL_TEXT("error en la librería mng"),
	IL_TEXT("error en la librería jp2"),
	IL_TEXT("error en la librería exr"),
};

ILconst_string iluMiscErrorStringsSpanish[2] = {
	IL_TEXT("no error"),
	IL_TEXT("unknown error")
};

#endif//ILU_ERR_SPANISH_H
