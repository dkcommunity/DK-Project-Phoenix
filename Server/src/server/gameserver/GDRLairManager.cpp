#include "GDRLairManager.h"
#include "Exception.h"
#include "ZoneUtil.h"
#include "Zone.h"
#include "EffectCastingTrap.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "MonsterInfo.h"
#include "MonsterManager.h"
#include "Gpackets/GCNoticeEvent.h"
#include "Gpackets/GCSystemMessage.h"
#include "EffectKickOut.h"
#include "ZoneGroupManager.h"
#include "PacketUtil.h"
#include "ItemFactoryManager.h"
#include "Gpackets/GCCreateItem.h"
#include "PlayerCreature.h"
#include "Inventory.h"
#include "ItemUtil.h"
#include "Player.h"
#include "EffectGDRLairClose.h"
#include "VariableManager.h"
#include <cmath>
#include <cstdio>

#include "SXml.h"
#include "DB.h"
#include "Properties.h"
#include "StringPool.h"
//#include "SystemAvailabilitiesManager.h"

#include "SystemAvailabilitiesManager.h"
#include <exception>

void GDRLairManager::init()
{
	cout << "Init GDR Lair Manager" << endl;

	m_pGDR = NULL;

	try { m_pZones[ILLUSIONS_WAY_1] = getZoneByZoneID(1410); } catch (...) { m_pZones[ILLUSIONS_WAY_1] = NULL; }
	try { m_pZones[ILLUSIONS_WAY_2] = getZoneByZoneID(1411); } catch (...) { m_pZones[ILLUSIONS_WAY_2] = NULL; }
	try { m_pZones[GDR_LAIR] = getZoneByZoneID(1412); } catch (...) { m_pZones[GDR_LAIR] = NULL; }
	try { m_pZones[GDR_LAIR_CORE] = getZoneByZoneID(1413); } catch (...) { m_pZones[GDR_LAIR_CORE] = NULL; }

	FlyweightStateFactory* pFactory = new FlyweightStateFactory();

	pFactory->registerState( new GDRLairIdle(getNextOpenTime()) );
	pFactory->registerState( new GDRLairEntrance() );
//	pFactory->registerState( new GDRLairIllusionsWayOnly() );
	pFactory->registerState( new GDRLairIcepole() );
	pFactory->registerState( new GDRLairScene1() );
	pFactory->registerState( new GDRLairSummonMonster() );
	pFactory->registerState( new GDRLairScene2() );
	pFactory->registerState( new GDRLairSummonGDRDup() );
	pFactory->registerState( new GDRLairScene3() );
	pFactory->registerState( new GDRLairGDRFight() );
	pFactory->registerState( new GDRLairScene4() );
	pFactory->registerState( new GDRLairAwakenedGDRFight() );
	pFactory->registerState( new GDRLairScene5() );
	pFactory->registerState( new GDRLairMinionFight() );
	pFactory->registerState( new GDRLairScene6() );
	pFactory->registerState( new GDRLairEnding() );
	pFactory->registerState( new GDRLairKillAll() );

	m_pStateFactory = pFactory;
	m_pCurrentState = m_pStateFactory->makeState( GDR_LAIR_IDLE );
	m_pCurrentState->start();

	m_ResetState = GDR_LAIR_IDLE;
	m_bCanEnter = false;

	// Wave listesi SQL'den. Tablo yoksa GS'i dusurmeden seed fallback.
	if (!loadSummonWaves() && m_SummonWaves.empty())
	{
		applyDefaultSummonWaves();
	}
}

void GDRLairManager::applyDefaultSummonWaves()
{
	// Mevcut rallser hardcoded listesinin birebir karsiligi.
	// Yalnizca GDRLairSummonWave tablosu yokken / query fail iken kullanilir.
	static const struct
	{
		int waveIndex;
		MonsterType_t monsterType;
		int monsterNum;
	} kSeed[] = {
		{ 1, 1377, 20 }, { 1, 1386, 10 }, { 1, 1398, 10 },
		{ 2, 1410, 20 }, { 2, 1422, 10 }, { 2, 1432, 20 },
		{ 3,  888, 20 }, { 3,  889, 20 }, { 3,  890,  2 },
		{ 4, 1459, 10 }, { 4, 1460, 20 }, { 4, 1461, 10 },
		{ 5, 1462, 10 }, { 5, 1463,  5 }, { 5, 1464, 20 },
		{ 6, 1465, 20 }, { 6, 1579, 10 }, { 6, 1490, 20 },
		{ 7, 1653, 10 }, { 7, 1578, 20 }, { 7, 1653,  5 },
		{ 8, 1655, 10 }, { 8, 1700, 10 }, { 8, 1697, 10 },
		{ 9, 1699,  4 }, { 9, 1500, 10 },
		{10, 1466,  1 }, {10, 1577,  3 }
	};

	m_SummonWaves.clear();
	int currentWave = -1;
	const size_t n = sizeof(kSeed) / sizeof(kSeed[0]);
	for (size_t i = 0; i < n; ++i)
	{
		if (kSeed[i].waveIndex != currentWave)
		{
			m_SummonWaves.push_back(std::vector<GDRLairSummonSlot>());
			currentWave = kSeed[i].waveIndex;
		}

		GDRLairSummonSlot slot;
		slot.monsterType = kSeed[i].monsterType;
		slot.monsterNum = kSeed[i].monsterNum;
		slot.x = 78;
		slot.y = 89;
		m_SummonWaves.back().push_back(slot);
	}

	filelog("GDRLair.log", "[SummonWave] using built-in seed fallback waves=%d slots=%d",
		(int)m_SummonWaves.size(), (int)n);
	cout << "[Warn] GDRLairSummonWave SQL missing/failed -> built-in seed fallback, waves="
		<< m_SummonWaves.size() << endl;
}

bool GDRLairManager::loadSummonWaves()
{
	std::vector<std::vector<GDRLairSummonSlot> > loaded;
	int skipped = 0;
	int lastWaveIndex = -1;

	try
	{
		Statement* pStmt = NULL;
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			Result* pResult = pStmt->executeQuery(
				"SELECT WaveIndex, Slot, MonsterType, MonsterNum, X, Y "
				"FROM GDRLairSummonWave ORDER BY WaveIndex, Slot");

			while (pResult->next())
			{
				uint col = 0;
				const int waveIndex = pResult->getInt(++col);
				const int slotIndex = pResult->getInt(++col);
				const int monsterType = pResult->getInt(++col);
				const int monsterNum = pResult->getInt(++col);
				const int x = pResult->getInt(++col);
				const int y = pResult->getInt(++col);

				if (waveIndex < 1)
				{
					filelog("GDRLair.log",
						"[SummonWave] skip invalid WaveIndex=%d Slot=%d",
						waveIndex, slotIndex);
					++skipped;
					continue;
				}

				if (monsterNum < 1)
				{
					filelog("GDRLair.log",
						"[SummonWave] skip WaveIndex=%d Slot=%d MonsterNum=%d",
						waveIndex, slotIndex, monsterNum);
					++skipped;
					continue;
				}

				if (g_pMonsterInfoManager != NULL)
				{
					try
					{
						g_pMonsterInfoManager->getMonsterInfo((MonsterType_t)monsterType);
					}
					catch (Throwable& t)
					{
						filelog("GDRLair.log",
							"[SummonWave] skip unknown MonsterType=%d WaveIndex=%d Slot=%d error=%s",
							monsterType, waveIndex, slotIndex, t.toString().c_str());
						++skipped;
						continue;
					}
				}

				if (waveIndex != lastWaveIndex)
				{
					loaded.push_back(std::vector<GDRLairSummonSlot>());
					lastWaveIndex = waveIndex;
				}

				GDRLairSummonSlot slot;
				slot.monsterType = (MonsterType_t)monsterType;
				slot.monsterNum = monsterNum;
				slot.x = (ZoneCoord_t)x;
				slot.y = (ZoneCoord_t)y;
				loaded.back().push_back(slot);
			}

			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
	}
	catch (Throwable& t)
	{
		filelog("GDRLair.log", "[SummonWave] load failed: %s", t.toString().c_str());
		cout << "[Error] GDRLairSummonWave load failed: " << t.toString() << endl;
		return false;
	}

	std::vector<std::vector<GDRLairSummonSlot> > compact;
	for (size_t w = 0; w < loaded.size(); ++w)
	{
		if (!loaded[w].empty())
		{
			compact.push_back(loaded[w]);
		}
	}

	// Tablo var ve bos: kullanicinin 0-wave tercihi. Cache'i bosalt.
	m_SummonWaves.swap(compact);

	int totalSlots = 0;
	for (size_t w = 0; w < m_SummonWaves.size(); ++w)
	{
		totalSlots += (int)m_SummonWaves[w].size();
	}

	filelog("GDRLair.log", "[SummonWave] loaded waves=%d slots=%d skipped=%d",
		(int)m_SummonWaves.size(), totalSlots, skipped);
	cout << "GDR Lair summon waves loaded: " << m_SummonWaves.size()
		<< " waves, " << totalSlots << " slots, skipped=" << skipped << endl;
	return true;
}

