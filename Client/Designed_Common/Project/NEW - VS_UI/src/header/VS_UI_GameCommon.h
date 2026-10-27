/*-----------------------------------------------------------------------------

	VS_UI_GameCommon.h

	Game common interface.

	Common:
				Chatting interface
				NPC
				Exchange

	2000.8.11. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_GAME_COMMON_H__
#define __VS_UI_GAME_COMMON_H__

#include "VS_UI_Base.h"
#include "VS_UI_Util.h"
#include "VS_UI_description.h"
#include "VS_UI_ExtraDialog.h"

//#include "VS_UI_GameSlayer.h"
class C_VS_UI_SLAYER_QUICKITEM;
class C_VS_UI_OUSTERS_QUICKITEM;

extern bool gbl_mine_progress;

#include "SkillDef.h"
#include "MItem.h"
#include "MMoneyManager.h"
#include "MSkillManager.h"
#include "ClientFunction.h"
#include "MZone.h"
#include "MilitarySlot.h"

#include <vector>	//by larosel

#include "TickTimer.h"
#include "VS_UI_GPS.H"

#include "ContentsFilter.h"

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
#include "MPremiumGiveItemInfo.h"
#include "VS_UI_PremiumGiveItem.h"
#endif //__PREMIUM_GIVE_ITEM_UI

#if __CONTENTS(__TITLE_UI_RENWEAL)
	#include "VS_UI_NewTitle.h"
#else	//__TITLE_UI_RENWEAL
	#include "VS_UI_Title.h"
#endif //__TITLE_UI_RENWEAL

#define SLAYER_FONT_COLOR				RGB(186, 189, 191)//RGB(20, 70, 0)	// by larosel
#define VAMPIRE_FONT_COLOR				RGB(186, 189, 191)//RGB(255, 255, 255)

#define ITEM_DISABLE_COLOR_SET			40

// For Window Manager
struct WindowStatus
{
	WindowStatus()
		: autoHide(Window::ATTRIBUTES_HIDE_NOT)
		, alpha(false) {}

	Rect windowRect;
	Window::ATTRIBUTES_HIDE autoHide;
	bool alpha;
};

//-----------------------------------------------------------------------------
// class C_VS_UI_LEVELUP
//
// vampire/slayer level up�� ��Ÿ���� button.
//-----------------------------------------------------------------------------
class C_VS_UI_LEVELUP : public Window
{
private:
	enum IMAGE_SPK_INDEX
	{
		LVU_GUARD_SLAYER,
		LVU_GUARD_VAMPIRE,
		LVU_GUARD_OUSTERS,
		PARTY_REQUEST,
		PARTY_INVITE,
		RESURRECT_SLAYER,
		RESURRECT_VAMPIRE,
		RESURRECT_OUSTERS,
		LVU_BUTTON,
		LVU_BUTTON_OUSTERS,
	};

	C_SPRITE_PACK				m_image_spk;

	bool							m_bl_pushed;
	bool							m_bl_focused;

public:
	C_VS_UI_LEVELUP();
	~C_VS_UI_LEVELUP();

	void	Show();
	bool	IsPixel(int _x, int _y);
	void	Start();
	void	Finish();
	bool	MouseControl(UINT message, int _x, int _y);
	void	CancelPushState() { m_bl_pushed = false; }
	void	UnacquireMouseFocus() { m_bl_focused = false; }
};

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
//-----------------------------------------------------------------------------
// class C_VS_UI_REQUEST_PREMIUM_GIVE_ITEM
//
// ���޹��� ���� �����̾� �������� ������ ��� ��Ÿ���� button.
//-----------------------------------------------------------------------------
class C_VS_UI_REQUEST_PREMIUM_GIVE_ITEM : public Window
{
private:
	C_VS_UI_PREMIUM_GIVE_ITEM	*m_pC_Premium_Give_Item;

	enum IMAGE_SPK_INDEX
	{
		REQUEST_GUARD_SLAYER,		// - 00 �����̾� ����
		REQUEST_GUARD_VAMPIRE,		// - 01 �����̾� ����
		REQUEST_GUARD_OUSTERS,		// - 02 �ƿ콺���� ����
		PARTY_REQUEST,				// - 03	��Ƽ���Խ�û
		PARTY_INVITE,				// - 04	��Ƽ�ʴ�
		RESURRECT_SLAYER,			// - 05	�����̾� ��Ȱ		- �׾�����.
		RESURRECT_VAMPIRE,			// - 06	�����̾� ��Ȱ		- �׾�����.
		RESURRECT_OUSTERS,			// - 07	�ƿ콺���� ��Ȱ		- �׾�����.
		LVU_BUTTON,					// - 08	�����̾� ������
		LVU_BUTTON_OUSTERS,			// - 09	�ƿ콺���� ������
		RESURRECT_SCROLL,			// - 10	��Ȱ��ũ��			- �׾�����.
		ELIXIR_SCROLL,				// - 11	������				- �׾�����.
		ETERNITY,					// - 12	�̽���Ƽ			- �׾�����. (�����̾���ΰ�?)
		SKILL_FROM_OTHER_SLAYER,	// - 13 �����̾� �츮�� ��ų
		SKILL_FROM_OTHER_OUSTERS,	// - 14 �ƿ콺���� �츮�� ��ų
		SKILL_FROM_OTHER_VAMPIRE,	// - 15 �����̾� �츮�� ��ų
		GIVE_ITEM01,				// - 16 �����̾� ���޾����� ������ 01 (���� UI�� �� �̹���)
		GIVE_ITEM02,				// - 17 �����̾� ���޾����� ������ 02 (���� UI�� �� �̹���)
		GIVE_ITEM_CASE01,			// - 18 �����̾� ���޾����� ������ ������ 01
		GIVE_ITEM_CASE02,			// - 19 �����̾� ���޾����� ������ ������ 02
	};
	
	C_SPRITE_PACK				m_image_spk;
	
	bool						m_bl_pushed;
	bool						m_bl_focused;

public:
	C_VS_UI_REQUEST_PREMIUM_GIVE_ITEM();
	~C_VS_UI_REQUEST_PREMIUM_GIVE_ITEM();

	void	Show();
	void	Process();
	bool	IsPixel(int _x, int _y);
	void	Start();
	void	Finish();
	bool	MouseControl(UINT message, int _x, int _y);
	void	RunPremiumGiveItemDialog();
	bool	ClosePremiumGiveItemDialog();
	void	Set_DynamicDesc_Process(int id) { if(m_pC_Premium_Give_Item!=NULL) m_pC_Premium_Give_Item->Set_DynamicDesc_Process(id); }
	void	Update_Item_List();

	void	CancelPushState() { m_bl_pushed = false; }
	void	UnacquireMouseFocus() { m_bl_focused = false; }
};
#endif //__PREMIUM_GIVE_ITEM_UI

//-----------------------------------------------------------------------------
// class C_VS_UI_REQUEST_PARTY
//
//-----------------------------------------------------------------------------
class C_VS_UI_REQUEST_PARTY : public Window
{
private:
	enum IMAGE_SPK_INDEX
	{
		REQUEST_GUARD_SLAYER,
		REQUEST_GUARD_VAMPIRE,
		REQUEST_GUARD_OUSTERS,
		PARTY_REQUEST,
		PARTY_INVITE,
		RESURRECT_SLAYER,
		RESURRECT_VAMPIRE,
		RESURRECT_OUSTERS,
		LVU_BUTTON,
		LVU_BUTTON_OUSTERS,
		RESURRECT_SCROLL,
		ELIXIR_SCROLL,
	};

	C_SPRITE_PACK				m_image_spk;

	bool							m_bl_pushed;
	bool							m_bl_focused;
	std::string					m_name;

// TIMER
	DWORD						m_dw_prev_tickcount;
	DWORD						m_dw_timer_tickcount;

	bool	Timer(bool reset = false);

public:
	enum REQUEST_PARTY
	{
		REQUEST,
		INVITE,
	};

private:
	REQUEST_PARTY				m_type;

public:
	C_VS_UI_REQUEST_PARTY(const char *name, DWORD timer);
	~C_VS_UI_REQUEST_PARTY();

	void	Show();
	bool	IsPixel(int _x, int _y);
	void	Start();
	void	Finish();
	bool	MouseControl(UINT message, int _x, int _y);
	void	CancelPushState() { m_bl_pushed = false; }
	void	UnacquireMouseFocus() { m_bl_focused = false; }
};

//-----------------------------------------------------------------------------
// class C_VS_UI_REQUEST_DIE
//
//-----------------------------------------------------------------------------
class C_VS_UI_REQUEST_DIE : public Window
{
private:
	enum IMAGE_SPK_INDEX
	{
		RESURRECT_GUARD_SLAYER,
		RESURRECT_GUARD_VAMPIRE,
		RESURRECT_GUARD_OUSTERS,
		PARTY_REQUEST,
		PARTY_INVITE,
		RESURRECT_SLAYER,
		RESURRECT_VAMPIRE,
		RESURRECT_OUSTERS,
		LVU_BUTTON,
		LVU_BUTTON_OUSTERS,
		RESURRECT_SCROLL,
		ELIXIR_SCROLL,
	};

	C_SPRITE_PACK				m_image_spk;

	bool						m_bl_pushed;
	bool						m_bl_focused;
	
	bool						m_bl_enforce_resurrect;

// TIMER
	DWORD						m_dw_prev_tickcount;
	DWORD						m_dw_timer_tickcount;

// levelwar timer
	DWORD						m_levelwar_prev_tickcount;
	DWORD						m_levelwar_timer_tickcount;


public:
	C_VS_UI_REQUEST_DIE(DWORD timer, bool enforceResurrect = false, DWORD level_timer = 0);
	~C_VS_UI_REQUEST_DIE();

	bool	Timer(bool reset = false);
	bool	levelWarTimer(bool reset = false);

	void	Show();
	bool	IsPixel(int _x, int _y);
	void	Start();
	void	Finish();
	bool	MouseControl(UINT message, int _x, int _y);
	void	CancelPushState() { m_bl_pushed = false; }
	void	UnacquireMouseFocus() { m_bl_focused = false; }
};

//-----------------------------------------------------------------------------
// class C_VS_UI_PARTY_MANAGER
//
//-----------------------------------------------------------------------------
class C_VS_UI_PARTY_MANAGER : public Window, public Exec, public ButtonVisual
{
private:
	enum
	{
		window_default_height = 18, 
		window_gap = 53, 
		window_bottom_gap = 7,
	//#if __CONTENTS(__080405_FIREST_UI_UPDATE)
		away_x = 82,
		away_y = 4,
	//#else
	//	away_x = 95,
	//	away_y = 8,
	//#endif //__080405_FIREST_UI_UPDATE
	};

	//enum IMAGE_SPK_INDEX
	//{
	//	PARTY_WINDOW,
	//	PARTY_WINDOW_ALPHA,
	//	PARTY_WINDOW_BOTTOM,
	//	DENY_BUTTON,
	//	DENY_BUTTON_PUSHED,
	//	DENY_BUTTON_HILIGHTED,
	//	DENY_BUTTON_HILIGHTED_PUSHED,
	//	HELP_BUTTON,
	//	HELP_BUTTON_PUSHED,
	//	HELP_BUTTON_HILIGHTED,
	//	HELP_BUTTON_PUSHED_HILIGHTED,
	//	CLOSE_BUTTON,
	//	CLOSE_BUTTON_PUSHED,
	//	CLOSE_BUTTON_HILIGHTED,
	//	CLOSE_BUTTON_PUSHED_HILIGHTED,
	//	AWAY_BUTTON,
	//	AWAY_BUTTON_HILIGHTED,
	//	AWAY_BUTTON_HILIGHTED_PUSHED,
	//	FACE_GRID,
	//	NAME_GRID,
	//	HPBAR,
	//};

	enum EXEC_ID
	{
		ALPHA_ID,
		DENY_ID,
		HELP_ID,
		CLOSE_ID,

		AWAY_I,
		AWAY_0,
		AWAY_1,
		AWAY_2,
		AWAY_3,
		AWAY_4,
	};

	enum IMAGE_SPK_INDEX
	{
	  PARTY_WINDOW,
	  PARTY_TouXiang_kuang,
	  PARTY_HP,
	  PARTY_MP,
	  PARTY_EP,
	  PARTY_EP2,
	  PARTY_BLOCK,
	  PARTY_REN_NAN,
	  PARTY_REN_NV,
	  PARTY_GUI_NAN,
	  PARTY_GUI_NV,
	  PARTY_MO,
	  PARTY_BUTTON_1,
	  PARTY_BUTTON_2,
	  PARTY_BUTTON_3,
	  PARTY_BUTTON_2_1,
	  PARTY_BUTTON_2_2,
	  PARTY_BUTTON_2_3,
	  PARTY_BUTTON_3_1,
	  PARTY_BUTTON_3_2,
	  PARTY_BUTTON_3_3,
	  PARTY_LEFT_BIG,
	  PARTY_LEFT_SMALL,
	  PARTY_RIGHT_BIG,
	  PARTY_RIGHT_SMALL,
	  PARTY_CLOSE_1,
	  PARTY_CLOSE_2,
	  PARTY_CLOSE_3,
	  PARTY_HIDE_1,
	  PARTY_HIDE_2,
	  PARTY_XuanXiang,
	};

	//C_SPRITE_PACK				* m_p_face_spk;
	//C_SPRITE_PACK				* m_p_large_face_spk;
	C_SPRITE_PACK				* m_p_image_spk;
	ButtonGroup *				m_pC_bottom_button_group;

#if __CONTENTS(!__080405_FIREST_UI_UPDATE)
	ButtonGroup *				m_pC_button_group;	
#endif //__080405_FIREST_UI_UPDATE

	bool	m_away_button_focused;
	bool	m_away_pushed;
	int		m_old_away_focused;
	int		m_away_focused;

public:
	C_VS_UI_PARTY_MANAGER();
	~C_VS_UI_PARTY_MANAGER();

	void	RefreshFaceImage();

	int		GetPartyManagerFocused()	{ return m_away_focused; }
#if __CONTENTS(__080405_FIREST_UI_UPDATE)
	void	UnacquireMouseFocus() { m_pC_bottom_button_group->UnacquireMouseFocus(); m_old_away_focused = -1; m_away_focused= -1;}
	void	CancelPushState() { m_pC_bottom_button_group->CancelPushState(); }
#else
	void	UnacquireMouseFocus() { m_pC_bottom_button_group->UnacquireMouseFocus(); m_pC_button_group->UnacquireMouseFocus(); m_old_away_focused = -1; m_away_focused= -1;}
	void	CancelPushState() { m_pC_button_group->CancelPushState(); m_pC_bottom_button_group->CancelPushState(); }
#endif //__080405_FIREST_UI_UPDATE
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);
	void	AcquireDisappear() {}
	void	Run(id_t id);
	void	Process();

	bool	Timer(bool reset = false);

	void	Show();
	bool	IsPixel(int _x, int _y);
	void	Start();
	void	Finish();
	bool	MouseControl(UINT message, int _x, int _y);
};

//-----------------------------------------------------------------------------
// HotKey (abstract base class)
//
// 
//-----------------------------------------------------------------------------
class HotKey
{
public:

	virtual void HotKey_PetInfo() = 0;		// ctrl+CapsLock
	virtual void HotKey_MailBox() = 0;		// ctrl+b
	virtual void HotKey_Party() = 0;		// ctrl+p
	virtual void HotKey_Mark() = 0;			// '����'Ű
//	virtual void HotKey_WindowToggle()=0;	// ctrl + space
	virtual void HotKey_Inventory(bool IsCheckSubInventory = false)=0;		// tab
	virtual void HotKey_Gear()=0;			// ctrl + tab
#if __CONTENTS(__GEAR_SWAP_CHANGE)		//	C_VS_UI_GAME	����â ��üŰ�� ��� �� ���� �Լ� ����
	virtual void HotKey_GearChange()=0;		// Alt + A
	virtual void HotKey_GearChangeShow()=0;	// Alt + S
#endif	// __GEAR_SWAP_CHANGE
	virtual void HotKey_QuickItemSlot() {}	// ctrl + ~
	virtual void HotKey_Skill()=0;			// ctrl + s
	virtual void CloseSkillChooseWindow() =0;
	virtual void HotKey_CharInfo()=0;		// ctrl + i
	virtual void HotKey_SkillInfo()=0;		// ctrl + k
	virtual void HotKey_Grade3Info()=0;		// ctrl + a
	virtual void HotKey_Grade2Info()=0;		// ctrl + r
	virtual void HotKey_Grade1Info()=0;		// ctrl + c
	virtual void HotKey_Help()=0;			// ctrl + h
	virtual void HotKey_WorldMap()=0;		// ctrl + m
	virtual void HotKey_Minimap()=0;		// ����
	virtual void HotKey_Chat_Up()=0;		// ctrl + PgUp
	virtual void HotKey_Chat_Dn()=0;		// ctrl + PgDn
	virtual void HotKey_Tribe_Prior()=0;	// alt	+ PgUp
	virtual void HotKey_Tribe_Next()=0;		// alt	+ PgDn
	virtual void HotKey_Guild_Info()=0;		// ctrl	+ v
	virtual void HotKey_Guild_Member_List()=0;// ctrl	+ l
	virtual void HotKey_Coer_Attack()=0;	// ctrl	+ o
// by svi	
#if __CONTENTS(__AUTO_ATTACT)	//__AUTO_ATTACT	
	virtual void HotKey_Auto_Attack()=0;	// ctrl	+ j
#endif  //__AUTO_ATTACT
// end

    virtual void HotKey_ZoneChat()=0;		// ctrl + z
	virtual void HotKey_GuildChat()=0;		// ctrl + g
	virtual void HotKey_UnionChat()=0;		// ctrl + d
	virtual void HotKey_Whisper()=0;		// ctrl + w
	virtual void HotKey_ExtendChat()=0;		// ctrl + e
	virtual void HotKey_Sysrq()=0;			// printscreen sysrq
	virtual void HotKey_Slash()=0;			// ctrl + /
	virtual void HotKey_Background_Music()=0;	// ä�ø��ɾ� /�����
	virtual void HotKey_Effect_Music()=0;		// ä�ø��ɾ� /ȿ����
	virtual void HotKey_Chat_Help()=0;			// ä�ø��ɾ� /ä�õ���
	virtual void HotKey_Where()=0;				// ä�ø��ɾ� /���

	virtual void HotKey_ESC() {}			// esc

	virtual void HotKey_F1() {}				// f1
	virtual void HotKey_F2() {}				// f2
	virtual void HotKey_F3() {}				// f3
	virtual void HotKey_F4() {}				// f4
	virtual void HotKey_F5() {}				// f5
	virtual void HotKey_F6() {}				// f6
	virtual void HotKey_F7() {}				// f7
	virtual void HotKey_F8() {}				// f8
	virtual void HotKey_F9() {}				// f9
	virtual void HotKey_F10(){}				// f10
	virtual void HotKey_F11(){}				// f11
	virtual void HotKey_F12(){}				// f12
	virtual void Hotkey_Function(int FKey)	{} // HotKey ó�� �Լ� 

//	virtual void HotKey_F9()=0;				// f9
//	virtual void HotKey_F10()=0;			// f10
//	virtual void HotKey_F11()=0;			// f11
//	virtual void HotKey_F12()=0;			// f12
	virtual void HotKey_NormalChat() = 0;
	virtual void HotKey_PartyChat() = 0;	// ctrl + a
	virtual void HotKey_Quest() = 0;		// ctrl + q

	// 2004, 5, 17 sobeit add start
	virtual void HotKey_SummonPet()=0;	// ctrl + space
	// 2004, 5, 17 sobeit add end
#if __CONTENTS(__ITEM_SHOW)
	virtual void CloseInventory()=0;
#endif //__ITEM_SHOW
};

enum CHAT_LINE_CONDITION
{
	CLD_NORMAL,				//0
	CLD_ZONECHAT,			//1
	CLD_WHISPER,			//2
	CLD_PARTY,				//3
	CLD_GUILD,				//4
	CLD_UNION,				//5
	CLD_INFO,				//6 ���� �޽���.
	CLD_GRADEDISPLAY,		//7 ����� ��� �޼���
	CLD_MASTER,				//8 ��� �޽���

	CLD_TOTAL = CLD_MASTER,	//8
};

//-----------------------------------------------------------------------------
// class StringCell
//
// Single string class.
//-----------------------------------------------------------------------------
class StringCell
{
private:
	char *						m_sz_string;

public:
	void	Release()
	{
		DeleteNewArray(m_sz_string);
	}

	StringCell()
	{
		m_sz_string = NULL;
	}

	virtual ~StringCell()
	{
		Release();
	}

	void	SetString(const char * sz_str)
	{
		if (sz_str == NULL)
			return;

		Release();
		m_sz_string = new char[strlen(sz_str)+1];
		strcpy(m_sz_string, sz_str);
	}

	const char * GetString() const { return m_sz_string; }
};

extern COLORREF g_color[4][4];

//-----------------------------------------------------------------------------
// class C_VS_UI_CHAT_LINE
//
//
//-----------------------------------------------------------------------------
class C_VS_UI_CHAT_LINE
{
private:
	StringCell					m_id_cell;
	StringCell					m_msg_cell;
	CHAT_LINE_CONDITION			m_condition;
	DWORD						m_Color;
	mutable bool				m_render_cache_valid;
	mutable int					m_render_cache_width;
	mutable int					m_render_cache_id_gap;
	mutable HFONT				m_render_cache_id_font;
	mutable HFONT				m_render_cache_chat_font;
	mutable const char *		m_render_cache_divisor;
	mutable int					m_render_cache_id_width;
	mutable int					m_render_cache_divisor_width;
	mutable int					m_render_cache_message_length;
	mutable std::vector<int>	m_render_cache_cut_lengths;
		
public:
	C_VS_UI_CHAT_LINE()
	{
		m_condition = CLD_NORMAL;
		m_Color = g_color[0][0];
		InvalidateRenderCache();
	}

	virtual ~C_VS_UI_CHAT_LINE() {}

	void	Set(const char * sz_id, const char * sz_msg, CHAT_LINE_CONDITION condition, DWORD color)
	{
		m_id_cell.SetString(sz_id);
		m_msg_cell.SetString(sz_msg);
		m_condition = condition;
		m_Color = color;
		InvalidateRenderCache();
	}

	const char * GetIdString() const { return m_id_cell.GetString(); }
	const char * GetMsgString() const { return m_msg_cell.GetString(); }
	CHAT_LINE_CONDITION	GetCondition() const { return m_condition; }
	DWORD	GetColor() const { return m_Color; }
	void	InvalidateRenderCache() const;
	void	PrepareRenderCache(int chatWindowWidth, int idGap, const char * divisor, HFONT idFont, HFONT chatFont) const;
	int		GetCachedIdWidth() const { return m_render_cache_id_width; }
	int		GetCachedDivisorWidth() const { return m_render_cache_divisor_width; }
	int		GetCachedMessageLength() const { return m_render_cache_message_length; }
	const std::vector<int> & GetCachedCutLengths() const { return m_render_cache_cut_lengths; }
};

//-----------------------------------------------------------------------------
// class C_VS_CHAT_LIST
//
//
//-----------------------------------------------------------------------------
class C_VS_CHAT_LIST
{
private:
	SimpleDataList<C_VS_UI_CHAT_LINE *>		m_list;

public:
	C_VS_CHAT_LIST() {}
	~C_VS_CHAT_LIST()
	{
		C_VS_UI_CHAT_LINE * data;
		for (int i=0; i < m_list.Size(); i++)
			if (m_list.Data(i, data))
				delete data;
	}

	void	Add(const char * sz_id, const char * sz_msg, CHAT_LINE_CONDITION condition, DWORD color)
	{
		C_VS_UI_CHAT_LINE * p_line = new C_VS_UI_CHAT_LINE;
		p_line->Set(sz_id, sz_msg, condition, color);

		m_list.Add(p_line);
	}

	int	Size() const { return m_list.Size(); }

	C_VS_UI_CHAT_LINE *	PopBack()
	{
		// !data�� delete������ �ʴ´�.

		C_VS_UI_CHAT_LINE * data;

		if (m_list.Data(0, data))
		{
			m_list.Delete(data);
			return data;
		}

		return NULL;
	}

	C_VS_UI_CHAT_LINE *	GetLine(int n) const
	{
		// n�� 0�� �������� �ֱٿ� �Էµ� line�̴�.

		if (n < 0)
			return NULL;

		int index = m_list.Size()-n-1;

		C_VS_UI_CHAT_LINE * data;

		if (m_list.Data(index, data))
			return data;

		return NULL;
	}
};

class PAPERING_HISTORY
{
	public:
	std::string			m_string;
	std::vector<DWORD>	m_timer;
};

//-----------------------------------------------------------------------------
// C_VS_UI_CHATTING
//
// �Ϲ����� chatting�� �ӼӸ�(slayer = PCS)�� �����Ѵ�.
//-----------------------------------------------------------------------------
class C_VS_UI_CHATTING : public Window, public Exec, public ButtonVisual
{
public:
	enum EXEC_ID
	{
		ALPHA_ID,
		PUSHPIN_ID,
		MARK_ID,
		COLOR_ID,
		LANGUAGE_ID,
		SPREAD_ID,

		// CHAT
		CHAT_NORMAL_ID,
		CHAT_ZONE_ID,
		CHAT_WHISPER_ID,
		CHAT_PARTY_ID,
		CHAT_GUILD_ID,
		CHAT_UNION_ID,

		// FILTER
		FILTER_NORMAL_ID,
		FILTER_ZONE_ID,
		FILTER_WHISPER_ID,
		FILTER_PARTY_ID,
		FILTER_GUILD_ID,
		FILTER_UNION_ID,

		SHOW_SET,
		PAGE_UP,
		PAGE_DOWN,
		PAGE_UNTILL,
		PAGE_OPEN,
		SETTING_OPEN,

	};

	
private:
	//enum MAIN_SPK_INDEX
	//{
	//	MAIN,
	//	MAIN_ALPHA,
	//	MAIN_RIGHT,
	//	MAIN_BOTTOM,
	//	INPUT,
	//	INPUT_ALPHA,
	//	INPUT_RIGHT,
	//	BUTTON_SPREAD,
	//	BUTTON_SPREAD_HILIGHTED,
	//	BUTTON_SPREAD_HILIGHTED_PUSHED,
	//	BUTTON_FILTER,
	//	BUTTON_FILTER_HILIGHTED,
	//	BUTTON_FILTER_SELECTED,
	//	BUTTON_MARK,
	//	BUTTON_MARK_HILIGHTED,
	//	BUTTON_MARK_HILIGHTED_PUSHED,
	//	BUTTON_COLOR,
	//	BUTTON_COLOR_HILIGHTED,
	//	BUTTON_COLOR_HILIGHTED_PUSHED,
	//	BUTTON_HAN,
	//	BUTTON_HAN_HILIGHTED,
	//	BUTTON_HAN_HILIGHTED_PUSHED,
	//	BUTTON_ENG,
	//	BUTTON_ENG_HILIGHTED,
	//	BUTTON_ENG_HILIGHTED_PUSHED,
	//	BUTTON_NORMAL,
	//	BUTTON_NORMAL_HILIGHTED,
	//	BUTTON_NORMAL_HILIGHTED_PUSHED,
	//	BUTTON_NORMAL_SELECTED,
	//	BUTTON_NORMAL_SELECTED_PUSHED,
	//	BUTTON_WHISPER,
	//	BUTTON_WHISPER_HILIGHTED,
	//	BUTTON_WHISPER_HILIGHTED_PUSHED,
	//	BUTTON_WHISPER_SELECTED,
	//	BUTTON_WHISPER_SELECTED_PUSHED,
	//	BUTTON_ZONE,
	//	BUTTON_ZONE_HILIGHTED,
	//	BUTTON_ZONE_HILIGHTED_PUSHED,
	//	BUTTON_ZONE_SELECTED,
	//	BUTTON_ZONE_SELECTED_PUSHED,
	//	BUTTON_PARTY,
	//	BUTTON_PARTY_HILIGHTED,
	//	BUTTON_PARTY_HILIGHTED_PUSHED,
	//	BUTTON_PARTY_SELECTED,
	//	BUTTON_PARTY_SELECTED_PUSHED,
	//	BUTTON_GUILD,
	//	BUTTON_GUILD_HILIGHTED,
	//	BUTTON_GUILD_HILIGHTED_PUSHED,
	//	BUTTON_GUILD_SELECTED,
	//	BUTTON_GUILD_SELECTED_PUSHED,
	//	BUTTON_UNION,
	//	BUTTON_UNION_HILIGHTED,
	//	BUTTON_UNION_HILIGHTED_PUSHED,
	//	BUTTON_UNION_SELECTED,
	//	BUTTON_UNION_SELECTED_PUSHED,
	//	BUTTON_CHANNEL,
	//};

	enum MAIN_SPK_INDEX
	{
		BUTTON_FILTER,//0
		BUTTON_SPREAD,//1
		BUTTON_NORMAL,//2
		BUTTON_ZONE,//3
		BUTTON_WHISPER,//4
		BUTTON_PARTY,//5
		BUTTON_GUILD,//6
		BUTTON_UNION,//7
		BUTTON_Money,//8
		MAIN,//9
		INPUT_ALPHA,//10 
		MAIN_BOTTOM,//11
		MAIN_RIGHT,//12
		MAIN_RIGHT_ALPHA,//13
		SMALL_MAIN_ALPHA_1,//14
		SMALL_MAIN_ALPHA_2,//15
		SMALL_MAIN_ALPHA_3,//16
		SMALL_MAIN_ALPHA_4,//17
		INPUT_LEFT,//18
		INPUT_MAIN,//19
		INPUT_RIGHT,//20
		MAIN_BUTTON_MARK,//21
		BUTTON_COLOR,//22
		BUTTON_COLOR_SELECT,//23
		BUTTON_UP,//24
		BUTTON_UP_HILIGHTED_PUSHED,//25
		BUTTON_DOWN,//26
		BUTTON_DOWN_HILIGHTED_PUSHED,//27
		BUTTON_DOWN_UT,//28
		BUTTON_DOWN_UT_HILIGHTED_PUSHED,//29
		BUTTON_SHOW_SET,//30
		BUTTON_SHOW_SET_HILIGHTED_PUSHED,//31
		BUTTON_OPEN_TYPE,//32
		BUTTON_OPEN_TYPE_HILIGHTED_PUSHED,//33
		BUTTON_SETTING,//34
		BUTTON_SETTING_HILIGHTED_PUSHED,//35

		BUTTON_OPEN_CHAT,//36
		BUTTON_CLOSE_CHAT,//37
		BUTTON_WIS_OPEN_NEW,//38
		BUTTON_WIS_OPEN_NEW_HILIGHTED,//39
		BUTTON_NORMAL_CHAT,//40
		BUTTON_NORMAL_CHAT_HILIGHTED,//41
		BUTTON_ZONE_CHAT,//42
		BUTTON_ZONE_CHAT_HILIGHTED,//43
		BUTTON_WHISPER_CHAT,//44
		BUTTON_WHISPER_CHAT_HILIGHTED,//45
		BUTTON_PARYT_CHAT,//46
		BUTTON_PARYT_CHAT_HILIGHTED,//47
		BUTTON_GUILD_CHAT,//48
		BUTTON_GUILD_CHAT_HILIGHTED,//49
		BUTTON_UNION_CHAT,//50
		BUTTON_UNION_CHAT_HILIGHTED,//51
		BUTTON_MONEY_CHAT,//52
		BUTTON_MONEY_CHAT_HILIGHTED,//53
		BUTTON_BIAOQIONG,//54
		BUTTON_COLOR_NEW,//55
		BUTTON_TYPE_CHANGE,//56
	};
	enum
	{
		CHAT_INPUT_LIMIT = 100,
	};

	enum HISTORY
	{
		HISTORY_SIZE = 200,
		HISTORY_SIZE_TEMP = 250,
	};

	enum
	{
		MARK_X = 12,
		MARK_Y = 17,
		COLOR_X = 20,
		COLOR_Y = 17,
	};

	enum RESIZE_STATE
	{
		RESIZE_NOT,
		RESIZE_LEFT,
		RESIZE_TOP,
		RESIZE_RIGHT,
		RESIZE_BOTTOM,
		RESIZE_LEFTTOP,
		RESIZE_RIGHTTOP,
		RESIZE_LEFTBOTTOM,
		RESIZE_RIGHTBOTTOM,
	};

private:
	int						CHAT_LINE_START_X, CHAT_LINE_START_Y;
	int						CHAT_HISTORY_START_Y;
	int						CHAT_WINDOW_WIDTH, CHAT_INPUT_WIDTH;

	bool					m_bl_input_mode;
	bool					m_show_chat_setting;
	bool					m_chat_filter[CLD_TOTAL];
	CHAT_LINE_CONDITION		m_chat_mode;
	CHAT_LINE_CONDITION		m_befor_chat_mode;

	Rect					m_backup_rect;

	int						m_sub_window;
	Rect					m_sub_rect;
	POINT					m_sub_selected;

	RESIZE_STATE			m_resize;
	POINT					m_backup_window_point;

	C_VS_UI_SCROLL_BAR		*m_pC_scroll_bar, *m_pC_sub_scroll_bar;
	C_SPRITE_PACK			* m_pC_chatting_spk;
	CSpriteSurface			* m_pC_chatting_surface;
	HDC						m_hChatHistoryCacheDC;
	HBITMAP					m_hChatHistoryCacheBitmap;
	HBITMAP					m_hChatHistoryCacheOldBitmap;
	WORD					* m_pChatHistoryCacheBits;
	int						m_chatHistoryCacheWidth;
	int						m_chatHistoryCacheHeight;
	int						m_chatHistoryCachePitchWords;
	int						m_chatHistoryCacheLeft;
	int						m_chatHistoryCacheTop;
	int						m_chatHistoryCacheScrollPos;
	int						m_chatHistoryCacheHistorySize;
	int						m_chatHistoryCacheFilterMask;
	HFONT					m_chatHistoryCacheUserFont;
	HFONT					m_chatHistoryCacheChatFont;
	bool					m_chatHistoryCacheValid;
	bool					m_chatHistoryCacheDirty;
	
	ButtonGroup *			m_pC_button_group;
	ButtonGroup *			m_pC_input_button_group;
	ButtonGroup *			m_pC_input_right_button_group;

	ButtonGroup *			m_pC_Choose_button_group;

	int							m_history_line;
	std::vector<PAPERING_HISTORY>	m_history;
	
//	int							m_string_output_x; // �ּ�ó�� by larosel
	
	void	ScrollHistory();
	void	InvalidateChatHistoryCache();
	bool	EnsureChatHistoryCacheSurface(int width, int height);
	void	ReleaseChatHistoryCache();
	bool	BuildChatHistoryCache();
	bool	BltChatHistoryCache();
	
	std::vector<std::string>	m_v_help_string;
	std::vector<bool>			m_v_help_check;
	
	//
	// Chat history buffer.
	//
	C_VS_CHAT_LIST				m_pC_history_list;
	C_VS_CHAT_LIST				m_pC_history_temp_list;

	int scroll_up_x, scroll_up_y, scroll_down_x, scroll_down_y, scrolltag_x, scrolltag_y, scrolltrack_x, scrolltrack_y;	// by larosel

	bool							m_chat_stacked[CLD_TOTAL];

	LineEditorVisual				m_lev_chatting;

	COLORREF						m_color_tab[CLD_TOTAL+1];

	bool							m_bl_spreadID;

	std::vector<std::string>		m_sz_whisper_id;	// by larosel
	std::string						m_sz_whisper_backup;	// by larosel
	bool							m_bl_whisper_mode;	// by larosel
	bool							m_bl_focus_whisper;	// by larosel
	int								m_whisper_index;	// by larosel

	//
	// Vampire/Slayer �� �� ��Ÿ�� �� �ִ� Window�� ���� 2��(Inventory, Gear)�̴�.
	// ȭ�� �� ���� ��Ÿ���� ��� click focus�� �ֱ� ������ �װ��� ��������� �Ѵ�.
	//

//	int								m_slayer_chat_mode;	// �ּ�ó�� by larosel
//	void	AddChatToHistoryTemp(char * str, char * sz_id, CHAT_LINE_CONDITION condition);	// �ּ�ó�� by laorsel

	enum
	{
		TIMER_NONE,
		TIMER_ZONECHAT,
		TIMER_REP,
		TIMER_PAPERING,
		TIMER_HIDE,
	};

// TIMER
	std::vector<DWORD>			m_dw_rep_tickcount;
	std::vector<DWORD>			m_dw_papering_tickcount;
	DWORD						m_dw_prev_tickcount;
	DWORD						m_dw_zonechat_timer;
	DWORD						m_dw_rep_timer;
	DWORD						m_dw_papering_timer;
	DWORD						m_dw_help_timer;
	DWORD						m_dw_help_prev_tickcount;
	DWORD						m_dw_hide_timer;
	DWORD						m_dw_hide_prev_tickcount;
	int							m_timer;

	bool	Timer(bool reset = false);
	bool	TimerHelp();
	bool	TimerHide(bool reset = false);

public:
	void	Extend();

	void	SetSubWindow(int type)		{ m_sub_window = type; }
	//-------------------------
	// WHISPER	// by larosel
	//-------------------------
	std::string	GetWhisperID(int num = -1);										// by larosel
	int		GetWhisperSize()				{ return m_sz_whisper_id.size();}	// by larosel
	bool	AddWhisperID(const char *sz_ID);	// by larosel
	void	ChangeWhisperFocus();												// by larosel
	bool	GetWhisperFocus()				{ return m_bl_focus_whisper; }		// by larosel
	bool	SlayerWhisperMode(bool mode);										// by larosel
	void	TribeChanged();
	bool	IsSpreadID()					{ return m_bl_spreadID; }
	bool	IsPixel(int _x, int _y);
	void	SetWhisperID(char *id);
	bool	IsInputMode()					{ return m_bl_input_mode; }
	int		GetChatMode()					{ return m_chat_mode;}				// 2007 01 16 by diesirace
	int		SetChatMode(int chat_mode);											// 2007 01 17 by diesirace
	LineEditorVisual	GetChatLev()		{ return m_lev_chatting; }			// 2007 01 23 by diesirace
	//---------------------------
	// GUILDCHAT // by larosel
	//---------------------------
	void	SetGuildChat( bool guild );

	void	ResetScroll();	// by larosel

	C_VS_UI_CHATTING();
	~C_VS_UI_CHATTING();

	void	Acquire() { m_lev_chatting.Acquire(); }
	bool	IsEmpty()	{ return (m_lev_chatting.Size() <= 0); }
	void	Start();
	void	Finish();

	void	RestoreHistoryTemp();
	void	Show();
	bool	MouseControl(UINT message, int _x, int _y);
//	bool	MouseControlExtra(UINT message, int x, int y);	// by sigi
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	AddChatToHistory(const char * str, const char * sz_id, CHAT_LINE_CONDITION condition, DWORD color);
	void	AddToChatHistory(const char * sz_str, const char * sz_id, CHAT_LINE_CONDITION condition, DWORD color);
	void	SetInputString(const char* pString);	// by sigi
	void	AddInputString(const char* pString);	// by sigi
	void	SetInputStringColor(COLORREF rgb)	{ m_lev_chatting.SetInputStringColor(rgb); } // by sigi
	const char* GetInputString() const			{ return (const char*)m_lev_chatting.GetString(); }
#if __CONTENTS(__ITEM_SHOW)
	const char* GetNewInputString()	{ char * p_temp = NULL; g_Convert_DBCS_Ascii2SingleByte(m_lev_chatting.GetString(), m_lev_chatting.Size(), p_temp); return p_temp; }
#endif //__ITEM_SHOW
	void	InsertMark(char_t ch)				{ m_lev_chatting.InsertMark(ch); }

	bool	IsNormalChatStacked()						{ return m_chat_stacked[CLD_NORMAL]; }
	bool	IsWhisperStacked()							{ return m_chat_stacked[CLD_WHISPER]; }
	bool	IsZoneChatStacked()							{ return m_chat_stacked[CLD_ZONECHAT]; }
	bool	IsGuildChatStacked()						{ return m_chat_stacked[CLD_GUILD]; }
	bool	IsPartyChatStacked()						{ return m_chat_stacked[CLD_PARTY]; }
	bool	IsInfoStacked()								{ return m_chat_stacked[CLD_INFO]; }

	// common
	void	UnacquireMouseFocus() 
	{
		m_pC_button_group->UnacquireMouseFocus();
		m_pC_input_button_group->UnacquireMouseFocus();
		m_pC_input_right_button_group->UnacquireMouseFocus();
		m_pC_Choose_button_group->UnacquireMouseFocus();
	}
	void	CancelPushState() 
	{
		m_pC_button_group->CancelPushState();
		m_pC_input_button_group->CancelPushState();
		m_pC_input_right_button_group->CancelPushState();
		m_pC_Choose_button_group->CancelPushState();
	}
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);
	void	AcquireDisappear() {}
	void	Run(id_t id);

	void	Process();
	
	bool	IsCannotSendChatInput();		// Chat Message ������ �Ұ����� �����ΰ�?
};

//-----------------------------------------------------------------------------
// class C_VS_UI_INVENTORY
//
// slayer/vampire inventory base class.
//
// - slayer/vampire inventory ������ interface�� �����Ѵ�. 
// - slayer���� vampire�� �ٲ�ų� �Ǵ� �� �ݴ��� ��, ��ü�� ������Ͽ�
//   inventory�� �ʱ�ȭ������, inventory�� item�� �״�� �����ȴ�. interface�󿡼���
//   inventory�� item�� �����ϰ� ���� �ʴ�. �׷��� �� class�� base�� �ϴ� ���̴�.
//-----------------------------------------------------------------------------
class C_VS_UI_INVENTORY : public Window, public Exec, public ButtonVisual
{
	//-----------
	// Data
	//-----------
public:
	enum 
	{ 
		GRID_X = 10, 
		GRID_Y = 6,
		GRID_UNIT_PIXEL_X = 30,
		GRID_UNIT_PIXEL_Y = 30,
	};

private:

protected:
	enum INVENTORY_SPK_INDEX
	{
		INVENTORY_WINDOW,
		INVENTORY_WINDOW_ALPHA,
		INVENTORY_ITEMBACK,
		INVENTORY_ITEMBACK_ALPHA,
		INVENTORY_WINDOW_BOTTOM,
		//*
		INVENTORY_CONTRACT_OF_BLOOD_SLOT_1,
		INVENTORY_CONTRACT_OF_BLOOD_SLOT_2,
		INVENTORY_CONTRACT_OF_BLOOD_SLOT_3,
		INVENTORY_CONTRACT_OF_BLOOD_SLOT_4,
		/**/
	};

	enum EXEC_ID
	{
		MONEY_ID,
		DESC_ID,
//		BIKE_ID,
//		EXCHANGE_ID,
		
		HELP_ID,
		CLOSE_ID,
		ALPHA_ID,
	};

	ButtonGroup *				m_pC_button_group;

	bool						m_bl_shift, m_bl_set_load;;

	C_SPRITE_PACK *				m_pC_inventory_spk;
	Rect						m_grid_rect; // ��ü Grid rect.

	int							m_focus_grid_x, m_focus_grid_y;

	// ��ӹ޴� class���� �ݵ�� set����� �Ѵ�.
	int							m_grid_start_offset_x, m_grid_start_offset_y;
	int							m_money_button_offset_x, m_money_button_offset_y;

