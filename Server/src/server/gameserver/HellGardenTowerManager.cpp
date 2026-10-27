////////////////////////////////////////////////////////////////////////////////
// Filename    : HellGardenTowerManager.h 
// Written By  : 쉭
// Description : 
////////////////////////////////////////////////////////////////////////////////


#include "HellGardenTowerManager.h"
#include "Assert.h"
#include "Zone.h"
#include "VariableManager.h"
#include "Timeval.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "MonsterManager.h"
#include "PlayerCreature.h"
#include "Inventory.h"
#include "PCManager.h"
#include "Item.h"
#include "ItemUtil.h"
#include "ItemFactoryManager.h"
#include "ItemInfoManager.h"
#include "PacketUtil.h"
#include "Player.h"
#include "MonsterCorpse.h"
#include "ZoneInfoManager.h"
#include "ZoneGroupManager.h"
#include "VSDateTime.h"
#include "StringPool.h"
#include "EventTransport.h"
#include "GamePlayer.h"
#include "MasterLairInfoManager.h"
#include "ItemGradeManager.h"
// 몬스터 리스트 로딩 땜시
#include "DB.h"

#include "Gpackets/GCNoticeEvent.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCDeleteObject.h"

#include <stdio.h>

// 최대 탑에 들어갈수 있는 플레이어 숫자
const int MaxPassPlayer = 6;
const int HellGardenEnterWaitTime = 120; // 헬가든 입장 제한시간 2분
const int HellGardenCombatWaitTime = 360; // 헬가든 전투 제한시간 6분
const int TimeOutMessageTime = 60; // 1분에 한번씩 제한시간 표시 메시지를 뿌린다.
/*
const SpriteType_t SpriteTypeTantacle = 255;
const MonsterType_t MonsterTypeTantacle = 809;
const MonsterType_t MonsterTypeMikllizz = 808;
const MonsterType_t MonsterTypeReward = 808;

const int NextOpenHour[24] =
{
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
	1, 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2
};

const int ClosingMessageMinuteMax = 5;
const int ClosingMessageMinute[ClosingMessageMinuteMax] =
{ 15, 10, 5, 1, 0 };

const int ResurrectMikllizzMessageMinuteMax = 8;
const int ResurrectMikllizzMessageMinute[ResurrectMikllizzMessageMinuteMax] =
{ 60, 50, 40, 30, 20, 10, 5, 0 };
*/

////////////////////////////////////////////////////////////////////////////////
//
// constructor
//
////////////////////////////////////////////////////////////////////////////////
HellGardenTowerManager::HellGardenTowerManager (Zone* pZone) 
	throw (Error)
{
	__BEGIN_TRY
		
	Assert(pZone != NULL);
	m_pZone = pZone;

	m_nMaxPassPlayer = MaxPassPlayer;
//	m_nMaxPassPlayer = 20;
	m_nPassPlayer = 0;

	m_State = STATE_OPEN;

	Timeval currentTime;
	getCurrentTime(currentTime);

	m_StateTime.tv_sec = currentTime.tv_sec;
	m_StateTime.tv_usec = 0;

	m_TimeOutMessageNextTime.tv_sec = currentTime.tv_sec;
	m_TimeOutMessageNextTime.tv_usec = 0;


	// 여기부터 맹든다 

	if ( m_pZone->getZoneID() >= 5001 && m_pZone->getZoneID() <= 5004 )
	{
		// 헬가든 1층 셋팅
		m_HellGardenType = TYPE_HELLGARDEN_1;
		m_HellGardenTowerType = m_pZone->getZoneID() % 5000;
		m_KickZoneID = 5000;
	}
	else if ( m_pZone->getZoneID() >= 5011 && m_pZone->getZoneID() <= 5014 )
	{
		// 헬가든 2층 셋팅
		m_HellGardenType = TYPE_HELLGARDEN_2;
		m_HellGardenTowerType = m_pZone->getZoneID() % 5010;
		m_KickZoneID = 5010;
	}
	else if( m_pZone->getZoneID() >= 5021 && m_pZone->getZoneID() <= 5024 )
	{
		// 헬가든 3층 셋팅
		m_HellGardenType = TYPE_HELLGARDEN_3;
		m_HellGardenTowerType = m_pZone->getZoneID() % 5020;
		m_KickZoneID = 5020;
	}
	else if( m_pZone->getZoneID() >= 5031 && m_pZone->getZoneID() <= 5034 )
	{
		// 헬가든 4층 셋팅
		m_HellGardenType = TYPE_HELLGARDEN_4;
		m_HellGardenTowerType = m_pZone->getZoneID() % 5030;
		m_KickZoneID = 5030;
	}
	else if( m_pZone->getZoneID() >= 5041 && m_pZone->getZoneID() <= 5044 )
	{
		// 헬가든 5층 셋팅
		m_HellGardenType = TYPE_HELLGARDEN_5;
		m_HellGardenTowerType = m_pZone->getZoneID() % 5040;
		m_KickZoneID = 5040;
	}

	// 헬가든 타워이면 HellGardenTowerMonsterList 테이블에서 가져온다.
	MonsterListLoad();

	m_SummonStep = 0;
	m_PartyID = 0;
	m_StartCombat = false;
	m_Mutex.setName("HellGardenTowerManager");

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// destructor
//
////////////////////////////////////////////////////////////////////////////////
HellGardenTowerManager::~HellGardenTowerManager () 
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}
	
