// u_window.cpp
#include "client_PCH.h"
#include "crash_trace_tool.h"
#include "../../../Client/frame_perf_probe.h"
#pragma warning(disable:4786)

#ifdef _CPPRTTI
#include <typeinfo>
#endif

#include "u_window.h"
#include <math.h>
#include "vs_ui.h"
#include "CImm.h"
#include "UserOption.h"
#include "UserInformation.h"

#define STATCH_VALUE 10
#define HIDE_GAP	4
#define HIDE_SPEED	10

extern HWND g_hWnd;
extern BOOL g_bActiveApp;
extern BOOL g_bActiveGame;

static void Phoenix_BuildWindowProbeName(char* out, int outSize, Window* pWindow)
{
	if (!out || outSize <= 0)
		return;

	out[0] = 0;

	if (!pWindow)
	{
		_snprintf(out, outSize - 1, "null-window");
		out[outSize - 1] = 0;
		return;
	}

	std::string windowName = pWindow->GetWindowName();
	const char* debugName = windowName.empty() ? NULL : windowName.c_str();

#ifdef _CPPRTTI
	if (!debugName || !debugName[0])
		debugName = typeid(*pWindow).name();
#endif

	if (!debugName || !debugName[0])
		debugName = "(unnamed)";

	_snprintf(
		out,
		outSize - 1,
		"%s|ptr=%p|vptr=%p|rect=%d:%d:%d:%d|pin=%d|top=%d",
		debugName,
		(void*)pWindow,
		*(void**)pWindow,
		(int)pWindow->x,
		(int)pWindow->y,
		(int)pWindow->w,
		(int)pWindow->h,
		pWindow->GetAttributes() ? (int)pWindow->GetAttributes()->pin : 0,
		pWindow->GetAttributes() ? (int)pWindow->GetAttributes()->topmost : 0);
	out[outSize - 1] = 0;
}


//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------
WindowManager *	gpC_window_manager; // App���� �Ҵ��Ѵ�.

//-----------------------------------------------------------------------------
// Window
//
// 
//-----------------------------------------------------------------------------
Window::Window(int _x, int _y, int _w, int _h) : Rect(_x, _y, _w, _h)
{ 
	assert(gpC_window_manager != NULL);

	gpC_window_manager->Register(this);

	Init();

	//
	// constructor/destructor������ pure virtual�� �Ұ����ϴ�. �ֱ׷�������
	// �𸣰ڴ�. �Ƹ� �׷����ϸ� instatance�� �Ұ����ϴٴ� ����... ��...
	// �׷��� �� event�� �� ������δ� �Ұ����ϴ�.
	//
	//WindowEventReceiver(EVENT_WINDOW_CREATE);
}

Window::Window()
{
	//assert(gpC_window_manager != NULL);

	//gpC_window_manager->Register(this);

	Init();

	//WindowEventReceiver(EVENT_WINDOW_CREATE);
}

//-----------------------------------------------------------------------------
// ~Window
//
// 
//-----------------------------------------------------------------------------
Window::~Window()
{
	//assert(gpC_window_manager != NULL);

	//gpC_window_manager->Unregister(this);

	//WindowEventReceiver(EVENT_WINDOW_DESTROY);

//	gC_ci->.FinishImeRunning();
//	gC_ci->.ClearCurrentIMEComposition();

}

//-----------------------------------------------------------------------------
// ShowWidget
//
// Window ���� ���̴� Widget���� ����Ѵ�.
//
// !�̰��� �ܺ��� Show() ��ƾ ���������� ����Ǿ� �Ѵ�.
//-----------------------------------------------------------------------------
void Window::ShowWidget() const
{
	LineEditorVisual * data;
	for (int i=0; i < m_sdl_lev.Size(); i++)
		if (m_sdl_lev.Data(i, data))
			data->Show(); // ���ο��� dc�� Get/Release �Ѵ�.
}

//-----------------------------------------------------------------------------
// Attach
//
// p_lev�� Window�� ���δ�.
//-----------------------------------------------------------------------------
void Window::Attach(LineEditorVisual * p_lev)
{
	if (p_lev == NULL)
		_Error(NULL_REF);

	m_sdl_lev.Add(p_lev); // �̹� �����ϸ� �����̴�.
}

//-----------------------------------------------------------------------------
// InitAttributes
//
// 
//-----------------------------------------------------------------------------
void Window::InitAttributes()
{
	m_attributes.window_move = true;
	m_attributes.pin = false;
	m_attributes.topmost = false;
	m_attributes.keyboard_control = false;
	m_attributes.alpha = false;
	m_attributes.statch = true;
	m_attributes.autohide = ATTRIBUTES_HIDE_NOT;
}

//-----------------------------------------------------------------------------
// AttrTopmost
//
// topmost �Ӽ��� �����Ѵ�.
//-----------------------------------------------------------------------------
void Window::AttrTopmost(bool state)
{
	m_attributes.topmost = state;
}

//-----------------------------------------------------------------------------
// AttrKeyboardControl
//
// keyboard control �Ӽ��� �����Ѵ�.
//-----------------------------------------------------------------------------
void Window::AttrKeyboardControl(bool state)
{
	m_attributes.keyboard_control = state;
	gC_ci->FinishImeRunning();
	gC_ci->ClearCurrentIMEComposition();
//	gC_ci->.SetEngInput();
}

//-----------------------------------------------------------------------------
// AttrWindowMove
//
// Window �̵� �Ӽ��� �����Ѵ�.
//-----------------------------------------------------------------------------
void Window::AttrWindowMove(bool new_state)
{ 
	m_attributes.window_move = new_state;

	// move ready�� true�� �� �� window_move attr�� false�� �� �� �ִ�.
	m_bl_window_move_ready = false;
}

//-----------------------------------------------------------------------------
// AttrAlpha
//
// Window �̵� �Ӽ��� �����Ѵ�.
//-----------------------------------------------------------------------------
void Window::AttrAlpha(bool new_state)
{ 
	m_attributes.alpha = new_state;
}

//-----------------------------------------------------------------------------
// AttrStatch
//
// Window �̵� �Ӽ��� �����Ѵ�.
//-----------------------------------------------------------------------------
void Window::AttrStatch(bool new_state)
{ 
	m_attributes.statch = new_state;
}

//-----------------------------------------------------------------------------
// AttrAutoHide
//
// Window �̵� �Ӽ��� �����Ѵ�.
//-----------------------------------------------------------------------------
void Window::AttrAutoHide(ATTRIBUTES_HIDE new_state)
{ 
	m_attributes.autohide = new_state;
}

#if __CONTENTS(__GPS_ADD)
//-----------------------------------------------------------------------------
// AttrGPS
//
// Window �̵� �Ӽ��� �����Ѵ�.
//-----------------------------------------------------------------------------
void Window::AttrGPS(bool new_state)
{
	m_attributes.gpsview = new_state;
}
#endif //__GPS_ADD

