//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuctionCancel.h
// Description : Auction House cancel request packet.
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_AUCTION_CANCEL_H__
#define __CG_AUCTION_CANCEL_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGAuctionCancel : public Packet
{
public:
	CGAuctionCancel() throw();

	void read(SocketInputStream& iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_CG_AUCTION_CANCEL; }
	PacketSize_t getPacketSize() const throw() { return szDWORD; }
	string getPacketName() const throw() { return "CGAuctionCancel"; }
	string toString() const throw();

	DWORD getListingID() const throw() { return m_ListingID; }
	void setListingID(DWORD listingID) throw() { m_ListingID = listingID; }

private:
	DWORD m_ListingID;
};

class CGAuctionCancelFactory : public PacketFactory
{
public:
	Packet* createPacket() throw() { return new CGAuctionCancel(); }
	string getPacketName() const throw() { return "CGAuctionCancel"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_AUCTION_CANCEL; }
	PacketSize_t getPacketMaxSize() const throw() { return szDWORD; }
};

class CGAuctionCancelHandler
{
public:
	static void execute(CGAuctionCancel* pPacket, Player* pPlayer) throw(Error);
};

#endif
