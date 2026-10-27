//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddNewItemToZoneHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAddNewItemToZone.h"
#include "ClientDef.h"
#include "MItem.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddNewItemToZoneHandler::execute ( GCAddNewItemToZone * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__
	#ifdef __DEBUG_OUTPUT__
		
#if __CONTENTS(__PCITEMINFO)
		PCItemInfo* pPCIteminfo = pPacket->GetPCItemInfo();
		DEBUG_ADD_FORMAT("[Add NewItem] class=%d, type=%d, opt=%d, id=%d, xy=(%d,%d)",
										(int)pPCIteminfo->getItemClass(), 
										(int)pPCIteminfo->getItemType(), 
										(int)pPCIteminfo->getOptionTypeSize(), 
										(int)pPCIteminfo->getObjectID(), 
										(int)pPacket->getX(), 
										(int)pPacket->getY());

#else //__PCITEMINFO
		DEBUG_ADD_FORMAT("[Add NewItem] class=%d, type=%d, opt=%d, id=%d, xy=(%d,%d)",
										(int)pPacket->getItemClass(), 
										(int)pPacket->getItemType(), 
										(int)pPacket->getOptionTypeSize(), 
										(int)pPacket->getObjectID(), 
										(int)pPacket->getX(), 
										(int)pPacket->getY());
#endif //__PCITEMINFO
	#endif
	
	// dropping = FALSE
	AddItemToZone( (GCAddItemToZone*) pPacket, FALSE );	
	
#endif

	__END_CATCH
}
