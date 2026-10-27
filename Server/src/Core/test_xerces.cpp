
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUni.hpp>


using namespace xercesc;

int main() {
  try {
    XMLPlatformUtils::Initialize();
    const XMLCh *s = XMLUni::fgSAX2CoreNameSpaces;
    (void)s;
    std::cout << "Success" << std::endl;
    XMLPlatformUtils::Terminate();
  } catch (...) {
    return 1;
  }
  return 0;
}
