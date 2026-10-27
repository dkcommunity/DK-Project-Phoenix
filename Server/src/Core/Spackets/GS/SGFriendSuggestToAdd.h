//----------------------------------------------------------------------
// 
// Filename    : SGFriendSuggestToAdd.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __SG_FRIEND_SUGGEST_TO_ADD_H__
#define __SG_FRIEND_SUGGEST_TO_ADD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class SGFriendSuggestToAdd : public Packet {

public:
	SGFriendSuggestToAdd() throw();
	virtual ~SGFriendSuggestToAdd() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_SG_FRIEND_SUGGEST_TO_ADD; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "SGFriendSuggestToAdd"; }
	string toString() const throw();
	
	void SetReceiver(const string& strName) { m_Receiver.SetString(strName); }
	string GetReceiver() { return m_Receiver.GetString(); }
	
	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); }
	string GetFriendName() { return m_FriendName.GetString(); }

private :
	StringInfo m_Receiver;
	StringInfo m_FriendName;	// 추가 요청한 친구

};


//////////////////////////////////////////////////////////////////////
//
// class SGFriendSuggestToAddFactory;
//
// Factory for SGFriendSuggestToAdd
//
//////////////////////////////////////////////////////////////////////

class SGFriendSuggestToAddFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new SGFriendSuggestToAdd(); }
	string getPacketName() const throw() { return "SGFriendSuggestToAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_SG_FRIEND_SUGGEST_TO_ADD; }

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

class SGFriendSuggestToAddHandler {
	
public:

	// execute packet's handler
	static void execute(SGFriendSuggestToAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
