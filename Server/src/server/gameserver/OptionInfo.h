//////////////////////////////////////////////////////////////////////////////
// Filename    : OptionInfo.h
// Written By  : ????
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __OPTIONINFO_H__
#define __OPTIONINFO_H__

#include "Exception.h"
#include "Types.h"
#include "Item.h"
#include <list>
#include <vector>
#include <map>
 
//////////////////////////////////////////////////////////////////////////////
// ??? ?????? ???
//////////////////////////////////////////////////////////////////////////////
const uint OPTION_LEVEL_MAX = 100;
const uint GAMBLE_OPTION_LEVEL_MAX = 100;

class PlayerCreature;
class Slayer;
class Vampire;
class Ousters;

//////////////////////////////////////////////////////////////////////////////
// ????? ???? ???? ????
//////////////////////////////////////////////////////////////////////////////
enum OptionClass
{
	OPTION_STR = 0,      // increase STR
	OPTION_DEX,          // increase DEX 
	OPTION_INT,          // increase INT 
	OPTION_HP,           // increase HP 
	OPTION_MP,           // increase MP 
	OPTION_HP_STEAL,     // steal HP 
	OPTION_MP_STEAL,     // steal MP 
	OPTION_HP_REGEN,     // regenerate hp over time
	OPTION_MP_REGEN,     // regenerate mp over time
	OPTION_TOHIT,        // increase tohit
	OPTION_DEFENSE,      // increase defense
	OPTION_DAMAGE,       // increase damage 
	OPTION_PROTECTION,   // increase protection
	OPTION_DURABILITY,   // increase item durability
	OPTION_POISON,       // increase poison resistence
	OPTION_ACID,         // increase acid resistence
	OPTION_CURSE,        // increase curse resistence
	OPTION_BLOOD,        // increase blood resistence
	OPTION_VISION,       // increase vision range
	OPTION_ATTACK_SPEED, // increase attack speed
	OPTION_CRITICAL_HIT, // increase chance of doing critical hit
	OPTION_LUCK,		 // increase looting item type
	OPTION_ALL_RES,		 // increase all registance
	OPTION_ALL_ATTR,	 // increase all attributes(str, dex, int)
	OPTION_STR_TO_DEX,	 // STR to DEX
	OPTION_STR_TO_INT,	 // STR to INT
	OPTION_DEX_TO_STR,	 // DEX to STR
	OPTION_DEX_TO_INT,	 // DEX to INT
	OPTION_INT_TO_STR,	 // INT to STR
	OPTION_INT_TO_DEX,	 // INT to DEX
	OPTION_CONSUME_MP,		 // decrease consume mana
	OPTION_TRANS,		 // translate race language
	OPTION_MAGIC_DAMAGE,  // increase magic damage
	OPTION_PHYSIC_DAMAGE, // increase physical damage
	OPTION_GAMBLE_PRICE, // decrease gamble price
	OPTION_POTION_PRICE, // decrease gamble price
	OPTION_MAGIC_PRO,
	OPTION_PHYSIC_PRO,

	OPTION_MAX
};

const string OptionClass2String[] =
{
	"OPTION_STR",
	"OPTION_DEX",
	"OPTION_INT",
	"OPTION_HP",
	"OPTION_MP",
	"OPTION_HP_STEAL",
	"OPTION_MP_STEAL",
	"OPTION_HP_REGEN",
	"OPTION_MP_REGEN",
	"OPTION_TOHIT",
	"OPTION_DEFENSE",
	"OPTION_DAMAGE",
	"OPTION_PROTECTION",
	"OPTION_DURABILITY",
	"OPTION_POISON",
	"OPTION_ACID",
	"OPTION_CURSE",
	"OPTION_BLOOD",
	"OPTION_VISION",
	"OPTION_ATTACK_SPEED",
	"OPTION_CRITICAL_HIT",
	"OPITON_LUCK",
	"OPITON_ALL_RES",
	"OPITON_ALL_ATTR",
	"OPTION_STR_TO_DEX",
	"OPTION_STR_TO_INT",
	"OPTION_DEX_TO_STR",
	"OPTION_DEX_TO_INT",
	"OPTION_INT_TO_STR",
	"OPTION_INT_TO_DEX",
	"OPTION_CONSUME_MP",
	"OPTION_TRANS",
	"OPTION_MAGIC_DAMAGE",
	"OPTION_PHYSIC_DAMAGE",
	"OPTION_GAMBLE_PRICE",
	"OPTION_POTION_PRICE",
	"OPTION_MAGIC_PRO",
	"OPTION_PHYSIC_PRO",

	"OPTION_MAX"
};