private:
	BOOL						m_bLimited_Item_Change;			
	int							m_focus_grid_x_Prev, m_focus_grid_y_Prev;

	//2008.09.24 shootkj
	//����� ���� ���� �������� �ð��� �����۰� ��ü �Ҷ� �������� cannotadd�� �� �� �ִµ� �̶����� 
	//��ü�� ���� �ʵ��� �ٽ� ����ġ ���� ��� �Ѵ�.

	//------------
	// Method
	//------------
private:
	void	SetDebugStart();
	void	SetDebugEnd();
	void	WriteLogLine(int line);
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);
	void	UnacquireMouseFocus();
	void	AcquireDisappear();
	void	AcquireMouseFocus();
//	void	CancelPushState();
	bool	AllMatchWindowPixel(int _x, int _y) const;
	bool	AnyMatchWindowPixel(int _x, int _y) const;

protected:
	bool	Click(int grid_start_x, int grid_start_y);
	void	Use();


public:
// TIMER
	static bool		Timer(bool reset = false);
	static DWORD						m_dw_millisec;

	static C_SPRITE_PACK *			m_pC_mine_progress_spk;
	enum MINE_PROGRESS_SPK_INDEX
	{
		INVENTORY_BAR_BACK,
		INVENTORY_BAR,
		SKILL_BAR_BACK,
		SKILL_BAR,
	};

	static int					m_mine_grid_x, m_mine_grid_y;

	C_VS_UI_INVENTORY();
	virtual ~C_VS_UI_INVENTORY();

	//���ڼ�ġ
	bool	StartInstallMineProgress(int focus_grid_x, int focus_grid_y);
	bool	IsInstallMineProgress()		{ return gbl_mine_progress; }
	void	EndInstallMineProgress()	{ gbl_mine_progress = false; }
	//���ڸ����
	bool	StartCreateMineProgress(int focus_grid_x, int focus_grid_y);
	bool	IsCreateMineProgress()		{ return gbl_mine_progress; }
	void	EndCreateMineProgress()	{ gbl_mine_progress = false; }
	//��ź �����
	bool	StartCreateBombProgress(int focus_grid_x, int focus_grid_y);
	bool	IsCreateBombProgress()		{ return gbl_mine_progress; }
	void	EndCreateBombProgress()	{ gbl_mine_progress = false; }

	void	Start(bool bl_set_load = true);
	void	Finish();
	void	Show();
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	CancelPushState() { m_pC_button_group->CancelPushState(); }
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
	void	Run(id_t id);

	void	ResetRect();
	bool	TestGridRect(int _x, int _y) const;
	int	GetFocusedItemGridX(const MItem * p_item);
	int	GetFocusedItemGridY(const MItem * p_item);
	int	GetFocusedItemGridH(const MItem * p_item);
	Rect	GetGridRect() const { return m_grid_rect; }
	
	int		GetFocusGridX() {return  m_focus_grid_x;}
	int		GetFocusGridY() {return  m_focus_grid_y;}

	static void	AutoMove( int grid_x, int grid_y );
	

	void	Prev_Item_Position_Remove();
	//2008.09.24 shootkj
	//����� ���� ���� �������� �ð��� �����۰� ��ü �Ҷ� �������� cannotadd�� �� �� �ִµ� �̶����� 
	//��ü�� ���� �ʵ��� �ٽ� ����ġ ���� ��� �Ѵ�.

	DWORD	m_dwWidth;
	DWORD	m_dwHight;
	int		m_nGearItemQuickViewScroll;
};

//-----------------------------------------------------------------------------
// class C_VS_UI_GEAR
//
// vampire/slayer gear base class.
//-----------------------------------------------------------------------------
class C_VS_UI_GEAR : public Window, public Exec, public ButtonVisual
{
	//-----------
	// Data
	//-----------
protected:
	//enum GEAR_SPK_INDEX
	//{
	//	GEAR_WINDOW,
	//	GEAR_WINDOW_ALPHA,
	//	GEAR_BLOOD_BIBLE_BLOCK,
	//	GEAR_CHANGE_ID1_CHECK,
	//	GEAR_CHANGE_ID2_CHECK,
	//	GEAR_CHANGE_ID1_UNCHECK,
	//	GEAR_CHANGE_ID2_UNCHECK,

	//	// ���⼭���� �����̾� ���� (���� ���� ����� ������� �ʴ� ������ ���� - Gamecommon.cpp�� ã�ƺ��� �ȳ���.)
	//	BUTTON_FIRST,
	//	BUTTON_FIRST_HILIGHTED,
	//	BUTTON_FIRST_HILIGHTED_PUSHED,
	//	BUTTON_SECOND,
	//	BUTTON_SECOND_HILIGHTED,
	//	BUTTON_SECOND_HILIGHTED_PUSHED,
	//};
	enum GEAR_SPK_INDEX
	{
		GEAR_WINDOW,//0
		GEAR_SLAYER_BACKGROUD_WINDOW,//1
		GEAR_VAMPRIE_BACKGROUD_WINDOW,//2
		GEAR_OUSTER_BACKGROUD_WINDOW,//3
		GEAR_WINDOW_LINE_1,//4
		GEAR_WINDOW_LINE_2,//5
		GEAR_BUTTON_SLOT_1_1,//6
		GEAR_BUTTON_SLOT_1_2,//7
		GEAR_BUTTON_SLOT_1_3,//8
		GEAR_BUTTON_SLOT_2_1,//9
		GEAR_BUTTON_SLOT_2_2,//10
		GEAR_BUTTON_SLOT_2_3,//11
		GEAR_BUTTON_CLOSE_1_1,//12
		GEAR_BUTTON_CLOSE_1_2,//13
		GEAR_BUTTON_CLOSE_1_3,//14
		GEAR_BUTTON_DOWN_1_1,//15
		GEAR_BUTTON_DOWN_1_2,//16
		GEAR_BUTTON_DOWN_1_3,//17
		GEAR_BUTTON_SEARCH_1_1,//18
		GEAR_BUTTON_SEARCH_1_2,//19
		GEAR_BUTTON_SEARCH_1_3,//20
		GEAR_BUTTON_ANY_1_1,//21
		GEAR_BUTTON_ANY_1_2,//22
		GEAR_BUTTON_ANY_1_3,//23
		GEAR_BLACK_LEFT,//24
		GEAR_BLACK_MID,//25
		GEAR_BLACK_WINDOW,//26
		GEAR_BALCK_SMALL,//27
		GEAR_BLUE_BAR,//28
		GEAR_BLOCK_BAR,//29
		GEAR_MONEY_BAR,//30
		GEAR_MONEY_ICON,//31
		GEAR_BLOCK_BAR_BIG,//32
		GEAR_BACKGROUD_1,//33
		GEAR_BACKGROUD_2,//34
		GEAR_BEIBAO_5X,//35
		GEAR_BEIBAO_FULL,//36
		GEAR_BEIBAO_SELECT,//37
		GEAR_ADDPOINT,//38
		GEAR_SLOT_1,//39
		GEAR_SLOT_2,//40
		GEAR_SLOT_3,//41
		GEAR_SLAYER_SWORD,//42
		GEAR_SLAYER_DUN,//43
		GEAR_SLAYER_TOU,//44
		GEAR_SLAYER_TUNXUN,//45
		GEAR_SLAYER_BEIJIAN,//46
		GEAR_SLAYER_YIFU,//47
		GEAR_SLAYER_KUZI,//48
		GEAR_SLAYER_XIEZI,//49
		GEAR_SLAYER_SHOUTAO,//50
		GEAR_SLAYER_YAODAI,//51
		GEAR_SLAYER_XIANGLIAN,//52
		GEAR_SLAYER_JIEZHI,//53
		GEAR_SLAYER_SHOUZUO,//54
		GEAR_SHENGSHU,//55
		GEAR_SHENGSHU_BLOCK,//56
		GEAR_VAMPRIE_WEAPON,//57
		GEAR_VAMPRIE_MIANJU,//58
		GEAR_VAMPRIE_WENXIN,//59
		GEAR_VAMPRIE_YIFU,//60
		GEAR_VAMPRIE_ERHUAN,//61
		GEAR_VAMPRIE_SHOUZUO,//62
		GEAR_VAMPRIE_QIU,//63
		GEAR_OUSTER_TOU,//64
		GEAR_OUSTER_HUADAI,//65
		GEAR_OUSTER_SHOUTAO,//66
		GEAR_OUSTER_BIHUAN,
		GEAR_OUSTER_YIFU,//67
		GEAR_OUSTER_SHITOU_1,//68
		GEAR_OUSTER_SHITOU_2,//69
		GEAR_OUSTER_SHITOU_3,//70
		GEAR_OUSTER_SHITOU_4,//71
		GEAR_OUSTER_WUQI_1,//72
		GEAR_OUSTER_WUQI_2,//73
		GEAR_OUSTER_XIEZI,//74
		GEAR_WINDOW_LAST,//75		
	};

	enum EXEC_ID
	{
		HELP_ID,
		CLOSE_ID,

		// ���⼭���� �����̾� ����
		FIRST_ID,
		SECOND_ID,

		ALPHA_ID,

		// ����â ��ü ��ư
		GEAR_ID1,
		GEAR_ID2,
		Point_ID1,
		Point_ID2,
		Point_ID3,
	};

	bool					m_bIsGearOther;

	ButtonGroup *				m_pC_button_group;
	ButtonGroup *				m_pC_point_button_group;

	C_SPRITE_PACK *			m_pC_gear_spk; // gear Window image
	C_SPRITE_PACK *			m_pC_gear_slot_spk; // gear slot image
	//by csm 2004.12.30  
	C_SPRITE_PACK *			m_pC_advance_gear_spk; // Advencegear Window image1
	

	int							m_focus_slot;
	bool						m_bl_set_load;

	// ��ӹ޴� class���� �ݵ�� set����� �Ѵ�.
	int								m_slot_size;
	Rect *							m_p_slot_rect;
	int *							m_p_slot_image;
	BOOL 							m_bl_Abvencement;

	// 2026-08-31 CHROME CACHE (perf): pencere iskeleti (baslik, slot
	// cerceveleri, bible cerceveleri, siyah barlar, cizgi grid) session
	// boyunca statiktir (race + cozunurluk sabit). Bir kez pencere boyutlu
	// offscreen surface'e cizilir, her frame tek colorkey Blt ile basilir.
	// Eskiden her frame ~80 sprite blit + ~30 clip blit tekrarlaniyordu.
	CSpriteSurface *				m_pChromeCache;
	bool							m_bChromeCacheValid;

