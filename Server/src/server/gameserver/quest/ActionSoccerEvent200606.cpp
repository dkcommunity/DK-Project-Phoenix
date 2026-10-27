////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionSoccerEvent200606.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionSoccerEvent200606.h"
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
	string	Option;
};

const int MAX_ITEM_OPTION = 4;
const OPTION_TEMPLATE OptionTemplate[MAX_ITEM_OPTION] = 
{
	{ "STR+1" },
	{ "STR+2" },
	{ "STR+3" },
	{ "STR+4" },
};

struct EVENT_ATONEMENT_ITEM_TEMPLATE
{
	Item::ItemClass		ItemClass;
	ItemType_t			ItemType;
	string				ItemName;
};

const int SLAYER_MAX_ATOMNEMENT_ITEM_MAX = 4;
const EVENT_ATONEMENT_ITEM_TEMPLATE SlayerAtonementItemTemplate[SLAYER_MAX_ATOMNEMENT_ITEM_MAX] =
{
	{ Item::ITEM_CLASS_COAT,	18,		"카라만 아머 (M)"	},
	{ Item::ITEM_CLASS_COAT,	19,		"카라만 아머 (W)"	},
	{ Item::ITEM_CLASS_COAT,	20,		"쿼러시어 아머 (M)"	},
	{ Item::ITEM_CLASS_COAT,	21,		"쿼러시어 아머 (W)"	},
};

const int VAMPIRE_MAX_ATOMNEMENT_ITEM_MAX = 4;
const EVENT_ATONEMENT_ITEM_TEMPLATE VampireAtonementItemTemplate[VAMPIRE_MAX_ATOMNEMENT_ITEM_MAX] =
{
	{ Item::ITEM_CLASS_VAMPIRE_COAT,	14,		"블러드 폴 (M)"	},
	{ Item::ITEM_CLASS_VAMPIRE_COAT,	15,		"블러드 콥 (W)"	},
	{ Item::ITEM_CLASS_VAMPIRE_COAT,	16,		"로리카 코트 (M)"	},
	{ Item::ITEM_CLASS_VAMPIRE_COAT,	17,		"로리카 코트 (W)"	},
};

const int OUSTERS_MAX_ATOMNEMENT_ITEM_MAX = 2;
const EVENT_ATONEMENT_ITEM_TEMPLATE OustersAtonementItemTemplate[OUSTERS_MAX_ATOMNEMENT_ITEM_MAX] =
{
	{ Item::ITEM_CLASS_OUSTERS_COAT,	8,		"페이트리스 코트"	},
	{ Item::ITEM_CLASS_OUSTERS_COAT,	9,		"에페리얼 코트"	},
};

////////////////////////////////////////////////////////////////////////////////
// ActionSoccerEvent200606
////////////////////////////////////////////////////////////////////////////////
ActionSoccerEvent200606::ActionSoccerEvent200606()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionSoccerEvent200606::~ActionSoccerEvent200606()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionSoccerEvent200606::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
		// read script id
		m_EventBallNum = propertyBuffer.getPropertyInt("EventBall");
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
void ActionSoccerEvent200606::execute(Creature * pCreature1 , Creature * pCreature2)
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

	bool		bSlayer = pPC->isSlayer();
	bool		bVampire = pPC->isVampire();
	bool		bOusters = pPC->isOusters();
	int			itemNum = 0;

	// 아이템의 갯수를 얻어 온다
	itemNum = pPC->getItemClassTypeNum( Item::ITEM_CLASS_EVENT_STAR, 6 );

	// 선택한 퀘스트 아템의 갯수가 충분하지 않을때 처리
	if( itemNum < m_EventBallNum )
	{
		GCNPCResponse response;
		response.setCode( NPC_NOT_ENOUGH_SOCCER_BALL );
		pPlayer->sendPacket( &response );
		return;
	}

	_TPOINT pt;
	ItemInfo* pItemInfo = NULL;
	int itemIndex = 0;
	string itemOption;

	if( bSlayer || bVampire )
	{
		switch( m_EventBallNum )
		{
			case 400:
			case 600:
				{
					if( pPC->getSex() == 1 ) // male
					{
						itemIndex = 0;
					}
					else if( pPC->getSex() == 0 ) // female
					{
						itemIndex = 1;
					}
					break;
				}
			case 800:
			case 1000:
				{
					if( pPC->getSex() == 1 ) // male
					{
						itemIndex = 2;
					}
					else if( pPC->getSex() == 0 ) // female
					{
						itemIndex = 3;
					}
					break;
				}
			default:
				break;
		}
	}
	else if( bOusters )
	{
		switch( m_EventBallNum )
		{
			case 400:
			case 600:
				{
					itemIndex = 0;
					break;
				}
			case 800:
			case 1000:
				{
					itemIndex = 1;
					break;
				}
			default:
				break;
		}
	}
	else 
		return;

	if( bSlayer )
	{
		pItemInfo = g_pItemInfoManager->getItemInfo( SlayerAtonementItemTemplate[itemIndex].ItemClass, SlayerAtonementItemTemplate[itemIndex].ItemType );
	}
	else if( bVampire )
	{
		pItemInfo = g_pItemInfoManager->getItemInfo( VampireAtonementItemTemplate[itemIndex].ItemClass, VampireAtonementItemTemplate[itemIndex].ItemType );
	}
	else if( bOusters )
	{
		pItemInfo = g_pItemInfoManager->getItemInfo( OustersAtonementItemTemplate[itemIndex].ItemClass, OustersAtonementItemTemplate[itemIndex].ItemType );
	}

	Assert( pItemInfo != NULL );

	if( !pInventory->getEmptySlot( pItemInfo->getVolumeWidth(), pItemInfo->getVolumeHeight(), pt) )
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
		response.setParameter( NOT_ENOUGH_INVENTORY_SPACE );
		pPlayer->sendPacket( &response );
		return;
	}

	switch( m_EventBallNum )
	{
		case 400:
		case 800:
			{
				itemOption = OptionTemplate[0].Option;
				break;
			}
		case 600:
		case 1000:
			{
				int index = (rand()%3)+1;
				itemOption = OptionTemplate[index].Option;
				break;
			}
	}

	list<OptionType_t> options;

	if( itemOption != "" )
	{
		makeOptionList( itemOption, options );
	}

	Item* pItem = g_pItemFactoryManager->createItem( pItemInfo->getItemClass(), pItemInfo->getItemType(), options );
	Assert( pItem != NULL );

	pZone->registerObject( pItem );

	if( pInventory->addItem( pItem, pt )
			&& pPC->getItemClassTypeNum( Item::ITEM_CLASS_EVENT_STAR, 6 ) >= m_EventBallNum
	  )
	{
		pPC->decreaseItemClassTypeNum( Item::ITEM_CLASS_EVENT_STAR, 6, m_EventBallNum );

		pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

		remainTraceLog( pItem, "SoccerEvent200606", pPC->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC );

		GCCreateItem gcCreateItem;
		makeGCCreateItem(&gcCreateItem, pItem, pt.x, pt.y);

		pPlayer->sendPacket( &gcCreateItem );
	}

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
string ActionSoccerEvent200606::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionSoccerEvent200606("
		<< "EventBallNum:" << (int)m_EventBallNum
	    << ")";
	return msg.toString();

	__END_CATCH
}
