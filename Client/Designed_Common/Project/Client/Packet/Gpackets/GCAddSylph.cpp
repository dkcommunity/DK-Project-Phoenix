//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddSlyph.cpp
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCAddSylph.h"


//////////////////////////////////////////////////////////////////////////////
// class GCAddSlyph member methods
//////////////////////////////////////////////////////////////////////////////

void GCAddSylph::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
		
	iStream.read( m_ObjectID );

	iStream.read( m_SylphType );

	switch(m_SylphType)
	{
		case 2:
#if __CONTENTS(__SECOND_TRANSFORTER)
			iStream.read( m_WingItemType );
#endif //__SECOND_TRANSFORTER

			iStream.read( m_WingBodyColor );
			iStream.read( m_WingEffectColor );
			break;
		case 1:
		default :
			break;
	}

	__END_CATCH
}

void GCAddSylph::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
		
	oStream.write( m_ObjectID );

	oStream.write( m_SylphType );

	switch(m_SylphType)
	{
		case 2 :
#if __CONTENTS(__SECOND_TRANSFORTER)
			oStream.write( m_WingItemType );
#endif //__SECOND_TRANSFORTER
			oStream.write( m_WingBodyColor );
			oStream.write( m_WingEffectColor );
			break;
		case 1 :
		default :
			break;
	}

	__END_CATCH
}

PacketSize_t GCAddSylph::getPacketSize() const  
{ 
	PacketSize_t PacketSize;

	PacketSize = szObjectID 
		+ sizeof(uchar);

	switch(m_SylphType)
	{
		case 2 :
			PacketSize += szColor + szColor;
#if __CONTENTS(__SECOND_TRANSFORTER)
			PacketSize += szItemType;
#endif //__SECOND_TRANSFORTER

			break;
		case 1 :
		default :
			break;
	}

	return PacketSize;
}

void GCAddSylph::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCAddSylphHandler::execute( this , pPlayer );
		
	__END_CATCH
}
#ifdef __DEBUG_OUTPUT__
std::string GCAddSylph::toString () const
       
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCAddSylph("
		<< "ObjectID:"   << m_ObjectID 
		<< ",SylphType: "  << (int)m_SylphType;

	switch(m_SylphType)
	{
		case 2 :
			msg 
#if __CONTENTS(__SECOND_TRANSFORTER)
				<< ",WingItemType: "  << (int)m_WingItemType
#endif //__SECOND_TRANSFORTER
				<< ",WingBodyColor: "  << (int)m_WingBodyColor
				<< ",WingEffectColor: "  << (int)m_WingEffectColor
				<< ")" ;
			break;

		case 1 :
		default :
			break;
	}

	return msg.toString();

	__END_CATCH
}
#endif //__DEBUG_OUTPUT__
PacketSize_t GCAddSylphFactory::getPacketMaxSize() const  
{
	PacketSize_t MaxPacketSize;

	MaxPacketSize = szObjectID 
		+ sizeof(uchar);

	MaxPacketSize += 
#if __CONTENTS(__SECOND_TRANSFORTER)
		szItemType +
#endif //__SECOND_TRANSFORTER
		szColor + szColor;

	return MaxPacketSize;
}
