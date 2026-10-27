//////////////////////////////////////////////////////////////////////////////
// Filename    : GCTMallGoodsList.cpp 
// Written By  : ±èŒº¹Î
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCTMallGoodsList.h"
#include "DarkAssert.h"

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
#include "Client.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////

GCTMallGoodsList::GCTMallGoodsList() 
	
{
	__BEGIN_TRY

	m_GoodsList.clear();
 
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GCTMallGoodsList::~GCTMallGoodsList() {
	__BEGIN_TRY

	std::list<TmallGoodsInfo*>::iterator itr = m_GoodsList.begin();
	std::list<TmallGoodsInfo*>::iterator endItr = m_GoodsList.end();

	for ( ; itr != endItr ; ++itr )
	{
		if ( *itr != NULL ) SAFE_DELETE(*itr);
	}

	m_GoodsList.clear();
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ÀÔ·ÂœºÆ®ž²(¹öÆÛ)Àž·ÎºÎÅÍ µ¥ÀÌÅžžŠ ÀÐŸîŒ­ ÆÐÅ¶À» ÃÊ±âÈ­ÇÑŽÙ.
//////////////////////////////////////////////////////////////////////////////
void GCTMallGoodsList::read ( SocketInputStream & iStream ) 
	 
{
	__BEGIN_TRY

	BYTE totalNum;
	iStream.read( totalNum );
	if ( totalNum > MAX_TMALL_GOODS_LIST ) throw DisconnectException("GCTMallGoodsList : list size exceeds maximum allowed");

	for ( int i=0; i < totalNum ; ++i )
	{
		TmallGoodsInfo* pGI = new TmallGoodsInfo;

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

		//iStream.read( pGI->Race );
		//iStream.read( pGI->Kind );
		iStream.read( pGI->Cash );
		iStream.read( pGI->Point );

		addGoodsInfo( pGI );
	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////////////
// Ãâ·ÂœºÆ®ž²(¹öÆÛ)Àž·Î ÆÐÅ¶ÀÇ ¹ÙÀÌ³Êž® ÀÌ¹ÌÁöžŠ ºž³œŽÙ.
//////////////////////////////////////////////////////////////////////////////
void GCTMallGoodsList::write ( SocketOutputStream & oStream ) const 
     
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	BYTE totalNum = m_GoodsList.size();
	if ( totalNum > MAX_TMALL_GOODS_LIST ) throw DisconnectException("GCTMallGoodsList : title count is corrupted/invalid");

	oStream.write( totalNum );
	
	std::list<TmallGoodsInfo*>::const_iterator itr = m_GoodsList.begin();
	std::list<TmallGoodsInfo*>::const_iterator endItr = m_GoodsList.end();

	for ( ; itr != endItr ; ++itr )
	{
		TmallGoodsInfo* pGI = *itr;
		Assert( pGI != NULL );

		oStream.write( pGI->objectID );
		oStream.write( pGI->itemClass );
		oStream.write( pGI->itemType );
		oStream.write( pGI->grade );
		
		BYTE optionNum = pGI->optionType.size();
		oStream.write( optionNum );

		std::list<OptionType_t>::const_iterator oitr = pGI->optionType.begin();
		std::list<OptionType_t>::const_iterator endoItr = pGI->optionType.end();

		for ( ; oitr != endoItr ; ++oitr )
		{
			oStream.write( *oitr );
		}

		oStream.write( pGI->num );
		oStream.write( pGI->timeLimit );

		//oStream.write( pGI->Race );
		//oStream.write( pGI->Kind );
		oStream.write( pGI->Cash );
		oStream.write( pGI->Point );
	}

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCTMallGoodsList::execute ( Player * pPlayer ) 
	 
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	GCTMallGoodsListHandler::execute( this , pPlayer );

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
PacketSize_t GCTMallGoodsList::getPacketSize () const 
	
{ 
	__BEGIN_TRY
	__BEGIN_DEBUG

	PacketSize_t size = szBYTE;

	std::list<TmallGoodsInfo*>::const_iterator itr = m_GoodsList.begin();
	std::list<TmallGoodsInfo*>::const_iterator endItr = m_GoodsList.end();

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
std::string GCTMallGoodsList::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCTMallGoodsList(";

	std::list<TmallGoodsInfo*>::const_iterator itr = m_GoodsList.begin();
	std::list<TmallGoodsInfo*>::const_iterator endItr = m_GoodsList.end();

	for ( ; itr != endItr ; ++itr )
	{
		msg << (*itr)->toString();
	}

	msg << ")";
	return msg.toString();
		
	__END_CATCH
}

