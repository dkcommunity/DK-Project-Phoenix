//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectAllWeekItem.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_ALL_WEEKITEM_H__
#define __CG_SELECT_ALL_WEEKITEM_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGSelectAllWeekItem;
//////////////////////////////////////////////////////////////////////////////
#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
class CGSelectAllWeekItem : public Packet 
{
public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_SELECT_ALL_WEEKITEM; }
	PacketSize_t getPacketSize() const  { return 0; }
	std::string getPacketName() const  { return "CGSelectAllWeekItem"; }
	std::string toString() const ;

public:

private:

};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectAllWeekItemFactory;
//////////////////////////////////////////////////////////////////////////////

class CGSelectAllWeekItemFactory : public PacketFactory 
{
	Packet* createPacket()  { return new CGSelectAllWeekItem(); }
	std::string getPacketName() const  { return "CGSelectAllWeekItem"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_SELECT_ALL_WEEKITEM; }
	PacketSize_t getPacketMaxSize() const  { return 0; }
};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectAllWeekItemHandler;
//////////////////////////////////////////////////////////////////////////////

class CGSelectAllWeekItemHandler 
{
public:
	static void execute(CGSelectAllWeekItem* pCGSelectAllWeekItem, Player* pPlayer) ;
};

#endif
#endif
