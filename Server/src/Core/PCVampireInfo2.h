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
	PCType getPCType () const throw () { return PC_VAMPIRE; }

	void read (SocketInputStream & iStream) throw (ProtocolException, Error);
	void write (SocketOutputStream & oStream) const throw (ProtocolException, Error);

	uint getSize () const throw ()
	{
		uint size;

		size = szObjectID				// ObjectID
			+ szBYTE + m_Name.size() 	// 뱀파이어 이름
			+ szLevel					// 레벨
			+ szSex;					// 성별
		
		size += szBYTE;

		switch(m_BatType)
		{
			case 2 : // Vampire::BAT_TYPE_WING
				size += szItemType;					// WingItemType
				size += szColor * 2;				// 색상
				break;
			case 1 : // Vampire::BAT_TYPE_LEGACY
				size += szColor * 2;				// 색상
				break;
			case 0 : // Vampire::BAT_TYPE_NONE
			default :
				break;;
		}

		size +=	szColor
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
#ifdef __CONTRIBUTION_SYSTEM__
			+ szDWORD // 기여도
#endif
			//20090601 ksym555
			+ szSpeed
			+ szGold    
			+ szGold  
			;

		return size;
	}

	static uint getMaxSize () throw ()
	{
		return szObjectID				// ObjectID
			+ szBYTE + 20				// 뱀파이어 이름
			+ szLevel					// 레벨
			+ szSex						// 성별

			+ szBYTE					// BatType
			+ szItemType				// WingItemType
			+ szColor* 2				// 색상

			+ szColor
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
#ifdef __CONTRIBUTION_SYSTEM__
			+ szDWORD // 기여도
#endif
			//20090601 ksym555
			+ szSpeed
			+ szGold 
			+ szGold    
			;
	}

	string toString () const throw ();

