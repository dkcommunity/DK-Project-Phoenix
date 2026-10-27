//----------------------------------------------------------------------
// 
// Filename    : SGFriendReqToAdd.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __SG_FRIEND_REQ_TO_ADD_H__
#define __SG_FRIEND_REQ_TO_ADD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class SGFriendReqToAdd : public Packet {

public:
	SGFriendReqToAdd();
	virtual ~SGFriendReqToAdd() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_SG_FRIEND_REQ_TO_ADD; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "SGFriendReqToAdd"; }
	string toString() const throw();
	
	void SetReceiver(const string& strName) { m_Receiver.SetString(strName); }
	string GetReceiver() { return m_Receiver.GetString(); }
		
	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); }
	string GetFriendName() { return m_FriendName.GetString(); }
	
	void SetResultCode(BYTE code) { m_ResultCode = code; }
	BYTE GetResultCode() { return m_ResultCode; }

private :
	StringInfo m_Receiver;
	StringInfo m_FriendName;
	BYTE m_ResultCode;

};


//////////////////////////////////////////////////////////////////////
//
// class SGFriendReqToAddFactory;
//
// Factory for SGFriendReqToAdd
//
//////////////////////////////////////////////////////////////////////

class SGFriendReqToAddFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new SGFriendReqToAdd(); }
	string getPacketName() const throw() { return "SGFriendReqToAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_SG_FRIEND_REQ_TO_ADD; }

	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		return StringInfo::getMaxSize() + StringInfo::getMaxSize() + szBYTE;
	}

};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class SGFriendReqToAddHandler {
	
public:

	// execute packet's handler
	static void execute(SGFriendReqToAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
