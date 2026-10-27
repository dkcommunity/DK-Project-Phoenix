//////////////////////////////////////////////////////////////////////
//
// Filename    : WebView2Host.cpp
// Written By  : Phoenix (2026-08-20)
// Description : PayPal Cash Charge — WebView2 pencere host'u.
//
// Akis: OpenCashChargeWindow → Evergreen Runtime tespiti
//       (GetAvailableCoreWebView2BrowserVersionString) → Win32 pencere
//       (480x640 client alani, parent'a ortalanmis, sabit boyut,
//       baslik "Cash Charge") → CreateCoreWebView2EnvironmentWithOptions
//       → controller → put_Bounds → Navigate(url).
//
// NavigationStarting filtresi: sadece endpoint host'u (gelen URL'nin
// host'u) + *.paypal.com + *.paypalobjects.com domainlerine izin verilir;
// diger navigation'lar cancel edilir (ShellExecute'a dusulmez).
//
// Hata yollari (runtime yok, pencere olusamadi, environment/controller
// olusamadi): ShellExecute ile sistem browser'ina fallback.
//
// Tum cagrilar UI thread'indedir — oyunun Win32 message loop'u WebView2
// callback'lerini pump eder. WebView2 out-of-process calistigi icin
// (msedgewebview2.exe) DX7/D3D9 render loop'una etkisi yoktur.
//
//////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "WebView2Host.h"

#include <WebView2.h>
#include <wrl.h>
#include <shellapi.h>

namespace
{

const wchar_t* const kCashChargeWndClass = L"PhoenixCashChargeWnd";
const int kCashChargeWidth  = 480;
const int kCashChargeHeight = 640;

//----------------------------------------------------------------------
// Pencere basina state — GWLP_USERDATA ile tasinir, WM_DESTROY'da silinir.
//----------------------------------------------------------------------
struct CashChargeWindowState
{
	Microsoft::WRL::ComPtr<ICoreWebView2Environment> Environment;
	Microsoft::WRL::ComPtr<ICoreWebView2Controller>  Controller;
	Microsoft::WRL::ComPtr<ICoreWebView2>            WebView;
	std::wstring EndpointHost;    // gelen URL'nin host'u (navigation allowlist)
	bool         ComInitialized;  // CoInitializeEx S_OK donduyse true — uninit bizde
};

// Tek pencere politikasi — yeni istek gelirse eskisi kapatilip yenisi acilir.
HWND g_CashChargeHwnd = NULL;

// Biz OS imlecini Cash Charge icin aciksa true. CheckActivate her alt-tab'da
// cagrilir; flag yokken Release yapmak InitApp disinda imleci gizler (bug).
bool g_CashChargeCursorHold = false;

//----------------------------------------------------------------------
// ShowCursor bir sayactir. InitApp (Client.cpp) SetCursor(NULL) +
// ShowCursor(FALSE) ile sayaci -1 yapar; oyun yazilim imleci kullanir.
// WebView2 ayni UI thread'de oldugu icin sayac < 0 iken overlay'de
// Win32 imlec cizilmez. Hedefler GameUI UI_ShowWindowCursor /
// UI_HiddenWindowCursor ile ayni: gorunur=0, gizli=-1.
//----------------------------------------------------------------------
void AcquireCashChargeOsCursor()
{
	int tempCount = ShowCursor(TRUE);
	if (tempCount != 0)
	{
		int maxChange = 10;
		if (tempCount > 0)
		{
			while (tempCount != 0 && maxChange-- > 0)
				tempCount = ShowCursor(FALSE);
		}
		else if (tempCount < 0)
		{
			while (tempCount != 0 && maxChange-- > 0)
				tempCount = ShowCursor(TRUE);
		}
	}

	// InitApp SetCursor(NULL) birakmis olabilir; sayac 0 olsa bile NULL
	// cursor gorunmez. Class hCursor IDC_ARROW; yine de thread cursor'i
	// okla cek.
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	g_CashChargeCursorHold = true;
}

void ReleaseCashChargeOsCursor()
{
	if (!g_CashChargeCursorHold)
		return;

	int tempCount = ShowCursor(FALSE);
	if (tempCount != -1)
	{
		int maxChange = 10;
		if (tempCount > -1)
		{
			while (tempCount != -1 && maxChange-- > 0)
				tempCount = ShowCursor(FALSE);
		}
		else if (tempCount < -1)
		{
			while (tempCount != -1 && maxChange-- > 0)
				tempCount = ShowCursor(TRUE);
		}
	}

	SetCursor(NULL);
	g_CashChargeCursorHold = false;
}

//----------------------------------------------------------------------
// UTF-8/ASCII → wide (URL'ler ASCII; yine de CP_UTF8 ile guvenli cevirim)
//----------------------------------------------------------------------
std::wstring UrlToWide(const std::string& url)
{
	if (url.empty())
		return std::wstring();

	int len = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), (int)url.size(), NULL, 0);
	if (len <= 0)
		return std::wstring();

	std::wstring out(len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, url.c_str(), (int)url.size(), &out[0], len);
	return out;
}

