/*-----------------------------------------------------------------------------

	VS_UI_widget.h

	VS UI ���� Widget.

	2000.6.7. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_WIDGET_H__
#define __VS_UI_WIDGET_H__

#include "VS_UI_Base.h"
#include "VS_UI_GlobalResource.h"
#include "CImm.h"
#include "VS_UI_MOUSE_POINTER.h"
#include "RarFile.h"
#include "crash_trace_tool.h"

// 2026-05-16 Dalga 2 — C4: GetCachedFontHeight inline helper, g_GetStringHeight
// forward decl (Fl2.h tam dahil etmemek icin — diger TU'lar etkilenmesin).
// Default argument Fl2.h'da set ediliyor — duplicate redefinition olmasin
// diye burada YOK; bu decl yalnizca sembol available olmasi icin.
int g_GetStringHeight(const char * sz_str, HFONT hfont);
/*
//----------------------------------------------------------------------------
// Button Class
//----------------------------------------------------------------------------
class C_VS_UI_BUTTON : public EventButton
{
private:
	enum
	{
		SPEED_MILLISEC = 75,
	};

	bool						m_bl_image_show;

	C_ANIMATION *			m_pC_ani_button;

public:
	C_VS_UI_BUTTON(int _x, int _y, int _w, int _h, id_t id, Exec * pC_exec_handler, bool set_end = false, bool image_show = true);
	~C_VS_UI_BUTTON();

	void Show();
	void EventFocusOn();
	void EventFocusOff();
	void EventPressDown();
	void EventPressUp();
	void SetPosition(int x, int y);
	void Refresh();
};*/

class C_VS_UI_EVENT_BUTTON : public EventButton
{
private:
	DWORD						m_dw_prev_tickcount;
	DWORD						m_dw_millisec;
	bool						m_bl_start;

public:
	int						m_alpha;
	int						m_image_index;

	enum { MAX_ALPHA = 32 };

public:
	C_VS_UI_EVENT_BUTTON(int _x, int _y, int _w, int _h, id_t id, Exec * pC_exec_handler, int image_index=-1, int millisec = 10) : 
									EventButton(_x, _y, _w, _h, id, pC_exec_handler)
	{
		Init();
		m_dw_millisec = millisec;

		if (m_image_index == -1) // default
			m_image_index = id;
		else
			m_image_index = image_index;
	}

	void	Init()
	{
		m_bl_start = false;
		m_alpha = 0;
	}

	void	Timer()
	{
		if (m_bl_start)
		{
			if (m_dw_prev_tickcount+m_dw_millisec <= GetTickCount())
			{
				// next frame!

				if (GetFocusState())
				{
					m_alpha++;
				}
				else
				{
					m_alpha--;
				}

				// !m_alpha = 0�� ���¿��� �� m_alpha--�� �� �� �ִ�. �̰��� �ð����� ���ؼ�
				// EventFocuxX�� �ι��̻� ����Ǳ� �����̴�.
				if (m_alpha <= 0)
				{
					m_alpha = 0;
					m_bl_start = false;
				}
				
				if (m_alpha >= MAX_ALPHA)
				{
					m_alpha = MAX_ALPHA;
					m_bl_start = false;
				}

				m_dw_prev_tickcount = GetTickCount();
			}
		}
	}

	void	EventFocusOn()
	{
		m_dw_prev_tickcount = GetTickCount();
		m_bl_start = true;
		if(gpC_Imm)
			gpC_Imm->ForceUI(CImm::FORCE_UI_BUTTON);
	}

	void	EventFocusOff()
	{
		m_dw_prev_tickcount = GetTickCount();
		m_bl_start = true;
		m_bl_prev_focus = false;
	}
};


//-----------------------------------------------------------------------------
// C_VS_UI_EVENT_STRING_BUTTON
// : ���ڿ��� �����ϰ� �ִ� ��ư Ŭ����
//-----------------------------------------------------------------------------
class C_VS_UI_EVENT_STRING_BUTTON : public C_VS_UI_EVENT_BUTTON
{
protected:
	std::string		m_String;
	PrintInfo*		m_pPrintInfo;
	CSize			m_sizeString;

public:
	C_VS_UI_EVENT_STRING_BUTTON(const CRect& rectPos, id_t id, Exec* pC_exec_handler,
								int image_index = -1, int millisec = 10,
								LPCTSTR buttonString = NULL, PrintInfo* printInfo = NULL);

	LPCTSTR GetButtonString() const	{ return m_String.c_str();		}

	virtual	void Show(const CPoint& offset) const = 0;
};


