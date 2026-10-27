////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionGiveComebackEventItem200604.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionGiveComebackEventItem200604.h"
#include "Item.h"
#include "Slayer.h"
#include "Vampire.h"
#include "NPC.h"
#include "PlayerCreature.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "ItemInfoManager.h"
#include "OptionInfo.h"
#include "PacketUtil.h"
#include "Zone.h"
#include "VariableManager.h"
#include "NicknameBook.h"
#include "FlagSet.h"
#include "DB.h"
#include "item/PetItem.h"

#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"

#include <list>

struct EVENT_ITEM_TEMPLATE 
{
	Item::ItemClass		ItemClass;
	ItemType_t			ItemType;
};

const int COMEBACK_EVENT_ITEM_SLAYER_MAX = 5;
const EVENT_ITEM_TEMPLATE ComeBackEventSlayer[COMEBACK_EVENT_ITEM_SLAYER_MAX] =
{
	{ Item::ITEM_CLASS_NECKLACE,		8 },
	{ Item::ITEM_CLASS_BRACELET,		8 },
	{ Item::ITEM_CLASS_BRACELET,		8 },
	{ Item::ITEM_CLASS_RING,			8 },
	{ Item::ITEM_CLASS_RING,			8 },
};

const int COMEBACK_EVENT_ITEM_VAMPIRE_MAX = 5;
const EVENT_ITEM_TEMPLATE ComeBackEventVampire[COMEBACK_EVENT_ITEM_VAMPIRE_MAX] =
{
	{ Item::ITEM_CLASS_VAMPIRE_NECKLACE,		8 },
	{ Item::ITEM_CLASS_VAMPIRE_BRACELET,		7 },
	{ Item::ITEM_CLASS_VAMPIRE_RING,			8 },
	{ Item::ITEM_CLASS_VAMPIRE_EARRING,			8 },
	{ Item::ITEM_CLASS_VAMPIRE_AMULET,			8 },
};

const int COMEBACK_EVENT_ITEM_OUSTERS_MAX = 5;
const EVENT_ITEM_TEMPLATE ComeBackEventOusters[COMEBACK_EVENT_ITEM_OUSTERS_MAX] =
{
	{ Item::ITEM_CLASS_OUSTERS_RING,		8 },
	{ Item::ITEM_CLASS_OUSTERS_RING,		8 },
	{ Item::ITEM_CLASS_OUSTERS_PENDENT,		8 },
	{ Item::ITEM_CLASS_OUSTERS_PENDENT,		8 },    
	{ Item::ITEM_CLASS_OUSTERS_PENDENT,		8 },    
};

////////////////////////////////////////////////////////////////////////////////
// ActionGiveComebackEventItem200604
////////////////////////////////////////////////////////////////////////////////
ActionGiveComebackEventItem200604::ActionGiveComebackEventItem200604()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionGiveComebackEventItem200604::~ActionGiveComebackEventItem200604()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionGiveComebackEventItem200604::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
		// read script id
		m_EventType = propertyBuffer.getProperty("EventCheckType");
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
void ActionGiveComebackEventItem200604::execute(Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY
	Assert( pCreature1 != NULL );
	Assert( pCreature2 != NULL );
	Assert( pCreature1->isNPC() );
	Assert( pCreature2->isPC() );

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert( pPC != NULL );
		
	Player* pPlayer = pPC->getPlayer();
	Assert( pPlayer != NULL );
	Assert( pCreature1 != NULL );
	Assert( pCreature2 != NULL );

	if( m_EventType == "COMEBACK2006" )
		executeComebackItem(pCreature1, pCreature2);
	else if( m_EventType == "COMEBACKPREMIUM2006" )
		executeComebackPremiumItem(pCreature1, pCreature2);
	else
	{
		GCNPCResponse	response;
		response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
		pPlayer->sendPacket( &response );
	}

	__END_CATCH
}

