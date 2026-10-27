#if 0
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectAllSupplyItemHandler.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGSelectAllSupplyItem.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "PlayerCreature.h"
	#include "Zone.h"
	#include "PacketUtil.h"
	#include "ItemUtil.h"
	#include "Inventory.h"
	#include "StringPool.h"

	#include "ItemFactoryManager.h"
	#include "SupplyItemListManager.h"

	#include "Gpackets/GCCreateItem.h"
	#include "Gpackets/GCUpdateSupplyItemList.h"
	#include "Gpackets/GCSystemMessage.h"
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSelectAllSupplyItemHandler::execute(CGSelectAllSupplyItem* pPacket , Player* pPlayer)
	 
{
	__BEGIN_TRY//__BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);
	
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);
	
	Creature* pCreature = pGamePlayer->getCreature();
	if ( pCreature == NULL )
	{
		return;
	}	
	
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	if ( pPC == NULL )
	{
		return;
	}
		
	Zone *pZone = pPC->getZone();
	if ( pZone == NULL )
	{
		return;
	}
	
	Inventory* pInventory = pPC->getInventory();
	if ( pInventory == NULL )
	{
		return;
	}
	
	SupplyItemListManager *pSupplyItemListManager;
	
	pSupplyItemListManager = pPC->getSupplyItemListManager();
	if ( pSupplyItemListManager == NULL )
	{
		return;
	}
	
	while( true )
	{
		Item *pItem = pSupplyItemListManager->PickupFrontItem();
		if ( pItem == NULL )
		{
			break;
		}
	
		(pZone->getObjectRegistry()).registerObject(pItem);
			
		// ì¸ë²¤í† ë¦¬ì˜ ë¹ˆ ê³³ì„ ì°¾ëŠ”ë‹¤.
		_TPOINT p;
		if (pInventory->getEmptySlot(pItem, p))
		{
			// ì¸ë²¤í† ë¦¬ì— ì¶”ê°€í•œë‹¤.
			pInventory->addItem(p.x, p.y, pItem);
			pItem->create(pCreature->getName(), STORAGE_INVENTORY, 0, p.x, p.y);
	
			// ItemTrace ì— Log ë¥¼ ë‚¨ê¸´ë‹¤
			if ( pItem != NULL )
			{
				remainTraceLog( pItem, "SupplyItem", pCreature->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
				remainTraceLogNew( pItem, pCreature->getName(), ITL_GET, ITLD_EVENTNPC, pZone->getZoneID() );
			}
	
			// ì¸ë²¤í† ë¦¬ì— ì•„ì´í…œ ìƒì„± íŒ¨í‚·ì„ ë³´ë‚´ì¤€ë‹¤.
			GCCreateItem gcCreateItem;
			makeGCCreateItem( &gcCreateItem, pItem, p.x, p.y );
			pGamePlayer->sendPacket(&gcCreateItem);
		}
		else
		{
			// ì¸ë²¤í† ë¦¬ì— ë¹ˆìžë¦¬ê°€ ì—†ìŒ
			
			StringStream buf;

			buf << g_pStringPool->getString( STRID_NOT_ENOUGH_INVENTORY_SPACE );
			
			GCSystemMessage	gcSystemMessage;
			gcSystemMessage.setMessage(buf.toString());
			
			pGamePlayer->sendPacket(&gcSystemMessage);	
	
			break;
		}
	}
	
	GCUpdateSupplyItemList gcUpdateSupplyItemList;
	makeGCUpdateSupplyItemList(&gcUpdateSupplyItemList, pPC);
	pGamePlayer->sendPacket( &gcUpdateSupplyItemList );
	
#endif /* __GAME_SERVER__ */

	__END_CATCH//__END_DEBUG_EX
}



#endif
