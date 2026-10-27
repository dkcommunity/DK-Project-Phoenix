//////////////////////////////////////////////////////////////////////
// 
// Filename    : SGFriendViewInfo.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "SGFriendViewInfo.h"

SGFriendViewInfo::SGFriendViewInfo()
{
	m_Receiver.SetMaxLength(20);
}

SGFriendViewInfo::~SGFriendViewInfo() throw()
{
}

void SGFriendViewInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.read(iStream);
	m_FriendDetailInfo.read(iStream);
	m_FriendIndividualOptionInfo.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void SGFriendViewInfo::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.write(oStream);
	m_FriendDetailInfo.write( oStream );
	m_FriendIndividualOptionInfo.write(oStream);

	__END_CATCH
}

void SGFriendViewInfo::SetFriendDetailInfo(FriendDetailInfo& Info)
{
	m_FriendDetailInfo = Info;
}

FriendDetailInfo& SGFriendViewInfo::GetFriendDetailInfo()
{
	return m_FriendDetailInfo;
}

void SGFriendViewInfo::SetFriendIndividualOptionInfo(FriendIndividualOptionInfo& Info)
{
	m_FriendIndividualOptionInfo = Info;
}

FriendIndividualOptionInfo& SGFriendViewInfo::GetFriendIndividualOptionInfo()
{
	return m_FriendIndividualOptionInfo;
}
	
PacketSize_t SGFriendViewInfo::getPacketSize() const throw() 
{ 
	return m_Receiver.getSize() + m_FriendDetailInfo.getSize() + m_FriendIndividualOptionInfo.getSize(); 
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void SGFriendViewInfo::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	SGFriendViewInfoHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string SGFriendViewInfo::toString () const
    throw ()
{
	StringStream msg;

	msg << "SGFriendViewInfo ("
		<< "Receiver:" << m_Receiver.toString() << ","
		<< "FriendDetailInfo:" << m_FriendDetailInfo.toString() << "," 
		<< "FriendIndividualOptionInfo:" << m_FriendIndividualOptionInfo.toString()
		<< ")";

	return msg.toString();
}

