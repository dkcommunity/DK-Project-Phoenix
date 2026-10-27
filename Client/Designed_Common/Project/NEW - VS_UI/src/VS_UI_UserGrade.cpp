#include "client_PCH.h"
#include "crash_trace_tool.h"

//#pragma warning(disable:4786)
#include "VS_UI_GameCommon.h"
#include "VS_UI.h"
#include "VS_UI_filepath.h"
#include "VS_UI_mouse_pointer.h"
#include "VS_UI_UserGrade.h"
#include "MGameStringTable.h"

C_VS_UI_USER_GRADE::C_VS_UI_USER_GRADE()
{
	g_RegisterWindow(this);

	//당장은 안쓰더라도 왠지 쓸것 같아서 버튼은 추가 시켜 놓는다.
	m_pC_button_group = new ButtonGroup(this);
	m_pC_button_group->Init();

	m_pC_use_grade = new C_SPRITE_PACK(SPK_USE_GRADE);
	//윈도우를 등록한다.

	int nW = m_pC_use_grade->GetWidth(ADULT) + m_pC_use_grade->GetWidth(VIOLENCE) + m_pC_use_grade->GetWidth(FEAR) + 40;
	int nH = m_pC_use_grade->GetHeight(ADULT) + 60;

	int nX = (RESOLUTION_X - nW)/2;
	int nY = (RESOLUTION_Y - nH)/2;

#if __CONTENTS(__080405_FIREST_UI_UPDATE)
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(148,141, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_CLOSE), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_CLOSE), CANCEL, this, C_GLOBAL_RESOURCE::AB_BUTTON_CLOSE));
#endif //__080405_FIREST_UI_UPDATE
	
	Set(nX,	nY,	nW, nH);
}

C_VS_UI_USER_GRADE::~C_VS_UI_USER_GRADE()
{
	g_UnregisterWindow(this);
	
	DeleteNew(m_pC_use_grade);
	DeleteNew(m_pC_button_group);
}
	
void C_VS_UI_USER_GRADE::Start()
{
	PI_Processor::Start();
	gpC_window_manager->AppearWindow(this);
}
void C_VS_UI_USER_GRADE::Finish()
{
	PI_Processor::Finish();

	gpC_window_manager->DisappearWindow(this);
	DeleteNew(m_pC_use_grade);
}
	
void C_VS_UI_USER_GRADE::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
#if __CONTENTS(__080405_FIREST_UI_UPDATE)
	if(p_button->GetFocusState())
	{
		if(p_button->GetPressState())
			gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->x+x, p_button->y+y, p_button->m_image_index+C_GLOBAL_RESOURCE::AB_BUTTON_PUSHED_OFFSET);
		else
			gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->x+x, p_button->y+y, p_button->m_image_index+C_GLOBAL_RESOURCE::AB_BUTTON_HILIGHTED_OFFSET);
	}
	else
		gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->x+x, p_button->y+y, p_button->m_image_index);
#endif //__080405_FIREST_UI_UPDATE
}
void C_VS_UI_USER_GRADE::ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button)
{

}
	
bool C_VS_UI_USER_GRADE::IsPixel(int _x, int _y)
{
	if(Moving()) return true;
	return IsInRect(_x, _y);
}
	
void C_VS_UI_USER_GRADE::Run(id_t id)
{
#if __CONTENTS(__USER_GRADE)
	switch (id)
	{
	case CANCEL:
		gC_vs_ui.ToggleUserGrade();
		break;
	}
#endif //__USER_GRADE
}

void C_VS_UI_USER_GRADE::Process()
{
	m_pC_button_group->Process();
}

void C_VS_UI_USER_GRADE::Show()
{
	CT_Done("WnSh: C_VS_UI_USER_GRADE::Show ENTER pthis=%p", (void*)this);
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		gpC_global_resource->DrawDialogLocked(x, y, w, h, GetAttributes()->alpha);

		int drowx = x + w - 20;
		int drowy = y + 20;
		static char szBuf[128];

		drowx -= m_pC_use_grade->GetWidth(ADULT);
		if(g_pUserInformation->GoreLevel)
		{
			m_pC_use_grade->BltLocked(drowx,drowy,ADULT);
			sprintf(szBuf, (*g_pGameStringTable)[UI_STRING_MESSAGE_USERGRADE_18].GetString());
		}
		else
		{
			m_pC_use_grade->BltLocked(drowx,drowy,CHILD);
			sprintf(szBuf, (*g_pGameStringTable)[UI_STRING_MESSAGE_USERGRADE_12].GetString());
		}
		drowx -= m_pC_use_grade->GetWidth(VIOLENCE);
		m_pC_use_grade->BltLocked(drowx,drowy,VIOLENCE);
		drowx -= m_pC_use_grade->GetWidth(FEAR);
		m_pC_use_grade->BltLocked(drowx,drowy,FEAR);
		drowy += m_pC_use_grade->GetHeight(ADULT);
		
	#if __CONTENTS(__080405_FIREST_UI_UPDATE)
		m_pC_button_group->Show();
	#endif //__080405_FIREST_UI_UPDATE
		
		gpC_base->m_p_DDSurface_back->Unlock();

	#if __CONTENTS(__080405_FIREST_UI_UPDATE)
	//	g_PrintColorStrShadow(x + (w - g_GetStringWidth(szBuf,gpC_base->m_char_chat_large_pi.hfont))/2, drowy,
	//		szBuf,gpC_base->m_char_chat_large_pi,RGB_WHITE);
	#else
		g_PrintColorStrShadow(x + (w - g_GetStringWidth(szBuf,gpC_base->m_char_chat_large_pi.hfont))/2, drowy,
			szBuf,gpC_base->m_char_chat_large_pi,RGB_WHITE);
	#endif //__080405_FIREST_UI_UPDATE

	}
	
}

	
bool C_VS_UI_USER_GRADE::MouseControl(UINT message, int _x, int _y)
{
	Window::MouseControl(message, _x, _y);
	CPoint ptLocalMouse(_x - x, _y - y);
	
	bool re;
	re = m_pC_button_group->MouseControl(message, ptLocalMouse.x, ptLocalMouse.y);
	switch(message)
	{
	case M_MOVING:
		break;
		
	case M_LEFTBUTTON_DOWN:
	case M_LB_DOUBLECLICK:
		// 반투명 상태에서 맵 클릭하면 이동되게
		if (gpC_mouse_pointer->GetPickUpItem() == false && re)
		{
			MoveReady();
			SetOrigin(ptLocalMouse.x, ptLocalMouse.y);
			break;
		}
		break;
	}
	return true;
}
	
void C_VS_UI_USER_GRADE::KeyboardControl(UINT message, UINT key, long extra)
{
}
void C_VS_UI_USER_GRADE::WindowEventReceiver(id_t event)
{
}
