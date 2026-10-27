#include "GQuestLoseQuestItemElement.h"
#include "PlayerCreature.h"
#include "GQuestInventory.h"
#include "Player.h"
#include "Gpackets/GCSystemMessage.h"
#include "StringPool.h"

GQuestElement::ResultType GQuestLoseQuestItemElement::checkCondition( PlayerCreature* pPC ) const
{
	GQuestInventory& inventory = pPC->getGQuestManager()->getGQuestInventory();

	list<ItemType_t>::iterator itr = find( inventory.getItems().begin(), inventory.getItems().end(), m_ItemType );
	if ( itr == inventory.getItems().end() ) return FAIL;

	inventory.getItems().erase(itr);
	pPC->getPlayer()->sendPacket(inventory.getInventoryPacket());

	GCSystemMessage gcSM;
	gcSM.setMessage( g_pStringPool->c_str(STRID_LOST_QUEST_ITEM) ); // 20070814
	pPC->getPlayer()->sendPacket( &gcSM );

	return OK;
}

GQuestLoseQuestItemElement* GQuestLoseQuestItemElement::makeElement(XMLTree* pTree)
{
	GQuestLoseQuestItemElement* pRet = new GQuestLoseQuestItemElement;

	DWORD itemType;
	if ( pTree->GetAttribute("id", itemType ) ) pRet->m_ItemType = itemType;

	return pRet;
}

GQuestLoseQuestItemElement g_LoseQuestItemElement;
