//-----------------------------------------------------------------------------
// 
// Filename    : CGRequestRepair.h 
// Written By  : 김성민
// Description :
// 
//-----------------------------------------------------------------------------

#ifndef __CG_REQUEST_REPAIR_H__
#define __CG_REQUEST_REPAIR_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CGRequestRepair;
//
//--------------------------------------------------------------------------------

class CGRequestRepair : public Packet {

public :
	
	// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
	void read ( SocketInputStream & iStream ) ;
		    
	// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
	void write ( SocketOutputStream & oStream ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_CG_REQUEST_REPAIR; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const  { return szObjectID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CGRequestRepair"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif
	
public :

	// get/set ObjectID
	ObjectID_t getObjectID()  { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID )  { m_ObjectID = ObjectID; }

private :
	
	// Item Object ID
	ObjectID_t m_ObjectID;

};


//-----------------------------------------------------------------------------
//
// class CGRequestRepairFactory;
//
// Factory for CGRequestRepair
//
//-----------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
class CGRequestRepairFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new CGRequestRepair(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "CGRequestRepair"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_CG_REQUEST_REPAIR; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const  { return szObjectID; }

};

#endif


//--------------------------------------------------------------------------------
//
// class CGRequestRepairHandler;
//
//--------------------------------------------------------------------------------
#ifndef __GAME_CLIENT__
	class CGRequestRepairHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGRequestRepair * pPacket , Player * player ) ;
		static void executeNormal ( CGRequestRepair * pPacket , Player * player ) ;
		static void executeMotorcycle ( CGRequestRepair * pPacket , Player * player ) ;
	};
#endif

#endif
