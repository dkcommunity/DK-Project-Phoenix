#if 0
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectAllSupplyItem.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_ALL_SUPPLYITEM_H__
#define __CG_SELECT_ALL_SUPPLYITEM_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGSelectAllSupplyItem;
//////////////////////////////////////////////////////////////////////////////

class CGSelectAllSupplyItem : public Packet 
{
public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_SELECT_ALL_SUPPLYITEM; }
	PacketSize_t getPacketSize() const  { return 0; }
	std::string getPacketName() const  { return "CGSelectAllSupplyItem"; }
	std::string toString() const ;

public:

private:

};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectAllSupplyItemFactory;
//////////////////////////////////////////////////////////////////////////////

class CGSelectAllSupplyItemFactory : public PacketFactory 
{
	Packet* createPacket()  { return new CGSelectAllSupplyItem(); }
	std::string getPacketName() const  { return "CGSelectAllSupplyItem"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_SELECT_ALL_SUPPLYITEM; }
	PacketSize_t getPacketMaxSize() const  { return 0; }
};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectAllSupplyItemHandler;
//////////////////////////////////////////////////////////////////////////////

class CGSelectAllSupplyItemHandler 
{
public:
	static void execute(CGSelectAllSupplyItem* pCGSelectAllSupplyItem, Player* pPlayer) ;
};

#endif

#endif
