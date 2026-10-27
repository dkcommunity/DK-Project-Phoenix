//////////////////////////////////////////////////////////////////////////////
// Filename    : Ousters.h
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __OUSTERS_H__
#define __OUSTERS_H__

#include "PlayerCreature.h"
#include "Inventory.h"
#include "Zone.h"
#include "PCOustersInfo.h"
#include "PCOustersInfo2.h"
#include "PCOustersInfo3.h"
#include "InventoryInfo.h"
#include "GearInfo.h"
#include "ExtraInfo.h"
#include "OptionInfo.h"
#include "OustersSkillInfo.h"
#include "Mutex.h"
#include "ModifyInfo.h"
#include "CreatureUtil.h"
#include <map>

#include "skill/OustersSkillSlot.h"

#include "Gpackets/GCModifyInformation.h"

class SkillInfo;
class SkillOutput;

#define OUSTERS_MAX_LEVEL				150		//abcd
#define OUSTERS_MAX_RANK				50		// 계급 max
#define OUSTERS_MAX_SUM					492
#define OUSTERS_MAX_ATTR				472
#define BONUS_POINTS_PER_LEVEL1			3		// 레벨업시 주어지는 능력치 point
#define BONUS_POINTS_PER_LEVEL2			2		// 레벨업시 주어지는 능력치 point
#define BONUS_POINTS_PER_LEVEL3			1		// 레벨업시 주어지는 능력치 point

////////////////////////////////////////////////////////////////////////////////
// 장비를 입고 벗을 때 장비 입고 벗기 전의 능력치를 
// 저장해 두기 위한 버퍼(?) 클래스
////////////////////////////////////////////////////////////////////////////////

class OUSTERS_RECORD
{
public:
	Attr_t       pSTR[3];
	Attr_t       pDEX[3];
	Attr_t       pINT[3];
	Rank_t    	 Rank;
	HP_t         pHP[2];
	MP_t         pMP[2];
	Damage_t     pDamage[2];
	Defense_t    Defense;
	Protection_t Protection;
	ToHit_t      ToHit;
	Speed_t      AttackSpeed;
	Bonus_t      Bonus;
	Damage_t     pSilverDamage;
	// 20070918
	Defense_t    MagicDefense;
	ToHit_t      MagicToHit;
#ifdef __CONTRIBUTION_SYSTEM__
	int			ContributePoint; // 기여도 점수
#endif
	int	    Luck;
	int	    HpSteal;
	int	    MpSteal;
	int 	    AllRes;
	int 	    Cri;
};




////////////////////////////////////////////////////////////////////////////////
//
// Class Ousters;
//
////////////////////////////////////////////////////////////////////////////////

class GCAddOusters;

class Ousters : public PlayerCreature 
{

////////////////////////////////////////////////////
// 클래스 내부 상수 선언
////////////////////////////////////////////////////
public:
	// 장착 부위
	enum WearPart
	{ 
		WEAR_CIRCLET,
		WEAR_COAT,
		WEAR_LEFTHAND,
		WEAR_RIGHTHAND,
		WEAR_BOOTS,
		WEAR_ARMSBAND1,
		WEAR_ARMSBAND2,
		WEAR_RING1,
		WEAR_RING2,
		WEAR_NECKLACE1,
		WEAR_NECKLACE2,
		WEAR_NECKLACE3,
		WEAR_STONE1,
		WEAR_STONE2,
		WEAR_STONE3,
		WEAR_STONE4,
		WEAR_ZAP1,
		WEAR_ZAP2,
		WEAR_ZAP3,
		WEAR_ZAP4,
		WEAR_FASCIA,
		WEAR_MITTEN,
		WEAR_NECK_CHAIN,
		WEAR_CUE1,
		WEAR_CUE2,
		OUSTERS_WEAR_MAX 
	};

