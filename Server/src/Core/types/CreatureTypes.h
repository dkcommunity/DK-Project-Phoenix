//////////////////////////////////////////////////////////////////////////////
// Filename    : CreatureTypes.h
// Written By  : Reiot
//////////////////////////////////////////////////////////////////////////////

#ifndef __CREATURE_TYPES_H__
#define __CREATURE_TYPES_H__

#include "SystemTypes.h"
#ifdef __GAME_CLIENT__
	#include "ModifyDef.h"
	#include "PacketItemDef.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// Player Character Type
//////////////////////////////////////////////////////////////////////////////
enum PCType 
{ 
	PC_SLAYER,
	PC_VAMPIRE,
	PC_OUSTERS
};
const string PCType2String [] = 
{
	"PC_SLAYER",
	"PC_VAMPIRE",
	"PC_OUSTERS"
};
const uint szPCType = szBYTE;

enum RaceType
{
	RACE_SLAYER,
	RACE_VAMPIRE,
	RACE_OUSTERS, 
	
	RACE_MAX
};

//////////////////////////////////////////////////////////////////////////////
// ??
//////////////////////////////////////////////////////////////////////////////
typedef BYTE Race_t;
const int szRace = sizeof(Race_t);

//////////////////////////////////////////////////////////////////////////////
// ??
//////////////////////////////////////////////////////////////////////////////
enum Competence 
{
	GOD = 0,
	DM,
	HELPER,
	PLAYER
};

//////////////////////////////////////////////////////////////////////////////
// ?????? ?? ??? ??...
//////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
enum ADDON
{
	ADDON_HAIR,			// ??
	ADDON_HELM,			// ??
	ADDON_COAT,			// ??
	ADDON_TROUSER,		// ??
	ADDON_LEFTHAND,		// ??			
	ADDON_RIGHTHAND,	// ???
	ADDON_MOTOR,		// ????
	ADDON_SHOULDER,		// ??
	ADDON_MAX
};
#endif

enum HelmetType 
{ 
	HELMET_NONE , 
	HELMET1 ,
	HELMET2 ,
	HELMET3 ,
	HELMET4 ,	//171	//by viva
	HELMET5 ,	//181
        HELMET6 ,
        HELMET7 ,//231
	HELMET_MAX
};

const string HelmetType2String[] = 
{ 
	"HELMET_NONE" , 
	"HELMET1" ,
	"HELMET2" ,
	"HELMET3" ,
	"HELMET4" ,	//171 //by viva
	"HELMET5" ,	//181
        "HELMET6" ,
        "HELMET7"
};

enum JacketType 
{ 
	JACKET_BASIC , 
	JACKET1 , 
	JACKET2 ,
	JACKET3 ,
	JACKET4 ,
	JACKET5 , //171	//by viva
	JACKET6 , //181	//by viva
	JACKET7 ,
	JACKET8 ,
	JACKET_MAX
};

const string JacketType2String[] = 
{ 
	"JACKET_BASIC" , 
	"JACKET1", 
	"JACKET2" ,
	"JACKET3" ,
	"JACKET4" ,
	"JACKET5" ,	//171	//by viva
	"JACKET6" ,	//181
	"JACKET7" ,
	"JACKET8"
};

enum PantsType 
{ 
	PANTS_BASIC , 
	PANTS1 , 
	PANTS2 ,
	PANTS3 ,
	PANTS4 ,
        PANTS5,
        PANTS6,
	PANTS_MAX
};

const string PantsType2String[] = 
{ 
	"PANTS_BASIC", 
	"PANTS1", 
	"PANTS2",
	"PANTS3",
	"PANTS4",
	"PANTS5",
	"PANTS6"
};

