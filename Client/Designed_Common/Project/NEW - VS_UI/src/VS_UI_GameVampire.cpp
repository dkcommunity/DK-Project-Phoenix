// VS_UI_GameVampire.cpp

#include "client_PCH.h"
#include "crash_trace_tool.h"
#include "../../Client/PhoenixDWrite.h"  // FULL DX11 gecis Faz T3 — DWrite text (GDI fallback'li)
#include "../../Client/DXLib/D3D11SpriteCommands.h"  // Faz G5 — ham pixel yazilari dirty bildirimi
#include "VS_UI_GameVampire.h"
#include "VS_UI_GameSlayer.h"
#include "VS_UI_filepath.h"
#include "VS_UI_Mouse_pointer.h"
#include "VS_UI.h"
#include "UserInformation.h"
#include "ExperienceTable.h"
#include "MGameStringTable.h"

namespace
{
	const WORD PHOENIX_VAMPIRE_EXP_TEXT_CACHE_KEY = 0x0001;

	struct PhoenixVampireExpTextCache
	{
		HDC hdc;
		HBITMAP bitmap;
		HBITMAP oldBitmap;
		WORD *bits;
		int width;
		int height;
		int pitchWords;
		int left;
		int top;
		DWORD signature;
		HFONT font;
		bool valid;
	};

	static PhoenixVampireExpTextCache g_vampireExpTextCache =
	{
		NULL, NULL, NULL, NULL,
		0, 0, 0, 0, 0, 0, NULL, false
	};

	static void Phoenix_ReleaseVampireExpTextCache()
	{
		if(g_vampireExpTextCache.hdc != NULL && g_vampireExpTextCache.oldBitmap != NULL)
		{
			SelectObject(g_vampireExpTextCache.hdc, g_vampireExpTextCache.oldBitmap);
			g_vampireExpTextCache.oldBitmap = NULL;
		}
		if(g_vampireExpTextCache.bitmap != NULL)
		{
			DeleteObject(g_vampireExpTextCache.bitmap);
			g_vampireExpTextCache.bitmap = NULL;
		}
		if(g_vampireExpTextCache.hdc != NULL)
		{
			DeleteDC(g_vampireExpTextCache.hdc);
			g_vampireExpTextCache.hdc = NULL;
		}
		g_vampireExpTextCache.bits = NULL;
		g_vampireExpTextCache.width = 0;
		g_vampireExpTextCache.height = 0;
		g_vampireExpTextCache.pitchWords = 0;
		g_vampireExpTextCache.left = 0;
		g_vampireExpTextCache.top = 0;
		g_vampireExpTextCache.signature = 0;
		g_vampireExpTextCache.font = NULL;
		g_vampireExpTextCache.valid = false;
	}

	static bool Phoenix_EnsureVampireExpTextCacheSurface(int width, int height)
	{
		if(width <= 0 || height <= 0)
			return false;

		if(g_vampireExpTextCache.hdc != NULL &&
			g_vampireExpTextCache.bitmap != NULL &&
			g_vampireExpTextCache.bits != NULL &&
			g_vampireExpTextCache.width == width &&
			g_vampireExpTextCache.height == height)
		{
			return true;
		}

		Phoenix_ReleaseVampireExpTextCache();

		g_vampireExpTextCache.hdc = CreateCompatibleDC(NULL);
		if(g_vampireExpTextCache.hdc == NULL)
			return false;

		struct RGB565BitmapInfo
		{
			BITMAPINFOHEADER header;
			DWORD masks[3];
		};

		RGB565BitmapInfo bitmapInfo;
		ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
		bitmapInfo.header.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.header.biWidth = width;
		bitmapInfo.header.biHeight = -height;
		bitmapInfo.header.biPlanes = 1;
		bitmapInfo.header.biBitCount = 16;
		bitmapInfo.header.biCompression = BI_BITFIELDS;
		bitmapInfo.masks[0] = 0xF800;
		bitmapInfo.masks[1] = 0x07E0;
		bitmapInfo.masks[2] = 0x001F;

		void *bits = NULL;
		g_vampireExpTextCache.bitmap = CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&bitmapInfo), DIB_RGB_COLORS, &bits, NULL, 0);
		if(g_vampireExpTextCache.bitmap == NULL || bits == NULL)
		{
			Phoenix_ReleaseVampireExpTextCache();
			return false;
		}

		g_vampireExpTextCache.oldBitmap = static_cast<HBITMAP>(SelectObject(g_vampireExpTextCache.hdc, g_vampireExpTextCache.bitmap));
		g_vampireExpTextCache.bits = static_cast<WORD*>(bits);
		g_vampireExpTextCache.width = width;
		g_vampireExpTextCache.height = height;
		g_vampireExpTextCache.pitchWords = (((width * 16 + 31) / 32) * 4) / sizeof(WORD);
		g_vampireExpTextCache.valid = false;
		return true;
	}

	static void Phoenix_FillVampireExpTextCache()
	{
		if(g_vampireExpTextCache.bits == NULL || g_vampireExpTextCache.pitchWords <= 0)
			return;

		for(int row = 0; row < g_vampireExpTextCache.height; ++row)
		{
			WORD *dst = g_vampireExpTextCache.bits + row * g_vampireExpTextCache.pitchWords;
			for(int col = 0; col < g_vampireExpTextCache.width; ++col)
				dst[col] = PHOENIX_VAMPIRE_EXP_TEXT_CACHE_KEY;
		}
	}

	static DWORD Phoenix_MixVampireExpTextSignature(DWORD signature, int value)
	{
		return (signature ^ static_cast<DWORD>(value)) * 16777619u;
	}

	static DWORD Phoenix_BuildVampireExpTextSignature(HFONT font)
	{
		DWORD signature = 2166136261u;
		signature = Phoenix_MixVampireExpTextSignature(signature, g_char_slot_ingame.STR_CUR);
		signature = Phoenix_MixVampireExpTextSignature(signature, g_char_slot_ingame.DEX_CUR);
		signature = Phoenix_MixVampireExpTextSignature(signature, g_char_slot_ingame.INT_CUR);
		signature = Phoenix_MixVampireExpTextSignature(signature, g_char_slot_ingame.DAM2);
		signature = Phoenix_MixVampireExpTextSignature(signature, g_char_slot_ingame.DAM);
		signature = Phoenix_MixVampireExpTextSignature(signature, g_char_slot_ingame.DEFENSE);
		signature = Phoenix_MixVampireExpTextSignature(signature, g_char_slot_ingame.PROTECTION);
		signature = Phoenix_MixVampireExpTextSignature(signature, static_cast<int>(reinterpret_cast<DWORD_PTR>(font)));
		return signature;
	}

	static bool Phoenix_NeedsVampireExpTextCacheBuild(int left, int top, DWORD signature, HFONT font)
	{
		return !g_vampireExpTextCache.valid ||
			g_vampireExpTextCache.left != left ||
			g_vampireExpTextCache.top != top ||
			g_vampireExpTextCache.signature != signature ||
			g_vampireExpTextCache.font != font;
	}

	static HDC Phoenix_BeginVampireExpTextCacheBuild()
	{
		if(g_vampireExpTextCache.hdc == NULL || g_vampireExpTextCache.bits == NULL)
			return NULL;

		Phoenix_FillVampireExpTextCache();
		return g_vampireExpTextCache.hdc;
	}

	static void Phoenix_CommitVampireExpTextCacheBuild(int left, int top, DWORD signature, HFONT font)
	{
		g_vampireExpTextCache.left = left;
		g_vampireExpTextCache.top = top;
		g_vampireExpTextCache.signature = signature;
		g_vampireExpTextCache.font = font;
		g_vampireExpTextCache.valid = true;
	}

	static int Phoenix_GetVampireCachedTextWidth(HDC hdc, const char *text, HFONT font)
	{
		if(hdc == NULL || text == NULL)
			return 0;
		if(font != NULL)
			SelectObject(hdc, font);

		SIZE textSize = {0, 0};
		GetTextExtentPoint32(hdc, text, static_cast<int>(strlen(text)), &textSize);
		return textSize.cx;
	}

	static int Phoenix_DrawVampireCachedTextShadow(HDC hdc, int x, int y, const char *text, const PrintInfo &pi, COLORREF textColor, COLORREF shadowColor)
	{
		if(hdc == NULL || text == NULL)
			return x;

		SelectObject(hdc, pi.hfont);
		SetTextAlign(hdc, pi.text_align);
		SetBkMode(hdc, pi.bk_mode);
		SetBkColor(hdc, pi.back_color);

		const int textLength = static_cast<int>(strlen(text));
		SetTextColor(hdc, shadowColor);
		Phoenix_TextOutDW(hdc, x + 1, y + 1, text, textLength);  // Faz T3
		SetTextColor(hdc, textColor);
		Phoenix_TextOutDW(hdc, x, y, text, textLength);

		SIZE textSize = {0, 0};
		GetTextExtentPoint32(hdc, text, textLength, &textSize);
		return x + textSize.cx;
	}

	static bool Phoenix_BltVampireExpTextCache()
	{
		if(!g_vampireExpTextCache.valid ||
			g_vampireExpTextCache.bits == NULL ||
			gpC_base == NULL ||
			gpC_base->m_p_DDSurface_back == NULL)
		{
			return false;
		}

		CDirectDrawSurface *backSurface = gpC_base->m_p_DDSurface_back;
		if(!backSurface->Lock())
			return false;

		WORD *destBase = static_cast<WORD*>(backSurface->GetSurfacePointer());
		const int destPitchWords = static_cast<int>(backSurface->GetSurfacePitch() / sizeof(WORD));
		const int destWidth = backSurface->GetWidth();
		const int destHeight = backSurface->GetHeight();

		if(destBase == NULL || destPitchWords <= 0 || destWidth <= 0 || destHeight <= 0)
		{
			backSurface->Unlock();
			return false;
		}

		int sourceX = 0;
		int sourceY = 0;
		int destX = g_vampireExpTextCache.left;
		int destY = g_vampireExpTextCache.top;
		int copyWidth = g_vampireExpTextCache.width;
		int copyHeight = g_vampireExpTextCache.height;

		if(destX < 0)
		{
			sourceX = -destX;
			copyWidth += destX;
			destX = 0;
		}
		if(destY < 0)
		{
			sourceY = -destY;
			copyHeight += destY;
			destY = 0;
		}
		if(destX + copyWidth > destWidth)
			copyWidth = destWidth - destX;
		if(destY + copyHeight > destHeight)
			copyHeight = destHeight - destY;

		if(copyWidth <= 0 || copyHeight <= 0)
		{
			backSurface->Unlock();
			return true;
		}

		for(int row = 0; row < copyHeight; ++row)
		{
			const WORD *src = g_vampireExpTextCache.bits + (sourceY + row) * g_vampireExpTextCache.pitchWords + sourceX;
			WORD *dst = destBase + (destY + row) * destPitchWords + destX;
			for(int col = 0; col < copyWidth; ++col)
			{
				const WORD pixel = src[col];
				if(pixel != PHOENIX_VAMPIRE_EXP_TEXT_CACHE_KEY)
					dst[col] = pixel;
			}
		}

		// Faz G5 (2026-09-01) — dx11gpu: ham colorkey blit dirty bildirimi
		{
			phoenix::render::D3D11SpriteCommands& cmds =
				phoenix::render::D3D11SpriteCommands::Instance();
			if(cmds.IsActive() && cmds.IsCompositionTarget(destBase))
			{
				RECT dirty;
				dirty.left   = destX;
				dirty.top    = destY;
				dirty.right  = destX + copyWidth;
				dirty.bottom = destY + copyHeight;
				cmds.MarkDirty(&dirty);
			}
		}

		backSurface->Unlock();
		return true;
	}

	static void Phoenix_DrawVampireExpTextLayerCached(HDC hdc, int cacheLeft, int cacheTop, int panelX, int panelY)
	{
		if(hdc == NULL || gpC_base == NULL)
			return;

		PrintInfo &pi = gpC_base->m_chatting_pi;
		char text[100];
		const int maxX = (245 - 55 * 3) / 4;

		wsprintf(text, "STR");
		int labelWidth = Phoenix_GetVampireCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawVampireCachedTextShadow(hdc, panelX + maxX - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);

		wsprintf(text, "%d", g_char_slot_ingame.STR_CUR);
		int valueWidth = Phoenix_GetVampireCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawVampireCachedTextShadow(hdc, panelX + maxX + labelWidth + (55 - labelWidth - valueWidth - 1) / 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "DEX");
		int dexLabelWidth = Phoenix_GetVampireCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawVampireCachedTextShadow(hdc, panelX + maxX * 2 + 55 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);

		wsprintf(text, "%d", g_char_slot_ingame.DEX_CUR);
		int dexValueWidth = Phoenix_GetVampireCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawVampireCachedTextShadow(hdc, panelX + maxX * 2 + 55 + dexLabelWidth + (55 - dexValueWidth - dexLabelWidth - 1) / 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "INT");
		int intLabelWidth = Phoenix_GetVampireCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawVampireCachedTextShadow(hdc, panelX + maxX * 3 + 55 * 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);

		wsprintf(text, "%d", g_char_slot_ingame.INT_CUR);
		int intValueWidth = Phoenix_GetVampireCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawVampireCachedTextShadow(hdc, panelX + maxX * 3 + 55 * 2 + intLabelWidth + (55 - intLabelWidth - intValueWidth - 1) / 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "%d~%d", g_char_slot_ingame.DAM2, g_char_slot_ingame.DAM);
		int textWidth = Phoenix_GetVampireCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawVampireCachedTextShadow(hdc, panelX + 45 + 21 + (60 - textWidth) / 2 - cacheLeft, panelY + 94 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "%d~%d", g_char_slot_ingame.DEFENSE, g_char_slot_ingame.PROTECTION);
		textWidth = Phoenix_GetVampireCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawVampireCachedTextShadow(hdc, panelX + 135 + 21 + (60 - textWidth) / 2 - cacheLeft, panelY + 94 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, ""); // Exp bar string EXP.Point
		Phoenix_DrawVampireCachedTextShadow(hdc, panelX + maxX - 10 - cacheLeft, panelY + 129 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);
	}
}

