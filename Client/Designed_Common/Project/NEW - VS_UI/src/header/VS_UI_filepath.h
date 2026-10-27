#ifndef __VS_UI_FILEPATH_H__
#define __VS_UI_FILEPATH_H__

#define _ROOT							"data\\ui\\"


#define TXT_ROOT						_ROOT"txt\\"
#define TXT_TUTORIAL_BRIEFING			"Briefing.txt"
#define TXT_TUTORIAL_COMPUTER			"Computer.xml"
//#define INF_SLAYER_PORTAL				"SlayerPortal.inf" // moved to Data\Info folder
#define TXT_TUTORIAL_WELCOME_SLAYER		"WelcomeSlayer.txt"
#define TXT_TUTORIAL_WELCOME_VAMPIRE	"WelcomeVampire.txt"
#define TXT_TUTORIAL_WELCOME_OUSTERS	"WelcomeOusters.txt"


//----------------------------------------------------------------------
// rpk file
//----------------------------------------------------------------------
#define RPK_ITEM						TXT_ROOT"Item.rpk"
#define RPK_HELP						TXT_ROOT"Help.rpk"
#define RPK_SKILL						TXT_ROOT"Skill.rpk"
#define RPK_BOOK						TXT_ROOT"Book.rpk"
#define RPK_TUTORIAL_ETC				TXT_ROOT"TutorialEtc.rpk"
#define RPK_PROGRESS					TXT_ROOT"progress.rpk"
#define RPK_TITLE						TXT_ROOT"title.rpk"

// Tek kaynak şifre. Eski RPK'ler cjdgorkr idi; aktif paket 1x700.
#ifndef RPK_PASSWORD
#define RPK_PASSWORD					"1x700"
#endif

//----------------------------------------------------------------------
// server info file // test
//----------------------------------------------------------------------
#ifndef _LIB
#define	FILE_INFO_STR_EXP						"Data\\Info\\STRExp.inf"
#define	FILE_INFO_DEX_EXP						"Data\\Info\\DEXExp.inf"
#define	FILE_INFO_INT_EXP						"Data\\Info\\INTExp.inf"
#define	FILE_INFO_VAMPIRE_EXP					"Data\\Info\\VampireExp.inf"
#define	FILE_INFO_OUSTERS_EXP					"Data\\Info\\OustersExp.inf"
#define	FILE_INFO_SKILL_DOMAIN_EXP				"Data\\Info\\DomainExp.inf"
#define	FILE_INFO_SKILL_INFO					"Data\\Info\\SkillInfo.inf"
#define FILE_INFO_ITEM_INFO						"Data\\Info\\item.inf"
#define FILE_INFO_CREATURE_INFO					"Data\\Info\\Creature.inf"
#define FILE_INFO_NPC_INFO						"Data\\Info\\NPC.inf"
#define FILE_INFO_SOUND_INFO					"Data\\Info\\Sound.inf"
#define FILE_INFO_GUILD_MAPPER					"Data\\Info\\GuildMapper.inf"
#define	FILE_INFO_RANK_BONUS_TABLE				"Data\\Info\\RankBonus.inf"
#define FILE_INFO_SLAYER_RANK_EXP				"Data\\Info\\SlayerRankExp.inf"
#define FILE_INFO_VAMPIRE_RANK_EXP				"Data\\Info\\VampireRankExp.inf"
#define FILE_INFO_OUSTERS_RANK_EXP				"Data\\Info\\OustersRankExp.inf"
#define FILE_INFO_ADVANCEMENT_EXP				"Data\\Info\\AdvancementClassExp.inf"
#define FILE_INFO_ZONE_TABLE					"Data\\Info\\Zone.inf"
#define	FILE_INFO_GPS							"Data\\Info\\Gps.inf"
#define FILE_INFO_FILEDEF						"Data\\Info\\FileDef.inf"

#define SPK_GAME_BACK							"data\\gameback.spk"
#define SPK_GUILD_MARK							"Data\\Image\\GuildMark.spk"

#define	SPK_GRADE_SLAYER						"Data\\Image\\GradeMarkSlayer.spk"
#define	SPK_GRADE_VAMPIRE						"Data\\Image\\GradeMarkVampire.spk"
#define	SPK_GRADE_OUSTERS						"Data\\Image\\GradeMarkOusters.spk"
#define	SPK_LEVEL_MARK							"Data\\Image\\LevelMarkPet.spk"
#define SPK_ORIGIN_MARK							"Data\\Image\\OriginMark.spk"
#endif

#define INF_SLAYER_PORTAL						"Data\\Info\\SlayerPortal.inf" // moved to Data\Info folder
#define FILE_INFO_TITLE_UI						"\\interface.ini"
#define FILE_INFO_HORN							"Data\\Info\\Horn.inf"
#define FILE_INFO_TESTSERVER					"Data\\Info\\TestServer.inf"

