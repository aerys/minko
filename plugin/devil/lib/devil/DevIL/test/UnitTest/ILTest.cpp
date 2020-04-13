// MoneyTest.cpp

#include "ILTest.h"
#include <IL/il.h>

#define CHECK_NO_ERROR()   CPPUNIT_ASSERT(ilGetError() == IL_NO_ERROR)

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ILTest );


#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <iostream>

void ILTest::setUp()
{
  ilInit();
}


void ILTest::tearDown()
{
  const ILenum lResult = ilGetError();

  while (ilGetError() != IL_NO_ERROR) {;}

  ilResetMemory();
  ilShutDown();
  CPPUNIT_ASSERT(ilGetError() == IL_NO_ERROR);


  CPPUNIT_ASSERT_MESSAGE("Received Error from ilGetError", lResult == IL_NO_ERROR);
}


void ILTest::TestilActiveImage()
{
  //Must call ilGenImages and ilBindImage for it to work
  ilActiveImage(3);
  CPPUNIT_ASSERT(ilGetError() == IL_ILLEGAL_OPERATION);
  CPPUNIT_ASSERT(ilGetError() == IL_NO_ERROR);

  ILuint MainImage = 0;
  ilGenImages(1, &MainImage);
  ilBindImage(MainImage);

  //Must have several images in the file
  ilLoadImage(".\\Data\\Cardinal.gif");

  //Number of images must be > 0
  const unsigned int lNumImages = ilGetInteger(IL_NUM_IMAGES);
  CPPUNIT_ASSERT(lNumImages > 0);
  
  //Never an error
  ilActiveImage(0);
  CPPUNIT_ASSERT(ilGetError() == IL_NO_ERROR);

  //Selects the second picture
  ilActiveImage(1);
  CPPUNIT_ASSERT(ilGetError() == IL_NO_ERROR);

  //It does not have that many images so error is reported
  ilActiveImage(2045);
  CPPUNIT_ASSERT(ilGetError() == IL_ILLEGAL_OPERATION);

  //Make a picture of each image.
  char lBuffer[4];
  std::string lFilename = ".\\results\\Cardinal";
  std::string lFinalFilename;
  for (unsigned int i = 0; i < lNumImages; ++i)
  {
	ilBindImage(MainImage);
	ilActiveImage(i);
	itoa (i,lBuffer,10);
	lFinalFilename = lFilename + lBuffer + ".bmp";
	FILE * lBuffer = fopen(lFinalFilename.c_str() , "wb");
	CPPUNIT_ASSERT(lBuffer != NULL);
	ilSaveF(IL_BMP,lBuffer);
	fclose(lBuffer);
  }
}

void ILTest::TestilBindImage()
{
  ILuint MainImage = 0;
  ilBindImage(MainImage);
//  CPPUNIT_ASSERT(ilGetError() == IL_ILLEGAL_OPERATION);

  ilGenImages(1, &MainImage);
  ilBindImage(MainImage);
  CPPUNIT_ASSERT(ilGetError() == IL_NO_ERROR);

  ilDeleteImages(1, &MainImage);
  CPPUNIT_ASSERT(ilGetError() == IL_NO_ERROR);

  ilBindImage(MainImage);
//  CPPUNIT_ASSERT(ilGetError() == IL_ILLEGAL_OPERATION);
}


void ILTest::TestilClearColour()
{
  ilClearColour(1.0, 0.0, 0.0, 10.0);
  CPPUNIT_ASSERT(ilGetError() == IL_NO_ERROR);
}

void ILTest::TestilClearImage()
{
  ilLoadImage(".\\Data\\Logo.png");
  ilClearColour(1.0, 1.0, 0.0, 0.0);
  const ILboolean lResult = ilClearImage();
  CPPUNIT_ASSERT(lResult == IL_TRUE);
  FILE * lBuffer = fopen(".\\results\\ilClearImage.bmp", "wb");
  ilSaveF(IL_BMP,lBuffer);
  fclose(lBuffer);

}

void ILTest::TestilCloneImage()
{
  ILuint MainImage[1];
  ilGenImages(1, MainImage);
  ilBindImage(MainImage[0]);
  CHECK_NO_ERROR();

  ilLoadImage(".\\Data\\Logo.png");
  CHECK_NO_ERROR();

  const ILuint lIndex = ilCloneCurImage();
  CHECK_NO_ERROR();
  CPPUNIT_ASSERT(lIndex != MainImage[0]);

  ilBindImage(lIndex);
  CHECK_NO_ERROR();

  ilClearColour(1.0, 0.0, 0.0, 0.0);
  ilClearImage();

  FILE * lBuffer = fopen(".\\results\\ilCloneImage_Cloned.bmp", "wb");
  ilSaveF(IL_BMP,lBuffer);
  fclose(lBuffer);


  ilBindImage(MainImage[0]);
  lBuffer = fopen(".\\results\\ilCloneImage_Original.bmp", "wb");
  ilSaveF(IL_BMP,lBuffer);
  fclose(lBuffer);
}

void ILTest::TestilConvertImage()
{
  ILuint MainImage = 0;

  ilGenImages(1, &MainImage);
  ilBindImage(MainImage);
  CHECK_NO_ERROR();

  ilLoadImage(".\\Data\\Logo.png");

  ILenum lDestFormat[6] =  {
							               IL_RGB, 
							               IL_RGBA, 
							               IL_BGR,
							               IL_BGRA, 
							               IL_LUMINANCE, 
							               IL_COLOUR_INDEX,
                           };
  
  ILenum lDestType[8] = {
                          IL_BYTE, 
                          IL_UNSIGNED_BYTE,
                          IL_SHORT, 
                          IL_UNSIGNED_SHORT, 
                          IL_INT, 
                          IL_UNSIGNED_INT, 
                          IL_FLOAT, 
                          IL_DOUBLE, 
                        };
   
   for (unsigned int i=0; i < 5; ++i)
     for (unsigned int j=0; j < 8; ++j)
     {
       ilConvertImage(lDestFormat[i], lDestType[j]);
       CHECK_NO_ERROR();
     }   

  ilConvertImage(lDestFormat[5], lDestType[0]);
  ilConvertImage(lDestFormat[5], lDestType[1]);
  CHECK_NO_ERROR();

  for (unsigned int j=2; j < 8; ++j)
  {
   ilConvertImage(lDestFormat[5], lDestType[j]);
   CPPUNIT_ASSERT(ilGetError() == IL_INVALID_CONVERSION);
  }      
}

void ILTest::TestilSaveF()
{
  FILE * lBuffer = fopen(".\\results\\result.bmp", "wb");
  ilLoadImage(".\\Data\\Logo.png");
  ilSaveF(IL_BMP,lBuffer);
  fclose(lBuffer);
}