void GDRLairManager::clear()
{
	SAFE_DELETE( m_pGDR );

	FiniteStateMachine::clear();
}

void GDRLairManager::run() throw()
{
	string host     = g_pConfig->getProperty("DB_HOST");
	string db       = g_pConfig->getProperty("DB_DB");
	string user     = g_pConfig->getProperty("DB_USER");
	string password = g_pConfig->getProperty("DB_PASSWORD");
	uint port		= 0;
	if ( g_pConfig->hasKey("DB_PORT") )
		port = g_pConfig->getPropertyInt("DB_PORT");

	Connection* pConnection = new Connection(host, db, user, password, port);
	g_pDatabaseManager->addConnection((int)Thread::self(), pConnection);
	cout << "******************************************************" << endl;
	cout << " GDR Lair THREAD CONNECT DB " << endl;
	cout << "******************************************************" << endl;

	Timeval dummyQueryTime;
	getCurrentTime( dummyQueryTime );

	while ( g_bRunning )
	{
		Timeval currentTime;
		getCurrentTime( currentTime );

		try
		{
			heartbeat(currentTime);
		}
		catch (Throwable& t)
		{
			// Yutma yok: log + abort. throw() thread'de what() kaybolmasin.
			State* pState = getCurrentState_Object();
			Monster* pGDR = m_pGDR;
			filelog("GDRLair.log",
				"[GDRThread] Throwable in heartbeat state=%s gdr=%p : %s",
				(pState != NULL) ? pState->toString().c_str() : "(null)",
				pGDR,
				t.toString().c_str());
			filelog("CriticalError.log",
				"GDRLairManager::run Throwable: %s",
				t.toString().c_str());
			if (pGDR != NULL)
			{
				try
				{
					Zone* pZone = pGDR->getZone();
					filelog("GDRLair.log",
						"[GDRThread] GDR oid=%d zone=%p zg=%p xy=(%d,%d)",
						(int)pGDR->getObjectID(),
						pZone,
						(pZone != NULL) ? (void*)pZone->getZoneGroup() : NULL,
						(int)pGDR->getX(),
						(int)pGDR->getY());
				}
				catch (Throwable& t2)
				{
					filelog("GDRLair.log",
						"[GDRThread] GDR deref failed: %s",
						t2.toString().c_str());
				}
				catch (...)
				{
					filelog("GDRLair.log", "[GDRThread] GDR deref failed: unknown");
				}
			}
			abort();
		}
		catch (std::exception& e)
		{
			filelog("GDRLair.log",
				"[GDRThread] std::exception in heartbeat: %s", e.what());
			filelog("CriticalError.log",
				"GDRLairManager::run std::exception: %s", e.what());
			abort();
		}
		catch (...)
		{
			filelog("GDRLair.log", "[GDRThread] unknown exception in heartbeat");
			filelog("CriticalError.log", "GDRLairManager::run unknown exception");
			abort();
		}

		////////////////////////////////////////////////////////
		// dummy query
		////////////////////////////////////////////////////////
		if ( dummyQueryTime < currentTime )
		{
			g_pDatabaseManager->executeDummyQuery( pConnection );

			dummyQueryTime.tv_sec += ( 15 + rand() % 5 ) * 60;
		}

		usleep(100);
	}
}

VSDateTime GDRLairManager::getNextOpenTime() const
{
	static int OpenTime[5] = { 01, 05, 14, 19, 23 };
	VSDateTime ret = VSDateTime::currentDateTime();
	VSTime time = ret.time();
/*	if ( time.setHMS(time.hour(), 0, 0) )
	{
		ret.setTime(time);
	}
*/
	int i;
	for ( i=0; i<5; ++i )
	{
		if ( time.hour() < OpenTime[i] ) break;
	}

	if ( i == 5 )
	{
		cout << "We move on to the next day." << endl;
		ret = ret.addDays(1);
		i=0;
	}

	// There is no 7:00 PM showing on Wednesdays and Sundays.
	if ( ( ret.date().dayOfWeek() == 7 || ret.date().dayOfWeek() == 3 ) && i==3) ++i;

	time.setHMS(OpenTime[i], 0, 0);
	ret.setTime(time);

/*	if ( time.hour()%2 )
		ret = ret.addSecs(3600);
	else
		ret = ret.addSecs(7200);*/
/*	SYSTEM_BEGIN_IF_NOT( SYSTEM_MASTER_LAIR_GDR )
	{
		// ???? ?????? ???? ????? ???? ???? 1?? ??? ????....
		ret = ret.addDays(365);
	}
	SYSTEM_END
*/
	cout << ret.toString() << "Back to GDR Lair" << endl;
	filelog( "GDRLair.log", "%s Back to GDR Lair", ret.toString().c_str() );

	return ret;
}

void GDRLairIdle::start()
{
	filelog( "GDRLair.log", "Starting GDR Lair Idle State : %d", GDRLairManager::Instance().getTotalPCs() );

	Monster* pGDR = GDRLairManager::Instance().getGDR();
	if ( pGDR != NULL )
	{
		pGDR->removeFlag( Effect::EFFECT_CLASS_NO_DAMAGE );
		pGDR->setHP(0);
		GDRLairManager::Instance().setGDR(NULL);
	}

	for ( int i=GDRLairManager::ILLUSIONS_WAY_1 ; i < GDRLairManager::GDR_LAIR_MAX ; ++i )
	{
		Zone* pZone = GDRLairManager::Instance().getZone(i);
		if ( pZone != NULL )
		{
			__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
			pZone->getPCManager()->transportAllCreatures(0xffff);
			pZone->killAllMonsters_UNLOCK();//by rallser fk-server
			pZone->deleteEffect_LOCKING(0);
			__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
		}
	}

	setTimer( GDRLairManager::Instance().getNextOpenTime() );
	SetTimeState::start();
}

void GDRLairEntrance::start()
{
	TimerState::start();

	GCSystemMessage gcSM;
	gcSM.setMessage( g_pStringPool->c_str(STRID_OPEN_GDR_LAIR) ); // 20070814
	g_pZoneGroupManager->pushBroadcastPacket( &gcSM );

	cout << "Starting GDR Lair Enter State" << endl;
	filelog( "GDRLair.log", "Starting GDR Lair Enter State : %d", GDRLairManager::Instance().getTotalPCs() );

	GDRLairManager::Instance().setCorrectPortal( (rand()%3) );
	cout << "The Right Portal : " << (int)GDRLairManager::Instance().getCorrectPortal() << endl;
	filelog( "GDRLair.log", "The Right Portal : %d", GDRLairManager::Instance().getCorrectPortal() );

	Zone* pIllusionsWay1 = getZoneByZoneID(1410);
	Zone* pIllusionsWay2 = getZoneByZoneID(1411);

	EffectKickOut* pEffectKickOut1 = new EffectKickOut( pIllusionsWay1, 20 );
//	pEffectKickOut1->setDeadline(12000);

	EffectKickOut* pEffectKickOut2 = new EffectKickOut( pIllusionsWay2, 20 );
//	pEffectKickOut2->setDeadline(12000);

	pIllusionsWay1->addEffect_LOCKING( pEffectKickOut1 );
	pIllusionsWay2->addEffect_LOCKING( pEffectKickOut2 );

	Zone* pLair = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR );
	Zone* pCore = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR_CORE );
	pEffectKickOut1 = new EffectKickOut( pLair, 115 );
	pEffectKickOut2 = new EffectKickOut( pCore, 115 );

	pLair->addEffect_LOCKING( pEffectKickOut1 );
	pCore->addEffect_LOCKING( pEffectKickOut2 );

	// Open the Lair item.
	cout << "Open the GRD Lair." << endl;
	GDRLairManager::Instance().open();

	EffectGDRLairClose* pEffectClose = new EffectGDRLairClose(20);
