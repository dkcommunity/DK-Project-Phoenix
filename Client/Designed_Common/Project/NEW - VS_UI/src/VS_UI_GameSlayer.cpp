// VS_UI_GameSlayer.cpp

#include "client_PCH.h"
#include "crash_trace_tool.h"
#include "../../Client/PhoenixDWrite.h"  // FULL DX11 gecis Faz T3 — DWrite text (GDI fallback'li)
#include "../../Client/DXLib/D3D11SpriteCommands.h"  // Faz G5 — ham pixel yazilari dirty bildirimi
#include "VS_UI_GameSlayer.h"
#include "VS_UI_filepath.h"
#include "VS_UI_Item.h"
#include "VS_UI_Mouse_pointer.h"
#include "MCreatureTable.h"
#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
#include "UserOption.h"
#include "VS_UI.h"
#include "ExperienceTable.h"
#include "MGameStringTable.h"
#include "UserInformation.h"
#include "MZoneTable.h"
#include "SystemAvailabilities.h"
//#include "ex\DebugInfo.h"

#define MP_CASE_X				708
#define MP_CASE_Y				508
#define HP_PIE_X				634
#define HP_PIE_Y				511
#define MP_PIE_X				724
#define MP_PIE_Y				511
#define PIE_W					50
#define PIE_H					50
#define HP_PIE_START_X		(HP_PIE_X+15) // 12
#define HP_PIE_START_Y		HP_PIE_Y
#define HP_PIE_END_X			(HP_PIE_X+16) // 14
#define HP_PIE_END_Y			HP_PIE_Y
#define MP_PIE_START_X		(MP_PIE_X+15)
#define MP_PIE_START_Y		MP_PIE_Y
#define MP_PIE_END_X			(MP_PIE_X+16)
#define MP_PIE_END_Y			MP_PIE_Y

extern bool gbl_item_lock;
extern HWND g_hWnd;

namespace
{
	// Text cache transparan key'i magenta olmamali; GDI anti-alias kenarlari
	// arka planla harmanlayip pembe sacak olarak ekrana tasiyordu.
	const WORD PHOENIX_SLAYER_EXP_TEXT_CACHE_KEY = 0x0001;

	struct PhoenixSlayerExpTextCache
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

	static PhoenixSlayerExpTextCache g_slayerExpTextCache =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		0,
		0,
		0,
		0,
		0,
		0,
		NULL,
		false
	};

	static void Phoenix_ReleaseSlayerExpTextCache()
	{
		if(g_slayerExpTextCache.hdc != NULL && g_slayerExpTextCache.oldBitmap != NULL)
		{
			SelectObject(g_slayerExpTextCache.hdc, g_slayerExpTextCache.oldBitmap);
			g_slayerExpTextCache.oldBitmap = NULL;
		}

		if(g_slayerExpTextCache.bitmap != NULL)
		{
			DeleteObject(g_slayerExpTextCache.bitmap);
			g_slayerExpTextCache.bitmap = NULL;
		}

		if(g_slayerExpTextCache.hdc != NULL)
		{
			DeleteDC(g_slayerExpTextCache.hdc);
			g_slayerExpTextCache.hdc = NULL;
		}

		g_slayerExpTextCache.bits = NULL;
		g_slayerExpTextCache.width = 0;
		g_slayerExpTextCache.height = 0;
		g_slayerExpTextCache.pitchWords = 0;
		g_slayerExpTextCache.left = 0;
		g_slayerExpTextCache.top = 0;
		g_slayerExpTextCache.signature = 0;
		g_slayerExpTextCache.font = NULL;
		g_slayerExpTextCache.valid = false;
	}

	static bool Phoenix_EnsureSlayerExpTextCacheSurface(int width, int height)
	{
		if(width <= 0 || height <= 0)
			return false;

		if(g_slayerExpTextCache.hdc != NULL &&
			g_slayerExpTextCache.bitmap != NULL &&
			g_slayerExpTextCache.bits != NULL &&
			g_slayerExpTextCache.width == width &&
			g_slayerExpTextCache.height == height)
		{
			return true;
		}

		Phoenix_ReleaseSlayerExpTextCache();

		g_slayerExpTextCache.hdc = CreateCompatibleDC(NULL);
		if(g_slayerExpTextCache.hdc == NULL)
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
		g_slayerExpTextCache.bitmap = CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&bitmapInfo), DIB_RGB_COLORS, &bits, NULL, 0);
		if(g_slayerExpTextCache.bitmap == NULL || bits == NULL)
		{
			Phoenix_ReleaseSlayerExpTextCache();
			return false;
		}

		g_slayerExpTextCache.oldBitmap = static_cast<HBITMAP>(SelectObject(g_slayerExpTextCache.hdc, g_slayerExpTextCache.bitmap));
		g_slayerExpTextCache.bits = static_cast<WORD*>(bits);
		g_slayerExpTextCache.width = width;
		g_slayerExpTextCache.height = height;
		g_slayerExpTextCache.pitchWords = (((width * 16 + 31) / 32) * 4) / sizeof(WORD);
		g_slayerExpTextCache.valid = false;
		return true;
	}

	static void Phoenix_FillSlayerExpTextCache()
	{
		if(g_slayerExpTextCache.bits == NULL || g_slayerExpTextCache.pitchWords <= 0)
			return;

		for(int row = 0; row < g_slayerExpTextCache.height; ++row)
		{
			WORD *dst = g_slayerExpTextCache.bits + row * g_slayerExpTextCache.pitchWords;
			for(int col = 0; col < g_slayerExpTextCache.width; ++col)
				dst[col] = PHOENIX_SLAYER_EXP_TEXT_CACHE_KEY;
		}
	}

	static DWORD Phoenix_MixSlayerExpTextSignature(DWORD signature, int value)
	{
		return (signature ^ static_cast<DWORD>(value)) * 16777619u;
	}

	static DWORD Phoenix_BuildSlayerExpTextSignature(
		int strCur,
		int dexCur,
		int intCur,
		int domainBlade,
		int domainSword,
		int domainGun,
		int domainHeal,
		int domainEnchant,
		HFONT font)
	{
		DWORD signature = 2166136261u;
		signature = Phoenix_MixSlayerExpTextSignature(signature, strCur);
		signature = Phoenix_MixSlayerExpTextSignature(signature, dexCur);
		signature = Phoenix_MixSlayerExpTextSignature(signature, intCur);
		signature = Phoenix_MixSlayerExpTextSignature(signature, domainBlade);
		signature = Phoenix_MixSlayerExpTextSignature(signature, domainSword);
		signature = Phoenix_MixSlayerExpTextSignature(signature, domainGun);
		signature = Phoenix_MixSlayerExpTextSignature(signature, domainHeal);
		signature = Phoenix_MixSlayerExpTextSignature(signature, domainEnchant);
		signature = Phoenix_MixSlayerExpTextSignature(signature, static_cast<int>(reinterpret_cast<DWORD_PTR>(font)));
		return signature;
	}

	static bool Phoenix_NeedsSlayerExpTextCacheBuild(int left, int top, DWORD signature, HFONT font)
	{
		return !g_slayerExpTextCache.valid ||
			g_slayerExpTextCache.left != left ||
			g_slayerExpTextCache.top != top ||
			g_slayerExpTextCache.signature != signature ||
			g_slayerExpTextCache.font != font;
	}

	static HDC Phoenix_BeginSlayerExpTextCacheBuild()
	{
		if(g_slayerExpTextCache.hdc == NULL || g_slayerExpTextCache.bits == NULL)
			return NULL;

		Phoenix_FillSlayerExpTextCache();
		return g_slayerExpTextCache.hdc;
	}

	static void Phoenix_CommitSlayerExpTextCacheBuild(int left, int top, DWORD signature, HFONT font)
	{
		g_slayerExpTextCache.left = left;
		g_slayerExpTextCache.top = top;
		g_slayerExpTextCache.signature = signature;
		g_slayerExpTextCache.font = font;
		g_slayerExpTextCache.valid = true;
	}

	static int Phoenix_GetSlayerCachedTextWidth(HDC hdc, const char *text, HFONT font)
	{
		if(hdc == NULL || text == NULL)
			return 0;

		if(font != NULL)
			SelectObject(hdc, font);

		SIZE textSize = {0, 0};
		GetTextExtentPoint32(hdc, text, static_cast<int>(strlen(text)), &textSize);
		return textSize.cx;
	}

	static int Phoenix_DrawSlayerCachedTextShadow(HDC hdc, int x, int y, const char *text, const PrintInfo &pi, COLORREF textColor, COLORREF shadowColor)
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

	static bool Phoenix_BltSlayerExpTextCache()
	{
		if(!g_slayerExpTextCache.valid ||
			g_slayerExpTextCache.bits == NULL ||
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
		int destX = g_slayerExpTextCache.left;
		int destY = g_slayerExpTextCache.top;
		int copyWidth = g_slayerExpTextCache.width;
		int copyHeight = g_slayerExpTextCache.height;

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
			const WORD *src = g_slayerExpTextCache.bits + (sourceY + row) * g_slayerExpTextCache.pitchWords + sourceX;
			WORD *dst = destBase + (destY + row) * destPitchWords + destX;
			for(int col = 0; col < copyWidth; ++col)
			{
				const WORD pixel = src[col];
				if(pixel != PHOENIX_SLAYER_EXP_TEXT_CACHE_KEY)
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

	static void Phoenix_DrawSlayerExpTextLayerCached(
		HDC hdc,
		int cacheLeft,
		int cacheTop,
		int panelX,
		int panelY,
		int bladeWidth,
		int domainLevelBlade,
		int domainLevelSword,
		int domainLevelGun,
		int domainLevelHeal,
		int domainLevelEnchant)
	{
		if(hdc == NULL || gpC_base == NULL)
			return;

		PrintInfo &pi = gpC_base->m_chatting_pi;
		char text[100];
		const int maxX = (245 - 55 * 3) / 4;

		wsprintf(text, "STR");
		int labelWidth = Phoenix_GetSlayerCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + maxX - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);

		wsprintf(text, "%d", g_char_slot_ingame.STR_CUR);
		int valueWidth = Phoenix_GetSlayerCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + maxX + labelWidth + (55 - labelWidth - valueWidth - 1) / 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "DEX");
		int dexLabelWidth = Phoenix_GetSlayerCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + maxX * 2 + 55 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);

		wsprintf(text, "%d", g_char_slot_ingame.DEX_CUR);
		int dexValueWidth = Phoenix_GetSlayerCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + maxX * 2 + 55 + dexLabelWidth + (55 - dexValueWidth - dexLabelWidth - 1) / 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "INT");
		int intLabelWidth = Phoenix_GetSlayerCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + maxX * 3 + 55 * 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);

		wsprintf(text, "%d", g_char_slot_ingame.INT_CUR);
		int intValueWidth = Phoenix_GetSlayerCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + maxX * 3 + 55 * 2 + intLabelWidth + (55 - intLabelWidth - intValueWidth - 1) / 2 - cacheLeft, panelY + 70 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "%d", domainLevelBlade);
		int domainWidth = Phoenix_GetSlayerCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + 35 + (18 - domainWidth) / 2 - cacheLeft, panelY + 111 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "%d", domainLevelSword);
		domainWidth = Phoenix_GetSlayerCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + 35 + bladeWidth + 20 - 1 + (18 - domainWidth) / 2 - cacheLeft, panelY + 111 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "%d", domainLevelGun);
		domainWidth = Phoenix_GetSlayerCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + 35 + bladeWidth * 2 + 20 * 2 - 1 + (18 - domainWidth) / 2 - cacheLeft, panelY + 111 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "%d", domainLevelHeal);
		domainWidth = Phoenix_GetSlayerCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + 35 + bladeWidth * 3 + 20 * 3 - 1 + (18 - domainWidth) / 2 - cacheLeft, panelY + 111 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, "%d", domainLevelEnchant);
		domainWidth = Phoenix_GetSlayerCachedTextWidth(hdc, text, pi.hfont);
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + 35 + bladeWidth * 4 + 20 * 4 - 1 + (18 - domainWidth) / 2 - cacheLeft, panelY + 111 - cacheTop, text, pi, RGB_RED, RGB_BLACK);

		wsprintf(text, ""); // Exp bar string EXP.Point
		Phoenix_DrawSlayerCachedTextShadow(hdc, panelX + maxX - 10 - cacheLeft, panelY + 133 - cacheTop, text, pi, RGB_WHITE, RGB_BLACK);
	}
}

static bool Phoenix_ReadSlayerPortalInt(const char *data, int dataSize, int &offset, int &value)
{
    if (data == NULL || dataSize < 0 || offset < 0 || offset > dataSize - (int)sizeof(int))
    {
        return false;
    }

    memcpy(&value, data + offset, sizeof(int));
    offset += sizeof(int);
    return true;
}

static bool Phoenix_IsValidSlayerPortalFlag(const UI_PORTAL_FLAG &flag)
{
    return flag.zone_id >= 0 &&
        flag.x >= 0 &&
        flag.y >= 0 &&
        flag.portal_x >= 0 && flag.portal_x < 256 &&
        flag.portal_y >= 0 && flag.portal_y < 256 &&
        flag.extend_portal >= 0;
}

static void Phoenix_LogSlayerPortalError(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    buffer[sizeof(buffer) - 1] = '\0';
    OutputDebugStringA(buffer);
    OutputDebugStringA("\n");
}

//
// help string
//
/*static S_DEFAULT_HELP_STRING	g_help_string[9] = {
	{"Inventory", "(������â)", "TAB"},
	{"PartyManager", "(��Ƽâ)", "Ctrl+P"},
	{"Character Info", "(ĳ���� ����)", "Ctrl+I"},
	{"Skill Info", "(��� ����)", "Ctrl+K"},
	{"Help", "(����)", "Ctrl+H"},
	{"Gear", "(����â)", "Ctrl+TAB"},
	{"Game Menu", "(���� �޴�)", "ESC"},
	{"Mini-Map", "(�̴ϸ�)", "Ctrl+M"},
	{"Mark", "(Ư������)", "Ctrl+X"},
};

static S_DEFAULT_HELP_STRING	g_chat_help_string[5] = {
	{"Extend Window", "(ä��â Ȯ��)", "Ctrl+E"},
	{"Chat", "(�Ϲ� ä��)", "Ctrl+C"},
	{"Guild/Party Chat", "(���/��Ƽ ä��)", "Ctrl+G"},
	{"Whisper", "(�ӼӸ�)", "Ctrl+W"},
	{"ZoneChat", "(��ġ��)", "Ctrl+Z"},
};*/

#ifndef _LIB // test
/*
long	C_VS_UI_SLAYER_PDS::m_str = 100;
long	C_VS_UI_SLAYER_PDS::m_dex = 99;
long	C_VS_UI_SLAYER_PDS::m_int = 66;
long	C_VS_UI_SLAYER_PDS::m_hp = 43;
long	C_VS_UI_SLAYER_PDS::m_mp = 90;
long	C_VS_UI_SLAYER_PDS::m_th = 232;
long	C_VS_UI_SLAYER_PDS::m_ac = 554;
long	C_VS_UI_SLAYER_PDS::m_cc = 14;
long	C_VS_UI_SLAYER_PDS::m_str_max = 100;
long	C_VS_UI_SLAYER_PDS::m_dex_max = 100;
long	C_VS_UI_SLAYER_PDS::m_int_max = 100;
long	C_VS_UI_SLAYER_PDS::m_hp_max = 100;
long	C_VS_UI_SLAYER_PDS::m_mp_max = 100;
long	C_VS_UI_SLAYER_PDS::m_th_max = 1000;
long	C_VS_UI_SLAYER_PDS::m_ac_max = 1000;
long	C_VS_UI_SLAYER_PDS::m_cc_max = 1000;*/
//long	C_VS_UI_SLAYER_PDS::m_pcs_number = 7654321;
#else
/*
long	C_VS_UI_SLAYER_PDS::m_str;	
long	C_VS_UI_SLAYER_PDS::m_dex;
long	C_VS_UI_SLAYER_PDS::m_int;
long	C_VS_UI_SLAYER_PDS::m_hp;
long	C_VS_UI_SLAYER_PDS::m_mp;
long	C_VS_UI_SLAYER_PDS::m_th;
long	C_VS_UI_SLAYER_PDS::m_ac;
long	C_VS_UI_SLAYER_PDS::m_cc;
long	C_VS_UI_SLAYER_PDS::m_str_max;
long	C_VS_UI_SLAYER_PDS::m_dex_max;
long	C_VS_UI_SLAYER_PDS::m_int_max;
long	C_VS_UI_SLAYER_PDS::m_hp_max;
long	C_VS_UI_SLAYER_PDS::m_mp_max;
long	C_VS_UI_SLAYER_PDS::m_th_max;
long	C_VS_UI_SLAYER_PDS::m_ac_max;
long	C_VS_UI_SLAYER_PDS::m_cc_max;*/
//long	C_VS_UI_SLAYER_PDS::m_pcs_number;
#endif

int C_VS_UI_SLAYER_GEAR::m_slot_image[SLOT_SIZE] = {
	GEAR_SLAYER_TOU, GEAR_SLAYER_XIANGLIAN, GEAR_SLAYER_YIFU, GEAR_SLAYER_DUN, GEAR_SLAYER_SWORD, GEAR_SLAYER_SHOUTAO, GEAR_SLAYER_YAODAI, GEAR_SLAYER_KUZI, GEAR_SLAYER_SHOUZUO, GEAR_SLAYER_SHOUZUO, GEAR_SLAYER_JIEZHI, GEAR_SLAYER_JIEZHI, GEAR_SLAYER_JIEZHI, GEAR_SLAYER_JIEZHI, GEAR_SLAYER_XIEZI
	,-1
	,-1
	,-1
	,-1
	,GEAR_SLAYER_TUNXUN,GEAR_SLAYER_BEIJIAN
	,-1// nanomech 2006.03.09 Item ���� �۾�
	,-1,-1 	// Cue Of Adam
	,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU,GEAR_SHENGSHU
};


//int C_VS_UI_ADVANCEMENTSLAYER_GEAR::m_slot_image[SLOT_SIZE] = {
//	HELM, NECKLACE, COAT, SHIELD, SWORD, GLOVE, BELT, TROUSER, BRACELET, BRACELET, RING, RING, RING, RING, SHOES
//	,-1
//	,-1
//	,-1
//	,-1
//	,BLOODBIBLE,BLOODBIBLE,BLOODBIBLE,BLOODBIBLE,BLOODBIBLE,BLOODBIBLE,PDA,SHOULDER
//};




