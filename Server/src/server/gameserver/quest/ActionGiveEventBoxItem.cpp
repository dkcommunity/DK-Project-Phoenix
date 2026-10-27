////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveEventBoxItem.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionGiveEventBoxItem.h"
#include "Item.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "NPC.h"
#include "Player.h"
#include "Inventory.h"
#include "ItemFactoryManager.h"
#include "ItemInfoManager.h"
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
#include "Gpackets/GCDeleteInventoryItem.h"

#include <list>

struct OPTION_TEMPLATE
{
	string		Option;
	uint		Ratio;
};

const int MAX_ITEM_OPTION = 22;
const OPTION_TEMPLATE OptionTemplate[MAX_ITEM_OPTION] =
{
	{ "STR+4", 10 },
	{ "STR+5", 2 },
	{ "DEX+4", 10 },
	{ "DEX+5", 2 },
	{ "INT+4", 10 },
	{ "INT+5", 2 },
	{ "HP+4", 8 },
	{ "HP+5", 2 },
	{ "HPSTL+2", 3 },
	{ "HPSTL+3", 1 },
	{ "MPSTL+2", 3 },
	{ "MPSTL+3", 1 },
	{ "TOHIT+4", 8 },
	{ "TOHIT+5", 2 },
	{ "DEF+4", 10 },
	{ "DEF+5", 2 },
	{ "DAM+4", 8 },
	{ "DAM+5", 2 },
	{ "PRO+4", 8 },
	{ "PRO+5", 2 },
	{ "ATTR+2", 3 },
	{ "ATTR+3", 1 }
};

struct EVENT_ATONEMENT_ITEM_TEMPLATE
{
	Item::ItemClass		ItemClass;
	string				ItemName;
	uint				Ratio;
};

const int SLAYER_EVENT_ATONEMENT_ITEM_MAX = 3;
const EVENT_ATONEMENT_ITEM_TEMPLATE SlayerEventAtonementItemTemplate[SLAYER_EVENT_ATONEMENT_ITEM_MAX] = 
{
	{ Item::ITEM_CLASS_RING, "링", 50 },
	{ Item::ITEM_CLASS_BRACELET, "팔찌", 30 },
	{ Item::ITEM_CLASS_NECKLACE, "목걸이", 20 },
};

const int VAMPIRE_EVENT_ATONEMENT_ITEM_MAX = 5;
const EVENT_ATONEMENT_ITEM_TEMPLATE VampireEventAtonementItemTemplate[VAMPIRE_EVENT_ATONEMENT_ITEM_MAX] = 
{
	{ Item::ITEM_CLASS_VAMPIRE_RING, "뱀파이어 링", 40 },
	{ Item::ITEM_CLASS_VAMPIRE_BRACELET, "뱀파이어 팔찌", 15 },
	{ Item::ITEM_CLASS_VAMPIRE_NECKLACE, "뱀파이어 목걸이", 10 },
	{ Item::ITEM_CLASS_VAMPIRE_EARRING, "뱀파이어 귀걸이", 10 },
	{ Item::ITEM_CLASS_VAMPIRE_AMULET, "뱀파이어 아뮬렛", 25 },
};

const int OUSTERS_EVENT_ATONEMENT_ITEM_MAX = 3;
const EVENT_ATONEMENT_ITEM_TEMPLATE OustersEventAtonementItemTemplate[OUSTERS_EVENT_ATONEMENT_ITEM_MAX] = 
{
	{ Item::ITEM_CLASS_OUSTERS_PENDENT, "아우스터즈 팬던트", 25 },
	{ Item::ITEM_CLASS_OUSTERS_RING, "아우스터즈 링", 15 },
	{ Item::ITEM_CLASS_OUSTERS_STONE, "아우스터즈 정령석", 60 },
};

////////////////////////////////////////////////////////////////////////////////
// ActionGiveEventBoxItem
////////////////////////////////////////////////////////////////////////////////
ActionGiveEventBoxItem::ActionGiveEventBoxItem()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionGiveEventBoxItem::~ActionGiveEventBoxItem()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionGiveEventBoxItem::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
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
void ActionGiveEventBoxItem::execute(Creature * pCreature1 , Creature * pCreature2)
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

	Slayer*		pSlayer = NULL;
	Vampire*	pVampire = NULL;
	Ousters*	pOusters = NULL;

	FlagSet* pFlagSet = pPC->getFlagSet();

	// 이벤트 보상품을 받았는지 확인한다.
	if ( pFlagSet->isOn( FLAGSET_TRADE_GIFT_BOX_2006_12 ) )
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_TRADE_GIFT_BOX_ALREADY_TRADE );
		pPlayer->sendPacket( &response );

		GCNPCResponse quit;
		quit.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &quit );

		return;
	}

	if( !g_pVariableManager->isEventBox200612() )
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_GIVE_EVENT_ITEM_FAIL_NOW );
		pPlayer->sendPacket( &response );

		GCNPCResponse quit;
		quit.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &quit );

		return;
	}

	// 선물 상자가 있는지 확인한다.
	CoordInven_t X, Y;
	Item* pGiftBoxItem = pInventory->findItem( Item::ITEM_CLASS_EVENT_GIFT_BOX, 1, X, Y );
	if( pGiftBoxItem == NULL )
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_TRADE_GIFT_BOX_NO_ITEM );
		pPlayer->sendPacket( &response );

		GCNPCResponse quit;
		quit.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		return;
	}