enum WeaponType 
{ 
	WEAPON_NONE ,
	WEAPON_SWORD ,
	WEAPON_SWORD_2, //181 171 //by viva
	WEAPON_BLADE ,
	WEAPON_BLADE_2,
	WEAPON_SR ,
	WEAPON_SR_2,
	WEAPON_AR ,
	WEAPON_AR_2,
	WEAPON_SG ,
	WEAPON_SMG ,
	WEAPON_CROSS ,
	WEAPON_CROSS_2,
	WEAPON_MACE ,
	WEAPON_MACE_2,
  //      WEAPON_SWORD_3,
//	WEAPON_BLADE_3,
//	WEAPON_SR_3,
//	WEAPON_AR_3,
	//WEAPON_CROSS_3,
//	WEAPON_MACE_3,
	WEAPON_MAX
//	WEAPON_SHIELD ,
//	WEAPON_SWORD_SHIELD ,
};

const string WeaponType2String[] = 
{ 
	"WEAPON_NONE",
	"WEAPON_SWORD",
	"WEAPON_SWORD_2",	//171 181	//by viva
	"WEAPON_BLADE",		
	"WEAPON_BLADE_2",
	"WEAPON_SR",
	"WEAPON_SR_2",
	"WEAPON_AR",
	"WEAPON_AR_2",
	"WEAPON_SG",
	"WEAPON_SMG",
	"WEAPON_CROSS",
	"WEAPON_CROSS_2",
	"WEAPON_MACE",
	"WEAPON_MACE_2",
      //  "WEAPON_SWORD_3",
	//"WEAPON_BLADE_3",
	//"WEAPON_SR_3",
	//"WEAPON_AR_3",
	//"WEAPON_CROSS_3",
	//"WEAPON_MACE_3",
//	"WEAPON_SHIELD",
//	"WEAPON_SWORD_SHIELD",
};

enum ShieldType 
{ 
	SHIELD_NONE ,
	SHIELD1 ,
	SHIELD2 ,
	SHIELD3 ,	//171	//by viva
	SHIELD4 ,	//181
	SHIELD5 ,
	SHIELD6 ,
	SHIELD_MAX
};

const string ShieldType2String[] = 
{ 
	"SHIELD_NONE",
	"SHIELD1",
	"SHIELD2",
	"SHIELD3",	//171	//by viva
	"SHIELD4",	//181
	"SHIELD5",
	"SHIELD6"
};

enum MotorcycleType 
{
	MOTORCYCLE_NONE ,
	MOTORCYCLE1 ,
	MOTORCYCLE2 ,
	MOTORCYCLE3 ,
	MOTORCYCLE_WING ,
	MOTORCYCLE_MAX
};

const string MotorcycleType2String[] = 
{
	"MOTORCYCLE_NONE" ,
	"MOTORCYCLE1" ,
	"MOTORCYCLE2" ,
	"MOTORCYCLE3" , 
	"MOTORCYCLE_WING"
};

enum VampireCoatType 
{ 
	VAMPIRE_COAT_BASIC , 
	VAMPIRE_COAT1 , 
	VAMPIRE_COAT2 ,
	VAMPIRE_COAT3 ,
	VAMPIRE_COAT4 ,
	VAMPIRE_COAT5 , //171	//by viva
	VAMPIRE_COAT6 , //181
	VAMPIRE_COAT7 ,
	VAMPIRE_COAT8 ,
	VAMPIRE_COAT_MAX
};

const string VampireCoatType2String[] = 
{ 
	"VAMPIRE_COAT_BASIC" , 
	"VAMPIRE_COAT1" , 
	"VAMPIRE_COAT2" ,
	"VAMPIRE_COAT3" ,
	"VAMPIRE_COAT4" ,
	"VAMPIRE_COAT5" ,	//171	//by viva
	"VAMPIRE_COAT6" ,	//181
	"VAMPIRE_COAT7" ,
	"VAMPIRE_COAT8" ,
	"VAMPIRE_COAT_MAX"
};

enum VampireArmType
{
	VAMPIRE_ARM_NONE,
	VAMPIRE_ARM_WEAPON,
	VAMPIRE_ARM_WEAPON2,	//171	//by viva
	VAMPIRE_ARM_WEAPON3,	//181	
	VAMPIRE_ARM_WEAPON4,
	VAMPIRE_ARM_WEAPON5,
	VAMPIRE_ARM_MAX
};

