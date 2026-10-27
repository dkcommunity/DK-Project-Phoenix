/*-----------------------------------------------------------------------------

	VS_UI_Base.h

	2000.8.9. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_BASE_H__
#define __VS_UI_BASE_H__

// Phoenix: Orijinal tasarım client_PCH.h'yi her .cpp başında precompiled
// header olarak bekliyor; PCH içinde <string>/<vector>/<list>/<map> +
// using std::* var. Modern build'de PCH her zaman aktif olmadığından
// VS_UI_Base.h'ye bu temel tanımları taşıyoruz. Böylece VS_UI_Base'i
// include eden tüm başlık dosyaları std kaplarını unqualified kullanabilir.
#include <string>
#include <vector>
#include <list>
#include <map>
#include <deque>
#include <bitset>
#include <fstream>
#include <iostream>
using std::string;
using std::vector;
using std::list;
using std::map;
using std::deque;
using std::bitset;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::istream;
using std::ostream;
using std::ios;
using std::ios_base;
using std::endl;
using std::cout;
using std::cerr;

// __CONTENTS macro'su — koşullu enum üyeleri için gerekli.
#include "ContentsFilter.h"

#include "BasicS.h"
#include "DLL.h"
#include "GL_import.h"
#include "Timer2.h"
#include "DXLib.h"
#include "Unicorn.h"
#include "SP.h"
//#include "VS_UI_Exception.h"
#include "VS_UI_ui_result_receiver.h"
#include "SimpleDataList.h"
#include "PI.h"
#include "PS.h"
#include "RaceType.h"
#include "UserInformation.h"
#include "ACriticalSection.h"

#include <assert.h>

#include "AtlTypes/atltypes.h"

// default help string
struct S_DEFAULT_HELP_STRING
{
	char *		sz_main_str;
	char *		sz_main_str2;
	char *		sz_sub_str;
};

struct S_BLOOD_BIBLE_DESCRIPTION
{
	char *		sz_name;
	char *		sz_place;
	char *		sz_status;
	char *		sz_shrine;
	char *		sz_option;
	char *		sz_player;
};

// �ֹι�ȣ�ڸ���(�ѱ�)
#define SSN_POS_COUNT					13
#define SSN_PART1_CHAR_COUNT			6
#define SSN_PART2_CHAR_COUNT			7
//#define _SHOW_WINDOW_ATTR

//
// SHOW_WINDOW_ATTR�� �� Window�� Window���¸� ����ϱ� ���� ���̴�. Window�� ������ϱ� ����
// ���� ���̴�.
//
#if (defined(_SHOW_WINDOW_ATTR) && !defined(_LIB))
#define SHOW_WINDOW_ATTR { \
								char buf[200]; \
								sprintf(buf, "[move=%d, pin=%d, topmost=%d, kctrl=%d", GetAttributes()->window_move, GetAttributes()->pin, GetAttributes()->topmost, GetAttributes()->keyboard_control); \
								g_Print(x, y, buf); \
								}
#else
#define SHOW_WINDOW_ATTR ;
#endif

//
// set/unset background mouse focus message
//
// client background�� mouse focus�� set/unset�Ѵ�.
// �̰��� mouse pointer�� UI Window�� acquire/unacquire�� client�� �˸��� ���̴�.
//
#define SET_BACKGROUND_MOUSE_FOCUS	if (gpC_window_manager->GetMouseFocusedWindow() == NULL) \
													gpC_base->SendMessage(UI_BACKGROUND_MOUSE_FOCUS, gpC_mouse_pointer->GetPointerX(), gpC_mouse_pointer->GetPointerY());
#define UNSET_BACKGROUND_MOUSE_FOCUS	gpC_base->SendMessage(UI_REMOVE_BACKGROUND_MOUSE_FOCUS);


#define TEXT_LINE(x)						(x*20) // 20 font height...

// LOWORD, HIWORD�� �˻��Ѵ�.
#define MAKEDWORD(a, b)					(DWORD)((a << 16)|b)

#define INTERFACE_BLINK_VALUE_MAX	4

extern bool gbl_global_empty_move;

//
// EMPTY_MOVE�� ������ ���콺�Է��� MOVE�� �� �� �߻��Ѵ�.
//
#define EMPTY_MOVE						gbl_global_empty_move = true;

//-----------------------------------------------------------------------------
// Global definition.
//-----------------------------------------------------------------------------
#define PCS_NUMBER_LIMIT				7 // 7�ڸ�

// rgb
#define RGB_WHITE							RGB(255, 255, 255)
#define RGB_BRIGHT_GRAY						RGB(216, 216, 216)
#define RGB_RED								RGB(255, 0, 0)
#define RGB_GREEN							RGB(0, 255, 0)
#define RGB_BLUE							RGB(0, 0, 255)
#define RGB_SKY_BLUE						RGB(0, 255, 255)
#define RGB_YELLOW							RGB(255, 255, 0)
#define RGB_GRAY							RGB(192, 192, 192)
#define RGB_DARKGRAY						RGB(100, 100, 100)
#define RGB_GOLD							RGB(187, 187, 0)
#define RGB_BLACK							RGB(0, 0, 0)
#define RGB_PEARL							RGB(150,50,170)
#define RGB_ORANGE							RGB(255,69,00)
#define RGB_PINK							RGB(255, 128, 255)
#define RGB_PURPLE							RGB(155, 80, 210)
#define RGB_LIGHTBLUE						RGB(0, 100, 255)
// color
enum 
{ 
	BLACK				= 0,
	WHITE				= 0xFFFF,
	GRAY				= 50712,
	RED					= 49152,
	GREEN				= 2016,
	BLUE				= 0xFF,
	YELLOW				= 65504,
	MAGENTA				= 63519,
	BORA				= 32799,
	LIGHT_BLUE			= 0x421F,
	SKY_BLUE			= 33823,
	DARKYELLOW			= 33792,
	YELLOWGREEN			= 34784,
	LIGHT_MAGENTA		= 0xFE1F,
	DARKGRAY			= 25388,
	LIGHTRED			= 0xf800,
	DARKRED				= 0x8000,    
};

// extended color
enum
{
	CURSOR_COLOR		= SKY_BLUE,
	CURSOR_COLOR_RED	= RED,

	POSSIBLE_COLOR		= LIGHT_BLUE,
	IMPOSSIBLE_COLOR	= RED,
	EXCHANGE_COLOR		= BORA,
};

// objective color
enum
{
	ERROR_MSG_COLOR = YELLOW,
};

/*
#define CONVERT_1024_768
// screen resolution
enum
{
#ifdef CONVERT_1024_768
	RESOLUTION_X = 1024,
	RESOLUTION_Y = 768,
#else
	RESOLUTION_X = 800,
	RESOLUTION_Y = 600,
#endif
	
};*/


