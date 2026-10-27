////////////////////////////////////////////////////////////////////////////////
// Filename    : HellGardenTowerManager.h 
// Written By  : ­
// Description : 
////////////////////////////////////////////////////////////////////////////////


#include "SanttisiManager.h"
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
// žóº ž®º® ·µù ¶«
#include "DB.h"

#include "Gpackets/GCNoticeEvent.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCDeleteObject.h"

#include <stdio.h>

// Žë Ÿ¿¡ µéŸî°¥ö Ž ·¹Ÿî ý
const int MaxPassPlayer = 6;
const int SanttisiEnterWaitTime = 120; // ï°¡µç å °£ 2º
const int SanttisiCombatWaitTime = 360; // ï°¡µç üõ °£ 6º
const int TimeOutMessageTime = 60; // 1º¿¡ ¹øŸ¿ °£ ¥ žöž »ž°Ž.
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
SanttisiManager::SanttisiManager (Zone* pZone) 
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


	// ¿©±âº žµçŽ 

	if ( m_pZone->getZoneID() == 6100)
	{
		// ï°¡µç 1þ 
		m_SanttisiType = TYPE_Santtisi_1;
		m_SanttisiTowerType = 1;
		m_KickZoneID = 6000;
	}
	
	// ï°¡µç ž¿öžé HellGardenTowerMonsterList ºí¿¡­ °¡®¿Ž.
	MonsterListLoad();

	m_SummonStep = 0;
	m_PartyID = 0;
	m_StartCombat = false;
	m_Mutex.setName("SanttisiManager");

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// destructor
//
////////////////////////////////////////////////////////////////////////////////
SanttisiManager::~SanttisiManager () 
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}
	
