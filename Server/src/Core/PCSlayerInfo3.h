//////////////////////////////////////////////////////////////////////////////
// Filename    : PCSlayerInfo3.h 
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __PC_SLAYER_INFO3_H__
#define __PC_SLAYER_INFO3_H__

#include "PCInfo.h"
#include "Assert.h"
#include <bitset>

//////////////////////////////////////////////////////////////////////////////
// 슬레이어의 정보는 다음과 같다. 
//
//(1) OID 
//(2) Name 
//(3) X,Y,Dir 
//
// bit_set 으로 표현되는 정보들
//
//(4) Sex - MALE | FEMALE 
//(5) HairStyle - HAIR_STYLE1 | HAIR_STYLE2 | HAIR_STYLE3 
//(8) Helmet Type - NONE | HELMET1 | HELMET2
//(9) Jacket Type - BASIC | JACKET1 | JACKET2 
//(10) Pants Type - BASIC | PANTS1 | PANTS2 
//(11) Weapon Type - NONE | SWORD | BLADE | SHIELD | SWORD + SHIELD | AR | TR | SG | SMG | CROSS 
//(12) Motorcycle Type - NONE | MOTORCYCLE1 
//
// 상의/하의를 제외하면 모두 한 부분의 색상만이 바뀐다.
//
//(13) HairColor 
//(14) SkinColor 
//(15) Helmet Color 
//(16) Jacket Color[2] 
//(17) Pants Color[2] 
//(18) Weapon Color 
//(19) Shield Color 
//(20) Motorcycle Color 
//////////////////////////////////////////////////////////////////////////////

class PCSlayerInfo3 : public PCInfo 
{
public:

	// Slayer Outlook Information
    enum SlayerBits {
        SLAYER_BIT_SEX ,
		SLAYER_BIT_HAIRSTYLE1 ,
		SLAYER_BIT_HAIRSTYLE2 ,
		SLAYER_BIT_HELMET1,
		SLAYER_BIT_HELMET2,
		SLAYER_BIT_JACKET1,
		SLAYER_BIT_JACKET2,
		SLAYER_BIT_JACKET3,
		SLAYER_BIT_PANTS1,
		SLAYER_BIT_PANTS2,
		SLAYER_BIT_PANTS3,
		SLAYER_BIT_WEAPON1,
		SLAYER_BIT_WEAPON2,
		SLAYER_BIT_WEAPON3,
		SLAYER_BIT_WEAPON4,
		SLAYER_BIT_MOTORCYCLE1,
		SLAYER_BIT_MOTORCYCLE2,
		SLAYER_BIT_MOTORCYCLE3,
		SLAYER_BIT_SHIELD1,
		SLAYER_BIT_SHIELD2,
		SLAYER_BIT_SHOULDER1,
		SLAYER_BIT_SHOULDER2,
		SLAYER_BIT_MAX
    };

	// Slayer Color Informations
	enum SlayerColors 
	{
		SLAYER_COLOR_HAIR ,
		SLAYER_COLOR_SKIN ,
		SLAYER_COLOR_HELMET ,
		SLAYER_COLOR_JACKET ,
		SLAYER_COLOR_PANTS ,
		SLAYER_COLOR_WEAPON ,
		SLAYER_COLOR_SHIELD ,
		SLAYER_COLOR_MOTORCYCLE ,
		SLAYER_COLOR_MOTORCYCLE_EFFECT ,
		SLAYER_COLOR_SHOULDER ,
		SLAYER_COLOR_MAX
	};


public:
	PCSlayerInfo3 () throw ()
	{
		m_Outlook.reset();
	}

	PCSlayerInfo3 (const PCSlayerInfo3 & slayerInfo) throw ()
		: m_ObjectID(slayerInfo.m_ObjectID), m_Name(slayerInfo.m_Name), 
		m_X(slayerInfo.m_X), m_Y(slayerInfo.m_Y), m_Dir(slayerInfo.m_Dir),m_Tou(slayerInfo.m_Tou),m_Yi(slayerInfo.m_Yi),m_Ku(slayerInfo.m_Ku), 
		m_Wuqi(slayerInfo.m_Wuqi),m_Dun(slayerInfo.m_Dun),m_Moto(slayerInfo.m_Moto),m_Jian(slayerInfo.m_Jian),
		m_Outlook(slayerInfo.m_Outlook), m_MasterEffectColor(slayerInfo.m_MasterEffectColor), m_Born(slayerInfo.m_Born),
		m_CurrentHP(slayerInfo.m_CurrentHP), m_MaxHP(slayerInfo.m_MaxHP), m_AttackSpeed(slayerInfo.m_AttackSpeed),
		m_Alignment(slayerInfo.m_Alignment), m_GuildID(slayerInfo.m_GuildID), m_Rank(slayerInfo.m_Rank), m_AdvancementLevel(slayerInfo.m_AdvancementLevel)
#ifdef __CONTRIBUTION_SYSTEM__
		,m_ContributePoint(slayerInfo.m_ContributePoint)
#endif
		,m_MotorItemType(slayerInfo.m_MotorItemType)
	{
		for (uint i = 0 ; i < SLAYER_COLOR_MAX ; i ++)
			m_Colors[i] = slayerInfo.m_Colors[i]; 	// 슬레이어 색깔 정보

		m_Competence = slayerInfo.m_Competence;
	}


public:
	PCType getPCType () const throw () { return PC_SLAYER; }
	
