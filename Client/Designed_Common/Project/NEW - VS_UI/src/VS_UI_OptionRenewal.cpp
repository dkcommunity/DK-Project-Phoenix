/*-----------------------------------------------------------------------------

	VS_UI_OptionRenewal.cpp

	Reborn Option menusu portu (E:\DK Phoenix Reborn -> Legacy).
	2026-09-02 — Plan: reborn_option_menü_portu

	Yerlesim degerleri Reborn Application.cpp'deki OptionSpriteMetadata /
	Option*Controls tablolarindan birebir alinmistir (1280x960 referans
	canvas). Pencere sol-ust referansi (396,258); tum rect'ler RX/RY
	yardimcilariyla ekrana tasinir, pencere ekran ortasina kurulur.

	Reborn'da olmayan eski-menu ozellikleri kaybolmaz (plan geregi):
	  - Enter-chat        -> Game sekmesine ek satir
	  - IFeel             -> Advanced sekmesine ek satir
	  - Critical hit fx   -> Advanced sekmesine ek satir
	  - Vignette          -> Advanced2 sekmesine ek satir
	  - Tus sifirlama     -> Hotkey penceresi Default/Reset butonu

-----------------------------------------------------------------------------*/

#include "client_PCH.h"
#include "MTopView.h"
#include "VS_UI.h"
#include "crash_trace_tool.h"

#include "VS_UI_filepath.h"
#include "VS_UI_Message.h"
#include "VS_UI_Mouse_pointer.h"
#include "VS_UI_GameCommon.h"
#include "UserOption.h"
#include "ClientConfig.h"
#include "BGMManager.h"
#include "DXLib.h"

#include "KeyAccelerator.h"
#include "UserInformation.h"
#include "mgamestringtable.H"

#include "VS_UI_NewTitle.h"		// C_VS_UI_OPTION enum'lari (UI_CHANGE_OPTION sozlesmesi)
#include "VS_UI_OptionRenewal.h"

// 2026-09-02 O2 — runtime kablolama: FPS limit/pacing + render ayarlari
#include "PhoenixFramePacer.h"
#include "DXLib/IRenderBackend.h"
// 2026-09-02 O3 — canli cozunurluk
#include "PhoenixResolutionLive.h"
extern bool g_bFullScreen;	// Client.cpp — resolution.inf FullScreen (startup gercegi)

#include <windows.h>
#include <stdio.h>
#include <string.h>

// D3D9 vignette toggle (VS_UI d3d9.h cekmek istemiyor — bkz. VS_UI_NewTitle.cpp)
extern "C" void D3D9PostProcess_SetVignetteEnabled(bool enabled);

// Tus sifirlama onay dialogu callback'i (VS_UI_NewTitle.cpp'de tanimli, aktif)
void ExecF_OptionResetButton(C_VS_UI_DIALOG * p_this_dialog, id_t id);

namespace
{
	// ------------------------------------------------------------------
	// Reborn referans canvas sabitleri
	// ------------------------------------------------------------------
	const int kRefWinX = 396;	// pencere sol-ust (1280x960 canvas)
	const int kRefWinY = 258;
	const int kWinW = 489;
	const int kWinH = 444;

	const int kRefHkX = 153;	// hotkey penceresi sol-ust
	const int kRefHkY = 253;
	const int kHkW = 974;
	const int kHkH = 454;

	// Reborn flag bit'leri
	const unsigned int kGraphVSyncBit           = 1u << 0;
	const unsigned int kGraphTransHPBarBit      = 1u << 1;
	const unsigned int kGraphBloodBit           = 1u << 2;
	const unsigned int kGraphSmoothAutoHideBit  = 1u << 3;
	const unsigned int kGraphChatBalloonLineBit = 1u << 4;
	const unsigned int kGraphShowFpsBit         = 1u << 5;
	const unsigned int kGraphHideShopMsgBit     = 1u << 6;
	const unsigned int kGraphHideOtherFxBit     = 1u << 7;
	const unsigned int kGraphHideOtherPetsBit   = 1u << 8;

	const unsigned int kGameHelperTipsBit       = 1u << 0;
	const unsigned int kGameNameLabelsBit       = 1u << 1;
	const unsigned int kGameDamageNumbersBit    = 1u << 2;
	const unsigned int kGameChatBalloonsBit     = 1u << 3;
	const unsigned int kGameSystemMessagesBit   = 1u << 4;
	const unsigned int kGameGuildMarksBit       = 1u << 5;
	const unsigned int kGamePartyAlertsBit      = 1u << 6;
	const unsigned int kGameConfirmDiscardBit   = 1u << 7;
	const unsigned int kGameBlockTradeBit       = 1u << 8;

	const unsigned int kAdvBackgroundLoadBit    = 1u << 0;
	const unsigned int kAdvNpcNamesBit          = 1u << 1;
	const unsigned int kAdvChainEffectsBit      = 1u << 2;
	const unsigned int kAdvReduceFlashBit       = 1u << 3;
	const unsigned int kAdvUiAnimationBit       = 1u << 4;
	const unsigned int kAdvLinearFilterBit      = 1u << 5;
	const unsigned int kAdvSkipIntroBit         = 1u << 6;

	const unsigned int kAdv2SmoothPacingBit     = 1u << 0;
	const unsigned int kAdv2SpriteLinearBit     = 1u << 2;
	const unsigned int kAdv2TextureCacheHighBit = 1u << 3;
	const unsigned int kAdv2BorderlessBit       = 1u << 4;
	const unsigned int kAdv2HqUiScalingBit      = 1u << 5;
	const unsigned int kAdv2PixelPerfectBit     = 1u << 6;

	const unsigned int kDefaultGraphFlags = 0x000001DFu;
	const unsigned int kDefaultGameFlags  = 0x000001FFu;
	const unsigned int kDefaultAdvFlags   = 0x0000003Fu;
	const unsigned int kDefaultAdv2Flags  = kAdv2SpriteLinearBit | kAdv2HqUiScalingBit;

	const BYTE kDefaultAlphaDepth = 23;	// Reborn LegacyWindowTransparency::DefaultAlphaDepth
	const int  kMaxAlphaDepth = 31;		// legacy ALPHA_DEPTH ust siniri

	// Slider surukleme hedefleri
	enum DRAG_TARGET
	{
		DRAG_NONE = 0,
		DRAG_OPACITY,
		DRAG_BGM,
		DRAG_SFX,
		DRAG_GAMMA,
		DRAG_BRIGHTNESS
	};

	// ------------------------------------------------------------------
	// Yerlesim tablolari (Reborn fallback rect'leri, referans canvas)
	// ------------------------------------------------------------------
	struct RefRect { int x, y, w, h; };

	// Ortak
	// 2026-09-02: Koordinatlar E:'nin GameData\interface.ini [Option_1280x960]
	// bolumunden alindi (Reborn LayoutRect override'lari). Application.cpp'deki
	// fallback tablolar E:'de fiilen KULLANILMIYOR — gercek gorunum ini'den gelir.
	const RefRect kRcCloseBtn       = { 864, 271, 12, 10 };
	const RefRect kRcTab[5]         = { { 418, 304, 52, 22 }, { 475, 304, 52, 22 }, { 532, 304, 52, 22 }, { 589, 304, 52, 22 }, { 646, 304, 52, 22 } };
	const RefRect kRcResetBtn       = { 418, 672, 68, 24 };
	const RefRect kRcCloseActionBtn = { 795, 672, 68, 24 };
	const RefRect kRcTitleText      = { 406, 264, 160, 20 };
	const RefRect kRcResetText      = { 426, 676, 54, 18 };
	const RefRect kRcCloseText      = { 806, 676, 44, 18 };
	const RefRect kRcTabText[5]     = { { 423, 308, 44, 16 }, { 480, 308, 44, 16 }, { 537, 308, 44, 16 }, { 594, 308, 44, 16 }, { 651, 308, 44, 16 } };
	// Ayirac cizgileri — sekmeye ozel (interface.ini'de tab basina farkli)
	// [tab][0..3] yatay, [tab][4] dikey. Tab sirasi: GRAPH,SOUND,GAME,ADV,ADV2
	const RefRect kRcSeps[5][5] = {
		{ { 397, 292, 489, 1 }, { 397, 326, 488, 2 }, { 397, 331, 488, 2 }, { 397, 429, 488, 2 }, { 510, 332, 2, 368 } },	// GRAPH
		{ { 397, 292, 489, 1 }, { 397, 326, 488, 2 }, { 397, 331, 488, 2 }, { 397, 423, 488, 2 }, { 510, 332, 2, 368 } },	// SOUND
		{ { 400, 292, 480, 1 }, { 400, 330, 480, 1 }, { 400, 386, 480, 1 }, { 400, 440, 480, 1 }, { 510, 330, 1, 370 } },	// GAME
		{ { 397, 292, 489, 1 }, { 397, 326, 488, 2 }, { 397, 331, 488, 2 }, { 397, 503, 488, 2 }, { 510, 332, 2, 368 } },	// ADV
		{ { 400, 292, 480, 1 }, { 398, 330, 486, 1 }, { 398, 335, 486, 1 }, { 398, 440, 486, 1 }, { 510, 330, 1, 370 } },	// ADV2
	};

	// Graphics sekmesi (interface.ini degerleri)
	const RefRect kRcGfxResBox      = { 520, 334, 182, 24 };
	const RefRect kRcGfxResArrow    = { 685, 340, 12, 11 };
	const RefRect kRcGfxResRow[4]   = { { 520, 358, 182, 22 }, { 520, 380, 182, 22 }, { 520, 403, 182, 22 }, { 520, 426, 182, 22 } };
	const RefRect kRcGfxFullscreen  = { 716, 335, 22, 22 };
	const RefRect kRcGfxFrameLimit  = { 520, 358, 110, 24 };
	const RefRect kRcGfxWindowed    = { 520, 383, 22, 22 };
	const RefRect kRcGfxBorderless  = { 638, 383, 22, 22 };
	const RefRect kRcGfxDefAlpha    = { 716, 404, 22, 22 };
	const RefRect kRcGfxOpacityBar  = { 520, 408, 182, 12 };
	const RefRect kRcGfxEffect[9]   = { { 520, 434, 22, 22 }, { 520, 457, 22, 22 }, { 520, 481, 22, 22 }, { 520, 504, 22, 22 }, { 520, 527, 22, 22 },
	                                    { 520, 550, 22, 22 }, { 520, 573, 22, 22 }, { 520, 596, 22, 22 }, { 520, 620, 22, 22 } };
	const RefRect kRcGfxEffectLbl[9] = { { 544, 436, 260, 18 }, { 543, 459, 260, 18 }, { 543, 482, 260, 18 }, { 543, 505, 260, 18 }, { 542, 529, 260, 18 },
	                                     { 542, 552, 260, 18 }, { 541, 575, 260, 18 }, { 543, 598, 260, 18 }, { 543, 622, 260, 18 } };
	const char* kGfxEffectLabel[9]  = { "VSync", "Transparent HP(MP) bar", "Blood flash below 30% HP", "Smooth auto-hide windows",
	                                    "Chat balloon line", "Show FPS/Ping", "Hide shop messages", "Hide other user effects", "Hide other user pets" };

