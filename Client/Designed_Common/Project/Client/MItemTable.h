//----------------------------------------------------------------------
// MItemTable.h
//----------------------------------------------------------------------
// Item의 종류에 따른 정보를 저장하는 class
//----------------------------------------------------------------------
//
// [ 종류별로 있는 정보 ] 
//
// - Tile에서의 FrameID
// - Inventory에서의 SpriteID
// - 이름... 등등...
// 
//
// (*g_pItemTable)[class][type] 으로 하나의 Item정보에 접근 가능하다.
//
//----------------------------------------------------------------------
//
// Value1 ~ 4는 가변적이다..
// Item의 종류에 따라서 특정한 value가 어느 값을 대표할 수 있다.
//
// MotorCycle				: 내구성(1), 운반가능무게(2), Def(6)
// GearItem					: 내구성(1), Protection(2), Def(6)
// Belt						: 내구성(1), Protection(2), 포켓수(3), Def(6)
// HolyWater, 지뢰, 폭탄	: MinDam(1)~MaxDam(2)
// Potion					: 회복수치(1)
// 검,도					: 내구성(1), Protection(?), MinDam(3)~MaxDam(4), Speed(7)
// 십자가					: 내구성(1), Protection(?), MinDam(3)~MaxDam(4), MP증가(5), Speed(7)
// 총						: 내구성(1), Protection(?), MinDam(3)~MaxDam(4), 사정거리(5), Speed(7)
// 탄창						: Max탄창수(1)
//----------------------------------------------------------------------

#ifndef	__MITEMTABLE_H__
#define	__MITEMTABLE_H__

#pragma warning(disable:4786)

#include "CTypeTable.h"
#include "DrawTypeDef.h"
#include "MTypeDef.h"
#include "MString.h"
#include "SoundDef.h"
#include "BitAttribute.h"
//#include "SkillDef.h"
#include <list>

//----------------------------------------------------------------------
// 필요능력치 Flag
//----------------------------------------------------------------------
#define	REQUIRE_STR							0x01
#define	REQUIRE_DEX							0x02
#define	REQUIRE_INT							0x04
#define	REQUIRE_LEVEL						0x08

//----------------------------------------------------------------------
// 아이템 이동 제한에 관련된 Flag
//----------------------------------------------------------------------
#define ITEMMOVE_CANNOT_DROP				0x01
//#define ITEMMOVE_CANNOT_SELL				0x02	// 상점에 못 파는건 서버에서 막아줌
#define ITEMMOVE_CANNOT_DISPLAY_PERSNALSHOP	0x04	// 개인상점
#define ITEMMOVE_CANNOT_KEEP_STORAGE		0x08	// 보관함ㅂ
#define ITEMMOVE_CANNOT_TRADE				0x10	// 거래
#define ITEMMOVE_PICKUP_ONCE				0x20	// 인벤토리에 한 개만 넣을 수 있는 아이템

#define ADVANCE_CANNOT_USE_ITEM				0x01	//승직 못쓴다.
#define NONADVANCE_CANNOT_USE_ITEM			0x02	//비승직 못쓴다.

extern COLORREF g_ELEMENTAL_COLOR[5];
extern int g_ELEMENTAL_STRING_ID[5];


typedef CBitAttribute<BYTE, DWORD> ITEM_MOVE_CONTROL_ATTR;
typedef CBitAttribute<BYTE, DWORD> ITEM_USE_CAN_ADVANCE;

//----------------------------------------------------------------------
//
// 한 종류의 Item에 대한 정보
//
//----------------------------------------------------------------------
class ITEMTABLE_INFO {
	public :
		enum ELEMENTAL_TYPE
		{
			ELEMENTAL_TYPE_ANY = -1,
			ELEMENTAL_TYPE_FIRE,
			ELEMENTAL_TYPE_WATER,
			ELEMENTAL_TYPE_EARTH,
			ELEMENTAL_TYPE_WIND,
			ELEMENTAL_TYPE_SUM,

			ELEMENTAL_TYPE_MAX,
		};

		MString					HName;				// Item 한글 이름
		MString					EName;				// Item 영어 이름
		MString					Description;		// Item에 대한 설명
		
