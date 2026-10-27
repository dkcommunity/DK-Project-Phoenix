//////////////////////////////////////////////////////////////////////////////
// Filename    : PCOustersInfo3.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "PCOustersInfo3.h"

//////////////////////////////////////////////////////////////////////////////
// read data from socket input stream
//////////////////////////////////////////////////////////////////////////////
void PCOustersInfo3::read ( SocketInputStream & iStream ) 
	
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
	BYTE shapeType;
	iStream.read( shapeType );
	m_CoatType = (OustersCoatType)(shapeType & 15);
	m_ArmType = (OustersArmType)(( shapeType >> 4 ) & 7);	//by viva
	m_SylphType = (OustersSylphType)(( shapeType >> 7 ) & 1);

#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	iStream.read( m_WingSylphType );
#if __CONTENTS(__SECOND_TRANSFORTER)
	iStream.read( m_WingItemType );
#endif //__SECOND_TRANSFORTER
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER

	// read colors
	for ( uint i = 0 ; i < OUSTERS_COLOR_MAX ; i ++ )
		iStream.read( m_Colors[i] );

	iStream.read( m_MasterEffectColor );
	iStream.read( m_Born );
	iStream.read( m_CurrentHP );

	iStream.read( m_MaxHP );

	iStream.read( m_AttackSpeed );

	iStream.read( m_Alignment );

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
void PCOustersInfo3::write ( SocketOutputStream & oStream ) const 
	
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
	BYTE shapeType = 0;
	shapeType = ( m_SylphType << 7 ) | ( m_ArmType << 4 ) | m_CoatType;
	oStream.write( shapeType );
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	oStream.write( m_WingSylphType );
#if __CONTENTS(__SECOND_TRANSFORTER)
	oStream.write( m_WingItemType );
#endif //__SECOND_TRANSFORTER
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	// write colors
	for ( uint i = 0 ; i < OUSTERS_COLOR_MAX ; i ++ )
		oStream.write( m_Colors[i] );

	oStream.write( m_MasterEffectColor );

	oStream.write( m_Born );

	oStream.write( m_CurrentHP );

	oStream.write( m_MaxHP );

	oStream.write( m_AttackSpeed );

	oStream.write( m_Alignment );

	oStream.write( m_Competence );

	oStream.write( m_GuildID );
	oStream.write( m_UnionID );

	oStream.write( m_Rank );
	oStream.write( m_AdvancementLevel );

#if __CONTENTS(__CONTRIBUTE_SYSTEM)
	oStream.write( m_ContributePoint );
#endif //__CONTRIBUTE_SYSTEM

//	std::cout << "승직 레벨 : " << (int)m_AdvancementLevel << std::endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get debug std::string
//////////////////////////////////////////////////////////////////////////////
std::string PCOustersInfo3::toString () const 
	
{

	StringStream msg;
	msg << "PCOustersInfo3("
		<< "ObjectID:" << m_ObjectID
		<< ",Name:" << m_Name
		<< ",X:" << (int)m_X
		<< ",Y:" << (int)m_Y
		<< ",Dir:" << Dir2String[m_Dir]
		<< ",Sex:" << Sex2String[m_Sex]
		<< ",CoatColor:" << (int)m_Colors[OUSTERS_COLOR_COAT]
		<< ",HairColor:" << (int)m_Colors[OUSTERS_COLOR_HAIR]
		<< ",ArmColor:" << (int)m_Colors[OUSTERS_COLOR_ARM]
		<< ",BootsColor:" << (int)m_Colors[OUSTERS_COLOR_BOOTS]
		<< ",CoatType:" << (int)m_CoatType
		<< ",ArmType:" << (int)m_ArmType
		<< ",SylphType:" << (int)m_SylphType
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
		<< ",WingSylphType:" << (int)m_WingSylphType
#if __CONTENTS(__SECOND_TRANSFORTER)
		<< ",m_WingItemType:" << (int)m_WingItemType
#endif __SECOND_TRANSFORTER
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
		<< ",Born:" << (int)m_Born
		<< ",CurrentHP:" << (int)m_CurrentHP
		<< ",MaxHP:" << (int)m_MaxHP
		<< ",AttackSpeed:" << (int)m_AttackSpeed
		<< ",Alignment:" << (int)m_Alignment
		<< ",Competence:" << (int)m_Competence
		<< ",GuildID:" << (int)m_GuildID
		<< ",Rank:" << (int)m_Rank
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
		<< ",ContributePoint:" << (int)m_ContributePoint
#endif //__CONTRIBUTE_SYSTEM
		<< ")";
	return msg.toString();
}
