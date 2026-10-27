//////////////////////////////////////////////////////////////////////
// 
// Filename    : SGFriendWaitList.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "SGFriendWaitList.h"

SGFriendWaitList::SGFriendWaitList()
{
	m_Receiver.SetMaxLength(20);
}

SGFriendWaitList::~SGFriendWaitList() throw()
{
}

void SGFriendWaitList::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.read(iStream);
	
	BYTE szListCount;

	iStream.read( szListCount );
	
	for(int i = 0; i < szListCount; ++i)
	{
		FriendWaitInfo info;
		
		info.read(iStream);
		AddFriendWaitInfo(info);
	}
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void SGFriendWaitList::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.write(oStream);
	
	BYTE szListCount;

	szListCount = GetFriendWaitListCount();
	 
	oStream.write( szListCount );

	TFriendWaitInfoList::const_iterator itr = m_FriendWaitInfoList.begin();
	for ( ; itr != m_FriendWaitInfoList.end(); itr++ )
	{
		(*itr).write( oStream );
	}


	__END_CATCH
}

void SGFriendWaitList::AddFriendWaitInfo(FriendWaitInfo& info)
{
	if ( GetFriendWaitListCount() >= FRIEND_MAX_WAIT_LIST_COUNT )
	{
		return;
	}
	
	m_FriendWaitInfoList.push_back(info);
}

FriendWaitInfo* SGFriendWaitList::GetFriendWaitInfo(int iIndex)
{
	if ( iIndex > GetFriendWaitListCount() )
	{
		return NULL;
	}
	
	return &m_FriendWaitInfoList[iIndex];
}

PacketSize_t SGFriendWaitList::getPacketSize() const throw()
{ 
	PacketSize_t packetSize;
	
	packetSize = m_Receiver.getSize();
	packetSize += szBYTE;
	
	TFriendWaitInfoList::const_iterator itr = m_FriendWaitInfoList.begin();
	for ( ; itr != m_FriendWaitInfoList.end(); itr++ )
	{
		packetSize += (*itr).getSize();
	}
	
	return packetSize;
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void SGFriendWaitList::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	SGFriendWaitListHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string SGFriendWaitList::toString () const
    throw ()
{
	StringStream msg;

	msg << "SGFriendWaitList ("
		<< "Receiver:" << m_Receiver.toString() << ","
		<< "ListCount:" << GetFriendWaitListCount();
	
	TFriendWaitInfoList::const_iterator itr = m_FriendWaitInfoList.begin();
	for ( ; itr != m_FriendWaitInfoList.end(); itr++ )
	{
		msg << "FriendInfo:" << (*itr).toString();
	}
	
	msg << " )";

	return msg.toString();
}

