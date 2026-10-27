#include "client_PCH.h"
#include "crash_trace_tool.h"
#include <assert.h>
#include "VS_UI_widget.h"
#include "VS_UI_filepath.h"

#if __CONTENTS(__ITEM_SHOW)
#include "VS_UI_descriptor.h"
#include "VS_UI_Description.h"
#include <algorithm>
void makeLowerChar(char& c) { c = tolower(c); }
#endif //__ITEM_SHOW

#ifndef _LIB
Button *	gpC_press_button;
#endif

//-----------------------------------------------------------------------------
// C_VS_UI_EVENT_STRING_BUTTON
// : ���ڿ��� �����ϰ� �ִ� ��ư Ŭ����
//-----------------------------------------------------------------------------
C_VS_UI_EVENT_STRING_BUTTON::C_VS_UI_EVENT_STRING_BUTTON(const CRect& rectPos, id_t id,
														 Exec * pC_exec_handler, int image_index,
														 int millisec, LPCTSTR buttonString, PrintInfo* printInfo)
	: C_VS_UI_EVENT_BUTTON(rectPos.left, rectPos.top, rectPos.Width(), rectPos.Height(),
						   id, pC_exec_handler, image_index, millisec)
{
	if(buttonString)
	{
		m_String = buttonString;
		
		if(printInfo)
			m_pPrintInfo = printInfo;
		else
			m_pPrintInfo = &gpC_base->m_chatting_pi;

		m_sizeString.cx	= g_GetStringWidth(buttonString, m_pPrintInfo->hfont);
		m_sizeString.cy	= g_GetStringHeight(buttonString, m_pPrintInfo->hfont);
	}
	else
	{
		m_pPrintInfo = NULL;
	}
}



//-----------------------------------------------------------------------------
// C_VS_UI_EVENT_CHECK_BUTTON
// : üũ��ư�� ���� Ưȭ�� Ŭ����.
//   ������ üũ��ư�� ���ڿ��� ���� �������ϴ� �������� �ؼ��ߴ�.
//-----------------------------------------------------------------------------
C_VS_UI_EVENT_CHECK_BUTTON::C_VS_UI_EVENT_CHECK_BUTTON(const CPoint& pos, id_t id, Exec* pC_exec_handler,
													   bool* outerState, LPCTSTR buttonString, PrintInfo* printInfo)
	: C_VS_UI_EVENT_STRING_BUTTON(CRect(pos.x, pos.y, pos.x, pos.y), id,
								  pC_exec_handler, 0, 10, buttonString, printInfo),
	  m_pOuterState(outerState)
{
	SetDisabe(false);
	
	if(outerState)
		SetCheck(*outerState);
	else
		SetCheck(false);
	
	int buttonWidth	= gpC_global_resource->m_pC_common_button_spk->GetWidth(
		C_GLOBAL_RESOURCE::CHECK_BACK_ENABLE);
	
	int buttonHeight = gpC_global_resource->m_pC_common_button_spk->GetHeight(
		C_GLOBAL_RESOURCE::CHECK_BACK_ENABLE);
	
	w += buttonWidth;
	h += buttonHeight;
	
	if(buttonString)
	{
		w += m_sizeString.cx + STRING_GAP;
		
		if(m_sizeString.cy > h)
			h = m_sizeString.cy;
	}
}


void C_VS_UI_EVENT_CHECK_BUTTON::EventPressUp()
{
	if(!m_bDisabe)
		m_bCheckState = !m_bCheckState;
	
	SyncOuterState();
}


void C_VS_UI_EVENT_CHECK_BUTTON::Show(const CPoint& offset) const
{
	int backIdx	= GetDisabe() || GetPressState() ?
		C_GLOBAL_RESOURCE::CHECK_BACK_DISABLE :
		C_GLOBAL_RESOURCE::CHECK_BACK_ENABLE;
	
	CPoint pt(offset);
	pt.x += x;
	pt.y += y;
	
	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		gpC_global_resource->m_pC_common_button_spk->BltLocked(
			pt.x, pt.y, backIdx);
		
		if(GetCheck())
			gpC_global_resource->m_pC_common_button_spk->BltLocked(
			pt.x + 2, pt.y + 2, C_GLOBAL_RESOURCE::CHECK_FOCUS);
		
		gpC_base->m_p_DDSurface_back->Unlock();
	}
	
	if(!m_String.empty())
	{
		int width	= gpC_global_resource->m_pC_common_button_spk->GetWidth(backIdx);
		int height	= gpC_global_resource->m_pC_common_button_spk->GetHeight(backIdx);
		
		g_FL2_GetDC();
		
		g_PrintColorStrShadow(
			pt.x + width + STRING_GAP, pt.y, m_String.c_str(),
			*m_pPrintInfo, RGB_WHITE, RGB_BLACK);
		
		g_FL2_ReleaseDC();
	}
}



