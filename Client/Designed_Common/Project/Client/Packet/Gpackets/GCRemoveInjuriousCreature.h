//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCRemoveInjuriousCreature.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_REMOVE_INJURIOUS_CREATURE_H__
#define __GC_REMOVE_INJURIOUS_CREATURE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCRemoveInjuriousCreature;
//
// 클라이언트가 서버에게 보내는 RemoveInjuriousCreature 패킷이다.
// 내부에 RemoveInjuriousCreature String 만을 데이타 필드로 가진다.
//
//////////////////////////////////////////////////////////////////////

class GCRemoveInjuriousCreature : public Packet {

public:
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_GC_REMOVE_INJURIOUS_CREATURE; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const  { return szBYTE + m_Name.size(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName() const  { return "GCRemoveInjuriousCreature"; }
		
		// get packet's debug std::string
		std::string toString() const ;
	#endif

	// get/set Name
	const std::string& getName() const  { return m_Name; }
	void setName(const std::string & Name)  { m_Name = Name; }

private :

	std::string m_Name;
	
};


//////////////////////////////////////////////////////////////////////
//
// class GCRemoveInjuriousCreatureFactory;
//
// Factory for GCRemoveInjuriousCreature
//
//////////////////////////////////////////////////////////////////////

class GCRemoveInjuriousCreatureFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket()  { return new GCRemoveInjuriousCreature(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName() const  { return "GCRemoveInjuriousCreature"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_REMOVE_INJURIOUS_CREATURE; }

	// get packet's max body size
	// message 의 최대 크기에 대한 설정이 필요하다.
	PacketSize_t getPacketMaxSize() const  {
		// Server GCRemoveInjuriousCreature max name = 100 (UTF-8).
		// Eski 10-byte factory limiti her irk icin Disconnect riski.
		return szBYTE + 100;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCRemoveInjuriousCreatureHandler;
//
//////////////////////////////////////////////////////////////////////

class GCRemoveInjuriousCreatureHandler {

public:

	// execute packet's handler
	static void execute(GCRemoveInjuriousCreature* pPacket, Player* pPlayer) ;

};

#endif
