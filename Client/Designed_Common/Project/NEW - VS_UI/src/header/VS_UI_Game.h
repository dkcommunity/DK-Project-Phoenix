/*-----------------------------------------------------------------------------

	VS_UI_Game.h

	Game interface.

	Game Object Components:
									Slayer 
									Vampire 
									Game menu
									NPC
									Exchange

	2000.8.11. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_GAME_H__
#define __VS_UI_GAME_H__

#include "VS_UI_GameSlayer.h"
#include "VS_UI_GameVampire.h"
#include "VS_UI_GameOusters.h"
#include "VS_UI_Widget.h"

#include "VS_UI_Shop.h"
#include "VS_UI_Storage.h"
#include "VS_UI_Exchange.h"
#include "VS_UI_Tutorial.h" //by larosel

#include "VS_UI_SKILL_VIEW.h"
#include "VS_UI_ELEVATOR.h"
#include "VS_UI_PetStorage.h"
#include "VS_UI_WebBrowser.h"
#include "VS_UI_TaxRate.H"
#include "VS_UI_GPS.h"

#include <map>

// 2026-05-12: Forward declare (concrete include VS_UI_SKILL_VIEW.h yukarda
// zaten dahil, ama header guard race / TU compile order farkliliklarinda
// pointer field declaration tipini bulamayabiliyor).
class C_VS_UI_SKILL_VIEW;

#if __CONTENTS(__TIPOJYU_CASTLE)	//�δ� ���̾�α� ������� ����
#include "VS_UI_INDOUN_NPC_DIALOG.H"
#endif // __TIPOJYU_CASTLE;

#if __CONTENTS(__TITLE_UI_RENWEAL)
	#include "VS_UI_NewTitle.h"
#else	//__TITLE_UI_RENWEAL
	#include "VS_UI_Title.h"
#endif //__TITLE_UI_RENWEAL
#include "VS_UI_PetMixingForge.h"

/*
//-----------------------------------------------------------------------------
// class C_VS_UI_GAMEMENU_OPTION
//
// GameMenu���� ������ option 
//
// by sigi
//
//-----------------------------------------------------------------------------
class C_VS_UI_GAMEMENU_OPTION : public C_VS_UI_DIALOG	// by sigi
{
public :
	enum MENU
	{
//		DRAW_MINIMAP,
//		DRAW_ZONE_NAME,
//		DRAW_GAME_TIME,
//		DRAW_BLENDING_CHARACTER_SHADOW,
		PLAY_SOUND,
		PLAY_MUSIC,
		VOLUME_SOUND,
		VOLUME_MUSIC,
		PLAY_WAV,
		PLAY_MIDI,
		FILTERING_CURSE,
		BLOOD_DROP,
		PARTY_HPBAR_ALPHA,
		IFEEL,
		USE_GAMMA,
		GAMMA_VALUE,
		CHAT_BOX,
		MENU_COUNT,

		INVALID_ID
	};

	enum GAMEMENU_SPK_INDEX
	{
		SPK_NORMAL_BOX,
		SPK_FOCUSED_BOX,
		SPK_CHECKED_BOX,
		SPK_FOCUSED_CHECKED_BOX,
		SPK_MINUS,
		SPK_PLUS,
		SPK_SCROLL_BAR,
		SPK_SCROLL_NORMAL_SELECT,
		SPK_SCROLL_FOCUSED_SELECT
	};

private:
	C_SPRITE_PACK	*			m_pC_option_spk;

	ButtonGroup *				m_pC_button_group;


	C_VS_UI_EVENT_BUTTON*		m_pC_button[MENU_COUNT];	// �׳� ������. delete�ϸ� �ȵȴ�.

	static GAMEMENU_SPK_INDEX			m_sprite_id[MENU_COUNT][4];

	BOOL			m_Checked[MENU_COUNT];
	
	int				m_SoundVolume;
	int				m_MusicVolume;
	int				m_GammaValue;

	Rect			m_rectSoundVolume;
	Rect			m_rectMusicVolume;
	bool			m_bSoundVolumePushed;
	bool			m_bSoundVolumeFocused;
	bool			m_bMusicVolumePushed;
	bool			m_bMusicVolumeFocused;

	Rect			m_rectGammaValue;
	bool			m_bGammaValuePushed;
	bool			m_bGammaValueFocused;
	
private :
	void			SetMusicVolume(int _x);
	void			SetSoundVolume(int _x);
	void			SetGammaValue(int _x);

public:
	C_VS_UI_GAMEMENU_OPTION();
	~C_VS_UI_GAMEMENU_OPTION();

	void	Start();
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	
	void	Run(id_t id);
	void	Show();
	void	Process();
	void	Finish();
	
	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);

	bool	IsRunningMinimap();	// by larosel

};
*/

class C_VS_UI_ARROW_TILE;
class C_VS_UI_CRAZY_MINE;
class C_VS_UI_QUEST_INVENTORY;
class C_VS_UI_STATUS_CTF;
class C_VS_UI_REGEN_TOWER_MINIMAP;
class C_VS_UI_MAILBOX;
class C_VS_UI_PET_INFO;
class C_VS_UI_HELPDESC;
class C_VS_UI_SMS_MESSAGE;
class C_VS_UI_SMS_LIST;
class C_VS_UI_SMS_RECORD;
class C_VS_UI_NAMING;
class C_VS_UI_NicknameInfo;
class C_VS_UI_NAMING_CHANGE;
class C_VS_UI_QUEST_MANAGER;
class C_VS_UI_RANGER_CHAT;
class C_VS_UI_PERSNALSHOP_MESSAGE;
class C_VS_UI_POWER_JJANG;
class C_VS_UI_QUEST_NPC_DIALOG;
class C_VS_UI_WEBBROWSER;
class C_VS_UI_INVENTORY_SUB;
class C_VS_UI_WORLD_MAP;
class C_VS_UI_OPERATOR;
class C_VS_UI_TIME_COUNT;
class C_VS_UI_USER_GRADE;


//-----------------------------------------------------------------------------
// C_VS_UI_GAMEMENU
//
//
//-----------------------------------------------------------------------------
class C_VS_UI_GAMEMENU : public Window, public Exec, public ButtonVisual
{
private:
//	enum MENU
//	{
////		TO_BOARD,
//		OPTION = 0,
//		LOGOUT,
//#if __CONTENTS(__080405_FIREST_UI_UPDATE)
//		QUITEXIT,
//#endif //__080405_FIREST_UI_UPDATE
//		CONTINUE,
//		MENU_COUNT,
//
//		INVALID_ID
//	};
//
//	enum GAMEMENU_SPK_INDEX
//	{
//		GAMEMENU_WINDOW,
////		TO_BOARD_IMAGE,
//		OPTION_IMAGE,
//		LOGOUT_IMAGE,
//#if __CONTENTS(__080405_FIREST_UI_UPDATE)
//		QUITEXIT_IMAGE,
//#endif //__080405_FIREST_UI_UPDATE
//		CANCEL_IMAGE,
//	};
	enum MENU
	{
	   Button_GEAR,
	   Button_SKILL,
	   Button_QUEST,
	   Button_PET,
	   Button_GUILD,
	   Button_GUILD_LIST,
	   Button_WATING_GUILD,
	   Button_GUILD_MEMBER,
	   Button_FRIEND,
	   Button_PARTY,
	   Button_TMALL,
	   Button_SHOP,
	   Button_GOODLIST,
	   Button_HELP,
	   Button_OPTION,
	   Button_LOGOUT,
	   Button_CLOSEGAME,
	   Button_CLOSE,
	};
	enum GAMEMENU_SPK_INDEX
	{
	  IMAGE_MAIN,
	  BUTTON_1,
	  BUTTON_2,
	  BUTTON_3,
	  IMAGE_INFO,
	  IMAGE_SKILL,
	  IMAGE_QUEST,
	  IMAGE_PET,
	  IMAGE_GUILD,
	  IMAGE_GUILD_LIST,
	  IMAGE_WATING_GUILD,
	  IMAGE_GUILD_MEMBER,
	  IMAGE_FRIEND,
	  IMAGE_PARTY,
	  IMAGE_MAIL,
	  IMAGE_TMALL,
	  IMAGE_SHOP,
	  IMAGE_GOODLIST,
	  IMAGE_HELP,
	  IMAGE_OPTION,
	  IMAGE_LOGOUT,
	  IMAGE_CLOSEGAME,
	  IMAGE_CLOSE,
	  BUTTON_CLOSE_1,
	  BUTTON_CLOSE_2,
	  BUTTON_CLOSE_3,
	};
	C_SPRITE_PACK	*			m_pC_gamemenu_spk;

