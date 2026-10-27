//////////////////////////////////////////////////////////////////////////////
// DynamicZoneMoba.cpp
//////////////////////////////////////////////////////////////////////////////

#include "DynamicZoneMoba.h"
#include "DynamicZoneInfo.h"
#include "DynamicZoneManager.h"
#include "DynamicZoneGroup.h"
#include "Zone.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"
#include "PCFinder.h"
#include "PCManager.h"
#include "Monster.h"
#include "MonsterManager.h"
#include "PlayerCreature.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemUtil.h"
#include "VariableManager.h"
#include "DB.h"
#include "Properties.h"
#include "PacketUtil.h"
#include "Mutex.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCMobaJoin.h"
#include "Gpackets/GCMobaInfoAll.h"
#include "Gpackets/GCMobaInfoSelf.h"
#include "Gpackets/GCCreateItem.h"

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

namespace
{
	const ZoneID_t MOBA_TEMPLATE_ZONE_ID = 8111;
	const int MOBA_SCOREBOARD_SECONDS = 5;
	const int MOBA_QUEUE_TIMEOUT_SECONDS = 300;
	const char* MOBA_REWARD_BUY_ID = "AOS_MOBA_REWARD";

	const MobaPlayConfig* getLoadedMobaConfig()
	{
		if (g_pVariableManager == NULL || !g_pVariableManager->isMobaPlayConfigValid())
			return NULL;

		return &g_pVariableManager->getMobaPlayConfig();
	}

	DWORD getMobaRewardGoodsID()
	{
		if (g_pVariableManager == NULL)
			return 450;

		int goodsID = g_pVariableManager->getMobaRewardGoodsID();
		return goodsID > 0 ? (DWORD)goodsID : 450;
	}

	enum MobaRuntimeState
	{
		MOBA_STATE_IDLE = 0,
		MOBA_STATE_RUNNING,
		MOBA_STATE_FINISHED
	};

	struct MobaSpawn
	{
		MonsterType_t MonsterType;
		ZoneCoord_t X;
		ZoneCoord_t Y;
		int Team;
	};

	const MobaSpawn RED_STRUCTURES[] =
	{
		{ 2309, 211, 40, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2309, 215, 44, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2309, 227, 91, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2309, 223, 145, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2309, 231, 199, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2309, 164, 28, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2309, 110, 32, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2309, 56, 24, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2309, 176, 79, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2309, 152, 95, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2309, 142, 121, DynamicZoneMoba::MOBA_TEAM_RED }
	};

	const MobaSpawn BLUE_STRUCTURES[] =
	{
		{ 2308, 40, 211, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2308, 44, 215, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2308, 91, 227, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2308, 145, 223, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2308, 199, 231, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2308, 28, 164, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2308, 32, 110, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2308, 24, 56, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2308, 79, 176, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2308, 103, 160, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2308, 113, 134, DynamicZoneMoba::MOBA_TEAM_BLUE }
	};

	const MobaSpawn WAVE_SPAWNS[] =
	{
		{ 2285, 202, 45, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2286, 205, 48, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2287, 209, 52, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2291, 214, 53, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2292, 218, 49, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2293, 220, 46, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2297, 202, 41, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2298, 205, 37, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2299, 208, 33, DynamicZoneMoba::MOBA_TEAM_RED },
		{ 2282, 45, 202, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2283, 48, 205, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2284, 52, 209, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2288, 53, 214, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2289, 49, 218, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2290, 46, 220, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2294, 41, 202, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2295, 37, 205, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2296, 33, 208, DynamicZoneMoba::MOBA_TEAM_BLUE },
		{ 2300, 126, 156, DynamicZoneMoba::MOBA_TEAM_NONE },
		{ 2301, 141, 181, DynamicZoneMoba::MOBA_TEAM_NONE },
		{ 2302, 128, 100, DynamicZoneMoba::MOBA_TEAM_NONE },
		{ 2303, 84, 67, DynamicZoneMoba::MOBA_TEAM_NONE },
		{ 2304, 74, 105, DynamicZoneMoba::MOBA_TEAM_NONE },
		{ 2305, 115, 74, DynamicZoneMoba::MOBA_TEAM_NONE }
	};

	vector<string> s_WaitingPlayers;
	Mutex s_QueueMutex;
	bool s_QueueMutexNamed = false;
	Timeval s_QueueExpireTime;
	bool s_QueueHasDeadline = false;

	void ensureQueueMutexName()
	{
		if (!s_QueueMutexNamed)
		{
			s_QueueMutex.setName("MobaQueue");
			s_QueueMutexNamed = true;
		}
	}

	BYTE getRaceCode(Race_t race)
	{
		if (race == RACE_VAMPIRE) return 1;
		if (race == RACE_OUSTERS) return 2;
		return 0;
	}

	bool isSameZone(Creature* pCreature, Zone* pZone)
	{
		return pCreature != NULL && pCreature->getZone() == pZone;
	}

	WORD getQueueRemainingSeconds(const Timeval& current)
	{
		if (!s_QueueHasDeadline)
			return 0;

		if (current >= s_QueueExpireTime)
			return 0;

		long seconds = s_QueueExpireTime.tv_sec - current.tv_sec;
		if (seconds < 0)
			seconds = 0;
		if (seconds > 65535)
			seconds = 65535;

		return (WORD)seconds;
	}

	void startQueueDeadline(const Timeval& current)
	{
		s_QueueExpireTime = current;
		s_QueueExpireTime.tv_sec += MOBA_QUEUE_TIMEOUT_SECONDS;
		s_QueueHasDeadline = true;
	}

	void clearQueueDeadline()
	{
		s_QueueHasDeadline = false;
		getCurrentTime(s_QueueExpireTime);
	}
}

DynamicZoneMoba::DynamicZoneMoba()
{
	resetMatch();
}

DynamicZoneMoba::~DynamicZoneMoba()
{
}

void DynamicZoneMoba::init()
{
	resetMatch();
}

void DynamicZoneMoba::resetMatch()
{
	m_Records.clear();
	m_RedBaseObjectID = 0;
	m_BlueBaseObjectID = 0;
	m_State = MOBA_STATE_IDLE;
	m_WinnerTeam = MOBA_TEAM_NONE;
	m_RewardGiven = false;
	getCurrentTime(m_EndTime);
	getCurrentTime(m_NextWaveTime);
	getCurrentTime(m_NextInfoTime);
	getCurrentTime(m_WaitOutTime);
}