#define	IFR_ROOT						_ROOT"txt\\"
#define	IFR_UI							IFR_ROOT"ui.ifr"
#define	IFR_ACTION						IFR_ROOT"action.ifr"
#define	IFR_SKILL						IFR_ROOT"skill.ifr"
#define	IFR_INVENTORY					IFR_ROOT"inventory.ifr"
#define	IFR_USE_ITEM					IFR_ROOT"useitem.ifr"
#define FILE_INFO_ACTION_FK	            IFR_ROOT"action.inf"

#define SPK_MINIMAP						_ROOT"spk\\Minimap.spk"
#define SPKI_MINIMAP					_ROOT"spk\\Minimap.spki"	// by sigi 2001.9.5

// tutorial image by larosel
#define SPK_TUTORIAL_BOOKCASE			_ROOT"spk\\TutorialBookcase.spk"
#define SPK_TUTORIAL_BOOK				_ROOT"spk\\TutorialBook.spk"
#define SPK_TUTORIAL_COMPUTER			_ROOT"spk\\TutorialComputer.spk"
#define SPK_ITEM						_ROOT"spk\\ItemDescription.spk"
#define SPKI_ITEM						_ROOT"spk\\ItemDescription.spki"

#define SPK_ELEVATOR					_ROOT"spk\\Elevator.spk"

#define SPK_SKILL_ICON					_ROOT"spk\\skillminirenewal2.spk"
#define SPKI_SKILL_ICON					_ROOT"spk\\skillminirenewal2.spki"

// Phoenix 2026-05-12: VS_UI_SKILL_VIEW.cpp icin SPK_SKILLVIEW_* asset path'leri.
// Phoenix asset'leri konsolide: ana pencere icin skillwindow.spk, irk-spesifik
// domain ikonlari icin skilletc{slayer,vampire,ousters}.spk kullaniliyor.
// Eski VS6 yedi farkli SkillView_*.spk dosyasi olusturuyordu (artik yok).
// 3 irk parite: Slayer/Vampire/Ousters icin de domain spk'lari mevcut.
#define SPK_SKILLVIEW_ETC				_ROOT"spk\\skillwindow.spk"
#define SPK_SKILLVIEW_SLAYER			_ROOT"spk\\skillwindow.spk"
#define SPK_SKILLVIEW_SLAYER_GRADE		_ROOT"spk\\skillwindow.spk"
#define SPK_SKILLVIEW_SLAYER_DOMAIN		_ROOT"spk\\skilletcslayer.spk"
#define SPK_SKILLVIEW_VAMPIRE			_ROOT"spk\\skillwindow.spk"
#define SPK_SKILLVIEW_VAMPIRE_GRADE		_ROOT"spk\\skillwindow.spk"
#define SPK_SKILLVIEW_VAMPIRE_DOMAIN	_ROOT"spk\\skilletcvampire.spk"
#define SPK_SKILLVIEW_OUSTERS			_ROOT"spk\\skillwindow.spk"
#define SPK_SKILLVIEW_OUSTERS_GRADE		_ROOT"spk\\skillwindow.spk"
#define SPK_SKILLVIEW_OUSTERS_DOMAIN	_ROOT"spk\\skilletcousters.spk"

#define SPK_MINE_PROGRESS				_ROOT"spk\\MineProgress.spk"

#define	SPK_BOX								_ROOT"spk\\AssembleMessageBox.spk"
#define	SPK_SCROLL_BAR						_ROOT"spk\\ScrollBar.spk"
#define SPK_ASSEMBLE_BOX_SLAYER				_ROOT"spk\\AssembleMessageBoxSlayer.spk"
#define SPK_ASSEMBLE_BOX_BUTTON_SLAYER		_ROOT"spk\\AssembleMessageBoxButtonSlayer.spk"
#define SPK_ASSEMBLE_BOX_VAMPIRE			_ROOT"spk\\AssembleMessageBoxVampire.spk"
#define SPK_ASSEMBLE_BOX_BUTTON_VAMPIRE		_ROOT"spk\\AssembleMessageBoxButtonVampire.spk"
#define	SPK_COMMON_BUTTON					_ROOT"spk\\CommonButton.spk"

#define SPK_EXCHANGE_SLAYER						_ROOT"spk\\ExchangeSlayer.spk"
#define SPK_SLAYER_SHOP_STORAGE			_ROOT"spk\\Shop&StorageSlayer.spk"
#define SPK_VAMPIRE_SHOP_STORAGE			_ROOT"spk\\Shop&StorageVampire.spk"

// title
#define SPK_ANI_TITLE					_ROOT"spk\\TitleAni.spk"
#define SPK_ANI_SYMBOL					_ROOT"spk\\Symbol.spk"
#define SPK_TITLE_MENU_DEFAULT		_ROOT"spk\\TitleMenuDefault.spk"
#define SPK_TITLE_MENU_SELECT			_ROOT"spk\\TitleMenuSelect.spk"
#define SPK_LOGIN_MENU			_ROOT"spk\\LoginMenu.spk"

// characters
#define SPK_COMMON						_ROOT"spk\\Common.spk"
#define SPK_COMMON_1024						_ROOT"spk\\Common1024.spk"