//-----------------------------------------------------------------------------
// C_VS_UI_EVENT_PUSH_BUTTON
// : ���� ������ ��ư Ŭ����
//-----------------------------------------------------------------------------
C_VS_UI_EVENT_PUSH_BUTTON::C_VS_UI_EVENT_PUSH_BUTTON(const CRect& rectPos, id_t id, Exec* pC_exec_handler,
													 LPCTSTR buttonString, PrintInfo* printInfo)
	: C_VS_UI_EVENT_STRING_BUTTON(rectPos, id, pC_exec_handler, 0, 10, buttonString, printInfo)
{


}

void C_VS_UI_EVENT_PUSH_BUTTON::Show(const CPoint& offset) const
{
	Rect rcDraw(x + offset.x, y + offset.y, w, h);

	if(GetPressState())
	{
		gpC_global_resource->DrawInBox(rcDraw);
		rcDraw.x += 1;
		rcDraw.y += 1;
	}
	else
	{
		gpC_global_resource->DrawOutBox(rcDraw);
	}

	if(!m_String.empty())
	{
		CPoint ptDrawString(
			rcDraw.x + (rcDraw.w - m_sizeString.cx) / 2,
			rcDraw.y + (rcDraw.h - m_sizeString.cy) / 2);

		g_FL2_GetDC();	
		
		g_PrintColorStrShadow(
			ptDrawString.x, ptDrawString.y, m_String.c_str(),
			*m_pPrintInfo, RGB_WHITE, RGB_BLACK);
		
		g_FL2_ReleaseDC();
	}
}


//bool ButtonGroup::press;

//----------------------------------------------------------------------------
// Operations
//----------------------------------------------------------------------------
/*
//-----------------------------------------------------------------------------
// SetPosition
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_BUTTON::SetPosition(int x, int y)
{
	m_pC_ani_button->SetPlayPosition(x, y);
}

//-----------------------------------------------------------------------------
// C_VS_UI_BUTTON
//
// set_end�� ani button�� focus rect�� �� ���Ŀ��θ� �����Ѵ�.
//-----------------------------------------------------------------------------
C_VS_UI_BUTTON::C_VS_UI_BUTTON(int _x, int _y, int _w, int _h, id_t id, Exec * pC_exec_handler, bool set_end, bool image_show) : 
						EventButton(_x, _y, _w, _h, id, pC_exec_handler)
{
	assert(gpC_global_resource);
	assert(gpC_global_resource->m_pC_ao_ani_button);

	m_pC_ani_button = new C_ANIMATION(gpC_global_resource->m_pC_ao_ani_button);

	m_pC_ani_button->SetSpeed(SPEED_MILLISEC);

	int px, py;
	if (set_end)
	{
		px = x+w-gpC_global_resource->m_pC_ao_ani_button->GetWidth(0);
		py = y;
	}
	else
	{
		px = x;
		py = y;
	}

	m_pC_ani_button->SetPlayPosition(px, py);
	m_bl_image_show = image_show;
}

//-----------------------------------------------------------------------------
// ~C_VS_UI_BUTTON
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_BUTTON::~C_VS_UI_BUTTON()
{
	DeleteNew(m_pC_ani_button);
}

//-----------------------------------------------------------------------------
// Refresh
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_BUTTON::Refresh()
{
	InitState();
	m_pC_ani_button->Refresh();
}

//-----------------------------------------------------------------------------
// Show
//
// Button�� ���δ�.
//-----------------------------------------------------------------------------
void C_VS_UI_BUTTON::Show()
{
	CT_Done("WnSh: C_VS_UI_BUTTON::Show ENTER pthis=%p", (void*)this);
	if (m_bl_image_show)
		m_pC_ani_button->Show();
}

//-----------------------------------------------------------------------------
// EventFocusOn
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_BUTTON::EventFocusOn()
{
	// animation play
	switch (m_pC_ani_button->GetAnimationState())
	{
		case C_ANIMATION::PLAY_BACK:
			m_pC_ani_button->SetNextPlayOrder(C_ANIMATION::PLAY);
			break;

		case C_ANIMATION::STOP:
			m_pC_ani_button->Play();
			break;

		case C_ANIMATION::PLAY:
			m_pC_ani_button->SetNextPlayOrder(C_ANIMATION::STOP);
			break;
	}
}

//-----------------------------------------------------------------------------
// EventFocusOff
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_BUTTON::EventFocusOff()
{
	// animation play back
	switch (m_pC_ani_button->GetAnimationState())
	{
		case C_ANIMATION::PLAY:
			m_pC_ani_button->SetNextPlayOrder(C_ANIMATION::PLAY_BACK);
			break;

		case C_ANIMATION::STOP:
			m_pC_ani_button->PlayBack();
			break;

		case C_ANIMATION::PLAY_BACK:
			m_pC_ani_button->SetNextPlayOrder(C_ANIMATION::STOP);
			break;
	}
}

//-----------------------------------------------------------------------------
// EventPressDown
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_BUTTON::EventPressDown()
{
//	m_pC_ani_button->SetPlayPosition(m_x+2, m_y+2);
}

//-----------------------------------------------------------------------------
// UpAction
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_BUTTON::EventPressUp()
{
//	m_pC_ani_button->SetPlayPosition(m_x, m_y);
}*/
C_VS_UI_LISTBOX::C_VS_UI_LISTBOX(STRING_LIST& liststring, CRect rectlist, id_t id, Exec* pC_exec_handler,
			   PrintInfo* printInfo)
