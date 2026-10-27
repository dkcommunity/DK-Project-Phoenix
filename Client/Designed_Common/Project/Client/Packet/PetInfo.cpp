
#include "Client_PCH.h"
#include "PetInfo.h"

PetInfo::PetInfo()
{
	m_PetType=PET_NONE;
	m_PetLevel=0;
	m_PetLevel=0;
	m_PetExp=0;
	m_PetHP=0;
	m_PetAttr=0;
	m_PetOption=0;
#if __CONTENTS(__PET_VISION_AMPLE)
	m_PetOption2 = 0;
	m_PetOption3 = 0;
#endif //__PET_VISION_AMPLE
	m_IsSummonInfo = 0;

	m_CanCutHead = 0;
	m_CanAttack = 0;
	m_CanGamble = 0;
	m_petItemObjectID = 0;

	m_EnchantSkillType = 0;
#if __CONTENTS(__EXPERT_PET_CHANGER)
	m_MagicRemainSec = 0;
	m_ExpertPetChangerRemainSec = 0;
#endif //__EXPERT_PET_CHANGER
#if __CONTENTS(__FUNCTION_PETFOOD)
	m_Function_Petfood_Type = 0;
#endif //__FUNCTION_PETFOOD

}

void PetInfo::read(SocketInputStream& iStream) 
{
	__BEGIN_TRY

	iStream.read( m_PetType );

	if ( m_PetType == PET_NONE ) return;

	iStream.read( m_PetCreatureType );
	iStream.read( m_PetLevel );
	iStream.read( m_PetExp );

	iStream.read( m_PetHP );
	iStream.read( m_PetAttr );
	iStream.read( m_PetAttrLevel );
	iStream.read( m_PetOption );
#if __CONTENTS(__PET_VISION_AMPLE)
	iStream.read( m_PetOption2 );
	iStream.read( m_PetOption3 );
#endif //__PET_VISION_AMPLE
	iStream.read( m_PetFoodType );

	iStream.read( m_CanGamble );
	iStream.read( m_CanCutHead );
	
	iStream.read( m_CanAttack );

	iStream.read( m_IsSummonInfo );
		
	BYTE szSTR;
	iStream.read( szSTR );
	if ( szSTR != 0 ) iStream.read( m_Nickname, szSTR );

	iStream.read( m_petItemObjectID );

	iStream.read( m_MagicRemainSec );

#if __CONTENTS(__EXPERT_PET_CHANGER)
	iStream.read( m_ExpertPetChangerRemainSec );
#endif //__EXPERT_PET_CHANGER
	
	iStream.read( m_EnchantSkillType );

#if __CONTENTS(__NEW_PET_INCUBUS)
	//081203 wlzzi - OptionType추가
	BYTE optionSize;
	iStream.read( optionSize );

	m_OptionType.clear();

	for ( int j=0; j < optionSize ; ++j )
	{
		OptionType_t optionType;
		iStream.read( optionType );
		m_OptionType.push_back( optionType );
	}
	//
#endif //__NEW_PET_INCUBUS
#if __CONTENTS(__FUNCTION_PETFOOD)
	iStream.read(m_Function_Petfood_Type);
#endif //__FUNCTION_PETFOOD
#if __CONTENTS(__PET_MIXINGFORGE)
	BYTE mixOptionSize;
	iStream.read( mixOptionSize );

	m_MixOptionType.clear();

	for ( int k=0; k < mixOptionSize ; ++k )
	{
		OptionType_t mixOptionType;
		iStream.read( mixOptionType );
		m_MixOptionType.push_back( mixOptionType );
	}
#endif //__PET_MIXINGFORGE
	__END_CATCH
}