enum OptionGroup
{
	OPTION_GROUP_ATTR = 0,			// ???? ???? ???
	OPTION_GROUP_STAT,				// ???????? ???? ???
	OPTION_GROUP_SPECIAL,			// ??? ??? (...?)
	OPTION_GROUP_RESIST,			// ???? ???
	OPTION_GROUP_ETC,				// ??? ???
	OPTION_GROUP_NOT				// ??? ????
};

class RareOptionUpgradeInfo
{
public:
	RareOptionUpgradeInfo( int level, int grade ) : m_Level(level), m_TotalGrade(grade)
	{
		m_UpgradeRatio[0].reserve(grade); m_UpgradeRatio[1].reserve(grade);
	}

	int				getKey() const { return m_Level*10 + m_TotalGrade; }

	int				getRatio( int grade, bool success ) const { Assert( grade < m_TotalGrade ); return m_UpgradeRatio[(int)success][grade]; }
	void			setRatio( int grade, bool success, int ratio ) { Assert( grade < m_TotalGrade ); m_UpgradeRatio[(int)success][grade] = ratio; }

private:
	int				m_Level;
	int				m_TotalGrade;
	vector<int>		m_UpgradeRatio[2];
};

class OptionClassInfo
{
public:
	OptionClassInfo( OptionClass optionClass ) : m_OptionClass(optionClass) { m_Level=0; m_TotalGrade=0; }

	OptionClass		getOptionClass() const { return m_OptionClass; }

	OptionGroup		getOptionGroup() const { return m_OptionGroup; }
	void			setOptionGroup( OptionGroup optionGroup ) { m_OptionGroup = optionGroup; }

	int				getKey() const { return m_Level*10 + m_TotalGrade; }

	const string&	getName() const { return m_Name; }
	void			setName( const string& name ) { m_Name = name; }

	const string&	getHName() const { return m_HName; }
	void			setHName( const string& hname ) { m_HName = hname; }

	int				getLevel() const { return m_Level; }
	void			setLevel( int level ) { m_Level = level; }

	int				getTotalGrade() const { return m_TotalGrade; }
	void			setTotalGrade( int grade ) { m_TotalGrade = grade; }
	
	//20090601 ksym555
	const string&	getIndexName() const { return m_IndexName; }
	void			setIndexName( const string& indexName ) { m_IndexName = indexName; }

	/*
	bool			ApplyOption( PlayerCreature* pPC, int PlusPoint );
	bool			ApplyOption( Slayer* pSlayer, int PlusPoint );
	bool			ApplyOption( Vampire* pVampire, int PlusPoint );
	bool			ApplyOption( Ousters* pOusters, int PlusPoint );
	*/

private:
	OptionClass		m_OptionClass;
	string			m_Name;
	string			m_HName;
	
	//20090601 ksym555
	string			m_IndexName;
	int				m_Level;			// ???
	int				m_TotalGrade;		// ????? ?? ??? ??
	OptionGroup		m_OptionGroup;
};

//////////////////////////////////////////////////////////////////////////////
// Class OptionInfo
//////////////////////////////////////////////////////////////////////////////

class OptionInfo 
{

///// Member methods /////
	
public:
	OptionInfo() throw();
	~OptionInfo() throw();

public:
	OptionType_t getType(void) const { return m_OptionType; }
	void setType(OptionType_t type) { m_OptionType = type; }