	 	// Frame ID
		TYPE_FRAMEID			TileFrameID;		// Tile에서의 FrameID
		TYPE_FRAMEID			InventoryFrameID;	// Inventory에서의 Frame ID
		TYPE_FRAMEID			GearFrameID;		// Gear에서의 Frame ID
		TYPE_FRAMEID			AddonMaleFrameID;	// 장착했을 때의 동작 FrameID - 남자
		TYPE_FRAMEID			AddonFemaleFrameID;	// 장착했을 때의 동작 FrameID - 여자
		TYPE_FRAMEID			DropFrameID;		// item을 바닥에 떨어뜨렸을때 FrameID
		TYPE_FRAMEID			UseFrameID;			// item을 사용했을 때

		// 2005, 1, 14, sobeit add start - ItemDescription.spk 에서 쓰는 frameID
		TYPE_FRAMEID			DescriptionFrameID;	// 
		// 2005, 1, 14, sobeit add endDescription에서의 FrameID
		
		// Sound ID
		TYPE_SOUNDID			UseSoundID;			// Item 사용 SoundID			
		TYPE_SOUNDID			TileSoundID;		// Item 줍기 SoundID
		TYPE_SOUNDID			InventorySoundID;	// Inventory에서의 Sound
		TYPE_SOUNDID			GearSoundID;		// Gear에서의 Sound

		bool					bMaleOnly;
		bool					bFemaleOnly;
		
		// inventory에서의 Grid크기
		BYTE					GridWidth;
		BYTE					GridHeight;

		// item 자체에 대한 고정된 정보
		TYPE_ITEM_WEIGHT		Weight;				// 무게	
		TYPE_ITEM_PRICE			Price;
		int						SilverMax;			// 은코팅.. 0이면 못하는거다.
		int						ToHit;				// ToHit

		// 값들.. --> Protection, 공격력, 사정거리...
		int						Value1;
		int						Value2;
		int						Value3;				
		int						Value4;
		int						Value5;
		int						Value6;
		int						Value7;

		// 기본 공격 ActionInfo
		TYPE_ACTIONINFO			UseActionInfo;	

		// 최대값 개수
		TYPE_ITEM_NUMBER		MaxNumber;

		// critical
		int						CriticalHit;
		std::list<TYPE_ITEM_OPTION>	DefaultOptionList;
		int						ItemStyle;
#if __CONTENTS(__ONIBLA_ITEM)
		// 일반 아이템 등급 - 일반 아이템인데도 특수한 이펙트 효과를 주기 위해 설정
		// 현재는 
		// 0 : 일반 등급
		// 1 : 상위 등급
		int						NormalItemGrade;
#endif // __ONIBLA_ITEM
		
		// ousters info
		ELEMENTAL_TYPE			ElementalType;
		WORD					Elemental;
		BYTE					Race;
		
		ITEM_MOVE_CONTROL_ATTR	ItemMoveControl;			// 아이템 이동에 관련된 제한
		ITEM_USE_CAN_ADVANCE	ItemCanAdvance;				// 아이템을 승직캐릭터가 사용 할수 있는가에 대한 제한
		
		DWORD					DropItemNameTag;
		int					tx;	// 필요 승직 레벨
		//DWORD					tx1;
		//DWORD					tx2;
	 //   DWORD					tx3;
		//BYTE					kimnew;
		//BYTE					kimneww;
		
	protected :	
		// 사용가능한 제한능력
		BYTE					RequireSTR;
		BYTE					RequireDEX;
		BYTE					RequireINT;		
		BYTE					RequireLevel;
		WORD					RequireSUM;					// 능력치 총합
		BYTE					RequireAdvancementLevel;	// 필요 승직 레벨
	public :
		ITEMTABLE_INFO();
		~ITEMTABLE_INFO();