//----------------------------------------------------------------------------
// Operations
//----------------------------------------------------------------------------
/*
//-----------------------------------------------------------------------------
//- C_VS_UI_SLAYER_CHAT by larosel
//-
//-----------------------------------------------------------------------------
C_VS_UI_SLAYER_CHAT::C_VS_UI_SLAYER_CHAT()
{
//	g_RegisterWindow(this);

//	AttrPin(true);

	m_chat_spk.Open(SPK_SLAYER_CHAT_BUTTON);

	m_chat = CHAT;
	m_bl_whisper = false;
//	Set(20, 577, 560, 597);
}

//-----------------------------------------------------------------------------
//- ~C_VS_UI_SLAYER_CHAT by larosel
//-
//-----------------------------------------------------------------------------
C_VS_UI_SLAYER_CHAT::~C_VS_UI_SLAYER_CHAT()
{
//	g_UnregisterWindow(this);
}

//-----------------------------------------------------------------------------
// WindowEventReceiver by larosel
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_CHAT::WindowEventReceiver(id_t event)
{
}

//-----------------------------------------------------------------------------
// IsPixel by larosel
//
// 
//-----------------------------------------------------------------------------
bool C_VS_UI_SLAYER_CHAT::IsPixel(int _x, int _y)
{
	return IsInRect(_x, _y);
}

#define HISTORY_LINE 4
//-----------------------------------------------------------------------------
// ShowButtonWidget by larosel
//
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_CHAT::ShowButton(C_VS_UI_EVENT_BUTTON * p_button)
{
	if((p_button->GetID() == WHISPER && m_bl_whisper) || g_HISTORY_LINE > HISTORY_LINE && p_button->GetID() == EXTEND || p_button->GetID() == m_chat)
	{
		m_chat_spk.Blt(p_button->x, p_button->y, p_button->m_image_index +CHAT_BUTTON_COUNT);
	}

	if (p_button->GetFocusState())
	{
		g_descriptor_manager.Set(DID_HELP, p_button->x, p_button->y, (void *)&g_chat_help_string[p_button->GetID()-0x10000], 2);

		if(p_button->GetPressState())
		{
			m_chat_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
		}
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_CHAT::Run by larosel
//
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_CHAT::Run(id_t id)
{
	switch (id)
	{
	case EXTEND:
		gC_vs_ui.HotKey_ExtendChat();
		break;

	case CHAT:
		m_chat = CHAT;
		gC_vs_ui.SetGuildChat(false);
		break;

	case ZONE:
		m_chat = ZONE;
		gC_vs_ui.SetGuildChat(false);
		if(m_bl_whisper)
		{
			m_bl_whisper = false;
			gC_vs_ui.SetSlayerWhisper(false);
		}
		break;

	case GUILD:
//		m_chat = GUILD;
//		gC_vs_ui.SetGuildChat(true);
//		gC_vs_ui.SetSlayerWhisper(false);
		break;

	case WHISPER:
		if(m_chat != GUILD)m_chat = CHAT;
		m_bl_whisper = !m_bl_whisper;
		gC_vs_ui.SetSlayerWhisper(m_bl_whisper);
		break;
	}
}


//-----------------------------------------------------------------------------
// Start by larosel
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_CHAT::Start()
{
//	PI_Processor::Start();

//	gpC_window_manager->AppearWindow(this);

}

//-----------------------------------------------------------------------------
// Process by larosel
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_CHAT::Process()
{

}

//-----------------------------------------------------------------------------
// KeyboardControl by larosel
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_CHAT::KeyboardControl(UINT message, UINT key, long extra)
{

}

//-----------------------------------------------------------------------------
// UnacquireMouseFocus by larosel
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_CHAT::UnacquireMouseFocus()
{
}

//-----------------------------------------------------------------------------
// CancelPushState by larosel
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_CHAT::CancelPushState()
{
}

//-----------------------------------------------------------------------------
//- MouseControl by larosel
//-
//-----------------------------------------------------------------------------
bool C_VS_UI_SLAYER_CHAT::MouseControl(UINT message, int _x, int _y)
{
//	Window::MouseControl(message, _x, _y);

	switch (message)
	{
		case M_MOVING:
			break;

		case M_LEFTBUTTON_DOWN:
		case M_LB_DOUBLECLICK:
			break;

		case M_LEFTBUTTON_UP:
			break;
	}

	return true;
}
*/
//----------------------------------------------------------------------------
// Operations
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Click
//
// ���������� true��, �׷��������� false�� ��ȯ�Ѵ�.
//-----------------------------------------------------------------------------
bool C_VS_UI_SLAYER_QUICKITEM::Click()
{
	if (m_focus_slot == NOT_SELECTED || gbl_item_lock || 
		 g_pQuickSlot == NULL)
	{
		return false;
	}

	int item_x, item_y;

	if (gpC_mouse_pointer->GetPickUpItem()) // ��� �ִ°�?
	{
		const MItem* p_cur_item = g_pQuickSlot->GetItem(m_focus_slot);

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

			if (g_pQuickSlot->CanReplaceItem(gpC_mouse_pointer->GetPickUpItem(),
												  m_focus_slot,
												  p_old_item))
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
		const MItem * p_item = g_pQuickSlot->GetItem(m_focus_slot);

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

			g_pQuickSlot->RemoveItem((BYTE)m_focus_slot);

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
void C_VS_UI_SLAYER_QUICKITEM::Use(int slot, bool bFunctionKey)
{
	if (slot == NOT_SELECTED || gbl_item_lock || g_pQuickSlot == NULL)
		return;

	if (gpC_mouse_pointer->GetPickUpItem())
	{

	}
	else
	{
		const MItem * p_item = g_pQuickSlot->GetItem(slot);

		if (p_item) // Item�� �ִ�.
		{
			gpC_base->SendMessage(UI_ITEM_USE_QUICKSLOT,
									  slot,
									  bFunctionKey, 
									  (MItem *)p_item);
		}
	}
}

/*-----------------------------------------------------------------------------
- C_VS_UI_SLAYER_QUICKITEM
-
-----------------------------------------------------------------------------*/
C_VS_UI_SLAYER_QUICKITEM::C_VS_UI_SLAYER_QUICKITEM(const char* spkPath)
{
	g_RegisterWindow(this);

	AttrPin(true);

	m_image_spk.Open(spkPath);

	// ���̴� ó�� no belt�� ����̴�. belt�� �����ϸ� �ٲ��.
	// ���̴� �����ϴ�.
	Set(0, 434+g_pUserInformation->iResolution_y-600 , m_image_spk.GetWidth(END_EDGE), m_image_spk.GetHeight(END_EDGE));

//#ifdef CONVERT_1024_768
//	Set(0, 434+168 , m_image_spk.GetWidth(END_EDGE), m_image_spk.GetHeight(END_EDGE));
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
- ~C_VS_UI_SLAYER_QUICKITEM
-
-----------------------------------------------------------------------------*/
C_VS_UI_SLAYER_QUICKITEM::~C_VS_UI_SLAYER_QUICKITEM()
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
// C_VS_UI_SLAYER_QUICKITEM::ToggleBelt
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_QUICKITEM::ToggleBelt()
{
//	m_bl_opened = !m_bl_opened;

	// reset Window width by open/close
	Run(PUSHPIN_ID);
	ResetSize();

	PlaySound(SOUND_SLAYER_BELT);
}

void	C_VS_UI_SLAYER_QUICKITEM::Open()
{
//	m_bl_opened = true;

	AttrAutoHide(Window::ATTRIBUTES_HIDE_NOT);
	// reset Window width by open/close
	ResetSize();

	PlaySound(SOUND_SLAYER_BELT);
}

void	C_VS_UI_SLAYER_QUICKITEM::Close()
{
//	m_bl_opened = false;
	AttrAutoHide(Window::ATTRIBUTES_HIDE_NOT);
	
	Run(PUSHPIN_ID);

	// reset Window width by open/close
	ResetSize();

	PlaySound(SOUND_SLAYER_BELT);
}

//-----------------------------------------------------------------------------
// ResetSize
//
// belt�� ��ü�ϰ����� size�� �缳���ؾ� �Ѵ�.
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_QUICKITEM::ResetSize()
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
	m_p_slot_x[SLOT_7] = m_p_slot_x[SLOT_6]+slot_size;
	m_p_slot_x[SLOT_8] = m_p_slot_x[SLOT_7]+slot_size;
}

//-----------------------------------------------------------------------------
// GetPocketCount
//
// 
//-----------------------------------------------------------------------------
int C_VS_UI_SLAYER_QUICKITEM::GetPocketCount() const
{
	if (g_pQuickSlot == NULL)
		return 0;

	return g_pQuickSlot->GetPocketNumber();
}

//-----------------------------------------------------------------------------
// WindowEventReceiver
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_QUICKITEM::WindowEventReceiver(id_t event)
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
bool C_VS_UI_SLAYER_QUICKITEM::IsPixel(int _x, int _y)
{
	return IsInRect(_x, _y);
}

//-----------------------------------------------------------------------------
// Start
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_QUICKITEM::Start()
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
	memcpy(m_Hotkey_buf, gpC_vs_ui_window_manager->GetHotKeyQuickSlot(), sizeof(int)*C_VS_UI_SLAYER_QUICKITEM::HOTKEY_MAX);
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
void C_VS_UI_SLAYER_QUICKITEM::Process()
{
	m_pC_button_group->Process();
	ProcessHide();
}

//-----------------------------------------------------------------------------
// KeyboardControl
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_QUICKITEM::KeyboardControl(UINT message, UINT key, long extra)
{

}

//-----------------------------------------------------------------------------
// UnacquireMouseFocus
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_QUICKITEM::UnacquireMouseFocus()
{
	m_pC_button_group->UnacquireMouseFocus();
	m_focus_slot = NOT_SELECTED;
}

/*-----------------------------------------------------------------------------
- MouseControl
-
-----------------------------------------------------------------------------*/
bool C_VS_UI_SLAYER_QUICKITEM::MouseControl(UINT message, int _x, int _y)
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

	if (g_pQuickSlot != NULL) // item selection
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
						
						p_selected_item = g_pQuickSlot->GetItem(m_focus_slot);
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

void C_VS_UI_SLAYER_QUICKITEM::CancelPushState()
{
	m_pC_button_group->CancelPushState();
}

void C_VS_UI_SLAYER_QUICKITEM::Run(id_t id)
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

void	C_VS_UI_SLAYER_QUICKITEM::ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button)
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

void	C_VS_UI_SLAYER_QUICKITEM::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
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
void C_VS_UI_SLAYER_QUICKITEM::Show()
{
	CT_Done("WnSh: C_VS_UI_SLAYER_QUICKITEM::Show ENTER pthis=%p", (void*)this);
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
			p_selected_item = g_pQuickSlot->GetItem(m_focus_slot);
		
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
			
			const MItem * p_item = g_pQuickSlot->GetItem(i);
			
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
					if (g_pQuickSlot->CanReplaceItem(p_item, m_focus_slot, p_old_item))
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
	char * p_hotkey_mark[] = {"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8" , "F9" , "F10" ,"F11","F12"};
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

		// ������ �ڽ� ���..
//		if (gpC_base->m_p_DDSurface_back->Lock())

		int	ifocusHotKey = m_Hotkey_buf[m_focus_slot]  ;

		if(ifocusHotKey >= 0)
		{
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
			//int	ifocusHotKey = m_Hotkey_buf[m_focus_slot]  ;

			//if(ifocusHotKey >= 0)
			g_PrintColorStr(point.x, point.y+18, p_hotkey_mark[ifocusHotKey], gpC_base->m_item_desc_pi, markColor);	
		}

		//g_PrintColorStr(point.x, point.y+18, p_hotkey_mark[m_focus_slot], gpC_base->m_item_desc_pi, markColor);	
	}

#ifndef _LIB
	/*
	//
	// focus slot rect -- Test
	//
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		S_SURFACEINFO	surface_info;
		SetSurfaceInfo(&surface_info, gpC_base->m_p_DDSurface_back->GetDDSD());

		if (m_focus_slot != NOT_SELECTED)
		{
			S_RECT rect;
			SetRect(rect, m_p_slot_x[m_focus_slot], QUICK_ITEM_Y,
							  QUICKSLOT_W, QUICKSLOT_H);
			filledRect(&surface_info, &rect, 0xFF);
		}

		gpC_base->m_p_DDSurface_back->Unlock();
	}
	*/
#endif
}


void	C_VS_UI_SLAYER_QUICKITEM::SetHotkey(HOTKEY hotkey , int slot)
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

void	C_VS_UI_SLAYER_QUICKITEM::InitHotKey(HOTKEY hotkey)
{
	for (int i=0; i < HOTKEY_MAX; i++)
	{
		if(m_Hotkey_buf[i] == hotkey)
				m_Hotkey_buf[i] = NOT_SELECTED;
	}
}

int	C_VS_UI_SLAYER_QUICKITEM::GetFocusedSlot()
{
	return	m_focus_slot	;
}

int	C_VS_UI_SLAYER_QUICKITEM::GetHotkey(int	slot)
{
	return m_Hotkey_buf[slot] ; 
}


int	C_VS_UI_SLAYER_QUICKITEM::GetSlot(HOTKEY hotkey)

{
	int slot = 0 ; 
	for (int i=0; i < HOTKEY_MAX; i++)
	{
		if(m_Hotkey_buf[i] == hotkey)
			return i ; 
			
	}

	return slot; 
}


/*
//-----------------------------------------------------------------------------
// Run
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_PDS::Run(id_t id)
{
	switch (id)
	{
		case PDS_CLOSE_ID:
			ControlPDS(NO_PDS_MENU);
			break;

		// pcs
		case PCS_SEL_ID:
			break;

		case PCS_MEM_ID:
			break;

		case PCS_DEL_ID:
			break;

		case PCS_END_ID:
			if (m_pcs_state == PCS_CONNECTING || m_pcs_state == PCS_ON_LINE)
			{
				AddToPCSHistory("<!> connecting aborted.");
				m_pcs_state = PCS_OFF_LINE;

				gpC_base->SendMessage(UI_END_PCS);
			}

			m_lev_pcs_number.EraseAll();
			break;

		case PCS_SEND_ID:
			if (m_lev_pcs_number.Size() > 0)
			{
				char * p_temp = NULL;
				g_Convert_DBCS_Ascii2SingleByte(m_lev_pcs_number.GetString(), m_lev_pcs_number.Size(), p_temp);

				// check valid PCS number
				char error_buf[150];
				if (m_lev_pcs_number.Size() != PCS_NUMBER_LIMIT)
				{
#ifdef _ENG_VERSION
					sprintf(error_buf, "%s%s%s", "<Error!> \"", p_temp, "\" is a invalid PCS number.");
					AddToPCSHistory(error_buf);
#else
					sprintf(error_buf, "%s%s%s", "<����!> \"", p_temp, "\"�� �߸��� PCS��ȣ�Դϴ�.");
					AddToPCSHistory(error_buf);
#endif
				}
				else
				{
					AddToPCSHistory(p_temp);

					// valid number !
					AddToPCSHistory("dialing...");
					AddToPCSHistory("connecting...");

					m_pcs_state = PCS_CONNECTING;

					char * p_temp = NULL;
					
					g_Convert_DBCS_Ascii2SingleByte(m_lev_pcs_number.GetString(), m_lev_pcs_number.Size(), p_temp);
					m_last_dialing_pcs_num = atoi(p_temp);
					DeleteNewArray(p_temp);

					gpC_base->SendMessage(UI_SEND_PCS_NUMBER, m_last_dialing_pcs_num);
				}

				DeleteNewArray(p_temp);
				m_lev_pcs_number.EraseAll();
			}
			break;

		case PCS_HISTORY_SCROLLUP_ID:
			m_pcs_scrollbar.IncreaseAmountToScroll();
			break;

		case PCS_HISTORY_SCROLLDOWN_ID:
			m_pcs_scrollbar.DecreaseAmountToScroll();
			break;

		case PCS_BACKSPACE_ID:
			KeyboardControl(WM_KEYDOWN, VK_BACK, 0);	
			break;

		case PCS_0_ID:
			KeyboardControl(WM_CHAR, '0', 0);			
			break;

		case PCS_1_ID:
			KeyboardControl(WM_CHAR, '1', 0);			
			break;

		case PCS_2_ID:
			KeyboardControl(WM_CHAR, '2', 0);	
			break;

		case PCS_3_ID:
			KeyboardControl(WM_CHAR, '3', 0);	
			break;

		case PCS_4_ID:
			KeyboardControl(WM_CHAR, '4', 0);	
			break;

		case PCS_5_ID:
			KeyboardControl(WM_CHAR, '5', 0);	
			break;

		case PCS_6_ID:
			KeyboardControl(WM_CHAR, '6', 0);	
			break;

		case PCS_7_ID:
			KeyboardControl(WM_CHAR, '7', 0);	
			break;

		case PCS_8_ID:
			KeyboardControl(WM_CHAR, '8', 0);	
			break;

		case PCS_9_ID:
			KeyboardControl(WM_CHAR, '9', 0);	
			break;

		case PCS_PHONE_1_ID:
			if (m_pcs_wait_room[0].bl_set == true)
			{
				if (m_pcs_wait_room[0].bl_granted == false)
				{
					//GrantPCSConnection(0);

					gpC_base->SendMessage(UI_PLEASE_PCS_CONNECT_ME, m_pcs_wait_room[0].pcs_number, 0);
				}
				else
				{
					gpC_base->SendMessage(UI_CHANGE_PCS_CONNECTED_SLOT, m_pcs_wait_room[0].pcs_number, 0);
				}
			}
			break;

		case PCS_PHONE_2_ID:
			if (m_pcs_wait_room[1].bl_set == true)
			{
				if (m_pcs_wait_room[1].bl_granted == false)
				{
					//	GrantPCSConnection(1);

					gpC_base->SendMessage(UI_PLEASE_PCS_CONNECT_ME, m_pcs_wait_room[1].pcs_number, 1);
				}
				else
				{
					gpC_base->SendMessage(UI_CHANGE_PCS_CONNECTED_SLOT, m_pcs_wait_room[0].pcs_number, 1);
				}
			}
			break;

		case PCS_PHONE_3_ID:
			if (m_pcs_wait_room[2].bl_set == true)
			{
				if (m_pcs_wait_room[2].bl_granted == false)
				{
					//GrantPCSConnection(2);

					gpC_base->SendMessage(UI_PLEASE_PCS_CONNECT_ME, m_pcs_wait_room[2].pcs_number, 2);
				}
				else
				{
					gpC_base->SendMessage(UI_CHANGE_PCS_CONNECTED_SLOT, m_pcs_wait_room[0].pcs_number, 2);
				}
			}
			break;

		case PCS_QUIT_ID:
			OffLinePCS();

			gpC_base->SendMessage(UI_QUIT_PCS_ONLINE_MODE);
			break;
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_SLAYER_PDS::C_VS_UI_SLAYER_PDS()
{
	m_pds_button_spk.Open(SPK_PDS_BUTTON);
	m_pds_pcs_spk.Open(SPK_PDS_PCS);
	m_pds_sub_spk.Open(SPK_PDS_SUB);

	m_pC_common_button_group = new ButtonGroup(this);

	m_pC_common_button_group->Add(new C_VS_UI_EVENT_BUTTON(510, 564, 47, 25, PDS_CLOSE_ID, this, CLOSE_PUSHED_BUTTON));

	m_pC_pcs_button_group = new ButtonGroup(this);

	m_pC_pcs_button_group2 = new ButtonGroup(this);

	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(474, 512, 31, 25, PCS_SEL_ID, this, PCS_SEL));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(474, 538, 31, 25, PCS_MEM_ID, this, PCS_MEM));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(474, 564, 31, 25, PCS_DEL_ID, this, PCS_DEL));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(510, 512, 47, 25, PCS_END_ID, this, PCS_END));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(510, 538, 47, 25, PCS_SEND_ID, this, PCS_SEND));

	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(373, 511, 31, 19, PCS_1_ID, this, PCS_1));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(405, 511, 31, 19, PCS_2_ID, this, PCS_2));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(437, 511, 31, 19, PCS_3_ID, this, PCS_3));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(373, 531, 31, 19, PCS_4_ID, this, PCS_4));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(405, 531, 31, 19, PCS_5_ID, this, PCS_5));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(437, 531, 31, 19, PCS_6_ID, this, PCS_6));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(373, 551, 31, 19, PCS_7_ID, this, PCS_7));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(405, 551, 31, 19, PCS_8_ID, this, PCS_8));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(437, 551, 31, 19, PCS_9_ID, this, PCS_9));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(405, 571, 31, 19, PCS_0_ID, this, PCS_0));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(437, 571, 31, 19, PCS_BACKSPACE_ID, this, PCS_BACKSPACE));

	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(352, 527, PDS_SCROLLBUTTON_W, PDS_SCROLLBUTTON_H, PCS_HISTORY_SCROLLUP_ID, this, SCROLLUP_PUSHED_BUTTON));
	m_pC_pcs_button_group->Add(new C_VS_UI_EVENT_BUTTON(352, 587, PDS_SCROLLBUTTON_W, PDS_SCROLLBUTTON_H, PCS_HISTORY_SCROLLDOWN_ID, this, SCROLLDOWN_PUSHED_BUTTON));

	// 2
	m_pC_pcs_button_group2->Add(new C_VS_UI_EVENT_BUTTON(PCS_ICON_X, PCS_PHNE1_Y, 13, 13, PCS_PHONE_1_ID, this, PCS_PHONE_PUSHED));
	m_pC_pcs_button_group2->Add(new C_VS_UI_EVENT_BUTTON(PCS_ICON_X, PCS_PHNE2_Y, 13, 13, PCS_PHONE_2_ID, this, PCS_PHONE_PUSHED));
	m_pC_pcs_button_group2->Add(new C_VS_UI_EVENT_BUTTON(PCS_ICON_X, PCS_PHNE3_Y, 13, 13, PCS_PHONE_3_ID, this, PCS_PHONE_PUSHED));
	m_pC_pcs_button_group2->Add(new C_VS_UI_EVENT_BUTTON(PCS_ICON_X, PCS_QUIT_Y, 13, 15, PCS_QUIT_ID, this, PCS_QUIT_PUSHED));

	m_pds_menu = NO_PDS_MENU;

	m_pcs_history_input_index = 0;
	m_pcs_history_start_index = 0;
	m_pcs_state = PCS_OFF_LINE;
	m_pcs_who_connection = NOT_SELECTED;
	m_bl_ring = false;

	int i;
	for (int i=0; i < PCS_HISTORY_SIZE; i++)
		m_pcs_history_buf[i] = NULL;

	for (int i=0; i < PCS_WAIT_ROOM_MAX; i++)
	{
		m_pcs_wait_room[i].sz_name = NULL;
		m_pcs_wait_room[i].pcs_number = 0;
		m_pcs_wait_room[i].bl_set = false;
		m_pcs_wait_room[i].bl_granted = false;
	}

	m_backup_room.sz_name = NULL;
	m_backup_room.pcs_number = 0;
	// m_backup_room���� �� �� �ٸ� �͵��� ���� �ʴ´�.

#ifdef _LIB

//	m_str = 0;
//	m_dex = 0;
//	m_int = 0;
//	m_hp = 0;
//	m_mp = 0;
//	m_th = 0;
//	m_ac = 0;
//	m_cc = 0;
//
//	m_str_max = 0;
///	m_dex_max = 0;
//	m_int_max = 0;
//	m_hp_max = 0;
//	m_mp_max = 0;
//	m_th_max = 0;
//	m_ac_max = 0;
//	m_cc_max = 0;

	m_pcs_number = 0;
#endif

	m_lev_pcs_number.SetInputCharCount(PCS_NUMBER_LIMIT);
	m_lev_pcs_number.SetPosition(PCS_MESSAGE_X, 579);
	m_lev_pcs_number.SetCursorColor(RGB(128, 128, 128));
	m_lev_pcs_number.SetPrintInfo(gpC_base->m_chatting_pi);

	m_last_dialing_pcs_num = 0;
}

//-----------------------------------------------------------------------------
// ~C_VS_UI_SLAYER_PDS
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_SLAYER_PDS::~C_VS_UI_SLAYER_PDS()
{
	DeleteNew(m_pC_common_button_group);
	DeleteNew(m_pC_pcs_button_group);
	DeleteNew(m_pC_pcs_button_group2);

	int i;
	for (int i=0; i < PCS_HISTORY_SIZE; i++)
		DeleteNewArray(m_pcs_history_buf[i]);

	for (int i=0; i < PCS_WAIT_ROOM_MAX; i++)
		DeleteNewArray(m_pcs_wait_room[i].sz_name);
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::GetCurrentChatting
//
// 
//-----------------------------------------------------------------------------
const C_VS_UI_CHATTING * C_VS_UI_SLAYER_PDS::GetCurrentChatting() const
{
	if (m_pcs_who_connection != NOT_SELECTED)
		return &m_pcs_chatting[m_pcs_who_connection];

	return NULL;
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::AddChatToPCShistory
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PDS::AddChatToPCShistory(char * str, int pcs_slot)
{
	if (pcs_slot >= 0 && pcs_slot < PCS_WAIT_ROOM_MAX)
	{
		m_pcs_chatting[pcs_slot].AddChatToHistory(str, NULL, CLD_NORMAL);
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::SetSlot
//
// slot�� ������ set�Ѵ�.
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PDS::SetSlot(char * sz_name, long pcs_number, int slot)
{
	if (slot >= 0 && slot < PCS_WAIT_ROOM_MAX)
	{
		m_pcs_wait_room[slot].pcs_number = pcs_number;

		DeleteNewArray(m_pcs_wait_room[slot].sz_name);
		if (sz_name != NULL)
		{
			m_pcs_wait_room[slot].sz_name = new char[strlen(sz_name)+1];
			strcpy(m_pcs_wait_room[slot].sz_name, sz_name);
		}
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::AcquireQuitPCSOnlineModeMessage
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PDS::AcquireQuitPCSOnlineModeMessage()
{
	m_lev_pcs_number.Acquire();
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::ShowConnectorInfo
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PDS::ShowConnectorInfo(int _x, int _y, S_PCS_WAIT_ROOM &pcs_wait_room)
{
	assert(pcs_wait_room.bl_set == true);

	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		const _BOX_H = 40;
		S_SURFACEINFO	surfaceinfo;
		SetSurfaceInfo(&surfaceinfo, gpC_base->m_p_DDSurface_back->GetDDSD());
		Rect rect(_x, _y-_BOX_H, 120, _BOX_H);

		FillRect(&surfaceinfo, &rect, WHITE);
		rectangle(&surfaceinfo, &rect, GREEN);

		gpC_base->m_p_DDSurface_back->Unlock();

		char sz_buf[100];
		sprintf(sz_buf, "PCS #%d", pcs_wait_room.pcs_number);
		g_Print(rect.x+2, rect.y+2, sz_buf);

		if (pcs_wait_room.sz_name != NULL)
		{
			sprintf(sz_buf, "Name: %s", pcs_wait_room.sz_name);
			g_Print(rect.x+2, rect.y+2+16, sz_buf);
		}
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::ConnectPCS
//
// ���濡 ����.
//
// empty wait room�� ���������� ���� room number�� ��ȯ�ϰ�, �����ϸ�
// -1�� ��ȯ�Ѵ�.
//-----------------------------------------------------------------------------
int	C_VS_UI_SLAYER_PDS::ConnectPCS(char * sz_name, long pcs_number, int slot)
{
	if (slot >= 0 && slot < PCS_WAIT_ROOM_MAX)
	{
		if (m_pcs_wait_room[slot].bl_set == false)
		{
			// ok, come in
			m_pcs_wait_room[slot].bl_set = true;
			m_pcs_wait_room[slot].bl_granted = false;
			SetSlot(sz_name, pcs_number, slot);

			return slot;
		}
	}

	return -1;
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::PushToBackupRoom
//
// PCS ���°� �ƴ� ���� ��ٷ� wait room�� �� �� ����.
// �ϴ� ���⿡ backup�صΰ� ����ڰ� PCS�� ��ȯ�Ͽ��� �� wait room���� ������.
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_PDS::PushToBackupRoom(char * sz_name, long pcs_number, int slot)
{
	m_backup_room.pcs_number = pcs_number;
	DeleteNewArray(m_backup_room.sz_name);

	if (sz_name != NULL)
	{
		m_backup_room.sz_name = new char[strlen(sz_name)+1];
		strcpy(m_backup_room.sz_name, sz_name);
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::GrantPCSConnection
//
// PCS ������ �³��Ѵ�.
//
// - room_n�� ����� �̹� ���� �־�� �Ѵ�.
// - �����ϸ� true�� ��ȯ�Ѵ�.
//-----------------------------------------------------------------------------
bool	C_VS_UI_SLAYER_PDS::GrantPCSConnection(int room_n)
{
	if (room_n >= 0 && room_n < PCS_WAIT_ROOM_MAX)
	{
		if (m_pcs_wait_room[room_n].bl_set == true)
		{
			// �̹� ����Ǿ��ִ� slot�ΰ�?
			if (m_pcs_wait_room[room_n].bl_granted == true)
			{
			}
			else
			{
				m_pcs_wait_room[room_n].bl_granted = true;
			}

			m_pcs_who_connection = room_n;
			
			m_pcs_chatting[m_pcs_who_connection].Start();
			m_pcs_chatting[m_pcs_who_connection].SlayerPCSMode(true);

			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::DisconnectPCS
//
// wait room���� �����Ѵ�.
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PDS::DisconnectPCS(int room_n)
{
	assert(room_n >= 0 && room_n < PCS_WAIT_ROOM_MAX);

	if (room_n >= 0 && room_n < PCS_WAIT_ROOM_MAX)
	{
		m_pcs_wait_room[room_n].bl_set = false;
		m_pcs_wait_room[room_n].bl_granted = false;
		m_pcs_wait_room[room_n].pcs_number = 0;
		DeleteNewArray(m_pcs_wait_room[room_n].sz_name);

		if (room_n == m_pcs_who_connection)
			m_pcs_who_connection = NOT_SELECTED;
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::OnLinePCS
//
// PCS�� ����Ǿ���.
//
// - �ڽ��� dialing�Ͽ� ����Ǿ��� ����.
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PDS::OnLinePCS(char * sz_name, long pcs_number)
{
	AddToPCSHistory("connected.");

	m_pcs_state = PCS_ON_LINE;

	// ��... ����Ǹ� �翬�� ù��° �������..?
	int room_n = ConnectPCS(sz_name, pcs_number, 0);
	if (room_n != -1)
	{
		GrantPCSConnection(room_n);
		m_pcs_who_connection = room_n;
		m_pcs_chatting[m_pcs_who_connection].Start();
		m_pcs_chatting[m_pcs_who_connection].SlayerPCSMode(true);
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::ConnectionFailed
//
// �������!
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_PDS::ConnectionFailed()
{
	AddToPCSHistory("<!> no response. try again.");
	m_pcs_state = PCS_OFF_LINE;
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::OffLinePCS
//
// PCS ������ ��������.
//
// - ������ ���������� PCS ������ ������.
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PDS::OffLinePCS()
{
	AddToPCSHistory("connection terminated.");

	m_pcs_state = PCS_OFF_LINE;

	DisconnectPCS(0);
	DisconnectPCS(1);
	DisconnectPCS(2);
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::AddToPCSHistory
//
//
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PDS::AddToPCSHistory(char * sz_str)
{
	assert(sz_str != NULL);

	if (sz_str == NULL)
		return;

	DeleteNewArray(m_pcs_history_buf[m_pcs_history_input_index]);

	m_pcs_history_buf[m_pcs_history_input_index] = new char[strlen(sz_str)+1];
	strcpy(m_pcs_history_buf[m_pcs_history_input_index], sz_str);

	m_pcs_history_input_index++;
	if (m_pcs_history_input_index == PCS_HISTORY_SIZE)
		m_pcs_history_input_index = 0;

	m_pcs_scrollbar.SetAmountForScroll(GetPCSHistorySizeForScroll());
	m_pcs_scrollbar.SetAmountToScroll(0); // �ʱ���·�.
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::GetPCSHistorySizeForScroll
//
// scroll��(���� �������� �ʴ�) line ���� ��´�.
//-----------------------------------------------------------------------------
int C_VS_UI_SLAYER_PDS::GetPCSHistorySizeForScroll() const
{
	int total = 0;

	for (int i = 0; i < PCS_HISTORY_SIZE; i++)
	{
		if (m_pcs_history_buf[i])
			total++;
	}

	int size = total - PCS_HISTORY_LINE;
	if (size < 0)
		size = 0;

	return size;
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::ControlPDS
//
// pds menu�� �ٲ۴�.
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PDS::ControlPDS(PDS_MENU pds_menu) 
{ 
	// toggle�ǵ���.
	if (m_pds_menu == pds_menu)
		m_pds_menu = NO_PDS_MENU;
	else
		m_pds_menu = pds_menu;

	switch (m_pds_menu)
	{
		
//		case GAGE:
//			gpC_base->SendMessage(UI_PLEASE_SET_SLAYER_VALUE);
//			break;

		case PCS:
			m_lev_pcs_number.Acquire();
			break;

		case NO_PDS_MENU:
			gpC_base->SendMessage(UI_PDS_CLOSED);
			break;
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::CancelPushState
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_PDS::CancelPushState()
{
	m_pC_common_button_group->CancelPushState();
	m_pC_pcs_button_group->CancelPushState();
	m_pC_pcs_button_group2->CancelPushState();
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::UnacquireMouseFocus
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_PDS::UnacquireMouseFocus()
{
	m_pC_common_button_group->UnacquireMouseFocus();
	m_pC_pcs_button_group->UnacquireMouseFocus();
	m_pC_pcs_button_group2->UnacquireMouseFocus();
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PDS::ShowButtonWidget
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PDS::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
	if (p_button->GetID() == PDS_CLOSE_ID)
	{
		if (p_button->GetFocusState() && p_button->GetPressState())
			m_pds_sub_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);

		return;
	}

	switch (m_pds_menu)
	{
		case PCS:
			if (m_pcs_state == PCS_ON_LINE)
			{
				if (p_button->GetID() == PCS_QUIT_ID)
				{
					if (p_button->GetFocusState() && p_button->GetPressState())
						m_pds_pcs_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
				}
				else
				{
					//
					// mark wait room
					//
					// wait room�� ������ blink�ϰ�, granted�Ǿ����� ���ǥ���Ѵ�.
					//
					switch (p_button->GetID())
					{
						case PCS_PHONE_1_ID:
							if (m_pcs_wait_room[0].bl_set)
							{
								if (m_pcs_wait_room[0].bl_granted)
								{
									m_pds_pcs_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
								}
								else
								{
									if (g_blink_value%3 == 0)
										m_pds_pcs_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
								}

								if (p_button->GetFocusState())
									ShowConnectorInfo(p_button->x, p_button->y, m_pcs_wait_room[0]);
							}
							break;

						case PCS_PHONE_2_ID:
							if (m_pcs_wait_room[1].bl_set)
							{
								if (m_pcs_wait_room[1].bl_granted)
								{
									m_pds_pcs_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
								}
								else
								{
									if (g_blink_value%3 == 0)
										m_pds_pcs_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
								}


								if (p_button->GetFocusState())
									ShowConnectorInfo(p_button->x, p_button->y, m_pcs_wait_room[1]);
							}
							break;

						case PCS_PHONE_3_ID:
							if (m_pcs_wait_room[2].bl_set)
							{
								if (m_pcs_wait_room[2].bl_granted)
								{
									m_pds_pcs_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
								}
								else
								{
									if (g_blink_value%3 == 0)
										m_pds_pcs_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
								}

								if (p_button->GetFocusState())
									ShowConnectorInfo(p_button->x, p_button->y, m_pcs_wait_room[2]);
							}
							break;
					}
				}
			}
			else
			{
				switch (p_button->GetID())
				{
					case PCS_HISTORY_SCROLLUP_ID:
					case PCS_HISTORY_SCROLLDOWN_ID:
						if (p_button->GetFocusState() && p_button->GetPressState())
							m_pds_sub_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
						break;

					default:
						if (p_button->GetFocusState() && p_button->GetPressState())
							m_pds_pcs_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
						break;
				}
			}
			break;
	}
}

//-----------------------------------------------------------------------------
// ShowButton
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PDS::ShowButton(C_VS_UI_EVENT_BUTTON * p_button)
{
	if (p_button->GetID() == m_pds_menu || (p_button->GetFocusState() && p_button->GetPressState()))
	{
		m_pds_button_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
	}

	if (p_button->GetID() == PCS)
		if (m_bl_ring)
			if (g_blink_value%3 == 0)
				m_pds_button_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
}

//-----------------------------------------------------------------------------
// KeyboardControl
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_PDS::KeyboardControl(UINT message, UINT key, long extra)
{
	switch (m_pds_menu)
	{
		case PCS:
			{
				if (m_pcs_state == PCS_ON_LINE)
				{
					switch (m_pcs_who_connection)
					{
						case 0:
							m_pcs_chatting[0].KeyboardControl(message, key, extra);
							break;

						case 1:
							m_pcs_chatting[1].KeyboardControl(message, key, extra);
							break;

						case 2:
							m_pcs_chatting[2].KeyboardControl(message, key, extra);
							break;				
					}
				}
				else
				{
					if (m_pcs_state == PCS_OFF_LINE)
					{
						// digit, return, backspace only

						if (message == WM_CHAR)
							if (key >= '0' && key <= '9')
								gC_ci.IME_MessageProcessor(message, key, extra);

						if (message == WM_KEYDOWN)
							switch (key)
							{
								case VK_RETURN:
									Run(PCS_SEND_ID);

									// here no 'break'
								case VK_BACK:
									gC_ci.IME_MessageProcessor(message, key, extra);
							}
					}
				}
			}
			break;
	}
}

//-----------------------------------------------------------------------------
// MouseControl
//
// 
//-----------------------------------------------------------------------------
bool C_VS_UI_SLAYER_PDS::MouseControl(UINT message, int _x, int _y)
{
	if (m_pds_menu == NO_PDS_MENU)
		return true;

	m_pC_common_button_group->MouseControl(message, _x, _y);

	switch (m_pds_menu)
	{
		case PCS:
			if (m_pcs_state == PCS_ON_LINE)
			{
				m_pC_pcs_button_group2->MouseControl(message, _x, _y);

				switch (m_pcs_who_connection)
				{
					case 0:
						m_pcs_chatting[0].MouseControl(message, _x, _y);
						break;

					case 1:
						m_pcs_chatting[1].MouseControl(message, _x, _y);
						break;

					case 2:
						m_pcs_chatting[2].MouseControl(message, _x, _y);
						break;
				}
			}
			else
			{
				m_pC_pcs_button_group->MouseControl(message, _x, _y);

				switch (message)
				{
					case M_WHEEL_UP:
						Run(PCS_HISTORY_SCROLLUP_ID);
						break;

					case M_WHEEL_DOWN:
						Run(PCS_HISTORY_SCROLLDOWN_ID);
						break;
				}
			}
			break;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Show
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_PDS::Show()
{
	CT_Done("WnSh: C_VS_UI_SLAYER_PDS::Show ENTER pthis=%p", (void*)this);
	if (m_pds_menu == NO_PDS_MENU)
		return;

	switch (m_pds_menu)
	{
		case PCS:
			{
				if (m_pcs_state == PCS_ON_LINE)
				{
					m_pds_pcs_spk.Blt(PCS_ICON_X+14, y, PCS_ICON_BOUNDARY);
					m_pds_pcs_spk.Blt(PCS_ICON_X, PCS_PHNE1_Y, PCS_PHONE);
					m_pds_pcs_spk.Blt(PCS_ICON_X, PCS_PHNE2_Y, PCS_PHONE);
					m_pds_pcs_spk.Blt(PCS_ICON_X, PCS_PHNE3_Y, PCS_PHONE);
					m_pds_pcs_spk.Blt(PCS_ICON_X, PCS_QUIT_Y, PCS_QUIT);

					switch (m_pcs_who_connection)
					{
						case 0:
							m_pds_pcs_spk.Blt(PCS_ICON_X+6, PCS_PHNE1_Y+8, PCS_CONNECTION_MARK);
							m_pcs_chatting[0].Show();
							break;

						case 1:
							m_pds_pcs_spk.Blt(PCS_ICON_X+6, PCS_PHNE2_Y+8, PCS_CONNECTION_MARK);
							m_pcs_chatting[1].Show();
							break;

						case 2:
							m_pds_pcs_spk.Blt(PCS_ICON_X+6, PCS_PHNE3_Y+8, PCS_CONNECTION_MARK);
							m_pcs_chatting[2].Show();
							break;
					}

					m_pC_pcs_button_group2->Show();
				}
				else
				{
					m_pds_pcs_spk.Blt(x, y, PCS_WINDOW);

					m_pC_pcs_button_group->Show();

					// pcs number
					char pcs_number_buf[10];
					sprintf(pcs_number_buf, "%d", m_pcs_number);
					g_Print(x+17, y, pcs_number_buf, &gpC_base->m_value2_pi);

					if (m_pcs_state == PCS_OFF_LINE)
						m_lev_pcs_number.Show();

					// history
					for (int i=0; i < PCS_HISTORY_LINE; i++)
					{
						int index = m_pcs_history_input_index-m_pcs_scrollbar.GetAmountToScroll();
						index -= i+1;
						if (index < 0)
							index += PCS_HISTORY_SIZE;

						char * sz_str = m_pcs_history_buf[index];
						if (sz_str != NULL)
							g_Print(PCS_MESSAGE_X, PCS_MESSAGE_Y-(PCS_MESSAGE_FONT_GAP*i), sz_str, &gpC_base->m_chatting_pi);
					}
				}
			}
			break;
	}

	m_pC_common_button_group->Show();

	
//#ifndef _LIB
//	if (gpC_base->m_p_DDSurface_back->Lock())
//	{
//		S_SURFACEINFO	surfaceinfo;
//		SetSurfaceInfo(&surfaceinfo, gpC_base->m_p_DDSurface_back->GetDDSD());
//
//		FillRect(&surfaceinfo, this, BLUE);
//
//		gpC_base->m_p_DDSurface_back->Unlock();
//	}
#endif

	
}
*/
/*-----------------------------------------------------------------------------
- C_VS_UI_SLAYER_GEAR
-
-----------------------------------------------------------------------------*/

