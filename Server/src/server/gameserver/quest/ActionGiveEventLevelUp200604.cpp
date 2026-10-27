////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveEventLevelUp200604.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionGiveEventLevelUp200604.h"
#include "Item.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "NPC.h"
#include "Player.h"
#include "Zone.h"
#include "VariableManager.h"
#include "DB.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "PacketUtil.h"

#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"
#include "Gpackets/GCCreateItem.h"

////////////////////////////////////////////////////////////////////////////////
// ActionGiveEventLevelUp200604
////////////////////////////////////////////////////////////////////////////////
ActionGiveEventLevelUp200604::ActionGiveEventLevelUp200604()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionGiveEventLevelUp200604::~ActionGiveEventLevelUp200604()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionGiveEventLevelUp200604::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
		// read script id
		m_Level = (Level_t)propertyBuffer.getPropertyInt("Level");
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
void ActionGiveEventLevelUp200604::execute(Creature * pCreature1 , Creature * pCreature2)
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

	Level_t level = pPC->getLevel();

	bool bSuccess = false;

	if ( level < m_Level )
	{
		GCNPCResponse gcNPCResponse;
		gcNPCResponse.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
		gcNPCResponse.setParameter( FAIL_GET_EVENT_ITEM );

		pPlayer->sendPacket( &gcNPCResponse );

		return;
	}

	Statement* pStmt = NULL;
	Result* pResult = NULL;
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		pResult = pStmt->executeQuery( "SELECT Level50, Level80, Level100 FROM EventLevelUp WHERE Name = '%s'", pPC->getName().c_str() );

		if ( pResult->next() )
		{
			int level50 = pResult->getInt(1);
			int level80 = pResult->getInt(2);
			int level100 = pResult->getInt(3);

			if ( ( m_Level == 50 && level50 != 0 )
				|| ( m_Level == 80 && level80 != 0 )
				|| ( m_Level == 100 && level100 != 0 )
			   )
			{
				GCNPCResponse gcNPCResponse;
				gcNPCResponse.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
				gcNPCResponse.setParameter( ALEADY_GET_EVENT_ITEM );
				pPlayer->sendPacket( &gcNPCResponse );

				SAFE_DELETE( pStmt );
				return;
			}
		}
		else
		{
			GCNPCResponse gcNPCResponse;
			gcNPCResponse.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
			gcNPCResponse.setParameter( YOU_ARE_NOT_EVENT_TARGET );
			pPlayer->sendPacket( &gcNPCResponse );

			SAFE_DELETE( pStmt );
			return;
		}

		SAFE_DELETE( pStmt );
	}
	END_DB(pStmt)

	TPOINT pt1, pt2;
	Item* pItem1 = NULL;
	Item* pItem2 = NULL;

	Item::ItemClass itemClass1 = Item::ITEM_CLASS_MAX;
	Item::ItemClass itemClass2 = Item::ITEM_CLASS_MAX;

	ItemType_t itemType1;
	ItemType_t itemType2;

	list<OptionType_t> option1;
	list<OptionType_t> option2;

	if ( pPC->isSlayer() )
	{
		switch( m_Level )
		{
			case 50:
				{
					itemClass1 = Item::ITEM_CLASS_COAT;
					itemClass2 = Item::ITEM_CLASS_TROUSER;

					if ( pPC->getSex() == MALE )
					{
						itemType1 = 12;
						itemType2 = 12;
					}
					else
					{
						itemType1 = 13;
						itemType2 = 13;
					}

					option1.push_back( 47 );	// DEF+5
					option1.push_back( 181 );	// RES+4
					option2.push_back( 47 );	// DEF+5
					option2.push_back( 181 );	// RES+4
				}
				break;
			case 80:
				{
					itemClass1 = Item::ITEM_CLASS_COAT;
					itemClass2 = Item::ITEM_CLASS_TROUSER;

					if ( pPC->getSex() == MALE )
					{
						itemType1 = 14;
						itemType2 = 14;
					}
					else
					{
						itemType1 = 15;
						itemType2 = 15;
					}

					option1.push_back( 47 );	// DEF+5
					option1.push_back( 181 );	// RES+4
					option2.push_back( 47 );	// DEF+5
					option2.push_back( 181 );	// RES+4
				}
				break;
			case 100:
				{
					itemClass1 = Item::ITEM_CLASS_RING;
					itemClass2 = Item::ITEM_CLASS_RING;

					itemType1 = 7;
					itemType2 = 7;

					option1.push_back( 185 );	// ATTR+3
					option2.push_back( 181 );	// RES+4
				}
				break;
			default:
				Assert(false);
				break;
		}
	}
	else if ( pPC->isVampire() )
	{
		switch( m_Level )
		{
			case 50:
				{
					itemClass1 = Item::ITEM_CLASS_VAMPIRE_COAT;

					if ( pPC->getSex() == MALE )
					{
						itemType1 = 8;
					}
					else
					{
						itemType1 = 9;
					}

					option1.push_back( 47 );	// DEF+5
					option1.push_back( 181 );	// RES+4
				}
				break;
			case 80:
				{
					itemClass1 = Item::ITEM_CLASS_VAMPIRE_COAT;

					if ( pPC->getSex() == MALE )
					{
						itemType1 = 10;
					}
					else
					{
						itemType1 = 11;
					}

					option1.push_back( 47 );	// DEF+5
					option1.push_back( 181 );	// RES+4
				}
				break;
			case 100:
				{
					itemClass1 = Item::ITEM_CLASS_VAMPIRE_RING;
					itemClass2 = Item::ITEM_CLASS_VAMPIRE_RING;

					itemType1 = 7;
					itemType2 = 7;

					option1.push_back( 185 );	// ATTR+3
					option2.push_back( 181 );	// RES+4
				}
				break;
			default:
				Assert(false);
				break;
		}
	}
	else if ( pPC->isOusters() )
	{
		switch( m_Level )
		{
			case 50:
				{
					itemClass1 = Item::ITEM_CLASS_OUSTERS_COAT;
					itemClass2 = Item::ITEM_CLASS_OUSTERS_BOOTS;

					itemType1 = 6;
					itemType2 = 6;

					option1.push_back( 47 );	// DEF+5
					option1.push_back( 181 );	// RES+4
					option2.push_back( 47 );	// DEF+5
					option2.push_back( 181 );	// RES+4
				}
				break;
			case 80:
				{
					itemClass1 = Item::ITEM_CLASS_OUSTERS_COAT;
					itemClass2 = Item::ITEM_CLASS_OUSTERS_BOOTS;

					itemType1 = 7;
					itemType2 = 7;

					option1.push_back( 47 );	// DEF+5
					option1.push_back( 181 );	// RES+4
					option2.push_back( 47 );	// DEF+5
					option2.push_back( 181 );	// RES+4
				}
				break;
			case 100:
				{
					itemClass1 = Item::ITEM_CLASS_OUSTERS_RING;
					itemClass2 = Item::ITEM_CLASS_OUSTERS_RING;

					itemType1 = 7;
					itemType2 = 7;

					option1.push_back( 185 );	// ATTR+3
					option2.push_back( 181 );	// RES+4
				}
				break;
			default:
				Assert(false);
				break;
		}
	}
	else
	{
		Assert(false);
	}

	if ( itemClass1 != Item::ITEM_CLASS_MAX )
	{
		pItem1 = g_pItemFactoryManager->createItem( itemClass1, itemType1, option1 );
		pZone->registerObject( pItem1 );
	}
	if ( itemClass2 != Item::ITEM_CLASS_MAX )
	{
		pItem2 = g_pItemFactoryManager->createItem( itemClass2, itemType2, option2 );
		pZone->registerObject( pItem2 );
	}

	if ( pItem1 != NULL && pInventory->addItem( pItem1, pt1 ) )
	{
		if ( pItem2 == NULL || pInventory->addItem( pItem2, pt2 ) )
		{
			bool bSendTimeLimitItemInfo = false;
			if ( pItem1 != NULL )
			{
				pItem1->create( pPC->getName(), STORAGE_INVENTORY, 0, pt1.x, pt1.y );

				if ( m_Level != 100 )
				{
					pPC->addTimeLimitItem( pItem1, 604800 );	// 7일 시간제
					bSendTimeLimitItemInfo = true;
				}
			}
			if ( pItem2 != NULL )
			{
				pItem2->create( pPC->getName(), STORAGE_INVENTORY, 0, pt2.x, pt2.y );

				if ( m_Level != 100 )
				{
					pPC->addTimeLimitItem( pItem2, 604800 );	// 7일 시간제
					bSendTimeLimitItemInfo = true;
				}
			}

			if ( bSendTimeLimitItemInfo )
				pPC->sendTimeLimitItemInfo();

			if ( pItem1 != NULL && pItem1->isTraceItem() )
			{
				remainTraceLog( pItem1, pCreature1->getName(), pCreature2->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC );
			}
			if ( pItem2 != NULL && pItem2->isTraceItem() )
			{
				remainTraceLog( pItem2, pCreature1->getName(), pCreature2->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC );
			}

			if ( pItem1 != NULL )
			{
				GCCreateItem gcCreateItem;
				makeGCCreateItem( &gcCreateItem, pItem1, pt1.x, pt1.y );
				pPlayer->sendPacket( &gcCreateItem );
			}
			if ( pItem2 != NULL )
			{
				GCCreateItem gcCreateItem;
				makeGCCreateItem( &gcCreateItem, pItem2, pt2.x, pt2.y );
				pPlayer->sendPacket( &gcCreateItem );
			}

			GCNPCResponse gcNPCResponse;
			gcNPCResponse.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
			gcNPCResponse.setParameter( YOU_GET_EVENT_ITEM );

			pPlayer->sendPacket( &gcNPCResponse );

			bSuccess = true;
		}
		else
		{
			pInventory->deleteItem(pt1.x, pt1.y);
			SAFE_DELETE( pItem1 );
			SAFE_DELETE( pItem2 );

			GCNPCResponse gcNPCResponse;
			gcNPCResponse.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
			gcNPCResponse.setParameter( NOT_ENOUGH_INVENTORY_SPACE );

			pPlayer->sendPacket( &gcNPCResponse );
		}
	}
	else
	{
		SAFE_DELETE( pItem1 );
		SAFE_DELETE( pItem2 );

		GCNPCResponse gcNPCResponse;
		gcNPCResponse.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
		gcNPCResponse.setParameter( NOT_ENOUGH_INVENTORY_SPACE );

		pPlayer->sendPacket( &gcNPCResponse );
	}

	GCNPCResponse gcNPCResponse;
	gcNPCResponse.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
	pPlayer->sendPacket( &gcNPCResponse );

	if ( bSuccess )
	{
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

			char subSQL[16];
			sprintf( subSQL, "Level%d", (int)m_Level );

			pStmt->executeQuery( "UPDATE EventLevelUp SET %s = 1 WHERE Name = '%s'", subSQL, pPC->getName().c_str() );

			SAFE_DELETE( pStmt );
		}
		END_DB( pStmt )
	}

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionGiveEventLevelUp200604::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionGiveEventLevelUp200604("
		<< "Level:" << (int)m_Level
	    << ")";
	return msg.toString();

	__END_CATCH
}