    void read (SocketInputStream & iStream) throw (ProtocolException, Error);
    void write (SocketOutputStream & oStream) const throw (ProtocolException, Error);

	uint getSize () const throw () 
	{ 
		return szObjectID 					// 크리처 아이디
			+ szBYTE + m_Name.size() 		// 이름
			+ szCoord + szCoord + szDir + szBYTE*7 	// 좌표와 방향
			+ szDWORD						// 슬레이어 플래그
			+ szColor* SLAYER_COLOR_MAX 	// 색깔 정보
			+ szWORD						// 마스터 이펙트
			+ szBYTE						// 출신
			+ szHP* 2						// 최대 체력
			+ szAlignment					// 성향
			+ szRank						// 계급
			+ szSpeed 						// 공격 스피드
			+ szGuildID						// 공격 스피드
			+ szBYTE						// 권한
			+ szuint
			+ szLevel
#ifdef __CONTRIBUTION_SYSTEM__
			+ szDWORD // 기여도
#endif
			+ szItemType
			;
	}

	static uint getMaxSize () throw ()
	{
		return szObjectID 					// 크리처 아이디
			+ szBYTE + 20					// 이름
			+ szCoord + szCoord + szDir + szBYTE*7	// 좌표와 방향
			+ szDWORD						// 슬레이어 플래그
			+ szColor* SLAYER_COLOR_MAX 	// 색깔 정보
			+ szWORD						// 마스터 이펙트
			+ szBYTE						// 출신
			+ szHP* 2						// 최대 체력
			+ szAlignment					// 성향
			+ szRank						// 계급
			+ szSpeed						// 공격 스피드
			+ szGuildID						// 공격 스피드
			+ szBYTE						// 권한
			+ szuint
			+ szLevel
#ifdef __CONTRIBUTION_SYSTEM__
			+ szDWORD // 기여도
#endif
			+ szItemType
			;
	}

	PCSlayerInfo3 &operator = (const PCSlayerInfo3 & slayerInfo) throw ()
	{    
		if (&slayerInfo == this)
			return *this;
		m_ObjectID = slayerInfo.m_ObjectID;
		m_Name = slayerInfo.m_Name;
		m_X = slayerInfo.m_X;
		m_Y = slayerInfo.m_Y;
		m_Dir = slayerInfo.m_Dir;

		m_Tou = slayerInfo.m_Tou;
		m_Yi =  slayerInfo.m_Yi;
		m_Ku =  slayerInfo.m_Ku;
		m_Wuqi = slayerInfo.m_Wuqi;
		m_Dun =  slayerInfo.m_Dun;
		m_Moto =  slayerInfo.m_Moto;
		m_Jian = slayerInfo.m_Jian;

		m_Outlook = slayerInfo.m_Outlook;
		m_CurrentHP = slayerInfo.m_CurrentHP;
		m_MaxHP = slayerInfo.m_MaxHP;
		m_AttackSpeed = slayerInfo.m_AttackSpeed;
		m_Alignment = slayerInfo.m_Alignment;

		for (uint i = 0 ; i < SLAYER_COLOR_MAX ; i ++)
			m_Colors[i] = slayerInfo.m_Colors[i]; 	// 슬레이어 색깔 정보

		m_MasterEffectColor = slayerInfo.m_MasterEffectColor;
		m_Born = slayerInfo.m_Born;

		m_Competence = slayerInfo.m_Competence;
		m_GuildID	 = slayerInfo.m_GuildID;
		m_UnionID	= slayerInfo.m_UnionID;

		m_Rank	 = slayerInfo.m_Rank;
		m_AdvancementLevel = slayerInfo.m_AdvancementLevel;
#ifdef __CONTRIBUTION_SYSTEM__
		m_ContributePoint = slayerInfo.m_ContributePoint;
#endif
		m_MotorItemType = slayerInfo.m_MotorItemType;

		return *this;
	}