class C_VS_UI_TRIBE;
int		C_VS_UI_VAMPIRE_GEAR::m_slot_image[SLOT_SIZE] = {
	GEAR_SLAYER_XIANGLIAN, GEAR_VAMPRIE_YIFU, GEAR_VAMPRIE_SHOUZUO, GEAR_VAMPRIE_SHOUZUO, GEAR_SLAYER_JIEZHI, GEAR_SLAYER_JIEZHI, GEAR_SLAYER_JIEZHI, GEAR_SLAYER_JIEZHI, GEAR_VAMPRIE_ERHUAN, GEAR_VAMPRIE_ERHUAN, GEAR_VAMPRIE_WEAPON, GEAR_VAMPRIE_WEAPON, GEAR_VAMPRIE_QIU, GEAR_VAMPRIE_QIU, GEAR_VAMPRIE_QIU, GEAR_VAMPRIE_QIU
	,-1
	,-1
	,-1
	,-1
	,GEAR_VAMPRIE_WENXIN,GEAR_VAMPRIE_MIANJU
	,-1// nanomech 2006.03.09 Item ���� �۾�
	,-1,-1	// Cue Of Adam
	,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU
};


//
// help string
//
//
// help string
//
/*static S_DEFAULT_HELP_STRING	g_help_string[9] = {
	{"Inventory", "(������â)", "TAB"},
	{"PartyManager", "(��Ƽâ)", "Ctrl+P"},
	{"Character Info", "(ĳ���� ����)", "Ctrl+I"},
	{"Magic Info", "(���� ����)", "Ctrl+K"},
	{"Help", "(����)", "Ctrl+H"},
	{"Gear", "(����â)", "Ctrl+TAB"},
	{"Game Menu", "(���� �޴�)", "ESC"},
	{"Mini-Map", "(�̴ϸ�)", "Ctrl+M"},
	{"Mark", "(Ư������)", "Ctrl+X"},
};

static S_DEFAULT_HELP_STRING	g_chat_help_string[5] = {
	{"Extend Window", "(ä��â Ȯ��)", "Ctrl+E"},
	{"Chat", "(�Ϲ� ä��)", "Ctrl+C"},
	{"Clan/Party Chat", "(Ŭ��/��Ƽ ä��)", "Ctrl+G"},
	{"Whisper", "(�ӼӸ�)", "Ctrl+W"},
	{"ZoneChat", "(��ġ��)", "Ctrl+Z"},
};*/

