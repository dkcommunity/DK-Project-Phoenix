//////////////////////////////////////////////////////////////////////////////
// Filename    : GCUpdateWeekItemList.cpp 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCUpdateWeekItemList.h"
#include "DarkAssert.h"

#ifdef __GAME_SERVER__
	
#endif

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////

GCUpdateWeekItemList::GCUpdateWeekItemList() 
	
{
	__BEGIN_TRY

	m_WeekItemList.clear();
 
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GCUpdateWeekItemList::~GCUpdateWeekItemList() {
	__BEGIN_TRY

	std::list<WeekItemInfo*>::iterator itr = m_WeekItemList.begin();
	std::list<WeekItemInfo*>::iterator endItr = m_WeekItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		if ( *itr != NULL )
		{
			delete *itr; 
			*itr = NULL;
		}
	}

	m_WeekItemList.clear();
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////////////
void GCUpdateWeekItemList::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	BYTE totalNum;
	iStream.read( totalNum );
	if ( totalNum > MAX_WEEKITEM_LIST ) throw DisconnectException("GCUpdateWeekItemList : 물건 개수가 제한을 초과했습니다.");

	for ( int i=0; i < totalNum ; ++i )
	{
		WeekItemInfo* pSI = new WeekItemInfo;

		iStream.read( pSI->ID );
		iStream.read( pSI->itemClass );
		iStream.read( pSI->itemType );
		iStream.read( pSI->grade );

		BYTE optionNum;
		iStream.read( optionNum );

		pSI->optionType.clear();

		for ( int j=0; j < optionNum ; ++j )
		{
			OptionType_t optionType;
			iStream.read( optionType );
			pSI->optionType.push_back( optionType );
		}
		
		iStream.read( pSI->num );
		iStream.read( pSI->timeLimit );
		iStream.read( pSI->Durability );

		addWeekItemInfo( pSI );
	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////////////
void GCUpdateWeekItemList::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	BYTE totalNum = m_WeekItemList.size();
	BYTE currentNum = 0;
	
	if ( totalNum > MAX_WEEKITEM_LIST ) 
	{
		totalNum = MAX_WEEKITEM_LIST;
	}

	oStream.write( totalNum );
	
	std::list<WeekItemInfo*>::const_iterator itr = m_WeekItemList.begin();
	std::list<WeekItemInfo*>::const_iterator endItr = m_WeekItemList.end();

	for ( ; itr != endItr, currentNum < totalNum ; ++itr )
	{
		WeekItemInfo* pSI = *itr;
		Assert( pSI != NULL );

		oStream.write( pSI->ID );
		oStream.write( pSI->itemClass );
		oStream.write( pSI->itemType );
		oStream.write( pSI->grade );
		
		BYTE optionNum = pSI->optionType.size();
		oStream.write( optionNum );

		std::list<OptionType_t>::const_iterator oitr = pSI->optionType.begin();
		std::list<OptionType_t>::const_iterator endoItr = pSI->optionType.end();

		for ( ; oitr != endoItr ; ++oitr )
		{
			oStream.write( *oitr );
		}

		oStream.write( pSI->num );
		oStream.write( pSI->timeLimit );
		oStream.write( pSI->Durability );

		++currentNum;
	}

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCUpdateWeekItemList::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	GCUpdateWeekItemListHandler::execute( this , pPlayer );

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
PacketSize_t GCUpdateWeekItemList::getPacketSize () const 
	
{ 
	__BEGIN_TRY
	__BEGIN_DEBUG

	PacketSize_t size = szBYTE;

	std::list<WeekItemInfo*>::const_iterator itr = m_WeekItemList.begin();
	std::list<WeekItemInfo*>::const_iterator endItr = m_WeekItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		size += (*itr)->getPacketSize();
	}

	return size;

	__END_DEBUG
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string GCUpdateWeekItemList::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCUpdateWeekItemList(";

	std::list<WeekItemInfo*>::const_iterator itr = m_WeekItemList.begin();
	std::list<WeekItemInfo*>::const_iterator endItr = m_WeekItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		msg << (*itr)->toString();
	}

	msg << ")";
	return msg.toString();
		
	__END_CATCH
}
#endif
#endif//__PREMIUM_GIVE_ITEM_UI
