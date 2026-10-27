//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendAdd.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCFriendAdd.h"

GCFriendAdd::~GCFriendAdd() throw()
{
}

void GCFriendAdd::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_FriendSimpleInfo.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCFriendAdd::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_FriendSimpleInfo.write( oStream );

	__END_CATCH
}

PacketSize_t GCFriendAdd::getPacketSize() const throw() 
{ 
	return m_FriendSimpleInfo.getSize(); 
}

void GCFriendAdd::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCFriendAddHandler::execute(this, pPlayer);
		
	__END_CATCH
}

void GCFriendAdd::SetFriendSimpleInfo(FriendSimpleInfo& Info)
{
	m_FriendSimpleInfo = Info;
}

FriendSimpleInfo& GCFriendAdd::GetFriendSimpleInfo()
{
	return m_FriendSimpleInfo;
}

string GCFriendAdd::toString () const
    throw ()
{
	StringStream msg;

	msg << "GCFriendAdd ("
		<< "FriendSimpleInfo:" << m_FriendSimpleInfo.toString() << ")";

	return msg.toString();
}

