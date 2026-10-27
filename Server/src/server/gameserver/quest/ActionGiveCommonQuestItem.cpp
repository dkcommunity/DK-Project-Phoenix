////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveCommonQuestItem.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionGiveCommonQuestItem.h"
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
#include "PacketUtil.h"

#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"

#include <list>

struct OPTION_TEMPLATE
{
	string		Option;
	uint		Ratio;
};

const int MAX_ITEM_OPTION = 7;
const OPTION_TEMPLATE OptionTemplate[MAX_ITEM_OPTION] =
{
	{ "STR", 15 },
	{ "DEX", 15 },
	{ "INT", 15 },
	{ "ATTR", 10 },
	{ "ASPD", 15 },
	{ "TOHIT", 15 },
	{ "PRO", 15 }
};

const int MAX_GRADE = 5;
const OPTION_TEMPLATE OptionGradeTemplate[MAX_GRADE] = 
{
	{ "+1", 3 },
	{ "+2", 7 },
	{ "+3", 45 },
	{ "+4", 30 },
	{ "+5", 15 }
};

struct QUEST_ATONEMENT_ITEM_TEMPLATE
{
	Item::ItemClass		ItemClass;
	ItemType_t			ItemType;
	string				ItemName;
};

const int QUEST_ATONEMENT_ITEM_MAX = 5;
const QUEST_ATONEMENT_ITEM_TEMPLATE QuestAtonementItemTemplate[QUEST_ATONEMENT_ITEM_MAX] = 
{
	{ Item::ITEM_CLASS_EFFECT_ITEM, 20, "광폭의 향"},
	{ Item::ITEM_CLASS_EFFECT_ITEM, 21, "강력의 향"},
	{ Item::ITEM_CLASS_EFFECT_ITEM, 22, "활성의 향"},
	{ Item::ITEM_CLASS_EFFECT_ITEM, 23, "철벽의 향"},
	{ Item::ITEM_CLASS_ETHEREAL_CHAIN, 0, "에테리얼 체인"}
};

////////////////////////////////////////////////////////////////////////////////
// ActionGiveCommonQuestItem
////////////////////////////////////////////////////////////////////////////////
ActionGiveCommonQuestItem::ActionGiveCommonQuestItem()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionGiveCommonQuestItem::~ActionGiveCommonQuestItem()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionGiveCommonQuestItem::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
		// read script id
		m_RequestItemNum = (ItemNum_t)propertyBuffer.getPropertyInt("ItemID");
		m_QuestItemNum = propertyBuffer.getPropertyInt("QuestItem");
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
void ActionGiveCommonQuestItem::execute(Creature * pCreature1 , Creature * pCreature2)
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

	ItemType_t itemType = 0;
	if ( bSlayer )
	{
		itemType = 0;
	}
	else if ( bVampire )
	{
		itemType = 1;
	}
	else if ( bOusters )
	{
		itemType = 2;
	}
	else
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &response );
		return;
	}

	string			itemOption; 
	
	ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( QuestAtonementItemTemplate[m_RequestItemNum-1].ItemClass, QuestAtonementItemTemplate[m_RequestItemNum-1].ItemType );
	Assert( pItemInfo != NULL );

	// 인벤토리의 공간을 체크
	_TPOINT	pt;
	if( !pInventory->getEmptySlot(pItemInfo->getVolumeWidth(), pItemInfo->getVolumeHeight(), pt) )
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
		response.setParameter( NOT_ENOUGH_INVENTORY_SPACE );
		pPlayer->sendPacket( &response );
		return;
	}

	if( m_RequestItemNum == 5 )
	{
		string				OptionResult1;
		string				OptionResult2;
		string				OptionGrade1;
		string				OptionGrade2;

		int Ratio = rand() % 100 + 1;
		int	Sum = 0;

		for( int i = 0; i < MAX_ITEM_OPTION; i++ )
		{
			Sum += OptionTemplate[i].Ratio;
			if( Sum >= Ratio )
			{
				OptionResult1 = OptionTemplate[i].Option;
				break;
			}
		}

		do{
			Sum = 0;
			Ratio = rand() % 100 + 1;
			for( int i = 0; i < MAX_ITEM_OPTION; i++ )
			{
				Sum += OptionTemplate[i].Ratio;
				if( Sum >= Ratio)
				{
					OptionResult2 = OptionTemplate[i].Option;
					break;
				}
			}
		}
		while( OptionResult1 == OptionResult2 );

		Sum = 0;
		Ratio = rand() % 100 + 1;
		for( int i = 0; i < MAX_GRADE; i++ )
		{
			Sum += OptionGradeTemplate[i].Ratio;
			if( Sum >= Ratio )
			{
				OptionGrade1 = OptionGradeTemplate[i].Option;
				break;
			}
		}

		Sum = 0;
		Ratio = rand() % 100 + 1;
		for( int i = 0; i < MAX_GRADE; i++ )
		{
			Sum += OptionGradeTemplate[i].Ratio;
			if( Sum >= Ratio )
			{
				OptionGrade2 = OptionGradeTemplate[i].Option;
				break;
			}
		}

		// 아이템의 옵션을 넣어준다.
		itemOption = OptionResult1+OptionGrade1+","+OptionResult2+OptionGrade2;
	}

	list<OptionType_t> options;

	if( itemOption != "" )
	{
		makeOptionList( itemOption, options );
	}

	Item* pItem = g_pItemFactoryManager->createItem( pItemInfo->getItemClass(), pItemInfo->getItemType(), options );
	Assert( pItem != NULL );

	pZone->registerObject( pItem );

	if( pInventory->addItem( pItem, pt) 
		&& pPC->getItemClassTypeNum( Item::ITEM_CLASS_COMMON_QUEST_ITEM, itemType ) >= m_QuestItemNum
	  )
	{
		pPC->decreaseItemClassTypeNum( Item::ITEM_CLASS_COMMON_QUEST_ITEM, itemType, m_QuestItemNum );

		pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

		remainTraceLog( pItem, "CommonQuestItem", pPC->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC );

		GCCreateItem gcCreateItem;
		makeGCCreateItem(&gcCreateItem, pItem, pt.x, pt.y);

		pPlayer->sendPacket( &gcCreateItem );
	}

	// 대화창 닫기
	GCNPCResponse response;
	response.setCode( NPC_RESPONSE_GIVE_EVENT_ITEM_OK );
	pPlayer->sendPacket( &response );

	response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
	pPlayer->sendPacket( &response );

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionGiveCommonQuestItem::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionGiveCommonQuestItem("
		<< "RequestItemNum:" << (int)m_RequestItemNum
		<< "QuestItemNum:" << (int)m_QuestItemNum
	    << ")";
	return msg.toString();

	__END_CATCH
}