const string VampireArmType2String[] =
{
	"VAMPIRE_ARM_NONE",
	"VAMPIRE_ARM_WEAPON",
	"VAMPIRE_ARM_WEAPON2",	//171	//by viva
	"VAMPIRE_ARM_WEAPON3",	//181
	"VAMPIRE_ARM_WEAPON4",
	"VAMPIRE_ARM_WEAPON5",
	"VAMPIRE_ARM_MAX"
};

enum OustersCoatType 
{ 
	OUSTERS_COAT_BASIC , 
	OUSTERS_COAT1 , 
	OUSTERS_COAT2 ,
	OUSTERS_COAT3 ,
	OUSTERS_COAT4 ,
	OUSTERS_COAT5 , //171	//by viva
	OUSTERS_COAT6 , //181
	OUSTERS_COAT7 ,
	OUSTERS_COAT8 ,
	OUSTERS_COAT_MAX
};

const string OustersCoatType2String[] = 
{ 
	"OUSTERS_COAT_BASIC" , 
	"OUSTERS_COAT1" , 
	"OUSTERS_COAT2" ,
	"OUSTERS_COAT3" ,
	"OUSTERS_COAT4" ,
	"OUSTERS_COAT5" ,	//171	//by viva
	"OUSTERS_COAT6" ,	//181
	"OUSTERS_COAT7" ,
	"OUSTERS_COAT8" ,
	"OUSTERS_COAT_MAX"
};

enum OustersArmType
{
	OUSTERS_ARM_GAUNTLET,
	OUSTERS_ARM_CHAKRAM,
	OUSTERS_ARM_CHAKRAM_171, //171	//by viva
	OUSTERS_ARM_CHAKRAM_181, //181
	OUSTERS_ARM_CHAKRAM_201, //201
	OUSTERS_ARM_CHAKRAM_231, //201
	OUSTERS_ARM_MAX
};

const string OustersArmType2String[] =
{
	"OUSTERS_ARM_GAUNTLET",
	"OUSTERS_ARM_CHAKRAM",
	"OUSTERS_ARM_CHAKRAM_171", //171	//by viva
	"OUSTERS_ARM_CHAKRAM_181", //181
	"OUSTERS_ARM_CHAKRAM_201",
	"OUSTERS_ARM_CHAKRAM_231",
	"OUSTERS_ARM_MAX"
};

enum OustersSylphType
{
	OUSTERS_SYLPH_NONE,
	OUSTERS_SYLPH1,
	OUSTERS_SYLPH_MAX
};

const string OustersSylphType2String[] =
{
	"OUSTERS_SYLPH_NONE",
	"OUSTERS_SYLPH1",
	"OUSTERS_SYLPH_MAX"
};


//////////////////////////////////////////////////////////////////////////////
// ??
//////////////////////////////////////////////////////////////////////////////
enum Sex 
{ 
	FEMALE ,	// female == 0   -_-; why? don't know?
	MALE		// male == 1     -_-;
};
const string Sex2String [] = 
{ 
	"FEMALE" ,
	"MALE"
};
const uint szSex = szBYTE;


//////////////////////////////////////////////////////////////////////////////
// ?????
//////////////////////////////////////////////////////////////////////////////
enum HairStyle 
{ 
	HAIR_STYLE1 , 
	HAIR_STYLE2 , 
	HAIR_STYLE3 
};
const string HairStyle2String [] = 
{
	"HAIR_STYLE1" ,
	"HAIR_STYLE2" ,
	"HAIR_STYLE3"
};
const uint szHairStyle = szBYTE;


//////////////////////////////////////////////////////////////////////////////
// Slot ( MAX == 3 )
//////////////////////////////////////////////////////////////////////////////
enum Slot 
{ 
	SLOT1 , 
	SLOT2 , 
	SLOT3 ,
	SLOT_MAX
};
const string Slot2String [] = 
{
	"SLOT1" ,
	"SLOT2" ,
	"SLOT3"
};
const uint szSlot = szBYTE;


//////////////////////////////////////////////////////////////////////////////
// ?? ?? 
//////////////////////////////////////////////////////////////////////////////
typedef WORD Color_t;
const uint szColor = sizeof(Color_t);
enum ColorType 
{ 
	MAIN_COLOR , 
	SUB_COLOR 
};


