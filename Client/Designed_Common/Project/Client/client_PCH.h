////////////////////////////////////////////////////////////////////////////////
//	created:	2004/12/22
//	file base:	client_pch.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

// FAZ 4 (2026-05-02): ContentsFilter.h EN BAŞA taşındı (Brief D Section 7 #3 +
// Brief E Section 8 önerisi). Sistem header'larından bile önce __TUNING_ITEM
// vb. macros tanımlı olur — header parse order'dan bağımsız. DESC sizeof
// asimetrisi (Client=248 vs Lib=252) bu değişikliğe duyarlı olabilir.
#include "ContentsFilter.h"

#pragma warning(disable : 4290)
#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4018)
#pragma warning(disable : 4786)
#pragma warning(disable : 4103)
#pragma warning(disable : 4049)

#pragma warning(error : 4067)

#pragma warning(push)

//---------------------------------------------------------
// System Headers First
//---------------------------------------------------------
#pragma pack(push, 8)
#include <DDraw.h>
#include <Digitalv.h>
#include <MMSystem.h>
#include <windows.h>

#pragma pack(pop)
#include <algorithm>
#include <assert.h>
#include <bitset>
#include <cstdarg>
#include <cstdio>
#include <deque>
#include <fcntl.h>
#include <fstream>
#include <io.h>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

//---------------------------------------------------------
// Project Headers
//---------------------------------------------------------
#include "AtlTypes/atltypes.h"
// FAZ 4 (2026-05-02): ContentsFilter.h zaten line 11'de include edildi
// (#pragma once ile no-op olsa da duplicate hijyen kotu, kaldirildi).

#pragma warning(pop)

using std::bitset;
using std::deque;
using std::list;
using std::map;
using std::string;
using std::vector;