	enum SylphType
	{
		SYLPH_TYPE_NONE = 0,
		SYLPH_TYPE_LEGACY = 1,
		SYLPH_TYPE_WING = 2
	};

typedef struct _OustersTaoZhuangItem
{
	int		m_Group;
	int 		m_WearPart;
	ItemType_t 	m_ItemType;    
} OustersTaoZhuangItem;

typedef list<OustersTaoZhuangItem>		ListItem;
typedef ListItem::iterator			ListItemItr;
typedef ListItem::const_iterator		ListItemConstItr;

////////////////////////////////////////////////////
// 생성자/소멸자
////////////////////////////////////////////////////
public:
	Ousters() throw();
	virtual ~Ousters() throw ();

////////////////////////////////////////////////////
// 하위 클래스(Creature) 상속 함수
////////////////////////////////////////////////////
public:
	virtual CreatureClass getCreatureClass() const throw() { return CREATURE_CLASS_OUSTERS; }
	virtual string getCreatureClassString() const throw() { return "CREATURE_CLASS_OUSTERS"; }

	virtual void registerObject() throw(Error);
	virtual void registerInitObject() throw(Error);

	virtual bool load() throw (InvalidProtocolException, Error);
	void loadItem( bool checkTimeLimit = false ) throw (InvalidProtocolException, Error);

	virtual void save() const throw (Error);
	virtual void tinysave(const string & field) const throw (Error);
//	virtual void tinysave(const char* field) const throw (Error);
	void saveSkills(void) const throw (Error);
	void saveGears(void) const throw (Error);
	void saveExps(void) const throw (Error);

	virtual void act(const Timeval& currentTime) throw(Error) {}

	virtual string toString() const throw();

	virtual Race_t getRace() const { return RACE_OUSTERS; }
	virtual GuildID_t getCommonGuildID() const { return OustersCommon; }

	Packet* getAddPacket() const;

//////////////////////////////////////////////////////////////
// 시간제한 아이템 관련 함수
//////////////////////////////////////////////////////////////
public:
	void checkItemTimeLimit() throw (Error);
	void updateEventItemTime( DWORD time ) throw(Error);

////////////////////////////////////////////////////
// 상태 관련 함수(Dead or Alive!)
////////////////////////////////////////////////////
public:
	bool isAlive() const throw() { return m_HP[ATTR_CURRENT] != 0; }
	bool isDead() const throw() { return m_HP[ATTR_CURRENT] == 0; }


////////////////////////////////////////////////////
// 겉모습 관련 함수(이름, 권한, 성별, 머리, 피부)
////////////////////////////////////////////////////
public:

	BYTE getCompetence() const throw() { return m_Competence; }
	void setCompetence(BYTE Competence) { m_Competence = Competence; }

	BYTE getCompetenceShape() const throw() { return m_CompetenceShape; }
	void setCompetenceShape(BYTE CompetenceShape) { m_CompetenceShape = CompetenceShape; }

	Sex getSex() const throw() { return m_Sex; }
	void setSex(Sex sex) throw() { m_Sex = sex; m_OustersInfo.setSex(sex); }
	void setSex(const string & sex) throw(InvalidProtocolException) 
	{
		if(sex == Sex2String[MALE])
			setSex(MALE);
		else if(sex == Sex2String[FEMALE])
			setSex(FEMALE);
		else throw InvalidProtocolException("invalid sex");
	}

	Color_t getHairColor() const throw() { return m_HairColor; }
	void setHairColor(Color_t hairColor) throw() { m_HairColor = hairColor; }

	SylphType GetWingSylphType() { return m_WingSylphType; }
	void SetWingSylphType(SylphType Type) { m_WingSylphType = Type; }
	
	ItemID_t GetWingItemType() { return m_WingItemType; }
	void SetWingItemType(ItemID_t ItemType) { m_WingItemType = ItemType;}

	Color_t GetWingBodyColor() { return m_WingBodyColor; }
	void SetWingBodyColor(Color_t Color) { m_WingBodyColor = Color; }

	Color_t GetWingEffectColor() { return m_WingEffectColor; }
	void SetWingEffectColor(Color_t Color) { m_WingEffectColor = Color; }

////////////////////////////////////////////////////
// 능력치 관련 함수(STR, DEX, INT)
////////////////////////////////////////////////////
public:

	Alignment_t getAlignment() const throw() { return m_Alignment; }
	void setAlignment(Alignment_t Alignment) throw();
	void saveAlignment(Alignment_t alignment) throw();

	Attr_t getSTR(AttrType attrType = ATTR_CURRENT) const throw() { return m_STR[attrType]; }
	void setSTR(Attr_t attr, AttrType attrType = ATTR_CURRENT) throw() { m_STR[attrType] = attr; }
	
	Attr_t getDEX(AttrType attrType = ATTR_CURRENT) const throw() { return m_DEX[attrType]; }
	void setDEX(Attr_t attr, AttrType attrType = ATTR_CURRENT) throw() { m_DEX[attrType] = attr; }

	Attr_t getINT(AttrType attrType = ATTR_CURRENT) const throw() { return m_INT[attrType]; }
	void setINT(Attr_t attr, AttrType attrType = ATTR_CURRENT) throw() { m_INT[attrType] = attr; }

////////////////////////////////////////////////////
// 능력치 관련 함수(Damage, Protect, Defense, ToHit)
////////////////////////////////////////////////////
public:
	// 20070918
	Defense_t    getMagicDefense(void) const throw() { return m_MagicDefense; }
	ToHit_t      getMagicToHit(void) const throw() { return m_MagicToHit; }
	
//////////////////////////////////////////////////////////////
// Blood Burst Point
//////////////////////////////////////////////////////////////
public:
	Attr_t getAttackBloodBurstPoint() const { return m_AttackBloodBurstPoint; }
	void setAttackBloodBurstPoint( Attr_t point ) { m_AttackBloodBurstPoint = point; }
	void increaseAttackBloodBurstPoint( Attr_t point ) { m_AttackBloodBurstPoint = min( m_AttackBloodBurstPoint + point, (int)ATTACK_BLOOD_BURST_POINT_MAX ); }
	void resetAttackBloodBurstPoint() { m_AttackBloodBurstPoint = 0; }
	Attr_t getDefenseBloodBurstPoint() const { return m_DefenseBloodBurstPoint; }
	void setDefenseBloodBurstPoint( Attr_t point ) { m_DefenseBloodBurstPoint = point; }
	void increaseDefenseBloodBurstPoint( Attr_t point ) { m_DefenseBloodBurstPoint = min( m_DefenseBloodBurstPoint + point, (int)DEFENSE_BLOOD_BURST_POINT_MAX ); }
	void resetDefenseBloodBurstPoint() { m_DefenseBloodBurstPoint = 0; }
	
////////////////////////////////////////////////////
// 경험치 관련 함수
////////////////////////////////////////////////////
public:
//	Exp_t getExp() const throw() { return m_Exp; }
//	void  setExp(Exp_t exp) throw() { m_Exp = exp; }

	Exp_t getGoalExp() const throw() { return m_GoalExp; }
	void  setGoalExp(Exp_t GoalExp) throw() { m_GoalExp = GoalExp; }

//	Exp_t getExpOffset() const throw() { return m_ExpOffset; }
//	void  setExpOffset(Exp_t expOffset) throw() { m_ExpOffset = expOffset; }

//	void setExp(Exp_t exp, Exp_t expOffset) throw() { m_Exp = exp; m_ExpOffset = expOffset; }

	virtual Level_t getLevel() const throw() { return m_Level; }
	void    setLevel(Level_t level) throw() { m_Level = level; }

	int		getQuestLevel() const { return getLevel(); }

	// 계급. by sigi. 2002.8.30
/*	Rank_t  getRank() const throw() { return m_Rank; }
	void    setRank(Rank_t rank) throw() { m_Rank = rank; }
	RankExp_t   getRankExp() const throw() { return m_RankExp; }
	void    setRankExp(RankExp_t exp) throw() { m_RankExp = exp; }
	RankExp_t   getRankGoalExp() const throw() { return m_RankGoalExp; }
	void    setRankGoalExp(RankExp_t RankGoalExp) throw() { m_RankGoalExp = RankGoalExp; }*/


