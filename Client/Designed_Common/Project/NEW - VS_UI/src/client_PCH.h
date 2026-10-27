////////////////////////////////////////////////////////////////////////////////
// vs_ui_lib src/ side PCH
//
// FAZ 4 (2026-04-29 Phase B): bu dosya BOM'lu BOS idi. Sadece 2 cpp dosyasi
// (Vs_ui.cpp, VS_UI_progress.cpp) bu PCH'yi include ediyor — kalan 59 cpp
// dolaysizen ContentsFilter.h gormuyordu.
//
// Layout-affecting header'lara (VS_UI_progress.h, VS_UI_DESC.h, u_window.h)
// dogrudan ContentsFilter.h include eklendi — header'i include eden her cpp
// otomatik dogru macro state ile derlenir. Bu PCH minimal kaldi (defansif).
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ContentsFilter.h"
