// VS_UI_GameOusters.cpp

#include "client_PCH.h"
#include "crash_trace_tool.h"
#include "../../Client/PhoenixDWrite.h"  // FULL DX11 gecis Faz T3 — DWrite text (GDI fallback'li)
#include "../../Client/DXLib/D3D11SpriteCommands.h"  // Faz G5 — ham pixel yazilari dirty bildirimi
#include "VS_UI_GameOusters.h"
#include "VS_UI_filepath.h"
#include "VS_UI_Mouse_pointer.h"
#include "VS_UI.h"
#include "MGameStringTable.h"
#include "UserOption.h"
#include "ExperienceTable.h"
#include "UserInformation.h"
namespace
{
	const WORD PHOENIX_OUSTERS_EXP_TEXT_CACHE_KEY = 0x0001;

	struct PhoenixOustersExpTextCache
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

	static PhoenixOustersExpTextCache g_oustersExpTextCache =
	{
		NULL, NULL, NULL, NULL,
		0, 0, 0, 0, 0, 0, NULL, false
	};

	static void Phoenix_ReleaseOustersExpTextCache()
	{
		if(g_oustersExpTextCache.hdc != NULL && g_oustersExpTextCache.oldBitmap != NULL)
		{
			SelectObject(g_oustersExpTextCache.hdc, g_oustersExpTextCache.oldBitmap);
			g_oustersExpTextCache.oldBitmap = NULL;
		}
		if(g_oustersExpTextCache.bitmap != NULL)
		{
			DeleteObject(g_oustersExpTextCache.bitmap);
			g_oustersExpTextCache.bitmap = NULL;
		}
		if(g_oustersExpTextCache.hdc != NULL)
		{
			DeleteDC(g_oustersExpTextCache.hdc);
			g_oustersExpTextCache.hdc = NULL;
		}
		g_oustersExpTextCache.bits = NULL;
		g_oustersExpTextCache.width = 0;
		g_oustersExpTextCache.height = 0;
		g_oustersExpTextCache.pitchWords = 0;
		g_oustersExpTextCache.left = 0;
		g_oustersExpTextCache.top = 0;
		g_oustersExpTextCache.signature = 0;
		g_oustersExpTextCache.font = NULL;
		g_oustersExpTextCache.valid = false;
	}

