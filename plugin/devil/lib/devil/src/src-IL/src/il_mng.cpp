//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_mng.cpp
//
// Description: Multiple Network Graphics (.mng) functions
//
//-----------------------------------------------------------------------------

#include "il_internal.h"
#ifndef IL_NO_MNG
#define MNG_SUPPORT_READ
#define MNG_SUPPORT_WRITE
#define MNG_SUPPORT_DISPLAY

#ifdef _WIN32
//#define MNG_USE_SO
#endif

#if defined(_WIN32) && defined(IL_USE_PRAGMA_LIBS)
	#if defined(_MSC_VER) || defined(__BORLANDC__)
		#ifndef _DEBUG
			#pragma comment(lib, "libmng.lib")
			#pragma comment(lib, "lcms2.lib")
			#pragma comment(lib, "libjpeg.lib")
			#pragma comment(lib, "zlibstatic.lib")
		#else
			#pragma comment(lib, "libmng.lib")
			#pragma comment(lib, "lcms2.lib")
			#pragma comment(lib, "libjpeg.lib")
			#pragma comment(lib, "zlibstatic.lib")
		#endif
	#endif
#endif


#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable : 4142)  // Redefinition in jmorecfg.h
#endif

#include <libmng.h>

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


//---------------------------------------------------------------------------------------------
// memory allocation; data must be zeroed
//---------------------------------------------------------------------------------------------
mng_ptr MNG_DECL mymngalloc(mng_size_t size)
{
	return (mng_ptr)icalloc(1, size);
}


//---------------------------------------------------------------------------------------------
// memory deallocation
//---------------------------------------------------------------------------------------------
void MNG_DECL mymngfree(mng_ptr p, mng_size_t size)
{
	ifree(p);
}


//---------------------------------------------------------------------------------------------
// Stream open:
//---------------------------------------------------------------------------------------------
mng_bool MNG_DECL mymngopenstream(mng_handle mng)
{
	return MNG_TRUE;
}


//---------------------------------------------------------------------------------------------
// Stream open for Writing:
//---------------------------------------------------------------------------------------------
mng_bool MNG_DECL mymngopenstreamwrite(mng_handle mng)
{
	return MNG_TRUE;
}


//---------------------------------------------------------------------------------------------
// Stream close:
//---------------------------------------------------------------------------------------------
mng_bool MNG_DECL mymngclosestream(mng_handle mng)
{
	return MNG_TRUE; // We close the file ourself, mng_cleanup doesnt seem to do it...
}


//---------------------------------------------------------------------------------------------
// feed data to the decoder
//---------------------------------------------------------------------------------------------
mng_bool MNG_DECL mymngreadstream(mng_handle mng, mng_ptr buffer, mng_size_t size, mng_uint32 *bytesread)
{
	// read the requested amount of data from the file
	*bytesread = iread(buffer, 1, (ILuint)size);

	return MNG_TRUE;
}


//---------------------------------------------------------------------------------------------
// callback for writing data
//---------------------------------------------------------------------------------------------
mng_bool MNG_DECL mymngwritedata(mng_handle mng, mng_ptr buffer, mng_size_t size, mng_uint32 *byteswritten)
{
	*byteswritten = iwrite(buffer, 1, (ILuint)size);

	if (*byteswritten < size) {
		ilSetError(IL_FILE_WRITE_ERROR);
		return MNG_FALSE;
	}

	return MNG_TRUE;
}


//---------------------------------------------------------------------------------------------
// the header's been read. set up the display stuff
//---------------------------------------------------------------------------------------------
mng_bool MNG_DECL mymngprocessheader(mng_handle mng, mng_uint32 width, mng_uint32 height)
{
	ILuint	AlphaDepth;

	AlphaDepth = mng_get_alphadepth(mng);

	if (AlphaDepth == 0) {
		ilTexImage(width, height, 1, 3, IL_BGR, IL_UNSIGNED_BYTE, NULL);
		iCurImage->Origin = IL_ORIGIN_LOWER_LEFT;
		mng_set_canvasstyle(mng, MNG_CANVAS_BGR8);
	}
	else {  // Use alpha channel
		ilTexImage(width, height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);
		iCurImage->Origin = IL_ORIGIN_LOWER_LEFT;
		mng_set_canvasstyle(mng, MNG_CANVAS_BGRA8);
	}

	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;

	return MNG_TRUE;
}


//---------------------------------------------------------------------------------------------
// return a row pointer for the decoder to fill
//---------------------------------------------------------------------------------------------
mng_ptr MNG_DECL mymnggetcanvasline(mng_handle mng, mng_uint32 line)
{
	return (mng_ptr)(iCurImage->Data + iCurImage->Bps * line);
}


//---------------------------------------------------------------------------------------------
// timer
//---------------------------------------------------------------------------------------------
mng_uint32 MNG_DECL mymnggetticks(mng_handle mng)
{
	return 0;
}


//---------------------------------------------------------------------------------------------
// Refresh:
//---------------------------------------------------------------------------------------------
mng_bool MNG_DECL mymngrefresh(mng_handle mng, mng_uint32 x, mng_uint32 y, mng_uint32 w, mng_uint32 h)
{
	return MNG_TRUE;
}


