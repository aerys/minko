// TestIL.cpp : Defines the entry point for the console application.

//



#define IL_NO_GIF  // Just for right now...



#include <stdio.h>

#include <il/il.h>

#include <il/ilu.h>

#include <il/ilut.h>



void NewTest(void);

void MemTest(void);

void RegTest(void);

void DoomTest(void);

void IcoCurTest(void);

void BitTest(void);

// In doom.cpp

FILE *TestOpenWad(char *WadName);

GLboolean TestLocateLump(char *LumpName);

GLvoid TestCloseWad();





int main(int argc, char* argv[])

{

	ILimage *Image, *Flipped, *SubImage, *JpegImage, *BmpImage, *PcxImage, *PpmImage, *PgmImage;

	ILimage *PngImage;



	ilEnable(IL_ORIGIN_SET);

	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);



	ilEnable(IL_FILE_OVERWRITE);  // Will now overwrite existing files



	MemTest();

	NewTest();

	RegTest();

	DoomTest();

	IcoCurTest();

	BitTest();



	Image = ilLoadTarga("default1.tga");



	ilSaveCHeader("header.h", Image, "Default");





	ilPushAttrib(IL_ALL_ATTRIB_BITS);

	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

	Flipped = ilLoadTarga("default1.tga");

	ilSaveTarga("default2.tga", Flipped, NULL);

	ilCloseImage(Flipped);

	ilPopAttrib();





	SubImage = ilTexSubImage2d(Image, 0, 0, 16, 16);

	ilSaveTarga("default3.tga", SubImage, NULL);





	ilCloseImage(SubImage);





	// Fux0red

/*	RotImage = iluRotateImage(Image, 45, 169);

	ilSaveTarga("default4.tga", RotImage, NULL);

	ilCloseImage(RotImage);*/



	JpegImage = ilLoadJpeg("default1.jpg");

	ilSaveTarga("jpeg.tga", JpegImage, NULL);

	ilCloseImage(JpegImage);



	// Need to convert in here from 8-bit to 24-bit!

	//ilSaveJpeg("default5.jpg", Image, 99);



	PcxImage = ilLoadPcx("default1.pcx");

	ilSaveTarga("pcx.tga", PcxImage, NULL);

	ilCloseImage(PcxImage);



	

	BmpImage = ilLoadBitmap("default1.bmp");

	ilSaveTarga("bitmap.tga", BmpImage, NULL);

	ilCloseImage(BmpImage);



	PpmImage = ilLoadPpm("default1.ppm");

	ilSaveTarga("ppm1.tga", PpmImage, NULL);

	ilCloseImage(PpmImage);



	PpmImage = ilLoadPpm("default2.ppm");

	ilSaveTarga("ppm2.tga", PpmImage, NULL);

	ilCloseImage(PpmImage);



	PgmImage = ilLoadPgm("default1.pgm");

	ilSaveTarga("pgm1.tga", PgmImage, NULL);

	ilCloseImage(PgmImage);



	PgmImage = ilLoadPgm("default2.pgm");

	ilSaveTarga("pgm2.tga", PgmImage, NULL);

	ilCloseImage(PgmImage);





	PngImage = ilLoadPng("default1.png");

	ilSaveTarga("png1.tga", PngImage, NULL);

	ilCloseImage(PngImage);



	PngImage = ilLoadPng("test1.png");

	ilSaveTarga("testpng1.tga", PngImage, NULL);

	ilCloseImage(PngImage);



	PngImage = ilLoadPng("test2.png");

	ilSaveTarga("testpng2.tga", PngImage, NULL);

	ilCloseImage(PngImage);



	PngImage = ilLoadPng("test256.png");

	ilSaveTarga("testpng256.tga", PngImage, NULL);

	ilCloseImage(PngImage);



	PngImage = ilLoadPng("testpnggrey.png");

	ilSaveTarga("testpnggrey.tga", PngImage, NULL);

	ilCloseImage(PngImage);









	GLenum Error;

	while (Error = ilGetError()) {

		printf("Error: %s\n", iluErrorString(Error));

	}







	ilCloseImage(Image);

	



