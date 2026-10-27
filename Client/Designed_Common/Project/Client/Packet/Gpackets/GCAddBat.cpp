//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddBat.cc 
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCAddBat.h"

//////////////////////////////////////////////////////////////////////////////
// class GCAddBat member methods
//////////////////////////////////////////////////////////////////////////////

void GCAddBat::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	int localLongName = 0;
#if __CONTENTS(__LOCALIZING_LONGNAME)
	localLongName = 12;
#endif //__LOCALIZING_LONGNAME		
	iStream.read( m_ObjectID );

	BYTE szName;

	iStream.read( szName );

	if ( szName == 0 )
		throw InvalidProtocolException("szName == 0");

	if ( szName > 100 + localLongName)
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
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	iStream.read( m_BatType );
	
	switch(m_BatType)
	{
	case 1 :
		iStream.read( m_BatColor );
		iStream.read( m_AdvanceBatColor ); // 2007.07.24 승직 박쥐 색깔
		break;
	case 2:
		iStream.read( m_WingColor1 );
		iStream.read( m_WingColor2 );
		break;
#if __CONTENTS(__SECOND_TRANSFORTER)
	case 3:
		iStream.read( m_WingColor1 );
		iStream.read( m_WingColor2 );
		break;
#endif //__SECOND_TRANSFORTER
	default :
		break;
	}
#else
	iStream.read( m_BatColor );
	iStream.read( m_AdvanceBatColor );
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	__END_CATCH
}

void GCAddBat::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

		
	int localLongName = 0;
#if __CONTENTS(__LOCALIZING_LONGNAME)
	localLongName = 12;
#endif //__LOCALIZING_LONGNAME
	oStream.write( m_ObjectID );

	BYTE szName = m_Name.size();

	if ( szName == 0 )
		throw InvalidProtocolException("szName == 0");

	if ( szName > 100 + localLongName)
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
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
	oStream.write( m_BatType );
	
	switch(m_BatType)
	{
	case 1:
		oStream.write( m_BatColor );
		oStream.write( m_AdvanceBatColor ); // 2007.07.24 승직 박쥐 색깔
		break;
	case 2:
		oStream.write( m_WingColor1 );
		oStream.write( m_WingColor2 );
		break;
#if __CONTENTS(__SECOND_TRANSFORTER)
	case 3:
		oStream.write( m_WingColor1 );
		oStream.write( m_WingColor2 );
		break;
#endif //__SECOND_TRANSFORTER
	default :
		break;
	}
#else
	oStream.write( m_BatColor );
	oStream.write( m_AdvanceBatColor );
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	__END_CATCH
}

void GCAddBat::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCAddBatHandler::execute( this , pPlayer );
		
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
	
	std::string GCAddBat::toString () const
		   
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
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
			<< ",BatType: "  << (int)m_BatType;
		switch(m_BatType)
		{
		case 1 :
			msg << ",Color: "  << (int)m_BatColor
				<< ",AdvanceColor: "  << (int)m_AdvanceBatColor;
			break;
			
		case 2 :
			msg << ",WingColor1: "  << (int)m_WingColor1
				<< ",WingColor2: "  << (int)m_WingColor2
				<< ")" ;
			break;
#if __CONTENTS(__SECOND_TRANSFORTER)
		case 3 :
			msg << ",WingColor1: "  << (int)m_WingColor1
				<< ",WingColor2: "  << (int)m_WingColor2
				<< ")" ;
			break;
#endif //__SECOND_TRANSFORTER
		default :
			break;
		}
#else
			<< ",BatColor: " << (int)m_BatColor
			<< ",AdvanceBatColor: " << (int)m_AdvanceBatColor
			<< ")" ;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
		return msg.toString();

		__END_CATCH
	}
#endif
