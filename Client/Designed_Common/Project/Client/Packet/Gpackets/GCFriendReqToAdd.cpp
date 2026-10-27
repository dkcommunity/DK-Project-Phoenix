//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendReqToAdd.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCFriendReqToAdd.h"

#if __CONTENTS(__FRIEND_ADDITION)
GCFriendReqToAdd::GCFriendReqToAdd()
{
	m_PCName.SetMaxLength(20);
}

void GCFriendReqToAdd::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	m_PCName.read(iStream);
	iStream.read( m_ResultCode );
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCFriendReqToAdd::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

	m_PCName.write(oStream);
	oStream.write( m_ResultCode );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCFriendReqToAdd::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCFriendReqToAddHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
std::string GCFriendReqToAdd::toString () const
    
{
	StringStream msg;

	msg << "GCFriendReqToAdd ("
		<< "PCName:" << m_PCName.GetString() << ","
		<< "ResultCode:" << (int)m_ResultCode
		<< " )";

	return msg.toString();
}
#endif //__FRIEND_ADDITION