	string toString () const throw ();

public:
	ObjectID_t getObjectID () const throw () { return m_ObjectID; }
	void setObjectID (ObjectID_t creatureID) throw () { m_ObjectID = creatureID; }

	string getName () const throw (Error) { Assert(m_Name != ""); return m_Name; }
	void setName (const string & name) throw (Error) { m_Name = name; Assert(m_Name != ""); }

	Coord_t getX () const throw () { return m_X; }
	void setX (Coord_t x) throw () { m_X = x; }
	
	Coord_t getY () const throw () { return m_Y; }
	void setY (Coord_t y) throw () { m_Y = y; }

	Dir_t getDir () const throw () { return m_Dir; }
	void setDir (Dir_t dir) throw () { m_Dir = dir; }

	BYTE getTou () const throw () { return m_Tou; }
	void setTou (BYTE Tou) throw () { m_Tou = Tou; }

	BYTE getYi () const throw () { return m_Yi; }
	void setYi (BYTE Yi) throw () { m_Yi = Yi; }

	BYTE getKu () const throw () { return m_Ku; }
	void setKu (BYTE Ku) throw () { m_Ku = Ku; }

	BYTE getWuqi () const throw () { return m_Wuqi; }
	void setWuqi (BYTE Wuqi) throw () { m_Wuqi = Wuqi; }

	BYTE getDun () const throw () { return m_Dun; }
	void setDun (BYTE Dun) throw () { m_Dun = Dun; }

	BYTE getMoto () const throw () { return m_Moto; }
	void setMoto (BYTE Moto) throw () { m_Moto = Moto; }

	BYTE getJian () const throw () { return m_Jian; }
	void setJian (BYTE Jian) throw () { m_Jian = Jian; }

public:
    Sex getSex () const throw () 
	{ 
		return m_Outlook.test(SLAYER_BIT_SEX)?MALE:FEMALE; 
	}
    void setSex (Sex sex) throw () 
	{ 
		m_Outlook.set(SLAYER_BIT_SEX,(sex==MALE?true:false)); 
	}

	HairStyle getHairStyle () const throw () 
	{ 
		return HairStyle((m_Outlook.to_ulong() >> SLAYER_BIT_HAIRSTYLE1) & 3); 
	}
	void setHairStyle (HairStyle hairStyle) throw () 
	{ 
		m_Outlook &= ~bitset<SLAYER_BIT_MAX>(3 << SLAYER_BIT_HAIRSTYLE1); 
		m_Outlook |= bitset<SLAYER_BIT_MAX>(hairStyle << SLAYER_BIT_HAIRSTYLE1); 
	}

	HelmetType getHelmetType () const throw () 
	{ 
		return HelmetType((m_Outlook.to_ulong() >> SLAYER_BIT_HELMET1) & 3); 
	}
	void setHelmetType (HelmetType helmetType) throw ()
	{ 
		m_Outlook &= ~bitset<SLAYER_BIT_MAX>(3 << SLAYER_BIT_HELMET1); 
		m_Outlook |= bitset<SLAYER_BIT_MAX>(helmetType << SLAYER_BIT_HELMET1); 
	}

	JacketType getJacketType () const throw () 
	{ 
		return JacketType((m_Outlook.to_ulong() >> SLAYER_BIT_JACKET1) & 7); 
	}
	void setJacketType (JacketType jacketType) throw ()
	{ 
		m_Outlook &= ~bitset<SLAYER_BIT_MAX>(7 << SLAYER_BIT_JACKET1); 
		m_Outlook |= bitset<SLAYER_BIT_MAX>(jacketType << SLAYER_BIT_JACKET1); 
	}

	PantsType getPantsType () const throw () 
	{ 
		return PantsType((m_Outlook.to_ulong() >> SLAYER_BIT_PANTS1) & 7); 
	}
	void setPantsType (PantsType pantsType) throw ()
	{ 
		m_Outlook &= ~bitset<SLAYER_BIT_MAX>(7 << SLAYER_BIT_PANTS1); 
		m_Outlook |= bitset<SLAYER_BIT_MAX>(pantsType << SLAYER_BIT_PANTS1); 
	}

	WeaponType getWeaponType () const throw ()
	{ 
		return WeaponType((m_Outlook.to_ulong() >> SLAYER_BIT_WEAPON1) & 15); 
	}
	void setWeaponType (WeaponType weaponType) throw ()
	{ 
		m_Outlook &= ~bitset<SLAYER_BIT_MAX>(15 << SLAYER_BIT_WEAPON1); 
		m_Outlook |= bitset<SLAYER_BIT_MAX>(weaponType << SLAYER_BIT_WEAPON1); 
	}

