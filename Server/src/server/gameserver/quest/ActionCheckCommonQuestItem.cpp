////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionCheckCommonQuestItem.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionCheckCommonQuestItem.h"
#include "Item.h"
#include "Slayer.h"
#include "Vampire.h"
#include "NPC.h"
#include "Player.h"
#include "Inventory.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "ItemInfoManager.h"
#include "OptionInfo.h"
#include "Zone.h"
#include "VariableManager.h"
#include "FlagSet.h"
#include "DB.h"

#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"

#include <list>

////////////////////////////////////////////////////////////////////////////////
// ActionCheckCommonQuestItem
////////////////////////////////////////////////////////////////////////////////
ActionCheckCommonQuestItem::ActionCheckCommonQuestItem()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionCheckCommonQuestItem::~ActionCheckCommonQuestItem()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionCheckCommonQuestItem::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
		// read script id
		m_QuestItemNum = propertyBuffer.getPropertyInt("QuestItem");
		m_EnoughGetItemScriptID = (ScriptID_t)propertyBuffer.getPropertyInt("EnoughGetItemScriptID");
		m_NotEnoughGetItemScriptID = (ScriptID_t)propertyBuffer.getPropertyInt("NotEnoughGetItemScriptID");
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
void ActionCheckCommonQuestItem::execute(Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);
	Assert(pCreature1->isNPC());
	Assert(pCreature2->isPC());

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert(pPC != NULL);

	Player* pPlayer = pPC->getPlayer();
	Assert(pPlayer != NULL);

	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );

	Zone* pZone = pPC->getZone();
	Assert( pZone != NULL );

	bool			bSlayer = pPC->isSlayer();
	bool			bVampire = pPC->isVampire();
	bool			bOusters = pPC->isOusters();

	int				 itemNum = 0;

	if( bSlayer )
	{
		itemNum = pPC->getItemClassTypeNum( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 0 );
	}
	else if( bVampire )
	{
		itemNum = pPC->getItemClassTypeNum( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 1 );
	}
	else if( bOusters )
	{
		itemNum = pPC->getItemClassTypeNum( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 2 );
	}
	else
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &response );
		return;
	}

	// 선택한 퀘스트 아템의 갯수가 충분하지 않을때 처리
	if( itemNum < m_QuestItemNum )
	{
		GCNPCAsk gcNPCAsk;
		gcNPCAsk.setObjectID( pCreature1->getObjectID() );
		gcNPCAsk.setScriptID( m_NotEnoughGetItemScriptID );
		gcNPCAsk.setNPCID( dynamic_cast<NPC*>(pCreature1)->getNPCID() );

		pPlayer->sendPacket( &gcNPCAsk );
	}
	else
	{
		GCNPCAsk gcNPCAsk;
		gcNPCAsk.setObjectID( pCreature1->getObjectID() );
		gcNPCAsk.setScriptID( m_EnoughGetItemScriptID );
		gcNPCAsk.setNPCID( dynamic_cast<NPC*>(pCreature1)->getNPCID() );

		pPlayer->sendPacket( &gcNPCAsk );
	}

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionCheckCommonQuestItem::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionCheckCommonQuestItem("
		<< "QuestItemNum:" << (int)m_QuestItemNum
		<< "EnoughScriptID:" << (int)m_EnoughGetItemScriptID
		<< "NotEnoughGetItemScriptID:" << (int)m_NotEnoughGetItemScriptID
	    << ")";
	return msg.toString();

	__END_CATCH
}
