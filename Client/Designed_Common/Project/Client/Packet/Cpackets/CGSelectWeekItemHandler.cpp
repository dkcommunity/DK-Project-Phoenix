//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectWeekItemHandler.cpp
// Written By  : rappi76
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGSelectWeekItem.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "PlayerCreature.h"
	#include "Zone.h"
	#include "PacketUtil.h"
	#include "ItemUtil.h"
	#include "Inventory.h"
	#include "StringPool.h"

	#include "ItemFactoryManager.h"
	#include "WeekItemListManager.h"

	#include "Gpackets/GCCreateItem.h"
	#include "Gpackets/GCUpdateWeekItemList.h"
	#include "Gpackets/GCSystemMessage.h"
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
void CGSelectWeekItemHandler::execute(CGSelectWeekItem* pPacket , Player* pPlayer)
	 
{
	__BEGIN_TRY //__BEGIN_DEBUG_EX

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
	
	WeekItemListManager *pWeekItemListManager;
	
	pWeekItemListManager = pPC->getWeekItemListManager();
	if ( pWeekItemListManager == NULL )
	{
		return;
	}
	
	Item *pItem = pWeekItemListManager->CreateWeekItemByID(pPacket->getSelectID());
	if ( pItem == NULL )
	{
		return;
	}
	
	///////////////////////////////////////////////////////////
	// 사용가능한 종족인지 체크
	
	
	
	///////////////////////////////////////////////////////////
	
	// 인벤토리의 빈 곳을 찾는다.
	_TPOINT p;
	if (pInventory->getEmptySlot(pItem, p))
	{
		(pZone->getObjectRegistry()).registerObject(pItem);
		
		pWeekItemListManager->RemoveWeekItemElement(pPacket->getSelectID());
		
		// 인벤토리에 추가한다.
		pInventory->addItem(p.x, p.y, pItem);
		pItem->create(pCreature->getName(), STORAGE_INVENTORY, 0, p.x, p.y);
		
		if(pItem->isTimeLimitItem())
		{
			pPC->addTimeLimitItem( pItem, pItem->getHour() );   
			pPC->sendTimeLimitItemInfo();
		}

		// ItemTrace 에 Log 를 남긴다
		if ( pItem != NULL )
		{
			remainTraceLog( pItem, "WeekItem", pCreature->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
			remainTraceLogNew( pItem, pCreature->getName(), ITL_GET, ITLD_EVENTNPC, pZone->getZoneID() );
		}

		// 인벤토리에 아이템 생성 패킷을 보내준다.
		GCCreateItem gcCreateItem;
		makeGCCreateItem( &gcCreateItem, pItem, p.x, p.y );
		pGamePlayer->sendPacket(&gcCreateItem);
	}
	else
	{
		// 인벤토리에 빈자리가 없음
					
		StringStream buf;

		buf << g_pStringPool->getString( STRID_NOT_ENOUGH_INVENTORY_SPACE );
		
		GCSystemMessage	gcSystemMessage;
		gcSystemMessage.setMessage(buf.toString());
		
		pGamePlayer->sendPacket(&gcSystemMessage);
	}
	
	GCUpdateWeekItemList gcWeekItemList;
	makeGCUpdateWeekItemList(&gcWeekItemList, pPC);
	pGamePlayer->sendPacket( &gcWeekItemList );
	
#endif /* __GAME_SERVER__ */

	__END_CATCH //__END_DEBUG_EX
}


#endif //__PREMIUM_GIVE_ITEM_UI
