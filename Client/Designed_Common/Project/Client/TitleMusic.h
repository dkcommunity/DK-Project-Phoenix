//////////////////////////////////////////////////////////////////////////////
// Filename    : TitleMusic.h
// Description : Login Title screen music random pool
// Added       : 2026-05-14 - random pool feature (3 ogg files)
//
// Title ekrani acildiginda Music.inf icindeki belirli index'lerden
// rastgele birini secip return eder. Asagidaki kullanim yerleri:
//   - GameMain.cpp  (MODE_MAINMENU geciste muzigi calar)
//   - UIMessageManager.cpp (PlayTitleMusic helper)
//
// Yeni Title muzik dosyasi eklemek icin:
//   1. .ogg dosyasini GameData\data\music\ klasorune koy
//   2. tools/inf_editors/music_inf_editor.py ile Music.inf'e add
//   3. TitleMusic.cpp icindeki kTitlePool[] dizisine yeni index'i ekle
//      (kTitlePoolSize sizeof hesabi ile otomatik artar)
//////////////////////////////////////////////////////////////////////////////

#ifndef __TITLE_MUSIC_H__
#define __TITLE_MUSIC_H__

// Title muzik havuzundan rastgele bir Music.inf index'i dondurur.
// srand() Client.cpp:WinMain icinde tek seferlik seed ediliyor;
// burada ek seed yok.
int GetTitleMusicID();

#endif // __TITLE_MUSIC_H__