	static bool Phoenix_EnsureOustersExpTextCacheSurface(int width, int height)
	{
		if(width <= 0 || height <= 0)
			return false;

		if(g_oustersExpTextCache.hdc != NULL &&
			g_oustersExpTextCache.bitmap != NULL &&
			g_oustersExpTextCache.bits != NULL &&
			g_oustersExpTextCache.width == width &&
			g_oustersExpTextCache.height == height)
		{
			return true;
		}

		Phoenix_ReleaseOustersExpTextCache();

		g_oustersExpTextCache.hdc = CreateCompatibleDC(NULL);
		if(g_oustersExpTextCache.hdc == NULL)
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
		g_oustersExpTextCache.bitmap = CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&bitmapInfo), DIB_RGB_COLORS, &bits, NULL, 0);
		if(g_oustersExpTextCache.bitmap == NULL || bits == NULL)
		{
			Phoenix_ReleaseOustersExpTextCache();
			return false;
		}

		g_oustersExpTextCache.oldBitmap = static_cast<HBITMAP>(SelectObject(g_oustersExpTextCache.hdc, g_oustersExpTextCache.bitmap));
		g_oustersExpTextCache.bits = static_cast<WORD*>(bits);
		g_oustersExpTextCache.width = width;
		g_oustersExpTextCache.height = height;
		g_oustersExpTextCache.pitchWords = (((width * 16 + 31) / 32) * 4) / sizeof(WORD);
		g_oustersExpTextCache.valid = false;
		return true;
	}

	static void Phoenix_FillOustersExpTextCache()
	{
		if(g_oustersExpTextCache.bits == NULL || g_oustersExpTextCache.pitchWords <= 0)
			return;

		for(int row = 0; row < g_oustersExpTextCache.height; ++row)
		{
			WORD *dst = g_oustersExpTextCache.bits + row * g_oustersExpTextCache.pitchWords;
			for(int col = 0; col < g_oustersExpTextCache.width; ++col)
				dst[col] = PHOENIX_OUSTERS_EXP_TEXT_CACHE_KEY;
		}
	}

	static DWORD Phoenix_MixOustersExpTextSignature(DWORD signature, int value)
	{
		return (signature ^ static_cast<DWORD>(value)) * 16777619u;
	}

	static DWORD Phoenix_BuildOustersExpTextSignature(int fireNum, int waterNum, int earthNum, HFONT font)
	{
		DWORD signature = 2166136261u;
		signature = Phoenix_MixOustersExpTextSignature(signature, g_char_slot_ingame.STR_CUR);
		signature = Phoenix_MixOustersExpTextSignature(signature, g_char_slot_ingame.DEX_CUR);
		signature = Phoenix_MixOustersExpTextSignature(signature, g_char_slot_ingame.INT_CUR);
		signature = Phoenix_MixOustersExpTextSignature(signature, fireNum);
		signature = Phoenix_MixOustersExpTextSignature(signature, waterNum);
		signature = Phoenix_MixOustersExpTextSignature(signature, earthNum);
		signature = Phoenix_MixOustersExpTextSignature(signature, static_cast<int>(reinterpret_cast<DWORD_PTR>(font)));
		return signature;
	}

	static bool Phoenix_NeedsOustersExpTextCacheBuild(int left, int top, DWORD signature, HFONT font)
	{
		return !g_oustersExpTextCache.valid ||
			g_oustersExpTextCache.left != left ||
			g_oustersExpTextCache.top != top ||
			g_oustersExpTextCache.signature != signature ||
			g_oustersExpTextCache.font != font;
	}

	static HDC Phoenix_BeginOustersExpTextCacheBuild()
	{
		if(g_oustersExpTextCache.hdc == NULL || g_oustersExpTextCache.bits == NULL)
			return NULL;

		Phoenix_FillOustersExpTextCache();
		return g_oustersExpTextCache.hdc;
	}

	static void Phoenix_CommitOustersExpTextCacheBuild(int left, int top, DWORD signature, HFONT font)
	{
		g_oustersExpTextCache.left = left;
		g_oustersExpTextCache.top = top;
		g_oustersExpTextCache.signature = signature;
		g_oustersExpTextCache.font = font;
		g_oustersExpTextCache.valid = true;
	}

	static int Phoenix_GetOustersCachedTextWidth(HDC hdc, const char *text, HFONT font)
	{
		if(hdc == NULL || text == NULL)
			return 0;
		if(font != NULL)
			SelectObject(hdc, font);

		SIZE textSize = {0, 0};
		GetTextExtentPoint32(hdc, text, static_cast<int>(strlen(text)), &textSize);
		return textSize.cx;
	}

	static int Phoenix_DrawOustersCachedTextShadow(HDC hdc, int x, int y, const char *text, const PrintInfo &pi, COLORREF textColor, COLORREF shadowColor)
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

	static bool Phoenix_BltOustersExpTextCache()
	{
		if(!g_oustersExpTextCache.valid ||
			g_oustersExpTextCache.bits == NULL ||
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
		int destX = g_oustersExpTextCache.left;
		int destY = g_oustersExpTextCache.top;
		int copyWidth = g_oustersExpTextCache.width;
		int copyHeight = g_oustersExpTextCache.height;

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
			const WORD *src = g_oustersExpTextCache.bits + (sourceY + row) * g_oustersExpTextCache.pitchWords + sourceX;
			WORD *dst = destBase + (destY + row) * destPitchWords + destX;
			for(int col = 0; col < copyWidth; ++col)
			{
				const WORD pixel = src[col];
				if(pixel != PHOENIX_OUSTERS_EXP_TEXT_CACHE_KEY)
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

	static void Phoenix_DrawOustersExpTextLayerCached(HDC hdc, int cacheLeft, int cacheTop, int panelX, int panelY, int fireNum, int waterNum, int earthNum)
	{
		if(hdc == NULL || gpC_base == NULL)
			return;

		PrintInfo &pi = gpC_base->m_chatting_pi;
		char text[100];
		const int maxX = (245 - 55 * 3) / 4;

		wsprintf(text, "STR");
		int labelWidth = Phoenix_GetOustersCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawOustersCachedTextShadow(hdc, panelX + maxX - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);

		wsprintf(text, "%d", g_char_slot_ingame.STR_CUR);
		int valueWidth = Phoenix_GetOustersCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawOustersCachedTextShadow(hdc, panelX + maxX + labelWidth + (55 - labelWidth - valueWidth - 1) / 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "DEX");
		int dexLabelWidth = Phoenix_GetOustersCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawOustersCachedTextShadow(hdc, panelX + maxX * 2 + 55 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);

		wsprintf(text, "%d", g_char_slot_ingame.DEX_CUR);
		int dexValueWidth = Phoenix_GetOustersCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawOustersCachedTextShadow(hdc, panelX + maxX * 2 + 55 + dexLabelWidth + (55 - dexValueWidth - dexLabelWidth - 1) / 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "INT");
		int intLabelWidth = Phoenix_GetOustersCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawOustersCachedTextShadow(hdc, panelX + maxX * 3 + 55 * 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);

		wsprintf(text, "%d", g_char_slot_ingame.INT_CUR);
		int intValueWidth = Phoenix_GetOustersCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawOustersCachedTextShadow(hdc, panelX + maxX * 3 + 55 * 2 + intLabelWidth + (55 - intLabelWidth - intValueWidth - 1) / 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "%d", fireNum);
		Phoenix_DrawOustersCachedTextShadow(hdc, panelX + 186 - cacheLeft, panelY + 89 - cacheTop, text, pi, RGB_RED, RGB_BLACK);
		wsprintf(text, "%d", waterNum);
		Phoenix_DrawOustersCachedTextShadow(hdc, panelX + 186 - cacheLeft, panelY + 103 - cacheTop, text, pi, RGB_RED, RGB_BLACK);
		wsprintf(text, "%d", earthNum);
		Phoenix_DrawOustersCachedTextShadow(hdc, panelX + 186 - cacheLeft, panelY + 117 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "");// Exp bar string EXP.Point
		Phoenix_DrawOustersCachedTextShadow(hdc, panelX + maxX - 10 - cacheLeft, panelY + 133 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);
	}
}

int		C_VS_UI_OUSTERS_GEAR::m_slot_image[SLOT_SIZE] = {
	GEAR_OUSTER_TOU, GEAR_OUSTER_YIFU, GEAR_OUSTER_WUQI_2, GEAR_OUSTER_WUQI_1, GEAR_OUSTER_XIEZI, GEAR_OUSTER_BIHUAN, GEAR_OUSTER_BIHUAN, GEAR_SLAYER_JIEZHI, GEAR_SLAYER_JIEZHI, GEAR_SLAYER_XIANGLIAN, GEAR_SLAYER_XIANGLIAN, GEAR_SLAYER_XIANGLIAN, GEAR_OUSTER_SHITOU_1, GEAR_OUSTER_SHITOU_2, GEAR_OUSTER_SHITOU_3, GEAR_OUSTER_SHITOU_4
	,-1
	,-1
	,-1
	,-1
	,GEAR_OUSTER_HUADAI,GEAR_OUSTER_SHOUTAO
	,-1// nanomech 2006.03.09 Item ���� �۾�
	,-1,-1 	// Cue Of Adam
	,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU
};

//----------------------------------------------------------------------------
// Operations
//----------------------------------------------------------------------------

MItem*	C_VS_UI_OUSTERS_QUICKITEM::GetItem(int slot)
{
	int pocketsize=0;

	if( g_pArmsBand1 != NULL)
		pocketsize += g_pArmsBand1->GetPocketNumber();
	if( g_pArmsBand2 != NULL)
		pocketsize += g_pArmsBand2->GetPocketNumber();
	
	if( slot < 0 || slot >= pocketsize )
		return NULL;
	
	if( g_pArmsBand1 != NULL && slot < g_pArmsBand1->GetPocketNumber() )
		return g_pArmsBand1->GetItem( slot );
	else
	{
		if( g_pArmsBand1 != NULL )
			return g_pArmsBand2->GetItem( slot - g_pArmsBand1->GetPocketNumber() );
		else
			return g_pArmsBand2->GetItem( slot );
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// C_VS_UI_OUSTERS
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_OUSTERS::C_VS_UI_OUSTERS():C_VS_UI_TRIBE()
{
	CT_Done("DIAG: C_VS_UI_OUSTERS ctor enter (post C_VS_UI_TRIBE)");
	m_pC_armsband = NULL;

	//Set(RESOLUTION_X-m_pC_main_spk->GetWidth()+1, RESOLUTION_Y-m_pC_main_spk->GetHeight()+1, m_pC_main_spk->GetWidth()-1, m_pC_main_spk->GetHeight()-1);

	CT_Done("DIAG: C_VS_UI_OUSTERS pre SetupMenuItems(SPK_OUSTERS_MAIN)");
	SetupMenuItems(SPK_OUSTERS_MAIN);
	CT_Done("DIAG: C_VS_UI_OUSTERS post SetupMenuItems");

	// common buttons
	// 2004, 12, 2, sobeit add start - ���
	// 2004, 12, 2, sobeit add end

// 	int system_x = w-m_pC_main_spk->GetWidth(BUTTON_SYSTEM)-5, system_y = h-m_pC_main_spk->GetHeight(BUTTON_SYSTEM);
// 	m_pC_common_button_group->Add( new C_VS_UI_EVENT_BUTTON(system_x, system_y, m_pC_main_spk->GetWidth(BUTTON_SYSTEM), m_pC_main_spk->GetHeight(BUTTON_SYSTEM), SYSTEM_ID, this, BUTTON_SYSTEM) );
// #if	__CONTENTS(__USER_GRADE)
// 	m_pC_common_button_group->Add( new C_VS_UI_EVENT_BUTTON(system_x - m_pC_main_spk->GetWidth(OUSTERS_USER_GRADE)-5, system_y, m_pC_main_spk->GetWidth(OUSTERS_USER_GRADE), m_pC_main_spk->GetHeight(OUSTERS_USER_GRADE), USER_GRADE, this, OUSTERS_USER_GRADE));
// #endif
// 	m_pC_common_button_group->Add( new C_VS_UI_EVENT_BUTTON(24, system_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), PUSHPIN_ID, this,C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN ) );

	const int button_x = 43, button_y = 89, button_x_gap = 37, button_y_gap = 37;
	
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
	//if(false == g_pUserInformation->IsNetmarble) // 2005, 1, 12, sobeit modify - �ݸ��� ��� ���� ��
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
	CT_Done("DIAG: C_VS_UI_OUSTERS pre new C_VS_UI_OUSTERS_GEAR (gear)");
	m_pC_gear = new C_VS_UI_OUSTERS_GEAR;
	CT_Done("DIAG: C_VS_UI_OUSTERS post gear=%p, pre new gear_other", m_pC_gear);
	m_pC_gear_other = new C_VS_UI_OUSTERS_GEAR;
	CT_Done("DIAG: C_VS_UI_OUSTERS post gear_other=%p, pre GearOther(true)", m_pC_gear_other);
	m_pC_gear_other->GearOther(true);
	CT_Done("DIAG: C_VS_UI_OUSTERS post GearOther, pre new armsband");

	m_pC_armsband = new C_VS_UI_OUSTERS_QUICKITEM;
	CT_Done("DIAG: C_VS_UI_OUSTERS post armsband=%p", m_pC_armsband);

	m_time = "";
	m_date = "";

	memcpy(m_HotKey_Type, gpC_vs_ui_window_manager->GetHotKeyType(), sizeof(int)*C_VS_UI_OUSTERS_QUICKITEM::HOTKEY_MAX);

#ifndef _LIB
	SetTime("12:34:05");
	SetDate("2001/06/30");
#endif
	CT_Done("DIAG: C_VS_UI_OUSTERS ctor exit");
}

//-----------------------------------------------------------------------------
// ~C_VS_UI_OUSTERS
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_OUSTERS::~C_VS_UI_OUSTERS()
{
	Phoenix_ReleaseOustersExpTextCache();

	gpC_vs_ui_window_manager->SetHotkeyType((int *)m_HotKey_Type);
	DeleteNew(m_pC_gear);
	DeleteNew(m_pC_gear_other);
	DeleteNew(m_pC_armsband);
}

//-----------------------------------------------------------------------------
// C_VS_UI_OUSTERS::GetGearWindow
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_GEAR * C_VS_UI_OUSTERS::GetGearWindow() const
{
	return m_pC_gear;
}

//-----------------------------------------------------------------------------
// C_VS_UI_OUSTERS::CloseGear
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_OUSTERS::CloseGear()
{
	m_pC_gear->Finish();
}

//-----------------------------------------------------------------------------
// C_VS_UI_OUSTERS::OpenGear
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_OUSTERS::OpenGear(bool bl_set_load)
{
	m_pC_gear->Start(bl_set_load);
}



//Add By Hays

C_VS_UI_GEAR * C_VS_UI_OUSTERS::GetGearOtherWindow() const
{
	return m_pC_gear_other;
}

void	C_VS_UI_OUSTERS::CloseGearOther()
{
	m_pC_gear_other->Finish();
}

void	C_VS_UI_OUSTERS::OpenGearOther(bool bl_set_load)
{
	m_pC_gear_other->Start(bl_set_load);
}



#if __CONTENTS(__GEAR_SWAP_CHANGE)	// C_VS_UI_OUSTERS::RunGearChange()
void	C_VS_UI_OUSTERS::RunGearChange()
{
	m_pC_gear->GearChange();
}
#endif // __GEAR_SWAP_CHANGE

//-----------------------------------------------------------------------------
// DoCommonActionBeforeEventOccured
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS::DoCommonActionBeforeEventOccured()
{
	C_VS_UI_TRIBE::DoCommonActionBeforeEventOccured();
}

//-----------------------------------------------------------------------------
// DoCommonActionAfterEventOccured
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS::DoCommonActionAfterEventOccured()
{
	C_VS_UI_TRIBE::DoCommonActionAfterEventOccured();
}

//-----------------------------------------------------------------------------
// Start
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS::Start()
{
	memcpy(m_HotKey_Type, gpC_vs_ui_window_manager->GetHotKeyType(), sizeof(int)*C_VS_UI_OUSTERS_QUICKITEM::HOTKEY_MAX);

	C_VS_UI_TRIBE::Start();

	m_pC_skill->Start();
	m_pC_hpbar->Start();
	m_pC_BloodBurst->Start();
//	m_pC_Market->Start();
	m_pC_ShortCutSlot->Start();

	m_pC_effect_status->Start();
	m_pC_minimap->Start();
	m_pC_armsband->Start();

	WindowEventReceiver(EVENT_WINDOW_MOVE);


}

//-----------------------------------------------------------------------------
// Show
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS::ShowExp()
{

	char sz_temp[100];

	Rect rect;
	int imgX = (m_pC_main_spk->GetWidth(SIMPLEINFOMATION_MAIN)-m_pC_main_spk->GetWidth(EXP_BAR_BLUE))/2;

	int firenum =   g_char_slot_ingame.ElementalFire;
	int waternum =   g_char_slot_ingame.ElementalWater;
	int earthnum =   g_char_slot_ingame.ElementalEarth;

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
			goal_exp = g_pExperienceTable->GetOustersInfo( g_char_slot_ingame.level ).GoalExp;
		}
		int exp_width = m_pC_main_spk->GetWidth(EXP_BAR_BLUE);
		int exp_height = m_pC_main_spk->GetHeight(EXP_BAR_BLUE);
		int exp_bar = exp_width * (goal_exp - exp_remain) / goal_exp;

		rect.Set(0, 0, exp_bar, exp_height);


		m_pC_main_spk->BltLocked(x+50, y+130, EXP_BAR_BACKGROUD);
		m_pC_main_spk->BltLockedClip(x+51, y+131, rect, EXP_BAR_BLUE);

		m_pC_main_spk->BltLocked(x+55,y+86,SIMPLEINFOMATION_FIRE);
		m_pC_main_spk->BltLocked(x+55+15,y+86+5,SIMPLEINFOMATION_OUSTER_BAR);
		m_pC_main_spk->BltLocked(x+55+16,y+86+6,SIMPLEINFOMATION_FIRE_BAR);

		m_pC_main_spk->BltLocked(x+55,y+86+15,SIMPLEINFOMATION_WATER);
		m_pC_main_spk->BltLocked(x+55+15,y+86+15+4,SIMPLEINFOMATION_OUSTER_BAR);
		m_pC_main_spk->BltLocked(x+55+16,y+86+15+5,SIMPLEINFOMATION_WATER_BAR);

		m_pC_main_spk->BltLocked(x+55-2,y+86+15+13,SIMPLEINFOMATION_EARTH);
		m_pC_main_spk->BltLocked(x+55+15,y+86+15+13+4,SIMPLEINFOMATION_OUSTER_BAR);
		m_pC_main_spk->BltLocked(x+55+16,y+86+15+5+13,SIMPLEINFOMATION_EARTH_BAR);


		gpC_base->m_p_DDSurface_back->Unlock();
	}
		

	const int oustersTextCacheLeft = x;
	const int oustersTextCacheTop = y + 68;
	const int oustersTextCacheWidth = 245;
	const int oustersTextCacheHeight = 80;
	bool oustersTextCacheBlt = false;

	if(Phoenix_EnsureOustersExpTextCacheSurface(oustersTextCacheWidth, oustersTextCacheHeight))
	{
		const DWORD textSignature = Phoenix_BuildOustersExpTextSignature(firenum, waternum, earthnum, gpC_base->m_chatting_pi.hfont);
		if(Phoenix_NeedsOustersExpTextCacheBuild(oustersTextCacheLeft, oustersTextCacheTop, textSignature, gpC_base->m_chatting_pi.hfont))
		{
			HDC cacheDC = Phoenix_BeginOustersExpTextCacheBuild();
			if(cacheDC != NULL)
			{
				Phoenix_DrawOustersExpTextLayerCached(cacheDC, oustersTextCacheLeft, oustersTextCacheTop, x, y, firenum, waternum, earthnum);
				Phoenix_CommitOustersExpTextCacheBuild(oustersTextCacheLeft, oustersTextCacheTop, textSignature, gpC_base->m_chatting_pi.hfont);
			}
		}

		oustersTextCacheBlt = Phoenix_BltOustersExpTextCache();
	}

	if(!oustersTextCacheBlt)
	{
		g_FL2_GetDC();

		int maxX = (245-55*3)/4;
		wsprintf(sz_temp,"STR");
		int max2 = g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont);
		g_PrintColorStrShadow(x+maxX, y+70, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
		wsprintf(sz_temp, "%d", g_char_slot_ingame.STR_CUR);
		int max3 = g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont);
		g_PrintColorStrShadow(x+maxX+max2+(55-max2-max3-1)/2, y+70, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);

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

		wsprintf(sz_temp, "%d", firenum);
		g_PrintColorStrShadow(x+55+15+125+6,y+86+5+1-3, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);
		wsprintf(sz_temp, "%d", waternum);
		g_PrintColorStrShadow(x+55+15+125+6,y+86+15+5-3, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);
		wsprintf(sz_temp, "%d", earthnum);
		g_PrintColorStrShadow(x+55+15+125+6,y+86+15+13+4+1-3, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);

		wsprintf(sz_temp,""); // Exp bar string EXP.Point
		g_PrintColorStrShadow(x+maxX-10, y+129, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);

		g_FL2_ReleaseDC();
	}




	SHOW_WINDOW_ATTR;

}

//-----------------------------------------------------------------------------
// Process
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS::Process()
{
	C_VS_UI_TRIBE::Process();
}

//-----------------------------------------------------------------------------
// KeyboardControl
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS::KeyboardControl(UINT message, UINT key, long extra)
{
	m_pC_chatting->KeyboardControl(message, key, extra);
}

void C_VS_UI_OUSTERS::HotKey_F1()
{
	Hotkey_Function(0) ;
	/*assert(m_pC_armsband != NULL);
	
	// ��ų , �Ͻ���� ó�� 

	int	current_mouse_x = gpC_window_manager->GetCurrentMouse_x() ;
	int	current_mouse_y = gpC_window_manager->GetCurrentMouse_y() ;

	C_VS_UI_SKILL * p_skill = GetSkillWindow()		;
	bool	bSkillFocus = p_skill->IsPixel2(current_mouse_x , current_mouse_y) ; 
	bool	bArmsBandFocus =  m_pC_armsband->IsPixel(current_mouse_x , current_mouse_y) ; 

	if(bSkillFocus)
	{
		int skillslot = p_skill->GetFocusedSlot()		;
		int skillid   = p_skill->GetSkillID(skillslot)	;
		if (skillid != NOT_SELECTED)
		{
			p_skill->SetHotkey(C_VS_UI_SKILL::HOTKEY1, (ACTIONINFO)skillid)	;
			m_pC_armsband->InitHotKey(C_VS_UI_OUSTERS_QUICKITEM::HOTKEY1) ; 
			//m_Ousters_HotKey_Type[C_VS_UI_SKILL::HOTKEY1] = SKILL_TYPE ;
		}
		else 
		{
			p_skill->ChangeSkillByHotkey(C_VS_UI_SKILL::HOTKEY1) ;
		}
	}	
	else if(bArmsBandFocus)
	{
		int ArmsBandSlot = m_pC_armsband->GetFocusedSlot();
		if(ArmsBandSlot >= 0)
		{
			m_pC_armsband->SetHotkey(C_VS_UI_OUSTERS_QUICKITEM::HOTKEY1 , ArmsBandSlot);
			p_skill->InitHotKey( C_VS_UI_SKILL::HOTKEY1) ;
			m_Ousters_HotKey_Type[C_VS_UI_SKILL::HOTKEY1] = QUICKITEM_TYPE ;
		}
	}
	else if(!bSkillFocus && !bArmsBandFocus)
	{
		if(m_Ousters_HotKey_Type[C_VS_UI_SKILL::HOTKEY1] == SKILL_TYPE) 
			p_skill->ChangeSkillByHotkey(C_VS_UI_SKILL::HOTKEY1) ; 
		else
		{
			int Armsbandslot = m_pC_armsband->GetSlot(C_VS_UI_OUSTERS_QUICKITEM::HOTKEY1) ;//
			m_pC_armsband->Use(Armsbandslot);
		}
	}*/
}

void C_VS_UI_OUSTERS::HotKey_F2()
{
	Hotkey_Function(1) ;

//	assert(m_pC_armsband != NULL);
//	int slot = m_pC_armsband->GetFocusedSlot();
//	
//	m_pC_armsband->Use(C_VS_UI_OUSTERS_QUICKITEM::SLOT_2);
}

void C_VS_UI_OUSTERS::HotKey_F3()
{
	Hotkey_Function(2) ;
	//assert(m_pC_armsband != NULL);
	//int slot = m_pC_armsband->GetFocusedSlot();

	//m_pC_armsband->Use(C_VS_UI_OUSTERS_QUICKITEM::SLOT_3);
}

void C_VS_UI_OUSTERS::HotKey_F4()
{
	Hotkey_Function(3) ;
	//assert(m_pC_armsband != NULL);
	//int slot = m_pC_armsband->GetFocusedSlot();

	//m_pC_armsband->Use(C_VS_UI_OUSTERS_QUICKITEM::SLOT_4);
}

void C_VS_UI_OUSTERS::HotKey_F5()
{
	assert(m_pC_armsband != NULL);
	int slot = m_pC_armsband->GetFocusedSlot();

	m_pC_armsband->Use(C_VS_UI_OUSTERS_QUICKITEM::SLOT_5);
}

void C_VS_UI_OUSTERS::HotKey_F6()
{
	assert(m_pC_armsband != NULL);
	int slot = m_pC_armsband->GetFocusedSlot();

	m_pC_armsband->Use(C_VS_UI_OUSTERS_QUICKITEM::SLOT_6);
}

void C_VS_UI_OUSTERS::HotKey_F7()
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

void C_VS_UI_OUSTERS::HotKey_F8()
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
// C_VS_UI_OUSTERS_GEAR::C_VS_UI_OUSTERS_GEAR
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_OUSTERS_GEAR::C_VS_UI_OUSTERS_GEAR()
{
	assert(MOustersGear::MAX_GEAR_OUSTERS == SLOT_SIZE);
  
//	if(g_char_slot_ingame.m_AdvancementLevel > 0)// 2������ bycsm 2004.12.31 
//	{
	m_slot_rect[SN_CIRCLET].Set(146, 46, 66, 66);			// ��Ŭ��
	m_slot_rect[SN_COAT].Set(146, 46+116, 66, 96);				// ��
	m_slot_rect[SN_LEFTHAND].Set(146-56-60+43+160, 46+34+139, 66, 96);			// �޼�
	m_slot_rect[SN_RIGHTHAND].Set(146-56-60+43, 46+34+139, 66, 96);			// ������
	m_slot_rect[SN_BOOTS].Set(146, 46+116+104, 66, 96);				// �Ź�
	m_slot_rect[SN_ARMSBAND1].Set(146-56-60+43, 46+34+139+105, 66, 66);			// �Ͻ����1
	m_slot_rect[SN_ARMSBAND2].Set(146-56-60+43+160, 46+34+139+105, 66, 66);			// �Ͻ����2
	m_slot_rect[SN_RING1].Set(146-56-60+43-9, 46+34+139+220-40, 36, 36);				// ��1
	m_slot_rect[SN_RING2].Set(146-56-60+43+160+36, 46+34+139+220-40, 36, 36);				// ��2
	m_slot_rect[SN_PENDENT1].Set(146+12-36,46+116-43,36, 36);			// �����1
	m_slot_rect[SN_PENDENT2].Set(146+12,46+116-43,36, 36);			// �����2
	m_slot_rect[SN_PENDENT3].Set(146+12+36,46+116-43,36, 36);			// �����3
	m_slot_rect[SN_STONE1].Set(146-56-60+43-9+36, 46+34+139+220-40, 36, 36);			// ���ɼ�1
	m_slot_rect[SN_STONE2].Set(146-56-60+43+160, 46+34+139+220-40, 36, 36);			// ���ɼ�2
	m_slot_rect[SN_STONE3].Set(146-56-60+43-9+36, 46+34+139+220, 36, 36);			// ���ɼ�3
	m_slot_rect[SN_STONE4].Set(146-56-60+43+160, 46+34+139+220, 36, 36);			// ���ɼ�4

	m_slot_rect[SN_COREZAP1].Set(146-56-60+43-9+36, 46+34+139+220-40, 36, 36);			// ���ɼ�1
	m_slot_rect[SN_COREZAP2].Set(146-56-60+43+160, 46+34+139+220-40, 36, 36);			// ���ɼ�2
	m_slot_rect[SN_COREZAP3].Set(146-56-60+43-9+36, 46+34+139+220, 36, 36);			// ���ɼ�3
	m_slot_rect[SN_COREZAP4].Set(146-56-60+43+160, 46+34+139+220, 36, 36);			// ���ɼ�4

	m_slot_rect[SN_FASCIA].Set(146-56-60, 46+34, 66, 66);	
	m_slot_rect[SN_MITTEN].Set(146+56+60, 46+34, 66, 66);
	// nanomech 2006.03.09 Item ���� �۾�
	m_slot_rect[SN_NECK_CHAIN].Set(146+12,46+116-43,36, 36);		// ���׸��� ü��

	m_slot_rect[SN_CUEOFADAM1].Set(146-56-60+43-9, 46+34+139+220-40, 36, 36);				// ��1
	m_slot_rect[SN_CUEOFADAM2].Set(146-56-60+43+160+36, 46+34+139+220-40, 36, 36);				// ��2

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
//
//		m_slot_rect[SN_CIRCLET].Set(114, 25, 60, 60);			// ��Ŭ��
//		m_slot_rect[SN_COAT].Set(114, 127, 60, 90);				// ��
//		m_slot_rect[SN_LEFTHAND].Set(201, 127, 60, 90);			// �޼�
//		m_slot_rect[SN_RIGHTHAND].Set(27, 127, 60, 90);			// ������
//		m_slot_rect[SN_BOOTS].Set(114, 223, 60, 90);				// �Ź�
//		m_slot_rect[SN_ARMSBAND1].Set(27, 61, 60, 60);			// �Ͻ����1
//		m_slot_rect[SN_ARMSBAND2].Set(201, 61, 60, 60);			// �Ͻ����2
//		m_slot_rect[SN_RING1].Set(42, 247, 30, 30);				// ��1
//		m_slot_rect[SN_RING2].Set(218, 247, 30, 30);				// ��2
//		m_slot_rect[SN_PENDENT1].Set(93, 91, 30, 30);			// �����1
//		m_slot_rect[SN_PENDENT2].Set(129, 91, 30, 30);			// �����2
//		m_slot_rect[SN_PENDENT3].Set(165, 91, 30, 30);			// �����3
//		m_slot_rect[SN_STONE1].Set(77, 246, 30, 30);			// ���ɼ�1
//		m_slot_rect[SN_STONE2].Set(181, 246, 30, 30);			// ���ɼ�2
//		m_slot_rect[SN_STONE3].Set(77, 282, 30, 30);			// ���ɼ�3
//		m_slot_rect[SN_STONE4].Set(181, 282, 30, 30);			// ���ɼ�4
//		
//		m_slot_rect[SN_COREZAP1].Set(77, 246, 30, 30);			// �ھ���1
//		m_slot_rect[SN_COREZAP2].Set(181, 246, 30, 30);			// �ھ���2
//		m_slot_rect[SN_COREZAP3].Set(77, 282, 30, 30);			// �ھ���3
//		m_slot_rect[SN_COREZAP4].Set(181, 282, 30, 30);			// �ھ���4
//		
//		m_slot_rect[SN_FASCIA].Set(26, 37, 60, 60);	
//		m_slot_rect[SN_MITTEN].Set(203, 37, 60, 60);
//
//
//		m_slot_rect[SN_BLOODBIBLE1].Set(37, 317, 30, 30);
//		m_slot_rect[SN_BLOODBIBLE2].Set(73, 317, 30, 30);
//		m_slot_rect[SN_BLOODBIBLE3].Set(109, 317, 30, 30);
//		m_slot_rect[SN_BLOODBIBLE4].Set(145, 317, 30, 30);
//		m_slot_rect[SN_BLOODBIBLE5].Set(181, 317, 30, 30);
//		m_slot_rect[SN_BLOODBIBLE6].Set(217, 317, 30, 30);
//		m_slot_size = SLOT_SIZE;
//				
//	}
	
	
	m_p_slot_rect = m_slot_rect;
	m_p_slot_image = m_slot_image;
}
//-----------------------------------------------------------------------------
// C_VS_UI_OUSTERS_GEAR::~C_VS_UI_OUSTERS_GEAR
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_OUSTERS_GEAR::~C_VS_UI_OUSTERS_GEAR()
{
//	DeleteNew(m_pC_gear);
}


/*-----------------------------------------------------------------------------
- C_VS_UI_OUSTERS_QUICKITEM
-
-----------------------------------------------------------------------------*/
C_VS_UI_OUSTERS_QUICKITEM::C_VS_UI_OUSTERS_QUICKITEM()
{
	g_RegisterWindow(this);

	AttrPin(true);

	m_image_spk.Open(SPK_OUSTERS_QUICKITEM);

	// ���̴� ó�� no belt�� ����̴�. belt�� �����ϸ� �ٲ��.
	// ���̴� �����ϴ�.
	
	Set(0, 434 + g_pUserInformation->iResolution_y - 600, m_image_spk.GetWidth(END_EDGE), m_image_spk.GetHeight(END_EDGE));
//#ifdef CONVERT_1024_768
//	Set(0, 434+168, m_image_spk.GetWidth(END_EDGE), m_image_spk.GetHeight(END_EDGE));
//#else
//	Set(0, 434, m_image_spk.GetWidth(END_EDGE), m_image_spk.GetHeight(END_EDGE));
//#endif
	

	m_pC_button_group = new ButtonGroup(this);

	int alpha_button_offset_x = 14, alpha_button_offset_y = 7;
	int pushpin_button_offset_x = 14, pushpin_button_offset_y = 23;
	int rotate_button_offset_x = 5, rotate_button_offset_y = 8;
	#if __CONTENTS(__080405_FIREST_UI_UPDATE)
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(alpha_button_offset_x, alpha_button_offset_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_OLD), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_OLD), ALPHA_ID, this, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_OLD));
	#else
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(alpha_button_offset_x, alpha_button_offset_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA), ALPHA_ID, this, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA));
	#endif //__080405_FIREST_UI_UPDATE
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(pushpin_button_offset_x, pushpin_button_offset_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), PUSHPIN_ID, this, C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN));
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(rotate_button_offset_x, rotate_button_offset_y, m_image_spk.GetWidth(BUTTON_ROTATE_HILIGHTED), m_image_spk.GetHeight(BUTTON_ROTATE_HILIGHTED), ROTATE_ID, this, BUTTON_ROTATE_HILIGHTED));

	m_bl_width = true;


	for (int i=0; i < HOTKEY_MAX; i++)
	{
		m_Hotkey_buf[i] = NOT_SELECTED;
	}
}

