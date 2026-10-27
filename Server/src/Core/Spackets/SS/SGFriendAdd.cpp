//////////////////////////////////////////////////////////////////////
// 
// Filename    : SGFriendAdd.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "SGFriendAdd.h"

SGFriendAdd::SGFriendAdd()
{
	m_Receiver.SetMaxLength(20);
	m_FriendName.SetMaxLength(20);
}

SGFriendAdd::~SGFriendAdd() throw()
{
}

void SGFriendAdd::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_Receiver.read(iStream);
	m_FriendName.read(iStream);
	m_FriendSimpleInfo.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void SGFriendAdd::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	m_Receiver.write(oStream);
	m_FriendName.write(oStream);
	m_FriendSimpleInfo.write(oStream);

	__END_CATCH
}

PacketSize_t SGFriendAdd::getPacketSize() const throw()
{ 
	return m_Receiver.getSize() + m_FriendName.getSize() + m_FriendSimpleInfo.getSize();
}

void SGFriendAdd::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	SGFriendAddHandler::execute(this, pPlayer);
		
	__END_CATCH
}

void SGFriendAdd::SetFriendSimpleInfo(FriendSimpleInfo& Info)
{
	m_FriendSimpleInfo = Info;
}

FriendSimpleInfo& SGFriendAdd::GetFriendSimpleInfo()
{
	return m_FriendSimpleInfo;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string SGFriendAdd::toString () const
    throw ()
{
	StringStream msg;

	msg << "SGFriendAdd ("
		<< "Receiver:" << m_Receiver.toString() << ","
		<< "FriendName:" << m_FriendName.toString() << ","
		<< "FriendSimpleInfo:" << m_FriendSimpleInfo.toString()
		<< " )";

	return msg.toString();
}

