//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGRequestWebMarket.h 
// Written By  :
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_REQUEST_WEB_MARKET_H__
#define __CG_REQUEST_WEB_MARKET_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGRequestWebMarket;
//
//////////////////////////////////////////////////////////////////////

class CGRequestWebMarket : public Packet
{
public:
	enum REQUEST_CODE
	{
		REQUEST_WEB_MARKET = 0,
		REQUEST_BUY_COMPLETE,
		REQUEST_GOODS_LIST,
	};

public:
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_CG_REQUEST_WEB_MARKET; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const  { return szBYTE; }

	// get packet name
	std::string getPacketName() const  { return "CGRequestWebMarket"; }

	// get packet's debug std::string
	std::string toString() const ;

	// get/set code
	BYTE getCode() const { return m_Code; }
	void setCode( BYTE code ) { m_Code = code; }

public:
	// Request Code
	BYTE m_Code;
};


//////////////////////////////////////////////////////////////////////
//
// class CGRequestWebMarketFactory;
//
// Factory for CGRequestWebMarket
//
//////////////////////////////////////////////////////////////////////

class CGRequestWebMarketFactory : public PacketFactory {

public:
	
	// constructor
	CGRequestWebMarketFactory()  {}
	
	// destructor
	virtual ~CGRequestWebMarketFactory() {}

	
public:
	
	// create packet
	Packet* createPacket()  { return new CGRequestWebMarket(); }

	// get packet name
	std::string getPacketName() const  { return "CGRequestWebMarket"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_REQUEST_WEB_MARKET; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const  { return szBYTE; }
};


//////////////////////////////////////////////////////////////////////
//
// class CGRequestWebMarketHandler;
//
//////////////////////////////////////////////////////////////////////

class CGRequestWebMarketHandler {

public:

	// execute packet's handler
	static void execute(CGRequestWebMarket* pCGRequestWebMarket, Player* pPlayer) ;

};

#endif