//	// 5??
//	pEffectClose->setDeadline(3000);
	pIllusionsWay1->addEffect_LOCKING( pEffectClose );

	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_AUGER, pIllusionsWay1 );

		pEffect->setStartXY( 116, 66 );
		pEffect->setLength( 48 );
		pEffect->setTick( 5 );
		pEffect->setUnit( 6 );
		pEffect->setDir( 7 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_AUGER, pIllusionsWay1 );

		pEffect->setStartXY( 63, 19 );
		pEffect->setLength( 48 );
		pEffect->setTick( 5 );
		pEffect->setUnit( 6 );
		pEffect->setDir( 1 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_AUGER_LARGE, pIllusionsWay1 );

		pEffect->setStartXY( 15, 72 );
		pEffect->setLength( 47 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 7 );
		pEffect->setDir( 3 );
		pEffect->setLarge( true );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_AUGER_LARGE, pIllusionsWay1 );

		pEffect->setStartXY( 68, 119 );
		pEffect->setLength( 27 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 5 );
		pEffect->setDir( 5 );
		pEffect->setLarge( true );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay1 );

		pEffect->setStartXY( 96, 84 );
		pEffect->setLength( 27 );
		pEffect->setTick( 20 );
		pEffect->setUnit( 2 );
		pEffect->setDir( 7 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay1 );

		pEffect->setStartXY( 60, 56 );
		pEffect->setLength( 10 );
		pEffect->setTick( 30 );
		pEffect->setUnit( 1 );
		pEffect->setDir( 1 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay1 );

		pEffect->setStartXY( 51, 73 );
		pEffect->setLength( 10 );
		pEffect->setTick( 30 );
		pEffect->setUnit( 1 );
		pEffect->setDir( 3 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_DROP, pIllusionsWay2 );

		pEffect->setStartXY( 123, 57 );
		pEffect->setLength( 43 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 10 );
		pEffect->setDir( 7 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_DROP, pIllusionsWay2 );

		pEffect->setStartXY( 61, 32 );
		pEffect->setLength( 43 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 10 );
		pEffect->setDir( 3 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_AUGER_LARGE, pIllusionsWay2 );

		pEffect->setStartXY( 91, 97 );
		pEffect->setLength( 43 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 2 );
		pEffect->setDir( 7 );
		pEffect->setLarge(true);
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay2 );

		pEffect->setStartXY( 33, 75 );
		pEffect->setLength( 35 );
		pEffect->setTick( 15 );
		pEffect->setUnit( 2 );
		pEffect->setDir( 3 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}

/*	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay2 );

		pEffect->setStartXY( 25, 67 );
		pEffect->setLength( 43 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 5 );
		pEffect->setDir( 3 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}*/

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay2 );

		pEffect->setStartXY( 49, 127 );
		pEffect->setLength( 30 );
		pEffect->setTick( 20 );
		pEffect->setUnit( 2 );
		pEffect->setDir( 7 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}
/*	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_DROP, pIllusionsWay1 );

		pEffect->setStartXY( 116, 66 );
		pEffect->setLength( 48 );
		pEffect->setTick( 5 );
		pEffect->setUnit( 10 );
		pEffect->setDir( 7 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingIceWall* pEffect = new EffectCastingIceWall( pIllusionsWay1 );

		pEffect->setStartXY( 63, 19 );
		pEffect->setLength( 48 );
		pEffect->setWallLength( 5 );
		pEffect->setTick( 15 );
		pEffect->setDir( 1 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingIceWall* pEffect = new EffectCastingIceWall( pIllusionsWay1 );

		pEffect->setStartXY( 15, 72 );
		pEffect->setLength( 47 );
		pEffect->setWallLength( 5 );
		pEffect->setTick( 15 );
		pEffect->setDir( 3 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay1 );

		pEffect->setStartXY( 68, 119 );
		pEffect->setLength( 27 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 5 );
		pEffect->setDir( 5 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay1 );

		pEffect->setStartXY( 96, 84 );
		pEffect->setLength( 27 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 5 );
		pEffect->setDir( 7 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay1 );

		pEffect->setStartXY( 60, 56 );
		pEffect->setLength( 10 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 5 );
		pEffect->setDir( 1 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay1 );

		pEffect->setStartXY( 51, 73 );
		pEffect->setLength( 10 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 5 );
		pEffect->setDir( 3 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay1->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_AUGER, pIllusionsWay2 );

		pEffect->setStartXY( 123, 57 );
		pEffect->setLength( 43 );
		pEffect->setTick( 5 );
		pEffect->setUnit( 10 );
		pEffect->setDir( 7 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_AUGER_LARGE, pIllusionsWay2 );

		pEffect->setStartXY( 61, 32 );
		pEffect->setLength( 43 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 15 );
		pEffect->setDir( 3 );
		pEffect->setLarge(true);
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingIcicleTrap* pEffect = new EffectCastingIcicleTrap( Effect::EFFECT_CLASS_ICICLE_AUGER_LARGE, pIllusionsWay2 );

		pEffect->setStartXY( 91, 97 );
		pEffect->setLength( 43 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 15 );
		pEffect->setDir( 7 );
		pEffect->setLarge(true);
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay2 );

		pEffect->setStartXY( 25, 67 );
		pEffect->setLength( 43 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 5 );
		pEffect->setDir( 3 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay2 );

		pEffect->setStartXY( 25, 67 );
		pEffect->setLength( 43 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 5 );
		pEffect->setDir( 3 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}

	{
		EffectCastingSideTrap* pEffect = new EffectCastingSideTrap( pIllusionsWay2 );

		pEffect->setStartXY( 55, 132 );
		pEffect->setLength( 43 );
		pEffect->setTick( 10 );
		pEffect->setUnit( 5 );
		pEffect->setDir( 7 );
		pEffect->setNextTime(0);
		pEffect->setDeadline(12000);
		pIllusionsWay2->addEffect_LOCKING( pEffect );
	}*/
}

DWORD GDRLairEntrance::heartbeat(Timeval currentTime)
{
	Zone* pZone = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR );

	__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

	const PCManager* pPM = pZone->getPCManager();
	//int limit = g_pVariableManager->getVariable(GDR_LAIR_PC_LIMIT);
	int limit = 50; 
	// 0??? ???? ??????
	if ( limit != 0 && pPM->getSize() >= limit )
	{
		pZone->getZoneGroup()->unlock();
		return GDR_LAIR_ICEPOLE;
	}

	__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

	return TimerState::heartbeat(currentTime);
}

/*void GDRLairIllusionsWayOnly::start()
{
	TimerState::start();
	filelog( "GDRLair.log", "Starting Illusions Way State" );
	cout << "??????? ????? ??????????" << endl;
}*/

//void GDRLairIllusionsWayOnly::end()
//{
//	Zone* pIllusionsWay1 = getZoneByZoneID(1410);
//	Zone* pIllusionsWay2 = getZoneByZoneID(1411);

/*	__ENTER_CRITICAL_SECTION( (*pIllusionsWay1) )
	pIllusionsWay1->getPCManager()->transportAllCreatures(0xffff);
	__LEAVE_CRITICAL_SECTION( (*pIllusionsWay1) )
	__ENTER_CRITICAL_SECTION( (*pIllusionsWay2) )
	pIllusionsWay2->getPCManager()->transportAllCreatures(0xffff);
	__LEAVE_CRITICAL_SECTION( (*pIllusionsWay2) )*/
