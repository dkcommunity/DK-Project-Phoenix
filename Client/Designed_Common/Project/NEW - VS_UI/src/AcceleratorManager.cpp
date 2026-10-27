//-----------------------------------------------------------------------------
// AcceleratorManager.cpp
//-----------------------------------------------------------------------------

#include "client_PCH.h"
#include "AcceleratorManager.h"
#include "AcceleratorDef.h"
#include "VS_UI.h"

//#include "Packet\Cpackets\CGChangeGearSlot.h"

//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------
AcceleratorManager*		g_pAcceleratorManager = NULL;
//extern DWORD			g_GearSlotID	= 0;
//-----------------------------------------------------------------------------
//
// constructor
//
//-----------------------------------------------------------------------------
AcceleratorManager::AcceleratorManager()
{
	MFunctionManager::Init( MAX_ACCELERATOR );

	MFunctionManager::SetFunction(ACCEL_F1, Function_ACCEL_F1);
	MFunctionManager::SetFunction(ACCEL_F2, Function_ACCEL_F2);
	MFunctionManager::SetFunction(ACCEL_F3, Function_ACCEL_F3);
	MFunctionManager::SetFunction(ACCEL_F4, Function_ACCEL_F4);
	MFunctionManager::SetFunction(ACCEL_F5, Function_ACCEL_F5);
	MFunctionManager::SetFunction(ACCEL_F6, Function_ACCEL_F6);
	MFunctionManager::SetFunction(ACCEL_F7, Function_ACCEL_F7);
	MFunctionManager::SetFunction(ACCEL_F8, Function_ACCEL_F8);
	MFunctionManager::SetFunction(ACCEL_F9, Function_ACCEL_F9);
	MFunctionManager::SetFunction(ACCEL_F10, Function_ACCEL_F10);
	MFunctionManager::SetFunction(ACCEL_F11, Function_ACCEL_F11);
	MFunctionManager::SetFunction(ACCEL_F12, Function_ACCEL_F12);
	MFunctionManager::SetFunction(ACCEL_ESC, Function_ACCEL_ESC);
	MFunctionManager::SetFunction(ACCEL_SKILL, Function_ACCEL_SKILL);
	MFunctionManager::SetFunction(ACCEL_INVENTORY, Function_ACCEL_INVENTORY);
	MFunctionManager::SetFunction(ACCEL_GEAR, Function_ACCEL_GEAR);
	MFunctionManager::SetFunction(ACCEL_CHARINFO, Function_ACCEL_CHARINFO);
	MFunctionManager::SetFunction(ACCEL_SKILLINFO, Function_ACCEL_SKILLINFO);
	MFunctionManager::SetFunction(ACCEL_GRADE1INFO, Function_ACCEL_GRADE1INFO);
	MFunctionManager::SetFunction(ACCEL_WORLDMAP, Function_ACCEL_WORLDMAP);
	MFunctionManager::SetFunction(ACCEL_PARTY, Function_ACCEL_PARTY);
	MFunctionManager::SetFunction(ACCEL_HELP, Function_ACCEL_HELP);
	MFunctionManager::SetFunction(ACCEL_QUICKITEM_SLOT, Function_ACCEL_QUICKITEM_SLOT);
	MFunctionManager::SetFunction(ACCEL_EXTEND_CHAT, Function_ACCEL_EXTEND_CHAT);
	MFunctionManager::SetFunction(ACCEL_CHAT, Function_ACCEL_CHAT_UP);
	MFunctionManager::SetFunction(ACCEL_CHAT_DN, Function_ACCEL_CHAT_DN);
	MFunctionManager::SetFunction(ACCEL_NORMALCHAT, Function_ACCEL_NORMALCHAT);
	MFunctionManager::SetFunction(ACCEL_ZONECHAT, Function_ACCEL_ZONECHAT);
	MFunctionManager::SetFunction(ACCEL_WHISPER, Function_ACCEL_WHISPER);
	MFunctionManager::SetFunction(ACCEL_PARTYCHAT, Function_ACCEL_PARTYCHAT);
	MFunctionManager::SetFunction(ACCEL_GUILD, Function_ACCEL_GUILD);
	MFunctionManager::SetFunction(ACCEL_UNION, Function_ACCEL_UNION);
//	MFunctionManager::SetFunction(ACCEL_GRADE2INFO, Function_ACCEL_GRADE2INFO);
//	MFunctionManager::SetFunction(ACCEL_GRADE3INFO, Function_ACCEL_GRADE3INFO);
	MFunctionManager::SetFunction(ACCEL_QUEST, Function_ACCEL_QUEST);
	MFunctionManager::SetFunction(ACCEL_MAILBOX, Function_ACCEL_MAILBOX);
	// 2004, 5, 17 sobeit add start
	MFunctionManager::SetFunction(ACCEL_PETINFO, Function_ACCEL_PETINFO);
	MFunctionManager::SetFunction(ACCEL_SUMMON_PET, Function_ACCEL_SUMMON_PET);
	// 2004, 5, 17 sobeit add end

	MFunctionManager::SetFunction(ACCEL_TRIBE_PRIOR, Function_ACCEL_TRIBE_PRIOR);
	MFunctionManager::SetFunction(ACCEL_TRIBE_NEXT, Function_ACCEL_TRIBE_NEXT);
//	MFunctionManager::SetFunction(ACCEL_GUILD_INFO, Function_ACCEL_GUILD_INFO);
//	MFunctionManager::SetFunction(ACCEL_GUILD_MEMBER_LIST, Function_ACCEL_GUILD_MEMBER_LIST);
	MFunctionManager::SetFunction(ACCEL_COER_ATTACK, Function_ACCEL_COER_ATTACK);
#if __CONTENTS(__AUTO_ATTACT)	//__AUTO_ATTACT
	MFunctionManager::SetFunction(ACCEL_AUTO_ATTACK, Function_ACCEL_AUTO_ATTACK);
#endif //__AUTO_ATTACT
	MFunctionManager::SetFunction(ACCEL_SYSRQ, Function_ACCEL_SYSRQ);
	MFunctionManager::SetFunction(ACCEL_MINIMAP, Function_ACCEL_MINIMAP);
//	MFunctionManager::SetFunction(ACCEL_SLASH, Function_ACCEL_SLASH);
// 	MFunctionManager::SetFunction(ACCEL_EVERYINVENTORYOPEN, Function_ACCEL_EVERYINVENTORYOPEN);
#if __CONTENTS(__GEAR_SWAP_CHANGE)	//AcceleratorManager::AcceleratorManager() ����Ű ���� �Լ�
	MFunctionManager::SetFunction(ACCEL_GEAR_CHANGE, Function_ACCEL_GEAR_CHANGE);
	MFunctionManager::SetFunction(ACCEL_GEAR_CHANGE_SHOW, Function_ACCEL_GEAR_CHANGE_SHOW);	
#endif
}

