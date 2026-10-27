//////////////////////////////////////////////////////////////////////////////
// Filename    : PCOustersInfo2.h
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __PC_OUSTERS_INFO_2_H__
#define __PC_OUSTERS_INFO_2_H__

#include "PCInfo.h"

//////////////////////////////////////////////////////////////////////////////
// Ousters 정보를 담고 있는 객체.
// GCPCList 패킷에 담겨서 클라이언트에게 전송된다.
// 아이템이나 걸려있는 마법 같은 정보는 담겨있지 않다.
//////////////////////////////////////////////////////////////////////////////

class PCOustersInfo2 : public PCInfo 
{
public:
	PCType getPCType () const  { return PC_OUSTERS; }

	void read (SocketInputStream & iStream) ;
	void write (SocketOutputStream & oStream) const ;

	uint getSize () const 
	{
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
		uint size;
		
		size = szObjectID					// ObjectID
			+ szBYTE + m_Name.size() 		// 아우스터스 이름
			+ szLevel						// 레벨
			+ szSex							// 성별
			+ szBYTE;						// Slyph Type
		
		switch( m_WingSylphType )
		{
		case 2 : // SLYPH_TYPE_WING
#if __CONTENTS(__SECOND_TRANSFORTER)
			size += szItemType;				// Wing ItemType
			size += szColor * 2;
#endif //__SECOND_TRANSFORTER
			break;
		case 1 : // SLYPH_TYPE_LEGACY
		case 0 : // SLYPH_TYPE_NONE
		default :
			break;
		}
		
		size += szColor						// 머리 색상
			+ szWORD						// 마스터 이펙트1
			+ szBYTE						// 출신
			+ szAlignment					// 성향
			+ szAttr * 3 * 3				// 능력치
			+ szHP* 2						// HP
			+ szMP* 2						// MP
			+ szRank + szRankExp			// 계급 경험치
			+ szExp							// 경험치
			+ szGold 						// 돈
			+ szFame						// Fame
			+ szSight						// 시야
			+ szBonus						// 보너스 포인트
			+ szSkillBonus					// 스킬 보너스 포인트
			+ szSilver						// 실버 데미지
			+ szBYTE						// 권한
			+ szGuildID						// 길드 아이디
			+ szBYTE + m_GuildName.size()	// 길드 이름
			+ szGuildMemberRank				// guild member rank
			+ szuint
			+ szLevel
			+ szExp
			+ szAttr
			+ szAttr
			+ szAttr
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
			+ szDWORD // 기여도
#endif
#if __CONTENTS(__ATTACK_SPEED)
			+ szSpeed
#endif //__ATTACK_SPEED
			+ szGold 						// 듐얩
			+ szGold						//생롸
			;
		
		return size;
#else
		return szObjectID					// ObjectID
			+ szBYTE + m_Name.size() 		// 아우스터스 이름
			+ szLevel						// 레벨
			+ szSex							// 성별
			+ szColor						// 머리 색상
			+ szWORD						// 마스터 이펙트1
			+ szBYTE						// 출신
			+ szAlignment					// 성향
			+ szAttr * 3 * 3				// 능력치
			+ szHP* 2						// HP
			+ szMP* 2						// MP
			+ szRank + szRankExp			// 계급 경험치
			+ szExp							// 경험치
			+ szGold 						// 돈
			+ szFame						// Fame
			+ szSight						// 시야
			+ szBonus						// 보너스 포인트
			+ szSkillBonus					// 스킬 보너스 포인트
			+ szSilver						// 실버 데미지
			+ szBYTE						// 권한
			+ szGuildID						// 길드 아이디
			+ szBYTE + m_GuildName.size()	// 길드 이름
			+ szGuildMemberRank				// guild member rank
			+ szuint
			+ szLevel
			+ szExp
			+ szAttr
			+ szAttr
			+ szAttr
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
			+szDWORD
#endif //__CONTRIBUTE_SYSTEM
#if __CONTENTS(__ATTACK_SPEED)
			+ szSpeed
#endif //__ATTACK_SPEED
			+ szGold 						// 듐얩
			+ szGold						//생롸
			;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	}
	
