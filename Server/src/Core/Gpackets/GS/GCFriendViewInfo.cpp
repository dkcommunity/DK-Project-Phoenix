//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendViewInfo.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCFriendViewInfo.h"

GCFriendViewInfo::GCFriendViewInfo()
{
}

GCFriendViewInfo::~GCFriendViewInfo() throw()
{
}

void GCFriendViewInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_FriendDetailInfo.read(iStream);
	m_FriendIndividualOptionInfo.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCFriendViewInfo::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_FriendDetailInfo.write( oStream );
	m_FriendIndividualOptionInfo.write(oStream);

	__END_CATCH
}

void GCFriendViewInfo::SetFriendDetailInfo(FriendDetailInfo& Info)
{
	m_FriendDetailInfo = Info;
}

FriendDetailInfo& GCFriendViewInfo::GetFriendDetailInfo()
{
	return m_FriendDetailInfo;
}

void GCFriendViewInfo::SetFriendIndividualOptionInfo(FriendIndividualOptionInfo& Info)
{
	m_FriendIndividualOptionInfo = Info;
}

FriendIndividualOptionInfo& GCFriendViewInfo::GetFriendIndividualOptionInfo()
{
	return m_FriendIndividualOptionInfo;	
}
	
PacketSize_t GCFriendViewInfo::getPacketSize() const throw() 
{ 
	return m_FriendDetailInfo.getSize() + m_FriendIndividualOptionInfo.getSize(); 
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCFriendViewInfo::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCFriendViewInfoHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GCFriendViewInfo::toString () const
    throw ()
{
	StringStream msg;

	msg << "GCFriendViewInfo ("
		<< "FriendDetailInfo:" << m_FriendDetailInfo.toString() << "," 
		<< "FriendIndividualOptionInfo:" << m_FriendIndividualOptionInfo.toString()
		<< ")";

	return msg.toString();
}

