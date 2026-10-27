// VS_UI_SKILL_VIEW.cpp: implementation of the C_VS_UI_SKILL_VIEW class.
//
//////////////////////////////////////////////////////////////////////

#include "client_PCH.h"
#include "crash_trace_tool.h"

// 2026-05-12: Tum class implementation /*...*/ blok-yorum icindeydi
// (line 8 /* ... line 901 */) -> linker C_VS_UI_SKILL_VIEW symbol'lerini
// (ctor, dtor, Show, Run, Start, Finish vs) bulamiyordu -> LNK2019.
// Yorum kaldirildi, 901 satirlik class implementation artik aktif.
//
// 2026-05-12 modernize: 99 derleme hatasi giderildi
//  - (g_eRaceInterface == RACE_VAMPIRE) -> (g_eRaceInterface == RACE_VAMPIRE) [3 irk parite icin]
//  - m_pC_spk -> m_C_spk (C_VS_UI_SKILL'in static uyesi rename edilmis)
//  - GetDomainExp() -> GetDomainExpRemain() (Phoenix DWORD donen API)
//  - C_VS_UI_DESC_DIALOG::VAMPIRESKILLINFO/SLAYERSKILLINFO -> ::SKILL
//    (Phoenix INTERFACE_HELP_ID enum'unda konsolide edildi)
//  - m_default_string / m_grade_string 30-byte sinirini asan CJK literal
//    yerine kisa Ingilizce string'ler (Phoenix kural #3: oyun ici string Ingilizce)
//  - SPK_SKILLVIEW_* path'leri VS_UI_filepath.h'a eklendi
#include "VS_UI_SKILL_VIEW.h"
#include "VS_UI_GameCommon.h"
#include "VS_UI_filepath.h"
#include "VS_UI_description.h"
#include "VS_UI_Mouse_pointer.h"
#include "VS_UI.h"
#include "VS_UI_ExtraDialog.h"   // C_VS_UI_DESC_DIALOG::SKILL enum icin
#include "RaceType.h"            // Race enum + g_eRaceInterface extern karsiligi
#include "SystemAvailabilities.h" // g_pSystemAvailableManager (skill name cap-altinda "???" gosterimi)

//domain�� ������ �׷����� ������ �������� ������ Ʋ���� ��ȯ�� ���̺�
int temp_domain[5] = {1,0,2,3,4};

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_VIEW
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_SKILL_VIEW::C_VS_UI_SKILL_VIEW()
{
	m_pC_button_group = NULL;
	m_p_skill_view_etc_spk = NULL;
	m_p_skill_view_spk = NULL;
	m_p_skill_view_grade_spk = NULL; 
	m_pC_skill_view_domain_spk = NULL;

	g_RegisterWindow(this);
	AttrPin(true);

	m_pC_button_group = new ButtonGroup(this);

	m_p_skill_view_etc_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_ETC);

// member data init
//	m_iGrade = 0;
//	m_iDomain = 0;
//	m_iScroll = 0;

	// 2026-05-22 GERİ ALINDI: Görev 3 Ousters domain init referansla parity için yorum.
	/*
	if (g_eRaceInterface == RACE_OUSTERS)
	{
		m_skill_domain = SKILLDOMAIN_OUSTERS;
		m_iDomain = 0;  // COMBAT (m_iDomain + SKILL_STEP_OUSTERS_COMBAT)
		m_iGrade = 0;
		m_iScroll = 0;
	}
	else
	*/
	if((g_eRaceInterface == RACE_VAMPIRE))
	{
//		if(m_skill_domain != SKILLDOMAIN_VAMPIRE)
//		{
			m_skill_domain = SKILLDOMAIN_VAMPIRE;
			m_iDomain = 0;
			m_iGrade = 0;
			m_iScroll = 0;
//		}
	}
	else
	{
//		if(m_skill_domain == SKILLDOMAIN_VAMPIRE)
//		{
		int level = -1;
		for(int i = 0; i < 5; i++)
		{
			int level_temp = (*g_pSkillManager)[(SKILLDOMAIN)i].GetDomainLevel();
			if(level_temp > level)
			{
				level = level_temp;
				m_skill_domain = (SKILLDOMAIN)i;
				m_iDomain = temp_domain[i];
			}
		}
			m_iGrade = 0;
			m_iScroll = 0;
//		}
	}

	// 2026-05-22 GERİ ALINDI: Görev 3 Ousters SPK load referansla parity için yorum.
	/*
	if (g_eRaceInterface == RACE_OUSTERS)
	{
		m_p_skill_view_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_OUSTERS);
		m_p_skill_view_grade_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_OUSTERS_GRADE);
		m_pC_skill_view_domain_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_OUSTERS_DOMAIN);
	}
	else
	*/
	if((g_eRaceInterface == RACE_VAMPIRE))
	{
		m_p_skill_view_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_VAMPIRE);
		m_p_skill_view_grade_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_VAMPIRE_GRADE);
		m_pC_skill_view_domain_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_VAMPIRE_DOMAIN);
	}
	else
	{
		m_p_skill_view_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_SLAYER);
		m_p_skill_view_grade_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_SLAYER_GRADE);
		m_pC_skill_view_domain_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_SLAYER_DOMAIN);
	}

	Set(RESOLUTION_X/2-m_p_skill_view_spk->GetWidth(SKILL_VIEW_BACK)/2, RESOLUTION_Y/2-m_p_skill_view_spk->GetHeight(SKILL_VIEW_BACK)/2, m_p_skill_view_spk->GetWidth(SKILL_VIEW_BACK), m_p_skill_view_spk->GetHeight(SKILL_VIEW_BACK));