/*-----------------------------------------------------------------------------
- ~C_VS_UI_OUSTERS_QUICKITEM
-
-----------------------------------------------------------------------------*/
C_VS_UI_OUSTERS_QUICKITEM::~C_VS_UI_OUSTERS_QUICKITEM()
{
	gpC_vs_ui_window_manager->SetAlpha(C_VS_UI_WINDOW_MANAGER::QUICKITEMSLOT, GetAttributes()->alpha);
	gpC_vs_ui_window_manager->SetAutoHide(C_VS_UI_WINDOW_MANAGER::QUICKITEMSLOT, GetAttributes()->autohide);
	gpC_vs_ui_window_manager->SetRect(C_VS_UI_WINDOW_MANAGER::QUICKITEMSLOT, Rect(x, y, w, h));
	gpC_vs_ui_window_manager->SetQuickItemSlotHeight(!m_bl_width);

	gpC_vs_ui_window_manager->SetQuickSlotHotkey((int *)m_Hotkey_buf);
	g_UnregisterWindow(this);
	DeleteNew(m_pC_button_group);

}

//-----------------------------------------------------------------------------
// C_VS_UI_OUSTERS_QUICKITEM::ToggleBelt
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_OUSTERS_QUICKITEM::ToggleBelt()
{
//	m_bl_opened = !m_bl_opened;

	// reset Window width by open/close
	Run(PUSHPIN_ID);
	ResetSize();

	PlaySound(SOUND_OUSTERS_ARMSBAND);
}