bool DynamicZoneMoba::setupMatch(const vector<PlayerCreature*>& players)
{
	return setupMatchInternal(players, false);
}

bool DynamicZoneMoba::setupMatchInternal(const vector<PlayerCreature*>& players, bool testMode)
{
	__BEGIN_TRY

	Assert(m_pZone != NULL);

	const MobaPlayConfig* pConfig = getLoadedMobaConfig();
	if (pConfig == NULL || pConfig->Enable == 0)
	{
		filelog("Moba.log", "[setupMatch] MobaPlayConfig unavailable zone=%u", (uint)getZoneID());
		m_Status = DYNAMIC_ZONE_STATUS_READY;
		return false;
	}

	if (!testMode && players.size() != (uint)pConfig->MobaMaxPC)
	{
		filelog("Moba.log", "[setupMatch] invalid player count=%u zone=%u", (uint)players.size(), (uint)getZoneID());
		m_Status = DYNAMIC_ZONE_STATUS_READY;
		return false;
	}

	if (testMode && (players.empty() || players.size() > (uint)pConfig->MobaMaxPC))
	{
		filelog("Moba.log", "[setupMatch] invalid test player count=%u max=%d zone=%u",
			(uint)players.size(), pConfig->MobaMaxPC, (uint)getZoneID());
		m_Status = DYNAMIC_ZONE_STATUS_READY;
		return false;
	}

	resetMatch();

	vector<PlayerCreature*> shuffled = players;
	random_shuffle(shuffled.begin(), shuffled.end());

	for (uint i = 0; i < shuffled.size(); ++i)
	{
		PlayerCreature* pPC = shuffled[i];
		if (pPC == NULL || !validateJoin(pPC, false, testMode))
		{
			filelog("Moba.log", "[setupMatch] player became invalid before match index=%u", i);
			m_Status = DYNAMIC_ZONE_STATUS_READY;
			return false;
		}
	}

	for (uint i = 0; i < shuffled.size(); ++i)
	{
		PlayerCreature* pPC = shuffled[i];
		MobaRecord record;
		record.Name = pPC->getName();
		record.ObjectID = pPC->getObjectID();
		record.Race = pPC->getRace();
		uint redTeamSize = testMode ? (uint)((shuffled.size() + 1) / 2) : (uint)pConfig->MobaTeamSize;
		record.Team = (i < redTeamSize) ? MOBA_TEAM_RED : MOBA_TEAM_BLUE;
		record.Body = 0;
		record.Hair = 0;
		m_Records.push_back(record);
	}

	if (!spawnInitialMonsters())
	{
		sendMatchMessage("AOS Battle cannot start because base monsters are not configured.");
		resetMatch();
		m_Status = DYNAMIC_ZONE_STATUS_READY;
		return false;
	}

	if (!testMode)
	{
		for (uint i = 0; i < shuffled.size(); ++i)
		{
			if (!increaseTodayPlayCount(shuffled[i]))
			{
				sendMatchMessage("AOS Battle cannot start because play limit data could not be updated.");
				resetMatch();
				m_Status = DYNAMIC_ZONE_STATUS_READY;
				return false;
			}
		}
	}

	spawnWave();

	getCurrentTime(m_EndTime);
	m_EndTime.tv_sec += pConfig->MobaPlayTime;
	getCurrentTime(m_NextWaveTime);
	m_NextWaveTime.tv_sec += pConfig->MobaWaveSpawnTime;
	getCurrentTime(m_NextInfoTime);
	m_NextInfoTime.tv_sec += MOBA_SCOREBOARD_SECONDS;
	m_State = MOBA_STATE_RUNNING;

	for (uint i = 0; i < m_Records.size(); ++i)
	{
		PlayerCreature* pPC = findOnlinePC(m_Records[i].Name);
		if (pPC == NULL)
			continue;

		if (m_Records[i].Team == MOBA_TEAM_RED)
			transportCreature(pPC, getZoneID(), 230, 27, true);
		else
			transportCreature(pPC, getZoneID(), 27, 230, true);

		m_Records[i].ObjectID = pPC->getObjectID();
		sendSystemMessage(pPC, testMode ? "AOS Battle test has started." : "AOS Battle has started.");
		sendSelfInfo(pPC);
		sendScoreboardTo(pPC, "");
	}

	filelog("Moba.log", "[setupMatch] started zone=%u players=%u test=%d", (uint)getZoneID(), (uint)m_Records.size(), testMode ? 1 : 0);
	return true;

	__END_CATCH
}

void DynamicZoneMoba::heartbeat()
{
	__BEGIN_TRY

	if (m_State == MOBA_STATE_IDLE)
		return;

	Timeval current;
	getCurrentTime(current);

	if (m_State == MOBA_STATE_RUNNING)
	{
		const MobaPlayConfig* pConfig = getLoadedMobaConfig();
		if (pConfig == NULL || pConfig->Enable == 0)
		{
			filelog("Moba.log", "[heartbeat] MobaPlayConfig became unavailable during match zone=%u", (uint)getZoneID());
			finishMatch(MOBA_TEAM_NONE);
			return;
		}

		if (current > m_NextWaveTime)
		{
			spawnWave();
			m_NextWaveTime = current;
			m_NextWaveTime.tv_sec += pConfig->MobaWaveSpawnTime;
		}

		if (current > m_NextInfoTime)
		{
			for (uint i = 0; i < m_Records.size(); ++i)
			{
				PlayerCreature* pPC = findOnlinePC(m_Records[i].Name);
				if (pPC != NULL && isSameZone(pPC, m_pZone))
					sendSelfInfo(pPC);
			}
			m_NextInfoTime = current;
			m_NextInfoTime.tv_sec += MOBA_SCOREBOARD_SECONDS;
		}

		if (!isMonsterAlive(m_RedBaseObjectID))
		{
			finishMatch(MOBA_TEAM_BLUE);
			return;
		}

		if (!isMonsterAlive(m_BlueBaseObjectID))
		{
			finishMatch(MOBA_TEAM_RED);
			return;
		}

		if (current > m_EndTime)
		{
			finishMatch(MOBA_TEAM_NONE);
			return;
		}
	}
	else if (m_State == MOBA_STATE_FINISHED)
	{
		if (current > m_WaitOutTime)
		{
			transportPlayersOut();
			clearMobaMonsters();
			resetMatch();
			m_Status = DYNAMIC_ZONE_STATUS_READY;
			filelog("Moba.log", "[heartbeat] zone=%u returned to READY", (uint)getZoneID());
		}
	}

	__END_CATCH
}

