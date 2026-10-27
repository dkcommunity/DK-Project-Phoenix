//////////////////////////////////////////////////////////////////////
// 
// Filename    : SGFriendUpdateConnect.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "SGFriendUpdateConnect.h"

SGFriendUpdateConnect::SGFriendUpdateConnect()
{
	m_Receiver.SetMaxLength(20);
	m_PCName.SetMaxLength(20);
	m_ServerName.SetMaxLength(20);
}

SGFriendUpdateConnect::~SGFriendUpdateConnect() throw()
{
}

void SGFriendUpdateConnect::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.read(iStream);
	m_PCName.read(iStream);
	m_ServerName.read(iStream);
	iStream.read(m_Connect);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void SGFriendUpdateConnect::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.write(oStream);
	m_PCName.write(oStream);
	m_ServerName.write(oStream);
	oStream.write( m_Connect );

	__END_CATCH
}

PacketSize_t SGFriendUpdateConnect::getPacketSize() const throw()
{ 
	return m_Receiver.getSize() + m_PCName.getSize() + m_ServerName.getSize() + szBYTE;
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void SGFriendUpdateConnect::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	SGFriendUpdateConnectHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string SGFriendUpdateConnect::toString () const
    throw ()
{
	StringStream msg;

	msg << "SGFriendUpdateConnect ("
		<< "PCName:" << GetPCName().c_str() << ","
		<< "ServerName:" << GetServerName().c_str() << ","
		<< "Connect:" << (int)GetConnect() 
		<< " )";

	return msg.toString();
}