//////////////////////////////////////////////////////////////////////////////
// STR/DEX/INT/HP/MP/AC/Damage...
//////////////////////////////////////////////////////////////////////////////

#define VAMP_REGENERATION_POINT 10

typedef BYTE Rank_t;
const uint szRank = sizeof(Rank_t);

typedef DWORD RankExp_t;
const uint szRankExp = sizeof(RankExp_t);

enum AttrType 
{
	ATTR_CURRENT = 0,
	ATTR_MAX,
	ATTR_BASIC
};

typedef WORD Attr_t;
const uint szAttr = sizeof(Attr_t);

// ??? ??? ?? ??? ?? ???..
// ?? 350?? ???? 2000?? ????. 
// (!) ?? ??? ??? ???? Error? catch?? log? ???? ? ????...
// by sigi. 2002.9.16
const uint maxSlayerAttr = 5000;
const uint maxVampireAttr = 5000;
const uint maxOustersAttr = 5000;

typedef DWORD HP_t;
const uint szHP = sizeof(HP_t);

typedef DWORD MP_t;
const uint szMP = sizeof(MP_t);

// ???
typedef WORD Defense_t;
const uint szDefense = sizeof(Defense_t);

// ????
typedef WORD Protection_t;
const uint szProtection = sizeof(Protection_t);

// ???
typedef WORD ToHit_t;
const uint szToHit = sizeof(ToHit_t);

typedef WORD Damage_t;
const uint szDamage = sizeof(Damage_t);

typedef BYTE SkillPoint_t;
const uint szSkillPoint = sizeof(SkillPoint_t);


//////////////////////////////////////////////////////////////////////////////
// defines for MODIFY bit flag //abcd
// ???? str, int, dex?? ??? ??? ??? ??? ?? ???? 
// ???
//////////////////////////////////////////////////////////////////////////////
#define MF_STR          0x01
#define MF_DEX          0x02
#define MF_INT          0x04
#define MF_MAX_HP       0x08
#define MF_MAX_MP       0x10
#define MF_DAM          0x20
#define MF_DEFENSE      0x40
#define MF_TOHIT        0x80

enum Attribute 
{ 
	STR = 0, 
	DEX, 
	INTE, 
	MP, 
	HP, 
	DEFENSE, 
	TOHIT, 
	PROTECT, 
	DAM, 
	SD, 
	DUR, 
	LEV, 
	MAX_ATTR 
};
	 
//////////////////////////////////////////////////////////////////////////////
// Skill ??
//////////////////////////////////////////////////////////////////////////////
// Skill? Type ??.
typedef WORD SkillType_t;
const uint szSkillType = sizeof(SkillType_t);

// Client ?? ???? EffectID.
typedef WORD CEffectID_t;
const uint szCEffectID = sizeof(CEffectID_t);

// Skill Effect ID
typedef WORD EffectID_t;
const uint szEffectID = sizeof(EffectID_t);

// Effect Attibute (??? ??)
typedef WORD EffectAttr_t;
const uint szEffectAttr = sizeof(EffectAttr_t);

// Slot? ??
typedef BYTE SlotID_t;
const uint szSlotID = sizeof(SlotID_t);

// Skill Domain
typedef BYTE SkillDomainType_t;
const uint szSkillDomainType = sizeof( SkillDomainType_t );

typedef BYTE SkillLevel_t;
const uint szSkillLevel = sizeof(SkillLevel_t);

typedef DWORD SkillExp_t;
const uint szSkillExp = sizeof(SkillExp_t);

typedef WORD ExpLevel_t;
const uint szExpLevel = sizeof(ExpLevel_t);

