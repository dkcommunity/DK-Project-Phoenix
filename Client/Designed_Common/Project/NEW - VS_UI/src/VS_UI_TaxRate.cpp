// VS_UI_ExtraDialog.cpp

#include "client_PCH.h"
#include "crash_trace_tool.h"

#pragma warning(disable:4786)
#include "VS_UI.h"
#include "MGameStringTable.H"
#include "VS_UI_TaxRate.H"
#include "VS_UI_filepath.h"


VS_UI_TAXRATE::VS_UI_TAXRATE(int	TexRate)
{
	AttrTopmost(false);
	AttrPin(true);
	g_RegisterWindow(this);	
	
	
	TexRate -= 100 ; 

	if(TexRate > 50) m_TexRate = 50 ; 
	else if(TexRate < 0 ) m_TexRate = 10 ; 
	else if(TexRate % 5 != 0)
	{
		int iRato = TexRate / 5 ; 
		m_TexRate = iRato * 5 ; 
	}
	else	m_TexRate = TexRate  ;

	

	/*m_check[_5PERSENT]		= (m_TexRate == 5)  ? CHECK_CHECK_DISABLE : CHECK_DISABLE;
	m_check[_10PERSENT]		= (m_TexRate == 10) ? CHECK_CHECK_DISABLE : CHECK_DISABLE;;
	m_check[_15PERSENT]		= (m_TexRate == 15) ? CHECK_CHECK_DISABLE : CHECK_DISABLE;
	m_check[_20PERSENT]		= (m_TexRate == 20) ? CHECK_CHECK_DISABLE : CHECK_DISABLE;;
	m_check[_25PERSENT]		= (m_TexRate == 25) ? CHECK_CHECK_DISABLE : CHECK_DISABLE;


	m_check[_30PERSENT]		= (m_TexRate == 30) ? CHECK_CHECK_DISABLE : CHECK_DISABLE;;
	m_check[_35PERSENT]		= (m_TexRate == 35) ? CHECK_CHECK_DISABLE : CHECK_DISABLE;
	m_check[_40PERSENT]		= (m_TexRate == 40) ? CHECK_CHECK_DISABLE : CHECK_DISABLE;;
	m_check[_45PERSENT]		= (m_TexRate == 45) ? CHECK_CHECK_DISABLE : CHECK_DISABLE;
	m_check[_50PERSENT]		= (m_TexRate == 50) ? CHECK_CHECK_DISABLE : CHECK_DISABLE;;

	m_check[_ZEROPERSENT]	= (m_TexRate == 0)  ? CHECK_CHECK_DISABLE : CHECK_DISABLE;;

*/


	m_check[_5PERSENT]		= CHECK_TYPE(m_TexRate == 5)  ;
	m_check[_10PERSENT]		= CHECK_TYPE(m_TexRate == 10) ;
	m_check[_15PERSENT]		= CHECK_TYPE(m_TexRate == 15) ;
	m_check[_20PERSENT]		= CHECK_TYPE(m_TexRate == 20) ;
	m_check[_25PERSENT]		= CHECK_TYPE(m_TexRate == 25) ;


	m_check[_30PERSENT]		= CHECK_TYPE(m_TexRate == 30) ;
	m_check[_35PERSENT]		= CHECK_TYPE(m_TexRate == 35) ;
	m_check[_40PERSENT]		= CHECK_TYPE(m_TexRate == 40) ;
	m_check[_45PERSENT]		= CHECK_TYPE(m_TexRate == 45) ;
	m_check[_50PERSENT]		= CHECK_TYPE(m_TexRate == 50) ;

	m_check[_ZEROPERSENT]	= CHECK_TYPE(m_TexRate == 0)  ;

	
	
	m_TaxRateArray[_5PERSENT]	= 5  ; 
	m_TaxRateArray[_10PERSENT]	= 10 ;
	m_TaxRateArray[_15PERSENT]	= 15 ;
	m_TaxRateArray[_20PERSENT]	= 20 ;
	m_TaxRateArray[_25PERSENT]	= 25 ; 

	m_TaxRateArray[_30PERSENT]	= 30 ;
	m_TaxRateArray[_35PERSENT]	= 35 ;
	m_TaxRateArray[_40PERSENT]	= 40 ; 
	m_TaxRateArray[_45PERSENT]	= 45 ;	
	m_TaxRateArray[_50PERSENT]	= 50 ; 

	m_TaxRateArray[_ZEROPERSENT] = 0 ;


	 
	m_pC_button_group = NULL ; 
	m_pC_TaxRate_spk = NULL;
	m_pC_TaxRate_spk = new C_SPRITE_PACK(SPK_TAXRATE);
		
	int Width  = 350 ; 
	int Height = 160 ; 


	Set(g_pUserInformation->iResolution_x/2-Width/2,  g_pUserInformation->iResolution_y/2-Height/2, Width, Height)	;
	m_pC_button_group = new ButtonGroup(this)	;

	int	ok_button_x = Width - (gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_OK) * 2) - 25 ;
	int	button_y = 110 ;
	int	cancel_button_x = Width - gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_OK) - 20 ;
	

	int m_check_x  = 0  ;
	int m_check_y  = 0  ; 


	int iButtonID = _5PERSENT ; 


	for(int iLoop = 0 ;  iLoop <= _ZEROPERSENT ;  iLoop++)
	{
			
		m_check_x = iLoop % 5 ; 
		m_check_y = iLoop / 5 ;
		
		m_check_x = (m_check_x * 60) + 20 ; 
		m_check_y = (m_check_y * 25) + 60 ;
		
		m_pC_button_group->Add( new C_VS_UI_EVENT_BUTTON(m_check_x, m_check_y, m_pC_TaxRate_spk->GetWidth(RADIO_BACK_DISABLE), m_pC_TaxRate_spk->GetHeight(RADIO_BACK_DISABLE), iButtonID , this, RADIO_BACK_DISABLE) );
		iButtonID++ ; 

	}


	//m_pC_button_group->Add( new C_VS_UI_EVENT_BUTTON(m_check_x+10, m_check_y, m_pC_TaxRate_spk->GetWidth(RADIO_BACK_DISABLE), m_pC_TaxRate_spk->GetHeight(RADIO_BACK_DISABLE), _5PERSENT, this, RADIO_BACK_DISABLE) );
	//m_pC_button_group->Add( new C_VS_UI_EVENT_BUTTON(m_check_x+120, m_check_y, m_pC_TaxRate_spk->GetWidth(RADIO_BACK_DISABLE), m_pC_TaxRate_spk->GetHeight(RADIO_BACK_DISABLE), _10PERSENT, this, RADIO_BACK_DISABLE) );

	//m_pC_button_group->Add( new C_VS_UI_EVENT_BUTTON(m_check_x+190, m_check_y, m_pC_TaxRate_spk->GetWidth(RADIO_BACK_DISABLE), m_pC_TaxRate_spk->GetHeight(RADIO_BACK_DISABLE), _15PERSENT, this, RADIO_BACK_DISABLE) );

		


	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(ok_button_x, button_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_OK), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_OK), OK_ID, this, C_GLOBAL_RESOURCE::AB_BUTTON_OK));
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(cancel_button_x, button_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_CANCEL), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_CANCEL), CANCEL_ID, this, C_GLOBAL_RESOURCE::AB_BUTTON_CANCEL));



	//m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(help_button_x, help_button_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_HELP), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_HELP), SHOP_HELP_ID, this, C_GLOBAL_RESOURCE::AB_BUTTON_HELP));
	

	//m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(779 - m_pC_WebBrowser_spk->GetWidth(WEBBROWSER_CLOSE_BUTTON) - 3 , 5 , m_pC_WebBrowser_spk->GetWidth(WEBBROWSER_CLOSE_BUTTON), m_pC_WebBrowser_spk->GetHeight(WEBBROWSER_CLOSE_BUTTON), WEBBROWSER_CLOSE, this, WEBBROWSER_CLOSE_BUTTON));	 

