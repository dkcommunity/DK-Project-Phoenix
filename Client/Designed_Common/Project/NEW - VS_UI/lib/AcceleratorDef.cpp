
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
//옵션 창에서 보이는 글자가 이걸로 표시됨

#if __CONTENTS(__080405_FIREST_UI_UPDATE)
const char* ACCELERATOR_NAME[MAX_ACCELERATOR][MAX_ACCELERATOR_NAME_TYPE] =
{
	"AcceleratorNULL",		"None",
	"F1",					"F1",
	"F2",					"F2",
	"F3",					"F3",
	"F4",					"F4",
	"F5",					"F5",
	"F6",					"F6",
	"F7",					"F7",
	"F8",					"F8",
	"F9",					"F9",
	"F10",				"F10",
	"F11",				"F11",
	"F12",				"F12",
	"ESC",				"Close",
	"SKILL",				"Skill",
	"INVENTORY",			"Inventory",
	"GEAR",				"Gear",
	"CHARINFO",			"Character info",
	"SKILLINFO",			"Skills info",
	"GRADE1",			"Rank info",
	"WORLDMAP",			"World map",
	"PARTY",				"Party",	
	"HELP",				"Help",
	"QUICKITEM_SLOT",	"Quickitem slot",
	"EXTEND_CHAT",		"Expand chat",
	"CHAT_UP",			"Chat up",
	"CHAT_DN",			"Chat down",
	"NOMAL_CHAT",		"General Chat",
	"ZONE_CHAT",		"Zone chat",
	"WISPER_CHAT",		"Whisper chat",
	"PARTY_CHAT",		"Party chat",
	"GUILD_CHAT",		"Guild chat",
	"UNION_CHAT",		"Union chat",
	"QUEST",				"Quest",
	"MAILBOX",			"Mailbox",
	"PETINFO",			"Pet information",
	"SUMMONPET",		"Summon pet",
	"TRIBE_PRIOR",		"Tribe prior",
	"TRIBE_NEXT",		"Tribe next",
//	"GUILD_INFO",		"Guild Info",
//	"GUILD_MEMBER_LIST","Guild Member List",
	"COERCION_ATTACK",	"Coercion Attack",
// by svi
	"AUTO_ATTACK",		"OneClick Attack",
// end
	"SCREEN SHOT",		"Screenshot",
	"MINIMAP",			"Minimap",
//	"INTERFACE_INIT",	"INTERFACE_INIT",
	#if __CONTENTS(__GEAR_SWAP_CHANGE)  //옵션에 단축키 등록 이름 	"GEAR_CHANGE"
	"GEAR_CHANGE",		"Gear Change",
	"GEAR_CHANGE_SHOW",	"Gear Change2",
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
	"WISPER_CHAT",
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
// by svi
	"AUTO_ATTACK",
// end	
	"SCREEN SHOT",
	"MINIMAP",
//	"INTERFACE_INIT",
#if __CONTENTS(__GEAR_SWAP_CHANGE)  //옵션에 단축키 등록 이름 	"GEAR_CHANGE"
	"GEAR_CHANGE",
	"GEAR_CHANGE_SHOW",
#endif //__GEAR_SWAP_CHANGE
};
#endif //__080405_FIREST_UI_UPDATE

//----------------------------------------------------------------------
// Set DefaultAccelerator
//----------------------------------------------------------------------
// default file을 생성해두고 load하는 것도 괜찮을거 같다.
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
		DIK_TAB,												// ACCEL_INVENTORY
		ACCEL_MAKE_KEY( DIK_TAB, FLAG_ACCELERATOR_CONTROL ),	// ACCEL_GEAR
		DIK_I,													// ACCEL_CHARINFO
		DIK_K,													// ACCEL_SKILLINFO
		DIK_R,													// ACCEL_GRADE2INFO -> GRADE1
		DIK_M,													// ACCEL_WORLDMAP
		DIK_P,													// ACCEL_PARTY
		DIK_H,													// ACCEL_HELP
		DIK_GRAVE,												// ACCEL_QUICKITEM_SLOT
		DIK_E,													// ACCEL_EXTEND_CHAT
		ACCEL_MAKE_KEY( DIK_PRIOR, FLAG_ACCELERATOR_CONTROL ),	// ACCEL_CHAT(UP 토글)
		ACCEL_MAKE_KEY( DIK_NEXT, FLAG_ACCELERATOR_CONTROL ),	// ACCEL_CHAT_DN
		DIK_C,													// ACCEL_GRADE1INFO -> NORMALCHAT
		DIK_Z,													// ACCEL_ZONECHAT
		DIK_W,													// ACCEL_WISHPER
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
// by svi
		DIK_J,													// ACCEL_AUTO_ATTACK
// end		
		DIK_SYSRQ,												// ACCEL_SYSRQ
		DIK_N,													// ACCEL_MINIMAP
//		ACCEL_MAKE_KEY( DIK_SLASH, FLAG_ACCELERATOR_CONTROL ),	// ACCEL_SLASH
#if __CONTENTS(__GEAR_SWAP_CHANGE) //단축키 등록
		ACCEL_MAKE_KEY( DIK_A, FLAG_ACCELERATOR_ALT ),			// ACCEL_GEAR_CHANGE
		ACCEL_MAKE_KEY( DIK_S, FLAG_ACCELERATOR_ALT ),			// ACCEL_GEAR_CHANGE_SHOW
#endif // __GEAR_SWAP_CHANGE;
	};

	for (int accel=1; accel<MAX_ACCELERATOR; accel++)
	{
		g_pKeyAccelerator->SetAcceleratorKey( accel, keys[accel] );
	}
}

