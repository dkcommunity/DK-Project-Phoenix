//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCFriendList.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCFriendList.h"

GCFriendList::~GCFriendList() throw()
{
}

void GCFriendList::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
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
     throw ( ProtocolException , Error )
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

PacketSize_t GCFriendList::getPacketSize() const throw()
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
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCFriendListHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string GCFriendList::toString () const
    throw ()
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