	ButtonGroup *				m_pC_button_group;
//	bool						m_bHangul;

public:
	C_VS_UI_GAMEMENU();
	~C_VS_UI_GAMEMENU();

	void	UnacquireMouseFocus() { m_pC_button_group->UnacquireMouseFocus(); }
	void	CancelPushState(){m_pC_button_group->CancelPushState();}
	//void	AcquireFirstSequence() { gpC_base->SendMessage(UI_REMOVE_BACKGROUND_MOUSE_FOCUS); }
	void	AcquireDisappear();
	void	Run(id_t id);
	bool	MouseControl(UINT message, int _x, int _y);
	void	KeyboardControl(UINT message, UINT key, long extra);
	void	Show();
	void	Process();
	void	Start();
	void	Finish();
	bool	IsPixel(int _x, int _y);
	void	WindowEventReceiver(id_t event);

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON * p_button);
};

//-----------------------------------------------------------------------------
// C_VS_UI_GAME
//
// 
//-----------------------------------------------------------------------------
class C_VS_UI_GAME : public PI_Processor, public HotKey
{
private:
	typedef std::map<DWORD, C_VS_UI_INVENTORY_SUB*> SUBINVENTORY_MAP;

	HotKey *					m_pC_hotkey;

	// common interface object
	C_VS_UI_CHATTING			*m_pC_chatting;
	C_VS_UI_SKILL				m_C_skill;

	C_VS_UI_SHOP *				m_pC_shop;
	C_VS_UI_STORAGE *			m_pC_storage;
	C_VS_UI_EXCHANGE *			m_pC_exchange;
	// 2026-05-12: SkillInfoWindow aktivasyon. Class implementation
	// VS_UI_SKILL_VIEW.cpp'de 901 satir mevcut. Member burada yorumdaydi,
	// ondan dolayi K hotkey -> tribe_interface->HotKey_SkillInfo zinciri
	// hicbir window yaratmiyordu.
	C_VS_UI_SKILL_VIEW *		m_pC_skill_view;

	C_VS_UI_INFO *						m_pC_info;
	C_VS_UI_OTHER_INFO*			m_pC_other_info;
	C_VS_UI_DESC_DIALOG *		m_pC_desc_dialog;
	//C_VS_UI_FILE_DIALOG *			m_pC_file_dialog;
	C_VS_UI_ELEVATOR *				m_pC_elevator;

	C_VS_UI_ASK_DIALOG*			m_pC_storage_buy_dialog;		// by sigi
	C_VS_UI_ASK_DIALOG*			m_pC_exchange_ask_dialog;		// by sigi
	C_VS_UI_ASK_DIALOG*			m_pC_exchange_cancel_dialog;	// by sigi
	C_VS_UI_ASK_DIALOG*			m_pC_tutorial_exit_ask_dialog;	// by larosel
	C_VS_UI_ASK_DIALOG*			m_pC_party_ask_dialog;		// by sigi
	C_VS_UI_ASK_DIALOG*			m_pC_party_cancel_dialog;	// by sigi
	C_VS_UI_ASK_DIALOG*			m_pC_enchant_dialog;	// by sigi
	C_VS_UI_ASK_DIALOG*			m_pC_thirdenchant_dialog;
	C_VS_UI_ASK_DIALOG*			m_pC_no_search_result_dialog;	// by sigi
	C_VS_UI_ASK_DIALOG*			m_pC_deposit_limit;	// by sigi
	C_VS_UI_ASK_DIALOG*			m_pC_withdraw_limit;	// by sigi
	C_VS_UI_ASK_DIALOG*			m_pC_bring_fee_limit;
	C_VS_UI_ASK_DIALOG*			m_pC_trans_item_dialog;
	C_VS_UI_ASK_DIALOG*			m_pC_use_pet_food_dialog;
	C_VS_UI_ASK_DIALOG*			m_pC_keep_petitem_dialog;
	C_VS_UI_ASK_DIALOG*			m_pC_get_keep_petitem_dialog;
	C_VS_UI_ASK_DIALOG*			m_pC_use_askitem_dialog;
	C_VS_UI_ASK_DIALOG*			m_pC_get_event_item;

#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
	C_VS_UI_ASK_DIALOG*			m_pC_RepairItemAllDialog;
#endif //__REPAIRSYSTEM_RENEWAL


	C_VS_UI_ASK_DIALOG*			m_pC_Delegation_Of_Power;

	C_VS_UI_BRING_FEE*			m_pC_bring_fee;						// by sonee

	// vampire/slayer interface
	C_VS_UI_TRIBE *				m_pC_tribe_interface;
	C_VS_UI_GAMEMENU *			m_pC_gamemenu;
	void						ReleaseFinishedGameMenu();

	// Tutorial by larosel
	C_VS_UI_BOOKCASE *			m_pC_bookcase;
//	C_VS_UI_BRIEFING *			m_pC_briefing;
	C_VS_UI_COMPUTER *			m_pC_computer;
//	C_VS_UI_TUTORIAL_EXIT *		m_pC_tutorial_exit;

	//TEAM
	C_VS_UI_TEAM_MEMBER_LIST *		m_pC_team_member_list;
	C_VS_UI_TEAM_LIST *				m_pC_team_list;
	C_VS_UI_TEAM_INFO *				m_pC_team_info;
	C_VS_UI_TEAM_MEMBER_INFO *		m_pC_team_member_info;
	C_VS_UI_TEAM_REGIST *			m_pC_team_regist;

	//SLAYER_PORTAL
	C_VS_UI_MAILBOX *				m_pC_mailbox;
	C_VS_UI_HORN *					m_pC_horn;
	C_VS_UI_SLAYER_PORTAL *			m_pC_slayer_portal;
	C_VS_UI_REQUEST_DIE *			m_pC_request_die;

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
	// PremiumGiveItem
	C_VS_UI_REQUEST_PREMIUM_GIVE_ITEM *	m_pC_request_premium_give_item;
#endif //__PREMIUM_GIVE_ITEM_UI

	// party
	C_VS_UI_REQUEST_PARTY *			m_pC_request_party;
	C_VS_UI_PARTY_MANAGER *			m_pC_party_manager;