	ShieldType getShieldType () const throw ()
	{
		return ShieldType((m_Outlook.to_ulong() >> SLAYER_BIT_SHIELD1) & 3);
	}
	void setShieldType (ShieldType shieldType) throw ()
	{
		m_Outlook &= ~bitset<SLAYER_BIT_MAX>(3 << SLAYER_BIT_SHIELD1);
		m_Outlook |= bitset<SLAYER_BIT_MAX>(shieldType << SLAYER_BIT_SHIELD1);
	}

	MotorcycleType getMotorcycleType () const throw ()
	{ 
		return MotorcycleType((m_Outlook.to_ulong() >> SLAYER_BIT_MOTORCYCLE1) & 7); 
	}
	void setMotorcycleType (MotorcycleType motorcycleType) throw ()
	{ 
		m_Outlook &= ~bitset<SLAYER_BIT_MAX>(7 << SLAYER_BIT_MOTORCYCLE1); 
		m_Outlook |= bitset<SLAYER_BIT_MAX>(motorcycleType << SLAYER_BIT_MOTORCYCLE1); 
	}

	BYTE getShoulderType () const throw ()
	{
		return BYTE((m_Outlook.to_ulong() >> SLAYER_BIT_SHOULDER1) & 3); 
	}
	void setShoulderType (BYTE shoulder) throw ()
	{
		m_Outlook &= ~bitset<SLAYER_BIT_MAX>(3 << SLAYER_BIT_SHOULDER1); 
		m_Outlook |= bitset<SLAYER_BIT_MAX>(shoulder << SLAYER_BIT_SHOULDER1); 
	}

public:
	Color_t getHairColor () const throw () 
	{ 
		return m_Colors[ SLAYER_COLOR_HAIR ]; 
	}
	void setHairColor (Color_t color) throw () 
	{ 
		m_Colors[ SLAYER_COLOR_HAIR ] = color; 
	}

	Color_t getSkinColor () const throw () 
	{ 
		return m_Colors[ SLAYER_COLOR_SKIN ]; 
	}
	void setSkinColor (Color_t color) throw () 
	{ 
		m_Colors[ SLAYER_COLOR_SKIN ] = color; 
	}

	Color_t getHelmetColor (ColorType colorType = MAIN_COLOR) const throw () 
	{ 
		return m_Colors[ SLAYER_COLOR_HELMET + (uint)colorType ]; 
	}
	void setHelmetColor (Color_t color, ColorType colorType = MAIN_COLOR) throw () 
	{ 
		m_Colors[ SLAYER_COLOR_HELMET + (uint)colorType ] = color; 
	}

	Color_t getJacketColor (ColorType colorType = MAIN_COLOR) const throw () 
	{ 
		return m_Colors[ SLAYER_COLOR_JACKET + (uint)colorType ]; 
	}
	void setJacketColor (Color_t color, ColorType colorType = MAIN_COLOR) throw () 
	{ 
		m_Colors[ SLAYER_COLOR_JACKET + (uint)colorType ] = color; 
	}

	Color_t getPantsColor (ColorType colorType = MAIN_COLOR) const throw () 
	{ 
		return m_Colors[ SLAYER_COLOR_PANTS + (uint)colorType ]; 
	}
	void setPantsColor (Color_t color, ColorType colorType = MAIN_COLOR) throw () 
	{ 
		m_Colors[ SLAYER_COLOR_PANTS + (uint)colorType ] = color; 
	}

	Color_t getWeaponColor (ColorType colorType = MAIN_COLOR) const throw () 
	{ 
		return m_Colors[ SLAYER_COLOR_WEAPON + (uint)colorType ]; 
	}
	void setWeaponColor (Color_t color, ColorType colorType = MAIN_COLOR) throw () 
	{ 
		m_Colors[ SLAYER_COLOR_WEAPON + (uint)colorType ] = color; 
	}

	Color_t getShieldColor (ColorType colorType = MAIN_COLOR) const throw () 
	{ 
		return m_Colors[ SLAYER_COLOR_SHIELD + (uint)colorType ]; 
	}
	void setShieldColor (Color_t color, ColorType colorType = MAIN_COLOR) throw () 
	{ 
		m_Colors[ SLAYER_COLOR_SHIELD + (uint)colorType ] = color; 
	}