C_VS_UI_SLAYER_GEAR::C_VS_UI_SLAYER_GEAR()
{
	assert(MSlayerGear::MAX_GEAR_SLAYER == SLOT_SIZE);
	 
//	if(g_char_slot_ingame.m_AdvancementLevel == 0 ) // 2�� ���� 
////	if(g_char_slot_ingame.m_AdvancementLevel == 0) // 2�� ���� �� ��� �������̽� 
//	{
		m_slot_rect[SN_HELM].Set(146, 46, 66, 66);
		m_slot_rect[SN_NECKLACE].Set(146+12,46+116-43,36, 36);
		m_slot_rect[SN_COAT].Set(146, 46+116, 66, 96);
		m_slot_rect[SN_LEFTHAND].Set(146-56-60+43+160, 46+34+139, 66, 96);
		m_slot_rect[SN_RIGHTHAND].Set(146-56-60+43, 46+34+139, 66, 96);
		m_slot_rect[SN_GLOVE].Set(146-56-60+43, 46+34+139+105, 66, 66);
		m_slot_rect[SN_BELT].Set(146-56-60+43+160, 46+34+139+105, 66, 66);
		m_slot_rect[SN_TROUSER].Set(146, 46+116+104, 66, 96);
		m_slot_rect[SN_LEFTBRACELET].Set(146-56-60+43+30, 46+34+139-43, 36, 36);
		m_slot_rect[SN_RIGHTBRACELET].Set(146-56-60+43+160, 46+34+139-43, 36, 36);

		m_slot_rect[SN_LEFTRING1].Set(146-56-60+43+160, 46+34+139+220, 36, 36);
		m_slot_rect[SN_LEFTRING2].Set(146-56-60+43+160+36, 46+34+139+220, 36, 36);
		m_slot_rect[SN_RIGHTRING1].Set(146-56-60+43-9, 46+34+139+220, 36, 36);
		m_slot_rect[SN_RIGHTRING2].Set(146-56-60+43-9+36, 46+34+139+220, 36, 36);

		m_slot_rect[SN_SHOES].Set(146, 46+116+104+104, 66, 66);
		
		m_slot_rect[SN_COREZAP1].Set(146-56-60+43-9, 46+34+139+220, 36, 36);
		m_slot_rect[SN_COREZAP2].Set(146-56-60+43-9+36, 46+34+139+220, 36, 36);
		m_slot_rect[SN_COREZAP3].Set(146-56-60+43+160, 46+34+139+220, 36, 36);
		m_slot_rect[SN_COREZAP4].Set(146-56-60+43+160+36, 46+34+139+220, 36, 36);
		
		m_slot_rect[SN_PDA].Set(146-56-60, 46+34, 66, 66);
		m_slot_rect[SN_SHOULDER].Set(146+56+60, 46+34, 66, 66);
		
		// nanomech 2006.03.09 Item ���� �۾� start
		m_slot_rect[SN_NECK_CHAIN].Set(146+12,46+116-43,36, 36);			// ���׸��� ü��
		// nanomech 2006.03.09 Item ���� �۾� end

		m_slot_rect[SN_CUEOFADAM1].Set(146-56-60+43+30, 46+34+139-43, 36, 36);
		m_slot_rect[SN_CUEOFADAM2].Set(146-56-60+43+160, 46+34+139-43, 36, 36);


		m_slot_rect[SN_BLOODBIBLE1].Set(70, 490, 36, 36);
		m_slot_rect[SN_BLOODBIBLE2].Set(70+36, 490, 36, 36);
		m_slot_rect[SN_BLOODBIBLE3].Set(70+36*2, 490, 36, 36);
		m_slot_rect[SN_BLOODBIBLE4].Set(70+36*3, 490, 36, 36);
		m_slot_rect[SN_BLOODBIBLE5].Set(70+36*4, 490, 36, 36);
		m_slot_rect[SN_BLOODBIBLE6].Set(70+36*5, 490, 36, 36);

		m_slot_size = SLOT_SIZE;

		m_p_slot_rect = m_slot_rect;
		m_p_slot_image = m_slot_image;

}

/*-----------------------------------------------------------------------------
- ~C_VS_UI_SLAYER_GEAR
-
-----------------------------------------------------------------------------*/
C_VS_UI_SLAYER_GEAR::~C_VS_UI_SLAYER_GEAR()
{
}
/*-----------------------------------------------------------------------------
- C_VS_UI_SLAYER_GEAR
-
-----------------------------------------------------------------------------*/
//C_VS_UI_ADVANCEMENTSLAYER_GEAR::C_VS_UI_ADVANCEMENTSLAYER_GEAR()
//{
//	assert(MSlayerGear::MAX_GEAR_SLAYER == SLOT_SIZE);
//
//	m_slot_rect[SN_HELM].Set(80, 15, 60, 60);
//	m_slot_rect[SN_NECKLACE].Set(145, 45, 30, 30);
//	m_slot_rect[SN_COAT].Set(80, 79, 60, 90);
//	m_slot_rect[SN_LEFTHAND].Set(145, 79, 60, 90);
//	m_slot_rect[SN_RIGHTHAND].Set(15, 79, 60, 90);
//	m_slot_rect[SN_GLOVE].Set(15, 173, 60, 60);
//	m_slot_rect[SN_BELT].Set(145, 173, 60, 60);
//	m_slot_rect[SN_TROUSER].Set(80, 173, 60, 90);
//	m_slot_rect[SN_LEFTBRACELET].Set(27, 239, 30, 30);
//	m_slot_rect[SN_RIGHTBRACELET].Set(162, 239, 30, 30);
//	m_slot_rect[SN_RIGHTRING1].Set(8, 275, 30, 30);
//	m_slot_rect[SN_RIGHTRING2].Set(43, 275, 30, 30);
//	m_slot_rect[SN_LEFTRING1].Set(144, 275, 30, 30);
//	m_slot_rect[SN_LEFTRING2].Set(179, 275, 30, 30);
//	m_slot_rect[SN_SHOES].Set(80, 267, 60, 60);
//
//	m_slot_rect[SN_COREZAP1].Set(8, 274, 30, 30);
//	m_slot_rect[SN_COREZAP2].Set(43, 274, 30, 30);
//	m_slot_rect[SN_COREZAP3].Set(144, 274, 30, 30);
//	m_slot_rect[SN_COREZAP4].Set(179, 274, 30, 30);
//
//	m_slot_rect[SN_BLOODBIBLE1].Set(6, 332, 30, 30);
//	m_slot_rect[SN_BLOODBIBLE2].Set(41, 332, 30, 30);
//	m_slot_rect[SN_BLOODBIBLE3].Set(76, 332, 30, 30);
//	m_slot_rect[SN_BLOODBIBLE4].Set(111, 332, 30, 30);
//	m_slot_rect[SN_BLOODBIBLE5].Set(146, 332, 30, 30);
//	m_slot_rect[SN_BLOODBIBLE6].Set(181, 332, 30, 30);
//	m_slot_rect[SN_PDA].Set(146, 332, 30, 30);
//	m_slot_rect[SN_SHOULDER].Set(181, 332, 30, 30);
//
//	m_slot_size = SLOT_SIZE;
//	m_p_slot_rect = m_slot_rect;
//	m_p_slot_image = m_slot_image;
//
//}

/*-----------------------------------------------------------------------------
- ~C_VS_UI_SLAYER_GEAR
-
-----------------------------------------------------------------------------*/
//C_VS_UI_ADVANCEMENTSLAYER_GEAR::~C_VS_UI_ADVANCEMENTSLAYER_GEAR()
//{
//}

/*
//-----------------------------------------------------------------------------
// SetZone
//
// ZoneID�� �޾Ƽ� �̴ϸ��� �ҷ��´� & ���̸� ����
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER::SetZone(int zone_id)
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
void C_VS_UI_SLAYER::SetSafetyZone(RECT rect)
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

/*-----------------------------------------------------------------------------
- KeyboardControl
-
-----------------------------------------------------------------------------*/
void C_VS_UI_SLAYER::KeyboardControl(UINT message, UINT key, long extra)
{
	m_pC_chatting->KeyboardControl(message, key, extra);
}

/*-----------------------------------------------------------------------------
- C_VS_UI_SLAYER
-
-----------------------------------------------------------------------------*/
C_VS_UI_SLAYER::C_VS_UI_SLAYER():C_VS_UI_TRIBE()
{
	CT_Done("DIAG: C_VS_UI_SLAYER ctor enter (post C_VS_UI_TRIBE)");
	m_pC_gear = NULL;
	m_pC_gear_other = NULL;
//	m_pC_advence_gear = NULL;
	m_pC_quickitem  = NULL;
	m_pC_main_spk  = NULL;

//	AttrPin(true);


	//Set(RESOLUTION_X-m_pC_main_spk->GetWidth()+1, RESOLUTION_Y-m_pC_main_spk->GetHeight()+1, m_pC_main_spk->GetWidth()-1, m_pC_main_spk->GetHeight()-1);

	CT_Done("DIAG: C_VS_UI_SLAYER pre SetupMenuItems(SPK_SLAYER_MAIN)");
	SetupMenuItems(SPK_SLAYER_MAIN);
	CT_Done("DIAG: C_VS_UI_SLAYER post SetupMenuItems");

	// common buttons
//	m_pC_common_button_group->Add( new C_VS_UI_EVENT_BUTTON(tab_x, tab_y, m_pC_sys_button_spk->GetWidth(TAB_MSG), m_pC_sys_button_spk->GetHeight(TAB_MSG), TAB_MSG_ID, this, TAB_MSG) );
//	tab_x += m_pC_sys_button_spk->GetWidth(TAB_MSG);
	// 2004, 12, 2, sobeit add start - ���
	// 2004, 12, 2, sobeit add end

// 	int system_x = w-m_pC_main_spk->GetWidth(BUTTON_SYSTEM)-5, system_y = h-m_pC_main_spk->GetHeight(BUTTON_SYSTEM);
// 	m_pC_common_button_group->Add( new C_VS_UI_EVENT_BUTTON(system_x, system_y, m_pC_main_spk->GetWidth(BUTTON_SYSTEM), m_pC_main_spk->GetHeight(BUTTON_SYSTEM), SYSTEM_ID, this, BUTTON_SYSTEM) );
// #if	__CONTENTS(__USER_GRADE)
// 	m_pC_common_button_group->Add( new C_VS_UI_EVENT_BUTTON(system_x - m_pC_main_spk->GetWidth(BUTTON_USER_GRADE)-5, system_y, m_pC_main_spk->GetWidth(BUTTON_USER_GRADE), m_pC_main_spk->GetHeight(BUTTON_USER_GRADE), USER_GRADE, this, BUTTON_USER_GRADE));
// #endif
// 	m_pC_common_button_group->Add( new C_VS_UI_EVENT_BUTTON(24, system_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN), PUSHPIN_ID, this,C_GLOBAL_RESOURCE::AB_BUTTON_PUSHPIN ) );

// 	const int button_x = 25, button_y = 72, button_x_gap = 37, button_y_gap = 37;
	
	// menu buttons
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*0, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_INVENTORY), m_pC_sys_button_spk->GetHeight(BUTTON_INVENTORY), INVENTORY_ID, this, BUTTON_INVENTORY) );
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_GEAR), m_pC_sys_button_spk->GetHeight(BUTTON_GEAR), GEAR_ID, this, BUTTON_GEAR) );
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*2, button_y, m_pC_sys_button_spk->GetWidth(BUTTON_INFO), m_pC_sys_button_spk->GetHeight(BUTTON_INFO), INFO_ID, this, BUTTON_INFO) );
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*0, button_y+button_y_gap, m_pC_sys_button_spk->GetWidth(BUTTON_PARTY), m_pC_sys_button_spk->GetHeight(BUTTON_PARTY), PARTY_ID, this, BUTTON_PARTY) );
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y+button_y_gap, m_pC_sys_button_spk->GetWidth(BUTTON_QUEST), m_pC_sys_button_spk->GetHeight(BUTTON_QUEST), QUEST_ID, this, BUTTON_QUEST) );
// 	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*2, button_y+button_y_gap, m_pC_sys_button_spk->GetWidth(BUTTON_MAIL), m_pC_sys_button_spk->GetHeight(BUTTON_MAIL), MAIL_ID, this, BUTTON_MAIL) );
//	m_pC_menu_button_group->Add( new C_VS_UI_EVENT_BUTTON(button_x+button_x_gap*1, button_y+button_y_gap, m_pC_sys_button_spk->GetWidth(BUTTON_QUEST), m_pC_sys_button_spk->GetHeight(BUTTON_QUEST), QUEST_ID, this, BUTTON_QUEST) );

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
	CT_Done("DIAG: C_VS_UI_SLAYER pre new C_VS_UI_SLAYER_QUICKITEM");
	m_pC_quickitem = new C_VS_UI_SLAYER_QUICKITEM(SPK_SLAYER_QUICKITEM);
	CT_Done("DIAG: C_VS_UI_SLAYER post quickitem=%p, pre new gear", m_pC_quickitem);

//	if(m_skill_domain > 150)
//		m_pC_advence_gear = new C_VS_UI_ADVANCEMENTSLAYER_GEAR;
//	else
	m_pC_gear = new C_VS_UI_SLAYER_GEAR;
	CT_Done("DIAG: C_VS_UI_SLAYER post gear=%p, pre new gear_other", m_pC_gear);
	m_pC_gear_other = new C_VS_UI_SLAYER_GEAR;
	CT_Done("DIAG: C_VS_UI_SLAYER post gear_other=%p, pre GearOther(true)", m_pC_gear_other);
	m_pC_gear_other->GearOther(true);
	CT_Done("DIAG: C_VS_UI_SLAYER post GearOther");

	m_time = "";
	m_date = "";

	memcpy(m_HotKey_Type, gpC_vs_ui_window_manager->GetHotKeyType(), sizeof(int)*C_VS_UI_SLAYER_QUICKITEM::HOTKEY_MAX);

#ifndef _LIB
	SetTime("12:34:05");
	SetDate("2001/06/30");
#endif
	CT_Done("DIAG: C_VS_UI_SLAYER ctor exit");
}

