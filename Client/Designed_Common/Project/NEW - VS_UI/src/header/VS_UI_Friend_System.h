/*
	2009.07.13 Create VS_UI_Friend_System.h by Li,Kwon-Hwak

	Description : 09년07월15일에 패치될 새로운 컨텐츠인 친구 시스템 관련 UI와
				우측 팝업창을 선언해 놓았다.
*/

#include "VS_UI_Base.h"
#include "VS_UI_Util.h"
#include "VS_UI_widget.h"
#include "VS_UI_Control.h"

#include "ClientFunction.h"

#include "Packet/FriendGlobalOptionInfo.h"
#include "Packet/FriendIndividualOptionInfo.h"


#if __CONTENTS(__FRIEND_ADDITION)
// 친구 시스템 관련 공통 UI 이미지는 이곳에서 전역으로 선언해 주었다.
enum FRIEND_COMMON_SPK_INDEX
{
	FCSI_LEFTTOPOUTLINE_DECORATION,
	FCSI_RIGHTTOPOUTLINE_DECORATION,
	FCSI_LEFTBOTTOMOUTLINE_DECORATION,
	FCSI_RIGHTBOTTOMOUTLINE_DECORATION,
	FCSI_TOP_OUTLINE,
	FCSI_BOTTOM_OUTLINE,
	FCSI_LEFT_OUTLINE,
	FCSI_RIGHT_OUTLINE,
	FCSI_DESCRIPTION_BUTTON_NORMAL,
	FCSI_DESCRIPTION_BUTTON_PUSH,
	FCSI_CLOSE_BUTTON_NORMAL,
	FCSI_CLOSE_BUTTON_PUSH,
	FCSI_SCROLL_BACKGROUND_FRONT,
	FCSI_SCROLL_BACKGROUND_BACK,
	FCSI_SCROLL_BAR,
	FCSI_SCROLL_UP_BUTTON_NORMAL,
	FCSI_SCROLL_UP_BUTTON_PUSH,
	FCSI_SCROLL_DOWN_BUTTON_NORMAL,
	FCSI_SCROLL_DOWN_BUTTON_PUSH,
	FCSI_TOP_OUTLINE_SMALL,
	FCSI_BOTTOM_OUTLINE_SMALL,
	FCSI_LEFT_OUTLINE_SMALL,
	FCSI_RIGHT_OUTLINE_SMALL,
	FCSI_POPUP_MESSAGE_BOX,
	FCSI_POPUP_MESSAGE_BOX_OK,
	FCSI_POPUP_MESSAGE_BOX_OK_OVER,
	FCSI_POPUP_MESSAGE_BOX_OK_PUSH,
	FCSI_POPUP_MESSAGE_BOX_CANCEL,
	FCSI_POPUP_MESSAGE_BOX_CANCEL_OVER,
	FCSI_POPUP_MESSAGE_BOX_CANCEL_PUSH,
};

/*
	캐릭터 팝업 메뉴
*/
class C_VS_UI_CHARACTER_POPUP_MENU : public C_VS_UI_MENU
{
	enum COMMAND_ID
	{
		CI_TRADE,
		CI_PARTY,
		CI_ADD_FRIEND,
		CI_REQ_INFO,
	};
	enum SPK_INDEX				// 이미지 순서 - 리소스를 작업 할때 반드시 이 순서를 지키도록 한다.
	{
		SI_MAIN,				// 배경 이미지
		SI_SELECT_BOX			// 커맨드 선택 상자
	};

private:
	bool LButtonDown(int cmd);

public:
	C_VS_UI_CHARACTER_POPUP_MENU(int _x, int _y, TYPE_OBJECTID id);
	virtual ~C_VS_UI_CHARACTER_POPUP_MENU();

	// 기본적으로 상위 클래스를 상속 받아 실행 하는 함수들
	void Start();
	void Finish();
	void Show();
	
	// 외부 인터페이스 함수
	void SetSpkImage(C_SPRITE_PACK*& image, int command_count);
	void SetPosition(int _x, int _y, MENU_POSITION pos = MP_RIGHTBOTTOM);
	void AddCommand(const char* pszCommand, id_t id, void (*pExecFn)(void* void_ptr) = NULL, int style = MENU_COMMAND::CS_NONE);
	void SetSubMenu(id_t id, C_VS_UI_MENU*& pSubMenu);

private:
	TYPE_OBJECTID			m_id;					// 유저 아이디
	C_SPRITE_PACK*			m_menu_spkimage;		// 팝업 메뉴
};

