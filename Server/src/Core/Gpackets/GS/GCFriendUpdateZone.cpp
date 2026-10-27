//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendUpdateZone.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCFriendUpdateZone.h"

GCFriendUpdateZone::~GCFriendUpdateZone() throw()
{
}

void GCFriendUpdateZone::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_PCName.read(iStream);
	iStream.read(m_ZoneID);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCFriendUpdateZone::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_PCName.write(oStream);
	oStream.write( m_ZoneID );

	__END_CATCH
}

PacketSize_t GCFriendUpdateZone::getPacketSize() const throw()
{ 
	return m_PCName.getSize() + szZoneID;
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCFriendUpdateZone::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCFriendUpdateZoneHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GCFriendUpdateZone::toString () const
    throw ()
{
	StringStream msg;

	msg << "GCFriendUpdateZone ("
		<< "PCName:" << GetPCName().c_str()
		<< "ZoneID:" << GetZoneID() << " )";

	return msg.toString();
}

