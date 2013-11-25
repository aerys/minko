//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 11/10/2008
//
// Filename: src-ILU/include/ilu_error/ilu_err-japanese.h
//
// Description: Error functions in Japanese, translated by Osamu Ohara
//
//-----------------------------------------------------------------------------


#ifndef ILU_ERR_JAPANESE_H
#define ILU_ERR_JAPANESE_H

#include "ilu_internal.h"

ILconst_string iluErrorStringsJapanese[IL_FILE_READ_ERROR - IL_INVALID_ENUM + 1] = {
	IL_TEXT("無効な列挙値"), //"invalid enumerant"),
    IL_TEXT("メモリ不足"), //"out of memory"),
	IL_TEXT("まだサポートされていないフォーマット"), //"format not supported yet"),
	IL_TEXT("内部エラー"), //"internal error"),
	IL_TEXT("無効な値"), //"invalid value"),
    IL_TEXT("不正なオペレーション"), //"illegal operation"),
	IL_TEXT("不正なファイルの値"), //"illegal file value"),
	IL_TEXT("無効なファイルヘッダ"), //"invalid file header"),
	IL_TEXT("無効なパラメタ"), //"invalid parameter"),
	IL_TEXT("ファイルが開けません"), //"could not open file"),
	IL_TEXT("無効な拡張子"), //"invalid extension"),
	IL_TEXT("ファイルは既に存在しています"), //"file already exists"),
	IL_TEXT("等価フォーマット外"), //"out format equivalent"),
	IL_TEXT("スタックオーバーフロー"), //"stack overflow"),
    IL_TEXT("スタックアンダーフロー"), //"stack underflow"),
	IL_TEXT("無効な変換"), //"invalid conversion"),
	IL_TEXT("不正なサイズ"), //"bad dimensions"),
	IL_TEXT("ファイル読み込みエラー") //"file read error"
};

ILconst_string iluLibErrorStringsJapanese[IL_LIB_EXR_ERROR - IL_LIB_GIF_ERROR + 1] = {
	IL_TEXT("gifライブラリエラー"), //"gif library error"),
	IL_TEXT("jpegライブラリエラー"), //"jpeg library error"),
	IL_TEXT("pngライブラリエラー"), //"png library error"),
	IL_TEXT("tiffライブラリエラー"), //"tiff library error"),
	IL_TEXT("mngライブラリエラー"), //"mng library error"
	IL_TEXT("jp2ライブラリエラー"), //"jp2 library error"
	IL_TEXT("exrライブラリエラー") //"exr library error"
};

ILconst_string iluMiscErrorStringsJapanese[2] = {
	IL_TEXT("エラー無し"),
	IL_TEXT("不明なエラー")
};

#endif//ILU_ERR_JAPANESE_H