//----------------------------------------------------------------------------
// Operations
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_VAMPIRE::C_VS_UI_VAMPIRE():C_VS_UI_TRIBE()
{
	CT_Done("DIAG: C_VS_UI_VAMPIRE ctor enter (post C_VS_UI_TRIBE)");

	//Set(RESOLUTION_X-m_pC_main_spk->GetWidth()+1, RESOLUTION_Y-m_pC_main_spk->GetHeight()+1, m_pC_main_spk->GetWidth()-1, m_pC_main_spk->GetHeight()-1);

	CT_Done("DIAG: C_VS_UI_VAMPIRE pre SetupMenuItems(SPK_VAMPIRE_MAIN)");
	SetupMenuItems(SPK_VAMPIRE_MAIN);
	CT_Done("DIAG: C_VS_UI_VAMPIRE post SetupMenuItems");

	// common buttons
//	m_pC_common_button_group->Add( new C_VS_UI_EVENT_BUTTON(tab_x, tab_y, m_pC_sys_button_spk->GetWidth(TAB_MSG), m_pC_sys_button_spk->GetHeight(TAB_MSG), TAB_MSG_ID, this, TAB_MSG) );
//	tab_x += m_pC_sys_button_spk->GetWidth(TAB_MSG);
	// 2004, 12, 2, sobeit add start - ���
	// 2004, 12, 2, sobeit add end

// 	int system_x = w-m_pC_main_spk->GetWidth(BUTTON_SYSTEM)-5, system_y = h-m_pC_main_spk->GetHeight(BUTTON_SYSTEM);
// 	m_pC_common_button_group->Add( new C_VS_UI_EVENT_BUTTON(system_x, system_y, m_pC_main_spk->GetWidth(BUTTON_SYSTEM), m_pC_main_spk->GetHeight(BUTTON_SYSTEM), SYSTEM_ID, this, BUTTON_SYSTEM) );
// #if	__CONTENTS(__USER_GRADE)
// 	m_pC_common_button_group->Add( new C_VS_UI_EVENT_BUTTON(system_x - m_pC_main_spk->GetWidth(VAMPIRE_USER_GRADE)-5, system_y, m_pC_main_spk->GetWidth(VAMPIRE_USER_GRADE), m_pC_main_spk->GetHeight(VAMPIRE_USER_GRADE), USER_GRADE, this, VAMPIRE_USER_GRADE));
// #endif
// 	m_pC_common_button_group->Add( new C_VS_UI_EVENT_BUTTON(24, system_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), PUSHPIN_ID, this,C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN ) );

// 	const int button_x = 24, button_y = 78, button_x_gap = 37, button_y_gap = 37;
	
	// menu buttons
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*0, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_INVENTORY), m_pC_sys_button_spk->GetHeight(BUTTON_INVENTORY), INVENTORY_ID, this, BUTTON_INVENTORY) );
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_GEAR), m_pC_sys_button_spk->GetHeight(BUTTON_GEAR), GEAR_ID, this, BUTTON_GEAR) );
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*2, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_INFO), m_pC_sys_button_spk->GetHeight(BUTTON_INFO), INFO_ID, this, BUTTON_INFO) );
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*0, button_y+button_y_gap, m_pC_sys_button_spk->GetWidth(BUTTON_PARTY), m_pC_sys_button_spk->GetHeight(BUTTON_PARTY), PARTY_ID, this, BUTTON_PARTY) );
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y+button_y_gap, m_pC_sys_button_spk->GetWidth(BUTTON_QUEST), m_pC_sys_button_spk->GetHeight(BUTTON_QUEST), QUEST_ID, this, BUTTON_QUEST) );
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*2, button_y+button_y_gap, m_pC_sys_button_spk->GetWidth(BUTTON_MAIL), m_pC_sys_button_spk->GetHeight(BUTTON_MAIL), MAIL_ID, this, BUTTON_MAIL) );

	// sms ��ư 
// #if __CONTENTS(__SMS_SERVICES)
// 	if(false == g_pUserInformation->IsNetmarble)
// 		m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*3, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_SMS), m_pC_sys_button_spk->GetHeight(BUTTON_SMS), SMS_ID, this, BUTTON_SMS) );
// #endif	//__SMS_SERVICES
	
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*3, button_y+button_y_gap, m_pC_sys_button_spk->GetWidth(BUTTON_NAMING), m_pC_sys_button_spk->GetHeight(BUTTON_NAMING), NAMING_ID, this, BUTTON_NAMING) );

	// guild buttons
// 	m_pC_guild_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*0, button_y+button_y_gap*0, m_pC_sys_button_spk->GetWidth(BUTTON_TEAM_INFO), m_pC_sys_button_spk->GetHeight(BUTTON_TEAM_INFO), TEAM_INFO_ID, this, BUTTON_TEAM_INFO) );
// 	m_pC_guild_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y+button_y_gap*0, m_pC_sys_button_spk->GetWidth(BUTTON_TEAM_MEMBER_LIST), m_pC_sys_button_spk->GetHeight(BUTTON_TEAM_MEMBER_LIST), TEAM_MEMBER_LIST_ID, this, BUTTON_TEAM_MEMBER_LIST) );
	// 2004, 10, 12, sobeit add start - ��� ���� ������
//	if(false == g_pUserInformation->IsNetmarble) // 2005, 1, 12, sobeit modify - �ݸ��� ��� ���� ��
// 	{
// 		m_pC_guild_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*0, button_y+button_y_gap*1, m_pC_sys_button_spk->GetWidth(BUTTON_GUILD_LIST), m_pC_sys_button_spk->GetHeight(BUTTON_GUILD_LIST), TEAM_LIST_ID, this, BUTTON_GUILD_LIST) );
// 		m_pC_guild_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y+button_y_gap*1, m_pC_sys_button_spk->GetWidth(BUTTON_WAIT_GUILD_LIST), m_pC_sys_button_spk->GetHeight(BUTTON_WAIT_GUILD_LIST), TEAM_WAIT_LIST_ID, this, BUTTON_WAIT_GUILD_LIST) );
// 		m_pC_guild_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*2, button_y+button_y_gap*1, m_pC_sys_button_spk->GetWidth(BUTTON_UNION), m_pC_sys_button_spk->GetHeight(BUTTON_UNION), TEAM_UNION_ID, this, BUTTON_UNION) );
// 	}
	// 2004, 10, 12, sobeit add end - ��� ���� ������

	// 2004, 12, 2, sobeit add start - ���
// 	m_pC_util_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*0, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_STORE), m_pC_sys_button_spk->GetHeight(BUTTON_STORE), UTIL_STORE_ID, this, BUTTON_STORE) );
// 	if(false == g_pUserInformation->IsNetmarble && false == g_pUserInformation->IsTestServer) // ������
// 	{
//#if __CONTENTS(__POWER_JJANG_ID)
//		m_pC_util_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_POWER_JJANG), m_pC_sys_button_spk->GetHeight(BUTTON_POWER_JJANG), UTIL_POWER_JJANG_ID, this, BUTTON_POWER_JJANG) );
//#endif	//__POWER_JJANG_ID
// 		int i = 0;
// #if __CONTENTS(__DARKEDEN_MARKET)
// 		m_pC_util_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*i, button_y+button_y_gap*1, m_pC_sys_button_spk->GetWidth(BUTTON_MARKETM), m_pC_sys_button_spk->GetHeight(BUTTON_MARKETM), UTIL_MARKETM_ID, this, BUTTON_MARKETM) );
// 		i++;
// #endif //__DARKEDEN_MARKET
// 		m_pC_util_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*i, button_y+button_y_gap*1, m_pC_sys_button_spk->GetWidth(BUTTON_MARKETB), m_pC_sys_button_spk->GetHeight(BUTTON_MARKETB), UTIL_MARKETB_ID, this, BUTTON_MARKETB) );
// #if __CONTENTS(__FRIEND_ADDITION)
// 		i++;
// 		m_pC_util_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y , m_pC_sys_button_spk->GetWidth(BUTTON_FRIEND_SYSTEM), m_pC_sys_button_spk->GetHeight(BUTTON_FRIEND_SYSTEM), UTIL_FRIEND_SYSTEM, this, BUTTON_FRIEND_SYSTEM) );
// 		m_pC_util_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*i, button_y+button_y_gap*1 , m_pC_sys_button_spk->GetWidth(BUTTON_FRIEND_WAIT), m_pC_sys_button_spk->GetHeight(BUTTON_FRIEND_WAIT), UTIL_FRIEND_WAIT, this, BUTTON_FRIEND_WAIT) );
// #endif //__FRIEND_ADDITION
// 	}
// 	else /*if (g_pUserInformation->IsNetmarble)*/
// 	{
// 		m_pC_util_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y , m_pC_sys_button_spk->GetWidth(BUTTON_MARKETB), m_pC_sys_button_spk->GetHeight(BUTTON_MARKETB), UTIL_MARKETB_ID, this, BUTTON_MARKETB) );
// #if __CONTENTS(__FRIEND_ADDITION)
// 		m_pC_util_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*2, button_y , m_pC_sys_button_spk->GetWidth(BUTTON_FRIEND_SYSTEM), m_pC_sys_button_spk->GetHeight(BUTTON_FRIEND_SYSTEM), UTIL_FRIEND_SYSTEM, this, BUTTON_FRIEND_SYSTEM) );
// 		m_pC_util_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*2, button_y+button_y_gap*1 , m_pC_sys_button_spk->GetWidth(BUTTON_FRIEND_WAIT), m_pC_sys_button_spk->GetHeight(BUTTON_FRIEND_WAIT), UTIL_FRIEND_WAIT, this, BUTTON_FRIEND_WAIT) );
// #endif //__FRIEND_ADDITION	
// 	}
	// 2004, 12, 2, sobeit add end

	// help buttons