	C_VS_UI_TRACE *					m_pC_trace;
	C_VS_UI_XMAS_CARD *				m_pC_xmas_card;
	C_VS_UI_BULLETIN_BOARD *		m_pC_bulletin_board;
	C_VS_UI_WAR_LIST	*			m_pC_war_list;
	C_VS_UI_BLOOD_BIBLE_STATUS*		m_pC_blood_bible_status;
	C_VS_UI_INPUT_NAME*				m_pC_input_name;
	C_VS_UI_POPUP_MESSAGE*			m_pC_popup_message;
	C_VS_UI_LOTTERY_CARD*			m_pC_lottery_card;
	C_VS_UI_ITEM_LIST*				m_pC_item_list;
	C_VS_UI_IMAGE_NOTICE*			m_pC_image_notice;
	C_VS_UI_REQUEST_RESURRECT *		m_pC_request_resurrect;
	C_VS_UI_MIXING_FORGE *			m_pC_mixing_forge;
	C_VS_UI_REMOVE_OPTION *			m_pC_remove_option;
	C_VS_UI_OUSTERS_SKILL_INFO *	m_pC_ousters_skill_info;
	C_VS_UI_FINDING_MINE *			m_pC_finding_mine;
	C_VS_UI_ARROW_TILE*				m_pC_arrow_tile;
	C_VS_UI_CRAZY_MINE*				m_pC_crazy_mine;
	C_VS_UI_QUEST_INVENTORY*		m_pC_quest_inventory;
	C_VS_UI_STATUS_CTF*				m_pC_status_ctf;
	C_VS_UI_REGEN_TOWER_MINIMAP*	m_pC_regen_tower_minimap;
	C_VS_UI_PET_INFO*				m_pC_pet_info;
	C_VS_UI_HELPDESC*				m_pC_help_desc;
#if __CONTENTS(__PET_MIXINGFORGE)	
	C_VS_UI_PET_MIXING_FORGE*		m_pC_pet_mixing_forge;
#endif //__PET_MIXINGFORGE
	// 2006.01.15 chyaya Add Start - World Map Interface
	C_VS_UI_WORLD_MAP*				m_pC_world_map;
	// 2006.01.15 chyaya Add End - World Map Interface
	
	// 2004, 5, 11 , sobeit add start - pet storage
	C_VS_UI_PETSTORAGE *			m_pC_petstorage;
	// 2004, 5, 11 , sobeit add end

	// 2004, 5, 27 sobeit add start - sms message
	C_VS_UI_SMS_MESSAGE*			m_pC_sms_message;
	C_VS_UI_SMS_LIST*				m_pC_sms_list;
	C_VS_UI_SMS_RECORD*				m_pC_sms_record;
	// 2004, 5, 27 sobeit add end

	// 2004, 6, 14 sobeit add start - naming window
	C_VS_UI_NAMING*					m_pC_naming;
	C_VS_UI_NAMING_CHANGE*			m_pC_naming_change;
	std::vector<C_VS_UI_NicknameInfo*>	m_NickNameList;
//	bool							m_ChangeableNickname;
//	bool							m_ChangeablePetNickname;
	// 2004, 6, 14 sobeit add end

	// 2004, 7, 9 sobeit add start
	C_VS_UI_QUEST_MANAGER		*m_pC_quest_manager;
	// 2004, 7, 9 sobeit add end

	// 2004, 10, 25, sobeit add start
	C_VS_UI_RANGER_CHAT				*m_pC_Ranger_Chat;
	// 2004, 10, 25, sobeit add end

	// 2004, 10, 25, sobeit add start
	C_VS_UI_POWER_JJANG				*m_pC_Powerjjang;
	// 2004, 10, 25, sobeit add end
	void	RunGameMenu();

	// 2004, 11, 18 thai00
	C_VS_UI_PERSNALSHOP				*m_pC_persnalshop;
	C_VS_UI_PERSNALSHOP_MESSAGE     *m_pC_persnalshopmessage;

//	bool	m_bl_set_save;
	C_VS_UI_QUEST_NPC_DIALOG		*m_pC_Quest_Npc_Dialog;

	// 2005, 2, 1, sobeit add start
	C_VS_UI_WEBBROWSER				*m_pC_WebBrowser;
	// 2005, 2, 1, sobeit add end

//	C_VS_UI_INVENTORY_SUB			*m_pC_SubInventory;
	SUBINVENTORY_MAP				m_SubInventoryMap;

	VS_UI_TAXRATE					*m_pC_TexRate	;

	// 2006.01.15 chyaya Add Start - Operator Only Interface
	C_VS_UI_OPERATOR*				m_pC_operator;
	// 2006.01.15 chyaya Add End - Operator Only Interface

	//20070529 by diesirace
	C_VS_UI_TIME_COUNT*				m_pC_timer;
#if __CONTENTS(__USER_GRADE)
	C_VS_UI_USER_GRADE*				m_pC_usergrade;
#endif	//__USER_GRADE
	
#if __CONTENTS(__TIPOJYU_CASTLE)	//�δ� ���̾�α� Ŭ���� ����
	C_VS_UI_INDOUN_DIALOG*			m_pC_Indoung_Dialog;
#endif // __TIPOJYU_CASTLE;


public:
	C_VS_UI_FILE_DIALOG *			m_pC_file_dialog;

	int C_VS_UI_GAME::GetQuikSlot(C_VS_UI_OUSTERS_QUICKITEM::HOTKEY hotkey) const;
	int C_VS_UI_GAME::GetQuikSlot(C_VS_UI_SLAYER_QUICKITEM::HOTKEY hotkey) const;

	C_VS_UI_GAME();
	~C_VS_UI_GAME();

	struct PETINFO* GetPetInfo();
	struct PETINFO* GetMyPetInfo();

	void	CloseOtherInfo();
	bool	IsNewMail();
	void	RequestDieTimerReset()			{ if(m_pC_request_die != NULL)m_pC_request_die->Timer(true); }
	void	SetBloodBibleStatusTimer(DWORD sec)	{ if(m_pC_blood_bible_status != NULL)m_pC_blood_bible_status->SetTimer(sec); }
	void	RefreshInfoImage()			{ if(m_pC_info != NULL)m_pC_info->RefreshImage(); if(m_pC_party_manager)m_pC_party_manager->RefreshFaceImage(); if(m_pC_other_info != NULL)m_pC_other_info->RefreshImage(); }
	bool	IsRunningMainHelp()			{ if(m_pC_desc_dialog != NULL)m_pC_desc_dialog->IsMainHelp(); return false; }
	bool	ReturnTeamRegistMember()	{ if(m_pC_team_regist!=NULL) return m_pC_team_regist->ReturnTeamRegistMember(); return false; }
	
	void	MouseControlExtra(UINT message, int _x, int _y) { if(m_pC_tribe_interface != NULL) m_pC_tribe_interface->MouseControlExtra(message, _x, _y); }

	// 2026-09-03 plan F3 — canli cozunurluk degisiminde HUD relayout
	void	RelayoutHudForResolution(int oldW, int oldH)
	{ if(m_pC_tribe_interface != NULL) m_pC_tribe_interface->RelayoutForResolution(oldW, oldH); }


	bool	IsInputMode()				{ if(m_pC_chatting == NULL) return false; return m_pC_chatting->IsInputMode();	}
	// ������ ���� ���̺� �ұ�?
//	void	SetSaveSet(bool bl_save)	{ m_bl_set_save = bl_save; }

	// ���ڼ�ġ
	void	StartInstallMineProgress(int focus_grid_x, int focus_grid_y)
	{ if(m_pC_tribe_interface)m_pC_tribe_interface->StartInstallMineProgress(focus_grid_x, focus_grid_y); }
	bool	IsInstallMineProgress()		{ return gbl_mine_progress; }
	void	EndInstallMineProgress()	{ gbl_mine_progress = false; }
	// ���ڸ����
	void	StartCreateMineProgress(int focus_grid_x, int focus_grid_y)
	{ if(m_pC_tribe_interface)m_pC_tribe_interface->StartCreateMineProgress(focus_grid_x, focus_grid_y); }
	bool	IsCreateMineProgress()		{ return gbl_mine_progress; }
	void	EndCreateMineProgress()	{ gbl_mine_progress = false; }
	//��ź�����
	void	StartCreateBombProgress(int focus_grid_x, int focus_grid_y)
	{ if(m_pC_tribe_interface)m_pC_tribe_interface->StartCreateBombProgress(focus_grid_x, focus_grid_y); }
	bool	IsCreateBombProgress()		{ return gbl_mine_progress; }
	void	EndCreateBombProgress()	{ gbl_mine_progress = false; }
	
