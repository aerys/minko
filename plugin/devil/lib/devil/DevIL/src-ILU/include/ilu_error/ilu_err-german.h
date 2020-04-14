//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 12/05/2008
//
// Filename: src-ILU/include/ilu_error/ilu_err_German.h
//
// Description: Error functions in German, translated by Andreas Stöckel.
//
//-----------------------------------------------------------------------------


#ifndef ILU_ERR_GERMAN_H
#define ILU_ERR_GERMAN_H

#include "ilu_internal.h"

ILconst_string iluErrorStringsGerman[IL_FILE_READ_ERROR - IL_INVALID_ENUM + 1] = {
	IL_TEXT("Ungültiger Enumerator"),
    IL_TEXT("Kein Speicher verfügbar"),
	IL_TEXT("Das Format wird noch nicht unterstützt"),
	IL_TEXT("Interner Fehler"),
	IL_TEXT("Ungültiger Wert"),
    IL_TEXT("Unzulässige Operation"),
	IL_TEXT("Unzulässiger Datei-Wert"),
	IL_TEXT("Unzulässiger Datei-Header"),
	IL_TEXT("Unzulässiger Parameter"),
	IL_TEXT("Datei konnte nicht geöffnet werden"),
	IL_TEXT("Ungültiger Erweiterung"),
	IL_TEXT("Die Datei existiert bereits"),
	IL_TEXT("out format equivalent"),
	IL_TEXT("Stack Überlauf"),
    IL_TEXT("Stack Unterlauf"),
	IL_TEXT("Ungültige Konvertierung"),
	IL_TEXT("Unzulässige Abmessungen"),
	IL_TEXT("Fehler beim Lesen der Datei")
};

ILconst_string iluLibErrorStringsGerman[IL_LIB_EXR_ERROR - IL_LIB_GIF_ERROR + 1] = {
	IL_TEXT("Fehler in der gif Bibliothek"),
	IL_TEXT("Fehler in der jpeg Bibliothek"),
	IL_TEXT("Fehler in der png Bibliothek"),
	IL_TEXT("Fehler in der tiff Bibliothek"),
	IL_TEXT("Fehler in der mng Bibliothek"),
	IL_TEXT("Fehler in der jp2 Bibliothek"),
	IL_TEXT("Fehler in der exr Bibliothek")
};

ILconst_string iluMiscErrorStringsGerman[2] = {
	IL_TEXT("Kein Fehler"),
	IL_TEXT("Unbekannter Fehler")
};

#endif//ILU_ERR_GERMAN_H
