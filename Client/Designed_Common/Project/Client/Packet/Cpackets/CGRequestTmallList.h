#ifndef __CG_REQUER_TMALL_LIST_H__
#define __CG_REQUER_TMALL_LIST_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"


class CGRequestTmallList : public Packet
{
public:
    void read(SocketInputStream & iStream) ;
		    
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_CG_REQUEST_TMALL_LIST; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const  { return szBYTE; }

	// get packet name
	std::string getPacketName() const  { return "CGRequestTmallList"; }

	// get packet's debug std::string
	std::string toString() const ;

	// get/set Kind
	BYTE getKind() const  { return m_Kind; }

	void setKind( BYTE Kind = 0 )  { m_Kind = Kind; }
	
	BYTE m_Kind;
	

};



class CGRequestTmallListFactory : public PacketFactory {

public:
	
	// constructor
	CGRequestTmallListFactory()  {}
	
	// destructor
	virtual ~CGRequestTmallListFactory() {}

	
public:
	
	// create packet
	Packet* createPacket()  { return new CGRequestTmallList(); }

	// get packet name
	std::string getPacketName() const  { return "CGRequestTmallList"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_REQUEST_TMALL_LIST; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const  { return szBYTE; }
};


class CGRequestTmallListHandler {

public:

	// execute packet's handler
	static void execute(CGRequestTmallList* pCGRequestTmallList, Player* pPlayer) ;

};

#endif
