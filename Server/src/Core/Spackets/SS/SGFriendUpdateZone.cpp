//////////////////////////////////////////////////////////////////////
// 
// Filename    : SGFriendUpdateZone.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "SGFriendUpdateZone.h"

SGFriendUpdateZone::SGFriendUpdateZone()
{
	m_Receiver.SetMaxLength(20);
	m_PCName.SetMaxLength(20);
}

SGFriendUpdateZone::~SGFriendUpdateZone() throw()
{
}

void SGFriendUpdateZone::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.read(iStream);
	m_PCName.read(iStream);
	iStream.read(m_ZoneID);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void SGFriendUpdateZone::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.write(oStream);
	m_PCName.write(oStream);
	oStream.write( m_ZoneID );

	__END_CATCH
}

PacketSize_t SGFriendUpdateZone::getPacketSize() const throw()
{ 
	return m_Receiver.getSize() + m_PCName.getSize() + szZoneID;
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void SGFriendUpdateZone::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	SGFriendUpdateZoneHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string SGFriendUpdateZone::toString () const
    throw ()
{
	StringStream msg;

	msg << "SGFriendUpdateZone ("
		<< "Receiver:" << m_Receiver.GetString()
		<< "PCName:" << m_PCName.GetString()
		<< "ZoneID:" << GetZoneID() << " )";

	return msg.toString();
}