// 	m_pC_help_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*0, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_HELP), m_pC_sys_button_spk->GetHeight(BUTTON_HELP), HELP_ID, this, BUTTON_HELP) );
// 	m_pC_help_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_CHAT_HELP), m_pC_sys_button_spk->GetHeight(BUTTON_CHAT_HELP), CHAT_HELP_ID, this, BUTTON_CHAT_HELP) );
// 	m_pC_help_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*2, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_BATTLE_HELP), m_pC_sys_button_spk->GetHeight(BUTTON_BATTLE_HELP), BATTLE_HELP_ID, this, BUTTON_BATTLE_HELP) );
// 	m_pC_help_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*0, button_y+button_y_gap, m_pC_sys_button_spk->GetWidth(BUTTON_SKILL_HELP), m_pC_sys_button_spk->GetHeight(BUTTON_SKILL_HELP), SKILL_HELP_ID, this, BUTTON_SKILL_HELP) );
// 	m_pC_help_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y+button_y_gap, m_pC_sys_button_spk->GetWidth(BUTTON_TEAM_MEMBER_LIST), m_pC_sys_button_spk->GetHeight(BUTTON_TEAM_MEMBER_LIST), GUILD_HELP_ID, this, BUTTON_TEAM_MEMBER_LIST) );
// 	m_pC_help_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*2, button_y+button_y_gap, m_pC_sys_button_spk->GetWidth(BUTTON_WORLD_MAP), m_pC_sys_button_spk->GetHeight(BUTTON_WORLD_MAP), WORLD_MAP_HELP_ID, this, BUTTON_WORLD_MAP) );

	// instance objects
	CT_Done("DIAG: C_VS_UI_VAMPIRE pre new C_VS_UI_VAMPIRE_GEAR (gear)");
	m_pC_gear = new C_VS_UI_VAMPIRE_GEAR;
	CT_Done("DIAG: C_VS_UI_VAMPIRE post gear=%p, pre new gear_other", m_pC_gear);
	m_pC_gear_other = new C_VS_UI_VAMPIRE_GEAR;
	CT_Done("DIAG: C_VS_UI_VAMPIRE post gear_other=%p, pre GearOther(true)", m_pC_gear_other);
	m_pC_gear_other->GearOther(true);
	CT_Done("DIAG: C_VS_UI_VAMPIRE post GearOther");

	m_pC_quickitem = new C_VS_UI_SLAYER_QUICKITEM(SPK_VAMPIRE_QUICKITEM);

	m_time = "";
	m_date = "";

	memcpy(m_HotKey_Type, gpC_vs_ui_window_manager->GetHotKeyType(), sizeof(int)*C_VS_UI_SLAYER_QUICKITEM::HOTKEY_MAX);

#ifndef _LIB
	SetTime("12:34:05");
	SetDate("2001/06/30");
#endif
	CT_Done("DIAG: C_VS_UI_VAMPIRE ctor exit");
}

//-----------------------------------------------------------------------------
// ~C_VS_UI_VAMPIRE
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_VAMPIRE::~C_VS_UI_VAMPIRE()
{
	Phoenix_ReleaseVampireExpTextCache();

	DeleteNew(m_pC_gear);
	DeleteNew(m_pC_gear_other);
	DeleteNew(m_pC_quickitem);

	gpC_vs_ui_window_manager->SetHotkeyType((int *)m_HotKey_Type);
}

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::SkillTreeRunning
//
// 
//-----------------------------------------------------------------------------
//bool	C_VS_UI_VAMPIRE::SkillTreeRunning() const
//{
//	return (m_pC_skilltree_vampire != NULL);
//}

/*
//-----------------------------------------------------------------------------
// DrawEnergy
//
// HP�� ����Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::DrawEnergy()
{
	POINT point = {3,30};
	RECT rt = {0,0,m_p_time_surface->GetWidth(),m_p_time_surface->GetHeight()};
	gpC_base->m_p_DDSurface_back->Blt(&point, m_p_time_surface, &rt);

	// draw energy 'case'
	m_pC_etc_spk->Blt(ENERGY_CASE_X, ENERGY_CASE_Y, ENERGY_CASE);

	Rect rect(0, 0, m_hp_line, m_pC_etc_spk->GetHeight(ENERGY_HP));
	m_pC_etc_spk->BltClip(ENERGY_CASE_X+5, ENERGY_CASE_Y+22, rect, ENERGY_HP);

	if(m_silver)
	{
		Rect rect(m_silver_hp_line, 0, m_pC_etc_spk->GetWidth(ENERGY_HP)-m_silver_hp_line, m_pC_etc_spk->GetHeight(ENERGY_HP));
		m_pC_etc_spk->BltClip(ENERGY_CASE_X+5, ENERGY_CASE_Y+22, rect, ENERGY_HP_SILVER);
	}

	const int date_x = 14, date_y = ENERGY_CASE_Y+49;
	const int time_x = 14, time_y = ENERGY_CASE_Y+71;

	g_FL2_GetDC();

	g_PrintColorStr(date_x +68 - g_GetStringWidth(m_date.c_str(), gpC_base->m_info_pi.hfont)/2, date_y, m_date.c_str() ,gpC_base->m_info_pi, RGB_WHITE);	
	g_PrintColorStr(time_x + 40 - g_GetStringWidth(m_time.c_str(), gpC_base->m_info_pi.hfont)/2, time_y, m_time.c_str(), gpC_base->m_info_pi, RGB_WHITE);	

	g_FL2_ReleaseDC();

	
	// ���� �̰� ��¾��Ҷ��� �Ȱ� ���Ƽ�.. by sigi
//	char sz_info_buf[15];
//	sprintf(sz_info_buf, "(%d/%d)", m_hp_cur, m_hp_max);
//	g_Print(270, 9, sz_info_buf);
	
}
*/
/*
//-----------------------------------------------------------------------------
// DrawMinimap
//
// �̴ϸ�, ��ǥ, ���̸��� ǥ���Ѵ�
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::DrawMinimap()
{
	int _x, _y, map_x_x, map_x_y, map_y_x, map_y_y;

	if(!m_bl_minimap)
	{
		_x = RESOLUTION_X - m_pC_etc_spk->GetWidth(MINIMAP_CASE);
		_y = 0;

		m_pC_etc_spk->Blt(_x, _y, MINIMAP_CASE);
		
		map_x_x = _x+71;
		map_x_y = _y + 24;
		map_y_x = map_x_x;
		map_y_y = _y + 45;
	}
	else
	{
		_x = RESOLUTION_X - m_pC_etc_spk->GetWidth(MINIMAP_CASE_EXTEND);
		_y = 0;

		m_pC_etc_spk->Blt(_x, _y, MINIMAP_CASE_EXTEND);

		RECT rt = {0,0,m_p_minimap_surface->GetWidth(),m_p_minimap_surface->GetHeight()};
		gpC_base->m_p_DDSurface_back->Blt(&m_map_start_point, m_p_minimap_surface, &rt);
		
		map_x_x = _x+71;
		map_x_y = _y + 24+12;
		map_y_x = map_x_x;
		map_y_y = _y + 45+12;

		int map_w = 200, map_h = 100;
		if(m_map_w != m_map_h)
		{
			if(m_map_w > m_map_h)map_h = map_h * m_map_h / m_map_w;
			if(m_map_h > m_map_w)map_w = map_w * m_map_w / m_map_h;
		}

		if(gpC_base->m_p_DDSurface_back->Lock())
		{
			WORD *mem = (WORD *)gpC_base->m_p_DDSurface_back->GetSurfacePointer();
			long pitch = gpC_base->m_p_DDSurface_back->GetSurfacePitch();

			int map_x = m_map_start_point.x + m_map_x*map_w/m_map_w + (200 - map_w)/2,
				map_y = m_map_start_point.y + m_map_y*map_h/m_map_h + (100 - map_h)/2;

			mem[(map_y)*pitch/2 + map_x] = 0xffff;
			mem[(map_y+1)*pitch/2 + map_x+1] = 0xffff;
			mem[(map_y-1)*pitch/2 + map_x-1] = 0xffff;
			mem[(map_y+1)*pitch/2 + map_x-1] = 0xffff;
			mem[(map_y-1)*pitch/2 + map_x+1] = 0xffff;

			static int color=21;
			static int color_direct = 1;
			if(Timer())
			{
				color += color_direct;
				if(color > 30 || color < 21)color_direct *= -1;
			}

			BYTE r = 31, g = 63, b = 31;
			WORD portal_color = CDirectDraw::Color(r, g, b);

			int i, _x, _y;
			WORD _color, _color2;

			r = color, g = color-10, b = color;
			_color = CDirectDraw::Color(r, g, b);
			r = color*2/3, g = (color-10)*2/3, b = color*2/3;
			_color2 = CDirectDraw::Color(r, g, b);
			for(int i = 0; i < m_portal.size(); i++)
			{
				_x = m_map_start_point.x + (m_portal[i].left+m_portal[i].right)/2*map_w/m_map_w + (200 - map_w)/2;
				_y = m_map_start_point.y + (m_portal[i].top+m_portal[i].bottom)/2*map_h/m_map_h + (100 - map_h)/2;

				mem[(_y)*pitch/2 + _x] = _color;
				mem[(_y-1)*pitch/2 + _x-1] = _color2;
				mem[(_y-1)*pitch/2 + _x] = _color;
				mem[(_y-1)*pitch/2 + _x+1] = _color2;
				mem[(_y-2)*pitch/2 + _x-1] = _color;
				mem[(_y-2)*pitch/2 + _x] = _color;
				mem[(_y-2)*pitch/2 + _x+1] = _color;

			}

			r = color-10, g = color, b = color-10;
			_color = CDirectDraw::Color(r, g, b);
			for(int i = 0; i < m_npc.size(); i++)
			{
				_x = m_map_start_point.x + m_npc[i].x*map_w/m_map_w + (200 - map_w)/2;
				_y = m_map_start_point.y + m_npc[i].y*map_h/m_map_h + (100 - map_h)/2;

				mem[(_y)*pitch/2 + _x] = _color;
				mem[(_y-1)*pitch/2 + _x] = _color;
				mem[(_y-2)*pitch/2 + _x-1] = _color;
				mem[(_y-2)*pitch/2 + _x] = _color;
				mem[(_y-2)*pitch/2 + _x+1] = _color;
				mem[(_y-3)*pitch/2 + _x] = _color;
			}
			gpC_base->m_p_DDSurface_back->Unlock();
		}

	}
	
	char sz_temp[100];


//	m_map_x = 125;
//	m_map_y = 208;

	g_FL2_GetDC();

	if(m_bl_minimap)
	{
		g_PrintColorStr(RESOLUTION_X - 115 - g_GetStringWidth(m_zone_name.c_str(), gpC_base->m_info_pi.hfont)/2, _y+9, m_zone_name.c_str(), gpC_base->m_info_pi, RGB_WHITE);	
	}

	wsprintf(sz_temp, "%d", m_map_x);
	g_PrintColorStr(map_x_x - g_GetStringWidth(sz_temp, gpC_base->m_info_pi.hfont)/2, map_x_y, sz_temp, gpC_base->m_info_pi, RGB_WHITE);	
	wsprintf(sz_temp, "%d", m_map_y);
	g_PrintColorStr(map_y_x - g_GetStringWidth(sz_temp, gpC_base->m_info_pi.hfont)/2, map_y_y, sz_temp, gpC_base->m_info_pi, RGB_WHITE);	

	g_FL2_ReleaseDC();


}
*/
/*
//-----------------------------------------------------------------------------
// SetZone
//
// ZoneID�� �޾Ƽ� �̴ϸ��� �ҷ��´� & ���̸� ����
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::SetZone(int zone_id)
{
	const int id_size = 39;
	int id[id_size] = {11, 12, 13, 14, 21, 22, 23 ,24, 31, 32, 33, 34, 1001, 1002, 1003, 1004, 1005, 1006, 2000, 2001, 2002, 2003, 2004, 2010, 2011, 2012, 2013, 2014, 2020, 2021, 2022, 2023, 2024, 2101, 2102, 2103, 2104, 2105, 2106};
	char map_name[id_size][20] =
	{
		// �ʵ�
		"������Ͼ� NE", "������Ͼ� NW", "������Ͼ� SE", "������Ͼ� SW",
		"������ NE", "������ NW", "������ SE", "������ SW",
		"��κ�Ÿ NE", "��κ�Ÿ NW", "��κ�Ÿ SE", "��κ�Ÿ SW",

		// ���� & ����
		"���� ������", "������Ͼ� ����", "�����̾� ����", "������ ����", "�̺�Ʈ OX", "�̺�Ʈ �����",

		// �ǹ�
		"���� ��� B1", "���� ��� 1F", "���� ��� 2F", "���� ��� 3F", "���� ��� ����",
		"������ ��� B1", "������ ��� 1F", "������ ��� 2F", "������ ��� 3F", "������ ��� ����",
		"���� ��� B1", "���� ��� 1F", "���� ��� 2F", "���� ��� 3F", "���� ��� ����",

		// Ʃ�丮��
		"����", "�긮�η�", "��ǻ�ͽ�", "������", "������", "�����",
	};

	int spk_id[id_size] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
							12, 13, 14, 13, 15, 27, 
							26, 16, 17, 18, 24,
							26, 28, 29, 30, 24,
							26, 31, 32, 33, 24,
							19, 20, 21, 22, 23, 25};

	for(int i = 0; i < id_size; i++)
		if(zone_id == id[i])break;

	if(i == id_size)return;
	m_zone_name = map_name[i];

	CSpritePack		m_minimap_spk;
	
	ifstream	file(SPK_MINIMAP, ios::binary|ios::nocreate);
	if (!file)
		_Error(FILE_OPEN);
	m_minimap_spk.LoadFromFile(file);
	file.close();

	POINT point = {0, 0};

	m_minimap_surface.FillSurface(0x0000);
	m_minimap_surface.SetTransparency( 0xffff );

	m_minimap_surface.Lock();
	m_minimap_surface.BltSprite(&point, &m_minimap_spk[spk_id[i]]);
	m_minimap_surface.Unlock();

	m_minimap_spk.Release();
}

//-----------------------------------------------------------------------------
// SetSafetyZone
//
// �������� ����
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::SetSafetyZone(RECT rect)
{
	m_minimap_surface.Lock();
	WORD *mem = (WORD *)m_minimap_surface.GetSurfacePointer();
	int pitch = m_minimap_surface.GetSurfacePitch();

	int map_w = 200, map_h = 100;
	if(m_map_w != m_map_h)
	{
		if(m_map_w > m_map_h)map_h = map_h * m_map_h / m_map_w;
		if(m_map_h > m_map_w)map_w = map_w * m_map_w / m_map_h;
	}
	
	for(int y = rect.top*map_h/m_map_h + (100 - map_h)/2; y <= rect.bottom*map_h/m_map_h + (100 - map_h)/2; y++)
	{
		for(int x = rect.left*map_w/m_map_w + (200 - map_w)/2; x <= rect.right*map_w/m_map_w + (200 - map_w)/2; x++)
		{
			mem[y*pitch/2+x] = mem[y*pitch/2+x] & CDirectDraw::Get_G_Bitmask();
		}
	}

	m_minimap_surface.Unlock();
}
*/

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::GetGearWindow
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_GEAR * C_VS_UI_VAMPIRE::GetGearOtherWindow() const
{
	return m_pC_gear_other;
}