/*
	친구 리스트 뷰 UI 클래스
*/
class C_VS_UI_FRIEND_LISTVIEW : public C_VS_UI_LISTVIEW
{
private:
	void DrawScrollBar();
	void GenerateVerticalScroll();
	void GenerateHorizonScorll();
	void LdbclkItemProc(LTVITEM* item);

public:
	C_VS_UI_FRIEND_LISTVIEW(Window* pParent, int _x, int _y, int _w, int _h, DWORD style = LSFBACKGROUNDNOTTRANS, SELECT_TYPE type = ST_SINGLE);
	virtual ~C_VS_UI_FRIEND_LISTVIEW();

	void Start();
	void Finish();
	void Show();
	void Process();
	bool IsPixel(int _x, int _y);
	bool MouseControl(UINT message, int _x, int _y);	

	void SetScrollSpkImage(C_SPRITE_PACK*& image);

private:
	C_SPRITE_PACK*			m_scroll_spkimage;
};

/*
	친구 대기 리스트 뷰 UI 클래스
*/
class C_VS_UI_FRIEND_WAIT_LISTVIEW : public C_VS_UI_LISTVIEW
{
private:
	void DrawScrollBar();
	void GenerateVerticalScroll();
	void GenerateHorizonScorll();

public:
	C_VS_UI_FRIEND_WAIT_LISTVIEW(Window* pParent, int _x, int _y, int _w, int _h, DWORD style = LSFBACKGROUNDNOTTRANS, SELECT_TYPE type = ST_SINGLE);
	virtual ~C_VS_UI_FRIEND_WAIT_LISTVIEW();

	void Start();
	void Finish();
	void Show();
	void Process();
	bool IsPixel(int _x, int _y);
	bool MouseControl(UINT message, int _x, int _y);	

	void SetScrollSpkImage(C_SPRITE_PACK*& image);

private:
	C_SPRITE_PACK*			m_scroll_spkimage;
};

/*
	친구 시스템 UI 클래스
*/
class C_VS_UI_FRIEND_SYSTEM : public Window, public Exec, public ButtonVisual
{
private:
	enum EXEC_ID
	{
		EIDESCRIPTION,
		EICLOSE,
		EIPAPER,
		EIMAIL,
		EIOTHERFUNC,
		EIFRIENDADD,
		EIFRIENDDEL,
		EIOPTION
	};
	// 친구 시스템의 버튼은 인덱스를 따로 해주지 않는다.
	// 이유는 버튼 그룹에서 처리해 주기 때문에...
	enum FRIEND_SYSTEM_SPK_INDEX
	{
		FSSI_BACKGROUND,
		FSSI_TITLE,
		FSSI_PAPER_NORMAL,
		FSSI_MAIL_NORMAL = 5,
		FSSI_OTHER_FUNC_NORMAL = 8,
		FSSI_FRIEND_ADD_NORMAL = 11,
		FSSI_FRIEND_DEL_NORMAL = 14,
		FSSI_OPTION_NORMAL = 17
	};

public:
	C_VS_UI_FRIEND_SYSTEM();
	virtual ~C_VS_UI_FRIEND_SYSTEM();

	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	void	Run(id_t id);
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	
	bool	IsPixel(int _x,int _y);
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	C_VS_UI_FRIEND_LISTVIEW*	GetListView() { return m_p_friend_list_view; }

	void	SetCheckConfirm(bool confirm) { m_check_box_confirm.SetCheck(confirm); m_old_check_state_confirm = m_check_box_confirm.GetCheck(); }
	void	SendOptionInfo();

private:
	bool					m_old_check_state_confirm;
	C_VS_UI_CHECKBOX		m_check_box_confirm;
	FriendGlobalOptionInfo	m_option_info;

	ButtonGroup*			m_p_button_group;
	C_VS_UI_FRIEND_LISTVIEW*		m_p_friend_list_view;
	
	C_SPRITE_PACK*			m_p_friend_common;
	C_SPRITE_PACK*			m_p_friend_system;
};

