//////////////////////////////////////////////////////////////////////////////
// Filename    : GamePlayer.h 
// Written by  : reiot@ewestsoft.com
// Description : 게임 서버용 플레이어 클래스
//////////////////////////////////////////////////////////////////////////////

#ifndef __GAME_PLAYER_H__
#define __GAME_PLAYER_H__

#include "Types.h"
#include "Exception.h"
#include "Player.h"
#include "SocketEncryptOutputStream.h"
#include "SocketEncryptInputStream.h"
#include "Mutex.h"
#include <deque>
#include "Packet.h"
#include "Timeval.h"
#include "PlayerStatus.h"
#include "EventManager.h"
#include "skill/Skill.h"
#include "PaySystem.h"
#include "Item.h"
#include "billing/BillingPlayerInfo.h"
#include "chinabilling/CBillingPlayerInfo.h"
#include "Gpackets/GCReconnectLogin.h"
#include <bitset>
#include <map>

//#include "gameguard/CSAuth.h"//by viva

//////////////////////////////////////////////////////////////////////////////
// class GamePlayer
//
// 게임 서버용 플레이어 클래스
//
// Player 클래스를 상속받아서, 게임 서버에서만 사용되는 Mutex 및 
// Creature 관련 데이터 및 메쏘드, PreviousPacket 관련 데이터 및
// 메소드들을 추가했다.
//
// 특히 processOutput() 및 sendPacket()은 Race Condition 이 발생될 수
// 있으므로, Mutex 로 보호되어야 한다.(MODE-IV의 경우이며, MODE-I, II
// 의 경우에는 processInput(), processCommand() 모두 Mutex 로 보호해야
// 한다.)
//////////////////////////////////////////////////////////////////////////////

class Creature;

class GamePlayer : public Player, public PaySystem, public BillingPlayerInfo, public CBillingPlayerInfo
{
public:
	// 저장해 놓을 이전 패킷의 개수
	const static BYTE nPacketHistorySize = 10;

public:
	GamePlayer (Socket * pSocket) throw ( Error );
	~GamePlayer() throw(Error);

public:
	// read socket's receive buffer and fill input buffer
	// virtual void processInput() throw(IOException, Error);
	
	// parse packet and execute handler for the packet
	virtual void processCommand(bool Option = true) throw(IOException, Error);
	
	// flush output buffer to socket's send buffer
	virtual void processOutput() throw(IOException, Error);
	
	// send packet to player's output buffer
	virtual void sendPacket(Packet* packet) throw(ProtocolException, Error);

	// disconnect
	// 정식 로그아웃의 경우 disconnect(LOGOUT)
	virtual void disconnect(bool bDisconnected = DISCONNECTED) throw (InvalidProtocolException, Error);
	
	// get debug string
	virtual string toString() const throw(Error);
	
	bool verifyPacketTimeStamp(Packet *pPacket);

	// 스피드 체크
	virtual bool verifySpeed(Packet* pPacket) throw(Error);
	bool checkToRecvVerifyTime();
	bool verifyMoveSpeed(DWORD dwPacketTimeStamp, ZONE_COORD& adjCoord);
	void rollbackMoveSpeed();
	void saveSpeedHackPlayer();
	void saveSpeedHackLog(const string& strAbuseAction, bool bSendWarningMesg = true);
	long GetBaseTimeToVerifyMoveSpeed();

	bool verifyChat(DWORD dwPacketTimeStamp);
		
	BYTE getAttackSpeedStage(Speed_t AttackSpeed);
	bool verifyMeleeAttackSpeed(DWORD dwPacketTimeStamp);

	bool verifySkillSpeed(DWORD dwPacketTimeStamp, SkillType_t SkillType);
	void rollbackSkillSpeed(SkillType_t SkillType);
	long GetBaseTimeToVerifySkillSpeed(SkillType_t SkillType, int& iCheckType);
	
	inline void IncreaseAbuseActionPoint(int iPoint);
	inline void DecreaseAbuseActionPoint(int iPoint);
	inline float GetAbuseActionPoint() { return m_AbuseActionPoint; } 

	// get creature pointer 
	Creature* getCreature() throw() { return m_pCreature; }
	const Creature* getCreature() const throw() { return m_pCreature; }

	// set creature pointer
	void setCreature(Creature* pCreature) throw() { m_pCreature = pCreature; }

	// return recent N-th packet
	// 최근 전송된 N 번째 패킷을 리턴한다.
	Packet* getOldPacket(uint prev = 0) throw(OutOfBoundException, NoSuchElementException);

