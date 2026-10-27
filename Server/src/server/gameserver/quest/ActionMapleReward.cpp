////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionMapleReward.cpp
// Written By  : smallheart
// Description : 2008년 가을(10월) 이벤트 - 메이플 문양등록
////////////////////////////////////////////////////////////////////////////////
#include "ActionMapleReward.h"
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
#include "SkillUtil.h"
#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"
#include "EventBlitzRewardManager.h"
#include <list>
#include "PacketUtil.h"
////////////////////////////////////////////////////////////////////////////////
// ActionMapleReward
////////////////////////////////////////////////////////////////////////////////
ActionMapleReward::ActionMapleReward()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionMapleReward::~ActionMapleReward()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionMapleReward::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY
	try 
	{
		m_MapleRewardCount = propertyBuffer.getPropertyInt("MapleRewardCount");
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
void ActionMapleReward::execute(Creature * pCreature1 , Creature * pCreature2)
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

	int MapleCount = 0;
	int MapleRemainCount = 0;


	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery( "SELECT MapleCount, MapleRemainCount FROM MapleEvent WHERE OwnerID = '%s'",
				pPC->getName().c_str() );
		
		
		if(pResult->next())
		{
			MapleCount = pResult->getInt(1);
			MapleRemainCount = pResult->getInt(2);

			if(m_MapleRewardCount <= MapleRemainCount)
			{
				do
				{
					Item*	pResultItem	= NULL;
	
					int LimitTime = 0;
					_TPOINT pt;
					
					switch(m_MapleRewardCount)
					{
					case 1:		// RewardManager는 Blitz의 것을 공용으로 사용한다. - MapleEvent는 GroupID가 4~7
						pResultItem = g_pEventBlitzRewardManager->getEventBlitzReward(pCreature2, 4, LimitTime, pt);
						break;
					case 10:
						pResultItem = g_pEventBlitzRewardManager->getEventBlitzReward(pCreature2, 5, LimitTime, pt);
						break;
					case 50:
						pResultItem = g_pEventBlitzRewardManager->getEventBlitzReward(pCreature2, 6, LimitTime, pt);
						break;
					case 100:
						pResultItem = g_pEventBlitzRewardManager->getEventBlitzReward(pCreature2, 7, LimitTime, pt);
						break;
					default:
						GCNPCResponse response;
						response.setCode( NPC_NOT_ENOUGH_EVENT_ITEM );
						pPlayer->sendPacket( &response );
						break;
					}

					if(pResultItem == NULL)
					{
						GCNPCResponse response;
						response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
						response.setParameter( NOT_ENOUGH_INVENTORY_SPACE );
						pPlayer->sendPacket( &response );
	
						filelog("MapleEvent.txt", "[Name] : %s : Can't Insert item because Not Enough Inventory splace. \n", pCreature2->getName().c_str());
						break;

					}
					
					pZone->registerObject(pResultItem);
					// inventory에 넣는다.
					if(pInventory->addItem(pt.x, pt.y, pResultItem))
					{
						pResultItem->create(pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y);
	
						if( LimitTime > 0)
						{
							pPC->addTimeLimitItem( pResultItem, LimitTime );        
							pPC->sendTimeLimitItemInfo();
						}
	
						GCCreateItem gcCreateItem;
	
						makeGCCreateItem( &gcCreateItem, pResultItem, pt.x, pt.y );
	
						pPlayer->sendPacket(&gcCreateItem);
	
						// ItemTraceLog 를 남긴다
						if ( pResultItem != NULL && pResultItem->isTraceItem() )
						{
							remainTraceLog( pResultItem, "MAPLE EVENT", pCreature2->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
						}
						
						MapleRemainCount -= m_MapleRewardCount;
						pStmt->executeQuery("UPDATE MapleEvent SET MapleRemainCount = %d WHERE OwnerID = '%s'",
								MapleRemainCount, pPC->getName().c_str());
						
						GCNPCResponse response;
						response.setCode( NPC_RESPONSE_BLITZ_REWARD_OK );
						response.setParameter( MAKEDWORD(pResultItem->getItemType() , pResultItem->getItemClass()) );
						pPlayer->sendPacket( &response );
	
					}
					else
					{
						GCNPCResponse response;
						response.setCode( NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG );
						response.setParameter( NOT_ENOUGH_INVENTORY_SPACE );
						pPlayer->sendPacket( &response );
	
						filelog("MapleEvent.txt", "[Name] : %s : Fail addItem to Inventory. Item : %s\n", pCreature2->getName().c_str(), pResultItem->toString().c_str());
					}
				}while(false);

			}
			else
			{
				GCNPCResponse response;
				response.setCode( NPC_NOT_ENOUGH_EVENT_ITEM );
				pPlayer->sendPacket( &response );
			}
		}
		else
		{
			GCNPCResponse response;
			response.setCode( NPC_NOT_ENOUGH_EVENT_ITEM );
			pPlayer->sendPacket( &response );
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionMapleReward::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionMapleReward("
	    << ")";
	return msg.toString();

	__END_CATCH
}
