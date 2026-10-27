#include "CoreSuite.h"
#include "ExampleTest.h"
#include <memory>


CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ExampleTest,
                                       CppUnitTest::coreSuiteName() );


ExampleTest::ExampleTest()
{
}


ExampleTest::~ExampleTest()
{
}


void 
ExampleTest::testDefaultConstructor()
{
	CPPUNIT_ASSERT_MESSAGE("Asdsad",0);
	CPPUNIT_ASSERT( 0 );
}