bool DynamicZoneMoba::spawnInitialMonsters()
{
	__BEGIN_TRY

	clearMobaMonsters();

	Monster* pRedBase = spawnMonster(2311, 217, 38, 2);
	if (pRedBase != NULL) m_RedBaseObjectID = pRedBase->getObjectID();

	Monster* pBlueBase = spawnMonster(2310, 38, 217, 2);
	if (pBlueBase != NULL) m_BlueBaseObjectID = pBlueBase->getObjectID();

	for (uint i = 0; i < sizeof(RED_STRUCTURES) / sizeof(RED_STRUCTURES[0]); ++i)
		spawnMonster(RED_STRUCTURES[i].MonsterType, RED_STRUCTURES[i].X, RED_STRUCTURES[i].Y, 2);

	for (uint i = 0; i < sizeof(BLUE_STRUCTURES) / sizeof(BLUE_STRUCTURES[0]); ++i)
		spawnMonster(BLUE_STRUCTURES[i].MonsterType, BLUE_STRUCTURES[i].X, BLUE_STRUCTURES[i].Y, 2);

	if (m_RedBaseObjectID == 0 || m_BlueBaseObjectID == 0)
	{
		filelog("Moba.log", "[spawnInitialMonsters] base spawn incomplete zone=%u red=%u blue=%u", (uint)getZoneID(), (uint)m_RedBaseObjectID, (uint)m_BlueBaseObjectID);
		return false;
	}

	return true;

	__END_CATCH
}

void DynamicZoneMoba::spawnWave()
{
	__BEGIN_TRY

	for (uint i = 0; i < sizeof(WAVE_SPAWNS) / sizeof(WAVE_SPAWNS[0]); ++i)
		spawnMonster(WAVE_SPAWNS[i].MonsterType, WAVE_SPAWNS[i].X, WAVE_SPAWNS[i].Y, 2);

	filelog("Moba.log", "[spawnWave] zone=%u wave spawned", (uint)getZoneID());

	__END_CATCH
}

Monster* DynamicZoneMoba::spawnMonster(MonsterType_t monsterType, ZoneCoord_t x, ZoneCoord_t y, Dir_t dir)
{
	__BEGIN_TRY

	if (m_pZone == NULL)
	{
		filelog("Moba.log", "[spawnMonster] null zone monster=%u", (uint)monsterType);
		return NULL;
	}

	Monster* pMonster = new Monster(monsterType);
	if (pMonster == NULL)
	{
		filelog("Moba.log", "[spawnMonster] allocation failed monster=%u zone=%u", (uint)monsterType, (uint)getZoneID());
		return NULL;
	}

	try
	{
		m_pZone->addCreature(pMonster, x, y, dir);
		return pMonster;
	}
	catch (Throwable& t)
	{
		filelog("Moba.log", "[spawnMonster] add failed monster=%u zone=%u x=%d y=%d error=%s",
			(uint)monsterType, (uint)getZoneID(), (int)x, (int)y, t.toString().c_str());
		SAFE_DELETE(pMonster);
		return NULL;
	}

	__END_CATCH
}

bool DynamicZoneMoba::isMonsterAlive(ObjectID_t objectID) const
{
	__BEGIN_TRY

	if (objectID == 0 || m_pZone == NULL || m_pZone->getMonsterManager() == NULL)
		return false;

	Creature* pCreature = m_pZone->getMonsterManager()->getCreature(objectID);
	return pCreature != NULL && !pCreature->isDead();

	__END_CATCH
}

void DynamicZoneMoba::finishMatch(int winnerTeam)
{
	__BEGIN_TRY

	if (m_State != MOBA_STATE_RUNNING)
		return;

	m_WinnerTeam = winnerTeam;
	m_State = MOBA_STATE_FINISHED;
	getCurrentTime(m_WaitOutTime);
	const MobaPlayConfig* pConfig = getLoadedMobaConfig();
	m_WaitOutTime.tv_sec += (pConfig != NULL) ? pConfig->MobaWaitOutTime : 30;

	if (!m_RewardGiven)
	{
		for (uint i = 0; i < m_Records.size(); ++i)
		{
			PlayerCreature* pPC = findOnlinePC(m_Records[i].Name);
			if (pPC == NULL)
				continue;

			if (winnerTeam != MOBA_TEAM_NONE && m_Records[i].Team == winnerTeam)
			{
				m_Records[i].Rewarded = giveReward(pPC);
			}
		}
		m_RewardGiven = true;
	}

	string message;
	if (winnerTeam == MOBA_TEAM_RED)
		message = "AOS Battle finished. Red team wins.";
	else if (winnerTeam == MOBA_TEAM_BLUE)
		message = "AOS Battle finished. Blue team wins.";
	else
		message = "AOS Battle finished. Draw.";

	sendMatchMessage(message);

	for (uint i = 0; i < m_Records.size(); ++i)
	{
		PlayerCreature* pPC = findOnlinePC(m_Records[i].Name);
		if (pPC != NULL && isSameZone(pPC, m_pZone))
			sendScoreboardTo(pPC, "");
	}

	filelog("Moba.log", "[finishMatch] zone=%u winnerTeam=%d", (uint)getZoneID(), winnerTeam);

	__END_CATCH
}

void DynamicZoneMoba::transportPlayersOut()
{
	__BEGIN_TRY

	MobaPlayConfig fallbackConfig;
	const MobaPlayConfig* pConfig = getLoadedMobaConfig();
	if (pConfig == NULL)
	{
		filelog("Moba.log", "[transportPlayersOut] MobaPlayConfig unavailable; using safe default exit coordinates");
		pConfig = &fallbackConfig;
	}

	for (uint i = 0; i < m_Records.size(); ++i)
	{
		PlayerCreature* pPC = findOnlinePC(m_Records[i].Name);
		if (pPC == NULL || !isSameZone(pPC, m_pZone))
			continue;

		if (pPC->isSlayer())
			transportCreature(pPC, pConfig->ExitZoneID, pConfig->ExitSlayerX, pConfig->ExitSlayerY, true);
		else if (pPC->isVampire())
			transportCreature(pPC, pConfig->ExitZoneID, pConfig->ExitVampireX, pConfig->ExitVampireY, true);
		else
			transportCreature(pPC, pConfig->ExitZoneID, pConfig->ExitOustersX, pConfig->ExitOustersY, true);
	}

	__END_CATCH
}