// button
	int close_button_x, close_button_y, help_button_x, help_button_y;
	int scroll_x, scroll_up_y, scroll_down_y;
	int grade_y, grade_prev_x, grade_next_x;

	// 2026-05-22 GERİ ALINDI: Görev 3 Ousters koordinatları referansla parity için yorum.
	/*
	if (g_eRaceInterface == RACE_OUSTERS)
	{
		help_button_y = close_button_y = 287;
		help_button_x = 189;
		close_button_x = 257;
		scroll_x = 319;
		scroll_up_y = 143;
		scroll_down_y = 255;
		grade_prev_x = 129;
		grade_next_x = 316;
		grade_y = 108;
	}
	else
	*/
	if((g_eRaceInterface == RACE_VAMPIRE))
	{
		// Domain_ICON load
//		ifstream	file(SPK_SKILLVIEW_VAMPIRE_DOMAIN, ios::binary|ios::nocreate);
//		if (!file)
//			_Error(FILE_OPEN);
//		m_pC_skill_view_domain_spk.LoadFromFile(file);
//		file.close();

		help_button_y = close_button_y = 417;
		help_button_x = 220;
		close_button_x = 288;
		scroll_x = 319-7;
		scroll_up_y = 143-4;
		scroll_down_y = 316;
		grade_prev_x = 129-9;
		grade_next_x = 316-9;
		grade_y = 102;
	}
	else
	{
		// Domain_ICON load
//		ifstream	file(SPK_SKILLVIEW_SLAYER_DOMAIN, ios::binary|ios::nocreate);
//		if (!file)
//			_Error(FILE_OPEN);
//		m_pC_skill_view_domain_spk.LoadFromFile(file);
//		file.close();

		help_button_y = close_button_y = 287;
		help_button_x = 189;
		close_button_x = 257;
		scroll_x = 319;
		scroll_up_y = 143;
		scroll_down_y = 255;
		grade_prev_x = 129;
		grade_next_x = 316;
		grade_y = 108;
	}


	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(help_button_x, help_button_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_HELP), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_HELP), HELP_ID, this, C_GLOBAL_RESOURCE::AB_BUTTON_HELP));
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(close_button_x, close_button_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_CLOSE), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_CLOSE), CLOSE_ID, this, C_GLOBAL_RESOURCE::AB_BUTTON_CLOSE));
//	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(scroll_x, scroll_up_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_UP), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_UP), SCROLL_UP_ID, this, C_GLOBAL_RESOURCE::AB_BUTTON_UP));
//	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(scroll_x, scroll_down_y, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_DOWN), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_DOWN), SCROLL_DOWN_ID, this, C_GLOBAL_RESOURCE::AB_BUTTON_DOWN));

	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(grade_prev_x, grade_y, m_p_skill_view_etc_spk->GetWidth(GRADE_PREV_PUSHED), m_p_skill_view_etc_spk->GetHeight(GRADE_PREV_PUSHED), GRADE_PREV_ID, this, GRADE_PREV_PUSHED));
	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(grade_next_x, grade_y, m_p_skill_view_etc_spk->GetWidth(GRADE_NEXT_PUSHED), m_p_skill_view_etc_spk->GetHeight(GRADE_NEXT_PUSHED), GRADE_NEXT_ID, this, GRADE_NEXT_PUSHED));

	// 2026-05-22 GERİ ALINDI: Görev 3 Ousters button add referansla parity için yorum.
	/*
	if (g_eRaceInterface == RACE_OUSTERS)
	{
		m_skill_x = 120;
		m_skill_y = 144;
		m_skill_x_gap = 66;
		m_skill_y_gap = 63;

		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(41, 120,    m_pC_skill_view_domain_spk->GetWidth(OUSTERS_COMBAT_BUTTON),    m_pC_skill_view_domain_spk->GetHeight(OUSTERS_COMBAT_BUTTON),    OUSTERS_COMBAT_BUTTON_ID,    this, OUSTERS_COMBAT_BUTTON));
		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(41, 120+36, m_pC_skill_view_domain_spk->GetWidth(OUSTERS_ELEMENTAL_BUTTON), m_pC_skill_view_domain_spk->GetHeight(OUSTERS_ELEMENTAL_BUTTON), OUSTERS_ELEMENTAL_BUTTON_ID, this, OUSTERS_ELEMENTAL_BUTTON));
	}
	else
	*/
	if((g_eRaceInterface == RACE_VAMPIRE))
	{
		m_skill_x = 112;
		m_skill_y = 140;
		m_skill_x_gap = 64;
		m_skill_y_gap = 65;


		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(103, 342, m_pC_skill_view_domain_spk->GetWidth(POISON_BUTTON), m_pC_skill_view_domain_spk->GetHeight(POISON_BUTTON), POISON_BUTTON_ID, this, POISON_BUTTON));
		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(103+80, 342, m_pC_skill_view_domain_spk->GetWidth(ACID_BUTTON), m_pC_skill_view_domain_spk->GetHeight(ACID_BUTTON), ACID_BUTTON_ID, this, ACID_BUTTON));
		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(103+80*2, 342, m_pC_skill_view_domain_spk->GetWidth(CURSE_BUTTON), m_pC_skill_view_domain_spk->GetHeight(CURSE_BUTTON), CURSE_BUTTON_ID, this, CURSE_BUTTON));
		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(103, 342+29, m_pC_skill_view_domain_spk->GetWidth(SUMMON_BUTTON), m_pC_skill_view_domain_spk->GetHeight(SUMMON_BUTTON), SUMMON_BUTTON_ID, this, SUMMON_BUTTON));
		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(103+80, 342+29, m_pC_skill_view_domain_spk->GetWidth(BLOOD_BUTTON), m_pC_skill_view_domain_spk->GetHeight(BLOOD_BUTTON), BLOOD_BUTTON_ID, this, BLOOD_BUTTON));
		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(103+80*2, 342+29, m_pC_skill_view_domain_spk->GetWidth(INNATE_BUTTON), m_pC_skill_view_domain_spk->GetHeight(INNATE_BUTTON), INNATE_BUTTON_ID, this, INNATE_BUTTON));
	}
	else
	{
		m_skill_x = 120;
		m_skill_y = 144;
		m_skill_x_gap = 66;
		m_skill_y_gap = 63;

		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(41, 120, m_pC_skill_view_domain_spk->GetWidth(SWORD_BUTTON), m_pC_skill_view_domain_spk->GetHeight(SWORD_BUTTON), SWORD_BUTTON_ID, this, SWORD_BUTTON));
		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(41, 120+36, m_pC_skill_view_domain_spk->GetWidth(BLADE_BUTTON), m_pC_skill_view_domain_spk->GetHeight(BLADE_BUTTON), BLADE_BUTTON_ID, this, BLADE_BUTTON));
		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(41, 120+36*2, m_pC_skill_view_domain_spk->GetWidth(GUN_BOMB_BUTTON), m_pC_skill_view_domain_spk->GetHeight(GUN_BOMB_BUTTON), GUN_BOMB_BUTTON_ID, this, GUN_BOMB_BUTTON));
		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(41, 120+36*3, m_pC_skill_view_domain_spk->GetWidth(HEAL_BUTTON), m_pC_skill_view_domain_spk->GetHeight(HEAL_BUTTON), HEAL_BUTTON_ID, this, HEAL_BUTTON));
		m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(41, 120+36*4, m_pC_skill_view_domain_spk->GetWidth(ENCHANT_BUTTON), m_pC_skill_view_domain_spk->GetHeight(ENCHANT_BUTTON), ENCHANT_BUTTON_ID, this, ENCHANT_BUTTON));
	}
	SetRect(&m_rcSkillDesciption, m_skill_x, m_skill_y, m_skill_x + m_skill_x_gap*3, m_skill_y + m_skill_y_gap*((g_eRaceInterface == RACE_VAMPIRE)?3:2));