//	m_pC_button_group->Add(	new C_VS_UI_EVENT_BUTTON(DIALOG_BUTTON_POS(BS_OK), DIALOG_BUTTON_Y, DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT, DIALOG_EXECID_OK, this, C_GLOBAL_RESOURCE::AB_BUTTON_OK));
	
	//m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(DIALOG_BUTTON_POS(BS_CANCEL), DIALOG_BUTTON_Y, DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT, DIALOG_EXECID_CANCEL, this, C_GLOBAL_RESOURCE::AB_BUTTON_CANCEL));

}

VS_UI_TAXRATE::~VS_UI_TAXRATE()
{
	g_UnregisterWindow(this);
	if(m_pC_TaxRate_spk)
	{
		DeleteNew(m_pC_TaxRate_spk);
		m_pC_TaxRate_spk = NULL;
	}

	DeleteNew(m_pC_button_group);

}

void	VS_UI_TAXRATE::Show()
{
	CT_Done("WnSh: VS_UI_TAXRATE::Show ENTER");
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		gpC_global_resource->DrawDialogLocked(x, y, w, h)	;
		
		
		m_pC_button_group->Show()  ; 
		gpC_base->m_p_DDSurface_back->Unlock();

	}


	int m_check_x  = 0  ;
	int m_check_y  = 0  ; 

	for(int iLoop = 0 ;  iLoop <= _ZEROPERSENT ;  iLoop++)
	{
			
		m_check_x = iLoop % 5 ; 
		m_check_y = iLoop / 5 ;
		
		m_check_x = (m_check_x * 60) + 25 + m_pC_TaxRate_spk->GetWidth(RADIO_BACK_DISABLE) ; 
		m_check_y = (m_check_y * 25) + 60 ;
		
		char temp_string[512];
		wsprintf(temp_string, (*g_pGameStringTable)[UI_STRING_MESSAGE_TAXRATE_PERSENT].GetString(), m_TaxRateArray[iLoop]);

		g_PrintColorStr(x+m_check_x, y+m_check_y ,  temp_string , gpC_base->m_item_name_pi, RGB_WHITE);

		//m_pC_button_group->Add( new C_VS_UI_EVENT_BUTTON(m_check_x, m_check_y, m_pC_TaxRate_spk->GetWidth(RADIO_BACK_DISABLE), m_pC_TaxRate_spk->GetHeight(RADIO_BACK_DISABLE), iButtonID , this, RADIO_BACK_DISABLE) );
		//iButtonID++ ; 

	}
	g_PrintColorStr(x+20, y+30 ,  (*g_pGameStringTable)[UI_STRING_MESSAGE_SELECT_TAXRATE].GetString(), gpC_base->m_item_name_pi, RGB_WHITE);
}


