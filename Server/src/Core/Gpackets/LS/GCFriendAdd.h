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

class GCFriendAdd : public Packet {

public:
	virtual ~GCFriendAdd() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_FRIEND_ADD; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GCFriendAdd"; }
	string toString() const throw();
	
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
	
	Packet* createPacket() throw() { return new GCFriendAdd(); }
	string getPacketName() const throw() { return "GCFriendAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FRIEND_ADD; }

	PacketSize_t getPacketMaxSize() const throw() 
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
	static void execute(GCFriendAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
