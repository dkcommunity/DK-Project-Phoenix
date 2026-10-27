// VS_UI_Base.cpp

#include "client_PCH.h"
#include "VS_UI_Base.h"
#include "UserInformation.h"
#include <windows.h>
//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------
Base * gpC_base;

//----------------------------------------------------------------------------
// Operations
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Base
//
// 
//-----------------------------------------------------------------------------
Base::Base()
{
	m_event_occured = false;
	m_p_DDSurface_back = NULL;

	m_chat_dialog_pi.hfont = NULL;

//	m_slayer_font	= C_FONT_MANAGER::NO_GID;
//	m_vampire_font = C_FONT_MANAGER::NO_GID;
//	m_bbs_font		= C_FONT_MANAGER::NO_GID;
//	m_item_font		= C_FONT_MANAGER::NO_GID;
//	m_normal_font	= C_FONT_MANAGER::NO_GID;
}

//-----------------------------------------------------------------------------
// ~Base
//
// 
//-----------------------------------------------------------------------------
Base::~Base()
{
	DeleteObject(m_small_pi.hfont);
	DeleteObject(m_chatting_pi.hfont);
	DeleteObject(m_user_id_pi.hfont);
	DeleteObject(m_value_pi.hfont);
	DeleteObject(m_value2_pi.hfont);
	DeleteObject(m_item_name_pi.hfont);
	DeleteObject(m_item_desc_pi.hfont);
	DeleteObject(m_dialog_msg_pi.hfont);
	DeleteObject(m_dialog_menu_pi.hfont);
	DeleteObject(m_desc_msg_pi.hfont);
	DeleteObject(m_desc_menu_pi.hfont);
	DeleteObject(m_money_pi.hfont);
	DeleteObject(m_char_value_pi.hfont);

	if (m_chat_dialog_pi.hfont != NULL)
		DeleteObject(m_chat_dialog_pi.hfont);
	DeleteObject(m_info_pi.hfont);
	DeleteObject(m_item_pi.hfont);

	DeleteObject(m_char_name_pi.hfont);		//by larosel
	DeleteObject(m_char_chat_pi.hfont);
	DeleteObject(m_char_chat_large_pi.hfont);
	DeleteObject(m_char_chat_event_pi.hfont);

	DeleteObject(m_party_name_pi.hfont);
	DeleteObject(m_xmas_pi.hfont);
	

	DeleteObject(m_money2_pi.hfont);
#if __CONTENTS(__TITLE_UI_RENWEAL)
	DeleteObject(m_title_menu_pi.hfont);  
#endif //__TITLE_UI_RENWEAL
	DeleteObject(m_gearinfo_pi.hfont);
}

//-----------------------------------------------------------------------------
// SetFont
//
// 
//-----------------------------------------------------------------------------
void Base::SetFont(PrintInfo &pi, LOGFONT &lf, COLORREF textcolor, COLORREF backcolor, int bk_mode, int align)
{
	HFONT hfont = CreateFontIndirect(&lf);


#ifdef UNICODE
	int a = 0;
#endif 

	if (hfont == NULL)
	  _Error(FAILED_JOB);

	pi.hfont = hfont;
	pi.text_color = textcolor;
	pi.back_color = backcolor;
	pi.bk_mode = bk_mode;
	pi.text_align = align;
}

//-----------------------------------------------------------------------------
// SetDefaultLogfont
//
// 
//-----------------------------------------------------------------------------
void Base::SetDefaultLogfont(LOGFONT &lf) const
{
	lf.lfHeight = 0; // 0 = default
	lf.lfWidth = 0; // 0 = ���̿� �����Ͽ� �ڵ����� ������.
	lf.lfEscapement = 0; // ���⼳�� (900, 2700)
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL; // FW_BOLD
	lf.lfItalic = 0; // 0 �ƴ� ���̸� italic�̴�.
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	if(gC_ci->IsChinese())
		lf.lfCharSet = GB2312_CHARSET;//*/JOHAB_CHARSET;
		//lf.lfCharSet = THAI_CHARSET;//*/JOHAB_CHARSET;̩�Ŀͻ���
    else if(gC_ci->IsJapanese())
		lf.lfCharSet = SHIFTJIS_CHARSET ;
	else
		// Phoenix default text contract is CP1254 for Latin/Turkish UI input.
		// Client language is currently forced through the legacy Korean CI path,
		// so the default branch must not keep HANGUL_CHARSET.
		lf.lfCharSet = TURKISH_CHARSET;
	if(gC_ci->IsJapanese())
	{
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = PROOF_QUALITY ; //DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_ROMAN  ; // FF_DONTCARE;
		strcpy(lf.lfFaceName, "MS PGothic");//"Times New Roman");
	}
	else
	{
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
		strcpy(lf.lfFaceName, "Times New Roman");//"Times New Roman");
	}
}