enum SkillDomain 
{
	SKILL_DOMAIN_BLADE = 0, // 0
	SKILL_DOMAIN_SWORD,     // 1
	SKILL_DOMAIN_GUN,       // 2
	SKILL_DOMAIN_HEAL,      // 3
	SKILL_DOMAIN_ENCHANT,   // 4
	SKILL_DOMAIN_ETC,       // 5
	SKILL_DOMAIN_VAMPIRE,   // 6
	SKILL_DOMAIN_OUSTERS,   // 7
	SKILL_DOMAIN_MAX        // 8
//	SKILL_DOMAIN_RIFLE ,    // 2...3?? ??? ??...
};

enum SkillGrade 
{
	SKILL_GRADE_APPRENTICE = 0, // 0
	SKILL_GRADE_ADEPT,			// 1
	SKILL_GRADE_EXPERT,			// 2
	SKILL_GRADE_MASTER,			// 3
	SKILL_GRADE_GRAND_MASTER,	// 4
	SKILL_GRADE_MAX				// 5
};

#define GRADE_APPRENTICE_LIMIT_LEVEL	24
#define GRADE_ADEPT_LIMIT_LEVEL			49
#define GRADE_EXPERT_LIMIT_LEVEL		74
#define GRADE_MASTER_LIMIT_LEVEL		99
#define GRADE_GRAND_MASTER_LIMIT_LEVEL	100

const string SkillDomain2String [] = 
{
	"SKILL_DOMAIN_BLADE",
	"SKILL_DOMAIN_SWORD",
	"SKILL_DOMAIN_GUN",
	"SKILL_DOMAIN_HEAL",	
	"SKILL_DOMAIN_ENCHANT",
	"SKILL_DOMAIN_ETC",
	"SKILL_DOMAIN_VAMPIRE"
	"SKILL_DOMAIN_OUSTERS"
	"SKILL_DOMAIN_MAX"
//	"SKILL_DOMAIN_RIFLE" ,
};

//////////////////////////////////////////////////////////////////////////////
// NPC??? ??? ?? ? ??? ????.
// ?? ??? ??? ? ??? ? ?? ?? ? ??? ??? ?
// ???? ??? ??? ????.
//////////////////////////////////////////////////////////////////////////////
const SkillLevel_t ALL_SKILL_LEARNED = 100;

//////////////////////////////////////////////////////////////////////////////
// PC ?? ?? ??
//////////////////////////////////////////////////////////////////////////////
typedef DWORD Fame_t;
const uint szFame = sizeof(Fame_t);

typedef DWORD Exp_t;
const uint szExp = sizeof(Exp_t);

typedef WORD Level_t;
const uint szLevel = sizeof(Level_t);

typedef WORD Bonus_t;
const uint szBonus = sizeof(Bonus_t);

typedef WORD SkillBonus_t;
const uint szSkillBonus = sizeof(SkillBonus_t);

typedef DWORD Gold_t;
const uint szGold = sizeof(Gold_t);

const Gold_t MAX_MONEY = 2000000000;

//////////////////////////////////////////////////////////////////////////////
// ??? ??
//////////////////////////////////////////////////////////////////////////////
typedef BYTE Coord_t;
const uint szCoord = sizeof(Coord_t);

typedef BYTE Dir_t;
const uint szDir = sizeof(Dir_t);

enum Directions 
{ 
	LEFT ,
	LEFTDOWN ,
	DOWN ,
	RIGHTDOWN ,
	RIGHT ,
	RIGHTUP ,
	UP ,
	LEFTUP ,
	DIR_MAX ,
	DIR_NONE = DIR_MAX
};
const string Dir2String [] = 
{
	"LEFT" ,
	"LEFTDOWN" ,
	"DOWN" ,
	"RIGHTDOWN" ,
	"RIGHT" ,
	"RIGHTUP" ,
	"UP" ,
	"LEFTUP",
	
	"DIR_MAX"
};


//////////////////////////////////////////////////////////////////////////////
// ?? ??
//////////////////////////////////////////////////////////////////////////////

typedef BYTE Vision_t;
const uint szVision = sizeof(Vision_t);