	//���� Progress Bar
	void	StartSkillCastingProgress(int skillID, DWORD time, CPoint inventoryGrid = CPoint(-1, -1))
	{ if(m_pC_tribe_interface) m_pC_tribe_interface->StartSkillCastingProgress(skillID, time, inventoryGrid); }
	bool	IsSkillCastingProgress()
	{ return m_pC_tribe_interface && m_pC_tribe_interface->IsSkillCastingProgress();	}
	void	EndSkillCastingProgress()
	{ if(m_pC_tribe_interface) m_pC_tribe_interface->EndSkillCastingProgress();			}

	const char *GetFileName()	{ if(m_pC_file_dialog)return m_pC_file_dialog->GetFileName(); return NULL; }
	void	AddReadyTeamList(const C_VS_UI_TEAM_LIST::READY_TEAM_LIST &team_info)
	{
		if(m_pC_team_list)
			m_pC_team_list->AddReadyTeamList(team_info);
	}
	void	AddRegistTeamList(const C_VS_UI_TEAM_LIST::REGIST_TEAM_LIST &team_info)
	{
		if(m_pC_team_list)
			m_pC_team_list->AddRegistTeamList(team_info);
	}
	void	AddTeamMemberList(const C_VS_UI_TEAM_MEMBER_LIST::TEAM_MEMBER_LIST &member_list , BYTE bType)
	{
		if(m_pC_team_member_list)
			m_pC_team_member_list->AddMemberList(member_list, bType);
	}
	char*	GetTeamMember_SelectedID(){
		if(m_pC_team_member_list)
			return m_pC_team_member_list->GetSelectedID();
		else
			return NULL;
	}
	
	int		GetPartyManagerFocused()		{ if(m_pC_party_manager)return m_pC_party_manager->GetPartyManagerFocused(); return -1; }

	void	SetWhisperID(char *id)	{ if(m_pC_tribe_interface)m_pC_tribe_interface->SetWhisperID(id); }
	void	SetOtherInfo(C_VS_UI_OTHER_INFO::PLAYER_INFO &info) { if(m_pC_other_info) m_pC_other_info->SetOtherInfo(info);}

	bool	ClosePopupWindow();

	bool	IsRunningStorage() const;
	bool	IsRunningPetStorage() const;
	bool	IsRunningExchange() const;
	bool	IsRunningShop() const;
	bool	IsRunningRepair() const;
	bool	IsRunningSilvering() const;
	bool	IsRunningSell() const;
	bool	IsRunningGameMenu(){ return (m_pC_gamemenu != NULL && m_pC_gamemenu->Running()); }	// by larosel
	bool	IsRunningDescDialog(){ return (m_pC_desc_dialog != NULL); }	// by larosel
	bool	IsRunningFileDialog(){ return (m_pC_file_dialog != NULL); }	// by larosel
	bool	IsRunningElevator() const;

	void	RunQuickItemSlot();
	void	CloseQuickItemSlot();
	bool	IsRunningQuickItemSlot();
	void	RunMinimap();
	void	CloseMinimap();
	bool	IsRunningMinimap();

	void	RestoreWhenActivateGame();
	void	SetHotkey(C_VS_UI_SKILL::HOTKEY hotkey, ACTIONINFO id);
	int	GetHotkey(C_VS_UI_SKILL::HOTKEY hotkey, C_VS_UI_SKILL::HOTKEY_GRADE grade) const;
	int	GetHotkeyType(C_VS_UI_SKILL::HOTKEY hotkey) const;
	void	FinishLevelUp();
	void	LevelUp();
	bool	IsRunningLevelUp();
	bool	IsRunningCharInfo();
	bool	IsRunningSkillInfo();
	bool	IsRunningGrade3Info();
	bool	IsRunningGrade2Info();
	bool	IsRunningGrade1Info();
	bool	IsRunningRareSkillInfo();
	bool	IsRunningOtherInfo();
	void	CloseInfo();
//	void	RunInfo(bool skill_mode);
	void	RunInfo(C_VS_UI_INFO::INFO_MODE Mode);
	void	RunOtherInfo(bool isMonster = false);
	void	SetPetInfo(struct PETINFO *pPetInfo)		{ if(m_pC_info)m_pC_info->SetPetInfo(pPetInfo); }
	void	SetOtherPetInfo(struct PETINFO *pPetInfo)		{ if(m_pC_other_info)m_pC_other_info->SetPetInfo(pPetInfo); }
	C_VS_UI_INFO * GetInfoWindow() { return m_pC_info; }
	void	CloseDescDialog();
	void	RunDescDialog(int type, void *ptr , void* ptr2 = NULL);
	void	CloseFileDialog();
	void	RunFileDialog(C_VS_UI_FILE_DIALOG::MODE Mode, char *type);
	void	CloseElevator();
	void	RunElevator();
#if __CONTENTS(__REPAIRSYSTEM_RENEWAL)
	void	RunRepairItemAll(DWORD objectID, DWORD price);
#endif //__REPAIRSYSTEM_RENEWAL

	void	AcquireChatting();
	bool	IsEmptyChatting();
	LineEditorVisual	GetChatLev()				{ return m_pC_chatting->GetChatLev(); }
	int		GetChatMode()							{ return m_pC_chatting->GetChatMode();}
	int		SetChatMode(int mode)					{ return m_pC_chatting->SetChatMode(mode);}
//	void	DisconnectPCS(int slot);
//	void	ConnectionFailed();
//	void	GrantPCSWaitRoomToDuplex(int slot);
//	int	GetSendPossibleSlot() const;
	void	ChangeWhisperFocus(){ m_pC_chatting->ChangeWhisperFocus(); }	// by larosel
	bool	SetSlayerWhisper( bool b ){return m_pC_chatting->SlayerWhisperMode(b);}	// by larosel
	bool	GetWhisperFocus(){ return m_pC_chatting->GetWhisperFocus(); }	// by larosel
	void	SetGuildChat(bool guild) { m_pC_chatting->SetGuildChat(guild); }

	void	CloseGameMenu();
	Point	GetInventoryPosition() const;
	Point	GetInventoryGridPosition(int grid_x, int grid_y) const;
	Point	GetInventoryGetWidthAndHeight();
	bool	ReplaceItemInGear(MItem* pItem, int slot, MItem*& pOldItem);
	MItem * RemoveItemInGear(int slot);
	bool	CanReplaceItemInGear(MItem* pItem, int slot, MItem*& pOldItem);
	bool	CanReplaceItemInMilitarySlot(MItem* pItem, int slot, MItem*& pOldItem);
	const MItem * GetGearItem(int slot) const;
	const MItem * GetGearOtherItem(int slot) const;
	const MItem * GetMilitarySlot(int slot) const;
	const MItem * GetGearItem_PickUp(int &slot) const;
	const MItem * GetGearCoreZapItem(int slot) const;
	const MItem * GetGearOtherCoreZapItem(int slot) const; //Add by hays
	const MItem * GetGearCoreZapedItem(int slot) const;
	const bool IsHasAllCoreZap(int CoreZapType) const;
	const bool IsHasAllCue(OUT bool CueOption[]) const;
	const bool IsCloseBloodBibleSlot(int slot) const;
	const int GetGearSize() const;
	const char* GetCurrentOtherInfoName(){if(m_pC_other_info) return m_pC_other_info->GetCurrentOtherInfoName(); return NULL;}

//	void	AcquireQuitPCSOnlineModeMessage();
//	void	PleaseConnectPCS(char * sz_name, long pcs_number, int slot);
//	void	OnLinePCS(char * sz_name, long pcs_number);
//	void	AcquirePDSClosedMessage();

	void	ToggleESC4UI();
	void	ResetSlayerQuickItemSize();
	void	ResetOustersQuickItemSize();
	
	void	Start();
	//void	Show();
	void	Process();
	//bool	MouseControl(UINT message, int _x, int _y);
	//void	KeyboardControl(UINT message, UINT key, long extra);

	int		GetZoneID();
	void	SetZone(int zone_id);
	void	SetBlock(int x, int y );
	void	SetTime(const char *str);
	void	SetDate(const char *str);
	void	SetXY(int x, int y);
	void	SetSize(SIZE size);
	int		GetX()						{ return m_pC_tribe_interface->GetX(); }
	int		GetY()						{ return m_pC_tribe_interface->GetY(); }
	void	SetSafetyZone(RECT rect, bool my_zone);
	void	SetNPC(MINIMAP_NPC npc);
	void	ClearNPC();
	void	SetPortal(RECT rect, int id);

	void	ChangeToSlayerInterface();
	void	ChangeToVampireInterface();
	void	AddChatToHistory(char * str, char * sz_id, CHAT_LINE_CONDITION condition, DWORD color);

	void	LockItemTrade();
	void	UnlockItemTrade();
	void	LockItem();
	void	UnlockItem();

	void	LockGear();
	void	UnlockGear();
	bool	GetInventoryOpenState() const;
	bool	GetGearOpenState() const;

	// skill tree interface
//	void	RunVampireSkillTree();
//	void	RunSwordSkillTree();
//	void	RunBladeSkillTree();
//	void	RunEnchantSkillTree();
//	void	RunHealSkillTree();
//	void	RunGunSkillTree();
//	void	CloseSkillTree();
	// 2026-05-12: SkillInfoWindow API — body'ler VS_UI_Game.cpp'de uncomment edildi.
	void	RunSkillView();
	void	CloseSkillView();
	bool	IsRunningSkillView() const;
	C_VS_UI_SKILL_VIEW * GetSkillViewWindow() { return m_pC_skill_view; }

	void	SetShop(MShop* pShop);
	void	SetStorage(MStorage* pStorage);
	void	SetPetStorage(MStorage* pStorage);
	void	RunShop();
	void	RunStorageBuy(int price);	
	void	RunExchangeAsk(const char* pName);	
	void	RunExchangeCancel(const char* pName);	
	void	RunTutorialExitAsk(const int select, const char* pName);	//by larosel
	void	RunStorage();
	void	RunExchange();
	void	RunExchangeCancel();
	void	CloseShop();
	void	CloseStorage();
	void	ClosePetStorage();
	void	CloseExchange();
	void	CloseExchangeAsk();
	void	CloseExchangeCancel();

	//void	OpenGameHook();
	//void	CloseGameHook();

	void    ToggleMilitary();
	void	OpenMilitary();
	void	CloseMilitary();
	void	InvalidateMilitarySkillLists();
	bool	IsRunningMilitary() const;
	void	DownSkillOuster();
	void    UpdateMilitarySlotData(int nSlotID, int nType, int nGrade);

	void    ToggleTMall();
	void	OpenTMall();
	void	CloseTMall();
	bool	AddItemToTMall(C_VS_UI_TMALL::ItemList pItem);

	bool	IsRunningPayEvent();
	void	OpenPayEvent();
	void	ClosePayEvent();
	bool	AddItemToPayEvent(C_VS_UI_PAYEVENT::ItemData pItem);
	void    SetPayEventCount(int nLevel, int nCount);
	void	SetPayEventStautus(int nLevel, int nStautus);

	void    OpenHollyWar();
	void    CloseHollyWar();
	void    AddHollyScore(C_VS_UI_HOLYWAR::PlayerScore score, int nRace);
	void    AddHollyRace(C_VS_UI_HOLYWAR::RaceData * data);
	void    AddPlayerData(C_VS_UI_HOLYWAR::PlayerData data);

	void	OpenInventoryToSell();
	void	FinishItemSelling();
	void	FinishItemRepairing();
	void	FinishItemSilvering();
	void	OpenInventoryToRepair();
	void	OpenInventoryToSilvering();

	// tutorial interface by larosel
	void	RunBookcase();
	void	CloseBookcase();
	bool	IsRunningBookcase() const;
//	void	RunBriefing();
//	void	CloseBriefing();
//	bool	IsRunningBriefing() const;
	void	RunComputer();
	void	CloseComputer();
	bool	IsRunningComputer() const;
//	void	RunTutorialExit();
//	void	CloseTutorialExit();
//	bool	IsRunningTutorialExit() const;
//	void	CloseTutorialExitAsk();
//	void	RunTutorialExitAccept();

	// mail
	void	AddMail(int tab_id, DWORD id, SIZE windowSize, const char * sender, const char * title, DWORD date, bool open,int guildid, int guildtype);

	void	AddMailContents(int tab_id, DWORD id, const char * contents);
	void	DeleteMail(int tab_id, DWORD id);
	void	RunMailBox();
	void	CloseMailBox();
	// 2004, 10, 19, sobeit add start
	void	CloseMailBoxDialogBox() ;
	// 2004, 10, 19, sobeit add end
	bool	IsRunningMailBox() const;
	
	// mail-helpmessage//
	void	AddHelpMail(DWORD id, bool open);
	bool	IsRunningHelpMailDesc() {return m_pC_help_desc != NULL;}

	// horn
	void	RunHorn(int currentZoneID);
	void	CloseHorn();
	bool	IsRunningHorn() const;

	// finding mine
	void	RunFindingMine();
	void	CloseFindingMine();
	bool	IsRunningFindingMine() const;
	
	// minigame arrow tile
	void	RunArrowTile();
	void	CloseArrowTile();
	bool	IsRunningArrowTile()	const;

	// minigame crazy tile
	void	RunCrazyMine();
	void	CloseCrazyMine();
	bool	IsRunningCrazyMine()	const;
	
	// slayer_portal
	void	RunSlayerPortal( int nExtendLevel );
	void	CloseSlayerPortal();
	bool	IsRunningSlayerPortal() const;

	// TEAMlist
	void	RunTeamList(bool ready, bool IsUnion);
	void	CloseTeamList();
	bool	IsRunningTeamList(bool IsUnion = false) const;
	void	RunTeamMemberList();
	void	CloseTeamMemberList();
	bool	IsRunningTeamMemberList() const;
	void	RunTeamInfo(bool ready, void *info, bool IsUnion);
	void	CloseTeamInfo();
	// 2004, 10, 19, sobeit add start
	void	CloseTeamInfoDialogBox() ;
	// 2004, 10, 19, sobeit add end
	bool	IsRunningTeamInfo() const;
	void	RunTeamMemberInfo(C_VS_UI_TEAM_MEMBER_INFO::MEMBER_INFO *info);
	void	CloseTeamMemberInfo();
	bool	IsRunningTeamMemberInfo() const;
	void	RunTeamRegist(bool member, int reg_fee, int rank, char *date, char *team_name = NULL, int guild_id = -1);
	void	CloseTeamRegist();
	bool	IsRunningTeamRegist() const;

	void	UnselectSkill();
	void	ChangeSkillWheel(bool direct);
	void	SelectSkill(int id);

	void	RequestDie(DWORD timer, bool enforce = false);
	void	FinishRequestDie();
	bool	IsRunningRequestDie() const;

	void	RequestShrineMinimap(DWORD timer);
	void	FinishRequestShrineMinimap();
	bool	IsRunningRequestShrineMinimap() const;

	void	RunNoSearchResult();	
	void	CloseNoSearchResult();	

	void	RunDepositLimit();	
	void	CloseDepositLimit();	

