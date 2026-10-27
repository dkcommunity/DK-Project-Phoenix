//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendReqToAdd.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCFriendReqToAdd.h"


GCFriendReqToAdd::GCFriendReqToAdd()
{
	m_PCName.SetMaxLength(20);
}

void GCFriendReqToAdd::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
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
     throw ( ProtocolException , Error )
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
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCFriendReqToAddHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GCFriendReqToAdd::toString () const
    throw ()
{
	StringStream msg;

	msg << "GCFriendReqToAdd ("
		<< "PCName:" << m_PCName.GetString() << ","
		<< "ResultCode:" << (int)m_ResultCode
		<< " )";

	return msg.toString();
}