//-----------------------------------------------------------------------------
// ProcessHide
//
// Window �̵� �Ӽ��� �����Ѵ�.
//-----------------------------------------------------------------------------
void Window::ProcessHide(int gap, bool alpha_window)
{
	gap += HIDE_GAP;
	bool bl_move = false;
	if(GetAttributes()->autohide == ATTRIBUTES_HIDE_NOT)
	{
		if(x < 0)
		{
				x = 0;
				bl_move = true;
		}
		//else if(x+w > RESOLUTION_X)
		//{
		//		x = RESOLUTION_X-w;
		//		bl_move = true;
		//}

		else if(x+w > g_pUserInformation->iResolution_x)
		{
				x = g_pUserInformation->iResolution_x-w;
				bl_move = true;
		}

		if(y < 0)
		{
				y = 0;
				bl_move = true;
		}
		//else if(y+h > RESOLUTION_Y)
		//{
		//		y = RESOLUTION_Y-h;
		//		bl_move = true;
		//}

		else if(y+h > g_pUserInformation->iResolution_y)
		{
				y = g_pUserInformation->iResolution_y-h;
				bl_move = true;
		}

		
		if(bl_move)WindowEventReceiver(EVENT_WINDOW_MOVE);
		return;
	}

	if(IsPixel(gpC_mouse_pointer->GetX(), gpC_mouse_pointer->GetY()) && (gpC_window_manager->GetMouseFocusedWindow() == this || alpha_window))
	// ���콺�� ���� ������ ������.
	{
		if(x < 0)
		{
			if(g_pUserOption->AutoHideSmoothScroll)
				x = min(0, x+HIDE_SPEED);
			else
				x = 0;
			bl_move = true;
		}
		//else if(x+w > RESOLUTION_X)
		//{
		//	if(g_pUserOption->AutoHideSmoothScroll)
		//		x = max(RESOLUTION_X-w, x-HIDE_SPEED);
		//	else
		//		x = RESOLUTION_X-w;
		//	bl_move = true;
		//}

		else if(x+w > g_pUserInformation->iResolution_x)
		{
			if(g_pUserOption->AutoHideSmoothScroll)
				x = max(g_pUserInformation->iResolution_x-w, x-HIDE_SPEED);
			else
				x = g_pUserInformation->iResolution_x-w;
			bl_move = true;
		}


		if(y < 0)
		{
			if(g_pUserOption->AutoHideSmoothScroll)
				y = min(0, y+HIDE_SPEED);
			else
				y = 0;
			bl_move = true;
		}
		//else if(y+h > RESOLUTION_Y)
		//{
		//	if(g_pUserOption->AutoHideSmoothScroll)
		//		y = max(RESOLUTION_Y-h, y-HIDE_SPEED);
		//	else
		//		y = RESOLUTION_Y-h;
		//	bl_move = true;
		//}
		else if(y+h > g_pUserInformation->iResolution_y)
		{
			if(g_pUserOption->AutoHideSmoothScroll)
				y = max(g_pUserInformation->iResolution_y-h, y-HIDE_SPEED);
			else
				y = g_pUserInformation->iResolution_y-h;
			bl_move = true;
		}

	}
	else
	// �ƴϸ� �����簡-_-;
	{
		//if(!(GetAttributes()->autohide == ATTRIBUTES_HIDE_HEIGHT
		//	&& (y <= 0 && y+h > gap || y+h >= RESOLUTION_Y && y < RESOLUTION_Y-gap))
		//	&& (x <= 0 && x+w > gap || x+w >= RESOLUTION_X && x < RESOLUTION_X-gap)
		//	&& y+h != gap && y != RESOLUTION_Y-gap)
		if(!(GetAttributes()->autohide == ATTRIBUTES_HIDE_HEIGHT
			&& (y <= 0 && y+h > gap || y+h >= g_pUserInformation->iResolution_y && y < g_pUserInformation->iResolution_y-gap))
			&& (x <= 0 && x+w > gap || x+w >= g_pUserInformation->iResolution_x && x < g_pUserInformation->iResolution_x -gap)
			&& y+h != gap && y != g_pUserInformation->iResolution_y-gap)

		{
			if(x <= 0 && x+w > gap)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					x = max(-(w-gap), x-HIDE_SPEED);
				else
					x = -(w-gap);
			}
			//else if(x+w >= RESOLUTION_X && x < RESOLUTION_X-gap)
			//{
			//	if(g_pUserOption->AutoHideSmoothScroll)
			//		x = min(RESOLUTION_X-gap, x+HIDE_SPEED);
			//	else
			//		x = RESOLUTION_X-gap;
			//}

			else if(x+w >= g_pUserInformation->iResolution_x && x < g_pUserInformation->iResolution_x-gap)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					x = min(g_pUserInformation->iResolution_x-gap, x+HIDE_SPEED);
				else
					x = g_pUserInformation->iResolution_x-gap;
			}


			else if(x+w < gap) x = gap-w;
			//else if(x > RESOLUTION_X-gap)x = RESOLUTION_X-gap;
			else if(x > g_pUserInformation->iResolution_x-gap)x = g_pUserInformation->iResolution_x-gap;
			// ������
			if(y < 0)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					y = min(0, y+HIDE_SPEED);
				else
					y = 0;
			}
			//else if(y+h > RESOLUTION_Y)
			//{
			//	if(g_pUserOption->AutoHideSmoothScroll)
			//		y = max(RESOLUTION_Y-h, y-HIDE_SPEED);
			//	else
			//		y = RESOLUTION_Y-h;
			//}

			else if(y+h > g_pUserInformation->iResolution_y)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					y = max(g_pUserInformation->iResolution_y-h, y-HIDE_SPEED);
				else
					y = g_pUserInformation->iResolution_y-h;
			}

			bl_move = true;
		}
		//else if(x+w != gap && x != RESOLUTION_X-gap)
		else if(x+w != gap && x != g_pUserInformation->iResolution_x-gap)
		{
			if(y <= 0 && y+h > gap)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					y = max(-(h-gap), y-HIDE_SPEED);
				else
					y = -(h-gap);
			}
			//else if(y+h >= RESOLUTION_Y && y < RESOLUTION_Y-gap)
			else if(y+h >= g_pUserInformation->iResolution_y && y < g_pUserInformation->iResolution_y-gap)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					//y = min(RESOLUTION_Y-gap, y+HIDE_SPEED);
					y = min(g_pUserInformation->iResolution_y-gap, y+HIDE_SPEED);
				else
					y = g_pUserInformation->iResolution_y-gap;
					//y = RESOLUTION_Y-gap;
			}
			else if(y+h < gap) y = gap-h;
			//else if(y > RESOLUTION_Y-gap)y = RESOLUTION_Y-gap;
			else if(y > g_pUserInformation->iResolution_y-gap)y = g_pUserInformation->iResolution_y-gap;
			// ������
			if(x < 0)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					x = min(0, x+HIDE_SPEED);
				else
					x = 0;
			}
			//else if(x+w > RESOLUTION_X)
			else if(x+w > g_pUserInformation->iResolution_x)
			{
				if(g_pUserOption->AutoHideSmoothScroll)
					//x = max(RESOLUTION_X-w, x-HIDE_SPEED);
					x = max(g_pUserInformation->iResolution_x-w, x-HIDE_SPEED);
				else
					//x = RESOLUTION_X-w;
					x = g_pUserInformation->iResolution_x-w;
			}
			bl_move = true;
		}
	}
	if(bl_move)WindowEventReceiver(EVENT_WINDOW_MOVE);

}