/*-----------------------------------------------------------------------------
- ~C_VS_UI_SLAYER
-
-----------------------------------------------------------------------------*/
C_VS_UI_SLAYER::~C_VS_UI_SLAYER()
{
	Phoenix_ReleaseSlayerExpTextCache();

	/*if(m_pC_main_spk)
	{
		DeleteNew(m_pC_main_spk);
		m_pC_main_spk = NULL;
	}
	if(m_pC_sys_button_spk)
	{
		DeleteNew(m_pC_sys_button_spk);
		m_pC_sys_button_spk = NULL;
	}*/

	gpC_vs_ui_window_manager->SetHotkeyType((int *)m_HotKey_Type);
	DeleteNew(m_pC_gear);
	DeleteNew(m_pC_gear_other);
//	DeleteNew(m_pC_advence_gear);
	DeleteNew(m_pC_quickitem);
	
	
	
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER::GetGearWindow
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_GEAR * C_VS_UI_SLAYER::GetGearWindow() const
{
	return m_pC_gear;
}

//C_VS_UI_ADVANCEMENTSLAYER_GEAR * C_VS_UI_SLAYER ::GetAdvenceGearWindow() const
//{
//	return m_pC_advence_gear;
//}


//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER::CloseGear
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER::CloseGear()
{
	m_pC_gear->Finish();
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER::OpenGear
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER::OpenGear(bool bl_set_load)
{
	m_pC_gear->Start(bl_set_load);
}



//Add By Hays
C_VS_UI_GEAR * C_VS_UI_SLAYER::GetGearOtherWindow() const
{
	return m_pC_gear_other;
}


void	C_VS_UI_SLAYER::CloseGearOther()
{
	m_pC_gear_other->Finish();
}

void	C_VS_UI_SLAYER::OpenGearOther(bool bl_set_load)
{
	m_pC_gear_other->Start(bl_set_load);
}




#if __CONTENTS(__GEAR_SWAP_CHANGE)		//C_VS_UI_SLAYER::RunGearChange()
void C_VS_UI_SLAYER::RunGearChange()
{
	m_pC_gear->GearChange();
}
#endif	//__GEAR_SWAP_CHANGE

//void	C_VS_UI_SLAYER::AdvenceCloseGear()
//{
//	m_pC_advence_gear->Finish();
//}
//
//void	C_VS_UI_SLAYER::AdvenceOpenGear(bool bl_set_load1)
//{
//	m_pC_advence_gear->Start(bl_set_load1);
//}

//-----------------------------------------------------------------------------
// DoCommonActionBeforeEventOccured
//
// C_VS_UI_TRIBE::DoCommonActionBeforeEventOccured() ���� ����.
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER::DoCommonActionBeforeEventOccured()
{
	C_VS_UI_TRIBE::DoCommonActionBeforeEventOccured();
}

//-----------------------------------------------------------------------------
// DoCommonActionAfterEventOccured
//
// C_VS_UI_TRIBE::DoCommonActionAfterEventOccured() ���� ����.
//
// ��� Window�� �ݴ´�.
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER::DoCommonActionAfterEventOccured()
{
	C_VS_UI_TRIBE::DoCommonActionAfterEventOccured();
}

//-----------------------------------------------------------------------------
// ResetSlayerQuickItemSize
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER::ResetSlayerQuickItemSize()
{
	m_pC_quickitem->ResetSize();
}

//void C_VS_UI_SLAYER::HotKey_Extend()		// ctrl + e
//{
//	Run(EXTEND);
//}

void C_VS_UI_SLAYER::HotKey_F1()				
{
	assert(m_pC_quickitem != NULL);

	m_pC_quickitem->Use(C_VS_UI_SLAYER_QUICKITEM::SLOT_1);
}

void C_VS_UI_SLAYER::HotKey_QuickItemSlot()
{
	assert(m_pC_quickitem != NULL);

	m_pC_quickitem->ToggleBelt();
}

void C_VS_UI_SLAYER::HotKey_F2()				
{
	assert(m_pC_quickitem != NULL);

	m_pC_quickitem->Use(C_VS_UI_SLAYER_QUICKITEM::SLOT_2);
}

void C_VS_UI_SLAYER::HotKey_F3()				
{
	assert(m_pC_quickitem != NULL);

	m_pC_quickitem->Use(C_VS_UI_SLAYER_QUICKITEM::SLOT_3);
}

void C_VS_UI_SLAYER::HotKey_F4()				
{
	assert(m_pC_quickitem != NULL);

	m_pC_quickitem->Use(C_VS_UI_SLAYER_QUICKITEM::SLOT_4);
}

void C_VS_UI_SLAYER::HotKey_F5()				
{
	assert(m_pC_quickitem != NULL);

	m_pC_quickitem->Use(C_VS_UI_SLAYER_QUICKITEM::SLOT_5);
}

void C_VS_UI_SLAYER::HotKey_F6()				
{
	assert(m_pC_quickitem != NULL);

	m_pC_quickitem->Use(C_VS_UI_SLAYER_QUICKITEM::SLOT_6);
}

void C_VS_UI_SLAYER::HotKey_F7()				
{
	assert(m_pC_quickitem != NULL);

	m_pC_quickitem->Use(C_VS_UI_SLAYER_QUICKITEM::SLOT_7);
}

void C_VS_UI_SLAYER::HotKey_F8()				
{
	assert(m_pC_quickitem != NULL);

	m_pC_quickitem->Use(C_VS_UI_SLAYER_QUICKITEM::SLOT_8);
}


void	C_VS_UI_SLAYER::Hotkey_Function(int FKey)
{
	int	current_mouse_x = gpC_window_manager->GetCurrentMouse_x() ;
	int	current_mouse_y = gpC_window_manager->GetCurrentMouse_y() ;

	C_VS_UI_SKILL * p_skill = GetSkillWindow()		;
	bool	bSkillFocus = p_skill->IsPixel2(current_mouse_x , current_mouse_y) ; 
	bool	bQuickItemFocus =  m_pC_quickitem->IsPixel(current_mouse_x , current_mouse_y) ; 


	if(bSkillFocus)
	{
		int skillslot = p_skill->GetFocusedSlot()		;
		int skillid   = p_skill->GetSkillID(skillslot)	;
		if (skillid != NOT_SELECTED)
		{
			p_skill->SetHotkey(C_VS_UI_SKILL::HOTKEY(FKey), (ACTIONINFO)skillid)	;
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
		else
		{
			int QuickItemslot = m_pC_quickitem->GetSlot(C_VS_UI_SLAYER_QUICKITEM::HOTKEY(FKey)) ;//
			m_pC_quickitem->Use(QuickItemslot);
		}
	}
}


void C_VS_UI_SLAYER::RunQuickItemSlot()
{
	assert(m_pC_quickitem != NULL);

	m_pC_quickitem->Open();
}

void C_VS_UI_SLAYER::CloseQuickItemSlot()
{
	assert(m_pC_quickitem != NULL);

	m_pC_quickitem->Close();
}

bool C_VS_UI_SLAYER::IsRunningQuickItemSlot()
{
	assert(m_pC_quickitem != NULL);

//	return m_pC_quickitem->IsOpen();
	return true;
}


//-----------------------------------------------------------------------------
// MouseControl
//
// Item�� ��� ���� ��� Window �̵��̳� Main interface���� �Է��� �� �� ����.
//-----------------------------------------------------------------------------
/*bool C_VS_UI_SLAYER::MouseControl(UINT message, int _x, int _y)
{
//	if(m_pC_chat->GetCurrentMode() == WHISPER)
//		m_pC_chat->MouseControl(message, _x, _y);

	//
	// Main interface.
	//
//	int i;

	if (gpC_mouse_pointer->GetPickUpItem() == NULL) // Item�� ��� ������ �Է��� �� ����.
	{
//		if (m_pC_pds->GetPDS() == NO_PDS_MENU)// || m_pC_pds->PCSOnline() == true)
		if(m_bl_mark && m_pC_main_spk->IsPixel(_x-m_mark_x, _y-m_mark_y, MARK_WINDOW))
		{
			switch(message)
			{
				case M_LEFTBUTTON_DOWN:
				case M_LB_DOUBLECLICK:
				{
					if(_x > m_mark_x+9 && _x < m_mark_x + 9*14+9 && _y > m_mark_y+9 && _y < m_mark_y + 14*5+9)
					{
						m_mark_col = (_y - m_mark_y-9)/14;
						m_mark_row = (_x - m_mark_x-9)/14;
						KeyboardControl(WM_KEYDOWN, VK_RETURN, 0);
					}
				}
				break;

				case M_WHEEL_UP:
					Run(MARK_UP_ID);
					break;

				case M_WHEEL_DOWN:
					Run(MARK_DOWN_ID);
					break;

			}
			m_pC_mark_button_group->MouseControl(message, _x, _y);
		}
		else
		{
			m_pC_chatting->MouseControl(message, _x, _y);
			m_pC_button_group->MouseControl(message, _x, _y);
		}
	}

	return true;

}
*/

/*-----------------------------------------------------------------------------
- KeyboardControl
-
-----------------------------------------------------------------------------*/
//void C_VS_UI_SLAYER::KeyboardControl(UINT message, UINT key, long extra)
//{

//}

int qsort_SortExp(const void *arg1,const void *arg2)
{
	return ((*g_pSkillManager)[*(int*)arg1].GetDomainLevel() < (*g_pSkillManager)[*(int*)arg2].GetDomainLevel());
}

/*-----------------------------------------------------------------------------
- Show
- 
-----------------------------------------------------------------------------*/
void C_VS_UI_SLAYER::ShowExp()
{
	CT_Done("SE: ENTER pthis=%p g_pSkillManager=%p", (void*)this, (void*)g_pSkillManager);

	char sz_temp[100];

	CT_Done("SE: pre SkillManager[SWORD].GetDomainLevel");
	int domain_level_Sword		= (*g_pSkillManager)[SKILLDOMAIN_SWORD].GetDomainLevel();
	CT_Done("SE: SWORD=%d, pre BLADE", domain_level_Sword);
	int domain_level_Blade		= (*g_pSkillManager)[SKILLDOMAIN_BLADE].GetDomainLevel();
	CT_Done("SE: BLADE=%d, pre GUN", domain_level_Blade);
	int domain_level_Gun		= (*g_pSkillManager)[SKILLDOMAIN_GUN].GetDomainLevel();
	CT_Done("SE: GUN=%d, pre HEAL", domain_level_Gun);
	int domain_level_Heal		= (*g_pSkillManager)[SKILLDOMAIN_HEAL].GetDomainLevel();
	CT_Done("SE: HEAL=%d, pre ENCHANT", domain_level_Heal);
	int domain_level_Enchant	= (*g_pSkillManager)[SKILLDOMAIN_ENCHANT].GetDomainLevel();
	CT_Done("SE: ENCHANT=%d", domain_level_Enchant);

	int domainLevel = 0;
	domainLevel = max(domain_level_Sword , domain_level_Blade);
	domainLevel = max(domainLevel, domain_level_Gun);
	domainLevel = max(domainLevel, domain_level_Heal);
	domainLevel = max(domainLevel, domain_level_Enchant);

	int playerLevel = domainLevel;

	Rect rect;
	int imgX = (m_pC_main_spk->GetWidth(SIMPLEINFOMATION_MAIN)-m_pC_main_spk->GetWidth(EXP_BAR_BLUE))/2;

	if(gpC_base->m_p_DDSurface_back->Lock())
	{

		// ��ų ����ġ
		//int skill_id = m_pC_skill->GetSelectedSkillID();
		//if (skill_id != NOT_SELECTED)
		//{
		//	int max_exp = (*g_pSkillInfoTable)[skill_id].maxExp;
		//	int exp = (*g_pSkillInfoTable)[skill_id].GetSkillExp();
		//	int exp_percent;
		//	if(max_exp > 0)
		//		exp_percent = (exp % (max_exp/100))*100/(max_exp/100);
		//	else exp_percent = 0;
		//	
		//	if(exp == max_exp)exp_percent = 100;
		//	Rect rect;
		//	rect.Set(0,	m_pC_main_spk->GetHeight(EXP_BAR_BLUE)-m_pC_main_spk->GetHeight(EXP_BAR_BLUE)*exp_percent/100,
		//
		//
		//		// �ض� ���̶� ����� �޶� ������-_-;
		//		// �ض� ���̶� ����� �޶� ������-_-;
		//		m_pC_main_spk->GetWidth(EXP_BAR_BLUE), m_pC_main_spk->GetHeight(EXP_BAR_BLUE)*exp_percent/100);
		//	m_pC_main_spk->BltLockedClip(x+79, y+6, rect, EXP_BAR_BLUE);
		//}

		//
		////////////////////////////////////////////////////////////////////////////
		//// �����̾� ����ġ
		//	
		//	// EXP �� ��ư�� ����
		//const int bar_x = 120, bar_y = 81, str_x = 58, num_x = 98, bar_gap = 12;
		//		char sz_temp[10];
		//		
		//		int domain[3];
		//		domain[0] = -1;
		//		domain[1] = -1;
		//		domain[2] = -1;
		//		
		//		for(int i = 0; i < 3; i++)
		//		{
		//			int high_level = -1;
		//			
		//			for(int d = SKILLDOMAIN_BLADE ; d <= SKILLDOMAIN_ENCHANT; d++)
		//			{
		//				const int domain_level = (*g_pSkillManager)[d].GetDomainLevel();
		//				if(d != domain[0] && d != domain[1] && d != domain[2] && domain_level > high_level)
		//				{
		//					domain[i] = d;
		//					high_level = domain_level;
		//				}
		//			}
		//		}
		//		
		//		if((*g_pSkillManager)[domain[0]].GetDomainLevel() == 0 &&
		//			(*g_pSkillManager)[domain[1]].GetDomainLevel() == 0 &&
		//			(*g_pSkillManager)[domain[2]].GetDomainLevel() == 0)
		//		{
		//			int domain2[5] = { SKILLDOMAIN_BLADE, SKILLDOMAIN_SWORD, SKILLDOMAIN_GUN, SKILLDOMAIN_HEAL, SKILLDOMAIN_ENCHANT };

		//			qsort((void*)domain2,5,sizeof(int),qsort_SortExp);
		//			
		//			memcpy(domain,domain2,sizeof(int) * 3);
		//		}
		//		
		//		char *domain_string[5] =
		//		{
		//			(*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_DOMAIN_BLADE].GetString(),
		//			(*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_DOMAIN_SWORD].GetString(),
		//			(*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_DOMAIN_GUN].GetString(),
		//			(*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_DOMAIN_HEAL].GetString(),
		//			(*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_DOMAIN_ENCHANT].GetString(),
		//		};
		//		
		//		//			if(gpC_base->m_p_DDSurface_back->Lock())
		//		{
		//			Rect rect;
		//			int exp_remain = g_char_slot_ingame.STR_EXP_REMAIN;
		//			int goal_exp = g_pExperienceTable->GetSTRInfo(g_char_slot_ingame.STR_PURE).GoalExp;
		//			if(goal_exp)
		//			{
		//				m_pC_main_spk->BltLocked(x+bar_x, y+bar_y+bar_gap*0, EXP_BAR_BACKGROUD);
		//				rect.Set(0, 0, m_pC_main_spk->GetWidth(EXP_BAR_BACKGROUD)*(goal_exp - exp_remain)/(max(1, goal_exp)), m_pC_main_spk->GetHeight(EXP_BAR_BLUE));
		//				m_pC_main_spk->BltLockedClip(x+bar_x+3, y+bar_y+bar_gap*0+3, rect, EXP_BAR_BLUE);
		//			}
		//			exp_remain = g_char_slot_ingame.DEX_EXP_REMAIN;
		//			goal_exp = g_pExperienceTable->GetDEXInfo(g_char_slot_ingame.DEX_PURE).GoalExp;
		//			if(goal_exp)
		//			{
		//				m_pC_main_spk->BltLocked(x+bar_x, y+bar_y+bar_gap*1, EXP_BAR_BACKGROUD);
		//				rect.Set(0, 0, m_pC_main_spk->GetWidth(EXP_BAR_BLUE)*(goal_exp - exp_remain)/(max(1, goal_exp)), m_pC_main_spk->GetHeight(EXP_BAR_BLUE));
		//				m_pC_main_spk->BltLockedClip(x+bar_x+3, y+bar_y+bar_gap*1+3, rect, EXP_BAR_BLUE);
		//			}
		//			exp_remain = g_char_slot_ingame.INT_EXP_REMAIN;
		//			goal_exp = g_pExperienceTable->GetINTInfo(g_char_slot_ingame.INT_PURE).GoalExp;
		//			if(goal_exp)
		//			{
		//				m_pC_main_spk->BltLocked(x+bar_x, y+bar_y+bar_gap*2, EXP_BAR_BACKGROUD);
		//				rect.Set(0, 0, m_pC_main_spk->GetWidth(EXP_BAR_BLUE)*(goal_exp - exp_remain)/(max(1, goal_exp)), m_pC_main_spk->GetHeight(EXP_BAR_BLUE));
		//				m_pC_main_spk->BltLockedClip(x+bar_x+3, y+bar_y+bar_gap*2+3, rect, EXP_BAR_BLUE);
		//			}
		//			
		//			for(int i = 0; i < 3; i++)
		//			{
		//				const int domain_level = (*g_pSkillManager)[domain[i]].GetDomainLevel();
		//				const int exp_remain = (*g_pSkillManager)[domain[i]].GetDomainExpRemain();
		//				const int goal_exp = (*g_pSkillManager)[domain[i]].GetExpInfo(domain_level).GoalExp;
		//				//������ ����ġ ��
		//				m_pC_main_spk->BltLocked(x+bar_x, y+bar_y+bar_gap*(3+i), EXP_BAR_BACKGROUD);
		//				rect.Set(0, 0, m_pC_main_spk->GetWidth(EXP_BAR_BLUE)*(goal_exp - exp_remain)/(goal_exp), m_pC_main_spk->GetHeight(EXP_BAR_BLUE));
		//				m_pC_main_spk->BltLockedClip(x+bar_x+3, y+bar_y+bar_gap*(3+i)+3, rect, EXP_BAR_BLUE);
		//			}
		//			


					// Fix #18 (2026-05-15): Slayer EXP bar non-advancement branch'i.
					// Slayer'da karakter EXP'i YOK — server MODIFY_VAMP_GOAL_EXP /
					// MODIFY_OUSTERS_GOAL_EXP Slayer'a gonderilmiyor. Slayer
					// progression STR + DEX + INT 3 stat-based. Onceki kod
					// g_char_slot_ingame.EXP_REMAIN (populate edilmiyor) +
					// GetVampireInfo (yanlis tablo) kullaniyordu → bar stuck.
					//
					// Fix: 3 stat'in (STR/DEX/INT) toplam remaining-to-next-goal
					// degerini al, toplam GoalExp'e oran ile bar fill. Her stat
					// kill'de update oldugu icin bar sorunsuz ilerler.
					int exp_remain = g_char_slot_ingame.EXP_REMAIN;
					__int64 goal_exp = 0;
					if(g_char_slot_ingame.m_AdvancementLevel>0)
					{
						// Domain Lord (Advancement Class) — single character EXP.
						// Server MODIFY_VAMP_GOAL_EXP gonderir, EXP_REMAIN populate olur.
						if(g_char_slot_ingame.m_AdvancementLevel>100)
							goal_exp=g_pExperienceTable->GetAdvanceMent(150).GoalExp;
						else
							goal_exp=g_pExperienceTable->GetAdvanceMent(g_char_slot_ingame.m_AdvancementLevel).GoalExp;
					}
					else
					{
						// Non-advancement Slayer — STR + DEX + INT combined progression
						__int64 total_remain = (__int64)g_char_slot_ingame.STR_EXP_REMAIN
						                     + (__int64)g_char_slot_ingame.DEX_EXP_REMAIN
						                     + (__int64)g_char_slot_ingame.INT_EXP_REMAIN;
						__int64 total_goal = (__int64)g_pExperienceTable->GetSTRInfo(g_char_slot_ingame.STR_PURE).GoalExp
						                   + (__int64)g_pExperienceTable->GetDEXInfo(g_char_slot_ingame.DEX_PURE).GoalExp
						                   + (__int64)g_pExperienceTable->GetINTInfo(g_char_slot_ingame.INT_PURE).GoalExp;
						exp_remain = (int)total_remain;
						goal_exp = total_goal;
					}
					int exp_width = m_pC_main_spk->GetWidth(EXP_BAR_BLUE);
					int exp_height = m_pC_main_spk->GetHeight(EXP_BAR_BLUE);
					// Division-by-zero guard — STR/DEX/INT_PURE out-of-range veya
					// AdvancementLevel lookup 0 dondurursa bar = 0 (sessiz).
					int exp_bar = (goal_exp > 0)
					              ? (int)((__int64)exp_width * (goal_exp - exp_remain) / goal_exp)
					              : 0;

					rect.Set(0, 0, exp_bar, exp_height);


					CT_Done("SE: pre BltLocked EXP_BAR_BACKGROUD x=%d y=%d", x+50, y+130);
					m_pC_main_spk->BltLocked(x+50, y+130, EXP_BAR_BACKGROUD);
					CT_Done("SE: post BltLocked EXP_BAR_BACKGROUD, pre BltLockedClip EXP_BAR_BLUE rect=(0,0,%d,%d)", exp_bar, exp_height);
					m_pC_main_spk->BltLockedClip(x+51, y+131, rect, EXP_BAR_BLUE);
					CT_Done("SE: post BltLockedClip EXP_BAR_BLUE — pre Sword block (sword=%d)", domain_level_Sword);


					if(domain_level_Sword>0)
					{
					 CT_Done("SE: Sword>0 branch, pre GetWidth BLADE");
					 int bladeW = m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE);
					 CT_Done("SE: bladeW=%d, pre BltLocked SWORD_LEARN", bladeW);
					 m_pC_main_spk->BltLocked(x+35+bladeW+20,y+90,SIMPLEINFOMATION_SWORD_LEARN);
					 CT_Done("SE: post SWORD_LEARN, pre JOB_CHOOSE");
					 m_pC_main_spk->BltLocked(x+35+bladeW+20-2,y+90-2,SIMPLEINFOMATION_JOB_CHOOSE);
					 CT_Done("SE: post JOB_CHOOSE Sword block");
					}
					else
					{
					   CT_Done("SE: Sword=0 branch, pre BltLocked SWORD");
					   m_pC_main_spk->BltLocked(x+35+m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE)+20,y+90,SIMPLEINFOMATION_SWORD);
					   CT_Done("SE: post BltLocked SWORD");
					}
					CT_Done("SE: pre Blade block (blade=%d)", domain_level_Blade);
					if(domain_level_Blade>0)
					{
					  CT_Done("SE: Blade>0 branch, pre BLADE_LEARN");
					  m_pC_main_spk->BltLocked(x+35,y+90,SIMPLEINFOMATION_BLADE_LEARN);
					  CT_Done("SE: post BLADE_LEARN, pre JOB_CHOOSE");
					  m_pC_main_spk->BltLocked(x+35-2,y+90-2,SIMPLEINFOMATION_JOB_CHOOSE);
					  CT_Done("SE: post Blade block");
					}
					else
					{
					   CT_Done("SE: Blade=0 branch, pre BltLocked BLADE");
					   m_pC_main_spk->BltLocked(x+35,y+90,SIMPLEINFOMATION_BLADE);
					   CT_Done("SE: post BltLocked BLADE");
					}
					CT_Done("SE: pre Gun block (gun=%d)", domain_level_Gun);
					if(domain_level_Gun>0)
					{
					   m_pC_main_spk->BltLocked(x+35+m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE)*2+20*2,y+90,SIMPLEINFOMATION_GUN_LEARN);
					   m_pC_main_spk->BltLocked(x+35+m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE)*2+20*2-2,y+90-2,SIMPLEINFOMATION_JOB_CHOOSE);
					}
					else
					{
						m_pC_main_spk->BltLocked(x+35+m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE)*2+20*2,y+90,SIMPLEINFOMATION_GUN);
					}
					CT_Done("SE: post Gun block, pre Heal block (heal=%d)", domain_level_Heal);
					if(domain_level_Heal>0)
					{
						m_pC_main_spk->BltLocked(x+35+m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE)*3+20*3,y+90,SIMPLEINFOMATION_HEALTH_LEARN);
						m_pC_main_spk->BltLocked(x+35+m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE)*3+20*3-2,y+90-2,SIMPLEINFOMATION_JOB_CHOOSE);
					}
					else
					{
					 m_pC_main_spk->BltLocked(x+35+m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE)*3+20*3,y+90,SIMPLEINFOMATION_HEALTH);
					}
					CT_Done("SE: post Heal block, pre Enchant block (enchant=%d)", domain_level_Enchant);
					if(domain_level_Enchant>0)
					{
					   m_pC_main_spk->BltLocked(x+35+m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE)*4+20*4,y+90,SIMPLEINFOMATION_ENCHANT_LEARN);
					   m_pC_main_spk->BltLocked(x+35+m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE)*4+20*4-2,y+90-2,SIMPLEINFOMATION_JOB_CHOOSE);
					}
					else
					{
						m_pC_main_spk->BltLocked(x+35+m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE)*4+20*4,y+90,SIMPLEINFOMATION_ENCHANT);
					}
					CT_Done("SE: post Enchant block, pre Unlock");

					gpC_base->m_p_DDSurface_back->Unlock();
					CT_Done("SE: post Unlock — Lock block done");
	}
	else
	{
		CT_Done("SE: Lock FAILED");
	}
				const int slayerTextCacheLeft = x;
				const int slayerTextCacheTop = y + 68;
				const int slayerTextCacheWidth = 245;
				const int slayerTextCacheHeight = 80;
				const int slayerBladeWidth = m_pC_main_spk->GetWidth(SIMPLEINFOMATION_BLADE);
				bool slayerTextCacheBlt = false;

				if(Phoenix_EnsureSlayerExpTextCacheSurface(slayerTextCacheWidth, slayerTextCacheHeight))
				{
					const DWORD textSignature = Phoenix_BuildSlayerExpTextSignature(
						g_char_slot_ingame.STR_CUR,
						g_char_slot_ingame.DEX_CUR,
						g_char_slot_ingame.INT_CUR,
						domain_level_Blade,
						domain_level_Sword,
						domain_level_Gun,
						domain_level_Heal,
						domain_level_Enchant,
						gpC_base->m_chatting_pi.hfont);

					if(Phoenix_NeedsSlayerExpTextCacheBuild(slayerTextCacheLeft, slayerTextCacheTop, textSignature, gpC_base->m_chatting_pi.hfont))
					{
						HDC cacheDC = Phoenix_BeginSlayerExpTextCacheBuild();
						if(cacheDC != NULL)
						{
							Phoenix_DrawSlayerExpTextLayerCached(
								cacheDC,
								slayerTextCacheLeft,
								slayerTextCacheTop,
								x,
								y,
								slayerBladeWidth,
								domain_level_Blade,
								domain_level_Sword,
								domain_level_Gun,
								domain_level_Heal,
								domain_level_Enchant);
							Phoenix_CommitSlayerExpTextCacheBuild(slayerTextCacheLeft, slayerTextCacheTop, textSignature, gpC_base->m_chatting_pi.hfont);
						}
					}

					slayerTextCacheBlt = Phoenix_BltSlayerExpTextCache();
				}

				if(!slayerTextCacheBlt)
				{
					g_FL2_GetDC();
					//
					//g_PrintColorStrShadow(x+str_x, y+bar_y+bar_gap*0, (*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_STR].GetString(), gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
					//wsprintf(sz_temp, "%d", g_char_slot_ingame.STR_CUR);
					//g_PrintColorStrShadow(x+num_x, y+bar_y+bar_gap*0, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
					//
					//g_PrintColorStrShadow(x+str_x, y+bar_y+bar_gap*1, (*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_DEX].GetString(), gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
					//wsprintf(sz_temp, "%d", g_char_slot_ingame.DEX_CUR);
					//g_PrintColorStrShadow(x+num_x, y+bar_y+bar_gap*1, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
					//
					//g_PrintColorStrShadow(x+str_x, y+bar_y+bar_gap*2, (*g_pGameStringTable)[UI_STRING_MESSAGE_ENG_INT].GetString(), gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
					//wsprintf(sz_temp, "%d", g_char_slot_ingame.INT_CUR);
					//g_PrintColorStrShadow(x+num_x, y+bar_y+bar_gap*2, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
					//
					//for(int i = 0; i < 3; i++)
					//{
					//	g_PrintColorStrShadow(x+str_x, y+bar_y+bar_gap*(3+i), domain_string[domain[i]-SKILLDOMAIN_BLADE], gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
					//	const int domain_level = (*g_pSkillManager)[domain[i]].GetDomainLevel();
					//	wsprintf(sz_temp, "%d", domain_level);
					//	g_PrintColorStrShadow(x+num_x, y+bar_y+bar_gap*(3+i), sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);
					//}
				
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


					wsprintf(sz_temp, "%d", domain_level_Blade);
					int max11= (18-g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont))/2;
					g_PrintColorStrShadow(x+35+max11,y+90+21, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);

					wsprintf(sz_temp, "%d", domain_level_Sword);
					int max12= (18-g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont))/2;
					g_PrintColorStrShadow(x+35+slayerBladeWidth+20-1+max12,y+90+21, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);

					wsprintf(sz_temp, "%d", domain_level_Gun);
					int max13= (18-g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont))/2;
					g_PrintColorStrShadow(x+35+slayerBladeWidth*2+20*2-1+max13,y+90+21, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);

					wsprintf(sz_temp, "%d", domain_level_Heal);
					int max14= (18-g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont))/2;
					g_PrintColorStrShadow(x+35+slayerBladeWidth*3+20*3-1+max14,y+90+21, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);

					wsprintf(sz_temp, "%d", domain_level_Enchant);
					int max15= (18-g_GetStringWidth(sz_temp,gpC_base->m_chatting_pi.hfont))/2;
					g_PrintColorStrShadow(x+35+slayerBladeWidth*4+20*4-1+max15,y+90+21, sz_temp, gpC_base->m_chatting_pi, RGB_RED, RGB_BLACK);


					wsprintf(sz_temp,""); // Exp bar string EXP.Point
					g_PrintColorStrShadow(x+maxX-10, y+129, sz_temp, gpC_base->m_chatting_pi, RGB_WHITE, RGB_BLACK);

					g_FL2_ReleaseDC();
				}
			
	

	SHOW_WINDOW_ATTR;

}