	Color_t getMotorcycleColor (ColorType colorType = MAIN_COLOR) const throw () 
	{ 
		return m_Colors[ SLAYER_COLOR_MOTORCYCLE + (uint)colorType ]; 
	}
	void setMotorcycleColor (Color_t color, ColorType colorType = MAIN_COLOR) throw () 
	{ 
		m_Colors[ SLAYER_COLOR_MOTORCYCLE + (uint)colorType ] = color; 
	}

	Color_t getMotorcycleEffectColor (ColorType colorType = MAIN_COLOR) const throw () 
	{ 
		return m_Colors[ SLAYER_COLOR_MOTORCYCLE_EFFECT + (uint)colorType ]; 
	}
	void setMotorcycleEffectColor (Color_t color, ColorType colorType = MAIN_COLOR) throw () 
	{ 
		m_Colors[ SLAYER_COLOR_MOTORCYCLE_EFFECT + (uint)colorType ] = color; 
	}


	Color_t getShoulderColor () const throw () 
	{ 
		return m_Colors[ SLAYER_COLOR_SHOULDER ]; 
	}
	void setShoulderColor (Color_t color) throw () 
	{ 
		m_Colors[ SLAYER_COLOR_SHOULDER ] = color; 
	}

	WORD getMasterEffectColor() const { return m_MasterEffectColor; }
	void setMasterEffectColor( WORD color ) { m_MasterEffectColor = color; }

	BYTE getBorn() const { return m_Born; }
	void setBorn( BYTE born ) { m_Born = born; }

	HP_t getCurrentHP() const throw() { return m_CurrentHP; }
	void setCurrentHP(HP_t CurrentHP) throw() { m_CurrentHP = CurrentHP; }

	HP_t getMaxHP() const throw() { return m_MaxHP; }
	void setMaxHP(HP_t MaxHP) throw() { m_MaxHP = MaxHP; }

	Speed_t getAttackSpeed() const throw() { return m_AttackSpeed; }
	void setAttackSpeed(Speed_t AttackSpeed) throw() { m_AttackSpeed = AttackSpeed; }

	Alignment_t getAlignment() const throw() { return m_Alignment; }
	void setAlignment(Alignment_t Alignment) throw() { m_Alignment = Alignment; }

	BYTE getCompetence(void) const { return m_Competence; }
	void setCompetence(BYTE competence) { m_Competence = competence; }

	GuildID_t getGuildID(void) const { return m_GuildID; }
	void setGuildID(GuildID_t GuildID ) { m_GuildID = GuildID; }

	uint getUnionID(void) const { return m_UnionID; }
	void setUnionID(uint UnionID ) { m_UnionID = UnionID; }

	Rank_t getRank () const throw () { return m_Rank; }
	void setRank (Rank_t rank) throw () { m_Rank = rank; }

	Level_t	getAdvancementLevel() const { return m_AdvancementLevel; }
	void setAdvancementLevel( Level_t level ) { m_AdvancementLevel = level; }
	
	ItemType_t	getMotorItemType() const { return m_MotorItemType; }
	void setMotorItemType( ItemType_t ItemType ) { m_MotorItemType = ItemType; }

private:
	ObjectID_t				m_ObjectID;					// OID
	string					m_Name; 					// PC의 이름
	Coord_t					m_X;						// X 좌표
	Coord_t					m_Y;						// Y 좌표
	Dir_t					m_Dir;						// 방향

	BYTE					m_Tou;
	BYTE					m_Yi;
	BYTE					m_Ku;
	BYTE					m_Wuqi;
	BYTE					m_Dun;
	BYTE					m_Moto;
	BYTE					m_Jian;


	bitset<SLAYER_BIT_MAX>	m_Outlook;					// 슬레이어외모정보
	Color_t					m_Colors[SLAYER_COLOR_MAX];	// 슬레이어색깔정보
	WORD					m_MasterEffectColor;		// 마스터 이펙트 색깔
	BYTE					m_Born;						// 출신지
	HP_t					m_CurrentHP;				// 슬레이어현재체력
	HP_t					m_MaxHP;					// 슬레이어최대체력
	Speed_t					m_AttackSpeed;				// 공격속도
	Alignment_t				m_Alignment;				// 성향
	BYTE					m_Competence;				// 권한
	GuildID_t				m_GuildID;					// 길드 아이디
	uint					m_UnionID;
	Rank_t					m_Rank;						// 계급
	Level_t					m_AdvancementLevel;
#ifdef __CONTRIBUTION_SYSTEM__
	int m_ContributePoint;
public:
	int getContributePoint() { return m_ContributePoint; }
	void setContributePoint(int point) { m_ContributePoint = point; }
#endif
	ItemType_t m_MotorItemType;
};

#endif