// 헬가든 초기화
void HellGardenTowerManager::initHellGarden()
{
	__BEGIN_TRY

	m_nPassPlayer = 0;

	m_State = STATE_OPEN;

	m_PartyID = 0;

	m_StartCombat = false;

	if(m_pZone)
		m_pZone->killAllMonsters_UNLOCK();

	__END_CATCH

}
////////////////////////////////////////////////////////////////////////////////
//
// enterCreature ( Creature* )
//
////////////////////////////////////////////////////////////////////////////////
//
// Creature가 이 Zone(HellGardenTower)에 들어올 수 있는지 체크하고
// 들어올 수 있다면 들어왔다고 보고 체크해둔다.
//
// [조건]
//   - STATE_TANTACLE_COMBAT
//   - m_nPassPlayer < m_nMaxPassPlayer
//   - 봉인석을 가지고 있는경우
//   - 100 레벨 이상
//
////////////////////////////////////////////////////////////////////////////////
bool HellGardenTowerManager::enterPC(PlayerCreature* pPC )
	throw(Error)
{	
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	if (pPC->isDM() || pPC->isGOD())
	{
		m_Mutex.unlock();

		filelog("HellGardenTower.log", "enterPC - DM or GOD pass! - %s", pPC->getName().c_str() );
		return true;
	}
	// 허용된 인원이 다 찻다. 못들어간다.
	else if( m_nPassPlayer >= MaxPassPlayer )
	{
		m_Mutex.unlock();
		return false;
	}

	if ( m_State == STATE_OPEN )
	{
		// 파티의 유무 확인
		if(pPC->getPartyID() == 0)
		{
			m_Mutex.unlock();
			return false;
		}

		// key 확인 ( m_HellGardenType 으로 구분 )
		Inventory* pInventory = pPC->getInventory();
		Assert( pInventory != NULL );
		if ( !pInventory->hasEnoughNumItem( Item::ITEM_CLASS_QUEST_ITEM, m_HellGardenType + 11, 1 ) )
		{
			m_Mutex.unlock();

			filelog("HellGardenTower.log", "enterPC - no key. deny! - %s", pPC->getName().c_str() );
			return false;
		}
		else
			pPC->decreaseItemClassTypeNum( Item::ITEM_CLASS_QUEST_ITEM, m_HellGardenType + 11, 1 );

		// Party ID 저장
		m_PartyID = pPC->getPartyID();
	
		// m_State 값을 STATE_WAIT으로 변경 // 2분

		m_State = STATE_WAIT;

		broadcastClosingMessage();
		
		Timeval currentTime;
		getCurrentTime(currentTime);

		m_StateTime.tv_sec = currentTime.tv_sec + HellGardenEnterWaitTime;

		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(m_pZone->getZoneID());

		char message[128];
		//sprintf(message, g_pStringPool->c_str(STRID_2MINUTE_LATER_APPEAR_HELLGARDEN_MONSTER), pZoneInfo->getShortName().c_str()); // 20070814
		sprintf(message, g_pStringPool->c_str(STRID_2MINUTE_LATER_APPEAR_HELLGARDEN_MONSTER), pZoneInfo->getEName().c_str()); // by viva

		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setType( SYSTEM_MESSAGE_MASTER_LAIR );
		gcSystemMessage.setMessage(message);
		m_pZone->broadcastPacket( &gcSystemMessage );

		// m_nPassPlayer 의 숫자를 눌려준다
		m_nPassPlayer = 1;

	}
	else if( m_State == STATE_WAIT )
	{
		// 여기에 먼저 들어온 사람의 파티원인지 확인한다
		if(pPC->getPartyID() != m_PartyID)
		{
			m_Mutex.unlock();
			return false;
		}
		// m_nPassPlayer 의 숫자를 눌려준다
		m_nPassPlayer++;
		if( m_nPassPlayer >= MaxPassPlayer )
		{
			m_State = STATE_STAGE_1;
		}
	}
	else
	{
		m_Mutex.unlock();
		return false;
	}

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// leaveCreature ( Creature* )
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool HellGardenTowerManager::leaveCreature(Creature* pCreature)
	throw(Error)
{	
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	if (pCreature->isDM() || pCreature->isGOD())
	{
		m_Mutex.unlock();
		return true;
	}

	--m_nPassPlayer;

	filelog("HellGardenTower.log", "leavePC - pass! - PassPlayerNum : %d", m_nPassPlayer );

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH

	return true;
}


////////////////////////////////////////////////////////////////////////////////
//
// heartbeat
// 
////////////////////////////////////////////////////////////////////////////////
bool HellGardenTowerManager::heartbeat() 
	throw (Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	switch (m_State)
	{
		case STATE_WAIT:
			// 2분이라는 타이머의 시간을 체크하여준다. 시간이 지났다면 STATE를 STATE_STAGE_1으로 변경해준다
			// 스테이트를 변경해주면서 시간을 셋팅해준다 6분 
			{
				Timeval 	currentTime;

				getCurrentTime(currentTime);

				if( m_StateTime.tv_sec > currentTime.tv_sec )
				{
					//		cout << "SummonNextTime = " << m_TimeOutMessageNextTime.tv_sec << " CurrentTime = " << currentTime.tv_sec << endl;
					break;
				}

				m_State = STATE_STAGE_1;
				
			}
		break;
		
		case STATE_STAGE_1:
		case STATE_STAGE_2:
		case STATE_STAGE_3:
		case STATE_STAGE_4:
		case STATE_STAGE_5:
		case STATE_STAGE_6:
		case STATE_STAGE_7:
		case STATE_STAGE_8:
		case STATE_STAGE_9:
		case STATE_STAGE_10:
			processHellGardenTowerState();
		break;

		default :
			break;
	}; 

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH

	return true;
}


////////////////////////////////////////////////////////////////////////////////
//
// process HellGarden
//
////////////////////////////////////////////////////////////////////////////////
void HellGardenTowerManager::processHellGardenTowerState()
throw (Error)
{
	__BEGIN_TRY

	Timeval currentTime;
	getCurrentTime(currentTime);
	if(!m_StartCombat)
	{
		if(currentTime.tv_sec < m_StateTime.tv_sec)
		{
			return;
		}
		// 존 중앙으로 옮기고
		if( m_State != STATE_STAGE_1 )
		{
			m_pZone->getPCManager()->transportAllCreatures( m_pZone->getZoneID(), 21, 23, defaultRaceValue, 0 );
		}
		
		// 몬스터 소환한다.
//		cout << "Summon Monster" << endl;
		summonMonster();
		// 전투시간을 셋팅한다.
		m_StateTime.tv_sec = currentTime.tv_sec + HellGardenCombatWaitTime;
		m_TimeOutMessageNextTime.tv_sec = currentTime.tv_sec + TimeOutMessageTime;
		m_StartCombat = true;
		char message[128];
		sprintf(message, g_pStringPool->c_str(STRID_HELLGARDEN_TOWER_LIMITED_TIME), m_State - 1, 3600); // 20070814
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setType( SYSTEM_MESSAGE_TIMER );
		gcSystemMessage.setMessage(message);
//		__ENTER_CRITICAL_SECTION( (*(m_pZone->getZoneGroup())) )
		m_pZone->broadcastPacket( &gcSystemMessage );
//		__LEAVE_CRITICAL_SECTION( (*(m_pZone->getZoneGroup())) )
		return;
	}

//	int CurrentMonster = m_pZone->getMonsterManager()->getMonsters();
	int CurrentMonster = m_pZone->getMonsterManager()->getSize();


	// 몬스터가 다 죽었으면
	if ( CurrentMonster == 0 )
	{
//		cout << "CurrentMonsts 0 giveKillingReward" << endl;
		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(m_pZone->getZoneID());
		m_State++;
		// 보상을 주고
		giveKillingReward();
		if(m_State == STATE_INIT)
		{
			char message[128];
			sprintf(message, g_pStringPool->c_str(STRID_HELLGARDEN_TOWER_LIMITED_TIME), m_State - 2, 0); // 10층 완료 후 0초 셋팅하여 클라에게 보낸다.
			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setType( SYSTEM_MESSAGE_TIMER );
			gcSystemMessage.setMessage(message);
	//		__ENTER_CRITICAL_SECTION( (*(m_pZone->getZoneGroup())) )
			m_pZone->broadcastPacket( &gcSystemMessage );
	//		__LEAVE_CRITICAL_SECTION( (*(m_pZone->getZoneGroup())) )
			// 모두 쫓아낸다.
//			cout << "CurrentState STATE_INIT" << endl;
			kickOutAllPC();
			// 헬가든 초기화
			initHellGarden();

			char msg[128];
			//sprintf( msg, g_pStringPool->c_str(STRID_CAN_ENTER_HELLGARDENTOWER), pZoneInfo->getShortName().c_str()); // 20070814
			sprintf( msg, g_pStringPool->c_str(STRID_CAN_ENTER_HELLGARDENTOWER), pZoneInfo->getEName().c_str()); // by viva
	
			gcSystemMessage.setType( SYSTEM_MESSAGE_MASTER_LAIR );
			gcSystemMessage.setMessage( msg );
			g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
			return;
		}
			
//		cout << "Wait 10 second" << endl;
		// 10초간 기다린다.
		m_StateTime.tv_sec = currentTime.tv_sec + 10;
		m_StartCombat = false;
		char message[128];
		//sprintf(message, g_pStringPool->c_str(STRID_AFTER_10SECOND_MOVE_HELLGARDENTOWER_FLOWER), pZoneInfo->getShortName().c_str() + 9, m_State - 1); // 푸하하 9가 뭘까요? ㅋㅋ
		sprintf(message, g_pStringPool->c_str(STRID_AFTER_10SECOND_MOVE_HELLGARDENTOWER_FLOWER), pZoneInfo->getEName().c_str(), m_State - 1); // by viva
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setType( SYSTEM_MESSAGE_MASTER_LAIR );
		gcSystemMessage.setMessage(message);
//		__ENTER_CRITICAL_SECTION( (*(m_pZone->getZoneGroup())) )
		m_pZone->broadcastPacket( &gcSystemMessage );
//		__LEAVE_CRITICAL_SECTION( (*(m_pZone->getZoneGroup())) )
		return;

	}
	
	if (currentTime.tv_sec >= m_StateTime.tv_sec)
	{
//		cout << "HellGarden Time Out KickOut All PC" << endl;
		// 시간 지났다.
		// 모두 쫓아낸다.
		kickOutAllPC();
		// 헬가든 초기화
		initHellGarden();
		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(m_pZone->getZoneID());
		char msg[128];
		//sprintf( msg, g_pStringPool->c_str(STRID_CAN_ENTER_HELLGARDENTOWER), pZoneInfo->getShortName().c_str());
		sprintf( msg, g_pStringPool->c_str(STRID_CAN_ENTER_HELLGARDENTOWER), pZoneInfo->getEName().c_str());//by viva
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setType( SYSTEM_MESSAGE_MASTER_LAIR );
		gcSystemMessage.setMessage( msg );
		g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );

		return;
	}

	//// 플레이어들이 다 죽은 경우
	//if (m_pZone->getPCManager()->getSize()==0)
	//{
	//	cout << "HellGarden All PC Dead" << endl;
	//	// 헬가든 초기화
	//	initHellGarden();
	//}		


	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// summon Tantacle
////////////////////////////////////////////////////////////////////////////////
void HellGardenTowerManager::summonMonster()
	throw (Error)
{
	__BEGIN_TRY

	//
	for ( int i=0; i< m_MonterList[m_State].MonsterCount; ++i )
	{
		// 존의 빈자리를 찾아낸다.
		ZoneCoord_t x,y;
		if ( !m_pZone->getMonsterManager()->findPosition( m_MonterList[m_State].MonsterType, x, y ) )
		{
			Assert(false);
			return;
		}

		Monster* pMonster = new Monster(m_MonterList[m_State].MonsterType);
		Assert( pMonster != NULL );

		try
		{
			pMonster->setDir( Directions(rand()%8) );
			m_pZone->addCreature( pMonster, x, y, pMonster->getDir() );
		}
		catch ( EmptyTileNotExistException& )
		{
			SAFE_DELETE( pMonster );
		}
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// kick out all PC
////////////////////////////////////////////////////////////////////////////////
void HellGardenTowerManager::kickOutAllPC()
	throw (Error)
{
	__BEGIN_TRY

	m_pZone->getPCManager()->transportAllCreatures( m_KickZoneID, 88, 110, defaultRaceValue, 0 );

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// set State Close
////////////////////////////////////////////////////////////////////////////////
void HellGardenTowerManager::setStateClose()
	throw (Error)
{
	__BEGIN_TRY
/*
	VSDateTime now, next;
	VSTime nextTime;

	Timeval currentTime;
	getCurrentTime(currentTime);

	now.setTime_t( currentTime.tv_sec );
	next = now.addSecs( NextOpenHour[now.time().hour()]*60*60 );
	nextTime = VSTime( next.time().hour(), 0, 0 );
	next.setTime( nextTime );

	m_StateTime.tv_sec = currentTime.tv_sec + now.secsTo( next );

	filelog("Mikllizz.log", "State Close : reamin %d minutes", now.secsTo( next ) / 60 );

	m_State = STATE_OPEN;
*/
	__END_CATCH
}

bool HellGardenTowerManager::isSummonTiming()
	throw (Error)
{
	Turn_t		Duration = 300;
	Timeval 	currentTime;

	getCurrentTime(currentTime);

	if( m_TimeOutMessageNextTime.tv_sec > currentTime.tv_sec )
	{
//		cout << "SummonNextTime = " << m_TimeOutMessageNextTime.tv_sec << " CurrentTime = " << currentTime.tv_sec << endl;
		return false;
	}

	m_TimeOutMessageNextTime.tv_sec = currentTime.tv_sec + Duration;

	return true;

}

////////////////////////////////////////////////////////////////////////////////
//
// give Killing Reward
//
////////////////////////////////////////////////////////////////////////////////
// 각 층별(STATE) 보상 및 모든 층을 클리어 했을 경우 보상
// 이미 가지고 있는 사람은 주울 수 없다.
////////////////////////////////////////////////////////////////////////////////
void HellGardenTowerManager::giveKillingReward() 
	throw (Error)
{
	__BEGIN_TRY

	const PCManager* pPCManager = m_pZone->getPCManager();
	const map< ObjectID_t, Creature* > & creatures = pPCManager->getCreatures();
	map< ObjectID_t, Creature* >::const_iterator itr;

	if(m_State == STATE_INIT)
	{
		// 10층을 깨면 반역의 십자가 및 프롤로그 퀘스트 아이템을 준다.
		// 공간이 없으면 없다고 알리고 안준다.
		ItemType_t CrossOfDefiance = m_HellGardenType + 11; // itemtype 11번이 반역의 십자가다. 
		
//		ItemType_t QuestItemType = 0; // 각 종족별 퀘스트 아이템
		Grade_t grade;
		int i;
		for (i=0, itr=creatures.begin(); itr!=creatures.end(); i++, itr++)
		{
			Creature* pCreature = itr->second;

			if (pCreature->isPC())
			{
				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
				Inventory* pInventory = pPC->getInventory();
				do
				{
					Item* pItem = NULL;

					// 반역의 십자가 넣어준다.
					list<OptionType_t> nullList;
					pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_MOON_CARD, CrossOfDefiance, nullList);
					pItem->setGrade(grade);

					// 20080424 헬가든 십자가 보상 더 주기 이벤트
					if(g_pVariableManager->getVariable(HELLGARDEN_CROSS_EVENT))
					{
						pItem->setNum( g_pVariableManager->getVariable(HELLGARDEN_CROSS_NUM) );
					}

					ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(Item::ITEM_CLASS_MOON_CARD, CrossOfDefiance);
					// 인벤토리의 같은 아이템이 있는지 찾는다.
					_TPOINT pt;
					if( !pInventory->findAddStackItem( pItem, pt ) )
					{
						// 인벤토리에 넣어 줄 아이템의 공간이 있는지 체크 한다.
						if( !pInventory->getEmptySlot( pItem, pt) )
						{
							char msg[128];
							sprintf(msg, g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE_NOT_GET_ITEM), pItemInfo->getName().c_str()); // 20070814
							GCSystemMessage gcSystemMessage;
							gcSystemMessage.setMessage(msg);
							pPC->getPlayer()->sendPacket(&gcSystemMessage);
							break;
						}


						(m_pZone->getObjectRegistry()).registerObject(pItem);

						if( pInventory->addItem( pItem, pt ) )
						{
							pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
							filelog( "HellGardenTower.log", "%s가 %s를 받았습니다.", pPC->getName().c_str(), pItemInfo->getName().c_str());

							remainTraceLog( pItem, "Leather", pPC->getName(), ITEM_LOG_CREATE, DETAIL_PICKUP );
					
						}
						else
						{
							pInventory->deleteItem( pItem->getObjectID() );
							char msg[128];
							sprintf(msg, g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE_NOT_GET_ITEM), pItemInfo->getName().c_str());
							GCSystemMessage gcSystemMessage;
							gcSystemMessage.setMessage(msg);
							pPC->getPlayer()->sendPacket(&gcSystemMessage);
							break;
						}

					}
					else
					{
						pItem = pInventory->findItem( pItem->getItemClass(), pItem->getItemType(), (CoordInven_t&)pt.x, (CoordInven_t&)pt.y );
						Assert(pItem != NULL);
						if(g_pVariableManager->getVariable(HELLGARDEN_CROSS_EVENT))
							pItem->setNum( pItem->getNum() + g_pVariableManager->getVariable(HELLGARDEN_CROSS_NUM) );
						else						
							pItem->setNum( pItem->getNum() + 1 );

						pItem->save( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
						filelog( "HellGardenTower.log", "%s가 %s를 받았습니다.", pPC->getName().c_str(), pItemInfo->getName().c_str());
					}

				}while(false);

				do
				{
					Item* pItem = NULL;

					list<OptionType_t> nullList;

					// 1층이 30% 층이 올라갈수록 10%씩 확률이 올라간다.
					int value = rand()%(100);
					if(value < (30 + (m_HellGardenType*10)))
					{
						// 각 종족별 퀘스트 아이템을 넣어준다.
						if(pCreature->isSlayer())
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_COMMON_QUEST_ITEM, 0, nullList);
						else if(pCreature->isVampire())
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_COMMON_QUEST_ITEM, 1, nullList);
						else if(pCreature->isOusters())
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_COMMON_QUEST_ITEM, 2, nullList);

						pItem->setGrade(grade);

						(m_pZone->getObjectRegistry()).registerObject(pItem);

						ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(Item::ITEM_CLASS_COMMON_QUEST_ITEM, pItem->getItemType());
						// 인벤토리의 같은 아이템이 있는지 찾는다.
						_TPOINT pt;
						if( !pInventory->findAddStackItem( pItem, pt ) )
						{
							// 인벤토리에 넣어 줄 아이템의 공간이 있는지 체크 한다.
							if( !pInventory->getEmptySlot( pItem, pt) )
							{
								char msg[128];
								sprintf(msg, g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE_NOT_GET_ITEM), pItemInfo->getName().c_str());
								GCSystemMessage gcSystemMessage;
								gcSystemMessage.setMessage(msg);
								//Player* pPlayer = dynamic_cast<Player*>(pPC);
								//pPlayer->sendPacket(&gcSystemMessage);
								pPC->getPlayer()->sendPacket(&gcSystemMessage);
								break;
							}


							(m_pZone->getObjectRegistry()).registerObject(pItem);

							if( pInventory->addItem( pItem, pt ) )
							{
								filelog( "HellGardenTower.log", "%s 가 %s을 받았습니다.", pPC->getName().c_str(), pItemInfo->getName().c_str());

								pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

								remainTraceLog( pItem, "Leather", pPC->getName(), ITEM_LOG_CREATE, DETAIL_PICKUP );

							}
							else
							{
								pInventory->deleteItem( pItem->getObjectID() );
								char msg[128];
								sprintf(msg, g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE_NOT_GET_ITEM), pItemInfo->getName().c_str());
								GCSystemMessage gcSystemMessage;
								gcSystemMessage.setMessage(msg);
								pPC->getPlayer()->sendPacket(&gcSystemMessage);
								break;
							}

						}
						else
						{
							pItem = pInventory->findItem( pItem->getItemClass(), pItem->getItemType(), (CoordInven_t&)pt.x, (CoordInven_t&)pt.y );
							Assert(pItem != NULL);

							pItem->setNum( pItem->getNum() + 1 );

							pItem->save( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
							filelog( "HellGardenTower.log", "%s 가 %s을 받았습니다.", pPC->getName().c_str(), pItemInfo->getName().c_str());
						}


					}

				}while(false);


//				20080422 2차 레어스킬북 아이템 추가
				if(g_pVariableManager->getVariable(HELLGARDEN_SKILL_BOOK_RATIO) > 0) // 0이면 안떨어진다. 
				{
				do
				{
					Item* pItem = NULL;

					int value = rand()%(g_pVariableManager->getVariable(HELLGARDEN_SKILL_BOOK_RATIO));
					list<OptionType_t> nullList;

					if(pCreature->isSlayer())
					{
						if((0 <= value) && (value < 5))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, m_HellGardenType + 31, nullList);
						}
						else if((5 <= value) && (value < 9))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 20, nullList);
						}
						else if((9 <= value) && (value < 13))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 21, nullList);
						}
						else if((13 <= value) && (value < 17))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 22, nullList);
						}
						else if((17 <= value) && (value < 21))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 23, nullList);
						}
						else if((21 <= value) && (value < 25))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 24, nullList);
						}
					}
					else if(pCreature->isVampire())
					{
						if((0 <= value) && (value < 5))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, m_HellGardenType + 31, nullList);
						}
						else if((5 <= value) && (value < 15))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 25, nullList);
						}
						else if((15 <= value) && (value < 25))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 26, nullList);
						}
					}
					else if(pCreature->isOusters())
					{
						if((0 <= value) && (value < 5))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, m_HellGardenType + 31, nullList);
						}
						else if((5 <= value) && (value < 10))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 27, nullList);
						}
						else if((10 <= value) && (value < 15))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 28, nullList);
						}
						else if((15 <= value) && (value < 20))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 29, nullList);
						}
						else if((20 <= value) && (value < 25))
						{
							pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKILL_BOOK, 30, nullList);
						}
					}

					if(pItem != NULL)
					{
			
						pItem->setGrade(grade);

						(m_pZone->getObjectRegistry()).registerObject(pItem);

						ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(Item::ITEM_CLASS_SKILL_BOOK, pItem->getItemType());
						// 인벤토리의 같은 아이템이 있는지 찾는다.
						_TPOINT pt;
						if( !pInventory->findAddStackItem( pItem, pt ) )
						{
							// 인벤토리에 넣어 줄 아이템의 공간이 있는지 체크 한다.
							if( !pInventory->getEmptySlot( pItem, pt) )
							{
								char msg[128];
								sprintf(msg, g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE_NOT_GET_ITEM), pItemInfo->getName().c_str());
								GCSystemMessage gcSystemMessage;
								gcSystemMessage.setMessage(msg);
								//Player* pPlayer = dynamic_cast<Player*>(pPC);
								//pPlayer->sendPacket(&gcSystemMessage);
								pPC->getPlayer()->sendPacket(&gcSystemMessage);
								break;
							}


							(m_pZone->getObjectRegistry()).registerObject(pItem);

							if( pInventory->addItem( pItem, pt ) )
							{
								filelog( "HellGardenTower.log", "%s 가 %s을 받았습니다.", pPC->getName().c_str(), pItemInfo->getName().c_str());

								pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

								remainTraceLog( pItem, "Leather", pPC->getName(), ITEM_LOG_CREATE, DETAIL_PICKUP );
							}
							else
							{
								pInventory->deleteItem( pItem->getObjectID() );
								char msg[128];
								sprintf(msg, g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE_NOT_GET_ITEM), pItemInfo->getName().c_str());
								GCSystemMessage gcSystemMessage;
								gcSystemMessage.setMessage(msg);
								pPC->getPlayer()->sendPacket(&gcSystemMessage);
								break;
							}

						}
						else
						{
							pItem = pInventory->findItem( pItem->getItemClass(), pItem->getItemType(), (CoordInven_t&)pt.x, (CoordInven_t&)pt.y );
							Assert(pItem != NULL);

							pItem->setNum( pItem->getNum() + 1 );

							pItem->save( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
							filelog( "HellGardenTower.log", "%s 가 %s을 받았습니다.", pPC->getName().c_str(), pItemInfo->getName().c_str());
						}


					}

				}while(false);
				} // 스킬 북 이벤트 온오프
			}
		}

	}
	{
		// 각 state를 깰때마다 씨앗을 10% 확률로 지급한다.
		// 공간이 없으면 없다고 알리고 안준다.
		ItemType_t SeedType = m_HellGardenType + 6;

		Grade_t grade;
		int i;
		for (i=0, itr=creatures.begin(); itr!=creatures.end(); i++, itr++)
		{
			Creature* pCreature = itr->second;

			if (pCreature->isPC())
			{
				PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
				Inventory* pInventory = pPC->getInventory();
				Item* pItem = NULL;
				int value = rand()%(100);
				if(value < 7) // 7%의 확률
				{
					list<OptionType_t> nullList;
					pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_MOON_CARD, SeedType, nullList);
					pItem->setGrade(grade);
	
					ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(Item::ITEM_CLASS_MOON_CARD, SeedType);
					// 인벤토리의 같은 아이템이 있는지 찾는다.
					_TPOINT pt;
					if( !pInventory->findAddStackItem( pItem, pt ) )
					{
						// 인벤토리에 넣어 줄 아이템의 공간이 있는지 체크 한다.
						if( !pInventory->getEmptySlot( pItem, pt) )
						{
							char msg[128];
							sprintf(msg, g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE_NOT_GET_ITEM), pItemInfo->getName().c_str());
							GCSystemMessage gcSystemMessage;
							gcSystemMessage.setMessage(msg);
							
							pPC->getPlayer()->sendPacket(&gcSystemMessage);
							continue;
						}

						filelog( "HellGardenTower.log", "%s 가 %s을 받았습니다.", pPC->getName().c_str(), pItemInfo->getName().c_str());

						(m_pZone->getObjectRegistry()).registerObject(pItem);

						if( pInventory->addItem( pItem, pt ) )
						{
							pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

							remainTraceLog( pItem, "Leather", pPC->getName(), ITEM_LOG_CREATE, DETAIL_PICKUP );
						}
						else
						{
							pInventory->deleteItem( pItem->getObjectID() );
							char msg[128];
							sprintf(msg, g_pStringPool->c_str(STRID_NOT_ENOUGH_INVENTORY_SPACE_NOT_GET_ITEM), pItemInfo->getName().c_str());
							GCSystemMessage gcSystemMessage;
							gcSystemMessage.setMessage(msg);
							
							pPC->getPlayer()->sendPacket(&gcSystemMessage);
							continue;
						}

					}
					else
					{
						pItem = pInventory->findItem( pItem->getItemClass(), pItem->getItemType(), (CoordInven_t&)pt.x, (CoordInven_t&)pt.y );
						Assert(pItem != NULL);

						pItem->setNum( pItem->getNum() + 1 );

						pItem->save( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );
						filelog( "HellGardenTower.log", "%s 가 %s을 받았습니다.", pPC->getName().c_str(), pItemInfo->getName().c_str());
					}
				}

			}
		}

	}

	__END_CATCH
}

