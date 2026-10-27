#pragma once

// BugTrap includes //////////////////////////////////////////////////////////////

// Include main BugTrap header.
#include "BugTrap/BugTrap.h"

// Link with one of BugTrap libraries.
#ifdef	__BUGTRAP
#ifdef _UNICODE
 #pragma comment(lib, "BugTrapU.lib")
#else
 #pragma comment(lib, "BugTrap.lib")
#endif
#endif //__BUGTRAP
// Enable Common Controls support

// 2008.8.22 ij-ch (warning LNK4044 없애기)
// 유니코드 일때만 윈도우XP스타일 컨트롤이 생성이 가능한것 같다. 지금 프로젝트가 유니코드가 아니고,
// manifestdependency가 옵션이 적용되지 않아서 워닝이 발생하므로, 아래코드가 적용되지 않도록 #ifdef _UNICODE를 걸어 워닝을 없앴다.
// 2005로 MFC를 생성하면 기본적으로 "stdafx.h"에 "#ifdef _UNICODE"까지 추가되는데 아래코드는 이것이 없었다.

#ifdef _UNICODE
#if defined _M_IX86
 #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
 #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
 #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
 #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