	// Sound sekmesi
	const RefRect kRcSndBgmBar      = { 520, 360, 227, 10 };
	const RefRect kRcSndBgmCheck    = { 772, 354, 22, 22 };
	const RefRect kRcSndSfxBar      = { 520, 406, 227, 10 };
	const RefRect kRcSndSfxCheck    = { 772, 400, 22, 22 };

	// Game sekmesi (interface.ini degerleri)
	const RefRect kRcGameCheck[9]   = { { 520, 337, 22, 22 }, { 520, 361, 22, 22 }, { 520, 385, 22, 22 }, { 520, 409, 22, 22 }, { 520, 433, 22, 22 },
	                                    { 520, 456, 22, 22 }, { 520, 479, 22, 22 }, { 520, 502, 22, 22 }, { 520, 525, 22, 22 } };
	const RefRect kRcGameCheckLbl[9] = { { 548, 337, 250, 18 }, { 548, 363, 250, 18 }, { 548, 387, 250, 18 }, { 548, 411, 250, 18 }, { 548, 434, 250, 18 },
	                                     { 548, 458, 250, 18 }, { 548, 483, 250, 18 }, { 548, 506, 250, 18 }, { 548, 530, 250, 18 } };
	const char* kGameLabel[9]       = { "Show helper tips", "Show name labels", "Show damage numbers", "Show chat balloons", "Show system messages",
	                                    "Show other guild marks", "Show party request alerts", "Confirm item discard", "Block trade requests" };
	const RefRect kRcGameKeyboard   = { 526, 630, 259, 32 };
	const RefRect kRcGameKeyboardLbl = { 573, 637, 150, 18 };
	const RefRect kRcGameEnterChat  = { 520, 556, 22, 22 };	// Legacy ek satiri (checklerin alti, klavye butonunun ustu)

	// Advanced sekmesi (interface.ini degerleri)
	const RefRect kRcAdvCheck[7]    = { { 520, 337, 22, 22 }, { 520, 361, 22, 22 }, { 520, 385, 22, 22 }, { 520, 409, 22, 22 },
	                                    { 520, 433, 22, 22 }, { 520, 456, 22, 22 }, { 520, 479, 22, 22 } };
	const RefRect kRcAdvCheckLbl[7] = { { 548, 339, 300, 18 }, { 548, 363, 260, 18 }, { 548, 387, 260, 18 }, { 548, 411, 260, 18 },
	                                    { 548, 434, 260, 18 }, { 548, 458, 260, 18 }, { 548, 482, 260, 18 } };
	const char* kAdvLabel[7]        = { "Display high-quality images during loading.", "Show all NPC names", "Enable chain effects",
	                                    "Reduce spell flashes", "Keep UI animation", "Use linear sprite filter", "Skip intro media" };
	// Legacy ek satirlari: ADV content ayiraci (y=503) altindaki bos bolge
	const RefRect kRcAdvIFeel       = { 520, 512, 22, 22 };	// Legacy ek satiri
	const RefRect kRcAdvCritical    = { 520, 536, 22, 22 };	// Legacy ek satiri
	const RefRect kRcAdvFilter      = { 520, 560, 22, 22 };	// Legacy ek: kufur filtresi
	const RefRect kRcAdvChatWhite   = { 520, 584, 22, 22 };	// Legacy ek: beyaz chat yazisi
	const RefRect kRcAdvWhisperPop  = { 520, 608, 22, 22 };	// Legacy ek: fisilti ile chat ac

	// Advanced2 sekmesi
	const RefRect kRcA2Pacing       = { 520, 350, 22, 22 };
	const RefRect kRcA2Nearest      = { 520, 384, 22, 22 };
	const RefRect kRcA2Linear       = { 650, 384, 22, 22 };
	const RefRect kRcA2TexNormal    = { 520, 418, 22, 22 };
	const RefRect kRcA2TexHigh      = { 650, 418, 22, 22 };
	const RefRect kRcA2GammaBar     = { 520, 464, 182, 12 };
	const RefRect kRcA2BrightBar    = { 520, 508, 182, 12 };
	const RefRect kRcA2HqUi         = { 520, 552, 22, 22 };
	const RefRect kRcA2PixelPerfect = { 520, 586, 22, 22 };
	const RefRect kRcA2Vignette     = { 520, 620, 22, 22 };	// Legacy ek satiri

	// Cozunurluk secenekleri — Client.cpp kPhoenixResolutionModes whitelist'i
	// ile birebir ayni (1024x768 whitelist disi, migrate ediliyordu; O3'te
	// 1440x900 ile degistirildi).
	struct ResChoice { int w, h; const char* label; };
	// 2026-09-03 R2 — kullanici istegi: liste kucukten buyuge (genislik,
	// sonra yukseklik). Dizi sirasi = dropdown satir sirasi (draw + hit
	// test ayni index'i kullanir, ek esleme gerekmez).
	const ResChoice kResChoices[4] = {
		{ 1280,  960, "1280 x 960"  },
		{ 1280, 1024, "1280 x 1024" },
		{ 1440,  900, "1440 x 900"  },
		{ 1920, 1080, "1920 x 1080" }
	};

	const char* ResolutionLabel(int w, int h)
	{
		for (int i = 0; i < 4; i++)
		{
			if (kResChoices[i].w == w && kResChoices[i].h == h)
				return kResChoices[i].label;
		}
		return "1280 x 960";
	}

	// ------------------------------------------------------------------
	// Hotkey penceresi klavye tablosu (Reborn HotkeySettingControls)
	// ------------------------------------------------------------------
	struct HkKey { const char* label; int sprite; RefRect rc; };
	const HkKey kHkKeys[] = {
		{ "ESC",   2, { 176, 304, 51, 51 } },  { "F1",  2, { 262, 304, 51, 51 } },  { "F2",  2, { 318, 304, 51, 51 } },
		{ "F3",    2, { 374, 304, 51, 51 } },  { "F4",  2, { 430, 304, 51, 51 } },  { "F5",  2, { 486, 304, 51, 51 } },
		{ "F6",    2, { 542, 304, 51, 51 } },  { "F7",  2, { 598, 304, 51, 51 } },  { "F8",  2, { 654, 304, 51, 51 } },
		{ "F9",    2, { 710, 304, 51, 51 } },  { "F10", 2, { 766, 304, 51, 51 } },  { "F11", 2, { 822, 304, 51, 51 } },
		{ "F12",   2, { 878, 304, 51, 51 } },  { "Print", 5, { 944, 304, 63, 51 } }, { "Scroll", 5, { 1012, 304, 63, 51 } },
		{ "Pause", 5, { 1080, 304, 63, 51 } },
		{ "`", 2, { 176, 360, 51, 51 } }, { "1", 2, { 232, 360, 51, 51 } }, { "2", 2, { 288, 360, 51, 51 } },
		{ "3", 2, { 344, 360, 51, 51 } }, { "4", 2, { 400, 360, 51, 51 } }, { "5", 2, { 456, 360, 51, 51 } },
		{ "6", 2, { 512, 360, 51, 51 } }, { "7", 2, { 568, 360, 51, 51 } }, { "8", 2, { 624, 360, 51, 51 } },
		{ "9", 2, { 680, 360, 51, 51 } }, { "0", 2, { 736, 360, 51, 51 } }, { "-", 2, { 792, 360, 51, 51 } },
		{ "=", 2, { 848, 360, 51, 51 } }, { "Back", 17, { 904, 360, 124, 51 } },
		{ "Tab", 5, { 176, 416, 63, 51 } },
		{ "Q", 2, { 244, 416, 51, 51 } }, { "W", 2, { 300, 416, 51, 51 } }, { "E", 2, { 356, 416, 51, 51 } },
		{ "R", 2, { 412, 416, 51, 51 } }, { "T", 2, { 468, 416, 51, 51 } }, { "Y", 2, { 524, 416, 51, 51 } },
		{ "U", 2, { 580, 416, 51, 51 } }, { "I", 2, { 636, 416, 51, 51 } }, { "O", 2, { 692, 416, 51, 51 } },
		{ "P", 2, { 748, 416, 51, 51 } }, { "[", 2, { 804, 416, 51, 51 } }, { "]", 2, { 860, 416, 51, 51 } },
		{ "\\", 8, { 916, 416, 93, 51 } },
		{ "Caps", 14, { 176, 472, 77, 51 } },
		{ "A", 2, { 258, 472, 51, 51 } }, { "S", 2, { 314, 472, 51, 51 } }, { "D", 2, { 370, 472, 51, 51 } },
		{ "F", 2, { 426, 472, 51, 51 } }, { "G", 2, { 482, 472, 51, 51 } }, { "H", 2, { 538, 472, 51, 51 } },
		{ "J", 2, { 594, 472, 51, 51 } }, { "K", 2, { 650, 472, 51, 51 } }, { "L", 2, { 706, 472, 51, 51 } },
		{ ";", 2, { 762, 472, 51, 51 } }, { "'", 2, { 818, 472, 51, 51 } }, { "Enter", 17, { 874, 472, 124, 51 } },
		{ "Shift", 17, { 176, 528, 124, 51 } },
		{ "Z", 2, { 305, 528, 51, 51 } }, { "X", 2, { 361, 528, 51, 51 } }, { "C", 2, { 417, 528, 51, 51 } },
		{ "V", 2, { 473, 528, 51, 51 } }, { "B", 2, { 529, 528, 51, 51 } }, { "N", 2, { 585, 528, 51, 51 } },
		{ "M", 2, { 641, 528, 51, 51 } }, { ",", 2, { 697, 528, 51, 51 } }, { ".", 2, { 753, 528, 51, 51 } },
		{ "/", 2, { 809, 528, 51, 51 } }, { "Shift", 17, { 865, 528, 124, 51 } }, { "Up", 2, { 996, 528, 51, 51 } },
		{ "Ctrl", 5, { 176, 584, 63, 51 } }, { "Alt", 5, { 244, 584, 63, 51 } }, { "Space", 23, { 312, 584, 225, 51 } },
		{ "Alt", 5, { 542, 584, 63, 51 } }, { "Ctrl", 5, { 610, 584, 63, 51 } },
		{ "Left", 2, { 928, 584, 51, 51 } }, { "Down", 2, { 996, 584, 51, 51 } }, { "Right", 2, { 1064, 584, 51, 51 } }
	};
	const int kHkKeyCount = (int)(sizeof(kHkKeys) / sizeof(kHkKeys[0]));

	// Hotkey penceresi alt butonlari (sprite 26 = kucuk buton)
	const RefRect kRcHkClose   = { 1098, 262, 14, 14 };
	const RefRect kRcHkCancel  = { 492, 650, 68, 24 };
	const RefRect kRcHkSave    = { 566, 650, 68, 24 };
	const RefRect kRcHkReset   = { 640, 650, 68, 24 };
	const RefRect kRcHkDefault = { 714, 650, 68, 24 };
	const RefRect kRcHkTitle   = { 608, 264, 180, 20 };

