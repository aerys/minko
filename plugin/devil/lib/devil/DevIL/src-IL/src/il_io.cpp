//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_io.cpp
//
// Description: Determines image types and loads/saves images
//
//-----------------------------------------------------------------------------

#include "il_internal.h"
#include "il_register.h"
#include "il_pal.h"
#include <string.h>


// Returns a widened version of a string.
// Make sure to free this after it is used.  Code help from
//  https://buildsecurityin.us-cert.gov/daisy/bsi-rules/home/g1/769-BSI.html
#if defined(_UNICODE)
wchar_t *WideFromMultiByte(const char *Multi)
{
	ILint	Length;
	wchar_t	*Temp;

	Length = (ILint)mbstowcs(NULL, (const char*)Multi, 0) + 1; // note error return of -1 is possible
	if (Length == 0) {
		ilSetError(IL_INVALID_PARAM);
		return NULL;
	}
	if (Length > ULONG_MAX/sizeof(wchar_t)) {
		ilSetError(IL_INTERNAL_ERROR);
		return NULL;
	}
	Temp = (wchar_t*)ialloc(Length * sizeof(wchar_t));
	mbstowcs(Temp, (const char*)Multi, Length); 

	return Temp;
}
#endif


ILenum ILAPIENTRY ilTypeFromExt(ILconst_string FileName)
{
	ILenum		Type;
	ILstring	Ext;

	if (FileName == NULL || ilStrLen(FileName) < 1) {
		ilSetError(IL_INVALID_PARAM);
		return IL_TYPE_UNKNOWN;
	}

	Ext = iGetExtension(FileName);
	//added 2003-08-31: fix sf bug 789535
	if (Ext == NULL) {
		return IL_TYPE_UNKNOWN;
	}

	if (!iStrCmp(Ext, IL_TEXT("tga")) || !iStrCmp(Ext, IL_TEXT("vda")) ||
		!iStrCmp(Ext, IL_TEXT("icb")) || !iStrCmp(Ext, IL_TEXT("vst")))
		Type = IL_TGA;
	else if (!iStrCmp(Ext, IL_TEXT("jpg")) || !iStrCmp(Ext, IL_TEXT("jpe")) ||
		!iStrCmp(Ext, IL_TEXT("jpeg")) || !iStrCmp(Ext, IL_TEXT("jif")) || !iStrCmp(Ext, IL_TEXT("jfif")))
		Type = IL_JPG;
	else if (!iStrCmp(Ext, IL_TEXT("jp2")) || !iStrCmp(Ext, IL_TEXT("jpx")) ||
		!iStrCmp(Ext, IL_TEXT("j2k")) || !iStrCmp(Ext, IL_TEXT("j2c")))
		Type = IL_JP2;
	else if (!iStrCmp(Ext, IL_TEXT("dds")))
		Type = IL_DDS;
	else if (!iStrCmp(Ext, IL_TEXT("png")))
		Type = IL_PNG;
	else if (!iStrCmp(Ext, IL_TEXT("bmp")) || !iStrCmp(Ext, IL_TEXT("dib")))
		Type = IL_BMP;
	else if (!iStrCmp(Ext, IL_TEXT("gif")))
		Type = IL_GIF;
	else if (!iStrCmp(Ext, IL_TEXT("blp")))
		Type = IL_BLP;
	else if (!iStrCmp(Ext, IL_TEXT("cut")))
		Type = IL_CUT;
	else if (!iStrCmp(Ext, IL_TEXT("dcm")) || !iStrCmp(Ext, IL_TEXT("dicom")))
		Type = IL_DICOM;
	else if (!iStrCmp(Ext, IL_TEXT("dpx")))
		Type = IL_DPX;
	else if (!iStrCmp(Ext, IL_TEXT("exr")))
		Type = IL_EXR;
	else if (!iStrCmp(Ext, IL_TEXT("fit")) || !iStrCmp(Ext, IL_TEXT("fits")))
		Type = IL_FITS;
	else if (!iStrCmp(Ext, IL_TEXT("ftx")))
		Type = IL_FTX;
	else if (!iStrCmp(Ext, IL_TEXT("hdr")))
		Type = IL_HDR;
	else if (!iStrCmp(Ext, IL_TEXT("iff")))
		Type = IL_IFF;
	else if (!iStrCmp(Ext, IL_TEXT("ilbm")) || !iStrCmp(Ext, IL_TEXT("lbm")) ||
        !iStrCmp(Ext, IL_TEXT("ham")))
		Type = IL_ILBM;
	else if (!iStrCmp(Ext, IL_TEXT("ico")) || !iStrCmp(Ext, IL_TEXT("cur")))
		Type = IL_ICO;
	else if (!iStrCmp(Ext, IL_TEXT("icns")))
		Type = IL_ICNS;
	else if (!iStrCmp(Ext, IL_TEXT("iwi")))
		Type = IL_IWI;
	else if (!iStrCmp(Ext, IL_TEXT("iwi")))
		Type = IL_IWI;
	else if (!iStrCmp(Ext, IL_TEXT("jng")))
		Type = IL_JNG;
	else if (!iStrCmp(Ext, IL_TEXT("ktx")))
		Type = IL_KTX;
	else if (!iStrCmp(Ext, IL_TEXT("lif")))
		Type = IL_LIF;
	else if (!iStrCmp(Ext, IL_TEXT("mdl")))
		Type = IL_MDL;
	else if (!iStrCmp(Ext, IL_TEXT("mng")) || !iStrCmp(Ext, IL_TEXT("jng")))
		Type = IL_MNG;
	else if (!iStrCmp(Ext, IL_TEXT("mp3")))
		Type = IL_MP3;
	else if (!iStrCmp(Ext, IL_TEXT("pcd")))
		Type = IL_PCD;
	else if (!iStrCmp(Ext, IL_TEXT("pcx")))
		Type = IL_PCX;
	else if (!iStrCmp(Ext, IL_TEXT("pic")))
		Type = IL_PIC;
	else if (!iStrCmp(Ext, IL_TEXT("pix")))
		Type = IL_PIX;
	else if (!iStrCmp(Ext, IL_TEXT("pbm")) || !iStrCmp(Ext, IL_TEXT("pgm")) ||
		!iStrCmp(Ext, IL_TEXT("pnm")) || !iStrCmp(Ext, IL_TEXT("ppm")))
		Type = IL_PNM;
	else if (!iStrCmp(Ext, IL_TEXT("psd")) || !iStrCmp(Ext, IL_TEXT("pdd")))
		Type = IL_PSD;
	else if (!iStrCmp(Ext, IL_TEXT("psp")))
		Type = IL_PSP;
	else if (!iStrCmp(Ext, IL_TEXT("pxr")))
		Type = IL_PXR;
	else if (!iStrCmp(Ext, IL_TEXT("rot")))
		Type = IL_ROT;
	else if (!iStrCmp(Ext, IL_TEXT("sgi")) || !iStrCmp(Ext, IL_TEXT("bw")) ||
		!iStrCmp(Ext, IL_TEXT("rgb")) || !iStrCmp(Ext, IL_TEXT("rgba")))
		Type = IL_SGI;
	else if (!iStrCmp(Ext, IL_TEXT("sun")) || !iStrCmp(Ext, IL_TEXT("ras")) ||
			 !iStrCmp(Ext, IL_TEXT("rs")) || !iStrCmp(Ext, IL_TEXT("im1")) ||
			 !iStrCmp(Ext, IL_TEXT("im8")) || !iStrCmp(Ext, IL_TEXT("im24")) ||
			 !iStrCmp(Ext, IL_TEXT("im32")))
		Type = IL_SUN;
	else if (!iStrCmp(Ext, IL_TEXT("texture")))
		Type = IL_TEXTURE;
	else if (!iStrCmp(Ext, IL_TEXT("tif")) || !iStrCmp(Ext, IL_TEXT("tiff")))
		Type = IL_TIF;
	else if (!iStrCmp(Ext, IL_TEXT("tpl")))
		Type = IL_TPL;
	else if (!iStrCmp(Ext, IL_TEXT("utx")))
		Type = IL_UTX;
	else if (!iStrCmp(Ext, IL_TEXT("vtf")))
		Type = IL_VTF;
	else if (!iStrCmp(Ext, IL_TEXT("wal")))
		Type = IL_WAL;
	else if (!iStrCmp(Ext, IL_TEXT("wbmp")))
		Type = IL_WBMP;
	else if (!iStrCmp(Ext, IL_TEXT("wdp")) || !iStrCmp(Ext, IL_TEXT("hdp")))
		Type = IL_WDP;
	else if (!iStrCmp(Ext, IL_TEXT("xpm")))
		Type = IL_XPM;
	else
		Type = IL_TYPE_UNKNOWN;

	return Type;
}


//changed 2003-09-17 to ILAPIENTRY
ILenum ILAPIENTRY ilDetermineType(ILconst_string FileName)
{
	ILHANDLE	File;
	ILenum		Type;

	if (FileName == NULL)
		return IL_TYPE_UNKNOWN;

	File = iopenr(FileName);
	if (File == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}
	Type = ilDetermineTypeF(File);
	icloser(File);

	return Type;
}


