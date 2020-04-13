//ILTest.h
#ifndef ILTEST_H
#define ILTEST_H

#include <cppunit/extensions/HelperMacros.h>

class ILTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( ILTest );
  CPPUNIT_TEST( TestilActiveImage );
  //CPPUNIT_TEST( TestilActiveLayer );
  //CPPUNIT_TEST( TestilActiveMipMap );
  //CPPUNIT_TEST( TestilApplyProfile );
  CPPUNIT_TEST( TestilBindImage );
  CPPUNIT_TEST( TestilClearColour );  
  CPPUNIT_TEST( TestilClearImage );  
  CPPUNIT_TEST( TestilCloneImage );  
  CPPUNIT_TEST( TestilConvertImage );  
  CPPUNIT_TEST( TestilSaveF );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void TestilActiveImage();
  void TestilBindImage();
  void TestilClearColour();
  void TestilClearImage();
  void TestilCloneImage();
  void TestilConvertImage();
  void TestilSaveF();

};

#endif  // ILTEST_H
