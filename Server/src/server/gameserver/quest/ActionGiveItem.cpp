////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveItem.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ActionGiveItem.h"
#include "Creature.h"
#include "PlayerCreature.h"
#include "GamePlayer.h"
#include "ItemFactoryManager.h"
#include "Inventory.h"
#include "ItemUtil.h"
#include "Zone.h"
#include "PacketUtil.h"

#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCNPCResponse.h"

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionGiveItem::read (PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try 
	{
		m_ItemClass = (Item::ItemClass)propertyBuffer.getPropertyInt("ItemClass");
		m_ItemType = (ItemType_t)propertyBuffer.getPropertyInt("ItemType");
		
		if (propertyBuffer.hasKey("RequiredItemClass"))
		{
			m_RequiredItemClass = (Item::ItemClass)propertyBuffer.getPropertyInt("RequiredItemClass");
			m_RequiredItemType = (ItemType_t)propertyBuffer.getPropertyInt("RequiredItemType");
			m_RequiredItemCount = propertyBuffer.getPropertyInt("RequiredItemCount");
			
			m_bRequiredItem = true;
		}
		else
		{
			m_bRequiredItem = false;
		}
		
		if (propertyBuffer.hasKey("ItemLimitTime"))
		{
			m_ItemLimitTime = propertyBuffer.getPropertyInt("ItemLimitTime");
		}
		else
		{
			m_ItemLimitTime = 0;
		}
	} 
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}
	
    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionGiveItem::execute (Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	Assert(pCreature2 != NULL);
	Assert(pCreature2->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert( pPC != NULL );

	Player* pPlayer = pCreature2->getPlayer();
	Assert( pPlayer != NULL );

	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );
	
	if (m_bRequiredItem )
	{
		if ( !pInventory->hasEnoughNumItem(m_RequiredItemClass, m_RequiredItemType, m_RequiredItemCount) )
		{
			// 아이템 없음
			GCNPCResponse response;
			response.setCode(NPC_RESPONSE_NOT_ENOUGH_ITEM);
			pPlayer->sendPacket(&response);
			
			return ;
		}
	}
	
	list<OptionType_t> optionTypeList;

	Item* pItem = g_pItemFactoryManager->createItem( m_ItemClass, m_ItemType, optionTypeList );
	Assert( pItem != NULL );
	
	_TPOINT pt;
	
	if (!pInventory->getEmptySlot(pItem, pt))
	{
//		// ConditionHasInvenSpace 컨디션과 반드시 함께 써야만 한다.
//		throw Error("ActionGiveItem: 제발 ConditionHasInvenSpace랑 같이 쓰자. 인벤토리에 자리없다.");
		
		GCNPCResponse response;
		response.setCode(NPC_RESPONSE_NO_EMPTY_SLOT);
		pPlayer->sendPacket(&response);
		
		return;		
	}

	pInventory->decreaseNumItem(m_RequiredItemClass, m_RequiredItemType, m_RequiredItemCount, pPlayer);
	
	CoordInven_t X = pt.x;
	CoordInven_t Y = pt.y;

	pPC->getZone()->getObjectRegistry().registerObject( pItem );
	pInventory->addItem( X, Y, pItem );
	pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, X, Y );
	
	if (m_ItemLimitTime > 0)
	{
		pPC->addTimeLimitItem( pItem, m_ItemLimitTime );
		pPC->sendTimeLimitItemInfo();
	}
	
	if ( pItem != NULL && pItem->isTraceItem() )
	{
		remainTraceLog( pItem, "ActionGiveItem", pCreature2->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
		remainTraceLogNew( pItem, pCreature2->getName(), ITL_GET, ITLD_EVENTNPC , pCreature2->getZone()->getZoneID() );
	}
				
	GCCreateItem gcCreateItem;
	makeGCCreateItem( &gcCreateItem, pItem, X, Y );
	pPlayer->sendPacket(&gcCreateItem);

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionGiveItem::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionGiveItem("
	    << ")";

	return msg.toString();

	__END_CATCH
}
