//----------------------------------------------------------------------
// 
// Filename    : GSFriendReqToAdd.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __GS_FRIEND_REQ_TO_ADD_H__
#define __GS_FRIEND_REQ_TO_ADD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class GSFriendReqToAdd : public Packet {

public:
	GSFriendReqToAdd();
	virtual ~GSFriendReqToAdd() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GS_FRIEND_REQ_TO_ADD; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "GSFriendReqToAdd"; }
	string toString() const throw();
	
	void SetSender(const string& strName) { m_Sender.SetString(strName); }
	string GetSender() { return m_Sender.GetString(); }
	
	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); }
	string GetFriendName() { return m_FriendName.GetString(); }

private :
	StringInfo m_Sender;
	StringInfo m_FriendName;	// 추가 요청할 친구
};


//////////////////////////////////////////////////////////////////////
//
// class GSFriendReqToAddFactory;
//
// Factory for GSFriendReqToAdd
//
//////////////////////////////////////////////////////////////////////

class GSFriendReqToAddFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new GSFriendReqToAdd(); }
	string getPacketName() const throw() { return "GSFriendReqToAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GS_FRIEND_REQ_TO_ADD; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + StringInfo::getMaxSize();
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GSFriendReqToAddHandler {
	
public:

	// execute packet's handler
	static void execute(GSFriendReqToAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