	// return recent packet which has packetID
	// 특정 ID를 가진 패킷 중 가장 최근의 패킷을 리턴한다.
	Packet* getOldPacket(PacketID_t packetID) throw(NoSuchElementException);

	// get player's status
	PlayerStatus getPlayerStatus() const throw() { return m_PlayerStatus; }

	// set player's status
	void setPlayerStatus(PlayerStatus playerStatus) throw() { m_PlayerStatus = playerStatus; }

	//
	void addEvent(Event* pEvent) throw(Error);
	Event* getEvent(Event::EventClass EClass) throw(Error);
	void deleteEvent(Event::EventClass EClass) throw(Error);

	// 패널티 Status 관련 함수
    //Set Flag
	void setPenaltyFlag(PenaltyType PenaltyFlag) throw() { m_PenaltyFlag.set(PenaltyFlag); }
	
	// remove Flag
	void removePenaltyFlag(PenaltyType PenaltyFlag) throw() { m_PenaltyFlag.reset(PenaltyFlag); }
	
	// Is Flag?
	bool isPenaltyFlag(PenaltyType PenaltyFlag) throw() { return m_PenaltyFlag.test(PenaltyFlag); }

public:
	uint getSpecialEventCount(void) const;
	void setSpecialEventCount(uint count);
	void loadSpecialEventCount(void) throw();
	void saveSpecialEventCount(void) throw();
	
public :	// '이미 접속 중'인 경우. 강제 종료를 위해서. by sigi.
	bool isKickForLogin() const throw();
	void setKickForLogin(bool bKickForLogin=true) throw();

	const string& 	getKickRequestHost() const throw();
	uint  			getKickRequestPort() const throw();

	void			setKickRequestHost(const string& host) throw();
	void			setKickRequestPort(uint port) throw();

public :
	// 쩝. 
	void	setReconnectPacket(GCReconnectLogin* pPacket) throw();
	GCReconnectLogin* getReconnectPacket() const throw();

	// by sigi. 2002.10.23
	bool isFreePass() const;
	void setFreePass(bool bFreePass=true);

public :
	void lock() throw(Error);
    void unlock() throw(Error);

public :
	void    setBillingSession() throw (Error)	{ BillingPlayerInfo::setBillingSession(this); }
	bool    sendBillingLogin() throw (Error);

	void	sendCBillingPayInfo() throw ( Error );

	// 패킷 암호화 관련
	// by sigi. 2002.11.27
	void	setEncryptCode() throw (Error);

public :

	void kickPlayer( uint nSeconds, uint KickMessageType ) throw (Error);

	int getPCRoomExpBonus();		// 집/무료PC방/유료PC방등에 대한 추가 경험치 배율

	//////////////////////////////////////////////////
	// PaySystem 관련
	//////////////////////////////////////////////////
public:
	bool	loginPayPlay( PayType payType,
						const string& PayPlayDate, int PayPlayHours, uint payPlayFlag,
						const string& ip, const string& playerID )
				throw (Error);

	bool	loginPayPlay( const string& ip, const string& playerID )
				throw (Error);

	bool	updatePayPlayTime( const string& playerID,
							const VSDateTime& currentDateTime,
							const Timeval& currentTime )
				throw (ProtocolException, Error );

	void	logoutPayPlay( const string& playerID, bool bClear=false, bool bDecreaseTime=true )
				throw (Error);

	bool	isPayPlaying() const;

	bool	isMetroFreePlayer() const;
	void	setMetroFreePlayer( bool bMetroFreePlayer = true );

	int getItemRatioBonusPoint(void) const;
	void setItemRatioBonusPoint(int point);

	bool	startPacketLog( uint sec );
//	CCSAuth&	getCSAuth() { return m_NProtectCSAuth; }//by viva

	void logLoginoutDateTime();

	enum GameFeature {
		GAME_FEATURE_GEARSWAPPING,
		GAME_FEATURE_GIVE_WEEKITEM,
		//1222 wlzzi - 프리미엄 존 혜택
		GAME_FEATURE_APPLY_PREMIUMZONE,

		GAME_FEATURE_MAX
	};

	bool isAuthGameFeature(GameFeature gameFeature, void *pData = NULL);
	
#ifdef __THAILAND_SERVER__
	bool getPermission(void) const { return m_bPermission; }
	void setPermission(bool permission) { m_bPermission = permission; }
#endif


private:

	void setPCRoomLottoStartTime();
	void checkPCRoomLotto( const Timeval& currentTime );
	void savePCRoomLottoTime();
	void giveLotto();

private:

	void recordSpeedGuardSample(const char* channel,
								DWORD dwPacketTimeStamp,
								long lBaseTime,
								long lDiffTime,
								long lErrorTime,
								DWORD dwMaxPerSecond,
								DWORD dwLogCooldown);

private:

	// creature
	Creature* m_pCreature;

	// previous packet queue
	deque<Packet *> m_PacketHistory;

	// player status
	PlayerStatus m_PlayerStatus;

	// Panelty status
	bitset<PENALTY_TYPE_MAX> m_PenaltyFlag;

	// expire time
	Timeval m_ExpireTime;

	int m_AbuseActionPoint;
	
	BYTE m_VerifyCount;
	int m_NumOfNotRecvVerifyTime;
	
	// 시간 검증 타임.
	Timeval m_SpeedVerify;
	Timeval m_MoveSpeedVerify;
	Timeval m_AttackSpeedVerify;
	
	Timeval m_BasePacketTime;
	Timeval m_UpdatePacketTime;
	DWORD m_dwPrevTimeStamp;
	DWORD m_dwPrevServerTime;
	DWORD m_dwPrevClientTime;
	bool m_bCalcPacketDeltaTime;
	DWORD m_PacketDeltaTime;
	int m_PacketDeltaSign;

	DWORD m_dwPrevMoveTime;
	ZoneID_t m_lastZoneID;
	deque<ZONE_COORD> m_MoveHist;
	int m_iAbnormalMoveTryCount;

	DWORD m_dwSpeedGuardMoveWindowStart;
	DWORD m_dwSpeedGuardMoveCount;
	DWORD m_dwSpeedGuardAttackWindowStart;
	DWORD m_dwSpeedGuardAttackCount;
	DWORD m_dwSpeedGuardSkillWindowStart;
	DWORD m_dwSpeedGuardSkillCount;
	DWORD m_dwSpeedGuardLastLogTime;
	DWORD m_dwSpeedGuardViolationCount;

	DWORD m_dwLastMeleeAttackTime;
	long m_lAccumAttackTime;		// unit - millisecond
	long m_lPenaltyAttackTime;		// unit - millisecond
	
	typedef deque<DWORD> TSkillTimeQueue;
	map<SkillType_t, TSkillTimeQueue> m_LastSkillTime;
	SkillType_t m_lastSkillType;
	BYTE m_GunSkillState;	
	
	DWORD m_dwPrevChatTime;
	int m_iAbnormalChatTryCount;		// 비정상 채팅 시도 회수 ( 일정 시간 이하로 채팅글이 들어올때 증가)
	int m_iAccumAbnormalChatCount;		// 비정상 채팅으로 판정된 회수

	// mutex
	mutable Mutex m_Mutex;

	EventManager m_EventManager;

	// 스페셜 이벤트 관련 카운터
	// 해골 많이 줍기나, 뭐 그런 종류의 이벤트에 사용될 수 있는 카운터
	uint m_SpecialEventCount;

	// '이미 접속 중'에서 강제 종료 시키는 경우
	bool	m_bKickForLogin;
	string  m_KickRequestHost;
	uint    m_KickRequestPort;

	// GameServer --> LoginServer로 갈때 사용한다. 으헤헤. by sigi. 2002.6.19
	GCReconnectLogin*	m_pReconnectPacket;

	bool m_bFreePass;

	// 빌링 시스템에 연결하지 않고 유료 플레이 하기
	bool m_bMetroFreePlayer;

	// 각 사용자별 아이템 획득 보너스 확률 
	int m_ItemRatioBonusPoint;

	Timeval			m_PCRoomLottoStartTime;		// PC 방 복권 계산용. 적용시작 시간
	uint			m_PCRoomLottoSumTime;		// PC 방 복권 계산용. 누적시간. logoutPayPlay시 저장용

	string			m_PacketLogFileName;
	bool			m_bPacketLog;
	Timeval			m_PacketLogEndTime;

//	CCSAuth			m_NProtectCSAuth;//by viva

	VSDateTime		m_LoginDateTime;


#ifdef __THAILAND_SERVER__
	bool            m_bPermission;
#endif
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class isSamePlayer 
{
public:
	isSamePlayer(GamePlayer* pGamePlayer) : m_pGamePlayer(pGamePlayer) {}
	bool operator()(GamePlayer* pGamePlayer) throw()
	{
		return pGamePlayer->getID() == m_pGamePlayer->getID();
	}

private:
	// Creature Pointer
	GamePlayer* m_pGamePlayer;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class isSamePlayerbyID
{
public:
	isSamePlayerbyID(const string & ID) : m_ID(ID) {}
	bool operator()(GamePlayer* pGamePlayer) throw()
	{
		return pGamePlayer->getID() == m_ID;
	}

private:
	// Creature ID
	string m_ID;
};

#endif