//-----------------------------------------------------------------------------
// Start
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER::Start()
{
	memcpy(m_HotKey_Type, gpC_vs_ui_window_manager->GetHotKeyType(), sizeof(int)*C_VS_UI_SLAYER_QUICKITEM::HOTKEY_MAX);
	C_VS_UI_TRIBE::Start();

	m_pC_quickitem->Start();	
	m_pC_skill->Start(); 
	m_pC_hpbar->Start();
	m_pC_BloodBurst->Start();
//	m_pC_Market->Start();
	m_pC_ShortCutSlot->Start();

	m_pC_effect_status->Start();
	m_pC_minimap->Start();

	WindowEventReceiver(EVENT_WINDOW_MOVE);

}

//-----------------------------------------------------------------------------
// Process
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER::Process()
{
	C_VS_UI_TRIBE::Process();
}

C_VS_UI_SLAYER_PORTAL::C_VS_UI_SLAYER_PORTAL( int nExtendLevel )
{
	g_RegisterWindow(this);

	AttrTopmost(true);

	m_pC_button_group = NULL;
	m_map = 0;
	m_nExtendLevel = nExtendLevel;
	m_bReady = false;

	m_etc_spk.Open(SPK_SLAYER_PORTAL_ETC);
	m_map_spk.Open(SPK_SLAYER_PORTAL_MAP);

	const int requiredEtcFrames = FLAG_HILIGHT + 7;
	if (m_etc_spk.GetSize() < requiredEtcFrames || m_map_spk.GetSize() < MAP_MAX)
	{
		Phoenix_LogSlayerPortalError("[SlayerPortal] asset contract failed. etc=%d required=%d map=%d required=%d",
			m_etc_spk.GetSize(), requiredEtcFrames, m_map_spk.GetSize(), MAP_MAX);
		return;
	}

	const int resolutionX = (g_pUserInformation != NULL) ? g_pUserInformation->iResolution_x : 800;
	const int resolutionY = (g_pUserInformation != NULL) ? g_pUserInformation->iResolution_y : 600;
	//Set(RESOLUTION_X/2-m_map_spk.GetWidth(0)/2, RESOLUTION_Y/2 - m_map_spk.GetHeight(0)/2, m_map_spk.GetWidth(0), m_map_spk.GetHeight(0));
	Set(resolutionX/2-m_map_spk.GetWidth(0)/2, resolutionY/2 - m_map_spk.GetHeight(0)/2, m_map_spk.GetWidth(0), m_map_spk.GetHeight(0));

	m_left_x = -12, m_left_y = 0;
	m_right_x = 313, m_right_y = m_left_y;
	m_close_x = 255, m_close_y = 105;

	// set button
	m_pC_button_group = new ButtonGroup(this);

	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(x + m_left_x, y + m_left_y, m_etc_spk.GetWidth(LEFT), m_etc_spk.GetHeight(LEFT), LEFT, this, LEFT));
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(x + m_right_x, y + m_right_y, m_etc_spk.GetWidth(RIGHT), m_etc_spk.GetHeight(RIGHT), RIGHT, this, RIGHT));
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(x + m_close_x, y + m_close_y, m_etc_spk.GetWidth(CLOSE), m_etc_spk.GetHeight(CLOSE), CLOSE, this, CLOSE));


	// ���� ��
	MAP_SPK_INDEX map_convert[MAP_MAX] = { ESLANIAN, LIMBO, DROBETA, RODIN, TIMORE, PERONA , VRANCO };

	int map = gC_vs_ui.GetZoneID()/10-1;
	if(map < 0 || (map >= MAP_MAX && map != 8))
		m_map = 0;
	else if(map == 8)
		m_map = VRANCO;
	else
		m_map = map_convert[map];

	m_nExtendLevel = nExtendLevel;