void DynamicZoneMoba::clearMobaMonsters()
{
	__BEGIN_TRY

	if (m_pZone != NULL && m_pZone->getMonsterManager() != NULL)
		m_pZone->getMonsterManager()->deleteAllMonsters();

	m_RedBaseObjectID = 0;
	m_BlueBaseObjectID = 0;

	__END_CATCH
}

void DynamicZoneMoba::sendMatchMessage(const string& message)
{
	__BEGIN_TRY

	for (uint i = 0; i < m_Records.size(); ++i)
	{
		PlayerCreature* pPC = findOnlinePC(m_Records[i].Name);
		if (pPC != NULL)
			sendSystemMessage(pPC, message);
	}

	__END_CATCH
}

void DynamicZoneMoba::sendSelfInfo(PlayerCreature* pPC)
{
	__BEGIN_TRY

	if (pPC == NULL || pPC->getPlayer() == NULL)
		return;

	MobaRecord* pRecord = findRecord(pPC);
	if (pRecord == NULL)
		return;

	GCMobaInfoSelf packet;
	packet.setLevel(pRecord->Level);
	packet.setKillPlayer(pRecord->KillPlayer);
	packet.setDead(pRecord->Dead);
	packet.setKillMonster(pRecord->KillMonster);
	packet.setTimeLeft(getTimeLeft());
	pPC->getPlayer()->sendPacket(&packet);

	__END_CATCH
}

void DynamicZoneMoba::sendScoreboardTo(PlayerCreature* pPC, const string& targetName)
{
	__BEGIN_TRY

	if (pPC == NULL || pPC->getPlayer() == NULL)
		return;

	GCMobaInfoAll packet;
	for (uint i = 0; i < m_Records.size(); ++i)
	{
		if (!targetName.empty() && m_Records[i].Name != targetName)
			continue;

		GCMobaInfoAll::Member member;
		member.Name = m_Records[i].Name;
		member.Body = m_Records[i].Body;
		member.Hair = m_Records[i].Hair;
		member.Level = m_Records[i].Level;
		member.KillPlayer = m_Records[i].KillPlayer;
		member.Dead = m_Records[i].Dead;
		member.KillMonster = m_Records[i].KillMonster;
		member.TeamCode = m_Records[i].Team;
		member.RaceCode = getRaceCode(m_Records[i].Race);

		if (m_WinnerTeam == MOBA_TEAM_NONE && m_State == MOBA_STATE_FINISHED)
			member.ResultCode = MOBA_RESULT_DRAW;
		else if (m_State == MOBA_STATE_FINISHED && m_Records[i].Team == m_WinnerTeam)
			member.ResultCode = MOBA_RESULT_WINNER;
		else if (m_State == MOBA_STATE_FINISHED)
			member.ResultCode = MOBA_RESULT_LOSER;
		else
			member.ResultCode = MOBA_RESULT_RUNNING;

		packet.addMember(member);
	}

	pPC->getPlayer()->sendPacket(&packet);

	__END_CATCH
}