//-----------------------------------------------------------------------------
// AttrPin
//
// Window�� pin�ȱ� ���θ� �����Ѵ�.
// pin�� ������ pin�� ���� ���� Window(WindowManager::m_show_list)���� ���� 
// �켱������ ���δ�. pin�� ���� Window(WindowManager::m_show_list_pinned_window)�� 
// �׵鳢�� �켱������ �����Ѵ�.
//-----------------------------------------------------------------------------
void Window::AttrPin(bool new_state)
{
	if (gpC_window_manager->GetShowState(this) == true)
	{
		// Window�� �������� �ִ� �����̸� ���� ������ show_list�� �����ϰ� new_state��
		// show_list�� �����Ѵ�.
		gpC_window_manager->DisappearWindow(this);

		m_attributes.pin = new_state;
		gpC_window_manager->AppearWindow(this);
	}
	else
	{
		// �������� ���� ���� ���¸� �׳� attr�� �ٲ۴�. �̰��� ���� AppearWindow��
		// ����Ǹ鼭 show_list�� ���������� ����� ���̴�.
		m_attributes.pin = new_state;
	}
}

//-----------------------------------------------------------------------------
// ClearInputState
//
// �ܺο��� Window�� �Է»��¸� clear�Ѵ�.
//-----------------------------------------------------------------------------
void Window::ClearInputState()
{
	MoveOk();
	m_u_mouse_state = MOUSE_NOSTATE;
}

//-----------------------------------------------------------------------------
// Init
//
// 
//-----------------------------------------------------------------------------
void Window::Init()
{
	InitAttributes();

	m_bl_window_move_ready = false;
	m_u_mouse_state = MOUSE_NOSTATE;
}

//-----------------------------------------------------------------------------
// GetMouseInputState
//
// 
//-----------------------------------------------------------------------------
MOUSE_STATE Window::GetMouseInputState()
{
	MOUSE_STATE state = m_u_mouse_state;
	m_u_mouse_state = MOUSE_NOSTATE;

	return state;
}

//-----------------------------------------------------------------------------
// MoveReady
//
// Window�� �̵� �غ� �Ǿ����� �˸���. �̰��� �����ؾ� Move�� �� �� �ִ�.
//-----------------------------------------------------------------------------
void Window::MoveReady()
{
	if (m_attributes.window_move == true)
	{
		m_bl_window_move_ready = true;
	}
}

//-----------------------------------------------------------------------------
// MoveOk
//
// Window �̵��� ������.
//-----------------------------------------------------------------------------
void Window::MoveOk()
{
	m_bl_window_move_ready = false;
}

//-----------------------------------------------------------------------------
// SetOrigin
//
// �̵��߽ɼ���, �� ������ ��ġ�� �����Ѵ�.
// � target�� ������ �̵������ν� �� target ��ü�� �̵��ϴ� ȿ���� �� �� �ִ�.
//
// 2���� ������ Window������ ������ (x, y)�� �������� ������ �� �ִ�.
//-----------------------------------------------------------------------------
void Window::SetOrigin(int ox, int oy)
{
	m_origin.Set(ox, oy);
}

