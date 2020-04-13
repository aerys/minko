//-----------------------------------------------------------------------------
//
// ImageLib Source Example
// Copyright (C) 2000-2001 by Denton Woods
// Last modified:  09/06/2001 <--Y2K Compliant! =]
//
// Filename: examples/file override/file override.c
//
// Description: An example of overriding the DevIL reading and
//				  writing functions via ilSetRead and ilSetWrite.
//
//-----------------------------------------------------------------------------

// Required include files.
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <IL/il.h>
#include <stdio.h>

/* We would need ILU just because of iluErrorString() function... */
/* So make it possible for both with and without ILU!  */
#ifdef ILU_ENABLED
#include <IL/ilu.h>
#define PRINT_ERROR_MACRO printf("Error: %s\n", iluErrorString(Error))
#else /* not ILU_ENABLED */
#define PRINT_ERROR_MACRO printf("Error: 0x%X\n", (unsigned int)Error)
#endif /* not ILU_ENABLED */

ILHANDLE ILAPIENTRY iOpenRead(const char *FileName)
{
	return (ILHANDLE)fopen(FileName, "rb");
}

void ILAPIENTRY iCloseRead(ILHANDLE Handle)
{
	fclose((FILE*)Handle);
	return;
}

ILHANDLE ILAPIENTRY iOpenWrite(const char *FileName)
{
	return (ILHANDLE)fopen(FileName, "wb");
}

void ILAPIENTRY iCloseWrite(ILHANDLE Handle)
{
	fclose((FILE*)Handle);
	return;
}

ILboolean ILAPIENTRY iEof(ILHANDLE Handle)
{
	return (feof((FILE*)Handle) != 0);
}

ILint ILAPIENTRY iGetc(ILHANDLE Handle)
{
	return fgetc((FILE*)Handle);
}

ILint ILAPIENTRY iPutc(ILubyte Char, ILHANDLE Handle)
{
	return fputc(Char, (FILE*)Handle);
}

ILint ILAPIENTRY iRead(void *Buffer, ILuint Size, ILuint Number, ILHANDLE Handle)
{
	return fread(Buffer, Size, Number, (FILE*)Handle);
}

ILint ILAPIENTRY iWrite(const void *Buffer, ILuint Size, ILuint Number, ILHANDLE Handle)
{
	return fwrite(Buffer, Size, Number, (FILE*)Handle);
}

ILint ILAPIENTRY iReadSeek(ILHANDLE Handle, ILint Offset, ILint Mode)
{
	return fseek((FILE*)Handle, Offset, Mode);
}

ILint ILAPIENTRY iWriteSeek(ILHANDLE Handle, ILint Offset, ILint Mode)
{
	return fseek((FILE*)Handle, Offset, Mode);
}

ILint ILAPIENTRY iReadTell(ILHANDLE Handle)
{
	return ftell((FILE*)Handle);
}

ILint ILAPIENTRY iWriteTell(ILHANDLE Handle)
{
	return ftell((FILE*)Handle);
}




int main(int argc, char **argv)
{
	ILuint	ImgId;
	ILenum	Error;

	// We use the filename specified in the first argument of the command-line.
	if (argc < 2) {
		printf("Please specify a file to open.\n");
		return 1;
	}

	// Check if the shared lib's version matches the executable's version.
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION) {
		printf("DevIL version is different...exiting!\n");
		return 2;
	}

	// Initialize DevIL.
	ilInit();
	// Generate the main image name to use.
	ilGenImages(1, &ImgId);
	// Bind this image name.
	ilBindImage(ImgId);


	// Override the reading functions.
	ilSetRead(iOpenRead, iCloseRead, iEof, iGetc, iRead, iReadSeek, iReadTell);
	ilSetWrite(iOpenWrite, iCloseWrite, iPutc, iWriteSeek, iWriteTell, iWrite);


	// Loads the image specified by File into the image named by ImgId.
	if (!ilLoadImage(argv[1])) {
		printf("Could not open file...exiting.\n");
		return 3;
	}

	// Display the image's dimensions to the end user.
	printf("Width: %d  Height: %d  Depth: %d  Bpp: %d\n", ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_DEPTH), ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL));

	// Enable this to let us overwrite the destination file if it already exists.
	ilEnable(IL_FILE_OVERWRITE);

	// If argv[2] is present, we save to this filename, else we save to test.tga.
	if (argc > 2)
		ilSaveImage(argv[2]);
	else
		ilSaveImage("test.tga");


	// Reset the reading / writing functions when we're done loading specially.
	//  This isn't required here, since we're exiting, but here's how it's done:
	ilResetRead();
	ilResetWrite();


	// We're done with the image, so let's delete it.
	ilDeleteImages(1, &ImgId);

	// Simple Error detection loop that displays the Error to the user in a human-readable form.
	while ((Error = ilGetError())) {
		PRINT_ERROR_MACRO;}

	return 0;
}
