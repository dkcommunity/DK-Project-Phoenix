//----------------------------------------------------------------------
// 
// Filename    : SGFriendResToAdd.h 
// Written By  : 
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __SG_FRIEND_RES_TO_ADD_H__
#define __SG_FRIEND_RES_TO_ADD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

#include "StringInfo.h"

class SGFriendResToAdd : public Packet {

public:
	SGFriendResToAdd();
	virtual ~SGFriendResToAdd() throw();
	
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_SG_FRIEND_RES_TO_ADD; }
	PacketSize_t getPacketSize() const throw();

	string getPacketName() const throw() { return "SGFriendResToAdd"; }
	string toString() const throw();
	
	void SetReceiver(const string& strName) { m_Receiver.SetString(strName); }
	string GetReceiver() const { return m_Receiver.GetString(); }
	
	void SetFriendName(const string& strName) { m_FriendName.SetString(strName); }
	string GetFriendName() const { return m_FriendName.GetString(); }
	
	void SetResultCode(BYTE ResultCode) { m_ResultCode = ResultCode; }
	BYTE GetResultCode() const { return m_ResultCode; }

private :
	StringInfo m_Receiver;
	StringInfo m_FriendName;
	BYTE m_ResultCode;
};


//////////////////////////////////////////////////////////////////////
//
// class SGFriendResToAddFactory;
//
// Factory for SGFriendResToAdd
//
//////////////////////////////////////////////////////////////////////

class SGFriendResToAddFactory : public PacketFactory {

public:
	
	Packet* createPacket() throw() { return new SGFriendResToAdd(); }
	string getPacketName() const throw() { return "SGFriendResToAdd"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_SG_FRIEND_RES_TO_ADD; }

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

class SGFriendResToAddHandler {
	
public:

	// execute packet's handler
	static void execute(SGFriendResToAdd* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
