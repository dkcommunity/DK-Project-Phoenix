/*-----------------------------------------------------------------------------

	VS_UI_PremiumGiveItem.h

	"ÇÁ¸®¹Ì¾ö¼­ºñ½º ¾ÆÀÌÅÛ Áö±ÞÃ¢ÀÔ´Ï´Ù."

	2008.9.19. ij-ch

-----------------------------------------------------------------------------*/
#pragma once
#include <vector>

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
typedef struct DYNAMIC_DESCITEM_SEL_INFO
{
	float m_x, m_y;
	float m_tx, m_ty;
	float m_color, m_tcolor;
	float m_speed;

	DYNAMIC_DESCITEM_SEL_INFO()
	{
		m_x = 0;
		m_y = 0;
		m_tx = m_x;
		m_ty = m_y;
		m_tcolor = 32.0f;
		m_color = m_tcolor;
		m_speed = 0.2f;
	};

	void set_x(float _x) { m_x = _x; }
	void set_y(float _y) { m_y = _y; }
	void set_tx(float _tx) { m_tx = _tx; }
	void set_ty(float _ty) { m_ty = _ty; }
	void set_color(float _color) { m_color = _color; }
	void set_tcolor(float _tcolor) { m_tcolor = _tcolor; }
	void set_speed(float _speed) { m_speed = _speed; }

	float get_x() { return m_x; }
	float get_y() { return m_y; }
	float get_tx() { return m_tx; }
	float get_ty() { return m_ty; }
	float get_color() { return m_color; }
	float get_tcolor() { return m_tcolor; }
	float get_speed() { return m_speed; }

	bool Is_end() { return ((int)m_tx==(int)m_x && (int)m_ty==(int)m_y); }

	void Process(int id);

}DYNAMIC_DESCITEM_INFO;

typedef struct DYNAMIC_DESCITEM_MAP_INFO
{
	int m_item_id;
	int m_w, m_h;
	DYNAMIC_DESCITEM_SEL_INFO **ImageMap;
	bool m_IsSet_EndingPoint;
	float m_amp_add;
	float m_speed;

	DYNAMIC_DESCITEM_MAP_INFO(int _w = 110, int _h = 110)
	{
		m_w = _w;
		m_h = _h;
		m_item_id = 0;
		m_IsSet_EndingPoint = false;
		m_amp_add = 9.0f;
		m_speed = 0.2f;
		
		ImageMap = new DYNAMIC_DESCITEM_SEL_INFO*[m_h];
	
		for(int i=0; i<m_h; i++)
		{
			ImageMap[i] = new DYNAMIC_DESCITEM_SEL_INFO[m_w];
		}
	};

	~DYNAMIC_DESCITEM_MAP_INFO()
	{
		for(int i=0; i<m_h; i++)
		{
			DeleteNewArray(ImageMap[i]);
		}
		DeleteNewArray(ImageMap);
	};

	void set_item_id(int _item_id) { m_item_id = _item_id; }
	int get_item_id() { return m_item_id; }
	int get_w() { return m_w; }
	int get_h() { return m_h; }

	bool Is_end()
	{
		bool is_end = true;
			
		for(int i=0; i<m_h; i++)
		for(int j=0; j<m_w; j++)
		{
			if(!ImageMap[i][j].Is_end())
			{
				is_end = false;
			}
		}

		return is_end;
	};

	void SetOriginal(float _x, float _y, float _color)
	{
		m_amp_add = 9.0f;

		for(int i=0; i<m_h; i++)
		for(int j=0; j<m_w; j++)
		{
			ImageMap[i][j].set_tx(_x+j);
			ImageMap[i][j].set_ty(_y+i);
			ImageMap[i][j].set_x(_x+j);
			ImageMap[i][j].set_y(_y+i);
			ImageMap[i][j].set_tcolor(_color);
			ImageMap[i][j].set_color(_color);
		}
	}

	void SetSpeed(float _speed)
	{
		for(int i=0; i<m_h; i++)
		for(int j=0; j<m_w; j++)
		{
			ImageMap[i][j].set_speed(_speed);
		}
	}

	void set_speed(float _speed) { m_speed = _speed; }
	float get_speed() { return m_speed; }
	
	void Process(int id);
	void SetStartingPoint(float _x, float _y, int id);
	void SetEndingPoint(float _x, float _y, int id);
	bool IsSet_EndingPoint() { return m_IsSet_EndingPoint; }

}DYNAMIC_DESCITEM_MAP_INFO;