#define SPK_SERVER_SELECT					_ROOT"spk\\ServerSelect.spk"
#define SPK__NEW_SERVER_SELECT				_ROOT"spk\\new\\serverselectrenewal\\00000.spk"

#define	SPK_CHAR_MANAGER				_ROOT"spk\\CharManager.spk"
#define	SPK_CHAR_MANAGER800				_ROOT"spk\\CharManager800.spk"
#define SPK_CHAR_CREATE					_ROOT"spk\\CharCreate.spk"
#define SPK_CHAR_CREATE800					_ROOT"spk\\CharCreate800.spk"
#define SPK_CHAR_CREATE_NEW					_ROOT"spk\\charcreaterenewal.spk"


// System button
#define SPK_SLAYER_SYS_BUTTON			_ROOT"spk\\SysButtonSlayer.spk"
#define SPK_VAMPIRE_SYS_BUTTON		_ROOT"spk\\SysButtonVampire.spk"

// Slayer skill etc
#define SPK_SLAYER_SKILL_ETC			_ROOT"spk\\SkillEtcSlayer.spk"
#define SPK_VAMPIRE_SKILL_ETC			_ROOT"spk\\SkillEtcVampire.spk"

//#define IMAGE_BOXBACK					_ROOT"back.tga"
#define SPK_MOUSE_POINTER				_ROOT"spk\\MousePointer.spk"
#define SPK_TITLE						_ROOT"spk\\Title.spk"
#define SPK_TITLE_1024					_ROOT"spk\\Title1024.spk"

#define SPK_NEW_TITLE					_ROOT"spk\\new\\renewalgametitle\\00000.spk"
#define SPK_NEW_ID_CREATE				_ROOT"spk\\new\\newidcreate\\NewIDCreate.spk"

// Reborn Option menu portu (2026-09-02) — 16'sar frame'lik parcalar
// renewaloption: tek dosyada 33 frame (Reborn OptionSpriteMetadata sirasi).
// 00016/00032 chunk'lari eski asset varyanti — kullanilmiyor.
#define SPK_RENEWAL_OPTION				_ROOT"spk\\new\\renewaloption\\00000.spk"
#define SPK_HOTKEY_SETTING_0			_ROOT"spk\\new\\hotkeysettingwindow\\00000.spk"
#define SPK_HOTKEY_SETTING_16			_ROOT"spk\\new\\hotkeysettingwindow\\00016.spk"

//#define SPK_BBS							_ROOT"spk\\BBS.spk"
#define SPK_PROGRESS					_ROOT"spk\\Progress.spk"
#define SPK_PROGRESS_1024					_ROOT"spk\\Progress1024.spk"

//#define SPK_PROGRESS_EVENT				_ROOT"spk\\progressevent.spk"
#define SPK_PROGRESS_CHARACTER			_ROOT"spk\\ProgressChar.spk"
#define SPKI_PROGRESS_CHARACTER			_ROOT"spk\\ProgressChar.spki"
#define SPK_PROGRESS_EVENT				_ROOT"spk\\ProgressEvent.spk"
#define SPK_PROGRESS_EVENT_1024			_ROOT"spk\\ProgressEvent1024.spk"
// 2026-09-03 Renewal loading ekrani (Reborn paritesi) — standalone spk klasoru.
// Buyuk dosyalar (>=512KB) rastgele loading gorseli; 00013=bar zemini (670x22),
// 00014=bar dolgusu (663x16). Reborn Application.cpp ayni kurali kullanir.
#define SPK_PROGRESS_EVENT_RENEWAL_DIR	_ROOT"spk\\progresseventrenewal\\"
#define SPK_PROGRESS_PPL				_ROOT"spk\\ProgressPPL.spk"
#define SPK_PROGRESS_PPL_1024			_ROOT"spk\\ProgressPPL1024.spk"
#define SPK_PROGRESS_CI					_ROOT"spk\\ProgressCI.spk"

#define	SPK_INFO_SLAYER					_ROOT"spk\\InfoSlayer.spk"
#define	SPK_INFO_VAMPIRE				_ROOT"spk\\InfoVampire.spk"
#define	SPK_INFO_OUSTERS				_ROOT"spk\\InfoOusters.spk"
// TEST 2026-08-31: Other Info deneme kabugu (orijinal Info*.spk yolu duruyor)
#define	SPK_OTHER_CHAR_INFO				_ROOT"spk\\OtherCharInfo.spk"
#define	SPK_PET_INFO_WOLFDOG				_ROOT"spk\\PetWolfDog.spk"//1
#define	SPK_PET_INFO_CENTAURO				_ROOT"spk\\PetCentauro.spk"//2
#define	SPK_PET_INFO_STIRGE					_ROOT"spk\\PetStirge.spk"//3
#define	SPK_PET_INFO_PIXIE					_ROOT"spk\\PetPixie.spk"//4
#define	SPK_PET_INFO_LUPENG					_ROOT"spk\\PetLupeng.spk"//5
#define SPK_PET_FACE						_ROOT"spk\\PetFace.spk"
#define	SPK_PET_BUTTON_SLAYER				_ROOT"spk\\PetButtonSlayer.spk"
#define	SPK_PET_BUTTON_VAMPIRE				_ROOT"spk\\PetButtonSlayer.spk"
#define	SPK_PET_BUTTON_OUSTERS				_ROOT"spk\\PetButtonSlayer.spk"

