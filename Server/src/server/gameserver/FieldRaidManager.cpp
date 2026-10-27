////////////////////////////////////////////////////////////////////////////////
// Filename    : MasterLairManager.h 
// Written By  : 쉭
// Description : 
////////////////////////////////////////////////////////////////////////////////


#include "FieldRaidManager.h"
#include "MasterLairInfoManager.h"
#include "Assert.h"
#include "Zone.h"
#include "ZoneInfo.h"
#include "ZoneUtil.h"
#include "VariableManager.h"
#include "Timeval.h"
#include "Monster.h"
#include "MonsterManager.h"
#include "PlayerCreature.h"
#include "Inventory.h"
#include "PCManager.h"
#include "Item.h"
#include "ItemFactory.h"
#include "ItemUtil.h"
#include "ItemFactoryManager.h"
#include "EffectKillTimer.h"
#include "EffectMasterLairPass.h"
#include "Effect.h"
#include "EffectContinualGroundAttack.h"
#include "PacketUtil.h"
#include "SkillUtil.h"
#include "Player.h"
#include "MonsterCorpse.h"
#include "ZoneInfoManager.h"
#include "ZoneGroupManager.h"
#include "StringPool.h"
#include "Gpackets/GCNoticeEvent.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCDeleteEffectFromTile.h"
#include "Gpackets/GCSay.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCModifyInformation.h"
#include <stdio.h>
#include "DB.h"
#include "Properties.h"
#include <cmath>
#include <cstdio>

struct OFFERING_TEMPLATE
{
	int	ZoneID;
};

const OFFERING_TEMPLATE RegenZoneID[13] =
{	
	// 드로베타
	{ 31 }, 
	{ 32 },
	{ 33 },
	{ 34 },
	// 로딘산
	{ 41 }, 
	{ 42 },
	{ 43 },
	{ 44 },
	// 티모르 호수
	{ 51 }, 
	{ 52 },
	{ 53 },
	{ 54 },
	// 페로나 국도
	{ 64 },
};