//----------------------------------------------------------------------
// "scheme://[userinfo@]host[:port]/path" → lowercase host
// (ASCII domain'ler icin manuel lowercase — locale bagimsiz)
//----------------------------------------------------------------------
std::wstring ExtractHostLower(const std::wstring& url)
{
	size_t start = 0;
	size_t schemePos = url.find(L"://");
	if (schemePos != std::wstring::npos)
		start = schemePos + 3;

	size_t end = url.find_first_of(L"/:?#", start);
	std::wstring host = url.substr(start, end - start);

	// userinfo@host durumu
	size_t atPos = host.rfind(L'@');
	if (atPos != std::wstring::npos)
		host = host.substr(atPos + 1);

	// port
	size_t colonPos = host.find(L':');
	if (colonPos != std::wstring::npos)
		host = host.substr(0, colonPos);

	for (size_t i = 0; i < host.size(); i++)
	{
		if (host[i] >= L'A' && host[i] <= L'Z')
			host[i] = host[i] + (L'a' - L'A');
	}

	return host;
}

//----------------------------------------------------------------------
// "paypal.com" ve tum subdomain'leri ("*.paypal.com") kabul
//----------------------------------------------------------------------
bool HostMatchesDomain(const std::wstring& host, const wchar_t* domain)
{
	const size_t domainLen = wcslen(domain);
	if (host == domain)
		return true;
	return host.size() > domainLen + 1 &&
	       host.compare(host.size() - domainLen, domainLen, domain) == 0 &&
	       host[host.size() - domainLen - 1] == L'.';
}

//----------------------------------------------------------------------
// Navigation allowlist: endpoint host'u + PayPal domain'leri.
// Disinda kalan her navigation cancel edilir (fallback YOK — sadece cancel).
//----------------------------------------------------------------------
bool IsAllowedNavigation(const std::wstring& navUrl, const std::wstring& endpointHost)
{
	const std::wstring host = ExtractHostLower(navUrl);
	if (host.empty())
		return false;
	if (!endpointHost.empty() && host == endpointHost)
		return true;
	return HostMatchesDomain(host, L"paypal.com") ||
	       HostMatchesDomain(host, L"paypalobjects.com");
}

