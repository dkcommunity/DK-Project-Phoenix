//////////////////////////////////////////////////////////////////////////////
// Filename    : LCSelectWorldError.h 
// Written By  : rappi76
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __LC_SELECT_WORLD_ERROR_H__
#define __LC_SELECT_WORLD_ERROR_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class LCSelectWorldError;
//
// PC Creation 이 실패했을 경우, 로그인 서버는 클라이언트에게 이 패킷을
// 보낸다.
//////////////////////////////////////////////////////////////////////////////
#if __CONTENTS(__LC_SELECT_WORLD_ERROR)

class LCSelectWorldError : public Packet 
{
public:
	enum 
	{
		ERROR_NONE				  = 0,
		ERROR_NOT_PREMIUM_ACCOUNT = 1
	} eErrorID;
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_LC_SELECT_WORLD_ERROR; }
	PacketSize_t getPacketSize() const  { return szBYTE; }
	std::string getPacketName() const  { return "LCSelectWorldError"; }
	std::string toString() const ;

public:
	BYTE getErrorID() const  { return m_ErrorID; }
	void setErrorID(BYTE ErrorID)  { m_ErrorID = ErrorID; }

private: 
	BYTE m_ErrorID;
};

//////////////////////////////////////////////////////////////////////////////
// class LCSelectWorldErrorFactory;
//////////////////////////////////////////////////////////////////////////////

class LCSelectWorldErrorFactory : public PacketFactory 
{
public:
	
	// create packet
	Packet* createPacket()  { return new LCSelectWorldError(); }

	// get packet name
	std::string getPacketName() const  { return "LCSelectWorldError"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_LC_SELECT_WORLD_ERROR; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const  { return szBYTE; }
	
};


//////////////////////////////////////////////////////////////////////
//
// class LCSelectWorldErrorHandler;
//
//////////////////////////////////////////////////////////////////////

class LCSelectWorldErrorHandler {

public:

	// execute packet's handler
	static void execute(LCSelectWorldError* pPacket, Player* pPlayer) ;

};
#endif //__LC_SELECT_WORLD_ERROR

#endif
