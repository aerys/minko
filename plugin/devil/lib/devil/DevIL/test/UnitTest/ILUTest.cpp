// MoneyTest.cpp

#include "ILUTest.h"
#include <IL/il.h>
#include <IL/ilu.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ILUTest );


void ILUTest::setUp()
{
  ilInit();
  iluInit();
}


void ILUTest::tearDown()
{
  const ILenum lResult = ilGetError();

  while (ilGetError() != IL_NO_ERROR) {;}

  ilResetMemory();
  CPPUNIT_ASSERT(ilGetError() == IL_NO_ERROR);

  CPPUNIT_ASSERT_MESSAGE("Received Error from ilGetError", lResult == IL_NO_ERROR);
}


void ILUTest::LoadStandardImage()
{
  ilLoadImage(".\\Data\\Logo.png");
}

void ILUTest::TestiluSaturate4f()
{
  ILuint MainImage = 0;

  FILE * lBuffer = fopen(".\\results\\result.bmp", "wb");
  ilGenImages(1, &MainImage);
  ilBindImage(MainImage);
  LoadStandardImage();
  ilConvertImage(IL_RGB,IL_BYTE);
  iluSaturate4f(-1.0, 0.0, 0.0, -1.0);
  iluMirror();
  iluBlurAvg(3);
  ilSaveF(IL_BMP,lBuffer);
  fclose(lBuffer);
}

void ILUTest::TestiluReplaceColour()
{
  ILuint MainImage = 0;

  FILE * lBuffer = fopen(".\\results\\result.bmp", "wb");
  ilGenImages(1, &MainImage);
  ilBindImage(MainImage);
  LoadStandardImage();
  ilConvertImage(IL_RGB,IL_BYTE);
  iluReplaceColour(1.0, 0.0, 0.0, 0.0);
  ilSaveF(IL_BMP,lBuffer);
  fclose(lBuffer);
}