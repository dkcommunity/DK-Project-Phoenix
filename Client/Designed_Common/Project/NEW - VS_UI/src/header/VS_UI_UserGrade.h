#ifndef __C_VS_UI_USER_GRADE__
#define __C_VS_UI_USER_GRADE__

#include "VS_UI_Widget.h"
#include "VS_UI_Util.h"
#include "VS_UI_GameCommon.h"
#include "VS_UI_Dialog.h"
#include "VS_UI_DESC.h"



class C_VS_UI_USER_GRADE : public Window, public Exec, public ButtonVisual
{

public:
#if __CONTENTS(__080405_FIREST_UI_UPDATE)
	enum MENU
	{
		CANCEL,
		INVALID_ID
	};
#endif //__080405_FIREST_UI_UPDATE
	
	C_VS_UI_USER_GRADE();
	~C_VS_UI_USER_GRADE();
	
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

private:
	enum USEGRADE
	{
		CHILD,
		ADULT,
		VIOLENCE,
		FEAR,
	};
	ButtonGroup*		m_pC_button_group;
	
	C_SPRITE_PACK*		m_pC_use_grade;
};
#endif //__C_VS_UI_USER_GRADE__