#include <cppunit/extensions/TestFactoryRegistry.h>
#include "CppUnitTestSuite.h"
#include "CoreSuite.h"


namespace CppUnitTest
{

CppUnit::Test *
suite()
{
  CppUnit::TestFactoryRegistry &registry = 
                      CppUnit::TestFactoryRegistry::getRegistry();

  registry.registerFactory( 
      &CppUnit::TestFactoryRegistry::getRegistry( coreSuiteName() ) );

  return registry.makeTest();
}


}  // namespace CppUnitTest