//-----------------------------------------------------------------------------
// SelectFont
//
// 
//-----------------------------------------------------------------------------
/*
void Base::SelectFont(font_id_t id)
{
	switch (id)
	{
		case FONT_SLAYER:
			gC_font.SelectFont(m_slayer_font);
			gC_font.NotUseNoPublicEng();
			break;

		case FONT_VAMPIRE:
			gC_font.SelectFont(m_vampire_font);
			gC_font.NotUseNoPublicEng();
			break;

		case FONT_BBS:
			gC_font.SelectFont(m_bbs_font);
			gC_font.NotUseNoPublicEng();
			break;

		case FONT_ITEM:
			gC_font.SelectFont(m_normal_font);
			gC_font.UseNoPublicEng(OLIVE);
			break;

		case FONT_ITEM_SMALL:
			gC_font.SelectFont(m_item_font);
			gC_font.NotUseNoPublicEng();
			break;

		case FONT_NORMAL:
			gC_font.SelectFont(m_normal_font);
			gC_font.NotUseNoPublicEng();
			break;
	}
}*/

//-----------------------------------------------------------------------------
// Init
//
// 
//-----------------------------------------------------------------------------
void Base::Init(CSpriteSurface *surface, void (*fp)(DWORD, int, int, void *))
{
	InitSurface(surface);
	InitFont();
	m_C_ui_result_receiver.SetResultReceiver(fp);
	gC_ci->RunCursorBlinker();
}

//-----------------------------------------------------------------------------
// InitFont
//
// 
//-----------------------------------------------------------------------------
void Base::InitFont()
{
	LOGFONT lf;

	// 
	// ������ style
	//
	// - ����, 13
	//
	// !Times New Romans�� ���� ����. OS�� ��ġ �ȸ´� ���� �߻���.
	//Tahoma
	//change by rallser  ̩���ͻ���
	// FAZ 4 (2026-05-04 oturum 2) — Font readability overhaul.
	// Old fonts (Korean: "����ü"/"����", Chinese: "����"/"������" = SimSun/SimHei)
	// were hard to read at 10-14px on modern displays. Switched all columns to
	// "Microsoft YaHei" (Win Vista+ default, ClearType-optimized, full CJK +
	// Latin coverage, sans-serif). One font handles every UI element across
	// every language — eliminates the historical Korean/Chinese/Japanese
	// per-column drift.
	//
	// Old "Tahoma" experimental block (commented out below) is kept for
	// historical reference but superseded — Tahoma has no CJK glyphs.
	// 2026-05-09: Microsoft YaHei -> orbitron-medium (modern bold sans-serif, Google Fonts).
	// CJK glyphlari yok — orbitron-medium Latin/Cyrillic; kullanici "CJK desteklememesi onemli
	// degil" dedi. CJK karakteri olan metinde Windows otomatik fallback yapar (system font).
	// TTF runtime AddFontResourceEx ile WinMain'de yuklenir (process-private).
	const char szFontName[5][3][20] = {
		// Hangul Font     Chinese Font     Japanese Font
		{ "orbitron-medium",     "orbitron-medium",     "orbitron-medium" },  // 0: regular (small/chat/desc)
		{ "orbitron-medium",     "orbitron-medium",     "orbitron-medium" },  // 1: value/number
		{ "orbitron-medium",     "orbitron-medium",     "orbitron-medium" },  // 2: item name (bold)
		{ "orbitron-medium",     "orbitron-medium",     "orbitron-medium" },  // 3: char name / chat / party
		{ "orbitron-medium",     "orbitron-medium",     "orbitron-medium" }   // 4: event chat / title menu
	};

    /*   const char szFontName[5][3][20] = {
		// Hangul Font      Chinese Font
		{ "����ü",			"Tahoma",			"�l�r �S�V�b�N"},
		{ "MS Sans Serif",	"Tahoma","�l�r �S�V�b�N"},
		{ "����ü",			"Tahoma",		"�l�r �S�V�b�N"},
		{ "����",			"Tahoma",			"�l�r ����"},
		{ "��������",		"Tahoma",			"�l�r ����"}
	};*/


	// FAZ 4 (2026-05-04 oturum 2): Global +3px boost for readability —
	// all lfHeight values below add `local_font_size`, so this single bump
	// scales the entire UI text. Reduce to +2 if anything overflows.
	//
	// Original Language detect had an if/if/else bug: Chinese branch set
	// Language=1, then the unconditional `if (IsJapanese)... else Language=0`
	// overrode it back to 0. Fixed to else-if chain — Chinese now actually
	// selects column 1 (currently moot since all columns are YaHei, but
	// correct for future per-language tuning).
	char Language = 0;  // default = Korean column (0)
	int local_font_size = 3;  // global +3 readability boost

	if(gC_ci->IsChinese())
	{
		Language = 1;
	}
	else if(gC_ci->IsJapanese())
	{
		Language = 2;
		gC_ci->SetJapanInput();
	}
	// else Language stays 0 (Korean / default)
	
	SetDefaultLogfont(lf); //by larosel
	lf.lfHeight = 10 + local_font_size;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_small_pi, lf, RGB(20, 70, 0));

	SetDefaultLogfont(lf); //by larosel
	lf.lfHeight = 12 + local_font_size;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_chatting_pi, lf, RGB(20, 70, 0));

	SetDefaultLogfont(lf); //by larosel
	lf.lfHeight = 12 + local_font_size;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_user_id_pi, lf, RGB(20, 70, 0));


	//
	SetDefaultLogfont(lf); //by larosel
	lf.lfHeight = 25 + local_font_size;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_haysNameBig, lf, RGB(20, 70, 0));

	SetDefaultLogfont(lf); //by larosel
	lf.lfHeight = 25 + local_font_size;
	lf.lfWeight = FW_HEAVY;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_PayEvent_pi, lf, RGB(20, 70, 0));


	SetDefaultLogfont(lf); //by larosel
	lf.lfHeight = 18 + local_font_size;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_haysLevel, lf, RGB(20, 70, 0));

	

	// new style...
	SetDefaultLogfont(lf);
	lf.lfHeight = 14 + local_font_size;
	//lf.lfItalic = 1;
	//lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[1][Language]);
	SetFont(m_value_pi, lf, RGB(255, 255, 255), 0, TRANSPARENT, TA_RIGHT);

	// new style...
	SetDefaultLogfont(lf);
	lf.lfHeight = 14 + local_font_size;
	lf.lfItalic = 1;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[1][Language]);
	SetFont(m_value2_pi, lf, RGB(20, 70, 0));

	// new style...
	SetDefaultLogfont(lf);
	lf.lfHeight = 14 + local_font_size;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[2][Language]);
	SetFont(m_item_name_pi, lf, RGB(255, 255, 255));

	// new style...
	SetDefaultLogfont(lf);
	lf.lfHeight = 12 + local_font_size;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_item_desc_pi, lf, RGB(192, 192, 255));

	// new style...
	SetDefaultLogfont(lf);
	lf.lfHeight = 14 + local_font_size;
