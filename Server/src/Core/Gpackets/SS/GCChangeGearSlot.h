//--------------------------------------------------------------------------------
// 
// Filename    : GCChangeGearSlot 
// Written By  : rappi76 
// Description : 
// 
//--------------------------------------------------------------------------------

#ifndef __GC_CHANGE_GEARSLOT_H__
#define __GC_CHNAGE_GEARSLOT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"
#include "GameTime.h"
#include "GearInfo.h"
#include "Assert.h"

//--------------------------------------------------------------------------------
//
// class GCChangeGearSlot;
//
//--------------------------------------------------------------------------------

class GCChangeGearSlot : public Packet {

public :

	// constructor
	GCChangeGearSlot() throw();

	// destructor
	~GCChangeGearSlot() throw();
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_CHANGE_GEARSLOT; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() 
	{ 
		return szGearSlotID + m_pGearInfo->getSize();
	}

	// get packet name
	string getPacketName() const throw() { return "GCChangeGearSlot"; }
	
	// get packet's debug string
	string toString() const throw();


//--------------------------------------------------
// methods
//--------------------------------------------------
public :

	GearSlotID_t getGearSlotID() { return m_GearSlotID; }
	void setGearSlotID(GearSlotID_t GearSlotID) { m_GearSlotID = GearSlotID;}

	// get/set Gear Info
	GearInfo* getGearInfo() const throw() { return m_pGearInfo; }
	void setGearInfo(GearInfo* pGearInfo) throw(Error) { m_pGearInfo = pGearInfo; }

private :

	//--------------------------------------------------------------------------------
	// Gear Information
	//--------------------------------------------------------------------------------
	GearSlotID_t m_GearSlotID;
	GearInfo* m_pGearInfo;

};


//--------------------------------------------------------------------------------
//
// class GCChangeGearSlotFactory;
//
// Factory for GCChangeGearSlot
//
//--------------------------------------------------------------------------------

class GCChangeGearSlotFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCChangeGearSlot(); }

	// get packet name
	string getPacketName() const throw() { return "GCChangeGearSlot"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_CHANGE_GEARSLOT; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return szGearSlotID + GearInfo::getMaxSize();
	}

};



//--------------------------------------------------------------------------------
//
// class GCChangeGearSlotHandler;
//
//--------------------------------------------------------------------------------

class GCChangeGearSlotHandler {

public :

	// execute packet's handler
	static void execute(GCChangeGearSlot* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