void ShellExecuteFallback(const std::wstring& url)
{
	if (url.empty())
		return;
	// Sistem tarayicisi baska process; yine de ayni thread'de sayac < 0
	// ise exclusive fullscreen oyun ostte kalinca imlec gorunmez.
	// Restore: NotifyCashChargeGameActivate (oyun tekrar aktif, hwnd yok).
	AcquireCashChargeOsCursor();
	ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

//----------------------------------------------------------------------
// Pencere WndProc
//----------------------------------------------------------------------
LRESULT CALLBACK CashChargeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CashChargeWindowState* state =
		reinterpret_cast<CashChargeWindowState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

	switch (msg)
	{
	case WM_NCCREATE:
		{
			CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
			return TRUE;
		}

	case WM_SIZE:
		// Sabit boyutlu pencere (WS_THICKFRAME yok) ama restore gibi
		// durumlar icin bounds'i yine de senkron tut.
		if (state != NULL && state->Controller != nullptr)
		{
			RECT bounds;
			GetClientRect(hwnd, &bounds);
			state->Controller->put_Bounds(bounds);
		}
		return 0;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		if (state != NULL)
		{
			if (state->Controller != nullptr)
				state->Controller->Close();

			bool comInitialized = state->ComInitialized;
			delete state;   // ComPtr'ler burada release olur
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);

			if (comInitialized)
				CoUninitialize();
		}
		if (g_CashChargeHwnd == hwnd)
			g_CashChargeHwnd = NULL;
		// Overlay kapandi — oyun exclusive fullscreen imlec politikasi.
		// Yeni pencere/fallback hemen ardindan tekrar Acquire eder.
		ReleaseCashChargeOsCursor();
		return 0;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

//----------------------------------------------------------------------
// WebView2 environment + controller olusumu (async — callback'ler UI
// thread'inde, oyun message loop'u uzerinden calisir)
//----------------------------------------------------------------------
void StartWebView2(HWND hwnd, const std::wstring& url)
{
	// Deterministik user data folder: %LOCALAPPDATA%\Phoenix\WebView2CashCharge
	wchar_t userDataFolder[MAX_PATH] = { 0 };
	DWORD n = ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\Phoenix\\WebView2CashCharge",
	                                    userDataFolder, MAX_PATH);
	const wchar_t* pUserDataFolder = (n > 0 && n < MAX_PATH) ? userDataFolder : NULL;

	HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
		NULL, pUserDataFolder, NULL,
		Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[hwnd, url](HRESULT envResult, ICoreWebView2Environment* environment) -> HRESULT
			{
				CashChargeWindowState* state =
					reinterpret_cast<CashChargeWindowState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

				// Callback gelene kadar pencere kapatilmis olabilir
				if (state == NULL || !IsWindow(hwnd))
					return S_OK;

				if (FAILED(envResult) || environment == nullptr)
				{
					DestroyWindow(hwnd);
					ShellExecuteFallback(url);
					return S_OK;
				}

				state->Environment = environment;

				HRESULT hrController = environment->CreateCoreWebView2Controller(hwnd,
					Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
						[hwnd, url](HRESULT ctrlResult, ICoreWebView2Controller* controller) -> HRESULT
						{
							CashChargeWindowState* state =
								reinterpret_cast<CashChargeWindowState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

							if (state == NULL || !IsWindow(hwnd))
								return S_OK;

							if (FAILED(ctrlResult) || controller == nullptr)
							{
								DestroyWindow(hwnd);
								ShellExecuteFallback(url);
								return S_OK;
							}

							state->Controller = controller;
							HRESULT hrWebView = controller->get_CoreWebView2(&state->WebView);
							if (FAILED(hrWebView) || state->WebView == nullptr)
							{
								DestroyWindow(hwnd);
								ShellExecuteFallback(url);
								return S_OK;
							}

							RECT bounds;
							GetClientRect(hwnd, &bounds);
							controller->put_Bounds(bounds);

							// NavigationStarting filtresi: endpoint host +
							// PayPal domain'leri disi cancel. Odeme akisi oldugu
							// icin filtre kaydi basarisiz olursa FAIL-CLOSED:
							// navigate etme, fallback'e dus.
							EventRegistrationToken navToken;
							HRESULT hrNavFilter = state->WebView->add_NavigationStarting(
								Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(
									[hwnd](ICoreWebView2* sender,
									       ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT
									{
										CashChargeWindowState* state =
											reinterpret_cast<CashChargeWindowState*>(
												GetWindowLongPtrW(hwnd, GWLP_USERDATA));
										if (state == NULL)
											return S_OK;

										LPWSTR uri = NULL;
										HRESULT hrUri = args->get_Uri(&uri);
										if (FAILED(hrUri) || uri == NULL)
										{
											// hedef bilinmiyor — fail-closed
											args->put_Cancel(TRUE);
											return S_OK;
										}

										if (!IsAllowedNavigation(uri, state->EndpointHost))
											args->put_Cancel(TRUE);
										CoTaskMemFree(uri);
										return S_OK;
									}).Get(),
								&navToken);

							if (FAILED(hrNavFilter))
							{
								DestroyWindow(hwnd);
								ShellExecuteFallback(url);
								return S_OK;
							}

							HRESULT hrNav = state->WebView->Navigate(url.c_str());
							if (FAILED(hrNav))
							{
								DestroyWindow(hwnd);
								ShellExecuteFallback(url);
								return S_OK;
							}

							controller->MoveFocus(COREWEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC);
							return S_OK;
						}).Get());

				if (FAILED(hrController))
				{
					DestroyWindow(hwnd);
					ShellExecuteFallback(url);
					return S_OK;
				}

				return S_OK;
			}).Get());

	if (FAILED(hr))
	{
		// Environment olusumu baslatilamadi bile — pencereyi kapat, fallback.
		DestroyWindow(hwnd);
		ShellExecuteFallback(url);
	}
}

} // namespace


//////////////////////////////////////////////////////////////////////
// Oyun focus: overlay duruyorsa imleci tekrar goster; yoksa restore.
//////////////////////////////////////////////////////////////////////
void NotifyCashChargeGameActivate(BOOL gameActive)
{
	if (!gameActive)
		return;

	if (g_CashChargeHwnd != NULL && IsWindow(g_CashChargeHwnd))
	{
		AcquireCashChargeOsCursor();
		return;
	}

	ReleaseCashChargeOsCursor();
}


