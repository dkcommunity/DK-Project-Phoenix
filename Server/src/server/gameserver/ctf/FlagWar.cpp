#include "FlagWar.h"
#include "FlagManager.h"
#include "ItemFactoryManager.h"
#include "Zone.h"
#include "ZoneUtil.h"
#include "ZoneInfoManager.h"
#include "GlobalItemPositionLoader.h"
#include "MonsterSummonInfo.h"

#include "ZoneGroupManager.h"
#include "Gpackets/GCNoticeEvent.h"

list<FlagWar::FlagGenZone> FlagWar::m_FlagGenInfo;

void FlagWar::execute() throw(Error)
{
	__BEGIN_TRY

	switch ( getState() )
	{
		case STATE_WAIT :
			filelog( "FlagWar.log", "FlagWar Ready.." );
			executeReady();
			setState( STATE_READY );
			break;
		case STATE_READY :
			filelog( "FlagWar.log", "FlagWar Start.." );
			executeStart();
			setState( STATE_START );
			break;
		case STATE_START :
			filelog( "FlagWar.log", "FlagWar Finising.." );
			executeFinish();
			setState( STATE_FINISH );
			break;
		case STATE_FINISH :
			filelog( "FlagWar.log", "FlagWar End.." );
			executeEnd();
			setState( STATE_WAIT );
			break;
		default :
			filelog( "FlagWar.log", "�̻��� FlagWar ����.." );
			break;
	}

	__END_CATCH
}

void FlagWar::executeReady() throw(Error)
{
	__BEGIN_TRY


	GCNoticeEvent gcNE;
	gcNE.setCode( NOTICE_EVENT_FLAG_WAR_READY );

	VSDateTime current = VSDateTime::currentDateTime();
	gcNE.setParameter( ((DWORD)((DWORD)(current.date().year() - 2000))*1000000) + ((DWORD)((DWORD)current.date().month())*10000)
					   + ((DWORD)((DWORD)current.date().day())*100) + ((DWORD)((DWORD)current.time().hour())) );

	g_pZoneGroupManager->pushBroadcastPacket( &gcNE );

	// 5���ִٰ� ��������
	g_pFlagManager->addSchedule( new Schedule( this, VSDateTime::currentDateTime().addSecs(300) ) );

	__END_CATCH
}

void FlagWar::addFlagsRandom( ZoneID_t zoneID, uint no )
{
	Zone* pZone = getZoneByZoneID( zoneID );
	Assert( pZone != NULL );
	VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
	BPOINT pt;

	pZone->lock();

	for ( uint i=0; i<no; ++i )
	{
		pt.x = rand()%pZone->getWidth();
		pt.y = rand()%pZone->getHeight();

		while ( !rect.ptInRect(pt.x, pt.y) || pZone->getTile(pt.x, pt.y).hasItem() || pZone->getTile(pt.x, pt.y).isBlocked(Creature::MOVE_MODE_WALKING) 
				|| ( (pZone->getZoneLevel(pt.x, pt.y))&SAFE_ZONE == 0 ) )
		{
			pt.x = rand()%pZone->getWidth();
			pt.y = rand()%pZone->getHeight();
		}

		Item* pItem = g_pItemFactoryManager->createItem( Item::ITEM_CLASS_EVENT_ITEM, 27, list<OptionType_t>() );
		Assert( pItem != NULL );

		pZone->registerObject( pItem );
		TPOINT ptInZone = pZone->addItem( pItem, pt.x, pt.y, true, 36000 );
		pItem->create( "", STORAGE_ZONE, pZone->getZoneID(), ptInZone.x, ptInZone.y );

		filelog( "FlagWar.log", "%d : (%d,%d) �� ����� ����������ϴ�.", pZone->getZoneID(), ptInZone.x, ptInZone.y );

		m_Flags.push_back( pItem->getItemID() );
	}

	pZone->unlock();

	g_pFlagManager->getAllowMap()[zoneID] = no;
}

