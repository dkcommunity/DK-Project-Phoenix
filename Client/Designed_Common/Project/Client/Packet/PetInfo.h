#ifndef __PET_INFO_H__
#define __PET_INFO_H__

#include "Types.h"
#include "types/PetTypes.h"
#include "Exception.h"

#include "Packet.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//#include "VSDateTime.h"
using std::string;

//class PetItem;

class PetInfo
{
public:
	PetInfo();

	virtual void read(SocketInputStream& iStream) ;
	virtual void write(SocketOutputStream& oStream) const ;

	virtual PacketSize_t getSize() const
	{
		if ( m_PetType == PET_NONE ) return szPetType;

//		if ( type == MY_INFO )
			return szPetType + szMonsterType + szPetLevel + szPetExp + szPetHP + szPetAttr + szPetAttrLevel + szOptionType + szItemType + szBYTE + szBYTE + szBYTE + szBYTE + szObjectID + szBYTE + m_Nickname.size() + szint + szCEffectID
#if __CONTENTS(__EXPERT_PET_CHANGER)
				+ szint
#endif //__EXPERT_PET_CHANGER
	
#if __CONTENTS(__NEW_PET_INCUBUS)
				+ szBYTE + m_OptionType.size() * szOptionType //081203 wlzzi - OptionType추가
#endif //__NEW_PET_INCUBUS
#if __CONTENTS(__PET_VISION_AMPLE)
				+szOptionType
#endif //__PET_VISION_AMPLE
#if __CONTENTS(__FUNCTION_PETFOOD)
				+ szBYTE
#endif //__FUNCTION_PETFOOD
#if __CONTENTS(__PET_MIXINGFORGE)
				+ szBYTE + m_MixOptionType.size() * szOptionType //mixOptionType추가
#endif //__PET_MIXINGFORGE
			;
				;
//		else
//			return szPetType + szMonsterType + szPetLevel + szPetExp + szPetAttr + szPetAttrLevel + szOptionType + szItemType + szObjectID;
	}
	static PacketSize_t getMaxSize()
	{
//		if ( type == MY_INFO )
			return szPetType + szMonsterType + szPetLevel + szPetExp + szPetHP + szPetAttr + szPetAttrLevel + szOptionType + szItemType + szBYTE + szBYTE + szBYTE + szBYTE + szObjectID + szBYTE + 22 + szint + szCEffectID
#if __CONTENTS(__EXPERT_PET_CHANGER)
				+ szint
#endif //__EXPERT_PET_CHANGER

#if __CONTENTS(__NEW_PET_INCUBUS)
				+ szBYTE + 10 * szOptionType;	//OptionType은 최대 10개까지만.. 들어갈 수 있다. //081203 wlzzi - OptionType추가
#endif //__NEW_PET_INCUBUS
#if __CONTENTS(__PET_VISION_AMPLE)
				+ szOptionType			//PetOption2
				+ szOptionType	 //pet3
#endif //__PET_VISION_AMPLE
#if __CONTENTS(__FUNCTION_PETFOOD)
			+ szBYTE
#endif //__FUNCTION_PETFOOD
#if __CONTENTS(__PET_MIXINGFORGE)
			+ szBYTE + 10 * szOptionType;	//OptionType은 최대 10개까지만.. 들어갈 수 있다. //mixOptionType추가
#endif //__PET_MIXINGFORGE
				;
//		else
//			return szPetType + szMonsterType + szPetLevel + szPetExp + szPetAttr + szPetAttrLevel + szOptionType + szItemType + szObjectID;
		}


public:
	void		setPetType(PetType_t PetType) { m_PetType = PetType; }
	PetType_t	getPetType() const { return m_PetType; }

	void			setPetCreatureType(MonsterType_t CreatureType) { m_PetCreatureType = CreatureType; }
	MonsterType_t	getPetCreatureType() const { return m_PetCreatureType; }

