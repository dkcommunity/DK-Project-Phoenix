//////////////////////////////////////////////////////////////////////////////
// Filename    : PCVampireInfo2.h
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __PC_VAMPIRE_INFO_2_H__
#define __PC_VAMPIRE_INFO_2_H__

#include "PCInfo.h"

//////////////////////////////////////////////////////////////////////////////
// Vampire 정보를 담고 있는 객체.
// GCPCList 패킷에 담겨서 클라이언트에게 전송된다.
// 아이템이나 걸려있는 마법 같은 정보는 담겨있지 않다.
//////////////////////////////////////////////////////////////////////////////

class PCVampireInfo2 : public PCInfo 
{
public:
	PCType getPCType () const  { return PC_VAMPIRE; }

	void read (SocketInputStream & iStream) ;
	void write (SocketOutputStream & oStream) const ;

	uint getSize () const 
	{
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
				uint size;

		size = szObjectID				// ObjectID
			+ szBYTE + m_Name.size() 	// 뱀파이어 이름
			+ szLevel					// 레벨
			+ szSex;					// 성별
		
		size += sizeof(uchar);

		switch(m_BatType)
		{
			case 2 : // Vampire::BAT_TYPE_WING
#if __CONTENTS(__SECOND_TRANSFORTER)
				size += szItemType;					// WingItemType
#endif //__SECOND_TRANSFORTER
				size += szColor* 2;				// 색상
				break;
			case 1 : // Vampire::BAT_TYPE_LEGACY
				size += szColor* 2;				// 색상
				break;
			case 0 : // Vampire::BAT_TYPE_NONE
			default :
				break;;
		}

		size += szWORD					// 마스터 이펙트1
			+ szBYTE					// 출신
			+ szAlignment				// 성향
			+ szAttr * 3 * 3			// 능력치
			+ szHP* 2					// HP
			+ szRank + szRankExp		// 계급 경험치
			+ szExp						// 경험치
			+ szFame					// Fame
			+ szGold 					// 돈
			+ szSight					// 시야
			+ szSkillType* 8			// 핫 키
			+ szSilver					// 핫 키
			+ szBonus					// 보너스포인트
			+ szGuildID					// 길드 아이디
			+ szBYTE + m_GuildName.size()	// 길드 이름
			+ szGuildMemberRank			// guild member rank
			+ szBYTE					// 권한
			+ szuint
			+ szLevel
			+ szExp
			+ szAttr
			+ szAttr
			+ szAttr
			+ szColor
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
			+ szDWORD // 기여도
#endif
#if __CONTENTS(__ATTACK_SPEED)
			+ szSpeed
#endif //__ATTACK_SPEED
			+ szGold 					// 듐얩
			+ szGold					//생롸
			;

		return size;

#else
		return szObjectID				// ObjectID
			+ szBYTE + m_Name.size() 	// 뱀파이어 이름
			+ szLevel					// 레벨
			+ szSex						// 성별
			+ szColor* 3				// 색상
			+ szWORD					// 마스터 이펙트
			+ szBYTE					// 출신
			+ szAlignment				// 성향
			+ szAttr * 3 * 3			// 능력치
			+ szHP* 2					// HP
			+ szRank + szRankExp		// 계급 경험치
			+ szExp						// 경험치
			+ szFame					// Fame
			+ szGold 					// 돈
			+ szSight					// 시야
			+ szSkillType* 8			// 핫 키
			+ szSilver					// 핫 키
			+ szBonus					// 보너스포인트
			+ szGuildID					// 길드 아이디
			+ szBYTE + m_GuildName.size()	// 길드 이름
			+ szGuildMemberRank			// guild member rank
			+ szBYTE					// 권한
			+ szuint
			+ szLevel
			+ szExp
			+ szAttr
			+ szAttr
			+ szAttr
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
			+ szDWORD // 기여도
#endif //__CONTRIBUTE_SYSTEM
#if __CONTENTS(__ATTACK_SPEED)
			+ szSpeed
#endif //__ATTACK_SPEED
			+ szGold 					// 듐얩
			+ szGold					//생롸
			;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	}