	Bonus_t getBonus() const throw() { return m_Bonus; }
	void    setBonus(Bonus_t bonus) throw() { m_Bonus = bonus; }

	SkillBonus_t	getSkillBonus() const throw() { return m_SkillBonus; }
	void			setSkillBonus( SkillBonus_t skillBonus ) throw() { m_SkillBonus = skillBonus; }

	SkillBonus_t	getSumOfUsedSkillBonus() const throw();

////////////////////////////////////////////////////
// 스킬 관련 함수
////////////////////////////////////////////////////
public:
	void addSkill(SkillType_t SkillType) throw();
    void addSkill(OustersSkillSlot* pSkillSlot) throw();
	void removeSkill(SkillType_t SkillType) throw();
	void removeCastleSkill(SkillType_t SkillType) throw();
	void removeAllCastleSkill() throw();
	bool isValidOustersSkill(SkillType_t SkillType) const throw();
	OustersSkillSlot* hasSkill(SkillType_t SkillType) const throw() { return getSkill(SkillType); }
	OustersSkillSlot* getSkill(SkillType_t SkillType) const throw();
	
	virtual bool ComputeSkillOutput(SkillType_t SkillType, SkillOutput& output);
	virtual bool GetSkillInterval(SkillType_t SkillType, int& iSkillInterval);
	virtual void ComputeAllSkillDelay();

////////////////////////////////////////////////////
// 아이템 착/탈 관련 함수
////////////////////////////////////////////////////
public:
	virtual void  wearItem(int iPart) throw(Error) { wearItem((Ousters::WearPart)iPart); }
	virtual void  wearItem(int iPart, Item* pItem) throw(Error) { wearItem((Ousters::WearPart)iPart, pItem); }

	void wearItem(WearPart Part) throw();
	void wearItem(WearPart Part, Item* pItem) throw();
	void takeOffItem(WearPart Part, bool bAddOnMouse, bool bSendModifyInfo) throw();
	
        virtual int hasTaoZhuang() throw(Error);//taozhuang 211

	// WearPart 이름은 같으나 enum 값이 종족별로 다름
	virtual int getWearPartOfLeftHand() { return WEAR_LEFTHAND; }
	virtual int getWearPartOfRightHand() { return WEAR_RIGHTHAND; }
	
	bool    addShape(Item::ItemClass IClass, ItemType_t IType, Color_t color);
    bool    removeShape(Item::ItemClass IClass, bool bSendPacket=true);
	Color_t getItemShapeColor(Item* pItem, OptionInfo* pOptionInfo=NULL) const;


	void destroyGears() throw();

	bool isRealWearing(WearPart Part) throw();
	bool isRealWearing(Item* pItem) throw();
	bool isRealWearingEx(WearPart Part) const;
	virtual DWORD sendRealWearingInfo(void) throw();

	void  getShapeInfo (DWORD& flag, Color_t color[PCOustersInfo::OUSTERS_COLOR_MAX]);

////////////////////////////////////////////////////
// 아우스터즈 종류 판별, 들고 있는 무기에 따라서
////////////////////////////////////////////////////
public:
	bool isWarrior();
	bool isFireUser();
	bool isWaterUser();
	bool isEarthUser();

////////////////////////////////////////////////////
// 아이템 착/탈시 능력치 수정 관련 함수
////////////////////////////////////////////////////
public:
	virtual void initAllStat(int numPartyMember=-1) throw();
	void initCastleSkill() throw();
	void sendModifyInfo(const OUSTERS_RECORD& previous) const throw();
	void addModifyInfo(const OUSTERS_RECORD& previous, ModifyInfo& pkt) const throw();
	void initAllStatAndSend();
	
private:
	void computeStatOffset(void) throw();
	void computeItemStat(Item* pItem) throw();
	void computeOptionStat(Item* pItem) throw();
	void computeOptionStat(OptionType_t optionType) throw();
	void computeOptionClassStat(OptionClass OClass, int PlusPoint);


//////////////////////////////////////////////////////////////
// 아이템 검색 함수
//////////////////////////////////////////////////////////////
public:

