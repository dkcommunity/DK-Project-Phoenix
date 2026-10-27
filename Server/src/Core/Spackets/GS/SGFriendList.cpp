//////////////////////////////////////////////////////////////////////
// 
// Filename    : SGFriendList.cpp 
// Written By  : 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "SGFriendList.h"

SGFriendList::SGFriendList()
{
	m_Receiver.SetMaxLength(20);	
}

SGFriendList::~SGFriendList() throw()
{
}

void SGFriendList::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.read(iStream);
	
	BYTE szListCount;

	iStream.read( szListCount );
	
	for(int i = 0; i < szListCount; ++i)
	{
		FriendSimpleInfo info;
		
		info.read(iStream);
		AddFriendSimpleInfo(info);
	}
	
	m_FriendGlobalOptionInfo.read(iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// Datagram 객체로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void SGFriendList::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Receiver.write(oStream);
	
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

void SGFriendList::AddFriendSimpleInfo(FriendSimpleInfo& info)
{
	if ( GetFriendListCount() >= m_iMaxFriendList )
	{
		return;
	}
	
	m_FriendSimpleInfoList.push_back(info);
}

FriendSimpleInfo* SGFriendList::GetFriendSimpleInfo(int iIndex)
{
	if ( iIndex > GetFriendListCount() )
	{
		return NULL;
	}
	
	return &m_FriendSimpleInfoList[iIndex];
}

PacketSize_t SGFriendList::getPacketSize() const throw()
{ 
	PacketSize_t packetSize;
	
	packetSize = m_Receiver.getSize();
	packetSize += szBYTE;
	
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
void SGFriendList::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	SGFriendListHandler::execute(this, pPlayer);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
string SGFriendList::toString () const
    throw ()
{
	StringStream msg;

	msg << "SGFriendList ("
		<< "ListCount:" << GetFriendListCount() << ",";
	
	TFriendSimpleInfoList::const_iterator itr = m_FriendSimpleInfoList.begin();
	for ( ; itr != m_FriendSimpleInfoList.end(); itr++ )
	{
		msg << "FriendInfo:" << (*itr).toString();
	}
	
	msg << ",";
	msg << "FriendGlobalOptionInfo:" << m_FriendGlobalOptionInfo.toString();
	
	msg << " )";

	return msg.toString();
}

