//--------------------------------------------------------------------------------
// 
// Filename    : GCAddBurrowingCreature.h 
// Written By  : crazydog 
// 
//--------------------------------------------------------------------------------

#ifndef __GC_ADD_BURROWING_CREATURE_H__
#define __GC_ADD_BURROWING_CREATURE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class GCAddBurrowingCreature;
//
// 시야 안에 BurrowingCreature 가 들어왔을 경우, 이 패킷에 BurrowingCreature 정보를 담아서 전송받는다.
//
//--------------------------------------------------------------------------------

class GCAddBurrowingCreature : public Packet {

public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read ( SocketInputStream & iStream ) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write ( SocketOutputStream & oStream ) const ;

	// execute packet's handler
	void execute ( Player * pPlayer ) ;

	// get packet id
	PacketID_t getPacketID () const  { return PACKET_GC_ADD_BURROWING_CREATURE; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	// const static GCAddBurrowingCreaturePacketSize 를 정의, 리턴하라.
	PacketSize_t getPacketSize () const  
	{ 
		return szObjectID 
			+ szBYTE + m_Name.size() 
			+ szCoord + szCoord; 
	}

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const  { return "GCAddBurrowingCreature"; }
		
		// get packet's debug std::string
		std::string toString () const ;
	#endif	


public :

	// get/set object id
	ObjectID_t getObjectID () const  { return m_ObjectID; }
	void setObjectID ( ObjectID_t creatureID )  { m_ObjectID = creatureID; }

	// get/set name
	const std::string& getName () const  { return m_Name; }
	void setName ( std::string name )  { m_Name = name; }


	// get/set X
	Coord_t getX () const  { return m_X; }
	void setX ( Coord_t x )  { m_X = x; }
	
	// get/set Y
	Coord_t getY () const  { return m_Y; }
	void setY ( Coord_t y )  { m_Y = y; }



private :
	
    // 존 레벨에서 유니크한 아이디로 객체 구분을 위해서 사용한다.
    ObjectID_t m_ObjectID;

    // BurrowingCreature의 이름
    std::string m_Name;

   	Coord_t m_X;				// X 좌표
	Coord_t m_Y;				// Y 좌표
};


//////////////////////////////////////////////////////////////////////
//
// class GCAddBurrowingCreatureFactory;
//
// Factory for GCAddBurrowingCreature
//
//////////////////////////////////////////////////////////////////////

class GCAddBurrowingCreatureFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket ()  { return new GCAddBurrowingCreature(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const  { return "GCAddBurrowingCreature"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const  { return Packet::PACKET_GC_ADD_BURROWING_CREATURE; }

	// get packet's body size
	// *OPTIMIZATION HINT*
	// const static GCAddBurrowingCreaturePacketSize 를 정의, 리턴하라.
	PacketSize_t getPacketMaxSize () const  
	{
		// Server GCAddBurrowingCreature max name = 100 (UTF-8).
		// Eski 20-byte CHS limiti "치프아이언티쓰" (21 byte) paketini
		// size=28 > max=27 ile InvalidProtocolException + Disconnect yapıyordu.
		return szObjectID
			+ szBYTE + 100
			+ szCoord + szCoord;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCAddBurrowingCreatureHandler;
//
//////////////////////////////////////////////////////////////////////

class GCAddBurrowingCreatureHandler {

public :

	// execute packet's handler
	static void execute ( GCAddBurrowingCreature * pPacket , Player * pPlayer ) ;

};

#endif