#ifndef _LIB
	(*g_pSkillManager)[SKILLDOMAIN_SWORD].SetDomainLevel(1);
	(*g_pSkillManager)[SKILLDOMAIN_SWORD].SetDomainExp(990);
	(*g_pSkillManager)[SKILLDOMAIN_BLADE].SetDomainLevel(30);
	(*g_pSkillManager)[SKILLDOMAIN_GUN].SetDomainLevel(60);
	(*g_pSkillManager)[SKILLDOMAIN_HEAL].SetDomainLevel(80);
	(*g_pSkillManager)[SKILLDOMAIN_ENCHANT].SetDomainLevel(100);
	(*g_pSkillManager)[SKILLDOMAIN_VAMPIRE].SetDomainLevel(21);

	MSkillDomain& swordDomain = (*g_pSkillManager)[SKILLDOMAIN_SWORD];

	swordDomain.SetNewSkill();
	swordDomain.LearnSkill( SKILL_DOUBLE_IMPACT );
	swordDomain.SetNewSkill();
	swordDomain.LearnSkill( SKILL_TRIPLE_SLASHER );
	swordDomain.SetNewSkill();
	swordDomain.LearnSkill( SKILL_THUNDER_SPARK );
	swordDomain.SetNewSkill();
	swordDomain.LearnSkill( SKILL_RAINBOW_SLASHER );
	swordDomain.SetNewSkill();
	swordDomain.LearnSkill( SKILL_SNAKE_COMBO );
	swordDomain.SetNewSkill();
	swordDomain.LearnSkill( SKILL_CROSS_COUNTER );
	swordDomain.SetNewSkill();
	(*g_pSkillInfoTable)[SKILL_DOUBLE_IMPACT].SetExpLevel( 00 );
	(*g_pSkillInfoTable)[SKILL_TRIPLE_SLASHER].SetExpLevel( 20 );
	(*g_pSkillInfoTable)[SKILL_THUNDER_SPARK].SetExpLevel( 40 );
	(*g_pSkillInfoTable)[SKILL_RAINBOW_SLASHER].SetExpLevel( 60 );
	(*g_pSkillInfoTable)[SKILL_SNAKE_COMBO].SetExpLevel( 80 );
	(*g_pSkillInfoTable)[SKILL_CROSS_COUNTER].SetExpLevel( 100 );

	(*g_pSkillInfoTable)[SKILL_DOUBLE_IMPACT].SetSkillExp( 149 );
	(*g_pSkillInfoTable)[SKILL_TRIPLE_SLASHER].SetSkillExp( 19999 );
	(*g_pSkillInfoTable)[SKILL_THUNDER_SPARK].SetSkillExp( 40 );
	(*g_pSkillInfoTable)[SKILL_RAINBOW_SLASHER].SetSkillExp( 60 );
	(*g_pSkillInfoTable)[SKILL_SNAKE_COMBO].SetSkillExp( 80 );
	(*g_pSkillInfoTable)[SKILL_CROSS_COUNTER].SetSkillExp( 100 );

	(*g_pSkillManager)[SKILLDOMAIN_GUN].SetNewSkill();
	(*g_pSkillManager)[SKILLDOMAIN_GUN].LearnSkill(SKILL_FAST_RELOAD);

	(*g_pSkillManager)[SKILLDOMAIN_VAMPIRE].SetNewSkill();
	(*g_pSkillManager)[SKILLDOMAIN_VAMPIRE].LearnSkill(MAGIC_ACID_TOUCH);
	
#endif
	m_domain_level[0] = 29-29*(*g_pSkillManager)[SKILLDOMAIN_SWORD].GetDomainLevel()/100;
	m_domain_level[1] = 29-29*(*g_pSkillManager)[SKILLDOMAIN_BLADE].GetDomainLevel()/100;
	m_domain_level[2] = 29-29*(*g_pSkillManager)[SKILLDOMAIN_GUN].GetDomainLevel()/100;
	m_domain_level[3] = 29-29*(*g_pSkillManager)[SKILLDOMAIN_HEAL].GetDomainLevel()/100;
	m_domain_level[4] = 29-29*(*g_pSkillManager)[SKILLDOMAIN_ENCHANT].GetDomainLevel()/100;
	m_domain_level[5] = 29-29*(*g_pSkillManager)[SKILLDOMAIN_VAMPIRE].GetDomainLevel()/100;

//			EFFECT_DARKER = 0,
//			EFFECT_GRAYSCALE,
//			EFFECT_LIGHTEN,
//			EFFECT_DARKEN,
//			EFFECT_COLORDODGE,
//			EFFECT_SCREEN,
//			EFFECT_DODGEBURN,
//			EFFECT_DIFFERENT,


// button groups
//	m_pC_button_group->Add(new C_VS_UI_EVENT_BUTTON(status_x_offset, status_y_offset, gpC_global_resource->m_pC_assemble_box_button_spk->GetWidth(C_GLOBAL_RESOURCE::AB_BUTTON_UP), gpC_global_resource->m_pC_assemble_box_button_spk->GetHeight(C_GLOBAL_RESOURCE::AB_BUTTON_UP), STATUS_UP, this, C_GLOBAL_RESOURCE::AB_BUTTON_UP));

//	m_skill_domain = MAX_SKILLDOMAIN;
}