	void	DrawChromeStatic(int base_x, int base_y);
	bool	EnsureChromeCache();

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// C_VS_UI_GEAR ���� â ��ư �� ID�� ���� ���� ����
	DWORD							m_dwSendGearID;							//������ ���� GearID
	DWORD							m_dwAccepGearID;						//�������� ���� GearID
	DWORD							m_dwGearChange_ID1_Button_Pos_X;		//����â�� ��ü�� ��ư�� ��ġ
	DWORD							m_dwGearChange_ID1_Button_Pos_Y;
	DWORD							m_dwGearChange_ID1_Button_Width;		//����â�� ��ü�� ��ư�� ũ��
	DWORD							m_dwGearChange_ID1_Button_Height;
	DWORD							m_dwGearChange_ID2_Button_Pos_X;		//����â�� ��ü�� ��ư�� ��ġ
	DWORD							m_dwGearChange_ID2_Button_Pos_Y;
	DWORD							m_dwGearChange_ID2_Button_Width;		//����â�� ��ü�� ��ư�� ũ��
	DWORD							m_dwGearChange_ID2_Button_Height;
#endif	// __GEAR_SWAP_CHANGE
// 

//	bool							m_bl_close_button_pushed;
//	bool							m_bl_help_button_pushed;
//	bool							m_bl_close_button_focused;
//	bool							m_bl_help_button_focused;
//	int							m_close_button_offset_x, m_close_button_offset_y;
//	int							m_help_button_offset_x, m_help_button_offset_y;


	void	Use();
	//------------
	// Method
	//------------
private:
//	void	CancelPushState();
	void	WindowEventReceiver(id_t event);
	void	AcquireDisappear();
	void	AcquireMouseFocus();
	void	UnacquireMouseFocus();
	bool	AnyMatchWindowPixel(int _x, int _y) const;
	bool	AllMatchWindowPixel(int _x, int _y) const;
	bool	TestSlotRect(int _x, int _y) const;
	bool	IsPixel(int _x, int _y);

public:
	C_VS_UI_GEAR();
	virtual ~C_VS_UI_GEAR();

	void	Start(bool bl_set_load = true);
	void	Finish();
	void	GearOther(bool b) { m_bIsGearOther = b; }
	
	bool	Click(int window_x, int window_y, Rect * slot_rect);
	bool	MouseControl(UINT message, int _x, int _y);
	void	Show() ;
	void	CancelPushState() { m_pC_button_group->CancelPushState(); }
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	Run(id_t id);
	void	SetFocusSlot(BYTE Slot) { m_focus_slot = Slot;}

#if __CONTENTS(__GEAR_SWAP_CHANGE)			// C_VS_UI_GEAR ����â ��ü �Լ� �� ��ư ���̱� �Լ� ����
	void	GearChange();
	void	GearChange(DWORD	dwGearID);
	void	GearChangeButtonShow();
#endif //__GEAR_SWAP_CHANGE

};

//-----------------------------------------------------------------------------
// class C_VS_UI_SKILL
//
// vampire/slayer skill base class.
//-----------------------------------------------------------------------------
class C_VS_UI_SKILL : public Window
{
public:
	// Phoenix: bu iki statik üye yorum satırına alınmıştı ama
	// vs_ui_gamecommon2.cpp (C_VS_UI_QUEST_MANAGER vb.) hâlâ kullanıyor
	// (sprite tablosu lookup'ı için). Geri açıldı — silme yok, devre dışı
	// bırakma var kuralı kapsamında değil; zaten kaynak kodda vardı.
	static CSpritePack				m_C_spk;      // Skill icon Sprite list
	static CSpritePack				m_C_spk_mini; // Skill icon Sprite list (küçük)
	static CSpritePack				m_C_spk_mini_new ;
	static CSpritePack				m_C_spk_comboCnt ;

	enum HOTKEY
	{
		HOTKEY1,
		HOTKEY2,
		HOTKEY3,
		HOTKEY4,

		//���Ŀ� ��Ű
		HOTKEY5,
		HOTKEY6,
		HOTKEY7,
		HOTKEY8,
//
		HOTKEY9,
		HOTKEY10,
		HOTKEY11,
		HOTKEY12,

		HOTKEY_MAX,
	};

	enum HOTKEY_GRADE
	{
		GRADE1,
		GRADE2,
		GRADE3,
		GRADE4,

		GRADE_MAX,
	};

protected:
	int							m_skill_hotkey_buf[HOTKEY_MAX][GRADE_MAX];
	int							m_current_skill_set, m_current_skill_grade;
	enum 
	{ 
		// �� ĳ���Ͱ� ��� �� �ִ� skill ���� 100�� ���϶��...
		SPREAD_X_MAX = 10, // �������� ���� ������ �ִ� ����(ù line�� Spread button�� �����Ѵ�).
		SPREAD_Y_MAX = 10,
	};

	C_SPRITE_PACK				m_etc_spk;
	//C_SPRITE_PACK				m_quikItemPack;

	// ��ӹ޴� ��ü���� set���� ��.
	int							m_skill_guard_x, m_skill_guard_y;
	int							m_skill_start_x, m_skill_start_y;
	int							m_skill_guard_extra_offset;
	int							m_extra_icon_offset_x, m_extra_icon_offset_y;

	enum ETC_SPK_INDEX
	{
		SKILL_GUARD, // skill icon �׵θ�
		SKILL_EMPTY_BUTTON,
		SKILL_PUSHED_BUTTON,
	};
	
	int							m_comboCnt ;
	int							m_selected_skillid; // NOT_SELECTED�̸� ���õ��� ���� ���̴�. ACTIONINFO�� �ϳ��̴�.
	bool						m_selected_comboAttackskill ; 
	int							m_focused_slot; // Spread button�� 0���� �ϰ� �¿��� ��� �����Ѵ�.
	bool						m_bl_pushed;
	bool						m_bl_open;

	// Progress Bar ����
	bool						m_bl_show_progress;
	int							m_progress_skill_id;
	CPoint						m_progress_inventory_grid;
	MItem*						m_pC_progress_item;
	CTickTimer					m_progress_timer;

	void	ResetSize();
	int		GetFocusSlot(int _x, int _y) const;
	
	bool	AbleToUse(int id) const;
	bool	IsEnableSkill(int id) const;
	int		GetDelay(int id) const;

	bool	NoPassive(int n);
	bool	findSkillAvailable(ACTIONINFO id);

// TIMER
	DWORD						m_dw_prev_tickcount;
	DWORD						m_dw_millisec;

	bool	Timer(bool reset = false);

	void	ProcessSkillCastingProgress();

	void	DrawSkillIcon(CPoint ptPos, int nSkillID);

public:
	void DrawSkillIconInQuickBar(CPoint ptPos);

public:
	C_VS_UI_SKILL();
	virtual ~C_VS_UI_SKILL();

	void	SetStartPoint(int x, int y)	{ m_skill_start_x = x; m_skill_start_y = y; ResetSize(); }
	int		GetSelectedSkillID();
	void	SelectSkill(int id);
	int		GetHotkey(HOTKEY hotkey, HOTKEY_GRADE grade) const { return m_skill_hotkey_buf[hotkey][grade]; }
	void	ChangeSkillWheel(bool direct);
	void	ChangeSkillByHotkey(HOTKEY hotkey);
	int		GetSkillID(int n);
	int		GetFocusedSlot() const { return m_focused_slot; }
	void	SetHotkey(HOTKEY hotkey, ACTIONINFO id);
	void	ToggleWindow();
	Point	GetIconPoint(int n) const;
	void	CloseInterface();
	void	UnselectSkill();

	void	AcquireDisappear() {}
	void	CancelPushState();
	void	UnacquireMouseFocus();	
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y)	{ return false; }
	bool	IsPixel2(int _x, int _y);

	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	Show() { return; }
	void	Show2();
	void	Start();
	void	Process();
	void	ResetSkillSet();
	void	SetComboCnt(int ComboCnt) ; 
	void	SetSelectedAttackComboSkill(bool	bAttackSkill) ; 
	void	ResetHotkey();
	void	InitHotKey(HOTKEY hotkey) ;

	void	StartSkillCastingProgress(int skillID, DWORD time, CPoint inventoryGrid = CPoint(-1, -1));
	void	EndSkillCastingProgress();
	bool	IsSkillCastingProgress()	{ return m_bl_show_progress;	}
};

//-----------------------------------------------------------------------------
// class C_VS_UI_HPBAR
//
// vampire/slayer HPBar class
//-----------------------------------------------------------------------------
/*class C_VS_UI_HPBAR : public Window, public Exec, public ButtonVisual
{
private:
	bool					m_width_mode, m_small_mode;
	C_SPRITE_PACK			* m_pC_hpbar_spk;

	C_SPRITE_PACK*			m_pImage_hays_0;
	C_SPRITE_PACK*			m_pImage_hays_1;
	C_SPRITE_PACK*			m_pImage_hays_2;

	//enum SPK_0
	//{
	//	BK_SLAYER,
	//	SLAYER_MALE,
	//	SLAYER_FEMALE,
	//	BK_SLAYER_FILL,
	//	BK_VAMPIRE,
	//	VAMPIRE_MALE,
	//	VAMPIRE_FEMALE,
	//	BK_VAMPIRE_FILL,
	//	BK_OUSTERS,
	//	OUSTERS_FEMAL,
	//	OUSTERS_FILL,

	//	BK_HPBAR_LEFT,
	//	LONG_HPBAR,
	//	SPK0_UNKNOW_0,
	//	SPK0_UNKNOW_1,
	//	SPK0_UNKNOW_2,
	//};

	//enum SPK_1
	//{
	//	SPK1_UNKNOW_0,
	//	SPK1_UNKNOW_1,
	//	SPK1_UNKNOW_2,
	//	BK_MPBAR_RIGHT,
	//	LONG_MPBAR,
	//	LONG_GRAYBAR,
	//	BK_SHORT_HP,
	//	SHORT_HP,
	//	BK_SHORT_MP,
	//	SHORT_MP,
	//	SHORT_GRAY,
	//	SHORT_YELLOW,
	//	SHORT_GREEN,
	//	LONG_YELLOW,
	//	LONG_GREEN,
	//	BK_V_HP,
	//};

	//enum SPK_2
	//{
	//	V_LONG_HP,
	//	BK_V_MP,
	//	V_LONG_MP,
	//	V_LONG_GRAY,
	//	BK_SHORT_HP,
	//	V_SHORT_HP,
	//	BK_SHORT_MP,
	//	V_MP_SHORT_GRAY,
	//	V_MP_SHORT_YELLOW,
	//	V_MP_SHORT_GREEN,
	//	V_MP_LONG_YELLOW,
	//	V_MP_LONG_GREEN,
	//	V_MP_SHORT_BLUE,
	//	H_MP_LONG_GRAY,
	//	H_MP_SHORT_GRAY,
	//	V_
	//};


	ButtonGroup *			m_pC_width_button_group;
	ButtonGroup *			m_pC_height_button_group;
	ButtonGroup *			m_pC_small_width_button_group;
	ButtonGroup *			m_pC_small_height_button_group;

	enum HPBAR_SPK_INDEX
	{
		MAIN_WIDTH,
		MAIN_HEIGHT,
		HPBAR_WIDTH,
		HPBAR_HEIGHT,

		// slayer
		MPBAR_WIDTH,
		MPBAR_HEIGHT,
		HPBAR_BLOOD_WIDTH,
		HPBAR_BLOOD_HEIGHT,
		CHANGE_BUTTON_WIDTH,
		CHANGE_BUTTON_WIDTH_HILIGHTED,
		CHANGE_BUTTON_HEIGHT,
		CHANGE_BUTTON_HEIGHT_HILIGHTED,

		SLAYER_MAX,
		
		// vampire
		EXPBAR_WIDTH = MPBAR_WIDTH,
		EXPBAR_HEIGHT,
		HPBAR_SILVER_WIDTH,
		HPBAR_SILVER_HEIGHT,
		CHANGE_BUTTON,
		CHANGE_BUTTON_PUSHED,
		CHANGE_BUTTON_HILIGHTED,
		CHANGE_BUTTON_HILIGHTED_PUSHED,
		SMALL_BUTTON,
		SMALL_BUTTON_PUSHED,
		SMALL_BUTTON_HILIGHTED,
		SMALL_BUTTON_HILIGHTED_PUSHED,

		VAMPIRE_MAX,

		// ousters				
		OUSTERS_EXPBAR_WIDTH = HPBAR_BLOOD_WIDTH,
		OUSTERS_EXPBAR_HEIGHT,
		OUSTERS_HPBAR_BLOOD_WIDTH,
		OUSTERS_HPBAR_BLOOD_HEIGHT,
		OUSTERS_HPBAR_SILVER_WIDTH,
		OUSTERS_HPBAR_SILVER_HEIGHT,
		OUSTERS_CHANGE_BUTTON_WIDTH,
		OUSTERS_CHANGE_BUTTON_WIDTH_PUSHED,
		OUSTERS_CHANGE_BUTTON_HEIGHT,
		OUSTERS_CHANGE_BUTTON_HEIGHT_PUSHED,
		OUSTERS_SMALL_BUTTON_WIDTH,
		OUSTERS_SMALL_BUTTON_WIDTH_PUSHED,
		OUSTERS_SMALL_BUTTON_HEIGHT,
		OUSTERS_SMALL_BUTTON_HEIGHT_PUSHED,

		OUSTERS_MAX,

		OUSTERS_2ND_MPBAR_WIDTH = 40,
		OUSTERS_2ND_MPBAR_HEIGHT,
		OUSTERS_3ND_MPBAR_WIDTH,
		OUSTERS_3ND_MPBAR_HEIGHT,

		OUSTERS_2ND_SMALL_MPBAR_WIDTH,
		OUSTERS_2ND_SMALL_MPBAR_HEIGHT,
		OUSTERS_3ND_SMALL_MPBAR_WIDTH,
		OUSTERS_3ND_SMALL_MPBAR_HEIGHT,
	};

	enum EXEC_ID
	{
		CHANGE_ID,
		RESIZE_ID,
	};

	int	m_small_offset;

public:
	C_VS_UI_HPBAR();
	~C_VS_UI_HPBAR();

	void	UnacquireMouseFocus() 
	{
		m_pC_width_button_group->UnacquireMouseFocus();
		m_pC_height_button_group->UnacquireMouseFocus();
		m_pC_small_width_button_group->UnacquireMouseFocus();
		m_pC_small_height_button_group->UnacquireMouseFocus();
	}
	void	CancelPushState() 
	{
		m_pC_width_button_group->CancelPushState();
		m_pC_height_button_group->CancelPushState();
		m_pC_small_width_button_group->CancelPushState();
		m_pC_small_height_button_group->CancelPushState();
	}
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);
	void	AcquireDisappear() {}
	void	Run(id_t id);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	Show();
	void	Process();
	void	Start();
	void	Finish();
};(			  */
class C_VS_UI_HPBAR : public Window, public Exec, public ButtonVisual
{
private:
	bool					m_width_mode, m_small_mode;
	C_SPRITE_PACK			* m_pC_hpbar_spk;

	//ButtonGroup *			m_pC_width_button_group;
	//ButtonGroup *			m_pC_height_button_group;
	//ButtonGroup *			m_pC_small_width_button_group;
	//ButtonGroup *			m_pC_small_height_button_group;

	enum HPBAR_SPK_INDEX
	{
		SLAYER_KUANG_JIA,//����ͷ��� 1
		SLAYER_NAN_TOU_XIANG,//������ͷ�� 2
		SLAYER_NV_TOU_XIANG,//����Ůͷ�� 3
		SLAYER_BEI_JING,//����ͷ��򱳾� 4

		VAMPIRE_KUANG_JIA,//���� ͷ��� 5
		VAMPIRE_NAN_TOU_XIANG,//���� ��ͷ�� 6
		VAMPIRE_NV_TOU_XIANG,//���� Ůͷ�� 7
		VAMPIRE_BEI_JING,//���� ͷ��򱳾� 8

		OUSTERS_KUANG_JIA,//ħ�� ͷ��� 9
		OUSTERS_TOU_XIANG,//ħ�� ͷ�� 10
		OUSTERS_BEIJING,//ħ�� ͷ��򱳾� 11

		LEFT_WIDTH_KUANGJIA,//��߿�Ѫ����� 12
		WIDTH_HPBAR_RED,//��߿�Ѫ�� 13
		CHANGE_BUTTON_WIDTH,//�ı��С��ť 14
		CHANGE_BUTTON_WIDTH_DOWN,//�ı��С��ť���� 15
		CHANGE_BUTTON_WIDTH_UP,//�ı��С��ť���� 16

		CHANGE_BUTTON_HEIGHT,//�ı������ť�� 17
		CHANGE_BUTTON_HEIGHT_DOWN,//�ı������ť���� 18
		CHANGE_BUTTON_HEIGHT_UP,//�ı������ť���� 19
		RIGHT_WIDTH_KUANGJIA,//�ұ߿�Ѫ����� 20
		WIDTH_MPBAR,//��MP�� 21
		WIDTH_HPBAR_BAN,//��Ч�� 22
		SMALL_LEFT_WIDTH_KUANGJIA,//С�Ϳ����Ѫ����� 23
		SMALL_WIDTH_HPBAR_RED,//С�Ϳ�Ѫ�� 24
		SMALL_RIGHT_WIDTH_KUANGJIA,//С�Ϳ��ұ�Ѫ����� 25
		SMALL_WIDTH_MPBAR,//С�Ϳ� MP�� 26
		SMALL_WIDTH_HPBAR_BAN,//С�Ϳ���Ч�� 27
		SMALL_WIDTH_HPBAR_YELLOW,//С�Ϳ���ɫ�� 28
		SMALL_WIDTH_HPBAR_GREEN,//С�Ϳ���ɫ�� 29
		WIDTH_HPBAR_YELLOW,//����ɫ�� 30
		WIDTH_HPBAR_GREEN,//����ɫ�� 31
		LEFT_HEIGHT_KUANGJIA,//�������Ѫ����� 32

		HEIGHT_HPBAR_RED,//����Ѫ��
		RIGHT_HEIGHT_KUANGJIA,//�����ұ�Ѫ����� 32
		HEIGHT_MPBAR,//����MP�� 33
		HEIGHT_HPBAR_BAN,//������Ч�� 34

		SMALL_LEFT_HEIGHT_KUANGJIA,//�������Ѫ����� 35
		SMALL_HEIGHT_HPBAR_RED,//����Ѫ�� 36
		SMALL_RIGHT_HEIGHT_KUANGJIA,//�����ұ�Ѫ����� 37
		SMALL_HEIGHT_HPBAR_BAN,//������Ч�� 38
		SMALL_HEIGHT_HPBAR_YELLOW,//������ɫ�� 39
		SMALL_HEIGHT_HPBAR_GREEN,//������ɫ�� 40
		HEIGHT_HPBAR_YELLOW,//������ɫ�� 41
		HEIGHT_HPBAR_GREEN,//������ɫ�� 42
		SMALL_HEIGHT_MPBAR,//С������MP�� 43
		BLOOD_WIDTH,//����Ѫ 44
		SMALL_BLOOD_WIDTH,//С�Ϳ�����Ѫ 45
		BLOOD_HEIGHT,//��������Ѫ 46
		SMALL_BLOOD_HEIGHT,//С����������Ѫ 47
		New_One,
	};

	enum EXEC_ID
	{
		CHANGE_ID,
		RESIZE_ID,
	};

	int	m_small_offset;

public:
	C_VS_UI_HPBAR();
	~C_VS_UI_HPBAR();

	//void	UnacquireMouseFocus() 
	//{
	//	m_pC_width_button_group->UnacquireMouseFocus();
	//	m_pC_height_button_group->UnacquireMouseFocus();
	//	m_pC_small_width_button_group->UnacquireMouseFocus();
	//	m_pC_small_height_button_group->UnacquireMouseFocus();
	//}
	//void	CancelPushState() 
	//{
	//	m_pC_width_button_group->CancelPushState();
	//	m_pC_height_button_group->CancelPushState();
	//	m_pC_small_width_button_group->CancelPushState();
	//	m_pC_small_height_button_group->CancelPushState();
	//}
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);
	void	AcquireDisappear() {}
	void	Run(id_t id);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	Show();
	void	Process();
	void	Start();
	void	Finish();
};


//-----------------------------------------------------------------------------
// C_VS_UI_SHORTCUT_SLOT
//
// 
//-----------------------------------------------------------------------------
class C_VS_UI_SHORTCUT_SLOT : public Window, public Exec, public ButtonVisual
{
private:
	enum SPK_ID
	{
		SLOT_BOX=12,
		SLOT_SELETED_BORDER=16,
		HOTKEY_BOX=13,
		HOTKEY_SELETED_BOX=17,

		BUTTON_HIDE=14,
		BUTTON_HIDE_EXPOSE=15,
	};


	int		m_slot_width;
	int		m_focused_slot;
	bool	m_bl_slot_hide;

	C_SPRITE_PACK *			m_pC_shortcut_slot_spk;
	ButtonGroup *			m_pC_button_group;
public:

	C_VS_UI_SHORTCUT_SLOT();
	~C_VS_UI_SHORTCUT_SLOT();

	void	UnacquireMouseFocus() 
	{
		m_pC_button_group->UnacquireMouseFocus();
	}
	void	CancelPushState() 
	{
		m_pC_button_group->CancelPushState();
	}
	void	AcquireDisappear() {}


	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);


	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	Run(id_t id);
	void	Show();

	void	DrawSkillIcon(POINT ptPos, int skillid);
	void	_DrawSkillIcon(int hk);
	void	_DrawSkillHotKey(int hk);
	void	_DrawSkillCooldownText(int hk);
	int		GetActiveHotkeySkillId(int hk) const;
	void	DrawSkillCooldownText(POINT ptPos, int skillid) const;

	int		GetDelay(int id) const;
	const	MItem* Vampire_SerumF1();
	const	MItem* Vampire_SerumF2();

	void	Process();
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	Start();
	void	Finish();
};

//-----------------------------------------------------------------------------
// class C_VS_UI_BLOOD_BURST
//
// vampire/slayer BloodBurst class
//-----------------------------------------------------------------------------
class C_VS_UI_BLOOD_BURST : public Window, public Exec, public ButtonVisual
{
private:
	
	enum EXEC_ID
	{
		BLOODBURST_CHANGE_ID,
		ATTACK_ID, 
		DEFENSE_ID,
		PARTY_ID,
	};


/*	enum BLOOD_BURS_SPK_INDEX
	{
		MAIN_WIDTH	,
		MAIN_HEIGHT , 
		
		PARTY_MAIN_WIDTH , 
		PARTY_MAIN_HEIGHT , 
		
		CHANGE_BUTTON_WIDTH , 
		CHANGE_BUTTON_WIDTH_HILIGHTED , 
		CHANGE_BUTTON_WIDTH_PUSHED ,

		CHANGE_BUTTON_HEIGHT , 
		CHANGE_BUTTON_HEIGHT_HILIGHTED , 
		CHANGE_BUTTON_HEIGHT_PUSHED ,

		MAIN_GAGE_BASE_ATTACK_WIDTH ,
		MAIN_GAGE_BASE_ATTACK_HEIGHT ,

		MAIN_GAGE_BASE_DEFENSE_WIDTH ,
		MAIN_GAGE_BASE_DEFENSE_HEIGHT ,
		
		ATTACK_GAGE_WIDTH ,
		ATTACK_GAGE_HEIGHT,
			
		DEFENSE_GAGE_WIDTH ,
		DEFENSE_GAGE_HEIGHT,
			
		PARTY_GAGE_WIDTH ,
		PARTY_GAGE_HEIGHT,
		
		ATTACK_GAGE_FULL_WIDTH ,
		ATTACK_GAGE_FULL_HEIGHT,

		DEFENSE_GAGE_FULL_WIDTH ,
		DEFENSE_GAGE_FULL_HEIGHT,

		PARTY_GAGE_FULL_WIDTH ,
		PARTY_GAGE_FULL_HEIGHT,
		
		ATTACK_BUTTON_HEIGHT,
		ATTACK_BUTTON_HILIGHTED_HEIGHT,
		ATTACK_BUTTON_PUSHED_HEIGHT,
		
		DEFENSE_BUTTON_HEIGHT,
		DEFENSE_BUTTON_HILIGHTED_HEIGHT,
		DEFENSE_BUTTON_PUSHED_HEIGHT,

		PARTY_BUTTON_HEIGHT,
		PARTY_BUTTON_HILIGHTED_HEIGHT,
		PARTY_BUTTON_PUSHED_HEIGHT,
		
		ATTACK_BUTTON_WIDTH,
		ATTACK_BUTTON_HILIGHTED_WIDTH,
		ATTACK_BUTTON_PUSHED_WIDTH,
		
		DEFENSE_BUTTON_WIDTH,
		DEFENSE_BUTTON_HILIGHTED_WIDTH,
		DEFENSE_BUTTON_PUSHED_WIDTH,

		PARTY_BUTTON_WIDTH,
		PARTY_BUTTON_HILIGHTED_WIDTH,
		PARTY_BUTTON_PUSHED_WIDTH,
	};*/

	enum BLOOD_BURS_SPK_INDEX
	{

		PARTY_MAIN ,
		PARTY_RED,
	    PARTY_BLUE,
		PARTY_GREEN,
		ATTACK_BUTTON,
		DEFENSE_BUTTON,
		PARTY_BUTTON,
	};

	enum	BLOOD_BURST_MAX 
	{
		ATTACK_MAX_POINT	= 3000	, 
 		DEFENSE_MAX_POINT	= 2000	, 
		PARTY_MAX_POINT		= 20000 , 
	};


	bool						m_width_mode	;

	bool						m_bGageAttackFull;
	bool						m_bGageDefenseFull;
	bool						m_bGagePartyFull;

	int							m_iAttackGage	;
	int							m_iDefenseGage	;
	int							m_iPartyGage	;
	
	float						m_fAttackGageStartPosition ; 
	float						m_fDefenseGageStartPosition ; 
	float						m_fPartyGageStartPosition ; 

	DWORD						m_dw_prev_Attacktickcount	;
	DWORD						m_dw_prev_Defensetickcount	;
	DWORD						m_dw_prev_Partyickcount		;

	bool						m_bAttackTimerCheck;
	bool						m_bDefenseTimerCheck;
	bool						m_bPartyTimerCheck;


	C_SPRITE_PACK	*			m_pC_BloodBurst_spk;

	ButtonGroup *				m_pC_width_button_group;


public:
	C_VS_UI_BLOOD_BURST();
	~C_VS_UI_BLOOD_BURST();

