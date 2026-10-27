//////////////////////////////////////////////////////////////////////
// 
// Filename    : PacketFactoryManager.h 
// Written By  : reiot@ewestsoft.com
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __PACKET_FACTORY_MANAGER_H__
#define __PACKET_FACTORY_MANAGER_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class PacketFactoryManager
//
//////////////////////////////////////////////////////////////////////

class PacketFactoryManager {

public :
	
	// constructor
	PacketFactoryManager () ;
	
	// destructor
	~PacketFactoryManager() ;

	// 패킷팩토리매니저를 초기화한다.
	// 게임서버객체의 init()에서 호출된다.
	void init () ;
	
	// 팩토리 객체를 특정 인덱스에 추가한다.
	void addFactory (PacketFactory* pFactory) ;
	
	// 패킷아이디로 패킷객체를 생성한다.
	Packet* createPacket (PacketID_t packetID) ;

#ifdef __DEBUG_OUTPUT__
	// 특정 패킷의 최대 크기를 리턴한다.
	std::string getPacketName (PacketID_t packetID) ;
		// get debug std::string
	std::string toString () const ;
#endif


	// 특정 패킷의 최대 크기를 리턴한다.
	PacketSize_t getPacketMaxSize (PacketID_t packetID) ;



private :
	
	// 패킷팩토리의 배열
	PacketFactory ** m_Factories;
	
	// 패킷팩토리배열의 크기
	ushort m_Size;

};

extern PacketFactoryManager* g_pPacketFactoryManager;

#endif