//-----------------------------------------------------------------------------
// Move
//
// Origin�� ��ȭ���� �����Ͽ� Window�� �̵���Ų��.
// �̵��Ͽ����� true��, �ƴϸ� false�� ��ȯ�Ѵ�.
//-----------------------------------------------------------------------------
bool Window::Move(int new_ox, int new_oy)
{
	if (m_bl_window_move_ready)
	{
		x = new_ox-m_origin.x;
		y = new_oy-m_origin.y;

		if(GetAttributes()->statch)
		{
			if(x < STATCH_VALUE && x > -STATCH_VALUE)
			{
				x = 0;
			}
			else if(x+w > g_pUserInformation->iResolution_x-STATCH_VALUE && x+w < g_pUserInformation->iResolution_x+STATCH_VALUE)
			{
				x = g_pUserInformation->iResolution_x-w;
			}

			if(y < STATCH_VALUE && y > -STATCH_VALUE)
			{
				y = 0;
			}
			else if(y+h > g_pUserInformation->iResolution_y-STATCH_VALUE && y+h < g_pUserInformation->iResolution_y+STATCH_VALUE)
			{
				y = g_pUserInformation->iResolution_y-h;
			}
		}

		// FAZ 4 (2026-05-03): Screen-bound clamp. Önceden yalnızca snap-to-edge
		// (statch) vardı, ama window herhangi bir koordinata gidebiliyordu →
		// ekran dışına çıkıp kaybolabiliyordu + saved position bir sonraki
		// session'da off-screen. Clamp: window tamamen ekran içinde kalır.
		const int resW = g_pUserInformation->iResolution_x;
		const int resH = g_pUserInformation->iResolution_y;
		// Window genişliği ekrandan büyükse 0'a bağla, yoksa max(0, min(x, res-w)).
		if (w >= resW)      x = 0;
		else if (x < 0)     x = 0;
		else if (x + w > resW) x = resW - w;
		if (h >= resH)      y = 0;
		else if (y < 0)     y = 0;
		else if (y + h > resH) y = resH - h;

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// MouseControl
//
// Window ���� Mouse control.
// (x, y)�� �׻� Window ���ο� �ִ�.
//-----------------------------------------------------------------------------
bool Window::MouseControl(UINT message, int _x, int _y)
{
	switch (message)
	{
		case M_MOVING:
				if (Move(_x, _y) == true)
				{

					// ���������� �̵��Ǿ���. ���� WindowManager���� �� ����� �˷��� �Ѵ�.
					m_u_mouse_state = MOUSE_MOVE;
				}
			break;

		case M_LEFTBUTTON_UP:
			MoveOk();
			break;
	}

	return true;
}

//-----------------------------------------------------------------------------
// KeyboardControl
//
// Window ���� Keyboard control.
//-----------------------------------------------------------------------------
void Window::KeyboardControl(UINT message, UINT key, long extra)
{
	gC_ci->IME_MessageProcessor(message, key, extra);
}

//-----------------------------------------------------------------------------
// WindowManager
//
// 
//-----------------------------------------------------------------------------
WindowManager::WindowManager()
{
	m_show_list.clear();
	m_show_list_pinned_window.clear();

	m_pC_mouse_click_window = NULL;
	m_pC_topmost_window = NULL;
	m_pC_keydown_window = NULL;
	m_pC_keyboard_control_window = NULL;
	m_pC_mouse_focused_window = NULL;
	m_pC_pushed_window = NULL;
	m_old_origin_x = 0;
	m_old_origin_y = 0;
	m_res_x = 0;
	m_res_y = 0;
	m_fp_handler_acquire_mousefocus = NULL;
	m_fp_handler_unacquire_mousefocus = NULL;
	m_fp_handler_acquire_disappear = NULL;
}

//-----------------------------------------------------------------------------
// ~WindowManager
//
// 
//-----------------------------------------------------------------------------
WindowManager::~WindowManager()
{

}

//-----------------------------------------------------------------------------
// SetKeyboardControlWindow
//
// window keyboard control attr�� �˻��Ͽ� m_pC_keyboard_control_window�� �����Ѵ�.
//-----------------------------------------------------------------------------
void WindowManager::SetKeyboardControlWindow(Window * p_window)
{
	if (p_window != NULL)
	{
		if (p_window->GetAttributes()->keyboard_control == true)
			m_pC_keyboard_control_window = p_window;
	}
	else
		m_pC_keyboard_control_window = NULL;
}

//-----------------------------------------------------------------------------
// SetNextKeyboardControlWindow
//
// 
//-----------------------------------------------------------------------------
void WindowManager::SetNextKeyboardControlWindow()
{
	m_pC_keyboard_control_window = NULL;

	if (m_pC_topmost_window == NULL)
	{
		List::iterator itr;

		itr = m_show_list_pinned_window.begin();
		while (itr != m_show_list_pinned_window.end())
		{
			if ((*itr)->GetAttributes()->keyboard_control == true)
			{
				m_pC_keyboard_control_window = *itr;
				return;
			}

			itr++;
		}

		itr = m_show_list.begin();
		while (itr != m_show_list.end())
		{
			if ((*itr)->GetAttributes()->keyboard_control == true)
			{
				m_pC_keyboard_control_window = *itr;
				return;
			}

			itr++;
		}
	}
}

//-----------------------------------------------------------------------------
// SetNextTopmostWindow
//
// Window topmost attr�� �˻��Ͽ� m_pC_topmost_window�� �����Ѵ�.
//-----------------------------------------------------------------------------
void WindowManager::SetNextTopmostWindow()
{
	// ! topmost Window�̸� pinned Window�� �ƴϴ�!!!

	List::iterator itr;

	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		if ((*itr)->GetAttributes()->topmost == true)
		{
			m_pC_topmost_window = *itr;
			return;
		}

		itr++;
	}

	m_pC_topmost_window = NULL;
}

//-----------------------------------------------------------------------------
// CancelPushStateOfCurrentPushedWindow
//
// ���� ���� Window(�̵��ϱ� ���ؼ���, ...)�� �������¸� �����Ѵ�.
// �̰��� �ܺο��� pushed state�� �� ���� clear�ϱ� ���� ���̴�.
//
// ���� ���¿� �ִٸ� true�� ��ȯ�Ѵ�.
//-----------------------------------------------------------------------------
bool WindowManager::CancelPushStateOfCurrentPushedWindow()
{
	if (m_pC_pushed_window != NULL)
	{
		m_pC_pushed_window->CancelPushState();
		m_pC_pushed_window = NULL;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// HasMouseCaptureState
//
// Release event'i kayboldugunda stale state tespiti icin aktif push/drag
// capture var mi diye WindowManager listelerini kontrol eder.
//-----------------------------------------------------------------------------
bool WindowManager::HasMouseCaptureState() const
{
	if (m_pC_pushed_window != NULL)
		return true;

	List::const_iterator itr = m_show_list_pinned_window.begin();
	while (itr != m_show_list_pinned_window.end())
	{
		const Window * p_window = (*itr);
		if (p_window != NULL && p_window->Moving())
			return true;

		itr++;
	}

	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		const Window * p_window = (*itr);
		if (p_window != NULL && p_window->Moving())
			return true;

		itr++;
	}

	return false;
}

//-----------------------------------------------------------------------------
// CancelMouseCaptureState
//
// Release event'i WindowManager'a ulasmadan yutuldugunda click aksiyonu
// calistirmadan sadece stale push/drag state'ini temizler.
//-----------------------------------------------------------------------------
bool WindowManager::CancelMouseCaptureState()
{
	bool cancelled = false;

	if (m_pC_pushed_window != NULL)
	{
		m_pC_pushed_window->CancelPushState();
		m_pC_pushed_window = NULL;
		cancelled = true;
	}

	List::iterator itr = m_show_list_pinned_window.begin();
	while (itr != m_show_list_pinned_window.end())
	{
		Window * p_window = (*itr);
		if (p_window != NULL && p_window->Moving())
		{
			p_window->MoveOk();
			cancelled = true;
		}
		itr++;
	}

	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		Window * p_window = (*itr);
		if (p_window != NULL && p_window->Moving())
		{
			p_window->MoveOk();
			cancelled = true;
		}
		itr++;
	}

	return cancelled;
}

//-----------------------------------------------------------------------------
// ShowedWindowSize
//
// 
//-----------------------------------------------------------------------------
int WindowManager::ShowedWindowSize() const 
{ 
	return m_show_list.size()+m_show_list_pinned_window.size(); 
}

//-----------------------------------------------------------------------------
// GetSequence
//
// p_this_window�� �켱������ ��ȯ�Ѵ�. 0�� �������� �켱������ ����.
// �����ϸ�, -1�� ��ȯ�Ѵ�.
//-----------------------------------------------------------------------------
int WindowManager::GetSequence(Window * p_this_window)
{
	if (m_pC_topmost_window != NULL && m_pC_topmost_window == p_this_window)
		return 0;

	int sequence = 0;
	List::iterator itr;

	itr = m_show_list_pinned_window.begin();
	while (itr != m_show_list_pinned_window.end())
	{
		if (*itr == p_this_window)
		{
			return sequence;
		}

		itr++;
		sequence++;
	}
	
	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		if (*itr == p_this_window)
		{
			return sequence;
		}

		itr++;
		sequence++;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// SendMouseMoveMessageToMouseFocusedWindow
//
// 
//-----------------------------------------------------------------------------
void WindowManager::SendMouseMoveMessageToMouseFocusedWindow()
{
	if (m_pC_mouse_focused_window != NULL)
		m_pC_mouse_focused_window->MouseControl(M_MOVING, m_current_mouse_x, m_current_mouse_y);
}

//-----------------------------------------------------------------------------
// GetFirstPriorityWindow
//
// �ֿ켱 Window�� ��ȯ�Ѵ�.
// �������� Window�� ������ NULL�� ��ȯ�Ѵ�.
//-----------------------------------------------------------------------------
Window * WindowManager::GetFirstPriorityWindow() const
{
	if (m_pC_topmost_window != NULL)
		return m_pC_topmost_window;

	if (m_show_list_pinned_window.empty() == false)
	{
		return m_show_list_pinned_window.front();
	}
	else
	{
		if (m_show_list.empty() == false)
			return m_show_list.front();
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// GetMovingWindow
//
// ���� �̵����� Window�� pointer�� ��ȯ�Ѵ�. �̰��� � Window�� ���� �̵��ϰ�
// �ִ��� �ܺο��� �˱����� ���̴�.
//
// !Window�� �� ������ �� �ϳ��� �̵��� �� ������ �װ��� '���̴�' ���̴�. ����
// �װ��� �ֿ켱 Window�̴�.
//-----------------------------------------------------------------------------
Window * WindowManager::GetMovingWindow() const
{
	Window * p_window;

	if (m_show_list_pinned_window.empty() == false)
	{
		p_window = m_show_list_pinned_window.front();
		if (p_window->Moving() == true)
			return p_window;
	}

	if (m_show_list.empty() == false)
	{
		p_window = m_show_list.front();
		if (p_window->Moving() == true)
			return p_window;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// FirstPriority
//
// p_this_window�� �ֿ켱������ �Ѵ�.
// p_this_window�� list�� �������� ������ �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void WindowManager::FirstPriority(Window * p_this_window)
{
	assert(p_this_window != NULL);

	// failed?
	if (p_this_window == NULL)
		 return;

	List::iterator itr;
	
	if (p_this_window->GetAttributes()->pin == true)
	{
		itr = m_show_list_pinned_window.begin();
		while (itr != m_show_list_pinned_window.end())
		{
			if (*itr == p_this_window)
			{
				m_show_list_pinned_window.erase(itr);
				m_show_list_pinned_window.push_front(p_this_window);
				break;
			}

			itr++;
		}
	}
	else
	{
		itr = m_show_list.begin();
		while (itr != m_show_list.end())
		{
			if (*itr == p_this_window)
			{
				m_show_list.erase(itr);
				m_show_list.push_front(p_this_window);
				break;
			}

			itr++;
		}	
	}
}

//-----------------------------------------------------------------------------
// Show
//
// m_show_list/m_show_list_pinned_window�� �ִ� Window�� Window::Show()�� �����Ѵ�.
//-----------------------------------------------------------------------------
void WindowManager::Show()
{
	CT_Done("WMS: ENTER show_list.size=%d pinned=%d", (int)m_show_list.size(), (int)m_show_list_pinned_window.size());
	{
		List::reverse_iterator enumItr;
		int k=0;
		for (enumItr = m_show_list.rbegin(); enumItr != m_show_list.rend(); ++enumItr, ++k)
			CT_Done("WMS-ENUM: list[%d] pWnd=%p vptr=%p name=\"%s\" xy=(%d,%d) topmost=%d", k, (void*)(*enumItr), *(void**)(*enumItr), (*enumItr)->GetWindowName().c_str(), (int)(*enumItr)->x, (int)(*enumItr)->y, (int)(*enumItr)->GetAttributes()->topmost);
		for (enumItr = m_show_list_pinned_window.rbegin(); enumItr != m_show_list_pinned_window.rend(); ++enumItr, ++k)
			CT_Done("WMS-ENUM: pinned[%d] pWnd=%p vptr=%p name=\"%s\" xy=(%d,%d)", k, (void*)(*enumItr), *(void**)(*enumItr), (*enumItr)->GetWindowName().c_str(), (int)(*enumItr)->x, (int)(*enumItr)->y);
	}
	List::reverse_iterator itr;
	int wmsIter = 0;

	itr = m_show_list.rbegin();
	while (itr != m_show_list.rend())
	{
		Window *pWindow = *itr;
		CT_Done("WMS: list iter %d pre Show pWnd=%p vptr=%p name=\"%s\" xy=(%d,%d)", wmsIter, (void*)pWindow, *(void**)pWindow, pWindow->GetWindowName().c_str(), (int)pWindow->x, (int)pWindow->y);
		if (pWindow->GetAttributes()->topmost == false)
		{
			char probeName[256];
			Phoenix_BuildWindowProbeName(probeName, sizeof(probeName), pWindow);
			Phoenix_FrameProbeNamedStageScope fpWindow("UI.Window", probeName, g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
			pWindow->Show();
		}
		CT_Done("WMS: list iter %d post Show", wmsIter); wmsIter++;

		itr++;
	}

	// pinned Window�� no pinned Window ���� ���� ����Ѵ�.
	itr = m_show_list_pinned_window.rbegin();
	while (itr != m_show_list_pinned_window.rend())
	{
		Window *pWindow = *itr;
//		(*itr)->Show();
		CT_Done("WMS: pinned iter %d pre Show pWnd=%p vptr=%p name=\"%s\" xy=(%d,%d)", wmsIter, (void*)pWindow, *(void**)pWindow, pWindow->GetWindowName().c_str(), (int)pWindow->x, (int)pWindow->y);
		{
			char probeName[256];
			Phoenix_BuildWindowProbeName(probeName, sizeof(probeName), pWindow);
			Phoenix_FrameProbeNamedStageScope fpWindow("UI.PinnedWindow", probeName, g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
			pWindow->Show();
		}
		CT_Done("WMS: pinned iter %d post Show", wmsIter); wmsIter++;

		itr++;
	}

	// show topmost
	itr = m_show_list.rbegin();
	while (itr != m_show_list.rend())
	{
		Window *pWindow = *itr;
		CT_Done("WMS: topmost iter %d pre Show pWnd=%p name=\"%s\" xy=(%d,%d)", wmsIter, (void*)pWindow, pWindow->GetWindowName().c_str(), (int)pWindow->x, (int)pWindow->y);
		if (pWindow->GetAttributes()->topmost == true)
		{
			char probeName[256];
			Phoenix_BuildWindowProbeName(probeName, sizeof(probeName), pWindow);
			Phoenix_FrameProbeNamedStageScope fpWindow("UI.TopmostWindow", probeName, g_hWnd, (int)g_bActiveApp, (int)g_bActiveGame);
			pWindow->Show();
		}
		CT_Done("WMS: topmost iter %d post Show", wmsIter); wmsIter++;

		itr++;
	}
	CT_Done("WMS: EXIT iters=%d", wmsIter);
}

//-----------------------------------------------------------------------------
// AppearWindow
//
// p_this_window�� show list�� ����Ѵ�. �̹� �Ǿ� ������ ���� node�� delete�ϰ�
// �ٽ� insert�Ѵ�.
//
// p_this_window�� ��ϵǾ� ���� ������ �ƹ��͵� ���� �ʴ´�.
//-----------------------------------------------------------------------------
void WindowManager::AppearWindow(Window * p_this_window)
{
	assert(p_this_window != NULL);

	// failed?
	if (p_this_window == NULL ||
		 AlreadyRegistered(p_this_window) == false)
		 return;

	Window * p_prev_first_window = GetFirstPriorityWindow();

	List::iterator itr;

	//
	// �̹� �����ϸ� �����Ѵ�.
	//

	if (p_this_window->GetAttributes()->pin == true)
	{
		itr = m_show_list_pinned_window.begin();
		while (itr != m_show_list_pinned_window.end())
		{
			if (*itr == p_this_window)
			{
				m_show_list_pinned_window.erase(itr);
				break;
			}

			itr++;
		}

		m_show_list_pinned_window.push_front(p_this_window);
	}
	else
	{
		itr = m_show_list.begin();
		while (itr != m_show_list.end())
		{
			if (*itr == p_this_window)
			{
				m_show_list.erase(itr);
				break;
			}

			itr++;
		}

		m_show_list.push_front(p_this_window);
	}

	//
	// cancel previous focused Window focus & focus new Window
	//
	Window * pC_prev_focused_window = m_pC_mouse_focused_window;
	SetMouseMoveFocusedWindow();
	if (pC_prev_focused_window != m_pC_mouse_focused_window)
	{
		if (pC_prev_focused_window != NULL)
		{
			pC_prev_focused_window->UnacquireMouseFocus();
			RunUnacquireMouseFocusHandler();
		}
		if (m_pC_mouse_focused_window != NULL)
		{
			m_pC_mouse_focused_window->AcquireMouseFocus();
			RunAcquireMouseFocusHandler();
		}
	}

	SendMouseMoveMessageToMouseFocusedWindow();

	SetNextTopmostWindow();

	// new first sequence Window detection?
	Window * p_first_window = GetFirstPriorityWindow();
	if (p_first_window != NULL && p_first_window != p_prev_first_window)
	{
		p_first_window->AcquireFirstSequence();
		SetKeyboardControlWindow(p_first_window);
	}
}

//-----------------------------------------------------------------------------
// DisappearWindow
//
// p_this_window�� show list���� delete�Ѵ�.
// p_this_window�� Disappear�Ǿ����� true�� ��ȯ�Ѵ�.
//-----------------------------------------------------------------------------
bool WindowManager::DisappearWindow(Window * p_this_window)
{
	assert(p_this_window != NULL);

	if (p_this_window == NULL)
		return false;

	if (m_pC_keydown_window==p_this_window)
	{
		m_pC_keydown_window = NULL;
	}
	if (m_pC_topmost_window == p_this_window)
	{
		m_pC_topmost_window = NULL;
	}

	Window * p_prev_first_window = GetFirstPriorityWindow();

	bool disappear_result = false;
	List::iterator itr;

	if (p_this_window->GetAttributes()->pin == true)
	{
		itr = m_show_list_pinned_window.begin();
		while (itr != m_show_list_pinned_window.end())
		{
			if (*itr == p_this_window)
			{
				p_this_window->ClearInputState();

				m_show_list_pinned_window.erase(itr);
				disappear_result = true;
				break;
			}

			itr++;
		}	
	}
	else
	{
		itr = m_show_list.begin();
		while (itr != m_show_list.end())
		{
			if (*itr == p_this_window)
			{
				p_this_window->ClearInputState();

				m_show_list.erase(itr);
				disappear_result = true;
				break;
			}

			itr++;
		}	
	}

	if (disappear_result == true)
	{
		// cancel previous focused Window focus & focus new Window
		Window * pC_prev_focused_window = m_pC_mouse_focused_window;
		SetMouseMoveFocusedWindow();
		if (pC_prev_focused_window != m_pC_mouse_focused_window)
		{
			if (pC_prev_focused_window != NULL)
			{
				pC_prev_focused_window->UnacquireMouseFocus();
				RunUnacquireMouseFocusHandler();
			}
			if (m_pC_mouse_focused_window != NULL)
			{
				m_pC_mouse_focused_window->AcquireMouseFocus();
				RunAcquireMouseFocusHandler();
			}
		}

		p_this_window->AcquireDisappear();
		RunAcquireDisappearHandler();

		SendMouseMoveMessageToMouseFocusedWindow();

		if (p_this_window == m_pC_pushed_window)
			m_pC_pushed_window = NULL;

		SetNextTopmostWindow();

		// new first sequence Window detection?
		Window * p_first_window = GetFirstPriorityWindow();
		if (p_first_window != NULL && p_first_window != p_prev_first_window)
			p_first_window->AcquireFirstSequence();

		//
		// DisappearWindow�� �ƴ� ��쿡�� Ȯ���� SetKeyboardControlWindow�� keyboard control Window��
		// ������ �� ������, Disappear �Ǵ� ��쿡�� ���������� �˻��Ͽ� �װ��� ã�ƾ� �Ѵ�.
		//
		SetNextKeyboardControlWindow();
	}

	return disappear_result;
}

//-----------------------------------------------------------------------------
// SetMouseMoveFocusedWindow
//
// �ֿ켱���� Window���� �˻��Ͽ� mouse (x, y)�� Window�� ��ġ�ϸ� move focused
// Window�� �����Ѵ�.
//-----------------------------------------------------------------------------
void WindowManager::SetMouseMoveFocusedWindow()
{
	List::iterator itr, endItr;

	// topmost first
	// "topmost Window�� pinned Window�� �ƴϴ�."
	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->GetAttributes()->topmost == true)
			if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
			{
				m_pC_mouse_focused_window = p_searched_window;
				return;
			}

		itr++;
	}

	itr = m_show_list_pinned_window.begin();
	endItr = m_show_list_pinned_window.end();
	while (itr != endItr)
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
		{
			m_pC_mouse_focused_window = p_searched_window;
			return;
		}

		itr++;
	}
	
	itr = m_show_list.begin();

	endItr = m_show_list.end();
	while (itr != endItr)
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->GetAttributes()->topmost == false)
		{
			if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
			{
				m_pC_mouse_focused_window = p_searched_window;
				return;
			}
		}			
		itr++;
	}

	m_pC_mouse_focused_window = NULL;
}

//-----------------------------------------------------------------------------
// GetFocusedWindow
//
// �ֿ켱���� Window���� �˻��Ͽ� mouse (x, y)�� Window�� ��ġ�ϸ� move focused
// Window�� �����Ѵ�.
//-----------------------------------------------------------------------------
Window* WindowManager::GetFocusedWindow(int x, int y)
{
	List::iterator itr;

	// topmost first
	// "topmost Window�� pinned Window�� �ƴϴ�."
	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->GetAttributes()->topmost == true)
			if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
			{
				return p_searched_window;
			}

		itr++;
	}

	itr = m_show_list_pinned_window.begin();
	while (itr != m_show_list_pinned_window.end())
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
		{
			return p_searched_window;
		}

		itr++;
	}
	
	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->GetAttributes()->topmost == false)
			if (p_searched_window->IsPixel(m_current_mouse_x, m_current_mouse_y))
			{
				return p_searched_window;
			}

		itr++;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// GetWindowbyName