void FlagWar::executeStart() throw(Error)
{
	__BEGIN_TRY

	GCNoticeEvent gcNE;
	gcNE.setCode( NOTICE_EVENT_FLAG_WAR_START );

	VSDateTime current = VSDateTime::currentDateTime();
	gcNE.setParameter( ((DWORD)((DWORD)(current.date().year() - 2000))*1000000) + ((DWORD)((DWORD)current.date().month())*10000)
					   + ((DWORD)((DWORD)current.date().day())*100)    + ((DWORD)((DWORD)current.time().hour())) );

	g_pZoneGroupManager->pushBroadcastPacket( &gcNE );
	
	m_Flags.clear();
	g_pFlagManager->getAllowMap().clear();

	addFlags();
//	addFlagsRandom( 1122, 20 );

	// �����ϰ� ���� �����ؼ� 100���� ����� �����Ѵ�.
	// 2�ð� ����
	g_pFlagManager->addSchedule( new Schedule( this, VSDateTime::currentDateTime().addSecs(getWarTime()) ) );
	g_pFlagManager->startFlagWar();

	__END_CATCH
}

void FlagWar::executeFinish() throw(Error)
{
	__BEGIN_TRY

	GCNoticeEvent gcNE;
	gcNE.setCode( NOTICE_EVENT_FLAG_WAR_FINISH );

	Race_t winnerRace = g_pFlagManager->getWinnerRace();
	gcNE.setParameter( ((DWORD)((DWORD)winnerRace << 16)) | (DWORD)g_pFlagManager->getFlagCount( winnerRace ) );

	g_pZoneGroupManager->pushBroadcastPacket( &gcNE );
	
	// 3���ִٰ� ���� ��ģ��.
	g_pFlagManager->addSchedule( new Schedule( this, VSDateTime::currentDateTime().addSecs(180) ) );
	g_pFlagManager->endFlagWar();

	__END_CATCH
}