	// Metin renkleri
	const COLORREF kClrLabel = RGB(200, 200, 200);
	const COLORREF kClrTitle = RGB(255, 255, 255);
	const COLORREF kClrValue = RGB(255, 255, 255);

	// OptionRenewal.set yolu (client CWD = GameData)
	const char* kRenewalSetPath = "UserSet\\OptionRenewal.set";

	bool RectHit(const RefRect& rc, int refX, int refY)
	{
		return refX >= rc.x && refX < rc.x + rc.w && refY >= rc.y && refY < rc.y + rc.h;
	}
}

/*-----------------------------------------------------------------------------
- ctor / dtor
-----------------------------------------------------------------------------*/
C_VS_UI_OPTION_RENEWAL::C_VS_UI_OPTION_RENEWAL(bool IsTitle)
{
	CT_Done("OPTREN::ctor ENTER IsTitle=%d", IsTitle ? 1 : 0);

	m_IsTitle = IsTitle;
	m_pC_dialog = NULL;

	m_iTab = TAB_GRAPHICS;
	m_bDropdownOpen = false;
	m_bHotkeyOpen = false;
	m_bLBPush = false;
	m_iDragSlider = DRAG_NONE;

	// Once dosya (Reborn'a ozgu alanlar + default'lar), sonra legacy aynalar
	m_graphFlags = kDefaultGraphFlags;
	m_gameFlags = kDefaultGameFlags;
	m_advFlags = kDefaultAdvFlags;
	m_adv2Flags = kDefaultAdv2Flags;
	m_frameLimit = 60;
	m_bLinearFilter = true;
	m_brightness = 50;
	m_pendingResW = g_pUserInformation->iResolution_x;
	m_pendingResH = g_pUserInformation->iResolution_y;
	m_bFullscreen = true;

	LoadRenewalSet();
	LoadFromLegacy();

	// O3 — fullscreen'in gercek kaynagi resolution.inf (g_bFullScreen);
	// OptionRenewal.set'teki eski deger startup gercegini ezmesin.
	m_bFullscreen = g_bFullScreen;

	// Sprite pack'ler.
	// renewaloption: Reborn'un fiilen yukledigi tek dosya 00000.spk (33
	// frame, OptionSpriteMetadata sirasi). 00016/00032 chunk'lari ESKI bir
	// asset varyanti (frame sirasi farkli — 4 close/4 tab/8 check), onlar
	// KULLANILMAZ. CTypePack2::LoadFromFile boyutu dosyadan okur, 33 gelir.
	m_pSpk = new C_SPRITE_PACK(SPK_RENEWAL_OPTION);
	// hotkeysettingwindow: 32 frame, 16'sar iki chunk — sira Reborn
	// HotkeySettingSpriteMetadata ile birebir dogrulandi.
	m_pHotkeySpk[0] = new C_SPRITE_PACK(SPK_HOTKEY_SETTING_0);
	m_pHotkeySpk[1] = new C_SPRITE_PACK(SPK_HOTKEY_SETTING_16);

	// Pencere ekran ortasina
	Set(g_pUserInformation->iResolution_x / 2 - kWinW / 2,
		g_pUserInformation->iResolution_y / 2 - kWinH / 2,
		kWinW, kWinH);

	AttrTopmost(true);
	g_RegisterWindow(this);
	CT_Done("OPTREN::ctor EXIT");
}

C_VS_UI_OPTION_RENEWAL::~C_VS_UI_OPTION_RENEWAL()
{
	CT_Done("OPTREN::dtor ENTER");

	// Reborn'a ozgu alanlar diske; legacy alanlar Execute_UI_CLOSE_OPTION'da
	// UserOption.set'e yazilir (asagidaki SendMessage tetikler).
	SaveRenewalSet();

	// O3 — resolution.inf menü state'iyle tutarli kalsin (pending cozunurluk
	// + fullscreen tercihi; canli uygulanan degerlerle de ayni).
	Phoenix_ResolutionLive_SavePending(m_pendingResW, m_pendingResH,
	                                   m_bFullscreen ? 1 : 0);

	gpC_base->SendMessage(UI_CLOSE_OPTION);
	gC_vs_ui.UnSetAccelMode();

	g_UnregisterWindow(this);

	DeleteNew(m_pC_dialog);
	DeleteNew(m_pSpk);
	for (int i = 0; i < 2; i++)
		DeleteNew(m_pHotkeySpk[i]);

	CT_Done("OPTREN::dtor EXIT");
}

/*-----------------------------------------------------------------------------
- PI / Window sozlesmesi
-----------------------------------------------------------------------------*/
void C_VS_UI_OPTION_RENEWAL::Start()
{
	m_bLBPush = false;
	m_iDragSlider = DRAG_NONE;
	PI_Processor::Start();
	gpC_window_manager->AppearWindow(this);
}

void C_VS_UI_OPTION_RENEWAL::Finish()
{
	PI_Processor::Finish();
	gpC_window_manager->DisappearWindow(this);
}

void C_VS_UI_OPTION_RENEWAL::Process()
{
}

bool C_VS_UI_OPTION_RENEWAL::IsPixel(int _x, int _y)
{
	if (m_bHotkeyOpen)
	{
		int hx0 = HX(kRefHkX), hy0 = HY(kRefHkY);
		if (_x >= hx0 && _x < hx0 + kHkW && _y >= hy0 && _y < hy0 + kHkH)
			return true;
	}
	return _x >= x && _x < x + w && _y >= y && _y < y + h;
}

void C_VS_UI_OPTION_RENEWAL::KeyboardControl(UINT message, UINT key, long extra)
{
	if (message == WM_KEYDOWN && key == VK_ESCAPE)
	{
		if (m_bHotkeyOpen)
			m_bHotkeyOpen = false;
		else
			RequestClose();
	}
}

// Vs_ui.cpp uyumu — hotkey toggle sonrasi menu acikken isaret guncel kalsin.
// Yeni menu durumu her Show()'da g_pUserOption'dan okudugu icin no-op yeterli;
// yine de aynalari tazeleyelim.
void C_VS_UI_OPTION_RENEWAL::Background_Music_ONOFF(int /*check_id*/)
{
	m_volume_music = g_pUserOption->VolumeMusic;
}

void C_VS_UI_OPTION_RENEWAL::Effect_Music_ONOFF(int /*check_id*/)
{
	m_volume_sound = g_pUserOption->VolumeSound;
}

/*-----------------------------------------------------------------------------
- Yerlesim yardimcilari
-----------------------------------------------------------------------------*/
int C_VS_UI_OPTION_RENEWAL::RX(int refX) const { return x + (refX - kRefWinX); }
int C_VS_UI_OPTION_RENEWAL::RY(int refY) const { return y + (refY - kRefWinY); }

int C_VS_UI_OPTION_RENEWAL::HX(int refX) const
{
	return g_pUserInformation->iResolution_x / 2 - kHkW / 2 + (refX - kRefHkX);
}

int C_VS_UI_OPTION_RENEWAL::HY(int refY) const
{
	return g_pUserInformation->iResolution_y / 2 - kHkH / 2 + (refY - kRefHkY);
}

bool C_VS_UI_OPTION_RENEWAL::HitR(int refX, int refY, int refW, int refH, int sx, int sy) const
{
	int px = RX(refX), py = RY(refY);
	return sx >= px && sx < px + refW && sy >= py && sy < py + refH;
}

bool C_VS_UI_OPTION_RENEWAL::HitH(int refX, int refY, int refW, int refH, int sx, int sy) const
{
	int px = HX(refX), py = HY(refY);
	return sx >= px && sx < px + refW && sy >= py && sy < py + refH;
}

/*-----------------------------------------------------------------------------
- Cizim yardimcilari
-----------------------------------------------------------------------------*/
void C_VS_UI_OPTION_RENEWAL::BltFrame(int frame, int sx, int sy)
{
	// Tek pack, 33 frame — dogrudan global indeks
	if (m_pSpk)
		m_pSpk->BltLocked(sx, sy, frame);
}

void C_VS_UI_OPTION_RENEWAL::BltFrameClip(int frame, int sx, int sy, int cw, int ch)
{
	if (m_pSpk)
	{
		Rect clip(0, 0, cw, ch);
		m_pSpk->BltLockedClip(sx, sy, clip, frame);
	}
}

// Panel sprite'ini hedef boyuta KENAR KORUYARAK dosar. Reborn ayni sprite'i
// GPU'da hedefe olcekler; Legacy blit olceklemedigi icin duz kirpmak sag/alt
// cerceve cizgisini yutuyordu (orn. 182x24 panelin 110'luk frame-limit kutusu).
// Cozum: sol/ust parca + sprite'in sag/alt kenarindan alinan serit.
void C_VS_UI_OPTION_RENEWAL::BltFrameEdges(int frame, int sx, int sy, int tw, int th)
{
	if (!m_pSpk)
		return;

	int sw = m_pSpk->GetWidth(frame);
	int sh = m_pSpk->GetHeight(frame);
	if (sw <= 0 || sh <= 0 || tw <= 0 || th <= 0)
		return;

	if (tw >= sw && th >= sh)
	{
		BltFrame(frame, sx, sy);
		return;
	}

	const int kEdgeW = 6;	// cerceve kalinligi payi (px)
	const int kEdgeH = 4;

	struct Piece { int dst, src, len; };
	Piece cols[2]; int nCols = 0;
	Piece rows[2]; int nRows = 0;

	if (tw >= sw)
		cols[nCols++] = { 0, 0, sw };
	else
	{
		int edge = (kEdgeW * 2 > tw) ? tw / 2 : kEdgeW;
		cols[nCols++] = { 0, 0, tw - edge };
		cols[nCols++] = { tw - edge, sw - edge, edge };
	}

	if (th >= sh)
		rows[nRows++] = { 0, 0, sh };
	else
	{
		int edge = (kEdgeH * 2 > th) ? th / 2 : kEdgeH;
		rows[nRows++] = { 0, 0, th - edge };
		rows[nRows++] = { th - edge, sh - edge, edge };
	}

	for (int r = 0; r < nRows; r++)
	{
		for (int c = 0; c < nCols; c++)
		{
			// BltLockedClip src-rect'i sprite-origin'e gore cizer; hedef
			// konumu tutturmak icin origin'i src offset'i kadar kaydir.
			Rect clip(cols[c].src, rows[r].src, cols[c].len, rows[r].len);
			m_pSpk->BltLockedClip(sx + cols[c].dst - cols[c].src,
			                      sy + rows[r].dst - rows[r].src, clip, frame);
		}
	}
}

void C_VS_UI_OPTION_RENEWAL::BltHotkeyFrame(int frame, int sx, int sy)
{
	C_SPRITE_PACK* pPack = m_pHotkeySpk[frame >> 4];
	if (pPack)
		pPack->BltLocked(sx, sy, frame & 15);
}

void C_VS_UI_OPTION_RENEWAL::DrawLabel(int sx, int sy, const char* str, COLORREF color)
{
	g_PrintColorStr(sx, sy, str, gpC_base->m_user_id_pi, color);
}

void C_VS_UI_OPTION_RENEWAL::DrawCheckAt(int refX, int refY, bool bChecked, bool bEnabled)
{
	int frame;
	if (bEnabled)
		frame = bChecked ? SPR_CHECK_ON : SPR_CHECK_OFF;
	else
		frame = bChecked ? SPR_CHECK_ON_GREY : SPR_CHECK_OFF_GREY;
	BltFrame(frame, RX(refX), RY(refY));
}

// ratio100: 0..100 dolgu orani. Ray + dolgu + ibre cizer.
void C_VS_UI_OPTION_RENEWAL::DrawSliderAt(int refX, int refY, int refW, int ratio100)
{
	if (ratio100 < 0) ratio100 = 0;
	if (ratio100 > 100) ratio100 = 100;

	int px = RX(refX), py = RY(refY);
	BltFrame(SPR_OPACITY_DARK, px, py);

	int fillW = refW * ratio100 / 100;
	if (fillW > 0)
		BltFrameClip(SPR_OPACITY_LIGHT, px, py, fillW, 12);

	// Ibre: dolgu ucunda, rayin altinda (Reborn: arrow.y = track.y + track.h + 2)
	int arrowX = px + fillW - 4;
	if (arrowX < px) arrowX = px;
	if (arrowX > px + refW - 8) arrowX = px + refW - 8;
	BltFrame(SPR_UP_ARROW, arrowX, py + 12 + 2);
}

/*-----------------------------------------------------------------------------
- Show
-----------------------------------------------------------------------------*/
void C_VS_UI_OPTION_RENEWAL::Show()
{
	if (m_bHotkeyOpen)
	{
		DrawHotkeyWindow();
		return;
	}

	DrawTabsAndChrome();

	switch (m_iTab)
	{
	case TAB_GRAPHICS:	DrawGraphicsTab();	break;
	case TAB_SOUND:		DrawSoundTab();		break;
	case TAB_GAME:		DrawGameTab();		break;
	case TAB_ADVANCED:	DrawAdvancedTab();	break;
	case TAB_ADVANCED2:	DrawAdvanced2Tab();	break;
	}
}

void C_VS_UI_OPTION_RENEWAL::DrawTabsAndChrome()
{
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		// Pencere arkaplani
		BltFrame(SPR_WINDOW_BG, x, y);

		// Ayirac cizgileri — sekmeye ozel rect'ler (interface.ini paritesi).
		// Yatay sprite 440x1, dikey 1x216; hedef w/h'ye satir/kolon doseyerek ulasilir.
		{
			for (int i = 0; i < 4; i++)
			{
				const RefRect& rc = kRcSeps[m_iTab][i];
				for (int yy = 0; yy < rc.h; yy++)
				{
					for (int xx = 0; xx < rc.w; xx += 440)
					{
						int cw = (rc.w - xx < 440) ? rc.w - xx : 440;
						BltFrameClip(SPR_SEPARATOR_H, RX(rc.x) + xx, RY(rc.y) + yy, cw, 1);
					}
				}
			}
			const RefRect& rv = kRcSeps[m_iTab][4];
			for (int xx = 0; xx < rv.w; xx++)
			{
				for (int yy = 0; yy < rv.h; yy += 216)
				{
					int ch = (rv.h - yy < 216) ? rv.h - yy : 216;
					BltFrameClip(SPR_SEPARATOR_V, RX(rv.x) + xx, RY(rv.y) + yy, 1, ch);
				}
			}
		}

		// Sekmeler
		for (int i = 0; i < 5; i++)
			BltFrame(m_iTab == i ? SPR_TAB_PRESS : SPR_TAB_NORMAL, RX(kRcTab[i].x), RY(kRcTab[i].y));

		// Reset (yesil) + Close (teal) + X
		BltFrame(SPR_GREEN_BTN, RX(kRcResetBtn.x), RY(kRcResetBtn.y));
		BltFrame(SPR_TEAL_BTN, RX(kRcCloseActionBtn.x), RY(kRcCloseActionBtn.y));
		BltFrame(SPR_CLOSE_BTN, RX(kRcCloseBtn.x), RY(kRcCloseBtn.y));

		gpC_base->m_p_DDSurface_back->Unlock();
	}

	g_FL2_GetDC();
	DrawLabel(RX(kRcTitleText.x), RY(kRcTitleText.y), "GAME OPTIONS", kClrTitle);
	DrawLabel(RX(kRcResetText.x), RY(kRcResetText.y), "RESET", kClrValue);
	DrawLabel(RX(kRcCloseText.x), RY(kRcCloseText.y), "CLOSE", kClrValue);

	const char* tabLabel[5] = { "GRAPH", "SOUND", "GAME", "ADV", "ADV2" };
	for (int i = 0; i < 5; i++)
		DrawLabel(RX(kRcTabText[i].x), RY(kRcTabText[i].y), tabLabel[i], m_iTab == i ? kClrTitle : kClrLabel);
	g_FL2_ReleaseDC();
}

void C_VS_UI_OPTION_RENEWAL::DrawGraphicsTab()
{
	bool bBorderless = (m_adv2Flags & kAdv2BorderlessBit) != 0;

	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		// Cozunurluk kutusu + ok
		BltFrame(SPR_PANEL_BG, RX(kRcGfxResBox.x), RY(kRcGfxResBox.y));
		BltFrame(SPR_UP_ARROW, RX(kRcGfxResArrow.x), RY(kRcGfxResArrow.y));

		// Fullscreen
		DrawCheckAt(kRcGfxFullscreen.x, kRcGfxFullscreen.y, m_bFullscreen);

		// Frame limit kutusu (182'lik panel 110'a kenar koruyarak daralir)
		BltFrameEdges(SPR_PANEL_BG, RX(kRcGfxFrameLimit.x), RY(kRcGfxFrameLimit.y), kRcGfxFrameLimit.w, kRcGfxFrameLimit.h);

		// Pencere modu
		DrawCheckAt(kRcGfxWindowed.x, kRcGfxWindowed.y, !bBorderless && !m_bFullscreen);
		DrawCheckAt(kRcGfxBorderless.x, kRcGfxBorderless.y, bBorderless && !m_bFullscreen);

		// Menu seffafligi
		DrawSliderAt(kRcGfxOpacityBar.x, kRcGfxOpacityBar.y, kRcGfxOpacityBar.w,
			(int)g_pUserOption->ALPHA_DEPTH * 100 / kMaxAlphaDepth);
		DrawCheckAt(kRcGfxDefAlpha.x, kRcGfxDefAlpha.y, g_pUserOption->DefaultAlpha != FALSE);

		// Efekt listesi
		for (int i = 0; i < 9; i++)
			DrawCheckAt(kRcGfxEffect[i].x, kRcGfxEffect[i].y, (m_graphFlags & (1u << i)) != 0);

		// Dropdown acik ise satirlar (24'luk panel 22'ye kenar koruyarak)
		if (m_bDropdownOpen)
		{
			// 2026-09-03 kullanici istegi — panel sprite'inin ici colorkey
			// transparan; acilan liste altindaki menu yazilariyla karisiyordu.
			// Once satir alani OPAK koyu dolguyla kapatilir (alpha2=0 = tam
			// opak, legacy dest*a/32 formulu), cerceve ustune cizilir.
			const WORD kRowFill565 = (WORD)((3 << 11) | (6 << 5) | 4);	// koyu gri
			for (int i = 0; i < 4; i++)
			{
				RECT rf;
				rf.left   = RX(kRcGfxResRow[i].x);
				rf.top    = RY(kRcGfxResRow[i].y);
				rf.right  = rf.left + kRcGfxResRow[i].w;
				rf.bottom = rf.top + kRcGfxResRow[i].h;
				gpC_base->m_p_DDSurface_back->BltColorAlpha(&rf, kRowFill565, 0);
				BltFrameEdges(SPR_PANEL_BG, RX(kRcGfxResRow[i].x), RY(kRcGfxResRow[i].y), kRcGfxResRow[i].w, kRcGfxResRow[i].h);
			}
		}

		gpC_base->m_p_DDSurface_back->Unlock();
	}

	g_FL2_GetDC();
	DrawLabel(RX(421), RY(334), "Resolution", kClrLabel);
	DrawLabel(RX(528), RY(338), ResolutionLabel(m_pendingResW, m_pendingResH), kClrValue);
	DrawLabel(RX(740), RY(337), "Fullscreen", kClrLabel);

	char szFrameLimit[32];
	if (m_frameLimit == 0)
		strcpy(szFrameLimit, "Unlimited");
	else
		sprintf(szFrameLimit, "%d FPS", m_frameLimit);
	DrawLabel(RX(420), RY(362), "Frame limit", kClrLabel);
	DrawLabel(RX(528), RY(360), szFrameLimit, kClrValue);

	DrawLabel(RX(420), RY(383), "Window mode", kClrLabel);
	DrawLabel(RX(541), RY(385), "Windowed", kClrLabel);
	DrawLabel(RX(662), RY(386), "Borderless", kClrLabel);

	DrawLabel(RX(420), RY(438), "Menu opacity", kClrLabel);
	DrawLabel(RX(740), RY(406), "Default alpha", kClrLabel);

	DrawLabel(RX(420), RY(532), "Effects", kClrLabel);
	for (int i = 0; i < 9; i++)
		DrawLabel(RX(kRcGfxEffectLbl[i].x), RY(kRcGfxEffectLbl[i].y), kGfxEffectLabel[i], kClrLabel);

	if (m_bDropdownOpen)
	{
		for (int i = 0; i < 4; i++)
			DrawLabel(RX(kRcGfxResRow[i].x + 8), RY(kRcGfxResRow[i].y + 3), kResChoices[i].label, kClrValue);
	}
	g_FL2_ReleaseDC();
}

void C_VS_UI_OPTION_RENEWAL::DrawSoundTab()
{
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		// BGM
		BltFrame(SPR_VOLUME_BG, RX(kRcSndBgmBar.x), RY(kRcSndBgmBar.y));
		int fillW = kRcSndBgmBar.w * m_volume_music / 100;
		if (fillW > 0)
			BltFrameClip(SPR_VOLUME_FILL, RX(kRcSndBgmBar.x), RY(kRcSndBgmBar.y), fillW, kRcSndBgmBar.h);
		DrawCheckAt(kRcSndBgmCheck.x, kRcSndBgmCheck.y, g_pUserOption->PlayMusic != FALSE);

		// SFX
		BltFrame(SPR_VOLUME_BG, RX(kRcSndSfxBar.x), RY(kRcSndSfxBar.y));
		fillW = kRcSndSfxBar.w * m_volume_sound / 100;
		if (fillW > 0)
			BltFrameClip(SPR_VOLUME_FILL, RX(kRcSndSfxBar.x), RY(kRcSndSfxBar.y), fillW, kRcSndSfxBar.h);
		DrawCheckAt(kRcSndSfxCheck.x, kRcSndSfxCheck.y, g_pUserOption->PlaySound != FALSE);

		gpC_base->m_p_DDSurface_back->Unlock();
	}

	g_FL2_GetDC();
	DrawLabel(RX(420), RY(354), "BGM volume", kClrLabel);
	DrawLabel(RX(800), RY(354), "Enable", kClrLabel);
	DrawLabel(RX(420), RY(400), "SFX volume", kClrLabel);
	DrawLabel(RX(800), RY(400), "Enable", kClrLabel);
	g_FL2_ReleaseDC();
}

