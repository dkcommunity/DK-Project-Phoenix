//////////////////////////////////////////////////////////////////////////////
// Filename    : CGRemoveRankGem.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_REMOVE_RANKGEM_H__
#define __CG_REMOVE_RANKGEM_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGRemoveRankGem;
//////////////////////////////////////////////////////////////////////////////

class CGRemoveRankGem : public Packet 
{
public:
	virtual ~CGRemoveRankGem() throw () {}
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_CG_REMOVE_RANKGEM; }
	PacketSize_t getPacketSize() const throw() { return szGold; }
	string getPacketName() const throw() { return "CGRemoveRankGem"; }
	string toString() const throw();
	
public:

	Gold_t getSlotID() const throw() { return m_SlotID; }
	void setSlotID(Gold_t SlotID) throw() { m_SlotID = SlotID; } 

private: 
	Gold_t	m_SlotID;
};


//////////////////////////////////////////////////////////////////////////////
// class CGRemoveRankGemFactory;
//////////////////////////////////////////////////////////////////////////////

class CGRemoveRankGemFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new CGRemoveRankGem(); }
	string getPacketName() const throw() { return "CGRemoveRankGem"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_REMOVE_RANKGEM; }
	PacketSize_t getPacketMaxSize() const throw() { return szGold; }
};


//////////////////////////////////////////////////////////////////////////////
// class CGRemoveRankGemHandler;
//////////////////////////////////////////////////////////////////////////////

class Inventory;
class Item;

class CGRemoveRankGemHandler 
{
public:
	static void execute(CGRemoveRankGem* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
