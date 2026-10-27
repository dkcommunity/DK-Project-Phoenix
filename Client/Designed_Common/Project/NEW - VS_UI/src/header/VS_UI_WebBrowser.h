/*-----------------------------------------------------------------------------

	WEBBROWSER.h

	webbrowser interface.

	2005,1,31, sobeit

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_WEBBROWSER_H__
#define __VS_UI_WEBBROWSER_H__

#include "VS_UI_Widget.h"
#include "VS_UI_Util.h"
#include "VS_UI_GameCommon.h"
#include "VS_UI_Dialog.h"
#include "VS_UI_DESC.h"


//#include <ddraw.h>
//#include "EXDISP.H"
//#include "ATLBASE.H"


//-----------------------------------------------------------------------------
// MWEBBROWSER
//
//-----------------------------------------------------------------------------
class C_VS_UI_WEBBROWSER : public Window, public Exec, public ButtonVisual
{
public:
	
private:


		enum EXEC_ID
	{
		WEBBROWSER_CLOSE 
	};


	enum WEBBROWSER_SPK_INDEX
	{
		WEBBROWSER_MAIN, 
		WEBBROWSER_CLOSE_BUTTON , 
		WEBBROWSER_CLOSE_BUTTON_HILIGHTED , 
		WEBBROWSER_CLOSE_BUTTON_PUSHED ,
	
	};

	
	C_SPRITE_PACK	*			m_pC_WebBrowser_spk;
	ButtonGroup *				m_pC_button_group;
	

	HINSTANCE			m_HIWebDll	;
	HWND				m_hWnd_Explorer;
	bool				m_IsMouseInWebBrowser;
	

	
public:

	C_VS_UI_WEBBROWSER();
	~C_VS_UI_WEBBROWSER();

	
	bool	Start(HWND hwndApp, char* pURL, HINSTANCE hInst) ; // void* pWebOjbect);
	int		GetMouseCursorInfo();
	bool	IsInRectPoint(int X, int Y);


	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);
	void	AcquireDisappear() {}
	void	Run(id_t id);
	void	Process();

	void	Show();
	bool	IsPixel(int _x, int _y);
	
	bool	MouseControl(UINT message, int _x, int _y);


	
	//void	Finish();

	//void	SetWebBrowserSize(int StartX, int StartY, int Width, int Height);
	//void	SetWebBrowserStyle(DWORD dwstyle);
	//DWORD	GetWebBrowserStyle();
	

//	IWebBrowser2* GetIWebBrowser2() {return m_pWebBrowser;}
};


#endif