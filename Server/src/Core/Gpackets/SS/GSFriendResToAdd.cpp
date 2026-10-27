//////////////////////////////////////////////////////////////////////
// 
// Filename    : GSFriendResToAdd.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GSFriendResToAdd.h"

GSFriendResToAdd::GSFriendResToAdd()
{
	m_Sender.SetMaxLength(20);
	m_FriendName.SetMaxLength(20);
}

GSFriendResToAdd::~GSFriendResToAdd() throw()
{
}

void GSFriendResToAdd::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.read(iStream);
	m_FriendName.read(iStream);
	iStream.read(m_Response);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GSFriendResToAdd::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.write( oStream );
	m_FriendName.write( oStream );
	oStream.write(m_Response);

	__END_CATCH
}

PacketSize_t GSFriendResToAdd::getPacketSize() const throw() 
{ 
	return m_Sender.getSize() + m_FriendName.getSize() + szBYTE; 
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GSFriendResToAdd::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GSFriendResToAddHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GSFriendResToAdd::toString () const
    throw ()
{
	StringStream msg;

	msg << "GSFriendResToAdd ("
		<< "Sender:" << GetSender().c_str() << "," 
		<< "FriendName:" << GetFriendName().c_str() << ","
		<< "Response:" << GetResponse() << ","
		<< ")";

	return msg.toString();
}

