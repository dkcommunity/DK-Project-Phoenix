/*-----------------------------------------------------------------------------

	u_window.h

	Window property base class.

	2000.8.14. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __U_WINDOW_H__
#define __U_WINDOW_H__

#pragma warning(disable:4786)

// FAZ 4 (2026-04-29 Phase B): ContentsFilter.h zorunlu — Window'in
// ATTRIBUTES struct'inda __CONTENTS(__GPS_ADD) guard'i (L196-198) var
// (gpsview field). Layout uyumu icin her cpp doğru macro state ile
// gormeli.
#include "ContentsFilter.h"

#include "Basics.h"
#include "PI.h"
#include <list>

#include "u_edit.h"

#define SCR2WIN_X(screen_x)			((screen_x)-x)
#define SCR2WIN_Y(screen_y)			((screen_y)-y)
#define WIN2SCR_X(win_x)				((win_x)+x)
#define WIN2SCR_Y(win_y)				((win_y)+y)

enum MOUSE_STATE
{
	MOUSE_NOSTATE,
	MOUSE_MOVE,
	MOUSE_LEFTDOWN,
	MOUSE_LEFTUP,
	MOUSE_RIGHTDOWN,
	MOUSE_RIGHTUP,
};

//-----------------------------------------------------------------------------
// Window
//
// ����:
//
//		Window�� ������ �ϳ��� �۾������̴�. �ϳ��� Multitasking�� ������ �� ����
//		background���� �۾��ϴ� process�� ������ ����ڿ��� �ڽ��� ���̸� �۾��ϴ�
//		Process�� �ִ�. �� �� �װ��� �ٸ� Process�� ������ �� �־�� �ϴµ� Window��
//		�̰��� �ϱ⿡ �����ϴ�. Window�� ��ü�ν� �ѷ��ϰ� ���̱� �����̴�.
//		Window��� �� �簢�� �ʿ�� ����. �ϳ��� �۾��������ν� �������� �׸��̴�.
//		�׸��� ���� �̰��� ����ڿ��� �ϳ��� �Ϻ��� �۾��������ν� ����� ���ؾ�
//		�Ѵ�.
//
//
// ���:
//
//		Window�� �ڱ� �ڽ��� ���� Process�� ���´�. �׸��� �ڽ��� �������ϸ�(Show)
//		����ڷκ��� ��� �޾ƾ��Ѵ�(Control). �� �������� �ٽɱ���̴�.
//
//		Process
//
//		�ڽ��� �۾��� ���������� ���Ѵ�. �̰��� ���̴� �Ͱ� �Է¹޾ƾ� �ϴ� �Ϳ�
//		������ �и��Ǿ� �ִ�. ���� �����ϰ� �۾��� ���ؼ� ����ȴ�.
//
//		Show
//
//		Process ����� ���̴� �� �Ǵ� ������¸� ���̴� �� �׸��� Window ��ü�� ���̴�
//		���� �����Ѵ�. '���̴� ��'�� �ӹ��̴�.
//
//		Control
//
//		����ڴ� ��ǻ�� ��ü�� �Է���ġ�� �Է��� �ϵ�, Window�� �Է��� ���Ѵ�. 
//		���� �̰��� �ڽ��� Window���Ը�(���� �۾�����) ���޵ȴ�.
//
//-----------------------------------------------------------------------------
//
// < Rect�� ��ӹ��� �Ϳ� ���� >
//
// Rect�� base class�� �Ѵ�. Window��� �Ӽ��� �� ���°� ���̴� �ﰢ���̵� �簢����
// ���� �� �ִ�. �簢���°� �ƴϸ� ��� Rect.w�� Rect.h�� �� �ǹ̰� �������� ������,
// ������ Ȯ���� �����ִ� �ǹ̿����� �ʿ��ϴ�.
//
// < Window �Ӽ� >
//
// Window�� �Ӽ��� ������ �� �ִ�. �ƹ��͵� �������� ������ �⺻���� ���ȴ�. 
// Window �̵��ϱ⵵ �ϳ��� �Ӽ��̴�. Window �Ӽ��� ������ Window�� ���� �� �ִ� 
// ���̴�. "�� Window�� ������ �ֿ켱������ ���´�"�� �Ӽ��� ��ü Window �� �ϳ���
// ���� �Ӽ��̹Ƿ� �̰��� WindowManager���� �ٷ��.
// Window ������ �����ϴ� �Ӽ��� Local attributes��� �ϰ� WindowManager�� ���ִ�
// �Ӽ��� Global attributes�� ����.
// �Ӽ��� ������ �ƹ����� �ص��Ǹ�, ������ ���ĺ��� ȿ���� ��Ÿ����.
//
// < Window �̵��ϱ� >
//
// ���°� �����ο�Ƿ� Window�� �̵��ϱ� ���ؼ� �����ϴ� point�� �ܺο��� �����ؾ�
// �Ѵ�. �װ��� IsPixel()�� test�� true�� ������쿩�� �Ѵ�. �׸��� MoveReady()�� 
// ������ �� SetOrigin()���� point�� �����Ѵ�. �� ���� ó���� Window::MouseControl�� 
// ó���Ѵ�.
//
// < Event ���� >
//
// Window�� �̵��ϰų� resize�ǰų� �� ���� �� event�� �߻��� ���� �ٷ� ������
// �Ѵ�. ������ �� �ʿ䰡 �ִ� �� �κ��� �ִ�. �ϳ��� ��ӹ��� ��ü���Է� �׸��� 
// �ٸ� �ϳ��� App����. App�� �� ����� �� �ʿ䰡 ������? App�� Window�� �ٷ��
// �ϳ��� WindowManager System�̶�� �׷����� �ְڴ�. ��ӹ��� ��ü���Դ� �� �����
// �� �ʿ䰡 �翬�� �ִ�. �׷��� Event �߻��� ��ٷ� �����Ѵ�. �̰� ������ �ڱ�
// ��ü�� method�� ���������ν� �����ϴ�. App���� �˸� ���� WindowManager��
// ���ϵ��� �Ѵ�.
//
// WindowManager���� event�� �˸��� ����� ��� �����ұ�? WindowManager�� App��
// event receiver�� ������ �� �� �ִ�. Window�� ������ ��ü�̰� �ڽŹۿ� �𸣹Ƿ�
// WindowManager�� ���������� �� ����� �˷��� �ؾ��Ѵ�. Window�� event�� WindowManager��
// �˻��ؾ� �Ѵٴ� ���̴�. �˻��� �����ϸ� �ڽŵ� �� event�� �˰� App���� ���� ��
// �� �ִ�.
//
// < ��ü���� ��� ��� >
//
// Window�� ��ӹ��� class�� pure virtual�� ���� ������ �ؾ��Ѵ�. �׸��� MouseControl����
// Window::MouseControl�� �����ؾ� �Ѵ�.
// Process, Show, MouseControl, KeyboardControl�� ���� WindowManager�� ���� ����ȴ�.
// WindowManager�� App�� Window ���� �߰迪Ȱ�� �����Ѵ�. IsPixel�� �� Window ��ġ��
// �׸��� Window ���¸� �����Ͽ� ������ (x, y)�� �� Window�� pixel�� �����ϴ��� test�Ѵ�.
// �̰��� pure virtual�ν� �������� ���¿� ���� Window focus�� ������Ų��.
//
// Window�� �������� ��ü�ν� �����ϸ� ���� �ڽŸ� �˻��̰�, ���� �ܺ��� ������ 
// ������ ����� ���� ����� ���̴�. �̰��� �ٷ� Window�̴�.
//
//
// * ���ǻ���
//
//		Rect�� base class�� �ϹǷ� ������ (x, y, w, h)�� Rect�� �̸���	���� �ʾƾ� 
//		�Ѵ�. �� Window class�� base�� �ϴ� ��쵵 ���� ���������̴�.
//
//
// [2000.8.21]
//
//	Size ������ ���� ���� �������� ����. �����ڵ常 ���� ����.
//
//
// [2000.10.13]
//
// < Window ���� �ٸ� Widget ����ϱ� >
//
// Window���� Widget�� Window ������ ��µȴ�. �׷��� �׵��� �����Ǽ� ��� Window��
// ���� ���ΰ��� �����ؾ߸� �Ѵ�. �װ��� Window�� << �����ڿ� ���ؼ� �����ȴ�.
//
//-----------------------------------------------------------------------------
class Window : public Rect, public PI_Visual
{
private:
	//
	// m_bl_window_move_ready
	//
	// �Ϲ������� �ܺο��� Window�� ���¸� �ľ��ϰ� Window�� �̵��ϱ� ����
	// �����ϴ� ���̴�. focus ok, push ok�� ������ �� ������, �� ��� �Ϳ� ����
	// ���� �� �ϳ��� ǥ���Ѵ�. �̰��� true�� �� �� Move�� ������ �� �ִ�.
	//
	bool							m_bl_window_move_ready;

	Point							m_origin;

	//
	// m_u_mouse_state
	//
	// Window�� Mouse event�� �߻��ϸ� �� ���� flag�� �����Ǿ� WindowManager�� �� ���� 
	// ������ �� �ֵ��� �Ѵ�. WindowManager�� �� ���� �����ϸ�, MOUSE_NOSTATE�� �ȴ�.
	//
	// WindowManager Process�� UpdateInput �����̳� ������ ���� ���̱� ������ 
	// ��ġ�� ���(���� ������ ��)�� ������ �ʿ䰡 ����.
	//
	MOUSE_STATE						m_u_mouse_state;

protected:
	// 
	// m_window_name
	//
	// ���� �������� �̸�
	std::string						m_window_name;

public:
	//
	// Local attributes flags
	//
	enum ATTRIBUTES_HIDE
	{
		ATTRIBUTES_HIDE_NOT,
		ATTRIBUTES_HIDE_WIDTH,
		ATTRIBUTES_HIDE_HEIGHT,
	};

	struct ATTRIBUTES
	{
		bool				window_move;		// Window�� movement enable/disable.
		bool				pin;					// pin�� ������ �Ȳ��� Window���� �켱������ ������.
		bool				topmost;				// topmost Window�̴�. pin �Ӽ��� disable�ȴ�.
		bool				keyboard_control;	// Window�� activation�Ǹ� keyboard control�� �޴´�.
		bool				alpha;
		bool				statch;
		ATTRIBUTES_HIDE		autohide;
#if __CONTENTS(__GPS_ADD)
		bool				gpsview;
#endif //__GPS_ADD
	};

private:
	ATTRIBUTES					m_attributes;

	virtual void	Init();

public:
	//
	// Event ���� 
	//
	virtual void WindowEventReceiver(id_t event) {}

	enum // event list
	{
		//EVENT_WINDOW_CREATE,
		//EVENT_WINDOW_DESTROY,
		EVENT_WINDOW_MOVE,
		EVENT_WINDOW_CREATE,
	};

public:
	Window(int _x, int _y, int _w, int _h);
	Window();
	virtual ~Window();

	virtual bool MouseControl(UINT message, int _x, int _y);
	virtual void KeyboardControl(UINT message, UINT key, long extra);
	virtual bool IsPixel(int _x, int _y) = 0;

	virtual void AcquireMouseFocus() {}		// mouse focus�� ������ ��.
	virtual void UnacquireMouseFocus() {}	// mouse focus�� ������ ��.

	// �ֿ켱 Window�� �Ǿ��� ��. topmost Window�� �ְ� Window�� topmost Window�� �ƴϸ� 
	// ������� �ʴ´�. (topmost Window�� �ֿ켱 Window�̹Ƿ�)
	virtual void AcquireFirstSequence() {} 

	// mouse�� click�� ��.
	// AcquireFirstSequence�� pinned Window�� ���� ���� �װ����θ� ���� ������ 
	// click�Ͽ� �Է¹ޱ� ���� �� ��� �̰��� �ʿ��ϴ�.
	virtual void AcquireMouseClick() {}		

	virtual void CancelPushState() {}		// mouse ������ ������ ��.

	virtual void AcquireDisappear() {}		// DisappearWindow�� ȣ��� ��.

	// 2026-09-03 minimap titreme fix — pencereden-pencereye statch (yapisma)
	// taramasinda YOK SAYILACAK pencere (grup takipcisi). Takipci pencere,
	// lider tasinirken EVENT_WINDOW_MOVE ile sabit offset'te birlikte hareket
	// eder; snap HEDEFI olursa WindowManager her M_MOVING'de lideri takipcinin
	// kenarina geri yapistirir (STATCH_VALUE'luk testere disi salinimi =
	// kullanicinin gordugu titreme). Default: yok sayilan pencere yok.
	virtual Window* GetStatchIgnoreWindow() { return NULL; }

	MOUSE_STATE	GetMouseInputState();

	//
	// Local attributes 
	//
	void	InitAttributes();
	void	AttrTopmost(bool state);
	void	AttrKeyboardControl(bool state);
	void	AttrWindowMove(bool new_state);
	void	AttrPin(bool new_state);
	void	AttrAlpha(bool new_state);
	void	AttrAutoHide(ATTRIBUTES_HIDE new_state);
	void	AttrStatch(bool new_state);
	void	ProcessHide(int gap = 0, bool alpha_window = false);
#if __CONTENTS(__GPS_ADD)
	void	AttrGPS(bool new_state);
#endif	//__GPS_ADD
	const Window::ATTRIBUTES *	GetAttributes() const { return &m_attributes; }

	//
	// Window moving
	//
	bool	Moving() const { return m_bl_window_move_ready; }
	void	MoveReady();
	void	SetOrigin(int ox, int oy);
	void	GetOrigin(Point &orgin) const { orgin = m_origin; }

private:
	bool	Move(int new_ox, int new_oy); // ���� ���ο� ���ǵ� InputControl�� ���ؼ� ����ȴ�.

public:
	// FAZ 4 (2026-05-03): WindowManager::MouseControl M_LEFTBUTTON_UP merkezi
	// drag-release fix bunu çağırıyor — public erişim gerek.
	void	MoveOk();
	void	ClearInputState();

	void					SetWindowName(const char* name) { m_window_name = name; }
	const std::string		GetWindowName() { return m_window_name; }

	//
	// Place other Widget.
	//
private:
	SimpleDataList<LineEditorVisual *>		m_sdl_lev;

public:
	void	ShowWidget() const;
	void	Attach(LineEditorVisual * p_lev);
};

//-----------------------------------------------------------------------------
// WindowManager
//
// �ϳ��� Window�� �����Ǹ� �� Window�� WindowManager�� ��ϵǾ� �����ȴ�.
// �������� Window ����, Window �켱���� ���� �ִ�.
//
//
// < �Ӽ� >
//
// ��ü Window �߿��� � Window�� ���� �Ӽ��� ������ �� �ִ�.
//
//------------------------------------------------------------------------------
// < Focused Window >
//
// Focused Window�� �� ������ �з��ȴ�. �ϳ��� Mouse�� Window ���� �̵��Ͽ� ��
// �����Ǵ� Mouse focused Window, �ٸ� �ϳ��� Ű�����Է��� ����Ͽ� �Է��� ����
// �� �ִ� Acquired Window�̴�.
//
// Focused Window:
//
// �ֿ켱 Window�� �ƴϴ��� Mouse pointer�� �̵��Ͽ� ��� Window�� scanning�� 
// �� �ִ�. �̰��� ���������� �Է��ۿ��� �Ͼ�� Window�� ���� ��� ó���� �Ͼ��
// �ʰ� �ϴ� �������� ������ ���̴�. �̰��� �ߴٰ� Window�� ������ �ʴ´�.
// �ݸ� scanning�� ���ÿ� Window �ݱ��ư�� Ŭ���ϹǷν� Window�� ���¸� ���ϰ��ϴ�
// ���� �� ���� Window�� Acquired�ȴٴ� �����Ͽ� �̷������ ���̴�.
// �׷��Ƿ� focused Window�� Mouse move���� ���������� �޴°��̴�. �ٸ� ���� ����.
//
//    - Mouse focus�� acquire/unacquire(set/unset)�Ǹ� Window�� ���ǵ� �Ʒ��� �� method��
//		  �����Ѵ�.
//
//		  o AcquireMouseFocus()
//		  o UnacquireMouseFocus()
//
//      �׷��� Mouse pointer�� �̵� �ܿ� ������ ���� Mouse focus�� set/unset�� �����ؾ�
//      �� ���� �ִ�.
//
//      o AppearWindow�� ����� �� - focused Window ���� �� Window�� ��µ� �� ����.
//		  o DisappearWindow�� ����� �� - focused Window�� disappear ��.
//		  o Unregister�� ����� �� - focused Window�� unregister ��.
//		  * Mouse pointer Move
//
//
// Acquired Window:
//
// Acquired Window�� �� �ϳ��� Window�ν� Ű�����Է°� ���� Ư���� �ۿ��� ����ϴ�
// ���̴�. Acquired�Ǿ��ٴ� ���� �װ��� �ֿ켱���� �������� ���� ����. �ϴ���
// Ư���� Window�� ��������. �ֳ��ϸ� Ű�����Է��� �޴� Window�� ������ �� �ֱ�
// �����̴�. �̰��� Acuqired Window�� �ƴϿ��� Ű�����Է��� ���� �� ������ �ǹ��Ѵ�.
//
// 
//-----------------------------------------------------------------------------
typedef std::list<Window *> List;

class WindowManager : public SimpleDataList<Window *>
{
private:
	//
	// handler
	//
	// �������� ó�������� handler�� �����Ѵ�.
	//
	void							(*m_fp_handler_acquire_mousefocus)(void);
	void							(*m_fp_handler_unacquire_mousefocus)(void);
	void							(*m_fp_handler_acquire_disappear)(void);

	//
	// (m_current_mouse_x, m_current_mouse_y)
	//
	// Window�� ������ų� ��Ÿ�� ��, mouse move focus Window�� �ٲ� �� �ִ�.
	// �� �� ���ο� Window�� ã�� ���� �ʿ��ϴ�.
	//
	int							m_current_mouse_x, m_current_mouse_y;

	//
	// m_pC_mouse_focused_window
	// 
	// �켱������ ������ ������� option(Mouse move�� ���õǵ� click�Ǽ� ���õǵ�)��
	// �ðܾ߰�����, ���������δ� ����ڰ� �˵縻�� Mouse move focus�� �˻��Ѵ�.
	//
	// �̰��� ����ڴ� �ٸ� Window�� click���� �ʰ��� Mouse pointer�� �̵��غ� �� 
	// �ֱ� �����̴�. �̴� �� Window�� �˻��ϴ� �ǹ̷� �޾Ƶ��� �� �ִ�.
	// �׷��� �����ϱ� ���ؼ��̱⵵������, ������ Mouse move focus�� Window ��ȯ����
	// ���� ���� �ְ� �� �ɷ� �پ��� option�� ������� �ֱ� �����̴�.
	//
	// m_pC_mouse_focused_window�� ���� move focus�� Window�� pointer�̴�. �̰��� �ٸ�
	// ������ �ٲ� ���� reset�ȴ�. �̰��� NULL�̸� move focus�� Window�� ���ٴ� ���̴�.
	//
	Window *						m_pC_mouse_focused_window;
	Window *						m_pC_keydown_window;

	//
	// m_pC_pushed_window
	//
	Window *						m_pC_pushed_window;

	//
	// m_pC_keyboard_control_window
	//
	// m_pC_keyboard_control_window�� ��µǰ� �ִ� Window�̴�.
	//
	// topmost Window�� �ƴ� Window �� �߿��� Keyboard control�� �޴� Window�̴�.
	// topmost Window�� ������ ��� Window�� keyboard �Է¹���� �̰��̴�.
	// Window �Ӽ����� keyboard_control�� �����Ǿ� ������ AcquireFirstSequence�� AcquireMouseClick��
	// ����� �� �� Window�� m_pC_keyboard_control_window�� �����Ѵ�.
	//
	Window *						m_pC_keyboard_control_window;

	//
	// m_show_list
	//
	// ���̴� Window�� �켱������ �����ϱ� ���ؼ� list�� ����Ѵ�.
	//
	List							m_show_list;

	//
	// m_show_list_pinned_window
	//
	// Window�� pin �Ӽ��� �����Ǹ� m_show_list�� ������� �ʰ� �� list�� ����ȴ�.
	// ��, �� list�� pin �Ӽ��� ������ Window�� ����ȴ�. m_show_list�� pin �Ӽ���
	// �������� ���� Window���� ����ȴ�. �̷��� �����ν� pinned Window�� no pinned Window��
	// ������ �� �ִ�. pinned Window�� no pinned Window�� ���� ��µȴ�.
	// pinned Window�� �켱������ pinned Window���� �����ϰ� �ȴ�.
	//
	List							m_show_list_pinned_window;

	//
	// m_pC_topmost_window
	//
	// topmost Window�� ��µ� Window�� �� ���� ���� �߸� �� Window�ܿ��� ���콺�� Ű������
	// ��� �Է��� �ڽŸ��� �����ϴ� Window�̴�.
	//---------------------------------------------------------------------------------------
	// m_pC_topmost_window�� ��µǰ� �ִ� Window�̴�.
	//
	// topmost Window�� ������ ������ �� �ִ�. �ϳ��� topmost Window�� �� �ִ� �� �Ǵٸ�
	// topmost Window�� �� �� �ִٴ� ���̴�. �̷��� �Ǿ��� �� ������� �ֿ켱�������� ��������̴�.
	// m_pC_topmost_window�� �׵� �� ������ ������� ������ �ִ� Window�̴�. �׷��Ƿ� �������
	// �ٲ� ���� m_pC_topmost_window�� �ٲ���� �ϴ� ���̴�.
	// �������� Window�� topmost ������ Window ������ save�ȴ�. �׵��� �����Ͽ� ��� ������
	// topmost Window�ν� m_pC_topmost_window�� �����Ѵ�.
	//
	Window *						m_pC_topmost_window;

	//
	// m_pC_mouse_click_window
	//
	// topmost Window�� ���� ��츦 ������ � Window�� click�Ͽ��� �� �����ȴ�.
	// ���� Window�̸� �缳������ �ʴ´�.
	//
	Window *						m_pC_mouse_click_window;

	int							m_old_origin_x;
	int							m_old_origin_y;
	int							m_res_x, m_res_y; // resolution

	enum DIR
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		DIRECTION,
	};

	void	SetNextTopmostWindow();
	void	SetNextKeyboardControlWindow();
	void	SetKeyboardControlWindow(Window * p_window);

	void	RunAcquireMouseFocusHandler() { if (m_fp_handler_acquire_mousefocus) m_fp_handler_acquire_mousefocus(); }
	void	RunAcquireDisappearHandler() { if (m_fp_handler_acquire_disappear) m_fp_handler_acquire_disappear(); }
	void	RunUnacquireMouseFocusHandler() { if (m_fp_handler_unacquire_mousefocus) m_fp_handler_unacquire_mousefocus(); }

public:
	WindowManager();
	~WindowManager();

	//
	// Window register/Unregister
	//
	Window*	GetTopmostWindow()	{ return m_pC_topmost_window; }
	void	Register(Window * p_window);
	void	Unregister(Window * p_window);

	bool	AlreadyRegistered(Window * p_window) const;

	void	Process();
	void	Show();

	//
	// Window input control
	// Window�� �Է��� WindowManager�� ���ؼ� �̷������.
	//
	bool	MouseControl(UINT message, int x, int y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	SetResolution(int res_x, int res_y) { m_res_x = res_x; m_res_y = res_y; }

public:
	//
	// set handler
	//
	void	SetAcquireMouseFocusHandler(void (*fp)(void)) { m_fp_handler_acquire_mousefocus = fp; }
	void	SetUnacquireMouseFocusHandler(void (*fp)(void)) { m_fp_handler_unacquire_mousefocus = fp; }
	void	SetAcquireDisappearHandler(void (*fp)(void)) { m_fp_handler_acquire_disappear = fp; }

	Window *GetFocusedWindow(int x, int y);
	Window *GetWindowbyName(const char* name);
	//
	// Window priority
	//
private: 

	void	SendMouseMoveMessageToMouseFocusedWindow();

public:
	void	SetMouseMoveFocusedWindow();	// ��������-_- ���� private���µ�-��-

	//
	// Window interface1
	//
	void	FirstPriority(Window * p_this_window);
	void	AppearWindow(Window * p_this_window);
	bool	DisappearWindow(Window * p_this_window);
	int	GetSequence(Window * p_this_window);
	Window * GetMouseFocusedWindow() const { return m_pC_mouse_focused_window; }
	Window * GetFirstPriorityWindow() const;
	Window * GetMovingWindow() const;
	bool	CancelPushStateOfCurrentPushedWindow();
	bool	HasMouseCaptureState() const;
	bool	CancelMouseCaptureState();

	int	ShowedWindowSize() const;
	int	ShowedPinnedWindowSize() const { return m_show_list_pinned_window.size(); }
	//int	RegisteredWindowSize() const { return m_window_buf_size; } // -> use 'Size()'
	bool	GetShowState(Window * p_window) const;

	int		GetCurrentMouse_x()	{	return m_current_mouse_x ; }
	int		GetCurrentMouse_y()	{	return m_current_mouse_y ; }


};

extern WindowManager *	gpC_window_manager;

void	g_RegisterWindow(Window * p_window);
void	g_UnregisterWindow(Window * p_window);

#endif
