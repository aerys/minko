//-----------------------------------------------------------------------------
//
// ImageLib Source Example
// Copyright (C) 2000-2001 by Denton Woods
// Last modified:  09/07/2001 <--Y2K Compliant! =]
//
// Filename: examples/register read/register read.c
//
// Description: An example of creating your own loading routine
//				  to use with DevIL -- uses a hypothetical format.
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

ILboolean ILAPIENTRY LoadFunction(const char *FileName)
{
	ILuint Width, Height;
	FILE *f = fopen(FileName, "rb");

	if (f == NULL)
		return IL_FALSE;
	fread(&Width,  1, 4, f);
	fread(&Height, 1, 4, f);

	if (!ilTexImage(Width, Height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL)) {
		fclose(f);
		return IL_FALSE;
	}

	// Set the origin via the register functions.
	ilRegisterOrigin(IL_ORIGIN_UPPER_LEFT);

	fread(ilGetData(), 1, Width * Height * 3, f);
	fclose(f);

	return IL_TRUE;
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


	// Set the loading function here.
	ilRegisterLoad("xxx", LoadFunction);


	// Generate the main image name to use.
	ilGenImages(1, &ImgId);
	// Bind this image name.
	ilBindImage(ImgId);
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


	// Remove the loading function when we're done using it or want to change it.
	//  This isn't required here, since we're exiting, but here's how it's done:
	ilRemoveLoad("xxx");


	// We're done with the image, so let's delete it.
	ilDeleteImages(1, &ImgId);

	// Simple Error detection loop that displays the Error to the user in a human-readable form.
	while ((Error = ilGetError())) {
		PRINT_ERROR_MACRO;
	}

	return 0;
}