void	C_VS_UI_OUSTERS_QUICKITEM::Open()
{
//	m_bl_opened = true;

	AttrAutoHide(Window::ATTRIBUTES_HIDE_NOT);
	// reset Window width by open/close
	ResetSize();

	PlaySound(SOUND_OUSTERS_ARMSBAND);
}

void	C_VS_UI_OUSTERS_QUICKITEM::Close()
{
//	m_bl_opened = false;
	AttrAutoHide(Window::ATTRIBUTES_HIDE_NOT);
	
	Run(PUSHPIN_ID);

	// reset Window width by open/close
	ResetSize();

	PlaySound(SOUND_OUSTERS_ARMSBAND);
}

//-----------------------------------------------------------------------------
// ResetSize
//
// belt�� ��ü�ϰ����� size�� �缳���ؾ� �Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS_QUICKITEM::ResetSize()
{
	// ���ƾƾ� �ϵ��ڵ�!!
	int slot_size = m_image_spk.GetWidth(SLOT);
	if(m_bl_width)
	{
		h = m_image_spk.GetHeight(START_EDGE);
		w = m_image_spk.GetWidth(START_EDGE_EXTEND);
		w += m_image_spk.GetWidth(END_EDGE);
		w += m_image_spk.GetWidth(SLOT)*GetPocketCount();
		if(x <= 0)
		{
			w -= m_image_spk.GetWidth(START_EDGE) - m_image_spk.GetWidth(STATCH_EDGE);

			m_p_slot_x[SLOT_1] = m_image_spk.GetWidth(STATCH_EDGE)+3;
		}
		//else if(x+w >= RESOLUTION_X)
		else if(x+w >= g_pUserInformation->iResolution_x)
		{
			m_bl_resize = true;
			m_p_slot_x[SLOT_1] = m_image_spk.GetWidth(START_EDGE_EXTEND)+3;
		}
		else
		{
			m_p_slot_x[SLOT_1] = m_image_spk.GetWidth(START_EDGE)+3;
		}
	}
	else
	{
		w = m_image_spk.GetWidth(START_EDGE+ROTATED_OFFSET);
		h = m_image_spk.GetHeight(START_EDGE_EXTEND+ROTATED_OFFSET);
		h += m_image_spk.GetHeight(END_EDGE+ROTATED_OFFSET);
		h += m_image_spk.GetHeight(SLOT+ROTATED_OFFSET)*GetPocketCount();
		if(y <= 0)
		{
			h -= m_image_spk.GetHeight(START_EDGE+ROTATED_OFFSET) - m_image_spk.GetHeight(STATCH_EDGE+ROTATED_OFFSET);
			m_p_slot_x[SLOT_1] = m_image_spk.GetWidth(STATCH_EDGE)+3;
		}
		//else if(y+h >= RESOLUTION_Y)
		else if(y+h >= g_pUserInformation->iResolution_y)
		{
			m_bl_resize = true;
			m_p_slot_x[SLOT_1] = m_image_spk.GetWidth(START_EDGE_EXTEND)+3;
		}
		else
			m_p_slot_x[SLOT_1] = m_image_spk.GetWidth(START_EDGE_EXTEND)+3;

	}
	m_p_slot_x[SLOT_2] = m_p_slot_x[SLOT_1]+slot_size;
	m_p_slot_x[SLOT_3] = m_p_slot_x[SLOT_2]+slot_size;
	m_p_slot_x[SLOT_4] = m_p_slot_x[SLOT_3]+slot_size;
	m_p_slot_x[SLOT_5] = m_p_slot_x[SLOT_4]+slot_size;
	m_p_slot_x[SLOT_6] = m_p_slot_x[SLOT_5]+slot_size;
}

//-----------------------------------------------------------------------------
// GetPocketCount
//
// 
//-----------------------------------------------------------------------------
int C_VS_UI_OUSTERS_QUICKITEM::GetPocketCount() const
{
	int PocketSize = 0;
	
	if(g_pArmsBand1 != NULL)
		PocketSize += g_pArmsBand1->GetPocketNumber();

	if(g_pArmsBand2 != NULL)
		PocketSize += g_pArmsBand2->GetPocketNumber();

	return PocketSize;
}