ILenum ILAPIENTRY ilDetermineTypeF(ILHANDLE File)
{
	if (File == NULL)
		return IL_TYPE_UNKNOWN;

	#ifndef IL_NO_JPG
	if (ilIsValidJpegF(File))
		return IL_JPG;
	#endif

	#ifndef IL_NO_DDS
	if (ilIsValidDdsF(File))
		return IL_DDS;
	#endif

	#ifndef IL_NO_PNG
	if (ilIsValidPngF(File))
		return IL_PNG;
	#endif

	#ifndef IL_NO_BMP
	if (ilIsValidBmpF(File))
		return IL_BMP;
	#endif

	#ifndef IL_NO_EXR
	if (ilIsValidExrF(File))
		return IL_EXR;
	#endif

	#ifndef IL_NO_GIF
	if (ilIsValidGifF(File))
		return IL_GIF;
	#endif

	#ifndef IL_NO_HDR
	if (ilIsValidHdrF(File))
		return IL_HDR;
	#endif

	#ifndef IL_NO_ICNS
	if (ilIsValidIcnsF(File))
		return IL_ICNS;
	#endif

	#ifndef IL_NO_ILBM
	if (ilIsValidIlbmF(File))
		return IL_ILBM;
	#endif

	#ifndef IL_NO_IWI
	if (ilIsValidIwiF(File))
		return IL_IWI;
	#endif

	#ifndef IL_NO_JP2
	if (ilIsValidJp2F(File))
		return IL_JP2;
	#endif

	#ifndef IL_NO_KTX
	if (ilIsValidKtxF(File))
		return IL_KTX;
	#endif

	#ifndef IL_NO_LIF
	if (ilIsValidLifF(File))
		return IL_LIF;
	#endif

	#ifndef IL_NO_MDL
	if (ilIsValidMdlF(File))
		return IL_MDL;
	#endif

	#ifndef IL_NO_MDL
	if (ilIsValidMp3F(File))
		return IL_MP3;
	#endif

	#ifndef IL_NO_PCX
	if (ilIsValidPcxF(File))
		return IL_PCX;
	#endif

	#ifndef IL_NO_PIC
	if (ilIsValidPicF(File))
		return IL_PIC;
	#endif

	#ifndef IL_NO_PNM
	if (ilIsValidPnmF(File))
		return IL_PNM;
	#endif

	#ifndef IL_NO_PSD
	if (ilIsValidPsdF(File))
		return IL_PSD;
	#endif

	#ifndef IL_NO_PSP
	if (ilIsValidPspF(File))
		return IL_PSP;
	#endif

	#ifndef IL_NO_SGI
	if (ilIsValidSgiF(File))
		return IL_SGI;
	#endif

	#ifndef IL_NO_SUN
	if (ilIsValidSunF(File))
		return IL_SUN;
	#endif

	#ifndef IL_NO_TIF
	if (ilIsValidTiffF(File))
		return IL_TIF;
	#endif

	#ifndef IL_NO_TPL
	if (ilIsValidTplF(File))
		return IL_TPL;
	#endif

	#ifndef IL_NO_VTF
	if (ilIsValidVtfF(File))
		return IL_VTF;
	#endif

	#ifndef IL_NO_XPM
	if (ilIsValidXpmF(File))
		return IL_XPM;
	#endif

	//moved tga to end of list because it has no magic number
	//in header to assure that this is really a tga... (20040218)
	#ifndef IL_NO_TGA
	if (ilIsValidTgaF(File))
		return IL_TGA;
	#endif
	
	return IL_TYPE_UNKNOWN;
}


ILenum ILAPIENTRY ilDetermineTypeL(const void *Lump, ILuint Size)
{
	if (Lump == NULL)
		return IL_TYPE_UNKNOWN;

	#ifndef IL_NO_JPG
	if (ilIsValidJpegL(Lump, Size))
		return IL_JPG;
	#endif

	#ifndef IL_NO_DDS
	if (ilIsValidDdsL(Lump, Size))
		return IL_DDS;
	#endif

	#ifndef IL_NO_PNG
	if (ilIsValidPngL(Lump, Size))
		return IL_PNG;
	#endif

	#ifndef IL_NO_BMP
	if (ilIsValidBmpL(Lump, Size))
		return IL_BMP;
	#endif

	#ifndef IL_NO_EXR
	if (ilIsValidExrL(Lump, Size))
		return IL_EXR;
	#endif

	#ifndef IL_NO_GIF
	if (ilIsValidGifL(Lump, Size))
		return IL_GIF;
	#endif

	#ifndef IL_NO_HDR
	if (ilIsValidHdrL(Lump, Size))
		return IL_HDR;
	#endif

	#ifndef IL_NO_ICNS
	if (ilIsValidIcnsL(Lump, Size))
		return IL_ICNS;
	#endif

	#ifndef IL_NO_IWI
	if (ilIsValidIwiL(Lump, Size))
		return IL_IWI;
	#endif

	#ifndef IL_NO_ILBM
	if (ilIsValidIlbmL(Lump,Size))
		return IL_ILBM;
	#endif

	#ifndef IL_NO_JP2
	if (ilIsValidJp2L(Lump, Size))
		return IL_JP2;
	#endif

	#ifndef IL_NO_KTX
	if (ilIsValidKtxL(Lump, Size))
		return IL_KTX;
	#endif

	#ifndef IL_NO_LIF
	if (ilIsValidLifL(Lump, Size))
		return IL_LIF;
	#endif

	#ifndef IL_NO_MDL
	if (ilIsValidMdlL(Lump, Size))
		return IL_MDL;
	#endif

	#ifndef IL_NO_MP3
	if (ilIsValidMp3L(Lump, Size))
		return IL_MP3;
	#endif

	#ifndef IL_NO_PCX
	if (ilIsValidPcxL(Lump, Size))
		return IL_PCX;
	#endif

	#ifndef IL_NO_PIC
	if (ilIsValidPicL(Lump, Size))
		return IL_PIC;
	#endif

	#ifndef IL_NO_PNM
	if (ilIsValidPnmL(Lump, Size))
		return IL_PNM;
	#endif

	#ifndef IL_NO_PSD
	if (ilIsValidPsdL(Lump, Size))
		return IL_PSD;
	#endif

	#ifndef IL_NO_PSP
	if (ilIsValidPspL(Lump, Size))
		return IL_PSP;
	#endif

	#ifndef IL_NO_SGI
	if (ilIsValidSgiL(Lump, Size))
		return IL_SGI;
	#endif

	#ifndef IL_NO_SUN
	if (ilIsValidSunL(Lump, Size))
		return IL_SUN;
	#endif

	#ifndef IL_NO_TIF
	if (ilIsValidTiffL(Lump, Size))
		return IL_TIF;
	#endif

	#ifndef IL_NO_TPL
	if (ilIsValidTplL(Lump, Size))
		return IL_TPL;
	#endif

	#ifndef IL_NO_VTF
	if (ilIsValidVtfL(Lump, Size))
		return IL_VTF;
	#endif

	#ifndef IL_NO_XPM
	if (ilIsValidXpmL(Lump, Size))
		return IL_XPM;
	#endif

	//Moved Targa to end of list because it has no magic number
	// in header to assure that this is really a tga... (20040218).
	#ifndef IL_NO_TGA
	if (ilIsValidTgaL(Lump, Size))
		return IL_TGA;
	#endif

	return IL_TYPE_UNKNOWN;
}


