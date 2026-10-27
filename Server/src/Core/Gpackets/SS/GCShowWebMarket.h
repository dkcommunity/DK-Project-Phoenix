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
    virtual ~GCShowWebMarket() throw() {}
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_SHOW_WEB_MARKET; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw()
	{ 
		return szBYTE +				// Player ID length
			   m_PlayerID.size() +	// Player ID
			   szBYTE +				// Name length
			   m_Name.size() +		// Name
			   szWorldID +			// World ID
			   szDWORD;				// Key
	}

	// get packet name
	string getPacketName() const throw() { return "GCShowWebMarket"; }
	
	// get packet's debug string
	string toString() const throw();

	// get/set Player ID
	string getPlayerID() const { return m_PlayerID; }
	void setPlayerID( const string playerID ) { m_PlayerID = playerID; }

	// get/set Name
	string getName() const { return m_Name; }
	void setName( const string name ) { m_Name = name; }

	// get/set World ID
	WorldID_t getWorldID() const { return m_WorldID; }
	void setWorldID( WorldID_t worldID ) { m_WorldID = worldID; }

	// get/set Key
	DWORD getKey() const { return m_Key; }
	void setKey( DWORD key ) { m_Key = key; }

private :
	// Player ID
	string m_PlayerID;

	// Name
	string m_Name;

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
	Packet* createPacket() throw() { return new GCShowWebMarket(); }

	// get packet name
	string getPacketName() const throw() { return "GCShowWebMarket"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_SHOW_WEB_MARKET; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static GCSystemMessagePacketMaxSize 를 정의, 리턴하라.
	PacketSize_t getPacketMaxSize() const throw()
	{
		return szBYTE +		// Player ID length
			   20 +			// Player ID
			   szBYTE +		// Name length
			   20 +			// Name
			   szWorldID +	// World ID
			   szDWORD;		// Key
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
	static void execute(GCShowWebMarket* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif

