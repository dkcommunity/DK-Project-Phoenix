////////////////////////////////////////////////////////////////////////////////
// Filename    : MikllizzLairManager.h 
// Written By  : ��
// Description : 
////////////////////////////////////////////////////////////////////////////////


#include "MikllizzLairManager.h"
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

#include "Gpackets/GCNoticeEvent.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCCreateItem.h"
#include "Gpackets/GCDeleteObject.h"

#include <stdio.h>

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

////////////////////////////////////////////////////////////////////////////////
//
// constructor
//
////////////////////////////////////////////////////////////////////////////////
MikllizzLairManager::MikllizzLairManager (Zone* pZone) 
	throw (Error)
{
	__BEGIN_TRY
		
	Assert(pZone != NULL);
	m_pZone = pZone;

	m_MikllizzID = 0;
	m_MikllizzCorpseID = 0;
	m_MikllizzX = 63;
	m_MikllizzY = 61;

	m_nMaxPassPlayer = g_pVariableManager->getVariable( MIKLLIZZ_MAX_PLAYER );
//	m_nMaxPassPlayer = 20;
	m_nPassPlayer = 0;

	m_State = STATE_CLOSE;

	Timeval currentTime;
	getCurrentTime(currentTime);

	m_StateTime.tv_sec = currentTime.tv_sec;
	m_StateTime.tv_usec = 0;

	m_SummonNextTime.tv_sec = currentTime.tv_sec;
	m_SummonNextTime.tv_usec = 0;

	m_ClosingMessageStep = ClosingMessageMinuteMax;
	m_ResurrectMikllizzMessageStep = ResurrectMikllizzMessageMinuteMax;

	setStateClose();

	m_Mutex.setName("MikllizzLairManager");

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// destructor
//
////////////////////////////////////////////////////////////////////////////////
MikllizzLairManager::~MikllizzLairManager () 
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}
	
////////////////////////////////////////////////////////////////////////////////
//
// enterCreature ( Creature* )
//
////////////////////////////////////////////////////////////////////////////////
//
// Creature�� �� Zone(MikllizzLair)�� ���� �� �ִ��� üũ�ϰ�
// ���� �� �ִٸ� ���Դٰ� ���� üũ�صд�.
//
// [����]
//   - STATE_TANTACLE_COMBAT
//   - m_nPassPlayer < m_nMaxPassPlayer
//   - ���μ��� ������ �ִ°��
//   - 100 ���� �̻�
//
////////////////////////////////////////////////////////////////////////////////
bool MikllizzLairManager::enterPC(PlayerCreature* pPC )
	throw(Error)
{	
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	if (pPC->isDM() || pPC->isGOD())
	{
		m_Mutex.unlock();
			
		filelog("Mikllizz.log", "enterPC - DM or GOD pass! - %s", pPC->getName().c_str() );
		return true;
	}

	if ( m_State != STATE_TANTACLE_COMBAT )
	{
		m_Mutex.unlock();
		return false;
	}

	// ���μ��� ������ �ֳ�?
	Inventory* pInventory = pPC->getInventory();
	Assert( pInventory != NULL );
	if ( !pInventory->hasEnoughNumItem( Item::ITEM_CLASS_QUEST_ITEM, 10, 1 ) )
	{
		m_Mutex.unlock();

		filelog("Mikllizz.log", "enterPC - no sealing stone. deny! - %s", pPC->getName().c_str() );
		return false;
	}

	// ���� ����
	if ( pPC->getLevel() < 100 )
	{
		m_Mutex.unlock();

		filelog("Mikllizz.log", "enterPC - under limited level. deny! - %s", pPC->getName().c_str() );
		return false;
	}

	// �ο� ����
	if (m_nPassPlayer >= m_nMaxPassPlayer )
	{
		m_Mutex.unlock();

		filelog("Mikllizz.log", "enterPC - over limited pass. deny! - %s", pPC->getName().c_str() );
		return false;
	}

	// ���� �� �ִٰ� �Ǵܵ� ���
	++m_nPassPlayer;

	filelog("Mikllizz.log", "enterPC - pass! - PassPlayerNum : %d", m_nPassPlayer );

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
bool MikllizzLairManager::leaveCreature(Creature* pCreature)
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

	filelog("Mikllizz.log", "leavePC - pass! - PassPlayerNum : %d", m_nPassPlayer );

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH

	return true;
}


