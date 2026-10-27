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

class CGSelectAllWeekItem : public Packet 
{
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_CG_SELECT_ALL_WEEKITEM; }
	PacketSize_t getPacketSize() const throw() { return 0; }
	string getPacketName() const throw() { return "CGSelectAllWeekItem"; }
	string toString() const throw();

public:

private:

};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectAllWeekItemFactory;
//////////////////////////////////////////////////////////////////////////////

class CGSelectAllWeekItemFactory : public PacketFactory 
{
	Packet* createPacket() throw() { return new CGSelectAllWeekItem(); }
	string getPacketName() const throw() { return "CGSelectAllWeekItem"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_SELECT_ALL_WEEKITEM; }
	PacketSize_t getPacketMaxSize() const throw() { return 0; }
};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectAllWeekItemHandler;
//////////////////////////////////////////////////////////////////////////////

class CGSelectAllWeekItemHandler 
{
public:
	static void execute(CGSelectAllWeekItem* pCGSelectAllWeekItem, Player* pPlayer) throw(Error);
};

#endif
