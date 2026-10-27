//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuctionListItem.h
// Description : Auction House item listing packet.
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_AUCTION_LIST_ITEM_H__
#define __CG_AUCTION_LIST_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGAuctionListItem : public Packet
{
public:
	CGAuctionListItem() throw();

	void read(SocketInputStream& iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_CG_AUCTION_LIST_ITEM; }
	PacketSize_t getPacketSize() const throw() { return szObjectID + szGold; }
	string getPacketName() const throw() { return "CGAuctionListItem"; }
	string toString() const throw();

	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t objectID) throw() { m_ObjectID = objectID; }
	Gold_t getPrice() const throw() { return m_Price; }
	void setPrice(Gold_t price) throw() { m_Price = price; }

private:
	ObjectID_t m_ObjectID;
	Gold_t m_Price;
};

class CGAuctionListItemFactory : public PacketFactory
{
public:
	Packet* createPacket() throw() { return new CGAuctionListItem(); }
	string getPacketName() const throw() { return "CGAuctionListItem"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_AUCTION_LIST_ITEM; }
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szGold; }
};

class CGAuctionListItemHandler
{
public:
	static void execute(CGAuctionListItem* pPacket, Player* pPlayer) throw(Error);
};

#endif
