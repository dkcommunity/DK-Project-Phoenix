//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseRankGemFromInventory.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_RANKGEM_FROM_INVENTORY_H__
#define __CG_USE_RANKGEM_FROM_INVENTORY_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGUseRankGemFromInventory;
//////////////////////////////////////////////////////////////////////////////

class CGUseRankGemFromInventory : public Packet 
{
public:
	virtual ~CGUseRankGemFromInventory() throw () {}
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_CG_USE_RANKGEM_FROM_INVENTORY; }
	PacketSize_t getPacketSize() const throw() { return szObjectID + szGold; }
	string getPacketName() const throw() { return "CGUseRankGemFromInventory"; }
	string toString() const throw();
	
public:
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t ObjectID) throw() { m_ObjectID = ObjectID; }

	Gold_t getSlotID() const throw() { return m_SlotID; }
	void setSlotID(Gold_t SlotID) throw() { m_SlotID = SlotID; } 

private:
	ObjectID_t   m_ObjectID; // 酒捞袍狼 object id 
	Gold_t	m_SlotID;
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseRankGemFromInventoryFactory;
//////////////////////////////////////////////////////////////////////////////

class CGUseRankGemFromInventoryFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new CGUseRankGemFromInventory(); }
	string getPacketName() const throw() { return "CGUseRankGemFromInventory"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_USE_RANKGEM_FROM_INVENTORY; }
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szGold; }
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseRankGemFromInventoryHandler;
//////////////////////////////////////////////////////////////////////////////

class Inventory;
class Item;

class CGUseRankGemFromInventoryHandler 
{
public:
	static void execute(CGUseRankGemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