	static uint getMaxSize () 
	{
		return szObjectID				// ObjectID
			+ szBYTE + 20				// 뱀파이어 이름
			+ szLevel					// 레벨
			+ szSex						// 성별
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
			+ sizeof(uchar)				// BatType
#if __CONTENTS(__SECOND_TRANSFORTER)
			+ szItemType				// WingItemType
#endif //__SECOND_TRANSFORTER
			+ szColor* 2				// 색상
#else
			+ szColor* 3				// 색상
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
			+ szWORD					// 마스터 이펙트
			+ szBYTE					// 출신
			+ szAlignment				// 성향
			+ szAttr * 3 * 3			// 능력치
			+ szHP* 2					// HP
			+ szRank + szRankExp		// 계급 경험치
			+ szExp						// 경험치
			+ szFame					// Fame
			+ szGold					// 돈
			+ szSight					// 시야
			+ szSkillType* 8			// 핫 키
			+ szSilver					// 핫 키
			+ szBonus					// 보너스포인트
			+ szGuildID					// 길드 아이디
			+ szBYTE + 30				// 길드 이름
			+ szGuildMemberRank +		// guild member rank
			+ szBYTE					// 권한
			+ szuint
			+ szLevel
			+ szExp
			+ szAttr
			+ szAttr
			+ szAttr
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
			+ szDWORD // 기여도
#endif //__CONTRIBUTE_SYSTEM
#if __CONTENTS(__ATTACK_SPEED)
			+ szSpeed
#endif //__ATTACK_SPEED
			+ szGold 					//듐얩
			+ szGold					//생롸
			;
	}

	std::string toString () const ;

public:
	ObjectID_t getObjectID () const  { return m_ObjectID; }
	void setObjectID (ObjectID_t objectID)  { m_ObjectID = objectID; }

    const std::string& getName () const  { return m_Name; }
    void setName (std::string name)  { m_Name = (name.size() > 20) ? name.substr(0,20) : name; }

    Level_t getLevel () const  { return m_Level; }
    void setLevel (Level_t Level)  { m_Level = Level; }

	Sex getSex () const  { return m_Sex; }
	void setSex (Sex sex)  { m_Sex = sex; }
	void setSex (std::string sex) 
	{
		if (sex == Sex2String[MALE]) 
			m_Sex = MALE;
		else if (sex == Sex2String[FEMALE]) 
			m_Sex = FEMALE;
		else
			throw InvalidProtocolException("invalid sex value");
	}

	Fame_t getFame () const  { return m_Fame; }
	void setFame (Fame_t f)  { m_Fame = f; }
	
	Color_t getBatColor () const  { return m_BatColor; }
	void setBatColor (Color_t batColor)  { m_BatColor = batColor; }

	Color_t getAdvanceBatColor () const  { return m_AdvanceBatColor; }
	void setAdvanceBatColor (Color_t advanceBatColor)  { m_AdvanceBatColor = advanceBatColor; }

	Color_t getSkinColor () const  { return m_SkinColor; }
	void setSkinColor (Color_t skinColor)  { m_SkinColor = skinColor; }

	WORD getMasterEffectColor () const  { return m_MasterEffectColor; }
	void setMasterEffectColor (WORD mastercolor)  { m_MasterEffectColor = mastercolor; }

	BYTE getBorn() const  { return m_Born; }
	void setBorn( BYTE born )  { m_Born = born; }

	Alignment_t getAlignment() const  { return m_Alignment; }
	void setAlignment(Alignment_t Alignment)  { m_Alignment = Alignment; }

	Attr_t getSTR (AttrType attrType = ATTR_CURRENT) const 
	{
		if (m_STR[attrType] > maxVampireAttr) throw Error("STR out of range"); 
		return m_STR[attrType]; 
	}
	void setSTR (Attr_t str, AttrType attrType = ATTR_CURRENT) 
	{ 
		if (str > maxVampireAttr) throw Error("STR out of range"); 
		m_STR[attrType] = str; 
	}
	