	void	UnacquireMouseFocus() 
	{
		m_pC_width_button_group->UnacquireMouseFocus();
	}
	void	CancelPushState() 
	{
		m_pC_width_button_group->CancelPushState();
	}
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);
	void	AcquireDisappear() {}
	void	Run(id_t id);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	Show();
	void	Process();
	void	Start();
	void	Finish();

	void	SetGage() ; 
	void	SetAttackGage() ; //int	AttackGage)	;
	void	SetDefenseGage(); //nt	DefenseGage);
	void	SetPartyGage() ; //int	PartyGage)	;

	bool	GetAttackGageFull()		{	return	m_bGageAttackFull;	}
	bool	GetDefenseGageFull()	{	return	m_bGageDefenseFull;	}
	bool	GetPartyGageFull()		{	return	m_bGagePartyFull;	}

};




//-----------------------------------------------------------------------------
// class C_VS_UI_BLOOD_BURST
//
// vampire/slayer BloodBurst class
//-----------------------------------------------------------------------------
/*class C_VS_UI_MARKET_ACCOUNT  : public Window, public Exec, public ButtonVisual
{
private:
	
	
	enum EXEC_ID
	{
		MARKETM_ID, 
		MARKETB_ID,
		MARKETH_ID,
	};


	enum MARKET_ACCOUNT_SPK_INDEX
	{
		MARKETM_BUTTON , 
		MARKETM_BUTTON_HILIGHTED , 
		MARKETM_BUTTON_PUSHED ,

		MARKETB_BUTTON , 
		MARKETB_BUTTON_HILIGHTED , 
		MARKETB_BUTTON_PUSHED ,

		MARKETH_BUTTON , 
		MARKETH_BUTTON_HILIGHTED , 
		MARKETH_BUTTON_PUSHED ,
	};

	
	C_SPRITE_PACK	*			m_pC_Market_spk;

	ButtonGroup *				m_pC_Market_button_group;
	

public:
	C_VS_UI_MARKET_ACCOUNT();
	~C_VS_UI_MARKET_ACCOUNT();

	void	UnacquireMouseFocus() 
	{
		m_pC_Market_button_group->UnacquireMouseFocus();
	}
	void	CancelPushState() 
	{
		m_pC_Market_button_group->CancelPushState();
	}

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);
	void	AcquireDisappear() {}
	void	Run(id_t id);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	Show();
	void	Process();
	void	Start();
	void	Finish();
}	;
*/


//-----------------------------------------------------------------------------
// class C_VS_UI_EFFECT_STATUS
//
// vampire/slayer EFFECT_STATUS class
//-----------------------------------------------------------------------------
class C_VS_UI_EFFECT_STATUS : public Window, public Exec, public ButtonVisual
{
private:
	int						m_scroll;
	bool					m_width_mode;
	bool					m_bl_effect_size_null;
	C_SPRITE_PACK			* m_pC_effect_status_spk;

	ButtonGroup *			m_pC_width_button_group;
	ButtonGroup *			m_pC_height_button_group;

	enum EFFECT_STATUS_SPK_INDEX
	{
		MAIN_WIDTH,
		MAIN_WIDTH_RIGHT,
		MAIN_HEIGHT,
		MAIN_HEIGHT_BOTTOM,

		BUTTON_LEFT,
		BUTTON_LEFT_HILIGHTED,
		BUTTON_LEFT_HILIGHTED_PUSHED,
		BUTTON_RIGHT,
		BUTTON_RIGHT_HILIGHTED,
		BUTTON_RIGHT_HILIGHTED_PUSHED,
		BUTTON_UP,
		BUTTON_UP_HILIGHTED,
		BUTTON_UP_HILIGHTED_PUSHED,
		BUTTON_DOWN,
		BUTTON_DOWN_HILIGHTED,
		BUTTON_DOWN_HILIGHTED_PUSHED,
		BUTTON_CHANGE,
		BUTTON_CHANGE_HILIGHTED,
		BUTTON_CHANGE_HILIGHTED_PUSHED,
	};

	enum EXEC_ID
	{
		PUSHPIN_ID,
		CHANGE_ID,
		UP_ID,
		DOWN_ID,
	};

public:
	C_VS_UI_EFFECT_STATUS();
	~C_VS_UI_EFFECT_STATUS();

	void	ResetSize();
	void	UnacquireMouseFocus() 
	{
		m_pC_width_button_group->UnacquireMouseFocus();
		m_pC_height_button_group->UnacquireMouseFocus();
	}
	void	CancelPushState() 
	{
		m_pC_width_button_group->CancelPushState();
		m_pC_height_button_group->CancelPushState();
	}
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);
	void	AcquireDisappear() {}
	void	Run(id_t id);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	Show();
	void	Process();
	void	Start();
	void	Finish();
};


struct MINIMAP_NPC
{
	int x;
	int y;
	int id;
	MString name;
};

struct MINIMAP_SHRINE
{
	int x;
	int y;
	int id;
	MString name;
	BYTE type;
};

struct MINIMAP_PORTAL
{
	CRect	position;
	int		zoneID;
};

//-----------------------------------------------------------------------------
// class C_VS_UI_MINIMAP
//
// vampire/slayer MINIMAP class
//-----------------------------------------------------------------------------
class C_VS_UI_MINIMAP : public Window, public Exec, public ButtonVisual
{
private:
	C_SPRITE_PACK			* m_pC_minimap_spk;
	
	CSpriteSurface			* m_p_minimap_surface;
	ButtonGroup *			m_pC_button_group;

	bool								m_bl_refresh;
	POINT								m_map_start_point;
	std::vector<RECT>					m_portal;
	std::vector<int>					m_portal_zone_id;
	std::vector<MINIMAP_NPC>			m_npc;
	std::vector<MINIMAP_SHRINE>			m_shrine;
	std::vector<POINT>					m_Block;
	std::vector<MINIMAP_NPC>			m_Flag;

	std::string m_zone_name;
	int									m_surface_w, m_surface_h;
	int									m_map_x, m_map_y, m_map_w, m_map_h, m_zone_id;
	int									m_board_x, m_board_y;

	//timer
	DWORD						m_dw_minimap_prev_tickcount;
	DWORD						m_dw_minimap_millisec;

	bool						m_bMiniMapDisableZone;	// �̴ϸ��� ������� �ʴ� ���̴�.

#if __CONTENTS(__GPS_ADD)
	int							m_GPSBoard_X;
	int							m_GPSBoard_Y;
	int							m_GPSButton_X;
	int							m_GPSButton_Y;
	bool						m_bGPSViewOpen;

	S_GPS_NPC_INFO				m_Gps_User_Info;
#endif // __GPS_ADD

	bool	TimerMinimap();

	enum MINIMAP_SPK_INDEX
	{
		MINIMAP_MAIN,
		MINIMAP_WORLD_MAP,
		MINIMAP_WORLD_MAP_HILIGHT,
		MINIMAP_WORLD_MAP_DOWN,
		MINIMAP_GAME_OPTIONS,
		MINIMAP_GAME_OPTIONS_HILIGHT,
		MINIMAP_GAME_OPTIONS_DOWN,
		MINIMAP_ALPHA_ID,
		MINIMAP_ALPHA_ID_DOWN,
		MINIMAP_HIDE_ID,
		MINIMAP_HIDE_ID_DOWN,
		MINIMAP_ICON_SELF,
		MINIMAP_ICON_PARTY,

#if __CONTENTS(__GPS_ADD)
		MINIMAP_GPSBTN_BOARD,
#endif	//__GPS_ADD
	};

	enum EXEC_ID
	{
		ALPHA_ID,
		PUSHPIN_ID,

#if __CONTENTS(__GPS_ADD)
		GPSVIEW_ID,
#endif //__GPS_ADD
		HOOK_ID,
		PAYEVENT_ID,
		GAME_OPTIONS_SHOW,
	};

	bool	m_bWindowEvent;

	// 2026-09-03 — skill hizli butonu minimap grubuna bagli (kullanici
	// istegi): minimap tasininca skill butonu birlikte tasinir. Offset,
	// TRIBE ctor'da legacy default yerlesimden hesaplanip verilir.
	C_VS_UI_SKILL *		m_pSkillFollow;
	int					m_SkillFollowOffX;
	int					m_SkillFollowOffY;

public:
	void	SetSkillFollow(C_VS_UI_SKILL * p_skill, int offX, int offY)
	{
		m_pSkillFollow = p_skill;
		m_SkillFollowOffX = offX;
		m_SkillFollowOffY = offY;
	}
	void	UpdateSkillFollowPosition();

	// 2026-09-03 minimap titreme fix — takipci skill butonu, minimap
	// suruklenirken pencereden-pencereye statch taramasinda yok sayilir
	// (u_window.h GetStatchIgnoreWindow aciklamasi; MMPOS log'undaki
	// STATCH_VALUE'luk testere disi salinimin root cause'u).
	Window* GetStatchIgnoreWindow() { return m_pSkillFollow; }

	void SetBlock(int x,int y);
	C_VS_UI_MINIMAP();
	~C_VS_UI_MINIMAP();

// 	static	int GetZoneNumber(int zone_id);
// 	static	char * GetZoneName(int zone_id);

	int		GetZoneID()							{ return m_zone_id; }
	void	SetZone(int zone_id);
	void	SetSize(SIZE size) { m_map_w = size.cx; m_map_h = size.cy; }
	void	SetPortal(RECT rect, int id);
	void	SetNPC(MINIMAP_NPC npc);
	void	ClearNPC() { m_npc.clear(); }
	void	SetSafetyZone(RECT rect, bool my_zone);
	void	SetFlagArea(POINT pt);
	void	SetXY(int x, int y)	{ m_map_x = x; m_map_y = y; }
	int		GetX()	{ return m_map_x; }
	int		GetY()	{ return m_map_y; }
	bool	m_bl_slot_hide;
	void	TogglePushPin() { Run(PUSHPIN_ID); }

	void	UnacquireMouseFocus() 
	{
		m_pC_button_group->UnacquireMouseFocus();
	}
	void	CancelPushState() 
	{
		m_pC_button_group->CancelPushState();
	}
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);
	void	AcquireDisappear() {}
	void	Run(id_t id);
	bool	MouseControl(UINT message, int _x, int _y);
	void	MouseControlExtra(UINT message, int _x, int _y);
#if __CONTENTS(__MOVE_ZONE)
	bool	SetMoveZone(int iPox_X, int iPox_Y);
#endif //__MOVE_ZONE
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	Show();
	void	Process();
	void	Start();
	void	Finish();
	
	void	ResetBoardPosition();

private:
#if __CONTENTS(__GPS_ADD)
	void	GPSRun();	
#endif //__GPS_ADD
};


//�̳ǽ���
class C_VS_UI_TMALL : public Window, public Exec, public ButtonVisual
{
public:
	C_VS_UI_TMALL();
	~C_VS_UI_TMALL();

	struct ItemList 
	{
		MItem*	pItem;
		DWORD	TimeLimit;
		int		num;
		int		nDianJuan;//���
		int		nPoint;//����
	};

	enum MILITARY_SPK
	{
		TMall_Background = 0,
		TMall_GridBackgound,

		TMall_FangJuNormal,
		TMall_FangJuDown,

		TMall_ShouShiNormal,
		TMall_ShowShiDown,

		TMall_BaoShiNormal,
		TMall_BaoShiDown,

		TMall_TeXiaoNormal,
		TMall_TeXiaoDown,

		TMall_ChongWuNormal,
		TMall_ChongwuDown,	

		TMall_JiFenBuyNormal,
		TMall_JiFenBuyDown,

		TMall_BuyNormal,
		TMall_BuyDown,

		TMall_BtnLeftArrow,
		TMall_BtnRightArrow,

		TMall_BtnIndexNormal,
		TMall_BtnIndexDown,

		TMall_BtnCloseNormal,
		TMall_BtnCloseDOwn,

		TMall_BtnChongZhiNormal,
		TMall_BtnChongZhiDown,
		
		TMall_BtnShangChenNormal,
		TMall_BtnShangChenDown,
	};

	enum BUTTON_ID
	{
		Button_FangJu = 0,
		Button_ShouShi,
		Button_BaoShi,
		Button_TeXiao,
		Button_ChongWu,
		Button_JiFenBuy,

		Button_Buy_0,
		Button_Buy_1,
		Button_Buy_2,
		Button_Buy_3,
		Button_Buy_4,
		Button_Buy_5,
		Button_Buy_6,
		Button_Buy_7,
		Button_Buy_8,

		Button_LeftArrow,
		Button_RightArrow,

		Button_Index_1,
		Button_Index_2,
		Button_Index_3,

		Button_Close,

		Button_ChongZhi,
	};

	enum EDEFINE
	{
		PageSize = 9,
		// 2026-08-19: sekme basina sayfa sayisi (3 -> 20). Sag-ok clamp'i ve "Page X / 20" etiketi buna bagli.
		TMALL_PAGE_COUNT = 20,
	};


	C_SPRITE_PACK			*m_pC_TMall_spk;

	ButtonGroup *			m_pC_button_group;

	int		m_nClassIndex;
	int		m_nPageIndex;

	int								m_slot_size;
	Rect							m_slot_rect[9];

	std::vector<ItemList>			m_ItemList;

	C_VS_UI_DIALOG	*			m_pC_dialog_buy_confirm;


public: 
	bool	IsPixel(int _x, int _y);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);

	virtual bool MouseControl(UINT message, int _x, int _y);
	virtual void KeyboardControl(UINT message, UINT key, long extra);

	void    ToggleTMall();
	bool	AddItem( C_VS_UI_TMALL::ItemList pItem );
	bool	DeleteItem( DWORD ID );
	void	ClearItem();
	void    BuyItem(int nIndex);

	void	Run(id_t id);
	void	Show();
	void    ShowItemListIcon();
	void    ShowItemListText();
	void	Process();
	void	Start();
	void	Finish();

protected:

};


//ʥս����
class C_VS_UI_HOLYWAR : public Window, public Exec, public ButtonVisual
{
public:
	C_VS_UI_HOLYWAR();
	~C_VS_UI_HOLYWAR();

	struct PlayerScore 
	{
		std::string strName;
		int		nKill;
		int		nDead;
		int		nScore;
	};

	struct PlayerData
	{
		int 	KillCount;
		int 	DeadCount;
		int 	RacePoint;
		int 	KillPoint;
		int 	TotalPoint;
	};

	struct RaceData
	{
		int nKill;
		int nDead;
		int nScore;
        std::vector<int> BookIDList;
	};

	enum C_VS_UI_HOLYWAR_SPK
	{
		HolyWar_Back = 0,
		HolyWar_S_Light,
		HolyWar_V_Light,
		HolyWar_O_Light,
		HolyWar_CloseBtnNormal,
		HolyWar_CloseBtnDown,
	};

	enum BUTTON_ID
	{
		Button_Close,
	};




	C_SPRITE_PACK			 *m_pC_HollyWar_spk;

	ButtonGroup *			 m_pC_button_group;

	Rect				     m_rcPersonScore[RACE_MAX];
	Rect					 m_rcPlayerData;
	Rect					 m_rcRaceRect[RACE_MAX];
	RaceData				 m_stRaceData[RACE_MAX];
	std::vector<PlayerScore> m_ScoreList[RACE_MAX];
	std::string				 m_vRaceName[RACE_MAX];
	int						 m_colorRace[RACE_MAX];
	int						 m_nRaceIndex;

	PlayerData				 m_stPlayerData;

public:
	void    ClearAllData();
	void    AddHollyScore(PlayerScore score, int nRace);
	void    AddHollyRace(C_VS_UI_HOLYWAR::RaceData * data);
	void    AddPlayerData(C_VS_UI_HOLYWAR::PlayerData data);

public:
	bool	IsPixel(int _x, int _y);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);

	virtual bool MouseControl(UINT message, int _x, int _y);
	virtual void KeyboardControl(UINT message, UINT key, long extra);

	void	Run(id_t id);
	void	Show();
	void    ShowRaceData();
	void    ShowBook();
	void    ShowScore(int nRace);
	void    ShowSelfData();
	void	Process();
	void	Start();
	void	Finish();
};



//�׼���ʯ����
class C_VS_UI_MILITARYDLG : public Window, public Exec, public ButtonVisual
{
public:
	C_VS_UI_MILITARYDLG();
	~C_VS_UI_MILITARYDLG();

	enum MILITARY_SPK
	{
		Window_0_Background = 0,
		Window_1_Background,
	    Window_2_Background,
		Progress_Background,
		Progress_UpShow,

		SanJiao_Red,
		SanJiao_Blue,
		SanJiao_Green,
		SanJiao_Yellow,
		SanJiao_Gray,

		ZhengFang_Red,
		ZhengFang_Blue,
		ZhengFang_Green,
		ZhengFang_Yellow,
		ZhengFang_Gray,

		WuBian_Red,
		WuBian_Blue,
		WuBian_Green,
		WuBian_Yellow,
		WuBian_Gray,

		LiuBian_Red,
		LiuBian_Blue,
		LiuBian_Green,
		LiuBian_Yellow,
		LiuBian_Gray,

		Slot_Lock,

		Window_Full_Background,

		TableBtn_Normal,
		TableBtn_Press,
		TableBtn_Down,

		Btn_Close,

		None_Show = -1,
	};

	enum Skill_New_SPK
	{
		 
		Window_Background= 0,
		Window_Slayer_Background,
		Window_Vamprie_Background,
		Window_Ouster_Background,

		BUTTON_1_1,
		BUTTON_1_2,
		BUTTON_1_3,

		BUTTON_2_1,
		BUTTON_2_2,
		BUTTON_2_3,

		ICON_BLADE,
		ICON_SWORD,
		ICON_GUN,
		ICON_HEAL,

		ICON_ENCHANT,
		ICON_MO_ZHAN,
		ICON_MO_FIRE,
		ICON_MO_WATER,
		ICON_MO_EARTH,

		CLOSE_BIG_1_1,
		CLOSE_BIG_1_2,
		CLOSE_BIG_1_3,
		SKILL_SLOT,
		CLOSE_SMALL_1_1,
		CLOSE_SMALL_1_2,
		CLOSE_SMALL_1_3,

		BUTTON_UP_1_1,
		BUTTON_UP_1_2,
		BUTTON_UP_1_3,
		BUTTON_DOWN_1_1,
		BUTTON_DOWN_1_2,
		BUTTON_DOWN_1_3,

		SKILL_SHOW,
		SKILL_LEVEL,

		OUSTER_ZHAN_SKILL,
		OUSTER_MOFA_SKILL,
	};
	enum BUTTON_ID
	{
		Button_Tab0,
		Button_Tab1,
		Button_Tab2,
		Button_Close,
		Button_Skill,
		Button_RankGem,
		Button_Guild,
		Button_Blade,
		Button_Sword,
		Button_Gun,
		Button_Heal,
		Button_Enchant,
		Button_mozhan,
		Button_mofa,
		Button_Up,
		Button_Down,
		Button_Vampire_adv_skill_1,
		Button_Vampire_adv_skill_2,
		Button_Vampire_adv_skill_3,
		Button_Vampire_adv_skill_4,
		Button_Vampire_adv_skill_5,
		Button_Vampire_adv_skill_6,
	};

	//const int OusterSkill[30][3];
	//const int OusterElementalSkill[44][3];

	C_SPRITE_PACK			*m_pC_Military_spk;
	C_SPRITE_PACK			*m_pC_Skill_spk;
	C_VS_UI_DIALOG			*m_pC_learn_grade_skill_confirm;

	ButtonGroup *			m_pC_button_group;
	ButtonGroup *			m_pC_button_ui_group;
	ButtonGroup *			m_pC_button_slayer_skill_group;
	ButtonGroup *			m_pC_button_slayer_ouster_group;
	ButtonGroup *			m_pC_button_mofa_skill_group;
	ButtonGroup *			m_pC_button_adv_group;

	Rect				    m_rcSlot[MilitarySlot::L_MAX];
	int						m_SlotPicIndex[MilitarySlot::L_MAX];
	bool					m_bSlotOpenState[MilitarySlot::L_MAX];

	Rect					m_rcPagePos;

	int						m_nPageIndex;
	int						m_focus_slot;
	int						m_ui_index;
	int						m_slayer_job_index;
	int						m_ouster_job_index;

	C_VS_UI_DIALOG*			m_pC_dialog_buy_confirm;
	bool					b_showRankGem;
	bool					b_showSkillUi;
	bool					b_showSword;
	bool					b_showMozhan;

	int						mozhan_gap;
	int						button_time;
	int						m_focused_skill_id;
	SKILLDOMAIN				m_skill_domain;
	MSkillDomain::SKILL_STEP_LIST	m_advance_skill_id_vec;		// ȭ�鿡 ǥ�õǴ� ���� ��ų ���̵� �����Ѵ�.
	MSkillDomain::SKILL_STEP_LIST	m_rare_skill_id_vec;		// ȭ�鿡 ǥ�õǴ� ���� ��ų ���̵� �����Ѵ�.

	// Faz A (2026-08-27): advance/rare liste rebuild yalnız dirty iken (her frame değil).
	bool					m_advanceSkillListDirty;
	bool					m_rareSkillListDirty;

	C_VS_UI_SCROLL_BAR *	m_pC_grade3_scroll_bar;
	C_VS_UI_SCROLL_BAR *	m_pC_rare_skill_scroll_bar;

	static int				m_selected_grade_skill;
	static int				m_selected_skill_domain;
	static int				m_ousters_Magic;
	static int				m_vampire_ACType;

	static int				m_selected_ACSkillID;

	bool					m_bOustersDownSkill;

public:
	bool	IsPixel(int _x, int _y);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);

	void    UpdateMilitarySlotData(int nSlotID, int nType, int nGrade);

	void    ToggleMilitary();
	int     GetIndexSharpe(int indexID);

	void    ShowMilitarySlotItem();

	bool    Click(int window_x, int window_y, Rect * slot_rect);
	bool	SkillInfoMouseControl(UINT message, int _x, int _y);
	bool	SkillLearnMouseControl(UINT message, int _x, int _y);

	virtual bool MouseControl(UINT message, int _x, int _y);
	virtual void KeyboardControl(UINT message, UINT key, long extra);

	void	Run(id_t id);
	void	Show();
	void	Process();
	void	Start();
	void	Finish();
	void	SetOustersDownSkill() { m_bOustersDownSkill = true; }
	void	ACSkillList();
	void	ACSkillShow();
	void    ShowRareSkillList();
	void    ProcessRareSkillList();
	// Advance + rare skill listelerini bir sonraki Process'te yeniden kur.
	void	InvalidateSkillLists();
	bool    RareSkillInfoMouseControl(UINT message, int _x, int _y);
	//void    GetMilitaryWindow() const { return m_pc_military; }
	//void	CloseMilitary() { m_pc_military->Finish(); }
	//void	OpenMilitary(bool bl_set_load = true)	{ if ( m_pc_military != NULL ) m_pc_military->Start(); }
	//void    UpdateMilitarySlotData(int nSlotID, int nType, int nGrade)
	//{
	//	if ( !m_pc_military )
	//		return ;

	//	m_pc_military->UpdateMilitarySlotData(nSlotID, nType, nGrade);
	//}
};
//��ֵ�����

class C_VS_UI_PAYEVENT : public Window, public Exec, public ButtonVisual
{
public:
	C_VS_UI_PAYEVENT();
	~C_VS_UI_PAYEVENT();


	enum LEVEL_PRICE
	{
		LEVEL_20,
		LEVEL_50,
		LEVEL_100,
		LEVEL_200,

		LEVEL_500,
		LEVEL_1000,
		LEVEL_2000,
		LEVEL_5000,
		LEVEL_MAX
	};

	enum PAYEVENT_SPK
	{
		PAYEVENT_Background = 0,
		PAYEVENT_GridBackgound,

		PAYEVENT_ItemGridFrame,

		PAYEVENT_UnFinished,
		PAYEVENT_GetItem,
		PAYEVENT_GetItemDown,
		PAYEVENT_Finished,

		PAYEVENT_Up,
		PAYEVENT_Up_D,
		PAYEVENT_Down,
		PAYEVENT_Down_D,

		PAYEVENT_Close,
		PAYEVENT_Close_D,
		PAYEVENT_ICON,
		PAYEVENT_ICON_D,
	};

	enum BUTTON_ID
	{
		Button_0,
		Button_1,
		Button_2,
		Button_3,

		Button_Up,
		Button_Down,
		Button_Close,
		Button_open,
	};

	typedef struct 
	{
		MItem*	pItem;
		int     nLevel;
		int		num;
		int		ImagID;//ͼ������
	}ItemData;

	typedef std::vector<ItemData> VEC_ITEM_DATA;

	typedef struct
	{
		ItemData itemData[3];
		int nCount;
		int nStautus;
	}PriceLevelData;


	C_SPRITE_PACK			*m_pC_PayEvent_spk;
	C_SPRITE_PACK			*m_pC_Give_item_spk;
	ButtonGroup *			m_pC_button_group;

	int						m_nLineIndex;

	int						m_slot_size;
	bool					bShow;
	Rect					m_slot_rect[12];

	POINT					m_ptCountMax[4];
	POINT					m_ptPrice[4];
	int						m_PriceMax[LEVEL_MAX];
	string					m_strPrice[LEVEL_MAX];
	PriceLevelData			m_arrPriceLvData[LEVEL_MAX];

	C_VS_UI_DIALOG	*		m_pC_dialog_buy_confirm;


public: 
	bool	IsPixel(int _x, int _y);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);

	virtual bool MouseControl(UINT message, int _x, int _y);
	virtual void KeyboardControl(UINT message, UINT key, long extra);

	void    TogglePayEvent();
	void    FillPriceData();

	void    SetCount(int nLevel, int nCount);
	void    SetStautus(int nLevel, int nStautus);

	bool	AddItem( C_VS_UI_PAYEVENT::ItemData pItem );
	void    OnGetPrice(int nIndex);

	void	Run(id_t id);
	void	Show();
	void	Show2();
	void    ShowItemListIcon();
	void    ShowItemListText();
	void	Process();
	void	Start();
	void	Finish();