bool DynamicZoneMoba::giveReward(PlayerCreature* pPC)
{
	__BEGIN_TRY

	if (pPC == NULL || pPC->getPlayer() == NULL)
		return false;

	DWORD rewardGoodsID = getMobaRewardGoodsID();
	Item* pItem = createItemByGoodsID(rewardGoodsID);
	if (pItem == NULL)
	{
		filelog("Moba.log", "[giveReward] invalid reward goodsID=%u player=%s", (uint)rewardGoodsID, pPC->getName().c_str());
		sendSystemMessage(pPC, "AOS Battle reward is not configured.");
		return false;
	}

	Inventory* pInventory = pPC->getInventory();
	Zone* pZone = pPC->getZone();
	TPOINT pt;

	if (pInventory != NULL && pZone != NULL && pInventory->getEmptySlot(pItem, pt))
	{
		pItem->setTraceItem(bTraceLog(pItem));
		pZone->registerObject(pItem);

		if (!pInventory->addItem(pt.x, pt.y, pItem))
		{
			SAFE_DELETE(pItem);
			filelog("Moba.log", "[giveReward] inventory add failed after preflight player=%s goodsID=%u", pPC->getName().c_str(), (uint)rewardGoodsID);
			return false;
		}

		pItem->create(pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y);
		pItem->whenPCTake(pPC);

		GCCreateItem gcCreateItem;
		makeGCCreateItem(&gcCreateItem, pItem, pt.x, pt.y);
		pPC->getPlayer()->sendPacket(&gcCreateItem);

		if (pItem->isTimeLimitItem())
		{
			unsigned long timeLimit = pItem->getHour();
			timeLimit *= 3600;
			pPC->addTimeLimitItem(pItem, timeLimit);
			pPC->sendTimeLimitItemInfo();
		}

		remainTraceLog(pItem, "AOSMoba", pPC->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
		sendSystemMessage(pPC, "AOS Battle reward has been added to your inventory.");
		return true;
	}

	SAFE_DELETE(pItem);

	Statement* pStmt = NULL;
	try
	{
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
			pStmt->executeQuery(
				"INSERT INTO GoodsListObject "
				"(BuyID, World, PlayerID, Name, GoodsID, Num, Status, `Count`) "
				"VALUES ('%s', %d, '%s', '%s', %u, %d, 'NOT', 0)",
				MOBA_REWARD_BUY_ID,
				g_pConfig->getPropertyInt("WorldID"),
				pPC->getPlayer()->getID().c_str(),
				pPC->getName().c_str(),
				(uint)rewardGoodsID,
				1
			);
			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
	}
	catch (Throwable& t)
	{
		filelog("Moba.log", "[giveReward] cart insert failed player=%s goodsID=%u error=%s", pPC->getName().c_str(), (uint)rewardGoodsID, t.toString().c_str());
		sendSystemMessage(pPC, "AOS Battle reward could not be delivered. Please contact an administrator.");
		return false;
	}

	pPC->loadGoods();
	if (pPC->getZone() != NULL)
		pPC->registerGoodsInventory(pPC->getZone()->getObjectRegistry());

	sendSystemMessage(pPC, "AOS Battle reward has been sent to Shop Basket because your inventory is full.");
	return true;

	__END_CATCH
}

DynamicZoneMoba::MobaRecord* DynamicZoneMoba::findRecord(const string& name)
{
	for (uint i = 0; i < m_Records.size(); ++i)
	{
		if (m_Records[i].Name == name)
			return &m_Records[i];
	}
	return NULL;
}

DynamicZoneMoba::MobaRecord* DynamicZoneMoba::findRecord(Creature* pCreature)
{
	if (pCreature == NULL)
		return NULL;
	return findRecord(pCreature->getName());
}

const DynamicZoneMoba::MobaRecord* DynamicZoneMoba::findRecord(Creature* pCreature) const
{
	if (pCreature == NULL)
		return NULL;
	for (uint i = 0; i < m_Records.size(); ++i)
	{
		if (m_Records[i].Name == pCreature->getName())
			return &m_Records[i];
	}
	return NULL;
}

void DynamicZoneMoba::recalcLevel(MobaRecord& record, bool byPlayerKill)
{
	if (byPlayerKill)
		record.Level = (record.KillPlayer / 5) + (record.KillMonster / 30);
	else
		record.Level = (record.KillMonster / 10) + (record.KillPlayer / 3);
}

PlayerCreature* DynamicZoneMoba::findOnlinePC(const string& name) const
{
	__BEGIN_TRY

	if (g_pPCFinder == NULL)
		return NULL;

	Creature* pCreature = g_pPCFinder->getCreature(name);
	if (pCreature == NULL || !pCreature->isPC())
		return NULL;

	return dynamic_cast<PlayerCreature*>(pCreature);

	__END_CATCH
}

uint DynamicZoneMoba::getTimeLeft() const
{
	Timeval current;
	getCurrentTime(current);
	if (current >= m_EndTime)
		return 0;
	return (uint)(m_EndTime.tv_sec - current.tv_sec);
}

void DynamicZoneMoba::OnAfterEnterZone(Creature* pCreature)
{
	MobaRecord* pRecord = findRecord(pCreature);
	if (pRecord != NULL)
		pRecord->ObjectID = pCreature->getObjectID();
}

void DynamicZoneMoba::OnBeforeLeaveZone(Creature* pCreature)
{
	MobaRecord* pRecord = findRecord(pCreature);
	if (pRecord != NULL)
		pRecord->ObjectID = 0;
}

bool DynamicZoneMoba::StartTestMatch(PlayerCreature* pStarter, string* pResultMessage)
{
	__BEGIN_TRY

	if (pResultMessage != NULL)
		pResultMessage->clear();

	if (pStarter == NULL || pStarter->getPlayer() == NULL)
	{
		if (pResultMessage != NULL) *pResultMessage = "AOS Battle test failed: invalid starter.";
		return false;
	}

	if (!(pStarter->isGOD() || pStarter->isDM()))
	{
		if (pResultMessage != NULL) *pResultMessage = "AOS Battle test failed: unauthorized.";
		return false;
	}

	if (getMobaZone(pStarter) != NULL)
	{
		if (pResultMessage != NULL) *pResultMessage = "AOS Battle test failed: already inside an AOS match.";
		return false;
	}

	const MobaPlayConfig* pConfig = getLoadedMobaConfig();
	if (pConfig == NULL || pConfig->Enable == 0)
	{
		if (pResultMessage != NULL) *pResultMessage = "AOS Battle test failed: configuration is unavailable or disabled.";
		return false;
	}

	try
	{
		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(MOBA_TEMPLATE_ZONE_ID);
		if (pZoneInfo == NULL)
			throw Error("AOS template zone missing");
	}
	catch (Throwable& t)
	{
		filelog("Moba.log", "[StartTestMatch] template zone unavailable zone=%u error=%s", (uint)MOBA_TEMPLATE_ZONE_ID, t.toString().c_str());
		if (pResultMessage != NULL) *pResultMessage = "AOS Battle test failed: template zone is unavailable.";
		return false;
	}

	if (g_pDynamicZoneManager == NULL)
	{
		if (pResultMessage != NULL) *pResultMessage = "AOS Battle test failed: DynamicZoneManager is unavailable.";
		return false;
	}

	DynamicZoneGroup* pGroup = g_pDynamicZoneManager->getDynamicZoneGroup(DYNAMIC_ZONE_MOBA);
	if (pGroup == NULL)
	{
		filelog("Moba.log", "[StartTestMatch] missing DynamicZoneGroup type=%d", DYNAMIC_ZONE_MOBA);
		if (pResultMessage != NULL) *pResultMessage = "AOS Battle test failed: dynamic zone group is unavailable.";
		return false;
	}

	Zone* pSourceZone = pStarter->getZone();
	if (pSourceZone == NULL || pSourceZone->getPCManager() == NULL)
	{
		if (pResultMessage != NULL) *pResultMessage = "AOS Battle test failed: current zone has no PC manager.";
		return false;
	}

	vector<PlayerCreature*> players;
	players.push_back(pStarter);

	const map<ObjectID_t, Creature*>& creatures = pSourceZone->getPCManager()->getCreatures();
	map<ObjectID_t, Creature*>::const_iterator itr = creatures.begin();
	for (; itr != creatures.end() && players.size() < (uint)pConfig->MobaMaxPC; ++itr)
	{
		PlayerCreature* pPC = (itr->second != NULL && itr->second->isPC()) ? dynamic_cast<PlayerCreature*>(itr->second) : NULL;
		if (pPC == NULL || pPC == pStarter || pPC->getPlayer() == NULL)
			continue;
		players.push_back(pPC);
	}

	DynamicZone* pZone = pGroup->getAvailableDynamicZone();
	DynamicZoneMoba* pMoba = dynamic_cast<DynamicZoneMoba*>(pZone);
	if (pMoba == NULL)
	{
		filelog("Moba.log", "[StartTestMatch] type mismatch dynamicZone=%p", (void*)pZone);
		if (pResultMessage != NULL) *pResultMessage = "AOS Battle test failed: dynamic zone type mismatch.";
		return false;
	}

	if (!pMoba->setupMatchInternal(players, true))
	{
		if (pResultMessage != NULL) *pResultMessage = "AOS Battle test failed: setup rejected the selected players.";
		return false;
	}

	for (uint i = 0; i < players.size(); ++i)
		sendQueueState(players[i], GCMobaJoin::QUEUE_STATE_STARTED, (BYTE)players.size(), (BYTE)pConfig->MobaMaxPC, 0);

	char buffer[128];
	snprintf(buffer, sizeof(buffer), "AOS Battle test started with %u player(s).", (uint)players.size());
	if (pResultMessage != NULL)
		*pResultMessage = buffer;

	filelog("Moba.log", "[StartTestMatch] starter=%s sourceZone=%u players=%u",
		pStarter->getName().c_str(), (uint)pSourceZone->getZoneID(), (uint)players.size());
	return true;

	__END_CATCH
}

void DynamicZoneMoba::JoinQueue(PlayerCreature* pPC)
{
	__BEGIN_TRY

	HeartbeatQueue();

	if (!validateJoin(pPC, true))
		return;

	ensureQueueMutexName();
	vector<string> snapshot;
	bool added = false;
	WORD remainingSeconds = 0;
	BYTE maxCount = 0;

	__ENTER_CRITICAL_SECTION(s_QueueMutex)

	const MobaPlayConfig* pConfig = getLoadedMobaConfig();
	if (pConfig != NULL && s_WaitingPlayers.empty())
	{
		Timeval current;
		getCurrentTime(current);
		startQueueDeadline(current);
	}

	if (find(s_WaitingPlayers.begin(), s_WaitingPlayers.end(), pPC->getName()) == s_WaitingPlayers.end())
	{
		s_WaitingPlayers.push_back(pPC->getName());
		added = true;
	}

	if (pConfig != NULL)
	{
		Timeval current;
		getCurrentTime(current);
		remainingSeconds = getQueueRemainingSeconds(current);
		maxCount = (BYTE)pConfig->MobaMaxPC;
	}

	snapshot = s_WaitingPlayers;

	__LEAVE_CRITICAL_SECTION(s_QueueMutex)

	if (added)
		filelog("Moba.log", "[JoinQueue] player=%s queueSize=%u", pPC->getName().c_str(), (uint)snapshot.size());

	broadcastQueueState(snapshot, GCMobaJoin::QUEUE_STATE_WAITING, (BYTE)snapshot.size(), maxCount, remainingSeconds);
	tryStartMatch();

	__END_CATCH
}

void DynamicZoneMoba::CancelQueue(PlayerCreature* pPC)
{
	__BEGIN_TRY

	if (pPC == NULL)
		return;

	ensureQueueMutexName();
	vector<string> snapshot;
	BYTE maxCount = 0;
	WORD remainingSeconds = 0;

	__ENTER_CRITICAL_SECTION(s_QueueMutex)

	const MobaPlayConfig* pConfig = getLoadedMobaConfig();
	if (pConfig != NULL)
		maxCount = (BYTE)pConfig->MobaMaxPC;

	vector<string>::iterator itr = find(s_WaitingPlayers.begin(), s_WaitingPlayers.end(), pPC->getName());
	if (itr != s_WaitingPlayers.end())
		s_WaitingPlayers.erase(itr);

	if (s_WaitingPlayers.empty())
	{
		clearQueueDeadline();
	}
	else
	{
		Timeval current;
		getCurrentTime(current);
		remainingSeconds = getQueueRemainingSeconds(current);
	}

	snapshot = s_WaitingPlayers;

	__LEAVE_CRITICAL_SECTION(s_QueueMutex)

	sendQueueState(pPC, GCMobaJoin::QUEUE_STATE_CANCELLED, 0, maxCount, 0);
	sendSystemMessage(pPC, "AOS Battle queue cancelled.");
	broadcastQueueState(snapshot, GCMobaJoin::QUEUE_STATE_WAITING, (BYTE)snapshot.size(), maxCount, remainingSeconds);
	filelog("Moba.log", "[CancelQueue] player=%s queueSize=%u", pPC->getName().c_str(), (uint)snapshot.size());

	__END_CATCH
}

void DynamicZoneMoba::tryStartMatch()
{
	__BEGIN_TRY

	const MobaPlayConfig* pConfig = getLoadedMobaConfig();
	if (pConfig == NULL || pConfig->Enable == 0)
		return;

	ensureQueueMutexName();
	vector<string> matchNames;
	vector<string> remainingSnapshot;
	WORD remainingSeconds = 0;

	__ENTER_CRITICAL_SECTION(s_QueueMutex)

	if (s_WaitingPlayers.size() >= (uint)pConfig->MobaMaxPC)
	{
		for (int i = 0; i < pConfig->MobaMaxPC; ++i)
			matchNames.push_back(s_WaitingPlayers[i]);
		s_WaitingPlayers.erase(s_WaitingPlayers.begin(), s_WaitingPlayers.begin() + pConfig->MobaMaxPC);

		if (s_WaitingPlayers.empty())
		{
			clearQueueDeadline();
		}
		else
		{
			Timeval current;
			getCurrentTime(current);
			startQueueDeadline(current);
			remainingSeconds = getQueueRemainingSeconds(current);
		}

		remainingSnapshot = s_WaitingPlayers;
	}

	__LEAVE_CRITICAL_SECTION(s_QueueMutex)

	if (matchNames.empty())
		return;

	vector<PlayerCreature*> players;
	for (uint i = 0; i < matchNames.size(); ++i)
	{
		Creature* pCreature = (g_pPCFinder != NULL) ? g_pPCFinder->getCreature(matchNames[i]) : NULL;
		PlayerCreature* pPC = (pCreature != NULL && pCreature->isPC()) ? dynamic_cast<PlayerCreature*>(pCreature) : NULL;
		if (pPC != NULL && validateJoin(pPC, true))
			players.push_back(pPC);
		else
			filelog("Moba.log", "[tryStartMatch] queued player invalid name=%s", matchNames[i].c_str());
	}

	if (players.size() != (uint)pConfig->MobaMaxPC)
	{
		filelog("Moba.log", "[tryStartMatch] cancelled start validPlayers=%u", (uint)players.size());
		for (uint i = 0; i < players.size(); ++i)
			sendQueueState(players[i], GCMobaJoin::QUEUE_STATE_CANCELLED, 0, (BYTE)pConfig->MobaMaxPC, 0);
		broadcastQueueState(remainingSnapshot, GCMobaJoin::QUEUE_STATE_WAITING, (BYTE)remainingSnapshot.size(), (BYTE)pConfig->MobaMaxPC, remainingSeconds);
		return;
	}

	try
	{
		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(MOBA_TEMPLATE_ZONE_ID);
		if (pZoneInfo == NULL)
			throw Error("AOS template zone missing");
	}
	catch (Throwable& t)
	{
		for (uint i = 0; i < players.size(); ++i)
		{
			sendQueueState(players[i], GCMobaJoin::QUEUE_STATE_CANCELLED, 0, (BYTE)pConfig->MobaMaxPC, 0);
			sendSystemMessage(players[i], "AOS Battle map data is unavailable.");
		}
		filelog("Moba.log", "[tryStartMatch] template zone unavailable zone=%u error=%s", (uint)MOBA_TEMPLATE_ZONE_ID, t.toString().c_str());
		return;
	}

	DynamicZoneGroup* pGroup = g_pDynamicZoneManager->getDynamicZoneGroup(DYNAMIC_ZONE_MOBA);
	if (pGroup == NULL)
	{
		for (uint i = 0; i < players.size(); ++i)
		{
			sendQueueState(players[i], GCMobaJoin::QUEUE_STATE_CANCELLED, 0, (BYTE)pConfig->MobaMaxPC, 0);
			sendSystemMessage(players[i], "AOS Battle dynamic zone is unavailable.");
		}
		filelog("Moba.log", "[tryStartMatch] missing DynamicZoneGroup type=%d", DYNAMIC_ZONE_MOBA);
		return;
	}

	DynamicZone* pZone = pGroup->getAvailableDynamicZone();
	DynamicZoneMoba* pMoba = dynamic_cast<DynamicZoneMoba*>(pZone);
	if (pMoba == NULL)
	{
		for (uint i = 0; i < players.size(); ++i)
		{
			sendQueueState(players[i], GCMobaJoin::QUEUE_STATE_CANCELLED, 0, (BYTE)pConfig->MobaMaxPC, 0);
			sendSystemMessage(players[i], "AOS Battle dynamic zone type mismatch.");
		}
		filelog("Moba.log", "[tryStartMatch] type mismatch dynamicZone=%p", (void*)pZone);
		return;
	}

	if (!pMoba->setupMatch(players))
	{
		for (uint i = 0; i < players.size(); ++i)
		{
			sendQueueState(players[i], GCMobaJoin::QUEUE_STATE_CANCELLED, 0, (BYTE)pConfig->MobaMaxPC, 0);
			sendSystemMessage(players[i], "AOS Battle could not start. Please try again later.");
		}
	}
	else
	{
		for (uint i = 0; i < players.size(); ++i)
			sendQueueState(players[i], GCMobaJoin::QUEUE_STATE_STARTED, (BYTE)pConfig->MobaMaxPC, (BYTE)pConfig->MobaMaxPC, 0);
	}

	broadcastQueueState(remainingSnapshot, GCMobaJoin::QUEUE_STATE_WAITING, (BYTE)remainingSnapshot.size(), (BYTE)pConfig->MobaMaxPC, remainingSeconds);

	__END_CATCH
}

bool DynamicZoneMoba::validateJoin(PlayerCreature* pPC, bool sendMessage, bool testMode)
{
	__BEGIN_TRY

	if (pPC == NULL || pPC->getPlayer() == NULL)
		return false;

	if (g_pVariableManager == NULL || !g_pVariableManager->isMobaPlayConfigValid())
	{
		if (sendMessage) sendSystemMessage(pPC, "AOS Battle configuration is unavailable.");
		return false;
	}

	const MobaPlayConfig& config = g_pVariableManager->getMobaPlayConfig();
	if (config.Enable == 0)
	{
		if (sendMessage) sendSystemMessage(pPC, "AOS Battle is disabled.");
		return false;
	}

	if (!testMode && pPC->getAdvancementClassLevel() < config.MobaMinAdvancementLevel)
	{
		if (sendMessage) sendSystemMessage(pPC, "AOS Battle requires a higher Advancement Level.");
		return false;
	}

	if (pPC->getPartyID() != 0)
	{
		if (sendMessage) sendSystemMessage(pPC, "Leave your party before joining AOS Battle.");
		return false;
	}

	if (!testMode)
	{
		int todayPlayCount = getTodayPlayCount(pPC);
		if (todayPlayCount < 0)
		{
			if (sendMessage) sendSystemMessage(pPC, "AOS Battle play limit data is unavailable.");
			return false;
		}

		if (todayPlayCount >= config.MobaMaxDailyPlays)
		{
			if (sendMessage) sendSystemMessage(pPC, "AOS Battle daily play limit reached.");
			return false;
		}
	}

	return true;

	__END_CATCH
}

int DynamicZoneMoba::getTodayPlayCount(PlayerCreature* pPC)
{
	if (pPC == NULL || pPC->getPlayer() == NULL)
		return -1;

	Statement* pStmt = NULL;
	Result* pResult = NULL;
	int playCount = 0;

	try
	{
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
			pResult = pStmt->executeQuery(
				"SELECT PlayTimes, IF(DATE(LastPlayDate)=CURDATE(),1,0) FROM MobaPlayLimit WHERE PlayerID='%s'",
				pPC->getPlayer()->getID().c_str()
			);

			if (pResult->next())
			{
				if (pResult->getInt(2) == 1)
					playCount = pResult->getInt(1);
			}

			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
	}
	catch (Throwable& t)
	{
		filelog("Moba.log", "[getTodayPlayCount] DB failed player=%s error=%s", pPC->getPlayer()->getID().c_str(), t.toString().c_str());
		return -1;
	}

	return playCount;
}

bool DynamicZoneMoba::increaseTodayPlayCount(PlayerCreature* pPC)
{
	if (pPC == NULL || pPC->getPlayer() == NULL)
		return false;

	Statement* pStmt = NULL;

	try
	{
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
			pStmt->executeQuery(
				"INSERT INTO MobaPlayLimit (PlayerID, PlayTimes, LastPlayDate) "
				"VALUES ('%s', 1, now()) "
				"ON DUPLICATE KEY UPDATE PlayTimes=IF(DATE(LastPlayDate)=CURDATE(),PlayTimes+1,1), LastPlayDate=now()",
				pPC->getPlayer()->getID().c_str()
			);
			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)
	}
	catch (Throwable& t)
	{
		filelog("Moba.log", "[increaseTodayPlayCount] DB failed player=%s error=%s", pPC->getPlayer()->getID().c_str(), t.toString().c_str());
		return false;
	}

	return true;
}