	void	RunWithdrawLimit();	
	void	CloseWithdrawLimit();	

	
#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
	// PremiumGiveItem
	void	Request_PremiumGiveItem();
	void	FinishRequest_PremiumGiveItem();
	bool	IsRunningRequest_PremiumGiveItem() const;
#endif //__PREMIUM_GIVE_ITEM_UI

	// party
	void	FinishRequestParty();
	void	RequestParty(const char *name, DWORD timer);
	bool	IsRunningRequestParty() const;

	void	RunUsePetFood(DWORD UsingObjectID = 0, MItem* SubInventory = NULL);
	void	CloseUsePetFood();
	//2004, 5, 11 sobeit add start - �� ������
	void	RunKeepPetItemDialog();
	void	CloseKeepPetItemDialog();
	void	RunGetKeepPetItemDialog();
	void	CloseGetKeepPetItemDialog();
	//2004, 5, 11 sobeit add end
	
	bool	IsRunningPartyAsk() const;
	void	RunPartyAsk(const char* pName, C_VS_UI_REQUEST_PARTY::REQUEST_PARTY type);
	void	ClosePartyAsk();
	void	RunPartyCancel(const char* pName);	
	void	ClosePartyCancel();	
	
	void	RunEnchant(int value);
	void	CloseEnchant();
	
#if __CONTENTS(__TUNING_ITEM)
	void	RunTuningConfirm1(int value, char *name1, char *name2);	// Ʃ�� ����
	void	RunTuningConfirm2(int value, char *name1, char *name2);	// Ʃ�� �Ұ���

	void	RunConfirmError(char *message);	// �Ұ��� �޼���
#endif //__TUNING_ITEM

	void	RunThirdEnchant(int value);
	void	CloseThirdEnchant();

	void	RunPartyManager();
	void	ClosePartyManager();
	bool	IsRunningPartyManager() const;

	void	RunTraceWindow();
	void	CloseTraceWindow();
	bool	IsStartTrace();
	void	SetCannotTrace();
	bool	IsRunningTraceWindow() const;

	void	RunXmasCardWindow(const MItem *pItem);
	void	CloseXmasCardWindow();
	bool	IsRunningXmasCardWindow() const;

	void	RunWarListWindow();
	void	CloseWarListWindow();
	bool	IsRunningWarListWindow() const;
	void	AddWarList(C_VS_UI_WAR_LIST::WarInfo &war_info);
	void	ClearWarList();

	void	RunBringFeeWindow(UINT totalfee = 0, UINT bringfee = 0);
	void	CloseBringFeeWindow();
	bool	IsRunningBringFeeWindow();
	void	SetTotalFee(UINT totalfee);
	void	SetBringFee(UINT	fee);
	UINT	GetWillBringFee();
	UINT	GetTotalFee();
	void	RunBringFeeLimit();

	void	RunBloodBibleStatusWindow();
	void	CloseBloodBibleStatus();
	bool	IsRunningBloodBibleStatus();
	void	SetBloodBibleStatus(int blood_bible_id, C_VS_UI_BLOOD_BIBLE_STATUS::BLOOD_BIBLE_STATUS &bbs);
	void	UnSetBloodBibleStatus(int blood_bible_id);

	void	RunInputNameWindow(C_VS_UI_INPUT_NAME::INPUT_NAME_MODE_LIST mode);
	void	CloseInputNameWindow();
	bool	IsRunningInputNameWindow();
	const char *GetCurrentInputName();
	void	SetInputNameWindow(const char* szName);
	void	SetEnableInputName();
	void	SetDisableInputName();

	void	RunPopupMessage(const char *str, C_VS_UI_POPUP_MESSAGE::POPUP_TYPE type = C_VS_UI_POPUP_MESSAGE::POPUP_NORMAL);
	void	RunPopupMessage( C_VS_UI_POPUP_MESSAGE::PopupWindowStyle& style );
	void	ClosePopupMessage();
	bool	IsRunningPopupMessage();

	void	RunQuestStatusWindow();
	void	CloseQuestStatusWindow();
	bool	IsRunningQuestStatusWindow();
	void	SetQuestStatusInit();
	DWORD	GetQuestStatusID();
	void	SetQuestStatus(C_VS_UI_QUEST_STATUS::QUEST_STATUS &status);
	

	void	RunLotteryCard( int& step );
	void	CloseLotteryCard();
	bool	IsRunningLotteryCard();
	void	SetResultLotteryCard( bool bSuccess ) { if(m_pC_lottery_card) m_pC_lottery_card->SetResult( bSuccess ); }

	void	RunItemListWindow();
	bool	IsRunningItemListWindow();
	void	CloseItemListWindow();
	bool	AddItemToItemList(C_VS_UI_ITEM_LIST::ItemList pItem);
	bool	RemoveItemInItemList( DWORD ID );

	void	RunImageNotice();
	void	AddNotice(const char *name, DWORD& id);
	void	CloseImageNotice();
	bool	IsRunningImageNotice();

	void	RunBulletinBoardWindow(const MItem *pItem);
	void	CloseBulletinBoardWindow();
	bool	IsRunningBulletinBoardWindow() const;
	
	void	RunTransItem();
	void	CloseTransItem();

	void	RequestResurrect(bool f1, bool f2, bool eternity, bool IsSiegeAttacker, bool IsSkillFromOther);
	void	RequestResurrectSetButton(int resurrectMode, bool enable);
	void	FinishRequestResurrect();
	bool	IsRunningRequestResurrect() const;
	void	SetDelayRequestResurrect( C_VS_UI_REQUEST_RESURRECT::RESURRECT_MODE &mode, int &delay );

	void	ChangeToOustersInterface();

	
	void	RunMixingForge(C_VS_UI_MIXING_FORGE::FORGE_CLASS forge_class, C_VS_UI_MIXING_FORGE::FORGE_TYPE forge_type);
	void	CloseMixingForge();
	bool	IsRunningMixingForge() const;
#if __CONTENTS(__PET_MIXINGFORGE)
	void	RunPetMixingForge();
	void	ClosePetMixingForge();
	bool	IsRunningPetMixingForge() const;
#endif //__PET_MIXINGFORGE
	void	RunRemoveOptionFromRareItem(const MItem* pItem, const MItem* pItem2);
	void	CloseRemoveOptionFromRareItem();
	bool	IsRunningRemoveOptionFromRareItem() const;

	void	RunOustersSkillInfo(int skillID, int window_x, int window_y, bool flag);
	void	CloseOustersSkillInfo();
	bool	IsRunningOustersSkillInfo() const;
	
	void	SetOustersDownSkill();
	void	SetMiniGameScore(BYTE& Type, std::string& topname, WORD& topscore, WORD& mybestscore);

	void	RunQuestInventory(const MItem* pItem);
	bool	IsRunningQuestInventory() const;
	void	CloseQuestInventory();

	void	RunCTFStatusWindow();
	void	CloseCTFStatusWindow();
	bool	IsRunningCTFStatusWindow();
	void	SetCTFStatus(DWORD &endtime, int &flag_s, int &flag_v, int &flag_o);
	
	void	RunPetInfo(struct PETINFO *petInfo);
	void	ClosePetInfo();
	bool	IsRunningPetInfo();

	// ���� �޼��� desc // 
	void	RunHelpDesc(const char *detail,const char *title,DWORD id);
	void	CloseHelpDesc();
	bool	IsRunningHelpDesc();
	
	// 2006.01.15 chyaya Add Start - World Map Interface
	void	RunWorldMap();
	void	CloseWorldMap();
	void	ToggleWorldMap();
	bool	IsRunningWorldMap();
	// 2006.01.15 chyaya Add End - World Map Interface

//	void	RunCannotPlayMessage();
//	void	RunCannotPlayByAttrMessage();

//	void	CloseCannotPlayMessage();