//
// ������ �̸����� ���ϴ� �������� �����͸� ���´�.
//-----------------------------------------------------------------------------
Window* WindowManager::GetWindowbyName(const char* name)
{
	if (!strlen(name))
		return NULL;

	List::iterator itr;

	itr = m_show_list.begin();
	while (itr != m_show_list.end())
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->GetWindowName().compare(name) == 0)
		{
			return p_searched_window;
		}

		itr++;
	}

	itr = m_show_list_pinned_window.begin();
	while (itr != m_show_list_pinned_window.end())
	{
		Window * p_searched_window = (*itr);
		if (p_searched_window->GetWindowName().compare(name) == 0)
		{
			return p_searched_window;
		}

		itr++;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// MouseControl
//
// WindowManager::MouseControl�� App MouseControl�� Window::MouseControl��
// �߰����̴�.
//
// > Mouse pointer move�� mouse pointer�� � Window�� ����Ű�� focus �����¿�
//   ���� �Ѵ�. �̰��� �켱������ �������. ���� �켱������ ���� �ͺ���
//   �˻��Ѵ�.
//-----------------------------------------------------------------------------
bool WindowManager::MouseControl(UINT message, int x, int y)
{
	m_current_mouse_x = x;
	m_current_mouse_y = y;

	//
	// change move focused Window
	//
	if (message == M_MOVING)
	{
		// �̵��߿��� move focused Window �������� �ʴ´�.
		if (gpC_Imm && m_pC_mouse_focused_window != NULL && m_pC_mouse_focused_window->Moving())
			gpC_Imm->ForceUI(CImm::FORCE_UI_DRAG);

		if (m_pC_mouse_focused_window == NULL || m_pC_mouse_focused_window->Moving() == false)
		{
			//
			// Acquire/Unacquire mouse focus
			//
			Window * pC_prev_focused_window = m_pC_mouse_focused_window;

			SetMouseMoveFocusedWindow();

			if (pC_prev_focused_window != m_pC_mouse_focused_window)
			{
				if(gpC_Imm)
					gpC_Imm->ForceUI(CImm::FORCE_UI_WINDOW);
				if (pC_prev_focused_window != NULL)
				{
					pC_prev_focused_window->UnacquireMouseFocus();
					RunUnacquireMouseFocusHandler();
				}
				if (m_pC_mouse_focused_window != NULL)
				{
					m_pC_mouse_focused_window->AcquireMouseFocus();
					RunAcquireMouseFocusHandler();
				}
			}
		}
	}

	//
	// change Window priority
	// topmost Window�� ���� ��쿡�� �켱������ ������ �� ����.
	//
	if (message == M_LEFTBUTTON_DOWN || message == M_RIGHTBUTTON_DOWN)
	{
		if (m_pC_mouse_focused_window != NULL)
		{
			if (m_pC_topmost_window == NULL)
			{
				Window * p_prev_first_window = GetFirstPriorityWindow();

				FirstPriority(m_pC_mouse_focused_window);
				
				Window * p_first_window = GetFirstPriorityWindow();
				if (p_prev_first_window != p_first_window)
				{
					p_first_window->AcquireFirstSequence();
					SetKeyboardControlWindow(p_first_window);
				}

				// acquire mouse click
				if (m_pC_mouse_click_window != m_pC_mouse_focused_window)
				{
					m_pC_mouse_click_window = m_pC_mouse_focused_window;
					m_pC_mouse_click_window->AcquireMouseClick();
					SetKeyboardControlWindow(m_pC_mouse_click_window);
				}
			}

			// set pushed Window
			m_pC_pushed_window = m_pC_mouse_focused_window;
		}
	}

	// focused Window���� ��� �Է��� �ش�.
	if (m_pC_mouse_focused_window != NULL)
	{
		if (m_pC_topmost_window != NULL)
		{
			if (m_pC_topmost_window == m_pC_mouse_focused_window)
				m_pC_topmost_window->MouseControl(message, x, y);
		}
		else
		{
			m_pC_mouse_focused_window->MouseControl(message, x, y);

			//â���� statch
			if(message == M_MOVING && m_pC_mouse_focused_window->Moving())
			{
				if(m_pC_mouse_focused_window->GetAttributes()->statch)
				{
					List::const_iterator itr;
					
					for(int k = 0; k < 2; k++)
					{
						if(k == 0)
							itr = m_show_list_pinned_window.begin();
						else if(k == 1)
							itr = m_show_list.begin();
						
						while (k ==0 && itr != m_show_list_pinned_window.end() || k ==1 && itr != m_show_list.end())
						{
							//						m_pC_mouse_focused_window->UnStatch();
							Window * p_searched_window = (*itr);

							// 2026-09-03 minimap titreme fix — surklenen
							// pencerenin grup takipcisi snap hedefi olamaz
							// (aciklama: u_window.h GetStatchIgnoreWindow).
							if (p_searched_window == m_pC_mouse_focused_window->GetStatchIgnoreWindow())
							{
								itr++;
								continue;
							}

							bool bl_statch = false;
							
							//���� ������ ������� ���Ե������� ���θ� ���ܼ� ���δ�
							if(m_pC_mouse_focused_window->y < p_searched_window->y+p_searched_window->h &&
								m_pC_mouse_focused_window->y+m_pC_mouse_focused_window->h > p_searched_window->y)
							{
								//���ʿ� ������ �ֳ�?
								if (m_pC_mouse_focused_window->x > p_searched_window->x+p_searched_window->w-STATCH_VALUE &&
									m_pC_mouse_focused_window->x < p_searched_window->x+p_searched_window->w+STATCH_VALUE)
								{
									m_pC_mouse_focused_window->x = p_searched_window->x+p_searched_window->w;
									bl_statch = true;
								}
								else //�����ʿ� ������ �ֳ�?
									if (m_pC_mouse_focused_window->x+m_pC_mouse_focused_window->w > p_searched_window->x-STATCH_VALUE &&
										m_pC_mouse_focused_window->x+m_pC_mouse_focused_window->w < p_searched_window->x+STATCH_VALUE)
									{
										m_pC_mouse_focused_window->x = p_searched_window->x-m_pC_mouse_focused_window->w;
										bl_statch = true;
									}
									
									if(bl_statch)	//��� �پ��ٸ� �� ���� ���� ���ϼ� �ֳ� ����
									{
										//������ ��ǥ�� ��������
										if(m_pC_mouse_focused_window->y > p_searched_window->y-STATCH_VALUE &&
											m_pC_mouse_focused_window->y < p_searched_window->y+STATCH_VALUE)
										{
											m_pC_mouse_focused_window->y = p_searched_window->y;
										}
										else	//�Ʒ����� ��ǥ�� ��������
											if(m_pC_mouse_focused_window->y+m_pC_mouse_focused_window->h > p_searched_window->y+p_searched_window->h-STATCH_VALUE &&
												m_pC_mouse_focused_window->y+m_pC_mouse_focused_window->h < p_searched_window->y+p_searched_window->h+STATCH_VALUE)
											{
												m_pC_mouse_focused_window->y = p_searched_window->y+p_searched_window->h-m_pC_mouse_focused_window->h;
											}
									}
									
							}
							
							bl_statch = false;
							//���� ������ ������� ���Ե������� ���θ� ���ܼ� ���δ�
							if (m_pC_mouse_focused_window->x < p_searched_window->x+p_searched_window->w &&
								m_pC_mouse_focused_window->x+m_pC_mouse_focused_window->w > p_searched_window->x)
							{
								//���ʿ� ������ �ֳ�?
								if (m_pC_mouse_focused_window->y > p_searched_window->y+p_searched_window->h-STATCH_VALUE &&
									m_pC_mouse_focused_window->y < p_searched_window->y+p_searched_window->h+STATCH_VALUE)
								{
									m_pC_mouse_focused_window->y = p_searched_window->y+p_searched_window->h;
									bl_statch = true;
								}
								else //�Ʒ��ʿ� ������ �ֳ�?
									if (m_pC_mouse_focused_window->y+m_pC_mouse_focused_window->h > p_searched_window->y-STATCH_VALUE &&
										m_pC_mouse_focused_window->y+m_pC_mouse_focused_window->h < p_searched_window->y+STATCH_VALUE)
									{
										m_pC_mouse_focused_window->y = p_searched_window->y-m_pC_mouse_focused_window->h;
										bl_statch = true;
									}
							}
							if(bl_statch == true)	//��� �پ��ٸ� �� ���� ���� ���ϼ� �ֳ� ����
							{
								//������ ��ǥ�� ��������
								if(m_pC_mouse_focused_window->x > p_searched_window->x-STATCH_VALUE &&
									m_pC_mouse_focused_window->x < p_searched_window->x+STATCH_VALUE)
								{
									m_pC_mouse_focused_window->x = p_searched_window->x;
								}
								else	//�������� ��ǥ�� ��������
									if(m_pC_mouse_focused_window->x+m_pC_mouse_focused_window->w > p_searched_window->x+p_searched_window->w-STATCH_VALUE &&
										m_pC_mouse_focused_window->x+m_pC_mouse_focused_window->w < p_searched_window->x+p_searched_window->w+STATCH_VALUE)
									{
										m_pC_mouse_focused_window->x = p_searched_window->x+p_searched_window->w-m_pC_mouse_focused_window->w;
									}
							}
							
							itr++;
						}
					}
				}

				// event
				m_pC_mouse_focused_window->WindowEventReceiver(Window::EVENT_WINDOW_MOVE);
				
			
			}
		}
	}

	//
	// clear mouse push state.
	//
	if (message == M_LEFTBUTTON_UP ||
		 message == M_RIGHTBUTTON_UP)
	{
		if (m_pC_pushed_window != NULL)
		{
			if (m_pC_pushed_window != m_pC_mouse_focused_window)
				m_pC_pushed_window->MouseControl(message, x, y);

			m_pC_pushed_window = NULL;
		}

		// FAZ 4 (2026-05-03): Drag-release fix. Subclass MouseControl
		// (INVENTORY, GEAR, INFO vb.) M_LEFTBUTTON_UP'ta MoveOk()
		// çağırmıyordu — sadece CancelPushState. Sonuç: window mouse'a
		// 'yapışıp' kalıyor, ikinci tıklama gerekiyor.
		// Merkezi fix: UP message'ında ek olarak focused window'un
		// MoveOk()'ini çağır → drag state reset garantili.
		if (m_pC_mouse_focused_window != NULL)
		{
			m_pC_mouse_focused_window->MoveOk();
		}
	}

	return (m_pC_mouse_focused_window != NULL);
}

#include "VS_UI.h"
//-----------------------------------------------------------------------------
// KeyboardControl
//
// WindowManager::KeyboardControl�� App KeyboardControl�� Window::KeyboardControl�� 
// �߰����̴�.
//-----------------------------------------------------------------------------
void WindowManager::KeyboardControl(UINT message, UINT key, long extra)
{
//	static Window * m_pC_keydown_window;

	if (message == WM_KEYDOWN)// || message == WM_IME_STARTCOMPOSITION)
	{
		m_pC_keydown_window = NULL;
		if (m_pC_topmost_window != NULL)// && m_pC_topmost_window->GetAttributes()->keyboard_control == true)
			m_pC_keydown_window = m_pC_topmost_window;
		else if (m_pC_keyboard_control_window != NULL)
			m_pC_keydown_window = m_pC_keyboard_control_window;
	}

	//
	// �켱����, (1) topmost Window
	//			 	 (2) keyboard control Window
	//
	if (m_pC_topmost_window != NULL)
	{
//		if (message == WM_KEYDOWN)
//		{
//			if (m_pC_topmost_window != m_pC_keydown_window)
//				return;
//		}

		if (m_pC_topmost_window == m_pC_keydown_window)
		{

		//if(key != VK_ESCAPE && gC_vs_ui.IsGameMode() || !gC_vs_ui.IsGameMode())
		if(m_pC_topmost_window->GetAttributes()->keyboard_control == true || gC_vs_ui.IsEmptyChatting() && key == VK_RETURN || !gC_vs_ui.IsGameMode() && key == VK_ESCAPE
#if __CONTENTS(__GAMEOPTION_ITEMBAY_MESSAGEBOX)
			|| gC_vs_ui.IsEmptyChatting() && key == VK_LEFT || gC_vs_ui.IsGameMode() && key == VK_RIGHT
#endif // __GAMEOPTION_ITEMBAY_MESSAGEBOX
			)
		{
			m_pC_topmost_window->KeyboardControl(message, key, extra);
			return;
		}

		//���Ӹ�尡 �ƴҶ��� �Ʒ��� ������ �ʿ䰡 ����
		if(!gC_vs_ui.IsGameMode())
			return;
		}
	}

	// topmost window�� ���������� ä�ø԰� �ϱ� ���ؼ�
	if (m_pC_keyboard_control_window != NULL && 
		m_pC_keyboard_control_window != m_pC_topmost_window)
		m_pC_keyboard_control_window->KeyboardControl(message, key, extra);

//	if (m_pC_keydown_window != NULL)
//	{
//		if (message == WM_KEYDOWN)
//		{
//			if(m_pC_keyboard_control_window != m_pC_keydown_window && 
//				m_pC_keydown_window->GetAttributes()->keyboard_control == true ||
//				m_pC_keyboard_control_window == m_pC_topmost_window ||
//				m_pC_keyboard_control_window == NULL)
//				return;
//			if (m_pC_keyboard_control_window != m_pC_keydown_window)
//				return;
//		}

//		if(
//			m_pC_topmost_window != NULL && 
//			m_pC_topmost_window->GetAttributes()->keyboard_control == true ||
//			m_pC_keyboard_control_window == m_pC_topmost_window ||
//			m_pC_keyboard_control_window == NULL
//			)
//			return;

//		if (m_pC_keyboard_control_window != m_pC_keydown_window)
//			return;

//	}
}

//-----------------------------------------------------------------------------
// AlreadyRegistered
//
// p_window�� ��ϵǾ��°�?
//-----------------------------------------------------------------------------
bool WindowManager::AlreadyRegistered(Window * p_window) const
{
	return Find(p_window);
}

//-----------------------------------------------------------------------------
// GetShowState
//
// 
//-----------------------------------------------------------------------------
bool WindowManager::GetShowState(Window * p_window) const
{
	if (p_window == NULL)
		return false;

	List::const_iterator itr;

	if (p_window->GetAttributes()->pin == true)
	{
		itr = m_show_list_pinned_window.begin();
		while (itr != m_show_list_pinned_window.end())
		{
			if (*itr == p_window)
				return true;

			itr++;
		}
	}
	else
	{
		itr = m_show_list.begin();
		while (itr != m_show_list.end())
		{
			if (*itr == p_window)
				return true;

			itr++;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Register
//
// Window�� ����Ѵ�.
// �̹� ��ϵǾ��°� �˻��� �ʿ䰡 ����.
//-----------------------------------------------------------------------------
void WindowManager::Register(Window * p_window)
{
	if (p_window == NULL)
		_Error(NULL_REF);

	Add(p_window);
}

//-----------------------------------------------------------------------------
// Unregister
//
// Window ����� �����Ѵ�.
//-----------------------------------------------------------------------------
void WindowManager::Unregister(Window * p_window)
{
	if (p_window == NULL)
		_Error(NULL_REF);

	Delete(p_window);
	// Modern portta register/show list drift olursa pencere kaydi silinemese bile
	// gorunur listeden dusur. Aksi halde title/char-create gibi eski pencereler
	// oyun frame'inde her kare cizilmeye devam edebilir.
	DisappearWindow(p_window);

	if (p_window == m_pC_pushed_window)
		m_pC_pushed_window = NULL;
}

//-----------------------------------------------------------------------------
// Process
//
//
//-----------------------------------------------------------------------------
void WindowManager::Process()
{
}

//-----------------------------------------------------------------------------
// g_RegisterWindow
//
// Window�� Window Manager�� ����Ѵ�.
//-----------------------------------------------------------------------------
void g_RegisterWindow(Window * p_window)
{
	if (gpC_window_manager == NULL)
		_ErrorStr("WindowManager not initialized.");
	if (p_window == NULL)
		_Error(NULL_REF);

	gpC_window_manager->Register(p_window);
}

//-----------------------------------------------------------------------------
// g_UnregisterWindow
//
// Window Manager���� Window�� ��������Ѵ�.
//-----------------------------------------------------------------------------
void g_UnregisterWindow(Window * p_window)
{
	if (gpC_window_manager == NULL)
		_ErrorStr("WindowManager not initialized.");
	if (p_window == NULL)
		_Error(NULL_REF);

	gpC_window_manager->Unregister(p_window);
}	