//////////////////////////////////////////////////////////////////////
// Cash Charge penceresini acar. Hata yollarinda ShellExecute fallback.
//////////////////////////////////////////////////////////////////////
void OpenCashChargeWindow(HWND parent, const std::string& url)
{
	if (url.empty())
		return;

	const std::wstring wideUrl = UrlToWide(url);
	if (wideUrl.empty())
		return;

	//------------------------------------------------------------------
	// Evergreen Runtime tespiti — yoksa direkt sistem browser'ina dus.
	//------------------------------------------------------------------
	LPWSTR versionInfo = NULL;
	HRESULT hrVersion = GetAvailableCoreWebView2BrowserVersionString(NULL, &versionInfo);
	bool runtimeAvailable = SUCCEEDED(hrVersion) && versionInfo != NULL;
	if (versionInfo != NULL)
		CoTaskMemFree(versionInfo);

	if (!runtimeAvailable)
	{
		ShellExecuteFallback(wideUrl);
		return;
	}

	//------------------------------------------------------------------
	// COM (STA) — WebView2 sart. Ana thread MTA ise calismaz → fallback.
	//------------------------------------------------------------------
	bool comInitialized = false;
	HRESULT hrCom = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (hrCom == RPC_E_CHANGED_MODE)
	{
		ShellExecuteFallback(wideUrl);
		return;
	}
	comInitialized = (hrCom == S_OK);

	//------------------------------------------------------------------
	// Tek pencere: eskisi aciksa kapat.
	//------------------------------------------------------------------
	if (g_CashChargeHwnd != NULL && IsWindow(g_CashChargeHwnd))
		DestroyWindow(g_CashChargeHwnd);
	g_CashChargeHwnd = NULL;

	//------------------------------------------------------------------
	// Window class (tekrar register hatasi zararsiz)
	//------------------------------------------------------------------
	WNDCLASSEXW wc = {};
	wc.cbSize        = sizeof(wc);
	wc.lpfnWndProc   = CashChargeWndProc;
	wc.hInstance     = GetModuleHandleW(NULL);
	wc.lpszClassName = kCashChargeWndClass;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClassExW(&wc);

	//------------------------------------------------------------------
	// 480x640 client alani — sabit boyut (resize yok), parent'a ortalanmis.
	// WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU: adres cubugu/chrome yok,
	// sadece baslik + kapat butonu.
	//------------------------------------------------------------------
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
	RECT rc = { 0, 0, kCashChargeWidth, kCashChargeHeight };
	AdjustWindowRect(&rc, style, FALSE);
	const int winW = rc.right - rc.left;
	const int winH = rc.bottom - rc.top;

	int originX = CW_USEDEFAULT;
	int originY = CW_USEDEFAULT;
	RECT parentRc;
	if (parent != NULL && GetWindowRect(parent, &parentRc))
	{
		originX = parentRc.left + ((parentRc.right - parentRc.left) - winW) / 2;
		originY = parentRc.top + ((parentRc.bottom - parentRc.top) - winH) / 2;

		// Ekran disina tasmayi engelle
		const int screenW = GetSystemMetrics(SM_CXSCREEN);
		const int screenH = GetSystemMetrics(SM_CYSCREEN);
		if (originX < 0) originX = 0;
		if (originY < 0) originY = 0;
		if (originX + winW > screenW) originX = screenW - winW;
		if (originY + winH > screenH) originY = screenH - winH;
	}

	CashChargeWindowState* state = new CashChargeWindowState();
	state->EndpointHost   = ExtractHostLower(wideUrl);
	state->ComInitialized = comInitialized;

	// parent owner olarak verilir — pencere oyun penceresinin ustunde kalir.
	HWND hwnd = CreateWindowExW(0, kCashChargeWndClass, L"Cash Charge", style,
	                            originX, originY, winW, winH,
	                            parent, NULL, wc.hInstance, state);
	if (hwnd == NULL)
	{
		delete state;
		if (comInitialized)
			CoUninitialize();
		ShellExecuteFallback(wideUrl);
		return;
	}

	g_CashChargeHwnd = hwnd;
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	SetForegroundWindow(hwnd);

	// Overlay gorunur; OS imleci InitApp gizlemesini kaldir.
	AcquireCashChargeOsCursor();

	StartWebView2(hwnd, wideUrl);
}
