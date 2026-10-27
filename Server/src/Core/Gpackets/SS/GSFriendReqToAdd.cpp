//////////////////////////////////////////////////////////////////////
// 
// Filename    : GSFriendReqToAdd.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GSFriendReqToAdd.h"

GSFriendReqToAdd::GSFriendReqToAdd()
{
	m_Sender.SetMaxLength(20);
	m_FriendName.SetMaxLength(20);
}

GSFriendReqToAdd::~GSFriendReqToAdd() throw()
{
}

void GSFriendReqToAdd::read ( SocketInputStream & iStream ) 
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
void GSFriendReqToAdd::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.write( oStream );
	m_FriendName.write( oStream );

	__END_CATCH
}

PacketSize_t GSFriendReqToAdd::getPacketSize() const throw() 
{ 
	return m_Sender.getSize() + m_FriendName.getSize(); 
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GSFriendReqToAdd::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GSFriendReqToAddHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GSFriendReqToAdd::toString () const
    throw ()
{
	StringStream msg;

	msg << "GSFriendReqToAdd ("
		<< "Sender:" << m_Sender.toString() << "," 
		<< "FriendName:" << m_FriendName.toString()
		<< ")";

	return msg.toString();
}