/*
	친구 정보창 UI 클래스
*/
class C_VS_UI_FRIEND_INFO : public Window, public Exec, public ButtonVisual
{
private:
	enum EXEC_ID
	{
		EIDESCRIPTION,
		EICLOSE,
		EIOK,
	};
	// 친구 정보의 버튼은 인덱스를 따로 해주지 않는다.
	// 이유는 버튼 그룹에서 처리해 주기 때문에...
	enum FRIEND_INFO_SPK_INDEX
	{
		FISI_BACKGROUND,
		FISI_TITLE,
		FISI_OK_NORMAL,
		FISI_EDIT_NORMAL = 5,
	};

	struct INFO_DATA
	{
		std::string						m_user_name;
		FriendIndividualOptionInfo		m_option_info;
	};

public:
	C_VS_UI_FRIEND_INFO();
	virtual ~C_VS_UI_FRIEND_INFO();

	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	void	Run(id_t id);
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	
	bool	IsPixel(int _x,int _y);
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	// 외부 설정 인터페이스
	void	SetUserName(const char* name) { m_user_name = name; }
	void	SetUserGrade(unsigned char grade) { m_user_grade = grade; }
	void	SetUserLevel(unsigned char level) { m_user_level = level; }
	void	SetUserAdvLevel(unsigned char level) { m_user_adv_level = level; }
	void	SetUserDomain(unsigned char domain) { m_user_domain = domain; }
	void	SetUserServer(const char* server) { m_user_server = server; }
	void	SetUserLocation(const char* location) { m_user_location = location; }
	void	SetUserSocietyID(unsigned short society_id) { m_user_society_id = society_id; }
	void	SetUserSociety(const char* society) { m_user_society = society; }
	void	SetUserPropensity(int alignment) { m_user_alignment = alignment; }
	void	SetUserContribute(int contribute) { m_user_contribute = contribute; }
	void	SetUserMemo(const char* memo) { m_friend_info_edit_box.EraseAll(); m_friend_info_edit_box.AddString(memo); }

	void	SetPaperCheck(bool check) { m_old_check_state_paper = m_check_box_paper.GetCheck(); m_check_box_paper.SetCheck(check); }
	void	SetMyInfoCheck(bool check) { m_old_check_state_my_info = m_check_box_my_info.GetCheck(); m_check_box_my_info.SetCheck(check); }

	void	SendOptionInfo();
	
private:
	unsigned char			m_user_grade;				// 계급
	unsigned char			m_user_level;				// 레벨
	unsigned char			m_user_adv_level;			// 어드벤스드 레벨
	unsigned char			m_user_domain;				// 도메인 타입 - 슬레이어 일때..
	unsigned short			m_user_society_id;			// 길드 아이디
	std::string				m_user_name;				// 이름
	std::string				m_user_server;				// 서버
	std::string				m_user_location;			// 위치
	std::string				m_user_society;				// 팀,클랜,길드
	std::string				m_user_memo;				// 메모
	int						m_user_alignment;			// 성향
	int						m_user_contribute;			// 기여도

	CRect					m_friend_edit_box;
	bool					m_old_check_state_paper;
	bool					m_old_check_state_my_info;

	INFO_DATA				m_info_data;

	PrintInfo				m_print_info;
	LineEditorVisual		m_friend_info_edit_box;
	C_VS_UI_CHECKBOX		m_check_box_paper;
	C_VS_UI_CHECKBOX		m_check_box_my_info;

	ButtonGroup*			m_p_button_group;
	C_SPRITE_PACK*			m_p_friend_common;
	C_SPRITE_PACK*			m_p_friend_info;
};

/*
	친구 대기창 UI 클래스
*/
class C_VS_UI_FRIEND_WAIT : public Window, public Exec, public ButtonVisual
{
private:
	enum EXEC_ID
	{
		EIDESCRIPTION,
		EICLOSE,
		EIADD,
		EIDEL = 12,
	};
	// 친구 대기창의 버튼은 인덱스를 따로 해주지 않는다.
	// 이유는 버튼 그룹에서 처리해 주기 때문에...
	enum FRIEND_WAIT_SPK_INDEX
	{
		FWSI_BACKGROUND,
		FWSI_TITLE,
		FWSI_DEL_NORMAL,
		FWSI_ADD_NORMAL = 5
	};

public:
	C_VS_UI_FRIEND_WAIT();
	virtual ~C_VS_UI_FRIEND_WAIT();

	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	void	Run(id_t id);
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	
	bool	IsPixel(int _x,int _y);
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	C_VS_UI_FRIEND_WAIT_LISTVIEW*	GetListView() { return m_friend_wait_list_view; }

private:
	std::string				m_str_player_name;
	ButtonGroup*			m_p_button_group;

