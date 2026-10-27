////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGambleSystem.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionGambleSystem.h"
#include "Item.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
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
#include "GamePlayer.h"
#include "DB.h"
#include "PacketUtil.h"

#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"

#include <list>
#include <math.h>

struct OPTION_TEMPLATE
{
	string		Option;
	uint		Ratio;
};

const int MAX_ITEM_OPTION	=	8;
const OPTION_TEMPLATE OptionTemplate[MAX_ITEM_OPTION] = 
{
	{ "STR", 8 },
	{ "DEX", 8 },
	{ "INT", 8 },
	{ "HP", 12 },
	{ "TOHIT", 16 },
	{ "DEF", 16 },
	{ "PRO", 16 },
	{ "CRI", 16 },
};

const int MAX_GRADE = 5;
const OPTION_TEMPLATE OptionGradeTemplate[MAX_GRADE] = 
{
	{ "+1", 0 },
	{ "+2", 0 },
	{ "+3", 0 },
	{ "+4", 0 },
	{ "+5", 0 },
};

struct GAMBLE_ITEM_TEMPLATE
{
	Item::ItemClass		ItemClass;
	ItemType_t			ItemType;
	string				ItemName;
};

const int GAMBLE_ITEM_MAX = 1;
const GAMBLE_ITEM_TEMPLATE GambleItemTemplate[GAMBLE_ITEM_MAX] = 
{
	{ Item::ITEM_CLASS_CONTRACT_OF_BLOOD, 0, "피의 계약" },
};