void C_VS_UI_OPTION_RENEWAL::DrawGameTab()
{
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		for (int i = 0; i < 9; i++)
			DrawCheckAt(kRcGameCheck[i].x, kRcGameCheck[i].y, (m_gameFlags & (1u << i)) != 0);

		// Klavye ayar butonu
		BltFrame(SPR_LARGE_TEAL_BTN, RX(kRcGameKeyboard.x), RY(kRcGameKeyboard.y));

		// Legacy ek: Enter-chat
		DrawCheckAt(kRcGameEnterChat.x, kRcGameEnterChat.y, g_pUserOption->UseEnterChat != FALSE);

		gpC_base->m_p_DDSurface_back->Unlock();
	}

	g_FL2_GetDC();
	for (int i = 0; i < 9; i++)
		DrawLabel(RX(kRcGameCheckLbl[i].x), RY(kRcGameCheckLbl[i].y), kGameLabel[i], kClrLabel);
	DrawLabel(RX(kRcGameKeyboardLbl.x), RY(kRcGameKeyboardLbl.y), "KEYBOARD SETTING", kClrValue);
	DrawLabel(RX(548), RY(558), "Use Enter to open chat", kClrLabel);
	g_FL2_ReleaseDC();
}

void C_VS_UI_OPTION_RENEWAL::DrawAdvancedTab()
{
	bool bIFeelAvailable = (gpC_Imm && gpC_Imm->IsDevice());

	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		for (int i = 0; i < 7; i++)
		{
			bool bChecked = (i == 5) ? m_bLinearFilter : ((m_advFlags & (1u << i)) != 0);
			DrawCheckAt(kRcAdvCheck[i].x, kRcAdvCheck[i].y, bChecked);
		}

		// Legacy ek satirlari
		DrawCheckAt(kRcAdvIFeel.x, kRcAdvIFeel.y, g_pUserOption->UseForceFeel != FALSE, bIFeelAvailable);
		DrawCheckAt(kRcAdvCritical.x, kRcAdvCritical.y, g_pUserOption->ShowCriticalHitEffect != FALSE);
		DrawCheckAt(kRcAdvFilter.x, kRcAdvFilter.y, g_pUserOption->FilteringCurse != FALSE);
		DrawCheckAt(kRcAdvChatWhite.x, kRcAdvChatWhite.y, g_pUserOption->ChatWhite != FALSE);
		DrawCheckAt(kRcAdvWhisperPop.x, kRcAdvWhisperPop.y, g_pUserOption->PopupChatByWhisper != FALSE);

		gpC_base->m_p_DDSurface_back->Unlock();
	}

	g_FL2_GetDC();
	for (int i = 0; i < 7; i++)
		DrawLabel(RX(kRcAdvCheckLbl[i].x), RY(kRcAdvCheckLbl[i].y), kAdvLabel[i], kClrLabel);
	DrawLabel(RX(548), RY(514), "Force feedback mouse (IFeel)", bIFeelAvailable ? kClrLabel : RGB(110, 110, 110));
	DrawLabel(RX(548), RY(538), "Show critical hit effect", kClrLabel);
	DrawLabel(RX(548), RY(562), "Filter profanity", kClrLabel);
	DrawLabel(RX(548), RY(586), "White chat text", kClrLabel);
	DrawLabel(RX(548), RY(610), "Open chat on whisper", kClrLabel);
	g_FL2_ReleaseDC();
}

void C_VS_UI_OPTION_RENEWAL::DrawAdvanced2Tab()
{
	bool bTexHigh = (m_adv2Flags & kAdv2TextureCacheHighBit) != 0;

	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		DrawCheckAt(kRcA2Pacing.x, kRcA2Pacing.y, (m_adv2Flags & kAdv2SmoothPacingBit) != 0);

		DrawCheckAt(kRcA2Nearest.x, kRcA2Nearest.y, !m_bLinearFilter);
		DrawCheckAt(kRcA2Linear.x, kRcA2Linear.y, m_bLinearFilter);

		DrawCheckAt(kRcA2TexNormal.x, kRcA2TexNormal.y, !bTexHigh);
		DrawCheckAt(kRcA2TexHigh.x, kRcA2TexHigh.y, bTexHigh);

		// Gamma 50..150 -> 0..100
		DrawSliderAt(kRcA2GammaBar.x, kRcA2GammaBar.y, kRcA2GammaBar.w, m_value_gamma - 50);
		// Brightness 0..100
		DrawSliderAt(kRcA2BrightBar.x, kRcA2BrightBar.y, kRcA2BrightBar.w, m_brightness);

		DrawCheckAt(kRcA2HqUi.x, kRcA2HqUi.y, (m_adv2Flags & kAdv2HqUiScalingBit) != 0);
		DrawCheckAt(kRcA2PixelPerfect.x, kRcA2PixelPerfect.y, (m_adv2Flags & kAdv2PixelPerfectBit) != 0);

		// Legacy ek: Vignette
		DrawCheckAt(kRcA2Vignette.x, kRcA2Vignette.y, g_pUserOption->VignetteEnabled != FALSE);

		gpC_base->m_p_DDSurface_back->Unlock();
	}

	g_FL2_GetDC();
	DrawLabel(RX(548), RY(352), "Smooth frame pacing", kClrLabel);
	DrawLabel(RX(420), RY(386), "Sprite filter", kClrLabel);
	DrawLabel(RX(548), RY(386), "Nearest", kClrLabel);
	DrawLabel(RX(678), RY(386), "Linear", kClrLabel);
	DrawLabel(RX(420), RY(420), "Texture cache", kClrLabel);
	DrawLabel(RX(548), RY(420), "Normal", kClrLabel);
	DrawLabel(RX(678), RY(420), "High", kClrLabel);
	DrawLabel(RX(420), RY(459), "Gamma", kClrLabel);
	DrawLabel(RX(420), RY(503), "Brightness", kClrLabel);
	DrawLabel(RX(548), RY(554), "High quality UI scaling", kClrLabel);
	DrawLabel(RX(548), RY(588), "Pixel perfect / nearest", kClrLabel);
	DrawLabel(RX(548), RY(622), "Vignette effect", kClrLabel);
	g_FL2_ReleaseDC();
}

void C_VS_UI_OPTION_RENEWAL::DrawHotkeyWindow()
{
	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		BltHotkeyFrame(0, HX(kRefHkX), HY(kRefHkY));

		for (int i = 0; i < kHkKeyCount; i++)
			BltHotkeyFrame(kHkKeys[i].sprite, HX(kHkKeys[i].rc.x), HY(kHkKeys[i].rc.y));

		// Alt butonlar (sprite 26 kucuk buton) + X
		BltHotkeyFrame(26, HX(kRcHkCancel.x), HY(kRcHkCancel.y));
		BltHotkeyFrame(26, HX(kRcHkSave.x), HY(kRcHkSave.y));
		BltHotkeyFrame(26, HX(kRcHkReset.x), HY(kRcHkReset.y));
		BltHotkeyFrame(26, HX(kRcHkDefault.x), HY(kRcHkDefault.y));
		BltHotkeyFrame(29, HX(kRcHkClose.x), HY(kRcHkClose.y));

		gpC_base->m_p_DDSurface_back->Unlock();
	}

	g_FL2_GetDC();
	DrawLabel(HX(kRcHkTitle.x), HY(kRcHkTitle.y), "SHORTKEY SETTINGS", kClrTitle);

	for (int i = 0; i < kHkKeyCount; i++)
	{
		if (kHkKeys[i].label[0])
			DrawLabel(HX(kHkKeys[i].rc.x + 6), HY(kHkKeys[i].rc.y + 6), kHkKeys[i].label, kClrLabel);
	}

	DrawLabel(HX(kRcHkCancel.x + 12), HY(kRcHkCancel.y + 4), "Cancel", kClrValue);
	DrawLabel(HX(kRcHkSave.x + 18), HY(kRcHkSave.y + 4), "Save", kClrValue);
	DrawLabel(HX(kRcHkReset.x + 16), HY(kRcHkReset.y + 4), "Reset", kClrValue);
	DrawLabel(HX(kRcHkDefault.x + 12), HY(kRcHkDefault.y + 4), "Default", kClrValue);
	g_FL2_ReleaseDC();
}

/*-----------------------------------------------------------------------------
- MouseControl
-----------------------------------------------------------------------------*/
bool C_VS_UI_OPTION_RENEWAL::MouseControl(UINT message, int _x, int _y)
{
	switch (message)
	{
	case M_LEFTBUTTON_DOWN:
	case M_LB_DOUBLECLICK:
		if (HandleClick(_x, _y))
			break;

		// Bos alan: pencereyi tasi (eski menu paritesi)
		if (!m_bHotkeyOpen && gpC_mouse_pointer->GetPickUpItem() == false &&
			_x >= x && _x < x + w && _y >= y && _y < y + h)
		{
			MoveReady();
			SetOrigin(_x - x, _y - y);
		}
		break;

	case M_MOVING:
		if (m_bLBPush)
			HandleSliderDrag(_x, _y);
		break;

	case M_LEFTBUTTON_UP:
		if (m_bLBPush)
			ReleaseSlider();
		break;
	}

	return true;
}

bool C_VS_UI_OPTION_RENEWAL::HandleClick(int sx, int sy)
{
	if (m_bHotkeyOpen)
		return HandleClickHotkey(sx, sy);

	// X butonu veya CLOSE aksiyonu
	if (HitR(kRcCloseBtn.x, kRcCloseBtn.y, kRcCloseBtn.w, kRcCloseBtn.h, sx, sy) ||
		HitR(kRcCloseActionBtn.x, kRcCloseActionBtn.y, kRcCloseActionBtn.w, kRcCloseActionBtn.h, sx, sy))
	{
		CT_Done("OPTREN CLOSE -> SendMessage(UI_CLOSE_OPTION)");
		RequestClose();
		return true;
	}

	// Sekmeler
	for (int i = 0; i < 5; i++)
	{
		if (HitR(kRcTab[i].x, kRcTab[i].y, kRcTab[i].w, kRcTab[i].h, sx, sy))
		{
			m_iTab = i;
			m_bDropdownOpen = false;
			return true;
		}
	}

	// RESET
	if (HitR(kRcResetBtn.x, kRcResetBtn.y, kRcResetBtn.w, kRcResetBtn.h, sx, sy))
	{
		ResetToDefaults();
		return true;
	}

	switch (m_iTab)
	{
	case TAB_GRAPHICS:	return HandleClickGraphics(sx, sy);
	case TAB_SOUND:		return HandleClickSound(sx, sy);
	case TAB_GAME:		return HandleClickGame(sx, sy);
	case TAB_ADVANCED:	return HandleClickAdvanced(sx, sy);
	case TAB_ADVANCED2:	return HandleClickAdvanced2(sx, sy);
	}
	return false;
}

