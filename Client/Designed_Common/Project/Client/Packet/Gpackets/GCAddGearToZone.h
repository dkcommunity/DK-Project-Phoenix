//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCAddGearToZone.h 
// Written By  : crazydog
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_GEAR_TO_ZONE_H__
#define __GC_ADD_GEAR_TO_ZONE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCAddGearToZone;
//
//////////////////////////////////////////////////////////////////////

class GCAddGearToZone : public Packet {
public :

	// constructor
	GCAddGearToZone() ;

	// destructor
	~GCAddGearToZone() ;

public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_GC_ADD_GEAR_TO_ZONE; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	// const static GCAddGearToZonePacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketSize () const  { return szSlotID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "GCAddGearToZone"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif
	
public :

	// get / set SlotID
	SlotID_t getSlotID()  { return m_SlotID; }
	void setSlotID( SlotID_t SlotID )  { m_SlotID = SlotID; }

private :
	
	// SlotID
	SlotID_t m_SlotID;

};


//////////////////////////////////////////////////////////////////////
//
// class GCAddGearToZoneFactory;
//
// Factory for GCAddGearToZone
//
//////////////////////////////////////////////////////////////////////

class GCAddGearToZoneFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new GCAddGearToZone(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "GCAddGearToZone"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_GC_ADD_GEAR_TO_ZONE; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static GCAddGearToZonePacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketMaxSize () const  { return szSlotID; }

};

//////////////////////////////////////////////////////////////////////
//
// class GCAddGearToZoneHandler;
//
//////////////////////////////////////////////////////////////////////

class GCAddGearToZoneHandler {
	
public :

	// execute packet's handler
	static void execute ( GCAddGearToZone * pPacket , Player * player ) ;
};

#endif
