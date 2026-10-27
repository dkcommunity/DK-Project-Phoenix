//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAuctionList.h
// Description : Auction House item list packet.
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_AUCTION_LIST_H__
#define __GC_AUCTION_LIST_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "PCItemInfo.h"
#include <list>

#define MAX_AUCTION_LIST 20

class GCAuctionList : public Packet
{
public:
	GCAuctionList() throw();
	virtual ~GCAuctionList() throw();

	struct Entry
	{
		DWORD ListingID;
		Gold_t Price;
		BYTE SellerRace;
		BYTE ItemRace;
		string SellerName;
		PCItemInfo ItemInfo;

		Entry() : ListingID(0), Price(0), SellerRace(0), ItemRace(0) {}
		PacketSize_t getPacketSize() const throw()
		{
			return szDWORD + szGold + szBYTE + szBYTE + szBYTE + SellerName.size() + ItemInfo.getSize();
		}
	};

public:
	void read(SocketInputStream& iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_AUCTION_LIST; }
	PacketSize_t getPacketSize() const throw();
	string getPacketName() const throw() { return "GCAuctionList"; }
	string toString() const throw();

	void clearEntries() throw();
	void addEntry(Entry* pEntry) throw() { m_Entries.push_back(pEntry); }
	const std::list<Entry*>& getEntries() const throw() { return m_Entries; }

private:
	std::list<Entry*> m_Entries;
};

class GCAuctionListFactory : public PacketFactory
{
public:
	Packet* createPacket() throw() { return new GCAuctionList(); }
	string getPacketName() const throw() { return "GCAuctionList"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_AUCTION_LIST; }
	PacketSize_t getPacketMaxSize() const throw()
	{
		return szBYTE + MAX_AUCTION_LIST * (szDWORD + szGold + szBYTE + szBYTE + szBYTE + 32 + PCItemInfo::getMaxSize());
	}
};

class GCAuctionListHandler
{
public:
	static void execute(GCAuctionList* pPacket, Player* pPlayer) throw(Error);
};

#endif
