#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <vector>

void test() {
   std::cout << "Hello" << std::endl;
   int a = std::min(1, 2);
   std::vector<int> v;
   v.push_back(1);
   // Force usage of copy
   std::vector<int> v2;
   std::copy(v.begin(), v.end(), std::back_inserter(v2));
}
