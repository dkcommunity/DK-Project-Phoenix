//////////////////////////////////////////////////////////////////////////////
// Filename    : PCOustersInfo2.cpp
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "PCOustersInfo2.h"

//////////////////////////////////////////////////////////////////////////////
// read data from socket input stream
//////////////////////////////////////////////////////////////////////////////
void PCOustersInfo2::read ( SocketInputStream & iStream ) 
	
{
	__BEGIN_TRY

	//--------------------------------------------------
	// read object id
	//--------------------------------------------------
	iStream.read( m_ObjectID );

	//--------------------------------------------------
	// read vampire name
	//--------------------------------------------------
	BYTE szName;

	iStream.read( szName );

	if ( szName == 0 )
		throw InvalidProtocolException("szName == 0");

	if ( szName > 20 )
		throw InvalidProtocolException("too long name length");

	iStream.read( m_Name , szName );

	//--------------------------------------------------
	// read Level
	//--------------------------------------------------
	iStream.read( m_Level );

	//--------------------------------------------------
	// read sex
	//--------------------------------------------------
	BYTE sex;
	iStream.read( sex );
	m_Sex = Sex(sex);
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	iStream.read( m_WingSylphType);
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER

	//--------------------------------------------------
	// read colors
	//--------------------------------------------------
	iStream.read( m_HairColor );

#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	switch(m_WingSylphType)
	{
		case 2 :	// SLYPH_TYPE_WING
#if __CONTENTS(__SECOND_TRANSFORTER)
			iStream.read( m_WingItemType );
#endif //__SECOND_TRANSFORTER
			iStream.read( m_WingBodyColor );
			iStream.read( m_WingEffectColor );
			break;
		case 1 :	// SLYPH_TYPE_LEGACY
		case 0 :	// SLYPH_TYPE_NONE
			break;
	}
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	
	iStream.read( m_MasterEffectColor );

	// 출신
	iStream.read( m_Born );

    //--------------------------------------------------
	// read Alignment
	//--------------------------------------------------
	iStream.read( m_Alignment );
    //--------------------------------------------------
	// read attributes
	//--------------------------------------------------
	iStream.read( m_STR[ATTR_CURRENT] );
	iStream.read( m_STR[ATTR_MAX] );
	iStream.read( m_STR[ATTR_BASIC] );
	iStream.read( m_DEX[ATTR_CURRENT] );
	iStream.read( m_DEX[ATTR_MAX] );
	iStream.read( m_DEX[ATTR_BASIC] );
	iStream.read( m_INT[ATTR_CURRENT] );
	iStream.read( m_INT[ATTR_MAX] );
	iStream.read( m_INT[ATTR_BASIC] );

	//--------------------------------------------------
	// read hp
	//--------------------------------------------------
	iStream.read( m_HP[ATTR_CURRENT] );
	iStream.read( m_HP[ATTR_MAX] );
	
	//--------------------------------------------------
	// read mp
	//--------------------------------------------------
	iStream.read( m_MP[ATTR_CURRENT] );
	iStream.read( m_MP[ATTR_MAX] );
	
	//--------------------------------------------------
	// read misc
	//--------------------------------------------------
	iStream.read( m_Rank );
	iStream.read( m_RankExp );

	iStream.read( m_Exp );
	iStream.read( m_Gold );


	//--------------------------------------------------
	// read fame&notoriety 
	//--------------------------------------------------
	iStream.read( m_Fame );

	//--------------------------------------------------
	// read sight
	//--------------------------------------------------
	iStream.read( m_Sight );

	//--------------------------------------------------
	// read bonus
	//--------------------------------------------------
	iStream.read( m_Bonus );
	iStream.read( m_SkillBonus );

	// 은도금 데미쥐
	iStream.read( m_SilverDamage );

	// 권한
	iStream.read( m_Competence );

	// 길드 아이디
	iStream.read( m_GuildID );

	BYTE szGuildName;

	iStream.read( szGuildName );

	if ( szGuildName > 30 )
		throw InvalidProtocolException( "too long szGuildName size" );

	if ( szGuildName != 0 )
		iStream.read( m_GuildName, szGuildName );

	iStream.read( m_GuildMemberRank );
	iStream.read( m_UnionID );
	iStream.read( m_AdvancementLevel );
	iStream.read( m_AdvancementGoalExp );

	iStream.read( m_AttackBloodBurstPoint );
	iStream.read( m_DefenseBloodBurstPoint );
	iStream.read( m_PartyBloodBurstPoint );
	
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
	iStream.read( m_ContributePoint );
#endif
#if __CONTENTS(__ATTACK_SPEED)
		iStream.read(m_AttackSpeed);
#endif //__ATTACK_SPEED
		iStream.read( m_DianJuan );
		iStream.read( m_JiFen );
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// write data to socket output stream
//////////////////////////////////////////////////////////////////////////////
void PCOustersInfo2::write ( SocketOutputStream & oStream ) const 
	
{
	__BEGIN_TRY

	//--------------------------------------------------
	// write object id
	//--------------------------------------------------
	oStream.write( m_ObjectID );

	//--------------------------------------------------
	// write vampire name
	//--------------------------------------------------
	BYTE szName = m_Name.size();

	if ( szName == 0 )
		throw InvalidProtocolException("szName == 0");

	if ( szName > 20 )
		throw InvalidProtocolException("too long name length");

	oStream.write( szName );

	oStream.write( m_Name );

	//--------------------------------------------------
	// write sex
	//--------------------------------------------------
	oStream.write( m_Level );

	//--------------------------------------------------
	// write sex
	//--------------------------------------------------
	oStream.write( (BYTE)m_Sex );
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	oStream.write( m_WingSylphType );
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	//--------------------------------------------------
	// write colors
	//--------------------------------------------------
	oStream.write( m_HairColor );
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	switch(m_WingSylphType)
	{
		case 2 :	// SLYPH_TYPE_WING
#if __CONTENTS(__SECOND_TRANSFORTER)
			oStream.write( m_WingItemType );
#endif //__SECOND_TRANSFORTER
			oStream.write( m_WingBodyColor );
			oStream.write( m_WingEffectColor );
			break;
		case 1 :	// SLYPH_TYPE_LEGACY
		case 0 :	// SLYPH_TYPE_NONE
			break;
	}
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	oStream.write( m_MasterEffectColor );

	// 출신
	oStream.write( m_Born );

    //--------------------------------------------------
	// write Alignment
	//--------------------------------------------------
	oStream.write( m_Alignment);

    //--------------------------------------------------
	// write attributes
	//--------------------------------------------------
	oStream.write( m_STR[ATTR_CURRENT] );
	oStream.write( m_STR[ATTR_MAX] );
	oStream.write( m_STR[ATTR_BASIC] );
	oStream.write( m_DEX[ATTR_CURRENT] );
	oStream.write( m_DEX[ATTR_MAX] );
	oStream.write( m_DEX[ATTR_BASIC] );
	oStream.write( m_INT[ATTR_CURRENT] );
	oStream.write( m_INT[ATTR_MAX] );
	oStream.write( m_INT[ATTR_BASIC] );

	//--------------------------------------------------
	// write hp
	//--------------------------------------------------
	oStream.write( m_HP[ATTR_CURRENT] );
	oStream.write( m_HP[ATTR_MAX] );
	
	//--------------------------------------------------
	// write mp
	//--------------------------------------------------
	oStream.write( m_MP[ATTR_CURRENT] );
	oStream.write( m_MP[ATTR_MAX] );
	
	//--------------------------------------------------
	// write misc
	//--------------------------------------------------
	oStream.write( m_Rank );
	oStream.write( m_RankExp );

	oStream.write( m_Exp );
	oStream.write( m_Gold );

	//--------------------------------------------------
	// read fame&notoriety 
	//--------------------------------------------------
	oStream.write( m_Fame );
	
	//--------------------------------------------------
	// write sight
	//--------------------------------------------------
	oStream.write( m_Sight );

	//--------------------------------------------------
	// write Bonus
	//--------------------------------------------------
	oStream.write( m_Bonus );
	oStream.write( m_SkillBonus );

	// 은도금 데미지
	oStream.write( m_SilverDamage );

	// 권한
	oStream.write( m_Competence );

	// 길드 아이디
	oStream.write( m_GuildID );

	BYTE szGuildName = m_GuildName.size();

	if ( szGuildName > 30 )
		throw InvalidProtocolException( "too long guild name length" );

	oStream.write( szGuildName );
	if ( szGuildName != 0 )
		oStream.write( m_GuildName );

	oStream.write( m_GuildMemberRank );
	oStream.write( m_UnionID );
	oStream.write( m_AdvancementLevel );
	oStream.write( m_AdvancementGoalExp );

	oStream.write( m_AttackBloodBurstPoint );
	oStream.write( m_DefenseBloodBurstPoint );
	oStream.write( m_PartyBloodBurstPoint );

#if __CONTENTS(__CONTRIBUTE_SYSTEM)
	oStream.write( m_ContributePoint );
#endif //__CONTRIBUTE_SYSTEM
#if __CONTENTS(__ATTACK_SPEED)
		oStream.write(m_AttackSpeed);
#endif //__ATTACK_SPEED
		oStream.write( m_DianJuan );
		oStream.write( m_JiFen );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get debug std::string
//////////////////////////////////////////////////////////////////////////////
std::string PCOustersInfo2::toString () const 
	
{
	StringStream msg;

	msg << "PCOustersInfo2("
		<< "ObjectID:" << m_ObjectID
		<< ",Name:" << m_Name
		<< ",Level:" << m_Level
		<< ",Sex:" << Sex2String[m_Sex]
		<< ",HairColor:" << (int)m_HairColor
#if __CONTENTS(__SECOND_TRANSFORTER)
		<< ",WingSylphType:" << (int)m_WingSylphType
		<< ",WingBodyColor:" << (int)m_WingBodyColor
		<< ",WingEffectColor:" << (int)m_WingEffectColor
#endif //__SECOND_TRANSFORTER
		<< ",MasterEffectColor:" << (int)m_MasterEffectColor
		<< ",Born:" << (int)m_Born
		<< ",Alignment:" << (int)m_Alignment
        << ",STR[CURRENT]:" << (int)m_STR[ATTR_CURRENT]
		<< ",STR[MAX]:" << (int)m_STR[ATTR_MAX]
		<< ",STR[BASIC]:" << (int)m_STR[ATTR_BASIC]
		<< ",DEX[CURRENT]:" << (int)m_DEX[ATTR_CURRENT]
		<< ",DEX[MAX]:" << (int)m_DEX[ATTR_MAX]
		<< ",DEX[BASIC]:" << (int)m_DEX[ATTR_BASIC]
		<< ",INT[CURRENT]:" << (int)m_INT[ATTR_CURRENT]
		<< ",INT[MAX]:" << (int)m_INT[ATTR_MAX]
		<< ",INT[BASIC]:" << (int)m_INT[ATTR_BASIC]
		<< ",HP:" << m_HP[ATTR_CURRENT] << "/" << m_HP[ATTR_MAX]
		<< ",MP:" << m_MP[ATTR_CURRENT] << "/" << m_MP[ATTR_MAX]
		<< ",Rank:" << (int)m_Rank
		<< ",RankExp:" << (int)m_RankExp
		<< ",Exp:" << m_Exp
		<< ",Gold:" << m_Gold
		<< ",Fame:" << m_Fame
		<< ",Sight:" << (int)m_Sight
		<< ",Bonus:" << (int)m_Bonus
		<< ",SkillBonus:" << (int)m_SkillBonus
		<< ",SilverDamage:" << (int)m_SilverDamage
		<< ",Competence:" << (int)m_Competence
		<< ",GuildID:" << (int)m_GuildID
		<< ",GuildName:" << m_GuildName
		<< ",GuildMemberRank:" << (int)m_GuildMemberRank
		<< ",AttackBloodBurstPoint:" << (int)m_AttackBloodBurstPoint
		<< ",DefenseBloodBurstPoint:" << (int)m_DefenseBloodBurstPoint
		<< ",PartyBloodBurstPoint:" << (int)m_PartyBloodBurstPoint
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
		<< ",ContributePoint:" << (int)m_ContributePoint
#endif //__CONTRIBUTE_SYSTEM
#if __CONTENTS(__ATTACK_SPEED)
		<< ",AttackSpeed:"	<<	(int)m_AttackSpeed
#endif //__ATTACK_SPEED
		<< ")";

	return msg.toString();
}
