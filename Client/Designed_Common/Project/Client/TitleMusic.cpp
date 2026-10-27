//////////////////////////////////////////////////////////////////////////////
// Filename    : TitleMusic.cpp
// Description : Login Title screen music random pool implementation
// Added       : 2026-05-14
//////////////////////////////////////////////////////////////////////////////

#include "client_PCH.h"
#include "TitleMusic.h"
#include <cstdlib>

// Title muzik havuzu: Music.inf'teki index'ler
//   31 -> Data\Music\riseofthephoenix.ogg
//   32 -> Data\Music\riseofthephoenix1.ogg
//   33 -> Data\Music\riseofthephoenix2.ogg
//
// Havuza yeni index eklemek icin TitleMusic.h basindaki checklist'e bak.
static const int kTitlePool[] = { 31, 32, 33, 34, 35, 36 };
static const size_t kTitlePoolSize =
    sizeof(kTitlePool) / sizeof(kTitlePool[0]);

int GetTitleMusicID()
{
    // srand() Client.cpp WinMain icinde tek seferlik seed ediliyor.
    return kTitlePool[ rand() % kTitlePoolSize ];
}