//}

/*DWORD GDRLairIllusionsWayOnly::heartbeat(Timeval currentTime)
{
	Zone* pZone = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR );

	__ENTER_CRITICAL_SECTION( (*pZone) )

	const PCManager* pPM = pZone->getPCManager();
	if ( pPM->getSize() > 0 )
	{
		pZone->unlock();
		return GDR_LAIR_ICEPOLE;
	}

	__LEAVE_CRITICAL_SECTION( (*pZone) )

	return TimerState::heartbeat(currentTime);
}*/

void GDRLairIcepole::start()
{
	filelog( "GDRLair.log", "Starting Ice Pole State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << "Ice Column Activation Mode" << endl;
	getCurrentTime( m_BroadcastTime );

	Zone* pIllusionsWay1 = getZoneByZoneID(1410);
	Zone* pIllusionsWay2 = getZoneByZoneID(1411);

	// At Illusion's Way, we kick them all out.
	GCSystemMessage gcSM;
	gcSM.setMessage(g_pStringPool->c_str(STRID_CANNOT_ENTER_ILLUSION_WAY_MOVE_REVIVE_POINT) ); // 20070814

	pIllusionsWay1->getPCManager()->transportAllCreatures(0xffff);
	pIllusionsWay1->deleteEffect_LOCKING(0);

	__ENTER_CRITICAL_SECTION( (*(pIllusionsWay1->getZoneGroup())) )
		pIllusionsWay1->broadcastPacket(&gcSM);
	__LEAVE_CRITICAL_SECTION( (*(pIllusionsWay1->getZoneGroup())) )

	pIllusionsWay2->getPCManager()->transportAllCreatures(0xffff);
	pIllusionsWay2->deleteEffect_LOCKING(0);

	__ENTER_CRITICAL_SECTION( (*(pIllusionsWay2->getZoneGroup())) )
		pIllusionsWay1->broadcastPacket(&gcSM);
	__LEAVE_CRITICAL_SECTION( (*(pIllusionsWay2->getZoneGroup())) )
}

DWORD GDRLairIcepole::heartbeat(Timeval currentTime)
{
	if ( GDRLairManager::Instance().getTotalPCs() < 1 )
	{
		return GDR_LAIR_IDLE;
	}

	Zone* pZone = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR );

/*	int illPCNum = pIllusionsWay1->getPCManager()->getSize() + pIllusionsWay2->getPCManager()->getSize();
	if ( illPCNum > 0 )
	{
		if ( currentTime > m_BroadcastTime )
		{
			GCSystemMessage gcSM;
			char buf[255];
			sprintf(buf, "??????? ????? %d ???? ??????. ??? ????? ??????? ????? ?????? ???? ???? ????????.", illPCNum);
			gcSM.setMessage( buf );

			__ENTER_CRITICAL_SECTION( (*pZone) )

			pZone->broadcastPacket( &gcSM );

			__LEAVE_CRITICAL_SECTION( (*pZone) )

			m_BroadcastTime = currentTime;
			m_BroadcastTime.tv_sec += 60;
		}

		return 0;
	}
*/

	__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

	const PCManager* pPM = pZone->getPCManager();
	const map<ObjectID_t, Creature*>& pCL = pPM->getCreatures();

	map<ObjectID_t, Creature*>::const_iterator itr = pCL.begin();
	map<ObjectID_t, Creature*>::const_iterator endItr = pCL.end();

	for ( ; itr != endItr; ++itr )
	{
		Creature* pCreature = itr->second;

		if ( abs( (int)(pCreature->getX()) - 78 ) < 13 && abs( (int)(pCreature->getY()) - 89 ) < 13 )
		{
			pZone->getZoneGroup()->unlock();
			return GDR_LAIR_SCENE_1;
		}
	}

	__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

	return 0;
}

void GDRLairScene1::start()
{
	filelog( "GDRLair.log", "Starting Scene 1 State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << "GDR Lair Scene 1" << endl;
	Monster* pGDR = new Monster(717);
	pGDR->setName("GDR");
	pGDR->setFlag( Effect::EFFECT_CLASS_NO_DAMAGE );
	pGDR->setTreasure(false);

//	MonsterAI* pAI = pGDR->getBrain();
	pGDR->setBrain(NULL);
//	SAFE_DELETE( pAI );

	Assert( getGDR() == NULL );
	setGDR(pGDR);

	Zone* pLair = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR );
	Assert( pLair != NULL );

	__ENTER_CRITICAL_SECTION( (*(pLair->getZoneGroup())) )

	pLair->addCreature( pGDR, 78, 89, 1 );

	__LEAVE_CRITICAL_SECTION( (*(pLair->getZoneGroup())) )

	m_ActionList.clear();

	m_ActionList.push_back( new ActionEffect( pGDR, Effect::EFFECT_CLASS_SUMMON_GDR, 15 ) );
	m_ActionList.push_back( new ActionWait(pGDR, 30) );
	m_ActionList.push_back( new ActionSay(pGDR, 331) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 332) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 333) );
	m_ActionList.push_back( new ActionWait(pGDR, 80) );
	m_ActionList.push_back( new ActionSay(pGDR, 334) );
	//m_ActionList.push_back( new ActionWalk( pGDR, 74, 85, 9 ) ); //walk by rallser
	//m_ActionList.push_back( new ActionWalk( pGDR, 75, 86, 9 ) );

	m_ActionList.push_back( new ActionWarp( pGDR, 74, 85) );
	m_ActionList.push_back( new ActionWarp( pGDR, 75, 86) );
	
	GDRScene::start();
}

void GDRLairScene1::end()
{
	cout << "End of Scene 1" << endl;

	list<Action*>::iterator itr = m_ActionList.begin();

	for ( ; itr != m_ActionList.end(); ++itr )
	{
		SAFE_DELETE( (*itr) );
	}

	m_ActionList.clear();
}

GDRLairSummonMonster::GDRLairSummonMonster() : MonsterSummonState( GDRLairManager::Instance().getZone(GDRLairManager::GDR_LAIR), GDR_LAIR_SCENE_2, GDR_LAIR_KILL_ALL ) { }