protected:

};
//class C_VS_UI_SIMPLEINFOMATION : public Window, public Exec, public ButtonVisual
//{
//public:
//	C_VS_UI_SIMPLEINFOMATION();
//	~C_VS_UI_SIMPLEINFOMATION();
//
//	enum SIMPLEINFOMATION_SPK
//	{
//		EXP_BAR_BACKGROUD = 0,
//		EXP_BAR_BLUE,
//
//		SIMPLEINFOMATION_ALPHA_ID,
//		SIMPLEINFOMATION_ALPHA_ID_DOWN,
//		SIMPLEINFOMATION_HIDE,
//		SIMPLEINFOMATION_HIDE_DOWN,
//
//		SIMPLEINFOMATION_TIME_BACKGROUD,
//		SIMPLEINFOMATION_TIME_SUN,
//		SIMPLEINFOMATION_TIME_MOON,
//		SIMPLEINFOMATION_SUN,
//		SIMPLEINFOMATION_MOON,
//
//		SIMPLEINFOMATION_ATTACK,
//		SIMPLEINFOMATION_DEFS,
//
//		SIMPLEINFOMATION_BLADE,
//		SIMPLEINFOMATION_SWORD,
//		SIMPLEINFOMATION_GUN,
//		SIMPLEINFOMATION_HEALTH,
//		SIMPLEINFOMATION_ENCHANT,
//
//		SIMPLEINFOMATION_BLADE_LEARN,
//		SIMPLEINFOMATION_SWORD_LEARN,
//		SIMPLEINFOMATION_GUN_LEARN,
//		SIMPLEINFOMATION_HEALTH_LEARN,
//		SIMPLEINFOMATION_ENCHANT_LEARN,
//		SIMPLEINFOMATION_JOB_CHOOSE,
//
//		SIMPLEINFOMATION_EARTH,
//		SIMPLEINFOMATION_WATER,
//		SIMPLEINFOMATION_FIRE,
//
//		SIMPLEINFOMATION_EARTH_BAR,
//		SIMPLEINFOMATION_WATER_BAR,
//		SIMPLEINFOMATION_FIRE_BAR,
//
//		SIMPLEINFOMATION_MAIN,
//		SIMPLEINFOMATION_OUSTER_BAR,
//
//
//	};
//
//	enum BUTTON_ID
//	{
//		BUTTON_HIDE,
//	};
//
//	C_SPRITE_PACK			*m_pC_SimpleInforMation_spk;
//	ButtonGroup *			m_pC_button_group;
//
//public: 
//	bool	IsPixel(int _x, int _y);
//	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
//	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
//
//	virtual bool MouseControl(UINT message, int _x, int _y);
//	virtual void KeyboardControl(UINT message, UINT key, long extra);
//
//	void	Run(id_t id);
//	void	Show();
//	void	Process();
//	void	Start();
//	void	Finish();
//
//protected:
//
//};
//
//class C_VS_UI_GAMEHOOK : public Window, public Exec, public ButtonVisual
//{
//public:
//	C_VS_UI_GAMEHOOK();
//	~C_VS_UI_GAMEHOOK();
//
//	enum HOOK_BUTTON
//	{
//		HOOK_BUTTON_GREEN_NORMAL,
//		HOOK_BUTTON_GREEN_PRESS,
//		HOOK_BUTTON_RED_NORMAL,
//		HOOK_BUTTON_RED_PRESS,
//		HOOK_BUTTON_BLUE_NORMAL,
//		HOOK_BUTTON_BLUE_PRESS,
//		HOOK_BUTTON_SMALLGREEN_NORMAL,
//		HOOK_BUTTON_SMALLGREEN_PRESS,
//		HOOK_BUTTON_SMALLRED_NORMAL,
//		HOOK_BUTTON_SMALLRED_PRESS,
//		HOOK_BUTTON_CLOSE_NORMAL,
//		HOOK_BUTTON_CLOSE_PRESS,
//		HOOK_BUTTON_CKECKBOX_FALSE,
//		HOOK_BUTTON_CHECKBOX_TRUE,
//
//	};
//
//	enum HOOK_DIALOG
//	{
//		HOOK_DIALOG_BACKGROUND,
//	};
//
//	enum BUTTON_ID
//	{
//		START_HOOK,
//		STOP_HOOK,
//		CLOSE_HOOK,
//	};
//
//	enum LINE_EDITEKEY_ID
//	{
//		LV_ATTACK_KEY,
//
//		LV_HP_KEY,
//		LV_HP_PECENT,
//		LV_HP_TIME,
//
//		LV_MP_KEY,
//		LV_MP_PECENT,
//		LV_MP_TIME,
//
//		LV_BULLET_KEY,
//		LV_BULLET_TIME,
//
//		LV_EFFECT_KEY1,
//		LV_EFFECT_TIME1,
//
//		LV_EFFECT_KEY2,
//		LV_EFFECT_TIME2,
//
//		LV_EFFECT_KEY3,
//		LV_EFFECT_TIME3,
//
//		LV_EFFECT_KEY4,
//		LV_EFFECT_TIME4,
//
//		LV_EFFECT_KEY5,
//		LV_EFFECT_TIME5,
//
//		LV_EFFECT_KEY6,
//		LV_EFFECT_TIME6,
//
//		LV_MAX,
//	};
//
//	C_SPRITE_PACK			*m_pC_hook_button_spk;
//	C_SPRITE_PACK			*m_pC_hook_dialog_spk;
//
//	ButtonGroup *			m_pC_button_group;
//
//	LineEditorVisual		m_lvKey[LV_MAX];
//
//protected:
//	void AddLineEditor(LineEditorVisual& kEditor, int x, int y);
//	void DrawRect(int left, int top, int right, int bottom);
//
//public:
//	bool	IsPixel(int _x, int _y);
//	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
//	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
//
//	virtual bool MouseControl(UINT message, int _x, int _y);
//	virtual void KeyboardControl(UINT message, UINT key, long extra);
//
//	void	Run(id_t id);
//	void	Show();
//	void	Process();
//	void	Start();
//	void	Finish();
//};



#if __CONTENTS(__JAPAN_UI)
#define MARK_MAX 3
#else
#define MARK_MAX 6
#endif //__JAPAN_UI
extern char g_mark[MARK_MAX][9][3];

#define BLANK_TIMER 500
//-----------------------------------------------------------------------------
// class C_VS_UI_QUEST_STATUS
//
//
//-----------------------------------------------------------------------------
class C_VS_UI_QUEST_STATUS : public Window, public Exec, public ButtonVisual
{
public :

	struct QUEST_STATUS
	{
		char*			Title;
		DWORD			QuestID;
		DWORD			current_point;
		DWORD			quest_time;
	};
	
	QUEST_STATUS		m_quest_status;					// ���� �������� ����Ʈ ����
	bool				m_bl_active;
	bool				m_bl_timeover;
	bool				m_bl_focus;
	
	DWORD				m_timer;
	DWORD				m_timer2;
	CSpriteSurface *	m_p_back_surface;
	std::vector<std::string>					m_hard_cording;		// �� ������!!!


private :

	ButtonGroup			*m_pC_button_group;

	enum EXEC_ID
	{
		PUSHPIN_ID,
		ALPHA_ID,
		CLOSE_ID,
		HELP_ID,
		
		DETAIL_ID,
	};

public :
	void IncreaseQuestPoint();

	C_VS_UI_QUEST_STATUS();
	~C_VS_UI_QUEST_STATUS();
	
	
	DWORD	GetQuestStatusID() { return m_quest_status.QuestID;}
	void	SetQuestStatusInit();
	void	SetQuestStatus(QUEST_STATUS &status);
	void	ToggleWindow();
	void	ShowQuestDescription(int _x, int _y);

	void	Show();
	void	Start();
	void	Finish();
	void	Process();
	void	Run(id_t id);
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	std::string		GetDetailInfo();

	bool	IsPixel(int _x,int _y);
	int		Timer(int time = -1);
	int		Timer2(int time = -1);

	void	ShowDesc(int strX,int strY,const char *str);

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
};



class C_VS_UI_TRIBE : public Window, public HotKey, public Exec, public ButtonVisual
{
public :

	enum	HOTKEYTYPE
	{
			QUICKITEM_TYPE ,
			SKILL_TYPE ,
			NOT_SELECT_TYPE ,
	} ; 

protected:

	// renewal by chyaya

	//C_VS_UI_MARKET_ACCOUNT*		m_pC_Market;

	

	//C_VS_UI_QUEST_STATUS *		m_pC_quest;


	enum MAIN_SPK_INDEX	// by larosel
	{
		//MAIN_WINDOW,
		//ICON_SUN,
		//ICON_MOON,

		//EXP_BACK,
		//EXP_BAR,
		//
		//SLAYER_SKILL_EXP_BAR,		// For Slayer Only

		EXP_BAR_BACKGROUD = 0,
		EXP_BAR_BLUE,

		SIMPLEINFOMATION_ALPHA_ID,
		SIMPLEINFOMATION_ALPHA_ID_DOWN,
		SIMPLEINFOMATION_HIDE,
		SIMPLEINFOMATION_HIDE_DOWN,

		SIMPLEINFOMATION_TIME_BACKGROUD,
		SIMPLEINFOMATION_TIME_SUN,
		SIMPLEINFOMATION_TIME_MOON,
		SIMPLEINFOMATION_SUN,
		SIMPLEINFOMATION_MOON,

		SIMPLEINFOMATION_ATTACK,
		SIMPLEINFOMATION_DEFS,

		SIMPLEINFOMATION_BLADE,
		SIMPLEINFOMATION_SWORD,
		SIMPLEINFOMATION_GUN,
		SIMPLEINFOMATION_HEALTH,
		SIMPLEINFOMATION_ENCHANT,

		SIMPLEINFOMATION_BLADE_LEARN,
		SIMPLEINFOMATION_SWORD_LEARN,
		SIMPLEINFOMATION_GUN_LEARN,
		SIMPLEINFOMATION_HEALTH_LEARN,
		SIMPLEINFOMATION_ENCHANT_LEARN,
		SIMPLEINFOMATION_JOB_CHOOSE,

		SIMPLEINFOMATION_EARTH,
		SIMPLEINFOMATION_WATER,
		SIMPLEINFOMATION_FIRE,

		SIMPLEINFOMATION_EARTH_BAR,
		SIMPLEINFOMATION_WATER_BAR,
		SIMPLEINFOMATION_FIRE_BAR,

		SIMPLEINFOMATION_MAIN,
		SIMPLEINFOMATION_OUSTER_BAR,


	};

	enum SYSTEM_BUTTON_INDEX
	{
		SUB_MENU_BOTTOM,
		SUB_MENU_BUTTON_NORMAL,
		SUB_MENU_BUTTON_HILIGHTED,
		SUB_MENU_BUTTON_PUSHED,
		SUB_MENU_BUTTON_DISABLE,

		TAB_INFO_NORMAL,
		TAB_INFO_HILIGHTED,
		TAB_INFO_PUSHED,
		TAB_INFO_DISABLE,
		TAB_GUILD_NORMAL,
		TAB_GUILD_HILIGHTED,
		TAB_GUILD_PUSHED,
		TAB_GUILD_DISABLE,
		TAB_COMMUNITY_NORMAL,
		TAB_COMMUNITY_HILIGHTED,
		TAB_COMMUNITY_PUSHED,
		TAB_COMMUNITY_DISABLE,
		TAB_SHOP_NORMAL,
		TAB_SHOP_HILIGHTED,
		TAB_SHOP_PUSHED,
		TAB_SHOP_DISABLE,
		TAB_HELP_NORMAL,
		TAB_HELP_HILIGHTED,
		TAB_HELP_PUSHED,
		TAB_HELP_DISABLE,

		TAB_SYSTEM_NORMAL,
		TAB_SYSTEM_HILIGHTED,
		TAB_SYSTEM_PUSHED,
		TAB_SYSTEM_DISABLE,
		
	};

	enum EXEC_INDEX	// by larosel
	{
		// COMMON
		PUSHPIN_ID,
		SYSTEM_ID,
		MENU_INFO_ID,
		MENU_GUILD_ID,
		MENU_COMMUNITY_ID,
		MENU_SHOP_ID,
		MENU_HELP_ID,

		// MENU TAB
		INVENTORY_ID,
		MILITARY_ID,
		GEAR_ID,
		INFO_ID,
		PARTY_ID,
		QUEST_ID,
		MAIL_ID,
#if __CONTENTS(__SMS_SERVICES)
		SMS_ID,
#endif
		NAMING_ID,
		
		// TEAM TAB
		TEAM_INFO_ID,
		TEAM_MEMBER_LIST_ID,
//		TEAM_COMMAND_ID, // ��� ����
		TEAM_LIST_ID, // ��� ����Ʈ
		TEAM_WAIT_LIST_ID, // ��� ��� ����Ʈ
		TEAM_UNION_ID,		// ���� ����

		// UTIL TAB
		UTIL_STORE_ID,
//#if __CONTENTS(__POWER_JJANG_ID)
//		UTIL_POWER_JJANG_ID,
//#endif
#if __CONTENTS(__DARKEDEN_MARKET)
		UTIL_MARKETM_ID, 
#endif //__DARKEDEN_MARKET
		UTIL_MARKETB_ID,

#if __CONTENTS(__FRIEND_ADDITION)
		UTIL_FRIEND_SYSTEM,
		UTIL_FRIEND_WAIT,
#endif //__FRIEND_ADDITION	

		
		// HELP TAB
		HELP_ID,
		CHAT_HELP_ID,
		BATTLE_HELP_ID,
		SKILL_HELP_ID,
		GUILD_HELP_ID,
		WORLD_MAP_HELP_ID,
#if __CONTENTS(__USER_GRADE)
		USER_GRADE,
#endif
		EXEC_MAX
	};


	C_VS_UI_SKILL	*			m_pC_skill;
	C_VS_UI_LEVELUP *			m_pC_level_up;
	Point						m_backup_inventory_xy, m_backup_gear_xy;
	//ButtonGroup *				m_pC_hide_button;
	enum { MENU_BUTTON_GROUP_COUNT = MENU_HELP_ID - MENU_INFO_ID + 1 };
	//ButtonGroup *				m_pC_menu_button_groups[MENU_BUTTON_GROUP_COUNT];11


	C_VS_UI_HPBAR *				m_pC_hpbar;
	C_VS_UI_BLOOD_BURST*		m_pC_BloodBurst;

	C_VS_UI_SHORTCUT_SLOT*		m_pC_ShortCutSlot;

	C_VS_UI_EFFECT_STATUS *		m_pC_effect_status;
	C_VS_UI_MINIMAP *			m_pC_minimap;
	//C_VS_UI_GAMEHOOK*			m_pc_gamehook;
	C_VS_UI_MILITARYDLG*		m_pc_military;
	C_VS_UI_TMALL*				m_pc_tmall;
	C_VS_UI_HOLYWAR*			m_pc_hollywar;
	C_VS_UI_PAYEVENT*			m_pc_payevent;
	C_VS_UI_CHATTING *			m_pC_chatting;
	C_VS_UI_SLAYER_QUICKITEM *	m_pC_quickitem;
	C_VS_UI_SLAYER_QUICKITEM *  m_pC_quickitemV;
	C_VS_UI_OUSTERS_QUICKITEM * m_pC_armsband;
	C_VS_UI_QUEST_STATUS *		m_pC_quest_status;
	C_VS_UI_INVENTORY *			m_pC_inventory;
	C_SPRITE_PACK *				m_pC_main_spk;
//	C_SPRITE_PACK * 			m_pC_sys_button_spk;

//	C_VS_UI_SIMPLEINFOMATION*   m_pC_SimpleInforMation;
	std::string m_time;
	std::string m_date;

	int							m_selected_menu;

//	bool						m_bl_help, m_bl_party;

//	bool						m_bl_hide;

	//timer
	DWORD						m_dw_prev_tickcount;
	DWORD						m_dw_millisec;


	id_t						m_OpenedHelpIdx;
	int							m_HotKey_Type[12] ; 

	bool	Timer();

public:
	void	MouseControlExtra(UINT message, int _x, int _y) 
	{
		if(m_pC_minimap != NULL)
		{
			m_pC_minimap->IsInRect(_x, _y);
			m_pC_minimap->MouseControlExtra(message, _x, _y); 
		}
	}
	void	ResetOpenedHelpIdx()		{ m_OpenedHelpIdx = EXEC_MAX;	}

	//���ڼ�ġ
	void	StartInstallMineProgress(int focus_grid_x, int focus_grid_y)
	{ if(m_pC_inventory)m_pC_inventory->StartInstallMineProgress(focus_grid_x, focus_grid_y); }
	bool	IsInstallMineProgress()		{ return gbl_mine_progress; }
	void	EndInstallMineProgress()	{ gbl_mine_progress = false; }
	//���ڸ����
	void	StartCreateMineProgress(int focus_grid_x, int focus_grid_y)
	{ if(m_pC_inventory)m_pC_inventory->StartCreateMineProgress(focus_grid_x, focus_grid_y); }
	bool	IsCreateMineProgress()		{ return gbl_mine_progress; }
	void	EndCreateMineProgress()	{ gbl_mine_progress = false; }
	//��ź �����
	void	StartCreateBombProgress(int focus_grid_x, int focus_grid_y)
	{ if(m_pC_inventory)m_pC_inventory->StartCreateBombProgress(focus_grid_x, focus_grid_y); }
	bool	IsCreateBombProgress()		{ return gbl_mine_progress; }
	void	EndCreateBombProgress()	{ gbl_mine_progress = false; }

	//���� Progress Bar
	void	StartSkillCastingProgress(int skillID, DWORD time, CPoint inventoryGrid = CPoint(-1, -1))
	{ if(m_pC_skill) m_pC_skill->StartSkillCastingProgress(skillID, time, inventoryGrid); }
	bool	IsSkillCastingProgress()
	{ return m_pC_skill && m_pC_skill->IsSkillCastingProgress();	}
	void	EndSkillCastingProgress()
	{ if(m_pC_skill) m_pC_skill->EndSkillCastingProgress();			}


	int		GetSelectedMenu() { return m_selected_menu; }
	void	SetSelectMenu(int menu);

	void	SetHotkey(C_VS_UI_SKILL::HOTKEY hotkey, ACTIONINFO id)
	{
		GetSkillWindow()->SetHotkey(hotkey, id);
	}

	void	ResetSkillSet()
	{
		if(m_pC_skill!=NULL)
			m_pC_skill->ResetSkillSet();
	}
	
	int		GetSelectedSkillID()
	{
		if(m_pC_skill!=NULL)
			return m_pC_skill->GetSelectedSkillID();
		else return -1 ;
	}

	int	GetHotkey(C_VS_UI_SKILL::HOTKEY hotkey, C_VS_UI_SKILL::HOTKEY_GRADE grade) const
	{
		return GetSkillWindow()->GetHotkey(hotkey, grade);
	}
	int	GetHotkeyType(C_VS_UI_SKILL::HOTKEY hotkey) const
	{
		return m_HotKey_Type[hotkey];
	}
	
	void	SetComboCnt(int ComboCnt)
	{
		if(m_pC_skill!=NULL)
			m_pC_skill->SetComboCnt(ComboCnt);
	}
		
	void	SetSelectedAttackComboSkill(bool	bAttackSkill)
	{
		if(m_pC_skill!=NULL)
			m_pC_skill->SetSelectedAttackComboSkill(bAttackSkill);
	}
	
	//void	SetGage()
	//{
	//	if(m_pC_BloodBurst!=NULL)
	//		m_pC_BloodBurst->SetGage()	;
	//}
	
	void	SetBloodBurstAttackGage()
	{
		if(m_pC_BloodBurst!=NULL)
			m_pC_BloodBurst->SetAttackGage();
	}

	void	SetBloodBurstDefenseGage()
	{
		if(m_pC_BloodBurst!=NULL)
			m_pC_BloodBurst->SetDefenseGage();
	}

	void	SetBloodBurstPartyGage()
	{
		if(m_pC_BloodBurst!=NULL)
			m_pC_BloodBurst->SetPartyGage();
	}


	bool	GetBloodBurstAttackGageFull()
	{
		if(m_pC_BloodBurst!=NULL)
			return	m_pC_BloodBurst->GetAttackGageFull();
		return false ;
	}

	bool	GetBloodBurstDefenseGageFull()
	{
		if(m_pC_BloodBurst!=NULL)
			return	m_pC_BloodBurst->GetDefenseGageFull();
		return false ;
	}

	bool	GetBloodBurstPartyGageFull()
	{
		if(m_pC_BloodBurst!=NULL)
			return	m_pC_BloodBurst->GetPartyGageFull();
		return false ;
	}

	void	SetTime(const char *str)			{ m_time = str; }
	void	SetDate(const char *str)			{ m_date = str; }
	void	ToggleMinimap()						{ m_pC_minimap->TogglePushPin(); }

	virtual void	RunQuickItemSlot() = 0;
	virtual void	CloseQuickItemSlot() = 0;
	virtual bool	IsRunningQuickItemSlot() = 0;
	void	RunMinimap() {  }
	void	CloseMinimap() {  }
	bool	IsRunningMinimap() { return false; }
	void	RunMark()		{  }
	void	CloseMark()		{  }
	bool	IsRunningMark()	{ return false; }
	void	SetWhisperID(char *id)	{ if(m_pC_chatting)m_pC_chatting->SetWhisperID(id); }

	bool	IsRunningQuestStatusWindow() { if(m_pC_quest_status) return m_pC_quest_status->Running(); return false;}
	void	OpenQuestStatusWindow() { if(!m_pC_quest_status->Running()) m_pC_quest_status->Start(); }
	void	CloseQuestStatusWindow() { if(m_pC_quest_status->Running()) m_pC_quest_status->Finish(); }
	void	SetQuestStatusWindow(C_VS_UI_QUEST_STATUS::QUEST_STATUS& status)	{ if(m_pC_quest_status) m_pC_quest_status->SetQuestStatus( status ); }
	DWORD	GetQuestStatusID()	{ 
		if(m_pC_quest_status) return m_pC_quest_status->GetQuestStatusID( ); 
		else return 0xffffffff;
	}
	void	UnSetQuestStatusWindow() { if(m_pC_quest_status) m_pC_quest_status->SetQuestStatusInit(); }

	void	LevelUp()
	{
		// �̹� �� ������ �ƹ��͵� ���Ѵ�.
		// �ݴ� ���� closing interface��...

		if (!m_pC_level_up)
		{
			m_pC_level_up = new C_VS_UI_LEVELUP;
			m_pC_level_up->Start();
		}
	}

	bool	IsRunningLevelUp()
	{
		return (m_pC_level_up != NULL);
	}

	void	FinishLevelUp()
	{
		DeleteNew(m_pC_level_up);
	}

	void	AcquireChatting()
	{
		m_pC_chatting->Acquire();
	}

	bool	IsEmptyChatting()
	{
		return m_pC_chatting->IsEmpty();
	}

	virtual void	DoCommonActionBeforeEventOccured(); // �߻��ϱ� ��
	virtual void	DoCommonActionAfterEventOccured()  // �߻��� ��
	{
		CloseInventory();
		CloseGear();

//		InitWindowToggleFlag();
		GetInventoryWindow()->AttrWindowMove(true);
		GetGearWindow()->AttrWindowMove(true);

		// restore xy
		InventoryXY(m_backup_inventory_xy.x, m_backup_inventory_xy.y);
		GearXY(m_backup_gear_xy.x, m_backup_gear_xy.y);

		gpC_base->FinishEvent();
	}

	void	OpenInventoryToSell();
	void	FinishItemSelling();

	void	OpenInventoryToRepair();
	void	FinishItemRepairing();

	void	OpenInventoryToSilvering();
	void	FinishItemSilvering();

	void	InventoryXY(int _x, int _y)
	{
		GetInventoryWindow()->XY(_x, _y);
		GetInventoryWindow()->ResetRect();
	}

	void	GearXY(int _x, int _y)
	{
		GetGearWindow()->XY(_x, _y);
	}

	void	ShopRunningAnnounced(); // Shop�� ���� �ʿ��� Window�� ����.
	
	void	StorageRunningAnnounced();
	
	void	GetItemBoxRunningAnnounced();
	
	void	ExchangeRunningAnnounced();
	
	void	SetChattingInterface(C_VS_UI_CHATTING * p_chatting);

	virtual void	ResetSlayerQuickItemSize() {}
	virtual void	ResetOustersQuickItemSize() {}

	virtual C_VS_UI_GEAR	* GetGearWindow() const=0;
	virtual void	OpenGear(bool bl_set_load = true)=0;
	virtual void	CloseGear()=0;

