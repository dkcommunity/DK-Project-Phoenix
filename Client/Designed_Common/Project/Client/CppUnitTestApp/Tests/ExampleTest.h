#ifndef EXAMPLETEST_H
#define EXAMPLETEST_H

#include <cppunit/extensions/HelperMacros.h>


class ExampleTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( ExampleTest );
  CPPUNIT_TEST( testDefaultConstructor );
  CPPUNIT_TEST_SUITE_END();

public:
  ExampleTest();
  virtual ~ExampleTest();

  void testDefaultConstructor();

private:
};



#endif  // EXAMPLETEST_H
