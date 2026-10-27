//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGAppointSubmaster.h 
// Written By  :
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_APPOINT_SUBMASTER_H__
#define __CG_APPOINT_SUBMASTER_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGAppointSubmaster;
//
//////////////////////////////////////////////////////////////////////

class CGAppointSubmaster : public Packet
{
public:
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_CG_APPOINT_SUBMASTER; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const  { return szGuildID + szBYTE + m_Name.size(); }
#ifdef __DEBUG_OUTPUT__
	// get packet name
	std::string getPacketName() const  { return "CGAppointSubmaster"; }

	// get packet's debug std::string
	std::string toString() const ;
#endif
	// get/set GuildID
	GuildID_t getGuildID() const  { return m_GuildID; }
	void setGuildID( GuildID_t guildID )  { m_GuildID = guildID; }

	// get/set name
	const std::string& getName() const  { return m_Name; }
	void setName( const std::string& name )  { m_Name = name; }


private :

	// GuildID
	GuildID_t m_GuildID;

	// name
	std::string m_Name;

};


//////////////////////////////////////////////////////////////////////
//
// class CGAppointSubmasterFactory;
//
// Factory for CGAppointSubmaster
//
//////////////////////////////////////////////////////////////////////

class CGAppointSubmasterFactory : public PacketFactory {

public:
	
	// constructor
	CGAppointSubmasterFactory()  {}
	
	// destructor
	virtual ~CGAppointSubmasterFactory() {}

	
public:
	
	// create packet
	Packet* createPacket()  { return new CGAppointSubmaster(); }

	// get packet name
	std::string getPacketName() const  { return "CGAppointSubmaster"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_APPOINT_SUBMASTER; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const  { return szGuildID + szBYTE + 20; }
};


//////////////////////////////////////////////////////////////////////
//
// class CGAppointSubmasterHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGAppointSubmasterHandler {

public:

	// execute packet's handler
	static void execute(CGAppointSubmaster* pCGAppointSubmaster, Player* pPlayer) ;

};
#endif
#endif