////////////////////////////////////////////////////////////////////////////////
//
// constructor
//
////////////////////////////////////////////////////////////////////////////////
FieldRaidManager::FieldRaidManager (Zone* pZone) 
	throw (Error)
{
	__BEGIN_TRY
		
	Assert(pZone != NULL);

	m_pZone = NULL;

	m_Event = EVENT_WAITING_REGEN;
	m_EventValue = 0;

	Timeval currentTime;
	getCurrentTime(currentTime);

	m_MasterID = 0;
	m_MasterX = 0;
	m_MasterY = 0;

	// 의미없당. - -;
	m_EventTime.tv_sec = currentTime.tv_sec + 120;
	m_EventTime.tv_usec = 0;

	// 1시간에 1번씩 필드 보스 몬스터가 리젠되어진다.
	m_RegenTime.tv_sec = currentTime.tv_sec + 600;
	m_RegenTime.tv_usec = 0;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// destructor
//
////////////////////////////////////////////////////////////////////////////////
FieldRaidManager::~FieldRaidManager () 
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}
////////////////////////////////////////////////////////////////////////////////
//
// heartbeat
// 
////////////////////////////////////////////////////////////////////////////////
bool FieldRaidManager::heartbeat() 
	throw (Error)
{
	__BEGIN_TRY

	switch (m_Event)
	{
		case EVENT_WAITING_REGEN:
			processEventWaitingRegen();
		break;
		case EVENT_WAITING_PLAYER :
			processEventWaitingPlayer();
		break;

		case EVENT_MONSTER_WAR :
			processEventMonsterWar();
		break;

		case EVENT_MINION_COMBAT :
			processEventMinionCombat();
		break;

		default :
			break;
	}; 

	__END_CATCH

	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
// process EventWaitingRegen
//
////////////////////////////////////////////////////////////////////////////////
void FieldRaidManager::processEventWaitingRegen() 
	throw (Error)
{
	__BEGIN_TRY
	Timeval currentTime;
	getCurrentTime(currentTime);

	//    하루에 4번을 이용할 수 있다.
	//   사용자들을 기다린다.
	if (currentTime >= m_RegenTime)
	{
		if (g_pVariableManager->isActiveMasterLair())
		{
			activeEventWaitingPlayer();
		}
		else
		{
			// 아니면 다음 리젠 시간까지 대기한다.
			m_RegenTime.tv_sec += 600;
		}
	}
	__END_CATCH
}
////////////////////////////////////////////////////////////////////////////////
//
// process EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void FieldRaidManager::processEventWaitingPlayer() 
	throw (Error)
{
	__BEGIN_TRY

	Timeval currentTime;
	getCurrentTime(currentTime);

	ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( m_pZone->getZoneID() );
	Assert( pZoneInfo != NULL );

	// 대기 시간이 끝나??.
	// 마스터가 몬스터를 소환하기 하기 시작한다.
	if (currentTime >= m_EventTime)
	{

		char str[80];
		sprintf(str, g_pStringPool->c_str( STRID_FIELD_BOSS_ZONE ), pZoneInfo->getFullName().c_str());

		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setType(SYSTEM_MESSAGE_NORMAL);
		gcSystemMessage.setMessage(str);
		g_pZoneGroupManager->broadcast( &gcSystemMessage);
		// Minion과의 싸움 시작
		activeEventMinionCombat();
	}
	else
	{ 
		int remainSec = m_EventTime.tv_sec - currentTime.tv_sec;

		// 1분 마다 한번씩 알린다.
		if (remainSec!=m_EventValue && remainSec!=0 && remainSec % 60 == 0)
		{
			int remainMin = remainSec/60;

			char str[80];
			sprintf(str, g_pStringPool->c_str( STRID_FIELD_BOSS_TIME ), pZoneInfo->getFullName().c_str(),remainMin);
			// 잠시후 필드 몬스터가 리젠된다고 알려준다.
			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setType(SYSTEM_MESSAGE_NORMAL);
			gcSystemMessage.setMessage(str);

			g_pZoneGroupManager->broadcast( &gcSystemMessage);


			m_EventValue = remainSec;
		}
	}


	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// process EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////

void FieldRaidManager::processEventMonsterWar() 
	throw (Error)
{
	__BEGIN_TRY
	Assert( m_pZone != NULL );

	Timeval currentTime;
	getCurrentTime(currentTime);

	ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( m_pZone->getZoneID() );
	Assert( pZoneInfo != NULL );

	// 필드내에서 울라이리가 리젠되어진다
	// 위치,체력 관련은 지속적으로 실시간으로 알려??예정이다.
	Monster* pMonster = new Monster( 1182 );
	Assert( pMonster != NULL );

	ZoneCoord_t x,y;
	x = 85, y = 36;
	try
	{
		if (!m_pZone->getMonsterManager()->findPosition(pMonster->getMonsterType(), x, y))
		{
			Assert(false);
			return;
		}
		m_pZone->addCreature( pMonster, x, y, Directions(rand() & 0x07) );

		EffectKillTimer* pEffect = new EffectKillTimer( pMonster, true );
		pMonster->setFlag( pEffect->getEffectClass() );
		pEffect->setDeadline(72000);
		pMonster->addEffect( pEffect );

		char str[80];
		sprintf(str, g_pStringPool->c_str( STRID_FIELD_BOSS_ZONE_CREATE ), pZoneInfo->getFullName().c_str());
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setType(SYSTEM_MESSAGE_NORMAL);
		gcSystemMessage.setMessage(str);
		g_pZoneGroupManager->broadcast( &gcSystemMessage);
		m_MasterID = pMonster->getObjectID();
	}
	catch ( EmptyTileNotExistException& )
	{
		SAFE_DELETE( pMonster );
	}

	m_Event = EVENT_MINION_COMBAT;
	__END_CATCH
}


void FieldRaidManager::processEventMinionCombat() 
	throw (Error)
{
	__BEGIN_TRY

	//char str[80];

	Timeval currentTime;
	getCurrentTime(currentTime);

	ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( m_pZone->getZoneID() );
	Assert( pZoneInfo != NULL );

	//activeSayFieldWaring();

	Creature* pMaster = m_pZone->getMonsterManager()->getCreature( m_MasterID );

	if (currentTime >= m_EventTime)
	{
		m_Event = EVENT_WAITING_REGEN;
		activeEventWaitingRegen();
		return;
	}
	// 필드 보스몬스터가 토벌되었다.
	if((pMaster==NULL || pMaster->isDead()))
	{
		if(m_MasterX && m_MasterY)giveKillingReward();
		activeEventWaitingRegen();
		return;
	}else{
		m_MasterX = pMaster->getX();
		m_MasterY = pMaster->getY();
	}

	int remainSec = m_EventTime.tv_sec - currentTime.tv_sec;

		// 1분 마다 한번씩 알린다.
	if (remainSec!=m_EventValue && remainSec!=0 && remainSec % 60 == 0)
	{
	char str[80];	
	sprintf(str, "%s 지역에 %s가 위치하고있습니다.", pZoneInfo->getFullName().c_str(), pMaster->getName().c_str());
	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType(SYSTEM_MESSAGE_NORMAL);
	gcSystemMessage.setMessage(str);
	g_pZoneGroupManager->broadcast( &gcSystemMessage );

	m_EventValue = remainSec;
	}

	//if (currentTime >= m_SystemMessageTime && pMaster!= NULL && pMaster2 != NULL)
	//{
	//m_SystemMessageTime.tv_sec = currentTime.tv_sec + 120;
	//m_SystemMessageTime.tv_usec = 0;

	//}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// active EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void FieldRaidManager::activeEventWaitingPlayer() 
	throw (Error)
{
	__BEGIN_TRY
	// 리젠되어질 위치 구역 지정하기.
	cout << "보스 레이드 시작"<<endl;
	m_pZone = getZoneByZoneID(64);
	// 10분 대기 시간
	Timeval currentTime;
	getCurrentTime(currentTime);
	m_EventTime.tv_sec = currentTime.tv_sec + 120;
	m_EventTime.tv_usec = currentTime.tv_usec;
	m_EventValue = 0;
	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType(SYSTEM_MESSAGE_NORMAL);
	gcSystemMessage.setMessage(g_pStringPool->c_str( STRID_FIELD_BOSS_REGEN ));
	g_pZoneGroupManager->broadcast( &gcSystemMessage);

	m_Event = EVENT_WAITING_PLAYER;
	cout << "보스 레이드 끝"<<endl;

	__END_CATCH
}
////////////////////////////////////////////////////////////////////////////////
//
// active EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void FieldRaidManager::activeEventMinionCombat() 
	throw (Error)
{
	__BEGIN_TRY

	m_Event = EVENT_MONSTER_WAR;
	m_EventValue = 0;

	// 언제까??싸울까?
	getCurrentTime( m_EventTime );
	m_EventTime.tv_sec += 600;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// active EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void FieldRaidManager::activeEventWaitingRegen() 
	throw (Error)
{
	__BEGIN_TRY

	deleteAllMonsters();

	m_Event = EVENT_WAITING_REGEN;
	m_EventValue = 0;

	__END_CATCH
}
////////////////////////////////////////////////////////////////////////////////
//
// delete All Monsters
//
////////////////////////////////////////////////////////////////////////////////
void FieldRaidManager::deleteAllMonsters()
	throw (Error)
{
	__BEGIN_TRY

	m_MasterID = 0;
	m_MasterX = 0;
	m_MasterY = 0;

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType(SYSTEM_MESSAGE_FIELD);
	gcSystemMessage.setMessage( "필드 보스몬스터가 토벌되었습니다."  );
	g_pZoneGroupManager->broadcast( &gcSystemMessage );

	Timeval currentTime;
	getCurrentTime(currentTime);

	m_RegenTime.tv_sec = currentTime.tv_sec + 600;
	m_RegenTime.tv_usec = currentTime.tv_usec;

	__END_CATCH
}
////////////////////////////////////////////////////////////////////////////////
//
// give Killing Reward
//
////////////////////////////////////////////////////////////////////////////////
// 마스터가 죽었을때의 보상
// 지금은 QuestItem을 현재 존의 사람들에게 각자의 인벤토리에 넣어준다.
// 인벤토리에 자리가 없는 경우엔 바닥에 떨어뜨리는데
// 이미 가지고 있는 사람은 주울 수 없다.
////////////////////////////////////////////////////////////////////////////////
void FieldRaidManager::giveKillingReward() 
	throw (Error)
{
	__BEGIN_TRY

	const PCManager* pPCManager = m_pZone->getPCManager();
	const map< ObjectID_t, Creature* > & creatures = pPCManager->getCreatures();
	map< ObjectID_t, Creature* >::const_iterator itr;

	if (creatures.empty())
		return;

	int CoreZapNum = 6;

	ItemType_t RuneBoxType = Random(40,43);
	for (itr=creatures.begin(); itr!=creatures.end(); itr++)
	{
		Creature* pCreature = itr->second;

		if (pCreature->isPC())
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
			Inventory* pInventory = pPC->getInventory();

			//------------------------------------------------------------
			// 계급 경험치를 올려준다.
			//------------------------------------------------------------
			// 마스터 위치와 7타일 이내에 이는 경우
			//
			if (pPC->getDistance(m_MasterX, m_MasterY) <= 7 && CoreZapNum)
			{
				int Random_item = Random(1,100);
				if(Random_item >= 1 && Random_item <= 40)RuneBoxType = 40;
				if(Random_item >= 41 && Random_item <= 70)RuneBoxType = 41;
				if(Random_item >= 71 && Random_item <= 90)RuneBoxType = 42;
				if(Random_item >= 91 && Random_item <= 100)RuneBoxType = 43;
				pPC->increaseRankExp( MASTER_KILL_RANK_EXP );
				
				list<OptionType_t> nullList;
				Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_GIFT_BOX, RuneBoxType, nullList);
 	
				(m_pZone->getObjectRegistry()).registerObject(pItem);

				// 인벤토리의 빈 곳을 찾는다.
				_TPOINT p;
				if (pInventory->getEmptySlot(pItem, p))
				{
				// 인벤토리에 추가한다.
				pInventory->addItem(p.x, p.y, pItem);

	            			pItem->create(pCreature->getName(), STORAGE_INVENTORY, 0, p.x, p.y);

				// ItemTrace 에 Log 를 남긴다
				if ( pItem != NULL && pItem->isTraceItem() )
				{
					remainTraceLog( pItem, "FieldRaid", pCreature->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
					remainTraceLogNew( pItem, pCreature->getName(), ITL_GET, ITLD_EVENTNPC, m_pZone->getZoneID() );
				}

				// 인벤토리에 아이템 생성 패킷을 보내준다.
					GCCreateItem gcCreateItem;
					makeGCCreateItem( &gcCreateItem, pItem, p.x, p.y );
					pCreature->getPlayer()->sendPacket(&gcCreateItem);
				}
				else
				{
					// 인벤토리에 자리가 없어서 바닥에 떨어뜨린다.

					TPOINT p = m_pZone->addItem(pItem, pCreature->getX(), pCreature->getY());
					if (p.x != -1)
					{
						pItem->create("", STORAGE_ZONE, m_pZone->getZoneID(), p.x, p.y );

						// ItemTrace 에 Log 를 남긴다
						if ( pItem != NULL && pItem->isTraceItem() )
						{
							char zoneName[15];
							sprintf( zoneName , "%4d%3d%3d", m_pZone->getZoneID(), p.x, p.y);
							remainTraceLog( pItem, "FieldRaid", zoneName, ITEM_LOG_CREATE, DETAIL_EVENTNPC);
							remainTraceLogNew( pItem, zoneName, ITL_GET , ITLD_EVENTNPC, m_pZone->getZoneID(), p.x, p.y);
						}
					}
					else
					{
						SAFE_DELETE(pItem);
					}
				}
				CoreZapNum--;
			}
		}
		else
		{
			throw Error("PCManager??PC아닌게 들어있노 -_-");
		}
	}

	__END_CATCH
}
////////////////////////////////////////////////////////////////////////////////
//
// start Event
//
////////////////////////////////////////////////////////////////////////////////
void FieldRaidManager::startEvent()
	throw (Error)
{
	__BEGIN_TRY

	activeEventWaitingPlayer();

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// start Event
//
////////////////////////////////////////////////////////////////////////////////
void FieldRaidManager::stopEvent()
	throw (Error)
{
	__BEGIN_TRY

	activeEventWaitingRegen();

	__END_CATCH
}

string FieldRaidManager::toString() const 
	throw(Error)
{
	StringStream msg;

	int eventSec = m_EventTime.tv_sec;

	Timeval currentTime;
	getCurrentTime(currentTime);

	return msg.toString();
}