:C_VS_UI_EVENT_BUTTON(rectlist.left, rectlist.top, rectlist.Width(), rectlist.Height(),
						   id, pC_exec_handler)
{
#if __CONTENTS(__ITEM_SHOW)
	m_null_string = "<NULL>";
	// 2026-05-16: FPS optimization — pre-compute lowercase copy.
	// m_null_string ctor'dan sonra hic degismiyor, bir kez yapilan transform Show() loop'u her frame'den kurtariyor.
	m_null_string_lower = m_null_string;
	std::for_each(m_null_string_lower.begin(), m_null_string_lower.end(), makeLowerChar);
	// m_find_string_lower default empty (std::string default ctor); SetFindString cagrilinca dolacak.
	SetDescription();
#endif //__ITEM_SHOW

	// 2026-05-16 Dalga 2 — C4: font height cache init. Lazy populate — ilk
	// kullanim site'inde hesap yapilir, sonra HFONT degismedikce reuse.
	m_CachedFontHeight = 0;
	m_CachedFontHFont  = NULL;

	m_RectList = rectlist;
	m_StringBoxGap.SetSize(3, 3);
	m_StringBoxSelectGap.SetSize(-2, 0);

	// Null���� ó�� ������..
//	m_ListString = liststring;
	
	STRING_LIST::iterator it;
	for(it = liststring.begin(); it != liststring.end(); ++it)
	{
#if __CONTENTS(__ITEM_SHOW)
		if(strlen(it->c_str())<=0)
			m_ListString.push_back(m_null_string);
		else
#endif //__ITEM_SHOW
			m_ListString.push_back(it->c_str());
	}
	m_MAXIndex = m_ListString.size() - 1;

	m_CurrentIndex = -1;
#if __CONTENTS(__ITEM_SHOW)
	m_OverIndex = -1;
	SetDoubleClickIndex();
#endif //__ITEM_SHOW

	if(printInfo)
		m_pPrintInfo = printInfo;
	else
		m_pPrintInfo = &gpC_base->m_chatting_pi;

#if __CONTENTS(__ITEM_SHOW)
	int	nGapHeight	= m_StringBoxGap.cy << 1;
	int	nFontHeight	= GetCachedFontHeight(m_pPrintInfo->hfont);  // 2026-05-16 Dalga 2 C4

	m_pC_scroll_bar = new C_VS_UI_SCROLL_BAR(0, Rect(rectlist.right-m_StringBoxGap.cx*6, rectlist.top+m_StringBoxGap.cy*6, -1, rectlist.Height()-m_StringBoxGap.cy*12));
	m_pC_scroll_bar->SetPosMax(m_MAXIndex-(m_RectList.Height()/(nFontHeight + nGapHeight))+2);

	FindStringClear();

	m_ColorFind		= RGB_RED;
	m_ColorOver		= RGB_DARKGRAY;
	m_ColorDefault	= RGB_WHITE;
	m_ColorClick	= RGB_GREEN;
#endif //__ITEM_SHOW
}
C_VS_UI_LISTBOX::~C_VS_UI_LISTBOX()
{
#if __CONTENTS(__ITEM_SHOW)
	DeleteNew( m_pC_scroll_bar);
	m_ColorMapString.clear();
#endif //__ITEM_SHOW
}