//#define __FLAG_SET_MODE
#ifdef  __FLAG_SET_MODE

	// FLAG ����
	UI_PORTAL_FLAG temp_flag;

	/////////////////////////////////////////////////////////////////////////////////////
	// Portal Flag Position
	
	temp_flag.extend_portal = 0 ; 

	// ������
	temp_flag.zone_id = 21;
	temp_flag.x = 292;
	temp_flag.y = 27;
	temp_flag.portal_x = 227;
	temp_flag.portal_y = 59;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 21;
	temp_flag.x = 211;
	temp_flag.y = 38;
	temp_flag.portal_x = 54;
	temp_flag.portal_y = 111;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 22;
	temp_flag.x = 115;
	temp_flag.y = 58;
	temp_flag.portal_x = 120;
	temp_flag.portal_y = 214;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 22;
	temp_flag.x = 121;
	temp_flag.y = 24;
	temp_flag.portal_x = 127;
	temp_flag.portal_y = 12;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 23;
	temp_flag.x = 290;
	temp_flag.y = 81;
	temp_flag.portal_x = 185;
	temp_flag.portal_y = 57;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 23;
	temp_flag.x = 230;
	temp_flag.y = 112;
	temp_flag.portal_x = 74;
	temp_flag.portal_y = 192;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 24;
	temp_flag.x = 145;
	temp_flag.y = 118;
	temp_flag.portal_x = 187;
	temp_flag.portal_y = 206;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 24;
	temp_flag.x = 81;
	temp_flag.y = 98;
	temp_flag.portal_x = 76;
	temp_flag.portal_y = 100;
	m_flag[LIMBO].push_back(temp_flag);


	// ������Ͼ�
	temp_flag.zone_id = 11;
	temp_flag.x = 206;
	temp_flag.y = 41;
	temp_flag.portal_x = 48;
	temp_flag.portal_y = 131;
	m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 11;
	temp_flag.x = 292;
	temp_flag.y = 31;
	temp_flag.portal_x = 223;
	temp_flag.portal_y = 75;
	m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 12;
	temp_flag.x = 109;
	temp_flag.y = 44;
	temp_flag.portal_x = 117;
	temp_flag.portal_y = 145;
	m_flag[ESLANIAN].push_back(temp_flag);

	//temp_flag.x = 67;
	//temp_flag.y = 36;
	//temp_flag.portal_x = 16;
	//temp_flag.portal_y = 99;
	//m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 12;
	temp_flag.x = 173;
	temp_flag.y = 31;
	temp_flag.portal_x = 229;
	temp_flag.portal_y = 60;
	m_flag[ESLANIAN].push_back(temp_flag);
	//temp_flag.x = 157;
	//temp_flag.y = 45;
	//temp_flag.portal_x = 211;
	//temp_flag.portal_y = 149;
	//m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 13;
	temp_flag.x = 259;
	temp_flag.y = 105;
	temp_flag.portal_x = 127;
	temp_flag.portal_y = 163;
	m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 13;
	temp_flag.x = 260;
	temp_flag.y = 84;
	temp_flag.portal_x = 140;
	temp_flag.portal_y = 74;
	m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 14;
	temp_flag.x = 72;
	temp_flag.y = 84;
	temp_flag.portal_x = 50;
	temp_flag.portal_y = 77;
	m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 14;
	temp_flag.x = 174;
	temp_flag.y = 79;
	temp_flag.portal_x = 233;
	temp_flag.portal_y = 55;
	m_flag[ESLANIAN].push_back(temp_flag);


	// �ε��
	temp_flag.zone_id = 41;
	temp_flag.x = 287;
	temp_flag.y = 44;
	temp_flag.portal_x = 207;
	temp_flag.portal_y = 142;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 41;
	temp_flag.x = 211;
	temp_flag.y = 42;
	temp_flag.portal_x = 55;
	temp_flag.portal_y = 131;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 42;
	temp_flag.x = 121;
	temp_flag.y = 28;
	temp_flag.portal_x = 124;
	temp_flag.portal_y = 53;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 42;
	temp_flag.x = 113;
	temp_flag.y = 61;
	temp_flag.portal_x = 119;
	temp_flag.portal_y = 223;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 43;
	temp_flag.x = 210;
	temp_flag.y = 90;
	temp_flag.portal_x = 45;
	temp_flag.portal_y = 99;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 43;
	temp_flag.x = 308;
	temp_flag.y = 108;
	temp_flag.portal_x = 217;
	temp_flag.portal_y = 184;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 44;
	temp_flag.x = 89;
	temp_flag.y = 91;
	temp_flag.portal_x = 82;
	temp_flag.portal_y = 103;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 44;
	temp_flag.x = 149;
	temp_flag.y = 106;
	temp_flag.portal_x = 194;
	temp_flag.portal_y = 163;
	m_flag[RODIN].push_back(temp_flag);


	// ��κ�Ÿ
	temp_flag.zone_id = 31;
	temp_flag.x = 198;
	temp_flag.y = 29;
	temp_flag.portal_x = 27;
	temp_flag.portal_y = 58;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 31;
	temp_flag.x = 252;
	temp_flag.y = 58;
	temp_flag.portal_x = 132;
	temp_flag.portal_y = 211;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 32;
	temp_flag.x = 153;
	temp_flag.y = 39;
	temp_flag.portal_x = 192;
	temp_flag.portal_y = 114;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 32;
	temp_flag.x = 84;
	temp_flag.y = 30;
	temp_flag.portal_x = 47;
	temp_flag.portal_y = 63;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 33;
	temp_flag.x = 198;
	temp_flag.y = 95;
	temp_flag.portal_x = 47;
	temp_flag.portal_y = 63;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 33;
	temp_flag.x = 291;
	temp_flag.y = 114;
	temp_flag.portal_x = 192;
	temp_flag.portal_y = 114;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 34;
	temp_flag.x = 138;
	temp_flag.y = 103;
	temp_flag.portal_x = 175;
	temp_flag.portal_y = 153;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 34;
	temp_flag.x = 62;
	temp_flag.y = 94;
	temp_flag.portal_x = 38;
	temp_flag.portal_y = 117;
	m_flag[DROBETA].push_back(temp_flag);

	// Ƽ��
	temp_flag.zone_id = 51;
	temp_flag.x = 201;
	temp_flag.y = 36;
	temp_flag.portal_x = 42;
	temp_flag.portal_y = 90;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 51;
	temp_flag.x = 275;
	temp_flag.y = 59;
	temp_flag.portal_x = 188;
	temp_flag.portal_y = 220;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 52;
	temp_flag.x = 66;
	temp_flag.y = 27;
	temp_flag.portal_x = 40;
	temp_flag.portal_y = 48;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 52;
	temp_flag.x = 160;
	temp_flag.y = 51;
	temp_flag.portal_x = 216;
	temp_flag.portal_y = 171;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 53;
	temp_flag.x = 188;
	temp_flag.y = 91;
	temp_flag.portal_x = 17;
	temp_flag.portal_y = 104;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 53;
	temp_flag.x = 293;
	temp_flag.y = 95;
	temp_flag.portal_x = 199;
	temp_flag.portal_y = 110;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 54;
	temp_flag.x = 77;
	temp_flag.y = 83;
	temp_flag.portal_x = 70;
	temp_flag.portal_y = 62;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 54;
	temp_flag.x = 135;
	temp_flag.y = 125;
	temp_flag.portal_x = 178;
	temp_flag.portal_y = 237;
	m_flag[TIMORE].push_back(temp_flag);

	// ��γ�
	temp_flag.zone_id = 61;
	temp_flag.x = 141;
	temp_flag.y = 44;
	temp_flag.portal_x = 44;
	temp_flag.portal_y = 139;
	m_flag[PERONA].push_back(temp_flag);

	temp_flag.zone_id = 62;
	temp_flag.x = 104;
	temp_flag.y = 35;
	temp_flag.portal_x = 93;
	temp_flag.portal_y = 89;
	m_flag[PERONA].push_back(temp_flag);

	temp_flag.zone_id = 63;
	temp_flag.x = 142;
	temp_flag.y = 90;
	temp_flag.portal_x = 57;
	temp_flag.portal_y = 102;
	m_flag[PERONA].push_back(temp_flag);

	temp_flag.zone_id = 64;
	temp_flag.x = 49;
	temp_flag.y = 128;
	temp_flag.portal_x = 40;
	temp_flag.portal_y = 243;
	m_flag[PERONA].push_back(temp_flag);


	/////////////////////////////////////////////////////////////////////////////////////
	// Extend 1 Portal Flag Position

	temp_flag.extend_portal = 1 ; 

	temp_flag.zone_id = 61;
	temp_flag.x = 170;
	temp_flag.y = 34;
	temp_flag.portal_x = 120;
	temp_flag.portal_y = 72;
	m_flag[PERONA].push_back(temp_flag);

	temp_flag.zone_id = 62;
	temp_flag.x = 67;
	temp_flag.y = 56;
	temp_flag.portal_x = 51;
	temp_flag.portal_y = 190;
	m_flag[PERONA].push_back(temp_flag);

	temp_flag.zone_id = 63;
	temp_flag.x = 164;
	temp_flag.y = 76;
	temp_flag.portal_x = 99;
	temp_flag.portal_y = 38;
	m_flag[PERONA].push_back(temp_flag);

	temp_flag.zone_id = 64;
	temp_flag.x = 67;
	temp_flag.y = 90;
	temp_flag.portal_x = 54;
	temp_flag.portal_y = 104;
	m_flag[PERONA].push_back(temp_flag);


	temp_flag.zone_id = 91;
	temp_flag.x = 203;
	temp_flag.y = 44;
	temp_flag.portal_x = 40;
	temp_flag.portal_y = 146;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 91;
	temp_flag.x = 235;
	temp_flag.y = 64;
	temp_flag.portal_x = 100;
	temp_flag.portal_y = 191;
	m_flag[VRANCO].push_back(temp_flag);
	
	temp_flag.zone_id = 92;
	temp_flag.x = 141;
	temp_flag.y = 41;
	temp_flag.portal_x = 174;
	temp_flag.portal_y = 123;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 92;
	temp_flag.x = 80;
	temp_flag.y = 51;
	temp_flag.portal_x = 48;
	temp_flag.portal_y = 180;
	m_flag[VRANCO].push_back(temp_flag);
	
	temp_flag.zone_id = 93;
	temp_flag.x = 277;
	temp_flag.y = 84;
	temp_flag.portal_x = 165;
	temp_flag.portal_y = 67;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 93;
	temp_flag.x = 202;
	temp_flag.y = 96;
	temp_flag.portal_x = 21;
	temp_flag.portal_y = 115;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 94;
	temp_flag.x = 66;
	temp_flag.y = 122;
	temp_flag.portal_x = 56;
	temp_flag.portal_y = 197;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 94;
	temp_flag.x = 163;
	temp_flag.y = 79;
	temp_flag.portal_x = 231;
	temp_flag.portal_y = 45;
	m_flag[VRANCO].push_back(temp_flag);



/*	temp_flag.zone_id = 61;
	temp_flag.x = 170;
	temp_flag.y = 34;
	temp_flag.portal_x = 120;
	temp_flag.portal_y = 72;
	m_flag[PERONA].push_back(temp_flag);

	temp_flag.zone_id = 62;
	temp_flag.x = 67;
	temp_flag.y = 56;
	temp_flag.portal_x = 51;
	temp_flag.portal_y = 190;
	m_flag[PERONA].push_back(temp_flag);

	temp_flag.zone_id = 63;
	temp_flag.x = 164;
	temp_flag.y = 76;
	temp_flag.portal_x = 99;
	temp_flag.portal_y = 38;
	m_flag[PERONA].push_back(temp_flag);

	temp_flag.zone_id = 64;
	temp_flag.x = 67;
	temp_flag.y = 90;
	temp_flag.portal_x = 54;
	temp_flag.portal_y = 104;
	m_flag[PERONA].push_back(temp_flag);

	temp_flag.zone_id = 91;
	temp_flag.x = 203;
	temp_flag.y = 44;
	temp_flag.portal_x = 40;
	temp_flag.portal_y = 146;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 91;
	temp_flag.x = 235;
	temp_flag.y = 64;
	temp_flag.portal_x = 100;
	temp_flag.portal_y = 191;
	m_flag[VRANCO].push_back(temp_flag);
	
	temp_flag.zone_id = 92;
	temp_flag.x = 141;
	temp_flag.y = 41;
	temp_flag.portal_x = 174;
	temp_flag.portal_y = 123;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 92;
	temp_flag.x = 80;
	temp_flag.y = 51;
	temp_flag.portal_x = 48;
	temp_flag.portal_y = 180;
	m_flag[VRANCO].push_back(temp_flag);
	
	temp_flag.zone_id = 93;
	temp_flag.x = 277;
	temp_flag.y = 84;
	temp_flag.portal_x = 165;
	temp_flag.portal_y = 67;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 93;
	temp_flag.x = 202;
	temp_flag.y = 96;
	temp_flag.portal_x = 21;
	temp_flag.portal_y = 115;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 94;
	temp_flag.x = 66;
	temp_flag.y = 122;
	temp_flag.portal_x = 56;
	temp_flag.portal_y = 227;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 94;
	temp_flag.x = 163;
	temp_flag.y = 79;
	temp_flag.portal_x = 231;
	temp_flag.portal_y = 45;
	m_flag[VRANCO].push_back(temp_flag);*/



	
	/////////////////////////////////////////////////////////////////////////////////////
	// Extend 2 Portal Flag Position

	temp_flag.extend_portal = 2 ;

	temp_flag.zone_id = 11;
	temp_flag.x = 240;
	temp_flag.y = 29;
	temp_flag.portal_x = 119;
	temp_flag.portal_y = 38;
	m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 11;
	temp_flag.x = 262;
	temp_flag.y = 60;
	temp_flag.portal_x = 132;
	temp_flag.portal_y = 204;
	m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 12;
	temp_flag.x = 154;
	temp_flag.y = 56;
	temp_flag.portal_x = 203;
	temp_flag.portal_y = 197;
	m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 12;
	temp_flag.x = 68;
	temp_flag.y = 28;
	temp_flag.portal_x = 23;
	temp_flag.portal_y = 52;
	m_flag[ESLANIAN].push_back(temp_flag);


	temp_flag.zone_id = 13;
	temp_flag.x = 313;
	temp_flag.y = 87;
	temp_flag.portal_x = 241;
	temp_flag.portal_y = 76;
	m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 13;
	temp_flag.x = 204;
	temp_flag.y = 100;
	temp_flag.portal_x = 43;
	temp_flag.portal_y = 136;
	m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 14;
	temp_flag.x = 115;
	temp_flag.y = 76;
	temp_flag.portal_x = 139;
	temp_flag.portal_y = 31;
	m_flag[ESLANIAN].push_back(temp_flag);

	temp_flag.zone_id = 14;
	temp_flag.x = 141;
	temp_flag.y = 124;
	temp_flag.portal_x = 188;
	temp_flag.portal_y = 233;
	m_flag[ESLANIAN].push_back(temp_flag);


	temp_flag.zone_id = 21;
	temp_flag.x = 207;
	temp_flag.y = 55;
	temp_flag.portal_x = 50;
	temp_flag.portal_y = 169;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 21;
	temp_flag.x = 268;
	temp_flag.y = 53;
	temp_flag.portal_x = 177;
	temp_flag.portal_y = 172;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 22;
	temp_flag.x = 77;
	temp_flag.y = 42;
	temp_flag.portal_x = 40;
	temp_flag.portal_y = 116;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 22;
	temp_flag.x = 173;
	temp_flag.y = 52;
	temp_flag.portal_x = 240;
	temp_flag.portal_y = 122;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 23;
	temp_flag.x = 313;
	temp_flag.y = 98;
	temp_flag.portal_x = 238;
	temp_flag.portal_y = 112;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 23;
	temp_flag.x = 237;
	temp_flag.y = 87;
	temp_flag.portal_x = 95;
	temp_flag.portal_y = 96;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 24;
	temp_flag.x = 149;
	temp_flag.y = 86;
	temp_flag.portal_x = 204;
	temp_flag.portal_y = 80;
	m_flag[LIMBO].push_back(temp_flag);

	temp_flag.zone_id = 24;
	temp_flag.x = 94;
	temp_flag.y = 114;
	temp_flag.portal_x = 91;
	temp_flag.portal_y = 202;
	m_flag[LIMBO].push_back(temp_flag);


	temp_flag.zone_id = 31;
	temp_flag.x = 235;
	temp_flag.y = 23;
	temp_flag.portal_x = 137;
	temp_flag.portal_y = 40;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 31;
	temp_flag.x = 300;
	temp_flag.y = 57;
	temp_flag.portal_x = 234;
	temp_flag.portal_y = 179;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 32;
	temp_flag.x = 90;
	temp_flag.y = 54;
	temp_flag.portal_x = 81;
	temp_flag.portal_y = 180;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 32;
	temp_flag.x = 158;
	temp_flag.y = 60;
	temp_flag.portal_x = 219;
	temp_flag.portal_y = 207;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 33;
	temp_flag.x = 251;
	temp_flag.y = 75;
	temp_flag.portal_x = 133;
	temp_flag.portal_y = 33;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 33;
	temp_flag.x = 223;
	temp_flag.y = 121;
	temp_flag.portal_x = 90;
	temp_flag.portal_y = 211;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 34;
	temp_flag.x = 88;
	temp_flag.y = 113;
	temp_flag.portal_x = 100;
	temp_flag.portal_y = 187;
	m_flag[DROBETA].push_back(temp_flag);

	temp_flag.zone_id = 34;
	temp_flag.x = 111;
	temp_flag.y = 75;
	temp_flag.portal_x = 127;
	temp_flag.portal_y = 29;
	m_flag[DROBETA].push_back(temp_flag);


	temp_flag.zone_id = 41;
	temp_flag.x = 229;
	temp_flag.y = 29;
	temp_flag.portal_x = 93;
	temp_flag.portal_y = 24;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 41;
	temp_flag.x = 249;
	temp_flag.y = 56;
	temp_flag.portal_x = 135;
	temp_flag.portal_y = 179;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 42;
	temp_flag.x = 69;
	temp_flag.y = 32;
	temp_flag.portal_x = 13;
	temp_flag.portal_y = 66;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 42;
	temp_flag.x = 150;
	temp_flag.y = 57;
	temp_flag.portal_x = 200;
	temp_flag.portal_y = 200;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 43;
	temp_flag.x = 290;
	temp_flag.y = 92;
	temp_flag.portal_x = 194;
	temp_flag.portal_y = 107;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 43;
	temp_flag.x = 229;
	temp_flag.y = 113;
	temp_flag.portal_x = 92;
	temp_flag.portal_y = 189;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 44;
	temp_flag.x = 71;
	temp_flag.y = 100;
	temp_flag.portal_x = 104;
	temp_flag.portal_y = 229;
	m_flag[RODIN].push_back(temp_flag);

	temp_flag.zone_id = 44;
	temp_flag.x = 83;
	temp_flag.y = 67;
	temp_flag.portal_x = 139;
	temp_flag.portal_y = 21;
	m_flag[RODIN].push_back(temp_flag);


	temp_flag.zone_id = 51;
	temp_flag.x = 260;
	temp_flag.y = 26;
	temp_flag.portal_x = 185;
	temp_flag.portal_y = 19;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 51;
	temp_flag.x = 240;
	temp_flag.y = 61;
	temp_flag.portal_x = 118;
	temp_flag.portal_y = 229;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 52;
	temp_flag.x = 82;
	temp_flag.y = 51;
	temp_flag.portal_x = 62;
	temp_flag.portal_y = 180;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 52;
	temp_flag.x = 107;
	temp_flag.y = 30;
	temp_flag.portal_x = 103;
	temp_flag.portal_y = 58;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 53;
	temp_flag.x = 258;
	temp_flag.y = 83;
	temp_flag.portal_x = 143;
	temp_flag.portal_y = 65;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 53;
	temp_flag.x = 209;
	temp_flag.y = 115;
	temp_flag.portal_x = 36;
	temp_flag.portal_y = 195;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 54;
	temp_flag.x = 109;
	temp_flag.y = 100;
	temp_flag.portal_x = 135;
	temp_flag.portal_y = 121;
	m_flag[TIMORE].push_back(temp_flag);

	temp_flag.zone_id = 54;
	temp_flag.x = 139;
	temp_flag.y = 77;
	temp_flag.portal_x = 182;
	temp_flag.portal_y = 20;
	m_flag[TIMORE].push_back(temp_flag);
	
	temp_flag.zone_id = 91;
	temp_flag.x = 280;
	temp_flag.y = 50;
	temp_flag.portal_x = 212;
	temp_flag.portal_y = 117;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 91;
	temp_flag.x = 260;
	temp_flag.y = 34;
	temp_flag.portal_x = 112;
	temp_flag.portal_y = 36;
	m_flag[VRANCO].push_back(temp_flag);
	
	
	temp_flag.zone_id = 92;
	temp_flag.x = 100;
	temp_flag.y = 33;
	temp_flag.portal_x = 104;
	temp_flag.portal_y = 81;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 92;
	temp_flag.x = 171;
	temp_flag.y = 59;
	temp_flag.portal_x = 236;
	temp_flag.portal_y = 213;
	m_flag[VRANCO].push_back(temp_flag);
	
	
	temp_flag.zone_id = 93;
	temp_flag.x = 248;
	temp_flag.y = 111;
	temp_flag.portal_x = 111;
	temp_flag.portal_y = 180;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 93;
	temp_flag.x = 220;
	temp_flag.y = 79;
	temp_flag.portal_x = 60;
	temp_flag.portal_y = 38;
	m_flag[VRANCO].push_back(temp_flag);
	
	temp_flag.zone_id = 94;
	temp_flag.x = 119;
	temp_flag.y = 89;
	temp_flag.portal_x = 139;
	temp_flag.portal_y = 98;
	m_flag[VRANCO].push_back(temp_flag);

	temp_flag.zone_id = 94;
	temp_flag.x = 157;
	temp_flag.y = 130;
	temp_flag.portal_x = 214;
	temp_flag.portal_y = 228;
	m_flag[VRANCO].push_back(temp_flag);


	ofstream flag_file(INF_SLAYER_PORTAL, ios::binary);
	int map_max = MAP_MAX;
	flag_file.write((const char *)&map_max, sizeof(int));
	for(int j = 0; j < MAP_MAX; j++)
	{
		int size = m_flag[j].size();
		flag_file.write((const char *)&size, sizeof(int));
		for(int i = 0; i < size; i++)
		{
			flag_file.write((const char *)&m_flag[j][i].zone_id, sizeof(int));
			flag_file.write((const char *)&m_flag[j][i].x, sizeof(int));
			flag_file.write((const char *)&m_flag[j][i].y, sizeof(int));
			flag_file.write((const char *)&m_flag[j][i].portal_x, sizeof(int));
			flag_file.write((const char *)&m_flag[j][i].portal_y, sizeof(int));
			flag_file.write((const char *)&m_flag[j][i].extend_portal, sizeof(int));
		}
	}
	flag_file.close();
	m_bReady = true;
