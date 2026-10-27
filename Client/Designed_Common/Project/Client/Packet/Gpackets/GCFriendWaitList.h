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

#include "Types/FriendType.h"
#include "FriendWaitInfo.h"

#include <vector>

#if __CONTENTS(__FRIEND_ADDITION)
class GCFriendWaitList : public Packet {

public:
	virtual ~GCFriendWaitList() ;
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_FRIEND_WAIT_LIST; }
	PacketSize_t getPacketSize() const ;

	std::string getPacketName() const  { return "GCFriendWaitList"; }
	std::string toString() const ;
	
	int GetFriendWaitListCount() const { return m_FriendWaitInfoList.size(); } 
	
	void AddFriendWaitInfo(FriendWaitInfo& info);
	FriendWaitInfo* GetFriendWaitInfo(int iIndex);

public:
	typedef std::vector<FriendWaitInfo> TFriendWaitInfoList;
	
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
	
	Packet* createPacket()  { return new GCFriendWaitList(); }
	std::string getPacketName() const  { return "GCFriendWaitList"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_FRIEND_WAIT_LIST; }

	PacketSize_t getPacketMaxSize() const  
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
	static void execute(GCFriendWaitList* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