//	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_dialog_menu_pi, lf, RGB(255, 255, 255));

	// new style...
	SetDefaultLogfont(lf);
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	lf.lfHeight = 13 + local_font_size;
	SetFont(m_dialog_msg_pi, lf, RGB(255, 255, 255));

	// new style...
	SetDefaultLogfont(lf);
	lf.lfHeight = 14 + local_font_size;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_desc_menu_pi, lf, RGB(255, 255, 255));

	// new style...
	SetDefaultLogfont(lf);
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	lf.lfHeight = 14 + local_font_size;
	SetFont(m_desc_msg_pi, lf, RGB(255, 255, 255));

	// new style...
	SetDefaultLogfont(lf);
	lf.lfHeight = 16 + local_font_size;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_money_pi, lf, RGB(255, 255, 255));

	// new style...
	SetDefaultLogfont(lf);
	lf.lfHeight = 16 + local_font_size;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_char_value_pi, lf, RGB(255, 255, 255));

	//
	//font used by Client by larosel ���� ��Ʈ���� 2�� ũ�� �������.
	//
	//SetDefaultLogfont(lf);
	//lf.lfHeight = 12;
	//strcpy(lf.lfFaceName, "����");
	//SetFont(m_chat_dialog_pi, lf, RGB(255, 255, 255));

	SetDefaultLogfont(lf);
	lf.lfHeight = 14 + local_font_size;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[3][Language]);
	SetFont(m_info_pi, lf, RGB(255, 255, 255));

	SetDefaultLogfont(lf);
	lf.lfHeight = 12 + local_font_size;
	strcpy(lf.lfFaceName, szFontName[3][Language]);
	SetFont(m_item_pi, lf, RGB(255, 255, 255));

	SetDefaultLogfont(lf);
	lf.lfHeight = 13 + local_font_size;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[3][Language]);
	SetFont(m_char_name_pi, lf, RGB(255, 255, 255));

	SetDefaultLogfont(lf);
	lf.lfHeight = 13 + local_font_size;
	strcpy(lf.lfFaceName, szFontName[3][Language]);
	SetFont(m_char_chat_pi, lf, RGB(255, 255, 255));

	//party
	SetDefaultLogfont(lf); //by larosel
	lf.lfHeight = 12 + local_font_size;