void FlagWar::executeEnd() throw(Error)
{
	__BEGIN_TRY

	// �����ߴ� ��ߵ��� �Ѿư��� �� �����Ӵ�.
	vector<ItemID_t>::iterator itr = m_Flags.begin();
	vector<ItemID_t>::iterator endItr = m_Flags.end();
	
	for ( ; itr != endItr ; ++itr )
	{
		GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load( Item::ITEM_CLASS_EVENT_ITEM, *itr );
		if ( pItemPosition == NULL ) continue;

		// popItem�� ������ �ش� ��ġ���� �̾Ƴ��Ƿ� ���������� �ȴ�.
		// �̰� FlagManager �� ���� �����忡�� �ҷ����Ƿ� �ȿ��� ���ɾ���� �ȴ�.
		Item* pItem = pItemPosition->popItem( true );
		if ( pItem != NULL )
		{
			pItem->destroy();
			SAFE_DELETE(pItem);
		}
		else
		{
			filelog( "FlagWar.log", "��� ������ ���� ����... ��.��");
		}
	}

	g_pFlagManager->resetFlagCounts();
	m_Flags.clear();

	// ======================================================================
	// 2026-05-09: CTF Event Reward — Boss + Minions + Item Drop
	// ----------------------------------------------------------------------
	// Eski yorum kod (asagida) tek monster spawn yapiyordu, simdi genisletildi:
	//   - 1x Boss "Chief Dun Wolfarch" (MType 599, Level 250) kazanan irk koord
	//   - 5x Elite minion (MType 256 IronTeeth, Temerie zone yerli yaratik)
	//   - 20x EVENT_GIFT_BOX item zone'a drop, 7 tanesi kazanan irk konumuna bias
	//   - GCNoticeEvent broadcast
	// Zone: 1122 "Temerie Sacred Ground" — CTF flag pole zone'u (FlagPolePosition VAMPIRE).
	// Kazanan irk yoksa (tie) reward atlanir.
	// ======================================================================
	{
		const ZoneID_t REWARD_ZONE = 1122;
		Zone* pRewardZone = getZoneByZoneID(REWARD_ZONE);
		Race_t winnerRace = g_pFlagManager->getWinnerRace();

		if (pRewardZone != NULL && (winnerRace == RACE_SLAYER || winnerRace == RACE_VAMPIRE || winnerRace == RACE_OUSTERS))
		{
			ZoneCoord_t winnerX = 0, winnerY = 0;
			switch (winnerRace)
			{
				case RACE_SLAYER:  winnerX = 90; winnerY = 50; break;
				case RACE_VAMPIRE: winnerX = 24; winnerY = 52; break;
				case RACE_OUSTERS: winnerX = 30; winnerY = 86; break;
				default: break;
			}

			SUMMON_INFO summonInfo;
			summonInfo.canScanEnemy = false;
			summonInfo.clanType = SUMMON_INFO::CLAN_TYPE_DEFAULT;

			// 1x Boss "Chief Dun Wolfarch" (Level 250 elite) — kazanan irk koord
			addMonstersToZone(pRewardZone, winnerX, winnerY, 0, 599, 1, summonInfo);

			// 5x Elite minion (IronTeeth type 256, Temerie yerli) — bos koordda
			addMonstersToZone(pRewardZone, winnerX, winnerY, 5, 256, 5, summonInfo);

			// 20x EVENT_GIFT_BOX item — zone'a dagit, 7 tanesi kazanan irk koord bias
			const int TOTAL_BOXES = 20;
			const int BIAS_COUNT = 7;
			const int LIFETIME_SEC = 1800;  // 30 dakika yere durur
			list<OptionType_t> emptyOpts;
			int zoneW = pRewardZone->getWidth();
			int zoneH = pRewardZone->getHeight();

			for (int i = 0; i < TOTAL_BOXES; i++)
			{
				Item* pItem = g_pItemFactoryManager->createItem(
					Item::ITEM_CLASS_EVENT_GIFT_BOX, 0, emptyOpts);
				if (pItem == NULL) continue;

				pRewardZone->registerObject(pItem);

				// Koord int olarak hesapla, sonra BYTE'a clamp (BPOINT.x/y unsigned)
				int rawX, rawY;
				if (i < BIAS_COUNT)
				{
					// Bias: kazanan irk koordinati etrafinda ±5 tile
					rawX = (int)winnerX + (rand() % 11 - 5);
					rawY = (int)winnerY + (rand() % 11 - 5);
				}
				else
				{
					// Rastgele tum zone
					rawX = rand() % zoneW;
					rawY = rand() % zoneH;
				}

				// Bounds clamp (BPOINT BYTE unsigned, asagi tasarsa yer kaybi)
				if (rawX < 0) rawX = 0;
				if (rawY < 0) rawY = 0;
				if (rawX >= zoneW) rawX = zoneW - 1;
				if (rawY >= zoneH) rawY = zoneH - 1;

				BPOINT pt;
				pt.x = (BYTE)rawX;
				pt.y = (BYTE)rawY;

				TPOINT ptInZone = pRewardZone->addItem(pItem, pt.x, pt.y, true, LIFETIME_SEC);
				pItem->create("", STORAGE_ZONE, REWARD_ZONE, ptInZone.x, ptInZone.y);
			}

			// Broadcast — kazanan irk + zone bilgisi (mevcut FINISH notice extension)
			GCNoticeEvent gcNE;
			gcNE.setCode(NOTICE_EVENT_FLAG_WAR_FINISH);
			gcNE.setParameter(((DWORD)((DWORD)winnerRace << 16)) | (DWORD)REWARD_ZONE);
			g_pZoneGroupManager->pushBroadcastPacket(&gcNE);

			filelog("FlagWar.log",
				"CTF Reward: winner=%d zone=%d boss=599 minions=5 boxes=%d",
				(int)winnerRace, (int)REWARD_ZONE, TOTAL_BOXES);
		}
		else
		{
			filelog("FlagWar.log", "CTF Reward: SKIP (zone NULL veya tie/unknown winner)");
		}
	}

	// Bir sonraki CTF event schedule
	g_pFlagManager->addSchedule( new Schedule( this, getNextFlagWarTime() ) );

	// Eski yorum kod — tek monster spawn (referans olarak korundu, yeni reward
	// blogu yukarida bunu genisletiyor):
/*	ZoneCoord_t	ZoneX, ZoneY;

	switch ( g_pFlagManager->getWinnerRace() )
	{
		case RACE_SLAYER:
			ZoneX = 90;
			ZoneY = 50;
			break;

		case RACE_VAMPIRE:
			ZoneX = 24;
			ZoneY = 52;
			break;

		case RACE_OUSTERS:
			ZoneX = 30;
			ZoneY = 86;
			break;

		default:
			return;
	}

	SUMMON_INFO summonInfo;
	summonInfo.canScanEnemy = false;
	summonInfo.clanType = SUMMON_INFO::CLAN_TYPE_DEFAULT;

	addMonstersToZone(getZoneByZoneID(1122), ZoneX, ZoneY, 0, 599, 1, summonInfo);*/

	__END_CATCH
}

