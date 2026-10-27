#ifndef __CG_REQUEST_RANK_GEM_DATA_H__
#define __CG_REQUEST_RANK_GEM_DATA_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"


class CGRequestRankGemData : public Packet
{
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_REQUEST_RANK_GEM_DATA; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return 0; }

	// get packet name
	string getPacketName() const throw() { return "CGRequestRankGemData"; }

	// get packet's debug string
	string toString() const throw() { return "CGRequestRankGemData"; }

	// get/set Kind
//	BYTE getKind() const throw() { return m_Kind; }

//	void setKind( BYTE Kind = 0 ) throw() { m_Kind = Kind; }
	
//	BYTE m_Kind;
	

};



class CGRequestRankGemDataFactory : public PacketFactory {

public:
	
	// constructor
	CGRequestRankGemDataFactory() throw() {}
	
	// destructor
	virtual ~CGRequestRankGemDataFactory() throw() {}

	
public:
	
	// create packet
	Packet* createPacket() throw() { return new CGRequestRankGemData(); }

	// get packet name
	string getPacketName() const throw() { return "CGRequestRankGemData"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_REQUEST_RANK_GEM_DATA; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return 0; }
};


class CGRequestRankGemDataHandler {

public:

	// execute packet's handler
	static void execute(CGRequestRankGemData* pPacket, Player* pPlayer) throw(Error);

};

#endif
