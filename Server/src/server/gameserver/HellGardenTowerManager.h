//////////////////////////////////////////////////////////////////////////////
// Filename    : HellGardenTowerManager.h 
// Written by  : 쉭
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __HELLGARDEN_TOWER_MANAGER_H__
#define __HELLGARDEN_TOWER_MANAGER_H__

#include "Timeval.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class HellGardenTowerManager
//////////////////////////////////////////////////////////////////////////////

class Zone;
class Creature;
class PlayerCreature;

class HellGardenTowerManager
{
public : 
	enum HellGardenTowerState
	{ 
		STATE_OPEN,
		STATE_WAIT,
		STATE_STAGE_1,		// 층마다 Clear하면 보상 랜덤 지급 (씨앗)
		STATE_STAGE_2,
		STATE_STAGE_3,
		STATE_STAGE_4,
		STATE_STAGE_5,
		STATE_STAGE_6,
		STATE_STAGE_7,
		STATE_STAGE_8,
		STATE_STAGE_9,
		STATE_STAGE_10,
		STATE_INIT,			// 애덜한테 보상 아이템 지급(십자가, 내용확인) 초기화

		STATE_MAX
	}; 

	// 헬가든 타입 각 층에 해당한다.
	enum HellGardenType
	{
		TYPE_HELLGARDEN_1,
		TYPE_HELLGARDEN_2,
		TYPE_HELLGARDEN_3,
		TYPE_HELLGARDEN_4,
		TYPE_HELLGARDEN_5,
		TYPE_HELLGARDEN_MAX
	};

	// 헬가든 타워 타입 헬가든 각층의 4개의 타워에 해당한다.
	enum HellGardenTowerType
	{
		TYPE_HELLGARDEN_TOWER_0,
		TYPE_HELLGARDEN_TOWER_1,
		TYPE_HELLGARDEN_TOWER_2,
		TYPE_HELLGARDEN_TOWER_3,
		TYPE_HELLGARDEN_TOWER_4,
		TYPE_HELLGARDEN_TOWER_MAX
	};

	struct MonsterTypeList
	{
		MonsterType_t	MonsterType;
		int				MonsterCount;
	};

public:
	HellGardenTowerManager(Zone* pZone) throw(Error);
	~HellGardenTowerManager() throw();

	int getCurrentState() const { return m_State; }

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

	void MonsterListLoad();
	void initHellGarden();
protected :
	void processCloseState() throw (Error);
	void processTantacleCombatState() throw (Error);
	void processMikllizzCombatState() throw (Error);
	void processHellGardenTowerState() throw (Error);

	void summonMonster() throw (Error);	// 몬스터 소환
	void addMikllizzCorpse() throw (Error);	// 미클리즈 시체 추가
	void removeMikllizzCorpse() throw (Error);	// 미클리즈 시체 삭제
	void activeMikllizz() throw (Error);	// 미클리즈 활성화
	void kickOutAllPC() throw (Error);		// 플레이어 모두 쫓아내기
	void setStateClose() throw (Error);		// State Close 상태로 만든다.
	void giveKillingReward() throw (Error);		// 마스터 죽였을 때 받는 보상
	void decreaseSealingStone() throw (Error); // 존에 있는 유저들의 봉인석을 해제
	void broadcastClosingMessage( ) throw (Error);
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

	Timeval				m_TimeOutMessageNextTime;

	int					m_State;             // 현재의 State
	Timeval				m_StateTime;         // 현재의 State 지속될 시간

	// 여기 부터 맹든다
	// 타워는 4가지 타입을 가진다
	int					m_HellGardenTowerType;
	// 1층 부터 5층까지
	int					m_HellGardenType;
	int					m_KickZoneID; // 부활 위치
	int					m_SummonStep;
	bool				m_StartCombat; // 

	MonsterTypeList		m_MonterList[STATE_MAX];
//	int					m_CurrentMonstCount;

	int					m_PartyID; // 현재 헬가든에 들어온 파티ID

	mutable Mutex		m_Mutex;				// m_nPassPlayer를 확실히 체크할려고..
};

#endif