VSDateTime FlagWar::getNextFlagWarTime()
{
	static const int NextFlagWarDay[8] =
	{
		0, 0, 1, 0, 1, 0, 0, 0
	};

	VSDateTime dt = VSDateTime::currentDateTime();

	VSDateTime nextWarDateTime;
	nextWarDateTime = dt.addDays( NextFlagWarDay[dt.date().dayOfWeek()] );

	if ( nextWarDateTime.date().dayOfWeek() < 6 )
		nextWarDateTime.setTime( VSTime( 18, 55, 0 ) );
	else
		nextWarDateTime.setTime( VSTime( 13, 55, 0 ) );

	if ( nextWarDateTime < VSDateTime::currentDateTime() )
	{
		bool anotherDay = true;
		if ( nextWarDateTime.date().dayOfWeek() == 6 )
		{
			nextWarDateTime.setTime( VSTime( 18, 55, 0 ) );
			if ( nextWarDateTime >= VSDateTime::currentDateTime() ) anotherDay = false;
		}

		if ( anotherDay )
		{
			//nextWarDateTime = nextWarDateTime.addDays( NextWarDay[warType][dt.addDays(1).date().dayOfWeek()] );
			nextWarDateTime = nextWarDateTime.addDays( 1 );
			nextWarDateTime = nextWarDateTime.addDays( NextFlagWarDay[nextWarDateTime.date().dayOfWeek()] );

			if ( nextWarDateTime.date().dayOfWeek() < 6 )
				nextWarDateTime.setTime( VSTime( 18, 55, 0 ) );
			else
				nextWarDateTime.setTime( VSTime( 13, 55, 0 ) );
		}
	}

	//cout << nextWarDateTime.toString() << "�� ��� ���� �����Դ�~" << endl;
	filelog( "FlagWar.log", "%s�� ��� ���� �̺�Ʈ ����", nextWarDateTime.toString().c_str() );

	return nextWarDateTime;
}

void FlagWar::addFlags()
{
	addFlagsRandom( 21, 6 );
	addFlagsRandom( 22, 6 );
	addFlagsRandom( 23, 6 );
	addFlagsRandom( 24, 7 );

	addFlagsRandom( 11, 7 );
	addFlagsRandom( 12, 6 );
	addFlagsRandom( 13, 6 );
	addFlagsRandom( 14, 6 );

	addFlagsRandom( 31, 6 );
	addFlagsRandom( 32, 6 );
	addFlagsRandom( 33, 7 );
	addFlagsRandom( 34, 6 );

	addFlagsRandom( 61, 11 );
	addFlagsRandom( 62, 11 );
	addFlagsRandom( 63, 12 );
	addFlagsRandom( 64, 11 );
}

int FlagWar::getWarTime() const { return 7200; }