//-----------------------------------------------------------------------------
// WindowEventReceiver
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS_QUICKITEM::WindowEventReceiver(id_t event)
{
	switch(event)
	{
	case EVENT_WINDOW_MOVE: 
		{
			ResetSize();
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// IsPixel
//
// 
//-----------------------------------------------------------------------------
bool C_VS_UI_OUSTERS_QUICKITEM::IsPixel(int _x, int _y)
{
	return IsInRect(_x, _y);
}

//-----------------------------------------------------------------------------
// Start
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS_QUICKITEM::Start()
{
	AttrAlpha(gpC_vs_ui_window_manager->IsAlpha(C_VS_UI_WINDOW_MANAGER::QUICKITEMSLOT));
	AttrAutoHide(gpC_vs_ui_window_manager->GetAutoHide(C_VS_UI_WINDOW_MANAGER::QUICKITEMSLOT));
	if(m_bl_width == gpC_vs_ui_window_manager->IsQuickItemSlotHeight())
		Run(ROTATE_ID);
	Rect &rect = gpC_vs_ui_window_manager->GetRect(C_VS_UI_WINDOW_MANAGER::QUICKITEMSLOT);
	if(rect.w != -1)
	{
		x = rect.x;
		y = rect.y;
	}

	PI_Processor::Start();
	memcpy(m_Hotkey_buf, gpC_vs_ui_window_manager->GetHotKeyQuickSlot(), sizeof(int)*C_VS_UI_OUSTERS_QUICKITEM::HOTKEY_MAX);

	gpC_window_manager->AppearWindow(this);

	m_focus_slot = NOT_SELECTED;
	m_bl_resize = false;
	ResetSize();
}

//-----------------------------------------------------------------------------
// Process
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS_QUICKITEM::Process()
{
	m_pC_button_group->Process();
	ProcessHide();
}

//-----------------------------------------------------------------------------
// KeyboardControl
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS_QUICKITEM::KeyboardControl(UINT message, UINT key, long extra)
{

}

//-----------------------------------------------------------------------------
// UnacquireMouseFocus
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS_QUICKITEM::UnacquireMouseFocus()
{
	m_pC_button_group->UnacquireMouseFocus();
	m_focus_slot = NOT_SELECTED;
}

/*-----------------------------------------------------------------------------
- MouseControl
-
-----------------------------------------------------------------------------*/
bool C_VS_UI_OUSTERS_QUICKITEM::MouseControl(UINT message, int _x, int _y)
{
	Window::MouseControl(message, _x, _y);
	_x-=x; _y-=y;

	// test open/close button
	bool ret = false;
	if(m_bl_width)
	{
		//if(x+w >= RESOLUTION_X)
		if(x+w >= g_pUserInformation->iResolution_x)
			ret = !m_pC_button_group->MouseControl(message, _x, _y);
		else
			ret = !m_pC_button_group->MouseControl(message, w-_x, _y);
	}
	else
	{
		if(y <= 0)
			ret = !m_pC_button_group->MouseControl(message, h-_y, _x);
		else
			ret = !m_pC_button_group->MouseControl(message, _y, _x);
	}

	if (g_pArmsBand1 != NULL || g_pArmsBand2 != NULL)
	{
		int i;

		const MItem * p_selected_item;

		switch (message)
		{
		case M_MOVING:
			for (int i=0; i < GetPocketCount(); i++)
			{
				int m;
				int loop;
				int px, py;
				
				if (gpC_mouse_pointer->GetPickUpItem())
					loop = ITEM_REF_POINT_COUNT;
				else
					loop = 1;
				
				for (m=0; m < loop; m++)
				{
					if(m_bl_width)
					{
						if (loop == ITEM_REF_POINT_COUNT)
						{
							px = g_item_ref_point[m].x+gpC_mouse_pointer->GetPointerX();
							py = g_item_ref_point[m].y+gpC_mouse_pointer->GetPointerY()-8;
						}
						else
						{
							px = _x+x;
							py = _y+y-8;
						}
					}
					else
					{
						if (loop == ITEM_REF_POINT_COUNT)
						{
							px = g_item_ref_point[m].y+gpC_mouse_pointer->GetPointerX()-8;
							py = g_item_ref_point[m].x+gpC_mouse_pointer->GetPointerY();
						}
						else
						{
							px = _x+x-8;
							py = _y+y;
						}
					}
					
					if (m_bl_width &&
						px >= x+m_p_slot_x[i] && px < x+m_p_slot_x[i]+QUICKSLOT_W &&
						py >= y && py < y+QUICKSLOT_H ||
						!m_bl_width &&
						px >= x && px < x+QUICKSLOT_W &&
						py >= y+m_p_slot_x[i] && py < y+m_p_slot_x[i]+QUICKSLOT_H
						)
					{
						if(m_focus_slot != i)
						{
							gpC_Imm->ForceUI(CImm::FORCE_UI_GRID);
							m_focus_slot = i;
						}

						if( g_pArmsBand1 != NULL && m_focus_slot < g_pArmsBand1->GetPocketNumber() )			// armsband 1���� üũ�Ѵ�.	
							p_selected_item = g_pArmsBand1->GetItem(m_focus_slot);
						else
						{
							if( g_pArmsBand1 != NULL)
								p_selected_item = g_pArmsBand2->GetItem(m_focus_slot- g_pArmsBand1->GetPocketNumber() );
							else
								p_selected_item = g_pArmsBand2->GetItem(m_focus_slot);								
						}

						if (p_selected_item != NULL)
						{
							if(m_bl_width)
								g_descriptor_manager.Set(DID_ITEM, x+m_p_slot_x[m_focus_slot], y, (void *)p_selected_item);
							else
								g_descriptor_manager.Set(DID_ITEM, x, y+m_p_slot_x[m_focus_slot], (void *)p_selected_item);
						}
						
						return true;
					}
				}
			}
			m_focus_slot = NOT_SELECTED;
			break;

			case M_LEFTBUTTON_DOWN:
			case M_LB_DOUBLECLICK:
				//
				// Item�� ������ ���´�.
				//
				{
					ret |= Click();

					if (ret)
						EMPTY_MOVE;
				}
				break;

			case M_RIGHTBUTTON_DOWN:
				//
				// Item�� ����Ѵ�.
				//
				Use(m_focus_slot);
				break;

			case M_RIGHTBUTTON_UP:
				break;
		}
	}
	switch (message)
	{
		case M_MOVING:
			break;

		case M_LEFTBUTTON_DOWN:
		case M_LB_DOUBLECLICK:
			if (gpC_mouse_pointer->GetPickUpItem() == NULL && ret == false)
			{
				MoveReady();
				SetOrigin(_x, _y);
				break;
			}
			break;

		case M_LEFTBUTTON_UP:
			if(m_bl_resize)
			{
				m_bl_resize = false;
				if(m_bl_width)
				{
					//if(x+w >= RESOLUTION_X)
					if(x+w >= g_pUserInformation->iResolution_x)
					{
						//if(x+w == RESOLUTION_X)
						if(x+w == g_pUserInformation->iResolution_x)
							x += (m_image_spk.GetWidth(START_EDGE) - m_image_spk.GetWidth(STATCH_EDGE));
						w -= (m_image_spk.GetWidth(START_EDGE) - m_image_spk.GetWidth(STATCH_EDGE));
					}
				}
				else
				{
					//if(y+h >= RESOLUTION_Y)
					if(y+h >= g_pUserInformation->iResolution_y)
					{
						//if(y+h == RESOLUTION_Y)
						if(y+h == g_pUserInformation->iResolution_y)
							y += (m_image_spk.GetHeight(START_EDGE+ROTATED_OFFSET) - m_image_spk.GetHeight(STATCH_EDGE+ROTATED_OFFSET));
						h -= (m_image_spk.GetHeight(START_EDGE+ROTATED_OFFSET) - m_image_spk.GetHeight(STATCH_EDGE+ROTATED_OFFSET));
					}
				}
			}
			break;
	}

	return true;
}

void C_VS_UI_OUSTERS_QUICKITEM::CancelPushState()
{
	m_pC_button_group->CancelPushState();
}

void C_VS_UI_OUSTERS_QUICKITEM::Run(id_t id)
{
	switch (id)
	{
	case ALPHA_ID:
		AttrAlpha(!GetAttributes()->alpha);
		EMPTY_MOVE;
		break;

	case PUSHPIN_ID:
		if(GetAttributes()->autohide)
			AttrAutoHide(ATTRIBUTES_HIDE_NOT);
		else
		{
			if(m_bl_width)
				AttrAutoHide(ATTRIBUTES_HIDE_HEIGHT);
			else
				AttrAutoHide(ATTRIBUTES_HIDE_WIDTH);
		}
		EMPTY_MOVE;
		break;

	case ROTATE_ID:
		if(m_bl_width)
		{
			if(GetAttributes()->autohide != ATTRIBUTES_HIDE_NOT)
				AttrAutoHide(ATTRIBUTES_HIDE_WIDTH);
			x += w-h;
		}
		else
		{
			if(GetAttributes()->autohide != ATTRIBUTES_HIDE_NOT)
				AttrAutoHide(ATTRIBUTES_HIDE_HEIGHT);
			x -= h-w;
		}
		m_bl_width = !m_bl_width;
		ResetSize();
		EMPTY_MOVE;
		break;
	}
}

void	C_VS_UI_OUSTERS_QUICKITEM::ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button)
{
	static char* m_quickitem_button_string[6] = 
	{
		(*g_pGameStringTable)[UI_STRING_MESSAGE_SHOW_ALPHA_WINDOW].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_AUTO_HIDE_ON].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_SHOW_WIDTH].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_SHOW_NO_ALPHA_WINDOW].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_AUTO_HIDE_OFF].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_SHOW_HEIGHT].GetString(),
	};
	
	if(GetAttributes()->alpha && p_button->GetID() == ALPHA_ID)
		g_descriptor_manager.Set(DID_INFO, p_button->x+x, p_button->y+y, (void *)m_quickitem_button_string[3],0,0);
	if(GetAttributes()->autohide && p_button->GetID() == PUSHPIN_ID)
		g_descriptor_manager.Set(DID_INFO, p_button->x+x, p_button->y+y, (void *)m_quickitem_button_string[4],0,0);
	if(m_bl_width && p_button->GetID() == ROTATE_ID)
		g_descriptor_manager.Set(DID_INFO, p_button->x+x, p_button->y+y, (void *)m_quickitem_button_string[5],0,0);
	else
		g_descriptor_manager.Set(DID_INFO, p_button->x+x, p_button->y+y, (void *)m_quickitem_button_string[p_button->GetID()],0,0);
}

