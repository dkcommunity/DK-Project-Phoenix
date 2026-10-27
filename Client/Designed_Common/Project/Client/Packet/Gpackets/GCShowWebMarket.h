//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCShowWebMarket.h 
// Written By  : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_SHOW_WEB_MARKET_H__
#define __GC_SHOW_WEB_MARKET_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCShowWebMarket;
//
// 클라이언트에 길드 등록 창을 띄우도록 한다.
//
//////////////////////////////////////////////////////////////////////

class GCShowWebMarket : public Packet {

public :
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_GC_SHOW_WEB_MARKET; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const 
	{ 
		return szBYTE +				// Player ID length
			   m_PlayerID.size() +	// Player ID
			   szBYTE +				// Name length
			   m_Name.size() +		// Name
			   szWorldID +			// World ID
			   szDWORD;				// Key length
	}

	// get packet name
	std::string getPacketName() const  { return "GCShowWebMarket"; }
	
	// get packet's debug std::string
	std::string toString() const ;

	// get/set Player ID
	const std::string& getPlayerID() const { return m_PlayerID; }
	void setPlayerID( const std::string playerID ) { m_PlayerID = playerID; }

	// get/set Name
	const std::string& getName() const { return m_Name; }
	void setName( const std::string name ) { m_Name = name; }

	// get/set World ID
	WorldID_t getWorldID() const { return m_WorldID; }
	void setWorldID( WorldID_t worldID ) { m_WorldID = worldID; }

	// get/set Key
	DWORD getKey() const { return m_Key; }
	void setKey( DWORD key ) { m_Key = key; }

private :
	// Player ID
	std::string m_PlayerID;

	// Name
	std::string m_Name;

	// World ID
	WorldID_t m_WorldID;

	// Key
	DWORD m_Key;
};


//////////////////////////////////////////////////////////////////////
//
// class GCShowWebMarketFactory;
//
// Factory for GCShowWebMarket
//
//////////////////////////////////////////////////////////////////////

class GCShowWebMarketFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket()  { return new GCShowWebMarket(); }

	// get packet name
	std::string getPacketName() const  { return "GCShowWebMarket"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_SHOW_WEB_MARKET; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static GCSystemMessagePacketMaxSize 를 정의, 리턴하라.
	PacketSize_t getPacketMaxSize() const 
	{
		return szBYTE +		// Player ID length
			   20 +			// Player ID
			   szBYTE +		// Name length
			   20 +			// Name
			   szWorldID +	// World ID
			   szDWORD;		// Key length
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCShowWebMarket;
//
//////////////////////////////////////////////////////////////////////

class GCShowWebMarketHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCShowWebMarket* pPacket, Player* pPlayer) ;

};

#endif

