/*-----------------------------------------------------------------------------

	VS_UI_skill_view.h

-----------------------------------------------------------------------------*/
// 2026-05-12: SkillInfoWindow header tamami /*...*/ ile yorumlanmistir
// (line 7'de /* baslayip line 144'te */ ile biten dev blok). Class definition
// + header guard hepsi disabled idi → #include "VS_UI_SKILL_VIEW.h" hiçbir
// tip tanimlamiyordu → undefined type C2027. Yorum acildi.
#ifndef __VS_UI_SKILL_VIEW_H__
#define __VS_UI_SKILL_VIEW_H__

#include "VS_UI_Widget.h"
#include "VS_UI_Util.h"

#include "MSkillManager.h"

//#define SKILLTREE_CLOSE_ID				0xFFFFFFFF

//struct SKILLTREE_SKILL
//{
//	ACTIONINFO				id;
//};

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_VIEW
//
// 
//-----------------------------------------------------------------------------
class C_VS_UI_SKILL_VIEW : public Window, public Exec, public ButtonVisual
{
private:
	enum EXEC_ID
	{
		CLOSE_ID,
		HELP_ID,
		SCROLL_UP_ID,
		SCROLL_DOWN_ID,

		GRADE_PREV_ID,
		GRADE_NEXT_ID,

		//slayer
		SWORD_BUTTON_ID = 6,
		BLADE_BUTTON_ID,
		GUN_BOMB_BUTTON_ID,
		HEAL_BUTTON_ID,
		ENCHANT_BUTTON_ID,

		//vampire
		POISON_BUTTON_ID = 6,
		ACID_BUTTON_ID,
		CURSE_BUTTON_ID,
		SUMMON_BUTTON_ID,
		BLOOD_BUTTON_ID,
		INNATE_BUTTON_ID,

		// 2026-05-22 GERİ ALINDI: Görev 3 Ousters branch'leri referansla parity için
		// yorum içine alındı (Kural 4.1). Referansta Ousters K menü yok — Phoenix-only patch.
		// Ousters K menü desteği bu yüzden kapanır; restore istenirse yorumdan çıkarılır.
		//ousters (2026-05-22 Görev 3: COMBAT + ELEMENTAL, race-mutex enum overlap)
		//OUSTERS_COMBAT_BUTTON_ID = 6,
		//OUSTERS_ELEMENTAL_BUTTON_ID,
	};

	// 2026-05-22: skillwindow.spk frame layout dokümantasyonu (kullanıcı SPK viewer analizi).
	// NOT: Tur 3'te SKILL_GUARD=32 yapılmıştı (frame 1=Slayer BG hipotezi ile).
	// Kullanıcı netleştirdi: "Phoenix eski client'ın modernize hali, görsel/ses dosyaları
	// aynı." Yani referans `SKILL_GUARD=1` (anonim enum default) ile eski client çalışıyor.
	// Tur 3 enum değişikliği referansla parity bozdu → geri alındı.
	enum SKILL_VIEW_SPK_INDEX
	{
		SKILL_VIEW_BACK,        // = 0 (anonim enum default, referansla parity)
		SKILL_GUARD,            // = 1
		SKILL_GUARD_SELECTED,   // = 2
	};

	enum SKILL_VIEW_ETC_INDEX
	{
		EXP_BAR,
		GRADE_PREV_PUSHED,
		GRADE_NEXT_PUSHED,
	};

	enum SKILL_VIEW_SLAYER_DOMAIN_INDEX
	{
		SWORD_BUTTON,
		BLADE_BUTTON,
		GUN_BOMB_BUTTON,
		HEAL_BUTTON,
		ENCHANT_BUTTON,
	};

	enum SKILL_VIEW_VAMPIRE_DOMAIN_INDEX
	{
		POISON_BUTTON,
		ACID_BUTTON,
		CURSE_BUTTON,
		SUMMON_BUTTON,
		BLOOD_BUTTON,
		INNATE_BUTTON,
	};

	// 2026-05-22 GERİ ALINDI: Görev 3 Ousters domain enum referansla parity için yorum.
	/*
	enum SKILL_VIEW_OUSTERS_DOMAIN_INDEX
	{
		OUSTERS_COMBAT_BUTTON,    // SPK frame 0
		OUSTERS_ELEMENTAL_BUTTON, // SPK frame 1
	};
	*/

	C_SPRITE_PACK *				m_p_skill_view_spk;
	C_SPRITE_PACK *				m_p_skill_view_etc_spk;
	C_SPRITE_PACK *				m_p_skill_view_grade_spk;

//	CSpritePack					m_pC_skill_icon_spk;
	C_SPRITE_PACK *				m_pC_skill_view_domain_spk;

	ButtonGroup *				m_pC_button_group;

	RECT						m_rcSkillDesciption;
	int							m_iGrade;
	int							m_iDomain;
	int							m_iScroll;

	SKILLDOMAIN					m_skill_domain;
	int							m_domain_level[6];  // 2026-05-22 GERİ ALINDI: 8→6 (referansla parity, Ousters slot kaldırıldı)

	int							m_skill_x, m_skill_y, m_skill_x_gap, m_skill_y_gap;

//	int							m_extra_offset_x, m_extra_offset_y; // ��ӹ޴� ������ set�� ��.

//	enum
//	{
//		SKILL_GUARD_X = 44,
//		SKILL_GUARD_Y = 44,
//	};

//	enum ETC_IMAGE_SPK_INDEX
//	{
//		ICON_GUARD,
//	};

public:
	C_VS_UI_SKILL_VIEW();
	~C_VS_UI_SKILL_VIEW();

	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);
	void	Process();

	void	AcquireDisappear() {}
	void	UnacquireMouseFocus() { m_pC_button_group->UnacquireMouseFocus(); }
	void	CancelPushState() { m_pC_button_group->CancelPushState(); }
	void	Run(id_t id);
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	Show();

	void	Start();
	void	Finish();
};

#endif