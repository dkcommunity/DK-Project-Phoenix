//////////////////////////////////////////////////////////////////////
// 
// Filename    : LCLoginOK.h 
// Written By  : Reiot
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __LC_LOGIN_OK_H__
#define __LC_LOGIN_OK_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCLoginOK;
//
// 로그인서버가 클라이언트에게 로그인 성공을 알려주는 패킷이다.
//
//////////////////////////////////////////////////////////////////////

class LCLoginOK : public Packet {

public:

    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_LC_LOGIN_OK; }
	
	// get packet body size
	// *OPTIMIZATION HINT*
	// const static LCLoginOKPacketSize 를 정의, 리턴하라.
	PacketSize_t getPacketSize() const ;
	
	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName() const  { return "LCLoginOK"; }

	#endif

	// get / set GoreLevel
	bool isAdult() const  { return m_isAdult; }
	void setAdult(bool isAdult)  { m_isAdult = isAdult; }

	// is UnderFifthteen
	bool isUnderFifthteen() const  { return m_bUnderFifthteen; }
	void setUnderFifthteen( bool bUnderFifthteen ) { m_bUnderFifthteen = bUnderFifthteen; }

	bool isFamily() const  { return m_bFamily; }
	void setFamily(bool isFamily)  { m_bFamily = isFamily; }

	BYTE getStat() const  { return m_Stat; }
	void setStat(BYTE Stat)  { m_Stat = Stat; }

	WORD getLastDays() const  { return m_LastDays; }
	void setLastDays( WORD day )  { m_LastDays = day; }
#if __CONTENTS(__PAYZONE_PASS_TICKET)
	//	20080312 유료존 티켓 추가로 PayType 추가
	BYTE getPayType() const  { return m_PayType; }
	void setPayType(BYTE PayType)  { m_PayType = PayType; }
#endif //__PAYZONE_PASS_TICKET
	// get packet's debug std::string
	std::string toString() const  { return "LCLoginOK"; }

#if __CONTENTS(__LOGIN_PACKET)
	BYTE getReservedValue() const  { return m_ReservedValue; }
	void setReservedValue(BYTE ReservedValue)  { m_ReservedValue = ReservedValue; }
#endif //__LOGIN_PACKET

	BYTE getServerID() const  { return m_ServerID; }
	void setServerID(BYTE ServerID)  { m_ServerID = ServerID; }

	BYTE getServerIDA() const  { return m_ServerIDA; }
	void setServerIDA(BYTE ServerIDA)  { m_ServerIDA = ServerIDA; }

	BYTE getServerIDB() const  { return m_ServerIDB; }
	void setServerIDB(BYTE ServerIDB)  { m_ServerIDB = ServerIDB; }

	BYTE getServerIDC() const  { return m_ServerIDC; }
	void setServerIDC(BYTE ServerIDC)  { m_ServerIDC = ServerIDC; }


private :

	// 고어 레벨 : 현재 플레이어가 미성년자 인가?
	// true일 경우 성인
	// false 일 경우 미성년자
	bool m_isAdult;
	
	// 15세 미만인가?
	bool m_bUnderFifthteen;
	
	// Family 요금제인가?
	bool m_bFamily;

	// 서버의 상태
	BYTE m_Stat;

	WORD m_LastDays;
#if __CONTENTS(__PAYZONE_PASS_TICKET)
	BYTE m_PayType;	//20080312 유료존 티켓 추가로 PayType 추가
#endif //__PAYZONE_PASS_TICKET
#if __CONTENTS(__LOGIN_PACKET)
	BYTE m_ReservedValue;	// 이전 클라이언트 접속을 제한하기 위해 필드 추가
#endif //__LOGIN_PACKET
	BYTE m_ServerID;
	BYTE m_ServerIDA;
	BYTE m_ServerIDB;
	BYTE m_ServerIDC;
};


//////////////////////////////////////////////////////////////////////
//
// class LCLoginOKFactory;
//
// Factory for LCLoginOK
//
//////////////////////////////////////////////////////////////////////

class LCLoginOKFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket()  { return new LCLoginOK(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName() const  { return "LCLoginOK"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_LC_LOGIN_OK; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const  { return szBYTE + szBYTE + szBYTE + szBYTE 
#if __CONTENTS(__PAYZONE_PASS_TICKET)
		+ szBYTE
#endif //__PAYZONE_PASS_TICKET

#if __CONTENTS(__LOGIN_PACKET)
		+ szBYTE 
#endif //__LOGIN_PACKET
		+ szBYTE+ szBYTE+ szBYTE+ szBYTE 
	;}
};


//////////////////////////////////////////////////////////////////////
//
// class LCLoginOKHandler;
//
//////////////////////////////////////////////////////////////////////

class LCLoginOKHandler {

public:

	// execute packet's handler
	static void execute(LCLoginOK* pPacket, Player* pPlayer) ;

};

#endif