AcceleratorManager::~AcceleratorManager()
{
}

//-----------------------------------------------------------------------------
// ACCEL_F1
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F1(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(0) ;
	//gC_vs_ui.HotKey_F1();
}

//-----------------------------------------------------------------------------
// ACCEL_F2
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F2(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(1) ;
	//gC_vs_ui.HotKey_F2();
}

//-----------------------------------------------------------------------------
// ACCEL_F3
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F3(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(2) ;
	//gC_vs_ui.HotKey_F3();
}

//-----------------------------------------------------------------------------
// ACCEL_F4
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F4(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(3) ;
	//gC_vs_ui.HotKey_F4();
}

//-----------------------------------------------------------------------------
// ACCEL_F5
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F5(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(4) ;
	//gC_vs_ui.HotKey_F5();
}

//-----------------------------------------------------------------------------
// ACCEL_F6
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F6(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(5) ;
	//gC_vs_ui.HotKey_F6();
}

//-----------------------------------------------------------------------------
// ACCEL_F7
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F7(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(6) ;
	//gC_vs_ui.HotKey_F7();
}

//-----------------------------------------------------------------------------
// ACCEL_F8
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F8(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(7) ;
	//gC_vs_ui.HotKey_F8();
}

//-----------------------------------------------------------------------------
// ACCEL_F9
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F9(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(8) ;
	//gC_vs_ui.HotKey_F9();
}

//-----------------------------------------------------------------------------
// ACCEL_F10
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F10(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(9) ;
	//gC_vs_ui.HotKey_F10();
}

//-----------------------------------------------------------------------------
// ACCEL_F11
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F11(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(10) ;
	//gC_vs_ui.HotKey_F11();
}

//-----------------------------------------------------------------------------
// ACCEL_F12
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_F12(void* pVoid)
{
	gC_vs_ui.Hotkey_Function(11) ;
	//gC_vs_ui.HotKey_F12();
}

//-----------------------------------------------------------------------------
// ACCEL_ESC
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_ESC(void* pVoid)
{
	gC_vs_ui.HotKey_ESC();
}

