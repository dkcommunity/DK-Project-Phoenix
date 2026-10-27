//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectWeekItem.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_WEEKITEM_H__
#define __CG_SELECT_WEEKITEM_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGSelectWeekItem;
//////////////////////////////////////////////////////////////////////////////
#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
class CGSelectWeekItem : public Packet 
{
public:
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_CG_SELECT_WEEKITEM; }
	PacketSize_t getPacketSize() const  { return szObjectID; }
	std::string getPacketName() const  { return "CGSelectWeekItem"; }
	std::string toString() const ;

public:
	ZoneID_t getSelectID() const   { return m_SelectID; }
	void setSelectID(ObjectID_t SelectID)  { m_SelectID = SelectID; }

private:
	ObjectID_t   m_SelectID;

};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectWeekItemFactory;
//////////////////////////////////////////////////////////////////////////////

class CGSelectWeekItemFactory : public PacketFactory 
{
	Packet* createPacket()  { return new CGSelectWeekItem(); }
	std::string getPacketName() const  { return "CGSelectWeekItem"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_SELECT_WEEKITEM; }
	PacketSize_t getPacketMaxSize() const  { return szObjectID; }
};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectWeekItemHandler;
//////////////////////////////////////////////////////////////////////////////

class CGSelectWeekItemHandler 
{
public:
	static void execute(CGSelectWeekItem* pCGSelectWeekItem, Player* pPlayer) ;
};

#endif
#endif
