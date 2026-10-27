//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCInstanceDoungeonResult.cpp 
// Written By  : rappi76
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCInstanceDoungeonResult.h"


//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
#if __CONTENTS(__TIPOJYU_CASTLE)
GCInstanceDoungeonResult::GCInstanceDoungeonResult()
	
{
}

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCInstanceDoungeonResult::~GCInstanceDoungeonResult() {
}

//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void GCInstanceDoungeonResult::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY
	
	BYTE szDoungeonTitle;

	iStream.read( szDoungeonTitle );
	iStream.read( m_strDoungeonTitle, szDoungeonTitle );
	iStream.read( m_ExpBonus);
	iStream.read( m_RankExpBonus);
	iStream.read( m_RemainTime);

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCInstanceDoungeonResult::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
	
	BYTE szDoungeonTitle;
	
	szDoungeonTitle = m_strDoungeonTitle.length();
		
	oStream.write( szDoungeonTitle );
	oStream.write( m_strDoungeonTitle);
	oStream.write( m_ExpBonus);
	oStream.write( m_RankExpBonus);
	oStream.write( m_RemainTime);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCInstanceDoungeonResult::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
		
	GCInstanceDoungeonResultHandler::execute( this , pPlayer );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet size
//////////////////////////////////////////////////////////////////////
PacketSize_t GCInstanceDoungeonResult::getPacketSize() const
	
{
	__BEGIN_TRY

	return szBYTE + m_strDoungeonTitle.size() + szExp + szRankExp + szDuration;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////
std::string GCInstanceDoungeonResult::toString () const
       
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

#endif //__TIPOJYU_CASTLE