	string getName(void) const { return m_Name; }
	void setName(const string& name) { m_Name = name; }

	string getHName(void) const { return m_HName; }
	void setHName(const string& hname) { m_HName = hname; }
	
	string getNickname(void) const { return m_Nickname; }
	void setNickname(const string& name) { m_Nickname = name; }

	OptionClass getClass(void) const { return m_OptionClass; }
	void setClass(OptionClass OClass) { m_OptionClass = OClass; }

	int getPlusPoint(void) const { return m_PlusPoint; }
	void setPlusPoint(int point) { m_PlusPoint = point; }

	int getPriceMultiplier(void) const { return m_PriceMultiplier; }
	void setPriceMultiplier(int mul) { m_PriceMultiplier = mul; }

	int getReqSTR(void) const  { return m_ReqSTR; }
	void setReqSTR(int req) { m_ReqSTR = req; }

	int getReqDEX(void) const  { return m_ReqDEX; }
	void setReqDEX(int req) { m_ReqDEX = req; }

	int getReqINT(void) const  { return m_ReqINT; }
	void setReqINT(int req) { m_ReqINT = req; }

	int getReqSum(void) const  { return m_ReqSum; }
	void setReqSum(int req) { m_ReqSum = req; }

	int getReqLevel(void) const  { return m_ReqLevel; }
	void setReqLevel(int req) { m_ReqLevel = req; }

	void setReqAbility(const string& text) throw();

	int getLevel(void) const { return m_Level; }
	void setLevel(int level) { m_Level = level; }

	Color_t getColor(void) const { return m_Color; }
	void setColor(Color_t color) { m_Color = color; }

	Ratio_t getRatio(void) const { return m_Ratio; }
	void setRatio(Ratio_t ratio) { m_Ratio = ratio; }

	int getGambleLevel(void) const { return m_GambleLevel; }
	void setGambleLevel(int level) { m_GambleLevel = level; }

	// ???? ????? ??????? downgrade??? ????? ????
	OptionType_t getPreviousType(void) const { return m_PreviousOptionType; }
	void setPreviousType(OptionType_t type) { m_PreviousOptionType = type; }

	// ???? ????? ??????? upgrade??? ????? ????
	OptionType_t getUpgradeType(void) const { return m_UpgradeOptionType; }
	void setUpgradeType(OptionType_t type) { m_UpgradeOptionType = type; }

	OptionType_t getUpgradeType2(void) const { return m_UpgradeOptionType2; }
	void setUpgradeType2(OptionType_t type) { m_UpgradeOptionType2 = type; }
	
	OptionType_t getUpgradeType3(void) const { return m_UpgradeOptionType3; }
	void setUpgradeType3(OptionType_t type) { m_UpgradeOptionType3 = type; }
	
	Ratio_t getUpgradeRatio(void) const { return m_UpgradeRatio; }
	void setUpgradeRatio(Ratio_t ratio) { m_UpgradeRatio = ratio; }
	
	Ratio_t getUpgradeRatio2(void) const { return m_UpgradeRatio2; }
	void setUpgradeRatio2(Ratio_t ratio) { m_UpgradeRatio2 = ratio; }
	
	Ratio_t getUpgradeRatio3(void) const { return m_UpgradeRatio3; }
	void setUpgradeRatio3(Ratio_t ratio) { m_UpgradeRatio3 = ratio; }

	Ratio_t getUpgradeSecondRatio(void) const { return m_UpgradeSecondRatio; }
	void setUpgradeSecondRatio(Ratio_t ratio) { m_UpgradeSecondRatio = ratio; }

	Ratio_t getUpgradeBlueBirdRatio(void) const { return m_UpgradeBlueBirdRatio; }
	void setUpgradeBlueBirdRatio(Ratio_t ratio) { m_UpgradeBlueBirdRatio = ratio; }

	int getUpgradeCrashPercent(void) const { return m_UpgradeCrashPercent; }
	void setUpgradeCrashPercent(int per) { m_UpgradeCrashPercent = per; }