void	C_VS_UI_OUSTERS_QUICKITEM::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{	
	// ���ο�
	if(m_bl_width)
	{
		//if(x+w >= RESOLUTION_X)
		if(x+w >= g_pUserInformation->iResolution_x)	
		{
			if(p_button->GetID() == ALPHA_ID)
			{
			#if __CONTENTS(__080405_FIREST_UI_UPDATE)
				if(GetAttributes()->alpha)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->x+x, p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_OLD);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->x+x, p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_PUSHED_OLD);
			#else
				if(GetAttributes()->alpha)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->x+x, p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->x+x, p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_PUSHED);	
			#endif //__080405_FIREST_UI_UPDATE
			}
			else if(p_button->GetID() == PUSHPIN_ID)
			{
				if(GetAttributes()->autohide)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->x+x, p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->x+x, p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN_PUSHED);
			}
			else
			{
				if(p_button->GetFocusState())
				{
					if(p_button->GetPressState())
						m_image_spk.BltLocked(p_button->x+x-1, p_button->y+y, BUTTON_ROTATE_HILIGHTED_PUSHED);
					else
						m_image_spk.BltLocked(p_button->x+x-1, p_button->y+y, BUTTON_ROTATE_HILIGHTED);
				}
			}
		}
		else
		{
			if(p_button->GetID() == ALPHA_ID)
			{
			#if __CONTENTS(__080405_FIREST_UI_UPDATE)
				if(GetAttributes()->alpha)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+w-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_OLD), p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_OLD);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+w-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_OLD), p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_PUSHED_OLD);
			#else
				if(GetAttributes()->alpha)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+w-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA), p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+w-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA), p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_PUSHED);	
			#endif //__080405_FIREST_UI_UPDATE
			}
			else if(p_button->GetID() == PUSHPIN_ID)
			{
				if(GetAttributes()->autohide)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+w-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+w-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), p_button->y+y, C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN_PUSHED);
			}
			else
			{
				if(p_button->GetFocusState())
				{
					if(p_button->GetPressState())
						m_image_spk.BltLocked(x+w-p_button->x-m_image_spk.GetWidth(BUTTON_ROTATE_HILIGHTED), p_button->y+y, BUTTON_ROTATE_HILIGHTED_PUSHED);
					else
						m_image_spk.BltLocked(x+w-p_button->x-m_image_spk.GetWidth(BUTTON_ROTATE_HILIGHTED), p_button->y+y, BUTTON_ROTATE_HILIGHTED);
				}
			}
		}
	}
	// ���ο�
	else
	{
		if(y > 0)
		{
			if(p_button->GetID() == ALPHA_ID)
			{
			#if __CONTENTS(__080405_FIREST_UI_UPDATE)
				if(GetAttributes()->alpha)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->y+x, p_button->x+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_OLD);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->y+x, p_button->x+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_PUSHED_OLD);
			#else
				if(GetAttributes()->alpha)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->y+x, p_button->x+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->y+x, p_button->x+y, C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_PUSHED);
			#endif //__080405_FIREST_UI_UPDATE
			}
			else if(p_button->GetID() == PUSHPIN_ID)
			{
				if(GetAttributes()->autohide)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->y+x, p_button->x+y, C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(p_button->y+x, p_button->x+y, C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN_PUSHED);
			}
			else
			{
				if(p_button->GetFocusState())
				{
					if(p_button->GetPressState())
						m_image_spk.BltLocked(p_button->y+x, p_button->x+y-1, BUTTON_ROTATE_HILIGHTED_PUSHED+ROTATED_OFFSET);
					else
						m_image_spk.BltLocked(p_button->y+x, p_button->x+y-1, BUTTON_ROTATE_HILIGHTED+ROTATED_OFFSET);
				}
			}
		}
		else
		{
			if(p_button->GetID() == ALPHA_ID)
			{
			#if __CONTENTS(__080405_FIREST_UI_UPDATE)
				if(GetAttributes()->alpha)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+p_button->y, y+h-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_OLD), C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_OLD);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+p_button->y, y+h-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_OLD), C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_PUSHED_OLD);
			#else
				if(GetAttributes()->alpha)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+p_button->y, y+h-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA), C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+p_button->y, y+h-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA), C_GLOBAL_RESOURCE::AB_BUTTON_ALPHA_PUSHED);	
			#endif //__080405_FIREST_UI_UPDATE
			}
			else if(p_button->GetID() == PUSHPIN_ID)
			{
				if(GetAttributes()->autohide)
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+p_button->y, y+h-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN);
				else
					gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(x+p_button->y, y+h-p_button->x-gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN_PUSHED);
			}
			else
			{
				if(p_button->GetFocusState())
				{
					if(p_button->GetPressState())
						m_image_spk.BltLocked(x+p_button->y, y+h-p_button->x-m_image_spk.GetHeight(BUTTON_ROTATE_HILIGHTED+ROTATED_OFFSET), BUTTON_ROTATE_HILIGHTED_PUSHED+ROTATED_OFFSET);
					else
						m_image_spk.BltLocked(x+p_button->y, y+h-p_button->x-m_image_spk.GetHeight(BUTTON_ROTATE_HILIGHTED+ROTATED_OFFSET), BUTTON_ROTATE_HILIGHTED+ROTATED_OFFSET);
				}
			}
		}
	}
	
}

