#ifndef __CG_REQUEST_PING_H__
#define __CG_REQUEST_PING_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"


class CGRequestPing : public Packet
{
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_CG_REQUEST_PING; }
	
	PacketSize_t getPacketSize() const throw() { return 0; }

	string getPacketName() const throw() { return "CGRequestPing"; }

	string toString() const throw() { return "CGRequestPing"; }
	

};



class CGRequestPingFactory : public PacketFactory {

public:
	
	// constructor
	CGRequestPingFactory() throw() {}
	
	// destructor
	virtual ~CGRequestPingFactory() throw() {}

	
public:
	
	// create packet
	Packet* createPacket() throw() { return new CGRequestPing(); }

	// get packet name
	string getPacketName() const throw() { return "CGRequestPing"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_REQUEST_PING; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return 0; }
};


class CGRequestPingHandler {

public:

	// execute packet's handler
	static void execute(CGRequestPing* pPacket, Player* pPlayer) throw(Error);

};

#endif
