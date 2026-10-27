//////////////////////////////////////////////////////////////////////
// 
// Filename    : CommonSMSPacket.h 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __COMMON_SMS_PACKET_H__
#define __COMMON_SMS_PACKET_H__

// include files
#include <stdio.h>
#include "Packet.h"
#include "PacketFactory.h"
#include "SMSInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class CommonSMSPacket;
//
// GameServer와 SMSServer가 주고 받는 packet이다.
//
//////////////////////////////////////////////////////////////////////

class Player;
class Creature;
class GamePlayer;

class CommonSMSPacket : public Packet, public SMSInfo {

public:
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_COMMON_SMS; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szSMSInfo; }

	// get packet name
	string getPacketName() const throw() { return "CommonSMSPacket"; }
	
	// get packet's debug string
	string toString() const throw();

private :
	// 512은 SMSInfo가 382이기 때문에 일단 이렇게 잡아둔거다
	char		m_pBuffer[512];
};


//////////////////////////////////////////////////////////////////////
//
// class CommonSMSPacketFactory;
//
// Factory for CommonSMSPacket
//
//////////////////////////////////////////////////////////////////////

class CommonSMSPacketFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new CommonSMSPacket(); }

	// get packet name
	string getPacketName() const throw() { return "CommonSMSPacket"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_COMMON_SMS; }

	// get packet's max body size
	// message 의 최대 크기에 대한 설정이 필요하다.
	PacketSize_t getPacketMaxSize() const throw() { return szSMSInfo; }

};


//////////////////////////////////////////////////////////////////////
//
// class CommonSMSPacketHandler;
//
//////////////////////////////////////////////////////////////////////

class CommonSMSPacketHandler {

public:

	// execute packet's handler
	static void execute(CommonSMSPacket* pPacket, Player* pPlayer) throw(ProtocolException, Error);

private :
	static void sendSMSMsg(CommonSMSPacket* pPacket, GamePlayer* pGamePlayer) throw (Error);

};

#endif