void
C_VS_UI_LISTBOX::Show(const CPoint& offset) const
{
//	if (gpC_base->m_p_DDSurface_back->Lock())
//	{
//		gpC_global_resource->DrawDialogLocked(x, y, w, h, GetAttributes()->alpha);

		CRect rcList(m_RectList);

		rcList.OffsetRect(offset.x, offset.y);

		gpC_global_resource->DrawInBox(
			rcList.left, rcList.top,
			rcList.Width(), rcList.Height());

//		gpC_base->m_p_DDSurface_back->Unlock();
//	}


	if(m_ListString.empty())
		return;
	CPoint	ptstringposition(m_RectList.TopLeft());
	CSize	sizestringboxgap(m_StringBoxGap);
	CSize	sizestringboxselectgap(m_StringBoxSelectGap);

	ptstringposition.y += sizestringboxgap.cy;

	STRING_LIST::const_iterator listindex;
	bool	bSelected	= false;
#if __CONTENTS(__ITEM_SHOW)
	bool	bOverSelected	= false;
#endif //__ITEM_SHOW
	int		count		= 0;

#if __CONTENTS(__ITEM_SHOW)
	listindex = m_ListString.begin();

	for(int k=0;k<m_pC_scroll_bar->GetScrollPos();k++)
	{ listindex++; }

	// FAZ 4 (2026-05-15): Admin menu Item tab FPS optimization —
	// virtualization. Without this cap the loop O(N) iterates every frame
	// (LISTBOX::Show is per-frame), with ~3 makeLowerChar + find_string
	// ops per item → ~500k char ops/frame on a 5k-item Admin Item tab.
	// We still have the existing rect-overflow break at the end of the
	// body, but THAT only kicks in after the heavy string work is done.
	// Hard-cap iterations to visible-row count + a small safety margin
	// so off-screen rows never run the lowercase/find machinery.
	const int _virt_gap   = m_StringBoxGap.cy << 1;
	// 2026-05-16 Dalga 2 — C4: cached font height. PrintInfo HFONT degismedikce
	// GDI roundtrip yok (per-frame GetTextExtentPoint32 elenir).
	const HFONT _virt_hfont = m_pPrintInfo ? m_pPrintInfo->hfont : gpC_base->m_chatting_pi.hfont;
	const int _virt_font  = GetCachedFontHeight(_virt_hfont);
	const int _virt_row   = _virt_font + _virt_gap;
	int _virt_visible     = (_virt_row > 0) ? ((m_RectList.bottom - m_RectList.top) / _virt_row) : 0;
	if (_virt_visible <= 0) _virt_visible = 10;
	const int _virt_limit = _virt_visible + 5; // safety margin
	int _virt_rendered    = 0;

	// 2026-05-15: Outer hoist v2 — Round 2 regression sonrası inner release-reacquire pattern.
	// FL2.cpp:99-102 kural: TEXT DC outstanding iken sprite Lock yasak (m_p_DDSurface_back->IsLock()
	// asserti). Composition buffer (Phoenix_AcquireD3D9DC -> m_pCompositionBuffer) ile
	// m_p_DDSurface_back AYRI surface'lar — DC ve sprite Lock farkli buffer'larda, AMA FL2 API'si
	// her GetDC oncesi m_p_DDSurface_back->IsLock() assert ediyor (driver state kurali).
	// Loop ici DrawOutBox/DrawInBox m_p_DDSurface_back->Lock() yapar; bu cagrilardan ONCE
	// outer DC release, SONRA reacquire gerekir. Item tab build'inde (__ITEM_SHOW=__ON)
	// DrawOutBox loop ici disable; outer hoist tek pair yeterli. !__ITEM_SHOW build'inde
	// (varsa) inner pair selected branch'inde aktif. Per-frame GetDC: ~25 -> ~1 (frame basi).
	// Onceki regression notu (revert edildi): project_d3d9_getdc_outer_hoist_regression.md
	g_FL2_GetDC();  // OUTER ACQUIRE — loop boyunca DC bir kez alinir

	for(; listindex != m_ListString.end() && _virt_rendered < _virt_limit; ++listindex, ++count, ++_virt_rendered)
	{
#else
	// 2026-05-15: Outer hoist v2 — !__ITEM_SHOW branch (DrawOutBox loop ici aktif)
	// FL2.cpp:99-102 kural: TEXT DC outstanding iken sprite Lock yasak
	g_FL2_GetDC();  // OUTER ACQUIRE — loop boyunca DC bir kez alinir
	for(m_ListString.begin(); listindex != m_ListString.end(); ++listindex, ++count)
	{
#endif //__ITEM_SHOW
		const char*	showstring	= listindex->c_str();

#if __CONTENTS(__ITEM_SHOW)
		if(m_CurrentIndex >= 0 && m_CurrentIndex-m_pC_scroll_bar->GetScrollPos() == count)
		{
#else
		if(m_CurrentIndex >= 0 && m_CurrentIndex == count)
		{
#endif //__ITEM_SHOW
			bSelected	= true;
		}
		else
		{
			bSelected	= false;
		}

#if __CONTENTS(__ITEM_SHOW)
		if(m_OverIndex >= 0 && m_OverIndex == count)
		{
			bOverSelected = true;
		}
		else
		{
			bOverSelected = false;
		}
#endif //__ITEM_SHOW

		// 2026-05-15: Outer hoist v2 — per-item g_FL2_GetDC() SILINDI.
		// Outer scope'ta acquire edildi (loop oncesi). Inner FL2 callers (g_GetStringWidth,
		// g_PrintColorStrShadow, g_GetStringHeight) idempotent — outer DC bulunca skip.
		// FL2.cpp:99-102 kural: TEXT DC outstanding iken sprite Lock yasak.

		int nFontWidth = 0;
		int nGapWidth		= m_StringBoxGap.cx << 1;

		string _ouputStr = showstring;
		int showstring_size = strlen(showstring);

#if __CONTENTS(__ITEM_SHOW)
		while(showstring_size--)
		{
			nFontWidth = g_GetStringWidth(_ouputStr.c_str(), gpC_base->m_chatting_pi.hfont);
			if(nFontWidth<m_RectList.Width()-nGapWidth-5)
				break;

			if(_ouputStr[showstring_size]>=32&&_ouputStr[showstring_size]<=126)
				_ouputStr[showstring_size] = 0;
		}

		COLORREF _color = m_ColorDefault;

		STRING_COLOR_MAP::const_iterator it_ColorMap = m_ColorMapString.find(m_pC_scroll_bar->GetScrollPos()+count);
		
		if(it_ColorMap != m_ColorMapString.end())
		{ _color = it_ColorMap->second; }

		// 2026-05-16: FPS optimization — sadece showstring her iter allocate.
		// m_find_string_lower + m_null_string_lower pre-computed member, redundant alloc + transform kaldirildi.
		// Onceki: 3 alloc + 3 lowercase per-iter | Sonrasi: 1 alloc + 1 lowercase per-iter (66% azalma).
		// �� �ұ��� ���� �˻�
		string _str = showstring;
		std::for_each(_str.begin(), _str.end(), makeLowerChar);

		if( m_null_string_lower.size()!=0 && _str.find(m_null_string_lower.c_str()) != std::string::npos ) {}
		else if(!bOverSelected)
		{
			// find..
			if( m_find_string_lower.size()!=0 && _str.find(m_find_string_lower.c_str()) != std::string::npos )
				_color = m_ColorFind;

			g_PrintColorStrShadow(
				offset.x + ptstringposition.x + sizestringboxgap.cx,
				offset.y + ptstringposition.y + sizestringboxgap.cy,
				_ouputStr.c_str(), gpC_base->m_chatting_pi,
				bSelected ? m_ColorClick : _color, RGB_BLACK);
		}
		else
		{
			g_PrintColorStrShadow(
				offset.x + ptstringposition.x + sizestringboxgap.cx,
				offset.y + ptstringposition.y + sizestringboxgap.cy,
				_ouputStr.c_str(), gpC_base->m_chatting_pi,
				bSelected ? m_ColorClick : m_ColorOver, RGB_BLACK);
		}
#else //__ITEM_SHOW
			g_PrintColorStrShadow(
				offset.x + ptstringposition.x + sizestringboxgap.cx,
				offset.y + ptstringposition.y + sizestringboxgap.cy,
				_ouputStr.c_str(), gpC_base->m_chatting_pi,
				bSelected ? RGB_GREEN : RGB_WHITE, RGB_BLACK);
#endif //__ITEM_SHOW

		// 2026-05-15: Outer hoist v2 — per-item g_FL2_ReleaseDC() SILINDI.
		// Outer scope'ta release edilecek (loop sonrasi). DrawOutBox call-site'inda
		// inner release-reacquire pair selected branch'inde tutuluyor (!__ITEM_SHOW build).

		const int nFontHeight	= g_GetStringHeight(showstring , gpC_base->m_chatting_pi.hfont);
		const int nBoxHeight	= nFontHeight + (sizestringboxgap.cy << 1);

		if(bSelected)
		{
#if __CONTENTS(!__ITEM_SHOW)
			// 2026-05-15: Outer hoist v2 — inner release-reacquire pair.
			// FL2.cpp:99-102 kural: TEXT DC outstanding iken sprite Lock yasak.
			// DrawOutBox m_p_DDSurface_back->Lock() yapar; outer DC simdi acik,
			// once release sonra reacquire. (Round 2 regression bu adim eksikti.)
			g_FL2_ReleaseDC();  // inner release — DrawOutBox sprite Lock ihlali engelle
			gpC_global_resource->DrawOutBox(
				offset.x + ptstringposition.x - sizestringboxselectgap.cx,
				offset.y + ptstringposition.y - sizestringboxselectgap.cy,
				m_RectList.Width() + (sizestringboxselectgap.cx << 1),
				nBoxHeight + (sizestringboxselectgap.cy << 1));
			g_FL2_GetDC();      // inner reacquire — loop devamı icin outer DC restore
#endif //__ITEM_SHOW
		}

		ptstringposition.y += nBoxHeight;

#if __CONTENTS(__ITEM_SHOW)
		if(m_RectList.bottom<=ptstringposition.y+nBoxHeight)
			break;
#endif //__ITEM_SHOW
	}

	// 2026-05-15: Outer hoist v2 — OUTER RELEASE. Loop sonu DC bir kez release edilir.
	// Adim 6: loop SONRASI g_FL2_ReleaseDC(). Outer pair tamamlandi.
	// g_GetStringHeight (asagidaki nFontHeight call) FL2 GetDC idempotent — outer release
	// edildikten sonra kendi acquire/release pair'i yapar (regular pattern).
	g_FL2_ReleaseDC();

#if __CONTENTS(__ITEM_SHOW)
	int	nGapHeight	= m_StringBoxGap.cy << 1;
	int	nFontHeight	= GetCachedFontHeight(m_pPrintInfo->hfont);  // 2026-05-16 Dalga 2 C4

	if(((m_RectList.bottom-m_RectList.top)/(nFontHeight + nGapHeight))<m_ListString.size())
		m_pC_scroll_bar->Show(offset.x, offset.y);
#endif //__ITEM_SHOW
}

bool
C_VS_UI_LISTBOX::MouseControl(UINT message, int _x, int _y)
{
//	Window::MouseControl(message, _x, _y);
	
	CPoint ptLocalMouse(_x , _y );

	bool bClickMessage	= message == M_LEFTBUTTON_DOWN || message == M_LB_DOUBLECLICK;
	bool bMoveMessage	= message == M_LEFTBUTTON_DOWN || message == M_LB_DOUBLECLICK;

	int	nGapHeight	= m_StringBoxGap.cy << 1;
	int	nFontHeight	= GetCachedFontHeight(m_pPrintInfo->hfont);  // 2026-05-16 Dalga 2 C4
	int nCategoryY	= ptLocalMouse.y - m_RectList.top;
	int	nSelected	= nCategoryY / (nFontHeight + nGapHeight);

#if __CONTENTS(__ITEM_SHOW)
	bool scrool_bt = m_pC_scroll_bar->MouseControl(message, _x, _y);
		
	if(m_RectList.PtInRect(ptLocalMouse))
	{
		if(scrool_bt&&message==M_WHEEL_UP)
		{
			m_pC_scroll_bar->ScrollUp(1);
		}
		else if(scrool_bt&&message==M_WHEEL_DOWN)
		{
			if(m_pC_scroll_bar->GetScrollPos()<m_ListString.size()-(m_RectList.Height()/(nFontHeight + nGapHeight))
					&& m_ListString.size()>((m_RectList.bottom-m_RectList.top)/(nFontHeight + nGapHeight)))
			{
				m_pC_scroll_bar->ScrollDown(1);
			}
		}
	}
#endif //__ITEM_SHOW

	//////////////////////////////////////////////////////////////////////////
	// ī�װ��� ���콺 ó��
	if(bClickMessage && m_RectList.PtInRect(ptLocalMouse)
#if __CONTENTS(__ITEM_SHOW)
		&& scrool_bt
#endif //__ITEM_SHOW
		)
	{

		// �ƽ��� �Ѱ����� ó�� ����� �ϱ�.. ��.��;;
		if(nSelected >= 0 
#if __CONTENTS(!__ITEM_SHOW)
			&& nSelected <= m_MAXIndex
#else
			&& nSelected <= m_ListString.size()
#endif //__ITEM_SHOW
			)
#if __CONTENTS(__ITEM_SHOW)
		{
			m_CurrentIndex = nSelected+m_pC_scroll_bar->GetScrollPos();

			if( message == M_LB_DOUBLECLICK )
				SetDoubleClickIndex(m_CurrentIndex);
		}
#else
			m_CurrentIndex = nSelected;
#endif //__ITEM_SHOW
		return false;
	}
#if __CONTENTS(__ITEM_SHOW)
	else if(m_RectList.PtInRect(ptLocalMouse) && scrool_bt)
	{
		if(nSelected >= 0 && (nSelected <= m_MAXIndex || nSelected <= m_ListString.size()))
			m_OverIndex = nSelected;

		if(m_ListString.size()!=0)
		{
			static char st_str_Description[1024] = {""};
			STRING_LIST::iterator listindex;
			int count = 0;
			// find..
			COLORREF _color = m_ColorDefault;

			for(listindex=m_ListString.begin(); listindex != m_ListString.end(); ++listindex, ++count)
			{
				if(count==m_OverIndex+m_pC_scroll_bar->GetScrollPos())
				{
					sprintf(st_str_Description,listindex->c_str());

					// 2026-05-16: FPS optimization — pre-computed lowercase member'lari kullan.
					// Bu loop hover-aware; selected item bulununca break, ama erken iter'larda redundant transform.
					// �� �ұ��� ���� �˻�
					string _str	= st_str_Description;
					std::for_each(_str.begin(), _str.end(), makeLowerChar);

					// find..
					if( m_find_string_lower.size()!=0 && _str.find(m_find_string_lower.c_str()) != std::string::npos )
						_color = m_ColorFind;

					// Null..
					if( m_null_string_lower.size()!=0 && _str.find(m_null_string_lower.c_str()) != std::string::npos )
						*st_str_Description = 0;

					break;
				}
			}
	
			if(lstrlen(st_str_Description)!=0 && b_Description)
				g_descriptor_manager.Set(DID_MULTILINE_INFO, gpC_mouse_pointer->GetX(), gpC_mouse_pointer->GetY()+100, (void *)st_str_Description,_color,60);
		}	
		return false;
	}
	else
	{
		m_OverIndex = -1;
	}
#endif //__ITEM_SHOW

	return true;
}

void
C_VS_UI_LISTBOX::KeyboardControl(UINT message, UINT key, long extra)
{
}
LPCSTR
C_VS_UI_LISTBOX::GetSelectListString(int selectindex)
{
	STRING_LIST::iterator listindex;
	int		count		= 0;

	for(listindex = m_ListString.begin(); listindex != m_ListString.end(); ++listindex, ++count)
	{
		if(count == selectindex)
			return listindex->c_str();
	}
	return NULL;
}

bool
C_VS_UI_LISTBOX::AddListNode(int index, std::string addstring)
{
	STRING_LIST::iterator listindex;
	int		count		= 0;

	for(listindex = m_ListString.begin(); listindex != m_ListString.end(); ++listindex, ++count)
	{
		if(index==count)
		{
#if __CONTENTS(__ITEM_SHOW)
			// Null..
			if(addstring.size()<=0)
				addstring = m_null_string;
#endif //__ITEM_SHOW
			
			m_ListString.insert(listindex , addstring);
			return true;
		}
	}

#if __CONTENTS(__ITEM_SHOW)
	int	nGapHeight	= m_StringBoxGap.cy << 1;
	int	nFontHeight	= GetCachedFontHeight(m_pPrintInfo->hfont);  // 2026-05-16 Dalga 2 C4

	m_pC_scroll_bar->SetPosMax(m_ListString.size()-(m_RectList.Height()/(nFontHeight + nGapHeight))+1);
#endif //__ITEM_SHOW
	return false;
}

#if __CONTENTS(__ITEM_SHOW)
bool
C_VS_UI_LISTBOX::AddString(std::string addstring)
{
	int	nGapHeight	= m_StringBoxGap.cy << 1;
	int	nFontHeight	= GetCachedFontHeight(m_pPrintInfo->hfont);  // 2026-05-16 Dalga 2 C4

	// Null..
	if(addstring.size()<=0)
		addstring = m_null_string;

	// 2026-05-16: FPS optimization (bonus, hot path degil) — _strfind redundant alloc kaldirildi.
	// AddString N item icin N kez cagrilir; pre-computed member kullanim hot-path-equivalent kazanim.
	// �� �ұ��� ���� �˻�
	string _str	= addstring;
	std::for_each(_str.begin(), _str.end(), makeLowerChar);

	// find..
	if( m_find_string_lower.size()!=0 && _str.find(m_find_string_lower.c_str()) != std::string::npos )
	{
		m_ListString.push_front(addstring);
	}
	else
	{
		m_ListString.push_back(addstring);
	}
	m_pC_scroll_bar->SetPosMax(m_ListString.size()-(m_RectList.Height()/(nFontHeight + nGapHeight))+1);
	return true;
}

void
C_VS_UI_LISTBOX::SetListStringColor(int Num, COLORREF Color)
{
	m_ColorMapString[Num] = Color;
}
#endif //__ITEM_SHOW

bool
C_VS_UI_LISTBOX::DelListNode(int index)
{
	STRING_LIST::iterator listindex;
	int		count		= 0;

	for(listindex = m_ListString.begin(); listindex != m_ListString.end(); ++listindex, ++count)
	{
		if(index==count)
		{
			m_ListString.erase(listindex);
			return true;
		}
	}
	return false;
}

#if __CONTENTS(__ITEM_SHOW)
bool C_VS_UI_LISTBOX::DelListNodeAll()
{
	STRING_LIST::iterator listindex = m_ListString.begin();
	
	while (listindex != m_ListString.end()) 
		m_ListString.erase(listindex++);
	
	m_pC_scroll_bar->SetPosMax(0);
	return true;
}

int C_VS_UI_LISTBOX::GetScrollPos()
{
	return m_pC_scroll_bar->GetScrollPos();
}

void C_VS_UI_LISTBOX::SetScrollPos(int _Pos)
{
	m_pC_scroll_bar->SetScrollPos(_Pos);
}

int C_VS_UI_LISTBOX::ListNodeMax()
{
	return m_ListString.size();
}

// find..
void C_VS_UI_LISTBOX::SetFindString(string _str)
{
	m_find_string = _str;

	// 2026-05-16: FPS optimization — pre-compute lowercase copy bir kez.
	// Show() loop her frame bunu okuyor, setter cagrildiginda hesapla.
	m_find_string_lower = m_find_string;
	std::for_each(m_find_string_lower.begin(), m_find_string_lower.end(), makeLowerChar);

	STRING_LIST::iterator listindex;
	for(listindex=m_ListString.begin(); listindex != m_ListString.end(); ++listindex)
	{
		// 2026-05-16: _strfind redundant alloc + transform per-iter kaldirildi — m_find_string_lower kullan.
		string _str	= listindex->c_str();
		// �� �ұ��� ���� �˻�
		std::for_each(_str.begin(), _str.end(), makeLowerChar);
		if( m_find_string_lower.size()!=0 && _str.find(m_find_string_lower.c_str()) != std::string::npos )
		{
			m_bfindOK = true;
			return;
		}
	}
	m_bfindOK = false;
}
#endif //__ITEM_SHOW