		//-------------------------------------------------------
		// Set
		//-------------------------------------------------------
		void	SetSoundID(TYPE_SOUNDID tile, TYPE_SOUNDID inventory, TYPE_SOUNDID gear, TYPE_SOUNDID use);
		void	SetFrameID(TYPE_FRAMEID tile, TYPE_FRAMEID inventory, TYPE_FRAMEID gear, TYPE_FRAMEID use = FRAMEID_NULL);
		void	SetDropFrameID(TYPE_FRAMEID drop);
		void	SetAddonFrameID(TYPE_FRAMEID male, TYPE_FRAMEID female);
		void	SetGrid(BYTE width, BYTE height);
		void	SetValue(int v1, int v2=-1, int v3=-1, int v4=-1, int v5=-1, int v6=-1, int v7=-1);
		void	SetDescriptionFrameID(TYPE_FRAMEID fID) { DescriptionFrameID = fID;}

		//-------------------------------------------------------
		// 필요능력치 설정
		//-------------------------------------------------------
		void	SetRequireSTR(BYTE str=0)			{ RequireSTR = str; }
		void	SetRequireDEX(BYTE dex=0)			{ RequireDEX = dex; }
		void	SetRequireINT(BYTE intel=0)			{ RequireINT = intel; }
		void	SetRequireSUM(WORD sum=0)			{ RequireSUM = sum; }
		void	SetRequireLevel(BYTE level=0)		{ RequireLevel = level; }		
		void	SetRequireAdvancementLevel(BYTE level=0)		{ RequireAdvancementLevel = level; }		

		//-------------------------------------------------------
		// Get
		//-------------------------------------------------------
		BYTE		GetRequireSTR()	const				{ return RequireSTR; }
		BYTE		GetRequireDEX()	const				{ return RequireDEX; }
		BYTE		GetRequireINT()	const				{ return RequireINT; }
		WORD		GetRequireSUM()	const				{ return RequireSUM; }
		BYTE		GetRequireLevel() const				{ return RequireLevel; }		
		BYTE		GetRequireAdvancementLevel() const	{ return RequireAdvancementLevel; }		

		//-------------------------------------------------------
		// 성별
		//-------------------------------------------------------
		bool		IsGenderForMale() const				{ return bMaleOnly; }
		bool		IsGenderForFemale() const			{ return bFemaleOnly; }
		bool		IsGenderForAll() const				{ return !bMaleOnly && !bFemaleOnly; }
		

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);		
		void			LoadFromFile(ivfstream& file);	
		void			SaveToCSVFile(std::ofstream& file, int classIdx, int typeIdx);

#if __CONTENTS(__ITEMINFO_TABLEATION_PROJECT)
		void			LoadToCSVFile(std::ifstream& file, int classIdx, int typeIdx);
#endif //__ITEMINFO_TABLEATION_PROJECT

};


//--------------------------------------------------------------------------
// 하나의 class에 대한 type들의 정보
//--------------------------------------------------------------------------
class ITEMTYPE_TABLE : public CTypeTable<ITEMTABLE_INFO>
{
public :
	void	SaveToCSVFile(std::ofstream& file, int classIdx);

#if __CONTENTS(__ITEMINFO_TABLEATION_PROJECT)
	void	LoadToCSVFile(std::ifstream& file, int classIdx);
#endif //__ITEMINFO_TABLEATION_PROJECT

	void	LoadFromFile(ivfstream& file);

	int		GetAveragePrice() const	{ return m_AveragePrice; }

private :
	int		m_AveragePrice;
};

//----------------------------------------------------------------------
//
// ItemClass에 대한 Table
//
//  :  (*g_pItemTable)[ itemClass ][ itemType ]
//
//----------------------------------------------------------------------
class ITEMCLASS_TABLE : public CTypeTable<ITEMTYPE_TABLE> {

	public :
		ITEMCLASS_TABLE();
		~ITEMCLASS_TABLE();

		void		SaveToCSVFile(std::ofstream& file);

#if __CONTENTS(__ITEMINFO_TABLEATION_PROJECT)
		void		LoadToCSVFile(std::ifstream& file);
#endif //__ITEMINFO_TABLEATION_PROJECT
		
		//-------------------------------------------------------
		// 각 classType에 맞는 개수를 초기화한다.
		//-------------------------------------------------------
		void		InitClass( int c, int size );
};

extern	ITEMCLASS_TABLE	*	g_pItemTable;


#endif
