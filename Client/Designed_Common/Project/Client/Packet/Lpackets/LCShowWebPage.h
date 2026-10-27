//////////////////////////////////////////////////////////////////////
// 
// Filename    : LCShowWebPage.h 
// Written By  : Reiot
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __LC_SHOW_WEB_PAGE_H__
#define __LC_SHOW_WEB_PAGE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCShowWebPage;
//
// 로그인서버가 클라이언트에게 로그인 성공을 알려주는 패킷이다.
//
//////////////////////////////////////////////////////////////////////

class LCShowWebPage : public Packet {

public:

    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) ;
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_LC_SHOW_WEB_PAGE; }
	
	// get packet body size
	// *OPTIMIZATION HINT*
	// const static LCShowWebPagePacketSize 를 정의, 리턴하라.
	PacketSize_t getPacketSize() const  { return szBYTE + m_Message.size() + szBYTE + m_URL.size(); }
	
	// get packet's name
	std::string getPacketName() const  { return "LCShowWebPage"; }

	// get / set message
	const std::string& getMessage() const { return m_Message; }
	void setMessage( const std::string& message ) { m_Message = message; }
	
	// get / set url
	const std::string& getURL() const { return m_URL; }
	void setURL( const std::string& url ) { m_URL = url; }
	
	// get packet's debug std::string
	std::string toString() const ;

private:
	std::string m_Message;
	std::string m_URL;
};


//////////////////////////////////////////////////////////////////////
//
// class LCShowWebPageFactory;
//
// Factory for LCShowWebPage
//
//////////////////////////////////////////////////////////////////////

class LCShowWebPageFactory : public PacketFactory {

public:
	
	// create packet
	Packet* createPacket()  { return new LCShowWebPage(); }

	// get packet name
	std::string getPacketName() const  { return "LCShowWebPage"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_LC_SHOW_WEB_PAGE; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const  { return szBYTE + 128 + szBYTE + 128; }
	
};


//////////////////////////////////////////////////////////////////////
//
// class LCShowWebPageHandler;
//
//////////////////////////////////////////////////////////////////////

class LCShowWebPageHandler {

public:

	// execute packet's handler
	static void execute(LCShowWebPage* pPacket, Player* pPlayer) ;

};

#endif