//	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[3][Language]);
	SetFont(m_party_name_pi, lf, RGB(20, 70, 0));

	//xmas
	SetDefaultLogfont(lf); //by larosel
	lf.lfHeight = 10 + local_font_size;
//	lf.lfWeight = FW_BOLD;
	lf.lfItalic = true;
	strcpy(lf.lfFaceName, szFontName[3][Language]);
	SetFont(m_xmas_pi, lf, RGB(20, 70, 0));

	SetDefaultLogfont(lf);
	lf.lfHeight = 16 + local_font_size;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[3][Language]);
	SetFont( m_char_chat_large_pi, lf, RGB(255,255,255));

	// new style...
	SetDefaultLogfont(lf);
	lf.lfHeight = 16 + local_font_size;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_money2_pi, lf, RGB(255, 255, 255), 0, TRANSPARENT, TA_RIGHT);

	// Event Message�� pi
	SetDefaultLogfont(lf);
	lf.lfHeight = 20 + local_font_size;
	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[4][Language]);
	SetFont(m_char_chat_event_pi, lf, RGB(255, 255, 255), 0, TRANSPARENT, TA_LEFT);

#if __CONTENTS(__TITLE_UI_RENWEAL)
	SetDefaultLogfont(lf);
	lf.lfHeight = 12 + local_font_size;
//	lf.lfWeight = FW_BOLD;
	strcpy(lf.lfFaceName, szFontName[4][Language]);
	SetFont(m_title_menu_pi, lf, RGB(255, 255, 255), 0, TRANSPARENT, TA_LEFT);
#endif // __TITLE_UI_RENWEAL


	SetDefaultLogfont(lf); //by larosel
	lf.lfHeight = 14 + local_font_size;
	strcpy(lf.lfFaceName, szFontName[0][Language]);
	SetFont(m_gearinfo_pi, lf, RGB(20, 70, 0));
}

//-----------------------------------------------------------------------------
// InitSurface
//
// 
//-----------------------------------------------------------------------------
void Base::InitSurface(CSpriteSurface *surface)
{
	assert(surface);

	m_p_DDSurface_back = surface;

	//g_pUserInformation->IsResolution1024


	bool ret = m_DDSurface_offscreen.InitOffsurface(g_pUserInformation->iResolution_x, g_pUserInformation->iResolution_y, DDSCAPS_SYSTEMMEMORY);

	//bool ret = m_DDSurface_offscreen.InitOffsurface(RESOLUTION_X, RESOLUTION_Y, DDSCAPS_SYSTEMMEMORY);


	if (!ret)
		_Error(FAILED_JOB);

	//
	// offscreen�� colorkey�� �����Ѵ�. ��Ȳ�� ���� Image���� �� ���� ���� 
	// �����ؾ� �Ѵ�.
	//
	m_colorkey_red = RED;

	m_DDSurface_offscreen.SetTransparency(m_colorkey_red); // default colorkey = red

	// set Unicorn edit widget surface
	g_SetFL2Surface(m_p_DDSurface_back->GetSurface());
}

//-----------------------------------------------------------------------------
// 2026-09-02 O3 — canli cozunurluk: offscreen'i yeni iResolution ile yeniden
// kur. InitOffsurface re-init'i kendi icinde eski surface'i release eder.
//-----------------------------------------------------------------------------
void Base::ReinitSurfaceForResolution()
{
	if (m_p_DDSurface_back != NULL)
		InitSurface(m_p_DDSurface_back);
}

//-----------------------------------------------------------------------------
// DispatchMessage
//
// 
//-----------------------------------------------------------------------------
void Base::DispatchMessage()
{
	m_C_ui_result_receiver._DispatchMessage();
}

//-----------------------------------------------------------------------------
// SendMessage
//
// 
//-----------------------------------------------------------------------------
void Base::SendMessage(DWORD message, int left, int right, void * void_ptr)
{
	m_C_ui_result_receiver._SendMessage(message, left, right, void_ptr);
}

//-----------------------------------------------------------------------------
// StartEvent
//
// 
//-----------------------------------------------------------------------------
void	Base::StartEvent() 
{ 
	//assert(m_event_occured == false);

	m_event_occured = true; 
}

//-----------------------------------------------------------------------------
// FinishEvent
//
// 
//-----------------------------------------------------------------------------
void	Base::FinishEvent() 
{ 
	//assert(m_event_occured == true);

	m_event_occured = false; 
}