void GDRLairSummonMonster::start()
{
	filelog( "GDRLair.log", "Starting Summon Monster State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << toString() << endl;
	MonsterSummonState::start();

	/*GroupSummonInfo* pGSI = new GroupSummonInfo;

	pGSI->getSummonInfos().push_back( new SummonInfo( 467, 20, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 471, 10, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 584, 10, 78, 89 ) );
	m_GroupSummonInfos.push_back( pGSI );

	pGSI = new GroupSummonInfo;
	pGSI->getSummonInfos().push_back( new SummonInfo( 586, 20, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 587, 10, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 589, 20, 78, 89 ) );
	m_GroupSummonInfos.push_back( pGSI );

	pGSI = new GroupSummonInfo;
	pGSI->getSummonInfos().push_back( new SummonInfo( 590, 20, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 592, 20, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 318, 20, 78, 89 ) );
	m_GroupSummonInfos.push_back( pGSI );

	pGSI = new GroupSummonInfo;
	pGSI->getSummonInfos().push_back( new SummonInfo( 596, 10, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 595, 20, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 407, 10, 78, 89 ) );
	m_GroupSummonInfos.push_back( pGSI );

	pGSI = new GroupSummonInfo;
	pGSI->getSummonInfos().push_back( new SummonInfo( 597, 10, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 598, 5, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 437, 20, 78, 89 ) );
	m_GroupSummonInfos.push_back( pGSI );

	pGSI = new GroupSummonInfo;
	pGSI->getSummonInfos().push_back( new SummonInfo( 604, 20, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 600, 10, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 483, 20, 78, 89 ) );
	m_GroupSummonInfos.push_back( pGSI );

	pGSI = new GroupSummonInfo;
	pGSI->getSummonInfos().push_back( new SummonInfo( 504, 10, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 503, 20, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 614, 5, 78, 89 ) );
	m_GroupSummonInfos.push_back( pGSI );

	pGSI = new GroupSummonInfo;
	pGSI->getSummonInfos().push_back( new SummonInfo( 624, 10, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 614, 10, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 600, 10, 78, 89 ) );
	m_GroupSummonInfos.push_back( pGSI );

	pGSI = new GroupSummonInfo;
	pGSI->getSummonInfos().push_back( new SummonInfo( 602, 4, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 493, 10, 78, 89 ) );
	m_GroupSummonInfos.push_back( pGSI );

	pGSI = new GroupSummonInfo;
//	pGSI->getSummonInfos().push_back( new SummonInfo( 493, 5, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 599, 1, 78, 89 ) );
//	pGSI->getSummonInfos().push_back( new SummonInfo( 624, 10, 78, 89 ) );
	pGSI->getSummonInfos().push_back( new SummonInfo( 603, 3, 78, 89 ) );
	m_GroupSummonInfos.push_back( pGSI );

	m_CurrentSummonInfo = m_GroupSummonInfos.begin();*/

	// Wave listesi GDRLairSummonWave tablosundan. Raid start'ta yeniden okunur.
	GDRLairManager::Instance().loadSummonWaves();
	if (GDRLairManager::Instance().getSummonWaves().empty())
	{
		filelog("GDRLair.log",
			"[SummonWave] cache empty at SUMMON_MONSTER start -> no trash waves, heartbeat will ClearState");
		cout << "[Warn] GDRLairSummonWave cache empty at summon start" << endl;
	}

	const std::vector<std::vector<GDRLairSummonSlot> >& waves =
		GDRLairManager::Instance().getSummonWaves();

	for (size_t w = 0; w < waves.size(); ++w)
	{
		if (waves[w].empty())
		{
			continue;
		}

		GroupSummonInfo* pGSI = new GroupSummonInfo;
		for (size_t s = 0; s < waves[w].size(); ++s)
		{
			const GDRLairSummonSlot& slot = waves[w][s];
			pGSI->getSummonInfos().push_back(
				new SummonInfo(slot.monsterType, slot.monsterNum, slot.x, slot.y));
			filelog("GDRLair.log",
				"[SummonWave] queue wave=%d slot=%d type=%u num=%d xy=(%d,%d)",
				(int)(w + 1), (int)s, (uint)slot.monsterType, slot.monsterNum,
				(int)slot.x, (int)slot.y);
		}
		m_GroupSummonInfos.push_back(pGSI);
	}

	m_CurrentSummonInfo = m_GroupSummonInfos.begin();
}

void GDRLairSummonMonster::end()
{
	list<GroupSummonInfo*>::iterator itr = m_GroupSummonInfos.begin();
	list<GroupSummonInfo*>::iterator endItr = m_GroupSummonInfos.end();

	for ( ; itr != endItr ; ++itr )
	{
		SAFE_DELETE( (*itr) );
	}

	m_GroupSummonInfos.clear();
}

void GDRLairScene2::start()
{
	cout << "Scene 2" << endl;
	filelog( "GDRLair.log", "Starting Scene 2 State : %d", GDRLairManager::Instance().getTotalPCs() );
	Monster* pGDR = getGDR();

	m_ActionList.clear();

	m_ActionList.push_back( new ActionSay( pGDR, 335 ) );
	m_ActionList.push_back( new ActionWait( pGDR, 50 ) );
	m_ActionList.push_back( new ActionSay( pGDR, 336 ) );
	m_ActionList.push_back( new ActionWait( pGDR, 50 ) );

	//m_ActionList.push_back( new ActionWalk( pGDR, 68, 80, 9 ) );//walk by rallser

	m_ActionList.push_back( new ActionWarp( pGDR, 68, 80) );

	m_ActionList.push_back( new ActionSay( pGDR, 361 ) );

	//m_ActionList.push_back( new ActionWalk( pGDR, 56, 66, 9 ) );//walk by rallser

	m_ActionList.push_back( new ActionWarp( pGDR, 56, 66) );

//	m_ActionList.push_back( new ActionWait( pGDR, 50 ) );
	m_ActionList.push_back( new ActionSay( pGDR, 337 ) );

	//m_ActionList.push_back( new ActionWalk( pGDR, 46, 47, 9 ) );//walk by rallser

	m_ActionList.push_back( new ActionWarp( pGDR, 46, 47) );

//	m_ActionList.push_back( new ActionWait( pGDR, 50 ) );
	m_ActionList.push_back( new ActionSay( pGDR, 338 ) );
	//m_ActionList.push_back( new ActionWalk( pGDR, 35, 39, 9 ) ); //walk by rallser

	m_ActionList.push_back( new ActionWarp( pGDR, 35, 39) );

//	m_ActionList.push_back( new ActionWait( pGDR, 30 ) );
	m_ActionList.push_back( new ActionSay( pGDR, 339 ) );
	m_ActionList.push_back( new ActionWait( pGDR, 50 ) );
	m_ActionList.push_back( new ActionSay( pGDR, 340 ) );
	m_ActionList.push_back( new ActionWait( pGDR, 50 ) );
	m_ActionList.push_back( new ActionSay( pGDR, 362 ) );
	m_ActionList.push_back( new ActionWait( pGDR, 60 ) );

//	m_ActionList.push_back( new ActionEffect( pGDR, Effect::EFFECT_CLASS_GDR_SATELLITE, 1200 ) );
//	m_ActionList.push_back( new ActionEffect( pGDR, Effect::EFFECT_CLASS_GDR_FLOATING, 1200 ) );

//	m_ActionList.push_back( new ActionWalk( pGDR, 58, 69, 9 ) );
//	m_ActionList.push_back( new ActionWalk( pGDR, 54, 56, 9 ) );
//	m_ActionList.push_back( new ActionWalk( pGDR, 33, 37, 9 ) );
//	m_ActionList.push_back( new ActionWalk( pGDR, 34, 38, 9 ) );
	
//	m_ActionList.push_back( new ActionRemoveEffect( pGDR, Effect::EFFECT_CLASS_GDR_SATELLITE ) );
//	m_ActionList.push_back( new ActionRemoveEffect( pGDR, Effect::EFFECT_CLASS_GDR_FLOATING ) );

//	m_ActionList.push_back( new ActionWait( pGDR, 30 ) );
	m_ActionList.push_back( new ActionSay( pGDR, 363 ) );
	m_ActionList.push_back( new ActionWait( pGDR, 20 ) );
	m_ActionList.push_back( new ActionEffect( pGDR, Effect::EFFECT_CLASS_COMA, 50 ) );
	m_ActionList.push_back( new ActionWait( pGDR, 60 ) );
	m_ActionList.push_back( new ActionSay( pGDR, 341 ) );
	m_ActionList.push_back( new ActionWait( pGDR, 50 ) );
	m_ActionList.push_back( new ActionSay( pGDR, 342 ) );
	m_ActionList.push_back( new ActionWait( pGDR, 50 ) );
//	m_ActionList.push_back( new ActionWalk( pGDR, 32, 36, 9 ) );

	GDRScene::start();
}

void GDRLairScene2::end()
{
	cout << "End of Scene 2" << endl;

	list<Action*>::iterator itr = m_ActionList.begin();

	for ( ; itr != m_ActionList.end(); ++itr )
	{
		SAFE_DELETE( (*itr) );
	}

	m_ActionList.clear();
}

GDRLairSummonGDRDup::GDRLairSummonGDRDup() : MonsterSummonState( GDRLairManager::Instance().getZone(GDRLairManager::GDR_LAIR), GDR_LAIR_SCENE_3, GDR_LAIR_KILL_ALL ) { }

void GDRLairSummonGDRDup::start()
{
	filelog( "GDRLair.log", "Starting GDR Dup State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << toString() << endl;
	MonsterSummonState::start();

	GroupSummonInfo* pGSI = new GroupSummonInfo;

	// 5????
	pGSI->getSummonInfos().push_back( new SummonInfo( 721, 5, 38, 43 ) );
	m_GroupSummonInfos.push_back( pGSI );

	m_CurrentSummonInfo = m_GroupSummonInfos.begin();
}

void GDRLairSummonGDRDup::end()
{
	list<GroupSummonInfo*>::iterator itr = m_GroupSummonInfos.begin();
	list<GroupSummonInfo*>::iterator endItr = m_GroupSummonInfos.end();

	for ( ; itr != endItr ; ++itr )
	{
		SAFE_DELETE( (*itr) );
	}

	m_GroupSummonInfos.clear();
}

void GDRLairScene3::start()
{
	filelog( "GDRLair.log", "Starting Scene 3 State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << "Scene 3" << endl;
	Monster* pGDR = getGDR();

	m_ActionList.clear();

	m_ActionList.push_back( new ActionSay( pGDR, 343 ) );
	m_ActionList.push_back( new ActionWait( pGDR, 50 ) );
	m_ActionList.push_back( new ActionSay( pGDR, 344 ) );
	m_ActionList.push_back( new ActionWait( pGDR, 50 ) );

	m_ActionList.push_back( new ActionEffect( pGDR, Effect::EFFECT_CLASS_GDR_SATELLITE, 9999999 ) );

	GDRScene::start();
}

void GDRLairScene3::end()
{
	cout << "End of Scene 3" << endl;

	list<Action*>::iterator itr = m_ActionList.begin();

	for ( ; itr != m_ActionList.end(); ++itr )
	{
		SAFE_DELETE( (*itr) );
	}

	m_ActionList.clear();
}

void GDRLairGDRFight::start()
{
	filelog( "GDRLair.log", "Starting GDR Fight State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << "GDR is here" << endl;

	Monster* pGDR = GDRLairManager::Instance().getGDR();
	Assert( pGDR != NULL );
	Zone* pZone = pGDR->getZone();
	Assert( pZone != NULL );

	__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
	
	pGDR->removeFlag( Effect::EFFECT_CLASS_NO_DAMAGE );
	const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo( pGDR->getMonsterType() );

	uint aitype = pMonsterInfo->getAIType();
	MonsterAI* pBrain = new MonsterAI(pGDR, aitype);
	pGDR->setBrain( pBrain );

	__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
}

DWORD GDRLairGDRFight::heartbeat(Timeval currentTime)
{
	Zone* pZone = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR );
	
	Timeval diffTime = timediff( m_previousTime, currentTime );
		
	// Check for success or failure every second
	if ( diffTime.tv_sec > 1 )
	{
		bool bGDRLive = false;
		int psize;

		Monster* pGDR = GDRLairManager::Instance().getGDR();
		
		__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
		
		const PCManager* pPM = pZone->getPCManager();
		psize = pPM->getSize();
		
		MonsterManager* pMM = pZone->getMonsterManager();
				
		map< ObjectID_t, Creature* > Creatures;
		map< ObjectID_t, Creature* >::iterator itr;
		
		Creature *pCreature;
		Monster *pMonster;
		
		Creatures = pMM->getCreatures();
		itr = Creatures.begin();
		for(; itr != Creatures.end(); ++itr)
		{
			pCreature = (itr->second);
			if ( pCreature != NULL && !pCreature->isMonster() )
			{
				continue;
			}
			
			pMonster = dynamic_cast<Monster *>(pCreature);
				
			if ( pMonster->getMonsterType() == pGDR->getMonsterType() )
			{
				bGDRLive = true;
				break;
			}
		}
		
		__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
	
		if ( psize < 1 )
		{
			return GDR_LAIR_KILL_ALL;
		}
		else if ( !bGDRLive )
		{
			return GDR_LAIR_SCENE_4;
		}
		else
		{
			return 0;
		}
		
		m_previousTime = currentTime;
	}
}

void GDRLairGDRFight::end()
{
	GDRLairManager::Instance().setGDR(NULL);

	GCNoticeEvent gcNE;
	gcNE.setCode( NOTICE_EVENT_GDR_LAIR_ENDING_1 );

	Zone* pZone = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR );

	__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

	pZone->broadcastPacket( &gcNE );

	__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
}

void GDRLairScene4::start()
{
	filelog( "GDRLair.log", "Starting Scene 4 State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << "GDR Lair Scene 4" << endl;
	Zone* pGDRLair = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR );
	Zone* pGDRCore = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR_CORE );

	__ENTER_CRITICAL_SECTION( (*(pGDRLair->getZoneGroup())) )

	pGDRLair->getPCManager()->transportAllCreatures( pGDRCore->getZoneID(), 38, 43, defaultRaceValue, 15 );

	__LEAVE_CRITICAL_SECTION( (*(pGDRLair->getZoneGroup())) )

	// GDR's Awakening
	Monster* pGDR = new Monster(723);

	pGDR->setName("GDR");
	pGDR->setTreasure(false);
	pGDR->setFlag( Effect::EFFECT_CLASS_NO_DAMAGE );

	pGDR->setBrain(NULL);

	Assert( getGDR() == NULL );
	setGDR( pGDR );

	__ENTER_CRITICAL_SECTION( (*(pGDRCore->getZoneGroup())) )

	pGDRCore->addCreature( pGDR, 34, 38, 1 );

	__LEAVE_CRITICAL_SECTION( (*(pGDRCore->getZoneGroup())) )

	m_ActionList.clear();
	m_ActionList.push_back( new ActionEffect(pGDR, Effect::EFFECT_CLASS_COMA, 120) );
	m_ActionList.push_back( new ActionWait(pGDR, 120) );
	m_ActionList.push_back( new ActionRemoveEffect(pGDR, Effect::EFFECT_CLASS_COMA) );
	m_ActionList.push_back( new ActionEffect(pGDR, Effect::EFFECT_CLASS_GDR_FLOATING, 100) );
	m_ActionList.push_back( new ActionWait(pGDR, 130) );
	m_ActionList.push_back( new ActionSay(pGDR, 364) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 365) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 366) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 367) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionHollywood(pGDR) );
	m_ActionList.push_back( new ActionWait(pGDR, 10) );
	m_ActionList.push_back( new ActionHollywood(pGDR) );
	m_ActionList.push_back( new ActionWait(pGDR, 10) );
	m_ActionList.push_back( new ActionHollywood(pGDR) );
	m_ActionList.push_back( new ActionWait(pGDR, 10) );
	m_ActionList.push_back( new ActionHollywood(pGDR) );
	m_ActionList.push_back( new ActionWait(pGDR, 10) );
	m_ActionList.push_back( new ActionHollywood(pGDR) );
	m_ActionList.push_back( new ActionWait(pGDR, 30) );
	m_ActionList.push_back( new ActionSay(pGDR, 368) );
	m_ActionList.push_back( new ActionWait(pGDR, 60) );
	m_ActionList.push_back( new ActionSay(pGDR, 347) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 369) );
	m_ActionList.push_back( new ActionWait(pGDR, 30) );

	GDRScene::start();
}

