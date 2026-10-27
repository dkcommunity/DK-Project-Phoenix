//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendList.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCFriendList.h"

#if __CONTENTS(__FRIEND_ADDITION)
const int GCFriendList::m_iMaxFriendList = 20;

GCFriendList::~GCFriendList() {
}

void GCFriendList::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	BYTE szListCount;

	iStream.read( szListCount );
	
	for(int i = 0; i < szListCount; ++i)
	{
		FriendSimpleInfo info;
		
		info.read(iStream);
		AddFriendInfo(info);
	}
	
	m_FriendGlobalOptionInfo.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCFriendList::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY

	BYTE szListCount;

	szListCount = GetFriendListCount();
	 
	oStream.write( szListCount );

	TFriendSimpleInfoList::const_iterator itr = m_FriendSimpleInfoList.begin();
	for ( ; itr != m_FriendSimpleInfoList.end(); itr++ )
	{
		(*itr).write( oStream );
	}
	
	m_FriendGlobalOptionInfo.write(oStream);


	__END_CATCH
}

void GCFriendList::AddFriendInfo(FriendSimpleInfo& info)
{
	if ( GetFriendListCount() >= m_iMaxFriendList )
	{
		return;
	}
	
	m_FriendSimpleInfoList.push_back(info);
}

FriendSimpleInfo* GCFriendList::GetFriendInfo(int iIndex)
{
	if ( iIndex > GetFriendListCount() )
	{
		return NULL;
	}
	
	return &m_FriendSimpleInfoList[iIndex];
}

PacketSize_t GCFriendList::getPacketSize() const 
{ 
	PacketSize_t packetSize;
	
	packetSize = szBYTE;
	
	TFriendSimpleInfoList::const_iterator itr = m_FriendSimpleInfoList.begin();
	for ( ; itr != m_FriendSimpleInfoList.end(); itr++ )
	{
		packetSize += (*itr).getSize();
	}
	
	packetSize += m_FriendGlobalOptionInfo.getSize();
	
	return packetSize;
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCFriendList::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCFriendListHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
std::string GCFriendList::toString () const
    
{
	StringStream msg;

	msg << "GCFriendList ("
		<< "ListCount:" << GetFriendListCount() << ",";
	
	TFriendSimpleInfoList::const_iterator itr = m_FriendSimpleInfoList.begin();
	for ( ; itr != m_FriendSimpleInfoList.end(); itr++ )
	{
		msg << "FriendInfo:" << (*itr).toString();
	}
	
	msg << ",";
	msg << "FriendGlobalOptionInfo" << m_FriendGlobalOptionInfo.toString();
	
	msg << " )";

	return msg.toString();
}
#endif //__FRIEND_ADDITION