	// ???? ????? ??????? upgrade?? ????????
	bool isUpgradePossible() const	{ return m_UpgradeOptionType!=0 && m_UpgradeRatio!=0; }

	// ???? ????? ??????? upgrade?? ????????
	bool isUpgradeSucceed() const;
	bool isUpgradeSucceed2() const;
	bool isUpgradeSucceed3() const;
	// ???? ????? ??????? upgrade?? ????????
	bool isUpgradeSecondSucceed() const;
	
	// ???? ????? ??????? upgrade ???? ??????? item?? ?????????
	bool isUpgradeCrash() const;

	Ratio_t getNextOptionRatio(void) const { return m_NextOptionRatio; }
	void setNextOptionRatio(Ratio_t ratio) { m_NextOptionRatio = ratio; }

	int	getGrade() const { return m_Grade; }
	void setGrade( int grade ) { m_Grade = grade; }
	
	int	getNotice() const { return m_Notice; }
	void setNotice( int notice ) { m_Notice = notice; }

	string toString() const throw();


///// Member data /////

private:
	OptionType_t  m_OptionType;      // ??? ???
	string        m_Name;            // ???? ???
	string        m_HName;           // ??? ???
	string        m_Nickname;        // Nickname
	OptionClass   m_OptionClass;     // ????? ????
	int           m_PlusPoint;       // ??? ???
	int           m_PriceMultiplier; // ?? ??? ??
	int           m_ReqSTR;          // ????? STR?? ??? ??
	int           m_ReqDEX;          // ????? DEX?? ??? ??
	int           m_ReqINT;          // ????? INT?? ??? ??
	int           m_ReqSum;          // ????? ???? ?????? ??? ??
	int           m_ReqLevel;        // ????? ?????? ??? ??
	int           m_Level;           // ????? ????
	Color_t       m_Color;           // ??? ????
	Ratio_t       m_Ratio;           // ??? ?????? ???

	int           m_GambleLevel;     // ????? ???? ????

	// ???? ????? ??????? upgrade??? ????? ????
	OptionType_t  m_PreviousOptionType;     // ???? ????? ??? ???
	OptionType_t  m_UpgradeOptionType;      // ???? ????? ??? ???
	OptionType_t  m_UpgradeOptionType2;      // ???? ????? ??? ???
	OptionType_t  m_UpgradeOptionType3;      // ???? ????? ??? ???
	Ratio_t       m_UpgradeRatio;           // upgrade?? ?????? ??? = 1/m_UpgradeRatio
	Ratio_t       m_UpgradeRatio2;           // upgrade?? ?????? ??? = 1/m_UpgradeRatio
	Ratio_t       m_UpgradeRatio3;           // upgrade?? ?????? ??? = 1/m_UpgradeRatio
	Ratio_t       m_UpgradeSecondRatio;     // ???? ??? 2?? ???????? ?????? ???
	int           m_UpgradeCrashPercent;    // upgrade?? ??????? ?????? ??? = m_UpgradeCrashPercent%
	Ratio_t       m_NextOptionRatio;        // ???? ????? ???? ???

	int			  m_Grade;					// ???? ????? ?????? ????? ???
	int			  m_Notice;					// ???? ????? ?????? ????? ???
	
	Ratio_t       m_UpgradeBlueBirdRatio;
};

//////////////////////////////////////////////////////////////////////////////
// class OptionInfoSet
// ?????? ??? ????????? ???? ???? ?? ??? ????? ??????? ??? ?????.
//////////////////////////////////////////////////////////////////////////////

class OptionInfoSet
{

public:
	OptionInfoSet() throw();
	~OptionInfoSet() throw();

public:
	void addOptionType(uint level, OptionType_t type) throw(DuplicatedException, Error);

	void getPossibleOptionTypes(uint minLevel, uint maxLevel, vector<OptionType_t>& rOptionVector) throw();

	void clear()	{ m_OptionTypes.clear(); }

