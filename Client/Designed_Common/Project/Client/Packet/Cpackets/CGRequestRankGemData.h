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
    void read(SocketInputStream & iStream) ;
		    
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_CG_REQUEST_RANK_GEM_DATA; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const  { return 0; }

	// get packet name
	std::string getPacketName() const  { return "CGRequestRankGemData"; }

	// get packet's debug std::string
	std::string toString() const  { return "CGRequestRankGemData"; }

	// get/set Kind
//	BYTE getKind() const  { return m_Kind; }

//	void setKind( BYTE Kind = 0 )  { m_Kind = Kind; }
	
//	BYTE m_Kind;
	

};



class CGRequestRankGemDataFactory : public PacketFactory {

public:
	
	// constructor
	CGRequestRankGemDataFactory()  {}
	
	// destructor
	virtual ~CGRequestRankGemDataFactory() {}

	
public:
	
	// create packet
	Packet* createPacket()  { return new CGRequestRankGemData(); }

	// get packet name
	std::string getPacketName() const  { return "CGRequestRankGemData"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_REQUEST_RANK_GEM_DATA; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const  { return 0; }
};


class CGRequestRankGemDataHandler {

public:

	// execute packet's handler
	static void execute(CGRequestRankGemData* pCGRequestRankGemData, Player* pPlayer) ;

};

#endif