/*-----------------------------------------------------------------------------
- Show
-
-----------------------------------------------------------------------------*/
void C_VS_UI_OUSTERS_QUICKITEM::Show()
{
	CT_Done("WnSh: C_VS_UI_OUSTERS_QUICKITEM::Show ENTER pthis=%p", (void*)this);
	static int old_size = GetPocketCount();
	if(old_size != GetPocketCount())
	{
		ResetSize();
		old_size = GetPocketCount();
	}

	int i;
	int temp_x = x, temp_y = y;

	int len = 0;
	RECT rect[8];
	int num[8];

	// �ƾ� ���� �ϵ��ڵ�-_- ���� �����-_-;;
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		bool bl_statch = false;

		if(m_bl_width)
		{
			if(x <= 0)
			{
				m_image_spk.BltLocked(temp_x, temp_y+1, STATCH_EDGE);
				temp_x += m_image_spk.GetWidth(STATCH_EDGE);
			}
			else
			{
				//if(x+w >= RESOLUTION_X)
				if(x+w >= g_pUserInformation->iResolution_x)
				{
					bl_statch = true;
					if(Moving() && m_bl_resize)	// �հ� ���� ����� �������ε�-_-;; �̷� �ϵ� �ڵ��� �ϸ� �ȵǴµ�-_-;;
						temp_x += (m_image_spk.GetWidth(START_EDGE) - m_image_spk.GetWidth(STATCH_EDGE));
					m_image_spk.BltLocked(temp_x, temp_y, START_EDGE_EXTEND);
					temp_x += m_image_spk.GetWidth(START_EDGE_EXTEND);
					if(Moving() && m_bl_resize)	// �հ� ���� ����� �������ε�-_-;; �̷� �ϵ� �ڵ��� �ϸ� �ȵǴµ�-_-;;
						x += (m_image_spk.GetWidth(START_EDGE) - m_image_spk.GetWidth(STATCH_EDGE));
					if(Moving() && m_bl_resize)	// �հ� ���� ����� �������ε�-_-;; �̷� �ϵ� �ڵ��� �ϸ� �ȵǴµ�-_-;;
						x -= (m_image_spk.GetWidth(START_EDGE) - m_image_spk.GetWidth(STATCH_EDGE));
				}
				else
				{
					m_image_spk.BltLocked(temp_x, temp_y, START_EDGE);
					temp_x += m_image_spk.GetWidth(START_EDGE);
				}
			}
		}
		else
		{
			if(y <= 0)
			{
				bl_statch = true;
				m_image_spk.BltLocked(temp_x+1, temp_y, STATCH_EDGE+ROTATED_OFFSET);
				temp_y += m_image_spk.GetHeight(STATCH_EDGE+ROTATED_OFFSET);
			}
			else
			{
				if(Moving() && m_bl_resize)	// �հ� ���� ����� �������ε�-_-;; �̷� �ϵ� �ڵ��� �ϸ� �ȵǴµ�-_-;;
				{
					temp_y += (m_image_spk.GetHeight(START_EDGE+ROTATED_OFFSET) - m_image_spk.GetHeight(STATCH_EDGE+ROTATED_OFFSET));
					m_image_spk.BltLocked(temp_x, temp_y, START_EDGE_EXTEND+ROTATED_OFFSET);
					temp_y += m_image_spk.GetHeight(START_EDGE_EXTEND+ROTATED_OFFSET);
					y += (m_image_spk.GetHeight(START_EDGE+ROTATED_OFFSET) - m_image_spk.GetHeight(STATCH_EDGE+ROTATED_OFFSET));
					y -= (m_image_spk.GetHeight(START_EDGE+ROTATED_OFFSET) - m_image_spk.GetHeight(STATCH_EDGE+ROTATED_OFFSET));
				}
				else
				{
					m_image_spk.BltLocked(temp_x, temp_y, START_EDGE_EXTEND+ROTATED_OFFSET);
					temp_y += m_image_spk.GetHeight(START_EDGE_EXTEND+ROTATED_OFFSET);
				}
			}
		}
		
		MItem * p_selected_item = NULL;
		if(m_focus_slot != -1)
		{
			if( g_pArmsBand1 != NULL && m_focus_slot < g_pArmsBand1->GetPocketNumber() )			// armsband 1���� üũ�Ѵ�.	
				p_selected_item = g_pArmsBand1->GetItem(m_focus_slot);
			else
			{
				if( g_pArmsBand1 != NULL)
					p_selected_item = g_pArmsBand2->GetItem(m_focus_slot - g_pArmsBand1->GetPocketNumber() );
				else
					p_selected_item = g_pArmsBand2->GetItem(m_focus_slot);
			}
//			p_selected_item = g_pQuickSlot->GetItem(m_focus_slot);
		}
		
		if(GetAttributes()->alpha)
		{
			if(m_bl_width)
			{
				RECT alpha_rect;
				alpha_rect.left = temp_x+3;
				alpha_rect.right = temp_x+GetPocketCount()*m_image_spk.GetWidth(SLOT_ALPHA)-4;
				alpha_rect.top = temp_y+9;
				alpha_rect.bottom = temp_y+37;

				DrawAlphaBox(&alpha_rect, 0, 2, 2, g_pUserOption->ALPHA_DEPTH);
			}
			else
			{
				RECT alpha_rect;
				alpha_rect.left = temp_x+9;
				alpha_rect.right = temp_x+37;
				alpha_rect.top = temp_y+3;
				alpha_rect.bottom = temp_y+GetPocketCount()*m_image_spk.GetHeight(SLOT_ALPHA+ROTATED_OFFSET)-4;

				DrawAlphaBox(&alpha_rect, 0, 2, 2, g_pUserOption->ALPHA_DEPTH);
			}
		}

		for (int i=0; i < GetPocketCount(); i++)
		{
			if(GetAttributes()->alpha)
			{
				if(m_bl_width)
					m_image_spk.BltLocked(temp_x, temp_y+1, SLOT_ALPHA);
				else
					m_image_spk.BltLocked(temp_x+1, temp_y, SLOT_ALPHA+ROTATED_OFFSET);
			}
			else
			{
				if(m_bl_width)
					m_image_spk.BltLocked(temp_x, temp_y+1, SLOT);
				else
					m_image_spk.BltLocked(temp_x+1, temp_y, SLOT+ROTATED_OFFSET);
			}
			
			const MItem * p_item = NULL;
			
			if( g_pArmsBand1 != NULL && i < g_pArmsBand1->GetPocketNumber() )			// armsband 1���� üũ�Ѵ�.	
				p_item = g_pArmsBand1->GetItem(i);
			else
			{
				if( g_pArmsBand1 != NULL )
					p_item = g_pArmsBand2->GetItem(i - g_pArmsBand1->GetPocketNumber() );
				else
					p_item = g_pArmsBand2->GetItem(i);
			}
			
			if (p_item != NULL)
			{
				// ������ ���
				TYPE_FRAMEID frame_id = p_item->GetInventoryFrameID();
				
				int _x;
				int _y;
				if(m_bl_width)
				{
					_x = temp_x + QUICKSLOT_W/2-gpC_item->GetWidth(frame_id)/2+3;
					_y = temp_y + QUICKSLOT_H/2-gpC_item->GetHeight(frame_id)/2+8;
				}
				else
				{
					_x = temp_x + QUICKSLOT_W/2-gpC_item->GetWidth(frame_id)/2+8;
					_y = temp_y + QUICKSLOT_H/2-gpC_item->GetHeight(frame_id)/2+3;
				}

				if (p_selected_item && (p_selected_item->GetID() == p_item->GetID()))
				{
					CIndexSprite::SetUsingColorSet(const_cast<MItem *>(p_item)->GetItemOptionColorSet(), 0);
					gpC_item->BltLockedOutline(_x, _y, WHITE, frame_id);
				}
				else
				{
					CIndexSprite::SetUsingColorSet(const_cast<MItem *>(p_item)->GetItemOptionColorSet(), 0);
					gpC_item->BltLocked(_x, _y, frame_id);
				}
				
				// ����ǥ�� AlphaBox�� ���, ���ڴ� �ڿ��� ��´�
				if(p_item->IsPileItem())
				{
					int depth = 0, number;
					for(  depth = 0, number = p_item->GetNumber(); number > 0; number/=10, depth++);
					if(depth == 0) depth = 1;

					rect[len].right = temp_x + QUICKSLOT_W+2;
					rect[len].left = rect[len].right - 7*depth;
					rect[len].bottom = temp_y+QUICKSLOT_H+7;
					rect[len].top = rect[len].bottom - 12;
					if(!m_bl_width)
					{
						rect[len].right += 5;
						rect[len].left += 5;
						rect[len].top -= 5;
						rect[len].bottom -= 5;
					}
					
					DrawAlphaBox(&rect[len], 0, 0, 0, g_pUserOption->ALPHA_DEPTH);
					num[len] = p_item->GetNumber();
					len++;
					
				}
				
			}
			else
				// ���� ��ġ �̸� �� �� �ֵ��� �Ѵ�.
				if (gpC_mouse_pointer->GetPickUpItem() && 
					m_focus_slot == i)
				{
					MItem * p_item = gpC_mouse_pointer->GetPickUpItem();
					
					TYPE_FRAMEID frame_id = p_item->GetInventoryFrameID();
					
					int _x;
					int _y;
					if(m_bl_width)
					{
						_x = temp_x + QUICKSLOT_W/2-gpC_item->GetWidth(frame_id)/2+3;
						_y = temp_y + QUICKSLOT_H/2-gpC_item->GetHeight(frame_id)/2+8;
					}
					else
					{
						_x = temp_x + QUICKSLOT_W/2-gpC_item->GetWidth(frame_id)/2+8;
						_y = temp_y + QUICKSLOT_H/2-gpC_item->GetHeight(frame_id)/2+3;
					}
					
					MItem * p_old_item;
					//if (g_pQuickSlot->CanReplaceItem(p_item, m_focus_slot, p_old_item))
					
					MOustersArmsBand *pQuickSlot;
					
					if( g_pArmsBand1 != NULL && g_pArmsBand1->GetPocketNumber() > m_focus_slot )
						pQuickSlot = g_pArmsBand1;
					else
						pQuickSlot = g_pArmsBand2;
					
					int skip_slot = 0;

					if( pQuickSlot == g_pArmsBand2 && g_pArmsBand1 != NULL )
						skip_slot = g_pArmsBand1->GetPocketNumber();

					if(pQuickSlot!= NULL && pQuickSlot->CanReplaceItem( p_item, m_focus_slot - skip_slot, p_old_item ) )
					{
						if (p_old_item)
							gpC_item->BltLockedOutlineOnly(_x, _y, ga_item_blink_color_table[g_blink_value], frame_id);
						else
							gpC_item->BltLockedOutlineOnly(_x, _y, POSSIBLE_COLOR, frame_id);
					}
				}
				
				
				if(m_bl_width)
					temp_x += m_image_spk.GetWidth(SLOT);
				else
					temp_y += m_image_spk.GetHeight(SLOT+ROTATED_OFFSET);
		}
		
		if(m_bl_width)
		{
			if(bl_statch)
				m_image_spk.BltLocked(temp_x, temp_y+1, STATCH_EDGE);
			else
			{
				m_image_spk.BltLocked(temp_x, temp_y, END_EDGE_EXTEND);
			}
		}
		else
		{
			if(bl_statch)
			{
				m_image_spk.BltLocked(temp_x, temp_y, END_EDGE_EXTEND+ROTATED_OFFSET);
			}
			else
			{
				//if(y+h >= RESOLUTION_Y)
				if(y+h >= g_pUserInformation->iResolution_y)
					m_image_spk.BltLocked(temp_x+1, temp_y, STATCH_EDGE+ROTATED_OFFSET);
				else
					m_image_spk.BltLocked(temp_x, temp_y, END_EDGE+ROTATED_OFFSET);
			}
		}
		m_pC_button_group->Show();
		gpC_base->m_p_DDSurface_back->Unlock();
	}

	if(Moving() && m_bl_resize)
	{
		if(m_bl_width)
		{
			//if(x+w == RESOLUTION_X)
			if(x+w == g_pUserInformation->iResolution_x)
				x += m_image_spk.GetWidth(START_EDGE) - m_image_spk.GetWidth(STATCH_EDGE);
		}
		else 
		{
			if(y != 0)//+h == RESOLUTION_Y)
				y += m_image_spk.GetWidth(START_EDGE) - m_image_spk.GetWidth(STATCH_EDGE);
		}
	}	
	
	if(Moving() && m_bl_resize)
	{
		if(m_bl_width)
		{
			//if(x+w == RESOLUTION_X)
			if(x+w == g_pUserInformation->iResolution_x)
				x -= m_image_spk.GetWidth(START_EDGE) - m_image_spk.GetWidth(STATCH_EDGE);
		}
		else 
		{
			if(y != 0)//+h == RESOLUTION_Y)
				y -= m_image_spk.GetWidth(START_EDGE) - m_image_spk.GetWidth(STATCH_EDGE);
		}
	}

	COLORREF markColor = RGB(220, 220, 220);;//RGB(140, 140, 255);
	char sz_temp[10];
	g_FL2_GetDC();
	for(int i = 0; i < len; i++)
	{
		wsprintf(sz_temp, "%d", num[i]);
		g_PrintColorStr(rect[i].left, rect[i].top, sz_temp, gpC_base->m_item_desc_pi, markColor);	
//		g_Print(rect.left, rect.top, sz_temp, &gpC_base->m_item_desc_pi);	
	}
	m_pC_button_group->ShowDescription();
	g_FL2_ReleaseDC();
	

	// show hot-key
	char * p_hotkey_mark[] = {"F1", "F2", "F3", "F4", "F5", "F6" , "F7" , "F8" , "F9" ,  "F10" , "F11" , "F12"};
	if (m_focus_slot != NOT_SELECTED)
	//for (int i=0; i < GetPocketCount(); i++)
	{
		POINT point;
		if(m_bl_width)
		{
			point.x = x+m_p_slot_x[m_focus_slot]+1;
			point.y = y+7;
		}
		else
		{
			point.x = x+8;
			point.y = y+m_p_slot_x[m_focus_slot];
		}

		int	ifocusHotKey = m_Hotkey_buf[m_focus_slot]  ;

		if(ifocusHotKey >= 0)
		{
		// ������ �ڽ� ���..
//		if (gpC_base->m_p_DDSurface_back->Lock())
			{
				RECT rect;

				rect.left = point.x;
				rect.top = point.y+18;
				rect.right = rect.left + 14;
				rect.bottom = rect.top + 12;

				DrawAlphaBox(&rect, 0, 0, 0, g_pUserOption->ALPHA_DEPTH);

	//			gpC_base->m_p_DDSurface_back->Unlock();
			}

			COLORREF markColor = RGB(140, 140, 255);

			//m_OustersQuickItem_Hotkey_buf[m_focus_slot] = 11 ; 
			//int	ifocusHotKey = m_Hotkey_buf[m_focus_slot]  ;

			//if(ifocusHotKey >= 0)
			g_PrintColorStr(point.x, point.y+18, p_hotkey_mark[ifocusHotKey], gpC_base->m_item_desc_pi, markColor);	
		}
		//g_PrintColorStr(point.x, point.y+18, p_hotkey_mark[m_focus_slot], gpC_base->m_item_desc_pi, markColor);	

		//g_PrintColorStr(point.x, point.y+18, p_hotkey_mark[m_focus_slot], gpC_base->m_item_desc_pi, markColor);	
	}

}

