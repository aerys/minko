#include <IL/il.h>
#include <string.h>
#include <malloc.h>

// NOTE!
//  The biggest problem about using DevIL from Fortran is the
//  fact that Fortran does arrays in column-major format.
//  This wrapper does no conversions between the two!
//
//  We are using DevIL return values as the first parameter.
//


// From http://www.ibiblio.org/pub/languages/fortran/ch1-11.html
//  This is how Fortran passes strings.
struct descriptor						/* VMS fixed length string    */
{										/* descriptor used in FORTRAN */
	unsigned short  length;
	unsigned char   data_type,			/* = 14      */
					dsc_class;			/* = 1       */
	char            *string_ptr;
}; 


void ilFActiveImage_(int *RetVal, int *Number)
{
	*RetVal = ilActiveImage((unsigned int)*Number);
	return;
}

void ilFActiveLayer_(int *RetVal, int *Number)
{
	*RetVal = ilActiveImage((unsigned int)*Number);
	return;
}

void ilFActiveMipmap_(int *RetVal, int *Number)
{
	*RetVal = ilActiveImage((unsigned int)*Number);
	return;
}

void ilFApplyPal_(int *RetVal, struct descriptor *Desc)
{
	char *Filename = malloc(Desc->length + 1);
	strncpy(Filename, Desc->string_ptr, Desc->length);
	Filename[Desc->length] = 0;  // Should really be NULL...getting a warning, though.
	*RetVal = ilApplyPal(Filename);

	free(Filename);
	return;
}

void ilFBindImage_(int *Image)
{
	ilBindImage((unsigned int)*Image);
	return;
}

void ilFBlit_(int *RetVal, int *Source, int *DestX, int *DestY, int *DestZ, int *SrcX, int *SrcY, int *SrcZ, int *Width, int *Height, int *Depth)
{
	*RetVal = ilBlit(*Source, *DestX, *DestY, *DestZ, *SrcX, *SrcY, *SrcZ, *Width, *Height, *Depth);
	return;
}

void ilFClearColour_(float *Red, float *Green, float *Blue, float *Alpha)
{
	ilClearColour(*Red, *Green, *Blue, *Alpha);
	return;
}

void ilFClearImage_(int *RetVal)
{
	*RetVal = ilClearImage();
	return;
}

void ilFCloneCurImage_(int *RetVal)
{
	*RetVal = ilCloneCurImage();
	return;
}

void ilFCompressFunc_(int *RetVal, int *Mode)
{
	*RetVal = ilCompressFunc(*Mode);
	return;
}

void ilFConvertImage_(int *RetVal, int *DestFormat, int *DestType)
{
	*RetVal = ilConvertImage(*DestFormat, *DestType);
	return;
}

void ilFConvertPal_(int *RetVal, int *DestFormat)
{
	*RetVal = ilConvertPal(*DestFormat);
}

void ilFCopyImage_(int *RetVal, int *Src)
{
	*RetVal = ilCopyImage(*Src);
	return;
}

void ilFCopyPixels_(int *RetVal, int *XOff, int *YOff, int *ZOff, int *Width, int *Height, int *Depth, int *Format, int *Type, void *Data)
{
	*RetVal = ilCopyPixels(*XOff, *YOff, *ZOff, *Width, *Height, *Depth, *Format, *Type, Data);
	return;
}

void ilFCreateSubImage_(int *RetVal, int *Type, int *Num)
{
	*RetVal = ilCreateSubImage(*Type, *Num);
	return;
}

void ilFDefaultImage_(int *RetVal)
{
	*RetVal = ilDefaultImage();
	return;
}

void ilFDeleteImage_(int *Num)
{
	ilDeleteImage(*Num);
	return;
}

// Not sure how to do the arrays properly.
//void ilFDeleteImages_(int *Num, int **Images)
//{
//	ilDeleteImages(*Num, *Images);
//}

void ilFDisable_(int *RetVal, int *Mode)
{
	*RetVal = ilDisable(*Mode);
	return;
}

void ilFEnable_(int *RetVal, int *Mode)
{
	*RetVal = ilEnable(*Mode);
	return;
}

void ilFFormatFunc_(int *RetVal, int *Mode)
{
	*RetVal = ilFormatFunc(*Mode);
	return;
}

// Not sure how to do the arrays properly.
//void ilFGenImages_(int *Num, int **Images)
//{
//	ilGenImages(*Num, *Images);
//}

void ilFGenImage_(int *RetVal)
{
	*RetVal = ilGenImage();
	return;
}

//void ilFGetAlpha_(

void ilFGetBoolean_(int *RetVal, int *Mode)
{
	*RetVal = ilGetBoolean(*Mode);
	return;
}

void ilFGetBooleanv_(int *Mode, int *Param)
{
	ilGetBooleanv(*Mode, Param);
	return;
}

void ilFGetData_(char *RetVal)
{
	RetVal = ilGetData();
	return;
}

// ilGetDXTCData

void ilFGetError_(int *RetVal)
{
	*RetVal = ilGetError();
	return;
}

void ilFGetInteger_(int *RetVal, int *Mode)
{
	*RetVal = ilGetInteger(*Mode);
	return;
}

void ilFGetIntegerv_(int *Mode, int *Param)
{
	ilGetIntegerv(*Mode, Param);
	return;
}

void ilFGetLumpPos_(int *RetVal)
{
	*RetVal = ilGetLumpPos();
	return;
}

void ilFGetPalette_(char *RetVal)
{
	RetVal = ilGetPalette();
	return;
}

//ilGetString

void ilFInit_()
{
	ilInit();
	return;
}

void ilFShutDown_()
{
	ilShutDown();
	return;
}

void ilFLoadImage_(int *RetVal, struct descriptor *Desc)
{
	char *Filename = malloc(Desc->length + 1);
	strncpy(Filename, Desc->string_ptr, Desc->length);
	Filename[Desc->length] = 0;  // Should really be NULL...getting a warning, though.
	*RetVal = ilLoadImage(Filename);

	free(Filename);
	return;
}

void ilFSave_(int *RetVal, int *Type, struct descriptor *Desc)
{
	char *Filename = malloc(Desc->length + 1);
	strncpy(Filename, Desc->string_ptr, Desc->length);
	Filename[Desc->length] = 0;  // Should really be NULL...getting a warning, though.
	*RetVal = ilSave(*Type, Filename);

	free(Filename);
	return;
}

void ilFSaveImage_(int *RetVal, struct descriptor *Desc)
{
	char *Filename = malloc(Desc->length + 1);
	strncpy(Filename, Desc->string_ptr, Desc->length);
	Filename[Desc->length] = 0;  // Should really be NULL...getting a warning, though.
	*RetVal = ilSaveImage(Filename);

	free(Filename);
	return;
}