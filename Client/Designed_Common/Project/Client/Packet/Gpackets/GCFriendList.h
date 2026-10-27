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

#if __CONTENTS(__FRIEND_ADDITION)
class GCFriendList : public Packet {

public:
	virtual ~GCFriendList() ;
	
    void read(SocketInputStream & iStream) ;
    void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;

	PacketID_t getPacketID() const  { return PACKET_GC_FRIEND_LIST; }
	PacketSize_t getPacketSize() const ;

	std::string getPacketName() const  { return "GCFriendList"; }
	std::string toString() const ;
	
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
	typedef std::vector<FriendSimpleInfo> TFriendSimpleInfoList;
	
	static const int m_iMaxFriendList;

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
	
	Packet* createPacket()  { return new GCFriendList(); }
	std::string getPacketName() const  { return "GCFriendList"; }
	
	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_FRIEND_LIST; }

	PacketSize_t getPacketMaxSize() const  
	{ 
		PacketSize_t packetSize = 0;
		
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
	static void execute(GCFriendList* pPacket, Player* pPlayer) ;

};
#endif //__FRIEND_ADDITION

#endif