bool C_VS_UI_OPTION_RENEWAL::HandleClickGraphics(int sx, int sy)
{
	// Dropdown acik: once satirlar
	if (m_bDropdownOpen)
	{
		for (int i = 0; i < 4; i++)
		{
			if (HitR(kRcGfxResRow[i].x, kRcGfxResRow[i].y, kRcGfxResRow[i].w, kRcGfxResRow[i].h, sx, sy))
			{
				m_pendingResW = kResChoices[i].w;
				m_pendingResH = kResChoices[i].h;
				m_bDropdownOpen = false;

				// O3 — canli cozunurluk, kuyruklu UI mesajiyla uygulanir
				// (handler title agacini VEYA in-game option menusunu
				// yeniden kurar). 2026-09-03 plan F2b: in-game restart-
				// pending yolu kaldirildi — Reborn gibi her yerde canli.
				// Dunya redraw'u Apply icinde (F2a RestoreSurface+
				// SetFirstDraw); HUD relayout F3 pass'inde.
				if (m_pendingResW != g_pUserInformation->iResolution_x ||
					m_pendingResH != g_pUserInformation->iResolution_y)
				{
					gpC_base->SendMessage(UI_APPLY_RESOLUTION, m_pendingResW, m_pendingResH);
				}
				return true;
			}
		}
	}

	// Cozunurluk kutusu / ok: dropdown ac-kapa
	if (HitR(kRcGfxResBox.x, kRcGfxResBox.y, kRcGfxResBox.w, kRcGfxResBox.h, sx, sy) ||
		HitR(kRcGfxResArrow.x, kRcGfxResArrow.y, kRcGfxResArrow.w, kRcGfxResArrow.h, sx, sy))
	{
		m_bDropdownOpen = !m_bDropdownOpen;
		return true;
	}

	// Fullscreen
	if (HitR(kRcGfxFullscreen.x, kRcGfxFullscreen.y, kRcGfxFullscreen.w, kRcGfxFullscreen.h, sx, sy))
	{
		m_bFullscreen = !m_bFullscreen;
		if (m_bFullscreen)
			m_adv2Flags &= ~kAdv2BorderlessBit;
		// 2026-09-02 fullscreen fix: dx11 backend'de canli borderless gecis
		// (Reborn ApplyWindowMode paritesi — cozunurluk degismedigi icin
		// relayout gerekmez, oyun icinde de guvenli). d3d9/ddraw7 backend
		// canli desteklemez -> restart-pending. Her iki durumda da
		// resolution.inf'e yazilir (kalicilik).
		Phoenix_WindowMode_Apply(m_bFullscreen);
		Phoenix_ResolutionLive_SavePending(m_pendingResW, m_pendingResH,
		                                   m_bFullscreen ? 1 : 0);
		return true;
	}

	// Frame limit dongusu: 60 -> 120 -> 144 -> 30 -> 0 -> 60
	if (HitR(kRcGfxFrameLimit.x, kRcGfxFrameLimit.y, kRcGfxFrameLimit.w, kRcGfxFrameLimit.h, sx, sy))
	{
		switch (m_frameLimit)
		{
		case 60:	m_frameLimit = 120;	break;
		case 120:	m_frameLimit = 144;	break;
		case 144:	m_frameLimit = 30;	break;
		case 30:	m_frameLimit = 0;	break;
		default:	m_frameLimit = 60;	break;
		}
		Phoenix_Pacer_SetFrameLimit(m_frameLimit);	// aninda uygula
		return true;
	}

	// Pencere modu
	if (HitR(kRcGfxWindowed.x, kRcGfxWindowed.y, kRcGfxWindowed.w, kRcGfxWindowed.h, sx, sy))
	{
		m_adv2Flags &= ~kAdv2BorderlessBit;
		m_bFullscreen = false;
		// Fullscreen aktifse canli geri don (dx11) + kalicilik
		Phoenix_WindowMode_Apply(false);
		Phoenix_ResolutionLive_SavePending(m_pendingResW, m_pendingResH, 0);
		return true;
	}
	if (HitR(kRcGfxBorderless.x, kRcGfxBorderless.y, kRcGfxBorderless.w, kRcGfxBorderless.h, sx, sy))
	{
		m_adv2Flags |= kAdv2BorderlessBit;
		m_bFullscreen = false;
		Phoenix_WindowMode_Apply(false);
		Phoenix_ResolutionLive_SavePending(m_pendingResW, m_pendingResH, 0);
		return true;
	}

	// Default alpha (seffaf menuler)
	if (HitR(kRcGfxDefAlpha.x, kRcGfxDefAlpha.y, kRcGfxDefAlpha.w, kRcGfxDefAlpha.h, sx, sy))
	{
		g_pUserOption->DefaultAlpha = !g_pUserOption->DefaultAlpha;

		// 2026-09-03 plan F4 — canli yayilim (Reborn parity: alpha global
		// davranir). Acik pencerelerin per-window alpha attr'i ve window
		// manager slotlari yeni degere cekilir; DrawDialog* per-frame
		// okudugu icin etki ayni frame'de gorunur. (Kendi penceremiz
		// SPR_WINDOW_BG opak sprite ile cizilir — o etkilenmez.)
		{
			const bool bAlpha = (g_pUserOption->DefaultAlpha != FALSE);

			for (int id = 0; id < (int)C_VS_UI_WINDOW_MANAGER::WINDOW_TOTAL; id++)
				gpC_vs_ui_window_manager->SetAlpha((C_VS_UI_WINDOW_MANAGER::WINDOW_ID)id, bAlpha);

			for (int i = 0; i < gpC_window_manager->Size(); i++)
			{
				Window* pWin = NULL;
				if (gpC_window_manager->Data(i, pWin) && pWin != NULL && pWin != this)
					pWin->AttrAlpha(bAlpha);
			}
		}
		return true;
	}

	// Menu opacity slider
	if (HitR(kRcGfxOpacityBar.x, kRcGfxOpacityBar.y - 2, kRcGfxOpacityBar.w, kRcGfxOpacityBar.h + 10, sx, sy))
	{
		m_bLBPush = true;
		m_iDragSlider = DRAG_OPACITY;
		HandleSliderDrag(sx, sy);
		return true;
	}

	// Efekt listesi
	for (int i = 0; i < 9; i++)
	{
		if (HitR(kRcGfxEffect[i].x, kRcGfxEffect[i].y, kRcGfxEffect[i].w, kRcGfxEffect[i].h, sx, sy))
		{
			m_graphFlags ^= (1u << i);
			ApplyGraphFlagToLegacy(i, (m_graphFlags & (1u << i)) != 0);
			return true;
		}
	}

	// Dropdown acikken baska yere tiklandi: kapat
	if (m_bDropdownOpen)
	{
		m_bDropdownOpen = false;
		return true;
	}
	return false;
}

bool C_VS_UI_OPTION_RENEWAL::HandleClickSound(int sx, int sy)
{
	// BGM enable
	if (HitR(kRcSndBgmCheck.x, kRcSndBgmCheck.y, kRcSndBgmCheck.w, kRcSndBgmCheck.h, sx, sy))
	{
		BOOL bOn = !g_pUserOption->PlayMusic;
		// Runtime uygulama Execute_UI_CHANGE_OPTION icinde (BGM baslat/durdur)
		gpC_base->SendMessage(UI_CHANGE_OPTION, C_VS_UI_OPTION::CHECK_MUSIC, bOn);
		return true;
	}

	// SFX enable
	if (HitR(kRcSndSfxCheck.x, kRcSndSfxCheck.y, kRcSndSfxCheck.w, kRcSndSfxCheck.h, sx, sy))
	{
		BOOL bOn = !g_pUserOption->PlaySound;
		gpC_base->SendMessage(UI_CHANGE_OPTION, C_VS_UI_OPTION::CHECK_SOUND, bOn);
		return true;
	}

	// Volume barlari (surukleme baslat)
	if (HitR(kRcSndBgmBar.x, kRcSndBgmBar.y - 4, kRcSndBgmBar.w, kRcSndBgmBar.h + 8, sx, sy))
	{
		m_bLBPush = true;
		m_iDragSlider = DRAG_BGM;
		HandleSliderDrag(sx, sy);
		return true;
	}
	if (HitR(kRcSndSfxBar.x, kRcSndSfxBar.y - 4, kRcSndSfxBar.w, kRcSndSfxBar.h + 8, sx, sy))
	{
		m_bLBPush = true;
		m_iDragSlider = DRAG_SFX;
		HandleSliderDrag(sx, sy);
		return true;
	}
	return false;
}

bool C_VS_UI_OPTION_RENEWAL::HandleClickGame(int sx, int sy)
{
	for (int i = 0; i < 9; i++)
	{
		if (HitR(kRcGameCheck[i].x, kRcGameCheck[i].y, kRcGameCheck[i].w, kRcGameCheck[i].h, sx, sy))
		{
			m_gameFlags ^= (1u << i);
			ApplyGameFlagToLegacy(i, (m_gameFlags & (1u << i)) != 0);
			return true;
		}
	}

	// Klavye ayar penceresi
	if (HitR(kRcGameKeyboard.x, kRcGameKeyboard.y, kRcGameKeyboard.w, kRcGameKeyboard.h, sx, sy))
	{
		m_bHotkeyOpen = true;
		return true;
	}

	// Legacy ek: Enter-chat
	if (HitR(kRcGameEnterChat.x, kRcGameEnterChat.y, kRcGameEnterChat.w, kRcGameEnterChat.h, sx, sy))
	{
		g_pUserOption->UseEnterChat = !g_pUserOption->UseEnterChat;
		return true;
	}
	return false;
}

bool C_VS_UI_OPTION_RENEWAL::HandleClickAdvanced(int sx, int sy)
{
	for (int i = 0; i < 7; i++)
	{
		if (HitR(kRcAdvCheck[i].x, kRcAdvCheck[i].y, kRcAdvCheck[i].w, kRcAdvCheck[i].h, sx, sy))
		{
			if (i == 5)
			{
				SetLinearFilter(!m_bLinearFilter);
			}
			else
			{
				m_advFlags ^= (1u << i);
				ApplyAdvancedFlagToLegacy(i, (m_advFlags & (1u << i)) != 0);
			}
			return true;
		}
	}

	// Legacy ek: IFeel
	if (HitR(kRcAdvIFeel.x, kRcAdvIFeel.y, kRcAdvIFeel.w, kRcAdvIFeel.h, sx, sy))
	{
		if (gpC_Imm && gpC_Imm->IsDevice())
			g_pUserOption->UseForceFeel = !g_pUserOption->UseForceFeel;
		return true;
	}

	// Legacy ek: Critical hit
	if (HitR(kRcAdvCritical.x, kRcAdvCritical.y, kRcAdvCritical.w, kRcAdvCritical.h, sx, sy))
	{
		g_pUserOption->ShowCriticalHitEffect = !g_pUserOption->ShowCriticalHitEffect;
		return true;
	}

	// Legacy ek: kufur filtresi
	if (HitR(kRcAdvFilter.x, kRcAdvFilter.y, kRcAdvFilter.w, kRcAdvFilter.h, sx, sy))
	{
		g_pUserOption->FilteringCurse = !g_pUserOption->FilteringCurse;
		return true;
	}

	// Legacy ek: beyaz chat yazisi
	if (HitR(kRcAdvChatWhite.x, kRcAdvChatWhite.y, kRcAdvChatWhite.w, kRcAdvChatWhite.h, sx, sy))
	{
		g_pUserOption->ChatWhite = !g_pUserOption->ChatWhite;
		return true;
	}

	// Legacy ek: fisilti gelince chat penceresi ac
	if (HitR(kRcAdvWhisperPop.x, kRcAdvWhisperPop.y, kRcAdvWhisperPop.w, kRcAdvWhisperPop.h, sx, sy))
	{
		g_pUserOption->PopupChatByWhisper = !g_pUserOption->PopupChatByWhisper;
		return true;
	}
	return false;
}

