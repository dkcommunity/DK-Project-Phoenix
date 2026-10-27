#ifndef __GDR_LAIR_MANAGER_H__
#define __GDR_LAIR_MANAGER_H__

#include "Types.h"
#include "FiniteStateMachine.h"
#include "GDRLairAbstractStates.h"
#include "Thread.h"

#include <vector>

class Monster;

// GDRLairSummonWave tablosundan gelen tek spawn slot'u (bir wave icindeki bir type).
struct GDRLairSummonSlot
{
	MonsterType_t monsterType;
	int monsterNum;
	ZoneCoord_t x;
	ZoneCoord_t y;
};

enum GDRLairStatus
{
	GDR_LAIR_IDLE = 1,
	GDR_LAIR_ENTRANCE,
	GDR_LAIR_ILLUSIONS_WAY_ONLY,
	GDR_LAIR_ICEPOLE,
	GDR_LAIR_SCENE_1,
	GDR_LAIR_SUMMON_MONSTER,
	GDR_LAIR_SCENE_2,
	GDR_LAIR_SUMMON_GDR_DUP,
	GDR_LAIR_SCENE_3,
	GDR_LAIR_GDR_FIGHT,
	GDR_LAIR_SCENE_4,
	GDR_LAIR_AWAKENED_GDR_FIGHT,
	GDR_LAIR_SCENE_5,
	GDR_LAIR_MINION_FIGHT,
	GDR_LAIR_SCENE_6,
	GDR_LAIR_ENDING,
	GDR_LAIR_KILL_ALL,
};

// ???. ????? ???? ??? ?????? ??? ???? ???????.
class GDRLairIdle : public SetTimeState
{
public:
	DWORD	getStateType() const { return GDR_LAIR_IDLE; }
	GDRLairIdle( const VSDateTime& date ) : SetTimeState( GDR_LAIR_ENTRANCE, date ) { }

	void start();

	State*	clone() { return new GDRLairIdle( getTimer() ); }
	string	toString() const { return "GDRLairIdle"; }
private:
};

// 20???? ???? ?????? ??????? ???? ?????? ???????.
class GDRLairEntrance : public TimerState
{
public:
	DWORD	getStateType() const { return GDR_LAIR_ENTRANCE; }
	GDRLairEntrance() : TimerState( GDR_LAIR_ICEPOLE, 1210 ) { }

	void start();
	DWORD heartbeat(Timeval currentTime);

	State*	clone() { return new GDRLairEntrance(); }
	string	toString() const { return "GDRLairEntrance"; }
};

// ???? ????? ???? ????? 5???? ??????? ????? ????? ?? ??? ?????? ???.
// ??? 5?? ??????????? ??????? ????? ?????? ?????? ??? Icepole
// State?? ?????.
/*class GDRLairIllusionsWayOnly : public TimerState
{
public:
	DWORD	getStateType() const { return GDR_LAIR_ILLUSIONS_WAY_ONLY; }
	GDRLairIllusionsWayOnly() : TimerState( GDR_LAIR_ICEPOLE, 600 ) { }
	DWORD	heartbeat(Timeval currentTime);

	void start();
	void end();

	State*	clone() { return new GDRLairIllusionsWayOnly(); }
	string	toString() const { return "GDRLairIllusionsWayOnly"; }
};*/

// ??????? ????? ????? ??????? ???? ???? ????? ?? ?? ??? ??????
// ???. ?????? ?? ???? ????? ?????? ???? ??? ?????? ?? ????? 1?????
// ???????? ??? ??????? ???????.. (--)
class GDRLairIcepole : public State
{
public:
	void start();
	DWORD	getStateType() const { return GDR_LAIR_ICEPOLE; }
	DWORD	heartbeat(Timeval currentTime);

	State*	clone() { return new GDRLairIcepole(); }
	string	toString() const { return "GDRLairIcepole"; }

private:
	Timeval	m_BroadcastTime;
};

// ???? ????? ????? ???? ???? ?????? ???????.
// ?????? ?????? ??????? ?????? ???? ??? 1?? ???.
class GDRLairScene1 : public GDRScene
{
public:
	GDRLairScene1() : GDRScene( GDR_LAIR_SUMMON_MONSTER ) { }
	void	start();
	DWORD	getStateType() const { return GDR_LAIR_SCENE_1; }
	void	end();