void DynamicZoneMoba::HeartbeatQueue()
{
	__BEGIN_TRY

	ensureQueueMutexName();

	vector<string> expiredPlayers;
	BYTE maxCount = 0;

	__ENTER_CRITICAL_SECTION(s_QueueMutex)

	const MobaPlayConfig* pConfig = getLoadedMobaConfig();
	if (pConfig != NULL)
		maxCount = (BYTE)pConfig->MobaMaxPC;

	if (!s_WaitingPlayers.empty() && s_QueueHasDeadline)
	{
		Timeval current;
		getCurrentTime(current);
		if (current >= s_QueueExpireTime)
		{
			expiredPlayers = s_WaitingPlayers;
			s_WaitingPlayers.clear();
			clearQueueDeadline();
		}
	}

	__LEAVE_CRITICAL_SECTION(s_QueueMutex)

	if (expiredPlayers.empty())
		return;

	for (uint i = 0; i < expiredPlayers.size(); ++i)
	{
		Creature* pCreature = (g_pPCFinder != NULL) ? g_pPCFinder->getCreature(expiredPlayers[i]) : NULL;
		PlayerCreature* pPC = (pCreature != NULL && pCreature->isPC()) ? dynamic_cast<PlayerCreature*>(pCreature) : NULL;
		if (pPC == NULL)
			continue;

		sendQueueState(pPC, GCMobaJoin::QUEUE_STATE_TIMEOUT, 0, maxCount, 0);
		sendSystemMessage(pPC, "AOS Battle queue timed out.");
	}

	filelog("Moba.log", "[HeartbeatQueue] timeout cancelled queue players=%u", (uint)expiredPlayers.size());

	__END_CATCH
}