//-----------------------------------------------------------------------------
// ACCEL_SKILL
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_SKILL(void* pVoid)
{
	gC_vs_ui.HotKey_Skill();
}

//-----------------------------------------------------------------------------
// ACCEL_INVENTORY
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_INVENTORY(void* pVoid)
{
	gC_vs_ui.HotKey_Inventory(true);
}

//-----------------------------------------------------------------------------
// ACCEL_GEAR
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_GEAR(void* pVoid)
{
	gC_vs_ui.HotKey_Gear();
}

//-----------------------------------------------------------------------------
// ACCEL_CHARINFO
//-----------------------------------------------------------------------------
// FAZ 4 (2026-05-11) Bug 1 fix — I tusu Character Info penceresini acmiyordu.
// Eski kod: gC_vs_ui.HotKey_Gear() — bu YANLIS, Gear (envanter cihazi)
// penceresini aciyordu, Character Info'yu degil. AcceleratorDef.cpp:155 default
// olarak DIK_I -> ACCEL_CHARINFO mapliyor. CharInfo icin HotKey_CharInfo()
// dogru cagri (VS_UI_GameCommon.cpp:1795). 3 irk paritesi: CharInfo race-bagimsiz.
void
AcceleratorManager::Function_ACCEL_CHARINFO(void* pVoid)
{
	gC_vs_ui.HotKey_Gear();
}

//-----------------------------------------------------------------------------
// ACCEL_SKILLINFO
//-----------------------------------------------------------------------------
// FAZ 4 (2026-05-11) Bug 1 fix — K tusu Skill Info penceresini acmiyordu.
// Eski kod: gC_vs_ui.ToggleMilitary() — bu YANLIS, Military (Guild Military)
// penceresini aciyordu, Skill Info'yu degil. AcceleratorDef.cpp:156 default
// olarak DIK_K -> ACCEL_SKILLINFO mapliyor. Skill Info icin HotKey_SkillInfo()
// dogru cagri (Vs_ui.cpp:223). 3 irk paritesi: SkillInfo race-bagimsiz.
void
AcceleratorManager::Function_ACCEL_SKILLINFO(void* pVoid)
{
	gC_vs_ui.HotKey_SkillInfo();
}

//-----------------------------------------------------------------------------
// ACCEL_WORLDMAP
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_WORLDMAP(void* pVoid)
{
	gC_vs_ui.HotKey_WorldMap();
}

//-----------------------------------------------------------------------------
// ACCEL_PARTY
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_PARTY(void* pVoid)
{
	gC_vs_ui.HotKey_Party();
}

//-----------------------------------------------------------------------------
// ACCEL_MARK
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_MARK(void* pVoid)
{
	gC_vs_ui.HotKey_Mark();
}

//-----------------------------------------------------------------------------
// ACCEL_HELP
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_HELP(void* pVoid)
{
	gC_vs_ui.HotKey_Help();
}

//-----------------------------------------------------------------------------
// ACCEL_QUICKITEM_SLOT
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_QUICKITEM_SLOT(void* pVoid)
{
	gC_vs_ui.HotKey_QuickItemSlot();
}

//-----------------------------------------------------------------------------
// ACCEL_EXTEND_CHAT
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_EXTEND_CHAT(void* pVoid)
{
	gC_vs_ui.HotKey_ExtendChat();
}

//-----------------------------------------------------------------------------
// ACCEL_CHAT
//-----------------------------------------------------------------------------
void		
AcceleratorManager::Function_ACCEL_CHAT_UP(void* pVoid)
{
	gC_vs_ui.HotKey_Chat_Up();
}
void		
AcceleratorManager::Function_ACCEL_CHAT_DN(void* pVoid)
{
	gC_vs_ui.HotKey_Chat_Dn();
}


void		
AcceleratorManager::Function_ACCEL_NORMALCHAT(void* pVoid)
{
	gC_vs_ui.HotKey_NormalChat();
}
void		
AcceleratorManager::Function_ACCEL_ZONECHAT(void* pVoid)
{
	gC_vs_ui.HotKey_ZoneChat();
}
void		
AcceleratorManager::Function_ACCEL_WHISPER(void* pVoid)
{
	gC_vs_ui.HotKey_Whisper();
}
void		
AcceleratorManager::Function_ACCEL_PARTYCHAT(void* pVoid)
{
	gC_vs_ui.HotKey_PartyChat();
}
void		
AcceleratorManager::Function_ACCEL_GUILD(void* pVoid)
{
	gC_vs_ui.HotKey_GuildChat();
}
void		
AcceleratorManager::Function_ACCEL_UNION(void* pVoid)
{
	gC_vs_ui.HotKey_UnionChat();
}
//-----------------------------------------------------------------------------
// ACCEL_GRADE
//-----------------------------------------------------------------------------
void
AcceleratorManager::Function_ACCEL_GRADE1INFO(void* pVoid)
{
	gC_vs_ui.HotKey_Grade1Info();
}


