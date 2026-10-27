#if 0
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectSupplyItemHandler.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGSelectSupplyItem.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "PlayerCreature.h"
	#include "Zone.h"
	#include "PacketUtil.h"
	#include "Inventory.h"

	#include "ItemFactoryManager.h"

	#include "Gpackets/GCCreateItem.h"
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGSelectSupplyItemHandler::execute(CGSelectSupplyItem* pPacket , Player* pPlayer)
	 
{
	__BEGIN_TRY// __BEGIN_DEBUG_EX

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
	
	Item::ItemClass itemClass;
	ItemType_t itemType;
	
	if ( pPacket->getSelectID() == 1 )
	{
		itemClass = (Item::ItemClass)97;
		itemType = 0;
		
		std::list<OptionType_t> nullList;
		Item* pItem = CREATE_ITEM(itemClass, itemType, nullList);

		(pZone->getObjectRegistry()).registerObject(pItem);
		
		// ì¸ë²¤í† ë¦¬ì˜ ë¹ˆ ê³³ì„ ì°¾ëŠ”ë‹¤.
		_TPOINT p;
		if (pInventory->getEmptySlot(pItem, p))
		{
			// ì¸ë²¤í† ë¦¬ì— ì¶”ê°€í•œë‹¤.
			pInventory->addItem(p.x, p.y, pItem);
			pItem->create(pCreature->getName(), STORAGE_INVENTORY, 0, p.x, p.y);

//			// ItemTrace ì— Log ë¥¼ ë‚¨ê¸´ë‹¤
//			if ( pItem != NULL && pItem->isTraceItem() )
//			{
//				remainTraceLog( pItem, "MenegrothDoungeon", pCreature->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
//				remainTraceLogNew( pItem, pCreature->getName(), ITL_GET, ITLD_EVENTNPC, m_pZone->getZoneID() );
//			}

			// ì¸ë²¤í† ë¦¬ì— ì•„ì´í…œ ìƒì„± íŒ¨í‚·ì„ ë³´ë‚´ì¤€ë‹¤.
			GCCreateItem gcCreateItem;
			makeGCCreateItem( &gcCreateItem, pItem, p.x, p.y );
			pCreature->getPlayer()->sendPacket(&gcCreateItem);
		}

				
				
	}
	else if ( pPacket->getSelectID() == 2 )
	{
		
	}
	else
	{
		
	}
	
#endif /* __GAME_SERVER__ */

	__END_CATCH// __END_DEBUG_EX
}



#endif
