//////////////////////////////////////////////////////////////////////
// 
// Filename    : SGFriendSuggestToAdd.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "SGFriendSuggestToAdd.h"

SGFriendSuggestToAdd::SGFriendSuggestToAdd() throw()
{
	m_Receiver.SetMaxLength(20);
	m_FriendName.SetMaxLength(20);
}

SGFriendSuggestToAdd::~SGFriendSuggestToAdd() throw()
{
}

void SGFriendSuggestToAdd::read ( SocketInputStream & iStream ) 
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
void SGFriendSuggestToAdd::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_Receiver.write(oStream);
	m_FriendName.write(oStream);

	__END_CATCH
}

PacketSize_t SGFriendSuggestToAdd::getPacketSize() const throw()
{ 
	return m_Receiver.getSize() + m_FriendName.getSize();
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void SGFriendSuggestToAdd::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	SGFriendSuggestToAddHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string SGFriendSuggestToAdd::toString () const
    throw ()
{
	StringStream msg;

	msg << "SGFriendSuggestToAdd ("
		<< "Receiver:" << m_Receiver.toString()
		<< "FriendName:" << m_FriendName.toString()
		<< " )";

	return msg.toString();
}

