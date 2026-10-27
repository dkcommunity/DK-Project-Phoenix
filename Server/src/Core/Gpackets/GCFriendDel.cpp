//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendDel.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCFriendDel.h"

GCFriendDel::~GCFriendDel() throw()
{
}

void GCFriendDel::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_PCName.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCFriendDel::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_PCName.write(oStream);

	__END_CATCH
}

PacketSize_t GCFriendDel::getPacketSize() const throw()
{ 
	return m_PCName.getSize();
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCFriendDel::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCFriendDelHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GCFriendDel::toString () const
    throw ()
{
	StringStream msg;

	msg << "GCFriendDel ("
		<< "PCName:" << GetPCName().c_str() << " )";

	return msg.toString();
}