//20071114 New Pet
#define	SPK_PET_INFO_MILITARYBEAR			_ROOT"spk\\PetMilitaryBear.spk"//6
#define	SPK_PET_INFO_GARGOYLE				_ROOT"spk\\PetGargoyle.spk"//7

// __NEW_PET_FIRE_ELEMENTAL 20080605 New Pet
#define	SPK_PET_INFO_FIREELEMENTAL			_ROOT"spk\\PetFireElemental.spk"//8
#define	SPK_PET_INFO_SUCCUBUS				_ROOT"spk\\PetSuccubus.spk"//9 
#define	SPK_PET_INFO_INCUBUS				_ROOT"spk\\PetIncubus.spk"//10
#define	SPK_PET_INFO_MASHIMARO				_ROOT"spk\\PetMashimaro.spk"//11

// __NEW_PET by viva 20090923
#define SPK_PET_INFO_TABBYCAT				_ROOT"spk\\pettabbycat.spk"//12
#define SPK_PET_INFO_WEALTHYCAT				_ROOT"spk\\petwealthycat.spk"//13	
#define SPK_PET_INFO_SIAMINGCAT				_ROOT"spk\\petsiamingcat.spk"//14

#define	SPK_PET_INFO_nepenthespot			_ROOT"spk\\petnepenthespot.spk" //16
#define	SPK_PET_INFO_thoriumruffian         _ROOT"spk\\petthoriumruffian.spk" //17
#define	SPK_PET_INFO_actiniumruffian		_ROOT"spk\\petactiniumruffian.spk" //18
#define	SPK_PET_INFO_deadbody				_ROOT"spk\\petdeadbody.spk"    //19
#define	SPK_PET_INFO_turningdead		    _ROOT"spk\\petturningdead.spk"//20
#define	SPK_PET_INFO_kid				    _ROOT"spk\\petkid.spk"//21
#define	SPK_PET_INFO_soldier				_ROOT"spk\\petsoldier.spk"// 22
#define	SPK_PET_INFO_captain				_ROOT"spk\\petcaptain.spk"// 23
#define	SPK_PET_INFO_canine				    _ROOT"spk\\petcanine.spk"// 24
#define	SPK_PET_INFO_degula			    	_ROOT"spk\\petduracula.spk"// 25
#define	SPK_PET_INFO_YML			    	_ROOT"spk\\PetWolfDog.spk"// 26 ��ڤ��
#define	SPK_PET_INFO_XKTK			    	_ROOT"spk\\PetCentauro.spk"// 27 XK2̹��
#define	SPK_PET_INFO_EML			    	_ROOT"spk\\PetStirge.spk"// 28 ��ħ��
#define	SPK_PET_INFO_HXZ			    	_ROOT"spk\\PetPixie.spk"// 29 ������

#define SPK_SLAYER_GAMEMENU				_ROOT"spk\\gamemenu.spk"
#define SPK_VAMPIRE_GAMEMENU			_ROOT"spk\\gamemenu.spk"

#define SPK_OPTION_SLAYER				_ROOT"spk\\OptionSlayer.spk"
#define SPK_OPTION_VAMPIRE				_ROOT"spk\\OptionVampire.spk"
#define SPK_OPTION						_ROOT"spk\\Option.spk"

#define SPK_SLAYER_MAIN					_ROOT"spk\\simpleinformation.spk"
#define SPK_VAMPIRE_MAIN				_ROOT"spk\\simpleinformation.spk"

#define SPK_LOGIN							_ROOT"spk\\Login.spk"
#define SPK_LOGIN_1024							_ROOT"spk\\Login1024.spk"
#define SPK_INTERFACE_BG					_ROOT"spk\\InterfaceBG.spk"

#define SPK_SLAYER_GEAR					_ROOT"spk\\GearSlayer.spk"
#define SPK_SLAYER_GEAR_SLOT			_ROOT"spk\\GearSlotSlayer.spk"
#define SPK_VAMPIRE_GEAR_SLOT			_ROOT"spk\\GearSlotVampire.spk"
#define SPK_VAMPIRE_GEAR				_ROOT"spk\\GearVampire.spk"
#define SPK_SLAYER_INVENTORY			_ROOT"spk\\InventorySlayer.spk"
#define SPK_VAMPIRE_INVENTORY			_ROOT"spk\\InventoryVampire.spk"

#define SPK_QUICKITEM_BAR				_ROOT"spk\\new\\QuickItemBarRenewal\\QuickitemBarRenewal.spk"

#define SPK_SLAYER_QUICKITEM			SPK_QUICKITEM_BAR
#define SPK_VAMPIRE_QUICKITEM			SPK_QUICKITEM_BAR
#define SPK_OUSTERS_QUICKITEM			SPK_QUICKITEM_BAR