	virtual C_VS_UI_GEAR	* GetGearOtherWindow() const=0;
	virtual void	OpenGearOther(bool bl_set_load = true)=0;
	virtual void	CloseGearOther()=0;

#if __CONTENTS(__GEAR_SWAP_CHANGE)			//������ ��ü ���� ���� �Լ� ����
	virtual	void	RunGearChange()=0;
#endif //__GEAR_SWAP_CHANGE
	
	

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);

	C_VS_UI_TRIBE();
	virtual ~C_VS_UI_TRIBE();

	void	SetupMenuItems(const char* mainSpkFileName);
	void	Start();
	void	Show();
	virtual void ShowExp() = 0;

	void Finish();

	// 2026-09-03 plan F3 — canli cozunurluk degisiminde HUD relayout:
	// eski cozunurlugun konumlari per-resolution deftere yazilir
	// (PhoenixHudLayout.set), yeni cozunurluk icin kayit varsa uygulanir,
	// yoksa ctor default formulune re-anchor edilir (TRIBE sol-alt,
	// minimap sag-alt + skill follow). Slayer/Vampire/Ouster hepsi bu
	// base uzerinden gecer (race parity).
	void	RelayoutForResolution(int oldW, int oldH);

	void	Process();

	void	AcquireDisappear() {}

	virtual void CancelPushState()
	{
		//m_pC_hide_button->CancelPushState();

	}

	virtual void UnacquireMouseFocus()//
	{
	//	m_pC_hide_button->UnacquireMouseFocus();
	}

	void AcquireFirstSequence()
	{

	}

	Point GetInventoryPosition() const
	{
		Point point(GetInventoryWindow()->x, GetInventoryWindow()->y);

		return point;
	}

	Point	GetInventoryGridPosition(int grid_x, int grid_y) const
	{
		Point point(GetInventoryWindow()->x + GetInventoryWindow()->GetGridRect().x+C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_X*grid_x, 
						GetInventoryWindow()->y + GetInventoryWindow()->GetGridRect().y+C_VS_UI_INVENTORY::GRID_UNIT_PIXEL_Y*grid_y); 

		return point;
	}

	Point	GetInventoryGetWidthAndHeight()
	{
		Point pt;
		pt.x	= GetInventoryWindow()->m_dwWidth;
		pt.y	= GetInventoryWindow()->m_dwHight;
		return pt;
	}


	C_VS_UI_OUSTERS_QUICKITEM * GetQuikSlotWindow() const { return m_pC_armsband; }
	C_VS_UI_SLAYER_QUICKITEM  * GetQuikSlotBeltWindow() const { return m_pC_quickitem; }

	void	Run(id_t id);
	bool	IsPixel(int _x, int _y);
	C_VS_UI_SKILL * GetSkillWindow() const { return m_pC_skill; }


	C_VS_UI_INVENTORY * GetInventoryWindow() const { return m_pC_inventory; }
	void	CloseInventory() { m_pC_inventory->Finish(); }
	void	OpenInventory(bool bl_set_load = true) { if( m_pC_inventory != NULL ) m_pC_inventory->Start(bl_set_load); }


	//C_VS_UI_GAMEHOOK* GetGameHookWindow() const { return m_pc_gamehook; }
	//void	CloseGameHook() { m_pc_gamehook->Finish(); }
	//void	OpenGameHook(bool bl_set_load = true)	{ if ( m_pc_gamehook != NULL ) m_pc_gamehook->Start(); }

	//�׼�����
	C_VS_UI_MILITARYDLG* GetMilitaryWindow() const { return m_pc_military; }
	void	CloseMilitary() { m_pc_military->Finish(); }
	void	OpenMilitary(bool bl_set_load = true)	{ if ( m_pc_military != NULL ) m_pc_military->Start(); }
	void	InvalidateMilitarySkillLists()
	{
		if (m_pc_military != NULL)
			m_pc_military->InvalidateSkillLists();
	}
	void	DownSkillOuster() { m_pc_military->SetOustersDownSkill();}
	void    UpdateMilitarySlotData(int nSlotID, int nType, int nGrade)
	{
		if ( !m_pc_military )
			return ;

		m_pc_military->UpdateMilitarySlotData(nSlotID, nType, nGrade);
	}

	//�̳ǽ���
	C_VS_UI_TMALL* GetTMallWindow() const { return m_pc_tmall; }
	void	CloseTMall() { m_pc_tmall->Finish(); }
	void	OpenTMall(bool bl_set_load = true)	{ if ( m_pc_tmall != NULL ) m_pc_tmall->Start(); }
	bool	AddItemToTMall(C_VS_UI_TMALL::ItemList pItem)
	{
		if ( !m_pc_tmall )
			return false;

		return m_pc_tmall->AddItem(pItem);
	}

	//��ֵ�����
	C_VS_UI_PAYEVENT* GetPayEventWindow() const { return m_pc_payevent; }
	void	ClosePayEvent() { m_pc_payevent->Finish(); }
	void	OpenPayEvent(bool bl_set_load = true)	{ if ( m_pc_payevent != NULL ) m_pc_payevent->Start(); }
	void    SetPayEventCount(int nLevel, int nCount)
	{
		if ( !m_pc_payevent )
			return;

		m_pc_payevent->SetCount(nLevel, nCount);
	}

	void	SetPayEventStautus(int nLevel, int nStautus)
	{
		if ( !m_pc_payevent )
			return;

		m_pc_payevent->SetStautus(nLevel, nStautus);
	}

	bool	AddItemToPayEvent(C_VS_UI_PAYEVENT::ItemData pItem)
	{
		if ( !m_pc_payevent )
			return false;

		return m_pc_payevent->AddItem(pItem);
	}

	//ʥս����
	C_VS_UI_HOLYWAR* GetHollyWarWindow() const { return m_pc_hollywar; }
	void	CloseHollyWar() { m_pc_hollywar->Finish(); }
	void	OpenHollyWar(bool bl_set_load = true)	{ if ( m_pc_hollywar != NULL ) m_pc_hollywar->Start(); }
	void    AddHollyScore(C_VS_UI_HOLYWAR::PlayerScore score, int nRace);
	void    AddHollyRace(C_VS_UI_HOLYWAR::RaceData * data);
	void    AddPlayerData(C_VS_UI_HOLYWAR::PlayerData data);

	void	WindowEventReceiver(id_t event);
	bool	MouseControl(UINT message, int _x, int _y);

	void	SetZone(int zone_id)				{ m_pC_minimap->SetZone(zone_id); }
	int		GetZoneID()							{ return m_pC_minimap->GetZoneID(); }
	void	SetBlock(int x,int y)				{ m_pC_minimap->SetBlock(x,y);}
	void	SetSize(SIZE size)					{ m_pC_minimap->SetSize(size); }
	void	SetXY(int x, int y)					{ m_pC_minimap->SetXY(x, y); }
	int		GetX()								{ return m_pC_minimap->GetX(); }
	int		GetY()								{ return m_pC_minimap->GetY(); }
	void	SetSafetyZone(RECT rect, bool my_zone)	{ m_pC_minimap->SetSafetyZone(rect, my_zone); }
	void	SetNPC(MINIMAP_NPC npc)					{ m_pC_minimap->SetNPC(npc); }
	void	ClearNPC()								{ m_pC_minimap->ClearNPC(); }
	void	SetPortal(RECT rect, int id)			{ m_pC_minimap->SetPortal(rect, id); }


	bool	CloseInventoryGearWindow();
	bool	GetGearOpenState() const;
	bool	GetInventoryOpenState() const;	
	bool	GetGearOtherOpenState() const;
	bool    GetTMallOpenState() const;
	bool	GetPayEventState()	const;
	bool	GetHollyWarOpenState() const;
	bool	GetMilitaryOpenState() const;

	void	UnselectSkill() { GetSkillWindow()->UnselectSkill(); }
	void	ChangeSkillWheel(bool direct) { GetSkillWindow()->ChangeSkillWheel(direct); }
	void	SelectSkill(int id)	{ GetSkillWindow()->SelectSkill(id); }
	
	void	OnClick_HelpTap( id_t execidx );

	void	HotKey_Party();
	void	HotKey_Mark();
	void	HotKey_Help();
	void	HotKey_SkillInfo();
	void	HotKey_Grade3Info();
	void	HotKey_Grade2Info();
	void	HotKey_Grade1Info();
	void	HotKey_Minimap();
	void	HotKey_WorldMap();
	void	HotKey_CharInfo();
//	void	HotKey_WindowToggle();
	void	HotKey_Inventory(bool IsCheckSubInventory = false);	
	void    HotKey_Military();
	void	HotKey_Gear();
#if	__CONTENTS(__GEAR_SWAP_CHANGE)	//C_VS_UI_TRIBE		//HotKey_GearChange();
	void	HotKey_GearChange();
	void	HotKey_GearChangeShow();
#endif	//__GEAR_SWAP_CHANGE
	void	HotKey_Skill();
	void	CloseSkillChooseWindow();
	void	HotKey_ExtendChat();		// ctrl + e
	void	HotKey_Chat_Up();			// ctrl + PgUp
	void	HotKey_Chat_Dn();			// ctrl + PgDn
	void	HotKey_Tribe_Prior();		// alt	+ PgUp
	void	HotKey_Tribe_Next();		// alt	+ PgDn
	void	HotKey_Guild_Info();			// ctrl	+ v
	void	HotKey_Guild_Member_List();	// ctrl	+ l
	void	HotKey_Coer_Attack();		// ctrl	+ o
// by svi
#if __CONTENTS(__AUTO_ATTACT)	//__AUTO_ATTACT	
	void	HotKey_Auto_Attack();		// ctrl	+ j
#endif  //__AUTO_ATTACT
// end	
	void	HotKey_Sysrq();				// printscreen sysrq
	void	HotKey_Slash();				// ctrl + /
	void	HotKey_Background_Music();	// ä�ø��ɾ� /�����
	void	HotKey_Effect_Music();		// ä�ø��ɾ� /ȿ����
	void	HotKey_Chat_Help();			// ä�ø��ɾ� /ä�õ���
	void	HotKey_ZoneChat();			// ä�ø��ɾ� /z
	void	HotKey_GuildChat();			// ä�ø��ɾ� /g
	void	HotKey_UnionChat();			// ä�ø��ɾ� /u
	void	HotKey_Whisper();			// ä�ø��ɾ� /w
	void	HotKey_Where();				// ä�ø��ɾ� /���
//	void	HotKey_F9();	
//	void	HotKey_F10();
//	void	HotKey_F11();
//	void	HotKey_F12();
	void	HotKey_PartyChat();			// ä�ø��ɾ� /p
	void	HotKey_NormalChat();		// ä�ø��ɾ� /c
	void	HotKey_Quest();				// ctrl + q
	void	HotKey_MailBox();			// ctrl + b
	void	HotKey_PetInfo();			// ctrl + CapsLock
	void    HotKey_SummonPet();			// ctrl + space
	void	HotKey_SMSWindow();
	void	HotKey_NamingWindow();

	// 2005, 1, 3, sobeit add start
	void	OpenInventoryToSwapAdvanceItem();
	void	FinishSwapAdvancementItem();
	// 2005, 1, 3, sobeit add end

	void	Inventory_Item_Position_Remove();
	//2008.09.24 shootkj
	//�ð��� �����۰� ��ġ�� ��ü �Ϸ� �Ұ�� �������� cannotadd�� ������ �����ش�.
	//�ٽ� ���� ��ü �� �� ����. �ٵ� Ŭ�󿡼��� ��ü �õ��� �Ѵ�.
	//��ü�� ���� �ʵ��� ��ġ�� �ٽ� ���� ��� �ٲ� ��� �Ѵ�.
};

//-----------------------------------------------------------------------------
// C_VS_UI_INFO
//
// 
//-----------------------------------------------------------------------------
class C_VS_UI_INFO : public Window, public Exec, public ButtonVisual
{
public :
	
	enum INFO_MODE
	{
		CHARACTER_INFO_MODE = 0,
		SKILL_INFO_MODE,
		GRADE1_INFO_MODE,
		GRADE2_INFO_MODE,
		GRADE3_INFO_MODE,
		RARE_SKILL_INFO_MODE,
		INFO_MODE_MAX,
	};

	static int				m_selected_grade_skill;
	static int				m_selected_skill_domain;
	static int				m_ousters_Magic;
	static int				m_vampire_ACType;

	static int				m_selected_ACSkillID;

private:
	struct PETINFO			*m_pPetInfo;
	C_SPRITE_PACK			m_petButtonSpk;

	C_VS_UI_DIALOG			*m_pC_learn_grade_skill_confirm;
	Rect					m_rt_tab;

	int						m_focused_skill_id;
	//bool					m_skill_info_mode;
	INFO_MODE				m_info_mode;
	int						m_iDomain;
	int						m_iGrade;
	int						m_draw_grade_skill_mark;

	int						m_modify_wide;
	
	std::vector<int>		m_grade_skill_id;
	
	SKILLDOMAIN				m_skill_domain;

	MSkillDomain::SKILL_STEP_LIST	m_advance_skill_id_vec;		// ȭ�鿡 ǥ�õǴ� ���� ��ų ���̵� �����Ѵ�.
	MSkillDomain::SKILL_STEP_LIST	m_rare_skill_id_vec;		// ȭ�鿡 ǥ�õǴ� ���� ��ų ���̵� �����Ѵ�.

	int						m_rare_skill_max;					// ������ ��� �� �ִ� ���ų�� �ִ밪

	
	ButtonGroup *			m_pC_common_button_group;
	ButtonGroup *			m_pC_char_button_group;
	ButtonGroup *			m_pC_skill_button_group;
	ButtonGroup *			m_pC_grade1_button_group;	
	ButtonGroup *			m_pC_grade2_button_group;	
	ButtonGroup *			m_pC_grade3_button_group;	
	C_VS_UI_SCROLL_BAR *	m_pC_char_scroll_bar;
	C_VS_UI_SCROLL_BAR *	m_pC_skill_scroll_bar;
	C_VS_UI_SCROLL_BAR *	m_pC_skill_scroll_bar_width;
	C_VS_UI_SCROLL_BAR *	m_pC_grade1_scroll_bar;
	C_VS_UI_SCROLL_BAR *	m_pC_grade3_scroll_bar;
	C_VS_UI_SCROLL_BAR *	m_pC_rare_skill_scroll_bar;
	CSprite					*m_p_guild_mark;

	RECT					m_rcSkillDesciption;

	POINT					m_CenterPos;
	bool					m_bOustersDownSkill;

	enum EXEC_ID
	{
		ALPHA_ID,
		CLOSE_ID,
		HELP_ID,

		// SLAYER SKILL INFO
		BLADE_ID,
		SWORD_ID,
		GUN_ID,
		HEAL_ID,
		ENCHANT_ID,
		TOTAL_ID,

		// VAMPIRE SKILL INFO
		POISON_ID,
		ACID_ID,
		CURSE_ID,
		SUMMON_ID,
		BLOOD_ID,
		INNATE_ID,

		// OUSTERS SKILL INFO
		COMBAT_ID,
		ELEMENTAL_ID,

		// VAMPIRE CHARACTER INFO
		STR_PLUS_ID,
		DEX_PLUS_ID,
		INT_PLUS_ID,
		
		PIVATE_ID,
		SERENT_ID,
		FEACEL_ID,
		LITENA_ID,
		KAINEL_ID,
		GENEAL_ID,
		FORE_GENEAL_ID,
		MAJORIS_GENEAL_ID,								
		CLOEL_GENEAL_ID,
		MARSHAL_ID,

		RITTER_ID,
		REICHSRITTER_ID,
		BARONET_ID,
		PREYHER_ID,
		GRAF_ID,
		MARKGRAF_ID,
		PFALZGRAF_ID,
		FURST_ID,								
		HERZOG_ID,
		LANDESHER_ID,

		MALCHUT_ID,					// ������Ʈ
		YESOD_ID,						// ���ҵ�
		HOD_ID,						// ȣ��
		NETRETH_ID,					// ����ũ
		TIPHRETH_ID,					// Ƽ�ķ�Ʈ
		GEBURAH_ID,
		CHESED_ID,
		BINSH_ID,								
		CHOKMA_ID,
		KEATHER_ID,
		ADVENCEMENT_ID,
		// GRADE1 BUTTON
		GRADE1_BUTTON1_ID,
		GRADE1_BUTTON2_ID,
		GRADE1_BUTTON3_ID,
		GRADE1_BUTTON4_ID,
		GRADE1_BUTTON5_ID,
		GRADE1_BUTTON6_ID,
		GRADE1_BUTTON7_ID,
		GRADE1_BUTTON8_ID,
		GRADE2_BUTTON1_ID,
		GRADE2_BUTTON2_ID,
		GRADE2_BUTTON3_ID,
		GRADE2_BUTTON4_ID,
		GRADE2_BUTTON5_ID,
		GRADE2_BUTTON6_ID,
		GRADE2_BUTTON7_ID,
		GRADE2_BUTTON8_ID,
		GRADE3_BUTTON1_ID,
		GRADE3_BUTTON2_ID,
		GRADE3_BUTTON3_ID,
		GRADE3_BUTTON4_ID,
		GRADE3_BUTTON5_ID,
		GRADE3_BUTTON6_ID,
//		GRADE3_BUTTON_TAB_ID1,
//		GRADE3_BUTTON_TAB_ID2,
//		GRADE3_BUTTON_TAB_ID3,
//		GRADE3_BUTTON_TAB_ID4,
//		GRADE3_BUTTON_TAB_ID5,
//		GRADE3_BUTTON_TAB_ID6,
//		GRADE3_BUTTON_TAB_ID7,
//		GRADE3_BUTTON_TAB_ID8,
//		GRADE3_BUTTON_TAB_ID9,
//		GRADE3_BUTTON_TAB_ID10,
		CHANGE_IMAGE_ID,		// �̹��� ���� ��ư
		PET_INFO_ID,
		
		// grade 2
			
		
		
	
		
	};

	enum GRADESKILL_STATUS
	{
		DISABLE,
		CAN_LEARN,
		LEARNED,
	};


	Rect					m_rt_char_box;
	CSpritePack *			m_p_face;

public:
	
	

	C_VS_UI_INFO();
	~C_VS_UI_INFO();

	void	RefreshImage();

	void	UnacquireMouseFocus()
	{
//		m_pC_button_group->UnacquireMouseFocus();
		m_pC_common_button_group->UnacquireMouseFocus();
		m_pC_char_button_group->UnacquireMouseFocus();
		m_pC_skill_button_group->UnacquireMouseFocus();
		m_pC_grade1_button_group->UnacquireMouseFocus();
	}
	void	CancelPushState() 
	{
//		m_pC_button_group->CancelPushState();
		m_pC_common_button_group->CancelPushState();
		m_pC_char_button_group->CancelPushState();
		m_pC_skill_button_group->CancelPushState();
		m_pC_grade1_button_group->CancelPushState();
	}

	void	SetPetInfo(struct PETINFO *pPetInfo);
	struct PETINFO* GetPetInfo();
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);
	void	AcquireDisappear() {}
	void	Run(id_t id);
	bool	MouseControl(UINT message, int _x, int _y);	
	bool	CharacterInfoMouseControl(UINT message, int _x, int _y);
	bool	SkillInfoMouseControl(UINT message, int _x, int _y);
	bool	RareSkillInfoMouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	CheckGradeSkillIDList();
	
	void	Show();
	void	_Show1();	// Skill Info
	void	_Show2();	// Character Info
	void	_Show3();	// Grade1
	void	_Show4();	// Grade2
	//by csm 2004.12.30 ���� ���� �������̽� �߰� 
	
	void	_Show5();	// Grade2
	void	_Show5_ACSkillList();		// ���� ��ų ����Ʈ ���
	
	void	_Show6();	// Rare Skill	by chyaya
	void	_Show6_RareSkillList();		// ���� ��ų ����Ʈ ���

	void	Process();
//	void	Start(bool skill_mode);
	void	Start(INFO_MODE	Info_Mode);
	void	Finish();

	bool	IsCharacterMode()	{ return m_info_mode == CHARACTER_INFO_MODE; }
	bool	IsSkillMode()	{ return m_info_mode == SKILL_INFO_MODE; }
	bool	IsGrade1Mode()	{ return m_info_mode == GRADE1_INFO_MODE; }
	bool	IsGrade2Mode()	{ return m_info_mode == GRADE2_INFO_MODE; }
	bool	IsGrade3Mode()	{ return m_info_mode == GRADE3_INFO_MODE; }
	bool	IsRareSkillMode() { return m_info_mode == RARE_SKILL_INFO_MODE; }

	void	SetOustersDownSkill() { m_bOustersDownSkill = true; }
	void	SetCorrectGrade();
	const char * GetChinhoLevel(const int level);

	// ������ ���� ó��
	void	Process5_ACSkillList();
	void	Process5_ACSkillList_Slayer();
	void	Process5_ACSkillList_Vampire();
	void	Process5_ACSkillList_Ousters();

	// ���ų�� ���� ó��
	void	Process6_RareSkillList();
};



class C_VS_UI_WINDOW_MANAGER
{
public:
	enum WINDOW_ID
	{
		HPBAR,
		MINIMAP,
		MAINMENU,
		CHATTING,
		CHATTING_OLD,
		INVENTORY,
		GEAR,
		INFO,
		QUICKITEMSLOT,
		PARTY,
		EFFECT_STATUS,
		OTHER_INFO,
		TRACE_WINDOW,
		BLOOD_BIBLE_WINDOW,
		QUEST_STATUS_WINDOW,
				
		CTF_STATUS,
		PET_INFO,

		NAMING_WINDOW,

		QUEST_MANAGER_LIST, // ���� â
		QUEST_MANAGER_DETAIL, // ���λ���
		QUEST_MANAGER_MISSION, // �ӹ�
		QUEST_MANAGER_ITEM, // �κ��丮
		INVENTORY_SUB,
		BLOOD_BURST_WINDOW,
		SHORTCUT_SLOT_WINDOW,
		SIMPLEINFOMATION_WINDOW,
		WINDOW_TOTAL,
	};

	typedef std::map<std::string, WindowStatus> WINDOWSTATUS_MAP;

	C_VS_UI_WINDOW_MANAGER();
	~C_VS_UI_WINDOW_MANAGER();

	void SetFilter(CHAT_LINE_CONDITION filter, bool set);
	bool GetFilter(CHAT_LINE_CONDITION filter) { return m_filter[filter]; }

	void SetAlpha(WINDOW_ID id, bool bl_alpha);
	bool IsAlpha(WINDOW_ID id) { return m_alpha[id]; }

	void SetAutoHide(WINDOW_ID id, Window::ATTRIBUTES_HIDE hide);
	Window::ATTRIBUTES_HIDE GetAutoHide(WINDOW_ID id) { return m_autohide[id]; }

	void SetRect(WINDOW_ID id, Rect rect);
	Rect& GetRect(WINDOW_ID id) { return m_rect[id]; }

	void SetHPBarSmall(bool bl_small) { m_bl_hpbar_small = bl_small; }
	bool IsHPBarSmall() { return m_bl_hpbar_small; }

	void SetHPBarHeight(bool bl_height) { m_bl_hpbar_height = bl_height; }
	bool IsHPBarHeight() { return m_bl_hpbar_height; }

	void SetBloodBurstHeight(bool bl_height) { m_bl_bloodburst_height = bl_height; }
	bool IsBloodBurstHeight() { return m_bl_bloodburst_height; }

	void SetQuickItemSlotHeight(bool bl_height) { m_bl_quickitemslot_height = bl_height; }
	bool IsQuickItemSlotHeight() { return m_bl_quickitemslot_height; }

	void SetEffectStatusHeight(bool bl_height) { m_bl_effectstatus_height = bl_height; }
	bool IsEffectStatusHeight() { return m_bl_effectstatus_height; }

	void SetMainMenu(int index) { m_i_main_menu = index; }
	int	 GetMainMenu() { return m_i_main_menu; }

	void SaveToFile(std::ofstream &file);
	void LoadFromFile(ivfstream &file);

	void SetDefault();
	void SetHotKey(int **hotkey = NULL);
	void SetQuickSlotHotkey(int *hotkey = NULL);
	void SetHotkeyType(int *hotkey = NULL);

	int** GetHotKey()	{ return (int **)m_skill_hotkey_buf; }
	int * GetHotKeyQuickSlot()	{	return	(int *)m_QuickSlot_hotkey_buf; }
	int * GetHotKeyType()	{	return	(int *)m_HotKey_Type;	 }

	void  SetSMSMynum(char* str);
	char* GetSMSMyNum() {return &m_SMS_MyNum[0];}
	  
	void  InitDefaultQuickSlot() ; 	

	void SetWindowStatus(const char* key, const WindowStatus* winStatus);
	const WindowStatus* GetWindowStatus(const char* key);
	void RemoveWindowStatus(const char* key);
	void ClearWindowStatus();

private:
	Rect m_rect[WINDOW_TOTAL];
	Window::ATTRIBUTES_HIDE m_autohide[WINDOW_TOTAL];
	bool m_alpha[WINDOW_TOTAL];

	WINDOWSTATUS_MAP m_WindowStatusMap;

	bool m_filter[CLD_TOTAL];
	int	 m_skill_hotkey_buf[C_VS_UI_SKILL::HOTKEY_MAX][C_VS_UI_SKILL::GRADE_MAX];

	int	 m_QuickSlot_hotkey_buf[C_VS_UI_SKILL::HOTKEY_MAX];
	int	 m_HotKey_Type[C_VS_UI_SKILL::HOTKEY_MAX] ; 

	bool m_bl_hpbar_small, m_bl_hpbar_height, m_bl_quickitemslot_height, m_bl_effectstatus_height, m_bl_bloodburst_height;

	int m_i_main_menu;

	char m_SMS_MyNum[12];
};


class C_VS_UI_TEAM_LIST : public Window, public Exec, public ButtonVisual
{
private:
	ButtonGroup *				m_pC_button_group;
	C_SPRITE_PACK				m_guild_spk;
	enum GUILD_SPK
	{
		LIST_WINDOW = 0,
		INFO_WINDOW,
		HELP_BUTTON,
		HELP_BUTTON_PUSHED,
		X_BUTTON,
		X_BUTTON_PUSHED,
		SCROLL_BAR,
		SCROLL_UP_BUTTON,
		SCROLL_UP_BUTTON_PUSHED,
		SCROLL_DOWN_BUTTON,
		SCROLL_DOWN_BUTTON_PUSHED,
		SCROLL_TAG,
		BAR_DARKEN,
		BAR_HILIGHTED,
		BAR_SEARCH,
		TITLE_TEAM_REGISTRATION,
		TITLE_MEMBER_REGISTRATION,
		TITLE_TEAM_INFO,
		TITLE_MEMBER_INFO,
		TITLE_TEAM_LIST,
		TITLE_TEAM_LIST_HILIGHTED,

		SEARCH_NAME,
		SEARCH_NAME_HILIGHTED,
		SEARCH_NAME_HILIGHTED_PUSHED,
		SEARCH_LEADER,
		SEARCH_LEADER_HILIGHTED,
		SEARCH_LEADER_HILIGHTED_PUSHED,
		SEARCH_ALL1,//modify by hays
		SEARCH_ALL_HILIGHTED,
		SEARCH_ALL_HILIGHTED_PUSHED,
		SEARCH_SEARCH,
		SEARCH_SEARCH_HILIGHTED,
		SEARCH_SEARCH_HILIGHTED_PUSHED,

		REGISTER_BUTTON,
		REGISTER_BUTTON_HILIGHTED,
		CLOSE_BUTTON,
		CLOSE_BUTTON_HILIGHTED,
		JOIN_BUTTON,
		JOIN_BUTTON_HILIGHTED,
		EXPEL_BUTTON,
		EXPEL_BUTTON_HILIGHTED,
		ACCEPT_BUTTON,
		ACCEPT_BUTTON_HILIGHTED,
		DENY_BUTTON,
		DENY_BUTTON_HILIGHTED,		
		MODIFY_BUTTON,
		MODIFY_BUTTON_HILIGHTED,		

		INDEX_TEAM,
		INDEX_TEAM_HILIGHTED,
		INDEX_LEADER,
		INDEX_LEADER_HILIGHTED,
		INDEX_EXPIREDATE,
		INDEX_EXPIREDATE_HILIGHTED,
		INDEX_MEMBERS,
		INDEX_MEMBERS_HILIGHTED,
		INDEX_RANKING,
		INDEX_RANKING_HILIGHTED,

		TITLE_WAR_LIST,
		TITLE_WAR_LIST_HILIGHTED,
		INDEX_WAR_CHALLENGER,
		INDEX_WAR_CHALLENGER_HILIGHTED,
		INDEX_WAR_DATE,
		INDEX_WAR_DATE_HILIGHTED,

		INDEX_UNITED_JOIN,				// ���հ���
		INDEX_UNITED_JOIN_HILIGHTED,	
		INDEX_UNITED_WITHDRAWAL,		// ����Ż��
		INDEX_UNITED_WITHDRAWAL_HILIGHTED,
		INDEX_UNITED_EXPEL,				// �����߹�
		INDEX_UNITED_EXPEL_HILIGHTED,
		INDEX_GUILD_APPOINT,			// �Ӹ�
		INDEX_GUILD_APPOINT_HILIGHTED,


	};

	enum EXEC_ID
	{
		HELP_ID,
		X_ID,
		CLOSE_ID,
		REGISTER_ID,

		SCROLL_UP_ID,
		SCROLL_DOWN_ID,

		TEAM_LIST_ID,

		INDEX_TEAM_ID,
		INDEX_LEADER_ID,
		INDEX_EXPIREDATE_ID,
		INDEX_MEMBERS_ID,
		INDEX_RANKING_ID,

		SEARCH_TYPE_ID,
		SEARCH_ID,


		EXEC_MAX,
	};
public:
	struct TEAM_LIST_BASE
	{
		std::string TEAM_NAME, LEADER_NAME;
		int	guild_id;
		int MEMBERS;
	};

	struct REGIST_TEAM_LIST:public TEAM_LIST_BASE
	{
		int RANKING;
	};
	
	struct READY_TEAM_LIST:public TEAM_LIST_BASE
	{
		std::string EXPIRE_DATE;
		int MEMBERS_MAX;
	};

private:
	POINT	m_vampire_point;
	bool	m_bl_ready;
	int		m_scroll;
	bool	m_bl_scrolling;

	int		m_search_type;
	int		m_print_x[4], m_print_y, m_print_gap;

	int		m_iFocus;

	LineEditorVisual			m_lev_search;