/*	Image = ilLoadGif("default1.gif");

	printf("\nGif\n---");

	printf("Width: %d  Height: %d  Bpp: %d\n", Image->Width, Image->Height, Image->Bpp);

	if (Image->Pal)

		printf("Pal: %d %d %d   %d %d %d   %d %d %d\n", Image->Pal[0], Image->Pal[1], Image->Pal[2],

		Image->Pal[3], Image->Pal[4], Image->Pal[5], Image->Pal[6], Image->Pal[7], Image->Pal[8]);

	printf("Data: %d %d   %d", Image->Data[0], Image->Data[1], Image->Data[8]);



	ilCloseImage(Image);*/







	return 0;

}





void NewTest()

{

	ILimage *Image;



	FILE *f = fopen("in\\rgb32.tga", "rb");

	Image = ilLoadTargaF(f);

	ilSaveTarga("out/rgb32.tga", Image, NULL);

	ilCloseImage(Image);

	fclose(f);



	ILimage *Scaled;

	Image = ilLoadTarga("in/rgb32rle.tga");

	Scaled = iluScaleImage(Image, 100, 100);

	ilSaveTarga("out/scaled small.tga", Scaled, NULL);

	ilCloseImage(Scaled);

	Scaled = iluScaleImage(Image, 400, 400);

	ilSaveTarga("out/scaled large.tga", Scaled, NULL);

	ilCloseImage(Scaled);

	ilSaveTarga("out/rgb32rle.tga", Image, NULL);

	ilCloseImage(Image);



	Image = ilLoadTarga("in/rgb24.tga");

	ilSaveTarga("out/rgb24.tga", Image, NULL);

	ilCloseImage(Image);



	Image = ilLoadTarga("in/rgb24rle.tga");

	ilSaveTarga("out/rgb24rle.tga", Image, NULL);

	ilCloseImage(Image);





	//Image = ilLoadTarga("in/rgb8.tga");

	//Image = ilLoadTarga("default1.tga");

	Image = ilLoadTarga("in/hardrle.tga");

	GLubyte *Verbatim = Image->Data, *Rle = (GLubyte*)malloc(Image->SizeOfData * 2);

	GLuint RleLen = 0, LineLen, OldSOD;

	for (GLuint i = 0; i < Image->SizeOfData; i += Image->Bps) {

		ilRleCompress(Image->Data + i, Image->Width, Image->Bpp, Rle + RleLen, &LineLen, IL_TGACOMP);

		RleLen += LineLen;

	}

	//ilRleCompress(Image->Data, Image->SizeOfData, 1, Rle, &LineLen, IL_TGACOMP);

	Image->Data = Rle;

	OldSOD = Image->SizeOfData;

	Image->SizeOfData = RleLen;

	ilSaveTarga("out/rlecomp.tga", Image, NULL);

	free(Image->Data);

	

	Image->Data = Verbatim;

	Image->SizeOfData = OldSOD;





	ilSaveTarga("out/rgb8.tga", Image, NULL);

	ilCloseImage(Image);



	Image = ilLoadTarga("in/grey.tga");

	ilSaveTarga("out/grey.tga", Image, NULL);

	ilCloseImage(Image);



	Image = ilLoadJpeg("default1.jpg");

	ilSaveJpeg("out/rgb24.jpg", Image, 99);

	ilSaveTarga("out/rgb24jpg.tga", Image, NULL);  // lossless, just to make sure of what it loaded

	ilCloseImage(Image);



	Image = ilCreateDefaultTex();

	ilSaveTarga("out/default.tga", Image, NULL);

	ilCloseImage(Image);



	ilPushAttrib(IL_ALL_ATTRIB_BITS);

	ilDisable(IL_ORIGIN_SET);



	Image = ilLoadPcx("in/rgb8.pcx");

	ilSaveTarga("out/rgb8pcx.tga", Image, NULL);

	ilCloseImage(Image);



	Image = ilLoadPcx("in/rgb24.pcx");

	ilSaveTarga("out/rgb24pcx.tga", Image, NULL);

	ilCloseImage(Image);



	Image = ilLoadSgi("in/jp_poster.tiny.rgb");

	ilSaveTarga("out/jp_poster.tga", Image, NULL);

	ilCloseImage(Image);



	ilPopAttrib();





	return;

}





