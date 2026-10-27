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
	virtual ~CGUseRankGemFromInventory() {}
	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_USE_RANKGEM_FROM_INVENTORY; }
	PacketSize_t getPacketSize() const  { return szObjectID + szGold; }
	std::string getPacketName() const  { return "CGUseRankGemFromInventory"; }
	std::string toString() const ;

public:
	ObjectID_t getObjectID() const  { return m_ObjectID; }
	void setObjectID(ObjectID_t ObjectID)  { m_ObjectID = ObjectID; }


	Gold_t getSlotID() const  { return m_SlotID; }
	void setSlotID(Gold_t SlotID)  { m_SlotID = SlotID; } 

private:
	ObjectID_t   m_ObjectID;
	Gold_t	m_SlotID;
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseRankGemFromInventoryFactory;
//////////////////////////////////////////////////////////////////////////////

class CGUseRankGemFromInventoryFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGUseRankGemFromInventory(); }
	std::string getPacketName() const  { return "CGUseRankGemFromInventory"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_USE_RANKGEM_FROM_INVENTORY; }
	PacketSize_t getPacketMaxSize() const  { return szObjectID + szGold; }
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseRankGemFromInventoryHandler;
//////////////////////////////////////////////////////////////////////////////

class Inventory;
class Item;

class CGUseRankGemFromInventoryHandler 
{
public:
	static void execute(CGUseRankGemFromInventory* pPacket, Player* pPlayer) ;
};

#endif
