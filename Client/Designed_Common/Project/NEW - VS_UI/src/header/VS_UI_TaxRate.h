/*-----------------------------------------------------------------------------

	VS_UI_TaxRate.h

	2000.12.6. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_TAXRATE_H__
#define __VS_UI_TAXRATE_H__

#include "VS_UI_Widget.h"
#include "VS_UI_Util.h"


#define MAX_CHECKTAXRATE	11
//-----------------------------------------------------------------------------
// class VS_UI_TAXRATE
//
// °ø¼ºÀü ¼ÎÀ²
//
//-----------------------------------------------------------------------------
class VS_UI_TAXRATE : public Window, public Exec, public ButtonVisual
{



private:
	ButtonGroup		*	m_pC_button_group;
	C_SPRITE_PACK	*	m_pC_TaxRate_spk;

	
	enum EXEC_ID
	{
		_5PERSENT   = 0 , 
		_10PERSENT , 
		_15PERSENT ,
		_20PERSENT ,
		_25PERSENT ,

		_30PERSENT  , 
		_35PERSENT , 
		_40PERSENT ,
		_45PERSENT ,
		_50PERSENT ,

		_ZEROPERSENT ,

		OK_ID ,
		CANCEL_ID,

 
	}	;
	

	enum CHECK_TYPE
	{
		CHECK_NOT,
		CHECK_CHECK,
		CHECK_CHECK_DISABLE = 0xFFFE,
		CHECK_DISABLE = 0xFFFF,
	};
	
	enum INTERFACE_TEXRATE_ID
	{
		RADIO_BACK_DISABLE,
		RADIO_BACK_ENABLE,
		RADIO_FOCUS,
	};


private :
	
	int				m_TexRate			;
	CHECK_TYPE		m_check[MAX_CHECKTAXRATE]	;
	int				m_TaxRateArray[MAX_CHECKTAXRATE]	;


public:
	
	
	
	 VS_UI_TAXRATE(int	TexRate);
	~VS_UI_TAXRATE();

	void	Process();
	void	UnacquireMouseFocus();
	bool	IsPixel(int _x, int _y);
	void	Run(id_t id);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	
	void	Finish();
	void	Start();
	void	Show();
};


#endif