void MemTest()

{

	ILimage *Image;

	char *Data;

	long FileLen;

	FILE *f = fopen("default1.tga", "rb");



	if (f == NULL)

		return;



	fseek(f, 0, SEEK_END);

	FileLen = ftell(f);

	Data = new char[FileLen];

	if (Data == NULL) {

		fclose(f);

		return;

	}

	fseek(f, 0, SEEK_SET);

	fread(Data, 1, FileLen, f);

	fclose(f);



	Image = ilLoadTargaL(Data, FileLen);

	ilSaveTarga("out/memtga.tga", Image, NULL);



	return;

}





ILimage* ILAPIENTRY LoadTest(char *FileName)

{

	printf("Load Test Function Ran\n");

	return NULL;

}





ILimage* ILAPIENTRY SaveTest(char *FileName, void *Data)

{

	printf("Save Test Function Ran\n");

	return NULL;

}







void RegTest()

{

	IL_LOADPROC LoadProc;

	IL_SAVEPROC SaveProc;



	ilRegisterFormat("xxx", LoadTest, SaveTest);



	LoadProc = (IL_LOADPROC)ilGetLoadProc("xxx");

	SaveProc = (IL_SAVEPROC)ilGetSaveProc("xxx");



	/*LoadProc("test.xxx");

	SaveProc("test.xxx", NULL);*/



	ilLoadImage("test.xxx");

}





void DoomTest()

{

	FILE *WadFile;

	ILimage *Face, *Wall, *Floor;

	GLboolean Found;



	WadFile = TestOpenWad("in/doomtest.wad");

	if (WadFile == NULL)

		return;



	Found = TestLocateLump("STFKILL0");

	Face = ilLoadDoomF(WadFile);

	if (Face == NULL) {

		fclose(WadFile);

		return;

	}



	if (ilLoadJascPal("in/doomtest.pal", &Face->Pal))

		ilSetDoomPal(Face->Pal.Palette);



	Found = TestLocateLump("WALL78_1");

	Wall = ilLoadDoomF(WadFile);

	if (Wall == NULL) {

		fclose(WadFile);

		return;

	}



	Found = TestLocateLump("FLOOR5_2");

	Floor = ilLoadDoomFlatF(WadFile);

	if (Floor == NULL) {

		fclose(WadFile);

		return;

	}



	ilSaveTarga("out/doomfloor.tga", Floor, NULL);

	ilSaveTarga("out/doomface.tga", Face, NULL);

	

	GLubyte a = 247;

	ilOverlayImage(Wall, Face, 52, 49, 0, &a);

	ilSaveTarga("out/doommixed.tga", Wall, NULL);



	ilCloseImage(Face);

	ilCloseImage(Wall);

	ilCloseImage(Floor);



	TestCloseWad();

	

	fclose(WadFile);



	return;

}





void IcoCurTest(void)

{

	ILimage* Image;



	Image = ilLoadIcon("in/icontest.ico");

	ilSaveTarga("out/icontest.tga", Image, NULL);



	FILE *f = fopen("a.txt", "wt");

	for (int i = 0; i < 7000; i++) {

		fprintf(f, "%d\n", Image->Data[i]);

	}

	fclose(f);

	ilCloseImage(Image);

}





void BitTest(void)

{

	BITFILE	*File;

	GLubyte	Buff[32];



	File = bopen("in/bit test.bt", "rw");

	if (File == NULL)

		return;



	bread(Buff, 1, 32, File);





	bclose(File);

}