#define RESOLUTION_X	(g_pUserInformation->iResolution_x)
#define RESOLUTION_Y	(g_pUserInformation->iResolution_y)


// font id
// ������ �� �ִ� font ����.
enum font_id_t
{
	FONT_SLAYER,
	FONT_VAMPIRE,
	FONT_BBS,
	FONT_ITEM,
	FONT_ITEM_SMALL,
	FONT_NORMAL,
};

//---------------------------------------
// UI result receiver void ptr caster
//---------------------------------------
struct MOUSEPOINTER_INFO
{
	int		x; // screen������ (x, y)
	int		y;
	int		width;
	int		height;
};

// Client�� login�� ������ ���� ����ü.
struct DELETE_CHARACTER
{
	char *	sz_part1;
	char *	sz_part2;
	int		slot;
};

struct LOGIN
{
	char *	sz_id;
	char *	sz_password;
};

struct NEW_REGISTRATION
{
	char *	sz_id;
	char *	sz_password;
	char *	sz_repassword;
	char *	sz_name;

	char *	sz_email;
	char *	sz_address;
	char *	sz_ssn_number_part1; // �ֹι�ȣ
	char *	sz_ssn_number_part2;
	char *	sz_homepage;
	char *	sz_woo;
	char *	sz_phone;

	bool		bl_female;
	bool		bl_announce_my_info;
};

struct NEW_CHARACTER
{
	char *		sz_name;

	Race		race;
//	bool		bl_slayer;
	bool		bl_female;
	int			STR;
	int			DEX;
	int			INT;
	int			slot;

	int			face;
	int			skin_color;
	int			hair_color;
	int			coat_color;
	int			trouser_color;
	int			job;
};

//-----------------------------------------------------------------------------
// Base
//
// ����� �����ϴ� class�̴�.
// VS_UI System�� ���������� ������ ������ �͵��� ��� ���⿡ ���ǵȴ�.
//-----------------------------------------------------------------------------
class Base
{
	//
	// System condition flag.
	//
private:
	//
	// m_event_occured
	//
	// C_VS_UI_TRIBE class ����.
	//
	// event�� �߻��ϸ� set�ȴ�.
	//
	bool						m_event_occured;

public:
	bool	EventOccured() const { return m_event_occured; }
	void	StartEvent();
	void	FinishEvent();