	Attr_t getDEX (AttrType attrType = ATTR_CURRENT) const 
	{ 
		if (m_DEX[attrType] > maxVampireAttr) throw Error("DEX out of range"); 
		return m_DEX[attrType]; 
	}
	void setDEX (Attr_t dex, AttrType attrType = ATTR_CURRENT) 
	{ 
		if (dex > maxVampireAttr) throw Error("DEX out of range"); 
		m_DEX[attrType] = dex; 
	}
	
	Attr_t getINT (AttrType attrType = ATTR_CURRENT) const 
	{ 
		if (m_INT[attrType] > maxVampireAttr) throw Error("INT out of range"); 
		return m_INT[attrType]; 
	}
	void setINT (Attr_t inte, AttrType attrType = ATTR_CURRENT) 
	{
		if (inte > maxVampireAttr) throw Error("INT out of range"); 
		m_INT[attrType] = inte; 
	}

	HP_t getHP (AttrType attrType = ATTR_CURRENT) const  { return m_HP[attrType]; }
	void setHP (HP_t hp, AttrType attrType = ATTR_CURRENT)  { m_HP[attrType] = hp; }
	void setHP (HP_t curHP, HP_t maxHP)  { m_HP[ATTR_CURRENT] = curHP; m_HP[ATTR_MAX] = maxHP; }

	Rank_t getRank () const  { return m_Rank; }
	void setRank (Rank_t rank)  { m_Rank = rank; }

	RankExp_t getRankExp () const  { return m_RankExp; }
	void setRankExp (RankExp_t rankExp)  { m_RankExp = rankExp; }

	Exp_t getExp () const  { return m_Exp; }
	void setExp (Exp_t exp)  { m_Exp = exp; }

	Gold_t getGold () const  { return m_Gold; }
	void setGold (Gold_t gold)  { m_Gold = gold; }

	Gold_t getDianJuan () const  { return m_DianJuan; }
	void setDianJuan (Gold_t gold)  { m_DianJuan = gold; }

	//생롸
	Gold_t getJiFen () const  { return m_JiFen; }
	void setJiFen (Gold_t jifen)  { m_JiFen = jifen; }

	Sight_t getSight () const  { return m_Sight; }
	void setSight (Sight_t sight)  { m_Sight = sight; }

	Bonus_t getBonus() const  { return m_Bonus; }
	void setBonus(Bonus_t Bonus)  { m_Bonus = Bonus; }

	SkillType_t getHotKey(BYTE pos) const  { return m_HotKey[pos]; }
	void setHotKey(BYTE pos, SkillType_t SkillType)  { m_HotKey[pos] = SkillType; }

	Silver_t getSilverDamage() const  { return m_SilverDamage; }
	void setSilverDamage(Silver_t SilverDamage )  { m_SilverDamage = SilverDamage; }

	BYTE getCompetence(void) const { return m_Competence; }
	void setCompetence(BYTE competence) { m_Competence = competence; }

	GuildID_t getGuildID(void) const { return m_GuildID; }
	void setGuildID(GuildID_t GuildID) { m_GuildID = GuildID; }

	const std::string& getGuildName() const { return m_GuildName; }
	void setGuildName( std::string guildName ) { m_GuildName = guildName; }

	GuildMemberRank_t getGuildMemberRank() const { return m_GuildMemberRank; }
	void setGuildMemberRank( GuildMemberRank_t guildMemberRank ) { m_GuildMemberRank = guildMemberRank; }

	uint getUnionID(void) const { return m_UnionID; }
	void setUnionID(uint UnionID) { m_UnionID = UnionID; }

	Level_t	getAdvancementLevel() const { return m_AdvancementLevel; }
	void setAdvancementLevel( Level_t level ) { m_AdvancementLevel = level; }

	Exp_t	getAdvancementGoalExp() const { return m_AdvancementGoalExp; }
	void setAdvancementGoalExp( Exp_t exp ) { m_AdvancementGoalExp = exp; }

	Attr_t getAttackBloodBurstPoint() const { return m_AttackBloodBurstPoint; }
	void setAttackBloodBurstPoint( Attr_t point ) { m_AttackBloodBurstPoint = point; }

