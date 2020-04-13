//
//
// UniquE's RAR File Library test
//
// Just a quick test I threw together to see how well URar and DevIL coexist.
//  For more info on URar, go to http://www.mountpoint.ch/unique/project/urarfilelib
//
//


// Required include files.
#include <il/il.h>
#include <il/ilu.h>
#include "urarlib.h"


int main(int argc, char **argv)
{
	ILuint	ImgId;
	ILenum	Error;
	char	*Data;
	long	Size;

	// We use the filename specified in the first argument of the command-line.
	if (argc < 3) {
		printf("Please specify a .rar file and file inside to open.\n");
		return 1;
	}

	// Check if the shared lib's version matches the executable's version.
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION) {
		printf("DevIL version is different...exiting!\n");
		return 2;
	}

	// Initialize DevIL.
	ilInit();
	// Generate the main image name to use.
	ilGenImages(1, &ImgId);
	// Bind this image name.
	ilBindImage(ImgId);

	if (!urarlib_get(&Data, &Size, argv[2], argv[1], "none")) {
		printf("Error loading .rar file.\n");
		return 3;
	} 

	// Loads the image specified by File into the image named by ImgId.
	ilLoadL(IL_TGA, Data, Size);

	// Display the image's dimensions to the end user.
	printf("Width: %d  Height: %d  Depth: %d  Bpp: %d\n", ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_DEPTH), ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL));

	// Enable this to let us overwrite the destination file if it already exists.
	ilEnable(IL_FILE_OVERWRITE);

	// If argv[2] is present, we save to this filename, else we save to test.tga.
	if (argc > 2)
		ilSaveImage(argv[3]);
	else
		ilSaveImage("test.tga");

	// We're done with the image, so let's delete it.
	ilDeleteImages(1, &ImgId);

	// Simple Error detection loop that displays the Error to the user in a human-readable form.
	while ((Error = ilGetError())) {
		printf("Error: %s\n", iluErrorString(Error));
	}

	return 0;
}
