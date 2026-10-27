//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuctionRequestList.h
// Description : Auction House listing request packet.
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_AUCTION_REQUEST_LIST_H__
#define __CG_AUCTION_REQUEST_LIST_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGAuctionRequestList : public Packet
{
public:
	void read(SocketInputStream& iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_CG_AUCTION_REQUEST_LIST; }
	PacketSize_t getPacketSize() const throw() { return 0; }
	string getPacketName() const throw() { return "CGAuctionRequestList"; }
	string toString() const throw();
};

class CGAuctionRequestListFactory : public PacketFactory
{
public:
	Packet* createPacket() throw() { return new CGAuctionRequestList(); }
	string getPacketName() const throw() { return "CGAuctionRequestList"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_AUCTION_REQUEST_LIST; }
	PacketSize_t getPacketMaxSize() const throw() { return 0; }
};

class CGAuctionRequestListHandler
{
public:
	static void execute(CGAuctionRequestList* pPacket, Player* pPlayer) throw(Error);
};

#endif