//-----------------------------------------------------------------------------
// class C_VS_UI_PREMIUM_GIVE_ITEM
//-----------------------------------------------------------------------------
class C_VS_UI_PREMIUM_GIVE_ITEM : public Window, public Exec, public ButtonVisual
{
public :
	typedef std::list<std::string> STRING_LIST;

private:
	enum EXEC_ID
	{
		EID_CLOSE = 0,
		EID_ALPHA,
		EID_GET,
		EID_GET_ALL,
		EID_MAX,
		EID_LISTBOX = EID_MAX
	};

	enum MAIN_SPK
	{
		SID_MAIN_WINDOW = 0,
		SID_CLOSE,
		SID_CLOSE_HILIGHTED,
		SID_CLOSE_PUSHED,
		SID_ALPHA,
		SID_ALPHA_HILIGHTED,
		SID_ALPHA_PUSHED,
		SID_GET,
		SID_GET_HILIGHTED,
		SID_GET_PUSHED,
		SID_GET_ALL,
		SID_GET_ALL_HILIGHTED,
		SID_GET_ALL_PUSHED,
	};

	STRING_LIST							m_ItemStrList;

	CRect								m_rcItemListBOX;
	C_VS_UI_LISTBOX *					m_pItemListBox;

	static int							m_saveSelectIndexItem;

	POINT								m_MousePoint;

	C_SPRITE_PACK	*					m_pC_main_spk;
	BOOL			*					IsDescSpkList;

	CRect								m_rcShowDesc;
	CSpritePack		*					m_Give_Item_Spk;

	ButtonGroup *						m_pC_button_group;

	int									m_ui_plus_x, m_ui_plus_y;

	bool								m_bActiveDlg;
	bool								m_bDescOver;
	
	int									m_id_DynamicDesc_Process;
		
	std::vector<MPremiumGiveItemInfo *>		m_Give_Item_List;
	
	std::vector<DYNAMIC_DESCITEM_MAP_INFO *> m_Dynamic_DescItem_List;

public:
	C_VS_UI_PREMIUM_GIVE_ITEM();
	~C_VS_UI_PREMIUM_GIVE_ITEM();

	void	AcquireDisappear() {}
	void	CancelPushState() 
	{
		m_pC_button_group->CancelPushState();
	}

	void	AcquireFirstSequence();
	void	UnacquireMouseFocus();
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	Run(id_t id);
	void	CloseDialog() { Run(EID_CLOSE); }
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	Show();
	void	ShowOption(MItem *p_item, int x, int y);
	void	ShowLimit(MPremiumGiveItemInfo *p_GiveItem, int x, int y);
	void	ShowDesc(int desc_x, int desc_y, int desc_item_num, int src_x = 0, int src_y = 0, int src_w = -10000, int src_h = -10000, int src_alpha = 32);

	void	Dynamic_DescItem_Insert(int _Select_Item);
	void	Dynamic_DescItem_Process();
	bool	Dynamic_DescItem_Show();
	void	Set_DynamicDesc_Process(int _DynamicDesc_Process);

	void	Process();
	void	Start();
	void	Finish();

	void	Update_Item_List();
	bool	Is_ActiveDlg() { return m_bActiveDlg; }

	void	Give_Item_List_ClearAll();
	void	Give_Item_List_Info_Insert(MPremiumGiveItemInfo *pItem);
	int		Give_Item_List_Size();

	bool	ItemImage_Loding();
};
#endif //__PREMIUM_GIVE_ITEM_UI