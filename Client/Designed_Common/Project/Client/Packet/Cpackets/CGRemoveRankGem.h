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
	virtual ~CGRemoveRankGem() {}
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_REMOVE_RANKGEM; }
	PacketSize_t getPacketSize() const  { return szGold; }
	std::string getPacketName() const  { return "CGRemoveRankGem"; }
	std::string toString() const ;
	
public:

	Gold_t getSlotID() const  { return m_SlotID; }
	void setSlotID(Gold_t SlotID)  { m_SlotID = SlotID; } 

private: 
	Gold_t	m_SlotID;
};


//////////////////////////////////////////////////////////////////////////////
// class CGRemoveRankGemFactory;
//////////////////////////////////////////////////////////////////////////////

class CGRemoveRankGemFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new CGRemoveRankGem(); }
	std::string getPacketName() const  { return "CGRemoveRankGem"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_REMOVE_RANKGEM; }
	PacketSize_t getPacketMaxSize() const  { return szGold; }
};


//////////////////////////////////////////////////////////////////////////////
// class CGRemoveRankGemHandler;
//////////////////////////////////////////////////////////////////////////////

class Inventory;
class Item;

class CGRemoveRankGemHandler 
{
public:
	static void execute(CGRemoveRankGem* pPacket, Player* pPlayer) ;
};

#endif