#define SPK_PARTY_CURSOR					_ROOT"spk\\PartyCursor.spk"


#define ISPK_ITEM							_ROOT"spk\\newitem.ispk"
#define SPK_SKILL_MINI						_ROOT"spk\\skillmini.spk"
#define SPKI_SKILL_MINI						_ROOT"spk\\skillmini.spki"

// Character Creation Sprite
#define SPK_FACE							_ROOT"spk\\Face.spk"
#define SPKI_FACE							_ROOT"spk\\Face.spki"
#define SPK_FACE_MAKE						_ROOT"spk\\FaceMake.spk"
#define SPK_PARTY_FACE						_ROOT"spk\\FaceParty.spk"
#define SPK_PROFILE							"UserSet\\"

#define ISPK_SLAYER_MAN					"data\\Image\\AddonMan.ispk"
#define ISPKI_SLAYER_MAN				"data\\Image\\AddonMan.ispki"
#define ISPK_SLAYER_WOMAN				"data\\Image\\AddonWoman.ispk"
#define ISPKI_SLAYER_WOMAN				"data\\Image\\Addonwoman.ispki"

#define ISPK_VAMPIRE					"data\\Image\\Vampire.ispk"
#define ISPKI_VAMPIRE					"data\\Image\\Vampire.ispki"
#define ISPK_OUSTERS					"data\\Image\\Ousters.ispk"
#define ISPKI_OUSTERS					"data\\Image\\Ousters.ispki"
#define CFPK_SLAYER_MAN					"data\\Image\\AddonMan.cfpk"
#define CFPK_SLAYER_WOMAN				"data\\Image\\AddonWoman.cfpk"
#define CFPK_VAMPIRE					"data\\Image\\Vampire.cfpk"
#define CFPK_OUSTERS					"data\\Image\\Ousters.cfpk"

// etc
#define SPK_NPCFACE						_ROOT"spk\\NpcFace.spk"
#define SPKI_NPCFACE					_ROOT"spk\\NpcFace.spki"

#define SPK_LEVELUP			_ROOT"spk\\LevelUp.spk"
#define SPK_CREDIT						_ROOT"spk\\credit.spk"
#define SPK_CREDIT_1024						_ROOT"spk\\credit1024.spk"


#define SPK_SLAYER_BUY_DIALOG			_ROOT"spk\\BuyDialogSlayer.spk"
#define SPK_VAMPIRE_BUY_DIALOG		_ROOT"spk\\BuyDialogVampire.spk"

//Slayer Portal
#define SPK_SLAYER_PORTAL_ETC			_ROOT"spk\\PortalEtcSlayer.spk"
#define SPK_SLAYER_PORTAL_MAP			_ROOT"spk\\PortalMapSlayer.spk"

//Party
#define SPK_SLAYER_PARTY				_ROOT"spk\\PartySlayer.spk"
#define SPK_VAMPIRE_PARTY				_ROOT"spk\\partyrenewal.spk"

//Guild
#define SPK_SLAYER_TEAM					_ROOT"spk\\TeamSlayer.spk"
#define SPK_VAMPIRE_TEAM				_ROOT"spk\\TeamVampire.spk"
#define SPK_SLAYER_TEAM_MEMBER			_ROOT"spk\\TeamMemberSlayer.spk"
#define SPK_VAMPIRE_TEAM_MEMBER			_ROOT"spk\\TeamMemberVampire.spk"

// HPBar
#define SPK_HPBAR_SLAYER				_ROOT"spk\\HPBarSlayer.spk"
#define SPK_HPBAR_VAMPIRE				_ROOT"spk\\HPBarVampire.spk"

// MINIMAP
#define SPK_MINIMAP_SLAYER				_ROOT"spk\\MinimapSlayer.spk"
#define SPK_MINIMAP_VAMPIRE				_ROOT"spk\\minimaprenewal.spk"

// GPS
#define SPK_GPS_SLAYER					_ROOT"spk\\new\\gpsforall_renewal\\gpsforall_renewal.spk"
#define SPK_GPS_VAMPIRE					_ROOT"spk\\new\\gpsforall_renewal\\gpsforall_renewal.spk"
#define SPK_GPS_OUSTERS					_ROOT"spk\\new\\gpsforall_renewal\\gpsforall_renewal.spk"

#define SPK_GPSMAP						_ROOT"spk\\GPSMAP.spk"

// CHATTING
#define SPK_CHATTING_SLAYER				_ROOT"spk\\ChattingSlayer.spk"
#define SPK_CHATTING_VAMPIRE			_ROOT"spk\\chattingslayernew.spk"

// EFFECT_STATUS
#define SPK_EFFECT_STATUS_SLAYER		_ROOT"spk\\EffectStatusSlayer.spk"
#define SPK_EFFECT_STATUS_VAMPIRE		_ROOT"spk\\EffectStatusVampire.spk"

// FILE DIALOG
#define SPK_FILE_DIALOG					_ROOT"spk\\FileDialog.spk"
#define DIR_PROFILE							"UserSet"

