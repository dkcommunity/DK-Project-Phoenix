//////////////////////////////////////////////////////////////////////////////
// Filename    : PCVampireInfo3.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"

#include "PCVampireInfo3.h"

//////////////////////////////////////////////////////////////////////////////
// read data from socket input stream
//////////////////////////////////////////////////////////////////////////////
void PCVampireInfo3::read ( SocketInputStream & iStream ) 
	
{
	__BEGIN_TRY

	// read object id
	iStream.read( m_ObjectID );

	// read vampire name
	BYTE szName;

	iStream.read( szName );

	if ( szName == 0 )
		throw InvalidProtocolException("szName == 0");

	if ( szName > 20 )
		throw InvalidProtocolException("too long name length");

	iStream.read( m_Name , szName );

	// read (X,Y,Dir)
	iStream.read( m_X );
	iStream.read( m_Y );
	iStream.read( m_Dir );
	
	// read sex
	BYTE sex;
	iStream.read( sex );
	m_Sex = Sex(sex);

	//--------------------------------------------------
	// read Shape
	//--------------------------------------------------
	BYTE coatType;
	iStream.read( coatType );
	m_CoatType = (VampireCoatType)(coatType & 15 );
	m_ArmType = (VampireArmType)((coatType >> 4) & 3);	//by viva
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	iStream.read( m_BatType );
#if __CONTENTS(__SECOND_TRANSFORTER)
	iStream.read( m_WingItemType );
#endif //__SECOND_TRANSFORTER
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	// read colors
	for ( uint i = 0 ; i < VAMPIRE_COLOR_MAX ; i ++ )
		iStream.read( m_Colors[i] );

	iStream.read( m_MasterEffectColor );
	iStream.read( m_Born );
	iStream.read( m_CurrentHP );

	iStream.read( m_MaxHP );

	iStream.read( m_AttackSpeed );

	iStream.read( m_Alignment );

	iStream.read( m_Shape );

	iStream.read( m_Competence );

	iStream.read( m_GuildID );
	iStream.read( m_UnionID );
	
	iStream.read( m_Rank );
	iStream.read( m_AdvancementLevel );
	
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
	iStream.read( m_ContributePoint );

#endif

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// write data to socket output stream
//////////////////////////////////////////////////////////////////////////////
void PCVampireInfo3::write ( SocketOutputStream & oStream ) const 
	
{
	__BEGIN_TRY

	// write object id
	oStream.write( m_ObjectID );

	// write vampire name
	BYTE szName = m_Name.size();

	if ( szName == 0 )
		throw InvalidProtocolException("szName == 0");

	if ( szName > 20 )
		throw InvalidProtocolException("too long name length");

	oStream.write( szName );

	oStream.write( m_Name );

	// write (X,Y,Dir)
	oStream.write( m_X );
	oStream.write( m_Y );
	oStream.write( m_Dir );
	
	// write sex
	oStream.write( (BYTE)m_Sex );

	//--------------------------------------------------
	// write Shape
	//--------------------------------------------------
	BYTE coatType = (m_ArmType << 4) | m_CoatType;
	oStream.write( coatType );

	//std::cout << "PCVampireInfo3: Name=" << m_Name.c_str() << ", CoatType=" << (int)coatType << std::endl;
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	oStream.write( m_BatType );
#if __CONTENTS(__SECOND_TRANSFORTER)
	oStream.write( m_WingItemType );
#endif //__SECOND_TRANSFORTER
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	// write colors
	for ( uint i = 0 ; i < VAMPIRE_COLOR_MAX ; i ++ )
		oStream.write( m_Colors[i] );

	oStream.write( m_MasterEffectColor );

	oStream.write( m_Born );

	oStream.write( m_CurrentHP );

	oStream.write( m_MaxHP );

	oStream.write( m_AttackSpeed );

	oStream.write( m_Alignment );

	oStream.write( m_Shape );

	oStream.write( m_Competence );

	oStream.write( m_GuildID );
	oStream.write( m_UnionID );

	oStream.write( m_Rank );
	oStream.write( m_AdvancementLevel );

	//std::cout << "승직 레벨 : " << (int)m_AdvancementLevel << std::endl;

#if __CONTENTS(__CONTRIBUTE_SYSTEM)
	oStream.write( m_ContributePoint );
#endif

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get debug std::string
//////////////////////////////////////////////////////////////////////////////
std::string PCVampireInfo3::toString () const 
	
{

	StringStream msg;
	msg << "PCVampireInfo3("
		<< "ObjectID:" << m_ObjectID
		<< ",Name:" << m_Name
		<< ",X:" << (int)m_X
		<< ",Y:" << (int)m_Y
		<< ",Dir:" << Dir2String[m_Dir]
		<< ",Sex:" << Sex2String[m_Sex]
		<< ",CoatType:" << (int)m_CoatType
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
		<< ",BatType:" << m_BatType
#if __CONTENTS(__SECOND_TRANSFORTER)
		<< ",m_WingItemType:" << m_WingItemType
#endif //__SECOND_TRANSFORTER
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
		<< ",BatColor:" << (int)m_Colors[VAMPIRE_COLOR_BAT]
		<< ",AdvanceBatColor:" << (int)m_Colors[VAMPIRE_COLOR_BAT]
		<< ",SkinColor:" << (int)m_Colors[VAMPIRE_COLOR_SKIN]
		<< ",CoatColor:" << (int)m_Colors[VAMPIRE_COLOR_COAT1] << "/" << (int)m_Colors[VAMPIRE_COLOR_COAT2]
		<< ",Born:" << (int)m_Born
		<< ",CurrentHP:" << (int)m_CurrentHP
		<< ",MaxHP:" << (int)m_MaxHP
		<< ",AttackSpeed:" << (int)m_AttackSpeed
		<< ",Alignment:" << (int)m_Alignment
		<< ",Shape:" << (int)m_Shape
		<< ",Competence:" << (int)m_Competence
		<< ",GuildID:" << (int)m_GuildID
		<< ",Rank:" << (int)m_Rank
		<< ",ArmType:" << (int)m_ArmType
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
		<< ",ContributePoint:" << (int)m_ContributePoint
#endif //__CONTRIBUTE_SYSTEM
		<< ")";
	return msg.toString();
}
