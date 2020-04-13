//-----------------------------------------------------------------------------
//
// DevIL Stress Test
// Copyright (C) 2000-2002 by Denton Woods
// Last modified: 06/27/2002 <--Y2K Compliant! =]
//
// Filename: stress.c
//
// Description: Stress tests DevIL.
//
//-----------------------------------------------------------------------------

#ifdef _DEBUG
#define IL_DEBUG
#endif

// Required include files.
#include <IL/il.h>
#include <IL/ilu.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <list>
#include <string>
#include <direct.h>
#include <conio.h>
using namespace std;

#include "mmgr.h"


bool ParseDirs(const string &_Dir, bool Recursion);
bool IsDir(WIN32_FIND_DATA *_Data);
bool StressTest(void);

ILuint	ImgId;
ILuint	ImgId2[20000];
ILuint	NumImages = 0;


ILvoid* ILAPIENTRY stressalloc(ILuint size)
{
	return new unsigned char[size];
}

ILvoid ILAPIENTRY stressfree(ILvoid *ptr)
{
	if (ptr)
		delete []ptr;
}

void m_breakOnAllocation(unsigned int count);

int main(int argc, char **argv)
{
	// Initialize DevIL.
	ilSetMemory(stressalloc, stressfree);
	ilInit();

	// Generate the main image name to use.
	ilGenImages(20000, ImgId2);
	ilGenImages(1, &ImgId);

	// Bind this image name.
	ilBindImage(ImgId);

	ParseDirs(".", true);

	printf("%d images stress tested.\n", NumImages);

	ilDeleteImages(1, &ImgId);
	ilDeleteImages(20000, ImgId2);

	ilShutDown();

	return 0;

}


// Calling functions do not check ParseDirs's return value - used in the recursion.
bool ParseDirs(const string &_Dir, bool Recursion)
{
	HANDLE			Search;
	ILenum			Error;
	WIN32_FIND_DATA	FindData;

	//chdir(_Dir.c_str());
	Search = FindFirstFile("*.*", &FindData);

	do {
		if (!strcmp(FindData.cFileName, ".") || !strcmp(FindData.cFileName, ".."))
			continue;
		if (IsDir(&FindData) && Recursion) {
			chdir(FindData.cFileName);
			string NewDir = _Dir + string("\\");
			NewDir += FindData.cFileName;
			if (ParseDirs(NewDir, Recursion) == false) {
				FindClose(Search);
				return false;
			}
			chdir("..");
			continue;
		}
		else if (IsDir(&FindData)) {
			continue;
		}

		if (kbhit()) {
			FindClose(Search);
			return false;
		}

		if (ilLoadImage(FindData.cFileName)) {
			// Perform all manipulations here.

			printf("File: %s\n Width: %d  Height: %d  Depth: %d  Bpp: %d\n",
					FindData.cFileName,
					ilGetInteger(IL_IMAGE_WIDTH),
					ilGetInteger(IL_IMAGE_HEIGHT),
					ilGetInteger(IL_IMAGE_DEPTH),
					ilGetInteger(IL_IMAGE_BITS_PER_PIXEL));
			NumImages++;

			if (ilGetInteger(IL_IMAGE_SIZE_OF_DATA) < 4000000) {
				if (!StressTest()) {
					FindClose(Search);
					return false;
				}
			}

			while ((Error = ilGetError())) {
				printf("Error: %s\n", iluErrorString(Error));
			}
			printf("\n\n");
		}
		else {
			while ((Error = ilGetError()));
		}
	} while (FindNextFile(Search, &FindData));

	FindClose(Search);
	return true;
}


// Is the file actually a directory?
bool IsDir(WIN32_FIND_DATA *_Data)
{
	if (_Data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	return false;
}


bool StressTest()
{
	ILuint Test;

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	ilConvertImage(IL_BGR, IL_UNSIGNED_BYTE);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	ilConvertImage(IL_RGBA, IL_FLOAT);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	iluScale(100, 100, 2);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	iluSharpen(2.0f, 2);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	iluRotate(rand()%3600 / 10.0f);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	iluEdgeDetectP();
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	iluEqualize();
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	iluNoisify(10.0f);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".");

	if (kbhit()) return false;
	Test = ilCloneCurImage();
	ilBindImage(Test);
	iluPixelize(rand()%20);
	ilBindImage(ImgId);
	ilDeleteImages(1, &Test);
	printf(".\n");

	return true;
}
