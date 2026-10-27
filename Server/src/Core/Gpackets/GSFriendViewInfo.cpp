//////////////////////////////////////////////////////////////////////
// 
// Filename    : GSFriendViewInfo.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GSFriendViewInfo.h"

GSFriendViewInfo::GSFriendViewInfo()
{
	m_Sender.SetMaxLength(20);
	m_FriendName.SetMaxLength(20);
}

GSFriendViewInfo::~GSFriendViewInfo() throw()
{
}

void GSFriendViewInfo::read ( SocketInputStream & iStream ) 
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
void GSFriendViewInfo::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.write(oStream);
	m_FriendName.write(oStream);

	__END_CATCH
}

PacketSize_t GSFriendViewInfo::getPacketSize() const throw()
{ 
	return m_Sender.getSize() + m_FriendName.getSize();
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GSFriendViewInfo::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GSFriendViewInfoHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GSFriendViewInfo::toString () const
    throw ()
{
	StringStream msg;

	msg << "GSFriendViewInfo ("
		<< "Sender:" << m_Sender.GetString().c_str() << ","
		<< "FriendName:" << m_FriendName.GetString().c_str() << " )";

	return msg.toString();
}

