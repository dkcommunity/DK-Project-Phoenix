//////////////////////////////////////////////////////////////////////////////
// Filename    : GamePlyaer.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "GamePlayer.h"
#include <cmath>

#include "Assert.h"
#include "BillingInfo.h"
#include "BillingPlayerManager.h"
#include "Creature.h"
#include "PlayerCreature.h"
#include "CreatureUtil.h"
#include "DB.h"
#include "DBExecuteManager.h"
#include "EffectMute.h"
#include "EventKick.h"
#include "Guild.h"
#include "GuildManager.h"
#include "IncomingPlayerManager.h"
#include "LogClient.h"
#include "Ousters.h"
#include "PCFinder.h"
#include "PacketFactoryManager.h"
#include "PacketValidator.h"
#include "Properties.h"
#include "RelicUtil.h"
#include "Shape.h"
#include "SkillCastingTimeManager.h"
#include "SkillHandlerManager.h"
#include "SkillInfo.h"
#include "Slayer.h"
#include "StringPool.h"
#include "TelephoneCenter.h"
#include "Thread.h"
#include "Utility.h"
#include "VSDateTime.h"
#include "Vampire.h"
#include "VariableManager.h"
#include "Zone.h"
#include "item/Motorcycle.h"
#include <fstream>

#include "Cpackets/CGConnect.h"
#include "Gpackets/GCKickMessage.h"
#include "Gpackets/GCSystemMessage.h"

#include "ServiceDeadline.h"

#include "chinabilling/CBillingInfo.h"
#ifdef __CONNECT_CBILLING_SYSTEM__
#include "chinabilling/CBillingPlayerManager.h"
#endif
// add by viva
#include "LoginServerManager.h"
#include <sstream>
#include <stdio.h>

// #include "PacketLog.h" // packet log를 보여주기 위한 헤더?

//////////////////////////////////////////////////////////////////////////////
// 모든 packet들에 대해서 profiling한다. by sigi. 2002.5.6
//
// 단, 이걸 사용하기 위해서는
// Profile.h에 MAX_PROFILE_SAMPLES += 300을 해줘야 한다.
//////////////////////////////////////////////////////////////////////////////
// #define __PROFILE_PACKETS__

#ifdef __PROFILE_PACKETS__
#include "Profile.h"
#endif

// by sigi. 2002.11.12
const int defaultGamePlayerInputStreamSize = 1024;
const int defaultGamePlayerOutputStreamSize = 20480;

static int maxIdleSec = 60 * 5; // 5분
// static int maxSpeedVerifyTime = 2;  		// 0.3 초
static int maxVerifyCount = 3;  // 3 번.
static int maxTimeGap = 2;      // 2 sec

static const int ATTACK_SPEED_FAST_BASE = 67;
static const int ATTACK_SPEED_EFFECTIVE_CAP = 100;
static const int ATTACK_SPEED_MAX_SPEEDUP_PERCENT = 12;

static bool IsAdminMoveSpeedBypassCreature(const Creature *pCreature) {
  return pCreature != NULL && (pCreature->isDM() || pCreature->isGOD());
}

static long ApplyHighAttackSpeedBonus(long baseAttackTime, Speed_t attackSpeed) {
  if (baseAttackTime <= 1 || attackSpeed <= ATTACK_SPEED_FAST_BASE) {
    return baseAttackTime;
  }

  int effectiveSpeed = attackSpeed;
  if (effectiveSpeed > ATTACK_SPEED_EFFECTIVE_CAP) {
    effectiveSpeed = ATTACK_SPEED_EFFECTIVE_CAP;
  }

  int extraSpeed = effectiveSpeed - ATTACK_SPEED_FAST_BASE;
  int speedRange = ATTACK_SPEED_EFFECTIVE_CAP - ATTACK_SPEED_FAST_BASE;
  long denominator = speedRange * 100L;
  long reduction = (baseAttackTime * ATTACK_SPEED_MAX_SPEEDUP_PERCENT *
                    extraSpeed + denominator / 2) /
                   denominator;
  long adjustedAttackTime = baseAttackTime - reduction;

  return adjustedAttackTime < 1 ? 1 : adjustedAttackTime;
}
static int SpeedCheckDelay = 5; // 5 sec

const int PCRoomLottoSec = 3600;    // 3600 초. 1시간
const int PCRoomLottoMaxAmount = 3; // 한번에 쌓일 수 있는 최대 복권수
// CLLoginHandler.cpp에 있는 함수다.
void addLogoutPlayerData(Player *pPlayer);

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////

GamePlayer::GamePlayer(Socket *pSocket) throw(Error)
    : // Player(pSocket), 	// by sigi. 2002.11.12
      m_pCreature(NULL), m_PlayerStatus(GPS_NONE), m_pReconnectPacket(NULL) {

  __BEGIN_TRY

  Assert(pSocket != NULL);
  m_pSocket = pSocket;

#ifdef __USE_ENCRYPTER__
  // create socket input stream
  m_pInputStream =
      new SocketEncryptInputStream(m_pSocket, defaultGamePlayerInputStreamSize);
  Assert(m_pInputStream != NULL);

  // create socket output stream
  m_pOutputStream = new SocketEncryptOutputStream(
      m_pSocket, defaultGamePlayerOutputStreamSize);
  Assert(m_pOutputStream != NULL);
#else
  // create socket input stream
  m_pInputStream =
      new SockettInputStream(m_pSocket, defaultGamePlayerInputStreamSize);
  Assert(m_pInputStream != NULL);

  // create socket output stream
  m_pOutputStream =
      new SockettOutputStream(m_pSocket, defaultGamePlayerOutputStreamSize);
  Assert(m_pOutputStream != NULL);
#endif

  m_Mutex.setName("GamePlayer");

  getCurrentTime(m_ExpireTime);
  m_ExpireTime.tv_sec += maxIdleSec;

  m_AbuseActionPoint = 0;

  // 현재 시간을 받아온다
  // getCurrentTime(m_SpeedVerify);
  m_SpeedVerify.tv_sec = 0;
  ;
  m_SpeedVerify.tv_usec = 0;
  ;
  m_VerifyCount = 0;
  m_NumOfNotRecvVerifyTime = 0;

  getCurrentTime(m_MoveSpeedVerify);
  getCurrentTime(m_AttackSpeedVerify);

  m_BasePacketTime.tv_sec = 0;
  m_BasePacketTime.tv_usec = 0;
  m_UpdatePacketTime.tv_sec = 0;
  m_UpdatePacketTime.tv_usec = 0;
  m_dwPrevTimeStamp = 0;
  m_dwPrevServerTime = 0;
  m_dwPrevClientTime = 0;
  m_bCalcPacketDeltaTime = false;
  m_PacketDeltaTime = 0;
  m_PacketDeltaSign = 1;

  m_dwPrevChatTime = 0;
  m_iAbnormalChatTryCount = 0;
  m_iAccumAbnormalChatCount = 0;

  DWORD dwCurrentTime;
  Timeval currentTime;

  getCurrentTime(currentTime);

  dwCurrentTime = currentTime.tv_sec * 1000 + currentTime.tv_usec / 1000;

  m_dwPrevMoveTime = dwCurrentTime;
  m_lastZoneID = 0;
  m_MoveHist.clear();
  m_iAbnormalMoveTryCount = 0;
  m_dwSpeedGuardMoveWindowStart = dwCurrentTime;
  m_dwSpeedGuardMoveCount = 0;
  m_dwSpeedGuardAttackWindowStart = dwCurrentTime;
  m_dwSpeedGuardAttackCount = 0;
  m_dwSpeedGuardSkillWindowStart = dwCurrentTime;
  m_dwSpeedGuardSkillCount = 0;
  m_dwSpeedGuardLastLogTime = 0;
  m_dwSpeedGuardViolationCount = 0;

  m_dwLastMeleeAttackTime = 0;
  m_lAccumAttackTime = 0;
  m_lPenaltyAttackTime = 0;

  m_lastSkillType = 0;
  m_GunSkillState = 0;

  m_SpecialEventCount = 0;

  m_bKickForLogin = false;

  m_bFreePass = false;

  m_bMetroFreePlayer = false;

  //	if ( m_bPCRoomPlay )
  //		m_ItemRatioBonusPoint =
  // g_pVariableManager->getPCRoomItemRatioBonusPercent(); 	else
  m_ItemRatioBonusPoint = 0;

  m_PCRoomLottoStartTime.tv_sec = 0;
  m_PCRoomLottoStartTime.tv_usec = 0;
  m_PCRoomLottoSumTime = 0;

  m_bPacketLog = false;

  // 객체가 생성된 시점을 로그인한 시간으로 본다.
  m_LoginDateTime = VSDateTime::currentDateTime();

#ifdef __THAILAND_SERVER__
  m_bPermission = false;
#endif

  //	m_NProtectCSAuth.Init();//by viva

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////

GamePlayer::~GamePlayer() throw(Error) {
  __BEGIN_TRY

  //__ENTER_CRITICAL_SECTION(m_Mutex)

  // 그 어떤 플레이어 객체가 삭제될 때에도, 그 상태는 로그아웃이어야 한다.
  // 즉 어떤 플레이어를 접속 종료 시키려면, 그 상태를 로그아웃으로 만들어야
  // 한다.
  Assert(m_PlayerStatus == GPS_END_SESSION);

  try {
#ifdef __CONNECT_BILLING_SYSTEM__
    // Pay 종료한다고 알려준다. by sigi. 2002.11.18
    if (isBillingPlayAvaiable() && !m_bMetroFreePlayer) // by sigi. 2002.11.23
    {
      g_pBillingPlayerManager->sendPayLogout(this);
    }
#elif defined(__CONNECT_CBILLING_SYSTEM__)
    // 중국 빌링 서버에 logout 패킷을 보낸다.
    g_pCBillingPlayerManager->sendLogout(this);
#endif

    if (m_pCreature != NULL) {
      g_pPCFinder->deleteCreature(m_pCreature->getName());
      m_pCreature->save();

      SAFE_DELETE(m_pCreature);
    }

  } catch (Throwable &t) {
    filelog("GamePlayer::~GamePlayer() : %s", t.toString().c_str());

    t.addStack(__PRETTY_FUNCTION__);
    throw("GamePlayer::~GamePlayer() : Throwable");
  }

  // 패킷을 삭제한다.
  while (!m_PacketHistory.empty()) {
    Packet *pPacket = m_PacketHistory.front();
    SAFE_DELETE(pPacket);
    m_PacketHistory.pop_front();
  }

  SAFE_DELETE(m_pReconnectPacket);

  //__LEAVE_CRITICAL_SECTION(m_Mutex)

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// parse packet and execute handler for the packet
//
//////////////////////////////////////////////////////////////////////

DWORD DiffDWORD(DWORD dwVal1, DWORD dwVal2) {
  DWORD dwDiff;

  if (dwVal1 > dwVal2) {
    dwDiff = dwVal1 - dwVal2;
  } else {
    dwDiff = dwVal2 - dwVal1;
  }

  return dwDiff;
}

bool GamePlayer::verifyPacketTimeStamp(Packet *pPacket) {
  Timeval currentTime;

  DWORD dwCurTimeStamp;
  DWORD dwServerTime;

  getCurrentTime(currentTime);

  if (m_BasePacketTime.tv_sec == 0 && m_BasePacketTime.tv_usec == 0) {
    // 기준 시간 설정
    m_BasePacketTime = currentTime;
    m_UpdatePacketTime = currentTime;
  }

  {
    Timeval diffTime;
    diffTime = timediff(m_UpdatePacketTime, currentTime);
    if (diffTime.tv_sec > 5 * 60) // 5분마다  시간차 갱신
    {
      // 클라이언트의 PC와 클럭차가 생겨 시간차가 누적이 되므로 적정 주기별로
      // 재계산 하도록 한다.
      m_UpdatePacketTime = currentTime;
      m_bCalcPacketDeltaTime = false;
    }
  }

  currentTime = timediff(currentTime, m_BasePacketTime);
  dwServerTime =
      currentTime.tv_sec * 1000 + currentTime.tv_usec / 1000; // ms 단위로 환산

  if (m_dwPrevServerTime > dwServerTime) {
    // getCurrentTime() 으로 현재 시각 얻을때 간헐적으로 이전시각을 돌려주는
    // 경우가 있음.
    dwServerTime = m_dwPrevServerTime;

    m_bCalcPacketDeltaTime = false;
  }

  dwCurTimeStamp = pPacket->GetTimeStamp();
  if (dwCurTimeStamp < m_dwPrevTimeStamp) {
    // TimeStamp가 Overflow 된 경우. 보통 클라이언트 PC가 켜진지 약 49일이 지난
    // 경우(timeGetTime() 함수 문제)
    m_bCalcPacketDeltaTime = false;
  }

  m_dwPrevTimeStamp = pPacket->GetTimeStamp();

  if (m_bCalcPacketDeltaTime) {
    // 재계산이 필요한 경우가 있는지 검사

    if (dwCurTimeStamp < m_PacketDeltaTime && m_PacketDeltaSign < 0) {
      m_bCalcPacketDeltaTime = false;
    }
    if ((0xFFFFFFFF - dwCurTimeStamp) < m_PacketDeltaTime &&
        m_PacketDeltaSign > 0) {
      dwCurTimeStamp = (0xFFFFFFFF - dwCurTimeStamp) + m_PacketDeltaTime;
      m_bCalcPacketDeltaTime = false;
    }
  }

  if (!m_bCalcPacketDeltaTime) {
    m_PacketDeltaTime = DiffDWORD(dwServerTime, dwCurTimeStamp);

    if (dwServerTime > dwCurTimeStamp) {
      m_PacketDeltaSign = 1;
    } else {
      m_PacketDeltaSign = -1;
    }

    m_bCalcPacketDeltaTime = true;
  }

  DWORD dwClientTime;
  DWORD dwDiffTime;
  DWORD dwDiffServerTime;
  DWORD dwDiffClientTime;

  // 클라이언트 기반 시각(단위는 ms)을 서버 시각 기준으로 변환을 한다.
  dwClientTime = dwCurTimeStamp + m_PacketDeltaSign * m_PacketDeltaTime;

  dwDiffTime = DiffDWORD(dwServerTime, dwClientTime);
  dwDiffServerTime = DiffDWORD(dwServerTime, m_dwPrevServerTime);
  dwDiffClientTime = DiffDWORD(dwClientTime, m_dwPrevClientTime);

  // 게임상에 들어오기 전까지는 m_pCreature는 NULL이 되어 있음
  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(m_pCreature);
  if (pPC != NULL && pPC->GetParameter<bool>("showTimeStampLog", false)) {
    char strBuf[1024];
    sprintf(strBuf,
            "PacketID=%d, PacketDT=%ld, CurTS=%ld, ST=%ld, DiffST=%ld, CT=%ld, "
            "DiffCT=%ld, DT=%c%ld",
            pPacket->getPacketID(), m_PacketDeltaSign * m_PacketDeltaTime,
            dwCurTimeStamp, dwServerTime, dwDiffServerTime, dwClientTime,
            dwDiffClientTime, dwServerTime > dwClientTime ? '-' : '+',
            dwDiffTime);

    GCSystemMessage msg;
    msg.setMessage(strBuf);

    sendPacket(&msg);
  }

  m_dwPrevServerTime = dwServerTime;
  m_dwPrevClientTime = dwClientTime;

  pPacket->SetTimeStamp(dwClientTime);

  if (dwServerTime < dwClientTime && dwDiffTime > 10 * 1000) {
    IncreaseAbuseActionPoint(100);

    // 클라이언트 시각이 서버보다 시간이 빠르고,
    // 클라이언트 시각과 서버 시각차이가 10초 이상이면 조작에 의한 패킷 전송으로
    // 간주

    filelog("AbuseTimeStamp.log",
            "Too much Difference between Client Time and Server Time. "
            "ID[%s] PacketID=%d, PacketDT=%ld, CurTS=%ld, ST=%ld, DiffST=%ld, "
            "CT=%ld, DiffCT=%ld, DT=%ld",
            m_ID.c_str(), pPacket->getPacketID(),
            m_PacketDeltaSign * m_PacketDeltaTime, dwCurTimeStamp, dwServerTime,
            dwDiffServerTime, dwClientTime, dwDiffClientTime, dwDiffTime);

    m_bCalcPacketDeltaTime = false;

    ostringstream strAbuseAction;

    strAbuseAction << "(TimeStamp, " << pPacket->getPacketID() << ", "
                   << dwServerTime << ", " << dwClientTime << ", " << dwDiffTime
                   << ")";

    // saveSpeedHackLog(strAbuseAction.str());

    return false;
  }

  return true;
}

static bool isPartyDiagPacket(PacketID_t packetID) {
  return packetID == Packet::PACKET_CG_PARTY_INVITE ||
         packetID == Packet::PACKET_CG_PARTY_LEAVE ||
         packetID == Packet::PACKET_CG_PARTY_POSITION ||
         packetID == Packet::PACKET_CG_PARTY_SAY;
}

void GamePlayer::processCommand(bool Option) throw(IOException, Error) {
  __BEGIN_TRY

  sPacketHeader header;

  PacketID_t packetID;
  PacketSize_t packetSize;
  Packet *pPacket = NULL;

  try {

    // 패널티를 받고 있는 사용자 일 경우 이 부분에서 처리 해준다.
    // 패널티를 받는 경우 어떤 예외 상황이 발생할지 모르므로,
    // 정상적인 처리를 다 한다음 패널티를 처리하도록 한다.
    if (isPenaltyFlag(PENALTY_TYPE_KICKED)) {
      filelog(
          "GamePlayer.txt", "Penalty Kicked. Name[%s],Host[%s],Type[%d]",
          ((getCreature() == NULL) ? "NULL" : getCreature()->getName().c_str()),
          ((getSocket() == NULL) ? "NULL" : getSocket()->getHost().c_str()),
          m_ItemRatioBonusPoint);

      throw DisconnectException("He is had penalty");
    }

    if (Option) {
      m_EventManager.heartbeat();
    }

    // 입력버퍼에 들어있는 완전한 패킷들을 모조리 처리한다.
    while (true) {
      if (m_EventManager.getEvent(Event::EVENT_CLASS_KICK) != NULL) {
        // 킥 이베트가 걸린  직후 들어오는 패킷은 무시하도록 한다.

        return;
      }

      // 입력스트림에서 패킷헤더크기만큼 읽어본다.
      // 만약 지정한 크기만큼 스트림에서 읽을 수 없다면,
      // Insufficient 예외가 발생하고, 루프를 빠져나간다.
      // NoSuch제거. by sigi. 2002.5.4
      // if (!m_pInputStream->peek(&header[0] , szPacketHeader))
      if (!m_pInputStream->peek((char *)&header, sizeof(sPacketHeader))) {
        Timeval currentTime;
        getCurrentTime(currentTime);
        if (currentTime >= m_ExpireTime) {
          filelog(
              "GamePlayer.txt",
              "Timeout Disconnect1. Name[%s],Host[%s],ZoneID[%d]",

              ((getCreature() == NULL) ? "NULL"
                                       : getCreature()->getName().c_str()),
              ((getSocket() == NULL) ? "NULL" : getSocket()->getHost().c_str()),
              ((getCreature() == NULL || getCreature()->getZone() == NULL)
                   ? -1
                   : getCreature()->getZone()->getZoneID())

          );

          throw DisconnectException("timeout, the connection is terminated.");
        }

        break;
      }

      // 패킷아이디 및 패킷크기를 알아낸다.
      // 이때 패킷크기는 헤더를 포함한다.
      packetID = header.packetID;
      packetSize = header.packetSize;

      if (isPartyDiagPacket(packetID)) {
        filelog("PartyDiag.log",
                "header player=%s status=%d packetID=%d size=%d inputLen=%u",
                m_ID.c_str(), getPlayerStatus(), packetID, packetSize,
                m_pInputStream->length());
      }

      // 패킷 아이디가 이상하면 프로토콜 에러로 간주한다.
      if (packetID >= (int)Packet::PACKET_MAX) {
        filelog(
            "GamePlayer.txt",
            "Packet ID exceed MAX, RECV [%d/%d],ID[%s],Host[%s]", packetID,
            Packet::PACKET_MAX, m_ID.c_str(),
            //					getCreature()->getName().c_str(),
            getSocket()->getHost().c_str());

        throw InvalidProtocolException("too large packet id");
      }

#ifdef __PACKET_LOG__
      filelog("Packet.log", "Recv Packet ID : [%d], ID[%s], Host[%s]", packetID,
              m_ID.c_str(), getSocket()->getHost().c_str());
      cout << "Recv Packet ID : [" << packetID << "], ID[" << m_ID.c_str()
           << "], Host[" << getSocket()->getHost().c_str() << "]" << endl;
#endif
      try {
        if (!g_pLoginServerManager->m_IsAllow) // by viva
          delete this;

        // 패킷 순서가 올바른지 검증한다.
        if (!g_pPacketValidator->isValidPacketID(getPlayerStatus(), packetID)) {
          filelog("GamePlayer.txt",
                  "Not Valid Packet, RECV [%d],ID[%s],Host[%s]", packetID,
                  m_ID.c_str(),
                  //						getCreature()->getName().c_str(),
                  getSocket()->getHost().c_str());
          throw InvalidProtocolException("invalid packet order");
        }

        if (!g_pPacketFactoryManager->isCGPacket(packetID)) {
          filelog("ErrorPacket.txt",
                  "Not Valid Packet, RECV [%d],ID[%s],Host[%s]", packetID,
                  m_ID.c_str(), getSocket()->getHost().c_str());
          throw InvalidProtocolException("invalid error packet");
        }

        // 패킷 크기가 너무 크면 프로토콜 에러로 간주한다.
        if (packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID)) {
          filelog("GamePlayer.txt",
                  "Too Larget Packet Size, RECV "
                  "[%d],PacketSize[%d/%d],ID[%s],Host[%s]",
                  packetID, packetSize,
                  g_pPacketFactoryManager->getPacketMaxSize(packetID),
                  m_ID.c_str(),
                  //						getCreature()->getName().c_str(),
                  getSocket()->getHost().c_str());
          throw InvalidProtocolException("too large packet size");
        }

        // 입력버퍼내에 패킷크기만큼의 데이타가 들어있는지 확인한다.
        // 최적화시 break 를 사용하면 된다. (여기서는 일단 exception을 쓸
        // 것이다.)
        // if (m_pInputStream->length() < szPacketHeader + packetSize)
        if (m_pInputStream->length() <
            sizeof(sPacketHeader) + sizeof(DWORD) + packetSize)
          // throw InsufficientDataException();
          break;

        // 패킷 하나를 읽었으니, expire time 을 연기한다.
        getCurrentTime(m_ExpireTime);
        m_ExpireTime.tv_sec += maxIdleSec;

        // 여기까지 왔다면 입력버퍼에는 완전한 패킷 하나 이상이 들어있다는
        // 뜻이다. 패킷팩토리매니저로부터 패킷아이디를 사용해서 패킷 스트럭처를
        // 생성하면 된다. 패킷아이디가 잘못될 경우는 패킷팩토리매니저에서
        // 처리한다.
        pPacket = g_pPacketFactoryManager->createPacket(packetID);

        m_pInputStream->skip(szPacketHeader);
        pPacket->readTimeStamp(*m_pInputStream);

        // 이제 이 패킷스트럭처를 초기화한다.
        // 패킷하위클래스에 정의된 read()가 virtual 메커니즘에 의해서 호출되어
        // 자동적으로 초기화된다.
        m_pInputStream->readPacket(pPacket, false);

        if (isPartyDiagPacket(packetID)) {
          Creature *pDiagCreature = getCreature();
          filelog("PartyDiag.log",
                  "read player=%s status=%d packet=%s creature=%s partyID=%d "
                  "zone=%d inputLen=%u body=%s",
                  m_ID.c_str(), getPlayerStatus(),
                  pPacket->getPacketName().c_str(),
                  pDiagCreature == NULL ? "NULL"
                                         : pDiagCreature->getName().c_str(),
                  pDiagCreature == NULL ? -1 : pDiagCreature->getPartyID(),
                  (pDiagCreature == NULL || pDiagCreature->getZone() == NULL)
                      ? -1
                      : pDiagCreature->getZone()->getZoneID(),
                  m_pInputStream->length(), pPacket->toString().c_str());
        }

        if (GetAbuseActionPoint() >
            g_pVariableManager->getSkillSpeedMaxAbuseActionPoint()) {
          if (g_pVariableManager->isSkillSpeedDisconnectAbuseAction()) {
            if (m_EventManager.getEvent(Event::EVENT_CLASS_KICK) == NULL) {
              string strAbuseAction;

              strAbuseAction = "(Kicked)";
              // saveSpeedHackLog(strAbuseAction, false);

              // 비정상적인 게임플레이가 설정회수를 초과하여 감시될 경우 접속
              // 종료 시킨다.
              kickPlayer(3, KICK_MESSAGE_ABUSE_ACTION);

              SAFE_DELETE(pPacket);
              return;
            }
          }
        }

        if (!verifyPacketTimeStamp(pPacket)) {
          // TimeStamp 조작으로 의심되는 패킷의 경우 무시한다.

          SAFE_DELETE(pPacket);
          return;
        }

        /*				if ( m_PacketHistory.size() > 0 )
                                        {
                                                Packet* prevPacket =
           m_PacketHistory.back(); if ( prevPacket->getPacketID() ==
           Packet::PACKET_CG_READY )
                                                {
                                                        switch(
           pPacket->getPacketID())
                                                        {
                                                                case
           Packet::PACKET_CG_READY : case Packet::PACKET_CG_SET_SLAYER_HOT_KEY :
                                                                case
           Packet::PACKET_CG_SET_VAMPIRE_HOT_KEY :
                                                                {
                                                                        filelog("CGReadyPacketDup.log",
           "PacketID=%d, PlayerID=%s, IP=%s", pPacket->getPacketID(),
                                                                                m_ID.c_str(),
                                                                                getSocket()->getHost().c_str()
                                                                        );

                                                                        continue;
                                                                }
                                                        }
                                                }
                                        }*/

        // 현재 패킷을 패킷 히스토리의 맨 뒤에 넣는다.
        m_PacketHistory.push_back(pPacket);

        // packet file log를 남긴다.
        if (m_bPacketLog) {
          Timeval currentTime;
          getCurrentTime(currentTime);

          if (currentTime >= m_PacketLogEndTime) {
            m_bPacketLog = false;
          } else {
            filelog(m_PacketLogFileName.c_str(), "%s",
                    pPacket->toString().c_str());
          }
        }

        if (m_pCreature != NULL) {
          PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(m_pCreature);

          if (pPC->GetTalkingCallNPC() != NULL) {
            switch (packetID) {
            case Packet::PACKET_CG_READY:
            // case Packet::PACKET_CG_MOVE :
            case Packet::PACKET_CG_NPC_TALK:
            case Packet::PACKET_CG_TRADE_PREPARE:
            case Packet::PACKET_CG_TRADE_FINISH: {
              PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(m_pCreature);
              pPC->FinishToTalkWithCallNPC();
            } break;
            default:
              break;
            }
          }
        }

        // cout << "[" << (int)Thread::self() << "] execute before : " <<
        // pPacket->getPacketName().c_str() << endl;

        if (m_pCreature != NULL &&
            m_pCreature->isFlag(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE)) {
          filelog("TraceBloodBible.log",
                  "ID=%s Name=%s ZoneID=%d X=%d Y=%d Packet=%s", m_ID.c_str(),
                  getCreature()->getName().c_str(),
                  getCreature()->getZone()->getZoneID(), getCreature()->getX(),
                  getCreature()->getY(), pPacket->toString().c_str());
        }

        // 이제 이 패킷스트럭처를 가지고 패킷핸들러를 수행하면 된다.
        // 패킷아이디가 잘못될 경우는 패킷핸들러매니저에서 처리한다.
        try {
#ifdef __PROFILE_PACKETS__

          beginProfileEx(pPacket->getPacketName().c_str());
          pPacket->execute(this);
          endProfileEx(pPacket->getPacketName().c_str());

#else
          pPacket->execute(this);
#endif
        } catch (Throwable &t) {
          if (isPartyDiagPacket(packetID)) {
            Creature *pDiagCreature = getCreature();
            filelog("PartyDiag.log",
                    "exception player=%s status=%d packet=%s creature=%s "
                    "partyID=%d error=%s",
                    m_ID.c_str(), getPlayerStatus(),
                    pPacket == NULL ? "NULL" : pPacket->getPacketName().c_str(),
                    pDiagCreature == NULL ? "NULL"
                                           : pDiagCreature->getName().c_str(),
                    pDiagCreature == NULL ? -1 : pDiagCreature->getPartyID(),
                    t.toString().c_str());
          }
          filelog("GPPC.txt", "%s PacketID : %d", t.toString().c_str(),
                  packetID);
          throw;
        }

        if (isPartyDiagPacket(packetID)) {
          Creature *pDiagCreature = getCreature();
          filelog("PartyDiag.log",
                  "executed player=%s status=%d packet=%s creature=%s "
                  "partyID=%d",
                  m_ID.c_str(), getPlayerStatus(),
                  pPacket == NULL ? "NULL" : pPacket->getPacketName().c_str(),
                  pDiagCreature == NULL ? "NULL"
                                         : pDiagCreature->getName().c_str(),
                  pDiagCreature == NULL ? -1 : pDiagCreature->getPartyID());
        }

        if (!checkToRecvVerifyTime()) {
          // 정상적으로 CGVerifyTime 이 수신되고 있지 않음
          saveSpeedHackPlayer();

          throw DisconnectException("Abnormal Sending CGVerifyTime packet.");
        }

        // cout << "[" << (int)Thread::self() << "] execute after : " <<
        // pPacket->getPacketName().c_str() << endl;

        // 패킷을 nPacketHistorySize 개만큼만 저장한다.
        while (m_PacketHistory.size() > nPacketHistorySize) {
          Packet *oldPacket = m_PacketHistory.front();
          SAFE_DELETE(oldPacket);
          m_PacketHistory.pop_front();
        }
      } catch (IgnorePacketException &igpe) {
        // PacketValidator 에서 패킷을 무시하라고 했으니,
        // 입력스트림에서 모두 지워버리고 실행하지 않도록 한다.

        // 패킷 크기가 너무 크면 프로토콜 에러로 간주한다.
        if (packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID)) {
          filelog("GamePlayer.txt",
                  "Too Larget Packet Size[Ignore], RECV "
                  "[%d],PacketSize[%d],Name[%s],Host[%s]",
                  packetID, packetSize,
                  ((getCreature() == NULL) ? "NULL"
                                           : getCreature()->getName().c_str()),
                  ((getSocket() == NULL) ? "NULL"
                                         : getSocket()->getHost().c_str()));
          throw InvalidProtocolException("too large packet sizeIgnore");
        }

        // 입력버퍼내에 패킷크기만큼의 데이타가 들어있는지 확인한다.
        // 최적화시 break 를 사용하면 된다. (여기서는 일단 exception을 쓸
        // 것이다.)
        // if (m_pInputStream->length() < szPacketHeader + packetSize)
        if (m_pInputStream->length() <
            sizeof(sPacketHeader) + sizeof(DWORD) + packetSize)
          throw InsufficientDataException();

        // 데이타가 모두 도착했으면, 그 크기만큼 무시하고,
        // 다른 패킷을 처리하도록 한다....
        // m_pInputStream->skip(szPacketHeader + packetSize);
        m_pInputStream->skip(sizeof(sPacketHeader) + // Packet Header
                             sizeof(DWORD) +         // TimeStamp
                             packetSize              // Packet Body
        );

        // 무시된 패킷은, expire 에 영향을 주지 않게 된다.
        // 즉 유효한 패킷만이 짤리지 않게 해준다.
        // 또한 히스토리에도 들어가지 않는다.
      }
    }
  } catch (InsufficientDataException &ide) {
    // expire time 을 초과했을 경우 연결을 종료한다.
    Timeval currentTime;
    getCurrentTime(currentTime);
    if (currentTime >= m_ExpireTime) {
      filelog(
          "GamePlayer.txt", "Timeout Diconnect. Name[%s],Host[%s]",
          ((getCreature() == NULL) ? "NULL" : getCreature()->getName().c_str()),
          ((getSocket() == NULL) ? "NULL" : getSocket()->getHost().c_str()));

      throw DisconnectException(
          "일정 시간동안 입력하지 않을 경우, 연결을 종료합니다.");
    }
  }
  // 주석처리 by sigi. 2002.5.14
  /*
  catch (InvalidProtocolException & ipe)
  {
          // 접속을 강제종료시켜야 한다. 무슨 방법으로??
          throw;
  }
  catch (Throwable & t)
  {
          //cerr << t.toString() << endl;
          //cerr << "GamePlayer Throwable Exception Check!" << endl;
          throw;
  }
  */

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// flush output buffer to socket's send buffer
//
// flush할 동안 다른 쓰레드에서 출력 버퍼에 sendPacket을 호출해서는 안된다.
// (이럴 경우는 서버간 통신으로 say가 날아오는 것밖에 없다.)
//
//////////////////////////////////////////////////////////////////////
void GamePlayer::processOutput() throw(IOException, Error) {
  __BEGIN_TRY

  __ENTER_CRITICAL_SECTION(m_Mutex)

  int i = 0;

  try {
    Player::processOutput();

    i = 100000;
  } catch (InvalidProtocolException &It) {
    // cerr << "GamePlayer::processOutput Exception Check!!" << endl;
    // cerr << It.toString() << endl;
    // cerr << "익셉션 났는데...... 아이는 몇인고?" << (int)i << endl;
    throw DisconnectException("Pipe 연결의 파괴로 접속을 짜른다");
  }

  __LEAVE_CRITICAL_SECTION(m_Mutex)

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// send packet to player's output buffer
//
//////////////////////////////////////////////////////////////////////
void GamePlayer::sendPacket(Packet *pPacket) throw(ProtocolException, Error) {
  __BEGIN_TRY

  __ENTER_CRITICAL_SECTION(m_Mutex)

  try {
    // packet file log를 남긴다.
    if (m_bPacketLog) {
      Timeval currentTime;
      getCurrentTime(currentTime);

      if (currentTime >= m_PacketLogEndTime) {
        m_bPacketLog = false;
      } else {
        filelog(m_PacketLogFileName.c_str(), "%s", pPacket->toString().c_str());
      }
    }

    Player::sendPacket(pPacket);

#ifdef __PACKET_LOG__
    filelog("Packet.log", "Send Packet ID : [%d], ID[%s], Host[%s]",
            pPacket->getPacketID(), m_ID.c_str(),
            getSocket()->getHost().c_str());
    cout << "Send Packet ID : [" << pPacket->getPacketID() << "], ID["
         << m_ID.c_str() << "], Host[" << getSocket()->getHost().c_str() << "]"
         << endl;
#endif

    // cout << "GamePlayer::sendPacket() : " << pPacket->toString() << endl;
    // cout << "GamePlayer::sendPacket() PACKET SIZE : " <<
    // pPacket->getPacketSize() << endl;

    /*
    if (getCreature() != NULL)
    {
            PacketID_t packetID = pPacket->getPacketID();
            switch (packetID)
            {
                    case Packet::PACKET_GC_UPDATE_INFO:
                    case Packet::PACKET_GC_MOVE_OK:
                    case Packet::PACKET_GC_SET_POSITION:
                            filelog("GamePlayer.txt", "SEND
    [%s],Name[%s],Host[%s]", pPacket->getPacketName().c_str(),
                                            getCreature()->getName().c_str(),
                                            getSocket()->getHost().c_str());
                            break;
                    default:
                            break;
            }
    }
    else
    {
            PacketID_t packetID = pPacket->getPacketID();
            switch (packetID)
            {
                    case Packet::PACKET_GC_UPDATE_INFO:
                    case Packet::PACKET_GC_MOVE_OK:
                    case Packet::PACKET_GC_SET_POSITION:
                            filelog("GamePlayer.txt", "SEND
    [%s],Name[NULL],Host[%s]", pPacket->getPacketName().c_str(),
                                            getSocket()->getHost().c_str());
                            break;
                    default:
                            break;
            }
    }
    */
  } catch (InvalidProtocolException &It) {
    // cout << "GamePlayer::sendPacket Exception Check!!" << endl;
    // cout << It.toString() << endl;
    // throw DisconnectException("Pipe 연결의 파괴로 접속을 짜른다");
  }

  __LEAVE_CRITICAL_SECTION(m_Mutex)

  __END_CATCH
}

//--------------------------------------------------------------------------------
//
// disconnect player
//
// (1) 크리처가 존재할 경우, 존에서 삭제하고 브로드캐스트한다.
// (2) 크리처를 저장한다.
//
//--------------------------------------------------------------------------------
void GamePlayer::disconnect(bool bDisconnected) throw(InvalidProtocolException,
                                                      Error) {
  __BEGIN_TRY

  __ENTER_CRITICAL_SECTION(m_Mutex)

  //--------------------------------------------------------------------------------
  // 크리처를 존에서 삭제한 후, 주변의 PC들에게 브로드캐스트해준다.
  // 그다음 크리처를 DB에 저장한다.
  //--------------------------------------------------------------------------------
  string CreatureName = "";
  if (m_pCreature != NULL) {
    CreatureName = m_pCreature->getName();

    try {
      // GPS_NORMAL 인 경우에만, 존에 크리처가 들어가 있게 된다.
      // *CAUTION*
      // 혹시 pushPC()가 호출된 후에 연결이 종료되면 어떻게 하나.. 라고 걱정할
      // 가능성도 있다. 그러나, 처리 루틴의 순서를 잘 살펴보면 그런 걱정을 할
      // 필요가 없다. 어차피 연결이 끊기더라도, 존이 처리된 후 크리처가 실제
      // 타일로 올라간 다음, 다시 플레이어의 처리루프가 돌아오기 때문이다.
      if (getPlayerStatus() == GPS_NORMAL) {
        //----------------------------------
        // 존에서 삭제하고 브로드캐스트한다.
        //----------------------------------
        Zone *pZone = m_pCreature->getZone();
        Assert(pZone != NULL);
        pZone->deleteQueuePC(m_pCreature);
        pZone->deleteCreature(m_pCreature, m_pCreature->getX(),
                              m_pCreature->getY());
        //--------------------------------------------------------------------------------
        // Extra slot (mouse) item destructor'da inventory silindikten sonra
        // çantaya yazılamaz. save() öncesi flush.
        //--------------------------------------------------------------------------------
        PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(m_pCreature);
        if (pPC != NULL) {
          pPC->restoreExtraInventorySlotToInventory(false);
        }
        //--------------------------------------------------------------------------------
        // 크리처를 저장한다.
        //--------------------------------------------------------------------------------
        m_pCreature->save();
      }
    } catch (Throwable &t) {
    }
  }

  setPlayerStatus(GPS_END_SESSION);

  //--------------------------------------------------------------------------------
  // 아이디가 설정되었다는 뜻은, 로그인이 이루어졌다는 뜻이다.
  //--------------------------------------------------------------------------------
  if (m_ID != "") {
    Statement *pStmt1 = NULL;
    Statement *pStmt2 = NULL;

    BEGIN_DB {

      //            pStmt =
      //            g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
      // 로그오프로 변경한다.
      // pStmt1 =
      // g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
      pStmt1 =
          g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

      // LogOn이 GAME상태인 경우만 LOGOFF로 바꾼다. by sigi. 2002.5.15
      pStmt1->executeQuery(
          "UPDATE Player SET LogOn='LOGOFF', LastLogoutDate=now() WHERE "
          "PlayerID = '%s' AND LogOn='GAME'",
          m_ID.c_str());

      // LogoutPlayerData 에 추가한다.
      //	addLogoutPlayerData(this);

      if (pStmt1->getAffectedRowCount() == 0) {
        // 이미 LOGOFF거나
        // LOGON상태인가?
        // 이럴 수 있을까? -_-;
      }

      // 빌링 by sigi. 2002.5.31
#if defined(__PAY_SYSTEM_LOGIN__) || defined(__PAY_SYSTEM_ZONE__) ||           \
    defined(__PAY_SYSTEM_FREE_LIMIT__)
      if (isPayPlaying() || isPremiumPlay()) {
        logoutPayPlay(m_ID);
      }
#endif

      SAFE_DELETE(pStmt1);
    }
    END_DB(pStmt1)
    BEGIN_DB {

      // IP 정보를 삭제해 준다.
      pStmt2 = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
      pStmt2->executeQuery("DELETE FROM UserIPInfo WHERE Name = '%s'",
                           CreatureName.c_str());

      SAFE_DELETE(pStmt2);
    }
    END_DB(pStmt2)
  }

  // login서버로 가라고 client에게 보낸다.
  // 원래는 LGIncomingConnectionOKHandler에서 처리했다. by sigi. 2002.6.19
  if (m_pReconnectPacket != NULL) {
    // cout << "[SendReconnect] " << m_pReconnectPacket->toString().c_str() <<
    // endl;

    try {
      // sendPacket( m_pReconnectPacket );
      Player::sendPacket(m_pReconnectPacket);
      // 출력 버퍼에 남아있는 데이타를 전송한다.
      m_pOutputStream->flush();
    } catch (Throwable &t) {
      // 무시
    }

    SAFE_DELETE(m_pReconnectPacket);
  }

  // 함수 윗부분에서 처리했는데.. 타이밍 문제(LogOn이나 UserIPInfo 등)로
  // 여기서 끊어주고.. 재접패킷~을 보내도록 한다.
  if (bDisconnected == UNDISCONNECTED) {
    try {
      // 클라이언트에게 GCDisconnect 패킷을 전송한다.
      // GCDisconnect gcDisconnect;
      // sendPacket(gcDisconnect);

      // 출력 버퍼에 남아있는 데이타를 전송한다.
      m_pOutputStream->flush();
    } catch (Throwable &t) {
      // cerr << "GamePlayer::disconnect() : GamePlayer::disconnect Exception
      // Check!!" << endl; cerr << t.toString() << endl;
    }
  }

  // 소켓 연결을 닫는다.
  m_pSocket->close();

  __LEAVE_CRITICAL_SECTION(m_Mutex)

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// 최근 N 번째의 패킷을 리턴한다.
//
// N == 0 일 경우, 가장 최근의 패킷을 리턴하게 된다.
//
// 최대 nPacketHistorySize - 1 까지 지정할 수 있다.
//
//////////////////////////////////////////////////////////////////////
Packet *GamePlayer::getOldPacket(uint prev) throw(OutOfBoundException,
                                                  NoSuchElementException) {
  __BEGIN_TRY

  if (prev >= nPacketHistorySize)
    throw OutOfBoundException();

  // if prev == 0 , return m_PacketHistory[9]
  // if prev == 9 , return m_PacketHistory[0]
  Packet *pPacket = m_PacketHistory[nPacketHistorySize - prev - 1];

  if (pPacket == NULL)
    throw NoSuchElementException("packet history is empty");

  return pPacket;

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// 특정 패킷아이디를 가진 가장 최근의 패킷을 리턴한다.
//
//////////////////////////////////////////////////////////////////////
Packet *
GamePlayer::getOldPacket(PacketID_t packetID) throw(NoSuchElementException) {
  __BEGIN_TRY

  Packet *pPacket = NULL;
  deque<Packet *>::reverse_iterator ritr = m_PacketHistory.rbegin();

  for (; ritr != m_PacketHistory.rend(); ritr++) {
    if ((*ritr)->getPacketID() == packetID) {
      pPacket = (*ritr);
      break;
    }
  }

  if (pPacket == NULL)
    throw NoSuchElementException("packet history is empty");

  return pPacket;

  __END_CATCH
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
void GamePlayer::addEvent(Event *pEvent) throw(Error) {
  __BEGIN_TRY

  m_EventManager.addEvent(pEvent);

  __END_CATCH
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
void GamePlayer::deleteEvent(Event::EventClass EClass) throw(Error) {
  __BEGIN_TRY

  m_EventManager.deleteEvent(EClass);

  __END_CATCH
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
Event *GamePlayer::getEvent(Event::EventClass EClass) throw(Error) {
  __BEGIN_TRY

  return m_EventManager.getEvent(EClass);

  __END_CATCH
}

uint GamePlayer::getSpecialEventCount(void) const {
  return m_SpecialEventCount;
}

void GamePlayer::setSpecialEventCount(uint count) {
  m_SpecialEventCount = count;
}

bool GamePlayer::isKickForLogin() const throw() {
  return m_bKickForLogin;
}

void GamePlayer::setKickForLogin(bool bKickForLogin) throw() {
  m_bKickForLogin = bKickForLogin;
}

const string &GamePlayer::getKickRequestHost() const throw() {
  return m_KickRequestHost;
}

uint GamePlayer::getKickRequestPort() const throw() {
  return m_KickRequestPort;
}

void GamePlayer::setKickRequestHost(const string &host) throw() {
  m_KickRequestHost = host;
}

void GamePlayer::setKickRequestPort(uint port) throw() {
  m_KickRequestPort = port;
}

void GamePlayer::setReconnectPacket(GCReconnectLogin *pPacket) throw() {
  SAFE_DELETE(m_pReconnectPacket);
  m_pReconnectPacket = pPacket;
}

GCReconnectLogin *GamePlayer::getReconnectPacket() const throw() {
  return m_pReconnectPacket;
}

bool GamePlayer::isFreePass() const {
  return m_bFreePass;
}

void GamePlayer::setFreePass(bool bFreePass) {
  m_bFreePass = bFreePass;
}

void GamePlayer::lock() throw(Error) {
  m_Mutex.lock();
}

void GamePlayer::unlock() throw(Error) {
  m_Mutex.unlock();
}

bool GamePlayer::isMetroFreePlayer() const {
  return m_bMetroFreePlayer;
}

void GamePlayer::setMetroFreePlayer(bool bMetroFreePlayer) {
  m_bMetroFreePlayer = bMetroFreePlayer;
}

int GamePlayer::getItemRatioBonusPoint(void) const {
  return m_ItemRatioBonusPoint;
}

void GamePlayer::setItemRatioBonusPoint(int point) {
  m_ItemRatioBonusPoint = point;
}

//////////////////////////////////////////////////////////////////////
//
// get debug string
//
//////////////////////////////////////////////////////////////////////
string GamePlayer::toString() const throw(Error) {
  __BEGIN_TRY

  StringStream msg;

  //////////////////////////////////////////////////
  // enter critical section
  //////////////////////////////////////////////////
  __ENTER_CRITICAL_SECTION(m_Mutex)

  msg << "GamePlayer(SocketID:" << m_pSocket->getSOCKET()
      << ",Host:" << m_pSocket->getHost() << ")";

  //////////////////////////////////////////////////
  // leave critical section
  //////////////////////////////////////////////////
  __LEAVE_CRITICAL_SECTION(m_Mutex)

  return msg.toString();

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// verifySpeed
//
//////////////////////////////////////////////////////////////////////
bool GamePlayer::verifySpeed(Packet *pPacket) throw(Error) {
  __BEGIN_TRY

  bool SpeedCheck = false;

  Timeval CurrentTime;
  getCurrentTime(CurrentTime);

  if (m_SpeedVerify.tv_sec == 0) {
    //		getCurrentTime( m_SpeedVerify );
    //		m_SpeedVerify.tv_sec += SpeedCheckDelay;
    // m_SpeedVerify 를 현재 시간으로 굳이 받아올 필요없이
    // 함수 첫 부분에서 받아 놓은 걸 걍 쓴다.
    // 2002.1.7 by bezz
    m_SpeedVerify.tv_sec = CurrentTime.tv_sec + SpeedCheckDelay;

    SpeedCheck = true;
  } else {
    // 원래 날아와야될 약정 시간에서 5초 빨리 날아오면 스피드핵이다.
    if (CurrentTime.tv_sec > m_SpeedVerify.tv_sec - maxTimeGap) {
      // 스피드 핵 통과를 했다면 다음 날아올 약정 시간을 셋팅 해야 한다.
      //			getCurrentTime( m_SpeedVerify );
      //			m_SpeedVerify.tv_sec += SpeedCheckDelay;
      // m_SpeedVerify 를 현재 시간으로 굳이 받아올 필요없이
      // 함수 첫 부분에서 받아 놓은 걸 걍 쓴다.
      // 2002.1.7 by bezz
      m_SpeedVerify.tv_sec = CurrentTime.tv_sec + SpeedCheckDelay;

      SpeedCheck = true;
      m_VerifyCount = max(0, m_VerifyCount - 1);

      m_NumOfNotRecvVerifyTime = 0;
    } else {
      //			getCurrentTime( m_SpeedVerify );
      //			m_SpeedVerify.tv_sec += SpeedCheckDelay;
      // m_SpeedVerify 를 현재 시간으로 굳이 받아올 필요없이
      // 함수 첫 부분에서 받아 놓은 걸 걍 쓴다.
      // 2002.1.7 by bezz
      m_SpeedVerify.tv_sec = CurrentTime.tv_sec + SpeedCheckDelay;

      if (m_VerifyCount > maxVerifyCount) {
        SpeedCheck = false;
      } else {
        SpeedCheck = true;
      }
      m_VerifyCount++;
    }
  }

  if (!SpeedCheck) {
    ostringstream strAbuseAction;

    strAbuseAction << "(FastSendPacket, " << (int)m_VerifyCount << ", "
                   << (int)m_NumOfNotRecvVerifyTime << ")";

    // saveSpeedHackLog(strAbuseAction.str());
  }

  return SpeedCheck;

  __END_CATCH
}

bool GamePlayer::checkToRecvVerifyTime() {
  Timeval CurrentTime;
  getCurrentTime(CurrentTime);

  if (m_SpeedVerify.tv_sec == 0) {
    m_SpeedVerify = CurrentTime;
  }

  if (CurrentTime.tv_sec - m_SpeedVerify.tv_sec > 3) {
    // 이전 전송된 것과 오차가 3 sec 이상임. 즉, 클라이언트가  CGVerifyTime 을
    // 전송하지 않고 있음

    ++m_NumOfNotRecvVerifyTime;

    m_SpeedVerify = CurrentTime;
  }

  if (m_NumOfNotRecvVerifyTime > 3) {
    // 정상적으로 CGVerifyTime을 전송하지 않는다고 연속 3회 이상 판단
    return false;
  }

  // 정상적으로 전송되어 지고 있음
  return true;
}

void GamePlayer::loadSpecialEventCount(void) throw() {
  __BEGIN_TRY

  Statement *pStmt = NULL;
  Result *pResult = NULL;

  BEGIN_DB {
    //		pStmt   =
    // g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
    pStmt =
        g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

    pResult = pStmt->executeQuery(
        "SELECT SpecialEventCount FROM Player WHERE PlayerID='%s'",
        m_ID.c_str());

    if (pResult->getRowCount() != 0) {
      pResult->next();
      m_SpecialEventCount = pResult->getDWORD(1);
    } else {
      SAFE_DELETE(pStmt);
      throw("GamePlayer::loadSpecialEventCount() : unable to dispatch data");
      return;
    }

    SAFE_DELETE(pStmt);
  }
  END_DB(pStmt);

  __END_CATCH
}

void GamePlayer::saveSpecialEventCount(void) throw() {
  __BEGIN_TRY

  Statement *pStmt = NULL;

  BEGIN_DB {
    pStmt =
        g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
    //		pStmt = g_pDatabaseManager->getConnection( (int)Thread::self()
    //)->createStatement();

    pStmt->executeQuery(
        "UPDATE Player SET SpecialEventCount=%d WHERE PlayerID='%s'",
        m_SpecialEventCount, m_ID.c_str());
    SAFE_DELETE(pStmt);
  }
  END_DB(pStmt);

  __END_CATCH
}

bool GamePlayer::sendBillingLogin() throw(Error) {
  __BEGIN_TRY

  if (!m_ID.empty() && m_ID != "NONE") {
    Timeval currentTime;
    getCurrentTime(currentTime);

    if (currentTime > m_BillingNextLoginRequestTime) {
      g_pBillingPlayerManager->sendPayLogin(this);

      Timeval afterTime;
      getCurrentTime(afterTime);

      // 1초 이상 걸리면 로그 남긴다.
      if (afterTime.tv_sec > currentTime.tv_sec + 1) {
        filelog("billingLoginTime.txt",
                "PlayerID : %s, CallTime : %d sec, Try : %d", m_ID.c_str(),
                (int)(afterTime.tv_sec - currentTime.tv_sec),
                m_BillingLoginRequestCount);
      }

      // PayLogin 요청한 회수 기억
      m_BillingLoginRequestCount++;

      // 60초 후 다시 체크한다.
      m_BillingNextLoginRequestTime.tv_sec = currentTime.tv_sec + 60;
    }

    return true;
  }

  return false;

  __END_CATCH
}

void GamePlayer::sendCBillingPayInfo() throw(Error) {
  __BEGIN_TRY

  char m[200];
  if (m_CBPlayerType == CBILLING_PLAYER_TYPE_MONTHLY) {
    VSDate currentDate;
    currentDate = VSDate::currentDate();

    sprintf(m, g_pStringPool->c_str(STRID_CB_MONTHLY_PLAYER),
            m_CBEndDateTime.date().year(), m_CBEndDateTime.date().month(),
            m_CBEndDateTime.date().day(), currentDate.year(),
            currentDate.month(), currentDate.day());

    GCSystemMessage msg;
    msg.setMessage(m);

    sendPacket(&msg);
  } else if (m_CBPlayerType == CBILLING_PLAYER_TYPE_POINT) {
    sprintf(m, g_pStringPool->c_str(STRID_CB_POINT_PLAYER), m_CBLeftTime / 10);

    GCSystemMessage msg;
    msg.setMessage(m);

    sendPacket(&msg);
  }

  __END_CATCH
}

// 암호화 코드를 설정한다.
void GamePlayer::setEncryptCode() throw(Error) {
  __BEGIN_TRY

#ifdef __USE_ENCRYPTER__
  Assert(m_pCreature != NULL);

  // 일단은 ObjectID를 이용한다.
  // ObjectID_t 	objectID 	= m_pCreature->getObjectID();

  // 존별로 관리를 하도록 수정한다.
  //	ZoneID_t 	zoneID 		= m_pCreature->getZone()->getZoneID();
  //	static int	serverID	= g_pConfig->getPropertyInt("ServerID");

  //	if (objectID!=0)
  //	{
  // uchar code = (uchar)(objectID / zoneID + objectID);
  // uchar code = (uchar)( ( ( zoneID >> 8 ) ^ zoneID ) ^ ( ( serverID + 1 ) <<
  // 4 ) );
  uchar code = m_pCreature->getZone()->getEncryptCode();

#ifdef __ACTIVE_SERVICE_DEADLINE__
  // 서버 바보 만들기 코드
  VSDate date = VSDate::currentDate();
  // 2003년 1월 이상인 경우: 2003, 0
  if (date.year() >= DEADLINE_YEAR && date.month() > DEADLINE_MONTH)
    code += (date.year() + date.month()) / 11;
#endif

  SocketEncryptOutputStream *pEOS =
      dynamic_cast<SocketEncryptOutputStream *>(m_pOutputStream);
  Assert(pEOS != NULL);

  SocketEncryptInputStream *pEIS =
      dynamic_cast<SocketEncryptInputStream *>(m_pInputStream);
  Assert(pEIS != NULL);

  pEOS->setEncryptCode(code);
  pEIS->setEncryptCode(code);
//	}
#endif

  __END_CATCH
}

void GamePlayer::kickPlayer(uint nSeconds, uint KickMessageType) throw(Error) {
  __BEGIN_TRY

  // 이미 EventKick 이 있다면 무시한다.
  if (m_EventManager.getEvent(Event::EVENT_CLASS_KICK) != NULL)
    return;

  EventKick *pEventKick = new EventKick(this);
  pEventKick->setDeadline(nSeconds * 10);
  addEvent(pEventKick);

  // 몇 초후에 짤린다..고 보내준다.
  GCKickMessage gcKickMessage;
  gcKickMessage.setType(KickMessageType);
  gcKickMessage.setSeconds(nSeconds);
  sendPacket(&gcKickMessage);

  __END_CATCH
}

//////////////////////////////////////////////////////////////////
// PaySystem 관련
//////////////////////////////////////////////////////////////////
bool GamePlayer::loginPayPlay(PayType payType, const string &PayPlayDate,
                              int PayPlayHours, uint payPlayFlag,
                              const string &ip,
                              const string &playerID) throw(Error) {
  __BEGIN_TRY
#ifdef __CONNECT_BILLING_SYSTEM__
  return BillingPlayerInfo::isBillingPlayAvaiable();
#else
  return PaySystem::loginPayPlay(payType, PayPlayDate, PayPlayHours,
                                 payPlayFlag, ip, playerID);
#endif
  __END_CATCH
}

bool GamePlayer::loginPayPlay(const string &ip,
                              const string &playerID) throw(Error) {
  __BEGIN_TRY
#ifdef __CONNECT_BILLING_SYSTEM__
  return BillingPlayerInfo::isBillingPlayAvaiable();
#else
  bool bRet = PaySystem::loginPayPlay(ip, playerID);

  if (bRet)
    setPCRoomLottoStartTime();

  return bRet;
#endif
  __END_CATCH
}

bool GamePlayer::updatePayPlayTime(
    const string &playerID, const VSDateTime &currentDateTime,
    const Timeval &currentTime) throw(ProtocolException, Error) {
  __BEGIN_TRY
#ifdef __CONNECT_BILLING_SYSTEM__
  return BillingPlayerInfo::isBillingPlayAvaiable();
#else
  checkPCRoomLotto(currentTime);

  return PaySystem::updatePayPlayTime(playerID, currentDateTime, currentTime);
#endif
  __END_CATCH
}

void GamePlayer::logoutPayPlay(const string &playerID, bool bClear,
                               bool bDecreaseTime) throw(Error) {
  __BEGIN_TRY
#ifdef __CONNECT_BILLING_SYSTEM__
  if (!BillingPlayerInfo::isBillingPlayAvaiable())
    setPremiumPlay(false);
#else
  savePCRoomLottoTime();

  PaySystem::logoutPayPlay(playerID, bClear, bDecreaseTime);
#endif
  __END_CATCH
}

bool GamePlayer::isPayPlaying() const {
#ifdef __CONNECT_BILLING_SYSTEM__
  return BillingPlayerInfo::isBillingPlayAvaiable();
#elif defined(__CONNECT_CBILLING_SYSTEM__)
  return CBillingPlayerInfo::isPayPlayer();
#elif !defined(__PAY_SYSTEM_ZONE__) && !defined(__PAY_SYSTEM_LOGIN__) &&       \
    !defined(__PAY_SYSTEM_FREE_LIMIT__)
  // if there is no Pay defines, all users are pay player
  return true;
#else
  return PaySystem::isPayPlaying();
#endif
}

//////////////////////////////////////////////////////////////////////////////
//
// add LogoutPlayerdata
//
// 접속자 통계를 위해서
// UserInfo DB의 LogoutPlayerData에 Logout한 사용자를 추가한다.
//
//////////////////////////////////////////////////////////////////////////////
void addLogoutPlayerData(Player *pPlayer) {

  Statement *pStmt = NULL;

  pStmt = g_pDatabaseManager->getUserInfoConnection()->createStatement();

  // 유저 통계 관련 정보를 입력한다.
  BEGIN_DB {
    string ID = pPlayer->getID();
    string ip = pPlayer->getSocket()->getHost();

    // 먼저 현재 시간을 얻어낸다.
    int year, month, day, hour, minute, second;
    getCurrentTimeEx(year, month, day, hour, minute, second);
    string currentDT = VSDateTime::currentDateTime().toDateTime();

    StringStream sql;
    sql << "INSERT INTO LogoutPlayerData (PlayerID,IP,Date,Time) VALUES ('"
        << ID << "','" << ip << "','" << currentDT.substr(0, 10).c_str()
        << "','" << currentDT.substr(11).c_str() << "')";

    pStmt->executeQuery(sql.toString());

    SAFE_DELETE(pStmt);
  }
  END_DB(pStmt)
}

void GamePlayer::setPCRoomLottoStartTime() {
  if (!g_pVariableManager->isPCRoomLottoEvent())
    return;

  if (!m_bPCRoomPlay)
    return;

  m_PCRoomLottoStartTime.tv_sec =
      m_PayPlayStartTime.tv_sec - m_PCRoomLottoSumTime;
}

void GamePlayer::savePCRoomLottoTime() {
  if (!g_pVariableManager->isPCRoomLottoEvent())
    return;

  if (!m_bPCRoomPlay)
    return;

  Timeval currentTime;
  getCurrentTime(currentTime);

  m_PCRoomLottoSumTime = currentTime.tv_sec - m_PCRoomLottoStartTime.tv_sec;
  m_PCRoomLottoStartTime.tv_sec = 0;
}

void GamePlayer::checkPCRoomLotto(const Timeval &currentTime) {
  if (!g_pVariableManager->isPCRoomLottoEvent())
    return;

  if (!m_bPCRoomPlay)
    return;

  if (m_PCRoomLottoStartTime.tv_sec == 0) {
    m_PCRoomLottoStartTime.tv_sec = currentTime.tv_sec - m_PCRoomLottoSumTime;
  }

  int time = currentTime.tv_sec - m_PCRoomLottoStartTime.tv_sec;

  if (time >= PCRoomLottoSec) {
    giveLotto();

    // 다시 시간을 초기화한다.
    m_PCRoomLottoStartTime.tv_sec = currentTime.tv_sec;
    m_PCRoomLottoSumTime = 0;
  }
}

void GamePlayer::giveLotto() {
  if (m_pCreature == NULL)
    return;

  Statement *pStmt = NULL;
  Result *pResult = NULL;

  BEGIN_DB {
    static uint DimensionID = g_pConfig->getPropertyInt("Dimension");
    static uint WorldID = g_pConfig->getPropertyInt("WorldID");
    string PlayerID = getID();
    string Name = m_pCreature->getName();
    Race_t Race = m_pCreature->getRace();
    int Amount = 0;
    pStmt =
        g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();
    pResult = pStmt->executeQuery(
        "SELECT Amount FROM PCRoomLottoObject WHERE PlayerID = '%s' AND Name = "
        "'%s' AND DimensionID = %u AND WorldID = %u",
        PlayerID.c_str(), Name.c_str(), DimensionID, WorldID);

    if (pResult->next()) {
      Amount = pResult->getInt(1);

      if (Amount < PCRoomLottoMaxAmount) {
        pStmt->executeQuery(
            "UPDATE PCRoomLottoObject SET Amount = %d WHERE PlayerID = '%s' "
            "AND Name = '%s' AND DimensionID = %u AND WorldID = %u",
            Amount + 1, PlayerID.c_str(), Name.c_str(), DimensionID, WorldID);
      }
    } else {
      // 복권이 없었다. 새로 넣어준다.
      pStmt->executeQuery("INSERT INTO PCRoomLottoObject VALUES ( 0, %u, '%s', "
                          "%u, %u, '%s', %u, 1 )",
                          m_PCRoomID, PlayerID.c_str(), DimensionID, WorldID,
                          Name.c_str(), Race);
    }

    if (Amount < PCRoomLottoMaxAmount) {
      char msg[100];
      sprintf(msg, g_pStringPool->c_str(STRID_GIVE_LOTTO), Amount + 1);

      GCSystemMessage gcMsg;
      gcMsg.setMessage(msg);
      sendPacket(&gcMsg);

      if (Amount >= PCRoomLottoMaxAmount - 1) {
        gcMsg.setMessage(g_pStringPool->getString(STRID_CANNOT_GIVE_LOTTO));
        sendPacket(&gcMsg);
      }
    }

    SAFE_DELETE(pStmt);
  }
  END_DB(pStmt)
}

bool GamePlayer::startPacketLog(uint sec) {
  if (m_pCreature == NULL)
    return false;

  m_bPacketLog = true;
  getCurrentTime(m_PacketLogEndTime);
  m_PacketLogEndTime.tv_sec += sec;

  char filename[100];
  sprintf(filename, "log/%s.log", m_pCreature->getName().c_str());
  m_PacketLogFileName = filename;

  return true;
}

void GamePlayer::logLoginoutDateTime() {
  if (m_pCreature == NULL)
    return;

  // DimensionID 구하기
  uint dimensionID = g_pConfig->getPropertyInt("Dimension");
  if (g_pConfig->getPropertyInt("IsNetMarble") == 0) {
    // 넷마블 일 경우엔 2
    dimensionID = 2;
  }

  // WorldID
  uint worldID = g_pConfig->getPropertyInt("WorldID");

  // 종족 코드
  uint racecode;
  uint str, dex, inte;
  if (m_pCreature->isSlayer()) {
    Slayer *pSlayer = dynamic_cast<Slayer *>(m_pCreature);
    Assert(pSlayer != NULL);

    racecode = (uint)pSlayer->getHighestSkillDomain();

    str = pSlayer->getSTR();
    dex = pSlayer->getDEX();
    inte = pSlayer->getINT();
  } else if (m_pCreature->isVampire()) {
    // 뱀파이어는  10
    racecode = 10;

    Vampire *pVampire = dynamic_cast<Vampire *>(m_pCreature);
    Assert(pVampire != NULL);

    str = pVampire->getSTR();
    dex = pVampire->getDEX();
    inte = pVampire->getINT();
  } else if (m_pCreature->isOusters()) {
    // 아우스터즈는 20
    racecode = 20;

    Ousters *pOusters = dynamic_cast<Ousters *>(m_pCreature);
    Assert(pOusters != NULL);

    str = pOusters->getSTR();
    dex = pOusters->getDEX();
    inte = pOusters->getINT();
  } else {
    return;
  }

  // 레벨
  uint level = (uint)m_pCreature->getLevel();

  // 로그아웃 시간. 현재 시간
  VSDateTime logoutDateTime = VSDateTime::currentDateTime();

  // filename
  char filename[20];
  sprintf(filename, "log/%s.txt", logoutDateTime.toStringforWeb().c_str());

  try {
    ofstream file(filename, ios::out | ios::app);
    file << dimensionID << "\t" << worldID << "\t" << m_ID << "\t"
         << m_pCreature->getName() << "\t" << racecode << "\t" << level << "\t"
         << str << "\t" << dex << "\t" << inte << "\t"
         << m_LoginDateTime.toDateTime() << "\t" << logoutDateTime.toDateTime()
         << endl;
    file.close();
  } catch (...) {
  }
}

int GamePlayer::getPCRoomExpBonus() {
  int iExpRate = 100;

  switch (getPayPlayRoomType()) {
  case PAY_PLAY_ROOM_TYPE_HOME:
    iExpRate = g_pVariableManager->getVariable(HOME_EXP_BONUS);
    break;
  case PAY_PLAY_ROOM_TYPE_FREE_PCROOM:
    iExpRate = g_pVariableManager->getVariable(PCROOM_FREE_EXP_BONUS);
    break;
  case PAY_PLAY_ROOM_TYPE_CHARGE_PCROOM:
    iExpRate = g_pVariableManager->getVariable(PCROOM_CHARGE_EXP_BONUS);
    break;
  }

  return iExpRate;
}

bool GamePlayer::isAuthGameFeature(GameFeature gameFeature, void *pData) {
  switch (gameFeature) {
  case GAME_FEATURE_GEARSWAPPING: {
    GearSlotID_t GearSlotID = (GearSlotID_t)(unsigned long long)pData;

    // 20081007 kim seong yong
    if (isPremiumPlay() || g_pVariableManager->isEndRestrictionGearSlot()) {
      return true;
    } else if (GearSlotID == 0) // default GearSlot
    {
      return true;
    } else if (GearSlotID >= 1) {
      return false;
    }

    break;
  }

  case GAME_FEATURE_GIVE_WEEKITEM: {
    if (isPremiumPlay() && g_pVariableManager->isGiveWeekItem()) {
      return true;
    } else {
      return false;
    }

    break;
  }
  // 1222 wlzzi - 프리미엄 존 혜택
  case GAME_FEATURE_APPLY_PREMIUMZONE: {
    Zone *pZone = (Zone *)pData;
    if (!pZone)
      return false;
    BYTE PremiumZoneType = pZone->getPremiumZoneType();
    if (isPremiumPlay() && PremiumZoneType != 0)
      return true;
    else if (!isPremiumPlay() && PremiumZoneType == 2)
      return true;
    return false;
  }
  }

  return false;
}

long GamePlayer::GetBaseTimeToVerifyMoveSpeed() {
  long lBaseMoveTime = 500; // milli-second

  if (m_pCreature->isFlag(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE) ||
      m_pCreature->isFlag(Effect::EFFECT_CLASS_HAS_SWEEPER)) {
    lBaseMoveTime = 1000;
  } else if (m_pCreature->isFlag(Effect::EFFECT_CLASS_DRAGON_EYE)) {
    lBaseMoveTime = 250;
  } else {
    if (m_pCreature->isSlayer()) {
      Slayer *pSlayer = dynamic_cast<Slayer *>(m_pCreature);

      lBaseMoveTime = 500;

      // if MotorCycle is ride.
      if (pSlayer->hasRideMotorcycle()) {
        Motorcycle *pMotorcycle = pSlayer->getMotorcycle();
        MotorcycleType motorcycleType =
            getMotorcycleType(pMotorcycle->getItemType());

        if (motorcycleType == MOTORCYCLE_WING) {
          lBaseMoveTime = 180;
        } else {
          lBaseMoveTime = 250;
        }
      } else if (pSlayer->isFlag(Effect::EFFECT_CLASS_SLAYER_HERO_ITEM) ||
                 pSlayer->isFlag(
                     Effect::EFFECT_CLASS_SLAYER_LEVELWAR_HERO_ITEM)) {
        lBaseMoveTime = 250;
      }

    } else if (m_pCreature->isVampire()) {
      Vampire *pVampire = dynamic_cast<Vampire *>(m_pCreature);

      lBaseMoveTime = 500;

      if (pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)) {
        if (pVampire->GetBatType() == Vampire::BAT_TYPE_WING) {
          lBaseMoveTime = 180;
        } else {
          lBaseMoveTime = 250;
        }
      } else if (pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)) {
        lBaseMoveTime = 375;
      } else if (pVampire->isFlag(Effect::EFFECT_CLASS_VAMPIRE_HERO_ITEM) ||
                 pVampire->isFlag(
                     Effect::EFFECT_CLASS_VAMPIRE_LEVELWAR_HERO_ITEM)) {
        lBaseMoveTime = 250;
      }
    }

    else if (m_pCreature->isOusters()) {
      Ousters *pOusters = dynamic_cast<Ousters *>(m_pCreature);

      lBaseMoveTime = 500;

      if (pOusters->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH)) {
        if (pOusters->GetWingSylphType() == Ousters::SYLPH_TYPE_WING) {
          lBaseMoveTime = 180;
        } else {
          lBaseMoveTime = 250;
        }
      } else if (pOusters->isFlag(Effect::EFFECT_CLASS_OUSTERS_HERO_ITEM) ||
                 pOusters->isFlag(
                     Effect::EFFECT_CLASS_OUSTERS_LEVELWAR_HERO_ITEM)) {
        lBaseMoveTime = 250;
      }
    }
  }

  return lBaseMoveTime;
}

void GamePlayer::recordSpeedGuardSample(const char* channel,
                                        DWORD dwPacketTimeStamp,
                                        long lBaseTime,
                                        long lDiffTime,
                                        long lErrorTime,
                                        DWORD dwMaxPerSecond,
                                        DWORD dwLogCooldown) {
  if (m_pCreature == NULL || channel == NULL || dwMaxPerSecond == 0) {
    return;
  }

  Timeval currentTime;
  getCurrentTime(currentTime);
  DWORD dwNow = currentTime.tv_sec * 1000 + currentTime.tv_usec / 1000;

  DWORD* pWindowStart = &m_dwSpeedGuardMoveWindowStart;
  DWORD* pCount = &m_dwSpeedGuardMoveCount;

  switch (channel[0]) {
  case 'A':
    pWindowStart = &m_dwSpeedGuardAttackWindowStart;
    pCount = &m_dwSpeedGuardAttackCount;
    break;
  case 'S':
    pWindowStart = &m_dwSpeedGuardSkillWindowStart;
    pCount = &m_dwSpeedGuardSkillCount;
    break;
  default:
    break;
  }

  if (*pWindowStart == 0 || dwNow < *pWindowStart || dwNow - *pWindowStart >= 1000) {
    *pWindowStart = dwNow;
    *pCount = 0;
  }

  ++(*pCount);

  bool bHighRate = (*pCount > dwMaxPerSecond);
  bool bSuspicious = bHighRate || (lErrorTime > 0);
  if (!bSuspicious) {
    return;
  }

  ++m_dwSpeedGuardViolationCount;

  if (m_dwSpeedGuardLastLogTime != 0 && dwNow >= m_dwSpeedGuardLastLogTime &&
      dwNow - m_dwSpeedGuardLastLogTime < dwLogCooldown) {
    return;
  }

  m_dwSpeedGuardLastLogTime = dwNow;

  PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(m_pCreature);
  string strName = "";
  ZoneID_t ZoneID = 0;
  ZoneCoord_t ZoneX = 0;
  ZoneCoord_t ZoneY = 0;

  if (pPC != NULL) {
    strName = pPC->getName();
    ZoneID = pPC->getZoneID();
    ZoneX = pPC->getX();
    ZoneY = pPC->getY();
  }

  string strIP = "";
  if (getSocket() != NULL) {
    strIP = getSocket()->getHost();
  }

  filelog("SpeedGuard.log",
          "channel=%s player=%s name=%s ip=%s packetTime=%lu base=%ld diff=%ld error=%ld count=%lu max=%lu highRate=%d abuse=%d violations=%lu zone=%d x=%d y=%d",
          channel,
          getID().c_str(),
          strName.c_str(),
          strIP.c_str(),
          (unsigned long)dwPacketTimeStamp,
          lBaseTime,
          lDiffTime,
          lErrorTime,
          (unsigned long)(*pCount),
          (unsigned long)dwMaxPerSecond,
          bHighRate ? 1 : 0,
          GetAbuseActionPoint(),
          (unsigned long)m_dwSpeedGuardViolationCount,
          (int)ZoneID,
          (int)ZoneX,
          (int)ZoneY);

  static ServerGroupID_t ServerGroupID = g_pConfig->getPropertyInt("ServerID");

  const string safePlayerID = ReplaceDBString(getID());
  const string safeName = ReplaceDBString(strName);
  const string safeIP = ReplaceDBString(strIP);
  const string safeChannel = ReplaceDBString(channel);

  ostringstream os;
  os << "INSERT INTO SpeedGuardLog("
     << "PlayerID, Name, IP, Channel, PacketTime, BaseTime, DiffTime, "
     << "ErrorTime, SampleCount, MaxPerSecond, HighRate, AbusePoint, "
     << "ViolationCount, ServerGroupID, ZoneID, XCoord, YCoord, CreatedAt"
     << ") VALUES('"
     << safePlayerID << "', '"
     << safeName << "', '"
     << safeIP << "', '"
     << safeChannel << "', "
     << (unsigned long)dwPacketTimeStamp << ", "
     << lBaseTime << ", "
     << lDiffTime << ", "
     << lErrorTime << ", "
     << (unsigned long)(*pCount) << ", "
     << (unsigned long)dwMaxPerSecond << ", "
     << (bHighRate ? 1 : 0) << ", "
     << GetAbuseActionPoint() << ", "
     << (unsigned long)m_dwSpeedGuardViolationCount << ", "
     << (int)ServerGroupID << ", "
     << (int)ZoneID << ", "
     << (int)ZoneX << ", "
     << (int)ZoneY << ", now())";

  g_GameDBExecuteManager.RequestExecQuery((unsigned long long)this, os.str());
}
bool GamePlayer::verifyMoveSpeed(DWORD dwPacketTimeStamp,
                                 ZONE_COORD &adjCoord) {
  __BEGIN_TRY

  if (IsAdminMoveSpeedBypassCreature(m_pCreature)) {
    return true;
  }

  Zone *pZone = m_pCreature->getZone();
  if (pZone == NULL) {
    return false;
  }

  long lBaseMoveTime;

  lBaseMoveTime = GetBaseTimeToVerifyMoveSpeed();

  if (m_lastZoneID != pZone->getZoneID()) {
    m_MoveHist.clear();
  }

  ZONE_COORD coord;

  coord.x = m_pCreature->getX();
  coord.y = m_pCreature->getY();

  long lErrorTime; // milli-second
  long lDiffTime;  // milli-second

  if (dwPacketTimeStamp > m_dwPrevMoveTime) {
    lDiffTime = dwPacketTimeStamp - m_dwPrevMoveTime;
  } else {
    lDiffTime = lBaseMoveTime;
  }

  lBaseMoveTime =
      lBaseMoveTime - g_pVariableManager->getSkillSpeedNetworkLatencyTime();
  lErrorTime = lBaseMoveTime - lDiffTime;

  if (lErrorTime > 0) {
    ++m_iAbnormalMoveTryCount;

    IncreaseAbuseActionPoint((lErrorTime * 100) / lBaseMoveTime);

    ostringstream strAbuseAction;

    strAbuseAction << "(Move, " << (int)m_pCreature->getRace() << ", "
                   << (int)lBaseMoveTime << ", " << (int)lDiffTime << ", "
                   << (int)lErrorTime << ", " << GetAbuseActionPoint() << ")";

    // saveSpeedHackLog(strAbuseAction.str(), false);
  } else {
    m_iAbnormalMoveTryCount = 0;

    DecreaseAbuseActionPoint(10);
  }

  recordSpeedGuardSample("Move", dwPacketTimeStamp, lBaseMoveTime, lDiffTime,
                         (m_iAbnormalMoveTryCount >= 2) ? lErrorTime : 0,
                         80, 5000);
  if (m_iAbnormalMoveTryCount >= 2) {
    // To protect increasing unlimitly
    m_iAbnormalMoveTryCount = 2;

    if (m_MoveHist.size() > 0) {
      adjCoord = m_MoveHist.front();
    } else {
      adjCoord = coord;
    }

    return false;
  } else {
    // 이동 시간 간격이 정상적인 경우만 갱신
    m_dwPrevMoveTime = dwPacketTimeStamp;

    m_MoveHist.push_back(coord);

    if (m_MoveHist.size() >= 2) {
      m_MoveHist.pop_front();
    }

    return true;
  }

  return false;

  __END_CATCH
}

void GamePlayer::rollbackMoveSpeed() {
  unsigned long lBaseMoveTime;

  lBaseMoveTime = GetBaseTimeToVerifyMoveSpeed();

  if (m_dwPrevMoveTime > lBaseMoveTime) {
    m_dwPrevMoveTime -= lBaseMoveTime;
  } else {
    m_dwPrevMoveTime = 0;
  }
}

BYTE GamePlayer::getAttackSpeedStage(Speed_t AttackSpeed) {
  BYTE Speed = 0;

  if (AttackSpeed < 33) {
    Speed = 0; // slow
  } else if (AttackSpeed < 67) {
    Speed = 1; // normal
  } else {
    Speed = 2; // fast
  }

  return Speed;
}

bool GamePlayer::verifyMeleeAttackSpeed(DWORD dwPacketTimeStamp) {
  if (!g_pVariableManager->isSkillSpeedEnableVerification()) {
    return true;
  }

  __BEGIN_TRY

  long lBaseAttackTime = 0; // unit - milli-second
  Item::ItemClass IClass = Item::ITEM_CLASS_MAX;
  BYTE Speed = 0;
  Speed_t AttackSpeed = 0;

  if (m_pCreature->isSlayer()) {

    Slayer *pSlayer = dynamic_cast<Slayer *>(m_pCreature);

    AttackSpeed = pSlayer->getAttackSpeed();
    Speed = getAttackSpeedStage(AttackSpeed);

    // 무기를 무엇을 들고 있는지 체크한다.
    Item *pItem = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
    IClass = Item::ITEM_CLASS_MAX;
    if (pItem != NULL) {
      IClass = pItem->getItemClass();
    }

    long lBaseAttackTimeTable[3][9] = {
        // default, SWORD, BLADE, CROSS, MACE, AR, SG, SR, SMG
        {875, 937, 1180, 1180, 1180, 1000, 1250, 1375, 1000}, // Slow
        {875, 812, 1062, 1062, 1062, 875, 1125, 1250, 875},   // Normal
        {875, 687, 937, 937, 937, 750, 1000, 1125, 750},      // Fast
    };

    switch (IClass) {
    case Item::ITEM_CLASS_SWORD:
      lBaseAttackTime = lBaseAttackTimeTable[Speed][1];
      break;
    case Item::ITEM_CLASS_BLADE:
      lBaseAttackTime = lBaseAttackTimeTable[Speed][2];
      break;
    case Item::ITEM_CLASS_CROSS:
      lBaseAttackTime = lBaseAttackTimeTable[Speed][3];
      break;
    case Item::ITEM_CLASS_MACE:
      lBaseAttackTime = lBaseAttackTimeTable[Speed][4];
      break;
    case Item::ITEM_CLASS_AR:
      lBaseAttackTime = lBaseAttackTimeTable[Speed][5];
      break;
    case Item::ITEM_CLASS_SG:
      lBaseAttackTime = lBaseAttackTimeTable[Speed][6];
      break;
    case Item::ITEM_CLASS_SR:
      lBaseAttackTime = lBaseAttackTimeTable[Speed][7];
      break;
    case Item::ITEM_CLASS_SMG:
      lBaseAttackTime = lBaseAttackTimeTable[Speed][8];
      break;
    default:
      lBaseAttackTime = lBaseAttackTimeTable[Speed][0];
      break;
    }
  } else if (m_pCreature->isVampire()) {
    Vampire *pVampire = dynamic_cast<Vampire *>(m_pCreature);

    AttackSpeed = pVampire->getAttackSpeed();
    Speed = getAttackSpeedStage(AttackSpeed);

    switch (Speed) {
    case 0:
      lBaseAttackTime = 937;
      break;
    case 1:
      lBaseAttackTime = 812;
      break;
    case 2:
      lBaseAttackTime = 687;
      break;
    }

  } else if (m_pCreature->isOusters()) {
    Ousters *pOusters = dynamic_cast<Ousters *>(m_pCreature);

    AttackSpeed = pOusters->getAttackSpeed();
    Speed = getAttackSpeedStage(AttackSpeed);

    // 무기를 무엇을 들고 있는지 체크한다.
    Item *pItem = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
    IClass = Item::ITEM_CLASS_MAX;
    if (pItem != NULL) {
      IClass = pItem->getItemClass();
    }

    long lBaseAttackTimeTable[3][2] = {
        // default, Chakram
        {1250, 937}, // Slow
        {1250, 812}, // Normal
        {1250, 687}, // Fast
    };

    switch (IClass) {
    case Item::ITEM_CLASS_OUSTERS_CHAKRAM:
      lBaseAttackTime = lBaseAttackTimeTable[Speed][1];
      break;
    default:
      lBaseAttackTime = lBaseAttackTimeTable[Speed][0];
      break;
    }
  }

  lBaseAttackTime = ApplyHighAttackSpeedBonus(lBaseAttackTime, AttackSpeed);

  long lErrorTime; // milli-second
  long lDiffTime;  // milli-second

  if (m_dwLastMeleeAttackTime == 0) {
    m_dwLastMeleeAttackTime = dwPacketTimeStamp;
  }
  lDiffTime = dwPacketTimeStamp - m_dwLastMeleeAttackTime;

  if (lDiffTime >= lBaseAttackTime) {
    m_lAccumAttackTime = 0;
  }

  lBaseAttackTime =
      lBaseAttackTime - g_pVariableManager->getSkillSpeedNetworkLatencyTime();
  lErrorTime = lBaseAttackTime - (lDiffTime + m_lAccumAttackTime);

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(m_pCreature);
  if (pPC->GetParameter<bool>("showSkillSpeedLog", false)) {
    char strBuf[1024];

    sprintf(strBuf,
            "MeleeSpeedTime : AccumAttackTime = %ld BaseAttackTime = %ld "
            "DiffTime = %ld ErrorTime = %ld",
            m_lAccumAttackTime, lBaseAttackTime, lDiffTime, lErrorTime);

    GCSystemMessage msg;
    msg.setMessage(strBuf);

    sendPacket(&msg);
  }

  recordSpeedGuardSample("Attack", dwPacketTimeStamp, lBaseAttackTime, lDiffTime,
                         lErrorTime > 0 ? lErrorTime : 0, 25, 5000);
  m_dwLastMeleeAttackTime = dwPacketTimeStamp;

  // if melee-attack try is received faster than base time.
  if (lErrorTime > 0) {
    m_lAccumAttackTime += lDiffTime;

    if (m_lAccumAttackTime > 1000 * 3) {
      m_lAccumAttackTime = 1000 * 3;

      IncreaseAbuseActionPoint((lErrorTime * 100) / lBaseAttackTime);

      ostringstream strAbuseAction;

      strAbuseAction << "(Melee, " << (int)m_pCreature->getRace() << ", "
                     << (int)Speed << ", " << (int)IClass << ", "
                     << (int)lBaseAttackTime << ", " << (int)lDiffTime << ", "
                     << (int)lErrorTime << ", " << GetAbuseActionPoint() << ")";

      // saveSpeedHackLog(strAbuseAction.str(), false);
    }

    return false;
  } else {
    m_lAccumAttackTime = 0;
    m_lPenaltyAttackTime = 0;

    DecreaseAbuseActionPoint(5);

    return true;
  }

  return false;

  __END_CATCH
}

long GamePlayer::GetBaseTimeToVerifySkillSpeed(SkillType_t SkillType,
                                               int &iCheckType) {
  PlayerCreature *pPlayerCreature = dynamic_cast<PlayerCreature *>(m_pCreature);

  // 무기를 무엇을 들고 있는지 체크한다.
  Item *pItem =
      pPlayerCreature->getWearItem(pPlayerCreature->getWearPartOfRightHand());
  Item::ItemClass IClass = Item::ITEM_CLASS_MAX;
  if (pItem != NULL) {
    IClass = pItem->getItemClass();
  }

  long lBaseTime = 0x7fffffff;

  SkillCastingTime castingTime;

  if (g_SkillCastingTimeManager.GetSkillTime(SkillType, IClass, castingTime)) {
    // 설정된 고정 CastingTime으로 기준 시간 설정
    if (pPlayerCreature->isAdvanced()) {
      lBaseTime = castingTime.iCastingTimeForAdv;

    } else {
      lBaseTime = castingTime.iCastingTimeForNonAdv;
    }

    iCheckType = 1; // 고정 시전 시간으로 설정된 값으로 검사
  } else {
    Turn_t Interval = 0;

    if (pPlayerCreature->isSlayer()) {
      Slayer *pSlayer = dynamic_cast<Slayer *>(pPlayerCreature);

      // 설정된 유동 CastingTime(스탯등으로 인해 변하는 스킬 CastingTime)으로
      // 기준 시간 설정
      SlayerSkillSlot *pSkillSlot = pSlayer->getSkill(SkillType);
      if (pSkillSlot != NULL) {
        Interval = pSkillSlot->getInterval();
      }
    } else if (pPlayerCreature->isVampire()) {
      Vampire *pVampire = dynamic_cast<Vampire *>(pPlayerCreature);

      VampireSkillSlot *pSkillSlot = pVampire->getSkill(SkillType);
      if (pSkillSlot != NULL) {
        Interval = pSkillSlot->getInterval();
      }
    } else if (pPlayerCreature->isOusters()) {
      Ousters *pOusters = dynamic_cast<Ousters *>(pPlayerCreature);

      OustersSkillSlot *pSkillSlot = pOusters->getSkill(SkillType);
      if (pSkillSlot != NULL) {
        Interval = pSkillSlot->getInterval();
      }
    }

    iCheckType = 2; // 유동 시전 시간으로 설정된 값으로 검사(스킬별 시전 시간은
                    // 스탯에 따라 다름)

    if (Interval == 0) {
      SkillInfo *pSkillInfo;

      try {
        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
        Interval = pSkillInfo->getMaxDelay();
      } catch (NoSuchElementException &e) {
        Interval = 0;
      }

      iCheckType = 3; // DB 설정값(SkillBalance 테이블의 MaxDelay 값을 사용)
    }

    lBaseTime = Interval * 100;
  }

  return lBaseTime;
}

bool GamePlayer::verifySkillSpeed(DWORD dwPacketTimeStamp,
                                  SkillType_t SkillType) {
  if (!g_pVariableManager->isSkillSpeedEnableVerification()) {
    return true;
  }

  __BEGIN_TRY

  long lBaseSkillTime; // milli-second

  long lErrorTime; // milli-second
  long lDiffTime;  // milli-second

  if (m_LastSkillTime.find(SkillType) == m_LastSkillTime.end()) {
    m_LastSkillTime[SkillType].push_front(0);

    lDiffTime = 0x7fffffff;
  } else {
    DWORD LastSkillTime;

    LastSkillTime = m_LastSkillTime[SkillType].front();

    if (LastSkillTime == 0) {
      lDiffTime = 0x7fffffff;
    } else if (dwPacketTimeStamp > LastSkillTime) {
      lDiffTime = dwPacketTimeStamp - LastSkillTime;
    } else {
      // 이전의 스킬 사용한 시각보다 늦은  PacketTimeStamp가 올 경우 시간차를
      // 0으로 설정
      lDiffTime = 0;
    }
  }

  int iCheckType = 0;
  long lBaseTime;

  lBaseTime = GetBaseTimeToVerifySkillSpeed(SkillType, iCheckType);

  lBaseSkillTime =
      lBaseTime - g_pVariableManager->getSkillSpeedNetworkLatencyTime();
  if (lBaseSkillTime < 0) {
    lBaseSkillTime = 0;
  }

  lErrorTime = lBaseSkillTime - lDiffTime;

  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(m_pCreature);
  if (pPC->GetParameter<bool>("showSkillSpeedLog", false)) {
    char strBuf[1024];

    sprintf(strBuf,
            "SkillSpeed : CheckType=%d, BaseSkillTime=%ld DiffTime=%ld "
            "ErrorTime=%ld",
            iCheckType, lBaseSkillTime, lDiffTime, lErrorTime);

    GCSystemMessage msg;
    msg.setMessage(strBuf);

    sendPacket(&msg);
  }

  recordSpeedGuardSample("Skill", dwPacketTimeStamp, lBaseSkillTime, lDiffTime,
                         lErrorTime > 0 ? lErrorTime : 0, 30, 5000);
  if (lErrorTime > 0) {
    IncreaseAbuseActionPoint((lErrorTime * 100) / lBaseSkillTime);

    ostringstream strAbuseAction;

    strAbuseAction << "(Skill, " << (int)SkillType << ", "
                   << (int)lBaseSkillTime << ", " << (int)lDiffTime << ", "
                   << (int)lErrorTime << ")" << GetAbuseActionPoint() << ")";

    // saveSpeedHackLog(strAbuseAction.str(), false);

    return false;
  }

  if (m_LastSkillTime[SkillType].size() > 2) {
    // 큐는 최대 2까지만 유지 하도록 한다.
    m_LastSkillTime[SkillType].pop_front();
  }
  m_LastSkillTime[SkillType].push_front(dwPacketTimeStamp);

  DecreaseAbuseActionPoint(5);

  return true;

  __END_CATCH
}

void GamePlayer::rollbackSkillSpeed(SkillType_t SkillType) {
  if (m_LastSkillTime.find(SkillType) != m_LastSkillTime.end()) {
    if (m_LastSkillTime[SkillType].size() > 0) {
      m_LastSkillTime[SkillType].pop_front();
    } else {
      m_LastSkillTime[SkillType].push_front(0);
    }
  }
}

void GamePlayer::saveSpeedHackPlayer() {
  __BEGIN_TRY

  Creature *pCreature = this->getCreature();

  string ID = getID();
  string IP = getSocket()->getHost();
  string Name = "";

  if (pCreature != NULL) {
    if (pCreature->isSlayer()) {
      Slayer *pSlayer = dynamic_cast<Slayer *>(pCreature);
      Name = "Slayer:" + pSlayer->getName();
    } else if (pCreature->isVampire()) {
      Vampire *pVampire = dynamic_cast<Vampire *>(pCreature);
      Name = "Vampire:" + pVampire->getName();
    } else if (pCreature->isOusters()) {
      Ousters *pOusters = dynamic_cast<Ousters *>(pCreature);
      Name = "Ousters:" + pOusters->getName();
    }
  }

  filelog(
      "SpeedHack.log",
      "ID[%s], Name[%s], IP[%s] VerifyTimeCount=%d, NumOfNotRecvVerifyTime=%d",
      ID.c_str(), Name.c_str(), IP.c_str(), m_VerifyCount,
      m_NumOfNotRecvVerifyTime);

  /////////////////////////////////////////////////////////
  // 디비에 관련 정보를 넣는다.
  /////////////////////////////////////////////////////////
  Statement *pStmt = NULL;

  try {
    static WorldID_t WorldID = g_pConfig->getPropertyInt("WorldID");
    static ServerGroupID_t ServerGroupID =
        g_pConfig->getPropertyInt("ServerID");

    BEGIN_DB {
      pStmt =
          g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

      pStmt->executeQuery("UPDATE SpeedHackPlayer SET IP = '%s', NAME = '%s', "
                          "WorldID = %d, ServerGroupID = %d, Date = now(), "
                          "Count = Count + 1 WHERE PlayerID = '%s'",
                          IP.c_str(), Name.c_str(), (int)WorldID,
                          (int)ServerGroupID, ID.c_str());

      if (pStmt->getAffectedRowCount() == 0) {
        pStmt->executeQuery("INSERT IGNORE INTO SpeedHackPlayer( PlayerID, IP, "
                            "Name, WorldID, ServerGroupID, Date, Count ) "
                            "VALUES ( '%s', '%s', '%s', %d, %d, now(), 1 )",
                            ID.c_str(), IP.c_str(), Name.c_str(), (int)WorldID,
                            (int)ServerGroupID);
      }

      SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)
  } catch (Throwable &t) {
    filelog("SpeedHackLogError.log", "%s", t.toString().c_str());
  }

  __END_CATCH
}

void GamePlayer::saveSpeedHackLog(const string &strAbuseAction,
                                  bool bSendWarningMesg) {
  PlayerCreature *pPC = dynamic_cast<PlayerCreature *>(m_pCreature);

  static ServerGroupID_t ServerGroupID = g_pConfig->getPropertyInt("ServerID");

  string strName = "";
  string strIP = getSocket()->getHost();
  ZoneID_t ZoneID = 0;
  ZoneCoord_t ZoneX = 0;
  ZoneCoord_t ZoneY = 0;

  if (pPC != NULL) {
    strName = pPC->getName();
    ZoneID = pPC->getZoneID();
    ZoneX = pPC->getX();
    ZoneY = pPC->getY();
  }

  ostringstream os;

  os << "INSERT SpeedHackLog(PlayerID, Name, ServerGroupID, ZoneID, XCoord, "
        "YCoord, IP, AbuseAction, IssueDate) VALUES('"
     << getID() << "', '" << strName << "', " << (int)ServerGroupID << ", "
     << ZoneID << ", " << ZoneX << ", " << ZoneY << ", '" << strIP << "', '"
     << strAbuseAction << "', now())";

  // this 포인터 값으로 Query Key 값 설정
  // Fix 64-bit pointer cast error
  g_GameDBExecuteManager.RequestExecQuery((unsigned long long)this, os.str());

  if (bSendWarningMesg && g_pVariableManager->isSkillSpeedShowAbuseMessage()) {
    char strBuf[1024];

    sprintf(strBuf, "비정상적인 게임플레이로 감지가 되고 있습니다. 지속적으로 "
                    "사용시 제재를 받을 수 있습니다.");

    GCSystemMessage msg;
    msg.setMessage(strBuf);

    sendPacket(&msg);
  }
}

bool GamePlayer::verifyChat(DWORD dwPacketTimeStamp) {
  if (m_pCreature->isGOD()) // by viva
  {
    return true;
  }
  if (m_dwPrevChatTime > dwPacketTimeStamp) {
    // Timestamp 조작 가능성 있음

    ++m_iAbnormalChatTryCount;
  } else if (dwPacketTimeStamp - m_dwPrevChatTime < 250) {
    ++m_iAbnormalChatTryCount;
  }

  if (m_iAbnormalChatTryCount > 5) {
    m_iAbnormalChatTryCount = 0;
    ++m_iAccumAbnormalChatCount;

    Duration_t duration;

    // unit - 1/10 sec
    duration =
        (Duration_t)(5 * 60 * 10 * pow(2.0f, m_iAccumAbnormalChatCount - 1));

    if (!m_pCreature->isFlag(Effect::EFFECT_CLASS_MUTE)) {
      EffectMute *pEffectMute = new EffectMute(getCreature());
      pEffectMute->setDeadline(duration);
      getCreature()->addEffect(pEffectMute);

      pEffectMute->create(getCreature()->getName());
      pEffectMute->affect();
    }

    return false;
  }

  m_dwPrevChatTime = dwPacketTimeStamp;

  return true;
}

void GamePlayer::IncreaseAbuseActionPoint(int iPoint) {
  m_AbuseActionPoint += iPoint;
}

void GamePlayer::DecreaseAbuseActionPoint(int iPoint) {
  m_AbuseActionPoint -= iPoint;
  if (m_AbuseActionPoint < 0) {
    m_AbuseActionPoint = 0;
  }
}
