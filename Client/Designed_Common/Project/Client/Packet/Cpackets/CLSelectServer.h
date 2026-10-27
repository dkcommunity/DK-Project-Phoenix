//////////////////////////////////////////////////////////////////////
// 
// Filename    : CLSelectServer.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CL_SELECT_SERVER_H__
#define __CL_SELECT_SERVER_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CLSelectServer;
//
//////////////////////////////////////////////////////////////////////

class CLSelectServer : public Packet {

public:
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_CL_SELECT_SERVER; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const  { return szServerGroupID; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName() const  { return "CLSelectServer"; }

		// get packet's debug std::string
		std::string toString() const  { return "CLSelectServer"; }
	#endif

	// get / set ServerGroupID
	ServerGroupID_t getServerGroupID() const  { return m_ServerGroupID; }
	void setServerGroupID(ServerGroupID_t ServerGroupID)  { m_ServerGroupID = ServerGroupID; }
	

private :

	ServerGroupID_t m_ServerGroupID;

};


//////////////////////////////////////////////////////////////////////
//
// class CLSelectServerFactory;
//
// Factory for CLSelectServer
//
//////////////////////////////////////////////////////////////////////

class CLSelectServerFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket()  { return new CLSelectServer(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName() const  { return "CLSelectServer"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_CL_SELECT_SERVER; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const  { return szServerGroupID; }

};


//////////////////////////////////////////////////////////////////////
//
// class CLSelectServerHandler;
//
//////////////////////////////////////////////////////////////////////

#ifndef __GAME_CLIENT__
	class CLSelectServerHandler {

	public:

		// execute packet's handler
		static void execute(CLSelectServer* pPacket, Player* player) ;

	};
#endif

#endif
