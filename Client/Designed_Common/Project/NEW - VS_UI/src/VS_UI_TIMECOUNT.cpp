#include "client_PCH.h"
#include "crash_trace_tool.h"

//#pragma warning(disable:4786)
#include "VS_UI_GameCommon.h"
#include "VS_UI.h"
#include "VS_UI_filepath.h"
#include "VS_UI_mouse_pointer.h"
#include "VS_UI_TIMECOUNT.h"

extern "C" unsigned __int64 Phoenix_TextPerf_Begin();
extern "C" void Phoenix_TextPerf_AddStageTicks(int stage, unsigned __int64 beginTick);
extern "C" void Phoenix_TextPerf_PushCategory(int stage);
extern "C" void Phoenix_TextPerf_PopCategory();
extern "C" void Phoenix_FL2_BeginTextFrame();
extern "C" void Phoenix_FL2_EndTextFrame();

enum
{
	PHOENIX_TEXT_STAGE_UI_LABEL = 9
};

class PhoenixUITimeTextPerfScope
{
public:
	explicit PhoenixUITimeTextPerfScope(int stage)
		: m_stage(stage), m_beginTick(Phoenix_TextPerf_Begin())
	{
		Phoenix_FL2_BeginTextFrame();
		Phoenix_TextPerf_PushCategory(stage);
	}

	~PhoenixUITimeTextPerfScope()
	{
		Phoenix_TextPerf_AddStageTicks(m_stage, m_beginTick);
		Phoenix_TextPerf_PopCategory();
		Phoenix_FL2_EndTextFrame();
	}

private:
	int m_stage;
	unsigned __int64 m_beginTick;
};
C_VS_UI_TIME_COUNT::C_VS_UI_TIME_COUNT(DWORD time, LPCTSTR desc)
{
	g_RegisterWindow(this);

	//당장은 안쓰더라도 왠지 쓸것 같아서 버튼은 추가 시켜 놓는다.
	m_pC_button_group	= new ButtonGroup(this);
	m_pC_button_group->Init();

	// Time은 1초단위
	m_end_time = GetTickCount() + time * 1000 + 999;

	// 종족별 배경 선택
	switch(g_eRaceInterface)
	{
	case RACE_SLAYER:	m_bg_index = TIME_COUNT_SLAYER;		break;
	case RACE_VAMPIRE:	m_bg_index = TIME_COUNT_VAMPIRE;	break;
	case RACE_OUSTERS:	m_bg_index = TIME_COUNT_OUSTERS;	break;
	}

	// desc가 NULL이거나 길이가 0이면
	if(desc == NULL || strlen(desc) == 0)
	{
		m_description = "남은 시간";
	}
	else
	{
		m_description = desc;
	}

	m_time_color = RGB_WHITE;
}

C_VS_UI_TIME_COUNT::~C_VS_UI_TIME_COUNT()
{
	g_UnregisterWindow(this);

	DeleteNew(m_pC_button_group);
	DeleteNew(m_pC_time_count);
}
	
void C_VS_UI_TIME_COUNT::Start()
{
	PI_Processor::Start();

	gpC_window_manager->AppearWindow(this);
	
	m_pC_time_count = new C_SPRITE_PACK(SPK_TIME_COUNT);

	// 윈도우 크기와 위치 세팅
	int nW = m_pC_time_count->GetWidth(m_bg_index);
	int nH = m_pC_time_count->GetHeight(m_bg_index);

	int nX = (RESOLUTION_X - nW) / 2;
	int nY = 45;

	Set(nX,	nY,	nW, nH);
}
void C_VS_UI_TIME_COUNT::Finish()
{
	PI_Processor::Finish();

	gpC_window_manager->DisappearWindow(this);

	DeleteNew(m_pC_time_count);
}
	
void C_VS_UI_TIME_COUNT::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{

}
void C_VS_UI_TIME_COUNT::ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button)
{

}
	
bool C_VS_UI_TIME_COUNT::IsPixel(int _x, int _y)
{
	if(Moving()) return true;
	return IsInRect(_x, _y);
}
	
