//----------------------------------------------------------------------
// 
// Filename    : GCFriendAdd.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GC_FRIEND_ADD_H__
#define __GC_FRIEND_ADD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "FriendSimpleInfo.h"

#if __CONTENTS(__FRIEND_ADDITION)
class GCFriendAdd : public Packet {

public:
	virtual ~GCFriendAdd() ;
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_FRIEND_ADD; }
	PacketSize_t getPacketSize() const ;

	std::string getPacketName() const  { return "GCFriendAdd"; }
	std::string toString() const ;
	
	void SetFriendSimpleInfo(FriendSimpleInfo& Info);
	FriendSimpleInfo& GetFriendSimpleInfo(); 

private :
	FriendSimpleInfo m_FriendSimpleInfo;
};


//////////////////////////////////////////////////////////////////////
//
// class GCFriendAddFactory;
//
// Factory for GCFriendAdd
//
//////////////////////////////////////////////////////////////////////

class GCFriendAddFactory : public PacketFactory {

public:
	
	Packet* createPacket()  { return new GCFriendAdd(); }
	std::string getPacketName() const  { return "GCFriendAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_FRIEND_ADD; }

	PacketSize_t getPacketMaxSize() const  
	{ 
		return FriendSimpleInfo::getMaxSize();
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GCFriendAddHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendAdd* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
