#ifndef __WIN32EDITE_H__
#define __WIN32EDITE_H__

#include <Windows.h>
#include <commctrl.h>
#include <string>
#include "VS_UI.h"
#pragma comment(lib,"comctl32.lib")

#define NTXFONTHEIGHT(PointSize, hDC) (-MulDiv((PointSize), GetDeviceCaps(hDC, LOGPIXELSY), 72))

class CWin32Edite : public IWin32Edite
{
public:
	CWin32Edite(HWND hParent, HINSTANCE hInst);
	~CWin32Edite();

public:
	void SetFont(const char* szFontName);
	void SetBKColor(DWORD dwColor);
	void SetTextColor(DWORD dwColor);
	void SetPos(int x, int y);
	void SetSize(int nWidth, int nHigh);
	void SetWidth(int nWidth);
	void SetHigh(int nHigh);

	void SetFocus(bool bFoucus);

	void Show(int nShow);

	bool PtInRect( POINT pt);
	void SetChatMode(int nChatMode);

	void OnPaint(HWND hWnd,UINT iMsg,WPARAM wParam,LPARAM lParam);
	long OnColor(HWND hWnd,UINT iMsg,WPARAM wParam,LPARAM lParam);
	void OnReturn();

	bool PtInBigRect(POINT pt);

	
protected:
	void UpdateRect();
	void SetupChooseFont(HWND hwnd,HFONT &cfont,LOGFONT &lfont);

protected:
	int		m_nX;
	int		m_nY;
	int		m_nWidth;
	int		m_nHigh;

	std::string  m_strInputString;

	int		m_nChatMode;

	RECT	m_rcRect;

	HWND	m_hParentHwnd;
	HWND	m_hEdit;
	HWND	m_hBtn;
	HDC		m_hdc;

	DWORD	m_dwTextColor;
	DWORD	m_dwBKColor;

	LOGBRUSH		logbrush;
	HBRUSH			hbrush;
	//new stuff
	HWND			changefont;
	HFONT			m_Currentfont;
	LOGFONT			m_logfont;
	CHOOSEFONT		choosefont;
	bool			m_bShowWindow;
	bool			m_bIsFoucus;
};







#endif