void VS_UI_TAXRATE::Start() //   void* pWebOjbect )
{
	PI_Processor::Start();
	m_pC_button_group->Init();
	gpC_window_manager->AppearWindow(this);
}

void	VS_UI_TAXRATE::Process()
{
	ProcessHide();
	m_pC_button_group->Process();
}

void	VS_UI_TAXRATE::UnacquireMouseFocus()
{
}

bool	VS_UI_TAXRATE::IsPixel(int _x, int _y)
{
		if(Moving()) return true;
	return IsInRect(_x, _y);
}


void	VS_UI_TAXRATE::Run(id_t id)
{

	int iData = (int)id ; 
		switch (id)
	{
		case OK_ID	:	gpC_base->SendMessage(UI_MODIFY_TAX, m_TexRate ) ;	 return ;
				
		case CANCEL_ID:	 gC_vs_ui.CloseTexRate()	;	return ;
		
		case _5PERSENT		:
		case _10PERSENT		:
		case _15PERSENT		:
		case _20PERSENT		:
		case _25PERSENT		:
		case _30PERSENT		:
		case _35PERSENT		:
		case _40PERSENT		:
		case _45PERSENT		:
		case _50PERSENT		:
			 m_TexRate = m_TaxRateArray[0] +  m_TaxRateArray[0] * iData ;
			 break ; 
			 
		case _ZEROPERSENT   :	 m_TexRate = 0 	;	break ;
		

	}

	if(id >= _5PERSENT && id <= _ZEROPERSENT)
	{

		m_check[_5PERSENT]		= CHECK_TYPE(m_TexRate == 5)  ;
		m_check[_10PERSENT]		= CHECK_TYPE(m_TexRate == 10) ;
		m_check[_15PERSENT]		= CHECK_TYPE(m_TexRate == 15) ;
		m_check[_20PERSENT]		= CHECK_TYPE(m_TexRate == 20) ;
		m_check[_25PERSENT]		= CHECK_TYPE(m_TexRate == 25) ;


		m_check[_30PERSENT]		= CHECK_TYPE(m_TexRate == 30) ;
		m_check[_35PERSENT]		= CHECK_TYPE(m_TexRate == 35) ;
		m_check[_40PERSENT]		= CHECK_TYPE(m_TexRate == 40) ;
		m_check[_45PERSENT]		= CHECK_TYPE(m_TexRate == 45) ;
		m_check[_50PERSENT]		= CHECK_TYPE(m_TexRate == 50) ;

		m_check[_ZEROPERSENT]	= CHECK_TYPE(m_TexRate == 0)  ;

	}

	
//	}
}

