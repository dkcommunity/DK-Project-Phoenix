//-----------------------------------------------------------------------------
// C_VS_UI_PET_MIXING_FORGE
// 
//-----------------------------------------------------------------------------
#pragma once
#if __CONTENTS(__PET_MIXINGFORGE)
class C_VS_UI_PET_MIXING_FORGE : public Window, public Exec, public ButtonVisual
{
private:
	C_SPRITE_PACK			*m_pC_back_spk;
	C_SPRITE_PACK			*m_pC_inventory_spk;
	ButtonGroup				*m_pC_button_group;
	
	Rect					m_my_grid_rect, m_your_grid_rect;
	int						m_focus_grid_x, m_focus_grid_y;
		
	MItem					*m_p_select_item[2];
	
	std::vector<bool>		m_v_check;
	

	enum EXEC_ID
	{
		OK_ID,
		CANCEL_ID,
	};

	enum MIXING_FORGE_SPK
	{
		MAIN_WINDOW = 0,
		BUTTON_OK,
		BUTTON_OK_HILIGHTED,
		BUTTON_OK_PUSHED,
		BUTTON_OK_INACTIVE,
		BUTTON_CANCEL,
		BUTTON_CANCEL_HILIGHTED,
		BUTTON_CANCEL_PUSHED,
		BUTTON_CANCEL_INACTIVE
	};

	enum INVENTORY_SPK_INDEX
	{
		INVENTORY_WINDOW,
		INVENTORY_WINDOW_ALPHA,
		INVENTORY_ITEMBACK,
		INVENTORY_ITEMBACK_ALPHA,
		INVENTORY_WINDOW_BOTTOM,
	};	
	
public :

	C_VS_UI_PET_MIXING_FORGE();
	~C_VS_UI_PET_MIXING_FORGE();


	int		GetFocusedItemGridX(const MItem * p_item);
	int		GetFocusedItemGridY(const MItem * p_item);	

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);
	void	AcquireDisappear() {}
	void	UnacquireMouseFocus()
	{
		m_pC_button_group->UnacquireMouseFocus();		
	}
	void	CancelPushState() 
	{
		m_pC_button_group->CancelPushState();		
	}

	void	Run(id_t id);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	
	void	Show();
	void	Process();
	void	Start();
	void	Finish();
	bool	IsPixel(int _x, int _y);	
	bool	Check(int grid_start_x, int grid_start_y);	
};
#endif //__PET_MIXINGFORGE