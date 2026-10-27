#ifndef __ITEMCLASSDEF_H__
#define __ITEMCLASSDEF_H__

#include "ContentsFilter.h"

//------------------------------------------------------
// Item class 설정.
//------------------------------------------------------
// (!) 이거 바뀌면 
//     s_NewItemClassTable의 순서도 바꿔야 한다.!
//------------------------------------------------------
enum ITEM_CLASS
{
	ITEM_CLASS_MOTORCYCLE,			// 0
	ITEM_CLASS_POTION,				// 1
	ITEM_CLASS_WATER,				// 2
	ITEM_CLASS_HOLYWATER,			// 3
	ITEM_CLASS_MAGAZINE,			// 4
	ITEM_CLASS_BOMB_MATERIAL,		// 5
	ITEM_CLASS_ETC,					// 6
	ITEM_CLASS_KEY,					// 7
	ITEM_CLASS_RING,				// 8
	ITEM_CLASS_BRACELET,			// 9
	ITEM_CLASS_NECKLACE,			// 10
	ITEM_CLASS_COAT,				// 11
	ITEM_CLASS_TROUSER,				// 12
	ITEM_CLASS_SHOES,				// 13
	ITEM_CLASS_SWORD,				// 14
	ITEM_CLASS_BLADE,				// 15
	ITEM_CLASS_SHIELD,				// 16
	ITEM_CLASS_CROSS,				// 17
	ITEM_CLASS_GLOVE,				// 18
	ITEM_CLASS_HELM,				// 19
	ITEM_CLASS_SG,					// 20
	ITEM_CLASS_SMG,					// 21
	ITEM_CLASS_AR,					// 22
	ITEM_CLASS_SR,					// 23
	ITEM_CLASS_BOMB,				// 24
	ITEM_CLASS_MINE,				// 25
	ITEM_CLASS_BELT,				// 26
	ITEM_CLASS_LEARNINGITEM,		// 27
	ITEM_CLASS_MONEY,				// 28
	ITEM_CLASS_CORPSE,				// 29
	
	// vampire용 item
	ITEM_CLASS_VAMPIRE_RING,		// 30
	ITEM_CLASS_VAMPIRE_BRACELET,	// 31
	ITEM_CLASS_VAMPIRE_NECKLACE,	// 32
	ITEM_CLASS_VAMPIRE_COAT,		// 33
	ITEM_CLASS_SKULL,				// 34
	ITEM_CLASS_MACE,				// 35
	ITEM_CLASS_SERUM,				// 36
	ITEM_CLASS_VAMPIRE_ETC,			// 37

	// 2001.10.22추가
	ITEM_CLASS_SLAYER_PORTAL_ITEM,	// 38
	ITEM_CLASS_VAMPIRE_PORTAL_ITEM,	// 39

	// 2001.12.10
	ITEM_CLASS_EVENT_GIFT_BOX,		// 40
	ITEM_CLASS_EVENT_STAR,			// 41

	ITEM_CLASS_VAMPIRE_EARRING,		// 42

	// 2002.6.7
	ITEM_CLASS_RELIC,				// 43

	// 2002.8.16
	ITEM_CLASS_VAMPIRE_WEAPON,		// 44
	ITEM_CLASS_VAMPIRE_AMULET,		// 45

	// 2002.9.7
	ITEM_CLASS_QUEST_ITEM,			// 46

	// 2002.12.09
	ITEM_CLASS_EVENT_TREE,			// 47
	ITEM_CLASS_EVENT_ETC,			// 48

	// 2003.1.29
	ITEM_CLASS_BLOOD_BIBLE,			// 49

	// 2003.2.12
	ITEM_CLASS_CASTLE_SYMBOL,		// 50

	ITEM_CLASS_COUPLE_RING,			// 51
	ITEM_CLASS_VAMPIRE_COUPLE_RING,	// 52			
	ITEM_CLASS_EVENT_ITEM,			// 53

	// 2003.5.15
	ITEM_CLASS_DYE_POTION,			// 54
	ITEM_CLASS_RESURRECT_ITEM,		// 55
	ITEM_CLASS_MIXING_ITEM,			// 56

