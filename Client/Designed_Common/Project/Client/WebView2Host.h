//////////////////////////////////////////////////////////////////////
//
// Filename    : WebView2Host.h
// Written By  : Phoenix (2026-08-20)
// Description : PayPal Cash Charge — WebView2 pencere host'u.
//
//////////////////////////////////////////////////////////////////////

#ifndef __WEBVIEW2_HOST_H__
#define __WEBVIEW2_HOST_H__

#include <windows.h>
#include <string>

// Cash Charge penceresini acar (480x640 client alani, parent'a ortalanmis,
// sabit boyut, baslik "Cash Charge", adres cubugu yok).
// WebView2 Runtime yoksa veya pencere/WebView2 olusumu basarisiz olursa
// ShellExecute ile sistem browser'ina fallback yapar.
// Tum cagrilar UI thread'inde (oyun ana thread'i) yapilmalidir.
void OpenCashChargeWindow(HWND parent, const std::string& url);

// Oyun tekrar aktif olunca (CheckActivate / WM_ACTIVATEAPP) cagrilir.
// WebView2 Cash Charge aciksa OS imlecini yeniden gosterir (DDraw restore
// gizlemis olabilir). Pencere yoksa ve biz imleci acmissak InitApp
// politikasina doner (ShowCursor sayaci -1, SetCursor NULL).
void NotifyCashChargeGameActivate(BOOL gameActive);

#endif