C_VS_UI_GEAR * C_VS_UI_VAMPIRE::GetGearWindow() const
{
	return m_pC_gear;
}

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::CloseGear
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_VAMPIRE::CloseGearOther()
{
	m_pC_gear_other->Finish();
}

void	C_VS_UI_VAMPIRE::CloseGear()
{
	m_pC_gear->Finish();
}

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::OpenGear
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_VAMPIRE::OpenGear(bool bl_set_load)
{
	m_pC_gear->Start(bl_set_load);
}

void	C_VS_UI_VAMPIRE::OpenGearOther(bool bl_set_load)
{
	m_pC_gear_other->Start(bl_set_load);
}


#if __CONTENTS(__GEAR_SWAP_CHANGE)		//C_VS_UI_VAMPIRE::RunGearChange()
void	C_VS_UI_VAMPIRE::RunGearChange()
{
	m_pC_gear->GearChange();
}
#endif //__GEAR_SWAP_CHANGE

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::CloseSkillTree
//
// 
//-----------------------------------------------------------------------------
//void	C_VS_UI_VAMPIRE::CloseSkillTree(SKILL_TREE_ARG arg)
//{
//	DeleteNew(m_pC_skilltree_vampire);
//}

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::RunSkillTree
//
// 
//-----------------------------------------------------------------------------
//void	C_VS_UI_VAMPIRE::RunSkillTree(SKILL_TREE_ARG arg)
//{
//	switch (arg)
//	{
//		case SKILLTREE_VAMPIRE:
//			if (m_pC_skilltree_vampire == NULL)
//			{
//				m_pC_skilltree_vampire = new C_VS_UI_SKILL_TREE_VAMPIRE;
//
//				m_pC_skilltree_vampire->Start();
//			}
//			break;
//	}
//}

//-----------------------------------------------------------------------------
// DoCommonActionBeforeEventOccured
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::DoCommonActionBeforeEventOccured()
{
	C_VS_UI_TRIBE::DoCommonActionBeforeEventOccured();
}

//-----------------------------------------------------------------------------
// DoCommonActionAfterEventOccured
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::DoCommonActionAfterEventOccured()
{
	C_VS_UI_TRIBE::DoCommonActionAfterEventOccured();
}

//-----------------------------------------------------------------------------
// Start
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::Start()
{
	C_VS_UI_TRIBE::Start();

	m_pC_skill->Start();
	m_pC_hpbar->Start();
	m_pC_BloodBurst->Start();
	//m_pC_Market->Start();
	m_pC_ShortCutSlot->Start();
	m_pC_effect_status->Start();
	m_pC_minimap->Start();
	if (m_pC_quickitem != NULL)
		m_pC_quickitem->Start();
	//m_pC_SimpleInforMation->Start();

	WindowEventReceiver(EVENT_WINDOW_MOVE);


}

//-----------------------------------------------------------------------------
// Show
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::ShowExp()
{
	//const int bar_x = 120, bar_y = 86, str_x = 70, num_x = 98, pure_x = 165, bar_gap = 14;
	char sz_temp[100];
	Rect rect;
	int imgX = (m_pC_main_spk->GetWidth(SIMPLEINFOMATION_MAIN)-m_pC_main_spk->GetWidth(EXP_BAR_BLUE))/2;

	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		int exp_remain = g_char_slot_ingame.EXP_REMAIN;
		__int64 goal_exp = 0;
		if(g_char_slot_ingame.m_AdvancementLevel>0)
		{
			if(g_char_slot_ingame.m_AdvancementLevel>100)
				goal_exp=g_pExperienceTable->GetAdvanceMent(150).GoalExp;
			else
				goal_exp=g_pExperienceTable->GetAdvanceMent(g_char_slot_ingame.m_AdvancementLevel).GoalExp;
		}
		else
		{
			goal_exp = g_pExperienceTable->GetVampireInfo( g_char_slot_ingame.level ).GoalExp;
		}
		int exp_width = m_pC_main_spk->GetWidth(EXP_BAR_BLUE);
		int exp_height = m_pC_main_spk->GetHeight(EXP_BAR_BLUE);
		int exp_bar = exp_width * (goal_exp - exp_remain) / goal_exp;
		
		rect.Set(0, 0, exp_bar, exp_height);
		
		
		m_pC_main_spk->BltLocked(x+50, y+130, EXP_BAR_BACKGROUD);
		m_pC_main_spk->BltLockedClip(x+51, y+131, rect, EXP_BAR_BLUE);



		m_pC_main_spk->BltLocked(x+45,y+90,SIMPLEINFOMATION_ATTACK);
		m_pC_main_spk->BltLocked(x+135,y+90,SIMPLEINFOMATION_DEFS);
		
		gpC_base->m_p_DDSurface_back->Unlock();		

	}

	

	const int vampireTextCacheLeft = x;
	const int vampireTextCacheTop = y + 68;
	const int vampireTextCacheWidth = 245;
	const int vampireTextCacheHeight = 80;
	bool vampireTextCacheBlt = false;

	if(Phoenix_EnsureVampireExpTextCacheSurface(vampireTextCacheWidth, vampireTextCacheHeight))
	{
		const DWORD textSignature = Phoenix_BuildVampireExpTextSignature(gpC_base->m_chatting_pi.hfont);
		if(Phoenix_NeedsVampireExpTextCacheBuild(vampireTextCacheLeft, vampireTextCacheTop, textSignature, gpC_base->m_chatting_pi.hfont))
		{
			HDC cacheDC = Phoenix_BeginVampireExpTextCacheBuild();
			if(cacheDC != NULL)
			{
				Phoenix_DrawVampireExpTextLayerCached(cacheDC, vampireTextCacheLeft, vampireTextCacheTop, x, y);
				Phoenix_CommitVampireExpTextCacheBuild(vampireTextCacheLeft, vampireTextCacheTop, textSignature, gpC_base->m_chatting_pi.hfont);
			}
		}

		vampireTextCacheBlt = Phoenix_BltVampireExpTextCache();
	}

	if(!vampireTextCacheBlt)
	{
		g_FL2_GetDC();
		//g_PrintColorStrShadow(x+str_x, y+bar_y+bar_gap*0, (*g_pGameStringTable)[UI_STRING_MESSAGE_DESC_LEVEL].GetString(), gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
		//wsprintf(sz_temp, "%d", g_char_slot_ingame.level);
		//g_PrintColorStrShadow(x+num_x, y+bar_y+bar_gap*0, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);

		int maxX = (245-55*3)/4;
		wsprintf(sz_temp,"STR");
		int max2 = g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont);
		g_PrintColorStrShadow(x+maxX, y+70, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
		//g_PrintColorStrShadow(x+maxX, y+70, (*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_STR].GetString(), gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
		wsprintf(sz_temp, "%d", g_char_slot_ingame.STR_CUR);
		int max3 = g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont);
		g_PrintColorStrShadow(x+maxX+max2+(55-max2-max3-1)/2, y+70, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);

		//wsprintf(sz_temp, "%d", g_char_slot_ingame.STR_PURE);
		//g_PrintColorStrShadow(x+pure_x, y+bar_y+bar_gap*1+8, sz_temp, gpC_base->m_chatting_pi, RGB_GRAY, RGB_BLACK);
		wsprintf(sz_temp,"DEX");
		int max4= g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont);
		g_PrintColorStrShadow(x+maxX*2+55, y+70, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
		wsprintf(sz_temp, "%d", g_char_slot_ingame.DEX_CUR);
		int max5 = g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont);
		g_PrintColorStrShadow(x+maxX*2+55+max4+(55-max5-max4-1)/2, y+70, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);

		wsprintf(sz_temp,"INT");
		int max6= g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont);
		g_PrintColorStrShadow(x+maxX*3+55*2, y+70, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
		wsprintf(sz_temp, "%d", g_char_slot_ingame.INT_CUR);
		int max7 = g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont);
		g_PrintColorStrShadow(x+maxX*3+55*2+max6+(55-max6-max7-1)/2, y+70, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);
		//wsprintf(sz_temp, "%d", g_char_slot_ingame.DEX_PURE);
		//g_PrintColorStrShadow(x+pure_x, y+bar_y+bar_gap*2+8, sz_temp, gpC_base->m_chatting_pi, RGB_GRAY, RGB_BLACK);

		////g_PrintColorStrShadow(x+imgX+20, y+70, (*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_INT].GetString(), gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
		////wsprintf(sz_temp, "%d", g_char_slot_ingame.INT_CUR);
		////g_PrintColorStrShadow(x+imgX+20, y+70, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
		//wsprintf(sz_temp, "%d", g_char_slot_ingame.INT_PURE);
		//g_PrintColorStrShadow(x+pure_x, y+bar_y+bar_gap*3+8, sz_temp, gpC_base->m_chatting_pi, RGB_GRAY, RGB_BLACK);

		wsprintf(sz_temp,"%d~%d",g_char_slot_ingame.DAM2,g_char_slot_ingame.DAM) ;
		int max8 = g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont);
		g_PrintColorStrShadow(x+45+21+(60-max8)/2, y+94, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);

		wsprintf(sz_temp,"%d~%d",g_char_slot_ingame.DEFENSE,g_char_slot_ingame.PROTECTION) ;
		int max9 = g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont);
		g_PrintColorStrShadow(x+135+21+(60-max9)/2, y+94, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);

		wsprintf(sz_temp,""); // Exp bar string EXP.Point
		g_PrintColorStrShadow(x+maxX-10, y+133, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);


		g_FL2_ReleaseDC();
	}


	SHOW_WINDOW_ATTR;

}
/*
//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::SetTime
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_VAMPIRE::SetTime(const char *str)
{
	if(str[0] != m_time[0] || str[1] != m_time[1])
	{
		CSpritePack		m_time_spk;
		
		ifstream	file(SPK_TIME, ios::binary|ios::nocreate);
		if (!file)
			_Error(FILE_OPEN);
		m_time_spk.LoadFromFile(file);
		file.close();
		
		POINT point = {0,0};
		RECT rect = {0,0,m_time_spk[0].GetWidth(),m_time_spk[0].GetHeight()};
		
		int time;

		time = str[0] - '0';
		if(str[1] != ':') time = time*10 + str[1]-'0';
		
		time = (time + 3)%24;

		int sprite = time % 6;
		int rotate = time/6;
		
		m_time_surface.FillSurface(0x001f);
		
		m_time_surface.Lock();
		
		WORD* surface = (WORD *)m_time_surface.GetSurfacePointer();
		int pitch = m_time_surface.GetSurfacePitch()/2;
		
		switch(rotate)
		{
		case 0:
			m_time_surface.BltSprite(&point, &m_time_spk[sprite]);
			break;
			
		case 1:
			{
				for(int i = 0, x = rect.right -1; i < rect.bottom; i++, x--)
					for(int j = 0, y = 0; j < rect.right; j++, y++)
					{
						if(m_time_spk[sprite].IsColorPixel(j, i))
						{
							surface[pitch * y + x] = m_time_spk[sprite].GetPixel(j, i);
						}
						
					}
			}
			break;

		case 2:
			{
				for(int i = 0, y = rect.bottom -1; i < rect.bottom; i++, y--)
					for(int j = 0, x = rect.right -1; j < rect.right; j++, x--)
					{
						if(m_time_spk[sprite].IsColorPixel(j, i))
						{
							surface[pitch * y + x] = m_time_spk[sprite].GetPixel(j, i);
						}
						
					}
			}
			break;

		case 3:
			{
				for(int i = 0, x = 0; i < rect.bottom; i++, x++)
					for(int j = 0, y = rect.bottom - 1; j < rect.right; j++, y--)
					{
						if(m_time_spk[sprite].IsColorPixel(j, i))
						{
							surface[pitch * y + x] = m_time_spk[sprite].GetPixel(j, i);
						}
						
					}
			}
			break;
		}

		m_time_surface.Unlock();

		m_time_spk.Release();
	}

	m_time = str;
}
*/
//-----------------------------------------------------------------------------
// Process
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::Process()
{
	C_VS_UI_TRIBE::Process();
}

//-----------------------------------------------------------------------------
// MouseControl
//
// 
//-----------------------------------------------------------------------------
//bool C_VS_UI_VAMPIRE::MouseControl(UINT message, int _x, int _y)
//{
//
//	if (gpC_mouse_pointer->GetPickUpItem() == NULL) // Item�� ��� ������ �Է��� �� ����.
//	{
//		m_pC_chatting->MouseControl(message, _x, _y);
//		m_pC_button_group->MouseControl(message, _x, _y);
//	}
/*	
	switch (message)
	{
	case M_MOVING:
		{
			// �̴ϸ� ���� ���콺�� �ִ�.
			if(_x >= m_map_start_point.x && _y >= m_map_start_point.y && _x < m_map_start_point.x+200 && _y < m_map_start_point.y+100)
			{
				int map_w = 200, map_h = 100;
				if(m_map_w != m_map_h)
				{
					if(m_map_w > m_map_h)map_h = map_h * m_map_h / m_map_w;
					if(m_map_h > m_map_w)map_w = map_w * m_map_w / m_map_h;
				}
				
				for(int i = 0; i < m_portal.size(); i++)
				{
					int x = m_map_start_point.x + (m_portal[i].left+m_portal[i].right)/2*map_w/m_map_w + (200 - map_w)/2;
					int y = m_map_start_point.y + (m_portal[i].top+m_portal[i].bottom)/2*map_h/m_map_h + (100 - map_h)/2;
					if(_x > x-5 && _x < x+5 && _y > y-5 && _y < y+5)
					{
						int print_x = gpC_mouse_pointer->GetPointerX();
						int print_y = gpC_mouse_pointer->GetPointerY();
						
						g_descriptor_manager.Set(DID_INFO, print_x, print_y, (void *)GetZoneName(m_portal_zone_id[i]));
						break;
					}
				}
				
				for(int i = 0; i < m_npc.size(); i++)
				{
					int x = m_map_start_point.x + m_npc[i].x*map_w/m_map_w + (200 - map_w)/2;
					int y = m_map_start_point.y + m_npc[i].y*map_h/m_map_h + (100 - map_h)/2;
					
					if(_x > x-5 && _x < x+5 && _y > y-5 && _y < y+5)
					{
						int print_x = gpC_mouse_pointer->GetPointerX();
						int print_y = gpC_mouse_pointer->GetPointerY();
						
						static char npc_name[50];
						if((*g_pCreatureTable)[m_npc[i].id].Name.GetString())
							strcpy(npc_name, (*g_pCreatureTable)[m_npc[i].id].Name.GetString());
						else strcpy(npc_name, "");
						
						g_descriptor_manager.Set(DID_INFO, print_x, print_y, (void *)npc_name);
						break;
					}
				}
//				C_VS_UI_EVENT_BUTTON * p_button;
//				p_button = m_pC_button_group->IsInRect(_x, _y);
//				if (p_button != NULL)
//				{
//					// help description
//					g_descriptor_manager.Set(DID_HELP, p_button->x, p_button->y, (void *)&g_help_string[p_button->GetID()-0xffff], 2);
//				}
			}
		}
		break;

	case M_LEFTBUTTON_DOWN:
	case M_LB_DOUBLECLICK:
	case M_LEFTBUTTON_UP:
	case M_RIGHTBUTTON_DOWN:
	case M_RIGHTBUTTON_UP:
//		if(_y < 300)return false;
		break;
	}

*///	return true;
//}

//-----------------------------------------------------------------------------
// KeyboardControl
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::KeyboardControl(UINT message, UINT key, long extra)
{
	m_pC_chatting->KeyboardControl(message, key, extra);
}


//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::HotKey_F5()
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::HotKey_F5()
{
	C_VS_UI_SKILL * p_skill = GetSkillWindow();

	int slot = p_skill->GetFocusedSlot();
	int id = p_skill->GetSkillID(slot);
	if (id != NOT_SELECTED)
	{
		p_skill->SetHotkey(C_VS_UI_SKILL::HOTKEY5, (ACTIONINFO)id);
	}
	else
	{
		// use
		p_skill->ChangeSkillByHotkey(C_VS_UI_SKILL::HOTKEY5);
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::HotKey_F6()
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::HotKey_F6()
{
	C_VS_UI_SKILL * p_skill = GetSkillWindow();

	int slot = p_skill->GetFocusedSlot();
	int id = p_skill->GetSkillID(slot);
	if (id != NOT_SELECTED)
	{
		p_skill->SetHotkey(C_VS_UI_SKILL::HOTKEY6, (ACTIONINFO)id);
	}
	else
	{
		// use
		p_skill->ChangeSkillByHotkey(C_VS_UI_SKILL::HOTKEY6);
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::HotKey_F7()
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::HotKey_F7()
{
	C_VS_UI_SKILL * p_skill = GetSkillWindow();

	int slot = p_skill->GetFocusedSlot();
	int id = p_skill->GetSkillID(slot);
	if (id != NOT_SELECTED)
	{
		p_skill->SetHotkey(C_VS_UI_SKILL::HOTKEY7, (ACTIONINFO)id);
	}
	else
	{
		// use
		p_skill->ChangeSkillByHotkey(C_VS_UI_SKILL::HOTKEY7);
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::HotKey_F8()
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::HotKey_F8()
{
	C_VS_UI_SKILL * p_skill = GetSkillWindow();

	int slot = p_skill->GetFocusedSlot();
	int id = p_skill->GetSkillID(slot);
	if (id != NOT_SELECTED)
	{
		p_skill->SetHotkey(C_VS_UI_SKILL::HOTKEY8, (ACTIONINFO)id);
	}
	else
	{
		// use
		p_skill->ChangeSkillByHotkey(C_VS_UI_SKILL::HOTKEY8);
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE::HotKey_F1()
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_VAMPIRE::HotKey_F1()
{
	//gpC_base->SendMessage(UI_USE_SERUM);

	const MItem* p_item = g_pInventory->FindItem( ITEM_CLASS_SERUM,5 ); // ��û
	if(g_pInventory->FindItem( ITEM_CLASS_SERUM,6 ) != NULL)
		p_item = g_pInventory->FindItem( ITEM_CLASS_SERUM, 6 ); // ������� - 3���� ��� ��밡��
	else if(g_pInventory->FindItem( ITEM_CLASS_SERUM, 3 ) != NULL)
		p_item = g_pInventory->FindItem( ITEM_CLASS_SERUM, 3);
	else if(g_pInventory->FindItem( ITEM_CLASS_SERUM,2 ) != NULL)
		p_item= g_pInventory->FindItem( ITEM_CLASS_SERUM,2 );
	else if(g_pInventory->FindItem( ITEM_CLASS_SERUM,1) != NULL)
		p_item= g_pInventory->FindItem( ITEM_CLASS_SERUM,1 );
	else if(g_pInventory->FindItem( ITEM_CLASS_SERUM,0 ) != NULL)
		p_item= g_pInventory->FindItem( ITEM_CLASS_SERUM,0 );
	
	if (p_item) // Item�� �ִ�.
	{
		gpC_base->SendMessage(UI_ITEM_USE,
								  p_item->GetID(), 0,
								  (MItem *)p_item);
	}
}

void C_VS_UI_VAMPIRE::HotKey_F2()
{
	const MItem* 	p_item = g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC, 20  ); // ��û
	if(g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC,20 ) != NULL)
		p_item = g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC, 20 ); // ������� - 3���� ��� ��밡��
	else if(g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC,21 ) != NULL)
		p_item = g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC, 21 ); // ������� - 3���� ��� ��밡��
	else if(g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC,14 ) != NULL)
		p_item = g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC, 14 ); // ������� - 3���� ��� ��밡��
	else if(g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC,17 ) != NULL)
		p_item = g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC, 17 ); // ������� - 3���� ��� ��밡��
	else if(g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC,16 ) != NULL)
		p_item = g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC, 16 ); // ������� - 3���� ��� ��밡��
	else if(g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC,15 ) != NULL)
		p_item = g_pInventory->FindItem( ITEM_CLASS_EVENT_ETC, 15 ); // ������� - 3���� ��� ��밡��
			
	if (p_item) // Item�� �ִ�.
	{
		gpC_base->SendMessage(UI_ITEM_USE,
								  p_item->GetID(), 0,
								  (MItem *)p_item);
	}
}