////////////////////////////////////////////////////////////////////////////////
//
// heartbeat
// 
////////////////////////////////////////////////////////////////////////////////
bool MikllizzLairManager::heartbeat() 
	throw (Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	switch (m_State)
	{
		case STATE_CLOSE:
			processCloseState();
		break;

		case STATE_TANTACLE_COMBAT:
			processTantacleCombatState();
		break;
		
		case STATE_MIKLLIZZ_COMBAT:
			processMikllizzCombatState();
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
// process Close State
//
////////////////////////////////////////////////////////////////////////////////
void MikllizzLairManager::processCloseState()
	throw (Error)
{
	__BEGIN_TRY

	Timeval currentTime;
	getCurrentTime(currentTime);

	if (currentTime >= m_StateTime)
	{
		// ��� ���͸� ���δ�.
		m_pZone->killAllMonsters_UNLOCK();

		// ��� �ð��� ������..
		// ��ŸŬ ��ȯ
		summonTantacle();
		// ��Ŭ���� ��ü �߰�
		addMikllizzCorpse();

		// Tantacle Combat ���·� �ٲ۴�.
		m_State = STATE_TANTACLE_COMBAT;
		m_StateTime.tv_sec = currentTime.tv_sec + 30 * 60; // 20������ ����

		// ��Ŭ���� ���� ���� �޽��� �߼�
		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setType( SYSTEM_MESSAGE_MASTER_LAIR );
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_SOON_MIKLLIZZ_REVIVE_RAOM_DUNGEON5) ); // 20070814
		g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
		gcSystemMessage.setMessage( g_pStringPool->c_str(STRID_CAN_ENTER_RAOM_DUNGEON_30MINUTE) );
		g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );

		// ���� �޽��� �߼� �ܰ� �ʱ�ȭ
		m_ClosingMessageStep = 0;

		m_nMaxPassPlayer = g_pVariableManager->getVariable( MIKLLIZZ_MAX_PLAYER ) ;

		filelog("Mikllizz.log", "State Tentacle Combat : reamin %d minutes", ( m_StateTime.tv_sec - currentTime.tv_sec ) / 60 );
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// process Tantacle Combat
//
////////////////////////////////////////////////////////////////////////////////
void MikllizzLairManager::processTantacleCombatState()
	throw (Error)
{
	__BEGIN_TRY

	Timeval currentTime;
	getCurrentTime(currentTime);

	int remainMinute = ( m_StateTime.tv_sec - currentTime.tv_sec ) / 60;
	if ( m_ClosingMessageStep >= 0 && m_ClosingMessageStep < ClosingMessageMinuteMax )
	{
		if ( ClosingMessageMinute[m_ClosingMessageStep] == remainMinute )
		{
			broadcastClosingMessage( remainMinute );
			++m_ClosingMessageStep;
		}
	}

	// ��� �ð��� ������..
	// �����߹��Ѵ�.
	if (currentTime >= m_StateTime)
	{
		// ��� �ðܳ���.
		kickOutAllPC();

		// Close ���·� �����.
		setStateClose();

		// ��Ŭ���� ��ü ����
		removeMikllizzCorpse();

		return;
	}

	// 2026-05-19: getMonsters() map key MonsterType_t (eski SpriteType_t).
	// Tantacle sprite=255 paylasan TUM counter'lari topla (gelecekte variant
	// eklenirse aggregate korunsun). Empirical 2026-05-19: SType=255 sadece
	// MType=809 ile eslesiyor, tek counter — ama defansif olarak total say.
	const map<MonsterType_t,MonsterCounter*>& monsters = m_pZone->getMonsterManager()->getMonsters();
	int tantacleTotalCount = 0;
	bool tantacleCounterFound = false;
	for (map<MonsterType_t,MonsterCounter*>::const_iterator iter = monsters.begin();
	     iter != monsters.end(); ++iter)
	{
		if (iter->second && iter->second->getSpriteType() == SpriteTypeTantacle)
		{
			tantacleCounterFound = true;
			tantacleTotalCount += iter->second->getCurrentMonsters();
		}
	}
	if ( tantacleCounterFound )
	{
		if ( tantacleTotalCount == 0 )
		{
			// ��ŸŬ�� ��� �׾���.
			// Mikllizz Combat ���·� �ٲ۴�.
			m_State = STATE_MIKLLIZZ_COMBAT;
			m_StateTime.tv_sec = currentTime.tv_sec + 60 * 60; // 45������ ����

			// ��Ŭ���� ��ü ����
			removeMikllizzCorpse();

			// ��Ŭ��� Ȱ���ϰ� �����.
			activeMikllizz();

			// ���μ��� �Һ��Ѵ�.
			decreaseSealingStone();

			// ���� �޽��� �߼� �ܰ� �ʱ�ȭ
			m_ResurrectMikllizzMessageStep = 0;

			filelog("Mikllizz.log", "State Mikllizz Combat : reamin %d minutes", ( m_StateTime.tv_sec - currentTime.tv_sec ) / 60 );
		}

//		cout << "���� ��ŸŬ ��: " << itr->second->getCurrentMonsters() << endl;
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// process Mikllizz Combat
//
////////////////////////////////////////////////////////////////////////////////
void MikllizzLairManager::processMikllizzCombatState()
	throw (Error)
{
	__BEGIN_TRY

	Timeval currentTime;
	getCurrentTime(currentTime);

	int remainMinute = ( m_StateTime.tv_sec - currentTime.tv_sec ) / 60;
	if ( m_ResurrectMikllizzMessageStep >= 0 && m_ResurrectMikllizzMessageStep < ResurrectMikllizzMessageMinuteMax )
	{
		if ( ResurrectMikllizzMessageMinute[m_ResurrectMikllizzMessageStep] == remainMinute )
		{
			broadcastResurrectMikllizMessage( remainMinute );
			++m_ResurrectMikllizzMessageStep;
		}
	}

	Creature* pMikllizz = m_pZone->getMonsterManager()->getCreature( m_MikllizzID );

	// ��Ŭ��� �׾�����
	if (pMikllizz==NULL || pMikllizz->isDead() )
	{
		// ������ �ְ�
		giveKillingReward();

		// ��� �ѾƳ���.
		kickOutAllPC();

		// Close ���·� �����.
		setStateClose();
	}
	else if (currentTime >= m_StateTime)
	{
		// �ð� ������.
		// ��� �ѾƳ���.
		kickOutAllPC();
		// Close ���·� �����.
		setStateClose();
	}

	// �÷��̾���� �� ���� ���
	if (m_pZone->getPCManager()->getSize()==0)
	{
		// Close ���·� �����.
		setStateClose();
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// summon Tantacle
////////////////////////////////////////////////////////////////////////////////
void MikllizzLairManager::summonTantacle()
	throw (Error)
{
	__BEGIN_TRY

	// 30�������� 70������ ���� 
	for ( int i=0; i<70; ++i )
	{
		// ���� ���ڸ��� ã�Ƴ���.
		ZoneCoord_t x,y;
		if ( !m_pZone->getMonsterManager()->findPosition( MonsterTypeTantacle, x, y ) )
		{
			Assert(false);
			return;
		}

		Monster* pMonster = new Monster(MonsterTypeTantacle);
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
// add Mikllizz corpse
////////////////////////////////////////////////////////////////////////////////
void MikllizzLairManager::addMikllizzCorpse()
	throw (Error)
{
	__BEGIN_TRY

	MonsterCorpse* pMikllizzCorpse = new MonsterCorpse( MonsterTypeMikllizz, "��Ŭ����", 2 );
	Assert( pMikllizzCorpse != NULL );

	m_pZone->registerObject( pMikllizzCorpse );
	m_MikllizzCorpseID = pMikllizzCorpse->getObjectID();

	TPOINT tp = m_pZone->addItem( pMikllizzCorpse, m_MikllizzX, m_MikllizzY, true, 20 * 60 * 10 );
	if ( tp.x == -1 )
	{
		SAFE_DELETE( pMikllizzCorpse );
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// remove Mikllizz corpse
////////////////////////////////////////////////////////////////////////////////
void MikllizzLairManager::removeMikllizzCorpse()
	throw (Error)
{
	__BEGIN_TRY

	Tile& tile = m_pZone->getTile( m_MikllizzX, m_MikllizzY );
	Item* pItem = NULL;

	if ( tile.hasItem() )
	{
		pItem = tile.getItem();

		if ( pItem != NULL && pItem->getObjectID() == m_MikllizzCorpseID )
		{
			m_pZone->deleteItem( pItem, m_MikllizzX, m_MikllizzY );

			GCDeleteObject gcDeleteObject;
			gcDeleteObject.setObjectID( pItem->getObjectID() );
			m_pZone->broadcastPacket( m_MikllizzX, m_MikllizzY, &gcDeleteObject );

			SAFE_DELETE( pItem );
		}
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// set Mikllizz
////////////////////////////////////////////////////////////////////////////////
void MikllizzLairManager::activeMikllizz()
	throw (Error)
{
	__BEGIN_TRY

	Monster* pMonster = new Monster(MonsterTypeMikllizz);
	pMonster->setTreasure(false);

	Assert( pMonster != NULL );

	try
	{
		m_pZone->addCreature( pMonster, m_MikllizzX, m_MikllizzY, 2 );
		m_pZone->monsterScan( pMonster, m_MikllizzX, m_MikllizzY, 2 );
		m_MikllizzID = pMonster->getObjectID();
	}
	catch ( EmptyTileNotExistException& )
	{
		SAFE_DELETE( pMonster );
	}
	
	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// kick out all PC
////////////////////////////////////////////////////////////////////////////////
void MikllizzLairManager::kickOutAllPC()
	throw (Error)
{
	__BEGIN_TRY

	m_pZone->getPCManager()->transportAllCreatures( 1129, 63, 63, defaultRaceValue, 10 );

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// set State Close
////////////////////////////////////////////////////////////////////////////////
void MikllizzLairManager::setStateClose()
	throw (Error)
{
	__BEGIN_TRY

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

	m_State = STATE_CLOSE;

	__END_CATCH
}

bool MikllizzLairManager::isSummonTiming()
	throw (Error)
{
	Turn_t		Duration = 300;
	Timeval 	currentTime;

	getCurrentTime(currentTime);

	if( m_SummonNextTime.tv_sec > currentTime.tv_sec )
	{
//		cout << "SummonNextTime = " << m_SummonNextTime.tv_sec << " CurrentTime = " << currentTime.tv_sec << endl;
		return false;
	}

	m_SummonNextTime.tv_sec = currentTime.tv_sec + Duration;

	return true;

}

////////////////////////////////////////////////////////////////////////////////
//
// give Killing Reward
//
////////////////////////////////////////////////////////////////////////////////
// �����Ͱ� �׾������� ����
// ������ QuestItem�� ���� ���� ����鿡�� ������ �κ��丮�� �־��ش�.
// �κ��丮�� �ڸ��� ���� ��쿣 �ٴڿ� ����߸��µ�
// �̹� ������ �ִ� ����� �ֿ� �� ����.
////////////////////////////////////////////////////////////////////////////////
void MikllizzLairManager::giveKillingReward() 
	throw (Error)
{
	__BEGIN_TRY

	const PCManager* pPCManager = m_pZone->getPCManager();
	const map< ObjectID_t, Creature* >& creatures = pPCManager->getCreatures();
	map< ObjectID_t, Creature* >::const_iterator itr = creatures.begin();
	map< ObjectID_t, Creature* >::const_iterator endItr = creatures.end();

	for ( ; itr != endItr; ++itr )
	{
		Creature* pCreature = itr->second;

		if (pCreature->isPC())
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
			Inventory* pInventory = pPC->getInventory();

			//------------------------------------------------------------
			// ��� ����ġ�� �÷��ش�.
			//------------------------------------------------------------
			// ������ ��ġ�� 7Ÿ�� �̳��� �̴� ���
			//
			pPC->increaseRankExp( MASTER_KILL_RANK_EXP );

			// ������ ����~ ���� ����ǥ���� �����´�.
			const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo( MonsterTypeReward );
			Assert( pMonsterInfo != NULL );

			TreasureList* pTreasureList = NULL;

			if ( pPC->isSlayer() )
				pTreasureList = pMonsterInfo->getSlayerTreasureList();
			else if ( pPC->isVampire() )
				pTreasureList = pMonsterInfo->getVampireTreasureList();
			else if ( pPC->isOusters() )
				pTreasureList = pMonsterInfo->getOustersTreasureList();
			else
				Assert( false );

			const list<Treasure*>& treasures = pTreasureList->getTreasures();
			list<Treasure*>::const_iterator itr = treasures.begin();
			list<Treasure*>::const_iterator endItr = treasures.end();

			ITEM_TEMPLATE it;
			Item* pItem = NULL;

			for ( ; itr != endItr; ++itr )
			{
				Treasure* pTreasure = (*itr);

				it.ItemClass = Item::ITEM_CLASS_MAX;
				it.ItemType = 0;

//				if ( pTreasure->getRandomItem(&it,true) )
				if ( !pTreasure->getRandomItem(&it,true) )
				{
					it.ItemClass = Item::ITEM_CLASS_SKILL_BOOK;
					it.ItemType = 33; // 20080711 �ｺ3���� ��ü
					static int HealthCount = 0;
					
					filelog("MiklizzHealth.log", "�ｺ 3 ������ ���� ���� : %d", HealthCount++);
				}	

				{
					pItem = g_pItemFactoryManager->createItem( it.ItemClass, it.ItemType, it.OptionType );
					Assert( pItem != NULL );

					pItem->setGrade( min(7, ItemGradeManager::Instance().getRandomBeadGrade() ) );
					TPOINT pt;
					m_pZone->registerObject( pItem );

					if ( pInventory->addItem( pItem, pt ) )
					{
						pItem->create( pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y );

						GCCreateItem gcCreateItem;
						makeGCCreateItem( &gcCreateItem, pItem, pt.x, pt.y );

						pPC->getPlayer()->sendPacket( &gcCreateItem );

						if ( pItem->isTraceItem() )
						{
							remainTraceLog( pItem, pPC->getName(), "Mikllizz", ITEM_LOG_CREATE, DETAIL_EVENTNPC );
						}
					}
					else
					{
						SAFE_DELETE( pItem );
					}
				}
			}
		}
	}

	__END_CATCH
}

string MikllizzLairManager::toString() const 
	throw(Error)
{
	StringStream msg;

	switch (m_State)
	{
		case STATE_CLOSE:
			msg << "STATE_CLOSE, ";
		break;

		case STATE_TANTACLE_COMBAT:
			msg << "TANTACLE_COMBAT, ";
		break;

		case STATE_MIKLLIZZ_COMBAT:
			msg << "MIKLLIZZ_COMBAT, ";
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

void MikllizzLairManager::decreaseSealingStone()
throw ( Error )
{
	const map<ObjectID_t, Creature*> & ZoneCreatures = m_pZone->getPCManager()->getCreatures();
	map<ObjectID_t, Creature*>::const_iterator BeginZoneCreatures = ZoneCreatures.begin();
	while( BeginZoneCreatures != ZoneCreatures.end() )
	{
		Creature* pCreature = BeginZoneCreatures->second;
		Assert(pCreature != NULL);

		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
		Assert( pPC != NULL );
		Inventory* pInventory = pPC->getInventory();
		Assert( pInventory != NULL );

		bool bAdvanceMode = pPC->isDM() || pPC->isGOD();

		if( !bAdvanceMode && pPC->getItemClassTypeNum( Item::ITEM_CLASS_QUEST_ITEM, 10 ) <= 0 )
		{
			// ���μ��� ������ ���� �������� ��Ż �¿� ��������!
			GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
			EventTransport* pEventTransport = dynamic_cast<EventTransport*>(pGamePlayer->getEvent(Event::EVENT_CLASS_TRANSPORT));
			
			if( pEventTransport == NULL )
			{
				pEventTransport = new EventTransport(pGamePlayer);
			}
			pEventTransport->setTargetZone( 1129, 63, 63 );
			
			pGamePlayer->addEvent(pEventTransport);
			return;
		}
		else
			pPC->decreaseItemClassTypeNum( Item::ITEM_CLASS_QUEST_ITEM, 10, 1 );

		++BeginZoneCreatures;
	}
}

void MikllizzLairManager::broadcastClosingMessage( int minute )
	throw (Error)
{
	__BEGIN_TRY

	char msg[256];
	if ( minute != 0 )
		sprintf( msg, g_pStringPool->c_str(STRID_CLOSING_RAOM_DUNGEON_AFTER_MINUTE), minute ); // 20070814
	else
		sprintf( msg, g_pStringPool->c_str(STRID_CLOSED_RAOM_DUNGEON_MIKLLIZZ_REVIVE) );

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType( SYSTEM_MESSAGE_MASTER_LAIR );
	gcSystemMessage.setMessage( msg );
	g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );

	__END_CATCH
}

void MikllizzLairManager::broadcastResurrectMikllizMessage( int minute )
	throw (Error)
{
	__BEGIN_TRY

	char msg[256];
	if ( minute != 0 )
		sprintf( msg, g_pStringPool->c_str(STRID_REMAIN_MINUTE_MIKLLIZZ_REVIVE), minute ); // 20070814
	else
		sprintf( msg, g_pStringPool->c_str(STRID_DONOT_STOP_MIKLLIZZ_REVIVE) );

	GCSystemMessage gcSystemMessage;
	gcSystemMessage.setType( SYSTEM_MESSAGE_MASTER_LAIR );
	gcSystemMessage.setMessage( msg );
	g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );

	__END_CATCH
}