	virtual Item* findItemOID(ObjectID_t id) throw() { return ::findItemOID(this, id); }
	virtual Item* findItemOID(ObjectID_t id, int& storage, int& x, int& y) throw() { return ::findItemOID(this, id, storage, x, y); }

	virtual Item* findItemIID(ItemID_t id) throw() { return ::findItemIID(this, id); }
	virtual Item* findItemIID(ItemID_t id, int& storage, int& x, int& y) throw() { return ::findItemIID(this, id, storage, x, y); }

	virtual Item* findBeltOID(ObjectID_t id) throw() { return ::findItemOID(this, id, Item::ITEM_CLASS_BELT); }
	virtual Item* findBeltOID(ObjectID_t id, int& storage, int& x, int& y) throw() { return ::findItemOID(this, id, Item::ITEM_CLASS_BELT, storage, x, y); }

	virtual Item* findBeltIID(ItemID_t id) throw(){ return ::findItemIID(this, id, Item::ITEM_CLASS_BELT); }
	virtual Item* findBeltIID(ItemID_t id, int& storage, int& x, int& y) throw(){ return ::findItemIID(this, id, Item::ITEM_CLASS_BELT, storage, x, y); }

////////////////////////////////////////////////////
// 인포 관련 함수
////////////////////////////////////////////////////
public:

	PCOustersInfo2* getOustersInfo2() throw();
	PCOustersInfo3  getOustersInfo3() const throw();
	InventoryInfo*  getInventoryInfo() const throw();
	ExtraInfo*      getExtraInfo() const throw();
	virtual void sendSkillInfo() throw();


////////////////////////////////////////////////////
// 기타 함수
////////////////////////////////////////////////////
public:

	virtual Gold_t getGold() const throw() { return m_Gold; }
	virtual void setGold(Gold_t gold) throw();
	virtual void setGoldEx(Gold_t gold) throw();
	virtual void   increaseGoldEx(Gold_t gold) throw();
    virtual void   decreaseGoldEx(Gold_t gold) throw();


	virtual bool checkGoldIntegrity();
	virtual bool checkStashGoldIntegrity();

	Gold_t DianJuan;
	virtual Gold_t getDianJuan() const throw() { return m_DianJuan; }
	virtual void   setDianJuan(Gold_t DianJuan) throw();
	virtual void   increaseDianJuanEx(Gold_t DianJuan) throw();
	virtual void   decreaseDianJuanEx(Gold_t DianJuan) throw();

	Gold_t JiFen;
	virtual Gold_t getJiFen() const throw() { return m_JiFen; }
	virtual void   setJiFen(Gold_t JiFen) throw();
	virtual void   increaseJiFenEx(Gold_t JiFen) throw();
	virtual void   decreaseJiFenEx(Gold_t JiFen) throw();

	void heartbeat(const Timeval& currentTime) throw();

	void getOustersRecord(OUSTERS_RECORD& record) const throw();

	virtual Fame_t getFame() const throw() { return m_Fame; }
	virtual void   setFame(Fame_t fame) throw() { m_Fame = fame; }

	virtual ZoneID_t getResurrectZoneID(void) const throw() { return m_ResurrectZoneID; }
	virtual void setResurrectZoneID(ZoneID_t id) throw() { m_ResurrectZoneID = id; }
	virtual void setResurrectZoneIDEx(ZoneID_t id) throw();

	Silver_t getSilverDamage() const throw() { return m_SilverDamage; }
	void setSilverDamage(Silver_t damage) throw() { m_SilverDamage = damage; }
	void saveSilverDamage(Silver_t damage) throw();

	virtual Steal_t getHPStealAmount(void) const { return m_HPStealAmount; }
	virtual void setHPStealAmount(Steal_t steal) { m_HPStealAmount = steal; }

	virtual Steal_t getHPStealRatio(void) const { return m_HPStealRatio; }
	virtual void setHPStealRatio(Steal_t steal) { m_HPStealRatio = steal; }

