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
    void read(SocketInputStream & iStream) ;
		    
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_CG_REQUEST_PAY_EVENT_LIST; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const  { return 0; }

	// get packet name
	std::string getPacketName() const  { return "CGRequestPayEventList"; }

	// get packet's debug std::string
	std::string toString() const  { return "CGRequestPayEventList"; }

};



class CGRequestPayEventListFactory : public PacketFactory {

public:
	
	// constructor
	CGRequestPayEventListFactory()  {}
	
	// destructor
	virtual ~CGRequestPayEventListFactory() {}

	
public:
	
	// create packet
	Packet* createPacket()  { return new CGRequestPayEventList(); }

	// get packet name
	std::string getPacketName() const  { return "CGRequestPayEventList"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_REQUEST_PAY_EVENT_LIST; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const  { return 0; }
};


class CGRequestPayEventListHandler {

public:

	// execute packet's handler
	static void execute(CGRequestPayEventList* pPacket, Player* pPlayer) ;

};

#endif