	C_VS_UI_FRIEND_WAIT_LISTVIEW*		m_friend_wait_list_view;
	
	C_SPRITE_PACK*			m_p_friend_common;
	C_SPRITE_PACK*			m_p_friend_wait;
};

class C_VS_UI_FRIEND_SYSTEM_OPTION : public Window, public Exec, public ButtonVisual
{
private:
	enum EXEC_ID
	{
		EIOK,
		EICANCEL,
	};

public:
	C_VS_UI_FRIEND_SYSTEM_OPTION();
	virtual ~C_VS_UI_FRIEND_SYSTEM_OPTION();

	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	void	Run(id_t id);
	
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	
	bool	IsPixel(int _x,int _y);
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	void	SetConfirmCheck(bool check) { m_old_check_state_confirm = m_check_box_confirm.GetCheck(); m_check_box_confirm.SetCheck(check); }
	void	SetPaperCheck(bool check) { m_old_check_state_paper = m_check_box_paper.GetCheck(); m_check_box_paper.SetCheck(check); }
	void	SetMyInfoCheck(bool check) { m_old_check_state_my_info = m_check_box_my_info.GetCheck(); m_check_box_my_info.SetCheck(check); }

	void	SendOptionInfo();

private:
	PrintInfo				m_print_info;
	FriendGlobalOptionInfo	m_option_info;

	bool					m_old_check_state_confirm;
	bool					m_old_check_state_paper;
	bool					m_old_check_state_my_info;

	C_VS_UI_CHECKBOX		m_check_box_confirm;
	C_VS_UI_CHECKBOX		m_check_box_paper;
	C_VS_UI_CHECKBOX		m_check_box_my_info;

	ButtonGroup*			m_p_button_group;
	C_SPRITE_PACK*			m_friend_spk;
};

class C_VS_UI_FRINEND_ADD_INPUT_PLAYER : public Window, public Exec, public ButtonVisual
{
	enum SPK_ID
	{
		SI_ADDWINDOW,
		SI_CONFIRM,
		SI_OK,
		SI_CANCEL,
	};
	enum EXEC_ID
	{
		EIOK,
		EICANCEL,
	};

public:
	C_VS_UI_FRINEND_ADD_INPUT_PLAYER();
	virtual ~C_VS_UI_FRINEND_ADD_INPUT_PLAYER();

	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	void	Run(id_t id);
	bool	IsPixel(int _x, int _y) { if (!Running()) return false; return IsInRect(_x, _y); }
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

private:
	std::string			m_str_message;
	LineEditorVisual	m_friend_edit_box;
	PrintInfo			m_print_info;

	ButtonGroup*		m_pC_button_group;
	C_SPRITE_PACK*		m_friend_spk;
};

class C_VS_UI_FRINEND_ADD_IS_PLAYER : public Window, public Exec, public ButtonVisual
{
	enum SPK_ID
	{
		SI_ADDWINDOW,
		SI_CONFIRM,
		SI_OK,
		SI_CANCEL,
	};
	enum EXEC_ID
	{
		EIOK,
		EICANCEL,
	};

public:
	C_VS_UI_FRINEND_ADD_IS_PLAYER(const char* player);
	virtual ~C_VS_UI_FRINEND_ADD_IS_PLAYER();

	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	void	Run(id_t id);
	bool	IsPixel(int _x, int _y) { if (!Running()) return false; return IsInRect(_x, _y); }
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

private:
	std::string			m_str_player;
	std::string			m_str_message;
	PrintInfo			m_print_info;

	ButtonGroup*		m_pC_button_group;
	C_SPRITE_PACK*		m_friend_spk;
};