void ActionGiveComebackEventItem200604::executeComebackPremiumItem(Creature* pCreature1, Creature* pCreature2)
{
	__BEGIN_TRY

	Assert( pCreature1 != NULL );
	Assert( pCreature2 != NULL );
	Assert( pCreature1->isNPC() );
	Assert( pCreature2->isPC() );

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert( pPC != NULL );
		
	Player* pPlayer = pPC->getPlayer();
	Assert( pPlayer != NULL );

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert( pGamePlayer != NULL );

	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );

	Zone* pZone = pPC->getZone();
	Assert( pZone != NULL );

	Statement* pStmt = NULL;

	bool	bSlayer = pPC->isSlayer();
	bool	bVampire = pPC->isVampire();
	bool	bOusters = pPC->isOusters();

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection( "DARKEDEN" )->createStatement();
		Result* pResult = pStmt->executeQuery( "SELECT PayPremiumDate, RecvPremiumItemDate FROM Event200604 WHERE PlayerID = '%s'",
				pGamePlayer->getID().c_str() );

		if ( pResult->next() )
		{
			string PayPremiumDate = pResult->getString(1);
			string RecvItemDate = pResult->getString(2);

			if ( PayPremiumDate == "0000-00-00" )
			{
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
				response.setParameter( YOU_ARE_NOT_EVENT_TARGET );
				pPlayer->sendPacket( &response );

				SAFE_DELETE( pStmt );
				return;
			}

			if ( RecvItemDate == "0000-00-00" )
			{
				// 아이템 주자~
				Creature* pCreature = pGamePlayer->getCreature();
				Assert( pCreature != NULL );

				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
				Assert( pPC != NULL );

				Zone* pZone = pPC->getZone();
				Assert( pZone != NULL );

				Inventory* pInventory = pPC->getInventory();
				Assert( pInventory != NULL );

				Item* pItem[5] = { NULL, NULL, NULL, NULL, NULL };

				// 옵션 DAM_3
				list<OptionType_t> optionType;
				optionType.push_back( 50 );     // DAM+3

				if ( bSlayer )
				{
					for( int loop_i = 0; loop_i < COMEBACK_EVENT_ITEM_SLAYER_MAX; ++loop_i )
					{
						pItem[loop_i] = g_pItemFactoryManager->createItem( ComeBackEventSlayer[loop_i].ItemClass, ComeBackEventSlayer[loop_i].ItemType, optionType );
						Assert( pItem != NULL );
					}
				}
				else if ( bVampire )
				{
					for( int loop_i = 0; loop_i < COMEBACK_EVENT_ITEM_VAMPIRE_MAX; ++loop_i )
					{
						pItem[loop_i] = g_pItemFactoryManager->createItem( ComeBackEventVampire[loop_i].ItemClass, ComeBackEventVampire[loop_i].ItemType, optionType );
						Assert( pItem != NULL );
					}
				}
				else if ( bOusters )
				{
					for( int loop_i = 0; loop_i < COMEBACK_EVENT_ITEM_OUSTERS_MAX; ++loop_i )
					{
						pItem[loop_i] = g_pItemFactoryManager->createItem( ComeBackEventOusters[loop_i].ItemClass, ComeBackEventOusters[loop_i].ItemType, optionType );
						Assert( pItem != NULL );
					}
				}
				else
				{
					GCNPCResponse response;
					response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
					response.setParameter( FAIL_GET_EVENT_ITEM );
					pPlayer->sendPacket( &response );

					SAFE_DELETE( pStmt );
					return;
				}

				for ( int i=0; i<5; ++i )
				{
					if ( pItem[i] == NULL )
					{
						for ( int j=0; j<5; ++j )
						{
							SAFE_DELETE( pItem[j] );
						}

						GCNPCResponse response;
						response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
						response.setParameter( FAIL_GET_EVENT_ITEM );
						pPlayer->sendPacket( &response );

						SAFE_DELETE( pStmt );
						return;
					}

					pItem[i]->setGrade( 6 );
				}

				_TPOINT tp[5];
				for ( int i=0; i<5; ++i )
				{
					// 빈공간 확인

					if ( !pInventory->getEmptySlot( pItem[i], tp[i] ) )
					{
						// 클리어. 롤백
						for ( int j=0; j<i; ++j )
						{
							pInventory->deleteItem( pItem[j]->getObjectID() );
						}

						for ( int j=0; j<5; ++j )
						{
							SAFE_DELETE( pItem[j] );
						}

						GCNPCResponse response;
						response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
						response.setParameter( NOT_ENOUGH_INVENTORY_SPACE );
						pPlayer->sendPacket( &response );

						SAFE_DELETE( pStmt );
						return;
					}

					pItem[i]->setTraceItem( bTraceLog( pItem[i] ) );
					pZone->registerObject( pItem[i] );

					if ( pItem[i]->isTraceItem() )
					{
						remainTraceLog( pItem[i], "", pPC->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC );
					}

					if ( !pInventory->addItem( pItem[i], tp[i] ) )
					{
						// 클리어. 롤백
						for ( int j=0; j<i; ++j )
						{
							pInventory->deleteItem( pItem[j]->getObjectID() );
						}

						for ( int j=0; j<5; ++j )
						{
							SAFE_DELETE( pItem[j] );
						}

						GCNPCResponse response;
						response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
						response.setParameter( NOT_ENOUGH_INVENTORY_SPACE );
						pPlayer->sendPacket( &response );

						SAFE_DELETE( pStmt );
						return;
					}
				}

				// DB 에 생성 및 로그 남기고 클라이언트에 보내기
				// 시간 제한 추가
				for ( int i=0; i<5; ++i )
				{
					pItem[i]->create( pPC->getName(), STORAGE_INVENTORY, 0, tp[i].x, tp[i].y );

					filelog( "CombackLog.txt", "[%s:%s] get %s", pGamePlayer->getID().c_str(), pPC->getName().c_str(), pItem[i]->toString().c_str() );

					GCCreateItem gcCI;
					makeGCCreateItem( &gcCI, pItem[i], tp[i].x, tp[i].y );

					pPlayer->sendPacket( &gcCI );
					// 시간제한
					pPC->addTimeLimitItem( pItem[i], 7 * 24 * 60 * 60 );
				}

				pPC->sendTimeLimitItemInfo();

				// 가져갔다고 DB 에 기록한다.
				pStmt->executeQuery( "UPDATE Event200604 SET RecvPremiumItemDate = now() WHERE PlayerID = '%s'", pGamePlayer->getID().c_str() );

				// 클라이언트에 수령했다는 메시지를 보낸다.
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
				response.setParameter( YOU_GET_EVENT_ITEM );
				pPlayer->sendPacket( &response );

				response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
				pPlayer->sendPacket( &response );
			}
			else
			{
				// 이미 아이템을 받아갔다.
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
				response.setParameter( ALEADY_GET_EVENT_ITEM );
				pPlayer->sendPacket( &response );

				SAFE_DELETE( pStmt );
				return;
			}
		}
		else
		{
			// 대상 자가 아니다.
			GCNPCResponse response;
			response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
			response.setParameter( YOU_ARE_NOT_EVENT_TARGET );
			pPlayer->sendPacket( &response );

			SAFE_DELETE( pStmt );
			return;
		}

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )

	__END_CATCH
}