// ???? ??/?? ?? ??? ???. ?? ??? ??? ?????.
//const Coord_t maxViewportWidth  = 9;
//const Coord_t maxViewportUpperHeight = 10;
//const Coord_t maxViewportLowerHeight = 10;
//
//const Coord_t maxViewportWidth  = 12;
//const Coord_t maxViewportUpperHeight = 13;
//const Coord_t maxViewportLowerHeight = 13;
//const Coord_t maxViewportWidth  = 14;
//const Coord_t maxViewportUpperHeight = 14;
//const Coord_t maxViewportLowerHeight = 14;

// ?????(vision)? ??/?? ??
//const Coord_t maxVisionWidth = maxViewportWidth * 2 + 1;
//const Coord_t maxVisionHeight = maxViewportUpperHeight + maxViewportLowerHeight + 1;

// ?? ??
typedef BYTE Sight_t;
const uint szSight = sizeof(Sight_t); 
const Sight_t minSight = 0;
const Sight_t maxSight = 13;


//////////////////////////////////////////////////////////////////////////////
// ?? ???? ?(0.1?)? ??? ? ????.
//////////////////////////////////////////////////////////////////////////////
typedef DWORD Turn_t;
const uint szTurn = sizeof(Turn_t);


//////////////////////////////////////////////////////////////////////////////
// ???
//////////////////////////////////////////////////////////////////////////////
typedef WORD MonsterType_t;
const uint szMonsterType = sizeof(MonsterType_t);

typedef WORD SpriteType_t;
const uint szSpriteType = sizeof(SpriteType_t);

typedef BYTE Moral_t;
const uint szMoral = sizeof(Moral_t);


//////////////////////////////////////////////////////////////////////////////
// NPC
//////////////////////////////////////////////////////////////////////////////
typedef WORD NPCType_t;
const uint szNPCType = sizeof(NPCType_t);

typedef WORD NPCID_t;
const uint szNPCID = sizeof(NPCID_t);

//////////////////////////////////////////////////////////////////////////////
// ???&?? ??
//////////////////////////////////////////////////////////////////////////////
#define MAX_PHONE_SLOT  3
typedef DWORD PhoneNumber_t;
const uint szPhoneNumber = sizeof(PhoneNumber_t);

enum Alignment {
	LESS_EVIL,
	EVIL,
	NEUTRAL,
	GOOD,
	MORE_GOOD
};

typedef int Alignment_t;
const uint szAlignment= sizeof(Alignment_t);

//////////////////////////////////////////////////////////////////////////////
// hp, mp steal & regeneration
//////////////////////////////////////////////////////////////////////////////
typedef BYTE Steal_t;
const uint szSteal = sizeof(Steal_t);

typedef BYTE Regen_t;
const uint szRegen = sizeof(Regen_t);

//////////////////////////////////////////////////////////////////////////////
// Luck
//////////////////////////////////////////////////////////////////////////////
typedef short Luck_t;
const uint szLuck = sizeof(Luck_t);

//////////////////////////////////////////////////////////////////////////////
// ?? ???
//////////////////////////////////////////////////////////////////////////////
typedef short Resist_t;
const uint szResist = sizeof(Resist_t);

enum MagicDomain
{
	MAGIC_DOMAIN_NO_DOMAIN = 0, // ??? ??
	MAGIC_DOMAIN_POISON    = 1, // ? ?? ??
	MAGIC_DOMAIN_ACID      = 2, // ? ?? ??
	MAGIC_DOMAIN_CURSE     = 3, // ?? ?? ??
	MAGIC_DOMAIN_BLOOD     = 4, // ? ?? ??
//////////////////////////////////////////////////////////////////////////////
//	???? ?? ????? ???? ?? ?? ?? domain
	MAGIC_DOMAIN_ALLRES    = 5, // ???	
	MAGIC_DOMAIN_MAX
};

const string MagicDomain2String[] = 
{
	"NO_DOMAIN", // ??? ??
	"POISON",    // ? ?? ??
	"ACID",      // ? ?? ??
	"CURSE",     // ?? ?? ??
	"BLOOD",     // ? ?? ??
	"ALLRES",    // ???
	"MAGIC_DOMAIN_MAX"
};

