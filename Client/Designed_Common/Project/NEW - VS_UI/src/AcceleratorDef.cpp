
//----------------------------------------------------------------------
// AcceleratorDef.cpp
//----------------------------------------------------------------------

#include "client_PCH.h"
#include "KeyAccelerator.h"
#include "CDirectInput.h"
#include "mgamestringtable.h"

//#include "VS_UI_Widget.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//�ɼ� â���� ���̴� ���ڰ� �̰ɷ� ǥ�õ�

#if __CONTENTS(__080405_FIREST_UI_UPDATE)
const char* ACCELERATOR_NAME[MAX_ACCELERATOR][MAX_ACCELERATOR_NAME_TYPE] =
{
	"AcceleratorNULL",	"None",
	"F1",				"F1",
	"F2",				"F2",
	"F3",				"F3",
	"F4",				"F4",
	"F5",				"F5",
	"F6",				"F6",
	"F7",				"F7",
	"F8",				"F8",
	"F9",				"F9",
	"F10",				"F10",
	"F11",				"F11",
	"F12",				"F12",
	"ESC",				"Close Window",
	"SKILL",			"Skills",
	"INVENTORY",		"Inventory",
	"GEAR",				"Gear Windows",
	"CHARINFO",			"Character Info Window",
	"SKILLINFO",		"Skill Info Windows",
	"GRADE1",			"Rank Information",
	"WORLDMAP",			"World Map",
	"PARTY",			"Party",	
	"HELP",				"Help",
	"QUICKITEM_SLOT",	"Quick Slot",
	"EXTEND_CHAT",		"Chat Window Expansion",
	"CHAT_UP",			"Switching Chat Modes",
	"CHAT_DN",			"Switching Chat Modes",
	"NOMAL_CHAT",		"General Chat",
	"ZONE_CHAT",		"Zone Chat",
	"WHISPER_CHAT",		"1:1",
	"PARTY_CHAT",		"Party Chat",
	"GUILD_CHAT",		"Guild Chat",
	"UNION_CHAT",		"Union Chat",
	"QUEST",			"Quest Info Window",
	"MAILBOX",			"Mail Box",
	"PETINFO",			"Pet Info Window",
	"SUMMONPET",		"Summon Pet",
	"TRIBE_PRIOR",		"Menu Conversion (Left)",
	"TRIBE_NEXT",		"Menu Conversion (Right)",
//	"GUILD_INFO",		"�������",
//	"GUILD_MEMBER_LIST","��������Ʈ",
	"COERCION_ATTACK",	"Forced Attack",
	"AUTO_ATTACK"		"OneClick Attack"
	"SCREEN SHOT",		"Take Screen Shot",
	"MINIMAP",			"Minimap",
//	"INTERFACE_INIT",	"INTERFACE_INIT",
	#if __CONTENTS(__GEAR_SWAP_CHANGE)  //�ɼǿ� ����Ű ��� �̸� 	"GEAR_CHANGE"
	"GEAR_CHANGE",		"Gear Window",
	"GEAR_CHANGE_SHOW",	"Gear Window/Change",
	#endif //__GEAR_SWAP_CHANGE
};
#else
const char* ACCELERATOR_NAME[MAX_ACCELERATOR] =
{
	"AcceleratorNULL",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"ESC",
	"SKILL",
	"INVENTORY",
	"GEAR",
	"CHARINFO",
	"SKILLINFO",
	"GRADE1",
	"WORLDMAP",
	"PARTY",
	"HELP",
	"QUICKITEM_SLOT",
	"EXTEND_CHAT",
	"CHAT_UP",
	"CHAT_DN",
	"NOMAL_CHAT",
	"ZONE_CHAT",
	"WHISPER_CHAT",
	"PARTY_CHAT",
	"GUILD_CHAT",
	"UNION_CHAT",
	"QUEST",
	"MAILBOX",
	"PETINFO",
	"SUMMONPET",
	"TRIBE_PRIOR",
	"TRIBE_NEXT",
//	"GUILD_INFO",
//	"GUILD_MEMBER_LIST",
	"COERCION_ATTACK",
	"AUTO_ATTACK",
	"SCREEN SHOT",
	"MINIMAP",
//	"INTERFACE_INIT",
#if __CONTENTS(__GEAR_SWAP_CHANGE)	//�ɼǿ� ����Ű ��� �̸� 	"GEAR_CHANGE"
	"GEAR_CHANGE",
	"GEAR_CHANGE_SHOW",
#endif // __GEAR_SWAP_CHANGE
};
#endif //__080405_FIREST_UI_UPDATE