	//
	// Video/Font defition.
	//
public:
	CSpriteSurface *		m_p_DDSurface_back; // UI�� ����� surface
	CSpriteSurface			m_DDSurface_offscreen; // effect�� ���� offscreen

private:
	int						m_colorkey_red; // offscreen colorkey

public:
	PrintInfo				m_small_pi;
	PrintInfo				m_chatting_pi;
	PrintInfo				m_user_id_pi;
	PrintInfo				m_value_pi; // �������ĵ� ���� ����..
	PrintInfo				m_value2_pi; // �������ĵ� ���� ����..
	PrintInfo				m_item_name_pi; // !item description�� ��µ� �� ���� ����� �� �ִ�.
	PrintInfo				m_item_desc_pi;
	PrintInfo				m_dialog_menu_pi;
	PrintInfo				m_dialog_msg_pi;
	PrintInfo				m_desc_menu_pi;
	PrintInfo				m_desc_msg_pi;
	PrintInfo				m_money_pi;
	PrintInfo				m_char_value_pi;
	PrintInfo				m_char_chat_large_pi;
	PrintInfo				m_char_chat_event_pi;
	
	// use by Client
	PrintInfo				m_chat_dialog_pi;
	PrintInfo				m_info_pi;
	PrintInfo				m_item_pi;

	PrintInfo				m_char_name_pi; //by larosel
	PrintInfo				m_char_chat_pi; //by larosel

	// party
	PrintInfo				m_party_name_pi;

	PrintInfo				m_haysNameBig;
	PrintInfo				m_haysLevel;

	PrintInfo				m_PayEvent_pi;

	// xmas
	PrintInfo				m_xmas_pi;
	
	PrintInfo				m_money2_pi; // ���� ���� ��

	PrintInfo				m_gearinfo_pi;

	ACriticalSection		m_ResultReceiverCS;
#if __CONTENTS(__TITLE_UI_RENWEAL)
	PrintInfo				m_title_menu_pi;	//Ÿ��Ʋ �޴����� ����� ��Ʈ �߰�
#endif	// __TITLE_UI_RENWEAL

private:
	C_VS_UI_RESULT_RECEIVER		m_C_ui_result_receiver;

	void	InitFont();
	void	InitSurface(CSpriteSurface *surface);
	void	SetDefaultLogfont(LOGFONT &lf) const;
	void	SetFont(PrintInfo &pi, LOGFONT &lf, COLORREF textcolor, COLORREF backcolor=0, int bk_mode=TRANSPARENT, int align=TA_LEFT);

public:
	Base();
	~Base();

	void	Init(CSpriteSurface *surface, void (*fp)(DWORD, int, int, void *));

	// 2026-09-02 O3 — canli cozunurluk: tam-ekran offscreen'i yeni
	// iResolution boyutuyla yeniden kurar (InitSurface'i tekrar cagirir).
	void	ReinitSurfaceForResolution();

	void	DispatchMessage();
	void	SendMessage(DWORD message, int left = 0, int right = 0, void *void_ptr = NULL);
//	void	SelectFont(font_id_t id);

#ifndef _LIB
	int	GetMessageSize() const { return m_C_ui_result_receiver.GetMessageSize(); }
#endif
};

extern Base * gpC_base;

extern UINT	g_blink_value;
extern int ga_item_blink_color_table[];

extern bool gbl_sell_running; // ��� ��..
extern bool gbl_buy_running; // �Ĵ� ��..
extern bool gbl_repair_running; // ��ġ�� ��...
extern bool gbl_silvering_running; // ��ġ�� ��...
extern bool gbl_item_lock;
extern bool gbl_gear_lock;
extern bool gbl_item_trade_lock;
//extern bool gbl_vampire_interface;
extern Race g_eRaceInterface;
extern bool gbl_swap_advancement_item_running; // ��� ��..
//20080214 diesirace ������ ������ ���� �������� ������ ���� �Ѿ�� �ȴ�.
//���� ���� �½� sell�̳� buy�� ture�� ���� ������ ������ ���� ������ ǥ�� �Ǹ鼭 �̻�����
//���� ���� ������ ��� ���� ǥ�����ִ� gbl�� �����  ������ ++ ���ִ� �ڵ常 �߰����ش�. 
//���� ���� �������� ������ ���ڶ��� �̿��� ����
// ���� ���� UI�����..
extern bool gbl_persnalshop_running;
#endif
