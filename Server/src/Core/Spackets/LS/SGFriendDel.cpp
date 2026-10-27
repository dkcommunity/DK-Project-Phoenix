//////////////////////////////////////////////////////////////////////
// 
// Filename    : SGFriendDel.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "SGFriendDel.h"

SGFriendDel::SGFriendDel()
{
	m_Receiver.SetMaxLength(20);
	m_FriendName.SetMaxLength(20);
}

SGFriendDel::~SGFriendDel() throw()
{
}

void SGFriendDel::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_Receiver.read(iStream);
	m_FriendName.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void SGFriendDel::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_Receiver.write(oStream);
	m_FriendName.write(oStream);

	__END_CATCH
}

PacketSize_t SGFriendDel::getPacketSize() const throw()
{ 
	return m_Receiver.getSize() + m_FriendName.getSize();
}

void SGFriendDel::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	SGFriendDelHandler::execute(this, pPlayer);
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string SGFriendDel::toString () const
    throw ()
{
	StringStream msg;

	msg << "SGFriendDel ("
		<< "Receiver:" << m_Receiver.toString() << ","
		<< "FriendName:" << m_FriendName.toString()
		<< " )";

	return msg.toString();
}