// ï°¡µç ±â­
void SanttisiManager::initSanttisi()
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
// Creature°¡  Zone(HellGardenTower)¿¡ µéŸî¿ ö Žö ©°í
// µéŸî¿ ö Žžé µéŸî¿Ž°í ºž°í ©µŽ.
//
// [¶°]
//   - STATE_TANTACLE_COMBAT
//   - m_nPassPlayer < m_nMaxPassPlayer
//   - º®» °¡ö°í Ž°æ¿ì
//   - 100 ·¹º§ »ó
//
////////////////////////////////////////////////////////////////////////////////
bool SanttisiManager::enterPC(PlayerCreature* pPC )
	throw(Error)
{	
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	if (pPC->isDM() || pPC->isGOD())
	{
		m_Mutex.unlock();

		filelog("Santtisi.log", "enterPC - DM or GOD pass! - %s", pPC->getName().c_str() );
		return true;
	}
	// ã¿ëµ ¿ø Ž þŽ. žøµéŸî°£Ž.
	else if( m_nPassPlayer >= MaxPassPlayer )
	{
		m_Mutex.unlock();
		return false;
	}

	if ( m_State == STATE_OPEN )
	{
		//  ¯¹« ®
		if(pPC->getPartyID() == 0)
		{
			m_Mutex.unlock();
			return false;
		}

		// key ® ( m_HellGardenType ž· ±žº )
		Inventory* pInventory = pPC->getInventory();
		Assert( pInventory != NULL );
		if ( !pInventory->hasEnoughNumItem( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 61, 1 ) )
		{
			m_Mutex.unlock();

			filelog("Santtisi.log", "enterPC - no key. deny! - %s", pPC->getName().c_str() );
			return false;
		}
		else
			pPC->decreaseItemClassTypeNum( Item::ITEM_CLASS_COMMON_QUEST_ITEM, 61, 1 );

		// Party ID úå
		m_PartyID = pPC->getPartyID();
	
		// m_State °ª» STATE_WAITž· º¯°æ // 2º

		m_State = STATE_WAIT;

		broadcastClosingMessage();
		
		Timeval currentTime;
		getCurrentTime(currentTime);

		m_StateTime.tv_sec = currentTime.tv_sec + SanttisiEnterWaitTime;

		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(m_pZone->getZoneID());

		char message[128];
		//sprintf(message, g_pStringPool->c_str(STRID_2MINUTE_LATER_APPEAR_HELLGARDEN_MONSTER), pZoneInfo->getShortName().c_str()); // 20070814
		sprintf(message, g_pStringPool->c_str(STRID_2MINUTE_LATER_APPEAR_HELLGARDEN_MONSTER), pZoneInfo->getEName().c_str()); // by viva

		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setType( SYSTEM_MESSAGE_MASTER_LAIR );
		gcSystemMessage.setMessage(message);
		m_pZone->broadcastPacket( &gcSystemMessage );

		// m_nPassPlayer  ýž Ž­·Ž
		m_nPassPlayer = 1;

	}
	else if( m_State == STATE_WAIT )
	{
		// ¿©±â¿¡ žú µéŸî¿ »ç¶÷ ¿øö ®Ž
		if(pPC->getPartyID() != m_PartyID)
		{
			m_Mutex.unlock();
			return false;
		}
		// m_nPassPlayer  ýž Ž­·Ž
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
bool SanttisiManager::leaveCreature(Creature* pCreature)
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

	filelog("Santtisi.log", "leavePC - pass! - PassPlayerNum : %d", m_nPassPlayer );

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	__END_CATCH

	return true;
}


////////////////////////////////////////////////////////////////////////////////
//
// heartbeat
// 
////////////////////////////////////////////////////////////////////////////////
bool SanttisiManager::heartbeat() 
	throw (Error)
{
	__BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	switch (m_State)
	{
		case STATE_WAIT:
			// 2º¶óŽ žž °£» ©¿©Ž. °£ ö³µŽžé STATEž STATE_STAGE_1ž· º¯°æŽ
			// º®ž º¯°æžé­ °£» Ž 6º 
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
			processSanttisiState();
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
void SanttisiManager::processSanttisiState()
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
		// ž Ÿž· ¿±â°í
		if( m_State != STATE_STAGE_1 )
		{
			m_pZone->getPCManager()->transportAllCreatures( m_pZone->getZoneID(), 21, 23, defaultRaceValue, 0 );
		}
		
		// žóº ¯Ž.
//		cout << "Summon Monster" << endl;
		summonMonster();
		// üõ°£» Ž.
		m_StateTime.tv_sec = currentTime.tv_sec + SanttisiCombatWaitTime;
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


	// žóº°¡ Ž Ÿúžžé
	if ( CurrentMonster == 0 )
	{
//		cout << "CurrentMonsts 0 giveKillingReward" << endl;
		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(m_pZone->getZoneID());
		m_State++;
		// ºž»ó» °í
		giveKillingReward();
		if(m_State == STATE_INIT)
		{
			char message[128];
			sprintf(message, g_pStringPool->c_str(STRID_HELLGARDEN_TOWER_LIMITED_TIME), m_State - 2, 0); // 10þ ¿·á  0 ¿© ¬¶ó¿¡° ºž³Ž.
			GCSystemMessage gcSystemMessage;
			gcSystemMessage.setType( SYSTEM_MESSAGE_TIMER );
			gcSystemMessage.setMessage(message);
	//		__ENTER_CRITICAL_SECTION( (*(m_pZone->getZoneGroup())) )
			m_pZone->broadcastPacket( &gcSystemMessage );
	//		__LEAVE_CRITICAL_SECTION( (*(m_pZone->getZoneGroup())) )
			// žðµ Ÿ³Ž.
//			cout << "CurrentState STATE_INIT" << endl;
			kickOutAllPC();
			// ï°¡µç ±â­
			initSanttisi();

			char msg[128];
			//sprintf( msg, g_pStringPool->c_str(STRID_CAN_ENTER_HELLGARDENTOWER), pZoneInfo->getShortName().c_str()); // 20070814
			sprintf( msg, g_pStringPool->c_str(STRID_CAN_ENTER_HELLGARDENTOWER), pZoneInfo->getEName().c_str()); // by viva
	
			gcSystemMessage.setType( SYSTEM_MESSAGE_MASTER_LAIR );
			gcSystemMessage.setMessage( msg );
			g_pZoneGroupManager->pushBroadcastPacket( &gcSystemMessage );
			return;
		}
			
//		cout << "Wait 10 second" << endl;
		// 10°£ ±âŽž°Ž.
		m_StateTime.tv_sec = currentTime.tv_sec + 10;
		m_StartCombat = false;
		char message[128];
		//sprintf(message, g_pStringPool->c_str(STRID_AFTER_10SECOND_MOVE_HELLGARDENTOWER_FLOWER), pZoneInfo->getShortName().c_str() + 9, m_State - 1); // ª 9°¡ ¹»±î¿ä? »»
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
		// °£ ö³µŽ.
		// žðµ Ÿ³Ž.
		kickOutAllPC();
		// ï°¡µç ±â­
		initSanttisi();
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

	//// ·¹Ÿîµé Ž º °æ¿ì
	//if (m_pZone->getPCManager()->getSize()==0)
	//{
	//	cout << "HellGarden All PC Dead" << endl;
	//	// ï°¡µç ±â­
	//	initHellGarden();
	//}		


	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// summon Tantacle
////////////////////////////////////////////////////////////////////////////////
void SanttisiManager::summonMonster()
	throw (Error)
{
	__BEGIN_TRY

	//
	for ( int i=0; i< m_MonterList[m_State].MonsterCount; ++i )
	{
		// ž ºóž®ž £Ÿ³Ž.
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
void SanttisiManager::kickOutAllPC()
	throw (Error)
{
	__BEGIN_TRY

	m_pZone->getPCManager()->transportAllCreatures( m_KickZoneID, 88, 110, defaultRaceValue, 0 );

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// set State Close
////////////////////////////////////////////////////////////////////////////////
void SanttisiManager::setStateClose()
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

bool SanttisiManager::isSummonTiming()
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
// °¢ þº°(STATE) ºž»ó ¹ žðµç þ» ¬ž®Ÿî » °æ¿ì ºž»ó
// ¹ °¡ö°í Ž »ç¶÷º ¿ï ö ŸøŽ.
////////////////////////////////////////////////////////////////////////////////
void SanttisiManager::giveKillingReward() 
	throw (Error)
{
	__BEGIN_TRY

	const PCManager* pPCManager = m_pZone->getPCManager();
	const map< ObjectID_t, Creature* > & creatures = pPCManager->getCreatures();
	map< ObjectID_t, Creature* >::const_iterator itr;

	if(m_State == STATE_INIT)
	{

		ItemType_t CrossOfDefiance = 50; // itemtype 11¹ø ¹¿ª °¡Ž. 
		
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

					// ¹¿ª °¡ ³ŸîŽ.
					list<OptionType_t> nullList;
					pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_COMMON_QUEST_ITEM, CrossOfDefiance, nullList);
					pItem->setGrade(grade);

					pItem->setNum(1);

					ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(Item::ITEM_CLASS_COMMON_QUEST_ITEM, CrossOfDefiance);
					// º¥äž® °°º Ÿ Žö £ŽŽ.
					_TPOINT pt;
					if( !pInventory->findAddStackItem( pItem, pt ) )
					{
						// º¥äž®¿¡ ³Ÿî  Ÿ °ø°£ Žö © Ž.
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
							filelog( "Santtisi.log", "%s°¡ %sž ¹ŸŽŽ.", pPC->getName().c_str(), pItemInfo->getName().c_str());

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
		  }
		
		}

	}

	__END_CATCH
}

string SanttisiManager::toString() const 
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

void SanttisiManager::broadcastClosingMessage()
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


void SanttisiManager::MonsterListLoad()
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
			"SELECT TowerState, MonsterType, MonsterCount from SanttisiMonsterList WHERE ZoneID=%d", zoneID );

		if (pResult->getRowCount() <= 0) 
		{
			SAFE_DELETE(pStmt);
			//			throw Error("MonsterManager::load() : ž žçö ŸŽŽ.");
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