void GDRLairAwakenedGDRFight::start()
{
	filelog( "GDRLair.log", "Starting Awakened GDR Fight State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << "The Awakened GDR has arrived~~" << endl;

	Monster* pGDR = GDRLairManager::Instance().getGDR();
	Assert( pGDR != NULL );
	Zone* pZone = pGDR->getZone();
	Assert( pZone != NULL );

	__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
	
	pGDR->removeFlag( Effect::EFFECT_CLASS_NO_DAMAGE );
	const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo( pGDR->getMonsterType() );

	uint aitype = pMonsterInfo->getAIType();
	MonsterAI* pBrain = new MonsterAI(pGDR, aitype);
	pGDR->setBrain( pBrain );

	__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
}

DWORD GDRLairAwakenedGDRFight::heartbeat(Timeval currentTime)
{
	Zone* pZone = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR_CORE );

//	Zone* pZone = pGDR->getZone();
	Assert( pZone != NULL );

	Timeval diffTime = timediff( m_previousTime, currentTime );
	
	// There's a dead core here because `m_pZoneGroup` is NULL inside `pZone`. slayer5
	// Check it out... Ah, I'm so sleepy...
		
	// Check for success or failure every second
	if ( diffTime.tv_sec > 1 )
	{
		bool bGDRLive = false;
		int psize;
		
		__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
		
		const PCManager* pPM = pZone->getPCManager();
		psize = pPM->getSize();

		MonsterManager* pMM = pZone->getMonsterManager();
		
		map< ObjectID_t, Creature* > Creatures;
		map< ObjectID_t, Creature* >::iterator itr;
		
		Creature *pCreature;
		Monster *pMonster;
		
		Creatures = pMM->getCreatures();
		itr = Creatures.begin();
		for(; itr != Creatures.end(); ++itr)
		{
			pCreature = (itr->second);
			if ( pCreature != NULL && !pCreature->isMonster() )
			{
				continue;
			}
			
			pMonster = dynamic_cast<Monster *>(pCreature);
				
			if ( pMonster->getMonsterType() == 723 /* Awakened GDR */ )
			{
				bGDRLive = true;
				break;
			}
		}
		
		__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
		
		if ( psize < 1 )
		{
			m_bGDRDamaged = false;
			return GDR_LAIR_KILL_ALL;
		}
		
		if ( !bGDRLive )
		{
			m_bGDRDamaged = false;
			// Whoa, it's over!
			return GDR_LAIR_ENDING;
		}
	}

	if ( !m_bGDRDamaged )
	{
		HP_t currentHP;
		HP_t maxHP;

		__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

		Monster* pGDR = GDRLairManager::Instance().getGDR();
		Assert( pGDR != NULL );
		currentHP = pGDR->getHP(ATTR_CURRENT);
		maxHP = pGDR->getHP(ATTR_MAX);

		__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

		if ( currentHP < maxHP/2 )
		{
			m_bGDRDamaged = true;
			return GDR_LAIR_SCENE_5;
		}
	}

	return 0;
}

