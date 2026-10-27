//----------------------------------------------------------------------
// 
// Filename    : SGFriendList.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __SG_FRIEND_LIST_H__
#define __SG_FRIEND_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"
#include "FriendSimpleInfo.h"
#include "FriendGlobalOptionInfo.h"

#include <vector>

class SGFriendList : public Packet {

public:
	SGFriendList();
	virtual ~SGFriendList() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_SG_FRIEND_LIST; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "SGFriendList"; }
	string toString() const throw();
	
	string GetReceiver() const { return m_Receiver.GetString(); }
	void SetReceiver(const string& strName) { m_Receiver.SetString(strName); }
	
	int GetFriendListCount() const { return m_FriendSimpleInfoList.size(); } 
	
	void AddFriendSimpleInfo(FriendSimpleInfo& info);
	FriendSimpleInfo* GetFriendSimpleInfo(int iIndex);

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
	StringInfo m_Receiver;
	TFriendSimpleInfoList m_FriendSimpleInfoList;
	
	FriendGlobalOptionInfo m_FriendGlobalOptionInfo;

};


//////////////////////////////////////////////////////////////////////
//
// class SGFriendListFactory;
//
// Factory for SGFriendList
//
//////////////////////////////////////////////////////////////////////

class SGFriendListFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new SGFriendList(); }
	string getPacketName() const throw() { return "SGFriendList"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_SG_FRIEND_LIST; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		PacketSize_t packetSize;
		
		packetSize = StringInfo::getMaxSize();
		packetSize += szBYTE + FriendSimpleInfo::getMaxSize() * SGFriendList::m_iMaxFriendList;
		packetSize += FriendGlobalOptionInfo::getMaxSize();
		
		return packetSize;		
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class SGFriendListHandler {
	
public:

	// execute packet's handler
	static void execute(SGFriendList* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