public:
	ObjectID_t getObjectID () const throw () { return m_ObjectID; }
	void setObjectID (ObjectID_t objectID) throw () { m_ObjectID = objectID; }

    string getName () const throw () { return m_Name; }
    void setName (string name) throw () { m_Name = (name.size() > 20) ? name.substr(0,20) : name; }

    Level_t getLevel () const throw () { return m_Level; }
    void setLevel (Level_t Level) throw () { m_Level = Level; }

	Sex getSex () const throw () { return m_Sex; }
	void setSex (Sex sex) throw () { m_Sex = sex; }
	void setSex (string sex) throw (InvalidProtocolException)
	{
		if (sex == Sex2String[MALE]) 
			m_Sex = MALE;
		else if (sex == Sex2String[FEMALE]) 
			m_Sex = FEMALE;
		else
			throw InvalidProtocolException("invalid sex value");
	}

	Fame_t getFame () const throw () { return m_Fame; }
	void setFame (Fame_t f) throw () { m_Fame = f; }
	
	Color_t getBatColor () const throw () { return m_BatColor; }
	void setBatColor (Color_t batColor) throw () { m_BatColor = batColor; }

	// 2007.07.24 승직 박쥐 색깔
	Color_t getAdvanceBatColor () const throw () { return m_AdvanceBatColor; }
	void setAdvanceBatColor (Color_t batColor) throw () { m_AdvanceBatColor = batColor; }

	Color_t getSkinColor () const throw () { return m_SkinColor; }
	void setSkinColor (Color_t skinColor) throw () { m_SkinColor = skinColor; }

	WORD getMasterEffectColor () const throw () { return m_MasterEffectColor; }
	void setMasterEffectColor (WORD color) throw () { m_MasterEffectColor = color; }

	BYTE getBatType() { return m_BatType; }
	void setBatType(uchar Type) { m_BatType = Type; }

	ItemID_t getWingItemType() { return m_WingItemType; }
	void setWingItemType(ItemID_t ItemType) { m_WingItemType = ItemType; }
		
	Color_t getWingBodyColor() { return m_WingBodyColor; }
	void setWingBodyColor(Color_t Color) { m_WingBodyColor = Color; }

	Color_t getWingEffectColor() { return m_WingEffectColor; }
	void setWingEffectColor(Color_t Color) { m_WingEffectColor = Color; }

	BYTE getBorn() const throw() { return m_Born; }
	void setBorn( BYTE born ) throw() { m_Born = born; }

	Alignment_t getAlignment() const throw() { return m_Alignment; }
	void setAlignment(Alignment_t Alignment) throw() { m_Alignment = Alignment; }

	Attr_t getSTR (AttrType attrType = ATTR_CURRENT)
	{
		if (m_STR[attrType] > maxVampireAttr) m_STR[attrType] = maxVampireAttr; 
		return m_STR[attrType]; 
	}
	void setSTR (Attr_t str, AttrType attrType = ATTR_CURRENT)
	{ 
		if (str > maxVampireAttr) str = maxVampireAttr; 
		m_STR[attrType] = str; 
	}
	
	Attr_t getDEX (AttrType attrType = ATTR_CURRENT)
	{ 
		if (m_DEX[attrType] > maxVampireAttr) m_DEX[attrType] = maxVampireAttr; 
		return m_DEX[attrType]; 
	}
	void setDEX (Attr_t dex, AttrType attrType = ATTR_CURRENT)
	{ 
		if (dex > maxVampireAttr) dex = maxVampireAttr; 
		m_DEX[attrType] = dex; 
	}
	
	Attr_t getINT (AttrType attrType = ATTR_CURRENT)
	{ 
		if (m_INT[attrType] > maxVampireAttr) m_INT[attrType] = maxVampireAttr; 
		return m_INT[attrType]; 
	}
	void setINT (Attr_t inte, AttrType attrType = ATTR_CURRENT)
	{
		if (inte > maxVampireAttr) inte = maxVampireAttr; 
		m_INT[attrType] = inte; 
	}

	HP_t getHP (AttrType attrType = ATTR_CURRENT) const throw () { return m_HP[attrType]; }
	void setHP (HP_t hp, AttrType attrType = ATTR_CURRENT) throw () { m_HP[attrType] = hp; }
	void setHP (HP_t curHP, HP_t maxHP) throw () { m_HP[ATTR_CURRENT] = curHP; m_HP[ATTR_MAX] = maxHP; }

	Rank_t getRank () const throw () { return m_Rank; }
	void setRank (Rank_t rank) throw () { m_Rank = rank; }

	RankExp_t getRankExp () const throw () { return m_RankExp; }
	void setRankExp (RankExp_t rankExp) throw () { m_RankExp = rankExp; }

	Exp_t getExp () const throw () { return m_Exp; }
	void setExp (Exp_t exp) throw () { m_Exp = exp; }

	Gold_t getGold () const throw () { return m_Gold; }
	void setGold (Gold_t gold) throw () { m_Gold = gold; }

	Gold_t getDianJuan () const throw () { return m_DianJuan; }
	void setDianJuan (Gold_t DianJuan) throw () { m_DianJuan = DianJuan; }

	Gold_t getJiFen () const throw () { return m_JiFen; }
	void setJiFen (Gold_t JiFen) throw () { m_JiFen = JiFen; }

	Sight_t getSight () const throw () { return m_Sight; }
	void setSight (Sight_t sight) throw () { m_Sight = sight; }

	Bonus_t getBonus() const throw() { return m_Bonus; }
	void setBonus(Bonus_t Bonus) throw() { m_Bonus = Bonus; }

	SkillType_t getHotKey(BYTE pos) const throw() { return m_HotKey[pos]; }
	void setHotKey(BYTE pos, SkillType_t SkillType) throw() { m_HotKey[pos] = SkillType; }

	Silver_t getSilverDamage() const throw() { return m_SilverDamage; }
	void setSilverDamage(Silver_t SilverDamage ) throw() { m_SilverDamage = SilverDamage; }

	BYTE getCompetence(void) const { return m_Competence; }
	void setCompetence(BYTE competence) { m_Competence = competence; }

	GuildID_t getGuildID(void) const { return m_GuildID; }
	void setGuildID(GuildID_t GuildID) { m_GuildID = GuildID; }

	string getGuildName() const { return m_GuildName; }
	void setGuildName( string guildName ) { m_GuildName = guildName; }

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

	Speed_t getAttackSpeed() const throw() { return m_AttackSpeed; }
	void setAttackSpeed(Speed_t AttackSpeed) throw() { m_AttackSpeed = AttackSpeed; }
	
private:

	// PC's object id
	ObjectID_t m_ObjectID;

	// PC name
	string m_Name;

	// PC name
	Level_t m_Level;

	// PC sex
	Sex m_Sex;

	// bat color, skin color,
	Color_t m_BatColor;
	Color_t m_AdvanceBatColor; // 2007.07.24 승직 박쥐 색깔
	Color_t m_SkinColor;

	BYTE m_BatType;
	ItemID_t m_WingItemType;
	Color_t m_WingBodyColor;
	Color_t m_WingEffectColor;

	WORD m_MasterEffectColor;

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
	Gold_t m_DianJuan;
	Gold_t m_JiFen;

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
	string m_GuildName;

	// guild member rank
	GuildMemberRank_t m_GuildMemberRank;

	uint m_UnionID;

	Level_t m_AdvancementLevel;
	Exp_t m_AdvancementGoalExp;

	// Blood Burst Point
	Attr_t	m_AttackBloodBurstPoint;
	Attr_t	m_DefenseBloodBurstPoint;
	Attr_t	m_PartyBloodBurstPoint;
	
	//20090601 ksym555
	Speed_t	m_AttackSpeed;
	
#ifdef __CONTRIBUTION_SYSTEM__
	int m_ContributePoint;
public:
	int getContributePoint() { return m_ContributePoint; }
	void setContributePoint(int point) { m_ContributePoint = point; }
#endif

};

#endif