//-----------------------------------------------------------------------------
// ACCEL_QUEST
//-----------------------------------------------------------------------------
void
AcceleratorManager::Function_ACCEL_QUEST(void* pVoid)
{
	gC_vs_ui.HotKey_Quest();
}

//-----------------------------------------------------------------------------
// ACCEL_MAILBOX
//-----------------------------------------------------------------------------
void
AcceleratorManager::Function_ACCEL_MAILBOX(void* pVoid)
{
	gC_vs_ui.HotKey_MailBox();
}

//-----------------------------------------------------------------------------
// ACCEL_MAILBOX
//-----------------------------------------------------------------------------
void
AcceleratorManager::Function_ACCEL_PETINFO(void* pVoid)
{
	gpC_base->SendMessage(UI_PLAYER_PETINFO); 
	gC_vs_ui.HotKey_PetInfo();
}
//////
//// 2004, 5, 17 sobeit add start
////-----------------------------------------------------------------------------
//// ACCEL_MAILBOX
////-----------------------------------------------------------------------------
void
AcceleratorManager::Function_ACCEL_SUMMON_PET(void* pVoid)
{
	gC_vs_ui.HotKey_SummonPet();
}
 //2004, 5, 17 sobeit add start
void
AcceleratorManager::Function_ACCEL_TRIBE_PRIOR(void* pVoid)
{
	gC_vs_ui.HotKey_Tribe_Prior();
}

void
AcceleratorManager::Function_ACCEL_TRIBE_NEXT(void* pVoid)
{
	gC_vs_ui.HotKey_Tribe_Next();
}

/*void
AcceleratorManager::Function_ACCEL_GRADE2INFO(void* pVoid)
{
	gC_vs_ui.HotKey_Grade2Info();
}*/

/*void
AcceleratorManager::Function_ACCEL_GRADE3INFO(void* pVoid)
{
	gC_vs_ui.HotKey_Grade3Info();
}*/

/*void
AcceleratorManager::Function_ACCEL_GUILD_INFO(void* pVoid)
{
	gC_vs_ui.HotKey_Guild_Info();
}*/

/*void
AcceleratorManager::Function_ACCEL_GUILD_MEMBER_LIST(void* pVoid)
{
	gC_vs_ui.HotKey_Guild_Member_List();
}*/

void 
AcceleratorManager::Function_ACCEL_COER_ATTACK(void* pVoid)
{
	gC_vs_ui.HotKey_Coer_Attack();
}

#if __CONTENTS(__AUTO_ATTACT)	//__AUTO_ATTACT
void 
AcceleratorManager::Function_ACCEL_AUTO_ATTACK(void* pVoid)
{
	gC_vs_ui.HotKey_Auto_Attack();
}
#endif  //__AUTO_ATTACT
void
AcceleratorManager::Function_ACCEL_SYSRQ(void* pVoid)
{
	gC_vs_ui.HotKey_Sysrq();
}

/*
void
AcceleratorManager::Function_ACCEL_SLASH(void* pVoid)
{
	gC_vs_ui.HotKey_Slash();
}
*/
void
AcceleratorManager::Function_ACCEL_MINIMAP(void* pVoid)
{
	gC_vs_ui.HotKey_Minimap();
}

//void AcceleratorManager::Function_ACCEL_EVERYINVENTORYOPEN(void* pVoid)
//{
//	gC_vs_ui.HotKey_EveryInventoryOpen();
// }

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// AcceleratorManager::Function_ACCEL_GEAR_CHANGE ����Ű�� ȣ��Ǵ� �Լ�
void
AcceleratorManager::Function_ACCEL_GEAR_CHANGE(void* pVoid)
{
	gC_vs_ui.HotKey_GearChange();
}

void
AcceleratorManager::Function_ACCEL_GEAR_CHANGE_SHOW(void* pVoid)
{
	gC_vs_ui.HotKey_GearChangeShow();
}
#endif //__GEAR_SWAP_CHANGE