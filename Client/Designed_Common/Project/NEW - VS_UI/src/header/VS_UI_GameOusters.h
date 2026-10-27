/*-----------------------------------------------------------------------------

	VS_UI_GameOusters.h

	Ousters interface.

	2000.8.11. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_GAMEOUSTERS_H__
#define __VS_UI_GAMEOUSTERS_H__

#include "VS_UI_GameCommon.h"
#include "VS_UI_skill_tree.h"

#include "MOustersGear.h"

//-----------------------------------------------------------------------------
// class C_VS_UI_OUSTERS_GEAR
//
// 
//-----------------------------------------------------------------------------

class C_VS_UI_OUSTERS_GEAR : public C_VS_UI_GEAR
{
public:
	enum SLOT_NAME
	{
		SN_CIRCLET,			// 서클릿
		SN_COAT,				// 옷
		SN_LEFTHAND,			// 왼손
		SN_RIGHTHAND,			// 오른손
		SN_BOOTS,				// 신발
		SN_ARMSBAND1,			// 암스밴드1
		SN_ARMSBAND2,			// 암스밴드2
		SN_RING1,				// 링1
		SN_RING2,				// 링2
		SN_PENDENT1,			// 목걸이1
		SN_PENDENT2,			// 목걸이2
		SN_PENDENT3,			// 목걸이3
		SN_STONE1,			// 정령석1
		SN_STONE2,			// 정령석2
		SN_STONE3,			// 정령석3
		SN_STONE4,			// 정령석4

		SN_COREZAP1,		// 코어잽1
		SN_COREZAP2,		// 코어잽2
		SN_COREZAP3,		// 코어잽3
		SN_COREZAP4,		// 코어잽4
		
		SN_FASCIA,			// 승직용 허리장식
        SN_MITTEN,			// 승직용 장갑?
		// nanomech 2006.03.09 Item 수정 작업
		SN_NECK_CHAIN,		// 에테리얼 체인

		SN_CUEOFADAM1,		// 큐오브아담1
		SN_CUEOFADAM2,		// 큐오브아담2

		SN_BLOODBIBLE1,		// 블러디바이블1
		SN_BLOODBIBLE2,		// 블러디바이블2
		SN_BLOODBIBLE3,		// 블러디바이블3
		SN_BLOODBIBLE4,		// 블러디바이블4
		SN_BLOODBIBLE5,		// 블러디바이블5
		SN_BLOODBIBLE6,		// 블러디바이블6

		SLOT_SIZE,	
	};

	C_VS_UI_OUSTERS_GEAR();
	~C_VS_UI_OUSTERS_GEAR();
private:
	
	enum GEAR_SLOT_SPK_INDEX
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

	static int					m_slot_image[SLOT_SIZE];
	Rect						m_slot_rect[SLOT_SIZE];


//	void Show();
};
//-----------------------------------------------------------------------------
// C_VS_UI_OUSTERS
//
// 
//-----------------------------------------------------------------------------
class C_VS_UI_OUSTERS : public C_VS_UI_TRIBE
{
public:
private:

	
	C_VS_UI_OUSTERS_GEAR	*	m_pC_gear;
	C_VS_UI_OUSTERS_GEAR	*	m_pC_gear_other;
	

public:
	C_VS_UI_OUSTERS();
	~C_VS_UI_OUSTERS();

	void	RunQuickItemSlot();
	void	CloseQuickItemSlot();
	bool	IsRunningQuickItemSlot();

	C_VS_UI_GEAR * GetGearWindow() const;
	void	CloseGear();
	void	OpenGear(bool bl_set_load = true);

	C_VS_UI_GEAR * GetGearOtherWindow() const;
	void	CloseGearOther();
	void	OpenGearOther(bool bl_set_load = true);

#if __CONTENTS(__GEAR_SWAP_CHANGE)		//	C_VS_UI_OUSTERS		아우스터즈 장착창 실행 함수 선언
	void	RunGearChange();
#endif //__GEAR_SWAP_CHANGE

	void	DoCommonActionBeforeEventOccured();
	void	DoCommonActionAfterEventOccured();

	void	ResetOustersQuickItemSize();

	void	Start();
	void	ShowExp();
	void	Process();
//	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);

	void	HotKey_F1();	//혈청 사용하기
	void	HotKey_F2();	//혈청 사용하기
	void	HotKey_F3();	//혈청 사용하기
	void	HotKey_F4();	//혈청 사용하기
	void	HotKey_F5();	//혈청 사용하기
	void	HotKey_F6();	//혈청 사용하기	
	void	HotKey_F7();
	void	HotKey_F8();
	void	HotKey_QuickItemSlot();

	void	Hotkey_Function(int FKey)	;
};

//-----------------------------------------------------------------------------
// C_VS_UI_OUSTERS_QUICKITEM
//
// OUSTERS only Quick item interface object.
//-----------------------------------------------------------------------------
class C_VS_UI_OUSTERS_QUICKITEM : public Window, public ButtonVisual, public Exec
{

public	:	
	enum HOTKEY
	{
		HOTKEY1,
		HOTKEY2,
		HOTKEY3,
		HOTKEY4,

		//뱀파용 핫키
		HOTKEY5,
		HOTKEY6,
		HOTKEY7,
		HOTKEY8,

		HOTKEY9,
		HOTKEY10,
		HOTKEY11,
		HOTKEY12,

		HOTKEY_MAX,
	};


private	:

	enum 
	{ 
		QUICKSLOT_TOTAL = 8,
		QUICKSLOT_W = 30,
		QUICKSLOT_H = 30,
	};

	enum EXEC_ID
	{
		ALPHA_ID,
		PUSHPIN_ID,
		ROTATE_ID,
	};

public:
	enum
	{
		SLOT_1 = 0,
		SLOT_2,
		SLOT_3,
		SLOT_4,
		SLOT_5,
		SLOT_6,
		
		SLOT_MAX
	};

private:
	enum SPK_INDEX
	{
		SLOT,
		SLOT_ALPHA,
		STATCH_EDGE,
		START_EDGE,
		START_EDGE_EXTEND,
		END_EDGE,
		END_EDGE_EXTEND,
		BUTTON_ROTATE_HILIGHTED,
		BUTTON_ROTATE_HILIGHTED_PUSHED,

		ROTATED_OFFSET,
	};
	
	int							m_Hotkey_buf[HOTKEY_MAX];


	ButtonGroup *				m_pC_button_group;

	C_SPRITE_PACK 				m_image_spk;

	int							m_p_slot_x[QUICKSLOT_TOTAL];

	int							m_focus_slot;

	bool						m_bl_width, m_bl_resize;

	bool	Click();

	int	GetPocketCount() const;

public:
	C_VS_UI_OUSTERS_QUICKITEM();
	~C_VS_UI_OUSTERS_QUICKITEM();

	void	ToggleBelt();
	void	Open();
	void	Close();
	void	AcquireDisappear() {}
	void	Use(int slot, bool bFunctionKey=true);
	void	UnacquireMouseFocus();
	void	ResetSize();
	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);
	void	CancelPushState();
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);
	void	Run(id_t id);


	void	KeyboardControl(UINT message, UINT key, long extra);
	bool	MouseControl(UINT message, int _x, int _y);
	void	Show();
	void	Start();
	void	Process();

	static MItem*	GetItem(int slot);

	void	SetHotkey(HOTKEY hotkey , int slot)	;
	void	InitHotKey(HOTKEY hotkey) ;
	int		GetFocusedSlot()	;
	int		GetSlot(HOTKEY hotkey) ; 
	int		GetHotkey(int	slot) ;


};


#endif