	std::vector<READY_TEAM_LIST>	m_v_ready_team_search_list;
	std::vector<READY_TEAM_LIST>	m_v_ready_team_list;
	std::vector<REGIST_TEAM_LIST>	m_v_regist_team_search_list;
	std::vector<REGIST_TEAM_LIST>	m_v_regist_team_list;

	int		m_last_sort_type;

	bool	m_bIsUnion;
public:
	C_VS_UI_TEAM_LIST(bool ready, bool IsUnion = false);
	~C_VS_UI_TEAM_LIST();

	void	AddReadyTeamList(const READY_TEAM_LIST &team_info);
	void	AddRegistTeamList(const REGIST_TEAM_LIST &team_info);

	void	Process();
	void	UnacquireMouseFocus();
	bool	IsPixel(int _x, int _y);
	void	Run(id_t id);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	bool	AllMatchWindowPixel(int _x, int _y) const;
	bool	IsUnionInfo() {return m_bIsUnion;}

	void	Start();
	void	Finish();

	void	Show();

};

class C_VS_UI_TEAM_MEMBER_LIST : public Window, public Exec, public ButtonVisual
{
private:
	ButtonGroup *				m_pC_button_group;
	C_SPRITE_PACK				m_guild_member_list_spk;
	C_VS_UI_SCROLL_BAR			*m_pC_scroll_bar;
	
	bool						m_SortFlag_Id;
	bool						m_SortFlag_Server;
	bool						m_SortFlag_Grade;
	BYTE						m_ButtonFlag[3];
	enum TEAM_MEMBER_SPK
	{
		BUTTON_CLOSE,
		BUTTON_CLOSE_HILIGHTED,
		BUTTON_CLOSE_HILIGHTED_PUSHED,
		IMAGE_NULL,
	};

	enum EXEC_ID
	{
		CLOSE_ID,
		SORT_ID,
		SORT_SERVER,
		SORT_GRADE,
		EXEC_MAX,
	};
public:
	struct TEAM_MEMBER_LIST
	{
		std::string MEMBER_NAME;
		std::string SERVER_NAME;
		int member_grade;
//		int server_type;
		bool bLogOn;
	};

private:
	int		m_print_x[2], m_print_y, m_print_gap;
	int		m_focus;
	std::vector<TEAM_MEMBER_LIST>	m_v_member_list;
	BYTE	m_bAvailableRecall; // Ÿ�Կ� ���� ��� ȣ���ϱ� 
	char	m_SelectedID[16];
	char	m_AskingMessage[128];
public:
	C_VS_UI_TEAM_MEMBER_LIST();
	~C_VS_UI_TEAM_MEMBER_LIST();

	void	AddMemberList(const TEAM_MEMBER_LIST &member_list, BYTE bType);

	void	Process();
	void	UnacquireMouseFocus();
	bool	IsPixel(int _x, int _y);
	void	Run(id_t id);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	Start();
	void	Finish();

	void	Show();
	void	_Sort(int nSort);
	BYTE	GetAvailableRecall() {return m_bAvailableRecall;}
	void	SetAvailableRecall(BYTE bFlag) {m_bAvailableRecall = bFlag;}
	char*	GetSelectedID() {return m_SelectedID;}
};

class C_VS_UI_NPC_DIALOG;
class C_VS_UI_TEAM_INFO : public Window, public Exec, public ButtonVisual
{
private:
	ButtonGroup *				m_pC_button_group;
	C_SPRITE_PACK				m_guild_spk;
	enum GUILD_SPK
	{
		LIST_WINDOW = 0,
		INFO_WINDOW,
		HELP_BUTTON,
		HELP_BUTTON_PUSHED,
		X_BUTTON,
		X_BUTTON_PUSHED,
		SCROLL_BAR,
		SCROLL_UP_BUTTON,
		SCROLL_UP_BUTTON_PUSHED,
		SCROLL_DOWN_BUTTON,
		SCROLL_DOWN_BUTTON_PUSHED,
		SCROLL_TAG,
		BAR_DARKEN,
		BAR_HILIGHTED,
		BAR_SEARCH,
		TITLE_TEAM_REGISTRATION,
		TITLE_MEMBER_REGISTRATION,
		TITLE_TEAM_INFO,
		TITLE_MEMBER_INFO,
		TITLE_TEAM_LIST,
		TITLE_TEAM_LIST_HILIGHTED,

		SEARCH_NAME,
		SEARCH_NAME_HILIGHTED,
		SEARCH_NAME_HILIGHTED_PUSHED,
		SEARCH_LEADER,
		SEARCH_LEADER_HILIGHTED,
		SEARCH_LEADER_HILIGHTED_PUSHED,
		SEARCH_ALL1,
		SEARCH_ALL_HILIGHTED,
		SEARCH_ALL_HILIGHTED_PUSHED,
		SEARCH_SEARCH,
		SEARCH_SEARCH_HILIGHTED,
		SEARCH_SEARCH_HILIGHTED_PUSHED,

		REGISTER_BUTTON,
		REGISTER_BUTTON_HILIGHTED,
		CLOSE_BUTTON,
		CLOSE_BUTTON_HILIGHTED,
		JOIN_BUTTON,
		JOIN_BUTTON_HILIGHTED,
		EXPEL_BUTTON,
		EXPEL_BUTTON_HILIGHTED,
		ACCEPT_BUTTON,
		ACCEPT_BUTTON_HILIGHTED,
		DENY_BUTTON,
		DENY_BUTTON_HILIGHTED,		
		MODIFY_BUTTON,
		MODIFY_BUTTON_HILIGHTED,		

		INDEX_TEAM,
		INDEX_TEAM_HILIGHTED,
		INDEX_LEADER,
		INDEX_LEADER_HILIGHTED,
		INDEX_EXPIREDATE,
		INDEX_EXPIREDATE_HILIGHTED,
		INDEX_MEMBERS,
		INDEX_MEMBERS_HILIGHTED,
		INDEX_RANKING,
		INDEX_RANKING_HILIGHTED,

		TITLE_WAR_LIST,
		TITLE_WAR_LIST_HILIGHTED,
		INDEX_WAR_CHALLENGER,
		INDEX_WAR_CHALLENGER_HILIGHTED,
		INDEX_WAR_DATE,
		INDEX_WAR_DATE_HILIGHTED,

		INDEX_UNITED_JOIN,				// ���հ���
		INDEX_UNITED_JOIN_HILIGHTED,	
		INDEX_UNITED_WITHDRAWAL,		// ����Ż��
		INDEX_UNITED_WITHDRAWAL_HILIGHTED,
		INDEX_UNITED_EXPEL,				// �����߹�
		INDEX_UNITED_EXPEL_HILIGHTED,
		INDEX_GUILD_APPOINT,			// �Ӹ�
		INDEX_GUILD_APPOINT_HILIGHTED,


	};
	enum EXEC_ID
	{
		HELP_ID,
		X_ID,
		CLOSE_ID,
		JOIN_ID,

		SCROLL_UP_ID,
		SCROLL_DOWN_ID,

		MODIFY_ID,
		UNITED_JOIN_ID,
		UNITED_WITHDRAWAL_ID,
		UNUTED_EXPEL_ID,
		
		EXEC_MAX,
	};
public:
	struct TEAM_INFO_BASE
	{
		std::string TEAM_NAME, LEADER_NAME, INTRODUCTION;
		int guild_id, MEMBERS, REG_FEE;
	};

	struct REGIST_TEAM_INFO:public TEAM_INFO_BASE
	{
		std::string REGISTERED_DATE;
		int RANKING;
	};
	
	struct READY_TEAM_INFO:public TEAM_INFO_BASE
	{
		std::string EXPIRE_DATE;
		std::vector<std::string> MEMBERS_NAME;
		int MEMBERS_MAX;
	};

private:
	bool	m_master;
	bool	m_bl_ready;
	int		m_scroll;
	int		m_scroll_max;
	bool	m_bl_scrolling;

	static REGIST_TEAM_INFO			m_regist_info;
	READY_TEAM_INFO				m_ready_info;
	C_VS_UI_NPC_DIALOG		*m_pC_dialog ;
	LineEditorVisual			m_lev_intro;

	bool	m_IsUnion;

public:
	C_VS_UI_TEAM_INFO(bool ready, void *info, bool IsUnion = false);
	~C_VS_UI_TEAM_INFO();

	void	Process();
	void	UnacquireMouseFocus();
	bool	IsPixel(int _x, int _y);
	void	Run(id_t id);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	static int		GetGuildId() { return m_regist_info.guild_id; }
	void	CloseTeamInfoDialogBox();

	void	Start();
	void	Finish();
	void	Show();

};

class C_VS_UI_TEAM_MEMBER_INFO : public Window, public Exec, public ButtonVisual
{
private:
	ButtonGroup *				m_pC_button_group;
	C_SPRITE_PACK				m_guild_member_spk;
	enum GUILD_SPK
	{
		LIST_WINDOW = 0,
		INFO_WINDOW,
		HELP_BUTTON,
		HELP_BUTTON_PUSHED,
		X_BUTTON,
		X_BUTTON_PUSHED,
		SCROLL_BAR,
		SCROLL_UP_BUTTON,
		SCROLL_UP_BUTTON_PUSHED,
		SCROLL_DOWN_BUTTON,
		SCROLL_DOWN_BUTTON_PUSHED,
		SCROLL_TAG,
		BAR_DARKEN,
		BAR_HILIGHTED,
		BAR_SEARCH,
		TITLE_TEAM_REGISTRATION,
		TITLE_MEMBER_REGISTRATION,
		TITLE_TEAM_INFO,
		TITLE_MEMBER_INFO,
		TITLE_TEAM_LIST,
		TITLE_TEAM_LIST_HILIGHTED,

		SEARCH_NAME,
		SEARCH_NAME_HILIGHTED,
		SEARCH_NAME_HILIGHTED_PUSHED,
		SEARCH_LEADER,
		SEARCH_LEADER_HILIGHTED,
		SEARCH_LEADER_HILIGHTED_PUSHED,
		SEARCH_ALL1,
		SEARCH_ALL_HILIGHTED,
		SEARCH_ALL_HILIGHTED_PUSHED,
		SEARCH_SEARCH,
		SEARCH_SEARCH_HILIGHTED,
		SEARCH_SEARCH_HILIGHTED_PUSHED,

		REGISTER_BUTTON,
		REGISTER_BUTTON_HILIGHTED,
		CLOSE_BUTTON,
		CLOSE_BUTTON_HILIGHTED,
		JOIN_BUTTON,
		JOIN_BUTTON_HILIGHTED,
		EXPEL_BUTTON,
		EXPEL_BUTTON_HILIGHTED,
		ACCEPT_BUTTON,
		ACCEPT_BUTTON_HILIGHTED,
		DENY_BUTTON,
		DENY_BUTTON_HILIGHTED,		
		MODIFY_BUTTON,
		MODIFY_BUTTON_HILIGHTED,		

		INDEX_TEAM,
		INDEX_TEAM_HILIGHTED,
		INDEX_LEADER,
		INDEX_LEADER_HILIGHTED,
		INDEX_EXPIREDATE,
		INDEX_EXPIREDATE_HILIGHTED,
		INDEX_MEMBERS,
		INDEX_MEMBERS_HILIGHTED,
		INDEX_RANKING,
		INDEX_RANKING_HILIGHTED,

		TITLE_WAR_LIST,
		TITLE_WAR_LIST_HILIGHTED,
		INDEX_WAR_CHALLENGER,
		INDEX_WAR_CHALLENGER_HILIGHTED,
		INDEX_WAR_DATE,
		INDEX_WAR_DATE_HILIGHTED,

		INDEX_UNITED_JOIN,							// ���հ���
		INDEX_UNITED_JOIN_HILIGHTED,	
		INDEX_UNITED_WITHDRAWAL,					// ����Ż��
		INDEX_UNITED_WITHDRAWAL_HILIGHTED,
		INDEX_UNITED_EXPEL,							// �����߹�
		INDEX_UNITED_EXPEL_HILIGHTED,
		INDEX_GUILD_APPOINT,						// �Ӹ�
		INDEX_GUILD_APPOINT_HILIGHTED,
		
		INDEX_GUILD_DELEGATION_OF_POWER,			// ���� ����
		INDEX_GUILD_DELEGATION_OF_POWER_HILIGHTED,

	};

	enum EXEC_ID
	{
		HELP_ID,
		X_ID,
		CLOSE_ID,

		EXPEL_ID,
		ACCEPT_ID,
		DENY_ID,

		SCROLL_UP_ID,
		SCROLL_DOWN_ID,

		MODIFY_ID,

		APPOINT_ID,

		DELEGATION_OF_POWER_ID,

		EXEC_MAX,
	};

public:
	struct MEMBER_INFO
	{
		std::string NAME, INTRODUCTION;
		int guild_id;
		BYTE GRADE;
	};

private:
	int		m_scroll;
	int		m_scroll_max;
	bool	m_bl_scrolling;
	bool	m_own_info;

	MEMBER_INFO m_member_info;

	LineEditorVisual			m_lev_intro;

public:
	C_VS_UI_TEAM_MEMBER_INFO(MEMBER_INFO *info);
	~C_VS_UI_TEAM_MEMBER_INFO();

	void	Process();
	void	UnacquireMouseFocus();
	bool	IsPixel(int _x, int _y);
	void	Run(id_t id);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	Start();
	void	Finish();

	void	Show();

};

class C_VS_UI_TEAM_REGIST : public Window, public Exec, public ButtonVisual
{
private:
	ButtonGroup *				m_pC_button_group;
	C_SPRITE_PACK				m_guild_spk;
	enum GUILD_SPK
	{
		LIST_WINDOW = 0,
		INFO_WINDOW,
		HELP_BUTTON,
		HELP_BUTTON_PUSHED,
		X_BUTTON,
		X_BUTTON_PUSHED,
		SCROLL_BAR,
		SCROLL_UP_BUTTON,
		SCROLL_UP_BUTTON_PUSHED,
		SCROLL_DOWN_BUTTON,
		SCROLL_DOWN_BUTTON_PUSHED,
		SCROLL_TAG,
		BAR_DARKEN,
		BAR_HILIGHTED,
		BAR_SEARCH,
		TITLE_TEAM_REGISTRATION,
		TITLE_MEMBER_REGISTRATION,
		TITLE_TEAM_INFO,
		TITLE_MEMBER_INFO,
		TITLE_TEAM_LIST,
		TITLE_TEAM_LIST_HILIGHTED,

		SEARCH_NAME,
		SEARCH_NAME_HILIGHTED,
		SEARCH_NAME_HILIGHTED_PUSHED,
		SEARCH_LEADER,
		SEARCH_LEADER_HILIGHTED,
		SEARCH_LEADER_HILIGHTED_PUSHED,
		SEARCH_ALL1,
		SEARCH_ALL_HILIGHTED,
		SEARCH_ALL_HILIGHTED_PUSHED,
		SEARCH_SEARCH,
		SEARCH_SEARCH_HILIGHTED,
		SEARCH_SEARCH_HILIGHTED_PUSHED,

		REGISTER_BUTTON,
		REGISTER_BUTTON_HILIGHTED,
		CLOSE_BUTTON,
		CLOSE_BUTTON_HILIGHTED,
		JOIN_BUTTON,
		JOIN_BUTTON_HILIGHTED,
		EXPEL_BUTTON,
		EXPEL_BUTTON_HILIGHTED,
		ACCEPT_BUTTON,
		ACCEPT_BUTTON_HILIGHTED,
		DENY_BUTTON,
		DENY_BUTTON_HILIGHTED,		
		MODIFY_BUTTON,
		MODIFY_BUTTON_HILIGHTED,		

		INDEX_TEAM,
		INDEX_TEAM_HILIGHTED,
		INDEX_LEADER,
		INDEX_LEADER_HILIGHTED,
		INDEX_EXPIREDATE,
		INDEX_EXPIREDATE_HILIGHTED,
		INDEX_MEMBERS,
		INDEX_MEMBERS_HILIGHTED,
		INDEX_RANKING,
		INDEX_RANKING_HILIGHTED,

		TITLE_WAR_LIST,
		TITLE_WAR_LIST_HILIGHTED,
		INDEX_WAR_CHALLENGER,
		INDEX_WAR_CHALLENGER_HILIGHTED,
		INDEX_WAR_DATE,
		INDEX_WAR_DATE_HILIGHTED,

		INDEX_UNITED_JOIN,				// ���հ���
		INDEX_UNITED_JOIN_HILIGHTED,	
		INDEX_UNITED_WITHDRAWAL,		// ����Ż��
		INDEX_UNITED_WITHDRAWAL_HILIGHTED,
		INDEX_UNITED_EXPEL,				// �����߹�
		INDEX_UNITED_EXPEL_HILIGHTED,
		INDEX_GUILD_APPOINT,			// �Ӹ�
		INDEX_GUILD_APPOINT_HILIGHTED,


	};

	enum EXEC_ID
	{
		HELP_ID,
		X_ID,
		CLOSE_ID,
		REGIST_ID,

		EXEC_MAX,
	};

private:
	POINT	m_vampire_point;
	bool	m_bl_member;
	int		m_scroll;
	bool	m_bl_scrolling;
	int		m_print_x, m_print_y, m_print_gap;
	int		m_mark_x, m_mark_y;

	LineEditorVisual			m_lev_intro, m_lev_team_name;

	int		m_guild_id;
	int		m_reg_fee, m_rank;
	std::string m_team_name, m_introduction, m_date;
	CDirectDrawSurface				*m_guild_mark;

public:
	C_VS_UI_TEAM_REGIST(bool member, int reg_fee, int rank, char *team_name = NULL, char *date = NULL, int guild_id = -1); // team_name�� member�� ��쿡��
	~C_VS_UI_TEAM_REGIST();

	void	Process();
	void	UnacquireMouseFocus();
	bool	IsPixel(int _x, int _y);
	void	Run(id_t id);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	bool	ReturnTeamRegistMember(){return m_bl_member;}

	void	Start();
	void	Finish();

	void	Show();

};

//-----------------------------------------------------------------------------
// C_VS_UI_INFO
//
// 
//-----------------------------------------------------------------------------
class C_VS_UI_OTHER_INFO : public Window, public Exec, public ButtonVisual
{
private:
//	C_SPRITE_PACK			*m_pC_info_spk;
	ButtonGroup				*m_pC_button_group;
	CSprite					*m_p_guild_mark;
	//CSprite					*m_p_guild_mark;
	struct PETINFO			*m_pPetInfo;
	C_SPRITE_PACK			m_petButtonSpk;
	// TEST: OtherCharInfo.spk. Orijinal DrawDialog+Info*.spk yolu silinmedi.
	C_SPRITE_PACK			m_otherCharInfoSpk;
	bool					m_bOtherCharInfoTestReady;

	void	ShowOtherCharInfoTest(const char* align_temp[5], const char* Origin_Server[5], const char* grade[20]);
	void	MouseOtherCharInfoTest(int local_x, int local_y);

#if __CONTENTS(__080405_FIREST_UI_UPDATE)
	enum EXEC_ID
	{
		ALPHA_ID,
		HELP_ID,
		CLOSE_ID,
		PET_INFO_ID,
	};
#else
	enum EXEC_ID
	{
		ALPHA_ID,

		CLOSE_ID,
		HELP_ID,
		PET_INFO_ID,
	};
#endif //__080405_FIREST_UI_UPDATE

public:
	struct PLAYER_INFO
	{
		void Init();

		std::string PLAYER_NAME;
		int DOMAINLEVEL[5];
		int GRADE, LEVEL, FAME;
		int ALIGNMENT;
		int STR_CUR, DEX_CUR, INT_CUR;
		int STR_PURE, DEX_PURE, INT_PURE;
		int guild_id;
		int face_type;
		bool bl_female;	
		
		int	 Origin_ServerIdx ;
		std::string PLAYER_OLDNAME;
		DWORD contributePoint;
	};
private :

	CSpritePack *				m_p_face;
//	C_SPRITE_PACK				*m_p_face_spk;
	PLAYER_INFO					m_player_info;

	int TopDomain;
	
public :

	C_VS_UI_OTHER_INFO();
	~C_VS_UI_OTHER_INFO();

	void	SetPetInfo(struct PETINFO *pPetInfo);
	void	RefreshImage();

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
	void	SetOtherInfo(PLAYER_INFO &info);
	const char*		GetCurrentOtherInfoName();

	void	Show();
	void	Process();
	void	Start();
	void	Finish();
	bool	IsPixel(int _x, int _y);
	//bool	IsRunningOtherInfo();
};

//-----------------------------------------------------------------------------
// C_VS_UI_TRACE
//
// 
//-----------------------------------------------------------------------------
class C_VS_UI_TRACE : public Window, public Exec, public ButtonVisual
{
private:
	bool status;
	C_SPRITE_PACK			*m_pC_trace_spk;
	ButtonGroup				*m_pC_button_group;

	enum EXEC_ID
	{
		OK_ID,
		CLOSE_ID,
		HELP_ID,
	};

	enum TRACE_SPK
	{
		MAIN_WINDOW = 0,
		TRACE_START,
		TRACE_START_HILIGHTED,
		TRACE_START_PUSHED,
		TRACE_CANCEL,
		TRACE_CANCEL_HILIGHTED,
		TRACE_CANCEL_PUSHED,
	};

	LineEditorVisual		m_lev_name;	
	std::string					m_status_msg;
	std::string					name;
public :

	C_VS_UI_TRACE();
	~C_VS_UI_TRACE();

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
	bool	IsStartTrace();
	void	SetCannotTrace();
};

//-----------------------------------------------------------------------------
// C_VS_UI_TRACE
//
// 
//-----------------------------------------------------------------------------
class C_VS_UI_XMAS_CARD : public Window, public Exec, public ButtonVisual
{
private:
	C_SPRITE_PACK			*m_pC_card_spk;
	ButtonGroup				*m_pC_button_group;
	bool					status;

	enum EXEC_ID
	{
		OK_ID,
		CANCEL_ID,
	};

	enum TRACE_SPK
	{
		MAIN_WINDOW = 0,
		BUTTON_OK,
		BUTTON_OK_HILIGHTED,
		BUTTON_OK_PUSHED,
		BUTTON_CANCEL,
		BUTTON_CANCEL_HILIGHTED,
		BUTTON_CANCEL_PUSHED,
	};

//	LineEditorVisual		m_lev_from;	
	LineEditorVisual		m_lev_to;	
	LineEditorVisual		m_lev_message;	

	MItem					*m_pItem;
	std::string				m_szTreeMessage;


public :

	C_VS_UI_XMAS_CARD(const MItem *pItem);
	~C_VS_UI_XMAS_CARD();

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
};

//-----------------------------------------------------------------------------
// class C_VS_UI_BRING_FEE
//
// ���� ��������
//
//-----------------------------------------------------------------------------
class C_VS_UI_BRING_FEE : public Window, public Exec, public ButtonVisual
{
private :
	UINT		m_TotalFee;
	UINT		m_BringFee;
	int		m_money_x,m_money_y;

	ButtonGroup			*m_pC_button_group;

	enum EXEC_ID
	{
		OK_ID,
		CANCEL_ID,
		MONEY_ID,
	};

public :
//	static	C_VS_UI_MONEY_DIALOG		*m_pC_bring_fee_dialog;
	C_VS_UI_BRING_FEE(UINT totalfee = 0,UINT bringfee = 0);
	~C_VS_UI_BRING_FEE();

	void	Show();
	void	Start();
	void	Finish();
	void	Process();
	void	Run(id_t id);

	void	SetTotalFee(UINT fee) {m_TotalFee = fee; }
	void	SetBringFee(UINT fee) {m_BringFee = fee; }
	UINT		GetTotalFee() {return m_TotalFee;}
	UINT		GetBringFee() {return m_BringFee;}

	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	bool	IsPixel(int _x,int _y);

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
};

class C_VS_UI_WAR_LIST : public Window, public Exec, public ButtonVisual
{
	ButtonGroup *				m_pC_button_group;
	C_SPRITE_PACK				m_guild_spk;
	enum GUILD_SPK
	{
		LIST_WINDOW = 0,
		INFO_WINDOW,
		HELP_BUTTON,
		HELP_BUTTON_PUSHED,
		X_BUTTON,
		X_BUTTON_PUSHED,
		SCROLL_BAR,
		SCROLL_UP_BUTTON,
		SCROLL_UP_BUTTON_PUSHED,
		SCROLL_DOWN_BUTTON,
		SCROLL_DOWN_BUTTON_PUSHED,
		SCROLL_TAG,
		BAR_DARKEN,
		BAR_HILIGHTED,
		BAR_SEARCH,
		TITLE_TEAM_REGISTRATION,
		TITLE_MEMBER_REGISTRATION,
		TITLE_TEAM_INFO,
		TITLE_MEMBER_INFO,
		TITLE_TEAM_LIST,
		TITLE_TEAM_LIST_HILIGHTED,

		SEARCH_NAME,
		SEARCH_NAME_HILIGHTED,
		SEARCH_NAME_HILIGHTED_PUSHED,
		SEARCH_LEADER,
		SEARCH_LEADER_HILIGHTED,
		SEARCH_LEADER_HILIGHTED_PUSHED,
		SEARCH_ALL1,
		SEARCH_ALL_HILIGHTED,
		SEARCH_ALL_HILIGHTED_PUSHED,
		SEARCH_SEARCH,
		SEARCH_SEARCH_HILIGHTED,
		SEARCH_SEARCH_HILIGHTED_PUSHED,

		REGISTER_BUTTON,
		REGISTER_BUTTON_HILIGHTED,
		CLOSE_BUTTON,
		CLOSE_BUTTON_HILIGHTED,
		JOIN_BUTTON,
		JOIN_BUTTON_HILIGHTED,
		EXPEL_BUTTON,
		EXPEL_BUTTON_HILIGHTED,
		ACCEPT_BUTTON,
		ACCEPT_BUTTON_HILIGHTED,
		DENY_BUTTON,
		DENY_BUTTON_HILIGHTED,		
		MODIFY_BUTTON,
		MODIFY_BUTTON_HILIGHTED,		

		INDEX_TEAM,
		INDEX_TEAM_HILIGHTED,
		INDEX_LEADER,
		INDEX_LEADER_HILIGHTED,
		INDEX_EXPIREDATE,
		INDEX_EXPIREDATE_HILIGHTED,
		INDEX_MEMBERS,
		INDEX_MEMBERS_HILIGHTED,
		INDEX_RANKING,
		INDEX_RANKING_HILIGHTED,

		TITLE_WAR_LIST,
		TITLE_WAR_LIST_HILIGHTED,
		INDEX_WAR_CHALLENGER,
		INDEX_WAR_CHALLENGER_HILIGHTED,
		INDEX_WAR_DATE,
		INDEX_WAR_DATE_HILIGHTED,