void C_VS_UI_TIME_COUNT::Run(id_t id)
{
}

void C_VS_UI_TIME_COUNT::Process()
{
	UpdateTimer();

	m_pC_button_group->Process();
}

void C_VS_UI_TIME_COUNT::Show()
{
	CT_Done("WnSh: C_VS_UI_TIME_COUNT::Show ENTER pthis=%p", (void*)this);
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		m_pC_time_count->BltLocked(x, y, m_bg_index);
		
		gpC_base->m_p_DDSurface_back->Unlock();
	}

	{
		PhoenixUITimeTextPerfScope textPerfScope(PHOENIX_TEXT_STAGE_UI_LABEL);
	// 설명 출력
	g_PrintColorStrShadow(x + 8, y + 9,
		m_description.c_str(), gpC_base->m_item_desc_pi,
		RGB_WHITE, RGB_BLACK);
	
	// 시간 출력
	int timeX	= x + 45;
	int timeY	= y + 26;
	int timeGap = 32;
	
	char szBuf[4];
	
	sprintf(szBuf, "%02d", m_hour);
	
	g_PrintColorStrShadow(timeX, timeY,
		szBuf, gpC_base->m_char_chat_large_pi,
		m_time_color, RGB_BLACK);
	
	timeX += timeGap;
	
	sprintf(szBuf, "%02d", m_min);
	
	g_PrintColorStrShadow(timeX, timeY,
		szBuf, gpC_base->m_char_chat_large_pi,
		m_time_color, RGB_BLACK);
	
	timeX += timeGap;
	
	sprintf(szBuf, "%02d", m_sec);
	
	g_PrintColorStrShadow(timeX, timeY,
		szBuf, gpC_base->m_char_chat_large_pi,
		m_time_color, RGB_BLACK);
	}


//	const int nFontWidth = g_GetStringWidth(m_timestr.c_str(), gpC_base->m_char_chat_large_pi.hfont);

//	g_PrintColorStrShadow(timer.left + timer.Width() - nFontWidth - 3, timer.top + 10,
//			m_timestr.c_str(), gpC_base->m_char_chat_large_pi, RGB_WHITE, RGB_BLACK);
	
//	g_PrintColorStrShadow(timer.left + timer.Width() - nFontWidth - 3, timer.top,
//			"가나다라마바사아자차", gpC_base->m_item_desc_pi, RGB_WHITE, RGB_BLACK);
		
//	g_PrintColorStrShadow(timer.left + timer.Width() - nFontWidth - 3, timer.top + 15,
//			"01234567890123456789", gpC_base->m_item_desc_pi, RGB_WHITE, RGB_BLACK);

//	g_PrintColorStrShadow(timer.left + timer.Width() - nFontWidth - 3, timer.top + 30,
//			"01234567890123456789", gpC_base->m_char_chat_large_pi, RGB_WHITE, RGB_BLACK);
	
}

	
bool C_VS_UI_TIME_COUNT::MouseControl(UINT message, int _x, int _y)
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
	
void C_VS_UI_TIME_COUNT::KeyboardControl(UINT message, UINT key, long extra)
{
}
void C_VS_UI_TIME_COUNT::WindowEventReceiver(id_t event)
{

}


void C_VS_UI_TIME_COUNT::UpdateTimer()
{
	if(GetTickCount() >= m_end_time)
	{
		Finish();
	}

	DWORD restTime	= m_end_time - GetTickCount();
	DWORD restSec	= restTime / 1000;

	if(restSec < 60)
	{
		if((restTime / 500) % 2)
		{
			m_time_color = RGB_RED;
		}
		else
		{
			m_time_color = RGB_WHITE;
		}
	}
	
	m_day		= restSec / 86400;
	restTime	= restSec % 86400;
	
	m_hour		= (restSec / 3600)%24;
	restTime	= restSec % 3600;
	
	m_min		= (restSec / 60)%60;
	m_sec		= restSec % 60;
}
