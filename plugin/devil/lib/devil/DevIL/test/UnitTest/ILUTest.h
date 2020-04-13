//ILTest.h
#ifndef ILUTEST_H
#define ILUTEST_H

#include <cppunit/extensions/HelperMacros.h>

class ILUTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( ILUTest );
  CPPUNIT_TEST( TestiluSaturate4f );
  CPPUNIT_TEST( TestiluReplaceColour );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void TestiluSaturate4f();
  void TestiluReplaceColour();

private:
  void LoadStandardImage();
};

#endif  // ILUTEST_H