		INDEX_UNITED_JOIN,				// ���հ���
		INDEX_UNITED_JOIN_HILIGHTED,	
		INDEX_UNITED_WITHDRAWAL,		// ����Ż��
		INDEX_UNITED_WITHDRAWAL_HILIGHTED,
		INDEX_UNITED_EXPEL,				// �����߹�
		INDEX_UNITED_EXPEL_HILIGHTED,
		INDEX_GUILD_APPOINT,			// �Ӹ�
		INDEX_GUILD_APPOINT_HILIGHTED,


	};
	enum EXEC_ID
	{
		HELP_ID,
		X_ID,
		CLOSE_ID,
		
		SCROLL_UP_ID,
		SCROLL_DOWN_ID,

		WAR_LIST_ID,
		INDEX_CHALLENGER_ID,
		INDEX_DATE_ID,
		
		EXEC_MAX,
	};
public:
	class WarInfo
	{
		public:
		WarInfo()
		{
			for(int i = 0; i< 5; i++)
				challengerGuildID[i] = 0;
			reinforceGuildID = 0;
		};
	
		BYTE warType;		// 0 : ������ 1 : ������
		WORD year;
		BYTE month;
		BYTE day;
		BYTE hour;
		WORD		challengerGuildID[5];
		std::string challengerGuildName[5];
		WORD		reinforceGuildID;
		std::string	reinforceGuildName;
	};
	
	struct _WarList
	{
		BYTE warType;		// 0 : ������ 1 : ������
		WORD year;
		BYTE month;
		BYTE day;
		BYTE hour;
		WORD		challengerGuildID;
		std::string challengerGuildName;
	};
private:
	std::vector<_WarList>			m_v_war_list;
	POINT							m_vampire_point;
	bool							m_bl_scrolling;
	int								m_scroll;
	
public:
	C_VS_UI_WAR_LIST();
	~C_VS_UI_WAR_LIST();


	void	AddWarList(const WarInfo	&war_info);
	void	ClearWarList();
	
	void	Process();
	void	UnacquireMouseFocus();
	bool	IsPixel(int _x, int _y);
	void	Run(id_t id);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	bool	AllMatchWindowPixel(int _x, int _y) const;

	void	Start();
	void	Finish();

	void	Show();
};

//-----------------------------------------------------------------------------
// C_VS_UI_INFO
//
// 
//-----------------------------------------------------------------------------
class C_VS_UI_BLOOD_BIBLE_STATUS : public Window, public Exec, public ButtonVisual
{
private:
	ButtonGroup				*m_pC_button_group;
	
	enum EXEC_ID
	{
		PUSHPIN_ID,
		ALPHA_ID,
		CLOSE_ID,
		HELP_ID,
	};
	
public :

	enum BLOOD_BIBLE_STATUS_INFO
	{
		BLOOD_BIBLE_NONE,
		BLOOD_BIBLE_DROP,
		BLOOD_BIBLE_HAS_SLAYER,
		BLOOD_BIBLE_HAS_VAMPIRE,
		BLOOD_BIBLE_SHRINE_SLAYER,
		BLOOD_BIBLE_SHRINE_VAMPIRE,
		BLOOD_BIBLE_SHRINE_OUSTERS,
		BLOOD_BIBLE_HAS_OUSTERS,
	};
	struct BLOOD_BIBLE_STATUS
	{
		int zone_id;
		int x;
		int y;
		BYTE shrine_race;
		BLOOD_BIBLE_STATUS_INFO status;
		//BYTE bonus;										-_- �Ⱦ���. 
		std::string name;
	};

	BLOOD_BIBLE_STATUS		m_BloodBibleStatus[12];

	DWORD					m_tickCount;
	DWORD					m_sec;

public :

	C_VS_UI_BLOOD_BIBLE_STATUS();
	~C_VS_UI_BLOOD_BIBLE_STATUS();

	void	SetTimer(DWORD sec) { m_sec = sec; m_tickCount = timeGetTime(); }

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

	void	SetBloodBible(int blood_bible_id, BLOOD_BIBLE_STATUS &blood_bible_status);
	void	UnSetBloodBible(int blood_bible_id);

	void	Show();
	void	Process();
	void	Start();
	void	Finish();
	bool	IsPixel(int _x, int _y);
};

//-----------------------------------------------------------------------------
// class C_VS_UI_BRING_FEE
//
// �̸� �Է��ϱ�
// -_- �ٿ뵵�� ���ô�.
//-----------------------------------------------------------------------------
class C_VS_UI_INPUT_NAME : public Window, public Exec, public ButtonVisual
{
public :
	enum INPUT_NAME_MODE_LIST
	{
		INPUT_NAME_HOPE_COUPLE = 0,					// Ŀ�� �����
		INPUT_NAME_BREAK_UP,					// Ŀ�� �Ằ��		
		INPUT_NAME_FORCE_BREAK_UP,

		INPUT_NAME_MAX,
	};

	enum	INPUT_STATUS_LIST
	{
		INPUT_STATUS_NORMAL,
		INPUT_STATUS_DISABLE,
		INPUT_STATUS_MAX,
		INPUT_STATUS_NULL,
	};
private :
	LineEditorVisual		m_lev_name;	
	int		m_name_x,m_name_y;
	INPUT_NAME_MODE_LIST	m_Mode;
	INPUT_STATUS_LIST		m_Status;

	ButtonGroup			*m_pC_button_group;

	std::string			m_name,m_status;

	enum EXEC_ID
	{
		OK_ID,
		CANCEL_ID,		
	};

public :	
//	static	C_VS_UI_MONEY_DIALOG		*m_pC_bring_fee_dialog;
	C_VS_UI_INPUT_NAME(INPUT_NAME_MODE_LIST mode = INPUT_NAME_HOPE_COUPLE);
	~C_VS_UI_INPUT_NAME();

	void	Show();
	void	Start();
	void	Finish();
	void	Process();
	void	Run(id_t id);

	const char* GetCurrentName();
	void	SetName(const char* name);

	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	bool	IsPixel(int _x,int _y);

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

	void	SetEnable() { m_Status = INPUT_STATUS_NORMAL; }
	void	SetDisable() { m_Status = INPUT_STATUS_DISABLE; }
};

//-----------------------------------------------------------------------------
// class C_VS_UI_BRING_FEE
//
// �̸� �Է��ϱ�
// -_- �ٿ뵵�� ���ô�.
//-----------------------------------------------------------------------------
class C_VS_UI_POPUP_MESSAGE : public Window, public Exec, public ButtonVisual
{
public :
	enum POPUP_TYPE
	{		
		POPUP_NORMAL,
		POPUP_LARGE,
		POPUP_WELCOME,
		POPUP_EVENT_QUEST,
		POPUP_MINI_WINDOW,
		POPUP_ASK_BEGINNER_ZONE,			// �ʺ��� ������ ����?
		
		POPUP_MAX,
		POPUP_NULL,
	};

	enum EXEC_ID
	{
		OK_ID,
		CANCEL_ID,
	};

	enum
	{
		STYLE_OK_BUTTON = 1,
		STYLE_CANCEL_BUTTON = 2,
		STYLE_JOIN_BUTTON = 4,
		STYLE_ABSENCE_BUTTON = 8,
	};
	
	struct PopupWindowStyle
	{
		short			WindowX,WindowY;
		POPUP_TYPE		Type;
		DWORD			Style;
		DWORD			SendID;
		std::string		str;

		DWORD			Value1;
		DWORD			Value2;
		
		void*			pValue;
	};

private :
	std::string				m_Str;
	POPUP_TYPE				m_Type;
	CSpritePack				m_p_NPCFace;
	DWORD					m_SendID;

	ButtonGroup			*m_pC_button_group;

public :	
//	static	C_VS_UI_MONEY_DIALOG		*m_pC_bring_fee_dialog;
	C_VS_UI_POPUP_MESSAGE(const char *str, POPUP_TYPE type = POPUP_NORMAL);
	C_VS_UI_POPUP_MESSAGE(PopupWindowStyle &style);
	~C_VS_UI_POPUP_MESSAGE();

	void	Show();
	void	Start();
	void	Finish();
	void	Process();
	void	Run(id_t id);
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	bool	IsPixel(int _x,int _y);

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
};

//-----------------------------------------------------------------------------
// class C_VS_UI_LOTTERY_CARD
//
//
//-----------------------------------------------------------------------------
class C_VS_UI_LOTTERY_CARD : public Window, public Exec, public ButtonVisual
{
public :

	enum LOTTERY_STATUS
	{
		LOTTERY_TYPE_READY,
		LOTTERY_TYPE_WAIT_CLIENT,
		LOTTERY_TYPE_SCRATCH,
		LOTTERY_TYPE_CLEARING,		
		LOTTERY_TYPE_CLOSE,
		LOTTERY_TYPE_MAX,		
	};

	int						m_step;
	C_SPRITE_PACK *			m_pC_lottery_spk;
	CSpriteSurface *		m_p_cover_surface;
	LOTTERY_STATUS			m_Type;	
	int						m_radio_select;
	int						m_line_select;
	bool					m_bl_scratch_enable;
	bool					m_bl_press;
	int						m_backimage[3];
	bool					m_bl_result;
	int						m_old_point_x;
	int						m_old_point_y;


private :
	
	struct GiftInfo
	{
		DWORD	id;
		BOOL	Active;
		std::string name;
		int		image;
	};

	std::vector<GiftInfo*>	m_GiftList;
	ButtonGroup			*m_pC_button_group;
	ButtonGroup			*m_pC_radio_group;

	enum EXEC_ID
	{
		BUTTON_ID,
		RADIO1,
		RADIO2,
		RADIO3,
		RADIO4,
		RADIO5,
	};

	enum LOTTERY_SPK
	{
		BACK = 0,
		BOTTOM,
		COVER,
		LOTTERY_READY,
		LOTTERY_READY_HILIGHTED,
		LOTTERY_READY_PUSHED,		
		LOTTERY_CLOSE,
		LOTTERY_CLOSE_HILIGHTED,
		LOTTERY_CLOSE_PUSHED,
		RADIO_BACK,
		RADIO_CHECK,
		
		GIFT_TICKET_1,
		DARKEDEN_TSHIRTS,
		DARKEDEN_PENDANT,
		DARKEDEN_TICKET_1,
		INLINE_SKATE,
//		VOICE_MAN,
//		MP3_PLAYER,
		SPEAKER,
		GIFT_TICKET_2,
		WATCH,
//		DVD_COMBO,
		CAMCORDER,
		CAMERA,
		GIFT_TICKET_3,
		HOME_THEATER,
		FLAT_TV,
		TFT_MONITOR,
		COMPUTER,
		DIGITAL_TV,
		DARKEDEN_TICKET_2,
		REFRIGERATOR,		
		DVD_PLAYER,
		HEELYS,
		IRIVER_395,
	};

public :	

	C_VS_UI_LOTTERY_CARD(int step = -1);
	~C_VS_UI_LOTTERY_CARD();

	void	Show();
	void	Start();
	void	Finish();
	void	Process();
	void	Run(id_t id);

	void	ClearGiftList();
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	bool	IsPixel(int _x,int _y);

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);
	void	AcquireDisappear() {}
	void	SetResult(bool	bSuccess);
	void	UnacquireMouseFocus()
	{
		m_pC_button_group->UnacquireMouseFocus();
	}
	void	CancelPushState() 
	{
		m_pC_button_group->CancelPushState();
	}

	void	ScratchCover(int _x, int _y);
	void	ClearingCover();
	void	ClearAllCover();
	int		PeelRate();	
	void	SetScratchEnable();
	void	LoadInfo(int &step);
	void	ShowDesc(int strX,int strY,const char *str);
};

//-----------------------------------------------------------------------------
// class C_VS_UI_ITEM_LIST
//
// �̸� �Է��ϱ�
// -_- �ٿ뵵�� ���ô�.
//-----------------------------------------------------------------------------
class C_VS_UI_ITEM_LIST : public Window, public Exec, public ButtonVisual
{
public :
	enum EXEC_ID
	{
		NEXT_ID,
		PREV_ID,
		BRING_ID,

		OK_ID,
		CANCEL_ID,
	};
	
	struct ItemList 
	{
		MItem*	pItem;
		DWORD	TimeLimit;
	};

	static MItem*				m_pItem;

private :
	std::string					m_Str;
	std::vector<ItemList>		m_ItemList;
	int							m_CurrentItem;
	int							m_MouseFocus;

	ButtonGroup					*m_pC_button_group;
	C_VS_UI_SCROLL_BAR			*m_pC_scroll_bar;

public :	
	C_VS_UI_ITEM_LIST();
	~C_VS_UI_ITEM_LIST();

	void	Show();
	void	ShowDesc( ItemList pItem );
	std::string		GetItemName( int Focus );

	void	Start();
	void	Finish();
	void	Process();
	void	Run(id_t id);
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	bool	IsPixel(int _x,int _y);

	bool	AddItem( ItemList pItem );
	bool	DeleteItem( DWORD ID );
	void	ClearItem();

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
	
};

//-----------------------------------------------------------------------------
// class C_VS_UI_IMAGE_NOTICE
//
// �̸� �Է��ϱ�
// -_- �ٿ뵵�� ���ô�.
//-----------------------------------------------------------------------------
class C_VS_UI_IMAGE_NOTICE : public Window, public Exec, public ButtonVisual
{
public :	
	struct SNotice
	{
		DWORD				m_Time;
		std::string			m_Name;
		std::string			m_GiftName;
		char				m_Alpha;
	};

private :
	int		m_CloseTime;
		
	C_SPRITE_PACK *			m_pC_image_spk;
		
	std::vector<SNotice*>			m_NoticeList;

public :		

	C_VS_UI_IMAGE_NOTICE();
	~C_VS_UI_IMAGE_NOTICE();

	void	Show();
	std::string	LoadInfo(DWORD ID);
	void	AddNotice( const char* name, DWORD id );
	void	ClearNotice();
	void	DeleteNotice(int n);

	void	Start();
	void	Finish();
	void	Process();
	void	Run(id_t id);
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	bool	IsPixel(int _x,int _y);

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);
	void	AcquireDisappear() {}
	void	UnacquireMouseFocus()
	{
	}
	void	CancelPushState() 
	{
	}
};

//-----------------------------------------------------------------------------
// C_VS_UI_TRACE
//
// 
//-----------------------------------------------------------------------------
class C_VS_UI_BULLETIN_BOARD : public Window, public Exec, public ButtonVisual
{
private:
	C_SPRITE_PACK			*m_pC_back_spk;
	ButtonGroup				*m_pC_button_group;
	bool					status;

	enum EXEC_ID
	{
		OK_ID,
		CANCEL_ID,
	};

	enum TRACE_SPK
	{
		MAIN_WINDOW = 0,
		BUTTON_OK,
		BUTTON_OK_HILIGHTED,
		BUTTON_OK_PUSHED,
		BUTTON_CANCEL,
		BUTTON_CANCEL_HILIGHTED,
		BUTTON_CANCEL_PUSHED,
	};

	LineEditorVisual		m_lev_from;	
	LineEditorVisual		m_lev_message;	

	MItem					*m_pItem;
	std::string				m_szTreeMessage;

	int						m_pos_from_x;
	int						m_pos_from_y;
	int						m_pos_msg_x;
	int						m_pos_msg_y;

public :

	C_VS_UI_BULLETIN_BOARD(const MItem *pItem);
	~C_VS_UI_BULLETIN_BOARD();

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
};

//-----------------------------------------------------------------------------
// class C_VS_UI_REQUEST_RESURRECT
//
//-----------------------------------------------------------------------------
class C_VS_UI_REQUEST_RESURRECT : public Window
{
public :
	enum IMAGE_SPK_INDEX
	{
		RESURRECT_GUARD_SLAYER,
		RESURRECT_GUARD_VAMPIRE,
		RESURRECT_GUARD_OUSTERS,
		PARTY_REQUEST,
		PARTY_INVITE,
		RESURRECT_SLAYER,
		RESURRECT_VAMPIRE,
		RESURRECT_OUSTERS,
		LVU_BUTTON,
		LVU_BUTTON_OUSTERS,
		RESURRECT_SCROLL,
		ELIXIR_SCROLL,
		ETERNITY,
		SKILL_FROM_OTHER_SLAYER,
		SKILL_FROM_OTHER_OUSTERS,
		SKILL_FROM_OTHER_VAMPIRE,
	};

	enum RESURRECT_MODE
	{
		RESURRECT_MODE_RESURRECT_SCROLL = 0,
		RESURRECT_MODE_ELIXIR,
		RESURRECT_MODE_ETERNITY,
		// 2004, 11, 16, sobeit add start
		RESURRECT_MODE_SIEGE_ATTACKER,
		// 2004, 11, 16, sobeit add end
		RESURRECT_MODE_SKILL_FROM_OTHER,	// �ٸ� ����� ��Ȱ���� �ַ��Ѵ�.
		RESURRECT_MODE_MAX
	};

	class CResurrect
	{
	public :
		CResurrect() : m_Delay(0), m_Time(0) { memset(&m_ButtonRect, 0, sizeof(RECT) ); m_Enable = false; m_Image = -1; }
		void		SetRect( int x,int y,int w,int h ) { m_ButtonRect.left = x; m_ButtonRect.right = x+w; m_ButtonRect.top = y; m_ButtonRect.bottom = y+h;}

		RECT		m_ButtonRect;
		bool		m_Enable;
		int			m_Image;
		int			m_Delay;
		DWORD		m_Time;
	};

	C_SPRITE_PACK				m_image_spk;

	CResurrect					m_ResurrectButton[ RESURRECT_MODE_MAX ];

	int							m_focus;
	int							m_pushed;

public:
	C_VS_UI_REQUEST_RESURRECT(bool resurrect, bool elixir, bool eternity, bool IsSiegeAttacker, bool IsSkillFromOther);
	~C_VS_UI_REQUEST_RESURRECT();

	void	Show();
	bool	IsPixel(int _x, int _y);
	void	Start();
	void	Finish();
	bool	MouseControl(UINT message, int _x, int _y);
	void	CancelPushState() { m_pushed = -1; }
	void	UnacquireMouseFocus() { m_pushed = -1; }
	void	SetDelay( RESURRECT_MODE mode, int delay );
	void	Process();
	void	RequestResurrectSetButton(int resurrectMode, bool enable);
};

//-----------------------------------------------------------------------------
// C_VS_UI_MIXING_FORGE
// 
//-----------------------------------------------------------------------------
class C_VS_UI_MIXING_FORGE : public Window, public Exec, public ButtonVisual
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
		BUTTON_CANCEL,
		BUTTON_CANCEL_HILIGHTED,
		BUTTON_CANCEL_PUSHED,
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

	enum FORGE_CLASS
	{
		CLASS_WEAPON,
		CLASS_ARMOR,
		CLASS_ACCESSORY,
		CLASS_COREZAP,
		CLASS_CUEOFADAM,
	};

	enum FORGE_TYPE
	{
		TYPE_A,
		TYPE_B,
		TYPE_C,
		TYPE_D,
		TYPE_E,
	};

	FORGE_CLASS				m_forge_class;
	FORGE_TYPE				m_forge_type;

	C_VS_UI_MIXING_FORGE( FORGE_CLASS forge_class, FORGE_TYPE forge_type );
	~C_VS_UI_MIXING_FORGE();

	bool	IsCorrectClass(ITEM_CLASS item_class);
	bool	IsCorrectType(TYPE_ITEMTYPE item_type, ITEM_CLASS item_class);
	bool	IsCorrectOption(MItem *p_item1, MItem *p_item2);

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

//-----------------------------------------------------------------------------
// class C_VS_UI_REMOVE_OPTION
//
//-----------------------------------------------------------------------------
class C_VS_UI_REMOVE_OPTION : public Window, public Exec, public ButtonVisual
{
public:
	enum TYPE
	{
		TYPE_A,
		TYPE_B,
		TYPE_C,
	};

	enum STATUS
	{
		STATUS_SELECT_OPTION,
		STATUS_QUESTION,
	};

	int					m_select_option;
	STATUS				m_status;
	std::string			m_str_option[2];
	bool				m_b_press;
	const MItem		*m_pMouseItem,*m_pCurItem;
	
private:

	ButtonGroup			*m_pC_button_group;
	bool				m_IsBokjory;

	
	enum EXEC_ID
	{
		OK_ID,
		CANCEL_ID,
	};

public:
	C_VS_UI_REMOVE_OPTION(const MItem *pItem, const MItem *pCurItem);
	~C_VS_UI_REMOVE_OPTION();

	static	bool	IsCanRemoveOption_Puritas(const MItem* pItem, const MItem* pTItem);

	void	Show();
	void	Start();
	void	Finish();
	void	Process();
	void	Run(id_t id);

	bool	IsPixel(int _x, int _y);	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);
	void	CheckOption();
	
};

//-----------------------------------------------------------------------------
// class C_VS_UI_OUSTERS_SKILL_INFO
//
//-----------------------------------------------------------------------------
class C_VS_UI_OUSTERS_SKILL_INFO : public C_VS_UI_DESC, public Window, public Exec, public ButtonVisual
{
public:
private:
	
	ButtonGroup			*m_pC_button_group;
	C_VS_UI_SCROLL_BAR			*m_pC_scroll_bar;
	bool				m_bDownSkill;	
	C_SPRITE_PACK		m_SPK;
	
	enum EXEC_ID
	{
		CLOSE_ID,
		HELP_ID,
		ALPHA_ID,
		LEARN_ID,
	};
	int		m_skillID;

public:
	C_VS_UI_OUSTERS_SKILL_INFO(int skillID, int window_x, int window_y,bool DownSkill = false);
	~C_VS_UI_OUSTERS_SKILL_INFO();
	
	void	SetSkillID(int skillID);
	void	Show();
	void	Start();
	void	Finish();
	void	Process();
	void	Run(id_t id);
	
	bool	IsPixel(int _x, int _y);	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);
	
};


//-----------------------------------------------------------------------------
// class C_VS_UI_HORN
//
//-----------------------------------------------------------------------------
class C_VS_UI_HORN : public Window, public Exec, public ButtonVisual
{
public:
private:
	
	ButtonGroup			*m_pC_button_group;
	
	C_SPRITE_PACK		m_SPK, m_MapSPK;
	
	enum EXEC_ID
	{
		CLOSE_ID,
		UP_ID,
		DOWN_ID,
	};

	enum SPRITE_ID
	{
		MAIN_WINDOW,
		UD_BUTTON,
		UD_BUTTON_HILIGHTED,
		UD_BUTTON_PUSHED,
		CLOSE_BUTTON,
		CLOSE_BUTTON_HILIGHTED,
		CLOSE_BUTTON_PUSHED,
	};

	int		m_CurrentZone;
	int		m_MapX, m_MapY;
	int		m_focus;
	int		m_Cost;

//	UI_PORTAL_VECTOR		m_portal;
	
public:
	C_VS_UI_HORN(int currentZoneID);
	~C_VS_UI_HORN();
	
	void	Show();
	void	Start();
	void	Finish();
	void	Process();
	void	Run(id_t id);
	
	bool	IsPixel(int _x, int _y);	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);

	int		GetNext(int map, bool bUp);	
};

//-----------------------------------------------------------------------------
// class C_VS_UI_FINDING_MINE
//
//-----------------------------------------------------------------------------
class C_VS_UI_FINDING_MINE : public Window, public Exec, public ButtonVisual
{
public:
private:
	
	ButtonGroup			*m_pC_button_group;
	
	enum EXEC_ID
	{
		CLOSE_ID,
		START_ID,
		LEVEL0_ID,
		LEVEL1_ID,
		LEVEL2_ID,
	};
	
//	enum SPRITE_ID
//	{
//		MAIN_WINDOW,
//			UD_BUTTON,
//			UD_BUTTON_HILIGHTED,
//			UD_BUTTON_PUSHED,
//			CLOSE_BUTTON,
//			CLOSE_BUTTON_HILIGHTED,
//			CLOSE_BUTTON_PUSHED,
//	};


	enum GAME_SIZES
	{
		BLOCK_WIDTH = 16,
		BLOCK_HEIGHT = 16,
		BLOCK_START_X = 12,
		BLOCK_START_Y = 96,
	};

	enum GAMELEVEL
	{
		GAMELEVEL_0,
		GAMELEVEL_1,
		GAMELEVEL_2,

		GAMELEVEL_MAX,
	};

	enum GAME_STATUS
	{
		GAMESTATUS_GAME,	// ������
		GAMESTATUS_DIE,		// ���� ���� ������-��-
		GAMESTATUS_CLEAR,	// ���� ���� �� ã�Ƽ�
		GAMESTATUS_READY,	// �� ����
	};

	enum LEFTMINE
	{
		LEFTMINE_LEVEL0 = 10,
		LEFTMINE_LEVEL1 = 40,
		LEFTMINE_LEVEL2 = 99,
	};

	enum BOARDSIZE
	{
		BOARDSIZE_LEVEL0_WIDTH = 10,
		BOARDSIZE_LEVEL0_HEIGHT = 10,
		BOARDSIZE_LEVEL1_WIDTH = 16,
		BOARDSIZE_LEVEL1_HEIGHT = 16,
		BOARDSIZE_LEVEL2_WIDTH = 30,
		BOARDSIZE_LEVEL2_HEIGHT = 16,
	};

	enum BLOCK_STATUS
	{
		BLOCKSTATUS_CLOSE,
		BLOCKSTATUS_OPEN,
		BLOCKSTATUS_QUESTION,
		BLOCKSTATUS_FLAG,
	};

	struct BLOCK_STRUCT
	{
		BLOCK_STRUCT()	{ num = 0; status = (BLOCK_STATUS)0; bl_focused = false; }

		int num;
		BLOCK_STATUS status;
		bool		bl_focused;
	};

	DWORD	m_startTime;
	int		m_leftMineCount;
	SIZE	m_boardSize;
	GAME_STATUS	m_status;

	BLOCK_STRUCT	*m_Blocks;

	Rect	m_BlockRect;

	GAMELEVEL m_level;
	bool bLButton, bRButton;

	void	DrawBlock(int x, int y, BLOCK_STRUCT &block);
	void	DrawRect(RECT &rect, int depth, WORD color, WORD color2, WORD color3);
	void	ClearBlockFocus(int x = -1, int y = -1, int w = -1, int h = -1);
	void	OpenBlock(int x, int y, bool bCenter = false);
	bool	CheckClear();
	void	DrawNumbers(int x, int y, int number);
	void	DrawNumber(int x, int y, BYTE flag);

public:
	C_VS_UI_FINDING_MINE();
	~C_VS_UI_FINDING_MINE();
	
	void	NewGame(GAMELEVEL level);
	void	Show();
	void	Start();
	void	Finish();
	void	Process();
	void	Run(id_t id);
	
	bool	IsPixel(int _x, int _y);	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	WindowEventReceiver(id_t event);
	
};

extern C_VS_UI_WINDOW_MANAGER* gpC_vs_ui_window_manager;

#define ITEM_REF_POINT_COUNT			4

extern S_SLOT	g_char_slot_ingame;
extern Point g_item_ref_point[];

extern int g_HISTORY_LINE;	// ä��â ����

void g_SetItemRefPoint(int item_rect_w, int item_rect_h);

#endif
