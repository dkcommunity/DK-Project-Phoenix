//----------------------------------------------------------------------
// MItemOptionTable.h
//----------------------------------------------------------------------
// Item의 Option의 종류에 따른 정보를 저장하는 class
//----------------------------------------------------------------------

#ifndef	__MITEMOPTIONTABLE_H__
#define	__MITEMOPTIONTABLE_H__

#include "CTypeTable.h"
#include "MTypeDef.h"
#include "MString.h"
std::ofstream;
std::ifstream;

//#define	MAX_PARTNAME_LENGTH		48
#define		UNIQUE_ITEM_COLOR		0xffff
#define		QUEST_ITEM_COLOR		0xfffe
#define		LEVEL_WAR_ITEM_COLOR	0xfffd

//#define		OPTIONEXT_ITEM_COLOR	495
#define		OPTIONEXT_ITEM_COLOR	795		// 2008.08.14 ij-ch "MAX_COLORSET"

//----------------------------------------------------------------------
//
// 한 종류의 Item에 대한 정보
//
//----------------------------------------------------------------------
class ITEMOPTION_INFO {
	public :
	public :
		MString					EName;					// ItemOption 영어 이름		
		MString					Name;					// ItemOption 이름
		int						Part;					// ItemOption Part				
		int						PlusPoint;				// 증가치
		int						PriceMultiplier;		// 가치	(%로 되어 있으므로, 100으로 나누어야 함)	
		//int						PlusRequireAbility;		// 필요 능력 증가치		

		// 필요 능력치
		int						RequireSTR;
		int						RequireDEX;
		int						RequireINT;
		int						RequireSUM;		// 능력치 총합
		int						RequireLevel;
		
		int						ColorSet;				// 색 번호
		int						UpgradeOptionType;		// 업그래이드 되면 변-_-신하는 타입 0이면 변-_-신 안함
		int						PreviousOptionType;		// 업그래이드 실패해서 변-_-신하는 타입 0이면 변-_-신 안함

	public :
		ITEMOPTION_INFO();
		~ITEMOPTION_INFO();

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);		
		void			LoadFromFile(ivfstream& file);		
};


//--------------------------------------------------------------------------
// 하나의 class에 대한 type들의 정보
//--------------------------------------------------------------------------
class ITEMOPTION_TABLE:public CTypeTable<ITEMOPTION_INFO>
{
public:
	enum ITEMOPTION_PART
	{
		PART_STR = 0,      // increase STR
		PART_DEX,          // increase DEX 
		PART_INT,          // increase INT 
		PART_HP,           // increase HP 
		PART_MP,           // increase MP 
		PART_HP_STEAL,     // steal HP 
		PART_MP_STEAL,     // steal MP 
		PART_HP_REGEN,     // regenerate hp over time
		PART_MP_REGEN,     // regenerate mp over time
		PART_TOHIT,        // increase tohit
		PART_DEFENSE,      // increase defense
		PART_DAMAGE,       // increase damage 
		PART_PROTECTION,   // increase protection
		PART_DURABILITY,   // increase item durability
		PART_POISON,       // increase poison resistence
		PART_ACID,         // increase acid resistence
		PART_CURSE,        // increase curse resistence
		PART_BLOOD,        // increase blood resistence
		PART_VISION,       // increase vision range
		PART_ATTACK_SPEED, // increase attack speed
		PART_CRITICAL_HIT,
			
		// 2002.10.21일 추가
		PART_LUCK,         // increase looting item type
		PART_ALL_RES,      // increase all registance
		PART_ALL_ATTR,     // increase all attributes(str, dex, int)

		// 2002.12.03일 추가
		PART_STR_TO_DEX,   // STR to DEX
		PART_STR_TO_INT,   // STR to INT
		PART_DEX_TO_STR,   // DEX to STR
		PART_DEX_TO_INT,   // DEX to INT
		PART_INT_TO_STR,   // INT to STR
		PART_INT_TO_DEX,   // INT to DEX

		// 2003.2.12
		PART_CONSUME_MP,       // decrease consume mana
		PART_TRANS,        // translate race language
		PART_MAGIC_DAMAGE,  // increase magic damage
		PART_PHYSIC_DAMAGE, // increase physical damage
		PART_GAMBLE_PRICE, // decrease gamble price
		PART_POTION_PRICE, // decrease gamble price
 
