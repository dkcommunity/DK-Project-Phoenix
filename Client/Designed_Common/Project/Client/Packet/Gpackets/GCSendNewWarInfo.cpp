//////////////////////////////////////////////////////////////////////////////
// Filename    : GCSendNewWarInfo.cpp 
// Written By  : ±èŒº¹Î
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCSendNewWarInfo.h"
#include "DarkAssert.h"
#include "Client.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
GCSendNewWarInfo::GCSendNewWarInfo() 

{
	__BEGIN_TRY

		m_RaceData.clear();

	m_ScoreListSlayer.clear();

	m_ScoreListVampire.clear();

	m_ScoreListOusters.clear();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GCSendNewWarInfo::~GCSendNewWarInfo() {
	__BEGIN_TRY

		std::list<RaceData*>::iterator itr = m_RaceData.begin();
	std::list<RaceData*>::iterator endItr = m_RaceData.end();

	for ( ; itr != endItr ; ++itr )
	{
		if ( *itr != NULL ) SAFE_DELETE(*itr);
	}

	m_RaceData.clear();

	std::list<ScoreListSlayer*>::iterator itrScoreListSlayer = m_ScoreListSlayer.begin();
	std::list<ScoreListSlayer*>::iterator endItrScoreListSlayer = m_ScoreListSlayer.end();

	for ( ; itrScoreListSlayer != endItrScoreListSlayer ; ++itrScoreListSlayer )
	{
		if ( *itrScoreListSlayer != NULL ) SAFE_DELETE(*itrScoreListSlayer);
	}

	m_ScoreListSlayer.clear();

	std::list<ScoreListVampire*>::iterator itrScoreListVampire = m_ScoreListVampire.begin();
	std::list<ScoreListVampire*>::iterator endItrScoreListVampire = m_ScoreListVampire.end();

	for ( ; itrScoreListVampire != endItrScoreListVampire ; ++itrScoreListVampire )
	{
		if ( *itrScoreListVampire != NULL ) SAFE_DELETE(*itrScoreListVampire);
	}

	m_ScoreListVampire.clear();

	std::list<ScoreListOusters*>::iterator itrScoreListOusters = m_ScoreListOusters.begin();
	std::list<ScoreListOusters*>::iterator endItrScoreListOusters = m_ScoreListOusters.end();

	for ( ; itrScoreListOusters != endItrScoreListOusters ; ++itrScoreListOusters )
	{
		if ( *itrScoreListOusters != NULL ) SAFE_DELETE(*itrScoreListOusters);
	}

	m_ScoreListOusters.clear();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ÀÔ·ÂœºÆ®ž²(¹öÆÛ)Àž·ÎºÎÅÍ µ¥ÀÌÅžžŠ ÀÐŸîŒ­ ÆÐÅ¶À» ÃÊ±âÈ­ÇÑŽÙ.
//////////////////////////////////////////////////////////////////////////////
void GCSendNewWarInfo::read ( SocketInputStream & iStream ) 

{
	__BEGIN_TRY

		BYTE totalNum;
	iStream.read( totalNum );
	if ( totalNum > MAX_RACE ) throw DisconnectException("GCSendNewWarInfo : read error.");

	for ( int i=0; i < totalNum ; ++i )
	{
		RaceData* pGI = new RaceData;

		iStream.read( pGI->BookCount );
		BYTE optionNum;
		iStream.read( optionNum );
		pGI->RaceBookType.clear();

		for ( int j=0; j < optionNum ; ++j )
		{
			ItemType_t RaceBookType;
			iStream.read( RaceBookType );
			pGI->RaceBookType.push_back( RaceBookType );
		}

		iStream.read( pGI->RaceKillCount );
		iStream.read( pGI->RaceDeadCount );
		iStream.read( pGI->RacePoint );

		addRaceData( pGI );
	}

	BYTE totalSlayerRankNum;
	iStream.read( totalSlayerRankNum );
	if ( totalSlayerRankNum > MAX_RANK ) throw DisconnectException("GCSendNewWarInfo : max Slayer Ranknum read error.");

	for ( int i=0; i < totalSlayerRankNum ; ++i )
	{
		ScoreListSlayer* pScoreList = new ScoreListSlayer;

		BYTE szName;

		iStream.read( szName );

		if ( szName == 0 )
			throw InvalidProtocolException("szName == 0");

		if ( szName > 20 )
			throw InvalidProtocolException("too long name length");

		iStream.read( pScoreList->RankName , szName );
		iStream.read( pScoreList->RankKillCount );
		iStream.read( pScoreList->RankDeadCount );
		iStream.read( pScoreList->RankPoint );

		addScoreListSlayer( pScoreList );
	}

	BYTE totalVampireRankNum;
	iStream.read( totalVampireRankNum );
	if ( totalVampireRankNum > MAX_RANK ) throw DisconnectException("GCSendNewWarInfo : max Vampire Ranknum read error.");

	for ( int i=0; i < totalVampireRankNum ; ++i )
	{
		ScoreListVampire* pScoreList = new ScoreListVampire;

		BYTE szName;

		iStream.read( szName );

		if ( szName == 0 )
			throw InvalidProtocolException("szName == 0");

		if ( szName > 20 )
			throw InvalidProtocolException("too long name length");

		iStream.read( pScoreList->RankName , szName );
		iStream.read( pScoreList->RankKillCount );
		iStream.read( pScoreList->RankDeadCount );
		iStream.read( pScoreList->RankPoint );

		addScoreListVampire( pScoreList );
	}

	BYTE totalOustersRankNum;
	iStream.read( totalOustersRankNum );
	if ( totalOustersRankNum > MAX_RANK ) throw DisconnectException("GCSendNewWarInfo : max Ousters Ranknum read error.");

	for ( int i=0; i < totalOustersRankNum ; ++i )
	{
		ScoreListOusters* pScoreList = new ScoreListOusters;

		BYTE szName;

		iStream.read( szName );

		if ( szName == 0 )
			throw InvalidProtocolException("szName == 0");

		if ( szName > 20 )
			throw InvalidProtocolException("too long name length");

		iStream.read( pScoreList->RankName , szName );
		iStream.read( pScoreList->RankKillCount );
		iStream.read( pScoreList->RankDeadCount );
		iStream.read( pScoreList->RankPoint );

		addScoreListOusters( pScoreList );
	}

	iStream.read( m_PlayerKillCount );
	iStream.read( m_PlayerDeadCount );
	iStream.read( m_PlayerRacePoint );
	iStream.read( m_PlayerKillPoint );
	iStream.read( m_PlayerTotalPoint );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// Ãâ·ÂœºÆ®ž²(¹öÆÛ)Àž·Î ÆÐÅ¶ÀÇ ¹ÙÀÌ³Êž® ÀÌ¹ÌÁöžŠ ºž³œŽÙ.
//////////////////////////////////////////////////////////////////////////////
void GCSendNewWarInfo::write ( SocketOutputStream & oStream ) const 

{
	__BEGIN_TRY
		__BEGIN_DEBUG

		BYTE totalNum = m_RaceData.size();
	if ( totalNum > MAX_RACE ) throw DisconnectException("GCSendNewWarInfo :write error.");

	oStream.write( totalNum );

	std::list<RaceData*>::const_iterator itr = m_RaceData.begin();
	std::list<RaceData*>::const_iterator endItr = m_RaceData.end();

	for ( ; itr != endItr ; ++itr )
	{
		RaceData* pGI = *itr;
		Assert( pGI != NULL );

		oStream.write( pGI->BookCount );

		BYTE optionNum = pGI->RaceBookType.size();
		oStream.write( optionNum );
		std::list<ItemType_t>::const_iterator oitr = pGI->RaceBookType.begin();
		std::list<ItemType_t>::const_iterator endoItr = pGI->RaceBookType.end();

		for ( ; oitr != endoItr ; ++oitr )
		{
			oStream.write( *oitr );
		}

		oStream.write( pGI->RaceKillCount );
		oStream.write( pGI->RaceDeadCount );
		oStream.write( pGI->RacePoint );
	}

	BYTE totalSlayerRankNum = m_ScoreListSlayer.size();
	if ( totalSlayerRankNum > MAX_RANK ) throw DisconnectException("GCSendNewWarInfo :Max Slayer Rank Num write error.");

	oStream.write( totalSlayerRankNum );

	std::list<ScoreListSlayer*>::const_iterator itrScoreListSlayer = m_ScoreListSlayer.begin();
	std::list<ScoreListSlayer*>::const_iterator endItrScoreListSlayer = m_ScoreListSlayer.end();

	for ( ; itrScoreListSlayer != endItrScoreListSlayer ; ++itrScoreListSlayer )
	{
		ScoreListSlayer* pScoreList = *itrScoreListSlayer;
		Assert( pScoreList != NULL );


		BYTE szName = pScoreList->RankName.size();

		if ( szName == 0 )
			throw InvalidProtocolException("szName == 0");

		if ( szName > 20 )
			throw InvalidProtocolException("too long name length");

		oStream.write( szName );

		oStream.write( pScoreList->RankName );
		oStream.write( pScoreList->RankKillCount );
		oStream.write( pScoreList->RankDeadCount );
		oStream.write( pScoreList->RankPoint );
	}
	///////////////////////////////////Vampire Rank////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BYTE totalVampireRankNum = m_ScoreListVampire.size();
	if ( totalVampireRankNum > MAX_RANK ) throw DisconnectException("GCSendNewWarInfo :Max Vampire Rank Num write error.");

	oStream.write( totalVampireRankNum );

	std::list<ScoreListVampire*>::const_iterator itrScoreListVampire = m_ScoreListVampire.begin();
	std::list<ScoreListVampire*>::const_iterator endItrScoreListVampire = m_ScoreListVampire.end();

	for ( ; itrScoreListVampire != endItrScoreListVampire ; ++itrScoreListVampire )
	{
		ScoreListVampire* pScoreList = *itrScoreListVampire;
		Assert( pScoreList != NULL );


		BYTE szName = pScoreList->RankName.size();

		if ( szName == 0 )
			throw InvalidProtocolException("szName == 0");

		if ( szName > 20 )
			throw InvalidProtocolException("too long name length");

		oStream.write( szName );

		oStream.write( pScoreList->RankName );
		oStream.write( pScoreList->RankKillCount );
		oStream.write( pScoreList->RankDeadCount );
		oStream.write( pScoreList->RankPoint );
	}
	//////////////////////Ousters Rank///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BYTE totalOustersRankNum = m_ScoreListOusters.size();
	if ( totalOustersRankNum > MAX_RANK ) throw DisconnectException("GCSendNewWarInfo :Max Ousters Rank Num write error.");

	oStream.write( totalOustersRankNum );

	std::list<ScoreListOusters*>::const_iterator itrScoreListOusters = m_ScoreListOusters.begin();
	std::list<ScoreListOusters*>::const_iterator endItrScoreListOusters = m_ScoreListOusters.end();

	for ( ; itrScoreListOusters != endItrScoreListOusters ; ++itrScoreListOusters )
	{
		ScoreListOusters* pScoreList = *itrScoreListOusters;
		Assert( pScoreList != NULL );


		BYTE szName = pScoreList->RankName.size();

		if ( szName == 0 )
			throw InvalidProtocolException("szName == 0");

		if ( szName > 20 )
			throw InvalidProtocolException("too long name length");

		oStream.write( szName );

		oStream.write( pScoreList->RankName );
		oStream.write( pScoreList->RankKillCount );
		oStream.write( pScoreList->RankDeadCount );
		oStream.write( pScoreList->RankPoint );
	}

	oStream.write( m_PlayerKillCount );
	oStream.write( m_PlayerDeadCount );
	oStream.write( m_PlayerRacePoint );
	oStream.write( m_PlayerKillPoint );
	oStream.write( m_PlayerTotalPoint );
	__END_DEBUG
		__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCSendNewWarInfo::execute ( Player * pPlayer ) 

{
	__BEGIN_TRY
		__BEGIN_DEBUG

		GCSendNewWarInfoHandler::execute( this , pPlayer );

	__END_DEBUG
		__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
PacketSize_t GCSendNewWarInfo::getPacketSize () const 

{ 
	__BEGIN_TRY
		__BEGIN_DEBUG

		PacketSize_t size = szBYTE;

	std::list<RaceData*>::const_iterator itr = m_RaceData.begin();
	std::list<RaceData*>::const_iterator endItr = m_RaceData.end();

	for ( ; itr != endItr ; ++itr )
	{
		size += (*itr)->getPacketSize();
	}

	std::list<ScoreListSlayer*>::const_iterator itrScoreListSlayer = m_ScoreListSlayer.begin();
	std::list<ScoreListSlayer*>::const_iterator endItrScoreListSlayer = m_ScoreListSlayer.end();

	for ( ; itrScoreListSlayer != endItrScoreListSlayer ; ++itrScoreListSlayer )
	{
		size += (*itrScoreListSlayer)->getPacketSize();
	}

	std::list<ScoreListVampire*>::const_iterator itrScoreListVampire = m_ScoreListVampire.begin();
	std::list<ScoreListVampire*>::const_iterator endItrScoreListVampire = m_ScoreListVampire.end();

	for ( ; itrScoreListVampire != endItrScoreListVampire ; ++itrScoreListVampire )
	{
		size += (*itrScoreListVampire)->getPacketSize();
	}


	std::list<ScoreListOusters*>::const_iterator itrScoreListOusters = m_ScoreListOusters.begin();
	std::list<ScoreListOusters*>::const_iterator endItrScoreListOusters = m_ScoreListOusters.end();

	for ( ; itrScoreListOusters != endItrScoreListOusters ; ++itrScoreListOusters )
	{
		size += (*itrScoreListOusters)->getPacketSize();
	}

	size += szGold + szGold + szGold + szGold + szGold;
	return size;

	__END_DEBUG
		__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////////////
std::string GCSendNewWarInfo::toString () const

{
	__BEGIN_TRY

		StringStream msg;
	msg << "GCSendNewWarInfo(";

	std::list<RaceData*>::const_iterator itr = m_RaceData.begin();
	std::list<RaceData*>::const_iterator endItr = m_RaceData.end();

	for ( ; itr != endItr ; ++itr )
	{
		msg << (*itr)->toString();
	}

	msg << ")";
	return msg.toString();

	__END_CATCH
}