	State*	clone() { return new GDRLairScene1(); }
	string	toString() const { return "GDRLairScene1"; }
};

// ???? ???? ??? ????? 10? ??????.
// ?????? ???? ???????? Idle??
// 10? ????? ????? ?????? ???? ?????? Scene2??
class GDRLairSummonMonster : public MonsterSummonState
{
public:
	GDRLairSummonMonster();
	DWORD	getStateType() const { return GDR_LAIR_SUMMON_MONSTER; }

	void	start();
	void	end();

	State*	clone() { return new GDRLairSummonMonster(); }
	string	toString() const { return "GDRLairSummonMonster"; }
};

// ?????? ??? 2?? ??? ????? ???? ?? ???? ????? ????? ??
// ??? 3?? ???.
class GDRLairScene2 : public GDRScene
{
public:
	GDRLairScene2() : GDRScene( GDR_LAIR_SUMMON_GDR_DUP ) { }
	void	start();
	DWORD	getStateType() const { return GDR_LAIR_SCENE_2; }
	void	end();

	State*	clone() { return new GDRLairScene2(); }
	string	toString() const { return "GDRLairScene2"; }
};

// ?????? ?????? ??????.
class GDRLairSummonGDRDup : public MonsterSummonState
{
public:
	GDRLairSummonGDRDup();
	DWORD	getStateType() const { return GDR_LAIR_SUMMON_GDR_DUP; }

	void	start();
	void	end();

	State*	clone() { return new GDRLairSummonGDRDup(); }
	string	toString() const { return "GDRLairSummonGDRDup"; }
};

// ?????? ??? 4?? ??? ??????? ???????.
class GDRLairScene3 : public GDRScene
{
public:
	GDRLairScene3() : GDRScene( GDR_LAIR_GDR_FIGHT ) { }
	void	start();
	DWORD	getStateType() const { return GDR_LAIR_SCENE_3; }
	void	end();

	State*	clone() { return new GDRLairScene3(); }
	string	toString() const { return "GDRLairScene3"; }
};

// ?????? ??????? ????????? ?????.
class GDRLairGDRFight : public State
{
public:
	GDRLairGDRFight() : State()
	{
		getCurrentTime(m_previousTime);
	}
	
	DWORD	getStateType() const { return GDR_LAIR_GDR_FIGHT; }
	void	start();
	DWORD	heartbeat(Timeval currentTime);
	void	end();

	State*	clone() { return new GDRLairGDRFight(); }
	string	toString() const { return "GDRLairGDRFight"; }
	
protected :
	Timeval m_previousTime;
};

// ?????? ??? 5?? ??? ???? ?????? ??????.
// ???? ???? ???? ??????? ???? ????????
// ???? ??????? ??? 6?? ???.
class GDRLairScene4 : public GDRScene
{
public:
	GDRLairScene4() : GDRScene( GDR_LAIR_AWAKENED_GDR_FIGHT ) { }
	void	start();
	DWORD	getStateType() const { return GDR_LAIR_SCENE_4; }

	State*	clone() { return new GDRLairScene4(); }
	string	toString() const { return "GDRLairScene4"; }
};

// ???? ??????? ??????? ????????? ?????.
// HP?? 50% ????? ???????? Scene5??, ?????? Scene6????.
class GDRLairAwakenedGDRFight : public State
{
	bool	m_bGDRDamaged;
public:
	GDRLairAwakenedGDRFight(bool damaged = false) : m_bGDRDamaged(damaged) 
	{ 
		getCurrentTime(m_previousTime);
	}
	DWORD	getStateType() const { return GDR_LAIR_AWAKENED_GDR_FIGHT; }
	void	start();
	DWORD	heartbeat(Timeval currentTime);

	State*	clone() { return new GDRLairAwakenedGDRFight(m_bGDRDamaged); }
	string	toString() const { return "GDRLairAwakenedGDRFight"; }
	
protected :
	Timeval m_previousTime;	
};