void GDRLairScene5::start()
{
	filelog( "GDRLair.log", "Starting Scene 5 State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << "GDR Starting Scene 5" << endl;

	Monster* pGDR = getGDR();
	Zone* pZone = pGDR->getZone();

	__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

	pGDR->setBrain(NULL);
	pGDR->setFlag( Effect::EFFECT_CLASS_NO_DAMAGE );

	__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

	m_ActionList.clear();
	m_ActionList.push_back( new ActionSay(pGDR, 348) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 349) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 350) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 370) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );

	int direction = (rand()%2);

	if ( direction == 0 )
	{
		//m_ActionList.push_back( new ActionWalk( pGDR, 79, 24, 9 ) );//walk by rallser
		//m_ActionList.push_back( new ActionWalk( pGDR, 134, 84, 9 ) );

		m_ActionList.push_back( new ActionWarp( pGDR, 79, 24) );
		m_ActionList.push_back( new ActionWarp( pGDR, 134, 84) );
	}
	else
	{
		//m_ActionList.push_back( new ActionWalk( pGDR, 19, 83, 9 ) );
		//m_ActionList.push_back( new ActionWalk( pGDR, 74, 140, 9 ) );
		//m_ActionList.push_back( new ActionWalk( pGDR, 80, 140, 9 ) );

		m_ActionList.push_back( new ActionWarp( pGDR, 19, 83) );
		m_ActionList.push_back( new ActionWarp( pGDR, 74, 140) );
		m_ActionList.push_back( new ActionWarp( pGDR, 80, 140 ) );
	}

	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 351) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 355) );
	m_ActionList.push_back( new ActionWait(pGDR, 30) );

	GDRScene::start();
}

GDRLairMinionFight::GDRLairMinionFight()
	: MonsterSummonState( GDRLairManager::Instance().getZone(GDRLairManager::GDR_LAIR_CORE), GDR_LAIR_SCENE_6, GDR_LAIR_KILL_ALL ) { }

void GDRLairMinionFight::start()
{
	filelog( "GDRLair.log", "Starting Minion Fight State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << toString() << endl;
	MonsterSummonState::start();

	GroupSummonInfo* pGSI = new GroupSummonInfo;

	Monster* pGDR = GDRLairManager::Instance().getGDR();

	if ( pGDR->getX() > pGDR->getY() )
	{
		pGSI->getSummonInfos().push_back( new SummonInfo( 724, 3, 136, 86 ) );
	}
	else
	{
		pGSI->getSummonInfos().push_back( new SummonInfo( 725, 3, 83, 139 ) );
	}
	
	m_GroupSummonInfos.push_back( pGSI );

	m_CurrentSummonInfo = m_GroupSummonInfos.begin();
}

void GDRLairMinionFight::end()
{
	list<GroupSummonInfo*>::iterator itr = m_GroupSummonInfos.begin();
	list<GroupSummonInfo*>::iterator endItr = m_GroupSummonInfos.end();

	for ( ; itr != endItr ; ++itr )
	{
		SAFE_DELETE( (*itr) );
	}

	m_GroupSummonInfos.clear();
}

void GDRLairScene6::start()
{
	filelog( "GDRLair.log", "Starting Scene 6 State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << "GDR Starting Scene 6" << endl;
	Monster* pGDR = getGDR();
	Zone* pZone = pGDR->getZone();

	// ???? ????
	const PCManager* pPCManager = pZone->getPCManager();
	const map< ObjectID_t, Creature* > & creatures = pPCManager->getCreatures();
	map< ObjectID_t, Creature* >::const_iterator itr;

	for (itr=creatures.begin(); itr!=creatures.end(); itr++)
	{
		Creature* pCreature = itr->second;

		if (pCreature->isPC())
		{
			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
			Inventory* pInventory = pPC->getInventory();
			Item* pItem = NULL;

			ItemType_t itemType = 8;
			filelog( "GDRLair.log", "%s received a gift.", pPC->getName().c_str() );
//				itemType = ((goodOneIndex[1]==i||goodOneIndex[2]==i)? 9:8);

			list<OptionType_t> nullList;
			pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_QUEST_ITEM, itemType, nullList);
	 
			(pZone->getObjectRegistry()).registerObject(pItem);

			// Find an empty slot in your inventory.
			_TPOINT p;
			if (pInventory->getEmptySlot(pItem, p))
			{
				// Add it to your inventory.
				pInventory->addItem(p.x, p.y, pItem);

	            pItem->create(pCreature->getName(), STORAGE_INVENTORY, 0, p.x, p.y);

				// Log to ItemTrace
				if ( pItem != NULL && pItem->isTraceItem() )
				{
					remainTraceLog( pItem, "GDRLair", pCreature->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
					remainTraceLogNew( pItem, pCreature->getName(), ITL_GET, ITLD_EVENTNPC, pZone->getZoneID() );
				}

				// It sends an item creation packet to the inventory.
				GCCreateItem gcCreateItem;

				makeGCCreateItem( &gcCreateItem, pItem, p.x, p.y );

				pCreature->getPlayer()->sendPacket(&gcCreateItem);
			}
			else
			{
				filelog( "GDRLair.log", "But there's no room in my inventory." );
				SAFE_DELETE( pItem );
			}
		}
	}

	// It'll fly away in 6 seconds.
	__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

	pGDR->setBrain(NULL);
	pZone->getPCManager()->transportAllCreatures( pZone->getZoneID(), 82, 93, defaultRaceValue, 15 );

	__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )

	// You have to say everything in those 15 seconds.
	m_ActionList.clear();
	m_ActionList.push_back( new ActionSay(pGDR, 356) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 357) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionSay(pGDR, 371) );
	m_ActionList.push_back( new ActionWait(pGDR, 50) );
	m_ActionList.push_back( new ActionWarp(pGDR, 78, 89) );
	// Please wait about 5 seconds while it loads.
	m_ActionList.push_back( new ActionWait(pGDR, 50) );

	m_ActionList.push_back( new ActionSay(pGDR, 358) );
	m_ActionList.push_back( new ActionWait(pGDR, 30) );
	m_ActionList.push_back( new ActionSay(pGDR, 359) );
	m_ActionList.push_back( new ActionWait(pGDR, 30) );

	GDRScene::start();
}