	void		setPetLevel(PetLevel_t PetLevel) { m_PetLevel = PetLevel; }
	PetLevel_t	getPetLevel() const { return m_PetLevel; }

	void		setPetExp(PetExp_t PetExp) { m_PetExp = PetExp; }
	PetExp_t	getPetExp() const { return m_PetExp; }

	void		setPetHP(PetHP_t PetHP) { m_PetHP = PetHP; }
	PetHP_t		getPetHP() const { return m_PetHP; }

	void		setPetAttr(PetAttr_t PetAttr) { m_PetAttr = PetAttr; }
	PetAttr_t	getPetAttr() const { return m_PetAttr; }

	void		setPetAttrLevel(PetAttrLevel_t PetAttrLevel) { m_PetAttrLevel = PetAttrLevel; }
	PetAttrLevel_t	getPetAttrLevel() const { return m_PetAttrLevel; }

	void			setPetOption(OptionType_t PetOption) { m_PetOption = PetOption; }
	OptionType_t	getPetOption() const { return m_PetOption; }

#if __CONTENTS(__PET_VISION_AMPLE)
	void			setPetOption2(OptionType_t PetOption) { m_PetOption2 = PetOption; }
	OptionType_t	getPetOption2() const { return m_PetOption2; }
	void			setPetOption3(OptionType_t PetOption) { m_PetOption3 = PetOption; }
	OptionType_t	getPetOption3() const { return m_PetOption3; }
#endif //__PET_VISION_AMPLE

#if __CONTENTS(__NEW_PET_INCUBUS)
	//081203 wlzzi - OptionType추가
	void						setOptionType(const std::list<OptionType_t>& OptionType)  { m_OptionType = OptionType; }
	int							getOptionTypeSize() const  { return m_OptionType.size(); }
	const std::list<OptionType_t>&	getOptionType() const  { return m_OptionType; }
	OptionType_t				getFirstOptionType() const  { if (m_OptionType.empty()) return 0; return m_OptionType.front(); }
	void						removeOptionType(OptionType_t OptionType)  { std::list<OptionType_t>::iterator itr = std::find(m_OptionType.begin(), m_OptionType.end(), OptionType); if (itr!=m_OptionType.end()) m_OptionType.erase(itr); }
	void						addOptionType(OptionType_t OptionType)  { m_OptionType.push_back(OptionType); }
#endif //__NEW_PET_INCUBUS
	
	void		setFoodType(ItemType_t FoodType) { m_PetFoodType = FoodType; }
	ItemType_t	getFoodType() const { return m_PetFoodType; }

	BYTE		canGamble() const { return m_CanGamble; }
	void		setGamble(BYTE bGamble) { m_CanGamble = bGamble; }

	BYTE		canCutHead() const { return m_CanCutHead; }
	void		setCutHead(BYTE bCutHead) { m_CanCutHead = bCutHead; }

	BYTE		canAttack() const { return m_CanAttack; }
	void		setAttack(BYTE bAttack) { m_CanAttack = bAttack; }

//	void		setPetItem(PetItem* pPetItem) { m_pPetItem = pPetItem; }
//	PetItem*	getPetItem() const { return m_pPetItem; }

	BYTE		isSummonInfo() const { return m_IsSummonInfo; }
	void		setSummonInfo( BYTE isSummon ) { m_IsSummonInfo = isSummon; }

	ObjectID_t	getPetItemObjectID() const { return m_petItemObjectID; }

	const std::string& getNickname() const { return m_Nickname; }
	void		setNickname(const std::string& name) { m_Nickname = name; }