// ??? 7?? ??? ???? ????? ????? ?? ??? 8?? ???.
class GDRLairScene5 : public GDRScene
{
public:
	GDRLairScene5() : GDRScene( GDR_LAIR_MINION_FIGHT ) { }
	void	start();
	DWORD	getStateType() const { return GDR_LAIR_SCENE_5; }

	State*	clone() { return new GDRLairScene5(); }
	string	toString() const { return "GDRLairScene5"; }
};

// ????????? ????? ???? ??? ???? ???.
class GDRLairMinionFight : public MonsterSummonState
{
public:
	GDRLairMinionFight();
	DWORD	getStateType() const { return GDR_LAIR_MINION_FIGHT; }

	void	start();
	void	end();

	State*	clone() { return new GDRLairMinionFight(); }
	string	toString() const { return "GDRLairMinionFight"; }
};

// ??? 9?? ??? ???? ????? ????? ?? ??? 10?? ???.
class GDRLairScene6 : public GDRScene
{
public:
	GDRLairScene6() : GDRScene( GDR_LAIR_AWAKENED_GDR_FIGHT ) { }
	void	start();
	DWORD	getStateType() const { return GDR_LAIR_SCENE_6; }

	State*	clone() { return new GDRLairScene6(); }
	string	toString() const { return "GDRLairScene6"; }
};

// ?????? ??? ??? ???? ???????? ???????.
// ??? ???? ?? ?? ???? -_-
class GDRLairEnding : public TimerState
{
public:
	GDRLairEnding() : TimerState( GDR_LAIR_IDLE, 10 ) { }
	DWORD	getStateType() const { return GDR_LAIR_ENDING; }

	void	start();
	void	end();

	State*	clone() { return new GDRLairEnding(); }
	string	toString() const { return "GDRLairEnding"; }
};

// ??????. ?? ????
class GDRLairKillAll : public TimerState
{
public:
	GDRLairKillAll() : TimerState( GDR_LAIR_IDLE, 10 ) { }
	DWORD	getStateType() const { return GDR_LAIR_KILL_ALL; }

	void	start();
//	DWORD	heartbeat(Timeval currentTime) { return GDR_LAIR_IDLE; }
	void	end();

	State*	clone() { return new GDRLairKillAll(); }
	string	toString() const { return "GDRLairKillAll"; }
};

// ???? ???? ?????? ?????. ClientManager?? ???????? ????.
class GDRLairManager : public FiniteStateMachine, public Thread
{
public:
	enum GDRLairZones
	{
		ILLUSIONS_WAY_1,
		ILLUSIONS_WAY_2,
		GDR_LAIR,
		GDR_LAIR_CORE,

		GDR_LAIR_MAX
	};

	void init();
	void clear();
	void run() throw();
	string getName() const throw() { return "GDRLairManager"; }

	static GDRLairManager& Instance() { static GDRLairManager theInstance; return theInstance; }
	virtual ~GDRLairManager() throw() {}
	Zone* getZone(int index) const { return m_pZones[index]; }

	Monster*	getGDR() const { return m_pGDR; }
	void		setGDR(Monster* pGDR) { m_pGDR = pGDR; }

	VSDateTime getNextOpenTime() const;

	BYTE		getCorrectPortal() const { return m_CorrectPortal; }
	void		setCorrectPortal(BYTE pid) { m_CorrectPortal = pid; }

	int			getTotalPCs() const;
	bool		isGDRLairZone(ZoneID_t ZoneID) const;

	void		open() { m_bCanEnter = true; }
	void		close() { m_bCanEnter = false; }
	bool		canEnter() const { return m_bCanEnter; }

	// GDRLairSummonWave SQL. Basarili load cache'i degistirir; query fail eski cache'i korur.
	bool		loadSummonWaves();
	const std::vector<std::vector<GDRLairSummonSlot> >& getSummonWaves() const { return m_SummonWaves; }

	friend class GDRScene;
private:
	void		applyDefaultSummonWaves();

	Monster*	m_pGDR;
	Zone*		m_pZones[GDR_LAIR_MAX];
	BYTE		m_CorrectPortal;
	bool		m_bCanEnter;
	std::vector<std::vector<GDRLairSummonSlot> > m_SummonWaves;
};

#endif
