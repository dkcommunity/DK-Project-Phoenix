//////////////////////////////////////////////////////////////////////////////
// Filename    : CGTakeOutGoodHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGTakeOutGood.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Assert.h"
	#include "PlayerCreature.h"
	#include "GoodsInventory.h"
	#include "Inventory.h"
	#include "PacketUtil.h"
	#include "ItemUtil.h"
	#include "Gpackets/GCTakeOutFail.h"
	#include "Gpackets/GCTakeOutOK.h"
	#include "Gpackets/GCCreateItem.h"
//20070911
	#include "CreatureUtil.h"

	#include "SystemAvailabilitiesManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGTakeOutGoodHandler::execute (CGTakeOutGood* pPacket , Player* pPlayer)
	throw (ProtocolException, Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert( pGamePlayer != NULL );

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
	Assert( pPC != NULL );
	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );

	SYSTEM_ASSERT( SYSTEM_MARKET );

	GoodsInventory* pGoodsInventory = pPC->getGoodsInventory();
	Assert( pGoodsInventory != NULL );

	TPOINT tp;

	Item* pItem = pGoodsInventory->getItem( pPacket->getObjectID() );
	if ( pItem != NULL )
	{
//		if ( pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY && pInventory->findItem( Item::ITEM_CLASS_SUB_INVENTORY ) != NULL )
		// 20070910 ��Ƽ�� ���� 3������ ��밡����� �ٲ۴�.
//		Creature* pCreature = dynamic_cast<Creature*>(pPC);
		if( pItem->getItemClass() == Item::ITEM_CLASS_SUB_INVENTORY && (getSubInventoryNum(pPC) >= 5) ) // 20080912 5���� Ȯ��
		{
		}
		else if ( pInventory->addItem( pItem, tp ) )
		{
			pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y );
			pGoodsInventory->popItem( pPacket->getObjectID() );
			pItem->whenPCTake( pPC );

			GCCreateItem gcCreateItem;
			makeGCCreateItem( &gcCreateItem, pItem, tp.x ,tp.y );
			pPlayer->sendPacket( &gcCreateItem );

			// 2026-05-13 ASSERT FIX: Hour > 0 guard.
			// TimeLimitItemManager::addTimeLimitItem Assert(time != 0).
			// GoodsInfo data bug'i: TimeLimit=1 fakat Hour=0 -> addTimeLimit
			// time=0 ile cagrilir -> assertion fail (log spam, non-fatal).
			// Defansif: TimeLimit ON ama Hour 0 ise tracking'i atla.
			if ( pItem->isTimeLimitItem() && pItem->getHour() > 0 )
			{
				unsigned long timeLimit = pItem->getHour();
				timeLimit *= 3600;

				pPC->addTimeLimitItem( pItem, timeLimit );
				pPC->sendTimeLimitItemInfo();
			}
			else if ( pItem->isTimeLimitItem() )
			{
				// Data inconsistency log — GoodsInfo'da TimeLimit=Y ama Hour=0.
				filelog("buyItemBug.txt", "TimeLimit flag ON ama Hour=0; GoodsInfo data check gerek. ItemClass=%u ItemType=%u OwnerID=%s",
					(uint)pItem->getItemClass(), (uint)pItem->getItemType(), pPC->getName().c_str());
			}

			remainTraceLog( pItem, "GoodsShop", pPC->getName(), ITEM_LOG_CREATE, DETAIL_MALLBUY );

			GCTakeOutOK gcTakeOutOK;
			gcTakeOutOK.setObjectID( pPacket->getObjectID() );
			pPlayer->sendPacket( &gcTakeOutOK );

			return;
		}
	}

	GCTakeOutFail gcTakeOutFail;
	gcTakeOutFail.setObjectID( pPacket->getObjectID() );

	pPlayer->sendPacket(&gcTakeOutFail);

#endif	// __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