	virtual Steal_t getMPStealAmount(void) const { return m_MPStealAmount; }
	virtual void setMPStealAmount(Steal_t steal) { m_MPStealAmount = steal; }

	virtual Steal_t getMPStealRatio(void) const { return m_MPStealRatio; }
	virtual void setMPStealRatio(Steal_t steal) { m_MPStealRatio = steal; }

	virtual Regen_t getHPRegen(void) const { return m_HPRegen; }
	virtual void setHPRegen(Regen_t regen) { m_HPRegen = regen; }

	virtual Regen_t getMPRegen(void) const { return m_MPRegen; }
	virtual void setMPRegen(Regen_t regen) { m_MPRegen = regen; }

	virtual Luck_t getLuck(void) const { return m_Luck; }
	virtual void setLuck(Luck_t luck) { m_Luck = luck; }

	virtual Sight_t getEffectedSight() throw();

//	WORD getRankExpSaveCount(void) const { return m_RankExpSaveCount; }
//	void setRankExpSaveCount(WORD count) { m_RankExpSaveCount = count; }

	WORD getExpSaveCount(void) const { return m_ExpSaveCount; }
	void setExpSaveCount(WORD count) { m_ExpSaveCount = count; }

	WORD getFameSaveCount(void) const { return m_FameSaveCount; }
	void setFameSaveCount(WORD count) { m_FameSaveCount = count; }

	WORD getAlignmentSaveCount(void) const { return m_AlignmentSaveCount; }
	void setAlignmentSaveCount(WORD count) { m_AlignmentSaveCount = count; }

//	void increaseOustersExp(Exp_t Point);
//	void increaseRankExp(RankExp_t Point);
//	2006.09.11
	void setGambleAccumulation(WORD accumulation) { m_GambleAccumulation = accumulation; }
	WORD getGambleAccumulation() { return m_GambleAccumulation; }
	void setGambleTryCount(WORD count) { m_GambleTryCount = count; }
	WORD getGambleTryCount() { return m_GambleTryCount; }

	void setInitialRank(void);

//////////////////////////////
// 정령 속성 관련 함수
//////////////////////////////
	Elemental_t getElementalFire() const { return m_ElementalFire; }
	Elemental_t getElementalWater() const { return m_ElementalWater; }
	Elemental_t getElementalEarth() const { return m_ElementalEarth; }
	Elemental_t getElementalWind() const { return m_ElementalWind; }
	Elemental_t getElementalSum() const { return m_ElementalFire + m_ElementalWater + m_ElementalEarth + m_ElementalWind; }

//////////////////////////////
// 정령 공격 보너스
//////////////////////////////
	Damage_t getFireDamage() const { return m_FireDamage; }
	Damage_t getWaterDamage() const { return m_WaterDamage; }
	Damage_t getEarthDamage() const { return m_EarthDamage; }

	Resist_t getSilverResist() const { return m_SilverResist; }

	bool satisfySkillRequire( SkillInfo* pSkillInfo );
	void checkReactiveArmor();
	void checkFrozenArmor();
	void checkPassiveSkill( SkillType_t skillType, Effect::EffectClass effectClass );

	void checkSkillRequiredCondition();
//////////////////////////////
// 정령 속성 관련 함수
//////////////////////////////
	bool isPassiveAvailable(SkillType_t type) { return m_PassiveSkillMap.find(type) != m_PassiveSkillMap.end() && m_PassiveSkillMap[type].first; }
	uint getPassiveBonus(SkillType_t type) { return ( m_PassiveSkillMap.find(type) != m_PassiveSkillMap.end() ) ? m_PassiveSkillMap[type].second : 0; }
	int getPassiveRatio() const { return m_PassiveRatio; }

	SkillBonus_t getSkillPointCount( ElementalDomain eDomain );
	bool canLearnSkill( SkillType_t skill );

public :
	 // by sigi. 2002.11.19
	virtual bool isPayPlayAvaiable() throw(Error);
	virtual bool canPlayFree() throw(Error);