		OPTION_MAGIC_PRO,	// 마법 방어력
		OPTION_PHYSIC_PRO,	// 물리 방어력

	/*	KOREA_NEW_0,
		KOREA_NEW_1,
		KOREA_NEW_2,
		KOREA_NEW_3,
		KOREA_NEW_4,
		KOREA_NEW_5,
		KOREA_NEW_6,
		KOREA_NEW_7,
		KOREA_NEW_8,
		KOREA_NEW_9,

		KOREA_NEW_10,
		KOREA_NEW_11,
		KOREA_NEW_12,
		KOREA_NEW_13,
		KOREA_NEW_14,
		KOREA_NEW_15,
		KOREA_NEW_16,
		KOREA_NEW_17,
		KOREA_NEW_18,
		KOREA_NEW_19,

		KOREA_NEW_20,
		KOREA_NEW_21,
		KOREA_NEW_22,
		KOREA_NEW_23,
		KOREA_NEW_24,
		KOREA_NEW_25,
		KOREA_NEW_26,
		KOREA_NEW_27,
		KOREA_NEW_28,
		KOREA_NEW_29,

		KOREA_NEW_30,
		KOREA_NEW_31,
		KOREA_NEW_32,
		KOREA_NEW_33,
		KOREA_NEW_34,
		KOREA_NEW_35,
		KOREA_NEW_36,
		KOREA_NEW_37,
		KOREA_NEW_38,
		KOREA_NEW_39,

		KOREA_NEW_40,
		KOREA_NEW_41,
		KOREA_NEW_42,
		KOREA_NEW_43,
		KOREA_NEW_44,
		KOREA_NEW_45,
		KOREA_NEW_46,
		KOREA_NEW_47,
		KOREA_NEW_48,
		KOREA_NEW_49,

		KOREA_NEW_50,
		KOREA_NEW_51,
		KOREA_NEW_52,
		KOREA_NEW_53,
		KOREA_NEW_54,
		KOREA_NEW_55,
		KOREA_NEW_56,
		KOREA_NEW_57,
		KOREA_NEW_58,
		KOREA_NEW_59,

		KOREA_NEW_60,
		KOREA_NEW_61,
		KOREA_NEW_62,
		KOREA_NEW_63,
		KOREA_NEW_64,
		KOREA_NEW_65,
		KOREA_NEW_66,
		KOREA_NEW_67,
		KOREA_NEW_68,
		KOREA_NEW_69,

		KOREA_NEW_70,
		KOREA_NEW_71,
		KOREA_NEW_72,
		KOREA_NEW_73,
		KOREA_NEW_74,
		KOREA_NEW_75,
		KOREA_NEW_76,
		KOREA_NEW_77,
		KOREA_NEW_78,
		KOREA_NEW_79,

		KOREA_NEW_80,
		KOREA_NEW_81,
		KOREA_NEW_82,
		KOREA_NEW_83,
		KOREA_NEW_84,
		KOREA_NEW_85,
		KOREA_NEW_86,
		KOREA_NEW_87,
		KOREA_NEW_88,
		KOREA_NEW_89,

		KOREA_NEW_90,
		KOREA_NEW_91,
		KOREA_NEW_92,
		KOREA_NEW_93,
		KOREA_NEW_94,
		KOREA_NEW_95,
		KOREA_NEW_96,
		KOREA_NEW_97,
		KOREA_NEW_98,
		KOREA_NEW_99,

		KOREA_NEW_100,
		KOREA_NEW_101,
		KOREA_NEW_102,
		KOREA_NEW_103,
		KOREA_NEW_104,
		KOREA_NEW_105,
		KOREA_NEW_106,
		KOREA_NEW_107,
		KOREA_NEW_108,
		KOREA_NEW_109,

		KOREA_NEW_110,
		KOREA_NEW_111,
		KOREA_NEW_112,
		KOREA_NEW_113,
		KOREA_NEW_114,
		KOREA_NEW_115,
		KOREA_NEW_116,
		KOREA_NEW_117,
		KOREA_NEW_118,
		KOREA_NEW_119,

		KOREA_NEW_120,
		KOREA_NEW_121,
		KOREA_NEW_122,
		KOREA_NEW_123,
		KOREA_NEW_124,
		KOREA_NEW_125,
		KOREA_NEW_126,
		KOREA_NEW_127,
		KOREA_NEW_128,
		KOREA_NEW_129,

		KOREA_NEW_130,
		KOREA_NEW_131,
		KOREA_NEW_132,
		KOREA_NEW_133,
		KOREA_NEW_134,
		KOREA_NEW_135,
		KOREA_NEW_136,
		KOREA_NEW_137,
		KOREA_NEW_138,
		KOREA_NEW_139,

		KOREA_NEW_140,
		KOREA_NEW_141,
		KOREA_NEW_142,
		KOREA_NEW_143,
		KOREA_NEW_144,
		KOREA_NEW_145,
		KOREA_NEW_146,
		KOREA_NEW_147,
		KOREA_NEW_148,
		KOREA_NEW_149,

		KOREA_NEW_150,
		KOREA_NEW_151,
		KOREA_NEW_152,
		KOREA_NEW_153,
		KOREA_NEW_154,
		KOREA_NEW_155,
		KOREA_NEW_156,
		KOREA_NEW_157,
		KOREA_NEW_158,
		KOREA_NEW_159,

		KOREA_NEW_160,
		KOREA_NEW_161,
		KOREA_NEW_162,
		KOREA_NEW_163,
		KOREA_NEW_164,
		KOREA_NEW_165,
		KOREA_NEW_166,
		KOREA_NEW_167,
		KOREA_NEW_168,
		KOREA_NEW_169,

		KOREA_NEW_170,
		KOREA_NEW_171,
		KOREA_NEW_172,
		KOREA_NEW_173,
		KOREA_NEW_174,
		KOREA_NEW_175,
		KOREA_NEW_176,
		KOREA_NEW_177,
		KOREA_NEW_178,
		KOREA_NEW_179,

		KOREA_NEW_180,
		KOREA_NEW_181,
		KOREA_NEW_182,
		KOREA_NEW_183,
		KOREA_NEW_184,
		KOREA_NEW_185,
		KOREA_NEW_186,
		KOREA_NEW_187,
		KOREA_NEW_188,
		KOREA_NEW_189,

		KOREA_NEW_190,
		KOREA_NEW_191,
		KOREA_NEW_192,
		KOREA_NEW_193,
		KOREA_NEW_194,
		KOREA_NEW_195,
		KOREA_NEW_196,
		KOREA_NEW_197,
		KOREA_NEW_198,
		KOREA_NEW_199,		 */