//-----------------------------------------------------------------------------
// C_VS_UI_EVENT_CHECK_BUTTON
// : üũ��ư�� ���� Ưȭ�� Ŭ����.
//   ������ üũ��ư�� ���ڿ��� ���� �������ϴ� �������� �ؼ��ߴ�.
//-----------------------------------------------------------------------------
class C_VS_UI_EVENT_CHECK_BUTTON : public C_VS_UI_EVENT_STRING_BUTTON
{
public:
	enum { STRING_GAP = 5 };

private:
	bool	m_bCheckState;
	bool	m_bDisabe;
	bool*	m_pOuterState;

public:
	C_VS_UI_EVENT_CHECK_BUTTON(const CPoint& pos, id_t id, Exec* pC_exec_handler,
							   bool* outerState = NULL,
							   LPCTSTR buttonString = NULL, PrintInfo* printInfo = NULL);

	void SetCheck(bool check = true)	{ m_bCheckState = check;	SyncOuterState();	}
	bool GetCheck() const				{ return m_bCheckState;		}

	void SetDisabe(bool disable = true)	{ m_bDisabe = disable;		}
	bool GetDisabe() const				{ return m_bDisabe;			}

	void EventPressUp();

	void Show(const CPoint& offset) const;

private:
	void SyncOuterState()	{ if(m_pOuterState) *m_pOuterState = m_bCheckState;	}

};


//-----------------------------------------------------------------------------
// C_VS_UI_EVENT_PUSH_BUTTON
// : ���� ������ ��ư Ŭ����
//-----------------------------------------------------------------------------
class C_VS_UI_EVENT_PUSH_BUTTON : public C_VS_UI_EVENT_STRING_BUTTON
{
private:
	bool	m_bDisabe;

public:
	C_VS_UI_EVENT_PUSH_BUTTON(const CRect& rectPos, id_t id, Exec* pC_exec_handler,
							  LPCTSTR buttonString = NULL, PrintInfo* printInfo = NULL);

	void SetDisabe(bool disable = true)	{ m_bDisabe = disable;		}
	bool GetDisabe() const				{ return m_bDisabe;			}

	void Show(const CPoint& offset) const;
};



class ButtonVisual
{
public:
	//
	// �ϳ��� button�� �����ϱ� ���� id�� button�� ���� flag�� ���ڷ� �Ѵ�.
	//
	virtual void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button) = 0;

	// Desciption �� �ִºκп��� ȣ��ȴ�.
	// ButtonGroup::ShowDescription(); ���� ȣ���� �Ǹ�, �ʿ����� ������� ������� �ʾƵ� ��
	virtual void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button){}

	virtual void	ShowButtonName(C_VS_UI_EVENT_BUTTON *p_button) {}

};

#ifndef _LIB
extern Button *	gpC_press_button;
#endif

//-----------------------------------------------------------------------------
// ButtonGroup
//
// C_VS_UI_BUTTON2 object�� �����Ѵ�.
//-----------------------------------------------------------------------------
class ButtonGroup : public SimpleDataList<C_VS_UI_EVENT_BUTTON *>
{
private:
	ButtonVisual *				m_p_button_visual;

public:
	ButtonGroup(ButtonVisual * p_visual) { m_p_button_visual = p_visual; }
	~ButtonGroup()
	{
		C_VS_UI_EVENT_BUTTON * data;
		for (int i=0; i < Size(); i++)
			if (Data(i, data))
				delete data;
	}

	C_VS_UI_EVENT_BUTTON * GetButton(int id)
	{
		C_VS_UI_EVENT_BUTTON * data;
		for (int i=0; i < Size(); i++)
			if (Data(i, data))
				if ((int)(data->GetID()) == id)
					return data;

		return NULL;
	}

	C_VS_UI_EVENT_BUTTON * GetFocusedButton()
	{
		C_VS_UI_EVENT_BUTTON * data;
		for (int i=0; i < Size(); i++)
			if (Data(i, data))
				if (data->GetFocusState())
					return data;

		return NULL;
	}

	
	C_VS_UI_EVENT_BUTTON * IsInRect(int _x, int _y)
	{
		C_VS_UI_EVENT_BUTTON * data;
		for (int i=0; i < Size(); i++)
			if (Data(i, data))
				if (data->IsInRect(_x, _y))
					return data;

		return NULL;
	}

	void	UnacquireMouseFocus()
	{
		C_VS_UI_EVENT_BUTTON * data;
		for (int i=0; i < Size(); i++)
			if (Data(i, data))
			{
				data->FocusOff();
				data->EventFocusOff();
			}
	}

	void	CancelPushState()
	{
		C_VS_UI_EVENT_BUTTON * data;
		for (int i=0; i < Size(); i++)
			if (Data(i, data))
			{
				data->PressUp();
			}
	}

	void	Init()
	{
		C_VS_UI_EVENT_BUTTON * data;
		for (int i=0; i < Size(); i++)
			if (Data(i, data))
			{
				data->Init();
				CancelPushState();
				UnacquireMouseFocus();
			}
	}

	void	ShowDescription()
	{
		C_VS_UI_EVENT_BUTTON *data;
		for(int i=0;i< Size();i++)
			if(Data(i,data))
				if(data->GetFocusState())
				{
					m_p_button_visual->ShowButtonDescription(data);
					return;
				}
	}