ILboolean ILAPIENTRY ilIsValid(ILenum Type, ILconst_string FileName)
{
	if (FileName == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	switch (Type)
	{
		#ifndef IL_NO_TGA
		case IL_TGA:
			return ilIsValidTga(FileName);
		#endif

		#ifndef IL_NO_JPG
		case IL_JPG:
			return ilIsValidJpeg(FileName);
		#endif

		#ifndef IL_NO_DDS
		case IL_DDS:
			return ilIsValidDds(FileName);
		#endif

		#ifndef IL_NO_PNG
		case IL_PNG:
			return ilIsValidPng(FileName);
		#endif

		#ifndef IL_NO_BMP
		case IL_BMP:
			return ilIsValidBmp(FileName);
		#endif

		#ifndef IL_NO_DICOM
		case IL_DICOM:
			return ilIsValidDicom(FileName);
		#endif

		#ifndef IL_NO_EXR
		case IL_EXR:
			return ilIsValidExr(FileName);
		#endif

		#ifndef IL_NO_GIF
		case IL_GIF:
			return ilIsValidGif(FileName);
		#endif

		#ifndef IL_NO_HDR
		case IL_HDR:
			return ilIsValidHdr(FileName);
		#endif

		#ifndef IL_NO_ICNS
		case IL_ICNS:
			return ilIsValidIcns(FileName);
		#endif

		#ifndef IL_NO_IWI
		case IL_IWI:
			return ilIsValidIwi(FileName);
		#endif

    	#ifndef IL_NO_ILBM
        case IL_ILBM:
            return ilIsValidIlbm(FileName);
	    #endif

		#ifndef IL_NO_JP2
		case IL_JP2:
			return ilIsValidJp2(FileName);
		#endif

		#ifndef IL_NO_KTX
		case IL_KTX:
			return ilIsValidKtx(FileName);
		#endif

		#ifndef IL_NO_LIF
		case IL_LIF:
			return ilIsValidLif(FileName);
		#endif

		#ifndef IL_NO_MDL
		case IL_MDL:
			return ilIsValidMdl(FileName);
		#endif

		#ifndef IL_NO_MP3
		case IL_MP3:
			return ilIsValidMp3(FileName);
		#endif

		#ifndef IL_NO_PCX
		case IL_PCX:
			return ilIsValidPcx(FileName);
		#endif

		#ifndef IL_NO_PIC
		case IL_PIC:
			return ilIsValidPic(FileName);
		#endif

		#ifndef IL_NO_PNM
		case IL_PNM:
			return ilIsValidPnm(FileName);
		#endif

		#ifndef IL_NO_PSD
		case IL_PSD:
			return ilIsValidPsd(FileName);
		#endif

		#ifndef IL_NO_PSP
		case IL_PSP:
			return ilIsValidPsp(FileName);
		#endif

		#ifndef IL_NO_SGI
		case IL_SGI:
			return ilIsValidSgi(FileName);
		#endif

		#ifndef IL_NO_SUN
		case IL_SUN:
			return ilIsValidSun(FileName);
		#endif

		#ifndef IL_NO_TIF
		case IL_TIF:
			return ilIsValidTiff(FileName);
		#endif

		#ifndef IL_NO_TPL
		case IL_TPL:
			return ilIsValidTpl(FileName);
		#endif

		#ifndef IL_NO_VTF
		case IL_VTF:
			return ilIsValidVtf(FileName);
		#endif

		#ifndef IL_NO_XPM
		case IL_XPM:
			return ilIsValidXpm(FileName);
		#endif
	}

	ilSetError(IL_INVALID_ENUM);
	return IL_FALSE;
}


ILboolean ILAPIENTRY ilIsValidF(ILenum Type, ILHANDLE File)
{
	if (File == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	switch (Type)
	{
		#ifndef IL_NO_TGA
		case IL_TGA:
			return ilIsValidTgaF(File);
		#endif

		#ifndef IL_NO_JPG
		case IL_JPG:
			return ilIsValidJpegF(File);
		#endif

		#ifndef IL_NO_DDS
		case IL_DDS:
			return ilIsValidDdsF(File);
		#endif

		#ifndef IL_NO_PNG
		case IL_PNG:
			return ilIsValidPngF(File);
		#endif

		#ifndef IL_NO_BMP
		case IL_BMP:
			return ilIsValidBmpF(File);
		#endif

		#ifndef IL_NO_DICOM
		case IL_DICOM:
			return ilIsValidDicomF(File);
		#endif

		#ifndef IL_NO_EXR
		case IL_EXR:
			return ilIsValidExrF(File);
		#endif

		#ifndef IL_NO_GIF
		case IL_GIF:
			return ilIsValidGifF(File);
		#endif

		#ifndef IL_NO_HDR
		case IL_HDR:
			return ilIsValidHdrF(File);
		#endif

		#ifndef IL_NO_ICNS
		case IL_ICNS:
			return ilIsValidIcnsF(File);
		#endif

		#ifndef IL_NO_IWI
		case IL_IWI:
			return ilIsValidIwiF(File);
		#endif

    	#ifndef IL_NO_ILBM
        case IL_ILBM:
            return ilIsValidIlbmF(File);
	    #endif

		#ifndef IL_NO_JP2
		case IL_JP2:
			return ilIsValidJp2F(File);
		#endif

		#ifndef IL_NO_KTX
		case IL_KTX:
			return ilIsValidKtxF(File);
		#endif

		#ifndef IL_NO_LIF
		case IL_LIF:
			return ilIsValidLifF(File);
		#endif

		#ifndef IL_NO_MDL
		case IL_MDL:
			return ilIsValidMdlF(File);
		#endif

		#ifndef IL_NO_MP3
		case IL_MP3:
			return ilIsValidMp3F(File);
		#endif

		#ifndef IL_NO_PCX
		case IL_PCX:
			return ilIsValidPcxF(File);
		#endif

		#ifndef IL_NO_PIC
		case IL_PIC:
			return ilIsValidPicF(File);
		#endif

		#ifndef IL_NO_PNM
		case IL_PNM:
			return ilIsValidPnmF(File);
		#endif

		#ifndef IL_NO_PSD
		case IL_PSD:
			return ilIsValidPsdF(File);
		#endif

		#ifndef IL_NO_PSP
		case IL_PSP:
			return ilIsValidPspF(File);
		#endif

		#ifndef IL_NO_SGI
		case IL_SGI:
			return ilIsValidSgiF(File);
		#endif

		#ifndef IL_NO_SUN
		case IL_SUN:
			return ilIsValidSunF(File);
		#endif

		#ifndef IL_NO_TIF
		case IL_TIF:
			return ilIsValidTiffF(File);
		#endif

		#ifndef IL_NO_TPL
		case IL_TPL:
			return ilIsValidTplF(File);
		#endif

		#ifndef IL_NO_VTF
		case IL_VTF:
			return ilIsValidVtfF(File);
		#endif

		#ifndef IL_NO_XPM
		case IL_XPM:
			return ilIsValidXpmF(File);
		#endif
	}

	ilSetError(IL_INVALID_ENUM);
	return IL_FALSE;
}


ILboolean ILAPIENTRY ilIsValidL(ILenum Type, void *Lump, ILuint Size)
{
	if (Lump == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	switch (Type)
	{
		#ifndef IL_NO_TGA
		case IL_TGA:
			return ilIsValidTgaL(Lump, Size);
		#endif

		#ifndef IL_NO_JPG
		case IL_JPG:
			return ilIsValidJpegL(Lump, Size);
		#endif

		#ifndef IL_NO_DDS
		case IL_DDS:
			return ilIsValidDdsL(Lump, Size);
		#endif

		#ifndef IL_NO_PNG
		case IL_PNG:
			return ilIsValidPngL(Lump, Size);
		#endif

		#ifndef IL_NO_BMP
		case IL_BMP:
			return ilIsValidBmpL(Lump, Size);
		#endif

		#ifndef IL_NO_DICOM
		case IL_DICOM:
			return ilIsValidDicomL(Lump, Size);
		#endif

		#ifndef IL_NO_EXR
		case IL_EXR:
			return ilIsValidExrL(Lump, Size);
		#endif

		#ifndef IL_NO_GIF
		case IL_GIF:
			return ilIsValidGifL(Lump, Size);
		#endif

		#ifndef IL_NO_HDR
		case IL_HDR:
			return ilIsValidHdrL(Lump, Size);
		#endif

		#ifndef IL_NO_ICNS
		case IL_ICNS:
			return ilIsValidIcnsL(Lump, Size);
		#endif

		#ifndef IL_NO_IWI
		case IL_IWI:
			return ilIsValidIwiL(Lump, Size);
		#endif

    	#ifndef IL_NO_ILBM
        case IL_ILBM:
            return ilIsValidIlbmL(Lump, Size);
	    #endif

		#ifndef IL_NO_JP2
		case IL_JP2:
			return ilIsValidJp2L(Lump, Size);
		#endif

		#ifndef IL_NO_KTX
		case IL_KTX:
			return ilIsValidKtxL(Lump, Size);
		#endif

		#ifndef IL_NO_LIF
		case IL_LIF:
			return ilIsValidLifL(Lump, Size);
		#endif

		#ifndef IL_NO_MDL
		case IL_MDL:
			return ilIsValidMdlL(Lump, Size);
		#endif

		#ifndef IL_NO_MP3
		case IL_MP3:
			return ilIsValidMp3L(Lump, Size);
		#endif

		#ifndef IL_NO_PCX
		case IL_PCX:
			return ilIsValidPcxL(Lump, Size);
		#endif

		#ifndef IL_NO_PIC
		case IL_PIC:
			return ilIsValidPicL(Lump, Size);
		#endif

		#ifndef IL_NO_PNM
		case IL_PNM:
			return ilIsValidPnmL(Lump, Size);
		#endif

		#ifndef IL_NO_PSD
		case IL_PSD:
			return ilIsValidPsdL(Lump, Size);
		#endif

		#ifndef IL_NO_PSP
		case IL_PSP:
			return ilIsValidPspL(Lump, Size);
		#endif

		#ifndef IL_NO_SGI
		case IL_SGI:
			return ilIsValidSgiL(Lump, Size);
		#endif

		#ifndef IL_NO_SUN
		case IL_SUN:
			return ilIsValidSunL(Lump, Size);
		#endif

		#ifndef IL_NO_TIF
		case IL_TIF:
			return ilIsValidTiffL(Lump, Size);
		#endif

		#ifndef IL_NO_TPL
		case IL_TPL:
			return ilIsValidTplL(Lump, Size);
		#endif

		#ifndef IL_NO_VTF
		case IL_VTF:
			return ilIsValidVtfL(Lump, Size);
		#endif

		#ifndef IL_NO_XPM
		case IL_XPM:
			return ilIsValidXpmL(Lump, Size);
		#endif
	}

	ilSetError(IL_INVALID_ENUM);
	return IL_FALSE;
}


//! Attempts to load an image from a file.  The file format is specified by the user.
/*! \param Type Format of this file.  Acceptable values are IL_BLP, IL_BMP, IL_CUT, IL_DCX, IL_DDS,
	IL_DICOM, IL_DOOM, IL_DOOM_FLAT, IL_DPX, IL_EXR, IL_FITS, IL_FTX, IL_GIF, IL_HDR, IL_ICO, IL_ICNS,
	IL_IFF, IL_IWI, IL_JP2, IL_JPG, IL_LIF, IL_MDL,	IL_MNG, IL_MP3, IL_PCD, IL_PCX, IL_PIX, IL_PNG,
	IL_PNM, IL_PSD, IL_PSP, IL_PXR, IL_ROT, IL_SGI, IL_SUN, IL_TEXTURE, IL_TGA, IL_TIF, IL_TPL,
	IL_UTX, IL_VTF, IL_WAL, IL_WBMP, IL_XPM, IL_RAW, IL_JASC_PAL and IL_TYPE_UNKNOWN.
	If IL_TYPE_UNKNOWN is specified, ilLoad will try to determine the type of the file and load it.
	\param FileName Ansi or Unicode string, depending on the compiled version of DevIL, that gives
	       the filename of the file to load.
	\return Boolean value of failure or success.  Returns IL_FALSE if all three loading methods
	       have been tried and failed.*/
ILboolean ILAPIENTRY ilLoad(ILenum Type, ILconst_string FileName)
{
	ILboolean	bRet;

	if (FileName == NULL || ilStrLen(FileName) < 1) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	switch (Type)
	{
		case IL_TYPE_UNKNOWN:
			bRet = ilLoadImage(FileName);
			break;

		#ifndef IL_NO_TGA
		case IL_TGA:
			bRet = ilLoadTarga(FileName);
			break;
		#endif

		#ifndef IL_NO_JPG
		case IL_JPG:
			bRet = ilLoadJpeg(FileName);
			break;
		#endif

		#ifndef IL_NO_JP2
		case IL_JP2:
			bRet = ilLoadJp2(FileName);
			break;
		#endif

		#ifndef IL_NO_DDS
		case IL_DDS:
			bRet = ilLoadDds(FileName);
			break;
		#endif

		#ifndef IL_NO_PNG
		case IL_PNG:
			bRet = ilLoadPng(FileName);
			break;
		#endif

		#ifndef IL_NO_BLP
		case IL_BLP:
			bRet = ilLoadBlp(FileName);
			break;
		#endif

		#ifndef IL_NO_BMP
		case IL_BMP:
			bRet = ilLoadBmp(FileName);
			break;
		#endif

		#ifndef IL_NO_DPX
		case IL_DPX:
			bRet = ilLoadDpx(FileName);
			break;
		#endif

		#ifndef IL_NO_GIF
		case IL_GIF:
			bRet = ilLoadGif(FileName);
			break;
		#endif

		#ifndef IL_NO_HDR
		case IL_HDR:
			bRet = ilLoadHdr(FileName);
			break;
		#endif

		#ifndef IL_NO_CUT
		case IL_CUT:
			bRet = ilLoadCut(FileName);
			break;
		#endif

		#ifndef IL_NO_DICOM
		case IL_DICOM:
			bRet = ilLoadDicom(FileName);
			break;
		#endif

		#ifndef IL_NO_DOOM
		case IL_DOOM:
			bRet = ilLoadDoom(FileName);
			break;
		case IL_DOOM_FLAT:
			bRet = ilLoadDoomFlat(FileName);
			break;
		#endif

		#ifndef IL_NO_EXR
		case IL_EXR:
			bRet = ilLoadExr(FileName);
			break;
		#endif

		#ifndef IL_NO_FITS
		case IL_FITS:
			bRet = ilLoadFits(FileName);
			break;
		#endif

		#ifndef IL_NO_FTX
		case IL_FTX:
			bRet = ilLoadFtx(FileName);
			break;
		#endif

		#ifndef IL_NO_ICO
		case IL_ICO:
			bRet = ilLoadIcon(FileName);
			break;
		#endif

		#ifndef IL_NO_ICNS
		case IL_ICNS:
			bRet = ilLoadIcns(FileName);
			break;
		#endif

		#ifndef IL_NO_IFF
		case IL_IFF:
			bRet = ilLoadIff(FileName);
			break;
		#endif

		#ifndef IL_NO_ILBM
		case IL_ILBM:
			bRet = ilLoadIlbm(FileName);
			break;
		#endif

		#ifndef IL_NO_IWI
		case IL_IWI:
			bRet = ilLoadIwi(FileName);
			break;
		#endif

		#ifndef IL_NO_KTX
		case IL_KTX:
			bRet = ilLoadKtx(FileName);
			break;
		#endif

		#ifndef IL_NO_LIF
		case IL_LIF:
			bRet = ilLoadLif(FileName);
			break;
		#endif

		#ifndef IL_NO_MDL
		case IL_MDL:
			bRet = ilLoadMdl(FileName);
			break;
		#endif

		#ifndef IL_NO_MNG
		case IL_MNG:
			bRet = ilLoadMng(FileName);
			break;
		#endif

		#ifndef IL_NO_MP3
		case IL_MP3:
			bRet = ilLoadMp3(FileName);
			break;
		#endif

		#ifndef IL_NO_PCD
		case IL_PCD:
			ilLoadPcd(FileName);
			break;
		#endif

		#ifndef IL_NO_PCX
		case IL_PCX:
			bRet = ilLoadPcx(FileName);
			break;
		#endif

		#ifndef IL_NO_PIC
		case IL_PIC:
			bRet = ilLoadPic(FileName);
			break;
		#endif

		#ifndef IL_NO_PIX
		case IL_PIX:
			bRet = ilLoadPix(FileName);
			break;
		#endif

		#ifndef IL_NO_PNM
		case IL_PNM:
			bRet = ilLoadPnm(FileName);
			break;
		#endif

		#ifndef IL_NO_PSD
		case IL_PSD:
			bRet = ilLoadPsd(FileName);
			break;
		#endif

		#ifndef IL_NO_PSP
		case IL_PSP:
			bRet = ilLoadPsp(FileName);
			break;
		#endif

		#ifndef IL_NO_PXR
		case IL_PXR:
			bRet = ilLoadPxr(FileName);
			break;
		#endif

		#ifndef IL_NO_RAW
		case IL_RAW:
			bRet = ilLoadRaw(FileName);
			break;
		#endif

		#ifndef IL_NO_ROT
		case IL_ROT:
			bRet = ilLoadRot(FileName);
			break;
		#endif

		#ifndef IL_NO_SGI
		case IL_SGI:
			bRet = ilLoadSgi(FileName);
			break;
		#endif

		#ifndef IL_NO_SUN
		case IL_SUN:
			bRet = ilLoadSun(FileName);
			break;
		#endif

		#ifndef IL_NO_TEXTURE
		case IL_TEXTURE:
			bRet = ilLoadTexture(FileName);
			break;
		#endif

		#ifndef IL_NO_TIF
		case IL_TIF:
			bRet = ilLoadTiff(FileName);
			break;
		#endif

		#ifndef IL_NO_TPL
		case IL_TPL:
			bRet = ilLoadTpl(FileName);
			break;
		#endif

		#ifndef IL_NO_UTX
		case IL_UTX:
			bRet = ilLoadUtx(FileName);
			break;
		#endif

		#ifndef IL_NO_VTF
		case IL_VTF:
			bRet = ilLoadVtf(FileName);
			break;
		#endif

		#ifndef IL_NO_WAL
		case IL_WAL:
			bRet = ilLoadWal(FileName);
			break;
		#endif

		#ifndef IL_NO_WBMP
		case IL_WBMP:
			bRet = ilLoadWbmp(FileName);
			break;
		#endif

		#ifndef IL_NO_XPM
		case IL_XPM:
			bRet = ilLoadXpm(FileName);
			break;
		#endif

		#ifndef IL_NO_WDP
		case IL_WDP:
			bRet = ilLoadWdp(FileName);
			break;
		#endif

		default:
			ilSetError(IL_INVALID_ENUM);
			bRet = IL_FALSE;
	}

	return bRet;
}


//! Attempts to load an image from a file stream.  The file format is specified by the user.
/*! \param Type Format of this file.  Acceptable values are IL_BLP, IL_BMP, IL_CUT, IL_DCX, IL_DDS,
	IL_DICOM, IL_DOOM, IL_DOOM_FLAT, IL_DPX, IL_EXR, IL_FITS, IL_FTX, IL_GIF, IL_HDR, IL_ICO, IL_ICNS,
	IL_IFF, IL_IWI, IL_JP2, IL_JPG, IL_LIF, IL_MDL,	IL_MNG, IL_MP3, IL_PCD, IL_PCX, IL_PIX, IL_PNG,
	IL_PNM, IL_PSD, IL_PSP, IL_PXR, IL_ROT, IL_SGI, IL_SUN, IL_TEXTURE, IL_TGA, IL_TIF, IL_TPL,
	IL_UTX, IL_VTF, IL_WAL, IL_WBMP, IL_XPM, IL_RAW, IL_JASC_PAL and IL_TYPE_UNKNOWN.
	If IL_TYPE_UNKNOWN is specified, ilLoadF will try to determine the type of the file and load it.
	\param File File stream to load from.
	\return Boolean value of failure or success.  Returns IL_FALSE if loading fails.*/
ILboolean ILAPIENTRY ilLoadF(ILenum Type, ILHANDLE File)
{
	if (File == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	if (Type == IL_TYPE_UNKNOWN)
		Type = ilDetermineTypeF(File);
	
	switch (Type)
	{
		case IL_TYPE_UNKNOWN:
			return IL_FALSE;

		#ifndef IL_NO_TGA
		case IL_TGA:
			return ilLoadTargaF(File);
		#endif

		#ifndef IL_NO_JPG
			#ifndef IL_USE_IJL
			case IL_JPG:
				return ilLoadJpegF(File);
			#endif
		#endif

		#ifndef IL_NO_JP2
		case IL_JP2:
			return ilLoadJp2F(File);
		#endif

		#ifndef IL_NO_DDS
		case IL_DDS:
			return ilLoadDdsF(File);
		#endif

		#ifndef IL_NO_PNG
		case IL_PNG:
			return ilLoadPngF(File);
		#endif

		#ifndef IL_NO_BLP
		case IL_BLP:
			return ilLoadBlpF(File);
		#endif

		#ifndef IL_NO_BMP
		case IL_BMP:
			return ilLoadBmpF(File);
		#endif

		#ifndef IL_NO_CUT
		case IL_CUT:
			return ilLoadCutF(File);
		#endif

		#ifndef IL_NO_DICOM
		case IL_DICOM:
			return ilLoadDicomF(File);
		#endif

		#ifndef IL_NO_DOOM
		case IL_DOOM:
			return ilLoadDoomF(File);
		case IL_DOOM_FLAT:
			return ilLoadDoomFlatF(File);
		#endif

		#ifndef IL_NO_DPX
		case IL_DPX:
			return ilLoadDpxF(File);
		#endif

		#ifndef IL_NO_EXR
		case IL_EXR:
			return ilLoadExrF(File);
		#endif

		#ifndef IL_NO_FITS
		case IL_FITS:
			return ilLoadFitsF(File);
		#endif

		#ifndef IL_NO_FTX
		case IL_FTX:
			return ilLoadFtxF(File);
		#endif

		#ifndef IL_NO_GIF
		case IL_GIF:
			return ilLoadGifF(File);
		#endif

		#ifndef IL_NO_HDR
		case IL_HDR:
			return ilLoadHdrF(File);
		#endif

		#ifndef IL_NO_ICO
		case IL_ICO:
			return ilLoadIconF(File);
		#endif

		#ifndef IL_NO_ICNS
		case IL_ICNS:
			return ilLoadIcnsF(File);
		#endif

		#ifndef IL_NO_IFF
		case IL_IFF:
			return ilLoadIffF(File);
		#endif

		#ifndef IL_NO_ILBM
		case IL_ILBM:
			return ilLoadIlbmF(File);
		#endif

		#ifndef IL_NO_IWI
		case IL_IWI:
			return ilLoadIwiF(File);
		#endif

		#ifndef IL_NO_KTX
		case IL_KTX:
			return ilLoadKtxF(File);
		#endif

		#ifndef IL_NO_LIF
		case IL_LIF:
			return ilLoadLifF(File);
		#endif

		#ifndef IL_NO_MDL
		case IL_MDL:
			return ilLoadMdlF(File);
		#endif

		#ifndef IL_NO_MNG
		case IL_MNG:
			return ilLoadMngF(File);
		#endif

		#ifndef IL_NO_MP3
		case IL_MP3:
			return ilLoadMp3F(File);
		#endif

		#ifndef IL_NO_PCD
		case IL_PCD:
			return ilLoadPcdF(File);
		#endif

		#ifndef IL_NO_PCX
		case IL_PCX:
			return ilLoadPcxF(File);
		#endif

		#ifndef IL_NO_PIC
		case IL_PIC:
			return ilLoadPicF(File);
		#endif

		#ifndef IL_NO_PIX
		case IL_PIX:
			return ilLoadPixF(File);
		#endif

		#ifndef IL_NO_PNM
		case IL_PNM:
			return ilLoadPnmF(File);
		#endif

		#ifndef IL_NO_PSD
		case IL_PSD:
			return ilLoadPsdF(File);
		#endif

		#ifndef IL_NO_PSP
		case IL_PSP:
			return ilLoadPspF(File);
		#endif

		#ifndef IL_NO_PXR
		case IL_PXR:
			return ilLoadPxrF(File);
		#endif

		#ifndef IL_NO_RAW
		case IL_RAW:
			return ilLoadRawF(File);
		#endif

		#ifndef IL_NO_ROT
		case IL_ROT:
			return ilLoadRotF(File);
		#endif

		#ifndef IL_NO_SGI
		case IL_SGI:
			return ilLoadSgiF(File);
		#endif

		#ifndef IL_NO_SUN
		case IL_SUN:
			return ilLoadSunF(File);
		#endif

		#ifndef IL_NO_TEXTURE
		case IL_TEXTURE:
			return ilLoadTextureF(File);
		#endif

		#ifndef IL_NO_TIF
		case IL_TIF:
			return ilLoadTiffF(File);
		#endif

		#ifndef IL_NO_TPL
		case IL_TPL:
			return ilLoadTplF(File);
		#endif

		#ifndef IL_NO_UTX
		case IL_UTX:
			return ilLoadUtxF(File);
		#endif

		#ifndef IL_NO_VTF
		case IL_VTF:
			return ilLoadVtfF(File);
		#endif

		#ifndef IL_NO_WAL
		case IL_WAL:
			return ilLoadWalF(File);
		#endif

		#ifndef IL_NO_WBMP
		case IL_WBMP:
			return ilLoadWbmpF(File);
		#endif

		#ifndef IL_NO_XPM
		case IL_XPM:
			return ilLoadXpmF(File);
		#endif
	}

	ilSetError(IL_INVALID_ENUM);
	return IL_FALSE;
}


//! Attempts to load an image from a memory buffer.  The file format is specified by the user.
/*! \param Type Format of this file.  Acceptable values are IL_BLP, IL_BMP, IL_CUT, IL_DCX, IL_DDS,
	IL_DICOM, IL_DOOM, IL_DOOM_FLAT, IL_DPX, IL_EXR, IL_FITS, IL_FTX, IL_GIF, IL_HDR, IL_ICO, IL_ICNS,
	IL_IFF, IL_IWI, IL_JP2, IL_JPG, IL_LIF, IL_MDL,	IL_MNG, IL_MP3, IL_PCD, IL_PCX, IL_PIX, IL_PNG,
	IL_PNM, IL_PSD, IL_PSP, IL_PXR, IL_ROT, IL_SGI, IL_SUN, IL_TEXTURE, IL_TGA, IL_TIF, IL_TPL,
	IL_UTX, IL_VTF, IL_WAL, IL_WBMP, IL_XPM, IL_RAW, IL_JASC_PAL and IL_TYPE_UNKNOWN.
	If IL_TYPE_UNKNOWN is specified, ilLoadL will try to determine the type of the file and load it.
	\param Lump The buffer where the file data is located
	\param Size Size of the buffer
	\return Boolean value of failure or success.  Returns IL_FALSE if loading fails.*/
ILboolean ILAPIENTRY ilLoadL(ILenum Type, const void *Lump, ILuint Size)
{
	if (Lump == NULL || Size == 0) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	if (Type == IL_TYPE_UNKNOWN)
		Type = ilDetermineTypeL(Lump, Size);
	
	switch (Type)
	{
		case IL_TYPE_UNKNOWN:
			return IL_FALSE;

		#ifndef IL_NO_TGA
		case IL_TGA:
			return ilLoadTargaL(Lump, Size);
		#endif

		#ifndef IL_NO_JPG
		case IL_JPG:
			return ilLoadJpegL(Lump, Size);
		#endif

		#ifndef IL_NO_JP2
		case IL_JP2:
			return ilLoadJp2L(Lump, Size);
		#endif

		#ifndef IL_NO_DDS
		case IL_DDS:
			return ilLoadDdsL(Lump, Size);
		#endif

		#ifndef IL_NO_PNG
		case IL_PNG:
			return ilLoadPngL(Lump, Size);
		#endif

		#ifndef IL_NO_BLP
		case IL_BLP:
			return ilLoadBlpL(Lump, Size);
		#endif

		#ifndef IL_NO_BMP
		case IL_BMP:
			return ilLoadBmpL(Lump, Size);
		#endif

		#ifndef IL_NO_CUT
		case IL_CUT:
			return ilLoadCutL(Lump, Size);
		#endif

		#ifndef IL_NO_DICOM
		case IL_DICOM:
			return ilLoadDicomL(Lump, Size);
		#endif

		#ifndef IL_NO_DOOM
		case IL_DOOM:
			return ilLoadDoomL(Lump, Size);
		case IL_DOOM_FLAT:
			return ilLoadDoomFlatL(Lump, Size);
		#endif

		#ifndef IL_NO_DPX
		case IL_DPX:
			return ilLoadDpxL(Lump, Size);
		#endif

		#ifndef IL_NO_EXR
		case IL_EXR:
			return ilLoadExrL(Lump, Size);
		#endif

		#ifndef IL_NO_FITS
		case IL_FITS:
			return ilLoadFitsL(Lump, Size);
		#endif

		#ifndef IL_NO_FTX
		case IL_FTX:
			return ilLoadFtxL(Lump, Size);
		#endif

		#ifndef IL_NO_GIF
		case IL_GIF:
			return ilLoadGifL(Lump, Size);
		#endif

		#ifndef IL_NO_HDR
		case IL_HDR:
			return ilLoadHdrL(Lump, Size);
		#endif

		#ifndef IL_NO_ICO
		case IL_ICO:
			return ilLoadIconL(Lump, Size);
		#endif

		#ifndef IL_NO_ICNS
		case IL_ICNS:
			return ilLoadIcnsL(Lump, Size);
		#endif

		#ifndef IL_NO_IFF
		case IL_IFF:
			return ilLoadIffL(Lump, Size);
		#endif

		#ifndef IL_NO_ILBM
		case IL_ILBM:
			return ilLoadIlbmL(Lump, Size);
		#endif

		#ifndef IL_NO_IWI
		case IL_IWI:
			return ilLoadIwiL(Lump, Size);
		#endif

		#ifndef IL_NO_KTX
		case IL_KTX:
			return ilLoadKtxL(Lump, Size);
		#endif

		#ifndef IL_NO_LIF
		case IL_LIF:
			return ilLoadLifL(Lump, Size);
		#endif

		#ifndef IL_NO_MDL
		case IL_MDL:
			return ilLoadMdlL(Lump, Size);
		#endif

		#ifndef IL_NO_MNG
		case IL_MNG:
			return ilLoadMngL(Lump, Size);
		#endif

		#ifndef IL_NO_MP3
		case IL_MP3:
			return ilLoadMp3L(Lump, Size);
		#endif

		#ifndef IL_NO_PCD
		case IL_PCD:
			return ilLoadPcdL(Lump, Size);
		#endif

		#ifndef IL_NO_PCX
		case IL_PCX:
			return ilLoadPcxL(Lump, Size);
		#endif

		#ifndef IL_NO_PIC
		case IL_PIC:
			return ilLoadPicL(Lump, Size);
		#endif

		#ifndef IL_NO_PIX
		case IL_PIX:
			return ilLoadPixL(Lump, Size);
		#endif

		#ifndef IL_NO_PNM
		case IL_PNM:
			return ilLoadPnmL(Lump, Size);
		#endif

		#ifndef IL_NO_PSD
		case IL_PSD:
			return ilLoadPsdL(Lump, Size);
		#endif

		#ifndef IL_NO_PSP
		case IL_PSP:
			return ilLoadPspL(Lump, Size);
		#endif

		#ifndef IL_NO_PXR
		case IL_PXR:
			return ilLoadPxrL(Lump, Size);
		#endif

		#ifndef IL_NO_RAW
		case IL_RAW:
			return ilLoadRawL(Lump, Size);
		#endif

		#ifndef IL_NO_ROT
		case IL_ROT:
			return ilLoadRotL(Lump, Size);
		#endif

		#ifndef IL_NO_SGI
		case IL_SGI:
			return ilLoadSgiL(Lump, Size);
		#endif

		#ifndef IL_NO_SUN
		case IL_SUN:
			return ilLoadSunL(Lump, Size);
		#endif

		#ifndef IL_NO_TEXTURE
		case IL_TEXTURE:
			return ilLoadTextureL(Lump, Size);
		#endif

		#ifndef IL_NO_TIF
		case IL_TIF:
			return ilLoadTiffL(Lump, Size);
		#endif

		#ifndef IL_NO_TPL
		case IL_TPL:
			return ilLoadTplL(Lump, Size);
		#endif

		#ifndef IL_NO_UTX
		case IL_UTX:
			return ilLoadUtxL(Lump, Size);
		#endif

		#ifndef IL_NO_VTF
		case IL_VTF:
			return ilLoadVtfL(Lump, Size);
		#endif

		#ifndef IL_NO_WAL
		case IL_WAL:
			return ilLoadWalL(Lump, Size);
		#endif

		#ifndef IL_NO_WBMP
		case IL_WBMP:
			return ilLoadWbmpL(Lump, Size);
		#endif

		#ifndef IL_NO_XPM
		case IL_XPM:
			return ilLoadXpmL(Lump, Size);
		#endif
	}

	ilSetError(IL_INVALID_ENUM);
	return IL_FALSE;
}


//! Attempts to load an image from a file with various different methods before failing - very generic.
/*! The ilLoadImage function allows a general interface to the specific internal file-loading
	routines.  First, it finds the extension and checks to see if any user-registered functions
	(registered through ilRegisterLoad) match the extension. If nothing matches, it takes the
	extension and determines which function to call based on it. Lastly, it attempts to identify
	the image based on various image header verification functions, such as ilIsValidPngF.
	If all this checking fails, IL_FALSE is returned with no modification to the current bound image.
	\param FileName Ansi or Unicode string, depending on the compiled version of DevIL, that gives
	       the filename of the file to load.
	\return Boolean value of failure or success.  Returns IL_FALSE if all three loading methods
	       have been tried and failed.*/
ILboolean ILAPIENTRY ilLoadImage(ILconst_string FileName)
{
	ILstring	Ext;
	ILenum		Type;
	ILboolean	bRet = IL_FALSE;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	if (FileName == NULL || ilStrLen(FileName) < 1) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	Ext = iGetExtension(FileName);

	// Try registered procedures first (so users can override default lib functions).
	if (Ext) {
		if (iRegisterLoad(FileName))
			return IL_TRUE;

		#ifndef IL_NO_TGA
		if (!iStrCmp(Ext, IL_TEXT("tga")) || !iStrCmp(Ext, IL_TEXT("vda")) ||
			!iStrCmp(Ext, IL_TEXT("icb")) || !iStrCmp(Ext, IL_TEXT("vst"))) {
			bRet = ilLoadTarga(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_JPG
		if (!iStrCmp(Ext, IL_TEXT("jpg")) || !iStrCmp(Ext, IL_TEXT("jpe")) ||
			!iStrCmp(Ext, IL_TEXT("jpeg")) || !iStrCmp(Ext, IL_TEXT("jif")) || !iStrCmp(Ext, IL_TEXT("jfif"))) {
			bRet = ilLoadJpeg(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_JP2
		if (!iStrCmp(Ext, IL_TEXT("jp2")) || !iStrCmp(Ext, IL_TEXT("jpx")) ||
			!iStrCmp(Ext, IL_TEXT("j2k")) || !iStrCmp(Ext, IL_TEXT("j2c"))) {
			bRet = ilLoadJp2(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_DDS
		if (!iStrCmp(Ext, IL_TEXT("dds"))) {
			bRet = ilLoadDds(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_PNG
		if (!iStrCmp(Ext, IL_TEXT("png"))) {
			bRet = ilLoadPng(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_BMP
		if (!iStrCmp(Ext, IL_TEXT("bmp")) || !iStrCmp(Ext, IL_TEXT("dib"))) {
			bRet = ilLoadBmp(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_BLP
		if (!iStrCmp(Ext, IL_TEXT("blp"))) {
			bRet = ilLoadBlp(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_DPX
		if (!iStrCmp(Ext, IL_TEXT("dpx"))) {
			bRet = ilLoadDpx(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_EXR
		if (!iStrCmp(Ext, IL_TEXT("exr"))) {
			bRet = ilLoadExr(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_GIF
		if (!iStrCmp(Ext, IL_TEXT("gif"))) {
			bRet = ilLoadGif(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_HDR
		if (!iStrCmp(Ext, IL_TEXT("hdr"))) {
			bRet = ilLoadHdr(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_CUT
		if (!iStrCmp(Ext, IL_TEXT("cut"))) {
			bRet = ilLoadCut(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_DCX
		if (!iStrCmp(Ext, IL_TEXT("dcx"))) {
			bRet = ilLoadDcx(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_DICOM
		if (!iStrCmp(Ext, IL_TEXT("dicom")) || !iStrCmp(Ext, IL_TEXT("dcm"))) {
			bRet = ilLoadDicom(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_FITS
		if (!iStrCmp(Ext, IL_TEXT("fits")) || !iStrCmp(Ext, IL_TEXT("fit"))) {
			bRet = ilLoadFits(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_FTX
		if (!iStrCmp(Ext, IL_TEXT("ftx"))) {
			bRet = ilLoadFtx(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_ICO
		if (!iStrCmp(Ext, IL_TEXT("ico")) || !iStrCmp(Ext, IL_TEXT("cur"))) {
			bRet = ilLoadIcon(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_ICNS
		if (!iStrCmp(Ext, IL_TEXT("icns"))) {
			bRet = ilLoadIcns(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_IFF
		if (!iStrCmp(Ext, IL_TEXT("iff"))) {
			bRet = ilLoadIff(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_ILBM
		if (!iStrCmp(Ext, IL_TEXT("ilbm")) || !iStrCmp(Ext, IL_TEXT("lbm")) ||
            !iStrCmp(Ext, IL_TEXT("ham")) ) {
			bRet = ilLoadIlbm(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_IWI
		if (!iStrCmp(Ext, IL_TEXT("iwi"))) {
			bRet = ilLoadIwi(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_KTX
		if (!iStrCmp(Ext, IL_TEXT("ktx"))) {
			bRet = ilLoadKtx(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_LIF
		if (!iStrCmp(Ext, IL_TEXT("lif"))) {
			bRet = ilLoadLif(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_MDL
		if (!iStrCmp(Ext, IL_TEXT("mdl"))) {
			bRet = ilLoadMdl(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_MNG
		if (!iStrCmp(Ext, IL_TEXT("mng")) || !iStrCmp(Ext, IL_TEXT("jng"))) {
			bRet = ilLoadMng(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_MP3
		if (!iStrCmp(Ext, IL_TEXT("mp3"))) {
			bRet = ilLoadMp3(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_PCD
		if (!iStrCmp(Ext, IL_TEXT("pcd"))) {
			bRet = ilLoadPcd(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_PCX
		if (!iStrCmp(Ext, IL_TEXT("pcx"))) {
			bRet = ilLoadPcx(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_PIC
		if (!iStrCmp(Ext, IL_TEXT("pic"))) {
			bRet = ilLoadPic(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_PIX
		if (!iStrCmp(Ext, IL_TEXT("pix"))) {
			bRet = ilLoadPix(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_PNM
		if (!iStrCmp(Ext, IL_TEXT("pbm"))) {
			bRet = ilLoadPnm(FileName);
			goto finish;
		}
		if (!iStrCmp(Ext, IL_TEXT("pgm"))) {
			bRet = ilLoadPnm(FileName);
			goto finish;
		}
		if (!iStrCmp(Ext, IL_TEXT("pnm"))) {
			bRet = ilLoadPnm(FileName);
			goto finish;
		}
		if (!iStrCmp(Ext, IL_TEXT("ppm"))) {
			bRet = ilLoadPnm(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_PSD
		if (!iStrCmp(Ext, IL_TEXT("psd")) || !iStrCmp(Ext, IL_TEXT("pdd"))) {
			bRet = ilLoadPsd(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_PSP
		if (!iStrCmp(Ext, IL_TEXT("psp"))) {
			bRet = ilLoadPsp(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_PXR
		if (!iStrCmp(Ext, IL_TEXT("pxr"))) {
			bRet = ilLoadPxr(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_ROT
		if (!iStrCmp(Ext, IL_TEXT("rot"))) {
			bRet = ilLoadRot(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_SGI
		if (!iStrCmp(Ext, IL_TEXT("sgi")) || !iStrCmp(Ext, IL_TEXT("bw")) ||
			!iStrCmp(Ext, IL_TEXT("rgb")) || !iStrCmp(Ext, IL_TEXT("rgba"))) {
			bRet = ilLoadSgi(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_SUN
		if (!iStrCmp(Ext, IL_TEXT("sun")) || !iStrCmp(Ext, IL_TEXT("ras")) ||
			!iStrCmp(Ext, IL_TEXT("rs")) || !iStrCmp(Ext, IL_TEXT("im1")) ||
			!iStrCmp(Ext, IL_TEXT("im8")) || !iStrCmp(Ext, IL_TEXT("im24")) ||
			!iStrCmp(Ext, IL_TEXT("im32"))) {
			bRet = ilLoadSun(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_TEXTURE
		if (!iStrCmp(Ext, IL_TEXT("texture"))) {
			bRet = ilLoadTexture(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_TIF
		if (!iStrCmp(Ext, IL_TEXT("tif")) || !iStrCmp(Ext, IL_TEXT("tiff"))) {
			bRet = ilLoadTiff(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_TPL
		if (!iStrCmp(Ext, IL_TEXT("tpl"))) {
			bRet = ilLoadTpl(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_UTX
		if (!iStrCmp(Ext, IL_TEXT("utx"))) {
			bRet = ilLoadUtx(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_VTF
		if (!iStrCmp(Ext, IL_TEXT("vtf"))) {
			bRet = ilLoadVtf(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_WAL
		if (!iStrCmp(Ext, IL_TEXT("wal"))) {
			bRet = ilLoadWal(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_WBMP
		if (!iStrCmp(Ext, IL_TEXT("wbmp"))) {
			bRet = ilLoadWbmp(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_WDP
		if (!iStrCmp(Ext, IL_TEXT("wdp")) || !iStrCmp(Ext, IL_TEXT("hdp")) ) {
			bRet = ilLoadWdp(FileName);
			goto finish;
		}
		#endif

		#ifndef IL_NO_XPM
		if (!iStrCmp(Ext, IL_TEXT("xpm"))) {
			bRet = ilLoadXpm(FileName);
			goto finish;
		}
		#endif
	}

	// As a last-ditch effort, try to identify the image
	Type = ilDetermineType(FileName);
	if (Type == IL_TYPE_UNKNOWN) {
		ilSetError(IL_INVALID_EXTENSION);
		return IL_FALSE;
	}
	return ilLoad(Type, FileName);

finish:
	return bRet;
}


//! Attempts to save an image to a file.  The file format is specified by the user.
/*! \param Type Format of this file.  Acceptable values are IL_BMP, IL_CHEAD, IL_DDS, IL_EXR,
	IL_HDR, IL_JP2, IL_JPG, IL_PCX, IL_PNG, IL_PNM, IL_PSD, IL_RAW, IL_SGI, IL_TGA, IL_TIF,
	IL_VTF, IL_WBMP and IL_JASC_PAL.
	\param FileName Ansi or Unicode string, depending on the compiled version of DevIL, that gives
	       the filename to save to.
	\return Boolean value of failure or success.  Returns IL_FALSE if saving failed.*/
ILboolean ILAPIENTRY ilSave(ILenum Type, ILconst_string FileName)
{
	switch (Type)
	{
		case IL_TYPE_UNKNOWN:
			return ilSaveImage(FileName);

		#ifndef IL_NO_BMP
		case IL_BMP:
			return ilSaveBmp(FileName);
		#endif

		#ifndef IL_NO_CHEAD
		case IL_CHEAD:
			return ilSaveCHeader(FileName, (char*)"IL_IMAGE");
		#endif

		#ifndef IL_NO_DDS
		case IL_DDS:
    		return ilSaveDds(FileName);
		#endif

		#ifndef IL_NO_EXR
		case IL_EXR:
    		return ilSaveExr(FileName);
		#endif

		#ifndef IL_NO_HDR
		case IL_HDR:
			return ilSaveHdr(FileName);
		#endif

		#ifndef IL_NO_JP2
		case IL_JP2:
			return ilSaveJp2(FileName);
		#endif

		#ifndef IL_NO_JPG
		case IL_JPG:
			return ilSaveJpeg(FileName);
		#endif

		/*#ifndef IL_NO_KTX
		case IL_KTX:
			return ilSaveKtx(FileName);
		#endif*/

		#ifndef IL_NO_PCX
		case IL_PCX:
			return ilSavePcx(FileName);
		#endif

		#ifndef IL_NO_PNG
		case IL_PNG:
			return ilSavePng(FileName);
		#endif

		#ifndef IL_NO_PNM
		case IL_PNM:
			return ilSavePnm(FileName);
		#endif

		#ifndef IL_NO_PSD
		case IL_PSD:
			return ilSavePsd(FileName);
		#endif

		#ifndef IL_NO_RAW
		case IL_RAW:
			return ilSaveRaw(FileName);
		#endif

		#ifndef IL_NO_SGI
		case IL_SGI:
			return ilSaveSgi(FileName);
		#endif

		#ifndef IL_NO_TGA
		case IL_TGA:
			return ilSaveTarga(FileName);
		#endif

		#ifndef IL_NO_TIF
		case IL_TIF:
			return ilSaveTiff(FileName);
		#endif

		#ifndef IL_NO_VTF
		case IL_VTF:
			return ilSaveVtf(FileName);
		#endif

		#ifndef IL_NO_WBMP
		case IL_WBMP:
			return ilSaveWbmp(FileName);
		#endif

		case IL_JASC_PAL:
			return ilSaveJascPal(FileName);
	}

	ilSetError(IL_INVALID_ENUM);
	return IL_FALSE;
}


//! Attempts to save an image to a file stream.  The file format is specified by the user.
/*! \param Type Format of this file.  Acceptable values are IL_BMP, IL_CHEAD, IL_DDS, IL_EXR,
	IL_HDR, IL_JP2, IL_JPG, IL_PCX, IL_PNG, IL_PNM, IL_PSD, IL_RAW, IL_SGI, IL_TGA, IL_TIF,
	IL_VTF, IL_WBMP and IL_JASC_PAL.
	\param File File stream to save to.
	\return Boolean value of failure or success.  Returns IL_FALSE if saving failed.*/
ILuint ILAPIENTRY ilSaveF(ILenum Type, ILHANDLE File)
{
	ILboolean Ret;

	if (File == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return 0;
	}

	switch (Type)
	{
		#ifndef IL_NO_BMP
		case IL_BMP:
			Ret = ilSaveBmpF(File);
			break;
		#endif

		#ifndef IL_NO_DDS
		case IL_DDS:
			Ret = ilSaveDdsF(File);
			break;
		#endif

		#ifndef IL_NO_EXR
		case IL_EXR:
			Ret = ilSaveExrF(File);
			break;
		#endif

		#ifndef IL_NO_HDR
		case IL_HDR:
			Ret = ilSaveHdrF(File);
			break;
		#endif

		#ifndef IL_NO_JP2
		case IL_JP2:
			Ret = ilSaveJp2F(File);
			break;
		#endif

		#ifndef IL_NO_JPG
			#ifndef IL_USE_IJL
			case IL_JPG:
				Ret = ilSaveJpegF(File);
				break;
			#endif
		#endif

		#ifndef IL_NO_PNM
		case IL_PNM:
			Ret = ilSavePnmF(File);
			break;
		#endif

		#ifndef IL_NO_PNG
		case IL_PNG:
			Ret = ilSavePngF(File);
			break;	
		#endif

		#ifndef IL_NO_PSD
		case IL_PSD:
			Ret = ilSavePsdF(File);
			break;
		#endif

		#ifndef IL_NO_RAW
		case IL_RAW:
			Ret = ilSaveRawF(File);
			break;
		#endif

		#ifndef IL_NO_SGI
		case IL_SGI:
			Ret = ilSaveSgiF(File);
			break;
		#endif

		#ifndef IL_NO_TGA
		case IL_TGA:
			Ret = ilSaveTargaF(File);
			break;
		#endif

		#ifndef IL_NO_VTF
		case IL_VTF:
			Ret = ilSaveVtfF(File);
			break;
		#endif

		#ifndef IL_NO_WBMP
		case IL_WBMP:
			Ret = ilSaveWbmpF(File);
			break;
		#endif

		#ifndef IL_NO_TIF
		case IL_TIF:
			Ret = ilSaveTiffF(File);
			break;
		#endif

		default:
			ilSetError(IL_INVALID_ENUM);
			return 0;
	}

	if (Ret == IL_FALSE)
		return 0;

	return itellw();
}


//! Attempts to save an image to a memory buffer.  The file format is specified by the user.
/*! \param Type Format of this image file.  Acceptable values are IL_BMP, IL_CHEAD, IL_DDS, IL_EXR,
	IL_HDR, IL_JP2, IL_JPG, IL_PCX, IL_PNG, IL_PNM, IL_PSD, IL_RAW, IL_SGI, IL_TGA, IL_TIF,
	IL_VTF, IL_WBMP and IL_JASC_PAL.
	\param Lump Memory buffer to save to
	\param Size Size of the memory buffer
	\return Boolean value of failure or success.  Returns IL_FALSE if saving failed.*/
ILuint ILAPIENTRY ilSaveL(ILenum Type, void *Lump, ILuint Size)
{
	if (Lump == NULL) {
		if (Size != 0) {
			ilSetError(IL_INVALID_PARAM);
			return 0;
		}
		// The user wants to know how large of a buffer they need.
		else {
			return ilDetermineSize(Type);
		}
	}

	switch (Type)
	{
		#ifndef IL_NO_BMP
		case IL_BMP:
			return ilSaveBmpL(Lump, Size);
		#endif

		#ifndef IL_NO_EXR
		case IL_EXR:
			return ilSaveExrL(Lump, Size);
		#endif

		#ifndef IL_NO_HDR
		case IL_HDR:
			return ilSaveHdrL(Lump, Size);
		#endif

		#ifndef IL_NO_JP2
		case IL_JP2:
			return ilSaveJp2L(Lump, Size);
		#endif

		#ifndef IL_NO_JPG
		case IL_JPG:
			return ilSaveJpegL(Lump, Size);
		#endif

		#ifndef IL_NO_PNG
		case IL_PNG:
			return ilSavePngL(Lump, Size);
		#endif

		#ifndef IL_NO_PNM
		case IL_PNM:
			return ilSavePnmL(Lump, Size);
		#endif

		#ifndef IL_NO_PSD
		case IL_PSD:
			return ilSavePsdL(Lump, Size);
		#endif

		#ifndef IL_NO_RAW
		case IL_RAW:
			return ilSaveRawL(Lump, Size);
		#endif

		#ifndef IL_NO_SGI
		case IL_SGI:
			return ilSaveSgiL(Lump, Size);
		#endif

		#ifndef IL_NO_TGA
		case IL_TGA:
			return ilSaveTargaL(Lump, Size);
		#endif

		#ifndef IL_NO_DDS
		case IL_DDS:
			return ilSaveDdsL(Lump, Size);
		#endif

		#ifndef IL_NO_VTF
		case IL_VTF:
			return ilSaveVtfL(Lump, Size);
		#endif

		#ifndef IL_NO_WBMP
		case IL_WBMP:
			return ilSaveWbmpL(Lump, Size);
		#endif

		#ifndef IL_NO_TIF
		case IL_TIF:
			return ilSaveTiffL(Lump, Size);
		#endif
	}

	ilSetError(IL_INVALID_ENUM);
	return 0;
}


//! Saves the current image based on the extension given in FileName.
/*! \param FileName Ansi or Unicode string, depending on the compiled version of DevIL, that gives
	       the filename to save to.
	\return Boolean value of failure or success.  Returns IL_FALSE if saving failed.*/
ILboolean ILAPIENTRY ilSaveImage(ILconst_string FileName)
{
	ILstring Ext;
	ILboolean	bRet = IL_FALSE;

	if (FileName == NULL || ilStrLen(FileName) < 1) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	Ext = iGetExtension(FileName);
	if (Ext == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return IL_FALSE;
	}

	#ifndef IL_NO_BMP
	if (!iStrCmp(Ext, IL_TEXT("bmp"))) {
		bRet = ilSaveBmp(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_CHEAD
	if (!iStrCmp(Ext, IL_TEXT("h"))) {
		bRet = ilSaveCHeader(FileName, (char*)"IL_IMAGE");
		goto finish;
	}
	#endif

	#ifndef IL_NO_DDS
	if (!iStrCmp(Ext, IL_TEXT("dds"))) {
		bRet = ilSaveDds(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_EXR
	if (!iStrCmp(Ext, IL_TEXT("exr"))) {
		bRet = ilSaveExr(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_HDR
	if (!iStrCmp(Ext, IL_TEXT("hdr"))) {
		bRet = ilSaveHdr(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_JP2
	if (!iStrCmp(Ext, IL_TEXT("jp2"))) {
		bRet = ilSaveJp2(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_JPG
	if (!iStrCmp(Ext, IL_TEXT("jpg")) || !iStrCmp(Ext, IL_TEXT("jpeg")) || !iStrCmp(Ext, IL_TEXT("jpe"))) {
		bRet = ilSaveJpeg(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_PCX
	if (!iStrCmp(Ext, IL_TEXT("pcx"))) {
		bRet = ilSavePcx(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_PNG
	if (!iStrCmp(Ext, IL_TEXT("png"))) {
		bRet = ilSavePng(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_PNM  // Not sure if binary or ascii should be defaulted...maybe an option?
	if (!iStrCmp(Ext, IL_TEXT("pbm"))) {
		bRet = ilSavePnm(FileName);
		goto finish;
	}
	if (!iStrCmp(Ext, IL_TEXT("pgm"))) {
		bRet = ilSavePnm(FileName);
		goto finish;
	}
	if (!iStrCmp(Ext, IL_TEXT("ppm"))) {
		bRet = ilSavePnm(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_PSD
	if (!iStrCmp(Ext, IL_TEXT("psd"))) {
		bRet = ilSavePsd(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_RAW
	if (!iStrCmp(Ext, IL_TEXT("raw"))) {
		bRet = ilSaveRaw(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_SGI
	if (!iStrCmp(Ext, IL_TEXT("sgi")) || !iStrCmp(Ext, IL_TEXT("bw")) ||
		!iStrCmp(Ext, IL_TEXT("rgb")) || !iStrCmp(Ext, IL_TEXT("rgba"))) {
		bRet = ilSaveSgi(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_TGA
	if (!iStrCmp(Ext, IL_TEXT("tga"))) {
		bRet = ilSaveTarga(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_TIF
	if (!iStrCmp(Ext, IL_TEXT("tif")) || !iStrCmp(Ext, IL_TEXT("tiff"))) {
		bRet = ilSaveTiff(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_VTF
	if (!iStrCmp(Ext, IL_TEXT("vtf"))) {
		bRet = ilSaveVtf(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_WBMP
	if (!iStrCmp(Ext, IL_TEXT("wbmp"))) {
		bRet = ilSaveWbmp(FileName);
		goto finish;
	}
	#endif

	#ifndef IL_NO_MNG
	if (!iStrCmp(Ext, IL_TEXT("mng"))) {
		bRet = ilSaveMng(FileName);
		goto finish;
	}
	#endif

	// Check if we just want to save the palette.
	if (!iStrCmp(Ext, IL_TEXT("pal"))) {
		bRet = ilSavePal(FileName);
		goto finish;
	}

	// Try registered procedures
	if (iRegisterSave(FileName))
		return IL_TRUE;

	ilSetError(IL_INVALID_EXTENSION);
	return IL_FALSE;

finish:
	return bRet;
}