	// 2004, 5, 11 sobeit add start - pet storage
	void	RunPetStorage();
	int		GetPetStorageEmptySlot();
	int		GetPetStorageFocusSlot();
	// 2004, 5, 11 sobeit add end

	// 2004, 5, 27 sobeit add start - sms message
	void	RunSMSMessage();
	bool	IsRunningSMSMessage() const;
	void	CloseSMSMessage() ;
	void	RunSMSList();
	bool	IsRunningSMSList() const;
	void	CloseSMSList() ;
	void	RunSMSRecord();
	bool	IsRunningSMSRecord() const;
	void	CloseSMSRecord() ;
	void	SetSMSAddressList(void* pVoid) ;
	void	AddSMSAddressList(DWORD element, char* name, char* id, char* num) ;
	void	DeleteSMSAddressList(int id) ;
	void	AddSMSSendList(char* str) ;
	void	AcquireSMS();
	void	SetSMSCharage(int i);
	// 2004, 5, 27 sobeit add end

	// 2004, 6, 14 sobeit add start - naming
	void	RunNaming();
	bool	IsRunningNaming() const;
	void	CloseNaming();
	bool	IsRunAskUseItemDialog() ;
	void	SetUseAskIitemRunning(bool bRunning);
	void	RunAskUseItemDialog(int AskType , int value = 0, void* pValue = NULL) ;
	void	CloseAskUseItemDialog();
	// 2004, 6, 14 sobeit add end
	// 2004, 6, 15 sobeit add start
	bool	IsRunningGearWindow() const;
	void	Change_Custom_Naming(int nID, char* szName);
	// 2004, 6, 15 sobeit add end
	// 2004, 6, 16 sobeit add start
	void	SetNickNameList(void* pData);
	void	AddNickNameList(void* pData);
	// 2004, 6, 16 sobeit add end

//	// 2004, 6, 18 sobeit add start
//	bool	GetChangeableNickName() { return m_ChangeableNickname;}
//	bool	GetChangeablePetNickName() { return m_ChangeablePetNickname;}
//	void	SetChangeableNickName(bool Is) { m_ChangeableNickname = Is;}
//	void	SetChangeablePetNickName(bool Is) { m_ChangeablePetNickname = Is;}
//	// 2004, 6, 18 sobeit add end
	// 2004, 6, 21 sobeit add start
	void	RunNamingChange(MItem* pItem, char* szCustom);
	bool	IsRunningNamingChange() const;
	void	CloseNamingChange();
	// 2004, 6, 21 sobeit add end
	// 2004, 6, 30 sobeit add start
	void	RunQuestManager();
	bool	IsRunningQuestManager() const;
	void	CloseQuestManager();
	bool	SetQuestManagerInfo(void* pVoid);
	bool	ModifyQuestManagerInfo(void* pVoid, int nType);
	void	SetQuestManagerSubWindow(DWORD dwID);
	void	SendQuestIntention(bool bIntention);
	void	ReleaseQuestXML();
	// 2004, 6, 30 sobeit add end
	// 2004, 7, 5 sobeit add start
	void	RunQuestList();
	bool	IsRunningQuestList() const;
	void	CloseQuestList();

	void	RunQuestDetail();
	bool	IsRunningQuestDetail() const;
	void	CloseQuestDetail();
		
	void	RunQuestMission();
	bool	IsRunningQuestMission() const;
	void	CloseQuestMission();
	
	void	RunQuestItem();
	bool	IsRunningQuestItem() const;
	void	CloseQuestItem();
	void	CloseQuestIcon(int JustClose);
	void	UpdateQuestItemInfo(std::vector<MItem*>& vItem);
	void	PushGQuestExcuteElement(DWORD qID, BYTE bCondition, WORD wIndex);
	void	RunNextGQuestExcuteElement();
	// 2004, 7, 5 sobeit add end
	void	DeleteQuestItem(int nSlot);
	
	// 2004, 10, 25, sobeit add start - ���� ����
	void	RunModifyTax();
	void	RunRangerChat();
	bool	IsRunngingRangerChat();
	void	CloseRangerChat();
	void	SetRangerChatString(char* str);
	// 2004, 10, 25, sobeit add end

	// 2004, 11,30, sobeit add start
	void	RunPowerjjang();
	bool	IsRunningPowerjjang();
	void	ClosePowerjjang();
	void	PowerjjangGambleResult(BYTE bItemCode);
	// 2004, 11,30, sobeit add end
	
	// 2005, 1, 3, sobeit add start
	void OpenInventoryToSwapAdvanceItem();
	void FinishSwapAdvancementItem();
	bool IsRunningSwapAdvancementItem() const;
	// 2005, 1, 3, sobeit add end
	
	// 2005, 1, 11, sobeit add start - �ҿ��̿����� â
	void	Run_Campaign_Help_Unfortunate_Neighbors(int value);
	// 2005, 1, 11, sobeit add end

	// 2005, 1, 17, sobeit add start - ����Ʈ ����
	void	SetQuestNpcDialog(void* pVoid);
	// 2005, 1, 17, sobeit add end - ����Ʈ ����
	
	// 2005, 1, 24, sobeit add start - ������ �ޱ� �̺�Ʈ
	void	Run_Confirm_GetItemEvent(int value);
	// 2005, 1, 24, sobeit add end

	// 2005, 2, 1, sobeit add start
	void	RunWebBrowser(HWND hWnd, char* szURL, HINSTANCE hInst) ;// void* pWebOjbect);
	bool	IsRunningWebBrowser();
	void	CloseWebBrowser();
	int		GetMouseCursorInfo();
	bool	IsInRectPointWebBrowser(int X, int Y);
	// 2005, 2, 1, sobeit add end

	// 2005, 2, 24, sobeit add start
	void	RunSubInventory(MItem* pItem);
	int		CountRunningSubInventory();

	bool	IsRunningSubInventory(TYPE_OBJECTID SubInventory);
	bool	ExistSubInventory(TYPE_OBJECTID SubInventory);

	bool	AddItemToSubInventory(TYPE_OBJECTID SubInventory, MItem* pItem, int X, int Y);
	bool	ReplaceSubInventoryItem(TYPE_OBJECTID SubInventory, MItem* pItem, BYTE x, BYTE y, MItem*& pOldItem);
	
	void	CloseSubInventory(TYPE_OBJECTID SubInventory);
	void	CloseAllSubInventory();
	void	ClearSubInventory();	
	// 2005, 2, 25, sobeit add end
	
	void	AutoMoveInventoryItemToQuickSlot(BYTE inven_grid_x, BYTE inven_grid_y, BYTE quick_slot_num);	///< chyaya Add
	
	// 2005, 4, 21, sjheon add start - ��ȥ���Ǳ�  â
	void	Run_Wedding_Contribution_Unfortunate_Neighbors(int value = 0);
	//2005, 4, 21, sjheon add end - ��ȥ���Ǳ� â

	//2005, 7, 14, sjheon add  - ComboSkill Add
	void	SetComboCnt(int ComboCnt = 1) ; 
	void	SetSelectedAttackComboSkill(bool	bAttackSkill = FALSE);
	//2005, 7, 14, sjheon add  - ComboSkill End
	
	//2005, 7, 18, sjheon add  - PopupSummerComBack Add
	void	PopupSummerComBack(int value = 0);
	//2005, 7, 18, sjheon add  - PopupSummerComBack End
	
	//2005, 7, 26, sjheon add  - BloodBurst ������ Add
	//void	SetGage() ; 
	void	SetBloodBurstAttackGage();
	void	SetBloodBurstDefenseGage();
	void	SetBloodBurstPartyGage();
	
