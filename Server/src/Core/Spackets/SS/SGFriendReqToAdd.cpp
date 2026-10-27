//////////////////////////////////////////////////////////////////////
// 
// Filename    : SGFriendReqToAdd.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "SGFriendReqToAdd.h"

SGFriendReqToAdd::SGFriendReqToAdd()
{
	m_Receiver.SetMaxLength(20);
	m_FriendName.SetMaxLength(20);
}

SGFriendReqToAdd::~SGFriendReqToAdd() throw()
{
}

void SGFriendReqToAdd::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_Receiver.read(iStream);
	m_FriendName.read(iStream);
	iStream.read( m_ResultCode );
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void SGFriendReqToAdd::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_Receiver.write(oStream);
	m_FriendName.write(oStream);
	oStream.write( m_ResultCode );

	__END_CATCH
}

PacketSize_t SGFriendReqToAdd::getPacketSize() const throw()
{ 
	return m_Receiver.getSize() + m_FriendName.getSize() + szBYTE;
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void SGFriendReqToAdd::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	SGFriendReqToAddHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string SGFriendReqToAdd::toString () const
    throw ()
{
	StringStream msg;

	msg << "SGFriendReqToAdd ("
		<< "Receiver:" << m_Receiver.toString() << ","
		<< "FriendName:" << m_FriendName.toString() << ","
		<< "ResultCode:" << m_ResultCode
		<< " )";

	return msg.toString();
}

