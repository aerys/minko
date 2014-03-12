//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 11/10/2008
//
// Filename: src-ILU/include/ilu_error/ilu_err-arabic.h
//
// Description: Error functions in Arabic, translated by Abdullah Alshammari
//
//-----------------------------------------------------------------------------


#ifndef IL_ERR_ARABIC_H
#define IL_ERR_ARABIC_H

#include "ilu_internal.h"

ILconst_string iluErrorStringsArabic[IL_FILE_READ_ERROR - IL_INVALID_ENUM + 1] = {
	IL_TEXT("enumerant خاطئ"),
    IL_TEXT("خارج حدود الذاكرة"),
	IL_TEXT("نسق الصورة غير مدعوم"),
	IL_TEXT("خطأ داخلي"),
	IL_TEXT("قيمة خاطئة"),
    IL_TEXT("عملية غير شرعية"),
	IL_TEXT("رقم (قيمة) الملف غير شرعي"),
	IL_TEXT("رأس الملف خاطئ"),
	IL_TEXT("البارمتر(المعامل) خاطئ"),
	IL_TEXT("لايمكن فتح الملف"),
	IL_TEXT("امتداد الملف خاطئ"),
	IL_TEXT("الملف موجود"),
	IL_TEXT("out format equivalent"),
	IL_TEXT("المكدس(الستاك) ممتلئ"),
    IL_TEXT("المكدس (الستاك) فارغ"),
	IL_TEXT("تحويل خاطئ"),
	IL_TEXT("أبعاد خاطئة"),
	IL_TEXT("حدث خطأ أثناء قراءة الملف")
};

ILconst_string iluLibErrorStringsArabic[IL_LIB_EXR_ERROR - IL_LIB_GIF_ERROR + 1] = {
	IL_TEXT("خطأ في مكتبة gif"),
	IL_TEXT("خطأ في مكتبة jpeg"),
	IL_TEXT("خطأ في مكتبة png"),
	IL_TEXT("خطأ في مكتبة tiff"),
	IL_TEXT("خطأ في مكتبة mng"),
	IL_TEXT("خطأ في مكتبة jp2"),
	IL_TEXT("خطأ في مكتبة exr")
};

ILconst_string iluMiscErrorStringsArabic[2] = {
	IL_TEXT("لايوجد خطأ"),
	IL_TEXT("خطأ غير معروف")
};

#endif//IL_ERR_ARABIC_H
