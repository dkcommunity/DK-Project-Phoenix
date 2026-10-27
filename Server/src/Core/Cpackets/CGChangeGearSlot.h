//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGChangeGearSlot.h 
// Written By  : rappi76
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_CHANGE_GEARSLOT_H__
#define __CG_CHANGE_GEARSLOT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGChangeGearSlot;
//
//////////////////////////////////////////////////////////////////////

class CGChangeGearSlot : public Packet {

public:
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_CHANGE_GEARSLOT; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szGearSlotID; }

	// get packet name
	string getPacketName() const throw() { return "CGChangeGearSlot"; }

	GearSlotID_t getGearSlotID() const throw() { return m_GearSlotID; }
	void setGearID(GearSlotID_t GearSlotID) throw() { m_GearSlotID = GearSlotID; }
	
	// get packet's debug string
	string toString() const throw() { return "CGChangeGearSlot"; }

private :

	GearSlotID_t m_GearSlotID;

};


//////////////////////////////////////////////////////////////////////
//
// class CGChangeGearSlotFactory;
//
// Factory for CGChangeGearSlot
//
//////////////////////////////////////////////////////////////////////

class CGChangeGearSlotFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket() throw() { return new CGChangeGearSlot(); }

	// get packet name
	string getPacketName() const throw() { return "CGChangeGearSlot"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_CHANGE_GEARSLOT; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const throw() { return szGearSlotID; }

};


//////////////////////////////////////////////////////////////////////
//
// class CGChangeGearSlotHandler;
//
//////////////////////////////////////////////////////////////////////

class CGChangeGearSlotHandler {

public:

	// execute packet's handler
	static void execute(CGChangeGearSlot* pPacket, Player* player) throw(ProtocolException, Error);

};

#endif