void DynamicZoneMoba::sendQueueState(PlayerCreature* pPC, BYTE state, BYTE joinedCount, BYTE maxCount, WORD remainingSeconds)
{
	if (pPC == NULL || pPC->getPlayer() == NULL)
		return;

	GCMobaJoin packet;
	packet.setState(state);
	packet.setJoinedCount(joinedCount);
	packet.setMaxCount(maxCount);
	packet.setRemainingSeconds(remainingSeconds);
	pPC->getPlayer()->sendPacket(&packet);
}

void DynamicZoneMoba::sendSystemMessage(PlayerCreature* pPC, const string& message)
{
	if (pPC == NULL || pPC->getPlayer() == NULL)
		return;

	GCSystemMessage packet;
	packet.setType(SYSTEM_MESSAGE_NORMAL);
	packet.setMessage(message);
	pPC->getPlayer()->sendPacket(&packet);
}

void DynamicZoneMoba::broadcastQueueState(const vector<string>& names, BYTE state, BYTE joinedCount, BYTE maxCount, WORD remainingSeconds)
{
	for (uint i = 0; i < names.size(); ++i)
	{
		Creature* pCreature = (g_pPCFinder != NULL) ? g_pPCFinder->getCreature(names[i]) : NULL;
		PlayerCreature* pPC = (pCreature != NULL && pCreature->isPC()) ? dynamic_cast<PlayerCreature*>(pCreature) : NULL;
		if (pPC != NULL)
			sendQueueState(pPC, state, joinedCount, maxCount, remainingSeconds);
	}
}

