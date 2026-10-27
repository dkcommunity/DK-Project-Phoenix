
#include <stdio.h>

#if _MSC_VER >= 1900
extern "C" {
// Legacy stdio definitions for old libraries like Jpeg.lib
FILE _iob[3];
FILE *__cdecl __iob_func(void) {
  _iob[0] = *stdin;
  _iob[1] = *stdout;
  _iob[2] = *stderr;
  return _iob;
}
}
#endif
