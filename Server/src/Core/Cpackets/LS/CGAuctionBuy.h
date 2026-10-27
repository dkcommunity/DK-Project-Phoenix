//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuctionBuy.h
// Description : Auction House buy request packet.
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_AUCTION_BUY_H__
#define __CG_AUCTION_BUY_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGAuctionBuy : public Packet
{
public:
	CGAuctionBuy() throw();

	void read(SocketInputStream& iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_CG_AUCTION_BUY; }
	PacketSize_t getPacketSize() const throw() { return szDWORD; }
	string getPacketName() const throw() { return "CGAuctionBuy"; }
	string toString() const throw();

	DWORD getListingID() const throw() { return m_ListingID; }
	void setListingID(DWORD listingID) throw() { m_ListingID = listingID; }

private:
	DWORD m_ListingID;
};

class CGAuctionBuyFactory : public PacketFactory
{
public:
	Packet* createPacket() throw() { return new CGAuctionBuy(); }
	string getPacketName() const throw() { return "CGAuctionBuy"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_AUCTION_BUY; }
	PacketSize_t getPacketMaxSize() const throw() { return szDWORD; }
};

class CGAuctionBuyHandler
{
public:
	static void execute(CGAuctionBuy* pPacket, Player* pPlayer) throw(Error);
};

#endif
