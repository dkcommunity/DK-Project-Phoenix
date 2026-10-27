//////////////////////////////////////////////////////////////////////
// 
// Filename    : SGFriendResToAdd.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "SGFriendResToAdd.h"

SGFriendResToAdd::SGFriendResToAdd()
{
	m_Receiver.SetMaxLength(20);
	m_FriendName.SetMaxLength(20);
}

SGFriendResToAdd::~SGFriendResToAdd() throw()
{
}

void SGFriendResToAdd::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.read(iStream);
	m_FriendName.read(iStream);
	iStream.read(m_ResultCode);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void SGFriendResToAdd::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.write( oStream );
	m_FriendName.write( oStream );
	oStream.write(m_ResultCode);

	__END_CATCH
}

PacketSize_t SGFriendResToAdd::getPacketSize() const throw() 
{ 
	return m_Receiver.getSize() + m_FriendName.getSize() + szBYTE; 
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void SGFriendResToAdd::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	SGFriendResToAddHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string SGFriendResToAdd::toString () const
    throw ()
{
	StringStream msg;

	msg << "SGFriendResToAdd ("
		<< "Receiver:" << GetReceiver().c_str() << "," 
		<< "FriendName:" << GetFriendName().c_str() << ","
		<< "GetResultCode:" << GetResultCode() << ","
		<< ")";

	return msg.toString();
}

