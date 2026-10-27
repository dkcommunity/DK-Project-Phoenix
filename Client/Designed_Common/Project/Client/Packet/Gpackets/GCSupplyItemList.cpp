#if 0
//////////////////////////////////////////////////////////////////////////////
// Filename    : GCSupplyItemList.cpp 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////


#include "DarkAssert.h"
#include "Client_PCH.h"
#include "GCSupplyItemList.h"

#ifdef __GAME_SERVER__
	
#endif

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////

GCSupplyItemList::GCSupplyItemList() 
	
{
	__BEGIN_TRY

	m_SupplyItemList.clear();
 
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GCSupplyItemList::~GCSupplyItemList() {
	__BEGIN_TRY

	std::list<SupplyItemInfo*>::iterator itr = m_SupplyItemList.begin();
	std::list<SupplyItemInfo*>::iterator endItr = m_SupplyItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		if ( *itr != NULL )
		{
			delete *itr; 
			*itr = NULL;
		}
	}

	m_SupplyItemList.clear();
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ìž…ë ¥ìŠ¤íŠ¸ë¦¼(ë²„í¼)ìœ¼ë¡œë¶€í„° ë°ì´íƒ€ë¥¼ ì½ì–´ì„œ íŒ¨í‚·ì„ ì´ˆê¸°í™”í•œë‹¤.
//////////////////////////////////////////////////////////////////////////////
void GCSupplyItemList::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	BYTE totalNum;
	iStream.read( totalNum );
	if ( totalNum > MAX_SUPPLYITEM_LIST ) throw DisconnectException("GCSupplyItemList : ë¬¼ê±´ ê°œìˆ˜ê°€ ì œí•œì„ ì´ˆê³¼í–ˆìŠµë‹ˆë‹¤.");

	for ( int i=0; i < totalNum ; ++i )
	{
		SupplyItemInfo* pSI = new SupplyItemInfo;

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

		addSupplyItemInfo( pSI );
	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////////////
// ì¶œë ¥ìŠ¤íŠ¸ë¦¼(ë²„í¼)ìœ¼ë¡œ íŒ¨í‚·ì˜ ë°”ì´ë„ˆë¦¬ ì´ë¯¸ì§€ë¥¼ ë³´ë‚¸ë‹¤.
//////////////////////////////////////////////////////////////////////////////
void GCSupplyItemList::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	BYTE totalNum = m_SupplyItemList.size();
	BYTE currentNum = 0;
	
	if ( totalNum > MAX_SUPPLYITEM_LIST ) 
	{
		totalNum = MAX_SUPPLYITEM_LIST;
	}

	oStream.write( totalNum );
	
	std::list<SupplyItemInfo*>::const_iterator itr = m_SupplyItemList.begin();
	std::list<SupplyItemInfo*>::const_iterator endItr = m_SupplyItemList.end();

	for ( ; itr != endItr, currentNum < totalNum ; ++itr )
	{
		SupplyItemInfo* pSI = *itr;
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
		
		++currentNum;
	}

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCSupplyItemList::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	GCSupplyItemListHandler::execute( this , pPlayer );

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
PacketSize_t GCSupplyItemList::getPacketSize () const 
	
{ 
	__BEGIN_TRY
	__BEGIN_DEBUG

	PacketSize_t size = szBYTE;

	std::list<SupplyItemInfo*>::const_iterator itr = m_SupplyItemList.begin();
	std::list<SupplyItemInfo*>::const_iterator endItr = m_SupplyItemList.end();

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
std::string GCSupplyItemList::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCSupplyItemList(";

	std::list<SupplyItemInfo*>::const_iterator itr = m_SupplyItemList.begin();
	std::list<SupplyItemInfo*>::const_iterator endItr = m_SupplyItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		msg << (*itr)->toString();
	}

	msg << ")";
	return msg.toString();
		
	__END_CATCH
}
#endif

#endif
