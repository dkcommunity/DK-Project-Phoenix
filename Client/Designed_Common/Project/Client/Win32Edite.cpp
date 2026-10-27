#include "Client_PCH.h"
#include "Win32Edite.h"
#include "VS_UI_Base.h"




CWin32Edite::CWin32Edite(HWND hParent, HINSTANCE hInst)
{
	m_hParentHwnd = hParent;
	m_hEdit = NULL;
	m_hBtn = NULL;
	m_hdc = NULL;
	hbrush = NULL;
	m_Currentfont = NULL;

	m_nX = 0;
	m_nY = 0;
	m_nWidth = 520+16;
	m_nHigh = 21;

	m_bIsFoucus = false;
	m_dwTextColor = 0;
	m_dwBKColor = RGB(40, 40, 40);

	m_nChatMode = 0;


	m_hEdit = CreateWindow("edit","",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL,
		m_nX, m_nY, m_nWidth, m_nHigh,hParent,NULL,hInst,NULL);


	logbrush.lbHatch = 0;
	logbrush.lbStyle = BS_SOLID;
	logbrush.lbColor = m_dwBKColor;
	hbrush = CreateBrushIndirect(&logbrush);


	SetupChooseFont(hParent,m_Currentfont,m_logfont);

	//m_Currentfont = CreateFontIndirect(&m_logfont);

	SendMessage(m_hEdit,WM_SETFONT,(WPARAM)m_Currentfont,MAKELPARAM(true,0));

	this->SetTextColor(RGB(0, 254, 0));

	Show(SW_HIDE);
}

void CWin32Edite::SetupChooseFont(HWND hwnd,HFONT &cfont,LOGFONT &lfont)
{
	choosefont.lStructSize = sizeof(CHOOSEFONT);
	choosefont.hwndOwner = hwnd;
	choosefont.lpLogFont = &lfont;
	choosefont.rgbColors = RGB(0,0,0);
	choosefont.Flags = CF_FORCEFONTEXIST|CF_SCREENFONTS|CF_INITTOLOGFONTSTRUCT;
	HDC hdc = GetDC(hwnd);
	const int fontHeight = (hdc != NULL) ? NTXFONTHEIGHT(10, hdc) : -10;
	if (hdc != NULL)
		ReleaseDC(hwnd, hdc);
	cfont = CreateFont(fontHeight,0,0,0,FW_REGULAR,0,0,0,THAI_CHARSET,0,0,PROOF_QUALITY,DEFAULT_PITCH,"Tahoma");
}

CWin32Edite::~CWin32Edite()
{
	if (m_hEdit != NULL && IsWindow(m_hEdit))
	{
		DestroyWindow(m_hEdit);
		m_hEdit = NULL;
	}
	if (hbrush != NULL)
	{
		DeleteObject(hbrush);
		hbrush = NULL;
	}
	if (m_Currentfont != NULL)
	{
		DeleteObject(m_Currentfont);
		m_Currentfont = NULL;
	}
}



void CWin32Edite::SetFont(const char* szFontName)
{
	
}

void CWin32Edite::SetBKColor(DWORD dwColor)
{
	m_dwBKColor = dwColor;
	logbrush.lbColor = m_dwBKColor;
	HBRUSH hNewBrush = CreateBrushIndirect(&logbrush);
	if (hNewBrush != NULL)
	{
		if (hbrush != NULL)
			DeleteObject(hbrush);
		hbrush = hNewBrush;
	}
	InvalidateRect(m_hEdit,&m_rcRect,true);
	SendMessage(m_hParentHwnd,WM_PAINT,0,0);
}

void CWin32Edite::SetTextColor(DWORD dwColor)
{
	m_dwTextColor = dwColor;
	InvalidateRect(m_hEdit,&m_rcRect,true);
	SendMessage(m_hParentHwnd,WM_PAINT,0,0);
}

void CWin32Edite::SetPos(int x, int y)
{
	m_nX = x;
	m_nY = y;
	UpdateRect();
}