		MAX_PART
	};

	// option part의 name std::string
	MString ITEMOPTION_PARTNAME[MAX_PART];
	MString ITEMOPTION_PARTENAME[MAX_PART];

	void	LoadFromFile(ivfstream& file);
};

const WORD ITEM_COLOR_PART[ITEMOPTION_TABLE::ITEMOPTION_PART::MAX_PART] = 
{
		32,		//PART_STR
		17,		//PART_DEX
		2,		//PART_INT
		228,	//PART_HP
		243,	//PART_MP
		94,		//PART_HP_STEAL
		80,		//PART_MP_STEAL
		320,	//PART_HP_REGEN
		305,	//PART_MP_REGEN
		243,	//PART_TOHIT
		273,	//PART_DEFENSE
		392,	//PART_DAMAGE
		332,	//PART_PROTECTION
		317,	//PART_DURABILITY
		259,	//PART_POISON
		289,	//PART_ACID
		79,		//PART_CURSE
		244,	//PART_BLOOD
		214,	//PART_VISION
		227,	//PART_ATTACK_SPEED
		362,	//PART_CRITICAL_HIT
		109,	//PART_LUCK
		122,	//PART_ALL_RES
		139,	//PART_ALL_ATTR
		180,	//PART_STR_TO_DEX
		195,	//PART_STR_TO_INT
		210,	//PART_DEX_TO_STR
		195,	//PART_DEX_TO_INT
		210,	//PART_INT_TO_STR
		180,	//PART_INT_TO_DEX
		0,		//PART_CONSUME_MP
		0,		//PART_TRANS
		0,		//PART_MAGIC_DAMAGE
		0,		//PART_PHYSIC_DAMAGE
		0,		//PART_GAMBLE_PRICE
		0,		//PART_POTION_PRICE
		180,	//OPTION_MAGIC_PRO
		180		//OPTION_PHYSIC_PRO
		/*180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180,
		180	   */
};

extern	ITEMOPTION_TABLE*		g_pItemOptionTable;


#endif
