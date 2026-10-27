//----------------------------------------------------------------------
// 
// Filename    : SGFriendWaitList.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __SG_FRIEND_WAIT_LIST_H__
#define __SG_FRIEND_WAIT_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "types/FriendType.h"
#include "StringInfo.h"
#include "FriendWaitInfo.h"

#include <vector>

class SGFriendWaitList : public Packet {

public:
	SGFriendWaitList();
	virtual ~SGFriendWaitList() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_SG_FRIEND_WAIT_LIST; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "SGFriendWaitList"; }
	string toString() const throw();
	
	void SetReceiver(const string& strName) { m_Receiver.SetString(strName); }
	string GetReceiver() { return m_Receiver.GetString(); }
	
	int GetFriendWaitListCount() const { return m_FriendWaitInfoList.size(); } 
	
	void AddFriendWaitInfo(FriendWaitInfo& info);
	FriendWaitInfo* GetFriendWaitInfo(int iIndex);

public:
	typedef vector<FriendWaitInfo> TFriendWaitInfoList;
	
private :
	StringInfo m_Receiver;
	TFriendWaitInfoList m_FriendWaitInfoList;

};


//////////////////////////////////////////////////////////////////////
//
// class SGFriendWaitListFactory;
//
// Factory for SGFriendWaitList
//
//////////////////////////////////////////////////////////////////////

class SGFriendWaitListFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new SGFriendWaitList(); }
	string getPacketName() const throw() { return "SGFriendWaitList"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_SG_FRIEND_WAIT_LIST; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + szBYTE + FriendWaitInfo::getMaxSize() * FRIEND_MAX_WAIT_LIST_COUNT;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class SGFriendWaitListHandler {
	
public:

	// execute packet's handler
	static void execute(SGFriendWaitList* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