	Attr_t getDefenseBloodBurstPoint() const { return m_DefenseBloodBurstPoint; }
	void setDefenseBloodBurstPoint( Attr_t point ) { m_DefenseBloodBurstPoint = point; }

	Attr_t getPartyBloodBurstPoint() const { return m_PartyBloodBurstPoint; }
	void setPartyBloodBurstPoint( Attr_t point ) { m_PartyBloodBurstPoint = point; }

#if __CONTENTS(__CONTRIBUTE_SYSTEM)
	int getContributePoint() { return m_ContributePoint; }
	void setContributePoint(int point) { m_ContributePoint = point; }
#endif //__CONTRIBUTE_SYSTEM
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	uchar GetBatType() { return m_BatType; }
	void SetBatType(uchar Type) { m_BatType = Type; }

#if __CONTENTS(__SECOND_TRANSFORTER)
	ItemID_t getWingItemType() { return m_WingItemType; }
	void setWingItemType(ItemID_t ItemType) { m_WingItemType = ItemType; }
#endif //__SECOND_TRANSFORTER

	Color_t GetWingColor1() { return m_WingColor1; }
	void SetWingColor1(Color_t Color) { m_WingColor1 = Color; }

	Color_t GetWingColor2() { return m_WingColor2; }
	void SetWingColor2(Color_t Color) { m_WingColor2 = Color; }
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
#if __CONTENTS(__ATTACK_SPEED)
	BYTE getAttackSpeed() { return m_AttackSpeed; }
	void setAttackSpeed(int attackSpeedpoint) { m_AttackSpeed = attackSpeedpoint; }
#endif //__ATTACK_SPEED

private:

	// PC's object id
	ObjectID_t m_ObjectID;

	// PC name
	std::string m_Name;

	// PC name
	Level_t m_Level;

	// PC sex
	Sex m_Sex;

	// bat color, skin color,
	Color_t m_BatColor;
	Color_t m_AdvanceBatColor;
	Color_t m_SkinColor;
	WORD m_MasterEffectColor;
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	uchar m_BatType;

#if __CONTENTS(__SECOND_TRANSFORTER)
	ItemID_t m_WingItemType;
#endif //__SECOND_TRANSFORTER

	Color_t m_WingColor1;
	Color_t m_WingColor2;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER

	// 출신지
	BYTE m_Born;

	// Alignment
	Alignment_t m_Alignment;

    // Attributes
	Attr_t m_STR[3];
	Attr_t m_DEX[3];
	Attr_t m_INT[3];

	// HP
	// HP[0] = current hp, hp[1] == max hp
	HP_t m_HP[2];

	// 계급
	Rank_t			m_Rank;
	RankExp_t		m_RankExp;

	// Exp
	Exp_t m_Exp;

	// Fame
	Fame_t m_Fame;

	// Gold
	Gold_t m_Gold;

	// 시야
	Sight_t m_Sight;

	// 보너스 포인트
	Bonus_t m_Bonus;

	// 핫 키
	SkillType_t m_HotKey[8];

	// 실버 데미지
	Silver_t m_SilverDamage;

	// Competence
	BYTE m_Competence;

	GuildID_t m_GuildID;

	// guild name
	std::string m_GuildName;

	// guild member rank
	GuildMemberRank_t m_GuildMemberRank;

	uint m_UnionID;

	Level_t m_AdvancementLevel;
	Exp_t m_AdvancementGoalExp;

	// Blood Burst Point
	Attr_t	m_AttackBloodBurstPoint;
	Attr_t	m_DefenseBloodBurstPoint;
	Attr_t	m_PartyBloodBurstPoint;

#if __CONTENTS(__CONTRIBUTE_SYSTEM)
	int m_ContributePoint;
#endif //__CONTRIBUTE_SYSTEM

#if __CONTENTS(__ATTACK_SPEED)
	BYTE	m_AttackSpeed;
#endif //__ATTACK_SPEED
	Gold_t m_DianJuan;
	Gold_t m_JiFen;
};

#endif