	string toString(void) const throw();

private:
	map<uint, OptionType_t> m_OptionTypes;
};

//////////////////////////////////////////////////////////////////////////////
// class PetEnchantOption
//////////////////////////////////////////////////////////////////////////////
class PetEnchantOption
{
public:
	PetEnchantOption() throw() { m_Type = 0; m_Ratio = 0; }
	~PetEnchantOption() throw() {}

	void setOptionType( OptionType_t type ) throw() { m_Type = type; }
	OptionType_t getOptionType() const throw() { return m_Type; }

	void setRatio( int ratio ) throw() { m_Ratio = ratio; }
	int getRatio() const throw() { return m_Ratio; }

private:
	OptionType_t 	m_Type;
	int				m_Ratio;

};

//////////////////////////////////////////////////////////////////////////////
// class CrystalEnchantOption
//////////////////////////////////////////////////////////////////////////////
class CrystalEnchantOption
{
public:
	CrystalEnchantOption() throw() { m_Type = 0; m_Ratio = 0; }
	~CrystalEnchantOption() throw() {}

	void setOptionType( OptionType_t type ) throw() { m_Type = type; }
	OptionType_t getOptionType() const throw() { return m_Type; }

	void setRatio( int ratio ) throw() { m_Ratio = ratio; }
	int getRatio() const throw() { return m_Ratio; }

private:
	OptionType_t 	m_Type;
	int				m_Ratio;

};

//////////////////////////////////////////////////////////////////////////////
// class OptionInfoManager
//////////////////////////////////////////////////////////////////////////////

class OptionInfoManager 
{

///// Member methods /////
	
public:
	OptionInfoManager() throw();
	~OptionInfoManager() throw();

public:
	void init() throw();
	void load() throw();

	void release() throw();
	
	OptionInfo* getOptionInfo(OptionType_t OptionType) throw(NoSuchElementException, Error);
	OptionInfo* getOptionInfo(const string& nickname) throw(NoSuchElementException, Error);
	OptionType_t getOptionType(const string& nickname) throw(NoSuchElementException, Error);
	string getOptionName(const list<OptionType_t>& optionTypes) throw(NoSuchElementException, Error);

	vector<OptionType_t> getPossibleOptionVector(Item::ItemClass IClass, uint minLevel, uint maxLevel) throw();

	void addOptionInfo(OptionInfo* pOptionInfo) throw(DuplicatedException, Error);

	// gamble ????
	void addGambleOption(Item::ItemClass itemClass, uint level, OptionType_t optionType) throw(DuplicatedException, Error);
	const vector<OptionType_t>& getPossibleGambleOptionVector(Item::ItemClass itemClass, uint level) throw()	{ return m_GambleOptions[itemClass][level]; }
	int	getTotalGambleRatio(Item::ItemClass itemClass, uint level) throw()	{ return m_TotalGambleRatio[itemClass][level]; }

	// rare enchant ????
	int getRareUpgradeRatio( OptionType_t optionType, bool success );
	const OptionClassInfo* getOptionClassInfo( OptionClass oc ) { return m_OptionClassInfos[oc]; }

	void addPetEnchantOption(PetEnchantOption* pPetEnchantOption) throw();
	const list<PetEnchantOption*>& getPetEnchantOptionList() const { return m_PetEnchantOptionList; }

	void setTotalPetEnchantOption( int total ) throw() { m_ToTalPetEnchantOption = total; }
	int getTotalPetEnchantOption() throw() { return m_ToTalPetEnchantOption; }

	// 20070726
	void addPetEnchantOption2(PetEnchantOption* pPetEnchantOption) throw();
	const list<PetEnchantOption*>& getPetEnchantOptionList2() const { return m_PetEnchantOptionList2; }

	void setTotalPetEnchantOption2( int total ) throw() { m_ToTalPetEnchantOption2 = total; }
	int getTotalPetEnchantOption2() throw() { return m_ToTalPetEnchantOption2; }

