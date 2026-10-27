//////////////////////////////////////////////////////////////////////////////
// Filename    : MikllizzLairManager.h 
// Written by  : 쉭
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __MIKLLIZZ_LAIR_MANAGER_H__
#define __MIKLLIZZ_LAIR_MANAGER_H__

#include "Timeval.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class MikllizzLairManager
//////////////////////////////////////////////////////////////////////////////

class Zone;
class Creature;
class PlayerCreature;

class MikllizzLairManager
{
public : 
	enum MikllizzLairState
	{ 
		STATE_CLOSE,
		STATE_TANTACLE_COMBAT,	// 촉수와의 전투. 입장 시간
		STATE_MIKLLIZZ_COMBAT,	// 미클리즈와의 싸움

		STATE_MAX
	}; 


public:
	MikllizzLairManager(Zone* pZone) throw(Error);
	~MikllizzLairManager() throw();

	MikllizzLairState getCurrentState() const { return m_State; }

	bool isSummonTiming() throw(Error);

	bool enterPC(PlayerCreature* pPC) throw(Error);  // 존에 출입이 가능한가? 
	bool leaveCreature(Creature* pCreature) throw(Error);  // 존에서 나간 경우

	void resetStateTime() { m_StateTime.tv_sec = 0; }

	bool heartbeat() throw (Error);

	void lock() throw(Error) { m_Mutex.lock(); }
    void ulnock() throw(Error) { m_Mutex.unlock(); }

	string toString() const throw(Error);

	// 2007.01.02
	void setMaxPassPlayer( int player ) { m_nMaxPassPlayer = player; }
	int	 getMaxPassPlayer() { return m_nMaxPassPlayer; }

protected :
	void processCloseState() throw (Error);
	void processTantacleCombatState() throw (Error);
	void processMikllizzCombatState() throw (Error);

	void summonTantacle() throw (Error);	// 탠타클 소환
	void addMikllizzCorpse() throw (Error);	// 미클리즈 시체 추가
	void removeMikllizzCorpse() throw (Error);	// 미클리즈 시체 삭제
	void activeMikllizz() throw (Error);	// 미클리즈 활성화
	void kickOutAllPC() throw (Error);		// 플레이어 모두 쫓아내기
	void setStateClose() throw (Error);		// State Close 상태로 만든다.
	void giveKillingReward() throw (Error);		// 마스터 죽였을 때 받는 보상
	void decreaseSealingStone() throw (Error); // 존에 있는 유저들의 봉인석을 해제
	void broadcastClosingMessage( int minute ) throw (Error);
	void broadcastResurrectMikllizMessage( int minute ) throw (Error);


private : 
	Zone*				m_pZone;
	ObjectID_t			m_MikllizzID;
	ObjectID_t			m_MikllizzCorpseID;
	ZoneCoord_t			m_MikllizzX;
	ZoneCoord_t			m_MikllizzY;

	int					m_ClosingMessageStep;
	int					m_ResurrectMikllizzMessageStep;

	int					m_nMaxPassPlayer;	   // 최대 출입 사용자 수
	int					m_nPassPlayer;       // Pass를 받은 사람 수 

	Timeval				m_SummonNextTime;

	MikllizzLairState	m_State;             // 현재의 State
	Timeval				m_StateTime;         // 현재의 State 지속될 시간

	mutable Mutex		m_Mutex;				// m_nPassPlayer를 확실히 체크할려고..
};

#endif