void PetInfo::write(SocketOutputStream& oStream) const 
{
	__BEGIN_TRY

	oStream.write( m_PetType );

	if ( m_PetType == PET_NONE ) return;

	oStream.write( m_PetCreatureType );
	oStream.write( m_PetLevel );
	oStream.write( m_PetExp );

	oStream.write( m_PetHP );
	oStream.write( m_PetAttr );
	oStream.write( m_PetAttrLevel );
	oStream.write( m_PetOption );
#if __CONTENTS(__PET_VISION_AMPLE)
	oStream.write( m_PetOption2 );
	oStream.write( m_PetOption3 );
#endif //__PET_VISION_AMPLE
	oStream.write( m_PetFoodType );

	oStream.write( m_CanGamble );
	oStream.write( m_CanCutHead );

	oStream.write( m_CanAttack );

	oStream.write( m_IsSummonInfo );

	BYTE szSTR = m_Nickname.size();
	oStream.write( szSTR );
	if ( szSTR != 0 ) oStream.write( m_Nickname );

	ObjectID_t ItemObjectID = 0;
#ifdef __GAME_SERVER__
	ItemObjectID = getItemObjectID();
#endif
	oStream.write( ItemObjectID );

	oStream.write( m_MagicRemainSec );

#if __CONTENTS(__EXPERT_PET_CHANGER)
	oStream.write( m_ExpertPetChangerRemainSec );
#endif //__EXPERT_PET_CHANGER

	oStream.write( m_EnchantSkillType );

#if __CONTENTS(__NEW_PET_INCUBUS)
//081203 wlzzi - OptionType추가
	BYTE optionSize = m_OptionType.size();
	oStream.write( optionSize );
	std::list<OptionType_t>::const_iterator itr = m_OptionType.begin();
	for (; itr!=m_OptionType.end(); itr++)
	{
		OptionType_t optionType = *itr;
		oStream.write( optionType );
	}
//-	
#endif //__NEW_PET_INCUBUS
#if __CONTENTS(__FUNCTION_PETFOOD)
	oStream.write(m_Function_Petfood_Type);
#endif //__FUNCTION_PETFOOD


#if __CONTENTS(__PET_MIXINGFORGE)
	BYTE mixOptionSize = m_MixOptionType.size();
	oStream.write( mixOptionSize );
	std::list<OptionType_t>::const_iterator mixitr = m_MixOptionType.begin();
	for (; mixitr!=m_MixOptionType.end(); mixitr++)
	{
		OptionType_t mixOptionType = *mixitr;
		oStream.write( mixOptionType );
	}
#endif //__PET_MIXINGFORGE
	__END_CATCH
}

std::string PetInfo::toString() const
{
	StringStream msg;
	
	msg << "PetInfo("
		<< "PetType : " << (int)m_PetType
		<< ", PetLevel : " << (int)m_PetLevel
		<< ", PetExp : " << (int)m_PetExp
		<< ", PetHP : " << (int)m_PetHP
		<< ", PetAttr : " << (int)m_PetAttr
		<< ", PetOption : " << (int)m_PetOption
#if __CONTENTS(__PET_VISION_AMPLE)
		<< ", PetOption2 : " << (int)m_PetOption2
		<< ", PetOption3 : " << (int)m_PetOption3
#endif //__PET_VISION_AMPLE
		<< ", PetFoodType : " << (int)m_PetFoodType
		<< ", PetCanCutHead : " << (int)m_CanCutHead
		<< ", PetCanAttack : " << (int)m_CanAttack
		<< ", EnchantSkillType : " << (int)m_EnchantSkillType;

#if __CONTENTS(__NEW_PET_INCUBUS)
		//081203 wlzzi - OptionType추가
		msg << ", Options : (";

		std::list<OptionType_t>::const_iterator itr = m_OptionType.begin();
		std::list<OptionType_t>::const_iterator endItr = m_OptionType.end();

		for ( ; itr != endItr ; ++itr )
		{
			msg << *itr << ", ";
		}
		//
#endif //__NEW_PET_INCUBUS

#if __CONTENTS(__FUNCTION_PETFOOD)
		msg << "m_Function_Petfood_Type" << (int)m_Function_Petfood_Type;
#endif //__FUNCTION_PETFOOD

#if __CONTENTS(__PET_MIXINGFORGE)
		msg << ", MixOptions : (";

		std::list<OptionType_t>::const_iterator mixitr = m_MixOptionType.begin();
		std::list<OptionType_t>::const_iterator mixendItr = m_MixOptionType.end();

		for ( ; mixitr != mixendItr ; ++mixitr )
		{
			msg << *mixitr << ", ";
		}
#endif //__PET_MIXINGFORGE
		msg << ")";


	return msg.toString();
}