//-----------------------------------------------------------------------------
// ~C_VS_UI_SKILL_VIEW
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_SKILL_VIEW::~C_VS_UI_SKILL_VIEW()
{
	g_UnregisterWindow(this);

	DeleteNew(m_p_skill_view_etc_spk); 
	DeleteNew(m_p_skill_view_spk); 
	DeleteNew(m_p_skill_view_grade_spk); 
	DeleteNew(m_pC_skill_view_domain_spk); 

	DeleteNew(m_pC_button_group);
}	

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_VIEW::Show
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_SKILL_VIEW::Show()
{
	CT_Done("WnSh: C_VS_UI_SKILL_VIEW::Show ENTER pthis=%p", (void*)this);
	CSpriteSurface::SetEffect(CSpriteSurface::EFFECT_GRAY_SCALE);

	m_p_skill_view_spk->Blt(x, y, SKILL_VIEW_BACK);

	// 2026-05-22 RACE BG YORUM İÇİNE ALINDI (Tur 3 yan etkisi geri çekme):
	// Kullanıcı netleştirdi: "Eski clientte sadece IRK BG'leri eklenmemiş ama mantık
	// tamamen çalışıyor". Referans Show() (ClientSRC Referans line 266) sadece ana menü
	// `Blt(SKILL_VIEW_BACK)` çağırıyor, race BG yok. Bu Phoenix-only patch idi.
	/*
	int race_bg_frame = (g_eRaceInterface == RACE_SLAYER)  ? SLAYER_RACE_BG  :
	                    (g_eRaceInterface == RACE_VAMPIRE) ? VAMPIRE_RACE_BG :
	                    (g_eRaceInterface == RACE_OUSTERS) ? OUSTERS_RACE_BG : SKILL_VIEW_BACK;
	m_p_skill_view_spk->Blt(x, y, race_bg_frame);
	*/

	int grade_x, grade_y, skill_ico_x, skill_ico_y;
	int scroll_size = ((g_eRaceInterface == RACE_VAMPIRE)?9:6);

	int scroll_x, scroll_y, scroll_height;

	if((g_eRaceInterface == RACE_VAMPIRE))
	{
		grade_x = x + 142;
		grade_y = y + 101;

		skill_ico_x = x+m_skill_x + 13;
		skill_ico_y = y+m_skill_y + 14;

		scroll_x = x + 313;
		scroll_y = y + 160;
		scroll_height = 134;

	}
	else
	{
		grade_x = x + 151;
		grade_y = y + 108;

		skill_ico_x = x+m_skill_x + 14;
		skill_ico_y = y+m_skill_y + 13;

		scroll_x = x + 319;
		scroll_y = y + 162;
		scroll_height = 73;
	}

	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		// 2026-05-22 GRADE TAB BltLocked REFERANS PATTERN RESTORE:
		// Tur 3'te yorum içine alınmıştı (m_iGrade frame mapping yanlış hipotezi).
		// Kullanıcı netleştirdi: referans Show() (ClientSRC Referans line 303-309)
		// `m_p_skill_view_grade_spk->BltLocked(grade_x, grade_y, m_iGrade)` çağrısı
		// AKTIF — eski client'ta sorunsuz çalışıyor. Phoenix'in skillwindow.spk
		// eski client'la aynı dosya, m_iGrade frame index'leri doğru çalışmalı.
		// 2026-05-22 GERİ ALINDI: Görev 3 Ousters branch'leri referansla parity için yorum.
		if((g_eRaceInterface == RACE_VAMPIRE))
		{
			if (m_iGrade >= 0 && m_iGrade < 3)
				m_p_skill_view_grade_spk->BltLocked(grade_x, grade_y, m_iGrade);
		}
		else  // RACE_SLAYER
		{
			if (m_iGrade >= 0 && m_iGrade < 5)
				m_p_skill_view_grade_spk->BltLocked(grade_x, grade_y, m_iGrade);
		}

		// SKILL STEP listesinden skill icons render. (Ousters branch referansla parity için kaldırıldı.)
		if(!(g_eRaceInterface == RACE_VAMPIRE) || m_iGrade == 0)
		{
			if(((g_eRaceInterface == RACE_VAMPIRE) && m_iDomain >=0 && m_iDomain < 6 && m_iScroll >= 0 && m_skill_domain == SKILLDOMAIN_VAMPIRE) ||
				/* (g_eRaceInterface == RACE_OUSTERS && m_iDomain >=0 && m_iDomain < 2 && m_iScroll >= 0 && m_skill_domain == SKILLDOMAIN_OUSTERS) || */
				(!(g_eRaceInterface == RACE_VAMPIRE) && m_iDomain >=0 && m_iDomain < 5 && m_iScroll >= 0 && (m_skill_domain == SKILLDOMAIN_SWORD || m_skill_domain == SKILLDOMAIN_BLADE || m_skill_domain == SKILLDOMAIN_GUN || m_skill_domain == SKILLDOMAIN_HEAL || m_skill_domain == SKILLDOMAIN_ENCHANT)))
			{
				SKILL_STEP step = (SKILL_STEP)((g_eRaceInterface == RACE_VAMPIRE) ? m_iDomain + SKILL_STEP_VAMPIRE_POISON : m_iGrade + SKILL_STEP_APPRENTICE);
				/* 2026-05-22 GERİ ALINDI: Ousters step mapping
				if (g_eRaceInterface == RACE_OUSTERS) step = (SKILL_STEP)(m_iDomain + SKILL_STEP_OUSTERS_COMBAT);
				*/
				if((*g_pSkillManager)[m_skill_domain].IsExistSkillStep(step))
				{
					MSkillDomain::SKILL_STEP_LIST list = *((*g_pSkillManager)[m_skill_domain].GetSkillStepList(step));
					
					MSkillDomain::SKILL_STEP_LIST::iterator ss = list.begin()+m_iScroll*3;
					
					for(int skill = 0; skill < min( scroll_size, list.size() - m_iScroll*3); skill++)
					{
						MSkillDomain::SKILLSTATUS status = (*g_pSkillManager)[m_skill_domain].GetSkillStatus((ACTIONINFO)*ss);
						int sprID = (*g_pSkillInfoTable)[(ACTIONINFO)*ss].GetSpriteID();
						//---------------------------------------
						// status�� ������ ����. 
						//---------------------------------------
						//	MSkillDomain::SKILLSTATUS_LEARNED		// �����.
						//	MSkillDomain::SKILLSTATUS_NEXT			// ������ ��� �� �ִ�.
						//	MSkillDomain::SKILLSTATUS_OTHER			// ������ ��� �� ����.	
						//---------------------------------------
						m_p_skill_view_spk->BltLocked(x+m_skill_x + skill%3 * m_skill_x_gap, y+m_skill_y + skill/3*m_skill_y_gap, SKILL_GUARD);
						
						POINT p = {skill_ico_x + skill%3 * m_skill_x_gap, skill_ico_y + skill/3*m_skill_y_gap};
						
						switch(status)
						{
						case MSkillDomain::SKILLSTATUS_LEARNED:
							{
								int level = (*g_pSkillInfoTable)[(ACTIONINFO)*ss].GetExpLevel();
								RECT rect = {p.x, p.y, p.x + 36, p.y+36-36*level/100};
								
								if((*g_pSkillInfoTable)[(ACTIONINFO)*ss].IsEnable() == false)
								{
									if(sprID < C_VS_UI_SKILL::m_C_spk.GetSize() && C_VS_UI_SKILL::m_C_spk[sprID].IsInit())
										gpC_base->m_p_DDSurface_back->BltSpriteEffect(&p, &C_VS_UI_SKILL::m_C_spk[sprID]);
								}
								if((*g_pSkillInfoTable)[(ACTIONINFO)*ss].IsPassive())
								{
									if(sprID < C_VS_UI_SKILL::m_C_spk.GetSize() && C_VS_UI_SKILL::m_C_spk[sprID].IsInit())
										gpC_base->m_p_DDSurface_back->BltSpriteColorSet(&p, &C_VS_UI_SKILL::m_C_spk[sprID], 315);
								}
								else
								{
									POINT p2 = p;
									if(sprID < C_VS_UI_SKILL::m_C_spk.GetSize() && C_VS_UI_SKILL::m_C_spk[sprID].IsInit())
										gpC_base->m_p_DDSurface_back->BltSprite(&p2, &C_VS_UI_SKILL::m_C_spk[sprID]);
									
									if(!(g_eRaceInterface == RACE_VAMPIRE))
									{
										// ���κи� ��� ǥ��
										//									gpC_base->m_p_DDSurface_back->ChangeBrightnessBit(&rect, 1);
										if(rect.bottom > 0 && rect.right > 0 && rect.left < 800 && rect.top < 600)
											DrawAlphaBox(&rect, 0, 0, 0, 15);
										
										// EXP BAR
										int max_exp = (*g_pSkillInfoTable)[(ACTIONINFO)*ss].maxExp;
										int exp = (*g_pSkillInfoTable)[(ACTIONINFO)*ss].GetSkillExp();
										int exp_percent;
										if(max_exp > 0)
											exp_percent = (exp % (max_exp/100))*100/(max_exp/100);
										else exp_percent = 0;

										if(exp == max_exp)exp_percent = 100;
										Rect clip_rect(0, 0, 50*exp_percent/100, 4);
										m_p_skill_view_etc_spk->BltLockedClip(p.x-7, p.y+40, clip_rect, EXP_BAR);
									}
								}
							}
							break;
							
						default:
							int domain_level;
							if((g_eRaceInterface == RACE_VAMPIRE))
								domain_level = (*g_pSkillManager)[SKILLDOMAIN_VAMPIRE].GetDomainLevel();
							else
							{
								if((*g_pSkillInfoTable)[(ACTIONINFO)*ss].DomainType >= 0)
									domain_level = (*g_pSkillManager)[(*g_pSkillInfoTable)[(ACTIONINFO)*ss].DomainType].GetDomainLevel();
								else
									domain_level = 0;
							}
							
							if((*g_pSkillInfoTable)[(ACTIONINFO)*ss].GetLearnLevel() <= domain_level && (*g_pSkillInfoTable)[(ACTIONINFO)*ss].GetLearnLevel() >= 0)
							{
								if(sprID < C_VS_UI_SKILL::m_C_spk.GetSize() && C_VS_UI_SKILL::m_C_spk[sprID].IsInit())
									gpC_base->m_p_DDSurface_back->BltSpriteColor(&p, &C_VS_UI_SKILL::m_C_spk[sprID], rgb_GREEN);
							}
							else
								
							{
								if(sprID < C_VS_UI_SKILL::m_C_spk.GetSize() && C_VS_UI_SKILL::m_C_spk[sprID].IsInit())
									gpC_base->m_p_DDSurface_back->BltSpriteEffect(&p, &C_VS_UI_SKILL::m_C_spk[sprID]);
							}
						}
						
						ss++;
					}
//					if(list.size() > scroll_size)
//						gpC_global_resource->m_pC_assemble_box_button_spk->BltLocked(scroll_x, scroll_y + scroll_height*(m_iScroll)/((list.size()-scroll_size+2)/3), ((g_eRaceInterface == RACE_VAMPIRE)?C_GLOBAL_RESOURCE::AB_VAMPIRE_SCROLL_TAG:C_GLOBAL_RESOURCE::AB_SLAYER_SCROLL_TAG));
				}
			}
		}
		gpC_base->m_p_DDSurface_back->Unlock();
	}

	// 2026-05-22 DEVRE DISI: Phoenix-only skill NAME text render bloğu (kullanıcı onayı,
	// "Referansta nasılsa o şekilde olsun" — Kural 4.1 silme yok, sadece yorum içine alındı).
	//
	// Tarihçe:
	// - 2026-05-15: Phoenix-only patch — skill icon'ları altına SKILL NAME text basılıyordu.
	//   Yorumda gerekçe: "modernize regression / VS6 yokluk telafisi". Referans VS_UI_SKILL_VIEW.cpp
	//   Show() içinde text yok, tek yöntem mouse hover (tooltip).
	// - 2026-05-22: Kullanıcı raporu: K menüsünde pencere overlap + text overlap görseli.
	//   Bu Phoenix-only text render bloğu race-agnostic çalışıp her domain'in skill isimlerini
	//   üst üste basıyor olabilir (sebebi: m_iDomain/m_iGrade değişimleri sırasında temizlik yok).
	//   Kullanıcı kararı: referansla parity restore.
	//
	// Skill name görünürlüğü tooltip yoluyla (mouse hover üzerinde, g_descriptor_manager
	// DID_SKILLTREE) korunuyor — VS_UI_GameCommon.cpp:13426 + diğer call site'ları. Önceki
	// "Skill tooltip yüzeysel guard fix" (project_skill_tooltip_void_ptr_guard_fix_2026_05_22)
	// ile tooltip artık doğru çalışıyor — hover'da skill name + description gözüküyor.
	/*
	{
		int scroll_size_text = ((g_eRaceInterface == RACE_VAMPIRE) ? 9 : 6);
		if(!(g_eRaceInterface == RACE_VAMPIRE) || m_iGrade == 0)
		{
			// 2026-05-22 Görev 3: Ousters branch eklendi.
			if(((g_eRaceInterface == RACE_VAMPIRE) && m_iDomain >= 0 && m_iDomain < 6 && m_iScroll >= 0 && m_skill_domain == SKILLDOMAIN_VAMPIRE) ||
				((g_eRaceInterface == RACE_OUSTERS) && m_iDomain >= 0 && m_iDomain < 2 && m_iScroll >= 0 && m_skill_domain == SKILLDOMAIN_OUSTERS) ||
				(g_eRaceInterface == RACE_SLAYER && m_iDomain >= 0 && m_iDomain < 5 && m_iScroll >= 0 && (m_skill_domain == SKILLDOMAIN_SWORD || m_skill_domain == SKILLDOMAIN_BLADE || m_skill_domain == SKILLDOMAIN_GUN || m_skill_domain == SKILLDOMAIN_HEAL || m_skill_domain == SKILLDOMAIN_ENCHANT)))
			{
				SKILL_STEP step_text;
				if (g_eRaceInterface == RACE_VAMPIRE)      step_text = (SKILL_STEP)(m_iDomain + SKILL_STEP_VAMPIRE_POISON);
				else if (g_eRaceInterface == RACE_OUSTERS) step_text = (SKILL_STEP)(m_iDomain + SKILL_STEP_OUSTERS_COMBAT);
				else                                       step_text = (SKILL_STEP)(m_iGrade + SKILL_STEP_APPRENTICE);
				if((*g_pSkillManager)[m_skill_domain].IsExistSkillStep(step_text))
				{
					MSkillDomain::SKILL_STEP_LIST list_text = *((*g_pSkillManager)[m_skill_domain].GetSkillStepList(step_text));
					MSkillDomain::SKILL_STEP_LIST::iterator ss_text = list_text.begin() + m_iScroll * 3;

					g_FL2_GetDC();
					for(int skill_t = 0; skill_t < min(scroll_size_text, (int)list_text.size() - m_iScroll * 3); skill_t++)
					{
						// VS_UI_Description.cpp:3722-3723 pattern — learn-level cap altinda "???" goster.
						const char *szSkillName;
						char szTemp2[10] = "???";
						if(g_pSystemAvailableManager && g_pSystemAvailableManager->GetLimitLearnSkillLevel() >= (*g_pSkillInfoTable)[(ACTIONINFO)*ss_text].GetLearnLevel())
							// 2026-05-15: GetHName (Hangul) -> GetName (English); CLAUDE.md kural 3
							szSkillName = (*g_pSkillInfoTable)[(ACTIONINFO)*ss_text].GetName();
						else
							szSkillName = szTemp2;

						if(szSkillName == NULL) szSkillName = szTemp2;

						// Icon merkez x: skill_ico_x + skill_t%3 * m_skill_x_gap (icon 36px wide)
						// Icon alti y: skill_ico_y + skill_t/3 * m_skill_y_gap + 38 (36px icon + 2px pad)
						int text_x = skill_ico_x + (skill_t % 3) * m_skill_x_gap - 8;
						int text_y = skill_ico_y + (skill_t / 3) * m_skill_y_gap + 38;

						// VS_UI_Description.cpp:3730-3731 shadow+text pattern.
						g_PrintColorStr(text_x + 1, text_y + 1, szSkillName, gpC_base->m_chatting_pi, RGB(0, 0, 0));
						g_PrintColorStr(text_x,     text_y,     szSkillName, gpC_base->m_chatting_pi, RGB_WHITE);

						ss_text++;
					}
					g_FL2_ReleaseDC();
				}
			}
		}
	}
	*/

	m_pC_button_group->Show();
}

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_VIEW::Run
//
// id�� ACTIONINFO�̴�.
//-----------------------------------------------------------------------------
void C_VS_UI_SKILL_VIEW::Run(id_t id)
{
	switch (id)
	{
	case CLOSE_ID:
//		gpC_base->SendMessage(UI_CLOSE_SKILL_VIEW);
//		gC_vs_ui.HotKey_SkillInfo();
		break;

	case HELP_ID:
		// 2026-05-22 GERİ ALINDI: Ousters HelpDescDialog ternary kaldırıldı.
		// VAMPIRESKILLINFO/SLAYERSKILLINFO referansta vardı ama Phoenix C_VS_UI_DESC_DIALOG::SKILL'e
		// konsolide etmiş — modern enum kullanıldı.
		gC_vs_ui.RunDescDialog(DID_HELP, (void *)C_VS_UI_DESC_DIALOG::SKILL);
		m_pC_button_group->UnacquireMouseFocus();
		break;

	case SCROLL_UP_ID:
		if(m_iScroll > 0) m_iScroll--;
		break;

	case SCROLL_DOWN_ID:
		{
			// 2026-05-22 GERİ ALINDI: Ousters validation + step mapping referansla parity için kaldırıldı.
			if(!(((g_eRaceInterface == RACE_VAMPIRE) && m_iDomain >=0 && m_iDomain <= 6 && m_iScroll >= 0 && m_skill_domain == SKILLDOMAIN_VAMPIRE) ||
				/* ((g_eRaceInterface == RACE_OUSTERS) && m_iDomain >=0 && m_iDomain <= 2 && m_iScroll >= 0 && m_skill_domain == SKILLDOMAIN_OUSTERS) || */
				(!(g_eRaceInterface == RACE_VAMPIRE) && m_iDomain >=0 && m_iDomain <= 5 && m_iScroll >= 0 && (m_skill_domain == SKILLDOMAIN_SWORD || m_skill_domain == SKILLDOMAIN_BLADE || m_skill_domain == SKILLDOMAIN_GUN || m_skill_domain == SKILLDOMAIN_HEAL || m_skill_domain == SKILLDOMAIN_ENCHANT))))
			if((g_eRaceInterface == RACE_VAMPIRE) && m_iGrade != 0)break;

			SKILL_STEP step = (SKILL_STEP)((g_eRaceInterface == RACE_VAMPIRE) ? m_iDomain + SKILL_STEP_VAMPIRE_POISON : m_iGrade + SKILL_STEP_APPRENTICE);
			/* 2026-05-22 GERİ ALINDI: Ousters step mapping
			if (g_eRaceInterface == RACE_OUSTERS) step = (SKILL_STEP)(m_iDomain + SKILL_STEP_OUSTERS_COMBAT);
			*/
			if((*g_pSkillManager)[m_skill_domain].IsExistSkillStep(step))
			{
				MSkillDomain::SKILL_STEP_LIST list = *((*g_pSkillManager)[m_skill_domain].GetSkillStepList(step));
				if(m_iScroll*3 +((g_eRaceInterface == RACE_VAMPIRE)?9:6) < list.size())m_iScroll++;
			}
		}
		break;

	case GRADE_PREV_ID:
		if(m_iGrade > 0)
		{
			m_iGrade--;
			m_iScroll = 0;
		}
		break;
		
	case GRADE_NEXT_ID:
		if((g_eRaceInterface == RACE_VAMPIRE))
		{
			if(m_iGrade < 1)
				m_iGrade++;
		}
		else
		{
			if(m_iGrade < 4)
				m_iGrade++;
		}
		m_iScroll = 0;
		break;

	default:
		// 2026-05-22 GERİ ALINDI: Ousters m_iDomain branch referansla parity için yorum.
		m_iGrade = 0;
		m_iScroll = 0;

		/*
		if (g_eRaceInterface == RACE_OUSTERS)
		{
			m_iDomain = id - OUSTERS_COMBAT_BUTTON_ID;
			m_skill_domain = SKILLDOMAIN_OUSTERS;
		}
		else
		*/
		if((g_eRaceInterface == RACE_VAMPIRE))
		{
			m_iDomain = id - POISON_BUTTON_ID;
			m_skill_domain = SKILLDOMAIN_VAMPIRE;
		}
		else
		{
			m_iDomain = id - SWORD_BUTTON_ID;
			switch(id)
			{
			case SWORD_BUTTON_ID:
				m_skill_domain = SKILLDOMAIN_SWORD;
				break;

			case BLADE_BUTTON_ID:
				m_skill_domain = SKILLDOMAIN_BLADE;
				break;

			case GUN_BOMB_BUTTON_ID:
				m_skill_domain = SKILLDOMAIN_GUN;
				break;

			case HEAL_BUTTON_ID:
				m_skill_domain = SKILLDOMAIN_HEAL;
				break;

			case ENCHANT_BUTTON_ID:
				m_skill_domain = SKILLDOMAIN_ENCHANT;
				break;
			}
		}
		break;
	}
//
//		default:
//			//
//			// ��� �� �ִ� �͸� �����ؼ� Client�� �����ش�.
//			//
//			assert(m_skill_domain != MAX_SKILLDOMAIN);
//
//			// ���ο� skill�� ��� �� �ִ� �������� üũ.. by sigi
//			if ((*g_pSkillManager)[m_skill_domain].HasNewSkill())
//			{
//				MSkillDomain::SKILLSTATUS status = (*g_pSkillManager)[m_skill_domain].GetSkillStatus((ACTIONINFO)id);
//
//				assert(status != MSkillDomain::SKILLSTATUS_NULL);
//
//				if (status == MSkillDomain::SKILLSTATUS_NEXT)
//				{
//					gpC_base->SendMessage(UI_LEARN_SLAYER_SKILL, id);
//				}
//			}
//			break;
//	}
}

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_VIEW::WindowEventReceiver
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SKILL_VIEW::WindowEventReceiver(id_t event)
{

}

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_VIEW::IsPixel
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_SKILL_VIEW::IsPixel(int _x, int _y)
{
	return m_p_skill_view_spk->IsPixel(SCR2WIN_X(_x), SCR2WIN_Y(_y), SKILL_VIEW_BACK);
}

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_VIEW::Process
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SKILL_VIEW::Process()
{
	m_pC_button_group->Process();
}

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_VIEW::ShowButtonWidget
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SKILL_VIEW::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{

	int button_hilight = ((g_eRaceInterface == RACE_VAMPIRE))? 6:5;
	int button_pushed = 0;
	// Phoenix: Eski CHS literal'lar 30-byte sinirini asiyordu (mojibake).
	// Phoenix kural #3: proje ici string'ler Ingilizce.
	static char m_default_string[2][30] =
	{
		"Close SkillInfo window",
		"Open help for SkillInfo",
	};

	static char m_grade_string[2][30] =
	{
		"Previous",
		"Next",
	};

	static char m_skill_string[5][30] =
	{
		"Sword Class",
		"Blade Class",
		"Gun Class",
		"Heal Class",
		"Enchant Class",
	};

	static char m_magic_string[6][30] =
	{
		"Poison Class",
		"Acid Class",
		"Curse Class",
		"Summon Class",
		"Blood Class",
		"Innate Class",
	};

	if (p_button->GetFocusState())
	{
		switch(p_button->GetID())
		{
		case CLOSE_ID:
		case HELP_ID:
			g_descriptor_manager.Set(DID_INFO, x+p_button->x, y+p_button->y, m_default_string[p_button->GetID() - CLOSE_ID]);

		case SCROLL_UP_ID:
		case SCROLL_DOWN_ID:
			if(p_button->GetPressState())
				gpC_global_resource->m_pC_assemble_box_button_spk->Blt(x+p_button->x, y+p_button->y, p_button->m_image_index+C_GLOBAL_RESOURCE::AB_BUTTON_PUSHED_OFFSET);
			break;
			
		case GRADE_PREV_ID:
		case GRADE_NEXT_ID:
			if(p_button->GetPressState())
				m_p_skill_view_etc_spk->Blt(x+p_button->x, y+p_button->y, p_button->m_image_index);
			g_descriptor_manager.Set(DID_INFO, x+p_button->x, y+p_button->y, m_grade_string[p_button->GetID() - GRADE_PREV_ID]);
			break;
			
		default:
			{
				if((g_eRaceInterface == RACE_VAMPIRE))
				{
					g_descriptor_manager.Set(DID_INFO, x+p_button->x, y+p_button->y, m_magic_string[p_button->GetID() - POISON_BUTTON_ID]);
				}
				else
				{
					static char temp_str[5][30];
					static LPSTR str[5] = { temp_str[0], temp_str[1], temp_str[2], temp_str[3], temp_str[4] };
					int domain = p_button->GetID() - SWORD_BUTTON_ID;

					strcpy(temp_str[0], m_skill_string[domain]);
					if(g_pSkillManager)
					{
						int level = (*g_pSkillManager)[temp_domain[domain]].GetDomainLevel();
						// Phoenix: GetDomainExp() yok, GetDomainExpRemain() DWORD donuyor
					int exp = static_cast<int>((*g_pSkillManager)[temp_domain[domain]].GetDomainExpRemain());
//						int prev_exp;
//						if(level == 0)
//						{
//							prev_exp = 0;
//						}
//						else
//						{
//							prev_exp = (*g_pSkillManager)[temp_domain[domain]].GetExpInfo(level).AccumExp;
//						}

						if(level >= 0 && exp >= 0)
						{
							int next_exp = (*g_pSkillManager)[temp_domain[domain]].GetExpInfo(level).AccumExp;
							
							wsprintf(temp_str[1], "Level : %d", level);
							wsprintf(temp_str[2], "Current Experience Points : %d", exp);
							wsprintf(temp_str[3], "Next Level : %d", next_exp);
							wsprintf(temp_str[4], "Remaining Experience Points : %d", next_exp - exp);
//							strcat(temp_str[2], "%");
							
							g_descriptor_manager.Set(DID_STRINGS, x+p_button->x, y+p_button->y, (void *)str, 5);//3);
						}
					}
					
					

				}

				if(p_button->GetPressState())
				{
					m_pC_skill_view_domain_spk->Blt(x+p_button->x, y+p_button->y, p_button->m_image_index+button_pushed);
					if(!(g_eRaceInterface == RACE_VAMPIRE))
					{
						RECT rect = {x+p_button->x, y+p_button->y, x+p_button->x + 69, y+p_button->y+m_domain_level[p_button->m_image_index]};
						if(rect.bottom > 0 && rect.right > 0 && rect.left < 800 && rect.top < 600)
							DrawAlphaBox(&rect, 0, 0, 0, 15);
					}
				}
				else
				{
					if((g_eRaceInterface == RACE_VAMPIRE))
					{
						if(p_button->GetID() == m_iDomain + POISON_BUTTON_ID)
							m_pC_skill_view_domain_spk->Blt(x+p_button->x, y+p_button->y, p_button->m_image_index+button_hilight);
					}
					else
					{
						if(p_button->GetID() == m_iDomain + SWORD_BUTTON_ID)
							m_pC_skill_view_domain_spk->Blt(x+p_button->x, y+p_button->y, p_button->m_image_index+button_hilight);
						RECT rect = {x+p_button->x, y+p_button->y, x+p_button->x + 69, y+p_button->y+m_domain_level[p_button->m_image_index]};
						if(rect.bottom > 0 && rect.right > 0 && rect.left < 800 && rect.top < 600)
							DrawAlphaBox(&rect, 0, 0, 0, 15);
					}
				}
			}
			break;
		}
	}
	else if(p_button->GetID() == m_iDomain + SWORD_BUTTON_ID)
	{
		m_pC_skill_view_domain_spk->Blt(x+p_button->x, y+p_button->y, p_button->m_image_index+button_hilight);
		if(!(g_eRaceInterface == RACE_VAMPIRE))
		{
			RECT rect = {x+p_button->x, y+p_button->y, x+p_button->x + 69, y+p_button->y+m_domain_level[p_button->m_image_index]};
			if(rect.bottom > 0 && rect.right > 0 && rect.left < 800 && rect.top < 600)
				DrawAlphaBox(&rect, 0, 0, 0, 15);
		}
	}
	else if(p_button->GetID() >= POISON_BUTTON_ID && p_button->GetID() <= INNATE_BUTTON_ID)
	{
//		gpC_base->m_p_DDSurface_back->BltSprite(&p, &m_pC_skill_view_domain_spk[p_button->m_image_index]);
		if(!(g_eRaceInterface == RACE_VAMPIRE))
		{
			RECT rect = {x+p_button->x, y+p_button->y, x+p_button->x + 69, y+p_button->y+m_domain_level[p_button->m_image_index]};
			if(rect.bottom > 0 && rect.right > 0 && rect.left < 800 && rect.top < 600)
				DrawAlphaBox(&rect, 0, 0, 0, 15);
//			gpC_base->m_p_DDSurface_back->ChangeBrightnessBit(&rect, 1);
		}
	}
//	else
//		gpC_global_resource->m_pC_assemble_box_button_spk->Blt(p_button->x, p_button->y, p_button->m_image_index);
}

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_VIEW::MouseControl
//
// 
//-----------------------------------------------------------------------------
bool	C_VS_UI_SKILL_VIEW::MouseControl(UINT message, int _x, int _y)
{
	Window::MouseControl(message, _x, _y);
	_x-=x;_y-=y;//â�̵� �����ϰ� �ϱ� ����
	bool re = m_pC_button_group->MouseControl(message, _x, _y);

	switch (message)
	{
	case M_MOVING:
		{
			if((g_eRaceInterface == RACE_VAMPIRE) && m_iGrade != 0)break;
			if(!(((g_eRaceInterface == RACE_VAMPIRE) && m_iDomain >=0 && m_iDomain <= 6 && m_iScroll >= 0 && m_skill_domain == SKILLDOMAIN_VAMPIRE) || 
				(!(g_eRaceInterface == RACE_VAMPIRE) && m_iDomain >=0 && m_iDomain <= 5 && m_iScroll >= 0 && (m_skill_domain == SKILLDOMAIN_SWORD || m_skill_domain == SKILLDOMAIN_BLADE || m_skill_domain == SKILLDOMAIN_GUN || m_skill_domain == SKILLDOMAIN_HEAL || m_skill_domain == SKILLDOMAIN_ENCHANT))))
				break;
			SKILL_STEP step = (SKILL_STEP)((g_eRaceInterface == RACE_VAMPIRE)?m_iDomain+SKILL_STEP_VAMPIRE_POISON:m_iGrade+SKILL_STEP_APPRENTICE);
			
			if(g_pSkillManager && (*g_pSkillManager)[m_skill_domain].IsExistSkillStep(step))
				if(_x >= m_rcSkillDesciption.left && _y >= m_rcSkillDesciption.top && _x < m_rcSkillDesciption.right && _y < m_rcSkillDesciption.bottom)
				{
					int num = (_y - m_skill_y)/m_skill_y_gap*3 + (_x - m_skill_x)/m_skill_x_gap + m_iScroll*3;
					
					if(num >= (*((*g_pSkillManager)[m_skill_domain].GetSkillStepList(step))).size())break;
					
					int id = ( *( (*g_pSkillManager)[m_skill_domain].GetSkillStepList( step ) ) )[num];
					
					g_descriptor_manager.Set(DID_SKILLTREE, x+_x, y+_y, (void *)&m_skill_domain, id, 0);
				}
		}
		break;

	case M_LEFTBUTTON_DOWN:
	case M_LB_DOUBLECLICK:
		{
			if(((g_eRaceInterface == RACE_VAMPIRE) && m_iDomain >=0 && m_iDomain <= 6 && m_iScroll >= 0 && m_skill_domain == SKILLDOMAIN_VAMPIRE) || 
				(!(g_eRaceInterface == RACE_VAMPIRE) && m_iDomain >=0 && m_iDomain <= 5 && m_iScroll >= 0 && (m_skill_domain == SKILLDOMAIN_SWORD || m_skill_domain == SKILLDOMAIN_BLADE || m_skill_domain == SKILLDOMAIN_GUN || m_skill_domain == SKILLDOMAIN_HEAL || m_skill_domain == SKILLDOMAIN_ENCHANT)))
			if(!(g_eRaceInterface == RACE_VAMPIRE) || m_iGrade == 0)
			{
				SKILL_STEP step = (SKILL_STEP)((g_eRaceInterface == RACE_VAMPIRE)?m_iDomain+SKILL_STEP_VAMPIRE_POISON:m_iGrade+SKILL_STEP_APPRENTICE);
				
				if(g_pSkillManager && (*g_pSkillManager)[m_skill_domain].IsExistSkillStep(step))
					if(_x >= m_rcSkillDesciption.left && _y >= m_rcSkillDesciption.top && _x < m_rcSkillDesciption.right && _y < m_rcSkillDesciption.bottom)
					{
						int num = (_y - m_skill_y)/m_skill_y_gap*3 + (_x - m_skill_x)/m_skill_x_gap + m_iScroll*3;
						
						if(num < (*((*g_pSkillManager)[m_skill_domain].GetSkillStepList(step))).size())
						{
							
							int id = ( *( (*g_pSkillManager)[m_skill_domain].GetSkillStepList( step ) ) )[num];
							
							gC_vs_ui.RunDescDialog(DID_SKILL, (void *)id);
//							g_descriptor_manager.Set(DID_SKILLTREE, _x, _y, (void *)&m_skill_domain, id, 0);
							break;
						}
					}
			}
			
			if (gpC_mouse_pointer->GetPickUpItem() == false && re)
			{
				MoveReady();
				SetOrigin(_x, _y);
				break;
			}
				
		}
		break;

	case M_WHEEL_UP:
		Run(SCROLL_UP_ID);
		break;

	case M_WHEEL_DOWN:
		Run(SCROLL_DOWN_ID);
		break;
	}

	return true;
}

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_VIEW::KeyboardControl
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SKILL_VIEW::KeyboardControl(UINT message, UINT key, long extra)
{
}

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_VIEW::Start
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SKILL_VIEW::Start()
{
	// 2026-05-22 Görev 3: Ousters silent-skip KALDIRILDI. Constructor + SPK + button +
	// Show 4 yer 3-branch yapildi (Vampire/Slayer/Ousters). SKILLDOMAIN_OUSTERS=7 +
	// SKILL_STEP_OUSTERS_COMBAT/ELEMENTAL/ETC enum'lari Phoenix'te tanimli (SkillDef.h).
	// SPK_SKILLVIEW_OUSTERS + skilletcousters.spk asset'leri yuklendi (VS_UI_filepath.h:104-106).
	// Memory: project_skillview_ousters_silent_skip.md (Görev 3 ile resolve).
	CT_Done("WnSh: C_VS_UI_SKILL_VIEW::Start ENTER (race=%d)", (int)g_eRaceInterface);

	PI_Processor::Start();
	m_pC_button_group->Init();

	AttrPin(true);

	DeleteNew(m_p_skill_view_grade_spk);
	DeleteNew(m_p_skill_view_etc_spk);
	DeleteNew(m_p_skill_view_spk);
	DeleteNew(m_pC_skill_view_domain_spk);

// SKILL_ICON load
//	ifstream	file(SPK_SKILL_ICON, ios::binary|ios::nocreate);
//	if (!file)
//		_Error(FILE_OPEN);
//	C_VS_UI_SKILL::m_C_spk.LoadFromFile(file);
//	file.close();


	m_p_skill_view_etc_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_ETC);
	// 2026-05-22 GERİ ALINDI: Görev 3 Ousters Start SPK load referansla parity için yorum.
	/*
	if (g_eRaceInterface == RACE_OUSTERS)
	{
		m_p_skill_view_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_OUSTERS);
		m_p_skill_view_grade_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_OUSTERS_GRADE);
		m_pC_skill_view_domain_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_OUSTERS_DOMAIN);
	}
	else
	*/
	if((g_eRaceInterface == RACE_VAMPIRE))
	{
		m_p_skill_view_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_VAMPIRE);
		m_p_skill_view_grade_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_VAMPIRE_GRADE);
		m_pC_skill_view_domain_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_VAMPIRE_DOMAIN);
		// Domain_ICON load
