//////////////////////////////////////////////////////////////////////
// 
// Filename    : GSFriendUpdateGlobalOption.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GSFriendUpdateGlobalOption.h"

GSFriendUpdateGlobalOption::GSFriendUpdateGlobalOption()
{
	m_Sender.SetMaxLength(20);
}

GSFriendUpdateGlobalOption::~GSFriendUpdateGlobalOption() throw()
{
}

void GSFriendUpdateGlobalOption::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.read(iStream);
	m_FriendGlobalOptionInfo.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GSFriendUpdateGlobalOption::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Sender.write(oStream);
	m_FriendGlobalOptionInfo.write(oStream);

	__END_CATCH
}

PacketSize_t GSFriendUpdateGlobalOption::getPacketSize() const throw()
{ 
	return m_Sender.getSize() + m_FriendGlobalOptionInfo.getSize();
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GSFriendUpdateGlobalOption::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GSFriendUpdateGlobalOptionHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GSFriendUpdateGlobalOption::toString () const
    throw ()
{
	StringStream msg;

	msg << "GSFriendUpdateGlobalOption ("
		<< "Sender:" << m_Sender.GetString().c_str() << ","
		<< "FriendGlobalOptionInfo:" << m_FriendGlobalOptionInfo.toString() 
		<< " )";

	return msg.toString();
}

