#if 0
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectSupplyItem.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_SUPPLYITEM_H__
#define __CG_SELECT_SUPPLYITEM_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGSelectSupplyItem;
//////////////////////////////////////////////////////////////////////////////

class CGSelectSupplyItem : public Packet 
{
public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_SELECT_SUPPLYITEM; }
	PacketSize_t getPacketSize() const  { return szObjectID; }
	std::string getPacketName() const  { return "CGSelectSupplyItem"; }
	std::string toString() const ;

public:
	ZoneID_t getSelectID() const   { return m_SelectID; }
	void setSelectID(ObjectID_t SelectID)  { m_SelectID = SelectID; }

private:
	ObjectID_t   m_SelectID;

};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectSupplyItemFactory;
//////////////////////////////////////////////////////////////////////////////

class CGSelectSupplyItemFactory : public PacketFactory 
{
	Packet* createPacket()  { return new CGSelectSupplyItem(); }
	std::string getPacketName() const  { return "CGSelectSupplyItem"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_SELECT_SUPPLYITEM; }
	PacketSize_t getPacketMaxSize() const  { return szObjectID; }
};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectSupplyItemHandler;
//////////////////////////////////////////////////////////////////////////////

class CGSelectSupplyItemHandler 
{
public:
	static void execute(CGSelectSupplyItem* pCGSelectSupplyItem, Player* pPlayer) ;
};

#endif

#endif