		// 20070726
	void addPetEnchantOption3(PetEnchantOption* pPetEnchantOption) throw();
	const list<PetEnchantOption*>& getPetEnchantOptionList3() const { return m_PetEnchantOptionList3; }

	void setTotalPetEnchantOption3( int total ) throw() { m_ToTalPetEnchantOption3 = total; }
	int getTotalPetEnchantOption3() throw() { return m_ToTalPetEnchantOption3; }
	
	// 20080218 by Kim Sung Yong
	void addCrystalEncantOption(CrystalEnchantOption* pCrystalEnchantOption) throw();
	const list<CrystalEnchantOption*>& getClystalEnchantOptionList() const { return m_CrystalEnchantList; }

	void setTotalCrystalEnchantOption( int total ) throw() { m_CrystalEnchantOption = total; }
	int getTotalCrystalEnchantOption() throw() { return m_CrystalEnchantOption; }

	// Blue Crystal (EventStar 81) ? BlueCrystalEnchantInfo tablosu
	void addBlueCrystalEncantOption(CrystalEnchantOption* pBlueCrystalEnchantOption) throw();
	const list<CrystalEnchantOption*>& getBlueCrystalEnchantOptionList() const { return m_BlueCrystalEnchantList; }

	void setTotalBlueCrystalEnchantOption( int total ) throw() { m_BlueCrystalEnchantOption = total; }
	int getTotalBlueCrystalEnchantOption() throw() { return m_BlueCrystalEnchantOption; }

	// Ordinary Crystal (EventStar 68) ? OrdinaryCrystalEnchantInfo tablosu
	void addOrdinaryCrystalEncantOption(CrystalEnchantOption* pOrdinaryCrystalEnchantOption) throw();
	const list<CrystalEnchantOption*>& getOrdinaryCrystalEnchantOptionList() const { return m_OrdinaryCrystalEnchantList; }

	void setTotalOrdinaryCrystalEnchantOption( int total ) throw() { m_OrdinaryCrystalEnchantOption = total; }
	int getTotalOrdinaryCrystalEnchantOption() throw() { return m_OrdinaryCrystalEnchantOption; }

	string toString() const throw();

private:
	void addOptionClassInfo( OptionClassInfo* pInfo ) { m_OptionClassInfos[pInfo->getOptionClass()] = pInfo; }

	uint                                m_nOptionCount;
	map<OptionType_t, OptionInfo*> m_OptionInfos;
	map<string, OptionInfo*>       m_NicknameOptionInfos;
	OptionInfoSet                       m_OptionInfoSet[OPTION_MAX];
	vector<OptionClassInfo*>			m_OptionClassInfos;
	vector<OptionType_t>                m_GambleOptions[Item::ITEM_CLASS_MAX][GAMBLE_OPTION_LEVEL_MAX+1];
	int					                m_TotalGambleRatio[Item::ITEM_CLASS_MAX][GAMBLE_OPTION_LEVEL_MAX+1];
	map<int,RareOptionUpgradeInfo*>		m_RareEnchantInfo;

	list<PetEnchantOption*> 			m_PetEnchantOptionList;
	int									m_ToTalPetEnchantOption;
	// 20070726
	list<PetEnchantOption*> 			m_PetEnchantOptionList2;
	int									m_ToTalPetEnchantOption2;
	list<PetEnchantOption*> 			m_PetEnchantOptionList3;
	int									m_ToTalPetEnchantOption3;
	// 20080218 by Kim Sung Yong
	list<CrystalEnchantOption*> 		m_CrystalEnchantList;
	int									m_CrystalEnchantOption;

	list<CrystalEnchantOption*> 		m_BlueCrystalEnchantList;
	int									m_BlueCrystalEnchantOption;

	list<CrystalEnchantOption*> 		m_OrdinaryCrystalEnchantList;
	int									m_OrdinaryCrystalEnchantOption;
};


//////////////////////////////////////////////////////////////////////////////
// global varibles
//////////////////////////////////////////////////////////////////////////////
extern OptionInfoManager* g_pOptionInfoManager;


#endif	
