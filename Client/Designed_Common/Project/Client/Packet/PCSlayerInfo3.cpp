//////////////////////////////////////////////////////////////////////////////
// Filename    : PCSlayerInfo3.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "PCSlayerInfo3.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void PCSlayerInfo3::read ( SocketInputStream & iStream ) 
	
{
	__BEGIN_TRY

	try 
	{
		// read OID
		iStream.read( m_ObjectID );

		// read name
		BYTE szName;
		iStream.read( szName );

		if ( szName == 0 )
			throw InvalidProtocolException("szName == 0");

		if ( szName > 20 )
			throw InvalidProtocolException("too large name length");

		iStream.read( m_Name , szName );

		// read (X,Y,Dir)
		iStream.read( m_X );
		iStream.read( m_Y );
		iStream.read( m_Dir );

		iStream.read( m_Tou );
		iStream.read( m_Yi );
		iStream.read( m_Ku );
		iStream.read( m_Wuqi );
		iStream.read( m_Dun );
		iStream.read( m_Moto );
		iStream.read( m_Jian );

		// read outlook bitset
		DWORD outlook;
		iStream.read( outlook);
		m_Outlook = bitset<SLAYER_BIT_MAX>(outlook);
		
		// read colors
		for ( uint i = 0 ; i < SLAYER_COLOR_MAX ; i ++ )
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
#endif //__CONTRIBUTE_SYSTEM
		
#if __CONTENTS(__SECOND_TRANSFORTER)
		iStream.read( m_MotorItemType );
#endif //__SECOND_TRANSFORTER
	}
	catch ( Throwable & t ) 
	{
		std::cout << t.toString().c_str() << std::endl;
	}
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////////////
void PCSlayerInfo3::write ( SocketOutputStream & oStream ) const 
	
{
	__BEGIN_TRY

	try 
	{
		// write OID
		oStream.write( m_ObjectID );

		// write name
		BYTE szName = m_Name.size();

		if ( szName == 0 )
			throw InvalidProtocolException("szName == 0");

		if ( szName > 20 )
			throw InvalidProtocolException("too large name length");

		oStream.write( szName );
		oStream.write( m_Name );

		// write (X,Y,Dir)
		oStream.write( m_X );
		oStream.write( m_Y );
		oStream.write( m_Dir );

		oStream.write( m_Tou );
		oStream.write( m_Yi );
		oStream.write( m_Ku );
		oStream.write( m_Wuqi );
		oStream.write( m_Dun );
		oStream.write( m_Moto );
		oStream.write( m_Jian );

		// write outlook bitset
		DWORD outlook = m_Outlook.to_ulong();
		oStream.write( outlook );
		
		// write colors
		for ( uint i = 0 ; i < SLAYER_COLOR_MAX ; i ++ )
			oStream.write( m_Colors[i] );

		oStream.write( m_MasterEffectColor );
		oStream.write( m_Born );
		oStream.write( m_CurrentHP );
		oStream.write( m_MaxHP );

		oStream.write( m_AttackSpeed );

		oStream.write( m_Alignment );
		
		oStream.write( m_Competence );
		oStream.write( m_GuildID);
		oStream.write( m_UnionID);

		oStream.write( m_Rank );
		oStream.write( m_AdvancementLevel );
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
		oStream.write( m_ContributePoint );
#endif //__CONTRIBUTE_SYSTEM
		//std::cout << "승직 레벨 : " << (int)m_AdvancementLevel << std::endl;

#if __CONTENTS(__SECOND_TRANSFORTER)
		oStream.write( m_MotorItemType );
#endif //__SECOND_TRANSFORTER

	} 
	catch ( Throwable & t ) 
	{
		std::cout << t.toString().c_str() << std::endl;
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////////////
std::string PCSlayerInfo3::toString () const 
	
{
	StringStream msg;

	msg << "PCSlayerInfo3("
		<< "ObjectID:" << m_ObjectID
		<< ",Name:" << m_Name
		<< ",X:" << (int)m_X
		<< ",Y:" << (int)m_Y
		<< ",Dir:" << Dir2String[m_Dir]
		<< ",Sex:" << Sex2String[getSex()]
		<< ",HairStyle:" << HairStyle2String[getHairStyle()]
		<< ",HairColor:" << (int)getHairColor()
		<< ",SkinColor:" << (int)getSkinColor()
		<< ",Helmet:" << HelmetType2String[getHelmetType()];

	if (getHelmetType()!=HELMET_NONE)
		msg << ",HelmetColor:" << (int)getHelmetColor();

	msg << ",Jacket:" << JacketType2String[getJacketType()]
		<< ",JacketColor:" << (int)getJacketColor(MAIN_COLOR) << "/" << (int)getJacketColor(SUB_COLOR)
		<< ",Pants:" << PantsType2String[getPantsType()]
		<< ",PantsColor:" << (int)getPantsColor(MAIN_COLOR) << "/" << (int)getPantsColor(SUB_COLOR)
		<< ",Weapon:" << WeaponType2String[getWeaponType()]
		<< ",WeaponColor:" << (int)getWeaponColor()
		<< ",Shield:" << ShieldType2String[getShieldType()]
		<< ",ShieldColor:" << (int)getShieldColor()
		<< ",Motorcycle:" << MotorcycleType2String[getMotorcycleType()]
		<< ",Shoulder:" << (int)getShoulderType();

	if (getMotorcycleType()!=MOTORCYCLE_NONE)
		msg << ",MotorcycleColor:" << (int)getMotorcycleColor();

	msg << ",Born:" << (int)m_Born;
	msg << ",CurerntHP:" << (int)m_CurrentHP;
	msg << ",MaxHP:" << (int)m_MaxHP;
	msg << ",Alignment:" << (int)m_Alignment;
	msg << ",AttackSpeed:" << (int)m_AttackSpeed;
	msg << ",Competence:" << (int)m_Competence;
	msg << ",GuildID:" << (int)m_GuildID;
	msg << ",Rank:" << (int)m_Rank;
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
	msg	<< ",ContributePoint:" << (int)m_ContributePoint;
#endif //__CONTRIBUTE_SYSTEM
#if __CONTENTS(__SECOND_TRANSFORTER)
	msg << ",m_MotorItemType:" << (int)m_MotorItemType;
#endif //__SECOND_TRANSFORTER
	msg << ")";

	return msg.toString();
}
