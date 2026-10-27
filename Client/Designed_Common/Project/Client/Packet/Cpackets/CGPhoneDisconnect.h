//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGPhoneDisconnect.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_PHONE_DISCONNECT_H__
#define __CG_PHONE_DISCONNECT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGPhoneDisconnect;
//
//////////////////////////////////////////////////////////////////////

class CGPhoneDisconnect : public Packet {
public :

	// constructor
	CGPhoneDisconnect() ;

	// destructor
	~CGPhoneDisconnect() ;

public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_CG_PHONE_DISCONNECT; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	// const static CGPhoneDisconnectPacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketSize () const  { return szSlotID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CGPhoneDisconnect"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif
	
public :

	// get / set phoneNumber
	SlotID_t getSlotID() const  { return m_SlotID; }
	void setSlotID( SlotID_t SlotID )  { m_SlotID = SlotID; }

private :
	
	// SlotID
	SlotID_t m_SlotID;

};


//////////////////////////////////////////////////////////////////////
//
// class CGPhoneDisconnectFactory;
//
// Factory for CGPhoneDisconnect
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGPhoneDisconnectFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new CGPhoneDisconnect(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CGPhoneDisconnect"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_CG_PHONE_DISCONNECT; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static CGPhoneDisconnectPacketSize 를 정의해서 리턴하라.
	PacketSize_t getPacketMaxSize () const  { return szSlotID; }

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGPhoneDisconnectHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGPhoneDisconnectHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGPhoneDisconnect * pPacket , Player * player ) ;
	};
#endif

#endif