void	VS_UI_TAXRATE::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{

	if(p_button->GetID()== OK_ID || p_button->GetID()==CANCEL_ID)
	{
		if(p_button->GetFocusState())
		{
			if(p_button->GetPressState())
				gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x + p_button->x, y + p_button->y, p_button->m_image_index+1);
			else
				gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x + p_button->x, y + p_button->y, p_button->m_image_index+2);
		}
		else
		{
			if(p_button->GetPressState())
				gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x + p_button->x, y + p_button->y, p_button->m_image_index + 1);
			else
				gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x + p_button->x, y + p_button->y, p_button->m_image_index);
		}
	}

	else if(p_button->m_image_index == RADIO_BACK_DISABLE)
	{
		{
			
			m_pC_TaxRate_spk->BltLocked(x + p_button->x, y + p_button->y, p_button->m_image_index+1);

			int radio_plus_x = 0, radio_plus_y = 0;
			if(p_button->m_image_index == RADIO_BACK_DISABLE)
			{
				radio_plus_x = 3; radio_plus_y = 3;
			}

			if(p_button->GetPressState())
				m_pC_TaxRate_spk->BltLocked(x + p_button->x + radio_plus_x , y + p_button->y + radio_plus_y, p_button->m_image_index+2)	;
			else	if(m_check[p_button->GetID()] == CHECK_CHECK)
				m_pC_TaxRate_spk->BltLocked(x + p_button->x + radio_plus_x , y + p_button->y + radio_plus_y, p_button->m_image_index+2);
		}

	}

	
}

void	VS_UI_TAXRATE::ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button)
{
}

bool	VS_UI_TAXRATE::MouseControl(UINT message, int _x, int _y)
{
	Window::MouseControl(message, _x, _y);
	_x-=x; _y-=y;
	m_pC_button_group->MouseControl(message, _x, _y);
	return	true ; 
}

void	VS_UI_TAXRATE::KeyboardControl(UINT message, UINT key, long extra)
{
}
	
void	VS_UI_TAXRATE::Finish()
{
	PI_Processor::Finish();
	gpC_window_manager->DisappearWindow(this);
}