bool C_VS_UI_OPTION_RENEWAL::HandleClickAdvanced2(int sx, int sy)
{
	if (HitR(kRcA2Pacing.x, kRcA2Pacing.y, kRcA2Pacing.w, kRcA2Pacing.h, sx, sy))
	{
		m_adv2Flags ^= kAdv2SmoothPacingBit;
		Phoenix_Pacer_SetSmoothPacing((m_adv2Flags & kAdv2SmoothPacingBit) != 0);
		return true;
	}
	if (HitR(kRcA2Nearest.x, kRcA2Nearest.y, kRcA2Nearest.w, kRcA2Nearest.h, sx, sy))
	{
		SetLinearFilter(false);
		return true;
	}
	if (HitR(kRcA2Linear.x, kRcA2Linear.y, kRcA2Linear.w, kRcA2Linear.h, sx, sy))
	{
		SetLinearFilter(true);
		return true;
	}
	if (HitR(kRcA2TexNormal.x, kRcA2TexNormal.y, kRcA2TexNormal.w, kRcA2TexNormal.h, sx, sy))
	{
		m_adv2Flags &= ~kAdv2TextureCacheHighBit;
		phoenix::render::SetSpriteCacheHighBudget(false);
		return true;
	}
	if (HitR(kRcA2TexHigh.x, kRcA2TexHigh.y, kRcA2TexHigh.w, kRcA2TexHigh.h, sx, sy))
	{
		m_adv2Flags |= kAdv2TextureCacheHighBit;
		phoenix::render::SetSpriteCacheHighBudget(true);
		return true;
	}

	// Gamma / Brightness sliderlari
	if (HitR(kRcA2GammaBar.x, kRcA2GammaBar.y - 2, kRcA2GammaBar.w, kRcA2GammaBar.h + 10, sx, sy))
	{
		m_bLBPush = true;
		m_iDragSlider = DRAG_GAMMA;
		HandleSliderDrag(sx, sy);
		return true;
	}
	if (HitR(kRcA2BrightBar.x, kRcA2BrightBar.y - 2, kRcA2BrightBar.w, kRcA2BrightBar.h + 10, sx, sy))
	{
		m_bLBPush = true;
		m_iDragSlider = DRAG_BRIGHTNESS;
		HandleSliderDrag(sx, sy);
		return true;
	}

	if (HitR(kRcA2HqUi.x, kRcA2HqUi.y, kRcA2HqUi.w, kRcA2HqUi.h, sx, sy))
	{
		m_adv2Flags ^= kAdv2HqUiScalingBit;
		if (m_adv2Flags & kAdv2HqUiScalingBit)
			SetLinearFilter(true);
		return true;
	}
	if (HitR(kRcA2PixelPerfect.x, kRcA2PixelPerfect.y, kRcA2PixelPerfect.w, kRcA2PixelPerfect.h, sx, sy))
	{
		m_adv2Flags ^= kAdv2PixelPerfectBit;
		if (m_adv2Flags & kAdv2PixelPerfectBit)
			SetLinearFilter(false);
		return true;
	}

	// Legacy ek: Vignette
	if (HitR(kRcA2Vignette.x, kRcA2Vignette.y, kRcA2Vignette.w, kRcA2Vignette.h, sx, sy))
	{
		g_pUserOption->VignetteEnabled = !g_pUserOption->VignetteEnabled;
		D3D9PostProcess_SetVignetteEnabled(g_pUserOption->VignetteEnabled != FALSE);
		return true;
	}
	return false;
}

bool C_VS_UI_OPTION_RENEWAL::HandleClickHotkey(int sx, int sy)
{
	// X / Cancel / Save: pencereyi kapat
	if (HitH(kRcHkClose.x, kRcHkClose.y, kRcHkClose.w, kRcHkClose.h, sx, sy) ||
		HitH(kRcHkCancel.x, kRcHkCancel.y, kRcHkCancel.w, kRcHkCancel.h, sx, sy) ||
		HitH(kRcHkSave.x, kRcHkSave.y, kRcHkSave.w, kRcHkSave.h, sx, sy))
	{
		m_bHotkeyOpen = false;
		return true;
	}

	// Reset / Default: tus atamalarini sifirla (onay dialogu — eski menu paritesi)
	if (HitH(kRcHkReset.x, kRcHkReset.y, kRcHkReset.w, kRcHkReset.h, sx, sy) ||
		HitH(kRcHkDefault.x, kRcHkDefault.y, kRcHkDefault.w, kRcHkDefault.h, sx, sy))
	{
		ClearInputState();
		DeleteNew(m_pC_dialog);
		m_pC_dialog = new C_VS_UI_DIALOG(-1, -1, 3, 0, ExecF_OptionResetButton, DIALOG_TITLE_OK | DIALOG_TITLE_CANCEL);
		char* pp_dmsg[1] = {
			(*g_pGameStringTable)[UI_STRING_MESSAGE_KEYSETTING_INIT].GetString(),
		};
		m_pC_dialog->SetMessage(pp_dmsg, 1, SMO_NOFIT, true);
		m_pC_dialog->Start();
		return true;
	}

	// Pencere icinde herhangi bir yer: yut (arkaya gecmesin)
	return true;
}

void C_VS_UI_OPTION_RENEWAL::HandleSliderDrag(int sx, int sy)
{
	const RefRect* pBar = NULL;
	switch (m_iDragSlider)
	{
	case DRAG_OPACITY:		pBar = &kRcGfxOpacityBar;	break;
	case DRAG_BGM:			pBar = &kRcSndBgmBar;		break;
	case DRAG_SFX:			pBar = &kRcSndSfxBar;		break;
	case DRAG_GAMMA:		pBar = &kRcA2GammaBar;		break;
	case DRAG_BRIGHTNESS:	pBar = &kRcA2BrightBar;		break;
	default:				return;
	}

	int px = RX(pBar->x);
	int ratio = (sx - px) * 100 / pBar->w;
	if (ratio < 0) ratio = 0;
	if (ratio > 100) ratio = 100;

	switch (m_iDragSlider)
	{
	case DRAG_OPACITY:
		g_pUserOption->ALPHA_DEPTH = (BYTE)(ratio * kMaxAlphaDepth / 100);
		break;

	case DRAG_BGM:
		m_volume_music = ratio;
		if (g_pUserOption->PlayMusic)
			BGMManager::Instance()->SetVolumeFromSlider(m_volume_music);
		break;

	case DRAG_SFX:
		m_volume_sound = ratio;
		if (g_pUserOption->PlaySound)
			g_DXSound.SetVolumeLimitFromSlider(m_volume_sound);
		break;

	case DRAG_GAMMA:
		m_value_gamma = 50 + ratio;
		if (CDirectDraw::IsSupportGammaControl())
		{
			g_pUserOption->UseGammaControl = TRUE;
			CDirectDraw::SetGammaRamp(m_value_gamma);
		}
		break;

	case DRAG_BRIGHTNESS:
		// O2: D3D11 present modulate (50 = notr) — surukleme sirasinda canli
		m_brightness = ratio;
		phoenix::render::SetUserBrightness(m_brightness);
		break;
	}
}

void C_VS_UI_OPTION_RENEWAL::ReleaseSlider()
{
	m_bLBPush = false;

	switch (m_iDragSlider)
	{
	case DRAG_BGM:
		gpC_base->SendMessage(UI_CHANGE_OPTION, C_VS_UI_OPTION::CHECK_VALUE_MUSIC_VOLUME, m_volume_music);
		break;

	case DRAG_SFX:
		gpC_base->SendMessage(UI_CHANGE_OPTION, C_VS_UI_OPTION::CHECK_VALUE_SOUND_VOLUME, m_volume_sound);
		break;

	case DRAG_GAMMA:
		g_pUserOption->GammaValue = m_value_gamma;
		break;

	default:
		break;
	}

	m_iDragSlider = DRAG_NONE;
}

/*-----------------------------------------------------------------------------
- Ayar akisi
-----------------------------------------------------------------------------*/
void C_VS_UI_OPTION_RENEWAL::LoadFromLegacy()
{
	// Ses
	m_volume_music = g_pUserOption->VolumeMusic;
	m_volume_sound = g_pUserOption->VolumeSound;

	// Gamma (BOOL tipli alan legacy'de 50..150 int deger saklar)
	m_value_gamma = (int)g_pUserOption->GammaValue;
	if (m_value_gamma < 50 || m_value_gamma > 150)
		m_value_gamma = 100;

	// Graphics efekt bit'leri (legacy karsiligi olanlar dosyayi ezer)
	const unsigned int legacyGraphMask =
		kGraphTransHPBarBit | kGraphBloodBit | kGraphSmoothAutoHideBit |
		kGraphChatBalloonLineBit | kGraphShowFpsBit | kGraphHideShopMsgBit;
	unsigned int legacyGraph = 0;
	if (g_pUserOption->DrawTransHPBar)			legacyGraph |= kGraphTransHPBarBit;
	if (g_pUserOption->BloodDrop)				legacyGraph |= kGraphBloodBit;
	if (g_pUserOption->AutoHideSmoothScroll)	legacyGraph |= kGraphSmoothAutoHideBit;
	if (g_pUserOption->DrawChatBoxOutline)		legacyGraph |= kGraphChatBalloonLineBit;
	if (g_pUserOption->DrawFPS)					legacyGraph |= kGraphShowFpsBit;
	if (g_pUserOption->DoNotShowPersnalShopMsg)	legacyGraph |= kGraphHideShopMsgBit;
	m_graphFlags = (m_graphFlags & ~legacyGraphMask) | legacyGraph;

	// Game bit'leri
	const unsigned int legacyGameMask =
		kGameHelperTipsBit | kGameDamageNumbersBit | kGameSystemMessagesBit | kGamePartyAlertsBit;
	unsigned int legacyGame = 0;
	if (g_pUserOption->ShowChoboHelp)			legacyGame |= kGameHelperTipsBit;
	if (g_pUserOption->ShowDamageNumber)		legacyGame |= kGameDamageNumbersBit;
	if (!g_pUserOption->DoNotShowWarMsg)		legacyGame |= kGameSystemMessagesBit;
	if (!g_pUserOption->DenyPartyRequest)		legacyGame |= kGamePartyAlertsBit;
	m_gameFlags = (m_gameFlags & ~legacyGameMask) | legacyGame;

	// Advanced bit'leri
	if (g_pUserOption->IsPreLoadMonster)
		m_advFlags |= kAdvBackgroundLoadBit;
	else
		m_advFlags &= ~kAdvBackgroundLoadBit;

	// Linear filtre bit'i uyeden turetilir (dosyadan geldi)
	if (m_bLinearFilter)
	{
		m_advFlags |= kAdvLinearFilterBit;
		m_adv2Flags |= kAdv2SpriteLinearBit;
	}
	else
	{
		m_advFlags &= ~kAdvLinearFilterBit;
		m_adv2Flags &= ~kAdv2SpriteLinearBit;
	}
}

