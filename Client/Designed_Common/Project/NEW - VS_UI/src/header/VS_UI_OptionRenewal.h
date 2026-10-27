/*-----------------------------------------------------------------------------

	VS_UI_OptionRenewal.h

	Reborn Option menusu portu (E:\DK Phoenix Reborn -> Legacy).
	2026-09-02 — Plan: reborn_option_menü_portu

	5 sekme: Graphics / Sound / Game / Advanced / Advanced2.
	Sprite pack: data\ui\spk\new\renewaloption\00000.spk (33 frame tek dosya,
	Reborn OptionSpriteMetadata sirasi — E:'den birebir kopya)
	Hotkey penceresi: data\ui\spk\new\hotkeysettingwindow (32 frame, 16'sar)

	Kalicilik iki katman:
	  - Legacy karsiligi olan ayarlar -> g_pUserOption (UserOption.set, binary
	    format DEGISMEZ; kayit Execute_UI_CLOSE_OPTION uzerinden)
	  - Reborn'a ozgu yeniler -> UserSet\OptionRenewal.set (key=value)

	Eski menu C_VS_UI_OPTION implementasyonu VS_UI_NewTitle.cpp'de #if 0
	blogunda tutulur (silinmedi); enum'lari codebase genelinde kullanildigi
	icin sinif deklarasyonu VS_UI_Newtitle.h'ta aynen durur.

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_OPTIONRENEWAL_H__
#define __VS_UI_OPTIONRENEWAL_H__

#include "u_window.h"

class C_SPRITE_PACK;
class C_VS_UI_DIALOG;

class C_VS_UI_OPTION_RENEWAL : public Window
{
public:
	enum RENEWAL_TAB
	{
		TAB_GRAPHICS,
		TAB_SOUND,
		TAB_GAME,
		TAB_ADVANCED,
		TAB_ADVANCED2,

		TAB_COUNT
	};

	// renewaloption pack frame indeksleri (Reborn OptionSpriteMetadata paritesi)
	enum OPTION_SPK
	{
		SPR_WINDOW_BG = 0,
		SPR_PANEL_BG = 1,			// dropdown / frame-limit kutusu
		SPR_SHORTCUT_PANEL_BG = 2,
		SPR_GREEN_BTN = 3,
		SPR_GREEN_BTN_HOVER = 4,
		SPR_GREEN_BTN_PRESS = 5,
		SPR_RED_BTN = 6,
		SPR_RED_BTN_HOVER = 7,
		SPR_RED_BTN_PRESS = 8,
		SPR_TEAL_BTN = 9,
		SPR_TEAL_BTN_HOVER = 10,
		SPR_TEAL_BTN_PRESS = 11,
		SPR_LARGE_TEAL_BTN = 12,
		SPR_LARGE_TEAL_BTN_HOVER = 13,
		SPR_LARGE_TEAL_BTN_PRESS = 14,
		SPR_CLOSE_BTN = 15,
		SPR_CLOSE_BTN_HOVER = 16,
		SPR_CLOSE_BTN_PRESS = 17,
		SPR_TAB_NORMAL = 18,
		SPR_TAB_HOVER = 19,
		SPR_TAB_PRESS = 20,
		SPR_CHECK_OFF = 21,
		SPR_CHECK_OFF_GREY = 22,
		SPR_CHECK_ON = 23,
		SPR_CHECK_ON_GREY = 24,
		SPR_SEPARATOR_H = 25,
		SPR_SEPARATOR_V = 26,
		SPR_OPACITY_LIGHT = 27,		// slider dolgu
		SPR_OPACITY_DARK = 28,		// slider ray
		SPR_UP_ARROW = 29,			// slider ibresi / dropdown ok
		SPR_VOLUME_BG = 30,
		SPR_VOLUME_FILL = 31,
		SPR_RADIO = 32
	};

	C_VS_UI_OPTION_RENEWAL(bool IsTitle = false);
	~C_VS_UI_OPTION_RENEWAL();

	// PI / Window sozlesmesi
	void	Start();
	void	Finish();
	void	Show();
	void	Process();
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	bool	IsPixel(int _x, int _y);
	void	AcquireFirstSequence() {}
	void	UnacquireMouseFocus() {}
	void	AcquireDisappear() {}

	// Vs_ui.cpp uyumu — hotkey toggle'lari (Ctrl+M vb.) menudeki isareti
	// senkron tutar. Parametre eski C_VS_UI_OPTION::EXEC_CHECK_ID degeriydi;
	// yeni menu dogrudan g_pUserOption'dan okudugu icin yok sayilir.
	void	Background_Music_ONOFF(int check_id);
	void	Effect_Music_ONOFF(int check_id);

private:
	// --- sprite pack'ler ---
	C_SPRITE_PACK*	m_pSpk;			// renewaloption 00000.spk (33 frame, tek dosya)
	C_SPRITE_PACK*	m_pHotkeySpk[2];// hotkeysettingwindow 00000/00016 (16'sar)

	C_VS_UI_DIALOG*	m_pC_dialog;	// tus sifirlama onay dialogu

	bool			m_IsTitle;

	// --- durum ---
	int				m_iTab;					// RENEWAL_TAB
	bool			m_bDropdownOpen;		// cozunurluk dropdown
	bool			m_bHotkeyOpen;			// SHORTKEY SETTINGS penceresi
	bool			m_bLBPush;				// slider surukleme
	int				m_iDragSlider;			// DRAG_* sabitleri

	// --- legacy-mapped aynalar (kaynak: g_pUserOption) ---
	int				m_volume_music;			// 0..100
	int				m_volume_sound;			// 0..100
	int				m_value_gamma;			// 50..150
	// --- Reborn'a ozgu (OptionRenewal.set) ---
	unsigned int	m_graphFlags;			// 9 bit (Graphics efekt listesi)
	unsigned int	m_gameFlags;			// 9 bit (Game listesi)
	unsigned int	m_advFlags;				// 7 bit (Advanced listesi)
	unsigned int	m_adv2Flags;			// Advanced2 bit'leri
	int				m_frameLimit;			// 0=Unlimited / 30/60/120/144
	bool			m_bLinearFilter;
	int				m_brightness;			// 0..100
	int				m_pendingResW, m_pendingResH;
	bool			m_bFullscreen;

	// --- yerlesim yardimcilari (Reborn 1280x960 referans canvas) ---
	int		RX(int refX) const;				// ref canvas -> ekran (option penceresi)
	int		RY(int refY) const;
	int		HX(int refX) const;				// hotkey penceresi icin
	int		HY(int refY) const;
	bool	HitR(int refX, int refY, int refW, int refH, int sx, int sy) const;
	bool	HitH(int refX, int refY, int refW, int refH, int sx, int sy) const;

	// --- cizim yardimcilari ---
	void	BltFrame(int frame, int sx, int sy);
	void	BltFrameClip(int frame, int sx, int sy, int w, int h);
	void	BltFrameEdges(int frame, int sx, int sy, int tw, int th);
	void	BltHotkeyFrame(int frame, int sx, int sy);
	void	DrawLabel(int sx, int sy, const char* str, COLORREF color);
	void	DrawCheckAt(int refX, int refY, bool bChecked, bool bEnabled = true);
	void	DrawSliderAt(int refX, int refY, int refW, int ratio100);
	void	DrawTabsAndChrome();
	void	DrawGraphicsTab();
	void	DrawSoundTab();
	void	DrawGameTab();
	void	DrawAdvancedTab();
	void	DrawAdvanced2Tab();
	void	DrawHotkeyWindow();

	// --- input yardimcilari ---
	bool	HandleClick(int sx, int sy);
	bool	HandleClickGraphics(int sx, int sy);
	bool	HandleClickSound(int sx, int sy);
	bool	HandleClickGame(int sx, int sy);
	bool	HandleClickAdvanced(int sx, int sy);
	bool	HandleClickAdvanced2(int sx, int sy);
	bool	HandleClickHotkey(int sx, int sy);
	void	HandleSliderDrag(int sx, int sy);
	void	ReleaseSlider();

	// --- ayar akisi ---
	void	LoadFromLegacy();				// g_pUserOption -> aynalar
	void	LoadRenewalSet();				// OptionRenewal.set -> uyeler
	void	SaveRenewalSet();				// uyeler -> OptionRenewal.set
	void	ApplyGraphFlagToLegacy(int bitIndex, bool bOn);
	void	ApplyGameFlagToLegacy(int bitIndex, bool bOn);
	void	ApplyAdvancedFlagToLegacy(int bitIndex, bool bOn);
	void	SetLinearFilter(bool bLinear);
	void	ResetToDefaults();
	void	RequestClose();
};

#endif // __VS_UI_OPTIONRENEWAL_H__