bool C_VS_UI_OUSTERS_QUICKITEM::Click()
{
	if (m_focus_slot == NOT_SELECTED || gbl_item_lock || 
		(g_pArmsBand1 == NULL && g_pArmsBand2 == NULL) )
//		 g_pQuickSlot == NULL)
	{
		return false;
	}

	int item_x, item_y;

	if (gpC_mouse_pointer->GetPickUpItem()) // ��� �ִ°�?
	{
		MItem* p_cur_item = NULL;
		
		if( g_pArmsBand1 != NULL && g_pArmsBand1->GetPocketNumber() > m_focus_slot )
			p_cur_item = g_pArmsBand1->GetItem( m_focus_slot );
		else
		{
			if( g_pArmsBand1 != NULL )
				p_cur_item = g_pArmsBand2->GetItem( m_focus_slot - g_pArmsBand1->GetPocketNumber() );
			else
				p_cur_item = g_pArmsBand2->GetItem( m_focus_slot );
		}

		// �ѿ� źâ�� ����� �Ͱ� ���� ���� insert item�̴�.
		if (gpC_mouse_pointer->GetPickUpItem()->IsInsertToItem( p_cur_item ))
		{
			// ������ �ִ� item�� �߰��� �� �ִ� ���

			// ��� Item�� ��� �ִ�(�߰��� Item)�� Client���� �˾ƾ� �Ѵ�.
			// ��� �ִ� Item�� Client���� access�� �� �����Ƿ� ��� Item�� ������.
			gpC_base->SendMessage(UI_ITEM_INSERT_FROM_QUICKSLOT,
																  m_focus_slot,
																  0,
																  (void *)p_cur_item); // ��� Item
			return true;
		}
		else
		{
			MItem * p_old_item = NULL;

//			if (g_pQuickSlot->CanReplaceItem(gpC_mouse_pointer->GetPickUpItem(),
//												  m_focus_slot,
//												  p_old_item))
			MOustersArmsBand *pQuickSlot = NULL;
			if( g_pArmsBand1 != NULL && g_pArmsBand1->GetPocketNumber() > m_focus_slot )
				pQuickSlot = g_pArmsBand1;
			else
				pQuickSlot = g_pArmsBand2;

			int skip_slot = 0;

			if( pQuickSlot == g_pArmsBand2 && g_pArmsBand1 != NULL)
				skip_slot = g_pArmsBand1->GetPocketNumber();				

			if( pQuickSlot != NULL && pQuickSlot->CanReplaceItem( gpC_mouse_pointer->GetPickUpItem(),
				m_focus_slot - skip_slot, p_old_item ) )
			{
				gpC_base->SendMessage(UI_ITEM_DROP_TO_QUICKSLOT, m_focus_slot);
				return true;
			}
			else
			{
				// failed.
				return false;
			}
		}
	}
	else
	{
		// ���´�.
		//const MItem * p_item = g_pQuickSlot->GetItem(m_focus_slot);
		MOustersArmsBand * pQuickSlot = NULL;
		MItem *p_item = NULL;
		int slot = m_focus_slot;
		
		if( g_pArmsBand1 == NULL )
			pQuickSlot = g_pArmsBand2;
		else
		{
			if( g_pArmsBand1->GetPocketNumber() > slot )
			{
				pQuickSlot = g_pArmsBand1;
			} else
			{
				pQuickSlot = g_pArmsBand2;
				slot -= g_pArmsBand1->GetPocketNumber();
			}
		}
		
		if( pQuickSlot == NULL )
			return false;

		p_item = pQuickSlot->GetItem( slot );

		if (p_item != NULL) // Item�� �ִ�.
		{
			if(m_bl_width)
			{
				item_x = x+m_p_slot_x[m_focus_slot]+QUICKSLOT_W/2-gpC_item->GetWidth(p_item->GetInventoryFrameID())/2;
				item_y = y+QUICKSLOT_H/2-gpC_item->GetHeight(p_item->GetInventoryFrameID())/2;
			}
			else
			{
				item_x = x+QUICKSLOT_W/2-gpC_item->GetWidth(p_item->GetInventoryFrameID())/2;
				item_y = y+m_p_slot_x[m_focus_slot]+QUICKSLOT_H/2-gpC_item->GetHeight(p_item->GetInventoryFrameID())/2;
			}

			//g_pQuickSlot->RemoveItem((BYTE)m_focus_slot);
			pQuickSlot->RemoveItem( (BYTE) slot );

			gpC_base->SendMessage(UI_ITEM_PICKUP_FROM_QUICKSLOT,
																  m_focus_slot, 
																  MAKEDWORD(item_x, item_y), 
																  (MItem *)p_item);
			// SHIFT������ Ŭ�������� ������ �ڵ� �̵� ó�� -> �κ��丮
			if(g_pDXInput->KeyDown(DIK_LSHIFT))
			{
				POINT point;
				if(p_item != NULL && g_pInventory->GetFitPosition((MItem *)p_item, point))
				{
					const MItem* p_cur_item = g_pInventory->GetItem(point.x, point.y);
					
					// �ѿ� źâ�� ����� �Ͱ� ���� ���� insert item�̴�.
					// ��ġ�� ������ ��ġ�Ұ�쿡�� �߰��Ѵ�.
					if (p_item->IsInsertToItem( p_cur_item ) && p_cur_item->GetGridX() == point.x && p_cur_item->GetGridY() == point.y)
					{
						// ������ �ִ� item�� �߰��� �� �ִ� ���
						
						// ��� Item�� ��� �ִ�(�߰��� Item)�� Client���� �˾ƾ� �Ѵ�.
						// ��� �ִ� Item�� Client���� access�� �� �����Ƿ� ��� Item�� ������.
						gpC_base->SendMessage(UI_ITEM_INSERT_FROM_INVENTORY,
							point.x, point.y,
							(void *)p_cur_item); // ��� Item
					}
					else
					{	
						// �߰��� �� ���� ���
						MItem* p_old_item  = NULL;
						
						if (g_pInventory->CanReplaceItem((MItem *)p_item,		// �߰��� item
							point.x, point.y,	// �߰��� ��ġ 
							p_old_item))								// �����ִ� item
						{
							
							gpC_base->SendMessage(UI_ITEM_DROP_TO_INVENTORY, 
								point.x, point.y,
								(void *)p_item);
							
						}
						else
						{
							// failed.
							return false;
						}
					}
					
				}
			}
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Use
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_OUSTERS_QUICKITEM::Use(int slot, bool bFunctionKey)
{
	if (slot == NOT_SELECTED || gbl_item_lock || g_pArmsBand1 == NULL && g_pArmsBand2 == NULL)
		return;

	if (gpC_mouse_pointer->GetPickUpItem())
	{

	}
	else
	{
		const MItem * p_item = GetItem( slot );

		if (p_item) // Item�� �ִ�.
		{
			gpC_base->SendMessage(UI_ITEM_USE_QUICKSLOT,
									  slot,
									  bFunctionKey, 
									  (MItem *)p_item);
		}
	}
}

void	C_VS_UI_OUSTERS_QUICKITEM::SetHotkey(HOTKEY hotkey , int slot)
{
	int i;
	
	for (int i=0; i < HOTKEY_MAX; i++)
	{
		//if (m_OustersQuickItem_Hotkey_buf[i] != NOT_SELECTED)
		//{
		//	m_OustersQuickItem_Hotkey_buf[i] = NOT_SELECTED;
			//turn;
		//}
		//else 
		if(( i != slot) && (m_Hotkey_buf[i]  == hotkey))
			m_Hotkey_buf[i] = NOT_SELECTED;
	}

	
	if (m_Hotkey_buf[slot] != NOT_SELECTED)
	{
		m_Hotkey_buf[slot] = NOT_SELECTED;
		return ;	
	}

	if(m_Hotkey_buf[slot] == NOT_SELECTED)
	{
		m_Hotkey_buf[slot] = hotkey ; 
	}
}

void	C_VS_UI_OUSTERS_QUICKITEM::InitHotKey(HOTKEY hotkey)
{
	for (int i=0; i < HOTKEY_MAX; i++)
	{
		if(m_Hotkey_buf[i] == hotkey)
				m_Hotkey_buf[i] = NOT_SELECTED;
	}
}

int	C_VS_UI_OUSTERS_QUICKITEM::GetFocusedSlot()
{
	return	m_focus_slot	;
}

int	C_VS_UI_OUSTERS_QUICKITEM::GetHotkey(int	slot)
{
	return m_Hotkey_buf[slot] ; 
}


int	C_VS_UI_OUSTERS_QUICKITEM::GetSlot(HOTKEY hotkey)

{
	int slot = 0 ; 
	for (int i=0; i < HOTKEY_MAX; i++)
	{
		if(m_Hotkey_buf[i] == hotkey)
			return i ; 
			
	}

	return slot; 
}


void	C_VS_UI_OUSTERS::RunQuickItemSlot()
{
	if( m_pC_armsband != NULL )
	{
		m_pC_armsband->Open();
	}
}

void	C_VS_UI_OUSTERS::CloseQuickItemSlot()
{
	if( m_pC_armsband != NULL )
	{
		m_pC_armsband->Close();
	}
}

bool	C_VS_UI_OUSTERS::IsRunningQuickItemSlot()
{
	return true;
}

void	C_VS_UI_OUSTERS::ResetOustersQuickItemSize()
{
	m_pC_armsband->ResetSize();
}

void	C_VS_UI_OUSTERS::HotKey_QuickItemSlot()
{
	assert(m_pC_armsband != NULL);

	m_pC_armsband->ToggleBelt();
}

void	C_VS_UI_OUSTERS::Hotkey_Function(int FKey)
{
	assert(m_pC_armsband != NULL);
	
	// ��ų , �Ͻ���� ó�� 

	int	current_mouse_x = gpC_window_manager->GetCurrentMouse_x() ;
	int	current_mouse_y = gpC_window_manager->GetCurrentMouse_y() ;

	C_VS_UI_SKILL * p_skill = GetSkillWindow()		;
	bool	bSkillFocus = p_skill->IsPixel2(current_mouse_x , current_mouse_y) ; 
	bool	bArmsBandFocus =  m_pC_armsband->IsPixel(current_mouse_x , current_mouse_y) ; 

	if(bSkillFocus)
	{
		int skillslot = p_skill->GetFocusedSlot()		;
		int skillid   = p_skill->GetSkillID(skillslot)	;
		if (skillid != NOT_SELECTED)
		{
			p_skill->SetHotkey(C_VS_UI_SKILL::HOTKEY(FKey), (ACTIONINFO)skillid)	;
			m_pC_armsband->InitHotKey(C_VS_UI_OUSTERS_QUICKITEM::HOTKEY(FKey)) ; 
			m_HotKey_Type[C_VS_UI_SKILL::HOTKEY(FKey)] = SKILL_TYPE ;
		}
		else 
		{
			p_skill->ChangeSkillByHotkey(C_VS_UI_SKILL::HOTKEY(FKey)) ;
		}
	}	
	else if(bArmsBandFocus)
	{
		int ArmsBandSlot = m_pC_armsband->GetFocusedSlot();
		if(ArmsBandSlot >= 0)
		{
			m_pC_armsband->SetHotkey(C_VS_UI_OUSTERS_QUICKITEM::HOTKEY(FKey) , ArmsBandSlot);
			p_skill->InitHotKey(C_VS_UI_SKILL::HOTKEY(FKey)) ;
			m_HotKey_Type[FKey] = QUICKITEM_TYPE ;
		}
	}
	else if(!bSkillFocus && !bArmsBandFocus)
	{
		if(m_HotKey_Type[FKey] == SKILL_TYPE) 
			p_skill->ChangeSkillByHotkey(C_VS_UI_SKILL::HOTKEY(FKey)) ; 
		else
		{
			int Armsbandslot = m_pC_armsband->GetSlot(C_VS_UI_OUSTERS_QUICKITEM::HOTKEY(FKey)) ;//
			m_pC_armsband->Use(Armsbandslot);
		}
	}}