void C_VS_UI_OPTION_RENEWAL::ApplyGraphFlagToLegacy(int bitIndex, bool bOn)
{
	switch (bitIndex)
	{
	case 0:	// D3D11 present interval + pacer etkilesim matrisi (plan F5)
		phoenix::render::SetVSyncEnabled(bOn);
		Phoenix_Pacer_NotifyVSync(bOn);
		break;
	case 1:	g_pUserOption->DrawTransHPBar = bOn ? TRUE : FALSE;			break;
	case 2:	g_pUserOption->BloodDrop = bOn ? TRUE : FALSE;				break;
	case 3:	g_pUserOption->AutoHideSmoothScroll = bOn ? TRUE : FALSE;	break;
	case 4:	g_pUserOption->DrawChatBoxOutline = bOn ? TRUE : FALSE;		break;
	case 5:	g_pUserOption->DrawFPS = bOn ? TRUE : FALSE;				break;
	case 6:	g_pUserOption->DoNotShowPersnalShopMsg = bOn ? TRUE : FALSE;break;
	case 7:	break;	// Hide other effects — persist-only
	case 8:	break;	// Hide other pets — persist-only
	}
}

void C_VS_UI_OPTION_RENEWAL::ApplyGameFlagToLegacy(int bitIndex, bool bOn)
{
	switch (bitIndex)
	{
	case 0:	g_pUserOption->ShowChoboHelp = bOn ? TRUE : FALSE;		break;
	case 1:	break;	// isim etiketleri — persist-only
	case 2:	g_pUserOption->ShowDamageNumber = bOn ? TRUE : FALSE;	break;
	case 3:	break;	// chat balonlari — persist-only
	case 4:	// sistem mesajlari: savas/lair/kutsal alan uclusunu birlikte yonetir
		g_pUserOption->DoNotShowWarMsg = bOn ? FALSE : TRUE;
		g_pUserOption->DoNotShowLairMsg = bOn ? FALSE : TRUE;
		g_pUserOption->DoNotShowHolyLandMsg = bOn ? FALSE : TRUE;
		break;
	case 5:	break;	// lonca isaretleri — persist-only
	case 6:	g_pUserOption->DenyPartyRequest = bOn ? FALSE : TRUE;	break;
	case 7:	break;	// esya atma onayi — persist-only
	case 8:	break;	// ticaret engelle — persist-only
	}
}

void C_VS_UI_OPTION_RENEWAL::ApplyAdvancedFlagToLegacy(int bitIndex, bool bOn)
{
	// 2026-09-03 plan F6 dogrulamasi: Reborn'da da bit 1-4 (NPC names /
	// chain effects / spell flashes / UI animation) persist-only — runtime
	// tuketicisi YOK (Application.cpp 2326-2335: yalniz bit0 background
	// loading, bit5 linear filter, bit6 skip intro sabit tanimli). Parity
	// geregi Phoenix'te de persist-only. bit5 ayri yoldan (m_bLinearFilter)
	// kablolu; bit6 burada canli koprulenir.
	switch (bitIndex)
	{
	case 0:	g_pUserOption->IsPreLoadMonster = bOn ? TRUE : FALSE;	break;
	case 6:	Phoenix_SetSkipIntroMedia(bOn);	break;	// plan F6 — skip intro
	default:	break;	// bit 1-4 persist-only (Reborn paritesi, ust nota bak)
	}
}

void C_VS_UI_OPTION_RENEWAL::SetLinearFilter(bool bLinear)
{
	m_bLinearFilter = bLinear;
	if (bLinear)
	{
		m_advFlags |= kAdvLinearFilterBit;
		m_adv2Flags |= kAdv2SpriteLinearBit;
		m_adv2Flags &= ~kAdv2PixelPerfectBit;
	}
	else
	{
		m_advFlags &= ~kAdvLinearFilterBit;
		m_adv2Flags &= ~kAdv2SpriteLinearBit;
	}
	// O2: dx11gpu frame default sampler'i (nearest/linear) aninda degisir
	phoenix::render::SetSpriteLinearFilter(bLinear);
}

void C_VS_UI_OPTION_RENEWAL::ResetToDefaults()
{
	CT_Done("OPTREN RESET defaults");

	m_graphFlags = kDefaultGraphFlags;
	m_gameFlags = kDefaultGameFlags;
	m_advFlags = kDefaultAdvFlags;
	m_adv2Flags = kDefaultAdv2Flags;
	m_frameLimit = 60;
	m_brightness = 50;
	m_value_gamma = 100;
	m_bDropdownOpen = false;
	SetLinearFilter(true);

	// O2 — runtime tarafi da default'a donsun
	Phoenix_Pacer_SetFrameLimit(m_frameLimit);
	Phoenix_Pacer_SetSmoothPacing((m_adv2Flags & kAdv2SmoothPacingBit) != 0);
	phoenix::render::SetUserBrightness(m_brightness);
	phoenix::render::SetSpriteCacheHighBudget((m_adv2Flags & kAdv2TextureCacheHighBit) != 0);

	// Legacy alanlara yansit
	for (int i = 0; i < 9; i++)
		ApplyGraphFlagToLegacy(i, (m_graphFlags & (1u << i)) != 0);
	for (int i = 0; i < 9; i++)
		ApplyGameFlagToLegacy(i, (m_gameFlags & (1u << i)) != 0);
	for (int i = 0; i < 7; i++)
		ApplyAdvancedFlagToLegacy(i, (m_advFlags & (1u << i)) != 0);

	g_pUserOption->DefaultAlpha = FALSE;
	g_pUserOption->ALPHA_DEPTH = kDefaultAlphaDepth;
	g_pUserOption->GammaValue = m_value_gamma;
	if (CDirectDraw::IsSupportGammaControl())
		CDirectDraw::SetGammaRamp(m_value_gamma);

	// Legacy ek satirlarin default'lari (UserOption ctor paritesi)
	g_pUserOption->FilteringCurse = TRUE;
	g_pUserOption->ChatWhite = FALSE;
	g_pUserOption->PopupChatByWhisper = TRUE;

	// Ses default'lari (Reborn: %80 + acik)
	m_volume_music = 80;
	m_volume_sound = 80;
	gpC_base->SendMessage(UI_CHANGE_OPTION, C_VS_UI_OPTION::CHECK_VALUE_MUSIC_VOLUME, m_volume_music);
	gpC_base->SendMessage(UI_CHANGE_OPTION, C_VS_UI_OPTION::CHECK_VALUE_SOUND_VOLUME, m_volume_sound);
	if (!g_pUserOption->PlayMusic)
		gpC_base->SendMessage(UI_CHANGE_OPTION, C_VS_UI_OPTION::CHECK_MUSIC, TRUE);
	if (!g_pUserOption->PlaySound)
		gpC_base->SendMessage(UI_CHANGE_OPTION, C_VS_UI_OPTION::CHECK_SOUND, TRUE);
}

void C_VS_UI_OPTION_RENEWAL::RequestClose()
{
	gpC_base->SendMessage(UI_CLOSE_OPTION);
}

/*-----------------------------------------------------------------------------
- OptionRenewal.set (key=value)
-----------------------------------------------------------------------------*/
void C_VS_UI_OPTION_RENEWAL::LoadRenewalSet()
{
	FILE* fp = fopen(kRenewalSetPath, "rt");
	if (!fp)
	{
		CT_Done("OPTREN LoadRenewalSet: '%s' yok, default'lar kullanilacak", kRenewalSetPath);
		return;
	}

	char line[256];
	while (fgets(line, sizeof(line), fp))
	{
		char key[64];
		unsigned int value = 0;
		if (sscanf(line, "%63[^=]=%u", key, &value) != 2)
			continue;

		if (strcmp(key, "GraphFlags") == 0)			m_graphFlags = value;
		else if (strcmp(key, "GameFlags") == 0)		m_gameFlags = value;
		else if (strcmp(key, "AdvFlags") == 0)		m_advFlags = value;
		else if (strcmp(key, "Adv2Flags") == 0)		m_adv2Flags = value;
		else if (strcmp(key, "FrameLimit") == 0)	m_frameLimit = (int)value;
		else if (strcmp(key, "LinearFilter") == 0)	m_bLinearFilter = (value != 0);
		else if (strcmp(key, "Brightness") == 0)	m_brightness = (int)value;
		else if (strcmp(key, "Fullscreen") == 0)	m_bFullscreen = (value != 0);
		else if (strcmp(key, "PendingResW") == 0)	m_pendingResW = (int)value;
		else if (strcmp(key, "PendingResH") == 0)	m_pendingResH = (int)value;
	}
	fclose(fp);

	if (m_brightness < 0 || m_brightness > 100)
		m_brightness = 50;
	if (m_frameLimit != 0 && m_frameLimit != 30 && m_frameLimit != 60 &&
		m_frameLimit != 120 && m_frameLimit != 144)
		m_frameLimit = 60;

	CT_Done("OPTREN LoadRenewalSet OK graph=0x%X game=0x%X adv=0x%X adv2=0x%X fl=%d",
		m_graphFlags, m_gameFlags, m_advFlags, m_adv2Flags, m_frameLimit);
}

void C_VS_UI_OPTION_RENEWAL::SaveRenewalSet()
{
	FILE* fp = fopen(kRenewalSetPath, "wt");
	if (!fp)
	{
		CT_Done("OPTREN SaveRenewalSet: '%s' ACILAMADI (errno write fail)", kRenewalSetPath);
		return;
	}

	fprintf(fp, "# Phoenix OptionRenewal.set — Reborn option menu ayarlari\n");
	fprintf(fp, "GraphFlags=%u\n", m_graphFlags);
	fprintf(fp, "GameFlags=%u\n", m_gameFlags);
	fprintf(fp, "AdvFlags=%u\n", m_advFlags);
	fprintf(fp, "Adv2Flags=%u\n", m_adv2Flags);
	fprintf(fp, "FrameLimit=%d\n", m_frameLimit);
	fprintf(fp, "LinearFilter=%d\n", m_bLinearFilter ? 1 : 0);
	fprintf(fp, "Brightness=%d\n", m_brightness);
	fprintf(fp, "Fullscreen=%d\n", m_bFullscreen ? 1 : 0);
	fprintf(fp, "PendingResW=%d\n", m_pendingResW);
	fprintf(fp, "PendingResH=%d\n", m_pendingResH);
	fclose(fp);

	CT_Done("OPTREN SaveRenewalSet OK");
}