	void ShowButtonName()
	{
		C_VS_UI_EVENT_BUTTON * data;
		for (int i=0; i < Size(); i++)
			if (Data(i, data))
				m_p_button_visual->ShowButtonName(data);
	}

	void	Show()
	{ 
		C_VS_UI_EVENT_BUTTON * data;
		for (int i=0; i < Size(); i++)
			if (Data(i, data))
				m_p_button_visual->ShowButtonWidget(data);
	}

	void	Process()
	{ 
		C_VS_UI_EVENT_BUTTON * data;
		for (int i=0; i < Size(); i++)
			if (Data(i, data))
				data->Timer();
	}

	bool	MouseControl(UINT message, int _x, int _y)
	{
		bool re = true;
		C_VS_UI_EVENT_BUTTON * data;
		for (int i=0; i < Size(); i++)
			if (Data(i, data))
			{
#ifndef _LIB
				if (!g_GetCtrlPushState())
#endif
					re = data->MouseControl(message, _x, _y) && re;

#ifndef _LIB
				static bool press;
				static int gapx, gapy;

				switch (message)
				{
					case M_LEFTBUTTON_DOWN:
					case M_LB_DOUBLECLICK:
						if (data->IsInRect(_x, _y) && g_GetCtrlPushState())
						{
							gapx = _x-data->x;
							gapy = _y-data->y;
							press = true;
							gpC_press_button = data;
						}
						break;

					case M_LEFTBUTTON_UP:
						press = false;
						gpC_press_button = NULL;
						break;

					case M_MOVING:
						if (press && gpC_press_button)
						{
							gpC_press_button->x = _x - gapx;
							gpC_press_button->y = _y - gapy;
						}
						break;
				}
#endif
			}
		return re;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// ��ũ�ѹ� Ŭ�����̴�
// ���������� Show�� ������ ������ �⺻ ��ũ�ѹ� spk�� ����Ѵ�.
// �ٸ� spk�� ����ϵ��� �����Ҽ� ������, ��������Ʈ�� ������ �⺻ spk�� ���ƾ� �Ѵ�.
////////////////////////////////////////////////////////////////////////////////////////////

class C_VS_UI_SCROLL_BAR : public Rect
{
private:
	C_SPRITE_PACK	*m_spk;
	int				m_pos_max;
	int				m_bar_plus, m_tag_plus, m_button_plus;
	int				m_pos;
	bool			m_tag_pushed, m_up_button_focused, m_down_button_focused, m_up_button_pushed, m_down_button_pushed;
	int				m_tag_height, m_button_height, m_button_width;
	bool			m_bl_reverse, m_bHeight;


public:
	C_VS_UI_SCROLL_BAR(int pos_max, const Rect scroll_rect, bool bl_reverse = false, const char *scroll_bar_spk_name = NULL, int bar_plus = 6, int tag_plus = 6, int button_plus = 0, bool bHeight = true)
	{
		m_spk = NULL;
		if(scroll_bar_spk_name)
			m_spk = new C_SPRITE_PACK(scroll_bar_spk_name);

		C_SPRITE_PACK *spk = m_spk;
		if(spk == NULL)
			spk = gpC_global_resource->m_pC_scroll_bar_spk;

		Set(scroll_rect.x, scroll_rect.y, scroll_rect.w, scroll_rect.h);
		if(bHeight)
		{
			if(scroll_rect.w == -1)
			{
				w = spk->GetWidth(C_GLOBAL_RESOURCE::SB_BUTTON);
			}
		}
		else
		{
			if(scroll_rect.h == -1)
			{
				h = spk->GetHeight(C_GLOBAL_RESOURCE::SB_BUTTON);
			}
		}

		m_pos_max = pos_max;
		m_bar_plus = bar_plus;
		m_tag_plus = tag_plus;
		m_button_plus = button_plus;

		m_up_button_focused = false;
		m_down_button_focused = false;
		m_up_button_pushed = false;
		m_down_button_pushed = false;
		m_tag_pushed = false;
		m_bl_reverse = bl_reverse;
		m_bHeight = bHeight;

		m_button_width = spk->GetWidth(C_GLOBAL_RESOURCE::SB_BUTTON);
		m_button_height = spk->GetHeight(C_GLOBAL_RESOURCE::SB_BUTTON);
		m_tag_height = spk->GetHeight(C_GLOBAL_RESOURCE::SB_TAG);

		m_pos = 0;
	}

	C_VS_UI_SCROLL_BAR()
	{
		m_spk = NULL;

		Set(-1, -1, -1, -1);

		m_pos_max = -1;
		m_bar_plus = 6;
		m_tag_plus = 6;
		m_button_plus = 0;

		m_up_button_focused = false;
		m_down_button_focused = false;
		m_up_button_pushed = false;
		m_down_button_pushed = false;
		m_tag_pushed = false;
		m_bl_reverse = false;
		m_bHeight = true;

		m_button_height = gpC_global_resource->m_pC_scroll_bar_spk->GetHeight(C_GLOBAL_RESOURCE::SB_BUTTON);
		m_tag_height = gpC_global_resource->m_pC_scroll_bar_spk->GetHeight(C_GLOBAL_RESOURCE::SB_TAG);

		m_pos = 0;
	}

	~C_VS_UI_SCROLL_BAR()
	{
		if(m_spk)
		{
			delete m_spk;
			m_spk = NULL;
		}
	}

	void	SetSize(Rect scroll_rect)
	{
		C_SPRITE_PACK *spk = m_spk;
		if(spk == NULL)
			spk = gpC_global_resource->m_pC_scroll_bar_spk;

		Set(scroll_rect.x, scroll_rect.y, scroll_rect.w, scroll_rect.h);
		if(m_bHeight)
		{
			if(scroll_rect.w == -1)
			{
				w = spk->GetWidth(C_GLOBAL_RESOURCE::SB_BUTTON);
			}
		}
		else
		{
			if(scroll_rect.h == -1)
			{
				h = spk->GetHeight(C_GLOBAL_RESOURCE::SB_BUTTON);
			}
		}
	}

	void	Show(int _x, int _y)
	{
		C_SPRITE_PACK *spk = m_spk;
		if(spk == NULL)
			spk = gpC_global_resource->m_pC_scroll_bar_spk;

		if(gpC_base->m_p_DDSurface_back->Lock())
		{
			if(spk)
			{
				Rect rect;
				if(m_bHeight)
				{
					rect.Set(0, 0, spk->GetWidth(C_GLOBAL_RESOURCE::SB_BAR), h);
					spk->BltLockedClip(_x+x+m_bar_plus, _y+y, rect, C_GLOBAL_RESOURCE::SB_BAR);

					if(m_up_button_focused)
					{
						if(m_up_button_pushed)
							spk->BltLocked(_x+x+m_button_plus, _y+y-m_button_height, C_GLOBAL_RESOURCE::SB_BUTTON_HILIGHTED_PUSHED);
						else
							spk->BltLocked(_x+x+m_button_plus, _y+y-m_button_height, C_GLOBAL_RESOURCE::SB_BUTTON_HILIGHTED);
					}
					else
					{
						if(m_up_button_pushed)
							spk->BltLocked(_x+x+m_button_plus, _y+y-m_button_height, C_GLOBAL_RESOURCE::SB_BUTTON_PUSHED);
						else
							spk->BltLocked(_x+x+m_button_plus, _y+y-m_button_height, C_GLOBAL_RESOURCE::SB_BUTTON);
					}
					if(m_down_button_focused)
					{
						if(m_down_button_pushed)
							spk->BltLocked(_x+x+m_button_plus, _y+y+h, C_GLOBAL_RESOURCE::SB_BUTTON_HILIGHTED_PUSHED);
						else
							spk->BltLocked(_x+x+m_button_plus, _y+y+h, C_GLOBAL_RESOURCE::SB_BUTTON_HILIGHTED);
					}
					else
					{
						if(m_down_button_pushed)
							spk->BltLocked(_x+x+m_button_plus, _y+y+h, C_GLOBAL_RESOURCE::SB_BUTTON_PUSHED);
						else
							spk->BltLocked(_x+x+m_button_plus, _y+y+h, C_GLOBAL_RESOURCE::SB_BUTTON);
					}
					if(m_pos_max > 1)
					{
						if(m_bl_reverse)
							m_pos = (m_pos_max-1) - m_pos;
						
						if(m_tag_pushed)
						{
							spk->BltLocked(_x+x+m_tag_plus, min(_y+y+h-m_tag_height, max(_y+y, gpC_mouse_pointer->GetY()-m_tag_height/2)), C_GLOBAL_RESOURCE::SB_TAG);
						}
						else
							spk->BltLocked(_x+x+m_tag_plus, _y+y+m_pos*(h-m_tag_height)/(m_pos_max-1), C_GLOBAL_RESOURCE::SB_TAG);
						
						if(m_bl_reverse)
							m_pos = (m_pos_max-1) - m_pos;
					}
				}
				else
				{
					rect.Set(0, 0, w, spk->GetHeight(C_GLOBAL_RESOURCE::SB_BAR_WIDTH));
					spk->BltLockedClip(_x+x, _y+y+m_bar_plus, rect, C_GLOBAL_RESOURCE::SB_BAR_WIDTH);

					if(m_up_button_focused)
					{
						if(m_up_button_pushed)
							spk->BltLocked(_x+x-m_button_width, _y+y+m_button_plus, C_GLOBAL_RESOURCE::SB_BUTTON_HILIGHTED_PUSHED);
						else
							spk->BltLocked(_x+x-m_button_width, _y+y+m_button_plus, C_GLOBAL_RESOURCE::SB_BUTTON_HILIGHTED);
					}
					else
					{
						if(m_up_button_pushed)
							spk->BltLocked(_x+x-m_button_width, _y+y+m_button_plus, C_GLOBAL_RESOURCE::SB_BUTTON_PUSHED);
						else
							spk->BltLocked(_x+x-m_button_width, _y+y+m_button_plus, C_GLOBAL_RESOURCE::SB_BUTTON);
					}
					if(m_down_button_focused)
					{
						if(m_down_button_pushed)
							spk->BltLocked(_x+x+w, _y+y+m_button_plus, C_GLOBAL_RESOURCE::SB_BUTTON_HILIGHTED_PUSHED);
						else
							spk->BltLocked(_x+x+w, _y+y+m_button_plus, C_GLOBAL_RESOURCE::SB_BUTTON_HILIGHTED);
					}
					else
					{
						if(m_down_button_pushed)
							spk->BltLocked(_x+x+w, _y+y+m_button_plus, C_GLOBAL_RESOURCE::SB_BUTTON_PUSHED);
						else
							spk->BltLocked(_x+x+w, _y+y+m_button_plus, C_GLOBAL_RESOURCE::SB_BUTTON);
					}
					if(m_pos_max > 1)
					{
						if(m_bl_reverse)
							m_pos = (m_pos_max-1) - m_pos;
						
						if(m_tag_pushed)
						{
							spk->BltLocked(min(_x+x+w-m_tag_height, max(_x+x, gpC_mouse_pointer->GetX()-m_tag_height/2)), _y+y+m_tag_plus, C_GLOBAL_RESOURCE::SB_TAG_WIDTH);
						}
						else
							spk->BltLocked(_x+x+m_pos*(w-m_tag_height)/(m_pos_max-1), _y+y+m_tag_plus, C_GLOBAL_RESOURCE::SB_TAG_WIDTH);
						
						if(m_bl_reverse)
							m_pos = (m_pos_max-1) - m_pos;
					}
				}

			}
			gpC_base->m_p_DDSurface_back->Unlock();
		}
	}
	void	Show2(int _x, int _y)
	{
		C_SPRITE_PACK *spk = m_spk;
		if(spk == NULL)
			spk = gpC_global_resource->m_pC_scroll_bar_spk;

		if(gpC_base->m_p_DDSurface_back->Lock())
		{
			if(spk)
			{
				Rect rect;
				rect.Set(0, 0, spk->GetWidth(C_GLOBAL_RESOURCE::SB_BAR), h);
				spk->BltLockedClip(_x+x, _y+y, rect, C_GLOBAL_RESOURCE::SB_BAR);

				if(m_up_button_focused)
				{
					if(m_up_button_pushed)
						spk->BltLocked(_x+x+m_button_plus, _y+y-m_button_height, C_GLOBAL_RESOURCE::SB_BUTTON_PUSHED);
					else
						spk->BltLocked(_x+x+m_button_plus, _y+y-m_button_height, C_GLOBAL_RESOURCE::SB_BUTTON);
				}
				if(m_down_button_focused)
				{
					if(m_down_button_pushed)
						spk->BltLocked(_x+x+m_button_plus, _y+y+h, C_GLOBAL_RESOURCE::SB_BUTTON_HILIGHTED_PUSHED);
					else
						spk->BltLocked(_x+x+m_button_plus, _y+y+h, C_GLOBAL_RESOURCE::SB_BUTTON_HILIGHTED);
				}
				if(m_pos_max > 1)
				{
					if(m_bl_reverse)
						m_pos = (m_pos_max-1) - m_pos;
					spk->BltLocked(_x+x, _y+y+m_pos*(h-m_tag_height)/(m_pos_max-1), C_GLOBAL_RESOURCE::SB_TAG);
					
					if(m_bl_reverse)
						m_pos = (m_pos_max-1) - m_pos;
				}
			}
			gpC_base->m_p_DDSurface_back->Unlock();
		}
	}
	void	Show3(int _x, int _y)
	{
		// FAZ 1.9 — Per-frame CrashTrace çağrıları kaldırıldı (fare takılması düzeltmesi).
		C_SPRITE_PACK *spk = m_spk;
		if(spk == NULL)
			spk = gpC_global_resource->m_pC_scroll_bar_spk;

		if(gpC_base->m_p_DDSurface_back->Lock())
		{
			int i = 0;
#if __CONTENTS(__TITLE_UI_RENWEAL)
			i	= 8;	// OptionTilteScroll.SPK
			if(spk)
			{
				Rect rect;
				rect.Set(0, 0, spk->GetWidth(C_GLOBAL_RESOURCE::SB_BAR + i), h);
				// Phoenix: BltLockedClip yerine BltLocked — clip path modernde crash ediyor
				spk->BltLocked(_x+x+1, _y+y-1, C_GLOBAL_RESOURCE::SB_BAR + i);

				spk->BltLocked(_x+x+1, _y+y-9, C_GLOBAL_RESOURCE::SB3_BAR + i);
				spk->BltLocked(_x+x+1, _y+y-9, C_GLOBAL_RESOURCE::SB3_BUTTON_UP + i);
				spk->BltLocked(_x+x+1, _y+y+h-3, C_GLOBAL_RESOURCE::SB3_BUTTON_DOWN + i);
				if(m_up_button_focused)
				{
					if(m_up_button_pushed)
						spk->BltLocked(_x+x, _y+y-9, C_GLOBAL_RESOURCE::SB3_BUTTON_UP_PUSHED + i);
					else
						spk->BltLocked(_x+x, _y+y-9, C_GLOBAL_RESOURCE::SB3_BUTTON_UP_HILIGHTED  + i);
				}
				else
					spk->BltLocked(_x+x, _y+y-9, C_GLOBAL_RESOURCE::SB3_BUTTON_UP  + i);
				if(m_down_button_focused)
				{
					if(m_up_button_pushed)
						spk->BltLocked(_x+x+1, _y+y+h-3, C_GLOBAL_RESOURCE::SB3_BUTTON_DOWN_PUSHED  + i);
					else
						spk->BltLocked(_x+x+1, _y+y+h-3, C_GLOBAL_RESOURCE::SB3_BUTTON_DOWN_HILIGHTED  + i);
				}
				else
					spk->BltLocked(_x+x+1, _y+y+h-3, C_GLOBAL_RESOURCE::SB3_BUTTON_DOWN  + i);
				
				if(m_pos_max > 1)
				{
					if(m_bl_reverse)
						m_pos = (m_pos_max-1) - m_pos;
					spk->BltLocked(_x+x+2, _y+y+m_tag_height+m_pos*(h-8-m_tag_height)/(m_pos_max-1)-5, C_GLOBAL_RESOURCE::SB3_TAG + i);
					
					if(m_bl_reverse)
						m_pos = (m_pos_max-1) - m_pos;
				}
			}
#else //__TITLE_UI_RENWEAL
			if(spk)
			{
				Rect rect;
				rect.Set(0, 0, spk->GetWidth(C_GLOBAL_RESOURCE::SB_BAR + i), h);
				spk->BltLockedClip(_x+x+1, _y+y-1, rect, C_GLOBAL_RESOURCE::SB_BAR);

				spk->BltLocked(_x+x, _y+y-1, C_GLOBAL_RESOURCE::SB3_BUTTON_UP_PUSHED + i);
				if(m_up_button_focused)
				{
					if(m_up_button_pushed)
						spk->BltLocked(_x+x+m_button_plus, _y+y-m_button_height-1, C_GLOBAL_RESOURCE::SB3_BUTTON_UP_PUSHED + i);
					else
						spk->BltLocked(_x+x+m_button_plus, _y+y-m_button_height-1, C_GLOBAL_RESOURCE::SB3_BUTTON_UP_HILIGHTED  + i);
				}
				else
					spk->BltLocked(_x+x+m_button_plus, _y+y-m_button_height-1, C_GLOBAL_RESOURCE::SB3_BUTTON_UP  + i);
				if(m_down_button_focused)
				{
					if(m_up_button_pushed)
						spk->BltLocked(_x+x+m_button_plus, _y+y+h-1, C_GLOBAL_RESOURCE::SB3_BUTTON_DOWN_PUSHED  + i);
					else
						spk->BltLocked(_x+x+m_button_plus, _y+y+h-1, C_GLOBAL_RESOURCE::SB3_BUTTON_DOWN_HILIGHTED  + i);
				}
				else
					spk->BltLocked(_x+x+m_button_plus, _y+y+h-1, C_GLOBAL_RESOURCE::SB3_BUTTON_DOWN  + i);
				
				if(m_pos_max > 1)
				{
					if(m_bl_reverse)
						m_pos = (m_pos_max-1) - m_pos;
					spk->BltLocked(_x+x+m_tag_plus, _y+y+m_pos*(h-m_tag_height)/(m_pos_max-1)-1, C_GLOBAL_RESOURCE::SB3_TAG + i);
					
					if(m_bl_reverse)
						m_pos = (m_pos_max-1) - m_pos;
				}
			}
#endif //__TITLE_UI_RENWEAL
			gpC_base->m_p_DDSurface_back->Unlock();
		}
	}
	bool	MouseControl(UINT message, int _x, int _y)
	{
		switch(message)
		{
		case M_MOVING:
			if(m_bHeight)
			{
				if(m_tag_pushed)
				{
					SetScrollPixel(_y);
				}
				else if(_x > x && _x < x+w && _y < y && _y > y-m_button_height)
				{
					if(m_up_button_focused == false)
					{
						m_up_button_focused = true;
						if(gpC_Imm)
							gpC_Imm->ForceUI(CImm::FORCE_UI_BUTTON);
					}
					m_down_button_focused = false;
				}
				else if(_x > x && _x < x+w && _y > y+h && _y < y+h+m_button_height)
				{
					if(m_down_button_focused == false)
					{
						m_down_button_focused = true;
						if(gpC_Imm)
							gpC_Imm->ForceUI(CImm::FORCE_UI_BUTTON);
					}
					m_up_button_focused = false;
				}
				else
				{
					m_up_button_focused = false;
					m_down_button_focused = false;
				}
			}
			else
			{
				if(m_tag_pushed)
				{
					SetScrollPixel(_x);
				}
				else if(_y > y && _y < y+h && _x < x && _x > x-m_button_width)
				{
					if(m_up_button_focused == false)
					{
						m_up_button_focused = true;
						if(gpC_Imm)
							gpC_Imm->ForceUI(CImm::FORCE_UI_BUTTON);
					}
					m_down_button_focused = false;
				}
				else if(_y > y && _y < y+h && _x > x+w && _x < x+w+m_button_width)
				{
					if(m_down_button_focused == false)
					{
						m_down_button_focused = true;
						if(gpC_Imm)
							gpC_Imm->ForceUI(CImm::FORCE_UI_BUTTON);
					}
					m_up_button_focused = false;
				}
				else
				{
					m_up_button_focused = false;
					m_down_button_focused = false;
				}
			}
			break;

		case M_LEFTBUTTON_DOWN:
		case M_LB_DOUBLECLICK:
			if(m_up_button_focused)
			{
				m_up_button_pushed = true;
				return false;
			}
			else if(m_down_button_focused)
			{
				m_down_button_pushed = true;
				return false;
			}
			else
			{
				if(m_bHeight)
				{
					if(_x > x && _x < x+w && _y > y && _y < y+h)
					{
						SetScrollPixel(_y);
						m_tag_pushed = true;
						return false;
					}
				}
				else
				{
					if(_y > y && _y < y+h && _x > x && _x < x+w)
					{
						SetScrollPixel(_x);
						m_tag_pushed = true;
						return false;
					}
				}
			}
			break;

		case M_LEFTBUTTON_UP:
			m_tag_pushed = false;
			if(m_up_button_pushed)
			{
				m_up_button_pushed = false;
				ScrollUp();
				return false;
			}
			else if(m_down_button_pushed)
			{
				m_down_button_pushed = false;
				ScrollDown();
				return false;
			}
			break;
		}
		return true;
	}

	void	CancelPushState()
	{
		m_up_button_pushed = false;
		m_down_button_pushed = false;
		m_tag_pushed = false;
	}

	void	ScrollUp(int pos = 1)
	{
		if(m_bl_reverse)
		{
			m_pos = min(m_pos_max-1, m_pos+pos);
		}
		else
		{
			m_pos = max(0, m_pos-pos);
		}
	}

	void	ScrollDown(int pos = 1)
	{
		if(m_bl_reverse)
		{
			m_pos = max(0, m_pos-pos);
		}
		else
		{
			m_pos = min(m_pos_max-1, m_pos+pos);
		}
	}

	void	SetScrollPos(int pos)
	{
		m_pos = max(0, min(m_pos_max-1, pos));
	}

	void	SetScrollPixel(int _pixel)
	{
		int pos;

		if(m_bHeight)
			pos = (_pixel-y-m_tag_height/2)*m_pos_max/(h-m_tag_height);
		else
			pos = (_pixel-x-m_tag_height/2)*m_pos_max/(w-m_tag_height);

		if( !m_bl_reverse )
			SetScrollPos( pos );
		else
			SetScrollPos( (m_pos_max-1) - pos );
	}

	int		GetScrollPos()
	{
		return m_pos;
	}

	void	SetPosMax(int max)	//pos_max�� ��ũ�ѵ� �׸��� �����̴�. ���� �� ȭ�鿡 5���� �׸��� ������, �� 10���� �׸��� �ִٸ� ��ũ�Ѱ��� 0~5 ���� �����Ƿ� pos_max == 6 �̴�. 
	{
		m_pos = 0;
		m_pos_max = max;
	}

	void	SetReverse(bool bl_reverse)
	{
		m_bl_reverse = bl_reverse;
	}
};
class C_VS_UI_LISTBOX : public C_VS_UI_EVENT_BUTTON
{

public:
	typedef std::list<std::string> STRING_LIST;

#if __CONTENTS(__ITEM_SHOW)
	typedef std::map<int, COLORREF> STRING_COLOR_MAP;
#endif //__ITEM_SHOW
	
	C_VS_UI_LISTBOX(STRING_LIST& liststring, CRect rectlist, id_t id, Exec* pC_exec_handler,
				PrintInfo* printInfo = NULL);
	~C_VS_UI_LISTBOX();
#if __CONTENTS(__ITEM_SHOW)
	C_VS_UI_SCROLL_BAR	*m_pC_scroll_bar;
#endif //__ITEM_SHOW
	
	void	Show(const CPoint& offset) const;

	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

#if __CONTENTS(__ITEM_SHOW)
	bool	AddString(std::string addstring);
	void	SetListStringColor(int Num, COLORREF Color);
#endif //__ITEM_SHOW

	bool	AddListNode(int index, std::string addstring);
	bool	DelListNode(int index);

#if __CONTENTS(__ITEM_SHOW)
	bool	DelListNodeAll();
#endif //__ITEM_SHOW

	int		GetSelectListIndex()						{ return m_CurrentIndex; }
	void	SetSelectListIndex(int setindex)			{ m_CurrentIndex = setindex; }
	LPCSTR	GetSelectListString(int selectindex);

#if __CONTENTS(__ITEM_SHOW)
	int		GetOverSelectListIndex()					{ return m_pC_scroll_bar->GetScrollPos()+m_OverIndex; }
	int		GetScrollPos(void);
	void	SetScrollPos(int _Pos);
	int		ListNodeMax(void);

	// find..
	void	SetFindString(string _str);
	const char *GetFindString()							{ return m_find_string.c_str(); }
	// 2026-05-16: m_find_string_lower de clear edilmeli — Show() loop bunu okuyor.
	void	FindStringClear()							{ m_find_string = ""; m_find_string_lower = ""; m_bfindOK = false; }
	bool	IsFindStringOK()							{ return m_bfindOK; }	// ����Ʈ ���� �˻�� �����ϴ°�?

	void	SetDescription()							{ b_Description = true; }
	void	UnSetDescription()							{ b_Description = false; }

	void	SetColorFind(COLORREF _ColorFind)			{ m_ColorFind = _ColorFind; }
	void	SetColorOver(COLORREF _ColorOver)			{ m_ColorOver = _ColorOver; }
	void	SetDefault(COLORREF _ColorDefault)			{ m_ColorDefault = _ColorDefault; }
	void	SetColorClick(COLORREF _ColorClick)			{ m_ColorClick = _ColorClick; }

	void	SetDoubleClickIndex(int _Index = -1)		{ m_DoubleClickIndex = _Index; }
	int		PopDoubleClickIndex()						{ int _DoubleClickIndex = m_DoubleClickIndex; m_DoubleClickIndex = -1; return _DoubleClickIndex; }
#endif //__ITEM_SHOW

private:

	CRect				m_RectList;
	PrintInfo*			m_pPrintInfo;
	STRING_LIST			m_ListString;
#if __CONTENTS(__ITEM_SHOW)
	STRING_COLOR_MAP	m_ColorMapString;
#endif //__ITEM_SHOW

	CSize				m_StringBoxGap;
	CSize				m_StringBoxSelectGap;

	int					m_MAXIndex;
	int					m_CurrentIndex;

#if __CONTENTS(__ITEM_SHOW)
	int					m_OverIndex;
	int					m_DoubleClickIndex;
	string				m_find_string;
	string				m_null_string;
	// 2026-05-16: FPS optimization — Show() loop her frame redundant heap alloc + transform yapiyordu.
	// m_find_string ve m_null_string frame icinde sabit, sadece setter cagrildiginda degisir.
	// Pre-computed lowercase copy'ler: SetFindString/FindStringClear/ctor'da bir kez compute.
	string				m_find_string_lower;
	string				m_null_string_lower;
	bool				m_bfindOK;

	bool				b_Description;			// Description�� �����ٰ����� ��������

	COLORREF			m_ColorFind;
	COLORREF			m_ColorOver;
	COLORREF			m_ColorDefault;
	COLORREF			m_ColorClick;
#endif //__ITEM_SHOW

	// 2026-05-16 Dalga 2 — C4: font height cache.
	// LISTBOX::Show + MouseControl + AddListNode + AddString her cagrida
	// `g_GetStringHeight("a", m_pPrintInfo->hfont)` GDI roundtrip yapiyor;
	// PrintInfo HFONT degismedigi surece sabit. mutable: Show const, ama
	// cache lazy populate.
	mutable int			m_CachedFontHeight;
	mutable HFONT		m_CachedFontHFont;

	// HFONT degismediyse cached height dondurur, aksi halde gercek
	// g_GetStringHeight cagirip cache'i refresh eder.
	int GetCachedFontHeight(HFONT hfont) const
	{
		if (hfont != m_CachedFontHFont || m_CachedFontHeight == 0)
		{
			m_CachedFontHeight = ::g_GetStringHeight("a", hfont);
			m_CachedFontHFont  = hfont;
		}
		return m_CachedFontHeight;
	}
};  // 2026-05-16 Dalga 2 — C4: C_VS_UI_LISTBOX kapanis (oncesi bu satir, edit'te yanlislikla kaybedildi)

#endif
