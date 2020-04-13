//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_jp2.cpp
//
// Description: Jpeg-2000 (.jp2) functions
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_JP2
#include <jasper/jasper.h>
#include "il_jp2.h"

#if defined(_WIN32) && defined(IL_USE_PRAGMA_LIBS)
	#if defined(_MSC_VER) || defined(__BORLANDC__)
		#ifndef _DEBUG
			#pragma comment(lib, "libjasper.lib")
		#else
			#pragma comment(lib, "libjasper.lib")  //libjasper-d.lib
		#endif
	#endif
#endif

ILboolean iIsValidJp2(void);

ILboolean JasperInit = IL_FALSE;


//! Checks if the file specified in FileName is a valid .jp2 file.
ILboolean ilIsValidJp2(ILconst_string FileName)
{
	ILHANDLE	Jp2File;
	ILboolean	bJp2 = IL_FALSE;

	if (!iCheckExtension(FileName, IL_TEXT("jp2")) && !iCheckExtension(FileName, IL_TEXT("jpx")) &&
		!iCheckExtension(FileName, IL_TEXT("j2k")) && !iCheckExtension(FileName, IL_TEXT("j2c"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bJp2;
	}

	Jp2File = iopenr(FileName);
	if (Jp2File == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bJp2;
	}

	bJp2 = ilIsValidJp2F(Jp2File);
	icloser(Jp2File);

	return bJp2;
}


//! Checks if the ILHANDLE contains a valid .jp2 file at the current position.
ILboolean ilIsValidJp2F(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidJp2();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Checks if Lump is a valid .jp2 lump.
ILboolean ilIsValidJp2L(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidJp2();
}


// Internal function to get the header and check it.
ILboolean iIsValidJp2(void)
{
	ILubyte Signature[4];

	iseek(4, IL_SEEK_CUR);  // Skip the 4 bytes that tell the size of the signature box.
	if (iread(Signature, 1, 4) != 4) {
		iseek(-4, IL_SEEK_CUR);
		return IL_FALSE;  // File read error
	}

	iseek(-8, IL_SEEK_CUR);  // Restore to previous state

	// Signature is 'jP\040\040' by the specs (or 0x6A502020).
	//  http://www.jpeg.org/public/fcd15444-6.pdf
	if (Signature[0] != 0x6A || Signature[1] != 0x50 ||
		Signature[2] != 0x20 || Signature[3] != 0x20)
		return IL_FALSE;

	return IL_TRUE;
}


//! Reads a Jpeg2000 file.
ILboolean ilLoadJp2(ILconst_string FileName)
{
	ILHANDLE	Jp2File;
	ILboolean	bRet;

	Jp2File = iopenr(FileName);
	if (Jp2File == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	bRet = ilLoadJp2F(Jp2File);
	icloser(Jp2File);

	return bRet;
}


//! Reads an already-opened Jpeg2000 file.
ILboolean ilLoadJp2F(ILHANDLE File)
{
	ILuint			FirstPos;
	ILboolean		bRet;
	jas_stream_t	*Stream;

	iSetInputFile(File);
	FirstPos = itell();

	if (!JasperInit) {
		if (jas_init()) {
			ilSetError(IL_LIB_JP2_ERROR);
			return IL_FALSE;
		}
		JasperInit = IL_TRUE;
	}
	Stream = iJp2ReadStream();
	if (!Stream)
	{
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	bRet = iLoadJp2Internal(Stream, NULL);
	// Close the input stream.
	jas_stream_close(Stream);

	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Reads from a memory "lump" that contains a Jpeg2000 stream.
ILboolean ilLoadJp2L(const void *Lump, ILuint Size)
{
	return ilLoadJp2LInternal(Lump, Size, NULL);
}


//! This is separated so that it can be called for other file types, such as .icns.
ILboolean ilLoadJp2LInternal(const void *Lump, ILuint Size, ILimage *Image)
{
	ILboolean		bRet;
	jas_stream_t	*Stream;

	if (!JasperInit) {
		if (jas_init()) {
			ilSetError(IL_LIB_JP2_ERROR);
			return IL_FALSE;
		}
		JasperInit = IL_TRUE;
	}
	Stream = jas_stream_memopen((char*)Lump, Size);
	if (!Stream)
	{
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	bRet = iLoadJp2Internal(Stream, Image);
	// Close the input stream.
	jas_stream_close(Stream);

	return bRet;
}


// Internal function used to load the Jpeg2000 stream.
ILboolean iLoadJp2Internal(jas_stream_t	*Stream, ILimage *Image)
{
	jas_image_t		*Jp2Image = NULL;
	jas_matrix_t	*origdata;
	ILuint			x, y, c, Error;
	ILimage			*TempImage;

	// Decode image
	Jp2Image = jas_image_decode(Stream, -1, 0);
	if (!Jp2Image)
	{
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		jas_stream_close(Stream);
		return IL_FALSE;
	}

	// JasPer likes to buffer a lot, so it may try buffering past the end
	//  of the file.  iread naturally sets IL_FILE_READ_ERROR if it tries
	//  reading past the end of the file, but this actually is not an error.
	Error = ilGetError();
	// Put the error back if it is not IL_FILE_READ_ERROR.
	if (Error != IL_FILE_READ_ERROR)
		ilSetError(Error);


	// We're not supporting anything other than 8 bits/component yet.
	if (jas_image_cmptprec(Jp2Image, 0) != 8)
	{
		jas_image_destroy(Jp2Image);
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		return IL_FALSE;
	}

	switch (jas_image_numcmpts(Jp2Image))
	{
		//@TODO: Can we do alpha data?  jas_image_cmpttype always returns 0 for this case.
		case 1:  // Assuming this is luminance data.
			if (Image == NULL) {
				ilTexImage(jas_image_width(Jp2Image), jas_image_height(Jp2Image), 1, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, NULL);
				TempImage = iCurImage;
			}
			else {
				ifree(Image->Data);  // @TODO: Not really the most efficient way to do this...
				ilInitImage(Image, jas_image_width(Jp2Image), jas_image_height(Jp2Image), 1, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, NULL);
				TempImage = Image;
			}
			break;

		case 2:  // Assuming this is luminance-alpha data.
			if (Image == NULL) {
				ilTexImage(jas_image_width(Jp2Image), jas_image_height(Jp2Image), 1, 2, IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE, NULL);
				TempImage = iCurImage;
			}
			else {
				ifree(Image->Data);  // @TODO: Not really the most efficient way to do this...
				ilInitImage(Image, jas_image_width(Jp2Image), jas_image_height(Jp2Image), 1, 2, IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE, NULL);
				TempImage = Image;
			}
			break;

		case 3:
			if (Image == NULL) {
				ilTexImage(jas_image_width(Jp2Image), jas_image_height(Jp2Image), 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);
				TempImage = iCurImage;
			}
			else {
				ifree(Image->Data);  // @TODO: Not really the most efficient way to do this...
				ilInitImage(Image, jas_image_width(Jp2Image), jas_image_height(Jp2Image), 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);
				TempImage = Image;
			}
			break;
		case 4:
			if (Image == NULL) {
				ilTexImage(jas_image_width(Jp2Image), jas_image_height(Jp2Image), 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
				TempImage = iCurImage;
			}
			else {
				ifree(Image->Data);  // @TODO: Not really the most efficient way to do this...
				ilInitImage(Image, jas_image_width(Jp2Image), jas_image_height(Jp2Image), 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
				TempImage = Image;
			}
			break;
		default:
			jas_image_destroy(Jp2Image);
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			return IL_FALSE;
	}
	TempImage->Origin = IL_ORIGIN_UPPER_LEFT;

	// JasPer stores the data channels separately.
	//  I am assuming RGBA format.  Is it possible for other formats to be included?
	for (c = 0; c < TempImage->Bpp; c++)
	{
		origdata = jas_matrix_create(TempImage->Height, TempImage->Width);
		if (!origdata)
		{
			ilSetError(IL_LIB_JP2_ERROR);
			return IL_FALSE;  // @TODO: Error
		}
		// Have to convert data into an intermediate matrix format.
		if (jas_image_readcmpt(Jp2Image, c, 0, 0, TempImage->Width, TempImage->Height, origdata))
		{
			return IL_FALSE;
		}

		for (y = 0; y < TempImage->Height; y++)
		{
			for (x = 0; x < TempImage->Width; x++)
			{
				TempImage->Data[y * TempImage->Width * TempImage->Bpp + x * TempImage->Bpp + c] = origdata->data_[y * origdata->numcols_ + x];
			}
		}

		jas_matrix_destroy(origdata);
	}

	jas_image_destroy(Jp2Image);

	return ilFixImage();
}



static int iJp2_file_read(jas_stream_obj_t *obj, char *buf, int cnt)
{
	obj;
	return iread(buf, 1, cnt);
}

static int iJp2_file_write(jas_stream_obj_t *obj, char *buf, int cnt)
{
	obj;
	return iwrite(buf, 1, cnt);
}

static long iJp2_file_seek(jas_stream_obj_t *obj, long offset, int origin)
{
	obj;

	// We could just pass origin to iseek, but this is probably more portable.
	switch (origin)
	{
		case SEEK_SET:
			return iseek(offset, IL_SEEK_SET);
		case SEEK_CUR:
			return iseek(offset, IL_SEEK_CUR);
		case SEEK_END:
			return iseek(offset, IL_SEEK_END);
	}
	return 0;  // Failed
}

static int iJp2_file_close(jas_stream_obj_t *obj)
{
	obj;
	return 0;  // We choose when we want to close the file.
}

static jas_stream_ops_t jas_stream_devilops = {
	iJp2_file_read,
	iJp2_file_write,
	iJp2_file_seek,
	iJp2_file_close
};

static jas_stream_t *jas_stream_create(void);
static void jas_stream_destroy(jas_stream_t *stream);
static void jas_stream_initbuf(jas_stream_t *stream, int bufmode );


// Modified version of jas_stream_fopen and jas_stream_memopen from jas_stream.c of JasPer
//  so that we can use our own file routines.
jas_stream_t *iJp2ReadStream()
{
	jas_stream_t *stream;
	jas_stream_memobj_t *obj;

	if (!(stream = jas_stream_create())) {
		return 0;
	}

	/* A stream associated with a memory buffer is always opened
	for both reading and writing in binary mode. */
	stream->openmode_ = JAS_STREAM_READ | JAS_STREAM_BINARY;

	/* We use buffering whether it is from memory or a file. */
	jas_stream_initbuf(stream, JAS_STREAM_FULLBUF);

	/* Select the operations for a memory stream. */
	stream->ops_ = &jas_stream_devilops;

	/* Allocate memory for the underlying memory stream object. */
	if (!(obj = (jas_stream_memobj_t*)jas_malloc(sizeof(jas_stream_memobj_t)))) {
		jas_stream_destroy(stream);
		return 0;
	}
	stream->obj_ = (void *) obj;

	/* Initialize a few important members of the memory stream object. */
	obj->myalloc_ = 0;
	obj->buf_ = 0;

	// Shouldn't need any of this.

	///* If the buffer size specified is nonpositive, then the buffer
	//is allocated internally and automatically grown as needed. */
	//if (bufsize <= 0) {
	//	obj->bufsize_ = 1024;
	//	obj->growable_ = 1;
	//} else {
	//	obj->bufsize_ = bufsize;
	//	obj->growable_ = 0;
	//}
	//if (buf) {
	//	obj->buf_ = (unsigned char *) buf;
	//} else {
	//	obj->buf_ = jas_malloc(obj->bufsize_ * sizeof(char));
	//	obj->myalloc_ = 1;
	//}
	//if (!obj->buf_) {
	//	jas_stream_close(stream);
	//	return 0;
	//}

	//if (bufsize > 0 && buf) {
	//	/* If a buffer was supplied by the caller and its length is positive,
	//	  make the associated buffer data appear in the stream initially. */
	//	obj->len_ = bufsize;
	//} else {
	//	/* The stream is initially empty. */
	//	obj->len_ = 0;
	//}
	//obj->pos_ = 0;
	
	return stream;
}


// The following functions are taken directly from jas_stream.c of JasPer,
//  since they are designed to be used within JasPer only.

static void jas_stream_initbuf(jas_stream_t *stream, int bufmode )
{
	/* If this function is being called, the buffer should not have been
	  initialized yet. */
	assert(!stream->bufbase_);

	if (bufmode != JAS_STREAM_UNBUF) {
		/* The full- or line-buffered mode is being employed. */
        if ((stream->bufbase_ = (unsigned char*)jas_malloc(JAS_STREAM_BUFSIZE +
          JAS_STREAM_MAXPUTBACK))) {
            stream->bufmode_ |= JAS_STREAM_FREEBUF;
            stream->bufsize_ = JAS_STREAM_BUFSIZE;
        } else {
            /* The buffer allocation has failed.  Resort to unbuffered
              operation. */
            stream->bufbase_ = stream->tinybuf_;
            stream->bufsize_ = 1;
        }
	} else {
		/* The unbuffered mode is being employed. */
		/* Use a trivial one-character buffer. */
		stream->bufbase_ = stream->tinybuf_;
		stream->bufsize_ = 1;
	}
	stream->bufstart_ = &stream->bufbase_[JAS_STREAM_MAXPUTBACK];
	stream->ptr_ = stream->bufstart_;
	stream->cnt_ = 0;
	stream->bufmode_ |= bufmode & JAS_STREAM_BUFMODEMASK;
}

static jas_stream_t *jas_stream_create()
{
	jas_stream_t *stream;

	if (!(stream = (jas_stream_t*)jas_malloc(sizeof(jas_stream_t)))) {
		return 0;
	}
	stream->openmode_ = 0;
	stream->bufmode_ = 0;
	stream->flags_ = 0;
	stream->bufbase_ = 0;
	stream->bufstart_ = 0;
	stream->bufsize_ = 0;
	stream->ptr_ = 0;
	stream->cnt_ = 0;
	stream->ops_ = 0;
	stream->obj_ = 0;
	stream->rwcnt_ = 0;
	stream->rwlimit_ = -1;

	return stream;
}

static void jas_stream_destroy(jas_stream_t *stream)
{
	/* If the memory for the buffer was allocated with malloc, free
	this memory. */
	if ((stream->bufmode_ & JAS_STREAM_FREEBUF) && stream->bufbase_) {
		jas_free(stream->bufbase_);
		stream->bufbase_ = 0;
	}
	jas_free(stream);
}




jas_stream_t *iJp2WriteStream()
{
	jas_stream_t *stream;
	jas_stream_memobj_t *obj;

	if (!(stream = jas_stream_create())) {
		return 0;
	}

	/* A stream associated with a memory buffer is always opened
	for both reading and writing in binary mode. */
	stream->openmode_ = JAS_STREAM_WRITE | JAS_STREAM_BINARY;

	/* We use buffering whether it is from memory or a file. */
	jas_stream_initbuf(stream, JAS_STREAM_FULLBUF);

	/* Select the operations for a memory stream. */
	stream->ops_ = &jas_stream_devilops;

	/* Allocate memory for the underlying memory stream object. */
	if (!(obj = (jas_stream_memobj_t*)jas_malloc(sizeof(jas_stream_memobj_t)))) {
		jas_stream_destroy(stream);
		return 0;
	}
	stream->obj_ = (void *) obj;

	/* Initialize a few important members of the memory stream object. */
	obj->myalloc_ = 0;
	obj->buf_ = 0;
	
	return stream;
}



//! Writes a Jp2 file
ILboolean ilSaveJp2(const ILstring FileName)
{
	ILHANDLE	Jp2File;
	ILuint		Jp2Size;

	if (ilGetBoolean(IL_FILE_MODE) == IL_FALSE) {
		if (iFileExists(FileName)) {
			ilSetError(IL_FILE_ALREADY_EXISTS);
			return IL_FALSE;
		}
	}

	Jp2File = iopenw(FileName);
	if (Jp2File == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	Jp2Size = ilSaveJp2F(Jp2File);
	iclosew(Jp2File);

	if (Jp2Size == 0)
		return IL_FALSE;
	return IL_TRUE;
}


//! Writes a Jp2 to an already-opened file
ILuint ilSaveJp2F(ILHANDLE File)
{
	ILuint Pos;
	iSetOutputFile(File);
	Pos = itellw();
	if (iSaveJp2Internal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


//! Writes a Jp2 to a memory "lump"
ILuint ilSaveJp2L(void *Lump, ILuint Size)
{
	ILuint Pos;
	iSetOutputLump(Lump, Size);
	Pos = itellw();
	if (iSaveJp2Internal() == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}



// Function from OpenSceneGraph (originally called getdata in their sources):
//  http://openscenegraph.sourcearchive.com/documentation/2.2.0/ReaderWriterJP2_8cpp-source.html
ILint Jp2ConvertData(jas_stream_t *in, jas_image_t *image)
{
	int ret;
	int numcmpts;
	int cmptno;
	jas_matrix_t *data[4];
	int x;
	int y;
	int width, height;

	width = jas_image_cmptwidth(image, 0);
	height = jas_image_cmptheight(image, 0);
	numcmpts = jas_image_numcmpts(image);

	ret = -1;

	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
	for (cmptno = 0; cmptno < numcmpts; ++cmptno) {
		if (!(data[cmptno] = jas_matrix_create(1, width))) {
			goto done;
		}
	}
 
	for (y = height - 1; y >= 0; --y)
	//        for (y = 0; y < height; ++y)
	{
		for (x = 0; x < width; ++x)
		{
			for (cmptno = 0; cmptno < numcmpts; ++cmptno)
			{
				// The sample data is unsigned.
				int c;
				if ((c = jas_stream_getc(in)) == EOF) {
					return -1;
				}
				jas_matrix_set(data[cmptno], 0, x, c);
			}
		}
		for (cmptno = 0; cmptno < numcmpts; ++cmptno) {
			if (jas_image_writecmpt(image, cmptno, 0, y, width, 1,
				data[cmptno])) {
				goto done;
			}
		}
	}

	jas_stream_flush(in);
	ret = 0;

done:
	for (cmptno = 0; cmptno < numcmpts; ++cmptno) {
		if (data[cmptno]) {
			jas_matrix_destroy(data[cmptno]);
		}
	}

	return ret;
}


// Internal function used to save the Jp2.
// Since the JasPer documentation is extremely incomplete, I had to look at how OpenSceneGraph uses it:
//  http://openscenegraph.sourcearchive.com/documentation/2.2.0/ReaderWriterJP2_8cpp-source.html

//@TODO: Do we need to worry about images with depths > 1?
ILboolean iSaveJp2Internal()
{
	jas_image_t *Jp2Image;
	jas_image_cmptparm_t cmptparm[4];
	jas_stream_t *Mem, *Stream;
	ILuint	NumChans, i;
	ILenum	NewFormat, NewType = IL_UNSIGNED_BYTE;
	ILimage	*TempImage = iCurImage;

	if (!JasperInit) {
		if (jas_init()) {
			ilSetError(IL_LIB_JP2_ERROR);
			return IL_FALSE;
		}
		JasperInit = IL_TRUE;
	}

	if (iCurImage->Type != IL_UNSIGNED_BYTE) {  //@TODO: Support greater than 1 bpc.
		NewType = IL_UNSIGNED_BYTE;
	}
	//@TODO: Do luminance/luminance-alpha/alpha separately.
	switch (iCurImage->Format)
	{
		case IL_LUMINANCE:
			NewFormat = IL_LUMINANCE;
			NumChans = 1;
			break;
		case IL_ALPHA:
			NewFormat = IL_ALPHA;
			NumChans = 1;
			break;
		case IL_LUMINANCE_ALPHA:
			NewFormat = IL_LUMINANCE_ALPHA;
			NumChans = 2;
			break;
		case IL_COLOUR_INDEX:  // Assuming the color palette does not have an alpha value.
								//@TODO: Check for this in the future.
		case IL_RGB:
		case IL_BGR:
			NewFormat = IL_RGB;
			NumChans = 3;
			break;
		case IL_RGBA:
		case IL_BGRA:
			NewFormat = IL_RGBA;
			NumChans = 4;
			break;
	}

	if (NewType != iCurImage->Type || NewFormat != iCurImage->Format) {
		TempImage = iConvertImage(iCurImage, NewFormat, NewType);
		if (TempImage == NULL)
			return IL_FALSE;
	}

	// The origin is always in the lower left corner.  Flip the buffer if it is not.
	if (TempImage->Origin == IL_ORIGIN_UPPER_LEFT)
		iFlipBuffer(TempImage->Data, TempImage->Depth, TempImage->Bps, TempImage->Height);

	// Have to tell JasPer about each channel.  In our case, they all have the same information.
	for (i = 0; i < NumChans; i++) {
		cmptparm[i].width = iCurImage->Width;
		cmptparm[i].height = iCurImage->Height;
		cmptparm[i].hstep = 1;
		cmptparm[i].vstep = 1;
		cmptparm[i].tlx = 0;
		cmptparm[i].tly = 0;
		cmptparm[i].prec = 8;
		cmptparm[i].sgnd = 0;  // Unsigned data
	}

	// Using the unknown color space, since we have not determined the space yet.
	//  This is done in the following switch statement.
	Jp2Image = jas_image_create(NumChans, cmptparm, JAS_CLRSPC_UNKNOWN);
	if (Jp2Image == NULL) {
		ilSetError(IL_LIB_JP2_ERROR);
		return IL_FALSE;
	}

	switch (NumChans)
	{
		case 1:
			jas_image_setclrspc(Jp2Image, JAS_CLRSPC_SGRAY);
			if (NewFormat == IL_LUMINANCE)
				jas_image_setcmpttype(Jp2Image, 0, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y));
			else // IL_ALPHA
				jas_image_setcmpttype(Jp2Image, 0, JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_OPACITY));
			break;
		case 2:
			jas_image_setclrspc(Jp2Image, JAS_CLRSPC_SGRAY);
			jas_image_setcmpttype(Jp2Image, 0, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y));
			jas_image_setcmpttype(Jp2Image, 1, JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_OPACITY));
			break;
		case 3:
			jas_image_setclrspc(Jp2Image, JAS_CLRSPC_SRGB);
			jas_image_setcmpttype(Jp2Image, 0, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R));
			jas_image_setcmpttype(Jp2Image, 1, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G));
			jas_image_setcmpttype(Jp2Image, 2, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B));
			break;
		case 4:
			jas_image_setclrspc(Jp2Image, JAS_CLRSPC_SRGB);
			jas_image_setcmpttype(Jp2Image, 0, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R));
			jas_image_setcmpttype(Jp2Image, 1, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G));
			jas_image_setcmpttype(Jp2Image, 2, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B));
			jas_image_setcmpttype(Jp2Image, 3, JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_OPACITY));
			break;
	}

	Mem = jas_stream_memopen((char*)TempImage->Data, TempImage->SizeOfData);
	if (Mem == NULL) {
		jas_image_destroy(Jp2Image);
		ilSetError(IL_LIB_JP2_ERROR);
		return IL_FALSE;
	}
	Stream = iJp2WriteStream();
	if (Stream == NULL) {
		jas_stream_close(Mem);
		jas_image_destroy(Jp2Image);
		ilSetError(IL_LIB_JP2_ERROR);
		return IL_FALSE;
	}

	// Puts data in the format that JasPer wants it in.
	Jp2ConvertData(Mem, Jp2Image);

	// Does all of the encoding.
	if (jas_image_encode(Jp2Image, Stream, jas_image_strtofmt((char*)"jp2"), NULL)) {  //@TODO: Do we want to use any options?
		jas_stream_close(Mem);
		jas_stream_close(Stream);
		jas_image_destroy(Jp2Image);
		ilSetError(IL_LIB_JP2_ERROR);
		return IL_FALSE;
	}
	jas_stream_flush(Stream);  // Do any final writing.

	// Close the memory and output streams.
	jas_stream_close(Mem);
	jas_stream_close(Stream);
	// Destroy the JasPer image.
	jas_image_destroy(Jp2Image);

	// Destroy our temporary image if we used one.
	if (TempImage != iCurImage)
		ilCloseImage(TempImage);

	return IL_TRUE;
}


#endif//IL_NO_JP2