//---------------------------------------------------------------------------------------------
// interframe delay callback
//---------------------------------------------------------------------------------------------
mng_bool MNG_DECL mymngsettimer(mng_handle mng, mng_uint32 msecs)
{
	return MNG_TRUE;
}



// Make this do something different soon!

//---------------------------------------------------------------------------------------------
// Error Callback;
//---------------------------------------------------------------------------------------------
mng_bool MNG_DECL mymngerror(
	mng_handle mng, mng_int32 code, mng_int8 severity,
	mng_chunkid chunktype, mng_uint32 chunkseq,
	mng_int32 extra1, mng_int32 extra2, mng_pchar text
	)
{
	// error occured;
	return MNG_FALSE;
}


ILboolean iLoadMngInternal(void);

// Reads a file
ILboolean ilLoadMng(ILconst_string FileName)
{
	ILHANDLE	MngFile;
	ILboolean	bMng = IL_FALSE;

	MngFile = iopenr(FileName);
	if (MngFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bMng;
	}

	bMng = ilLoadMngF(MngFile);
	icloser(MngFile);

	return bMng;
}


// Reads an already-opened file
ILboolean ilLoadMngF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadMngInternal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


// Reads from a memory "lump"
ILboolean ilLoadMngL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadMngInternal();
}


ILboolean iLoadMngInternal()
{
	mng_handle mng;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	mng = mng_initialize(MNG_NULL, mymngalloc, mymngfree, MNG_NULL);
	if (mng == MNG_NULL) {
		ilSetError(IL_LIB_MNG_ERROR);
		return IL_FALSE;
	}

	// If .mng background is available, use it.
	mng_set_usebkgd(mng, MNG_TRUE);

	// Set the callbacks.
	mng_setcb_errorproc(mng, mymngerror);
    mng_setcb_openstream(mng, mymngopenstream);
    mng_setcb_closestream(mng, mymngclosestream);
    mng_setcb_readdata(mng, (mng_readdata)mymngreadstream);
	mng_setcb_gettickcount(mng, mymnggetticks);
	mng_setcb_settimer(mng, mymngsettimer);
	mng_setcb_processheader(mng, mymngprocessheader);
	mng_setcb_getcanvasline(mng, mymnggetcanvasline);
	mng_setcb_refresh(mng, mymngrefresh);

	mng_read(mng);
	mng_display(mng);

	return ilFixImage();
}


ILboolean iSaveMngInternal(void);

//! Writes a Mng file
ILboolean ilSaveMng(const ILstring FileName)
{
	ILHANDLE	MngFile;
	ILuint		MngSize;

	if (ilGetBoolean(IL_FILE_MODE) == IL_FALSE) {
		if (iFileExists(FileName)) {
			ilSetError(IL_FILE_ALREADY_EXISTS);
			return IL_FALSE;
		}
	}

	MngFile = iopenw(FileName);
	if (MngFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	MngSize = ilSaveMngF(MngFile);
	iclosew(MngFile);

	if (MngSize == 0)
		return IL_FALSE;
	return IL_TRUE;
}


//! Writes a Mng to an already-opened file
ILuint ilSaveMngF(ILHANDLE File)
{
	ILuint Pos = itellw();
	iSetOutputFile(File);
	if (iSaveMngInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


//! Writes a Mng to a memory "lump"
ILuint ilSaveMngL(void *Lump, ILuint Size)
{
	ILuint Pos;
	iSetOutputLump(Lump, Size);
	Pos = itellw();
	if (iSaveMngInternal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


// Internal function used to save the Mng.
ILboolean iSaveMngInternal()
{
	//mng_handle mng;

	// Not working yet, so just error out.
	ilSetError(IL_INVALID_EXTENSION);
	return IL_FALSE;

	//if (iCurImage == NULL) {
	//	ilSetError(IL_ILLEGAL_OPERATION);
	//	return IL_FALSE;
	//}

	//mng = mng_initialize(MNG_NULL, mymngalloc, mymngfree, MNG_NULL);
	//if (mng == MNG_NULL) {
	//	ilSetError(IL_LIB_MNG_ERROR);
	//	return IL_FALSE;
	//}

	//mng_setcb_openstream(mng, mymngopenstreamwrite);
	//mng_setcb_closestream(mng, mymngclosestream);
	//mng_setcb_writedata(mng, mymngwritedata);

	//// Write File:
 //  	mng_create(mng);

	//// Check return value.
	//mng_putchunk_mhdr(mng, iCurImage->Width, iCurImage->Height, 1000, 3, 1, 3, 0x0047);
	//mng_putchunk_basi(mng, iCurImage->Width, iCurImage->Height, 8, MNG_COLORTYPE_RGB, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 1);
	//mng_putchunk_iend(mng);
	//mng_putimgdata_ihdr(mng, iCurImage->Width, iCurImage->Height, MNG_COLORTYPE_RGB, 8, 0, 0, 0, 0, mymnggetcanvasline);

	//// Now write file:
	//mng_write(mng);

	//mng_cleanup(&mng);

	//return IL_TRUE;
}

#endif//IL_NO_MNG
