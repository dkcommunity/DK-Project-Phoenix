//////////////////////////////////////////////////////////////////////////////
// Filename    : GCSendRankGemData.cpp 
// Written By  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "GCSendRankGemData.h"
#include "Assert.h"

GCSendRankGemData::GCSendRankGemData() 
	throw ()
{
	__BEGIN_TRY

	m_RankGemData.clear();
 
	__END_CATCH
}
GCSendRankGemData::~GCSendRankGemData() 
	throw ()
{
	__BEGIN_TRY

	list<RankGemData*>::iterator itr = m_RankGemData.begin();
	list<RankGemData*>::iterator endItr = m_RankGemData.end();

	for ( ; itr != endItr ; ++itr )
	{
		if ( *itr != NULL ) SAFE_DELETE(*itr);
	}

	m_RankGemData.clear();
	
	__END_CATCH
}
void GCSendRankGemData::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
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
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	BYTE totalNum = m_RankGemData.size();
	if ( totalNum > MAX_RANK_GEM_DATA ) throw DisconnectException("GCSendRankGemData :write error.");

	oStream.write( totalNum );
	
	list<RankGemData*>::const_iterator itr = m_RankGemData.begin();
	list<RankGemData*>::const_iterator endItr = m_RankGemData.end();

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
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	GCSendRankGemDataHandler::execute( this , pPlayer );

	__END_DEBUG
	__END_CATCH
}

PacketSize_t GCSendRankGemData::getPacketSize () const 
	throw ()
{ 
	__BEGIN_TRY
	__BEGIN_DEBUG

	PacketSize_t size = szBYTE;

	list<RankGemData*>::const_iterator itr = m_RankGemData.begin();
	list<RankGemData*>::const_iterator endItr = m_RankGemData.end();

	for ( ; itr != endItr ; ++itr )
	{
		size += (*itr)->getPacketSize();
	}

	return size;

	__END_DEBUG
	__END_CATCH
}

string GCSendRankGemData::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCSendRankGemData(";

	list<RankGemData*>::const_iterator itr = m_RankGemData.begin();
	list<RankGemData*>::const_iterator endItr = m_RankGemData.end();

	for ( ; itr != endItr ; ++itr )
	{
		msg << (*itr)->toString();
	}

	msg << ")";
	return msg.toString();
		
	__END_CATCH
}