void ActionGiveComebackEventItem200604::executeComebackItem(Creature* pCreature1, Creature* pCreature2)
{
	__BEGIN_TRY
	Assert( pCreature1 != NULL );
	Assert( pCreature2 != NULL );
	Assert( pCreature1->isNPC() );
	Assert( pCreature2->isPC() );

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert( pPC != NULL );
		
	Player* pPlayer = pPC->getPlayer();
	Assert( pPlayer != NULL );

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert( pGamePlayer != NULL );

	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );

	Zone* pZone = pPC->getZone();
	Assert( pZone != NULL );

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getDistConnection( "DARKEDEN" )->createStatement();
		Result* pResult = pStmt->executeQuery( "SELECT RecvItemDate FROM Event200604 WHERE PlayerID = '%s'",
				pGamePlayer->getID().c_str() );

		if ( pResult->next() )
		{
			string  RecvItemDate = pResult->getString(1);

			if ( RecvItemDate == "0000-00-00" )
			{
				// 아이템 주자~
				Creature* pCreature = pGamePlayer->getCreature();
				Assert( pCreature != NULL );

				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
				Assert( pPC != NULL );

				Zone* pZone = pPC->getZone();
				Assert( pZone != NULL );

				Inventory* pInventory = pPC->getInventory();
				Assert( pInventory != NULL );

				list<OptionType_t>  optionType;
				Item* pItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_PET_ITEM, 1, optionType );
				if ( pItem == NULL )
				{
					GCNPCResponse response;
					response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
					response.setParameter( FAIL_GET_EVENT_ITEM );
					pPlayer->sendPacket( &response );

					SAFE_DELETE( pStmt );
					return;
				}

				// 펫 정보 설정
				PetInfo* pPetInfo = new PetInfo;
				pPetInfo->setPetType(1);
				pPetInfo->setPetLevel(15);
				pPetInfo->setPetCreatureType(688);
				pPetInfo->setPetAttr(0xff);
				pPetInfo->setPetExp(231770);
				pPetInfo->setPetAttrLevel(0);
				pPetInfo->setFoodType(0);
				pPetInfo->setGamble(0);
				pPetInfo->setCutHead(0);
				pPetInfo->setPetHP(600);
				pPetInfo->setFeedTime( VSDateTime::currentDateTime() );
				PetItem* pPetItem = dynamic_cast<PetItem*>(pItem);
				Assert( pPetItem != NULL );

				pPetItem->setPetInfo( pPetInfo );
				pPetInfo->setPetItem( pPetItem );

				// 빈공간 확인
				_TPOINT tp;
				if ( !pInventory->getEmptySlot( pItem, tp ) )
				{
					SAFE_DELETE( pItem );

					GCNPCResponse response;
					response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
					response.setParameter( NOT_ENOUGH_INVENTORY_SPACE );
					pPlayer->sendPacket( &response );

					SAFE_DELETE( pStmt );
					return;
				}

				filelog( "CombackLog.txt", "[%s:%s] get %s", pGamePlayer->getID().c_str(), pPC->getName().c_str(), pItem->toString().c_str() );

				pItem->setTraceItem( bTraceLog( pItem ) );
				pZone->registerObject( pItem );

				if ( pItem->isTraceItem() )
				{
					remainTraceLog( pItem, "", pPC->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC );
				}

				if ( !pInventory->addItem( pItem, tp ) )
				{
					SAFE_DELETE( pItem );

					GCNPCResponse response;
					response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
					response.setParameter( NOT_ENOUGH_INVENTORY_SPACE );
					pPlayer->sendPacket( &response );

					SAFE_DELETE( pStmt );
					return;
				}

				pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, tp.x, tp.y );

				GCCreateItem gcCI;
				makeGCCreateItem( &gcCI, pItem, tp.x, tp.y );

				pPlayer->sendPacket( &gcCI );

				// 가져갔다고 DB 에 기록한다.
				pStmt->executeQuery( "UPDATE Event200604 SET RecvItemDate = now() WHERE PlayerID = '%s'", pPlayer->getID().c_str());

				// 클라이언트에 수령했다는 메시지를 보낸다.
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
				response.setParameter( YOU_GET_EVENT_ITEM );
				pPlayer->sendPacket( &response );

				response.setCode( NPC_RESPONSE_QUIT_DIALOGUE );
				pPlayer->sendPacket( &response );
			}
			else
			{
				// 이미 아이템을 받아갔다.
				GCNPCResponse response;
				response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
				response.setParameter( ALEADY_GET_EVENT_ITEM );
				pPlayer->sendPacket( &response );

				SAFE_DELETE( pStmt );
				return;
			}
		}
		else
		{
			// 대상 자가 아니다.
			GCNPCResponse response;
			response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
			response.setParameter( YOU_ARE_NOT_EVENT_TARGET );
			pPlayer->sendPacket( &response );

			SAFE_DELETE( pStmt );
			return;
		}

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionGiveComebackEventItem200604::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionGiveComebackEventItem200604("
		<< "EventType : " << m_EventType
	    << ")";
	return msg.toString();

	__END_CATCH
}