/*	// 아래로 옴겼다 보상 해주고 삭제한다.
	// 선물 상자를 지우라고 보내준다 Client에~
	GCDeleteInventoryItem gcDeleteInventoryItem;
	gcDeleteInventoryItem.setObjectID( pGiftBoxItem->getObjectID() );
	pPlayer->sendPacket( &gcDeleteInventoryItem );

	// 선물 상자 삭제
	pInventory->deleteItem( X, Y );

	if( pGiftBoxItem != NULL && pGiftBoxItem->isTraceItem() )
	{
		remainTraceLog( pGiftBoxItem, pCreature2->getName(), pCreature1->getName(), ITEM_LOG_DELETE, DETAIL_EVENTPC );
	}
	//완존 삭제
	pGiftBoxItem->destroy();
	SAFE_DELETE( pGiftBoxItem );
*/
	int		ratio = rand()%100+1;
	int		sum = 0;
	string	OptionResult;
	for( int i = 0; i < MAX_ITEM_OPTION; i++ )
	{
		sum += OptionTemplate[i].Ratio;
		if( sum >= ratio )
		{
			OptionResult = OptionTemplate[i].Option;
			break;
		}
	}

	bool bAdvanceClass = false;
	Level_t	advanceClassLevel = 0;
	Level_t level = 0;
	ItemType_t itemType = 0;
	Item::ItemClass itemClass;

	if( bSlayer )
	{
		ratio = rand()%100+1;
		sum = 0;
		for( int i = 0; i < SLAYER_EVENT_ATONEMENT_ITEM_MAX; i++ )
		{
			sum += SlayerEventAtonementItemTemplate[i].Ratio;
			if( sum >= ratio )
			{
				itemClass = SlayerEventAtonementItemTemplate[i].ItemClass;
				break;
			}
		}

		pSlayer = dynamic_cast<Slayer*>(pCreature2);
		if( pSlayer != NULL )
		{
			level = pSlayer->getLevel();
			if( pSlayer->isAdvanced() )
			{
				bAdvanceClass = true;
				advanceClassLevel = pSlayer->getAdvancementClassLevel();

				if( advanceClassLevel <= 10 )
				{
					itemType = 9;
				}
				else if( advanceClassLevel <= 20 )
				{
					itemType = 11;
				}
				else
				{
					itemType = 12;
				}
			}
			else
			{
				if( level <= 30 )
				{
					itemType = 4;
				}
				else if( level <= 60 )
				{
					itemType = 5;
				}
				else if( level <= 90 )
				{
					itemType = 6;
				}
				else if( level <= 120 )
				{
					itemType = 7;
				}
				else
				{
					itemType = 8;
				}
			}
		}
	}
	else if( bVampire )
	{
		ratio = rand()%100+1;
		sum = 0;
		for( int i = 0; i < VAMPIRE_EVENT_ATONEMENT_ITEM_MAX; i++ )
		{
			sum += VampireEventAtonementItemTemplate[i].Ratio;
			if( sum >= ratio )
			{
				itemClass = VampireEventAtonementItemTemplate[i].ItemClass;
				break;
			}
		}

		pVampire = dynamic_cast<Vampire*>(pCreature2);
		if( pVampire != NULL )
		{
			level = pVampire->getLevel();
			if( pVampire->isAdvanced() )
			{
				bAdvanceClass = true;
				advanceClassLevel = pVampire->getAdvancementClassLevel();
				if( advanceClassLevel <= 10 )
				{
					if( itemClass == Item::ITEM_CLASS_VAMPIRE_BRACELET )
						itemType = 10;
					else
						itemType = 9;
				}
				else if( advanceClassLevel <= 20 )
				{
					if( itemClass == Item::ITEM_CLASS_VAMPIRE_BRACELET )
						itemType = 10;
					else
						itemType = 11;
				}
				else
				{
					if( itemClass == Item::ITEM_CLASS_VAMPIRE_BRACELET )
						itemType = 11;
					else
						itemType = 12;
				}
			}
			else
			{
				if( level <= 30 )
				{
					itemType = 4;
				}
				else if( level <= 60 )
				{
					itemType = 5;
				}
				else if( level <= 90 )
				{
					itemType = 6;
				}
				else if( level <= 120 )
				{
					itemType = 7;
				}
				else
				{
					itemType = 8;
				}
			}
		}
	}
	else if( bOusters )
	{
		ratio = rand()%100+1;
		sum = 0;
		for( int i = 0; i < OUSTERS_EVENT_ATONEMENT_ITEM_MAX; i++ )
		{
			sum += OustersEventAtonementItemTemplate[i].Ratio;
			if( sum >= ratio )
			{
				itemClass = OustersEventAtonementItemTemplate[i].ItemClass;
				break;
			}
		}

		ratio = rand()%3;
		pOusters = dynamic_cast<Ousters*>(pCreature2);
		if( pOusters != NULL )
		{
			level = pOusters->getLevel();
			if( pOusters->isAdvanced() )
			{
				bAdvanceClass = true;
				advanceClassLevel = pOusters->getAdvancementClassLevel();

				if( advanceClassLevel <= 10 )
				{
					if( itemClass == Item::ITEM_CLASS_OUSTERS_STONE )
					{
						itemType = 4 + (ratio*5);
					}
					else
					{
						itemType = 9;
					}
				}
				else if( advanceClassLevel <= 20 )
				{
					if( itemClass == Item::ITEM_CLASS_OUSTERS_STONE )
					{
						itemType = 4 + (ratio*5);
					}
					else
					{
						itemType = 10;
					}
				}
				else if( advanceClassLevel < 30 )
				{
					if( itemClass == Item::ITEM_CLASS_OUSTERS_STONE )
					{
						itemType = 4 + (ratio*5);
					}
					else
					{
						itemType = 11;
					}
				}
			}
			else
			{
				if( level <= 30 )
				{
					if( itemClass == Item::ITEM_CLASS_OUSTERS_STONE ) 
					{
						itemType = 1 + (ratio*5);
					}
					else
						itemType = 4;
				}
				else if( level <= 60 )
				{
					if( itemClass == Item::ITEM_CLASS_OUSTERS_STONE )
					{
						itemType = 2 + (ratio*5);
					}
					else
						itemType = 5;
				}
				else if( level <= 90 )
				{
					if( itemClass == Item::ITEM_CLASS_OUSTERS_STONE )
					{
						itemType = 3 + (ratio*5);
					}
					else
						itemType = 6;
				}
				else if( level <=120 )
				{
					if( itemClass == Item::ITEM_CLASS_OUSTERS_STONE )
					{
						itemType = 3 + (ratio*5);
					}
					else
						itemType = 7;
				}
				else
				{
					if( itemClass == Item::ITEM_CLASS_OUSTERS_STONE )
					{
						itemType = 4 + (ratio*5);
					}
					else
						itemType = 8;
				}
			}
		}
	}
	else
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &response );
		return;
	}

	ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( itemClass, itemType );
	Assert( pItemInfo != NULL );

	list<OptionType_t> options;

	if( OptionResult != "" )
	{
		makeOptionList( OptionResult, options );
	}

	Item* pItem = g_pItemFactoryManager->createItem( pItemInfo->getItemClass(), pItemInfo->getItemType(), options );
	Assert( pItem != NULL );

	_TPOINT pt;
	if( !pInventory->getEmptySlot(pItemInfo->getVolumeWidth(), pItemInfo->getVolumeHeight(), pt) )
	{
		GCNPCResponse quit;
		quit.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		quit.setParameter( NOT_ENOUGH_INVENTORY_SPACE );
		pPlayer->sendPacket( &quit );
		return;
	}

	pZone->registerObject( pItem );

	if( pInventory->addItem( pItem, pt ) )
	{
		pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

		remainTraceLog( pItem, "200612EventBox", pPC->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);

		GCCreateItem gcCreateItem;
		makeGCCreateItem( &gcCreateItem, pItem, pt.x, pt.y );
		pPlayer->sendPacket( &gcCreateItem );
	}
		
	// 선물 상자를 지우라고 보내준다 Client에~
	GCDeleteInventoryItem gcDeleteInventoryItem;
	gcDeleteInventoryItem.setObjectID( pGiftBoxItem->getObjectID() );
	pPlayer->sendPacket( &gcDeleteInventoryItem );

	// 선물 상자 삭제
	pInventory->deleteItem( X, Y );

	if( pGiftBoxItem != NULL && pGiftBoxItem->isTraceItem() )
	{
		remainTraceLog( pGiftBoxItem, pCreature2->getName(), pCreature1->getName(), ITEM_LOG_DELETE, DETAIL_EVENTNPC );
	}

	//완존 삭제
	pGiftBoxItem->destroy();
	SAFE_DELETE( pGiftBoxItem );
	// Flag를 켠다.
	pFlagSet->turnOn( FLAGSET_TRADE_GIFT_BOX_2006_12 );
	// Flag를 저장한다.
	pFlagSet->save( pPC->getName() );

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
string ActionGiveEventBoxItem::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionGiveEventBoxItem("
	    << ")";
	return msg.toString();

	__END_CATCH
}