//		ifstream	file(SPK_SKILLVIEW_VAMPIRE_DOMAIN, ios::binary|ios::nocreate);
//		if (!file)
//			_Error(FILE_OPEN);
//		m_pC_skill_view_domain_spk.LoadFromFile(file);
//		file.close();

	}
	else
	{
		m_p_skill_view_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_SLAYER);
		m_p_skill_view_grade_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_SLAYER_GRADE);
		m_pC_skill_view_domain_spk = new C_SPRITE_PACK(SPK_SKILLVIEW_SLAYER_DOMAIN);


		// Domain_ICON load
//		ifstream	file(SPK_SKILLVIEW_SLAYER_DOMAIN, ios::binary|ios::nocreate);
//		if (!file)
//			_Error(FILE_OPEN);
//		m_pC_skill_view_domain_spk.LoadFromFile(file);
//		file.close();
		
	}

	gpC_window_manager->AppearWindow(this);
}

//-----------------------------------------------------------------------------
// C_VS_UI_SKILL_TREE::Finish
//
// 
//-----------------------------------------------------------------------------
void	C_VS_UI_SKILL_VIEW::Finish()
{
	PI_Processor::Finish();

	gpC_window_manager->DisappearWindow(this);

//	C_VS_UI_SKILL::m_C_spk.Release();

	DeleteNew(m_pC_skill_view_domain_spk);
	DeleteNew(m_p_skill_view_grade_spk);
	DeleteNew(m_p_skill_view_etc_spk);
	DeleteNew(m_p_skill_view_spk);
}
