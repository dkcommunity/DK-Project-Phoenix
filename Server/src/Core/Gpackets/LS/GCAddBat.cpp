//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddBat.cc 
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "GCAddBat.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
// class GCAddBat member methods
//////////////////////////////////////////////////////////////////////////////

GCAddBat::GCAddBat()
{
	m_BatType = Vampire::BAT_TYPE_LEGACY;
	m_Color = 0;
	m_AdvanceColor = 0;
}

void GCAddBat::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	iStream.read( m_ObjectID );

	BYTE szName;

	iStream.read( szName );

	if ( szName == 0 )
		throw InvalidProtocolException("szName == 0");

	if ( szName > 100 )
		throw InvalidProtocolException("too large name length");
		
	iStream.read( m_Name , szName );

	//iStream.read( m_SpriteType );
	//iStream.read( m_MainColor );
	//iStream.read( m_SubColor );

	iStream.read( m_ItemType );
	iStream.read( m_X );
	iStream.read( m_Y );
	iStream.read( m_Dir );
	iStream.read( m_CurrentHP);
	iStream.read( m_MaxHP );
	iStream.read( m_GuildID );

	iStream.read( m_BatType );

	switch(m_BatType)
	{
		case Vampire::BAT_TYPE_LEGACY :
			iStream.read( m_Color );
			iStream.read( m_AdvanceColor ); // 2007.07.24 ½ÂÁ÷ ¹ÚÁã »ö±ò
			break;
		case Vampire::BAT_TYPE_WING :
			iStream.read( m_WingBodyColor );
			iStream.read( m_WingEffectColor );
			break;
		default :
			break;
	}

	__END_CATCH
}

void GCAddBat::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	oStream.write( m_ObjectID );

	BYTE szName = m_Name.size();

	if ( szName == 0 )
		throw InvalidProtocolException("szName == 0");

	if ( szName > 100 )
		throw InvalidProtocolException("too large name length");

	oStream.write( szName );
	oStream.write( m_Name );

	//oStream.write( m_SpriteType );
	//oStream.write( m_MainColor );
	//oStream.write( m_SubColor );

	oStream.write( m_ItemType );
	oStream.write( m_X );
	oStream.write( m_Y );
	oStream.write( m_Dir );
	oStream.write( m_CurrentHP);
	oStream.write( m_MaxHP);
	oStream.write( m_GuildID );

	oStream.write( m_BatType );

	switch(m_BatType)
	{
		case Vampire::BAT_TYPE_LEGACY :
			oStream.write( m_Color );
			oStream.write( m_AdvanceColor ); // 2007.07.24 ½ÂÁ÷ ¹ÚÁã »ö±ò
			break;
		case Vampire::BAT_TYPE_WING :
			oStream.write( m_WingBodyColor );
			oStream.write( m_WingEffectColor );
			break;
		default :
			break;
	}

	__END_CATCH
}

PacketSize_t GCAddBat::getPacketSize() const throw() 
{ 
	PacketSize_t PacketSize;

	PacketSize = szObjectID 
		+ szBYTE + m_Name.size() 
		//+ szSpriteType 
		//+ szColor + szColor
		+ szItemType
		+ szCoord + szCoord + szDir
		+ szHP*2
		+ szGuildID
		+ sizeof(uchar);

	switch(m_BatType)
	{
		case Vampire::BAT_TYPE_LEGACY :
		case Vampire::BAT_TYPE_WING :
			PacketSize += szColor + szColor; 
			break;
		default :
			break;
	}

	return PacketSize;
}

void GCAddBat::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCAddBatHandler::execute( this , pPlayer );
		
	__END_CATCH
}

string GCAddBat::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCAddBat("
		<< "ObjectID:"   << m_ObjectID 
		<< ",Name:"      << m_Name 
		<< ",ItemType:"  << (int)m_ItemType
		<< ",X:"         << (int)m_X 
		<< ",Y:"         << (int)m_Y 
		<< ",Dir:"       << (int)m_Dir 
		<< ",CurrentHP:" << (int)m_CurrentHP
		<< ",MaxHP: "    << (int)m_MaxHP
		<< ",GuildID: "  << (int)m_GuildID
		<< ",BatType: "  << (int)m_BatType;

	switch(m_BatType)
	{
		case Vampire::BAT_TYPE_LEGACY :
			msg << ",Color: "  << (int)m_Color
				<< ",AdvanceColor: "  << (int)m_AdvanceColor;
			break;

		case Vampire::BAT_TYPE_WING :
			msg << ",WingBodyColor: "  << (int)m_WingBodyColor
				<< ",WingEffectColor: "  << (int)m_WingEffectColor
				<< ")" ;
			break;

		default :
			break;
	}

	return msg.toString();

	__END_CATCH
}

PacketSize_t GCAddBatFactory::getPacketMaxSize() const throw() 
{
	PacketSize_t MaxPacketSize;

	MaxPacketSize = szObjectID 
		+ szBYTE + 100 
		+ szItemType
		//+ szSpriteType 
		//+ szColor + szColor
		+ szCoord + szCoord + szDir
		+ szHP
		+ szGuildID
		+ sizeof(uchar);

	MaxPacketSize += szColor + szColor;

	return MaxPacketSize;
}
