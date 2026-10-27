//////////////////////////////////////////////////////////////////////
// 
// Filename    : GSFriendUpdateZone.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GSFriendUpdateZone.h"

GSFriendUpdateZone::GSFriendUpdateZone()
{
	m_Sender.SetMaxLength(20);
}

GSFriendUpdateZone::~GSFriendUpdateZone() throw()
{
}

void GSFriendUpdateZone::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.read(iStream);
	iStream.read(m_ZoneID);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GSFriendUpdateZone::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.write(oStream);
	oStream.write( m_ZoneID );

	__END_CATCH
}

PacketSize_t GSFriendUpdateZone::getPacketSize() const throw()
{ 
	return m_Sender.getSize() + szZoneID;
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GSFriendUpdateZone::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GSFriendUpdateZoneHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GSFriendUpdateZone::toString () const
    throw ()
{
	StringStream msg;

	msg << "GSFriendUpdateZone ("
		<< "Sender:" << m_Sender.GetString().c_str() << ","
		<< "ZoneID:" << GetZoneID() << " )";

	return msg.toString();
}

