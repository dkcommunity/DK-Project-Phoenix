#ifndef __VS_UI_TIMECOUNT_H__
#define __VS_UI_TIMECOUNT_H__

#include "VS_UI_Widget.h"
#include "VS_UI_Util.h"
#include "VS_UI_GameCommon.h"
#include "VS_UI_Dialog.h"
#include "VS_UI_DESC.h"



class C_VS_UI_TIME_COUNT : public Window, public Exec, public ButtonVisual
{

public:
	enum TIME_COUNT_SPK
	{
		TIME_COUNT_SLAYER,
		TIME_COUNT_VAMPIRE,
		TIME_COUNT_OUSTERS,
		TIME_COUNT_MAX,	
	};

	C_VS_UI_TIME_COUNT(DWORD time, LPCTSTR desc);
	~C_VS_UI_TIME_COUNT();
	
	void	Start();
	void	Finish();
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);
	
	bool	IsPixel(int _x, int _y);
	
	void	Run(id_t id);
	void	Process();
	
	void	Show();
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	UpdateTimer();

private:
	ButtonGroup*		m_pC_button_group;
	DWORD				m_end_time;

	COLORREF			m_time_color;

	C_SPRITE_PACK *		m_pC_time_count;
	int					m_bg_index;

	std::string			m_description;
	int					m_day;
	int					m_hour;
	int					m_min;
	int					m_sec;
	int					m_strWidth;
};
#endif //__VS_UI_TIMECOUNT_H__