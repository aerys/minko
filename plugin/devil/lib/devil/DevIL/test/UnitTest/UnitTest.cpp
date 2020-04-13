// UnitTest.cpp : Defines the entry point for the console application.
//

#include <IL/il.h>
#include <IL/ilu.h>
#include <assert.h>
#include <IL/ilut.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

void TestGenerate()
{

  ILuint lImages[5];
  ilInit();
  iluInit();

  ilGenImages(5, lImages);
  ilBindImage(lImages[3]);
  ilLoadImage("D:\\rgbrle4.bmp");


  ilBindImage(lImages[4]);
  ilLoadImage("D:\\rgbrle4.bmp");
  iluMirror();


  ilBindImage(lImages[0]);
  ilSaveImage("D:\\Image0.bmp");

  ilBindImage(lImages[1]);
  ilSaveImage("D:\\Image1.bmp");

  ilBindImage(lImages[2]);
  ilSaveImage("D:\\Image2.bmp");

  ilBindImage(lImages[3]);
  ilSaveImage("D:\\Image3.bmp");

  ilBindImage(lImages[4]);
  ilSaveImage("D:\\Image4.bmp");

}

void TestClipboard()
{
  ilInit();
  iluInit();

  FILE * lBuffer = fopen("D:\\result.bmp", "wb");
//  ilutGetWinClipboard();
  ilSaveF(IL_BMP,lBuffer);
  assert(ilGetError() == IL_NO_ERROR);
  fclose(lBuffer);
}


int main(int argc, char* argv[])
{
  // Get the top level suite from the registry
  CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

  // Adds the test to the list of test to run
  CPPUNIT_NS::TextUi::TestRunner runner;
  runner.addTest( suite );

  // Change the default outputter to a compiler error format outputter
  runner.setOutputter( new CPPUNIT_NS::CompilerOutputter( &runner.result(),
                                                       std::cerr ) );
  // Run the test.
  bool wasSucessful = runner.run();

  // Return error code 1 if the one of test failed.
  return wasSucessful ? 0 : 1;  
}