void CWin32Edite::SetSize(int nWidth, int nHigh)
{
	m_nWidth = nWidth;
	m_nHigh = nHigh;

	UpdateRect();
}

void CWin32Edite::SetWidth(int nWidth)
{
	m_nWidth = nWidth;
	UpdateRect();
}

void CWin32Edite::SetHigh(int nHigh)
{
	m_nHigh = nHigh;
	UpdateRect();
}

void CWin32Edite::SetFocus(bool bFoucus)
{
	if ( !m_bShowWindow )
		return;

	m_bIsFoucus = bFoucus;

	if ( bFoucus )
	{
		::SetFocus(m_hEdit);
		SendMessage(m_hEdit, EM_SETSEL, 100, 100);
		
	}
	else
	{
		ShowWindow(m_hEdit, SW_HIDE);
		ShowWindow(m_hEdit, SW_NORMAL);
		//SendMessage(m_hParentHwnd,WM_SETFOCUS,0,0);
		//SendMessage(m_hParentHwnd,WM_PAINT,0,0);
		//SendMessage(m_hParentHwnd,WM_SETFOCUS,0,0);
	}
}

void CWin32Edite::SetChatMode(int nChatMode)
{
	m_nChatMode = nChatMode;
	this->SetFocus(true);
	if ( nChatMode == 2 )
		::SetWindowText(m_hEdit, "/w ");
	else
		::SetWindowText(m_hEdit, "");
}

bool CWin32Edite::PtInRect(POINT pt)
{
	UpdateRect();
	return ::PtInRect(&m_rcRect, pt);
}

bool CWin32Edite::PtInBigRect(POINT pt)
{
	UpdateRect();
	
	RECT bigRect = m_rcRect;
	bigRect.top -= 50;
	return ::PtInRect(&bigRect, pt);
}

void CWin32Edite::UpdateRect()
{
	if ( !m_bShowWindow )
		return;

	m_rcRect.left = m_nX;
	m_rcRect.top = m_nY;
	m_rcRect.right = m_nX + m_nWidth;
	m_rcRect.bottom = m_nY + m_nHigh;

	SetWindowPos(m_hEdit, NULL, m_nX, m_nY, m_nWidth, m_nHigh, SWP_SHOWWINDOW);
}

void CWin32Edite::OnPaint(HWND hWnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{

}

long CWin32Edite::OnColor(HWND hWnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	if ( !m_bShowWindow )
		return 0;

	::SetTextColor((HDC)wParam,m_dwTextColor);
	SetBkMode((HDC)wParam,OPAQUE);	//*look on the bottom for more info
	SetBkColor((HDC)wParam,m_dwBKColor);

	return (long)hbrush;
}

void CWin32Edite::OnReturn()
{
	if ( !m_bShowWindow )
		return;

	char buff[128];
	GetWindowText(m_hEdit, buff, 128);
	m_strInputString = buff;

	char* szTemp = new char[m_strInputString.length()];
	strcpy(szTemp, m_strInputString.c_str());

	std::string strTemp = buff;
	int n = strTemp.find("/w ");

	if ( n != -1 )
	{
		strTemp.erase(n, 3);
		int nspace = strTemp.find(" ");
		m_strInputString.erase(nspace + 4);
		SetWindowText(m_hEdit, m_strInputString.c_str());
		
	}
	else
		SetWindowText(m_hEdit, "");

	this->SetFocus(!m_bIsFoucus);


	if( strlen(szTemp) > 0 )
		gpC_base->SendMessage(UI_CHAT_RETURN, m_nChatMode, m_dwTextColor, (void*)szTemp);
}



void CWin32Edite::Show(int nShow)
{
	if ( nShow == SW_HIDE )
	{
		m_bShowWindow = false;
	}
	else
	{
		m_bShowWindow = true;
	}
	::ShowWindow(m_hEdit, nShow);
}
