//----------------------------------------------------------------------
// 
// Filename    : SGFriendAdd.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __SG_FRIEND_ADD_H__
#define __SG_FRIEND_ADD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "FriendSimpleInfo.h"
#include "StringInfo.h"

class SGFriendAdd : public Packet {

public:
	SGFriendAdd();
	virtual ~SGFriendAdd() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_SG_FRIEND_ADD; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "SGFriendAdd"; }
	string toString() const throw();
	
	void SetReceiver(const string& strName) { m_Receiver.SetString(strName); }
	string GetReceiver() { return m_Receiver.GetString(); }
		
	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); }
	string GetFriendName() { return m_FriendName.GetString(); }
	
	void SetFriendSimpleInfo(FriendSimpleInfo& Info);
	FriendSimpleInfo& GetFriendSimpleInfo(); 
		
private :
	StringInfo m_Receiver;
	StringInfo m_FriendName;
	FriendSimpleInfo m_FriendSimpleInfo;

};


//////////////////////////////////////////////////////////////////////
//
// class SGFriendAddFactory;
//
// Factory for SGFriendAdd
//
//////////////////////////////////////////////////////////////////////

class SGFriendAddFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new SGFriendAdd(); }
	string getPacketName() const throw() { return "SGFriendAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_SG_FRIEND_ADD; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + StringInfo::getMaxSize() + FriendSimpleInfo::getMaxSize();
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class SGFriendAddHandler {
	
public:

	// execute packet's handler
	static void execute(SGFriendAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
