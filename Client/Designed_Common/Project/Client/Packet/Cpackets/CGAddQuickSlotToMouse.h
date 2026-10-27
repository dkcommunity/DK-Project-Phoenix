//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGAddQuickSlotToMouse.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_QUICKSLOT_TO_MOUSE_H__
#define __CG_ADD_QUICKSLOT_TO_MOUSE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGAddQuickSlotToMouse;
//
//////////////////////////////////////////////////////////////////////

class CGAddQuickSlotToMouse : public Packet {
public :

	// constructor
	CGAddQuickSlotToMouse() ;

	// destructor
	~CGAddQuickSlotToMouse() ;

public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_CG_ADD_QUICKSLOT_TO_MOUSE; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	// const static CGAddQuickSlotToMousePacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketSize () const  { return szObjectID + szSlotID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CGAddQuickSlotToMouse"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif
	
public :

	// get / set ObjectID
	ObjectID_t getObjectID()  { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID )  { m_ObjectID = ObjectID; }

	// get / set SlotID
	SlotID_t getSlotID()  { return m_SlotID; }
	void setSlotID( SlotID_t SlotID )  { m_SlotID = SlotID; }

private :
	
	// ObjectID
	ObjectID_t m_ObjectID;

	// SlotID
	SlotID_t m_SlotID;

};


//////////////////////////////////////////////////////////////////////
//
// class CGAddQuickSlotToMouseFactory;
//
// Factory for CGAddQuickSlotToMouse
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGAddQuickSlotToMouseFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new CGAddQuickSlotToMouse(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CGAddQuickSlotToMouse"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_CG_ADD_QUICKSLOT_TO_MOUSE; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static CGAddQuickSlotToMousePacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketMaxSize () const  { return szObjectID + szSlotID; }

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGAddQuickSlotToMouseHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGAddQuickSlotToMouseHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGAddQuickSlotToMouse * pPacket , Player * player ) ;
	};

#endif
#endif
