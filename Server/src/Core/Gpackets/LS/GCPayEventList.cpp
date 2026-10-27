#include "GCPayEventList.h"
#include "Assert.h"

#ifdef __GAME_SERVER__
	#include "Item.h"
	#include "ItemInfoManager.h"
	#include "Inventory.h"
	#include "AR.h"
	#include "SR.h"
	#include "SG.h"
	#include "SMG.h"
	#include "Belt.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////

GCPayEventList::GCPayEventList() 
	throw ()
{
	__BEGIN_TRY

	m_ItemList.clear();
 
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GCPayEventList::~GCPayEventList() 
	throw ()
{
	__BEGIN_TRY

	list<PayEventInfo*>::iterator itr = m_ItemList.begin();
	list<PayEventInfo*>::iterator endItr = m_ItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		if ( *itr != NULL ) SAFE_DELETE(*itr);
	}

	m_ItemList.clear();
	
	__END_CATCH
}

void GCPayEventList::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	BYTE totalNum;
	iStream.read( totalNum );
	if ( totalNum > MAX_PAY_EVENT_LIST ) throw DisconnectException("GCPayEventList : read error.");

	for ( int i=0; i < totalNum ; ++i )
	{
		PayEventInfo* pGI = new PayEventInfo;

		iStream.read( pGI->objectID );
		iStream.read( pGI->itemClass );
		iStream.read( pGI->itemType );
		iStream.read( pGI->grade );

		BYTE optionNum;
		iStream.read( optionNum );

		pGI->optionType.clear();

		for ( int j=0; j < optionNum ; ++j )
		{
			OptionType_t optionType;
			iStream.read( optionType );
			pGI->optionType.push_back( optionType );
		}
		
		iStream.read( pGI->num );
		iStream.read( pGI->timeLimit );

		iStream.read( pGI->ImageID );

		addPayEventInfo( pGI );
	}

	iStream.read( m_Count20 );
	iStream.read( m_Count50 );
	iStream.read( m_Count100 );
	iStream.read( m_Count200 );
	iStream.read( m_Count500 );
	iStream.read( m_Count1000 );
	iStream.read( m_Count2000 );
	iStream.read( m_Count5000 );

	iStream.read( m_Status20 );
	iStream.read( m_Status50 );
	iStream.read( m_Status100 );
	iStream.read( m_Status200 );
	iStream.read( m_Status500 );
	iStream.read( m_Status1000 );
	iStream.read( m_Status2000 );
	iStream.read( m_Status5000 );

	__END_CATCH
}

void GCPayEventList::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	BYTE totalNum = m_ItemList.size();
	if ( totalNum > MAX_PAY_EVENT_LIST ) throw DisconnectException("GCPayEventList :write error.");

	oStream.write( totalNum );
	
	list<PayEventInfo*>::const_iterator itr = m_ItemList.begin();
	list<PayEventInfo*>::const_iterator endItr = m_ItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		PayEventInfo* pGI = *itr;
		Assert( pGI != NULL );

		oStream.write( pGI->objectID );
		oStream.write( pGI->itemClass );
		oStream.write( pGI->itemType );
		oStream.write( pGI->grade );
		
		BYTE optionNum = pGI->optionType.size();
		oStream.write( optionNum );

		list<OptionType_t>::const_iterator oitr = pGI->optionType.begin();
		list<OptionType_t>::const_iterator endoItr = pGI->optionType.end();

		for ( ; oitr != endoItr ; ++oitr )
		{
			oStream.write( *oitr );
		}

		oStream.write( pGI->num );
		oStream.write( pGI->timeLimit );

		oStream.write( pGI->ImageID );
	}

	oStream.write( m_Count20 );
	oStream.write( m_Count50 );
	oStream.write( m_Count100 );
	oStream.write( m_Count200 );
	oStream.write( m_Count500 );
	oStream.write( m_Count1000 );
	oStream.write( m_Count2000 );
	oStream.write( m_Count5000 );

	oStream.write( m_Status20 );
	oStream.write( m_Status50 );
	oStream.write( m_Status100 );
	oStream.write( m_Status200 );
	oStream.write( m_Status500 );
	oStream.write( m_Status1000 );
	oStream.write( m_Status2000 );
	oStream.write( m_Status5000 );

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCPayEventList::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	GCPayEventListHandler::execute( this , pPlayer );

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
PacketSize_t GCPayEventList::getPacketSize () const 
	throw ()
{ 
	__BEGIN_TRY
	__BEGIN_DEBUG

	PacketSize_t size = szBYTE;

	list<PayEventInfo*>::const_iterator itr = m_ItemList.begin();
	list<PayEventInfo*>::const_iterator endItr = m_ItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		size += (*itr)->getPacketSize();
	}
	size += szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID +szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID;
	return size;

	__END_DEBUG
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////////////
string GCPayEventList::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCPayEventList(";

	list<PayEventInfo*>::const_iterator itr = m_ItemList.begin();
	list<PayEventInfo*>::const_iterator endItr = m_ItemList.end();

	for ( ; itr != endItr ; ++itr )
	{
		msg << (*itr)->toString();
	}

	msg << ")";
	return msg.toString();
		
	__END_CATCH
}

