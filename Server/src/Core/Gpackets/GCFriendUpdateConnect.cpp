//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendUpdateConnect.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCFriendUpdateConnect.h"

GCFriendUpdateConnect::GCFriendUpdateConnect()
{
	m_PCName.SetMaxLength(20);
	m_ServerName.SetMaxLength(20);
}

GCFriendUpdateConnect::~GCFriendUpdateConnect() throw()
{
}

void GCFriendUpdateConnect::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_PCName.read(iStream);
	m_ServerName.read(iStream);
	iStream.read(m_Connect);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCFriendUpdateConnect::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_PCName.write(oStream);
	m_ServerName.write(oStream);
	oStream.write( m_Connect );

	__END_CATCH
}

PacketSize_t GCFriendUpdateConnect::getPacketSize() const throw()
{ 
	return m_PCName.getSize() + m_ServerName.getSize() + szBYTE;
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCFriendUpdateConnect::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCFriendUpdateConnectHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GCFriendUpdateConnect::toString () const
    throw ()
{
	StringStream msg;

	msg << "GCFriendUpdateConnect ("
		<< "PCName:" << GetPCName().c_str()
		<< "ServerName:" << GetServerName().c_str()
		<< "Connect:" << GetConnect() << " )";

	return msg.toString();
}

