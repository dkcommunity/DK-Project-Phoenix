//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGAddMouseToGear.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_MOUSE_TO_GEAR_H__
#define __CG_ADD_MOUSE_TO_GEAR_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGAddMouseToGear;
//
//////////////////////////////////////////////////////////////////////

class CGAddMouseToGear : public Packet {
public :

	// constructor
	CGAddMouseToGear() ;

	// destructor
	~CGAddMouseToGear() ;

public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_CG_ADD_MOUSE_TO_GEAR; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	// const static CGAddMouseToGearPacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketSize () const  
	{
		PacketSize_t	szPacketSize	= 0;
#if __CONTENTS(__GEAR_SWAP_CHANGE)	// CGAddMouseToGear 장착창 ID 패킷 추가
		szPacketSize	+= szGearSlotID;
#endif //__GEAR_SWAP_CHANGE
		szPacketSize	+= szObjectID;
		szPacketSize	+= szSlotID;
		return  szPacketSize;
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CGAddMouseToGear"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif
	
public :
#if __CONTENTS(__GEAR_SWAP_CHANGE)	// CGAddMouseToGear - getGearSlotID, setGearSlotID 함수 추가
	GearSlotID_t getGearSlotID()  { return m_GearSlotID; }
	void setGearSlotID(GearSlotID_t GearSlotID)  { m_GearSlotID = GearSlotID; }
#endif //__GEAR_SWAP_CHANGE
	// get / set ObjectID

	ObjectID_t getObjectID()  { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID )  { m_ObjectID = ObjectID; }

	SlotID_t getSlotID() const  { return m_SlotID; }
	void setSlotID( SlotID_t SlotID )  { m_SlotID = SlotID; }

private :
#if __CONTENTS(__GEAR_SWAP_CHANGE)	// CGAddMouseToGear m_GearSlotID 추가
	GearSlotID_t	m_GearSlotID;
#endif //__GEAR_SWAP_CHANGE
	// ObjectID
	ObjectID_t		m_ObjectID;

	// SlotID
	SlotID_t		m_SlotID;

};


//////////////////////////////////////////////////////////////////////
//
// class CGAddMouseToGearFactory;
//
// Factory for CGAddMouseToGear
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGAddMouseToGearFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new CGAddMouseToGear(); }

	
	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CGAddMouseToGear"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_CG_ADD_MOUSE_TO_GEAR; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static CGAddMouseToGearPacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketMaxSize () const  
	{ 
		PacketSize_t	szPacketSize	= 0;
#if __CONTENTS(__GEAR_SWAP_CHANGE)		//CGAddMouseToGearFactory 패킷 추가
		szPacketSize	+= szGearSlotID;
#endif	// __GEAR_SWAP_CHANGE
		szPacketSize	+= szObjectID;
		szPacketSize	+= szSlotID;
		return szPacketSize; 
	}

};

#endif

//////////////////////////////////////////////////////////////////////
//
// class CGAddMouseToGearHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGAddMouseToGearHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGAddMouseToGear * pPacket , Player * player ) ;
	};

#endif
#endif
