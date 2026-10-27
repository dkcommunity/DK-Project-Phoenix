//----------------------------------------------------------------------
// 
// Filename    : GCFriendViewInfo.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GC_FRIEND_VIEW_INFO_H__
#define __GC_FRIEND_VIEW_INFO_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"
#include "FriendDetailInfo.h"
#include "FriendIndividualOptionInfo.h"

#if __CONTENTS(__FRIEND_ADDITION)
class GCFriendViewInfo : public Packet {

public:
	GCFriendViewInfo();
	virtual ~GCFriendViewInfo() ;
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_FRIEND_VIEW_INFO; }
	PacketSize_t getPacketSize() const ;

	std::string getPacketName() const  { return "GCFriendViewInfo"; }
	std::string toString() const ;
	
	void SetFriendDetailInfo(FriendDetailInfo& Info);
	FriendDetailInfo& GetFriendDetailInfo();
	
	void SetFriendIndividualOptionInfo(FriendIndividualOptionInfo& Info);
	FriendIndividualOptionInfo& GetFriendIndividualOptionInfo();

private :
	FriendDetailInfo m_FriendDetailInfo;
	FriendIndividualOptionInfo m_FriendIndividualOptionInfo;
};


//////////////////////////////////////////////////////////////////////
//
// class GCFriendViewInfoFactory;
//
// Factory for GCFriendViewInfo
//
//////////////////////////////////////////////////////////////////////

class GCFriendViewInfoFactory : public PacketFactory {

public:
	
	Packet* createPacket()  { return new GCFriendViewInfo(); }
	std::string getPacketName() const  { return "GCFriendViewInfo"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_FRIEND_VIEW_INFO; }

	PacketSize_t getPacketMaxSize() const  
	{ 
		return FriendDetailInfo::getMaxSize() + StringInfo::getMaxSize() + szBYTE + szBYTE;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GCFriendViewInfoHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendViewInfo* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