	ITEM_CLASS_OUSTERS_ARMSBAND,     // 57
	ITEM_CLASS_OUSTERS_BOOTS,        // 58
	ITEM_CLASS_OUSTERS_CHAKRAM,      // 59
	ITEM_CLASS_OUSTERS_CIRCLET,      // 60
	ITEM_CLASS_OUSTERS_COAT,         // 61
	ITEM_CLASS_OUSTERS_PENDENT,      // 62
	ITEM_CLASS_OUSTERS_RING,         // 63
	ITEM_CLASS_OUSTERS_STONE,        // 64
	ITEM_CLASS_OUSTERS_WRISTLET,     // 65
	
	ITEM_CLASS_LARVA,                // 66
	ITEM_CLASS_PUPA,                 // 67
	ITEM_CLASS_COMPOS_MEI,           // 68
	ITEM_CLASS_OUSTERS_SUMMON_ITEM,  // 69
	ITEM_CLASS_EFFECT_ITEM,			 // 70
	
	ITEM_CLASS_CODE_SHEET,			 // 71
	ITEM_CLASS_MOON_CARD,			 // 72
	
	ITEM_CLASS_SWEEPER,				 // 73
	ITEM_CLASS_PET_ITEM,			 // 74
	ITEM_CLASS_PET_FOOD,             // 75
	ITEM_CLASS_PET_ENCHANT_ITEM,     // 76
	ITEM_CLASS_LUCKY_BAG,			 // 77

    ITEM_CLASS_SMS_ITEM,             // 78

	ITEM_CLASS_CORE_ZAP,             // 79

	ITEM_CLASS_GQUEST_ITEM,          // 80

	ITEM_CLASS_TRAP_ITEM,			 // 81

	ITEM_CLASS_BLOOD_BIBLE_SIGN,     // 82

	ITEM_CLASS_WAR_ITEM,			 // 83
	// by csm 12.27 2차 전직 아이템 
	ITEM_CLASS_CARRYING_RECEIVER,    // 84
	ITEM_CLASS_SHOULDER_ARMOR,       // 85
	ITEM_CLASS_DERMIS,               // 86
	ITEM_CLASS_PERSONA,              // 87
	ITEM_CLASS_FASCIA,               // 88
	ITEM_CLASS_MITTEN,               // 89

	ITEM_CLASS_SUB_INVENTORY,		 // 90
	ITEM_CLASS_COMMON_QUEST_ITEM ,   // 91 CommonQuestItemInfo,
	ITEM_CLASS_ETHEREAL_CHAIN	  ,	 // 92 EtherealChain
	ITEM_CLASS_OUSTERS_HARMONIC_PENDENT , // 93
	ITEM_CLASS_CHECK_MONEY,			 // 94
	ITEM_CLASS_CUE_OF_ADAM ,		 // 95
	ITEM_CLASS_CONTRACT_OF_BLOOD,	 // 96
	
	ITEM_CLASS_SKILL_BOOK,			 // 97
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	ITEM_CLASS_VAMPIREWING_ITEM,	 // 98
	ITEM_CLASS_OUSTERSWING_ITEM,	 // 99
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER

#if __CONTENTS(__TUNING_ITEM)
	ITEM_CLASS_TUNING_SLAYER,		// 100
	ITEM_CLASS_TUNING_VAMPIRE,		// 101
	ITEM_CLASS_TUNING_OUSTERS,		// 102
#endif //__TUNING_ITEM
#if __CONTENTS(__GLOBAL_NPC)
	ITEM_CLASS_CALLNPC_CARD,		// 103
	ITEM_CLASS_RANK_GEM,		// 104
#endif //__GLOBAL_NPC
	MAX_ITEM_CLASS,

	ITEM_CLASS_NULL
};

