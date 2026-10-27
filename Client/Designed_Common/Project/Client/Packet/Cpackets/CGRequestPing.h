#ifndef __CG_REQUEST_PING_H__
#define __CG_REQUEST_PING_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"


class CGRequestPing : public Packet
{
public:
    void read(SocketInputStream & iStream) ;
		    
    void write(SocketOutputStream & oStream) const ;

	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_CG_REQUEST_PING; }
	
	PacketSize_t getPacketSize() const  { return 0; }

	std::string getPacketName() const  { return "CGRequestPing"; }

	std::string toString() const  { return "CGRequestPing"; }
	

};



class CGRequestPingFactory : public PacketFactory {

public:
	
	// constructor
	CGRequestPingFactory()  {}
	
	// destructor
	virtual ~CGRequestPingFactory() {}

	
public:
	
	// create packet
	Packet* createPacket()  { return new CGRequestPing(); }

	// get packet name
	std::string getPacketName() const  { return "CGRequestPing"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_REQUEST_PING; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const  { return 0; }
};


class CGRequestPingHandler {

public:

	// execute packet's handler
	static void execute(CGRequestPing* pPacket, Player* pPlayer) ;

};

#endif
