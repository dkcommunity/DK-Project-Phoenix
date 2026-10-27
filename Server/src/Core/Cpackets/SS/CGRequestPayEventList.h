#ifndef __CG_REQUEST_PAY_EVENT_LIST_H__
#define __CG_REQUEST_PAY_EVENT_LIST_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"


class CGRequestPayEventList : public Packet
{
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_REQUEST_PAY_EVENT_LIST; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return 0; }

	// get packet name
	string getPacketName() const throw() { return "CGRequestPayEventList"; }

	// get packet's debug string
	string toString() const throw() { return "CGRequestPayEventList"; }

};



class CGRequestPayEventListFactory : public PacketFactory {

public:
	
	// constructor
	CGRequestPayEventListFactory() throw() {}
	
	// destructor
	virtual ~CGRequestPayEventListFactory() throw() {}

	
public:
	
	// create packet
	Packet* createPacket() throw() { return new CGRequestPayEventList(); }

	// get packet name
	string getPacketName() const throw() { return "CGRequestPayEventList"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_REQUEST_PAY_EVENT_LIST; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return 0; }
};


class CGRequestPayEventListHandler {

public:

	// execute packet's handler
	static void execute(CGRequestPayEventList* pPacket, Player* pPlayer) throw(Error);

};

#endif
