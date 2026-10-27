//////////////////////////////////////////////////////////////////////////////
// Filename    : EventSurvivalZoneManager.h 
// Written by  : 쉭
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __EVENT_SURVIVAL_ZONE_MANAGER_H__
#define __EVENT_SURVIVAL_ZONE_MANAGER_H__

#include "Timeval.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class EventSurvivalZoneManager
//////////////////////////////////////////////////////////////////////////////

class Zone;
class Creature;
class PlayerCreature;

class EventSurvivalZoneManager
{
public : 
	enum EventSurvivalZoneState
	{ 
		STATE_CLOSE,
		STATE_OPEN,		// 입장을 허가한다.
		STATE_START,	// 서바이벌을 시작한다.

		STATE_MAX
	}; 


public:
	EventSurvivalZoneManager(Zone* pZone) throw(Error);
	~EventSurvivalZoneManager() throw();

	EventSurvivalZoneState getCurrentState() const { return m_State; }

	bool enterPC(PlayerCreature* pPC) throw(Error);  // 존에 출입이 가능한가? 
	bool leaveCreature(Creature* pCreature) throw(Error);  // 존에서 나간 경우

	void resetStateTime() { m_StateTime.tv_sec = 0; }

	void startEvent() throw(Error);

	bool isOpenZone() { return m_bStartEvent; }

	bool heartbeat() throw (Error);

	void lock() throw(Error) { m_Mutex.lock(); }
    void ulnock() throw(Error) { m_Mutex.unlock(); }

	void setRace( Race_t race )		{ m_Race = race; }
	Race_t getRace()				{ return m_Race; }

	void setRaceSurvival(bool racesurvival) { m_bRace = racesurvival; }
	bool getRaceSurvival()			{ return m_bRace; }

	void setEdgearchState(int step)	{ m_EdgearchMessageStep = step; }

	void setStandByEffectTime(Timeval time)	{ m_StandByEffectTime.tv_sec = time.tv_sec; }

	void initStandByEffectStep()	{ m_StandByEffectStep = 0; }

	string toString() const throw(Error);

protected :
	void processOpenState() throw (Error);	// 입장이 가능한 상태
	void processCloseState() throw (Error);	// 열려있지 않은 상태
	void processStartState() throw (Error); // 서바이벌 중인 상태

	void kickOutAllPC() throw (Error);		// 플레이어 모두 쫓아내기
	void setStateClose() throw (Error);		// State Close 상태로 만든다.
	void broadcastEdgearchMessage( int minute ) throw (Error);
	void broadcastSurvivalLiveMessage( int pc ) throw (Error);
	void fixTimeband(uint timeband) throw (Error);
	void resumeTimeband() throw (Error);
	void releaseAllSafeZone() throw (Error);
	void resetAllSafeZone() throw (Error);

private : 
	Zone*				m_pZone;

	int					m_ClosingMessageStep;
	int					m_SurvivalLiveMessageStep;
	int					m_EdgearchMessageStep;
	int					m_SurvivalEnterMessageStep;

	Race_t				m_Race;					// 출입이 허가된 종족
	bool				m_bRace;				// 종족별 전투인가?

	int					m_nMaxPassPlayer;		// 최대 출입 사용자 수
	int					m_nPassPlayer;			// Pass를 받은 사람 수 

	bool				m_bStartEvent;			// 존이 입장 가능한지를 체크

	EventSurvivalZoneState	m_State;            // 현재의 State
	Timeval				m_StateTime;         	// 현재의 State 지속될 시간
	Timeval				m_StandByEffectTime;	// 대기 시간
	int					m_StandByEffectStep;

	mutable Mutex		m_Mutex;				// m_nPassPlayer를 확실히 체크할려고..
};

#endif
