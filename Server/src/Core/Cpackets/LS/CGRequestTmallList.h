#ifndef __CG_REQUEST_TMALL_LIST_H__
#define __CG_REQUEST_TMALL_LIST_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"


class CGRequestTmallList : public Packet
{
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_REQUEST_TMALL_LIST; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szBYTE; }

	// get packet name
	string getPacketName() const throw() { return "CGRequestTmallList"; }

	// get packet's debug string
	string toString() const throw();

	// get/set Kind
	BYTE getKind() const throw() { return m_Kind; }

	void setKind( BYTE Kind = 0 ) throw() { m_Kind = Kind; }
	
	BYTE m_Kind;
	

};



class CGRequestTmallListFactory : public PacketFactory {

public:
	
	// constructor
	CGRequestTmallListFactory() throw() {}
	
	// destructor
	virtual ~CGRequestTmallListFactory() throw() {}

	
public:
	
	// create packet
	Packet* createPacket() throw() { return new CGRequestTmallList(); }

	// get packet name
	string getPacketName() const throw() { return "CGRequestTmallList"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_REQUEST_TMALL_LIST; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE; }
};


class CGRequestTmallListHandler {

public:

	// execute packet's handler
	static void execute(CGRequestTmallList* pCGRequestTmallList, Player* pPlayer) throw(Error);

};

#endif