void	C_VS_UI_VAMPIRE::Hotkey_Function(int FKey)
{
	if(FKey == 0)	HotKey_F1() ; 
	else if(FKey == 1)	HotKey_F2()  ; 
	else
	{
	int	current_mouse_x = gpC_window_manager->GetCurrentMouse_x() ;
	int	current_mouse_y = gpC_window_manager->GetCurrentMouse_y() ;

	C_VS_UI_SKILL * p_skill = GetSkillWindow()		;
	bool	bSkillFocus = p_skill->IsPixel2(current_mouse_x , current_mouse_y) ; 
	bool	bQuickItemFocus = (m_pC_quickitem != NULL) && m_pC_quickitem->IsPixel(current_mouse_x , current_mouse_y) ; 

	if(bSkillFocus)
	{
		int skillslot = p_skill->GetFocusedSlot()		;
		int skillid   = p_skill->GetSkillID(skillslot)	;
		if (skillid != NOT_SELECTED)
		{
			p_skill->SetHotkey(C_VS_UI_SKILL::HOTKEY(FKey), (ACTIONINFO)skillid)	;
			if (m_pC_quickitem != NULL)
				m_pC_quickitem->InitHotKey(C_VS_UI_SLAYER_QUICKITEM::HOTKEY(FKey)) ; 
			m_HotKey_Type[C_VS_UI_SKILL::HOTKEY(FKey)] = SKILL_TYPE ;
		}
		else 
		{
			p_skill->ChangeSkillByHotkey(C_VS_UI_SKILL::HOTKEY(FKey)) ;
		}
	}	
	else if(bQuickItemFocus)
	{
		int QuickItemslot = m_pC_quickitem->GetFocusedSlot();
		if(QuickItemslot >= 0)
		{
			m_pC_quickitem->SetHotkey(C_VS_UI_SLAYER_QUICKITEM::HOTKEY(FKey) , QuickItemslot);
			p_skill->InitHotKey(C_VS_UI_SKILL::HOTKEY(FKey)) ;
			m_HotKey_Type[FKey] = QUICKITEM_TYPE ;
		}
	}
	else if(!bSkillFocus && !bQuickItemFocus)
	{
		if(m_HotKey_Type[FKey] == SKILL_TYPE) 
			p_skill->ChangeSkillByHotkey(C_VS_UI_SKILL::HOTKEY(FKey)) ; 
		else if (m_pC_quickitem != NULL)
		{
			int QuickItemslot = m_pC_quickitem->GetSlot(C_VS_UI_SLAYER_QUICKITEM::HOTKEY(FKey)) ;//
			m_pC_quickitem->Use(QuickItemslot);
		}
	}
	}
}

void C_VS_UI_VAMPIRE::RunQuickItemSlot()
{
	assert(m_pC_quickitem != NULL);
	m_pC_quickitem->Open();
}

void C_VS_UI_VAMPIRE::CloseQuickItemSlot()
{
	assert(m_pC_quickitem != NULL);
	m_pC_quickitem->Close();
}

bool C_VS_UI_VAMPIRE::IsRunningQuickItemSlot()
{
	assert(m_pC_quickitem != NULL);
	return true;
}

void C_VS_UI_VAMPIRE::ResetSlayerQuickItemSize()
{
	if (m_pC_quickitem != NULL)
		m_pC_quickitem->ResetSize();
}



//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE_GEAR::C_VS_UI_VAMPIRE_GEAR
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_VAMPIRE_GEAR::C_VS_UI_VAMPIRE_GEAR()
{
	assert(MVampireGear::MAX_GEAR_VAMPIRE == SLOT_SIZE);
	
//	if(g_char_slot_ingame.m_AdvancementLevel== 0)// 2������ bycsm 2004.12.31
//	{
		m_slot_rect[SN_NECKLACE].Set(25+66+36+10+24, 83+10+36, 36, 36);
		m_slot_rect[SN_COAT].Set(25+66+36+10+24-15, 83+10+36+10+36, 66, 96);
		m_slot_rect[SN_BRACELET1].Set(25+10+66, 83+10+36+10+36+15, 36, 36);
		m_slot_rect[SN_BRACELET2].Set(357-25-10-66-36, 83+10+36+10+36+15, 36, 36);
		m_slot_rect[SN_RING1].Set(60, 400, 36, 36);
		m_slot_rect[SN_RING2].Set(60+36+5, 400, 36, 36);
		m_slot_rect[SN_RING3].Set(357-60-36-5-36, 400, 36, 36);
		m_slot_rect[SN_RING4].Set(357-60-36, 400, 36, 36);
		m_slot_rect[SN_EARRING1].Set(25+10+66, 83, 36, 36);
		m_slot_rect[SN_EARRING2].Set(357-25-10-66-36, 83, 36, 36);
		m_slot_rect[SN_LEFTHAND].Set (25+10+66-30, 83+10+36+10+36+15+10+36, 66, 66);
		m_slot_rect[SN_RIGHTHAND].Set(357-25-10-66-36, 83+10+36+10+36+15+10+36, 66, 66);
		m_slot_rect[SN_AMULET1].Set(60, 400+36+5, 36, 36); 
		m_slot_rect[SN_AMULET2].Set(60+36+5, 400+36+5, 36, 36);
		m_slot_rect[SN_AMULET3].Set(357-60-36-5-36, 400+36+5, 36, 36);
		m_slot_rect[SN_AMULET4].Set(357-60-36, 400+36+5, 36, 36);

		m_slot_rect[SN_COREZAP1].Set(60, 400, 36, 36);
		m_slot_rect[SN_COREZAP2].Set(60+36+5, 400, 36, 36);
		m_slot_rect[SN_COREZAP3].Set(357-60-36-5-36, 400, 36, 36);
		m_slot_rect[SN_COREZAP4].Set(357-60-36, 400, 36, 36);

		m_slot_rect[SN_DERMIS].Set(357-25-66, 83, 66, 66);
		m_slot_rect[SN_PERSONA].Set(25, 83, 66, 66);


		m_slot_rect[SN_NECK_CHAIN].Set(25+66+36+10+24, 83+10+36, 36, 36);

		m_slot_rect[SN_CUEOFADAM1].Set(25+10+66, 83+10+36+10+36+15, 36, 36);
		m_slot_rect[SN_CUEOFADAM2].Set(357-25-10-66-36, 83+10+36+10+36+15, 36, 36);

		m_slot_rect[SN_BLOODBIBLE1].Set(70, 490, 36, 36);
		m_slot_rect[SN_BLOODBIBLE2].Set(70+36, 490, 36, 36);
		m_slot_rect[SN_BLOODBIBLE3].Set(70+36*2, 490, 36, 36);
		m_slot_rect[SN_BLOODBIBLE4].Set(70+36*3, 490, 36, 36);
		m_slot_rect[SN_BLOODBIBLE5].Set(70+36*4, 490, 36, 36);
		m_slot_rect[SN_BLOODBIBLE6].Set(70+36*5, 490, 36, 36);

		m_slot_size = SLOT_SIZE;

		
//	}
//	else
//	{
//		m_slot_rect[SN_NECKLACE].Set(110, 14, 30, 30);
//		m_slot_rect[SN_COAT].Set(95, 49, 60, 90);
//		m_slot_rect[SN_BRACELET1].Set(56, 109, 30, 30);
//		m_slot_rect[SN_BRACELET2].Set(164, 108, 30, 31);
//		m_slot_rect[SN_RING1].Set(56, 144, 30, 30);
//		m_slot_rect[SN_RING2].Set(92, 144, 30, 30);
//		m_slot_rect[SN_RING3].Set(128, 144, 30, 30);
//		m_slot_rect[SN_RING4].Set(164, 144, 30, 30);
//		m_slot_rect[SN_EARRING1].Set(59, 14, 30, 30);
//		m_slot_rect[SN_EARRING2].Set(161, 14, 30, 30);
//		m_slot_rect[SN_LEFTHAND].Set(28, 47, 60, 60);
//		m_slot_rect[SN_RIGHTHAND].Set(161, 46, 60, 60);
//		m_slot_rect[SN_AMULET1].Set(56, 178, 30, 30); 
//		m_slot_rect[SN_AMULET2].Set(92, 178, 30, 30);
//		m_slot_rect[SN_AMULET3].Set(128, 178, 30, 30);
//		m_slot_rect[SN_AMULET4].Set(164, 178, 30, 30);
//		
//		m_slot_rect[SN_COREZAP1].Set(56, 144, 30, 30);
//		m_slot_rect[SN_COREZAP2].Set(93, 144, 30, 30);
//		m_slot_rect[SN_COREZAP3].Set(128, 144, 30, 30);
//		m_slot_rect[SN_COREZAP4].Set(164, 144, 30, 30);
//
//		m_slot_rect[SN_PERSONA].Set(159, 45, 60, 60);
//		m_slot_rect[SN_DERMIS].Set(26, 45, 60, 60);
//		
//
//		
//		m_slot_rect[SN_BLOODBIBLE1].Set(20, 212, 30, 30);
//		m_slot_rect[SN_BLOODBIBLE2].Set(56, 212, 30, 30);
//		m_slot_rect[SN_BLOODBIBLE3].Set(92, 212, 30, 30);
//		m_slot_rect[SN_BLOODBIBLE4].Set(128, 212, 30, 30);
//		m_slot_rect[SN_BLOODBIBLE5].Set(164, 212, 30, 30);
//		m_slot_rect[SN_BLOODBIBLE6].Set(200, 212, 30, 30);
//		m_slot_size = SLOT_SIZE;
//	}
//	
	
	m_p_slot_rect = m_slot_rect;
	m_p_slot_image = m_slot_image;
}

//-----------------------------------------------------------------------------
// C_VS_UI_VAMPIRE_GEAR::~C_VS_UI_VAMPIRE_GEAR
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_VAMPIRE_GEAR::~C_VS_UI_VAMPIRE_GEAR()
{}
