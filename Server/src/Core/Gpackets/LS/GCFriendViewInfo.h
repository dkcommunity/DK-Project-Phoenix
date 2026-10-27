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

class GCFriendViewInfo : public Packet {

public:
	GCFriendViewInfo();
	virtual ~GCFriendViewInfo() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_FRIEND_VIEW_INFO; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GCFriendViewInfo"; }
	string toString() const throw();
	
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
	
	Packet* createPacket() throw() { return new GCFriendViewInfo(); }
	string getPacketName() const throw() { return "GCFriendViewInfo"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FRIEND_VIEW_INFO; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return FriendDetailInfo::getMaxSize() + FriendIndividualOptionInfo::getMaxSize();
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
	static void execute(GCFriendViewInfo* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