void DynamicZoneMoba::SendScoreboard(PlayerCreature* pPC, const string& targetName)
{
	__BEGIN_TRY

	DynamicZoneMoba* pMoba = getMobaZone(pPC);
	if (pMoba == NULL)
	{
		sendSystemMessage(pPC, "AOS Battle scoreboard is available only inside an AOS match.");
		return;
	}

	pMoba->sendScoreboardTo(pPC, targetName);

	__END_CATCH
}

bool DynamicZoneMoba::IsSameTeam(Creature* pAttacker, Creature* pDefender)
{
	DynamicZoneMoba* pMoba = getMobaZone(pAttacker);
	if (pMoba == NULL || pMoba != getMobaZone(pDefender))
		return false;

	const MobaRecord* pAttackerRecord = pMoba->findRecord(pAttacker);
	const MobaRecord* pDefenderRecord = pMoba->findRecord(pDefender);
	return pAttackerRecord != NULL
		&& pDefenderRecord != NULL
		&& pAttackerRecord->Team != MOBA_TEAM_NONE
		&& pAttackerRecord->Team == pDefenderRecord->Team;
}

void DynamicZoneMoba::HandleKill(Creature* pAttacker, Creature* pDeadCreature)
{
	__BEGIN_TRY

	DynamicZoneMoba* pMoba = getMobaZone(pAttacker);
	if (pMoba == NULL || pMoba != getMobaZone(pDeadCreature))
		return;

	MobaRecord* pAttackerRecord = pMoba->findRecord(pAttacker);
	if (pAttackerRecord == NULL)
		return;

	if (pDeadCreature != NULL && pDeadCreature->isPC())
	{
		if (IsSameTeam(pAttacker, pDeadCreature))
			return;

		pAttackerRecord->KillPlayer++;
		pMoba->recalcLevel(*pAttackerRecord, true);

		MobaRecord* pDeadRecord = pMoba->findRecord(pDeadCreature);
		if (pDeadRecord != NULL)
			pDeadRecord->Dead++;
	}
	else if (pDeadCreature != NULL && pDeadCreature->isMonster())
	{
		pAttackerRecord->KillMonster++;
		pMoba->recalcLevel(*pAttackerRecord, false);
	}

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pAttacker);
	if (pPC != NULL)
		pMoba->sendSelfInfo(pPC);

	__END_CATCH
}

Damage_t DynamicZoneMoba::ScaleDamage(Creature* pAttacker, Creature* pTarget, Damage_t damage)
{
	DynamicZoneMoba* pMoba = getMobaZone(pAttacker);
	if (pMoba == NULL || pMoba != getMobaZone(pTarget))
		return damage;

	const MobaRecord* pRecord = pMoba->findRecord(pAttacker);
	if (pRecord == NULL || pRecord->Level == 0)
		return damage;

	double scaled = (double)damage * (1.0 + ((double)pRecord->Level / 10.0));
	if (scaled < 0)
		return damage;
	return (Damage_t)scaled;
}

DynamicZoneMoba* DynamicZoneMoba::getMobaZone(Creature* pCreature)
{
	if (pCreature == NULL || pCreature->getZone() == NULL || !pCreature->getZone()->isDynamicZone())
		return NULL;

	DynamicZone* pDynamicZone = pCreature->getZone()->getDynamicZone();
	if (pDynamicZone == NULL || pDynamicZone->getTemplateZoneID() != MOBA_TEMPLATE_ZONE_ID)
		return NULL;

	return dynamic_cast<DynamicZoneMoba*>(pDynamicZone);
}

DEFINE_DYNAMIC_ZONE_FACTORY(DynamicZoneMoba, DYNAMIC_ZONE_MOBA)
