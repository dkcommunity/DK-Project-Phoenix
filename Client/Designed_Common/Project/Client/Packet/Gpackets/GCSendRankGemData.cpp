//////////////////////////////////////////////////////////////////////////////
// Filename    : GCSendRankGemData.cpp 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "Client.h"
#include "GCSendRankGemData.h"
#include "DarkAssert.h"

GCSendRankGemData::GCSendRankGemData() 
	
{
	__BEGIN_TRY

	m_RankGemData.clear();
 
	__END_CATCH
}
GCSendRankGemData::~GCSendRankGemData() {
	__BEGIN_TRY

	std::list<RankGemData*>::iterator itr = m_RankGemData.begin();
	std::list<RankGemData*>::iterator endItr = m_RankGemData.end();

	for ( ; itr != endItr ; ++itr )
	{
		if ( *itr != NULL ) SAFE_DELETE(*itr);
	}

	m_RankGemData.clear();
	
	__END_CATCH
}
void GCSendRankGemData::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	BYTE totalNum;
	iStream.read( totalNum );
	if ( totalNum > MAX_RANK_GEM_DATA ) throw DisconnectException("GCSendRankGemData : read error.");

	for ( int i=0; i < totalNum ; ++i )
	{
		RankGemData* pGI = new RankGemData;

		iStream.read( pGI->Slot );
		iStream.read( pGI->Type );
		iStream.read( pGI->Grade );

		addRankGemData( pGI );
	}

	__END_CATCH
}
void GCSendRankGemData::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	BYTE totalNum = m_RankGemData.size();
	if ( totalNum > MAX_RANK_GEM_DATA ) throw DisconnectException("GCSendRankGemData :write error.");

	oStream.write( totalNum );
	
	std::list<RankGemData*>::const_iterator itr = m_RankGemData.begin();
	std::list<RankGemData*>::const_iterator endItr = m_RankGemData.end();

	for ( ; itr != endItr ; ++itr )
	{
		RankGemData* pGI = *itr;
		Assert( pGI != NULL );

		oStream.write( pGI->Slot );
		oStream.write( pGI->Type );
		oStream.write( pGI->Grade );
	}

	__END_DEBUG
	__END_CATCH
}

void GCSendRankGemData::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	GCSendRankGemDataHandler::execute( this , pPlayer );

	__END_DEBUG
	__END_CATCH
}

PacketSize_t GCSendRankGemData::getPacketSize () const 
	
{ 
	__BEGIN_TRY
	__BEGIN_DEBUG

	PacketSize_t size = szBYTE;

	std::list<RankGemData*>::const_iterator itr = m_RankGemData.begin();
	std::list<RankGemData*>::const_iterator endItr = m_RankGemData.end();

	for ( ; itr != endItr ; ++itr )
	{
		size += (*itr)->getPacketSize();
	}

	return size;

	__END_DEBUG
	__END_CATCH
}

std::string GCSendRankGemData::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCSendRankGemData(";

	std::list<RankGemData*>::const_iterator itr = m_RankGemData.begin();
	std::list<RankGemData*>::const_iterator endItr = m_RankGemData.end();

	for ( ; itr != endItr ; ++itr )
	{
		msg << (*itr)->toString();
	}

	msg << ")";
	return msg.toString();
		
	__END_CATCH
}

