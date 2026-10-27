//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCInstanceDoungeonLobby.cpp 
// Written By  : rappi76
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCInstanceDoungeonLobby.h"


//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCInstanceDoungeonLobby::GCInstanceDoungeonLobby()
	throw()
{
}

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCInstanceDoungeonLobby::~GCInstanceDoungeonLobby()
	throw()
{
	TInstanceDoungeonInfoItr itr = m_DoungeonList.begin();
	for ( ; itr != m_DoungeonList.end(); itr++ )
	{
		delete (*itr);
	}
}

//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void GCInstanceDoungeonLobby::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	BYTE szDoungeonTitle;
	BYTE szDoungeonListCount;
	BYTE szDoungeonDesc;
	
	iStream.read( m_ObjectID );
	iStream.read( m_NPCID );

	iStream.read( szDoungeonTitle );
	iStream.read( m_strDoungeonTitle, szDoungeonTitle );

	iStream.read( szDoungeonListCount);
	for(int i = 0 ; i < szDoungeonListCount; ++i)
	{
		InstanceDoungeonInfo *pInstanceDoungeonInfo = new InstanceDoungeonInfo();
		pInstanceDoungeonInfo->read(iStream);
		m_DoungeonList.push_back(pInstanceDoungeonInfo);
	}
	
	iStream.read( szDoungeonDesc );
	iStream.read( m_strDoungeonDesc, szDoungeonDesc );

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCInstanceDoungeonLobby::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	BYTE szDoungeonTitle;
	BYTE szDoungeonDesc;
	BYTE szDoungeonListCount;
	
	oStream.write( m_ObjectID );
	oStream.write( m_NPCID );
	
	szDoungeonTitle = m_strDoungeonTitle.length();
	szDoungeonListCount = m_DoungeonList.size();
	szDoungeonDesc = m_strDoungeonDesc.length();
		
	oStream.write( szDoungeonTitle );
	oStream.write( m_strDoungeonTitle);
	
	oStream.write(szDoungeonListCount);
	TInstanceDoungeonInfoItr itr = m_DoungeonList.begin();
	for ( ; itr != m_DoungeonList.end(); itr++ )
	{
		(*itr)->write( oStream );
	}
	
	oStream.write( szDoungeonDesc );
	oStream.write( m_strDoungeonDesc);


	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCInstanceDoungeonLobby::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCInstanceDoungeonLobbyHandler::execute( this , pPlayer );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet size
//////////////////////////////////////////////////////////////////////
PacketSize_t GCInstanceDoungeonLobby::getPacketSize() const
	throw()
{
	__BEGIN_TRY

	PacketSize_t size = 0;
	
	size += szObjectID + szNPCID; 
	size += szBYTE + m_strDoungeonTitle.size();
	
	TInstanceDoungeonInfoItr itr = m_DoungeonList.begin();
	for ( ; itr != m_DoungeonList.end(); itr++ )
	{
		size += (*itr)->getSize();
	}
	
	size += szBYTE + m_strDoungeonDesc.size();
	
	return size;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////
string GCInstanceDoungeonLobby::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	
	msg << "GCInstanceDoungeonLobby("
		<< "DoungeonTitle:" << m_strDoungeonTitle
		<< "DoungeonDesc:" << m_strDoungeonDesc
		<< ")";
	
	return msg.toString();
		
	__END_CATCH
}