#if __CONTENTS(__ITEM_SHOW)
const static char *ITEM_SHOW_NAME[MAX_ITEM_CLASS] =
{
	"(Motorcycle)",						// 0
	"(Potion)",								// 1
	"(Water)",								// 2
	"(Holywater)",							// 3
	"(Magazine)",								// 4
	"(Bomb Material)",					// 5
	"(Etc)",								// 6
	"(Key)",							// 7
	"(Ring)",								// 8
	"(Bracelet)",						// 9
	"(Necklace)",						// 10
	"(Coat)",								// 11
	"(Trouser)",							// 12
	"(Shoes)",								// 13
	"(Sword)",								// 14
	"(Blade)",								// 15
	"(Shield)",								// 16
	"(Cross)",							// 17
	"(Glove)",							// 18
	"(Helm)",								// 19
	"(SG)",							// 20
	"(SMG)",							// 21
	"(AR)",							// 22
	"(SR)",							// 23
	"(BOMB)",						// 24
	"(Mine)",								// 25
	"(Belt)",								// 26
	"(LearningItem)",					// 27
	"(Money)",								// 28
	"(Corpse)",								// 29
	
	// vampire용 item
	"(Vampire Ring)",				// 30
	"(Vampire Bracelet)",	// 31
	"(Vampire Necklace)",		// 32
	"(Vampire Coat)",				// 33
	"(Skull)",								// 34
	"(Mace)",								// 35
	"(Serum)",								// 36
	"(Vampire Etc)",				// 37

	// 2001.10.22추가
	"(Slayer Portal)",				// 38
	"(Vampire Portal)",			// 39

	// 2001.12.10
	"(Event Gift Box)",				// 40
	"(Event Star)",					// 41

	"(Vampire Earring)",			// 42

	// 2002.6.7
	"(Relic)",								// 43

	// 2002.8.16
	"(Vampire Weapon)",			// 44
	"(Vampire Amulet)",			// 45

	// 2002.9.7
	"(Quest Item)",				// 46

	// 2002.12.09
	"(Event Tree)",					// 47
	"(Event Etc)",					// 48

	// 2003.1.29
	"(Blood Bible)",					// 49

	// 2003.2.12
	"(Castle Symbol)",					// 50

	"(Couple Ring)",						// 51
	"(Couple Ring)",				// 52			
	"(Event Item)",				// 53

	// 2003.5.15
	"(Dye Potion)",							// 54
	"(Resurrect Item)",				// 55
	"(Mixing Item)",					// 56

	"(Ousters Armsband)",	// 57
	"(Ousters Boots)",			// 58
	"(Ousters Chakram)",		// 59
	"(Ousters Circlet)",		// 60
	"(Ousters Coat)",			// 61
	"(Ousters Pendent)",		// 62
	"(Ousters Ring)",			// 63
	"(Ousters Stone)",			// 64
	"(Ousters Wristlet)",	// 65
	
	"(Larva)",								// 66
	"(Pupa)",								// 67
	"(Compos Mei)",							// 68
	"(Ousters Summon Item)",		// 69
	"(Effect Item)",					// 70

	"(Code Sheet)",						// 71
	"(Moon Card)",						// 72
	
	"(Sweeper)",							// 73
	"(Pet Item)",						// 74
	"(Pet Food)",						// 75
	"(Pet Enchant)",					// 76
	"(Lucky Bag)",						// 77

    "(Sms Item)",					// 78

	"(Core Zap)",							// 79

	"(Quest Item)",				// 80

	"(Trap Item)",					// 81

	"(Blood Bible Sign)",			// 82

	"(War Item)",					// 83
	// by csm 12.27 2차 전직 아이템 
	"2 Job Adv. Items(Carrying Receiver)",		// 84
	"(Shoulder Armor)",				// 85
	"(Dermis)",								// 86
	"(Persona)",						// 87
	"(Fascia)",							// 88
	"(Mitten)",								// 89

	"(Sub Inventory)",				// 90
	"(Common Quest Item)",			// 91
	"(Ethereal Chain)",			// 92
	"(Harmonic Pendent)",			// 93
	"(Check Money)",						// 94
	"(Cue Of Adam)",				// 95
	"(Contract Of Blood)",			// 96
	
	"(Skill Book)",						// 97
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	"(Vampirewing Item)",	// 98
	"(Ousterswing Item)",// 99
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER

#if __CONTENTS(__TUNING_ITEM)
	"(Tuning Items Slayer)",		// 100
	"(Tuning Items Vampire)",		// 101
	"(Tuning Items Ousters)",	// 102
#endif //__TUNING_ITEM
#if __CONTENTS(__GLOBAL_NPC)
	"(CallNPC item)",				// 103
	"(Rank Gem)",//104
#endif //__GLOBAL_NPC
};
#endif //__ITEM_SHOW

#endif