//----------------------------------------------------------------------
// Set DefaultAccelerator
//----------------------------------------------------------------------
// default file�� �����صΰ� load�ϴ� �͵� �������� ����.
//----------------------------------------------------------------------
void	
SetDefaultAccelerator()
{
	const WORD keys[MAX_ACCELERATOR] =  
	{
		0,														// ACCEL_NULL,
		DIK_F1,													// ACCEL_F1
		DIK_F2,													// ACCEL_F2
		DIK_F3,													// ACCEL_F3
		DIK_F4,													// ACCEL_F4
		DIK_F5,													// ACCEL_F5
		DIK_F6,													// ACCEL_F6
		DIK_F7,													// ACCEL_F7
		DIK_F8,													// ACCEL_F8
		DIK_F9,													// ACCEL_F9
		DIK_F10,												// ACCEL_F10
		DIK_F11,												// ACCEL_F11
		DIK_F12,												// ACCEL_F12
		DIK_ESCAPE,												// ACCEL_ESC
		DIK_S,													// ACCEL_SKILL
		// 2026-05-22: Modern MMORPG mapping — I = Inventory, TAB = CharInfo (önceki swap).
		// Phoenix default (eski K-MMO): TAB = Inventory, I = CharInfo idi. Kullanıcı
		// I tuşu Inventory açmasını bekliyor (modern alışkanlık). Swap uygulandı.
		DIK_I,													// ACCEL_INVENTORY
		ACCEL_MAKE_KEY( DIK_TAB, FLAG_ACCELERATOR_CONTROL ),	// ACCEL_GEAR (Ctrl+TAB)
		DIK_TAB,												// ACCEL_CHARINFO
		DIK_K,													// ACCEL_SKILLINFO
		DIK_R,													// ACCEL_GRADE2INFO -> GRADE1
		DIK_M,													// ACCEL_WORLDMAP
		DIK_P,													// ACCEL_PARTY
		DIK_H,													// ACCEL_HELP
		DIK_GRAVE,												// ACCEL_QUICKITEM_SLOT
		DIK_E,													// ACCEL_EXTEND_CHAT
		ACCEL_MAKE_KEY( DIK_PRIOR, FLAG_ACCELERATOR_CONTROL ),	// ACCEL_CHAT(UP ���)
		ACCEL_MAKE_KEY( DIK_NEXT, FLAG_ACCELERATOR_CONTROL ),	// ACCEL_CHAT_DN
		DIK_C,													// ACCEL_GRADE1INFO -> NORMALCHAT
		DIK_Z,													// ACCEL_ZONECHAT
		DIK_W,													// ACCEL_WHISPER
		DIK_A,													// ACCEL_GRADE3INFO -> PARTYCHAT
		DIK_G,													// ACCEL_GUILD
		DIK_U,													// ACCEL_UNION
		DIK_Q,													// ACCEL_QUEST
		DIK_B,													// ACCEL_MAILBOX
		ACCEL_MAKE_KEY( DIK_SPACE, FLAG_ACCELERATOR_ALT ),		// ACCEL_PETINFO
		ACCEL_MAKE_KEY( DIK_SPACE, FLAG_ACCELERATOR_CONTROL ),	// ACCEL_SUMMON_PET
		ACCEL_MAKE_KEY( DIK_PRIOR, FLAG_ACCELERATOR_ALT ),		// ACCEL_TRIBE_PRIOR
		ACCEL_MAKE_KEY( DIK_NEXT, FLAG_ACCELERATOR_ALT ),		// ACCEL_TRIBE_NEXT
//		DIK_V,													// ACCEL_GUILD_INFO
//		DIK_L,													// ACCEL_GUILD_MEMBER_LIST
		DIK_O,													// ACCEL_COER_ATTACK
		DIK_J,													// ACCEL_AUTO_ATTACK
		DIK_SYSRQ,												// ACCEL_SYSRQ
		DIK_N,													// ACCEL_MINIMAP
//		ACCEL_MAKE_KEY( DIK_SLASH, FLAG_ACCELERATOR_CONTROL ),	// ACCEL_SLASH
#if __CONTENTS(__GEAR_SWAP_CHANGE)	//����Ű ���
		ACCEL_MAKE_KEY( DIK_A, FLAG_ACCELERATOR_ALT ),			// ACCEL_GEAR_CHANGE
		ACCEL_MAKE_KEY( DIK_S, FLAG_ACCELERATOR_ALT ),			// ACCEL_GEAR_CHANGE_SHOW
#endif	// __GEAR_SWAP_CHANGE
	};

	for (int accel=1; accel<MAX_ACCELERATOR; accel++)
	{
		g_pKeyAccelerator->SetAcceleratorKey( accel, keys[accel] );
	}
}

