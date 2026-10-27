//----------------------------------------------------------------------
// 
// Filename    : GCFriendList.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GC_FRIEND_LIST_H__
#define __GC_FRIEND_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "FriendSimpleInfo.h"
#include "FriendGlobalOptionInfo.h"

#include <vector>

class GCFriendList : public Packet {

public:
	virtual ~GCFriendList() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_FRIEND_LIST; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GCFriendList"; }
	string toString() const throw();
	
	int GetFriendListCount() const { return m_FriendSimpleInfoList.size(); } 
	
	void AddFriendInfo(FriendSimpleInfo& ifo);
	FriendSimpleInfo* GetFriendInfo(int iIndex);
	
	void SetFriendGlobalOptionInfo(FriendGlobalOptionInfo& info)
	{
		m_FriendGlobalOptionInfo = info;
	}
	
	FriendGlobalOptionInfo& GetFriendGlobalOptionInfo()
	{
		return m_FriendGlobalOptionInfo;	
	}

public:
	typedef vector<FriendSimpleInfo> TFriendSimpleInfoList;
	
	static const int m_iMaxFriendList = 20;

private :
	TFriendSimpleInfoList m_FriendSimpleInfoList;
	
	FriendGlobalOptionInfo m_FriendGlobalOptionInfo;

};


//////////////////////////////////////////////////////////////////////
//
// class GCFriendListFactory;
//
// Factory for GCFriendList
//
//////////////////////////////////////////////////////////////////////

class GCFriendListFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GCFriendList(); }
	string getPacketName() const throw() { return "GCFriendList"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FRIEND_LIST; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		PacketSize_t packetSize;
		
		packetSize += szBYTE + FriendSimpleInfo::getMaxSize() * GCFriendList::m_iMaxFriendList;
		packetSize += FriendGlobalOptionInfo::getMaxSize();
		
		return packetSize;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GCFriendListHandler {
	
public:

	// execute packet's handler
	static void execute(GCFriendList* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