	void	initPetQuestTarget();
	int		getBloodBibleSignOpenNum() const;

	ADDON GetAddonType(Item::ItemClass IClass);
	
	void ChangeShapeInfoWhenWear(Item *pItem);
	bool ChangeShapeInfoWhenTakeOff(Item *pItem);
	
	virtual void ChangeGearSlotID(GearSlotID_t GearSlotID);
	virtual void OnChangeItemInGearSlot(Item *pWearItem, Item *pTakeOffItem);
	
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
	
private:

	// 권한
	BYTE m_Competence;
	BYTE m_CompetenceShape;

	// Sex, Color
	Sex m_Sex;
	Color_t m_HairColor;

	SylphType m_WingSylphType;
	ItemID_t m_WingItemType;
	Color_t m_WingBodyColor;
	Color_t m_WingEffectColor;

	// Alignment
	Alignment_t m_Alignment;

	// STR/DEX/INT
	Attr_t m_STR[3];
	Attr_t m_DEX[3];
	Attr_t m_INT[3];

	// 20070918
	Defense_t    m_MagicDefense;
	ToHit_t      m_MagicToHit;

	// Blood Burst Point
	Attr_t		m_AttackBloodBurstPoint;
	Attr_t		m_DefenseBloodBurstPoint;

	// Exp/ExpOffset
//	Exp_t m_Exp;
//	Exp_t m_ExpOffset;

	// Gaol Exp
	Exp_t m_GoalExp;

	// level
	Level_t m_Level;

	// 계급. by sigi. 2002.8.30
//	Rank_t 		m_Rank;
//	RankExp_t 	m_RankExp;
//	RankExp_t 	m_RankGoalExp;

	// bonus
	Bonus_t m_Bonus;

	// Skill bonus
	SkillBonus_t m_SkillBonus;

	// gold
	Gold_t m_Gold;
	Gold_t m_DianJuan;
	Gold_t m_JiFen;

	Fame_t m_Fame;

	ZoneCoord_t m_VisionWidth;
	ZoneCoord_t m_VisionHeight;

	// SkillSlot
	//OustersSkillSlot* m_pSkillSlot[MAX_OUSTERS_SKILL_SLOT];
	map<SkillType_t, OustersSkillSlot*> m_SkillSlot;

	// Ousters Outlook Information
	mutable PCOustersInfo3 m_OustersInfo;

	// mutable Thread Mutex
	mutable Mutex m_Mutex;

	// 부활존 관련
	ZoneID_t m_ResurrectZoneID;

	// 은 도금 데미지 관련
	Silver_t m_SilverDamage;

	// HP 스틸
	Steal_t m_HPStealAmount;
	Steal_t m_HPStealRatio;

	// MP 스틸
	Steal_t m_MPStealAmount;
	Steal_t m_MPStealRatio;

	// HP, MP 재생
	Regen_t m_HPRegen;
	Regen_t m_MPRegen;

	Luck_t m_Luck;

	// 정령 속성
	Elemental_t m_ElementalFire;
	Elemental_t m_ElementalWater;
	Elemental_t m_ElementalEarth;
	Elemental_t m_ElementalWind;

	// 정력 공격 보너스 포인트
	Damage_t m_FireDamage;
	Damage_t m_WaterDamage;
	Damage_t m_EarthDamage;

	// 은 저항
	Resist_t m_SilverResist;

	map<SkillType_t, pair<bool, uint> > m_PassiveSkillMap;
	int m_PassiveRatio;

	// 각종 경험치 세이브 카운트
//	WORD m_RankExpSaveCount;
	WORD m_ExpSaveCount;
	WORD m_FameSaveCount;
	WORD m_AlignmentSaveCount;

	Timeval m_MPRegenTime;

	GCAddOusters* m_pAddPacket;

	// 2006.09.11 겜블 정보 추가
	WORD m_GambleTryCount;
	WORD m_GambleAccumulation;

	OustersTaoZhuangItem MyTaoZhuang;
	ListItem m_TaoZhuang;
};


#endif // __OUSTERS_H__
