#include <IL/IL.h>
#include <IL/ILU.h>
#include <IL/ILUT.h>
#include <mathlink.h>

int main(int argc, char *argv[])
{
	int retval;
	int image;

	ilInit();
	iluInit();
	ilutInit();

	ilGenImages(1, &image);
	ilBindImage(image);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	retval = MLMain(argc, argv);

	ilDeleteImages(1, &image);

	ilShutDown();  // Only need to shut down DevIL, not ILU or ILUT

	return retval;
}

/*int f(int x, int y)
{
	return x+y;
}


void bits(int i)
{
	int a[32], k;
	for(k=0; k<32; k++) {
		a[k] = i%2;
		i >>= 1;
		if (i==0) break;
	}

	if (k<32) k++;
	MLPutInteger32List(stdlink, a, k);
	return;
}*/

int mlLoad(char *Filename)
{
	int ret = ilLoadImage(Filename);
	if (ilGetInteger(IL_ORIGIN_MODE) != IL_ORIGIN_UPPER_LEFT)
		iluFlipImage();
	ilConvertImage(IL_RGBA, IL_DOUBLE);
	return ret;
}

int mlWidth(void)
{
	return ilGetInteger(IL_IMAGE_WIDTH);
}

int mlHeight(void)
{
	return ilGetInteger(IL_IMAGE_HEIGHT);
}

void mlData(void)
{
	/*int		x, y, c;
	int		*Data;
	ILubyte	*OrigData = ilGetData();

	Data = malloc(ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 4 * sizeof(int));
	for (y = 0; y < ilGetInteger(IL_IMAGE_HEIGHT); y++) {
		for (x = 0; x < ilGetInteger(IL_IMAGE_WIDTH); x++) {
			for (c = 0; c < 4; c++) {
				Data[y * ilGetInteger(IL_IMAGE_WIDTH) * 4 + x * 4 + c] = OrigData[y * ilGetInteger(IL_IMAGE_WIDTH) * 4 + x * 4 + c];
			}
		}
	}

	MLPutInteger32List(stdlink, Data, ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 4);

	free(Data);*/

	MLPutReal64List(stdlink, (ILdouble*)ilGetData(), ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 4);

	return;
}