#else
	m_bReady = LoadPortalFlags();
#endif
}

bool C_VS_UI_SLAYER_PORTAL::LoadPortalFlags()
{
	for (int i = 0; i < MAP_MAX; ++i)
	{
		m_flag[i].clear();
		m_zoneidList[i].clear();
	}

	if (g_pSystemAvailableManager == NULL || g_pZoneTable == NULL)
	{
		Phoenix_LogSlayerPortalError("[SlayerPortal] global contract failed. system=%p zoneTable=%p",
			g_pSystemAvailableManager, g_pZoneTable);
		return false;
	}

	CRarFile flag_file;
	flag_file.SetRAR(RPK_TUTORIAL_ETC, RPK_PASSWORD);
	if (!flag_file.Open(INF_SLAYER_PORTAL))
	{
		Phoenix_LogSlayerPortalError("[SlayerPortal] failed to open %s from %s", INF_SLAYER_PORTAL, RPK_TUTORIAL_ETC);
		return false;
	}

	const char *data = flag_file.GetFilePointer();
	const int dataSize = flag_file.GetSize();
	int offset = 0;
	int map_max = 0;
	if (!Phoenix_ReadSlayerPortalInt(data, dataSize, offset, map_max) || map_max != MAP_MAX)
	{
		Phoenix_LogSlayerPortalError("[SlayerPortal] invalid map count. read=%d expected=%d size=%d", map_max, MAP_MAX, dataSize);
		return false;
	}

	int visibleFlags = 0;
	for (int j = 0; j < MAP_MAX; ++j)
	{
		int count = 0;
		if (!Phoenix_ReadSlayerPortalInt(data, dataSize, offset, count) || count <= 0)
		{
			Phoenix_LogSlayerPortalError("[SlayerPortal] invalid flag count. map=%d count=%d offset=%d size=%d", j, count, offset, dataSize);
			return false;
		}

		const int recordBytes = (int)sizeof(int) * 6;
		if (offset > dataSize || count > (dataSize - offset) / recordBytes)
		{
			Phoenix_LogSlayerPortalError("[SlayerPortal] truncated flag data. map=%d count=%d offset=%d size=%d", j, count, offset, dataSize);
			return false;
		}

		for (int i = 0; i < count; ++i)
		{
			UI_PORTAL_FLAG temp_flag;
			if (!Phoenix_ReadSlayerPortalInt(data, dataSize, offset, temp_flag.zone_id) ||
				!Phoenix_ReadSlayerPortalInt(data, dataSize, offset, temp_flag.x) ||
				!Phoenix_ReadSlayerPortalInt(data, dataSize, offset, temp_flag.y) ||
				!Phoenix_ReadSlayerPortalInt(data, dataSize, offset, temp_flag.portal_x) ||
				!Phoenix_ReadSlayerPortalInt(data, dataSize, offset, temp_flag.portal_y) ||
				!Phoenix_ReadSlayerPortalInt(data, dataSize, offset, temp_flag.extend_portal))
			{
				Phoenix_LogSlayerPortalError("[SlayerPortal] failed to read flag. map=%d index=%d offset=%d size=%d", j, i, offset, dataSize);
				return false;
			}

			if (!Phoenix_IsValidSlayerPortalFlag(temp_flag))
			{
				Phoenix_LogSlayerPortalError("[SlayerPortal] invalid flag skipped. map=%d index=%d zone=%d pos=(%d,%d) portal=(%d,%d) ext=%d",
					j, i, temp_flag.zone_id, temp_flag.x, temp_flag.y, temp_flag.portal_x, temp_flag.portal_y, temp_flag.extend_portal);
				continue;
			}

			if (g_pZoneTable->Get(temp_flag.zone_id) == NULL)
			{
				Phoenix_LogSlayerPortalError("[SlayerPortal] unknown zone skipped. map=%d index=%d zone=%d", j, i, temp_flag.zone_id);
				continue;
			}

			if (std::find(m_zoneidList[j].begin(), m_zoneidList[j].end(), temp_flag.zone_id) == m_zoneidList[j].end())
			{
				m_zoneidList[j].push_back(temp_flag.zone_id);
			}

			if (g_pSystemAvailableManager->ZoneFiltering(temp_flag.zone_id))
			{
				m_flag[j].push_back(temp_flag);
				++visibleFlags;
			}
		}
	}

	if (visibleFlags <= 0)
	{
		Phoenix_LogSlayerPortalError("[SlayerPortal] no visible portal flags after filtering. size=%d", dataSize);
	}

	return true;
}

C_VS_UI_SLAYER_PORTAL::~C_VS_UI_SLAYER_PORTAL()
{
	g_UnregisterWindow(this);

	DeleteNew( m_pC_button_group );
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PORTAL::Process
//
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PORTAL::Process()
{
	if (!m_bReady || m_pC_button_group == NULL)
		return;

	m_pC_button_group->Process();
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PORTAL::UnacquireMouseFocus
//
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PORTAL::UnacquireMouseFocus()
{
	if (!m_bReady || m_pC_button_group == NULL)
		return;

	m_pC_button_group->UnacquireMouseFocus();
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PORTAL::Start
//
//
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PORTAL::Start()
{
	if (!m_bReady || m_pC_button_group == NULL)
	{
		Phoenix_LogSlayerPortalError("[SlayerPortal] Start blocked. ready=%d buttons=%p", m_bReady ? 1 : 0, m_pC_button_group);
		return;
	}

	PI_Processor::Start();
	m_pC_button_group->Init();

	AttrPin(true);
	gpC_window_manager->AppearWindow(this);
	PlaySound(SOUND_WORLD_MAP);

}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PORTAL::Finish
//
//
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PORTAL::Finish()
{
	PI_Processor::Finish();

	gpC_window_manager->DisappearWindow(this);
}


//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PORTAL::Show
//
//
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_PORTAL::Show()
{
	CT_Done("WnSh: C_VS_UI_SLAYER_PORTAL::Show ENTER pthis=%p", (void*)this);
	if (!m_bReady || m_pC_button_group == NULL)
		return;

	static int flag_animation = 0;
	const int flag_frame = 3;
	
	flag_animation++;
	if(flag_animation >= 7*flag_frame)
		flag_animation%=7*flag_frame;

	if(gpC_base->m_p_DDSurface_back->Lock())
	{		
		m_map_spk.BltLocked(x, y, m_map);
		
		for(int i = 0; i < m_flag[m_map].size(); i++)
		{
			if( m_flag[m_map][i].extend_portal > m_nExtendLevel )
				continue;

			int _x = gpC_mouse_pointer->GetX(), _y = gpC_mouse_pointer->GetY();
			
			if(_x >= x+m_flag[m_map][i].x -10 && _x <= x+m_flag[m_map][i].x +10 &&
				_y >= y+m_flag[m_map][i].y -20 && _y <= y+m_flag[m_map][i].y +10)
				m_etc_spk.BltLocked(x+m_flag[m_map][i].x - m_etc_spk.GetWidth(FLAG_HILIGHT)/2-1, y+m_flag[m_map][i].y - m_etc_spk.GetHeight(FLAG_HILIGHT)+3, FLAG_HILIGHT+flag_animation/flag_frame);
			else
				m_etc_spk.BltLocked(x+m_flag[m_map][i].x - m_etc_spk.GetWidth(FLAG)/2-1, y+m_flag[m_map][i].y - m_etc_spk.GetHeight(FLAG)+3, FLAG+flag_animation/flag_frame);
		}		
		m_pC_button_group->Show();
		gpC_base->m_p_DDSurface_back->Unlock();
	}
	m_pC_button_group->ShowDescription();

	SHOW_WINDOW_ATTR;
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PORTAL::ShowDescription
//
//
//-----------------------------------------------------------------------------
void C_VS_UI_SLAYER_PORTAL::ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button)
{
	static char *m_help_string[MAP_MAX+1] = {
		(*g_pGameStringTable)[UI_STRING_MESSAGE_LIMBOLAIR].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_ESLANIA].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_RODIN].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_DROBETA].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_PERONA].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_TIMORE].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_VRANCO].GetString(),
		(*g_pGameStringTable)[UI_STRING_MESSAGE_CANCEL].GetString(),		
	};

	// �ݸ�����
	int map_max = MAP_MAX;
//	if(g_pUserInformation->IsNetmarble)
//		map_max = 4;

	if(m_etc_spk.IsPixel(gpC_mouse_pointer->GetX()-p_button->x, gpC_mouse_pointer->GetY()-p_button->y, p_button->GetID()))
	{
		id_t ButtonID=p_button->GetID();
		if(ButtonID==CLOSE)
			g_descriptor_manager.Set(DID_INFO, p_button->x, p_button->y, (void *)m_help_string[MAP_MAX],0,0);
		else if(ButtonID==LEFT)
		{
			int map = (m_map-1+map_max)%map_max;
			
			if(map == VRANCO  && !m_nExtendLevel)
				map = TIMORE ; 
				
			std::vector<int>::iterator itr = m_zoneidList[map].begin();
			std::vector<int>::iterator enditr =m_zoneidList[map].end();
			
			bool bCheck = false;
			while( itr != enditr )
			{
				if( !g_pSystemAvailableManager->ZoneFiltering( *itr ) )
				{
					bCheck = true;
					break;
				}
				itr++;
			}
			if( bCheck )
				map = GetNext( map, true );
			g_descriptor_manager.Set(DID_INFO, p_button->x, p_button->y, (void *)m_help_string[map],0,0);
		}
		else if(ButtonID==RIGHT)
		{
			int map = (m_map+1)%map_max;

			if(map == VRANCO  && !m_nExtendLevel)
				map = LIMBO ; 

			std::vector<int>::iterator itr = m_zoneidList[map].begin();
			std::vector<int>::iterator enditr =m_zoneidList[map].end();
			bool bCheck = false;
			
			while( itr != enditr )
			{
				if( !g_pSystemAvailableManager->ZoneFiltering( *itr ) )
				{
					bCheck = true;
					break;
				}
				itr++;
			}
			if( bCheck )
				map = GetNext(map, false );
			g_descriptor_manager.Set(DID_INFO, p_button->x, p_button->y, (void *)m_help_string[map],0,0);
		}
	}	
}


//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PORTAL::ShowButtonWidget
//
//
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PORTAL::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
	assert(p_button);

//	if (p_button->GetFocusState())
//		g_descriptor_manager.Set(DID_INFO, p_button->x, p_button->y, (void *)m_help_string[p_button->GetID()-1],0,0);

	if(p_button->GetFocusState() && m_etc_spk.IsPixel(gpC_mouse_pointer->GetX()-p_button->x, gpC_mouse_pointer->GetY()-p_button->y, p_button->GetID()))
	{
		if(p_button->GetPressState())
		{
			switch(p_button->GetID())
			{
			case CLOSE:				
				m_etc_spk.BltLocked(p_button->x, p_button->y, p_button->GetID()+2);
				break;
				
			case LEFT:
					m_etc_spk.BltLocked(p_button->x+2, p_button->y+1, p_button->GetID()+2);
				EMPTY_MOVE;
				break;
				
			case RIGHT:
					m_etc_spk.BltLocked(p_button->x, p_button->y+1, p_button->GetID()+2);
				EMPTY_MOVE;
				break;
			}
		} else
			m_etc_spk.BltLocked(p_button->x, p_button->y, p_button->GetID()+1);
	}
	else
		m_etc_spk.BltLocked(p_button->x, p_button->y, p_button->GetID());

//	if(p_button->GetFocusState() && p_button->GetPressState())m_image_spk.Blt(p_button->x, p_button->y, p_button->m_image_index);
//	else if(m_iLevel == p_button->GetID())
//		m_image_spk.Blt(p_button->x-1, p_button->y-1, p_button->m_image_index);
}

//-----------------------------------------------------------------------------
//
// keyboard
//
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PORTAL::KeyboardControl(UINT message, UINT key, long extra)
{
	Window::KeyboardControl(message, key, extra);

	if (message == WM_KEYDOWN)
	{
		switch (key)
		{
			case VK_ESCAPE: 
				gpC_base->SendMessage(UI_CLOSE_SLAYER_PORTAL);
				break;
		}
	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PORTAL::MouseControl
//
//
//-----------------------------------------------------------------------------
bool	C_VS_UI_SLAYER_PORTAL::MouseControl(UINT message, int _x, int _y)
{
	if (!m_bReady || m_pC_button_group == NULL)
		return false;

	Window::MouseControl(message, _x, _y);

	int flag = -1;
	for(int i = 0; i < m_flag[m_map].size(); i++)
	{
		if( m_flag[m_map][i].extend_portal > m_nExtendLevel )
				continue;

		if(_x >= x+m_flag[m_map][i].x -10 && _x <= x+m_flag[m_map][i].x +10 &&
			_y >= y+m_flag[m_map][i].y -20 && _y <= y+m_flag[m_map][i].y +10)
			flag = i;
	}

	if(flag != -1)
	{
		switch(message)
		{
		case M_MOVING:
			{
				static S_DEFAULT_HELP_STRING flag_string;
				static char flag_temp[50];

				ZONETABLE_INFO *pZoneInfo = (g_pZoneTable != NULL) ? g_pZoneTable->Get(m_flag[m_map][flag].zone_id) : NULL;
				if (pZoneInfo == NULL)
				{
					Phoenix_LogSlayerPortalError("[SlayerPortal] hover zone missing. map=%d flag=%d zone=%d", m_map, flag, m_flag[m_map][flag].zone_id);
					break;
				}

				flag_string.sz_main_str = pZoneInfo->Name.GetString();
				flag_string.sz_sub_str = flag_temp;
				wsprintf(flag_temp, (*g_pGameStringTable)[UI_STRING_MESSAGE_ZONEINFO_XY].GetString(), m_flag[m_map][flag].portal_x, m_flag[m_map][flag].portal_y);

				g_descriptor_manager.Set(DID_HELP, m_flag[m_map][flag].x+x, m_flag[m_map][flag].y+y-m_etc_spk.GetHeight(FLAG)-3, (void *)&flag_string);
			}
			break;

		case M_LEFTBUTTON_DOWN:
		case M_LB_DOUBLECLICK:
			{
				int left = m_flag[m_map][flag].zone_id;
				int right = ((m_flag[m_map][flag].portal_x)<<16) + m_flag[m_map][flag].portal_y;
				gpC_base->SendMessage(UI_SLAYER_PORTAL, left, right);
			}
			break;
		}
	}

	m_pC_button_group->MouseControl(message, _x, _y);

	return true;
}

//-----------------------------------------------------------------------------
// C_VS_UI_ELEVATOR::IsPixel
//
//
//-----------------------------------------------------------------------------
bool	C_VS_UI_SLAYER_PORTAL::IsPixel(int _x, int _y)
{
	if (!m_bReady)
		return false;

	bool re = m_map_spk.IsPixel(SCR2WIN_X(_x), SCR2WIN_Y(_y));

	if(re == false)
	{
		if(m_etc_spk.IsPixel(_x - m_left_x-x, _y - m_left_y-y, LEFT) ||
			m_etc_spk.IsPixel(_x - m_right_x-x, _y - m_right_y-y, RIGHT) ||
			m_etc_spk.IsPixel(_x - m_close_x-x, _y - m_close_y-y, CLOSE))
			re = true;
	}

	return re;
}

int		C_VS_UI_SLAYER_PORTAL::GetNext(int map, bool bLeft)
{
	for(int i = 1; i < MAP_MAX; i++ )
	{
		int nmap;
		
		if( bLeft )
			nmap = (map-i+MAP_MAX)%MAP_MAX;
		else
			nmap = (map+i)%MAP_MAX;
		
		bool bCheck = true;
		std::vector<int>::iterator itr = m_zoneidList[nmap].begin();
		std::vector<int>::iterator enditr =m_zoneidList[nmap].end();
		
		while( itr != enditr )
		{
			if( !g_pSystemAvailableManager->ZoneFiltering( *itr ) )
			{
				bCheck = false;
				break;
			}
			itr++;
		}
		if( bCheck )
			return nmap;
		
	}
	if( bLeft )
		return (map+1)%MAP_MAX;
	
	return (map-1+MAP_MAX)%MAP_MAX;
}

//-----------------------------------------------------------------------------
// C_VS_UI_SLAYER_PORTAL::Run
//
//
//-----------------------------------------------------------------------------
void	C_VS_UI_SLAYER_PORTAL::Run(id_t id)
{
	if (!m_bReady || m_pC_button_group == NULL)
	{
		Phoenix_LogSlayerPortalError("[SlayerPortal] Run blocked. ready=%d buttons=%p id=%d", m_bReady ? 1 : 0, m_pC_button_group, id);
		return;
	}
	// �ݸ�����
	int map_max = MAP_MAX;
//	if(g_pUserInformation->IsNetmarble)
//		map_max = 4;

	C_VS_UI_EVENT_BUTTON *p_button = NULL;
	if (!m_pC_button_group->Data(id/3, p_button) || p_button == NULL)
	{
		Phoenix_LogSlayerPortalError("[SlayerPortal] button lookup failed. id=%d", id);
		return;
	}

	if(m_etc_spk.IsPixel(gpC_mouse_pointer->GetX()-p_button->x, gpC_mouse_pointer->GetY()-p_button->y, id))
	switch (id)
	{
	case LEFT:
		PlaySound(SOUND_WORLD_MAP);
		m_map = (m_map-1+map_max)%map_max;

		if(m_map == VRANCO  && !m_nExtendLevel)
				m_map = TIMORE ; 

		{
			bool bCannot = false;
			std::vector<int>::iterator itr = m_zoneidList[m_map].begin();
			std::vector<int>::iterator enditr =m_zoneidList[m_map].end();
			
			while( itr != enditr )
			{
				if( !g_pSystemAvailableManager->ZoneFiltering( *itr ) )
				{
					bCannot = true;
					break;
				}
				itr++;
			}
			
			if( bCannot )
				m_map = GetNext( m_map, true );
		}
		break;

	case RIGHT:
		PlaySound(SOUND_WORLD_MAP);
		m_map = (m_map+1)%map_max;
		if(m_map == VRANCO  && !m_nExtendLevel)
				m_map = LIMBO ; 

		{
			bool bCannot = false;
			std::vector<int>::iterator itr = m_zoneidList[m_map].begin();
			std::vector<int>::iterator enditr =m_zoneidList[m_map].end();
			
			while( itr != enditr )
			{
				if( !g_pSystemAvailableManager->ZoneFiltering( *itr ) )
				{
					bCannot = true;
					break;
				}
				itr++;
			}
			if( bCannot )
				m_map = GetNext( m_map, false);
		}
		break;

	case CLOSE:
		gpC_base->SendMessage(UI_CLOSE_SLAYER_PORTAL);
		break;
	}
}
