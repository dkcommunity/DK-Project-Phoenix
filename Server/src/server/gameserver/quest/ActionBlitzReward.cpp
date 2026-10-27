////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionBlitzReward.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionBlitzReward.h"
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
// ActionBlitzReward
////////////////////////////////////////////////////////////////////////////////
ActionBlitzReward::ActionBlitzReward()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionBlitzReward::~ActionBlitzReward()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionBlitzReward::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY
	try 
	{
		m_BlitzRewardCount = propertyBuffer.getPropertyInt("BlitzRewardCount");
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
void ActionBlitzReward::execute(Creature * pCreature1 , Creature * pCreature2)
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

	int BlitzCount = 0;
	int BlitzRemainCount = 0;


	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery( "SELECT BlitzCount, BlitzRemainCount FROM BlitzEvent WHERE OwnerID = '%s'",
				pPC->getName().c_str() );
		
		
		if(pResult->next())
		{
			BlitzCount = pResult->getInt(1);
			BlitzRemainCount = pResult->getInt(2);

			if(m_BlitzRewardCount <= BlitzRemainCount)
			{
				do
				{
					Item*	pResultItem	= NULL;
	
					int LimitTime = 0;
					_TPOINT pt;
					
					switch(m_BlitzRewardCount)
					{
					case 5:
						pResultItem = g_pEventBlitzRewardManager->getEventBlitzReward(pCreature2, 0, LimitTime, pt);
						break;
					case 20:
						pResultItem = g_pEventBlitzRewardManager->getEventBlitzReward(pCreature2, 1, LimitTime, pt);
						break;
					case 50:
						pResultItem = g_pEventBlitzRewardManager->getEventBlitzReward(pCreature2, 2, LimitTime, pt);
						break;
					case 100:
						pResultItem = g_pEventBlitzRewardManager->getEventBlitzReward(pCreature2, 3, LimitTime, pt);
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
	
						filelog("BlitzEvent.txt", "[Name] : %s : 인벤토리공간이 부족해 아이템을 넣을 수 없다. \n", pCreature2->getName().c_str());
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
							remainTraceLog( pResultItem, "BLITZ EVENT", pCreature2->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
						}
						
						BlitzRemainCount -= m_BlitzRewardCount;
						pStmt->executeQuery("UPDATE BlitzEvent SET BlitzRemainCount = %d WHERE OwnerID = '%s'",
								BlitzRemainCount, pPC->getName().c_str());
						
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
	
						filelog("BlitzEvent.txt", "[Name] : %s : 인벤토리에 아이템을 넣을 수 없다. Item : %s\n", pCreature2->getName().c_str(), pResultItem->toString().c_str());
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
string ActionBlitzReward::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionBlitzReward("
	    << ")";
	return msg.toString();

	__END_CATCH
}