class C_VS_UI_FRINEND_DEL : public Window, public Exec, public ButtonVisual
{
	enum SPK_ID
	{
		SI_ADDWINDOW,
		SI_CONFIRM,
		SI_OK,
		SI_CANCEL,
	};
	enum EXEC_ID
	{
		EIOK,
		EICANCEL,
	};

public:
	C_VS_UI_FRINEND_DEL(const char* player);
	virtual ~C_VS_UI_FRINEND_DEL();

	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	void	Run(id_t id);
	bool	IsPixel(int _x, int _y) { if (!Running()) return false; return IsInRect(_x, _y); }
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

private:
	std::string			m_del_player_name;
	std::string			m_str_message;

	ButtonGroup*		m_pC_button_group;
	C_SPRITE_PACK*		m_friend_spk;
};

class C_VS_UI_FRINEND_DEL_NOTSELECT : public Window, public Exec, public ButtonVisual
{
	enum SPK_ID
	{
		SI_ADDWINDOW,
		SI_CONFIRM,
		SI_OK,
	};
	enum EXEC_ID
	{
		EIOK,
	};

public:
	C_VS_UI_FRINEND_DEL_NOTSELECT();
	virtual ~C_VS_UI_FRINEND_DEL_NOTSELECT();

	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	void	Run(id_t id);
	bool	IsPixel(int _x, int _y) { if (!Running()) return false; return IsInRect(_x, _y); }
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

private:
	std::string			m_str_message;

	ButtonGroup*		m_pC_button_group;
	C_SPRITE_PACK*		m_friend_spk;
};

class C_VS_UI_FRINEND_CONFIRM : public Window, public Exec, public ButtonVisual
{
	enum SPK_ID
	{
		SI_ADDWINDOW,
		SI_CONFIRM,
		SI_OK,
		SI_CANCEL,
	};
	enum EXEC_ID
	{
		EIOK,
		EICANCEL,
	};

public:
	C_VS_UI_FRINEND_CONFIRM(const char* player, const char* message);
	virtual ~C_VS_UI_FRINEND_CONFIRM();

	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	void	Run(id_t id);
	bool	IsPixel(int _x, int _y) { if (!Running()) return false; return IsInRect(_x, _y); }
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

private:
	std::string			m_str_player_name;
	std::string			m_str_message;

	ButtonGroup*		m_pC_button_group;
	C_SPRITE_PACK*		m_friend_spk;
};

class C_VS_UI_FRINEND_OPTION_CONFIRM : public Window, public Exec, public ButtonVisual
{
	enum SPK_ID
	{
		SI_ADDWINDOW,
		SI_CONFIRM,
		SI_OK,
		SI_CANCEL,
	};
	enum EXEC_ID
	{
		EIOK,
		EICANCEL,
	};

public:
	enum PARENT_TYPE
	{
		PT_SYSTEM_OPTION,
		PT_INFO_OPTION,
	};

public:
	C_VS_UI_FRINEND_OPTION_CONFIRM();
	virtual ~C_VS_UI_FRINEND_OPTION_CONFIRM();

	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	void	Run(id_t id);
	bool	IsPixel(int _x, int _y) { if (!Running()) return false; return IsInRect(_x, _y); }
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	void	SetParentWindowType(unsigned char type, Window* pParent) { m_option_type = type; m_parent = pParent; }

private:
	std::string			m_str_message;

	unsigned char		m_option_type;
	Window*				m_parent;

	ButtonGroup*		m_pC_button_group;
	C_SPRITE_PACK*		m_friend_spk;
};

class C_VS_UI_FRINEND_MESSAGE_BOX : public Window, public Exec, public ButtonVisual
{
	enum SPK_ID
	{
		SI_ADDWINDOW,
		SI_CONFIRM,
		SI_OK,
	};
	enum EXEC_ID
	{
		EIOK,
	};

public:
	C_VS_UI_FRINEND_MESSAGE_BOX(const char* message);
	virtual ~C_VS_UI_FRINEND_MESSAGE_BOX();

	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	void	Run(id_t id);
	bool	IsPixel(int _x, int _y) { if (!Running()) return false; return IsInRect(_x, _y); }
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

private:
	std::string			m_str_message;

	ButtonGroup*		m_pC_button_group;
	C_SPRITE_PACK*		m_friend_spk;
};
#endif //__FRIEND_ADDITION