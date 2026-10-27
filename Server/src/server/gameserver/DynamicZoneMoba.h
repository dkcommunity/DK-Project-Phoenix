//////////////////////////////////////////////////////////////////////////////
// DynamicZoneMoba.h
//////////////////////////////////////////////////////////////////////////////

#ifndef __DYNAMIC_ZONE_MOBA_H__
#define __DYNAMIC_ZONE_MOBA_H__

#include "DynamicZone.h"
#include "Types.h"
#include <vector>

class PlayerCreature;

//////////////////////////////////////////////////////////////////////////////
// class DynamicZoneMoba
//////////////////////////////////////////////////////////////////////////////
class DynamicZoneMoba : public DynamicZone
{
public:
	enum MobaTeam
	{
		MOBA_TEAM_NONE = 0,
		MOBA_TEAM_BLUE = 1,
		MOBA_TEAM_RED = 2
	};

	enum MobaResult
	{
		MOBA_RESULT_RUNNING = 0,
		MOBA_RESULT_WINNER = 1,
		MOBA_RESULT_LOSER = 2,
		MOBA_RESULT_DRAW = 3
	};

	struct MobaRecord
	{
		string Name;
		ObjectID_t ObjectID;
		Race_t Race;
		BYTE Body;
		BYTE Hair;
		int Team;
		uint Level;
		uint KillPlayer;
		uint Dead;
		uint KillMonster;
		bool Rewarded;

		MobaRecord()
			: ObjectID(0), Race((Race_t)0), Body(0), Hair(0), Team(MOBA_TEAM_NONE),
			  Level(0), KillPlayer(0), Dead(0), KillMonster(0), Rewarded(false) {}
	};

public:
	DynamicZoneMoba();
	virtual ~DynamicZoneMoba();

	void init();
	void heartbeat();

	bool setupMatch(const vector<PlayerCreature*>& players);

	static void JoinQueue(PlayerCreature* pPC);
	static void CancelQueue(PlayerCreature* pPC);
	static void HeartbeatQueue();
	static bool StartTestMatch(PlayerCreature* pStarter, string* pResultMessage = NULL);
	static void SendScoreboard(PlayerCreature* pPC, const string& targetName);
	static bool IsSameTeam(Creature* pAttacker, Creature* pDefender);
	static void HandleKill(Creature* pAttacker, Creature* pDeadCreature);
	static Damage_t ScaleDamage(Creature* pAttacker, Creature* pTarget, Damage_t damage);

protected:
	virtual void OnAfterEnterZone(Creature* pCreature);
	virtual void OnBeforeLeaveZone(Creature* pCreature);

private:
	bool setupMatchInternal(const vector<PlayerCreature*>& players, bool testMode);
	void resetMatch();
	bool spawnInitialMonsters();
	void spawnWave();
	Monster* spawnMonster(MonsterType_t monsterType, ZoneCoord_t x, ZoneCoord_t y, Dir_t dir);
	bool isMonsterAlive(ObjectID_t objectID) const;
	void finishMatch(int winnerTeam);
	void transportPlayersOut();
	void clearMobaMonsters();
	void sendMatchMessage(const string& message);
	void sendSelfInfo(PlayerCreature* pPC);
	void sendScoreboardTo(PlayerCreature* pPC, const string& targetName);
	bool giveReward(PlayerCreature* pPC);
	MobaRecord* findRecord(const string& name);
	MobaRecord* findRecord(Creature* pCreature);
	const MobaRecord* findRecord(Creature* pCreature) const;
	void recalcLevel(MobaRecord& record, bool byPlayerKill);
	PlayerCreature* findOnlinePC(const string& name) const;
	uint getTimeLeft() const;

	static bool validateJoin(PlayerCreature* pPC, bool sendMessage, bool testMode = false);
	static int getTodayPlayCount(PlayerCreature* pPC);
	static bool increaseTodayPlayCount(PlayerCreature* pPC);
	static void sendQueueState(PlayerCreature* pPC, BYTE state, BYTE joinedCount, BYTE maxCount, WORD remainingSeconds);
	static void sendSystemMessage(PlayerCreature* pPC, const string& message);
	static void broadcastQueueState(const vector<string>& names, BYTE state, BYTE joinedCount, BYTE maxCount, WORD remainingSeconds);
	static void tryStartMatch();
	static DynamicZoneMoba* getMobaZone(Creature* pCreature);

private:
	vector<MobaRecord> m_Records;
	ObjectID_t m_RedBaseObjectID;
	ObjectID_t m_BlueBaseObjectID;
	Timeval m_EndTime;
	Timeval m_NextWaveTime;
	Timeval m_NextInfoTime;
	Timeval m_WaitOutTime;
	int m_State;
	int m_WinnerTeam;
	bool m_RewardGiven;
};

DECLARE_DYNAMIC_ZONE_FACTORY(DynamicZoneMoba)

#endif