#define SPK_QUESTION					_ROOT"spk\\Question.spk"


// SoulChain
#define SPK_SOUL_CHAIN					_ROOT"spk\\Trace.spk"

// Age Rating 
#define SPK_USE_GRADE					_ROOT"spk\\UseGrade.spk"

#define SPK_XMAS_CARD					_ROOT"spk\\XMasCard.spk"

#define SPK_LOTTERY_CARD				_ROOT"spk\\LotteryCard.spk"
#define SPKI_LOTTERY_CARD				_ROOT"spk\\LotteryCard.spki"

// Message Board
#define SPK_BULLETIN_BOARD				_ROOT"spk\\BulletinBoard.spk"

// Ousters Interface File
#define SPK_ASSEMBLE_BOX_OUSTERS		_ROOT"spk\\AssembleMessageBoxOusters.spk"
#define SPK_ASSEMBLE_BOX_BUTTON_OUSTERS	_ROOT"spk\\AssembleMessageBoxButtonOusters.spk"
#define SPK_OUSTERS_GAMEMENU			_ROOT"spk\\GameMenuOusters.spk"
#define SPK_OPTION_OUSTERS				_ROOT"spk\\OptionOusters.spk"
#define SPK_OUSTERS_MAIN				_ROOT"spk\\simpleinformation.spk"
#define SPK_OUSTERS_GEAR_SLOT			_ROOT"spk\\GearSlotOusters.spk"
#define SPK_OUSTERS_GEAR				_ROOT"spk\\GearOusters.spk"
#define SPK_OUSTERS_GEAR_SLOT			_ROOT"spk\\GearSlotOusters.spk"
#define SPK_OUSTERS_INVENTORY			_ROOT"spk\\InventoryOusters.spk"
#define SPK_OUSTERS_QUICKITEM			SPK_QUICKITEM_BAR
#define SPK_OUSTERS_PARTY				_ROOT"spk\\PartyOusters.spk"
#define SPK_OUSTERS_TEAM				_ROOT"spk\\TeamOusters.spk"
#define SPK_OUSTERS_TEAM_MEMBER			_ROOT"spk\\TeamMemberOusters.spk"
#define SPK_HPBAR_OUSTERS				_ROOT"spk\\HPBarOusters.spk"
#define SPK_MINIMAP_OUSTERS				_ROOT"spk\\MinimapOusters.spk"
#define SPK_CHATTING_OUSTERS			_ROOT"spk\\ChattingOusters.spk"
#define SPK_EFFECT_STATUS_OUSTERS		_ROOT"spk\\EffectStatusOusters.spk"
#define SPK_OUSTERS_SHOP_STORAGE		_ROOT"spk\\Shop&StorageOusters.spk"
#define SPK_OUSTERS_SYS_BUTTON			_ROOT"spk\\SysButtonOusters.spk"
#define SPK_OUSTERS_SKILL_ETC			_ROOT"spk\\SkillEtcOusters.spk"
#define SPK_EXCHANGE_OUSTERS			_ROOT"spk\\ExchangeOusters.spk"

// Mixing UI 2003.5.20 by gony
#define SPK_MIXING_FORGE				_ROOT"spk\\MixingForge.spk"

#if __CONTENTS(__PET_MIXINGFORGE)
#define SPK_PET_MIXING_FORGE			_ROOT"spk\\PetMixingForge.spk"
#endif //__PET_MIXINGFORGE

#define SPK_HORN						_ROOT"spk\\PortalEtcOusters.spk"//_ROOT"spk\\Horn.spk"
#define SPK_HORN_MAP					_ROOT"spk\\PortalMapOusters.spk"//_ROOT"spk\\HornMap.spk"

#define SPK_MAILBOX						_ROOT"spk\\MailBox.spk"

// CTF Status 2003.7.24 by sonee

#define SPK_CTF_STATUS					_ROOT"spk\\CTF.spk"

#define RPK_INFO						"data\\info\\infodata.rpk"
#define KOREAN_INTERFACE_INFO			"Interface.inf"
// #define CHINESE_INTERFACE_INFO			"ChinaInterface.inf"
#define JAPANESE_INTERFACE_INFO			"JapanInterface.inf"

// RegenTower 2003.10.14 by sonee

#define RPK_HOLYLAND_MINIMAP			_ROOT"spk\\HolylandMinimap.spk"
#define HELP_MESSAGE_DESC				_ROOT"spk\\HelpMessage.spk"