const int MAX_RESIST = 90;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
typedef BYTE Shape_t;
const uint szShape = sizeof(Shape_t);
enum Shape
{
	SHAPE_NORMAL = 0,
	SHAPE_WOLF,
	SHAPE_BAT,
	SHAPE_WERWOLF,
	SHAPE_WING,
	SHAPE_MAX
};

//////////////////////////////////////////////////////////////////////////////
// ??? ?? ??
// ?? ?????, ?? ? ?? ??? ????.
//////////////////////////////////////////////////////////////////////////////
const uint TARGET_UNDERGROUND = 0x01;
const uint TARGET_GROUND      = 0x02;
const uint TARGET_AIR         = 0x04;

//////////////////////////////////////////////////////////////////////////////
// ?? ??
//////////////////////////////////////////////////////////////////////////////
typedef WORD ClanType_t;	// BYTE?? WORD? ??. by sigi. 2002.12.27
const uint szClanType = sizeof(ClanType_t);

//////////////////////////////////////////////////////////////////////////////
// ??? ?? ??? ??
//////////////////////////////////////////////////////////////////////////////
const WORD ATTR_EXP_SAVE_PERIOD    = 100;
const WORD DOMAIN_EXP_SAVE_PERIOD  = 100;
const WORD SKILL_EXP_SAVE_PERIOD   = 100;
const WORD VAMPIRE_EXP_SAVE_PERIOD = 100;
const WORD ALIGNMENT_SAVE_PERIOD   = 150;
const WORD FAME_SAVE_PERIOD        = 200;
const WORD RANK_EXP_SAVE_PERIOD    = 100;
const WORD OUSTERS_EXP_SAVE_PERIOD = 100;

////////////////////////////////////////////////////////////////////////////////
//
// ?? ?? ???
//
// VisionState >= IN_SIGHT     : ?? ?? ??.
// VisionState == OUT_OF_SIGHT : ??? ???.
//
////////////////////////////////////////////////////////////////////////////////
enum VisionState {
	OUT_OF_SIGHT ,      // ?? 8??? ?? ??. ??? ???.
	IN_SIGHT ,          // ?? 8??? ?? ??. ???. ?? ?? ??.
	ON_SIGHT,           // ?? 8??? ?? ??. ???. ?? ?? ??.
	NEW_SIGHT           // ?? 8??? ?? ??. ?????. ???. ?? ??.
};

const string VisionState2String [] =
{
	"OUT_OF_SIGHT" ,
	"IN_SIGHT" ,
	"ON_SIGHT" ,
	"NEW_SIGHT"
};

////////////////////////////////////////////////////////////////////////////////
// ????? ???? ??
////////////////////////////////////////////////////////////////////////////////
enum ElementalType 
{
	ELEMENTAL_ANY = -1,
	ELEMENTAL_FIRE = 0,
	ELEMENTAL_WATER,
	ELEMENTAL_EARTH,
	ELEMENTAL_WIND,

	ELEMENTAL_SUM,

	ELEMENTAL_MAX
};

const string Elemental2SimpleString[] =
{
	"Fire",
	"Water",
	"Earth",
	"Wind",

	"Sum",
};

typedef WORD Elemental_t;
const uint szElemental = sizeof(Elemental_t);

enum ElementalDomain
{
	ELEMENTAL_DOMAIN_NO_DOMAIN = -1,	// ???
	ELEMENTAL_DOMAIN_FIRE = 0,			// ???
	ELEMENTAL_DOMAIN_WATER,				// ???
	ELEMENTAL_DOMAIN_EARTH,				// ????
	ELEMENTAL_DOMAIN_WIND,				// ????
	ELEMENTAL_DOMAIN_COMBAT,			// ?? ?? ??
	ELEMENTAL_DOMAIN_ELEMENTAL_COMBAT,	// ?? ?? ??
	ELEMENTAL_DOMAIN_ETC,				// ??(??????)

	ELEMENTAL_DOMAIN_MAX
};

const WORD ATTACK_BLOOD_BURST_POINT_MAX = 3000;
const WORD DEFENSE_BLOOD_BURST_POINT_MAX = 2000;
const WORD PARTY_BLOOD_BURST_POINT_MAX = 20000;

#endif
