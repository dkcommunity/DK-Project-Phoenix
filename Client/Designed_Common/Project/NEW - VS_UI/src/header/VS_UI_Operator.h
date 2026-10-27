/*-----------------------------------------------------------------------------

	VS_UI_Operator.h

	Operator Interface.

	2007.01.12 chyaya

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_OPERATOR_H__
#define __VS_UI_OPERATOR_H__

#include "VS_UI_Widget.h"
#include "VS_UI_Util.h"
#include "VS_UI_GameCommon.h"
#include "VS_UI_Dialog.h"
#include "VS_UI_DESC.h"


class COperatorCategory;

//-----------------------------------------------------------------------------
// C_VS_UI_OPERATOR
//
//-----------------------------------------------------------------------------
class C_VS_UI_OPERATOR : public Window, public Exec, public ButtonVisual
{
public:
	
private:
	enum EXEC_ID
	{
		EID_SET_DEFAULT,
		EID_CLOSE,
#if __CONTENTS(__MOVE_ZONE)
		EID_MOVE_ZONE,
#endif //__MOVE_ZONE
	};

	enum WEBBROWSER_SPK_INDEX
	{
		WEBBROWSER_MAIN, 
		WEBBROWSER_CLOSE_BUTTON , 
		WEBBROWSER_CLOSE_BUTTON_HILIGHTED , 
		WEBBROWSER_CLOSE_BUTTON_PUSHED ,
	};

	enum CATEGORY_ID
	{
		CATEGORY_UTILITY,
		CATEGORY_SHOWINFO,
		CATEGORY_GAMESETTING,
		CATEGORY_SIMPLECOMMAND,
#if __CONTENTS(__ITEM_SHOW)
		CATEGORY_ITEM_SHOW,
#endif //__ITEM_SHOW
		CATEGORY_MAX,
	};

	bool				m_bDelete_Operator;
	POINT				m_MousePoint;

	ButtonGroup*		m_pC_button_group;

	CSize				m_size_window;
	
	CRect				m_rect_category;
	CRect				m_rect_detail;

	COperatorCategory*	m_pC_category[CATEGORY_MAX];

	static	CATEGORY_ID	s_selected_category_id;

public:
	C_VS_UI_OPERATOR();
	~C_VS_UI_OPERATOR();
	
	void	Start();
	void	Finish();
	void	CloseOperator()	{ m_bDelete_Operator = true; }
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	
	bool	IsPixel(int _x, int _y);

	void	Run(id_t id);
	void	Process();

	void	Show();
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	static	bool	IsValidCategoryID(CATEGORY_ID cid)	{ return cid >= 0 && cid < CATEGORY_MAX;	}
};


#endif	//__VS_UI_OPERATOR_H__