//2004, 5, 31 sobeit add start - sms message
#define SPK_SLAYER_SMS_MESSAGE			_ROOT"spk\\sms_slayer_main.spk"
#define SPK_VAMPIRE_SMS_MESSAGE			_ROOT"spk\\sms_vampire_main.spk"
#define SPK_OUSTERS_SMS_MESSAGE			_ROOT"spk\\sms_ousters_main.spk"
#define SPK_SLAYER_SMS_LIST			_ROOT"spk\\sms_slayer_list.spk"
#define SPK_VAMPIRE_SMS_LIST			_ROOT"spk\\sms_vampire_list.spk"
#define SPK_OUSTERS_SMS_LIST			_ROOT"spk\\sms_ousters_list.spk"
#define SPK_SLAYER_SMS_RECORD			_ROOT"spk\\sms_slayer_record.spk"
#define SPK_VAMPIRE_SMS_RECORD			_ROOT"spk\\sms_vampire_record.spk"
#define SPK_OUSTERS_SMS_RECORD			_ROOT"spk\\sms_ousters_record.spk"
#define SPK_SLAYER_SMS_SCROLL			_ROOT"spk\\sms_slayer_scroll.spk"
#define SPK_VAMPIRE_SMS_SCROLL			_ROOT"spk\\sms_vampire_scroll.spk"
#define SPK_OUSTERS_SMS_SCROLL			_ROOT"spk\\sms_ousters_scroll.spk"

//2004, 5, 31 sobeit add end - sms message

// 2004, 6, 21 sobeit add start 
#define SPK_SLAYER_NAMING			_ROOT"spk\\NamingSlayer.spk"
#define SPK_VAMPIRE_NAMING			_ROOT"spk\\NamingVampire.spk"
#define SPK_OUSTERS_NAMING			_ROOT"spk\\NamingOusters.spk"
// 2004, 6, 21 sobeit add end 
// 2004, 7, 2 sobeit add start 
#define SPK_SLAYER_QUESTMAMAGER			_ROOT"spk\\QuestManager_Slayer.spk"
#define SPK_VAMPIRE_QUESTMAMAGER		_ROOT"spk\\QuestManager_Vampire.spk"
#define SPK_OUSTERS_QUESTMAMAGER		_ROOT"spk\\QuestManager_Ousters.spk"
// 2004, 7, 2 sobeit add end

// 2004, 7, 9 sobeit add start
#define QUEST_XML_FILE					"SimpleGQuest.xml"
#define QUEST_EVENT_XML_FILE			"EventGQuest.xml"
// 2004, 7, 9 sobeit add end

// 2004, 7, 22 sobeit add start
#define SPK_QUEST_MODIFY						_ROOT"spk\\QuestModify.spk"
// 2004, 7, 22 sobeit add end

// 2004, 8, 2 sobeit add start
#define SPK_TITLE_OPTION						_ROOT"spk\\OptionTitle.spk"
#define SPK_TITLE_OPTION_1024						_ROOT"spk\\OptionTitle1024.spk"

#define SPK_TITLE_OPTION_SCROLL					_ROOT"spk\\OptionTitleScroll.spk"

#define SPK_PERSNALSHOP_SLAYER						   	_ROOT"spk\\SlayerPersnalShop.spk"
#define SPK_PERSNALSHOP_VAMPIRE							_ROOT"spk\\VampirePersnalShop.spk"
#define SPK_PERSNALSHOP_OUSTERS							_ROOT"spk\\OustersPersnalShop.spk"
// 2004, 8, 2 sobeit add end

#define SPK_PERSNALSHOP_SLAYER						   	_ROOT"spk\\SlayerPersnalShop.spk"
#define SPK_PERSNALSHOP_VAMPIRE							_ROOT"spk\\VampirePersnalShop.spk"
#define SPK_PERSNALSHOP_OUSTERS							_ROOT"spk\\OustersPersnalShop.spk"
// 2004, 12, 1, sobeit add start
#define SPK_SLAYER_POWERJJANG				_ROOT"spk\\Powerjjang_Slayer.spk"
#define SPK_VAMPIRE_POWERJJANG				_ROOT"spk\\Powerjjang_Vampire.spk"
#define SPK_OUSTERS_POWERJJANG				_ROOT"spk\\Powerjjang_Ousters.spk"

#define SPK_ITEM_POWERJJANG					_ROOT"spk\\Powerjjang_Item.spk"
#define SPK_SLAYER_ADVANCEMENTGEAR			_ROOT"spk\\GearSlayerAdvancement.spk"
#define SPK_VAMPIRE_ADVANCEMENTGEAR			_ROOT"spk\\myinformation.spk"
#define SPK_OUSTERS_ADVANCEMENTGEAR			_ROOT"spk\\GearOustersAdvancement.spk"

// 2004, 12, 1, sobeit add end

// 2004-12-13 added about advancement class, by sonee;
#define ISPK_ADVANCEMENT_VAMPIRE_MAN	"data\\Image\\ACVampireMan.ispk"
#define ISPK_ADVANCEMENT_VAMPIRE_WOMAN	"data\\Image\\ACVampireWoman.ispk"
#define ISPK_ADVANCEMENT_OUSTERS		"data\\Image\\ACOusters.ispk"
#define ISPK_ADVANCEMENT_SLAYER_MAN		"data\\Image\\ACSlayerMan.ispk"
#define ISPK_ADVANCEMENT_SLAYER_WOMAN	"data\\Image\\ACSlayerWoman.ispk"


