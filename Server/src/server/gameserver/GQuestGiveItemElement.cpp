#include "GQuestGiveItemElement.h"
#include "PlayerCreature.h"
#include "GQuestInventory.h"
#include "Player.h"
#include "Treasure.h"
#include "ItemUtil.h"
#include "ItemFactoryManager.h"
#include "Inventory.h"
#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCSystemMessage.h"
#include "PacketUtil.h"
#include "Zone.h"
#include "StringPool.h"

#include "EffectPrecedence.h"

GQuestElement::ResultType GQuestGiveItemElement::checkCondition( PlayerCreature* pPC ) const
{
//	cout << "GQuestGiveItemElement : " << (int)m_ItemClass << ", " << (int)m_ItemType << endl;
	Item* pItem = g_pItemFactoryManager->createItem( m_ItemClass, m_ItemType, m_Option );
	if ( pItem == NULL ) return FAIL;
	
	pItem->setNum(m_Num);
	Inventory* pInventory = pPC->getInventory();
	TPOINT pt;
	pPC->getZone()->registerObject( pItem );
	setItemGender( pItem, (pPC->getSex()==FEMALE)?GENDER_FEMALE:GENDER_MALE );

	if ( !pInventory->addItem( pItem, pt ) )
	{
		pt = pPC->getZone()->addItem( pItem, pPC->getX(), pPC->getY() );
		if ( pt.x == -1 )
		{
			SAFE_DELETE(pItem);
		}
		else
		{
			EffectPrecedence* pEffectPrecedence = new EffectPrecedence(pItem);
			pEffectPrecedence->setDeadline(999999);
			pEffectPrecedence->setHostName( pPC->getName() );
			pEffectPrecedence->setHostPartyID( pPC->getPartyID() );

			EffectManager& rEffectManager = pItem->getEffectManager();
			rEffectManager.deleteEffect(Effect::EFFECT_CLASS_PRECEDENCE);
			rEffectManager.addEffect(pEffectPrecedence);
			pItem->setFlag(Effect::EFFECT_CLASS_PRECEDENCE);

			pItem->create("", STORAGE_ZONE, pPC->getZone()->getZoneID(), pt.x, pt.y );
		}
	}
	else
	{
		pItem->create(pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

		GCCreateItem gcCreateItem;
		makeGCCreateItem( &gcCreateItem, pItem, pt.x, pt.y );
		pPC->getPlayer()->sendPacket( &gcCreateItem );

		if ( m_LimitedTime > 0 )
		{
			pPC->addTimeLimitItem( pItem, m_LimitedTime );
			pPC->sendTimeLimitItemInfo();
		}

		// ItemTraceLog 를 남긴다
		if ( pItem->isTraceItem() )
		{
			remainTraceLog( pItem, "GQuest", pPC->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
			remainTraceLogNew( pItem, pPC->getName(), ITL_GET, ITLD_EVENTNPC, pPC->getZone()->getZoneID() );
		}
	}
	
	GCSystemMessage gcSM;
	gcSM.setMessage(g_pStringPool->c_str(STRID_GET_ITEM) ); // 20070814
	pPC->getPlayer()->sendPacket( &gcSM );

	return OK;
}

GQuestGiveItemElement* GQuestGiveItemElement::makeElement(XMLTree* pTree)
{
	GQuestGiveItemElement* pRet = new GQuestGiveItemElement;

	string iClass;
	Assert( pTree->GetAttribute("class", iClass ) );

	pRet->m_ItemClass = TreasureItemClass::getItemClassFromString( iClass );

	DWORD itemType;
	Assert( pTree->GetAttribute("type", itemType ) );

	pRet->m_ItemType = itemType;

	string option;
	if ( pTree->GetAttribute("option", option) )
	{
		makeOptionList( option, pRet->m_Option );
	}

	if ( !pTree->GetAttribute("num", (int&)pRet->m_Num) ) pRet->m_Num=1;
	if ( !pTree->GetAttribute("limitedtime", (DWORD&)pRet->m_LimitedTime) ) 
	{
		pRet->m_LimitedTime = 0;
	}

	return pRet;
}

GQuestGiveItemElement g_GiveItemElement;
