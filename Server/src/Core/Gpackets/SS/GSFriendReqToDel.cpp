//////////////////////////////////////////////////////////////////////
// 
// Filename    : GSFriendReqToDel.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GSFriendReqToDel.h"

GSFriendReqToDel::GSFriendReqToDel()
{
	m_Sender.SetMaxLength(20);
	m_FriendName.SetMaxLength(20);
}

GSFriendReqToDel::~GSFriendReqToDel() throw()
{
}

void GSFriendReqToDel::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.read(iStream);
	m_FriendName.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GSFriendReqToDel::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.write( oStream );
	m_FriendName.write( oStream );

	__END_CATCH
}

PacketSize_t GSFriendReqToDel::getPacketSize() const throw() 
{ 
	return m_Sender.getSize() + m_FriendName.getSize(); 
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GSFriendReqToDel::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GSFriendReqToDelHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GSFriendReqToDel::toString () const
    throw ()
{
	StringStream msg;

	msg << "GSFriendReqToDel ("
		<< "Sender:" << m_Sender.toString() << "," 
		<< "FriendName:" << m_FriendName.toString()
		<< ")";

	return msg.toString();
}