	int			getMagicRemainSec() const { return m_MagicRemainSec; }
	void		setMagicRemainSec( int sec ) { m_MagicRemainSec = sec; }
#if __CONTENTS(__EXPERT_PET_CHANGER)
	int			getExpertPetChangerRemainSec() const { return m_ExpertPetChangerRemainSec; }
	void		setExpertPetChangerRemainSec( int sec ) { m_ExpertPetChangerRemainSec = sec; }
#endif //__EXPERT_PET_CHANGER

#ifdef __GAME_SERVER__
	// -_- 게임서버쪽에 정의해야쥐 PetItem.cpp 에 있음
	ObjectID_t	getItemObjectID() const;
#endif

//	VSDateTime&	getLastFeedTime() { return m_LastFeedTime; }
//	void		setFeedTime(const VSDateTime& time) { m_LastFeedTime = time; }

	CEffectID_t	getEnchantSkillType() const				{ return m_EnchantSkillType;	}
	void		setEnchantSkillType(CEffectID_t type)	{ m_EnchantSkillType = type;	}

	std::string		toString() const;

#if __CONTENTS(__FUNCTION_PETFOOD)
	void setFunctionPetfoodType(BYTE function_Petfood_Type)		{m_Function_Petfood_Type = function_Petfood_Type;}
	BYTE getFunctionPetfoodType() const 	{return m_Function_Petfood_Type;}
#endif //__FUNCTION_PETFOOD

#if __CONTENTS(__PET_MIXINGFORGE)
	void						setMixOptionType(const std::list<OptionType_t>& mixOptionType)  { m_MixOptionType = mixOptionType; }
	int							getMixOptionTypeSize() const  { return m_MixOptionType.size(); }
	const std::list<OptionType_t>&	getMixOptionType() const  { return m_MixOptionType; }
	OptionType_t				getMixFirstOptionType() const  { if (m_MixOptionType.empty()) return 0; return m_MixOptionType.front(); }
	void						removeMixOptionType(OptionType_t mixOptionType)  { std::list<OptionType_t>::iterator itr = std::find(m_MixOptionType.begin(), m_MixOptionType.end(), mixOptionType); if (itr!=m_MixOptionType.end()) m_MixOptionType.erase(itr); }
	void						addMixOptionType(OptionType_t mixOptionType)  { m_MixOptionType.push_back(mixOptionType); }
#endif //__PET_MIXINGFORGE


private:
	PetType_t		m_PetType;
	MonsterType_t	m_PetCreatureType;
	PetLevel_t		m_PetLevel;
	PetExp_t		m_PetExp;
	PetHP_t			m_PetHP;		///< 남은 먹이양 (분단위)
	PetAttr_t		m_PetAttr;
	PetAttrLevel_t	m_PetAttrLevel;
	OptionType_t	m_PetOption;

#if __CONTENTS(__PET_VISION_AMPLE)
	OptionType_t	m_PetOption2;
	OptionType_t	m_PetOption3;
#endif //__PET_VISION_AMPLE

#if __CONTENTS(__NEW_PET_INCUBUS)
	//081203 wlzzi - OptionType추가
	std::list<OptionType_t>	m_OptionType;
#endif //__NEW_PET_INCUBUS

	ItemType_t		m_PetFoodType;
	BYTE			m_CanGamble;
	BYTE			m_CanCutHead;
	
	BYTE			m_CanAttack;

	BYTE			m_IsSummonInfo;
//	VSDateTime		m_LastFeedTime;
	ObjectID_t		m_petItemObjectID;

	std::string			m_Nickname;
	int				m_MagicRemainSec;
#if __CONTENTS(__EXPERT_PET_CHANGER)
	int				m_ExpertPetChangerRemainSec;
#endif //__EXPERT_PET_CHANGER
	CEffectID_t		m_EnchantSkillType;

#if __CONTENTS(__FUNCTION_PETFOOD)
	BYTE				m_Function_Petfood_Type;
#endif //__FUNCTION_PETFOOD
#if __CONTENTS(__PET_MIXINGFORGE)
	std::list<OptionType_t>	m_MixOptionType;
#endif //__PET_MIXINGFORGE
	
//	PetItem*		m_pPetItem;
};

#endif
