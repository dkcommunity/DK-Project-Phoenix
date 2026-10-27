//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendWaitList.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCFriendWaitList.h"

#if __CONTENTS(__FRIEND_ADDITION)
GCFriendWaitList::~GCFriendWaitList() {
}

void GCFriendWaitList::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

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
void GCFriendWaitList::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

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

void GCFriendWaitList::AddFriendWaitInfo(FriendWaitInfo& info)
{
	if ( GetFriendWaitListCount() >= FRIEND_MAX_WAIT_LIST_COUNT )
	{
		return;
	}
	
	m_FriendWaitInfoList.push_back(info);
}

FriendWaitInfo* GCFriendWaitList::GetFriendWaitInfo(int iIndex)
{
	if ( iIndex > GetFriendWaitListCount() )
	{
		return NULL;
	}
	
	return &m_FriendWaitInfoList[iIndex];
}

PacketSize_t GCFriendWaitList::getPacketSize() const 
{ 
	PacketSize_t packetSize;
	
	packetSize = szBYTE;
	
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
void GCFriendWaitList::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCFriendWaitListHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
std::string GCFriendWaitList::toString () const
    
{
	StringStream msg;

	msg << "GCFriendWaitList ("
		<< "ListCount:" << GetFriendWaitListCount();
	
	TFriendWaitInfoList::const_iterator itr = m_FriendWaitInfoList.begin();
	for ( ; itr != m_FriendWaitInfoList.end(); itr++ )
	{
		msg << "FriendInfo:" << (*itr).toString();
	}
	
	msg << " )";

	return msg.toString();
}
#endif //__FRIEND_ADDITION