	static uint getMaxSize () 
	{
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
		uint size;
		
		size = szObjectID				// ObjectID
			+ szBYTE + 20				// 아우스터스 이름
			+ szLevel					// 레벨
			+ szSex						// 성별
			+ szBYTE					// Slyph Type
#if __CONTENTS(__SECOND_TRANSFORTER)
			+ szItemType					// Wing ItemType
#endif //__SECOND_TRANSFORTER
			+ szColor * 2
			
			+ szColor					// 색상
			+ szWORD					// 마스터 이펙트
			+ szBYTE					// 출신
			+ szAlignment				// 성향
			+ szAttr * 3 * 3			// 능력치
			+ szHP* 2					// HP
			+ szMP* 2					// MP
			+ szRank + szRankExp		// 계급 경험치
			+ szExp						// 경험치
			+ szGold					// 돈
			+ szFame					// Fame
			+ szSight					// 시야
			+ szBonus					// 보너스 포인트
			+ szSkillBonus				// 스킬 보너스 포인트
			+ szSilver					// 실버 데미지
			+ szBYTE					// 권한
			+ szGuildID					// 길드 아이디
			+ szBYTE + 30				// 길드 이름
			+ szGuildMemberRank	 		// guild member rank
			+ szuint
			+ szLevel
			+ szExp
			+ szAttr
			+ szAttr
			+ szAttr
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
			+ szDWORD // 기여도
#endif
#if __CONTENTS(__ATTACK_SPEED)
			+ szSpeed
#endif //__ATTACK_SPEED
			+ szGold					//듐얩
			+ szGold					//생롸
			;
		
		return size;

#else
		return szObjectID				// ObjectID
			+ szBYTE + 20				// 아우스터스 이름
			+ szLevel					// 레벨
			+ szSex						// 성별
			+ szColor					// 색상
			+ szWORD					// 마스터 이펙트
			+ szBYTE					// 출신
			+ szAlignment				// 성향
			+ szAttr * 3 * 3			// 능력치
			+ szHP* 2					// HP
			+ szMP* 2					// MP
			+ szRank + szRankExp		// 계급 경험치
			+ szExp						// 경험치
			+ szGold					// 돈
			+ szFame					// Fame
			+ szSight					// 시야
			+ szBonus					// 보너스 포인트
			+ szSkillBonus				// 스킬 보너스 포인트
			+ szSilver					// 실버 데미지
			+ szBYTE					// 권한
			+ szGuildID					// 길드 아이디
			+ szBYTE + 30				// 길드 이름
			+ szGuildMemberRank	 		// guild member rank
			+ szuint
			+ szLevel
			+ szExp
			+ szAttr
			+ szAttr
			+ szAttr
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
			+szDWORD
#endif //__CONTRIBUTE_SYSTEM
#if __CONTENTS(__ATTACK_SPEED)
			+ szSpeed
#endif //__ATTACK_SPEED
			+ szGold					//듐얩
			+ szGold					//생롸
			;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
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
	
	Color_t getHairColor () const  { return m_HairColor; }
	void setHairColor (Color_t hairColor)  { m_HairColor = hairColor; }
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	BYTE getWingSylphType() { return m_WingSylphType; }
	void setWingSylphType(BYTE Type) { m_WingSylphType = Type; }

#if __CONTENTS(__SECOND_TRANSFORTER)
	BYTE getWingItemType() { return m_WingItemType; }
	void setWingItemType(ItemID_t ItemType) { m_WingItemType = ItemType; }
#endif //__SECOND_TRANSFORTER

	Color_t getWingBodyColor() { return m_WingBodyColor; }
	void setWingBodyColor(Color_t Color) { m_WingBodyColor = Color; }

	Color_t getWingEffectColor() { return m_WingEffectColor; }
	void setWingEffectColor(Color_t Color) { m_WingEffectColor = Color; }
#endif//__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	WORD getMasterEffectColor () const  { return m_MasterEffectColor; }
	void setMasterEffectColor (WORD mastercolor)  { m_MasterEffectColor = mastercolor; }

	BYTE getBorn() const  { return m_Born; }
	void setBorn( BYTE born )  { m_Born = born; }

	Alignment_t getAlignment() const  { return m_Alignment; }
	void setAlignment(Alignment_t Alignment)  { m_Alignment = Alignment; }

	Attr_t getSTR (AttrType attrType = ATTR_CURRENT) const 
	{
		if (m_STR[attrType] > maxOustersAttr) throw Error("STR out of range"); 
		return m_STR[attrType]; 
	}
	void setSTR (Attr_t str, AttrType attrType = ATTR_CURRENT) 
	{ 
		if (str > maxOustersAttr) throw Error("STR out of range"); 
		m_STR[attrType] = str; 
	}
	
	Attr_t getDEX (AttrType attrType = ATTR_CURRENT) const 
	{ 
		if (m_DEX[attrType] > maxOustersAttr) throw Error("DEX out of range"); 
		return m_DEX[attrType]; 
	}
	void setDEX (Attr_t dex, AttrType attrType = ATTR_CURRENT) 
	{ 
		if (dex > maxOustersAttr) throw Error("DEX out of range"); 
		m_DEX[attrType] = dex; 
	}
	
	Attr_t getINT (AttrType attrType = ATTR_CURRENT) const 
	{ 
		if (m_INT[attrType] > maxOustersAttr) throw Error("INT out of range"); 
		return m_INT[attrType]; 
	}
	void setINT (Attr_t inte, AttrType attrType = ATTR_CURRENT) 
	{
		if (inte > maxOustersAttr) throw Error("INT out of range"); 
		m_INT[attrType] = inte; 
	}

	HP_t getHP (AttrType attrType = ATTR_CURRENT) const  { return m_HP[attrType]; }
	void setHP (HP_t hp, AttrType attrType = ATTR_CURRENT)  { m_HP[attrType] = hp; }
	void setHP (HP_t curHP, HP_t maxHP)  { m_HP[ATTR_CURRENT] = curHP; m_HP[ATTR_MAX] = maxHP; }

	HP_t getMP (AttrType attrType = ATTR_CURRENT) const  { return m_MP[attrType]; }
	void setMP (MP_t mp, AttrType attrType = ATTR_CURRENT)  { m_MP[attrType] = mp; }
	void setMP (MP_t curMP, MP_t maxMP)  { m_MP[ATTR_CURRENT] = curMP; m_MP[ATTR_MAX] = maxMP; }

	Rank_t getRank () const  { return m_Rank; }
	void setRank (Rank_t rank)  { m_Rank = rank; }

	RankExp_t getRankExp () const  { return m_RankExp; }
	void setRankExp (RankExp_t rankExp)  { m_RankExp = rankExp; }

	Exp_t getExp () const  { return m_Exp; }
	void setExp (Exp_t exp)  { m_Exp = exp; }

	Gold_t getGold () const  { return m_Gold; }
	void setGold (Gold_t gold)  { m_Gold = gold; }

	Gold_t getDianJuan() const  { return m_DianJuan; }
	void setDianJuan (Gold_t gold)  { m_DianJuan = gold; }

	//생롸
	Gold_t getJiFen() const  { return m_JiFen; }
	void setJiFen(Gold_t jifen)  { m_JiFen = jifen; }

	Sight_t getSight () const  { return m_Sight; }
	void setSight (Sight_t sight)  { m_Sight = sight; }

	Bonus_t getBonus() const  { return m_Bonus; }
	void setBonus(Bonus_t Bonus)  { m_Bonus = Bonus; }

	SkillBonus_t getSkillBonus() const  { return m_SkillBonus; }
	void setSkillBonus(SkillBonus_t skillBonus)  { m_SkillBonus = skillBonus; }

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
	void setContributePoint(int point) { m_ContributePoint = point;}
#endif //__CONTRIBUTE_SYSTEM

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

	// hair color, skin color,
	Color_t m_HairColor;
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	BYTE m_WingSylphType;

#if __CONTENTS(__SECOND_TRANSFORTER)
	ItemID_t m_WingItemType;
#endif //__SECOND_TRANSFORTER

	Color_t m_WingBodyColor;
	Color_t m_WingEffectColor;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER

	WORD m_MasterEffectColor;

	// 출신
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

	// MP
	// MP[0] = current mp, mp[1] == max mp
	MP_t m_MP[2];

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

	// 스킬 보너스 포인트
	SkillBonus_t m_SkillBonus;

	// 실버 데미지
	Silver_t m_SilverDamage;

	// Competence
	BYTE m_Competence;

	// 길드 ID
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
