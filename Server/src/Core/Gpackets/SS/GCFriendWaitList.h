//----------------------------------------------------------------------
// 
// Filename    : GCFriendWaitList.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GC_FRIEND_WAIT_LIST_H__
#define __GC_FRIEND_WAIT_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "types/FriendType.h"
#include "FriendWaitInfo.h"

#include <vector>

class GCFriendWaitList : public Packet {

public:
	virtual ~GCFriendWaitList() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_FRIEND_WAIT_LIST; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GCFriendWaitList"; }
	string toString() const throw();
	
	int GetFriendWaitListCount() const { return m_FriendWaitInfoList.size(); } 
	
	void AddFriendWaitInfo(FriendWaitInfo& info);
	FriendWaitInfo* GetFriendWaitInfo(int iIndex);

public:
	typedef vector<FriendWaitInfo> TFriendWaitInfoList;
	
private :
	TFriendWaitInfoList m_FriendWaitInfoList;

};


//////////////////////////////////////////////////////////////////////
//
// class GCFriendWaitListFactory;
//
// Factory for GCFriendWaitList
//
//////////////////////////////////////////////////////////////////////

class GCFriendWaitListFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GCFriendWaitList(); }
	string getPacketName() const throw() { return "GCFriendWaitList"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FRIEND_WAIT_LIST; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return szBYTE + FriendWaitInfo::getMaxSize() * FRIEND_MAX_WAIT_LIST_COUNT;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GCFriendWaitListHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendWaitList* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