string HellGardenTowerManager::toString() const 
	throw(Error)
{
	StringStream msg;

	switch (m_State)
	{
		case STATE_OPEN:
			msg << "STATE_OPEN, ";
		break;


		default :
		break;
	}

	Timeval currentTime;
	getCurrentTime(currentTime);

	int timeGap = m_StateTime.tv_sec - currentTime.tv_sec;

	msg << timeGap << " sec remain, " << (int)m_pZone->getPCManager()->getSize() << " players";

	return msg.toString();
}

void HellGardenTowerManager::broadcastClosingMessage()
	throw (Error)
{
	__BEGIN_TRY

	char msg[256];
	ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(m_pZone->getZoneID());
	//sprintf( msg, g_pStringPool->c_str(STRID_CANNOT_ENTER_HELLGARDENTOWER), pZoneInfo->getShortName().c_str()); // 20070814
	sprintf( msg, g_pStringPool->c_str(STRID_CANNOT_ENTER_HELLGARDENTOWER), pZoneInfo->getEName().c_str()); // by viva

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType( SYSTEM_MESSAGE_MASTER_LAIR );
	gcSystemMessage.setMessage( msg );
	g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );

	__END_CATCH
}

void HellGardenTowerManager::broadcastResurrectMikllizMessage( int minute )
	throw (Error)
{
	__BEGIN_TRY

	char msg[256];
	if ( minute != 0 )
		sprintf( msg, "미클리즈 완전 부활까지 %d분 남았습니다.", minute );
	else
		sprintf( msg, "미클리즈의 부활을 막지 못했습니다." );

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType( SYSTEM_MESSAGE_MASTER_LAIR );
	gcSystemMessage.setMessage( msg );
	g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );

	__END_CATCH
}

void HellGardenTowerManager::MonsterListLoad()
{
	__BEGIN_TRY

	Statement*	pStmt = NULL;
	Result*		pResult = NULL;
	int			TowerState = 0;

	BEGIN_DB
	{
		ZoneID_t zoneID = m_pZone->getZoneID();

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery(
			"SELECT TowerState, MonsterType, MonsterCount from HellGardenTowerMonsterList WHERE ZoneID=%d", zoneID );

		if (pResult->getRowCount() <= 0) 
		{
			SAFE_DELETE(pStmt);
			//			throw Error("MonsterManager::load() : 존이 존재하지 않습니다.");
			return;
		}

		while (pResult->next()) 
		{
			TowerState = pResult->getInt(1) + 1;
			if((TowerState >= STATE_INIT) || (TowerState < 0))
				continue;
			m_MonterList[TowerState].MonsterType = pResult->getInt(2);
			m_MonterList[TowerState].MonsterCount = pResult->getInt(3);
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)


	__END_CATCH
}
