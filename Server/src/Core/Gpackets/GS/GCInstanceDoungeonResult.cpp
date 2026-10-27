//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCInstanceDoungeonResult.cpp 
// Written By  : rappi76
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GCInstanceDoungeonResult.h"


//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCInstanceDoungeonResult::GCInstanceDoungeonResult()
	throw()
{
}

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCInstanceDoungeonResult::~GCInstanceDoungeonResult()
	throw()
{
}

//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void GCInstanceDoungeonResult::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	BYTE szDoungeonTitle;
	BYTE szDoungeonName;
	BYTE szDoungeonDesc;

	iStream.read( szDoungeonTitle );
	iStream.read( m_strDoungeonTitle, szDoungeonTitle );
	
	iStream.read( szDoungeonName );
	iStream.read( m_strDoungeonName, szDoungeonName );
	
	iStream.read( szDoungeonDesc );
	iStream.read( m_strDoungeonDesc, szDoungeonDesc );
		
	iStream.read( m_NPCID);
	
	iStream.read( m_ExpBonus);
	iStream.read( m_RankExpBonus);
	iStream.read( m_RemainTime);

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCInstanceDoungeonResult::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	BYTE szDoungeonTitle;
	BYTE szDoungeonName;
	BYTE szDoungeonDesc;
	
	szDoungeonTitle = m_strDoungeonTitle.length();
	szDoungeonName = m_strDoungeonName.length();
	szDoungeonDesc = m_strDoungeonDesc.length();
		
	oStream.write( szDoungeonTitle );
	oStream.write( m_strDoungeonTitle);
	
	oStream.write( szDoungeonName );
	oStream.write( m_strDoungeonName);
	
	oStream.write( szDoungeonDesc );
	oStream.write( m_strDoungeonDesc);
		
	oStream.write( m_NPCID);
	
	oStream.write( m_ExpBonus);
	oStream.write( m_RankExpBonus);
	oStream.write( m_RemainTime);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCInstanceDoungeonResult::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCInstanceDoungeonResultHandler::execute( this , pPlayer );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet size
//////////////////////////////////////////////////////////////////////
PacketSize_t GCInstanceDoungeonResult::getPacketSize() const
	throw()
{
	__BEGIN_TRY

	return szBYTE + m_strDoungeonTitle.size() + szExp + szRankExp + szDuration;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////
string GCInstanceDoungeonResult::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	
	msg << "GCInstanceDoungeonResult("
		<< "DoungeonTitle:" << m_strDoungeonTitle
		<< "ExpBonus:" << m_ExpBonus
		<< "RankExpBonus:" << m_RankExpBonus
		<< "RemainTime:" << m_RemainTime
		<< ")";
	
	return msg.toString();
		
	__END_CATCH
}