#define ISPK_CREATURE_3	"data\\Image\\creature3.ispk"
#define SSPK_CREATURE_3	"data\\Image\\creature3.sspk"
// FAZ 4 (2026-05-07): Creature4-7 paketleri eklendi
// Pet sprite'ları büyük olasılıkla bu paketlerde. filedef.inf'te de entry var.
#define ISPK_CREATURE_4	"data\\Image\\creature4.ispk"
#define SSPK_CREATURE_4	"data\\Image\\creature4.sspk"
#define ISPK_CREATURE_5	"data\\Image\\creature5.ispk"
#define SSPK_CREATURE_5	"data\\Image\\creature5.sspk"
#define ISPK_CREATURE_6	"data\\Image\\creature6.ispk"
#define SSPK_CREATURE_6	"data\\Image\\creature6.sspk"
#define ISPK_CREATURE_7	"data\\Image\\creature7.ispk"
#define SSPK_CREATURE_7	"data\\Image\\creature7.sspk"

#define CFPK_ADVANCEMENT_SLAYER_MAN		"data\\Image\\ACSlayerMan.cfpk"
#define CFPK_ADVANCEMENT_SLAYER_WOMAN	"data\\Image\\ACSlayerWoman.cfpk"
#define CFPK_ADVANCEMENT_VAMPIRE_MAN	"data\\Image\\ACVampireMan.cfpk"
#define CFPK_ADVANCEMENT_VAMPIRE_WOMAN	"data\\Image\\ACVampireWoman.cfpk"
#define CFPK_ADVANCEMENT_OUSTERS		"data\\Image\\ACOusters.cfpk"

// comeback event
#define TXT_TUTORIAL_COMEBACKEVENT_NOTICE		"ComeBackEventNotice.txt"
#define TXT_TUTORIAL_COMEBACKEVENT_PAY			"ComeBackEventPay.txt"
#define TXT_TUTORIAL_COMEBACKEVENT_RECOMMEND	"ComeBackEventRecommend.txt"

// sub inventory
#define SPK_SLAYER_INVENTORY_SUB			_ROOT"spk\\InventorySlayerSub.spk"
#define SPK_VAMPIRE_INVENTORY_SUB			_ROOT"spk\\InventoryVampireSub.spk"
#define SPK_OUSTERS_INVENTORY_SUB			_ROOT"spk\\InventoryOustersSub.spk"

// BloodBurst UI File
#define SPK_BLOOD_BRUST_SLAYER				_ROOT"spk\\BloodBurstSlayer.spk"
#define SPK_BLOOD_BRUST_VAMPIRE				_ROOT"spk\\bloodburstrenewal.spk"
#define SPK_BLOOD_BRUST_OUSTERS				_ROOT"spk\\BloodBurstOusters.spk"

#define SPK_WEBBROWSER						_ROOT"spk\\WebBrowser.spk"

// ComboCount  File
#define SPK_COMBO_CNT						_ROOT"spk\\ComboCnt.spk"
#define SPK_TAXRATE							_ROOT"spk\\TaxRate.spk"

// World Map File
#define SPK_WORLD_MAP						_ROOT"spk\\WorldMap.spk"
#define SPK_WORLD_MAP_ICON					_ROOT"spk\\WorldMapIcon.spk"

// Time Count
#define SPK_TIME_COUNT						_ROOT"spk\\TimeCount.spk"

#define SPK_TIPOJYU_SLAYER					_ROOT"spk\\TipojyuDlg_Slayer.spk"

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
#define SPK_PREMIUM_GIVE_ITEM_SLAYER		_ROOT"spk\\new\\premiumgiveitem_renewal\\00000.spk" // \spk\new\premiumgiveitem_renewal
#define SPK_PREMIUM_GIVE_ITEM_VAMPIRE		_ROOT"spk\\new\\premiumgiveitem_renewal\\00000.spk" // \spk\new\premiumgiveitem_renewal
#define SPK_PREMIUM_GIVE_ITEM_OUSTERS		_ROOT"spk\\new\\premiumgiveitem_renewal\\00000.spk" // \spk\new\premiumgiveitem_renewal
#endif //__PREMIUM_GIVE_ITEM_UI

#if __CONTENTS(__FRIEND_ADDITION)
#define SPK_PLAYER_POPUP					_ROOT"spk\\characterpopupmenu.spk"
#define SPK_FRIEND_COMMON					_ROOT"spk\\friendcommon.spk"
#define SPK_FRIEND_SYSTEM					_ROOT"spk\\friendsystem.spk"
#define SPK_FRIEND_INFO						_ROOT"spk\\friendinfo.spk"
#define SPK_FRIEND_WAIT						_ROOT"spk\\friendwait.spk"
#endif //__FRIEND_ADDITION
#define SPK_NEW_HPBAR				_ROOT"spk\\hpmprenewal.spk"
#define SPK_NEW_NPC_DIALOG						_ROOT"spk\\npcdialog.spk"
#define SPK_NPC_ICON						_ROOT"spk\\npcmenu_normal.spk"
#define SPK_MONSTER_LEVEL						_ROOT"spk\\monsterlevel.spk"
#endif