////////////////////////////////////////////////////////////////////////////////
// ActionGambleSystem
////////////////////////////////////////////////////////////////////////////////
ActionGambleSystem::ActionGambleSystem()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionGambleSystem::~ActionGambleSystem()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionGambleSystem::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
		/*
		m_EnoughGetItemScriptID = (ScriptID_t)propertyBuffer.getPropertyInt("");
		*/
		m_SelectID				= (int)propertyBuffer.getPropertyInt("SelectID");
		m_NextGambleScriptID	= (ScriptID_t)propertyBuffer.getPropertyInt("NextScript");
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
void ActionGambleSystem::execute(Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	Assert(pCreature1 != NULL);
	Assert(pCreature2 != NULL);
	Assert(pCreature1->isNPC());
	Assert(pCreature2->isPC());

	Gold_t	GambleMoney = 10000;

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert(pPC != NULL);

	Player* pPlayer = pPC->getPlayer();
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );

	Zone* pZone = pPC->getZone();
	Assert( pZone != NULL );

	bool			bSlayer = pPC->isSlayer();
	bool			bVampire = pPC->isVampire();
	bool			bOusters = pPC->isOusters();
	Slayer*			pSlayer = NULL;
	Vampire*		pVampire = NULL;
	Ousters*		pOusters = NULL;
	BYTE			Count = 0;
	BYTE			Accumulation = 0;
	uint			oldAccumulation = 0;
	Gold_t			playerGold = 0;
	bool			gambleInit = false;
	bool			giveItem = false;
	string			PlayerID;

	if( bSlayer )
	{
		pSlayer = dynamic_cast<Slayer*>(pCreature2);
		Count = pSlayer->getGambleTryCount();
		Accumulation = pSlayer->getGambleAccumulation();
		playerGold = pSlayer->getGold();
		PlayerID = "Slayer";
	}
	else if( bVampire )
	{
		pVampire= dynamic_cast<Vampire*>(pCreature2);
		Count = pVampire->getGambleTryCount();
		Accumulation = pVampire->getGambleAccumulation();
		playerGold = pVampire->getGold();
		PlayerID = "Vampire";
	}
	else if( bOusters )
	{
		pOusters= dynamic_cast<Ousters*>(pCreature2);
		Count = pOusters->getGambleTryCount();
		Accumulation = pOusters->getGambleAccumulation();
		playerGold = pOusters->getGold();
		PlayerID = "Ousters";
	}
	else
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &response );
		return;
	}

	if( m_SelectID == 1 )
	{
		/*
		Statement* pStmt;
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getDistConnection("DARKEDEN")->createStatement();
			pStmt->executeQuery( "UPDATE %s set GambleAccumulation = 0, GambleTryCount = 0 WHERE Name = '%s'", PlayerID.c_str(), pCreature2->getName().c_str() );
			SAFE_DELETE( pStmt );
		}
		END_DB( pStmt )
		*/

		char query[128];
		sprintf( query, "GambleAccumulation = %d, GambleTryCount = %d", 0, 0);
		if( pSlayer != NULL )
		{
			pSlayer->setGambleAccumulation(0);
			pSlayer->setGambleTryCount(0);
			pSlayer->tinysave(query);
		}
		else if( pVampire != NULL )
		{
			pVampire->setGambleAccumulation(0);
			pVampire->setGambleTryCount(0);
			pVampire->tinysave(query);
		}
		else if( pOusters != NULL )
		{
			pOusters->setGambleAccumulation(0);
			pOusters->setGambleTryCount(0);
			pOusters->tinysave(query);
		}
			
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_GAMBLE_INIT_OK );
		pPlayer->sendPacket( &response );

		GCNPCAsk gcNPCAsk;
		gcNPCAsk.setObjectID( pCreature1->getObjectID() );
		gcNPCAsk.setScriptID( m_NextGambleScriptID );
		gcNPCAsk.setNPCID( dynamic_cast<NPC*>(pCreature1)->getNPCID() );

		pPlayer->sendPacket( &gcNPCAsk );

		return;
	}

	// 겜블시의 필요한 금액 
	GambleMoney = (Gold_t)(pow(1.5, Count) * GambleMoney);

	// 돈이 충분 하지 않을때
	if( GambleMoney > playerGold )
	{
/*		GCNPCAsk gcNPCAsk;
		gcNPCAsk.setObjectID( Creature1->getObjectID() );
		gcNPCAsk.setScriptID( m_NotEnoughGoldScriptID );
		gcNPCask.setNPCID( dynamic_cast<NPC*>(pCreature1)->getNPCID() );
		pPlayer->sendPacket( &gcNPCAsk ); */
		GCNPCResponse	gcNPCResponse;
		gcNPCResponse.setCode( NPC_RESPONSE_NOT_ENOUGH_MONEY );
		pPC->getPlayer()->sendPacket( &gcNPCResponse );
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &response );
		return;
	}

	ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo( GambleItemTemplate[0].ItemClass, GambleItemTemplate[0].ItemType );
	Assert( pItemInfo != NULL );

	//아이템의 공간 체크
	_TPOINT pt;
	if( !pInventory->getEmptySlot(pItemInfo->getVolumeWidth(), pItemInfo->getVolumeHeight(), pt) )
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
		response.setParameter( NOT_ENOUGH_INVENTORY_SPACE );
		pPlayer->sendPacket( &response );
		return;
	}

	// 소지 금액을 빼준다
	pPC->decreaseGoldEx( GambleMoney );

	GCModifyInformation	gcMI;
	gcMI.addLongData( MODIFY_GOLD, pPC->getGold() );
	pPlayer->sendPacket( &gcMI );

	int randomCount = rand()%3+1;

	Accumulation = Accumulation+randomCount;
	BYTE	bingo = 0;				// Accumulation의 10단위의 숫자를 제거한 수
	int		itemOptionType = 0;		// 옵션의 타입 
	string	OptionValue;			// 옵션의 타입
	string	ItemOption;				// 옵션의 등급과 타입

	oldAccumulation = (uint)Accumulation;

	itemOptionType = (int)(Accumulation/10);

	int Sum= 0;
	int ratio = rand()%100 + 1;
	for( int i= 0; i < MAX_ITEM_OPTION; i++ )
	{
		Sum += OptionTemplate[i].Ratio;
		if( Sum >= ratio )
		{
			OptionValue = OptionTemplate[i].Option;
			break;
		}
	}

	if( Accumulation >= 100 )
	{
		gambleInit = true;
	}
	else
	{
		bingo = Accumulation%10;
	}

	switch( bingo )
	{
		case 3:
		case 6:
		case 9:
			gambleInit = true;
			break;
	}

	if( gambleInit )
	{
		switch( itemOptionType )
		{
			case 0:
			case 5:
				ItemOption = OptionValue + OptionGradeTemplate[0].Option;
				break;
			case 1:
			case 6:
				ItemOption = OptionValue + OptionGradeTemplate[1].Option;
				break;
			case 2:
			case 7:
				ItemOption = OptionValue + OptionGradeTemplate[2].Option;
				break;
			case 3:
			case 8:
				ItemOption = OptionValue + OptionGradeTemplate[3].Option;
				break;
			case 4:
			case 9:
				ItemOption = OptionValue + OptionGradeTemplate[4].Option;
				break;
			default:
				ItemOption = OptionValue + OptionGradeTemplate[4].Option;
				break;
		}
		list<OptionType_t> options;
		makeOptionList( ItemOption, options );

		Item* pItem = g_pItemFactoryManager->createItem(pItemInfo->getItemClass(), pItemInfo->getItemType(), options );
		Assert( pItem != NULL );
		
		pZone->registerObject( pItem );

		if( pInventory->addItem( pItem, pt ) )
		{
			pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

			remainTraceLog( pItem, "GambleItem", pPC->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC );

			if( itemOptionType < 5 )
			{
				pPC->addTimeLimitItem( pItem, 1209600 );		// 14일
				pPC->sendTimeLimitItemInfo();
			}

			GCCreateItem gcCreateItem;
			makeGCCreateItem(&gcCreateItem, pItem, pt.x, pt.y);

			pPlayer->sendPacket( &gcCreateItem );
		}
		Accumulation = 0;
		Count = 0;
		giveItem = true;
	}
	else
	{
		Count += 1;
	}

	char query[128];

	if( pSlayer != NULL )
	{
		pSlayer->setGambleAccumulation(Accumulation);
		pSlayer->setGambleTryCount(Count);
		sprintf( query, "GambleAccumulation=%d, GambleTryCount=%d", Accumulation, Count );			
		pSlayer->tinysave( query );
	}
	else if( pVampire != NULL )
	{
		pVampire->setGambleAccumulation(Accumulation);
		pVampire->setGambleTryCount(Count);
		sprintf( query, "GambleAccumulation=%d, GambleTryCount=%d", Accumulation, Count );			
		pVampire->tinysave( query );
	}
	else if( pOusters != NULL )
	{
		pOusters->setGambleAccumulation(Accumulation);
		pOusters->setGambleTryCount(Count);
		sprintf( query, "GambleAccumulation=%d, GambleTryCount=%d", Accumulation, Count );			
		pOusters->tinysave( query );
	}
/*	
	Statement* pStmt;
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection("DARKEDEN")->createStatement();
		pStmt->executeQuery( "UPDATE %s set GambleAccumulation = %d, GambleTryCount = %d WHERE Name = '%s'", PlayerID.c_str(), Accumulation, Count, pCreature2->getName().c_str() ); 
		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )
*/
	if( giveItem )
	{
		GCNPCResponse response;
		response.setCode( NPC_RESPONSE_GIVE_GAMBLE_ITEM_OK );
		response.setParameter(oldAccumulation);
		pPlayer->sendPacket( &response );
		
		response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &response );
		return;
	}

	GCNPCAsk gcNPCAsk;
	gcNPCAsk.setObjectID( pCreature1->getObjectID() );
	gcNPCAsk.setScriptID( m_NextGambleScriptID );
	gcNPCAsk.setNPCID( dynamic_cast<NPC*>(pCreature1)->getNPCID() );
	
	pPlayer->sendPacket( &gcNPCAsk );

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionGambleSystem::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionGambleSystem("
	    << ")";
	return msg.toString();

	__END_CATCH
}
