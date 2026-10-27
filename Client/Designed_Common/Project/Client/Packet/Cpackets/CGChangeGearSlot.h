//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGChangeGearSlot.h 
// Written By  : shootkj
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
#if __CONTENTS(__GEAR_SWAP_CHANGE)	// CGChangeGearSlot.h

class CGChangeGearSlot : public Packet
 {
public:
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_CG_CHANGE_GEARSLOT; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const  { return szGearSlotID; }

	// get packet name
	std::string getPacketName() const  { return "CGChangeGearSlot"; }

	GearSlotID_t getGearSlotID() const  { return m_GearSlotID; }
	void setGearID(GearSlotID_t GearSlotID)  { m_GearSlotID = GearSlotID; }
	
	// get packet's debug std::string
	std::string toString() const  { return "CGChangeGearSlot"; }

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
	Packet* createPacket()  { return new CGChangeGearSlot(); }

	// get packet name
	std::string getPacketName() const  { return "CGChangeGearSlot"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_CHANGE_GEARSLOT; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const  { return szGearSlotID; }
};


//////////////////////////////////////////////////////////////////////
//
// class CGChangeGearSlotHandler;
//
//////////////////////////////////////////////////////////////////////

class CGChangeGearSlotHandler 
{

public:

	// execute packet's handler
//	static void execute(CGChangeGearSlot* pPacket, Player* player) ;

};
#endif //__GEAR_SWAP_CHANGE

#endif
