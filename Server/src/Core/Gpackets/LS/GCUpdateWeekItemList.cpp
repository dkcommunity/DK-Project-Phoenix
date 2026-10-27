//////////////////////////////////////////////////////////////////////////////
// Filename    : GCUpdateWeekItemList.cpp 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "GCUpdateWeekItemList.h"
#include "Assert.h"

#ifdef __GAME_SERVER__
	
#endif

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////

GCUpdateWeekItemList::GCUpdateWeekItemList() 
	throw ()
{
	__BEGIN_TRY

	m_WeekItemList.clear();
 
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GCUpdateWeekItemList::~GCUpdateWeekItemList() 
	throw ()
{
	__BEGIN_TRY

	list<WeekItemInfo*>::iterator itr = m_WeekItemList.begin();
	list<WeekItemInfo*>::iterator endItr = m_WeekItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		if ( *itr != NULL ) SAFE_DELETE(*itr);
	}

	m_WeekItemList.clear();
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////////////
void GCUpdateWeekItemList::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
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
		iStream.read( pSI->durability );

		addWeekItemInfo( pSI );
	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////////////
void GCUpdateWeekItemList::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
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
	
	list<WeekItemInfo*>::const_iterator itr = m_WeekItemList.begin();
	list<WeekItemInfo*>::const_iterator endItr = m_WeekItemList.end();

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

		list<OptionType_t>::const_iterator oitr = pSI->optionType.begin();
		list<OptionType_t>::const_iterator endoItr = pSI->optionType.end();

		for ( ; oitr != endoItr ; ++oitr )
		{
			oStream.write( *oitr );
		}

		oStream.write( pSI->num );
		oStream.write( pSI->timeLimit );
		oStream.write( pSI->durability );
		
		++currentNum;
	}

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCUpdateWeekItemList::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
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
	throw ()
{ 
	__BEGIN_TRY
	__BEGIN_DEBUG

	PacketSize_t size = szBYTE;

	list<WeekItemInfo*>::const_iterator itr = m_WeekItemList.begin();
	list<WeekItemInfo*>::const_iterator endItr = m_WeekItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		size += (*itr)->getPacketSize();
	}

	return size;

	__END_DEBUG
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////////////
string GCUpdateWeekItemList::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCUpdateWeekItemList(";

	list<WeekItemInfo*>::const_iterator itr = m_WeekItemList.begin();
	list<WeekItemInfo*>::const_iterator endItr = m_WeekItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		msg << (*itr)->toString();
	}

	msg << ")";
	return msg.toString();
		
	__END_CATCH
}

