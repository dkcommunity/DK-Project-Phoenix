//-----------------------------------------------------------------------------
// 
// Filename    : CGRequestRepairInfo.h 
// Written By  : 김성민
// Description :
// 
//-----------------------------------------------------------------------------

#ifndef __CG_REQUEST_REPAIR_INFO_H__
#define __CG_REQUEST_REPAIR_INFO_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CGRequestRepairInfo;
//
//--------------------------------------------------------------------------------

class CGRequestRepairInfo : public Packet {

public:
	
	// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
	// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_REQUEST_REPAIR_INFO; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szObjectID; }

	// get packet name
	string getPacketName() const throw() { return "CGRequestRepairInfo"; }
	
	// get packet's debug string
	string toString() const throw();
	
public:

	// get/set ObjectID
	ObjectID_t getObjectID() throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t ObjectID) throw() { m_ObjectID = ObjectID; }

private :
	
	// Item Object ID
	ObjectID_t m_ObjectID;

};


//-----------------------------------------------------------------------------
//
// class CGRequestRepairInfoFactory;
//
// Factory for CGRequestRepairInfo
//
//-----------------------------------------------------------------------------

class CGRequestRepairInfoFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new CGRequestRepairInfo(); }

	// get packet name
	string getPacketName() const throw() { return "CGRequestRepairInfo"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_REQUEST_REPAIR_INFO; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID; }

};


//--------------------------------------------------------------------------------
//
// class CGRequestRepairInfoHandler;
//
//--------------------------------------------------------------------------------

class CGRequestRepairInfoHandler {
	
public:

	// execute packet's handler
	static void execute(CGRequestRepairInfo* pPacket, Player* player) throw(ProtocolException, Error);
	static void executeNormal(CGRequestRepairInfo* pPacket, Player* player, double dRepairDiscountRate = 0.0) throw(ProtocolException, Error);
	static void executeMotorcycle(CGRequestRepairInfo* pPacket, Player* player, double dRepairDiscountRate = 0.0) throw(ProtocolException, Error);
	static void executeAll(CGRequestRepairInfo* pPacket, Player* player, double dRepairDiscountRate = 0.0) throw(ProtocolException, Error);
};

#endif