void GDRLairEnding::start()
{
	filelog( "GDRLair.log", "Starting Ending State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << "Wow... It's the ending -o-" << endl;
	TimerState::start();

	Zone* pZone = GDRLairManager::Instance().getZone( GDRLairManager::GDR_LAIR_CORE );

	const PCManager* pPCManager = pZone->getPCManager();
	const map< ObjectID_t, Creature* > & creatures = pPCManager->getCreatures();
	map< ObjectID_t, Creature* >::const_iterator itr;

	int totalNum = creatures.size();
	
	int iBaseCoreZapNum = 2;
	int iBasePendentNum = 3;
	
	int EventCoreZapNum = 0;
	int EventPedentZapNum = 0;
	
	int iBaseRewardUser;
	int iMaxRewardUser;
	int iQuan;
	int iRem;
	
	int CoreZapNum = iBaseCoreZapNum;
	int PendentNum = iBasePendentNum;
	
	//totalNum = 21
	
	iBaseRewardUser = iBaseCoreZapNum + iBasePendentNum;
	iMaxRewardUser = min( iBaseRewardUser * g_pVariableManager->getGDRLairRewardMag() , totalNum);
	iQuan = iMaxRewardUser / iBaseRewardUser;
	iRem = iMaxRewardUser % iBaseRewardUser;
	
	CoreZapNum = iQuan * iBaseCoreZapNum + min(iRem, iBaseCoreZapNum);
	PendentNum = iQuan * iBasePendentNum + max(iRem - iBaseCoreZapNum, 0);
	
	filelog( "GDRLair.log", "GDRLair RewardMagnification=%d, RewardCoreZap=%d, RewardPendant=%d, TotalUser=%d", 
		g_pVariableManager->getGDRLairRewardMag(), 
		CoreZapNum, 
		PendentNum,
		totalNum
	);

	
	int* rewardType = new int[totalNum];

	for ( int i=0; i<totalNum; ++i )
	{
		if ( CoreZapNum + PendentNum > 0 )
		{
			int value = rand()%(totalNum-i);
			if ( value < CoreZapNum )
			{
				rewardType[i] = 0;
				CoreZapNum--;
			}
			else if ( value - CoreZapNum < PendentNum )
			{
				rewardType[i] = 1;
				PendentNum--;
			}
			else rewardType[i] = 2;
		}
		else rewardType[i] = 2;
	}

	ItemType_t itemType;
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

			if ( rewardType[i] == 0 )
			{
			//	int value = rand()%3;
				/*static int iTypeRatio[4] = { 5, 25, 35, 35 };
				for ( itemType = 0 ; itemType < 4; ++itemType )
				{
					value -= iTypeRatio[itemType];
					if ( value < 0 ) break;
				}
				if ( itemType >= 4 ) itemType = 3;

				value = rand()%1000;
				static int gradeRatio[5] = { 340, 490, 148, 20, 2 };
				for ( grade = 0 ; grade < 5; ++grade )
				{
					value -= gradeRatio[grade];
					if (value < 0 ) break;
				}
				if ( grade >= 5 ) grade = 4;
				grade++;*/

			//	if(value == 2)
				//{
					itemType = 72;
			//	}
			//	else if(value == 1)
			//	{
			//		itemType = 71;
			//	}
			//	else if(value == 0)
			//	{
			//		itemType = 70;
			//	}
				list<OptionType_t> nullList;
				pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_STAR, itemType, nullList);
				//pItem->setGrade(grade);
				filelog( "GDRLair.log", "%s The event was held at the venue. : %d/%d", pPC->getName().c_str(), itemType, grade );
			}
			else
			{
				if ( rewardType[i] == 1 )
				{
					itemType = 75;
					filelog( "GDRLair.log", "%s Earn the Pendant Reward. :", pPC->getName().c_str() );
					list<OptionType_t> nullList;
					pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_STAR, itemType, nullList);
				}
				else
				{
//					itemType = 8;
//					filelog( "GDRLair.log", "%s ?? ????j ???????.", pPC->getName().c_str() );
					// ????? ????? ???.
					continue;
				}
//				itemType = ((goodOneIndex[1]==i||goodOneIndex[2]==i)? 9:8);

			}
	 
			(pZone->getObjectRegistry()).registerObject(pItem);

			// Find an empty slot in your inventory.
			_TPOINT p;
			if (pInventory->getEmptySlot(pItem, p))
			{
				// Add it to your inventory.
				pInventory->addItem(p.x, p.y, pItem);

	            pItem->create(pCreature->getName(), STORAGE_INVENTORY, 0, p.x, p.y);

				// Log to ItemTrace
				if ( pItem != NULL && pItem->isTraceItem() )
				{
					remainTraceLog( pItem, "GDRLair", pCreature->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
					remainTraceLogNew( pItem, pCreature->getName(), ITL_GET, ITLD_EVENTNPC );
				}

				// It sends an item creation packet to the inventory.
				GCCreateItem gcCreateItem;

				makeGCCreateItem( &gcCreateItem, pItem, p.x, p.y );

				pCreature->getPlayer()->sendPacket(&gcCreateItem);
			}
			else
			{
				SAFE_DELETE( pItem );
			}
		}
	}

}

void GDRLairEnding::end()
{
	cout << "This is the ending here too, lol" << endl;

//	Monster* pGDR = GDRLairManager::Instance().getGDR();
//	SAFE_DELETE( pGDR );
	GDRLairManager::Instance().setGDR(NULL);

	for ( int i=GDRLairManager::ILLUSIONS_WAY_1 ; i < GDRLairManager::GDR_LAIR_MAX ; ++i )
	{
		Zone* pZone = GDRLairManager::Instance().getZone(i);
		if ( pZone != NULL )
		{
			__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
			pZone->getPCManager()->transportAllCreatures(0xffff);
			pZone->killAllMonsters_UNLOCK();
			pZone->deleteEffect_LOCKING(0);
			__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
		}
	}

	TimerState::end();
}

void GDRLairKillAll::start()
{
	filelog( "GDRLair.log", "Starting Killall State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << "Start GDRLairKillAll" << endl;

	GCSystemMessage gcSM;
	gcSM.setMessage( g_pStringPool->c_str(STRID_FAILED_GDR_LAIR_ATTACK_MOVE_REVIVE_POINT) ); // 20070814

	for ( int i=GDRLairManager::ILLUSIONS_WAY_1 ; i < GDRLairManager::GDR_LAIR_MAX ; ++i )
	{
		Zone* pZone = GDRLairManager::Instance().getZone(i);
		if ( pZone != NULL )
		{
			__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
			pZone->broadcastPacket( &gcSM );
			__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
		}
	}
}

void GDRLairKillAll::end()
{
	filelog( "GDRLair.log", "Ending Killall State : %d", GDRLairManager::Instance().getTotalPCs() );
	cout << "End of GDRLairKillAll" << endl;

	Monster* pGDR = GDRLairManager::Instance().getGDR();
	if ( pGDR != NULL )
	{
		pGDR->removeFlag( Effect::EFFECT_CLASS_NO_DAMAGE );
		pGDR->setHP(0);
	}
//	SAFE_DELETE( pGDR );
	GDRLairManager::Instance().setGDR(NULL);

	for ( int i=GDRLairManager::ILLUSIONS_WAY_1 ; i < GDRLairManager::GDR_LAIR_MAX ; ++i )
	{
		Zone* pZone = GDRLairManager::Instance().getZone(i);
		if ( pZone != NULL )
		{
			__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
			pZone->getPCManager()->transportAllCreatures(0xffff);
			pZone->killAllMonsters_UNLOCK();
			pZone->deleteEffect_LOCKING(0);
			__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) )
		}
	}
}

int	GDRLairManager::getTotalPCs() const
{
	int ret = 0;
	for ( int i=ILLUSIONS_WAY_1; i<GDR_LAIR_MAX; ++i )
	{
		if (m_pZones[i] == NULL) continue;
		ret += m_pZones[i]->getPCManager()->getSize();
	}
	return ret;
}

bool GDRLairManager::isGDRLairZone(ZoneID_t ZoneID) const
{
	for ( int i=ILLUSIONS_WAY_1; i<GDR_LAIR_MAX; ++i )
	{
		if (m_pZones[i] == NULL) continue;
		if (m_pZones[i]->getZoneID() == ZoneID) return true;
	}

	return false;
}