	bool	GetBloodBurstAttackGageFull();
	bool	GetBloodBurstDefenseGageFull();
	bool	GetBloodBurstPartyGageFull();
	//2005, 7, 26, sjheon add  - BloodBurst ������ End
	int		GetSelectedSkillID();
	
	void	RunTaxRate(int	TexRate)	;
	bool	IsRunTaxRate()	;
	void	CloseTexRate()	;


	bool	IsRunningDelegation_Of_Power() ;
	void	RunDelegation_Of_Power(const char* pName)	;	
	void	CloseDelegation_Of_Power();	

	// 2006.01.15 chyaya Add Start - Operator Only Interface
	void	RunOperator();
	void	CloseOperator();
	void	ToggleOperator();
	bool	IsRunningOperator();
	// 2006.01.15 chyaya Add End - Operator Only Interface
	

	// 20070529 by diesirace Timecount 
	void	RunTimeCount(DWORD time, LPCTSTR desc);
	void	CloseTimeCount();
	bool	IsRuningTimeCount();

#if __CONTENTS(__GEAR_SWAP_CHANGE)		// C_VS_UI_GAME		����â�� ID�� ���� �Լ� ����
	DWORD	GetGearID();
#endif	//__GEAR_SWAP_CHANGE

#if __CONTENTS(__QUEST_RENEWAL)
	void	GpsToBringQuestInfo();
#endif

#if __CONTENTS(__USER_GRADE)
	void ToggleUserGrade();
	bool IsRuningUserGrade();
#endif	//__USER_GRADE

#if __CONTENTS(__TIPOJYU_CASTLE)	//RunIndounNpcDlg
	void RunIndounNpcDlg(S_INDOUN_DLG_INFO Indoun_Dlg_Info, ENUMTYPE_INDUONSELECT etIndounSelect);
	void CloseIndounNpcDlg();
#endif // __TIPOJYU_CASTLE;
	
#if __CONTENTS(__ITEM_SHOW)
	void CloseInventory();
#endif //__ITEM_SHOW
	//---------------------------------------------------------------
	// �Է����� ���ڿ��� �ٷ� �����Ѵ�.by sigi
	//---------------------------------------------------------------
	void	SetInputString(const char* pString)
	{
		if(m_pC_chatting!=NULL)
			m_pC_chatting->SetInputString( pString );
	}

	//---------------------------------------------------------------
	// �Է����� ���ڿ��� �߰� by sigi
	//---------------------------------------------------------------
	void	AddInputString(const char* pString)
	{
		m_pC_chatting->AddInputString( pString );
	}

	//---------------------------------------------------------------
	// �Է����� ���ڿ� �о���� by sigi
	//---------------------------------------------------------------
	const char* GetInputString() const
	{
		return m_pC_chatting->GetInputString();
	}

#if __CONTENTS(__ITEM_SHOW)
	const char* GetNewInputString() const
	{
		return m_pC_chatting->GetNewInputString();
	}
#endif //__ITEM_SHOW

	//---------------------------------------------------------------
	// �Է����� ������ ���� �ٲٱ� by sigi
	//---------------------------------------------------------------
	void	SetInputStringColor(COLORREF rgb)
	{
		m_pC_chatting->SetInputStringColor(rgb);
	}

	//---------------------------------------------------------------
	// -_- ���� �� �˾Ƴ���
	int		GetSelectedMainMenu()
	{
		if(m_pC_tribe_interface != NULL)
			return m_pC_tribe_interface->GetSelectedMenu();
		
		return 0;
	}

	void	SetMainMenu(int menu)
	{
		if(m_pC_tribe_interface != NULL)
			m_pC_tribe_interface->SetSelectMenu(menu);
	}
	//---------------------------------------------------------------

	
	//---------------------------------------
	// Hot key
	//---------------------------------------
	void HotKey_PetInfo();		// ctrl + CapsLock
	void HotKey_MailBox();		// ctrl + b
	void HotKey_PartyChat();	// ä�� ���ɾ� /p
	void HotKey_Quest();		// ctrl + q
	void HotKey_Party();		// crtr + p
	void HotKey_Mark();			// '����'Ű
	void HotKey_Help();			// ctrl + h
	void HotKey_SkillInfo();	// ctrl + k
	void HotKey_WorldMap();		// ctrl + m
	void HotKey_Minimap();		// ����
	void HotKey_CharInfo();		// ctrl + i
	void HotKey_Grade1Info();	// ctrl + c
	void HotKey_Grade2Info();	// ctrl + r
	void HotKey_Grade3Info();	// ctrl + a
//	void HotKey_WindowToggle();
	void HotKey_Inventory(bool IsCheckSubInventory = false);	
	void HotKey_Gear();			
#if __CONTENTS(__GEAR_SWAP_CHANGE) // ����â ��üŰ�� ��� �� ���� �Լ� ����
	void HotKey_GearChange();		// Alt + A
	void HotKey_GearChangeShow();	// Alt + S	
#endif
	void HotKey_QuickItemSlot();
	void HotKey_Skill();
	void CloseSkillChooseWindow();
	void HotKey_Chat_Up();		// ctrl + PgUp
	void HotKey_Chat_Dn();		// ctrl + PgDn
	void HotKey_Tribe_Prior();	// alt	+ PgUp
	void HotKey_Tribe_Next();	// alt	+ PgDn
	void HotKey_Guild_Info();	// ctrl + v
	void HotKey_Guild_Member_List();// ctrl + l
	void HotKey_Coer_Attack();	// ctrl + o
// by svi	
#if __CONTENTS(__AUTO_ATTACT)	//__AUTO_ATTACT
	void HotKey_Auto_Attack();	// ctrl + j	
#endif  //__AUTO_ATTACT	
// end
	void HotKey_Sysrq();		// printscreen sysrq
	void HotKey_Slash();		// ctrl + /
	void HotKey_Background_Music();	// ä�� ���ɾ� /�����
	void HotKey_Effect_Music();		// ä�� ���ɾ� /ȿ����
	void HotKey_Where();			// ä�� ���ɾ� /���
	void HotKey_Chat_Help();		// ä�� ���ɾ� /ä�õ���
	void HotKey_NormalChat();	// ä�� ���ɾ� /c
	void HotKey_ZoneChat();		// ä�� ���ɾ� /z
	void HotKey_GuildChat();	// ä�� ���ɾ� /g
	void HotKey_UnionChat();	// ä�� ���ɾ� /u
	void HotKey_Whisper();		// ä�� ���ɾ� /w
	void HotKey_ExtendChat();	// ctrl + e
	void HotKey_ESC();			
	void HotKey_F1();				
	void HotKey_F2();				
	void HotKey_F3();				
	void HotKey_F4();				
	void HotKey_F5();				
	void HotKey_F6();				
	void HotKey_F7();				
	void HotKey_F8();				
	void HotKey_F9();				
	void HotKey_F10();			
	void HotKey_F11();			
	void HotKey_F12();
	void Hotkey_Function(int FKey) ;


	// 2004, 5, 17 sobeit add start
	void HotKey_SummonPet();	// ctrl + space
	// 2004, 5, 17 sobeit add end

	void	ResetHotKey();

		// 2004, 11, 18 thai00
	bool	IsRunningPersnalShop() const;
	void    RunPersnalShop();
	void	ClosePersnalShop();

	bool	IsRunningPersnalShopMessage() const;
	void    RunPersnalShopMessage();
	void	ClosePersnalShopMessage();
	void	SetPersnalShop(MStorage* pStorage);

	void	Inventory_Item_Position_Remove();

#if __CONTENTS(__FRIEND_ADDITION)
	// 2009-07-06 Add
	bool	CloseFriendMessagePopup();
	void	ReleaseFriendMessagePopup();
#endif //__FRIEND_ADDITION
};

#endif
